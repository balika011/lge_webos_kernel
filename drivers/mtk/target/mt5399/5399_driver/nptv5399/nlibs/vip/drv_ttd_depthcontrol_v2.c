/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2005, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: drv_ttd.c $
 * $Revision: #1 $
 *
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*
 * Header Files
 *----------------------------------------------------------------------------*/
#include "general.h"
#include "x_assert.h"
#include "x_bim.h"
#include "x_os.h"
#include "hw_tddc.h"
#include "hw_ttd.h"
#ifdef CC_MT5399
#include "hw_mjc.h"
#endif
#include "hw_sw.h"
#include "hw_ycproc.h"
#include "nptv_debug.h"
#include "drv_ttd.h"
#include "drv_video.h"
#include "drv_tdnavi.h"
#include "drv_tdtv_drvif.h"
#include "drv_scaler_drvif.h"
#include "drv_meter.h"
#include "drv_mjc.h"
#include "fbm_drvif.h"
#include "drv_tdtv_feature.h"
#include "drv_display.h"

#define TDDC_DS_DRAM_SIZE       0x8800  //(240*135)     => (256*135) = 0x8700
#define TDDC_CID_DRAM_SIZE      0x2200  //(120*68)      => (128*68) = 0x2200
#define TDDC_MV_DRAM_SIZE       0x8800  //(120*68*2)*2  => (128*68*2)*2 = 0x8800
#define TDDC_DRAM_TOTAL_SIZE    (((TDDC_DS_DRAM_SIZE*2+TDDC_CID_DRAM_SIZE*2)*2)+(TDDC_MV_DRAM_SIZE*2)+(TDDC_MV_DRAM_SIZE*2))
#define TDDC_MV_SIZE			(2)
#define TDDC_MV_PACK			(4)
#define TDDC_MV_PACK_SIZE		(TDDC_MV_SIZE*TDDC_MV_PACK)
#define TDDC_MV_PACK_LR_SIZE	(TDDC_MV_PACK_SIZE*2)

#define ClipInt(p)  ((p > 255) ? (255) : ((p < 0) ? 0 : p))
#define Diff(a, b)  (((a)>(b))?((a)-(b)):((b)-(a)))
#define CLIP(val, min, max) ((val>=max) ? max : ((val<=min) ? min : val)) 

#define TDDC_VIR(addr)          (gTddcCfg.u4VirAddrBase+addr)
#define TDDC_PHY(addr)          (gTddcCfg.u4PhyAddrBase+addr)
#define TDDC_VIR_TO_PHY(addr)   (addr-gTddcCfg.u4VirAddrBase+gTddcCfg.u4PhyAddrBase)

typedef struct 
{
    signed y:4;
    signed x:9;
    signed blk:3;
} DVEC;

typedef struct
{
    UINT32 u4DsoAddrL[2];
    UINT32 u4DsoAddrR[2]; 
    UINT32 u4CidAddrL[2]; 
    UINT32 u4CidAddrR[2]; 
    UINT32 u4MvAddrIn[2];
    UINT32 u4MvAddrOut[2];     // 0:LR 1:RL
    UINT32 u4VirAddrBase;
    UINT32 u4PhyAddrBase;
} TDDC_DRAM_T;


#define TDDC_DS                 (8)
#define TDDC_MB_SIZE            (2)
#define TDDC_DV_WIDTH_LIMIT     (512+32)
#define TDDC_OSD_MARK           (240)
#define TDDC_IMG_WIDTH_MAX      (1920)
#define TDDC_IMG_HEIGHT_MAX     (1080)
#define TDDC_DV_MAX             (120*68) 

static TDDC_DRAM_T gTddcCfg;
static UINT32 u4BufIndex;
static UINT8 u1IsrFlag = 0;
static UINT8 u1TddcInitFlag = SV_FALSE;
static INT32 occ_count_L, occ_count_R;
static INT32 occ_count_L_2, occ_count_R_2;
static INT32 depth_width, depth_height;
static INT32 global_gain;
static INT32 AutoC_Out_value = 0;
INT32 final_shift_H=0;

#ifdef CC_FPGA 
#define PRE_FRM     (0)
#define CUR_FRM     (0)
#define NEXT_FRM    (0)
#else
#define PRE_FRM     (u4BufIndex ? 0 : 1)
#define CUR_FRM     (u4BufIndex)
#define NEXT_FRM    (u4BufIndex ? 0 : 1)
#endif

static PEL *m_pDownY32_L = NULL;   	    //Dram2 : CID
static PEL *m_pDownY32_R = NULL;   	    //Dram2 : CID
static PEL *m_pDownY32_L_ISR = NULL;    //Dram2 : CID
static PEL *m_pDownY32_R_ISR = NULL;    //Dram2 : CID
static INT32 *check_map = NULL;
static INT32 *complete_map = NULL;
static DVEC * DE_buf_L = NULL; 		    //Dram3 : MV Write
static DVEC * DE_buf_R = NULL; 		    //Dram3 : MV Write
static DVEC * DE_buf_L_ISR = NULL; 		//Dram3 : MV Write
static DVEC * DE_buf_R_ISR = NULL; 		//Dram3 : MV Write
static DVEC * DE_buf_L_PRE = NULL; 		
static DVEC * DE_buf_L_INV = NULL;
static INT32 * final_depth_buf_L = NULL;     
static INT32 * final_depth_buf_R = NULL;        
static INT32 * render_depth_buf_L = NULL;   //Dram4 : MV Read
static INT32 * render_depth_buf_R = NULL;   //Dram4 : MV Read
static INT32 * occ_map_L = NULL;             
static INT32 * occ_map_R = NULL;    
static INT32 * non_occ_L = NULL;              
static INT32 * non_occ_R = NULL; 
static INT32 * occ_diff_L = NULL;              
static INT32 * occ_diff_R = NULL; 
static INT32 * hole_buf_L = NULL;              
static INT32 * hole_buf_R = NULL; 
static INT32 * osd_sm_buf_L = NULL;              
static INT32 * osd_sm_buf_R = NULL; 

static INT32 * osd_map_L = NULL;              
static INT32 * osd_map_R = NULL; 


//depth gain control                                                                                                    
static INT32 fb_smth_local_gain = 0;                                                                                                 
static INT32 fb_local_gain_IIR = 0;                                                                                                  
static INT32 dp_smth_local_gain = 0;                                                                                                 
static INT32 dp_local_gain_IIR = 0;                                                          
static INT32 max_local_diff_L=0;                                                                                                 
static INT32 min_local_diff_L=0;                                                                                                 
static INT32 max_local_diff_R=0;                                                                                                 
static INT32 min_local_diff_R=0;  
static INT32 c_ada_occ = 4;         

static INT32 max_depth_L=0;                                                                                                 
static INT32 min_depth_L=0;                                                                                                 
static INT32 max_depth_R=0;                                                                                                 
static INT32 min_depth_R=0;  

//FS detection
static UINT32 DvCount = (120*68);                                                                                                 
static UINT32 FsNaviCur = TD_NAVI_DETECT_2D;                                                                                                 
static UINT32 FsNaviCnt = 0;


HAL_TIME_T TimeStart, TimeNow, TimeDelta;

EXTERN void HalFlushInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
EXTERN void HalIOMMUFlushInvalidateDCacheMultipleLine(UINT32 u4VirAddr, UINT32 u4PhyAddr, UINT32 u4Size);
EXTERN UINT8 u1GetFlipMirrorConfig(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* =========================================================================================== */
static void HalFlushInvalidateDCacheMultipleLine_Ex(UINT32 u4Addr, UINT32 u4Size)
{    
#ifdef __KERNEL__
    HalIOMMUFlushInvalidateDCacheMultipleLine(u4Addr, TDDC_VIR_TO_PHY(u4Addr), u4Size);    
#else
    HalFlushInvalidateDCacheMultipleLine(u4Addr, u4Size);
#endif
}

#ifdef CC_FPGA
#define FPGA_INDIRECT_ACCESS    1
#endif

#ifdef FPGA_INDIRECT_ACCESS
// FPGA TTD dram base address
#define TDDC_FPGA_DRAM_BASE     0x6080000

#define C_FPGA_CTRL_8 (IO_TDDC_BASE + 0xF20)
    #define DRAM_ADDR           Fld(28,0,AC_MSKDW)

// Dram write data    
#define C_FPGA_CTRL_9   (IO_TDDC_BASE + 0xF24)
#define C_FPGA_CTRL_10  (IO_TDDC_BASE + 0xF28)
#define C_FPGA_CTRL_11  (IO_TDDC_BASE + 0xF2C)
#define C_FPGA_CTRL_12  (IO_TDDC_BASE + 0xF30)
#define C_FPGA_CTRL_13  (IO_TDDC_BASE + 0xF34)   
    #define DRAM_RW             Fld(1,0,AC_MSKB0)   //0: read           1: write
    #define DRAM_TRIG           Fld(1,1,AC_MSKB0)
    #define DRAM_BURST          Fld(1,2,AC_MSKB0)

// Dram read data 
#define STA_FPGA_28     (IO_TDDC_BASE + 0xF70)
#define STA_FPGA_29     (IO_TDDC_BASE + 0xF74)
#define STA_FPGA_30     (IO_TDDC_BASE + 0xF78)
#define STA_FPGA_31     (IO_TDDC_BASE + 0xF7C)


static UINT32* pGetMvData = NULL;   
static UINT32* pSetMvData = NULL;   
static UINT32* pGetCidDataL = NULL;   
static UINT32* pGetCidDataR = NULL;   

static void vDrvGetFpgaData(UINT32* pData, UINT32 u4Addr, UINT32 u4Size)
{    
    UINT32 i, u4Loop = u4Size/16;

	vIO32WriteFldAlign(C_FPGA_CTRL_13, 0, DRAM_BURST);		  
	vIO32WriteFldAlign(C_FPGA_CTRL_13, 0, DRAM_RW);
	
    for (i = 0; i < u4Loop; i++)
    {
        vIO32WriteFldAlign(C_FPGA_CTRL_8, u4Addr+16*i, DRAM_ADDR);

        vIO32WriteFldAlign(C_FPGA_CTRL_13, 1, DRAM_TRIG);
        vIO32WriteFldAlign(C_FPGA_CTRL_13, 0, DRAM_TRIG);
		
        *(pData+4*i+0) = u4IO32Read4B(STA_FPGA_28);
        *(pData+4*i+1) = u4IO32Read4B(STA_FPGA_29);
        *(pData+4*i+2) = u4IO32Read4B(STA_FPGA_30);
        *(pData+4*i+3) = u4IO32Read4B(STA_FPGA_31);
    }
}

static void vDrvSetFpgaData(UINT32* pData, UINT32 u4Addr, UINT32 u4Size)
{    
    UINT32 i, u4Loop = u4Size/16;	
	
	vIO32WriteFldAlign(C_FPGA_CTRL_13, 0, DRAM_BURST);		  
	vIO32WriteFldAlign(C_FPGA_CTRL_13, 1, DRAM_RW);

    for (i = 0; i < u4Loop; i++)
    {        
        vIO32WriteFldAlign(C_FPGA_CTRL_8, u4Addr+16*i, DRAM_ADDR);

        vIO32Write4B(C_FPGA_CTRL_9,  *(pData+4*i+0));
        vIO32Write4B(C_FPGA_CTRL_10, *(pData+4*i+1));
        vIO32Write4B(C_FPGA_CTRL_11, *(pData+4*i+2));
        vIO32Write4B(C_FPGA_CTRL_12, *(pData+4*i+3));
		
        vIO32WriteFldAlign(C_FPGA_CTRL_13, 1, DRAM_TRIG);
        vIO32WriteFldAlign(C_FPGA_CTRL_13, 0, DRAM_TRIG);
    }
}

#endif  //FPGA_INDIRECT_ACCESS

/* =========================================================================================== */

static void vTimeLog(UINT8* string)
{    
    static UINT32 u4Count = 0;
    
    HAL_GetTime(&TimeNow);
    HAL_GetDeltaTime(&TimeDelta, &TimeStart, &TimeNow);
    
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_TIME_LOG))
    {
        u4Count++;
        Printf("%s  %d.%06d sec.\n", string, TimeDelta.u4Seconds, TimeDelta.u4Micros);
    }
    
    HAL_GetTime(&TimeStart);     
    
    if (u4Count > 256)
    {
        u4Count = 0;
        vIO32WriteFldAlign(SW_TDDC_REG_01, 0, TDDC_TIME_LOG);
    }
}


static void vTimeLog2(UINT8* string)
{    
    static UINT32 u4Count2 = 0;    
    static HAL_TIME_T TimeStart2, TimeNow2, TimeDelta2;
    
    HAL_GetTime(&TimeNow2);
    HAL_GetDeltaTime(&TimeDelta2, &TimeStart2, &TimeNow2);
    
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_ISR_LOG))
    {
        u4Count2++;
        Printf("%s  %d.%06d sec.\n", string, TimeDelta2.u4Seconds, TimeDelta2.u4Micros);
    }
    
    HAL_GetTime(&TimeStart2);     
    
    if (u4Count2 > 256)
    {
        u4Count2 = 0;
        vIO32WriteFldAlign(SW_TDDC_REG_01, 0, TDDC_ISR_LOG);
    }
}

static void vSystemLog(void)
{    
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DEBUG_LOG))
    {       
        Printf("\nTDDC  : W  %d    H  %d     \n", 
            IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_WIDTH), 
            IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_HEIGHT));
        Printf("DS4   : H  %d    V  %d\n", 
            IO32ReadFldAlign(TTD_04, TTD_DS4_H_MODE), 
            IO32ReadFldAlign(TTD_04, TTD_DS4_V_MODE));
        Printf("DV    : W  %d    H  %d\n", depth_width, depth_height);
        
        Printf("\n======== TDDC Dram Config =========\n");
        Printf("DSO L 0x%08x 0x%08x \n", 
            TDDC_PHY(gTddcCfg.u4DsoAddrL[0]), TDDC_PHY(gTddcCfg.u4DsoAddrL[1]));        
        Printf("    R 0x%08x 0x%08x \n", 
            TDDC_PHY(gTddcCfg.u4DsoAddrR[0]), TDDC_PHY(gTddcCfg.u4DsoAddrR[1]));        
        Printf("CID L 0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4CidAddrL[0]), TDDC_PHY(gTddcCfg.u4CidAddrL[1]));          
        Printf("    R 0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4CidAddrR[0]), TDDC_PHY(gTddcCfg.u4CidAddrR[1]));        
        Printf("MVO   0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4MvAddrOut[0]), TDDC_PHY(gTddcCfg.u4MvAddrOut[1]));
        Printf("MVI   0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4MvAddrIn[0]), TDDC_PHY(gTddcCfg.u4MvAddrIn[1]));
        
        vIO32WriteFldAlign(SW_TDDC_REG_01, 0, TDDC_DEBUG_LOG);
    }
}

static void vStatusLog(UINT8* string, UINT8 u1Status)
{
    static UINT32 u4Count = 0;
    
    UINT32 u4StatusSeq = (u4IO32Read4B(SW_TDDC_REG_0C)<<4) + u1Status;
    
    vIO32Write4B(SW_TDDC_REG_0C, u4StatusSeq);
    
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_STATUS_LOG))
    {
        u4Count++;
        Printf("%s \n", string);
    }

    if (u4Count > 256)
    {
        u4Count = 0;
        vIO32WriteFldAlign(SW_TDDC_REG_01, 0, TDDC_STATUS_LOG);
    }
}

/* =========================================================================================== 
static void IIR4(INT32 *cur, INT32 new_in, INT32 *final, INT32 weight)//weight4 IIR cur (8.3) new_in(8.0) final(8.0)
{
	*cur=(((4-weight)*(*cur)+(weight*new_in*8))*1);
	if(*cur>=0)	*cur=(*cur+2)/4;
	else		*cur=(*cur-2)/4;

	if(*cur>=0)	*final=(*cur+4)/8;
	else		*final=(*cur-4)/8;
}

static void IIR8(INT32 *cur, INT32 new_in, INT32 *final, INT32 weight)//weight8 IIR cur (8.4) new_in(8.0) final(8.0)
{
	*cur=(((8-weight)*(*cur)+(weight*new_in*16))*1);
	if(*cur>=0)	*cur=(*cur+4)/8;
	else		*cur=(*cur-4)/8;

	if(*cur>=0)	*final=(*cur+8)/16;
	else		*final=(*cur-8)/16;
}
*/

static void IIR16(INT32 *cur, INT32 new_in, INT32 *final, INT32 weight)//weight16 IIR cur (8.5) new_in(8.0) final(8.0)
{
	*cur=(((16-weight)*(*cur)+(weight*new_in*32))*1);
	if(*cur>=0)	*cur=(*cur+8)/16;
	else		*cur=(*cur-8)/16;

	if(*cur>=0)	*final=(*cur+16)/32;
	else		*final=(*cur-16)/32;
}

    
/* =========================================================================================== */
void vDrvTddcOccProcessNew(PEL *Cin_L, PEL *Cin_R, DVEC *level_L, DVEC *level_R, 
        INT32 *occ_map_L, INT32 *occ_map_R, INT32 *non_occ_L, INT32 *non_occ_R, INT32 *occ_diff_L, INT32 *occ_diff_R)
{	
    INT32 i = 0;
    INT32 x, y;
	INT32 c_ada_occ_base = IO32ReadFldAlign(SW_TDDC_REG_06, TDDC_OCC_DV_ERROR);
	INT32 TDDC_LR_PIX_ERROR = IO32ReadFldAlign(SW_TDDC_REG_06, TDDC_OCC_LR_ERROR); 
    INT32 L_disp_1, R_disp_1;
    INT32 tile_x = TDDC_DS*TDDC_MB_SIZE;

    tile_x = IO32ReadFldAlign(TTD_04, TTD_DS4_H_MODE) ? (tile_x/2) : tile_x;  

    occ_count_L = 0;
    occ_count_R = 0;    
    occ_count_L_2 = 0;
    occ_count_R_2 = 0;
    c_ada_occ = IO32ReadFldAlign(SW_TDDC_REG_06, TDDC_OCC_ADA_RANGE); 
    
    for (y=0; y<depth_height; y++)   // LRC L map
    {
        for (x=0; x<depth_width; x++)
        {
    		L_disp_1 = (level_L[i].x>=0) ? (level_L[i].x+tile_x/2)/tile_x : (level_L[i].x-tile_x/2)/tile_x;

    		if ((x+L_disp_1)>=0 && (x+L_disp_1)<=depth_width-1)
    		{   //has correspondence
    			if ( level_R[i+L_disp_1].x!=-512 &&  level_L[i].x!=-512 
                    && ABS(Cin_L[i]-Cin_R[i+L_disp_1])<=TDDC_LR_PIX_ERROR)
    			{   //LR perfact match
    				occ_diff_L[i]=MAX(c_ada_occ-MAX(ABS(level_L[i].x+level_R[i+L_disp_1].x)-c_ada_occ_base,0),0);
                    
    				if (occ_diff_L[i]==0)	//real occ			
                    {
                        occ_map_L[i] = 0;      
                        non_occ_L[i] = -128;          
                        occ_count_L++;  
                        occ_count_L_2++;
                    }
    				else if(occ_diff_L[i]==c_ada_occ)	//near occ
                    {
                        occ_map_L[i] = 255;    
                        non_occ_L[i] = level_L[i].x;      
                    }
    				else//perfect match								
                    { 
                        occ_map_L[i] = 128;    
                        non_occ_L[i] = level_L[i].x;       
                    }
					osd_map_L[i] = 0;
    			}
    			else //not found vector  and OSD
    			{   
    				occ_count_L++;		
                    occ_count_L_2++;
                    occ_map_L[i] = 0;
                    non_occ_L[i] = -128;	
                    occ_diff_L[i] = 0;
					if(ABS(Cin_L[i]-Cin_R[i+L_disp_1])>TDDC_LR_PIX_ERROR)
                    	osd_map_L[i] = TDDC_OSD_MARK;
					else 
						osd_map_L[i] = 0;
    			}
    		}
    		else
    		{   //out of boundary
    			occ_count_L++;		
                occ_map_L[i] = 0;	
                non_occ_L[i] = -128;	
                occ_diff_L[i] = 0;
				osd_map_L[i] = 0;
    		}
            
    		R_disp_1 = (level_R[i].x>=0) ? (level_R[i].x+tile_x/2)/tile_x : (level_R[i].x-tile_x/2)/tile_x;

            if ((x+R_disp_1)>=0 && (x+R_disp_1)<=depth_width-1)
    		{   //has correspondence
    			if( level_R[i].x!=-512 &&  level_L[i+R_disp_1].x!=-512 
                    && ABS(Cin_R[i]-Cin_L[i+R_disp_1]) <= TDDC_LR_PIX_ERROR)
    			{   //LR perfact match
    				occ_diff_R[i]=MAX(c_ada_occ-MAX(ABS(level_R[i].x+level_L[i+R_disp_1].x)-c_ada_occ_base,0),0);

                    if (occ_diff_R[i]==0)			
                    {
                        occ_map_R[i] = 0;      
                        non_occ_R[i] = -128;           
                        occ_count_R++;  
                        occ_count_R_2++;  
                    }
    				else if (occ_diff_R[i]==c_ada_occ)	
                    {
                        occ_map_R[i] = 255;    
                        non_occ_R[i] = level_R[i].x;                   
                    }
    				else								
                    {
                        occ_map_R[i] = 128;    
                        non_occ_R[i] = level_R[i].x;                   
                    }
					osd_map_R[i] = 0;
    			}
    			else
    			{   
    			    occ_count_R++;		
                    occ_count_R_2++;
                    occ_map_R[i] = 0;		
                    non_occ_R[i] = -128;	
                    occ_diff_R[i] = 0;
					if(ABS(Cin_R[i]-Cin_L[i+R_disp_1]) > TDDC_LR_PIX_ERROR)
						osd_map_R[i] = TDDC_OSD_MARK;
					else
						osd_map_R[i] = 0;
    			}
    		}
    		else
    		{   //out of boundary
    		    occ_count_R++;		
                occ_map_R[i] = 0;		
                non_occ_R[i] = -128;	
                occ_diff_R[i] = 0;
				osd_map_R[i] = 0;
    		}       
            
            i++;
        }
	}    
}

void vDrvTddcFallBack(void)
{
    INT32 fb_local_gain, dp_local_gain;
    INT32 max_dp_diff;
	INT32 occ_count_AVG = (occ_count_L+occ_count_R)/2;
    
	//occlusion-based adaptive fallback	
    INT32 c_occ_fb_L = IO32ReadFldAlign(SW_TDDC_REG_03, TDDC_OCC_THR_LOW);     
    INT32 c_occ_fb_range = IO32ReadFldAlign(SW_TDDC_REG_03, TDDC_OCC_THR_RANGE);
	INT32 c_dp_thld = IO32ReadFldAlign(SW_TDDC_REG_02, TDDC_DP_THLD);
	INT32 c_occ_fb_U = c_occ_fb_L + c_occ_fb_range;
	
    if (occ_count_AVG > c_occ_fb_U)								
        fb_local_gain = 0;
	else if ((occ_count_AVG > c_occ_fb_L) && (occ_count_AVG <= c_occ_fb_U))	
        fb_local_gain = MAX((c_occ_fb_range-occ_count_AVG+c_occ_fb_L), 0)*32/c_occ_fb_range;   
	else 															
        fb_local_gain = 32;

	max_dp_diff = MAX(ABS(max_local_diff_L), ABS(min_local_diff_L)) 
					+ MAX(ABS(max_local_diff_L), ABS(min_local_diff_L))/2;

	if (max_dp_diff == 0)		
	{
        dp_local_gain = 32;
	}
    else
    {                       
        dp_local_gain = MIN(c_dp_thld*32/max_dp_diff, 32);
	}
    
	// IIR of DIBR local gain
	IIR16(&fb_local_gain_IIR, fb_local_gain, &fb_smth_local_gain, 1);
	IIR16(&dp_local_gain_IIR, dp_local_gain, &dp_smth_local_gain, 1);
}

void vDrvTddcHoleFill_L(INT32 *fi_im, DVEC *level, INT32 *occ_map, INT32 *occ_diff)
{
    INT32 x, y, addr;    
    INT32 x_max, x_min, y_max, y_min;
	INT32 width  = depth_width;
	INT32 height = depth_height;
	INT32 result, count, sum_neighbor;    
	INT32 diff, x_nbr;
    
	max_local_diff_L=0; max_depth_L=-512;
	min_local_diff_L=0; min_depth_L=512;	
	
	for (addr=0; addr<(width*height); addr++)
	{
        check_map[addr] = (occ_diff[addr]==0) ? 0 : 1;
        complete_map[addr] = (occ_diff[addr]==c_ada_occ) ? 1 : 0;
	}
    
	//UL filling
	for (y=0;y<height;y++)
	for (x=0;x<width;x++)
	{	
        addr = x+y*width;
        
		if (complete_map[addr]==0)
		{
			x_min = MAX(x-1, 0);
			y_min = MAX(y-1, 0);
            
			if ((check_map[x+y_min*width]==1)||(check_map[x_min+y*width]==1))
			{
				count = 0;
				sum_neighbor = 0;
                
				if(x-1>=0	 && check_map[addr-1    ]!=0)	{	sum_neighbor+=fi_im[addr-1    ];	count++;	}//L
				if(x+1<width && check_map[addr+1    ]!=0)	{	sum_neighbor+=level[addr+1    ].x;	count++;	}//R
				if(y-1>=0	 && check_map[addr-width]!=0)	{	sum_neighbor+=fi_im[addr-width];	count++;	}//U
				if(y+1<height&& check_map[addr+width]!=0)	{	sum_neighbor+=level[addr+width].x;	count++;	}//D
				
				if(count==0)	
                    check_map[addr]=0;
				else
				{	
					result=sum_neighbor/count; 
					if(c_ada_occ==0)
                        fi_im[addr]=result;
					else
					    fi_im[addr]=(occ_diff[addr]*level[addr].x+(c_ada_occ-occ_diff[addr])*result+c_ada_occ/2)/c_ada_occ;
					check_map[addr]=1;
					complete_map[addr]=1;
				}
			}
		}
		else
		{
			fi_im[addr]=level[addr].x;	
            check_map[addr]=1; 
		}	
	}
    
	//DR filling
	for (y=height-1;y>=0;y--)
	for (x=width-1;x>=0;x--)
	{	
        addr = x+y*width;
        
		if(complete_map[addr]==0)
		{
			y_max = MIN(y+1, height-1);
			x_max = MIN(x+1, width-1);
            
			if ((check_map[x+y_max*width]==1)||(check_map[x_max+y*width]==1))
			{
				count=0;
				sum_neighbor=0;
                
				if(x-1>=0	 && check_map[addr-1    ]!=0)	{	sum_neighbor+=fi_im[addr-1    ];	count++;	}//L
				if(x+1<width && check_map[addr+1    ]!=0)	{	sum_neighbor+=fi_im[addr+1    ];	count++;	}//R
				if(y-1>=0	 && check_map[addr-width]!=0)	{	sum_neighbor+=fi_im[addr-width];	count++;	}//U
				if(y+1<height&& check_map[addr+width]!=0)	{	sum_neighbor+=fi_im[addr+width];	count++;	}//D
				
				if(count==0)	
                    check_map[addr]=0;
				else				
				{	
					result=sum_neighbor/count; 
					if(c_ada_occ==0)
                        fi_im[addr]=result;
					else					
					    fi_im[addr]=(occ_diff[addr]*level[addr].x+(c_ada_occ-occ_diff[addr])*result+c_ada_occ/2)/c_ada_occ;
					check_map[addr]=1;
					complete_map[addr]=1;
				}
			}
		}

		x_nbr = (x+1>=width) ? (width-1) : (x+1);        
		diff = fi_im[addr]-fi_im[x_nbr+y*width];
        
		if (diff>max_local_diff_L)			max_local_diff_L=diff;
		if (diff<min_local_diff_L)			min_local_diff_L=diff;

		if (fi_im[addr]<min_depth_L)	    min_depth_L=fi_im[addr];
		if (fi_im[addr]>max_depth_L)	    max_depth_L=fi_im[addr];
	}
}

void vDrvTddcHoleFill_R(INT32 *fi_im, DVEC *level, INT32 *occ_map, INT32 *occ_diff)
{
    INT32 x, y, addr;
    INT32 x_max, x_min, y_max, y_min;
	INT32 width  = depth_width;
	INT32 height = depth_height;
	INT32 result, count, sum_neighbor;
	INT32 diff, x_nbr;
    
	max_local_diff_R=0;	max_depth_R=-512;
	min_local_diff_R=0;	min_depth_R=512;

	for (addr=0; addr<(width*height); addr++)
	{
        check_map[addr] = (occ_diff[addr]==0) ? 0 : 1;
        complete_map[addr] = (occ_diff[addr]==c_ada_occ) ? 1 : 0;
	}
    
	//UL filling
	for (y=0;y<height;y++)
	for (x=width-1;x>=0;x--)
	{	
        addr = x+y*width;

		if (complete_map[addr]==0)
		{
			x_max = MIN(x+1, width-1);
			y_min = MAX(y-1, 0);
            
			if ((check_map[x+y_min*width]==1)||(check_map[x_max+y*width]==1))
			{	
				count = 0;
				sum_neighbor = 0;
                
				if(x-1>=0	 && check_map[addr-1    ]!=0)	{	sum_neighbor+=level[addr-1    ].x;	count++;	}//L
				if(x+1<width && check_map[addr+1    ]!=0)	{	sum_neighbor+=fi_im[addr+1    ];	count++;	}//R
				if(y-1>=0	 && check_map[addr-width]!=0)	{	sum_neighbor+=fi_im[addr-width];	count++;	}//U
				if(y+1<height&& check_map[addr+width]!=0)	{	sum_neighbor+=level[addr+width].x;	count++;	}//D
				
				if(count==0)	
                    check_map[addr]=0;
				else				
				{	
					result=sum_neighbor/count; 
					if(c_ada_occ==0)
                        fi_im[addr]=result;
					else
					    fi_im[addr]=(occ_diff[addr]*level[addr].x+(4-occ_diff[addr])*result+2)/4; 
					check_map[addr]=1;
					complete_map[addr]=1;
				}
			}
		}
		else
		{
			fi_im[addr]=level[addr].x;	
            check_map[addr]=1;
		}	
	}
    
	//DR filling
	for (y=height-1;y>=0;y--)
	for (x=0;x<width;x++)
	{	
        addr = x+y*width;
        
		if(complete_map[x+y*width]==0)
		{            
			x_min = MAX(x-1, 0);
			y_max = MIN(y+1, height-1);
            
			if ((check_map[x+y_max*width]==1)||(check_map[x_min+y*width]==1))
			{	
				count=0;
				sum_neighbor=0;
                
				if(x-1>=0	 && check_map[addr-1    ]!=0)	{	sum_neighbor+=fi_im[addr-1    ];    count++;	}//L
				if(x+1<width && check_map[addr+1    ]!=0)	{	sum_neighbor+=fi_im[addr+1    ];	count++;	}//R
				if(y-1>=0	 && check_map[addr-width]!=0)	{	sum_neighbor+=fi_im[addr-width];	count++;	}//U
				if(y+1<height&& check_map[addr+width]!=0)	{	sum_neighbor+=fi_im[addr+width];	count++;	}//D

				if(count==0)	
                    check_map[addr]=0;
				else
				{	
					result=sum_neighbor/count; 
					if(c_ada_occ==0)
                        fi_im[addr]=result;
					else
					    fi_im[addr]=(occ_diff[addr]*level[addr].x+(4-occ_diff[addr])*result+2)/4; 
					check_map[addr]=1;
					complete_map[addr]=1;
				}
			}
		}
        
		x_nbr = (x-1<0) ? 0 : (x-1);
		diff = fi_im[addr]-fi_im[x_nbr+y*width];
        
		if (diff>max_local_diff_R)		max_local_diff_R=diff;
		if (diff<min_local_diff_R)		min_local_diff_R=diff;
        
		if (fi_im[addr]<min_depth_R)	min_depth_R=fi_im[addr];
		if (fi_im[addr]>max_depth_R)	max_depth_R=fi_im[addr];
	}
}

void vDrvTddcDrawMaskSquare(UINT32 str_x, UINT32 str_y, UINT32 end_x, UINT32 end_y, INT32 *mask, INT32 depth)
{
    UINT32 x, y;

	for(x=str_x; x<=end_x; x++)
	{
    	for(y=str_y; y<=end_y; y++)
    	{
    		mask[x+y*depth_width] = depth;
    	}
    }
}


UINT32 u4DrvTddcDetectOSDinMask(UINT32 str_x, UINT32 str_y, UINT32 end_x, UINT32 end_y, INT32 *mask)
{
    UINT32 x, y;
	UINT32 osd_mask_count=0;
	UINT32 osd_mask_detect=0;
	UINT32 osd_ratio = IO32ReadFldAlign(SW_TDDC_REG_14, TDDC_OSD_RATIO);
	
	for (x=str_x; x<=end_x; x++)
	{
    	for (y=str_y; y<=end_y; y++)
    	{
    		if (mask[x+y*depth_width]==TDDC_OSD_MARK)  
    		    osd_mask_count++;
    	}
    }
    
	if (osd_mask_count > (((end_x-str_x+1)*(end_y-str_y+1)*osd_ratio)>>8))
	{
		osd_mask_detect=1;
	}
	
	return osd_mask_detect;
}


void vDrvTddcGetBGDepth(UINT32 str_x, UINT32 str_y, UINT32 end_x, UINT32 end_y, INT32 *hole_buf)
{
	INT32 x, y;
	INT32 sum_depth=0;
	INT32 count=0;
	INT32 depth;
	
	y=str_y-1;
	if(y>=0&&y<depth_height)
	for(x=str_x;x<=end_x;x++)
	{
		sum_depth+=hole_buf[x+y*depth_width];
		count++;
	}
	
	y=end_y+1;
	if(y>=0&&y<depth_height)
	for(x=str_x;x<=end_x;x++)
	{
		sum_depth+=hole_buf[x+y*depth_width];
		count++;
	}
	
	x=str_x-1;
	if(x>=0&&x<depth_width)
	for(y=str_y;y<=end_y;y++)
	{
		sum_depth+=hole_buf[x+y*depth_width];
		count++;
	}

	x=end_x+1;
	if(x>=0&&x<depth_width)
	for(y=str_y;y<=end_y;y++)
	{
		sum_depth+=hole_buf[x+y*depth_width];
		count++;
	}

    depth = (count) ? (sum_depth/count) : 0;
	vDrvTddcDrawMaskSquare(str_x, str_y, end_x, end_y, hole_buf, depth);
}

void vDrvTddcSmoothOSDDepth(INT32 *smooth_buf, INT32 *hole_buf, INT32 *osd_buf)
{
    INT32 x, y;
	INT32 width = depth_width;
	INT32 height = depth_height;
	INT32 sum_neighbor;
	
	for(y=0;y<height;y++)
	for(x=0;x<width;x++)
	{	
		if(osd_buf[x+y*width]==TDDC_OSD_MARK)
		{
			sum_neighbor=0;
			if(y-2>=0	 ){	sum_neighbor+=  hole_buf[x+  (y-2)*width];}//U
			if(y-1>=0	 ){	sum_neighbor+=2*hole_buf[x+  (y-1)*width];}//U
			if(y+1<height){	sum_neighbor+=2*hole_buf[x+  (y+1)*width];}//D
			if(y+2<height){	sum_neighbor+=  hole_buf[x+  (y+2)*width];}//D
							sum_neighbor+=2*hole_buf[x+   y   *width];

			smooth_buf[x+y*width]=(sum_neighbor+4)/8; 
		}
		else if(osd_buf[x+(y-1)*width]==TDDC_OSD_MARK&&(y-1)>=0)
		{
			sum_neighbor=0;
			if(y-1>=0	 ){	sum_neighbor+=1*hole_buf[x+  (y-1)*width];}//U
			if(y+1<height){	sum_neighbor+=1*hole_buf[x+  (y+1)*width];}//D
							sum_neighbor+=2*hole_buf[x+   y   *width];

			smooth_buf[x+y*width]=(sum_neighbor+2)/4; 
		}
		else if(osd_buf[x+(y+1)*width]==TDDC_OSD_MARK&&(y+1)<height)
		{
			sum_neighbor=0;
			if(y-1>=0	 ){	sum_neighbor+=1*hole_buf[x+  (y-1)*width];}//U
			if(y+1<height){	sum_neighbor+=1*hole_buf[x+  (y+1)*width];}//D
							sum_neighbor+=2*hole_buf[x+   y   *width];

			smooth_buf[x+y*width]=(sum_neighbor+2)/4; 			
		}
		else
			smooth_buf[x+y*width]=hole_buf[x+y*width];
	}
}

void vDrvTddcSetOsdDetect(void)
{
    static E_TD_IN  eMibInPre = E_TD_IN_END;
	E_TD_IN  eMibInCur = TD_MIB_IN(u4DrvTDTVDIModeQuery());

	if (eMibInCur != eMibInPre)
	{
		eMibInPre = eMibInCur;
	}
	else
	{
		return;
	}

    switch (eMibInCur)
    {
        case E_TD_IN_TB_I: 
        case E_TD_IN_TB_P:        
            vIO32WriteFldMulti(SW_TDDC_REG_10, P_Fld(0x7, TDDC_OSD_L_STR_X)
                                        | P_Fld(0x26, TDDC_OSD_L_END_X));        
            vIO32WriteFldMulti(SW_TDDC_REG_11, P_Fld(0x07, TDDC_OSD_L_STR_Y)
                                        | P_Fld(0x1A, TDDC_OSD_L_END_Y));
            vIO32WriteFldMulti(SW_TDDC_REG_14, P_Fld(0x1, TDDC_OSD_DET_EN)
                                            | P_Fld(0x1, TDDC_OSD_DET_INDEX)
                                            | P_Fld(0x30, TDDC_OSD_RATIO));
            break;
        case E_TD_IN_SBS_I:      
        case E_TD_IN_SBS_P:            
            vIO32WriteFldMulti(SW_TDDC_REG_10, P_Fld(0x11, TDDC_OSD_L_STR_X)
                                        | P_Fld(0x4F, TDDC_OSD_L_END_X));        
            vIO32WriteFldMulti(SW_TDDC_REG_11, P_Fld(0x02, TDDC_OSD_L_STR_Y)
                                        | P_Fld(0x0D, TDDC_OSD_L_END_Y));
            vIO32WriteFldMulti(SW_TDDC_REG_14, P_Fld(0x1, TDDC_OSD_DET_EN)
                                            | P_Fld(0x1, TDDC_OSD_DET_INDEX)
                                            | P_Fld(0x30, TDDC_OSD_RATIO));
            break;            
        default:
            vIO32WriteFldMulti(SW_TDDC_REG_14, P_Fld(0x0, TDDC_OSD_DET_EN)
                                            | P_Fld(0x0, TDDC_OSD_DET_INDEX)
                                            | P_Fld(0x30, TDDC_OSD_RATIO));            
            break;
    }
}


void vDrvTddcOsdDetect(INT32 *fi_im, DVEC *level, INT32 *occ_map, INT32 *occ_diff)
{
    UINT32 L_str_x, L_end_x, L_str_y, L_end_y;
    UINT32 R_str_x, R_end_x, R_str_y, R_end_y;

    if (IO32ReadFldAlign(SW_TDDC_REG_14, TDDC_OSD_DET_EN))
    {		
        vDrvTddcSetOsdDetect();
		
        L_str_x = IO32ReadFldAlign(SW_TDDC_REG_10, TDDC_OSD_L_STR_X);
        L_end_x = IO32ReadFldAlign(SW_TDDC_REG_10, TDDC_OSD_L_END_X);
        L_str_y = IO32ReadFldAlign(SW_TDDC_REG_11, TDDC_OSD_L_STR_Y);
        L_end_y = IO32ReadFldAlign(SW_TDDC_REG_11, TDDC_OSD_L_END_Y);
        R_str_x = IO32ReadFldAlign(SW_TDDC_REG_12, TDDC_OSD_R_STR_X);
        R_end_x = IO32ReadFldAlign(SW_TDDC_REG_12, TDDC_OSD_R_END_X);
        R_str_y = IO32ReadFldAlign(SW_TDDC_REG_13, TDDC_OSD_R_STR_Y);
        R_end_y = IO32ReadFldAlign(SW_TDDC_REG_13, TDDC_OSD_R_END_Y);

		L_str_x = (L_str_x>=depth_width)?(depth_width-1):L_str_x;
		L_end_x = (L_end_x>=depth_width)?(depth_width-1):L_end_x;
		R_str_x = (R_str_x>=depth_width)?(depth_width-1):R_str_x;
		R_end_x = (R_end_x>=depth_width)?(depth_width-1):R_end_x;

		L_str_y = (L_str_y>=depth_height)?(depth_height-1):L_str_y;
		L_end_y = (L_end_y>=depth_height)?(depth_height-1):L_end_y;
		R_str_y = (R_str_y>=depth_height)?(depth_height-1):R_str_y;
		R_end_y = (R_end_y>=depth_height)?(depth_height-1):R_end_y;
    
        switch (IO32ReadFldAlign(SW_TDDC_REG_14, TDDC_OSD_DET_INDEX))
        {   
            case 0:
        	    vDrvTddcDrawMaskSquare(L_str_x, L_str_y, L_end_x, L_end_y, osd_map_L, TDDC_OSD_MARK);
        	    vDrvTddcDrawMaskSquare(R_str_x, R_str_y, R_end_x, R_end_y, osd_map_R, TDDC_OSD_MARK);
        	    break;
            case 1:            
            	if (u4DrvTddcDetectOSDinMask(L_str_x, L_str_y, L_end_x, L_end_y, osd_map_L))
            	{
            	    vDrvTddcGetBGDepth(L_str_x, L_str_y, L_end_x, L_end_y, hole_buf_L);            	    
                    vDrvTddcDrawMaskSquare(L_str_x, L_str_y, L_end_x, L_end_y, osd_map_L, TDDC_OSD_MARK);
					vDrvTddcDrawMaskSquare(L_str_x, L_str_y, L_end_x, L_end_y, osd_map_R, TDDC_OSD_MARK);
            	}
            	if (u4DrvTddcDetectOSDinMask(R_str_x, R_str_y, R_end_x, R_end_y, osd_map_R))
            	{
            	    vDrvTddcGetBGDepth(R_str_x, R_str_y, R_end_x, R_end_y, hole_buf_R);            	    
                    vDrvTddcDrawMaskSquare(R_str_x, R_str_y, R_end_x, R_end_y, osd_map_R, TDDC_OSD_MARK);
					vDrvTddcDrawMaskSquare(R_str_x, R_str_y, R_end_x, R_end_y, osd_map_L, TDDC_OSD_MARK);
            	}
            	break;
            case 2:            
                vDrvTddcSmoothOSDDepth(osd_sm_buf_L, hole_buf_L, osd_map_L);
                vDrvTddcSmoothOSDDepth(osd_sm_buf_R, hole_buf_R, osd_map_R);
                x_memcpy(hole_buf_L, osd_sm_buf_L, sizeof(DVEC)*depth_width*depth_height);
                x_memcpy(hole_buf_R, osd_sm_buf_R, sizeof(DVEC)*depth_width*depth_height);
                break;
        }
    }
}

void vDrvTddcTmprSmooth(INT32 *final_depth_buf_L, INT32 *final_depth_buf_R, INT32 *cur_depth_buf_L, INT32 *cur_depth_buf_R)
{
    INT32 i;
    
    //if(u1GetFlipMirrorConfig() == SYS_MIRROR_CONFIG_ON)
    if (fgDrvMJCGetFlipMirror() & 1)
    {
    	//IIR the DMAP data and get global shift of Depth Map
    	for (i=0; i<depth_width*depth_height; i++)
    	{
            final_depth_buf_R[i] = (cur_depth_buf_L[i]+3*final_depth_buf_R[i]+2)>>2;
            final_depth_buf_L[i] = (cur_depth_buf_R[i]+3*final_depth_buf_L[i]+2)>>2;
    	}
    }
	else
	{        
    	for (i=0; i<depth_width*depth_height; i++)
    	{
    		final_depth_buf_L[i] = (cur_depth_buf_L[i]+3*final_depth_buf_L[i]+2)>>2;
            final_depth_buf_R[i] = (cur_depth_buf_R[i]+3*final_depth_buf_R[i]+2)>>2;
    	}
	}
}
/* =========================================================================================== */
static void AutoConversion(INT32 *final_depth_buf_L)
{
    INT32 i;    
    INT32 AutoC_global_th0, AutoC_global_th1, AutoC_global_th2;
    INT32 AutoC_local_th0, AutoC_local_th1, AutoC_local_th2, AutoC_local_th3, AutoC_local_th4, AutoC_local_th5, AutoC_local_thln;
    INT32 SearchLocalMax = 0, SearchLocalMin = 100;
    INT32 AutoC_Global_Fac, AutoC_Local_Fac1;
    UINT32 AutoC_Local_value;
    INT32 AutoC_Global_value, AutoC_Global_limit, AutoC_Final_value;
    static INT32 AutoC_Final_value_IIR = 0;
    static INT32 AutoC_Target_value = 0;
    static INT32 AutoC_Out_valueRec = 0;    
    UINT32 bAutoC_IIR = 0;
    UINT32 LocalMaxCnt = 0, LocalMinCnt = 0;        
    static UINT32 LowCnt0, LowCntIIR;
    UINT32 ResetFrame;
    UINT32 AutoC_local_blkth0, AutoC_local_blkth1, AutoC_local_blkth2, AutoC_local_blkth3, AutoC_local_blkth4, AutoC_local_blkthln;

    //Get local min/max info
    for(i=0; i<depth_width*depth_height; i++)
    {
        if((final_shift_H - final_depth_buf_L[i]) > SearchLocalMax)
        {
            SearchLocalMax = (final_shift_H - final_depth_buf_L[i]);
            LocalMaxCnt = 1;
        }
        else
        {
            LocalMaxCnt++;
        }
        
        if((final_shift_H - final_depth_buf_L[i]) < SearchLocalMin)
        {
            SearchLocalMin = (final_shift_H - final_depth_buf_L[i]);
            LocalMinCnt = 1;
        }
        else
        {
            LocalMinCnt++;
        }
    }
    
    vIO32WriteFldAlign(SW_AUTO_CONV_07, SearchLocalMax, AUTO_CONV_LOCAL_MAX);
    vIO32WriteFldAlign(SW_AUTO_CONV_07, SearchLocalMin, AUTO_CONV_LOCAL_MIN);    
    vIO32WriteFldAlign(SW_AUTO_CONV_04, LocalMaxCnt, AUTO_CONV_LOCAL_FAC2); 

    if(g_u1AutoConvEnable)
    {
        AutoC_global_th0 = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_GLOBAL_TH_0);
        AutoC_global_th1 = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_GLOBAL_TH_1);
        AutoC_global_th2 = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_GLOBAL_TH_2);
        AutoC_Global_limit = IO32ReadFldAlign(SW_AUTO_CONV_08, AUTO_CONV_GLOBAL_LIMIT);        
        
        AutoC_local_th0 = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_01, AUTO_CONV_LOCAL_TH_0);
        AutoC_local_th1 = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_01, AUTO_CONV_LOCAL_TH_1);
        AutoC_local_th2 = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_01, AUTO_CONV_LOCAL_TH_2);
        AutoC_local_th3 = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_01, AUTO_CONV_LOCAL_TH_3);
        AutoC_local_th4 = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_09, AUTO_CONV_LOCAL_TH_4);
        AutoC_local_th5 = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_09, AUTO_CONV_LOCAL_TH_5);
        
        AutoC_local_blkth0 = IO32ReadFldAlign(SW_AUTO_CONV_02, AUTO_CONV_LOCAL_BLKTH_0);
        AutoC_local_blkth1 = IO32ReadFldAlign(SW_AUTO_CONV_02, AUTO_CONV_LOCAL_BLKTH_1);
        AutoC_local_blkth2 = IO32ReadFldAlign(SW_AUTO_CONV_02, AUTO_CONV_LOCAL_BLKTH_2);
        AutoC_local_blkth3 = IO32ReadFldAlign(SW_AUTO_CONV_09, AUTO_CONV_LOCAL_BLKTH_3);
        AutoC_local_blkth4 = IO32ReadFldAlign(SW_AUTO_CONV_09, AUTO_CONV_LOCAL_BLKTH_4);        
        AutoC_local_blkthln = IO32ReadFldAlign(SW_AUTO_CONV_02, AUTO_CONV_LOCAL_BLKTH_LN);
        bAutoC_IIR = IO32ReadFldAlign(SW_AUTO_CONV_03, AUTO_CONV_IIR_STRENTH);
        ResetFrame = IO32ReadFldAlign(SW_AUTO_CONV_06, AUTO_CONV_RESET_FRAME);

        AutoC_Global_Fac = final_shift_H + (INT32)(final_shift_H*global_gain/64) + (g_u4ProtrudeFromScreen-0x200)/4;
        AutoC_Local_Fac1 = SearchLocalMax;
        vIO32WriteFldAlign(SW_AUTO_CONV_03, AutoC_Global_Fac, AUTO_CONV_GLOBAL_FAC);
        vIO32WriteFldAlign(SW_AUTO_CONV_04, AutoC_Local_Fac1, AUTO_CONV_LOCAL_FAC1);
     
        //Global conversion value
        if(IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_GLOBAL_CURVE_MODE))
        {
            if(AutoC_Global_Fac >= AutoC_global_th0)
            {
                AutoC_Global_value = ABS(AutoC_Global_Fac - AutoC_global_th0);
                AutoC_Global_value = CLIP((INT32)(AutoC_Global_value*IO32ReadFldAlign(SW_AUTO_CONV_08, AUTO_CONV_GB_CRV_GAIN) + 0x20) / 0x40, 0, 
                                      AutoC_Global_limit);
            }
            else if(AutoC_Global_Fac < (-1*AutoC_global_th0))
            {
                AutoC_Global_value = ABS((-1*AutoC_global_th0) - AutoC_Global_Fac);
                AutoC_Global_value = CLIP((INT32)(AutoC_Global_value*IO32ReadFldAlign(SW_AUTO_CONV_08, AUTO_CONV_GB_CRV_GAIN) + 0x20) / 0x40, 0, 
                                      AutoC_Global_limit);
                AutoC_Global_value = -1*AutoC_Global_value;
            }
            else
                AutoC_Global_value = 0;
        }
        else
        {
            if(AutoC_Global_Fac >= AutoC_global_th2)
                AutoC_Global_value = 3;
            else if(AutoC_Global_Fac >= AutoC_global_th1)
                AutoC_Global_value = 2;
            else if(AutoC_Global_Fac >= AutoC_global_th0)
                AutoC_Global_value = 1;

            else
                AutoC_Global_value = 0;

            AutoC_Global_value = CLIP((AutoC_Global_value*IO32ReadFldAlign(SW_AUTO_CONV_08, AUTO_CONV_GB_CRV_GAIN) + 0x20) / 0x40, 0, 
                                      AutoC_Global_limit);

        }
                                    
        //Local conversion value
        if(IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_LOCAL_CURVE_MODE))
        {
            if(LocalMaxCnt >= (AutoC_local_blkth4<<3))
                AutoC_local_thln = AutoC_local_th5;
            else if(LocalMaxCnt >= (AutoC_local_blkth3<<3))
                AutoC_local_thln = AutoC_local_th4;
            else if(LocalMaxCnt >= (AutoC_local_blkth2<<3))
                AutoC_local_thln = AutoC_local_th3;
            else if(LocalMaxCnt >= (AutoC_local_blkth1<<3))
                AutoC_local_thln = AutoC_local_th2;
            else if(LocalMaxCnt >= (AutoC_local_blkth0<<3))
                AutoC_local_thln = AutoC_local_th1;
            else
                AutoC_local_thln = AutoC_local_th0;

            vIO32WriteFldAlign(SW_AUTO_CONV_03, AutoC_local_thln, AUTO_CONV_LOCAL_TH_LN);

            if((LocalMaxCnt >= (AutoC_local_blkthln<<3)) && (AutoC_Local_Fac1 >= AutoC_local_thln))
                AutoC_Local_value = (UINT32)(AutoC_Local_Fac1 - AutoC_local_thln);
            else
                AutoC_Local_value = 0;
        }
        else
        {  
            
            if(LocalMaxCnt >= AutoC_local_blkth2)
            {
                if(AutoC_Local_Fac1 >= AutoC_local_th0)
                    AutoC_Local_value = 3;
                else if(AutoC_Local_Fac1 >= AutoC_local_th0)
                    AutoC_Local_value = 3;
                else
                    AutoC_Local_value = 0;
            }
            else if(LocalMaxCnt >= AutoC_local_blkth1)
            {
                if(AutoC_Local_Fac1 >= AutoC_local_th2)
                    AutoC_Local_value = 3;
                else if(AutoC_Local_Fac1 >= AutoC_local_th1)
                    AutoC_Local_value = 2;
                else if(AutoC_Local_Fac1 >= AutoC_local_th0)
                    AutoC_Local_value = 1;

                else
                    AutoC_Local_value = 0;
            }
            else if(LocalMaxCnt >= AutoC_local_blkth0)       
            {
                if(AutoC_Local_Fac1 >= AutoC_local_th2)
                    AutoC_Local_value = 1;

                else
                    AutoC_Local_value = 0;
            }
            else
            {
                AutoC_Local_value = 0;
            }
        }
            AutoC_Local_value = CLIP((UINT32)((AutoC_Local_value*(UINT8)IO32ReadFldAlign(SW_AUTO_CONV_08, AUTO_CONV_LC_CRV_GAIN)) / 0x40), 0, 
                                    IO32ReadFldAlign(SW_AUTO_CONV_08, AUTO_CONV_LOCAL_LIMIT));

        AutoC_Final_value = AutoC_Global_value + AutoC_Local_value;

        if(IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_IIR_EN))
        {
            switch(bAutoC_IIR)
            {
                case 0:
                    AutoC_Final_value_IIR = (AutoC_Final_value_IIR * 15 + 8) / 16 + AutoC_Final_value;
                    AutoC_Final_value = (AutoC_Final_value_IIR + 8) / 16;
                break;
                case 1:
                    AutoC_Final_value_IIR = (AutoC_Final_value_IIR * 31 + 16) / 32 + AutoC_Final_value;
                    AutoC_Final_value = (AutoC_Final_value_IIR + 16) / 32;
                break;
                case 2:
                    AutoC_Final_value_IIR = (AutoC_Final_value_IIR * 63 + 32) / 64 + AutoC_Final_value;
                    AutoC_Final_value = (AutoC_Final_value_IIR + 32) / 64;
                break;
                default:
                    AutoC_Final_value_IIR = (AutoC_Final_value_IIR * 63 + 32) / 64 + AutoC_Final_value;
                    AutoC_Final_value = (AutoC_Final_value_IIR + 32) / 64;
                break;                    
            }

            if((AutoC_Final_value == 1) && ((AutoC_Global_value + AutoC_Local_value) != 1) && (LowCntIIR >= (ResetFrame*(5<<bAutoC_IIR))))
            {
                AutoC_Final_value = 0;
                AutoC_Final_value_IIR = 0;
                LowCntIIR = 0;
            }
            else
            {
                LowCntIIR++;
            }            
        }
        else
        {
            AutoC_Final_value_IIR = 0;
        }
            
        vIO32WriteFldAlign(SW_AUTO_CONV_05, AutoC_Local_value, AUTO_CONV_LOCAL_PXL);
        vIO32WriteFldAlign(SW_AUTO_CONV_05, AutoC_Global_value, AUTO_CONV_GLOBAL_PXL);        
        vIO32WriteFldAlign(SW_AUTO_CONV_05, AutoC_Final_value, AUTO_CONV_FINAL_PXL);

        if(IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_DEBUG))
        {
            AutoC_Final_value = (INT32)IO32ReadFldAlign(SW_AUTO_CONV_05, AUTO_CONV_DEBUG_PXL)-0x80;
        }
        
        if(ABS(AutoC_Target_value - AutoC_Final_value) > IO32ReadFldAlign(SW_AUTO_CONV_06, AUTO_CONV_CHANGE_THD))
            AutoC_Target_value = AutoC_Final_value;

        if(AutoC_Out_value > AutoC_Target_value)
            AutoC_Out_value = AutoC_Out_value - 1;
        else if(AutoC_Out_value < AutoC_Target_value)
            AutoC_Out_value = AutoC_Out_value + 1;

        if((AutoC_Final_value != AutoC_Out_value) && (LowCnt0 >= ResetFrame))
        {
            AutoC_Out_value = AutoC_Final_value;
            AutoC_Target_value = AutoC_Final_value;
            LowCnt0 = 0;
        }
        else
        {
            LowCnt0++;
        }
    }
    else
    {
        AutoC_Out_value = 0;
    }

    vIO32WriteFldAlign(SW_AUTO_CONV_06, AutoC_Out_value, AUTO_CONV_OUTPUT_PXL);
    g_u4AutoConvOutPxl = ABS(AutoC_Out_value);
    g_u1AutoConvBlkBarUpdate = (AutoC_Out_valueRec == AutoC_Out_value) ? SV_OFF : SV_ON;
    g_u1AutoConvBlkBarPxl = IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_BLACKBAR_ENABLE) ? g_u4AutoConvOutPxl: 0;    

    AutoC_Out_valueRec = AutoC_Out_value;
}

static void AdaptiveGlobalGain(void)
{
    static UINT32 GlobalGainRatio = 0;
    UINT32 curGlobalGainRatio = 0;    
    UINT32 LocalMax = IO32ReadFldAlign(SW_AUTO_CONV_07, AUTO_CONV_LOCAL_MAX);
    UINT32 AdapGlobalGainTh1 = IO32ReadFldAlign(SW_AUTO_CONV_0A, ADAP_GBL_GAIN_TH_1);
    UINT32 AdapGlobalGainTh2 = IO32ReadFldAlign(SW_AUTO_CONV_0A, ADAP_GBL_GAIN_TH_2);
    UINT32 AdapGlobalGainTh3 = IO32ReadFldAlign(SW_AUTO_CONV_0A, ADAP_GBL_GAIN_TH_3);
    UINT32 AdapGlobalGainRatio1 = IO32ReadFldAlign(SW_AUTO_CONV_0B, ADAP_GBL_GAIN_RATIO_1);
    UINT32 AdapGlobalGainRatio2 = IO32ReadFldAlign(SW_AUTO_CONV_0B, ADAP_GBL_GAIN_RATIO_2);
    UINT32 AdapGlobalGainRatio3 = IO32ReadFldAlign(SW_AUTO_CONV_0B, ADAP_GBL_GAIN_RATIO_3);

    if(LocalMax >= AdapGlobalGainTh3)
        curGlobalGainRatio = AdapGlobalGainRatio3;
    else if(LocalMax >= AdapGlobalGainTh2)
        curGlobalGainRatio = AdapGlobalGainRatio2;
    else if(LocalMax >= AdapGlobalGainTh1)
        curGlobalGainRatio = AdapGlobalGainRatio1;
    else
        curGlobalGainRatio = 0x80;

    GlobalGainRatio = (3*GlobalGainRatio+curGlobalGainRatio+2)/4;
    vIO32WriteFldAlign(SW_AUTO_CONV_0B, GlobalGainRatio, ADAP_GBL_GAIN_RATIO);      
}

void vDrvTddcDepthBlend(INT32 *render_depth_buf_L, INT32 *final_depth_buf_L,INT32 *occ_map_L,
						INT32 *render_depth_buf_R, INT32 *final_depth_buf_R,INT32 *occ_map_R)
{
    INT32 x, y, addr;
	INT32 total_shift_L=0, total_shift_R=0;
	INT32 local_gain, global_gain;
	INT32 total_cnt_L=0, total_cnt_R=0;
    INT32 fg_plan = IO32ReadFldAlign(SW_TDDC_REG_07, TDDC_ZDV_PLAN);
    INT32 bg_weight = IO32ReadFldAlign(SW_TDDC_REG_07, TDDC_BG_WEIGHT);
    INT32 fg_weight = IO32ReadFldAlign(SW_TDDC_REG_07, TDDC_FG_WEIGHT);    
	INT32 check_L, check_R;
	INT32 all_occ_en = 0;
    INT32 total_shift_H, temp;
    INT32 i4NewDV, i4Value, i4Protrude;
    UINT32 PxlGain = IO32ReadFldAlign(SW_AUTO_CONV_06, AUTO_CONV_PXL_GAIN);

    g_u1AutoConvEnable = (UINT8)IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_ENABLE);

	for (y=0; y<depth_height; y++)
	{
		check_L=0;
		check_R=0;
        
		for (x=0; x<depth_width; x++)
		{
			addr = x+y*depth_width;
            
			if(check_L==1||all_occ_en==1)
			{
				if(final_depth_buf_L[addr]<=(fg_plan-128))
				{
					total_cnt_L+=fg_weight;
					total_shift_L+=final_depth_buf_L[addr]*fg_weight;
				}
				else
				{
					total_cnt_L+=bg_weight;
					total_shift_L+=final_depth_buf_L[addr]*bg_weight;
				}
			}
			else
			{
				if(occ_map_L[addr]==255)
					check_L=1;
			}

            if(check_R==1||all_occ_en==1)
			{
				if(final_depth_buf_R[addr]>=-(fg_plan-128))
				{
					total_cnt_R+=fg_weight;
					total_shift_R+=final_depth_buf_R[addr]*fg_weight;
				}
				else
				{
					total_cnt_R+=bg_weight;
					total_shift_R+=final_depth_buf_R[addr]*bg_weight;
				}
			}
			else
			{
				if(occ_map_R[addr]==255)
					check_R=1;
			}
		}
	}

    total_shift_H = (total_cnt_L && total_cnt_R) ? (total_shift_L/total_cnt_L-total_shift_R/total_cnt_R)/2 : 0;    
    final_shift_H = (final_shift_H*3+total_shift_H+2)/4;

    global_gain = (INT32)IO32ReadFldAlign(SW_TDDC_REG_02, TDDC_GLOBAL_GAIN_UI)-0x20;

    AutoConversion(final_depth_buf_L);

    if(IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_ADAP_GAIN_ENABLE) == SV_TRUE)
        AdaptiveGlobalGain();  

	if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_MANUAL_GAIN))
	{
		local_gain = (INT32)IO32ReadFldAlign(SW_TDDC_REG_02, TDDC_LOCAL_GAIN_UI)-0x20;
	}
	else
	{   
        local_gain = MIN(dp_smth_local_gain, fb_smth_local_gain);
        local_gain = (local_gain*((INT32)IO32ReadFldAlign(SW_TDDC_REG_02, TDDC_LOCAL_GAIN_UI)-0x20))/32;
	}       

    temp = (g_u1AutoConvEnable) ? (AutoC_Out_value*PxlGain) : 0;
    i4Protrude = (LVDS_DISP_3D == LVDS_DISP_3D_POLARIZED_RLRL) ? (0x400-g_u4ProtrudeFromScreen) : g_u4ProtrudeFromScreen;
    
    if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_CTRL_ORI))
    {        
        for (addr=0; addr<depth_width*depth_height; addr++)
        { 
            render_depth_buf_L[addr] = ClipInt(((-final_depth_buf_L[addr]*local_gain)/64)+128) + temp;
            render_depth_buf_R[addr] = ClipInt(((-final_depth_buf_R[addr]*local_gain)/64)+128) - temp;
        }
    }
    else
    {   
        for (addr=0; addr<depth_width*depth_height; addr++)
        {
            i4Value = ClipInt(-(final_depth_buf_L[addr]-final_shift_H)*(local_gain)/64-final_shift_H*global_gain/64+128+(i4Protrude-0x200)/4);
            i4NewDV = final_depth_buf_L[addr]-2*(i4Value-128);
            if (i4NewDV < -2*g_i4dLimMax)   // L-eye image safety
            {
                i4Value = ClipInt(final_depth_buf_L[addr]/2 + g_i4dLimMax + 128);
            }
            else if (i4NewDV > -2*g_i4dLimMin)
            {
                i4Value = ClipInt(final_depth_buf_L[addr]/2 + g_i4dLimMin + 128);
            }
            render_depth_buf_L[addr] = i4Value + temp;

            i4Value = ClipInt(-(final_depth_buf_R[addr]+final_shift_H)*(local_gain)/64+final_shift_H*global_gain/64+128+(0x200-i4Protrude)/4);
            i4NewDV = final_depth_buf_R[addr]-2*(i4Value-128);
            if (i4NewDV > 2*g_i4dLimMax)   // R-eye image safety
            {
                i4Value = ClipInt(final_depth_buf_R[addr]/2 - g_i4dLimMax + 128);
            }
            else if (i4NewDV < 2*g_i4dLimMin)
            {
                i4Value = ClipInt(final_depth_buf_R[addr]/2 - g_i4dLimMin + 128);
            }
            render_depth_buf_R[addr] = i4Value - temp;
        }
    }
    
    // Update status
    vIO32WriteFldMulti(SW_TDDC_REG_04, P_Fld(fb_smth_local_gain, TDDC_OC_FB_GAIN) 
                                    | P_Fld(dp_smth_local_gain, TDDC_DP_FB_GAIN)
                                    | P_Fld(local_gain+0x20, TDDC_LOCAL_GAIN));    
    vIO32WriteFldMulti(SW_TDDC_REG_05, P_Fld(occ_count_L, TDDC_OCC_CNT_L) 
                                    | P_Fld(occ_count_R, TDDC_OCC_CNT_R));         
    vIO32WriteFldMulti(SW_TDDC_REG_08, P_Fld(total_shift_H, TOTAL_SHIFT_H) 
                                    | P_Fld(final_shift_H, FINAL_SHIFT_H));     
}

void vDrvTddcFwTiggerHw(UINT8 u1TrigNum, UINT32 u4flag)
{     
	vIO32WriteFldAlign(TDDC_00, u1TrigNum, C_TDDC_TRIG_NUM); 
	vIO32WriteFldAlign(TDDC_00, 0, C_TDDC_FW_TRIG);          
	vIO32WriteFldAlign(TDDC_00, 1, C_TDDC_FW_TRIG); 
	vIO32WriteFldAlign(TDDC_00, 0, C_TDDC_FW_TRIG); 
    u1IsrFlag = u4flag;                
}

void vDrvTtdSetWriteAddr(UINT32 u4DsoAddrL, UINT32 u4DsoAddrR, UINT32 u4CidAddrL, UINT32 u4CidAddrR)
{
    vIO32WriteFldAlign(TTDDM_01, TDDC_PHY(u4DsoAddrL)>>4, TTD_DSO_STR_ADR_L);   
    vIO32WriteFldAlign(TTDDM_02, TDDC_PHY(u4DsoAddrR)>>4, TTD_DSO_STR_ADR_R);  
    vIO32WriteFldAlign(TTDDM_03, TDDC_PHY(u4CidAddrL)>>4, TTD_CID_STR_ADR_L);    
    vIO32WriteFldAlign(TTDDM_04, TDDC_PHY(u4CidAddrR)>>4, TTD_CID_STR_ADR_R);
}

void vDrvTtdSetReadAddr(UINT32 u4DvAddr, UINT32 u4CidAddrL, UINT32 u4CidAddrR)
{
    vIO32WriteFldAlign(TTDDM_07, TDDC_PHY(u4DvAddr)>>4, TTD_MV_RD_STR_ADR);   
    vIO32WriteFldAlign(TTDDM_08, TDDC_PHY(u4CidAddrL)>>4, TTD_CID_RD_STR_ADR_L); 
    vIO32WriteFldAlign(TTDDM_09, TDDC_PHY(u4CidAddrR)>>4, TTD_CID_RD_STR_ADR_R);         
}

void vDrvTddcSetAddr(UINT32 u4DsoAddrL, UINT32 u4DsoAddrR, UINT32 u4DvAddr)
{
    vIO32WriteFldAlign(TDDC_05, TDDC_PHY(u4DsoAddrL)>>4, C_TDDC_SA_STR_ADR_L);   
    vIO32WriteFldAlign(TDDC_06, TDDC_PHY(u4DsoAddrR)>>4, C_TDDC_SA_STR_ADR_R);      
    vIO32WriteFldAlign(TDDC_07, TDDC_PHY(u4DvAddr)>>4, C_TDDC_MV_RD_STR_ADR);
    vIO32WriteFldAlign(TDDC_03, TDDC_PHY(u4DvAddr)>>4, C_TDDC_MV_WT_STR_ADR);
}

void vDrvTddcFsDetDump(void)
{
    INT32 i;
    UINT32 u4Size, u4Width, u4Height, u4Num;
    UINT32 u4DbgIndex = IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DEBUG_INDEX);
    UINT8 *u1Data_0, *u1Data_1, *u1Data_2, *u1Data_3;    
    UINT8 *pbDsoDataL0, *pbDsoDataR0, *pbDsoDataL1, *pbDsoDataR1;
    
    FBM_POOL_T* prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_PQ_TOOL, NULL);     
    FBM_POOL_T* prPool2 = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_TOTAL2, NULL);

    if (u4BufIndex)
    {
        return;
    }

    if ((u4DbgIndex == 3) || (u4DbgIndex == 4))
    {
        vIO32WriteFldAlign(TTDDM_02, SV_OFF, TTD_DRAM_WT_DSO_EN);
    }

    u4Num = 3;
    u4Width = depth_width;
    u4Height = depth_height;
    u4Size = u4Width*u4Height;
    
    vIO32Write4B(SWRW_01, prPool->u4Addr);
    vIO32Write4B(SWRW_02, prPool->u4Size);
    
    if (prPool->u4Addr >= prPool2->u4Addr)    
    {
        u1Data_0 = (UINT8*)BSP_MapReservedMem((void *)prPool->u4Addr, prPool->u4Size);
    }
    else
    {
        u1Data_0 = (UINT8*)VIRTUAL(prPool->u4Addr);
    }

    if ((u4DbgIndex == 1) || (u4DbgIndex == 2))
    {        
        u4Num = 3;
        u4Width = depth_width;
        u4Height = depth_height;
        u4Size = u4Width*u4Height;
        
        u1Data_1 = u1Data_0+u4Size;
        u1Data_2 = u1Data_1+u4Size;        

        if (u4DbgIndex == 1)
        {
            for (i=0; i < u4Size; i++)
            {
                u1Data_0[i] = DE_buf_L[i].x;            
                u1Data_1[i] = DE_buf_L_INV[i].x;
                u1Data_2[i] = DE_buf_L_PRE[i].x;
            }
        }
        else
        {            
            for (i=0; i < u4Size; i++)
            {
                u1Data_0[i] = 0;            
                u1Data_1[i] = 128;
                u1Data_2[i] = 255;
            }
        }        
    }
    else if (u4DbgIndex == 3)
    {       
        u4Num = 4;
        u4Width = 256;
        u4Height = 135;        
        u4Size = u4Width*u4Height;        
       
        pbDsoDataL0 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrL[0]);    
        pbDsoDataR0 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrR[0]);  
        pbDsoDataL1 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrL[1]);    
        pbDsoDataR1 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrR[1]);  
        
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDsoDataL0, u4Size);
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDsoDataR0, u4Size);        
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDsoDataL1, u4Size);
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDsoDataR1, u4Size);
        
        u1Data_1 = u1Data_0+u4Size;
        u1Data_2 = u1Data_1+u4Size;
        u1Data_3 = u1Data_2+u4Size;
        
        for (i=0; i < u4Size; i++)
        {
            u1Data_0[i] = pbDsoDataL0[i];            
            u1Data_1[i] = pbDsoDataR0[i];             
            u1Data_2[i] = pbDsoDataL1[i];            
            u1Data_3[i] = pbDsoDataR1[i];   
        }               
    }        
    else if (u4DbgIndex == 4)
    {       
        u4Num = 2;
        u4Width = 256;
        u4Height = 135;        
        u4Size = u4Width*u4Height;        
       
        pbDsoDataL0 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrL[0]);    
        pbDsoDataR0 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrR[0]);  
        pbDsoDataL1 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrL[1]);    
        pbDsoDataR1 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrR[1]);  
        
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDsoDataL0, u4Size);
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDsoDataR0, u4Size);        
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDsoDataL1, u4Size);
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDsoDataR1, u4Size);
        
        u1Data_1 = u1Data_0+u4Size;
        
        for (i=0; i < u4Size; i++)
        {
            u1Data_0[i] = 128+(pbDsoDataL1[i]-pbDsoDataL0[i]);            
            u1Data_1[i] = 128+(pbDsoDataR1[i]-pbDsoDataR0[i]);             
        }               
    }    
        
    if (u4DbgIndex)    
    {
        vIO32WriteFldAlign(SW_TDDC_REG_01, 0, TDDC_DEBUG_INDEX);
        vIO32WriteFldAlign(SWRW_03, u4Height*u4Num, DUMP_CTRL_HEIGHT);
        vIO32WriteFldAlign(SWRW_03, u4Width, DUMP_CTRL_WIDTH);
        vIO32WriteFldAlign(SWRW_03, DUMP_FMT_Y_8BIT, DUMP_CTRL_FORMAT);    
        vIO32WriteFldAlign(SWRW_04, u4Size*u4Num, DUMP_CTRL_SIZE); 
    
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)u1Data_0, u4Size*u4Num);
    }
}

void vDrvTddcFsDetect(void)
{
    INT32 x, y;
    INT32 mv_x, mv_x_d, mv_x_d2;
    INT32 mv_x_diff01, mv_x_diff12, mv_x_diff02;
    UINT32 x_cnt[3] = {0, 0, 0};
    
    UINT32 fsnavi_fs_thr = IO32ReadFldAlign(SW_TDDC_REG_09, FSNAVI_FS_THR);
    UINT32 fsnavi_fs_gain = IO32ReadFldAlign(SW_TDDC_REG_09, FSNAVI_FS_GAIN);
    UINT32 fsnavi_2d_thr = IO32ReadFldAlign(SW_TDDC_REG_09, FSNAVI_2D_THR);
    UINT32 fsnavi_fs_fcr_thr = depth_width*depth_height*IO32ReadFldAlign(SW_TDDC_REG_09, FSNAVI_FS_FCR_THR)/1024;
    UINT32 fsnavi_2d_fcr_hi_thr = depth_width*depth_height*IO32ReadFldAlign(SW_TDDC_REG_0A, FSNAVI_2D_FCR_HI_THR)/1024;
    UINT32 fsnavi_2d_fcr_lo_thr = depth_width*depth_height*IO32ReadFldAlign(SW_TDDC_REG_0A, FSNAVI_2D_FCR_LO_THR)/1024;
    UINT32 fsnavi_2d_fcr_slp_st = depth_width*depth_height*IO32ReadFldAlign(SW_TDDC_REG_0A, FSNAVI_2D_FCR_SLP_ST)/1024;
    UINT32 fsnavi_2d_fcr_slp = IO32ReadFldAlign(SW_TDDC_REG_0A, FSNAVI_2D_FCR_SLP);

    vStatusLog("Fs Detect", 3);
    
    for (y=0; y < depth_height; y++)
    {
        for (x=0; x < depth_width; x++) 
        {   
            mv_x = DE_buf_L[y*depth_width+x].x;            
            mv_x_d = DE_buf_L_INV[y*depth_width+x].x;
            mv_x_d2 = DE_buf_L_PRE[y*depth_width+x].x;

            mv_x_diff01 = mv_x - mv_x_d;
            mv_x_diff12 = mv_x_d - mv_x_d2;
            mv_x_diff02 = mv_x - mv_x_d2;

            if (((mv_x_d != 0) && (mv_x != 0)) && ((mv_x_diff12*mv_x_diff01) < 0) 
                && (ABS(mv_x_diff12) > fsnavi_fs_thr) && (ABS(mv_x_diff01) > fsnavi_fs_thr) 
                && (ABS(mv_x_diff02) <= (((ABS(mv_x_diff01)+ABS(mv_x_diff12))*fsnavi_fs_gain+8)>>4)))
                x_cnt[0]++;
            else if ((mv_x_d != 0 && (mv_x != 0)) && ((mv_x_diff12*mv_x_diff01) >= 0) 
                && (ABS(mv_x_diff12) < fsnavi_2d_thr) && (ABS(mv_x_diff01) < fsnavi_2d_thr))
                x_cnt[1]++;
            else
                x_cnt[2]++;
        }
    }
    
    vTimeLog2("FsDetect     time");

    if (x_cnt[0] > fsnavi_fs_fcr_thr) 
    {
        FsNaviCur = TD_NAVI_DETECT_FS;
    }
    else if ((x_cnt[1] > fsnavi_2d_fcr_hi_thr) 
        || ((x_cnt[0] <= fsnavi_2d_fcr_slp_st) && (x_cnt[1] > fsnavi_2d_fcr_lo_thr))
        || ((x_cnt[0] > fsnavi_2d_fcr_slp_st) && (x_cnt[1] > fsnavi_2d_fcr_lo_thr+(x_cnt[0]-fsnavi_2d_fcr_slp_st)*fsnavi_2d_fcr_slp))) 
    {            
        FsNaviCur = TD_NAVI_DETECT_2D;
    }
    else 
    {            
        FsNaviCur = TD_NAVI_DETECT_UNKNOWN;
    }   

    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_DET_LOG))
    {        
        LOG(4, "FS Det: %d       %d %d %d\n", FsNaviCur, x_cnt[0], x_cnt[1], x_cnt[2]);
    }
    
    u1IsrFlag = 0;
}

void vDrvTddcFsNavi(void)
{          
    UINT32 u4FsNavi2d = (DvCount*IO32ReadFldAlign(SW_TDDC_REG_0B, FSNAVI_2D_RATIO))>>8;
    UINT32 u4FsNavi3d = (DvCount*IO32ReadFldAlign(SW_TDDC_REG_0B, FSNAVI_3D_RATIO))>>8;
	UINT32 occ_count_AVG = (occ_count_L_2+occ_count_R_2)/2;

    if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_SUPPORT_FS_DET) == 0)
    {
        return;
    }
        
    if (occ_count_AVG > u4FsNavi2d) 
        FsNaviCur = TD_NAVI_DETECT_2D; 
    else if (occ_count_AVG < u4FsNavi3d)
        FsNaviCur = TD_NAVI_DETECT_FS; 
    else
        FsNaviCur = TD_NAVI_DETECT_UNKNOWN;
   
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_DET_LOG))
    {        
        LOG(3, "Occ %d  3d   %d   2d   %d\n", occ_count_AVG, u4FsNavi3d, u4FsNavi2d);
    }
}

void vDrvTddcFsNaviReslut(void)
{
    TD_NAVI_DETECT eResult;
    UINT32 u4MaxCnt = IO32ReadFldAlign(SW_TDDC_REG_0E, FSNAVI_TH_MAX);
    UINT32 u4ActCnt = IO32ReadFldAlign(SW_TDDC_REG_0E, FSNAVI_TH_ACT);
    UINT32 u4IncCnt = IO32ReadFldAlign(SW_TDDC_REG_0E, FSNAVI_INC_STEP);
    UINT32 u4DecCnt = IO32ReadFldAlign(SW_TDDC_REG_0E, FSNAVI_DEC_STEP);

    if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_SUPPORT_FS_DET) == 0)
    {
        return;
    }

    // 2D mode detect FS source
    if (IO32ReadFldAlign(TTD_02, MJC_PRE_EN) == 1)  
    {
        if (FsNaviCur == TD_NAVI_DETECT_FS)
        {
            FsNaviCnt = (FsNaviCnt < u4MaxCnt) ? (FsNaviCnt+u4IncCnt) : u4MaxCnt;
        }
        else if (FsNaviCur == TD_NAVI_DETECT_2D) 
        {
            FsNaviCnt = (FsNaviCnt >= u4DecCnt) ? (FsNaviCnt-u4DecCnt) : 0;
            FsNaviCnt = (FsNaviCnt < u4ActCnt) ? 0 : FsNaviCnt;
        }

        eResult = (FsNaviCnt > u4ActCnt) ? TD_NAVI_DETECT_FS : TD_NAVI_DETECT_2D;
    }
    else    // 3d mode detect 2d source        
    {
        if (FsNaviCur == TD_NAVI_DETECT_2D)
        {
            FsNaviCnt = (FsNaviCnt < u4MaxCnt) ? (FsNaviCnt+u4IncCnt) : u4MaxCnt;
        }
        else 
        {
            FsNaviCnt = (FsNaviCnt >= u4DecCnt) ? (FsNaviCnt-u4DecCnt) : 0;
            FsNaviCnt = (FsNaviCnt < u4ActCnt) ? 0 : FsNaviCnt;
        }

        eResult = (FsNaviCnt > u4ActCnt) ? TD_NAVI_DETECT_2D : TD_NAVI_DETECT_FS;
    }        
         
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_DET_LOG))
    {        
        LOG(2, "FS Det: Cur %d  Result  %d => %d / %d\n", FsNaviCur, eResult, FsNaviCnt, u4ActCnt);
    }

    // update FS detection status
    vIO32WriteFldMulti(SW_TDDC_REG_0B, P_Fld(FsNaviCur, FSNAVI_CURRENT)
                                    | P_Fld(eResult, FSNAVI_RESULT));    
}

void vDrvTddcSetPicSize(UINT16 u2Width, UINT16 u2Height)
{   
    static UINT16 u2TddcW = 0, u2TddcH = 0;
    UINT16 u2DvWidth, u2DvHeight;

    u2DvWidth = u2Width;
    u2DvHeight = u2Height;
    
    if (IO32ReadFldAlign(TTD_02, SRC_3D_TYPE) == SRC_3D_TYPE_LI)
    {
        u2DvHeight /= 2;    
    }
    
    if (u2DvHeight <= 540) 
    {
        vIO32WriteFldAlign(TTD_04, 1, TTD_DS4_V_MODE);
        u2DvHeight *= 2;
    }
    else
    {
        vIO32WriteFldAlign(TTD_04, 0, TTD_DS4_V_MODE);
    }
    
    if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_SUPPORT_DS4_H) && (u2DvWidth <= 960))
    {        
        vIO32WriteFldAlign(TTD_04, 1, TTD_DS4_H_MODE);
        u2DvWidth *= 2;        
    }
    else
    {
        vIO32WriteFldAlign(TTD_04, 0, TTD_DS4_H_MODE);
    }

    if ((u2TddcW != u2Width) || (u2TddcH != u2Height))
    {
        u2TddcW = u2Width;
        u2TddcH = u2Height;
        
        depth_width = ((u2DvWidth+TDDC_DS-1)/TDDC_DS+TDDC_MB_SIZE-1)/TDDC_MB_SIZE;
        depth_height = ((u2DvHeight+TDDC_DS-1)/TDDC_DS+TDDC_MB_SIZE-1)/TDDC_MB_SIZE;    
        
        DvCount = depth_width*depth_height;

        if (DvCount <= TDDC_DV_MAX)
        {            
            vIO32WriteFldMulti(TDDC_00, P_Fld(u2Width, C_TDDC_IMG_WIDTH) | P_Fld(u2Height, C_TDDC_IMG_HEIGHT));
            vIO32WriteFldMulti(TDDCDS_11, P_Fld(0, C_LPBOX_UP) | P_Fld(u2Height-1, C_LPBOX_DN));         
            LOG(1, "Depth : Width  %d  Height  %d  Total  %d \n", depth_width, depth_height, DvCount);
        }
    }
}

void vDrvTddcInitHw(void)
{
    vIO32WriteFldAlign(CKGEN_REG_TDDC_CKCFG, 1, FLD_TDDC_CK_SEL); // 0:xtal_d1_ck 1:syspll_d2_ck
    
    // Dram bug, Need ECO
    vIO32WriteFldAlign(TDDC_01, 0xFF, C_TDDC_SA_ACCESS_INTERVAL); 
    vIO32WriteFldAlign(TTDWRAP_LARB0, 0xFF, C_LARB_TDDC_DMTIMARBEN); 
    
    vIO32WriteFldAlign(TDDC_02, 0x0E, C_TDDC_SR_V);    
    vIO32WriteFldAlign(TDDCDS_00, 0x06, C_ORD_CORING);    
    vIO32WriteFldAlign(TTDDM_08, 1, MV_HW_MODE);
    vIO32WriteFldAlign(TTDVP_02, 1, CID_TDDC_EN); 

	vIO32Write4B(TDDCDS_01, 0x80060480);
}

void vDrvTddcInitFw(void)
{
    vIO32WriteFldMulti(SW_TDDC_REG_00, P_Fld(0, TDDC_TRIGGER_LOOP) 
                                    | P_Fld(0xFFFF, TDDC_MV_INK_INDEX));    

    if (BSP_GetIcVersion() > IC_VER_5399_AA)
    {
        vIO32WriteFldAlign(SW_TDDC_REG_00, 1, TDDC_ECO_IC);
    }
    
    vIO32WriteFldMulti(SW_TDDC_REG_01, P_Fld(0, TDDC_DEBUG_INDEX)                                    
                                    | P_Fld(0, TDDC_LBOX_DET));

    #ifdef CC_FPGA      
    vIO32WriteFldAlign(SW_TDDC_REG_00, 1, TDDC_FW_IN_ISR);
    vIO32WriteFldMulti(SW_TDDC_REG_01, P_Fld(0, TDDC_STILL_DET)
                                    | P_Fld(0, TDDC_TRIGGER_CHK));
    #else
    vIO32WriteFldAlign(SW_TDDC_REG_00, 0, TDDC_FW_IN_ISR);
    vIO32WriteFldMulti(SW_TDDC_REG_01, P_Fld(1, TDDC_STILL_DET)
                                    | P_Fld(1, TDDC_TRIGGER_CHK));    
    #endif
    
    vIO32WriteFldMulti(SW_TDDC_REG_02, P_Fld(0x0, TDDC_LOCAL_GAIN_UI) 
                                    | P_Fld(0x0, TDDC_GLOBAL_GAIN_UI)
                                    | P_Fld(0x40, TDDC_DP_THLD));     

    vIO32WriteFldMulti(SW_TDDC_REG_03,P_Fld(0x300, TDDC_OCC_THR_LOW)
                                    | P_Fld(0x500, TDDC_OCC_THR_RANGE));     

	vIO32WriteFldMulti(SW_TDDC_REG_06,P_Fld(6, TDDC_OCC_DV_ERROR)
	                                | P_Fld(2, TDDC_OCC_ADA_RANGE)
									| P_Fld(0xE, TDDC_OCC_LR_ERROR));	
	
	vIO32WriteFldMulti(SW_TDDC_REG_07,P_Fld(10, TDDC_FG_WEIGHT)
									| P_Fld(1, TDDC_BG_WEIGHT)
									| P_Fld(128, TDDC_ZDV_PLAN));

	vIO32WriteFldMulti(SW_TDDC_REG_09, P_Fld(8, FSNAVI_FS_THR)
									| P_Fld(1, FSNAVI_FS_GAIN)
									| P_Fld(8, FSNAVI_2D_THR)
									| P_Fld(200, FSNAVI_FS_FCR_THR));
    
	vIO32WriteFldMulti(SW_TDDC_REG_0A, P_Fld(150, FSNAVI_2D_FCR_HI_THR)
									| P_Fld(50, FSNAVI_2D_FCR_LO_THR)
									| P_Fld(10, FSNAVI_2D_FCR_SLP_ST)
									| P_Fld(85, FSNAVI_2D_FCR_SLP));
    
	vIO32WriteFldMulti(SW_TDDC_REG_0B, P_Fld(0x24, FSNAVI_3D_RATIO)
									| P_Fld(0x38, FSNAVI_2D_RATIO));
    
	vIO32WriteFldAlign(SW_TDDC_REG_0D, 0x10, TDDC_WAIT_CNT);
    
	vIO32WriteFldMulti(SW_TDDC_REG_0E, P_Fld(0x40, FSNAVI_TH_ACT)
									| P_Fld(0x68, FSNAVI_TH_MAX)
									| P_Fld(0x1, FSNAVI_INC_STEP)
									| P_Fld(0x1, FSNAVI_DEC_STEP));   
}

void vDrvTddcGetData(void)
{
    UINT32 x, y;    
    UINT32 u4MvDramWidth = ((((depth_width*2*2)+63)>>6)<<6);    
    UINT32 u4CidDramWidth = (((depth_width+63)>>6)<<6);
    UINT8* pbMvData = (UINT8*)TDDC_VIR(gTddcCfg.u4MvAddrOut[0]);
    UINT8* pbCidDataL = (UINT8*)TDDC_VIR(gTddcCfg.u4CidAddrL[CUR_FRM]);
    UINT8* pbCidDataR = (UINT8*)TDDC_VIR(gTddcCfg.u4CidAddrR[CUR_FRM]);
    DVEC *stMvL, *stMvR; 
    
    vTimeLog2("ISR1 L00P    time");
    vStatusLog("Get Data", 1);

    #ifndef FPGA_INDIRECT_ACCESS 
    HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbMvData, u4MvDramWidth*depth_height);    
    #else    
    vDrvGetFpgaData(pGetMvData, TDDC_VIR(gTddcCfg.u4MvAddrOut[0]), u4MvDramWidth*depth_height);
    pbMvData = (UINT8*)pGetMvData;
    #endif

    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DC_CAL) == SV_TRUE) 
    {        
        #ifndef FPGA_INDIRECT_ACCESS 
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbCidDataL, u4CidDramWidth*depth_height);
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbCidDataR, u4CidDramWidth*depth_height);
        #else
        vDrvGetFpgaData(pGetCidDataL, TDDC_VIR(gTddcCfg.u4CidAddrL[CUR_FRM]), u4CidDramWidth*depth_height);
        vDrvGetFpgaData(pGetCidDataR, TDDC_VIR(gTddcCfg.u4CidAddrR[CUR_FRM]), u4CidDramWidth*depth_height);
        pbCidDataL = (UINT8*)pGetCidDataL;
        pbCidDataR = (UINT8*)pGetCidDataR;    
        #endif
        
        for (y=0; y < depth_height; y++)
        {
            for (x=0; x < depth_width; x++)
            {             
                m_pDownY32_L_ISR[x+y*depth_width] = pbCidDataL[u4CidDramWidth*y+x];
                m_pDownY32_R_ISR[x+y*depth_width] = pbCidDataR[u4CidDramWidth*y+x];
                
                stMvL = (DVEC*)(pbMvData+y*u4MvDramWidth
                    +(x/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+(x%TDDC_MV_PACK)*TDDC_MV_SIZE);
                stMvR = (DVEC*)(pbMvData+y*u4MvDramWidth
                    +(x/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+TDDC_MV_PACK_SIZE+(x%TDDC_MV_PACK)*TDDC_MV_SIZE);
                DE_buf_L_ISR[x+y*depth_width] = *stMvL;
                DE_buf_R_ISR[x+y*depth_width] = *stMvR;
            }
        }       

        vTimeLog2("GetData      Time");

        if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_IN_ISR) == SV_TRUE)      
        {
            vDrvTddcProc();
        }
    
        u1IsrFlag = 0;
    }    
    
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_DET) == SV_TRUE)          
    {
        x_memcpy(DE_buf_L_PRE, DE_buf_L, sizeof(DVEC)*depth_width*depth_height);
        
        for (y=0; y < depth_height; y++)
        {
            for (x=0; x < depth_width; x++)
            {                            
                stMvL = (DVEC*)(pbMvData+y*u4MvDramWidth
                    +(x/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+(x%TDDC_MV_PACK)*TDDC_MV_SIZE);
                DE_buf_L[x+y*depth_width] = *stMvL;
            }
        }               
           
        // Reverse DV detection      
        vDrvTddcSetAddr(gTddcCfg.u4DsoAddrR[CUR_FRM], gTddcCfg.u4DsoAddrL[NEXT_FRM], gTddcCfg.u4MvAddrOut[1]);
        vDrvTddcFwTiggerHw(1, 2);
    }  
}

void vDrvTddcGetDataInv(void)
{
    UINT32 x, y;    
    UINT32 u4MvDramWidth = ((((depth_width*2*2)+63)>>6)<<6);    
    UINT8* pbMvData = (UINT8*)TDDC_VIR(gTddcCfg.u4MvAddrOut[1]);    
    DVEC *stMvL;      
    
    vTimeLog2("ISR2 L00P    time ");
    vStatusLog("Get Data Inv", 2);

    HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbMvData, u4MvDramWidth*depth_height);    
    
    for (y=0; y < depth_height; y++)
    {
        for (x=0; x < depth_width; x++)
        {             
            stMvL = (DVEC*)(pbMvData+y*u4MvDramWidth
                +(x/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+(x%TDDC_MV_PACK)*TDDC_MV_SIZE);
            DE_buf_L_INV[x+y*depth_width] = *stMvL;
        }
    }    
    
    vTimeLog2("GetDataInv   time ");
}

void vDrvTddcISR(UINT16 u2Vector)
{
    UINT8 u1StaInt;
    ASSERT(u2Vector == VECTOR_TDDC);
    
    u1StaInt = IO32ReadFldAlign(TTDWRAP_00, STA_TTD_INT);
   
    if (u1StaInt == 0x2) 
    {       
        if (u1TddcInitFlag == SV_TRUE) 
        {
            if (u1IsrFlag == 1)
            {                
                vDrvTddcGetData();                
            }
            else if (u1IsrFlag == 2)
            {                
                vDrvTddcGetDataInv();
                vDrvTddcFsDetect();
            }
        }
    }

    if ((u1StaInt) && (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_NOT_CLEAN_ISR) == 0))
    {
        vIO32WriteFldAlign(TTDWRAP_00, u1StaInt, C_TTD_INT_CLEAR);
        vIO32WriteFldAlign(TTDWRAP_00, 0x0, C_TTD_INT_CLEAR);
    }
}

void vDrvTddcInitISR(void)
{
    x_os_isr_fct    pfnOldIsr;
    
    vIO32WriteFldAlign(TTDWRAP_00, 0, C_TTD_INT_MASK);    

    if (x_reg_isr(VECTOR_TDDC, vDrvTddcISR, &pfnOldIsr) != OSR_OK)
    {
        LOG(0, "Error: Fail to register TDDC ISR!\n");
        return;
    }

    LOG(0, "TDDC ISR registered!\n");
}


void vDrvTddcResume(void)
{
    FBM_POOL_T *prPool;

    vDrvTddcInitHw();

    prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_PQ_3DC, NULL);
    
    vIO32WriteFldAlign(TTDDM_09, (prPool->u4Addr>>31), TTD_DRAM_ADDR_MSB);
    vIO32WriteFldAlign(TTDWRAP_LARB0, (prPool->u4Addr>>28)&0x7, C_TDDC_ADDR_MSB);
    vIO32WriteFldAlign(TTDDM_01, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, TTD_DSO_STR_ADR_L);    // TTD Write predown L dram address
    vIO32WriteFldAlign(TTDDM_02, TDDC_PHY(gTddcCfg.u4DsoAddrR[0])>>4, TTD_DSO_STR_ADR_R);    // TTD Write predown R dram address
    vIO32WriteFldAlign(TTDDM_03, TDDC_PHY(gTddcCfg.u4CidAddrL[0])>>4, TTD_CID_STR_ADR_L);    // TTD Write CID L dram address   
    vIO32WriteFldAlign(TTDDM_04, TDDC_PHY(gTddcCfg.u4CidAddrR[0])>>4, TTD_CID_STR_ADR_R);    // TTD Write CID R dram address    
    vIO32WriteFldAlign(TTDDM_08, TDDC_PHY(gTddcCfg.u4CidAddrL[0])>>4, TTD_CID_RD_STR_ADR_L); // TTD Read CID L dram address  
    vIO32WriteFldAlign(TTDDM_09, TDDC_PHY(gTddcCfg.u4CidAddrR[0])>>4, TTD_CID_RD_STR_ADR_R); // TTD Read CID R dram address         
    vIO32WriteFldAlign(TTDDM_05, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, TTD_DRAM_ADR_LOW);     // TTD dram Low boundary          
    vIO32WriteFldAlign(TTDDM_06, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, TTD_DRAM_ADR_HIGH);   // TTD dram High boundary 
    vIO32WriteFldAlign(TTDDM_07, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, TTD_MV_RD_STR_ADR);   // TTD Read mv dram address 
    vIO32WriteFldMulti(TTDDM_01, P_Fld(0, CID_FRM_WT_NUM) | P_Fld(0, DSO_FRM_WT_NUM));
    
    vIO32WriteFldAlign(TDDC_03, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, C_TDDC_MV_WT_STR_ADR); // TDDC Write mv output address
    vIO32WriteFldAlign(TDDC_04, TDDC_PHY(gTddcCfg.u4MvAddrIn[0])>>4, C_TDDC_MV_END_ADR);     // TDDC Write mv boundary   
    vIO32WriteFldAlign(TDDC_05, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, C_TDDC_SA_STR_ADR_L);   // TDDC Read predown L dram address
    vIO32WriteFldAlign(TDDC_06, TDDC_PHY(gTddcCfg.u4DsoAddrR[0])>>4, C_TDDC_SA_STR_ADR_R);   // TDDC Read predown R dram address    
    vIO32WriteFldAlign(TDDC_07, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, C_TDDC_MV_RD_STR_ADR); // TDDC Read mv address for Tempral candidate
}

void vDrvTddcInit(UINT32 img_width, UINT32 img_height)
{    
    INT32 i;        
    FBM_POOL_T *prPool, *prPool2;

    #ifndef FPGA_INDIRECT_ACCESS
    prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_PQ_3DC, NULL);
    prPool2 = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_TOTAL2, NULL);
    #else
    prPool->u4Size = TDDC_DRAM_TOTAL_SIZE;
    prPool->u4Addr = TDDC_FPGA_DRAM_BASE;    
    prPool2->u4Addr = TDDC_FPGA_DRAM_BASE;
    #endif

    if (u1TddcInitFlag != SV_FALSE)
    {        
        LOG(0, "TDDC already init\n");
        return;
    }
    
    vDrvTddcInitHw();
    vDrvTddcInitFw();

    depth_width = (depth_width > TDDC_IMG_WIDTH_MAX) ? TDDC_IMG_WIDTH_MAX : depth_width;
    depth_height = (depth_height > TDDC_IMG_HEIGHT_MAX) ? TDDC_IMG_HEIGHT_MAX : depth_height;   

    depth_width = ((img_width+TDDC_DS-1)/TDDC_DS+TDDC_MB_SIZE-1)/TDDC_MB_SIZE;
    depth_height = ((img_height+TDDC_DS-1)/TDDC_DS+TDDC_MB_SIZE-1)/TDDC_MB_SIZE;    
    
    if (prPool->u4Size < TDDC_DRAM_TOTAL_SIZE)        
    {
        LOG(0, "TDDC init failed => No memory\n");
        return;
    }    
    else
    {           
        // Physical Dram Address
        gTddcCfg.u4PhyAddrBase = (prPool->u4Addr & 0x0FFFFFFF);   
        gTddcCfg.u4DsoAddrL[0] = 0;     
        gTddcCfg.u4DsoAddrR[0] = gTddcCfg.u4DsoAddrL[0] + TDDC_DS_DRAM_SIZE;        
        gTddcCfg.u4DsoAddrL[1] = gTddcCfg.u4DsoAddrR[0] + TDDC_DS_DRAM_SIZE;   
        gTddcCfg.u4DsoAddrR[1] = gTddcCfg.u4DsoAddrL[1] + TDDC_DS_DRAM_SIZE;        
        gTddcCfg.u4CidAddrL[0] = gTddcCfg.u4DsoAddrR[1] + TDDC_DS_DRAM_SIZE;
        gTddcCfg.u4CidAddrL[1] = gTddcCfg.u4CidAddrL[0] + TDDC_CID_DRAM_SIZE;
        gTddcCfg.u4CidAddrR[0] = gTddcCfg.u4CidAddrL[1] + TDDC_CID_DRAM_SIZE;        
        gTddcCfg.u4CidAddrR[1] = gTddcCfg.u4CidAddrR[0] + TDDC_CID_DRAM_SIZE;
        gTddcCfg.u4MvAddrOut[0] = gTddcCfg.u4CidAddrR[1] + TDDC_CID_DRAM_SIZE;        
        gTddcCfg.u4MvAddrOut[1] = gTddcCfg.u4MvAddrOut[0] + TDDC_MV_DRAM_SIZE;
        gTddcCfg.u4MvAddrIn[0] = gTddcCfg.u4MvAddrOut[1] + TDDC_MV_DRAM_SIZE;
        gTddcCfg.u4MvAddrIn[1] = gTddcCfg.u4MvAddrIn[0] + TDDC_MV_DRAM_SIZE;
        
        #ifndef FPGA_INDIRECT_ACCESS
        // Virtual Dram Address        
        if (prPool->u4Addr >= prPool2->u4Addr)    
        {
            gTddcCfg.u4VirAddrBase = (UINT32)BSP_MapReservedMem((void *)prPool->u4Addr, prPool->u4Size);
        }
        else
        {
            gTddcCfg.u4VirAddrBase = VIRTUAL(prPool->u4Addr);
        }        
        #else    
        gTddcCfg.u4VirAddrBase = gTddcCfg.u4PhyAddrBase;        
        #endif        
        
        vIO32WriteFldAlign(TTDDM_09, (prPool->u4Addr>>31), TTD_DRAM_ADDR_MSB);
        vIO32WriteFldAlign(TTDWRAP_LARB0, (prPool->u4Addr>>28)&0x7, C_TDDC_ADDR_MSB);
        vIO32WriteFldAlign(TTDDM_01, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, TTD_DSO_STR_ADR_L);    // TTD Write predown L dram address
        vIO32WriteFldAlign(TTDDM_02, TDDC_PHY(gTddcCfg.u4DsoAddrR[0])>>4, TTD_DSO_STR_ADR_R);    // TTD Write predown R dram address
        vIO32WriteFldAlign(TTDDM_03, TDDC_PHY(gTddcCfg.u4CidAddrL[0])>>4, TTD_CID_STR_ADR_L);    // TTD Write CID L dram address   
        vIO32WriteFldAlign(TTDDM_04, TDDC_PHY(gTddcCfg.u4CidAddrR[0])>>4, TTD_CID_STR_ADR_R); 	 // TTD Write CID R dram address	
        vIO32WriteFldAlign(TTDDM_08, TDDC_PHY(gTddcCfg.u4CidAddrL[0])>>4, TTD_CID_RD_STR_ADR_L); // TTD Read CID L dram address  
        vIO32WriteFldAlign(TTDDM_09, TDDC_PHY(gTddcCfg.u4CidAddrR[0])>>4, TTD_CID_RD_STR_ADR_R); // TTD Read CID R dram address   		
		vIO32WriteFldAlign(TTDDM_05, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, TTD_DRAM_ADR_LOW);     // TTD dram Low boundary          
        vIO32WriteFldAlign(TTDDM_06, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, TTD_DRAM_ADR_HIGH);   // TTD dram High boundary 
        vIO32WriteFldAlign(TTDDM_07, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, TTD_MV_RD_STR_ADR);   // TTD Read mv dram address 
        vIO32WriteFldMulti(TTDDM_01, P_Fld(0, CID_FRM_WT_NUM) | P_Fld(0, DSO_FRM_WT_NUM));

        vIO32WriteFldAlign(TDDC_03, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, C_TDDC_MV_WT_STR_ADR); // TDDC Write mv output address
        vIO32WriteFldAlign(TDDC_04, TDDC_PHY(gTddcCfg.u4MvAddrIn[0])>>4, C_TDDC_MV_END_ADR);     // TDDC Write mv boundary   
        vIO32WriteFldAlign(TDDC_05, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, C_TDDC_SA_STR_ADR_L);   // TDDC Read predown L dram address
        vIO32WriteFldAlign(TDDC_06, TDDC_PHY(gTddcCfg.u4DsoAddrR[0])>>4, C_TDDC_SA_STR_ADR_R);   // TDDC Read predown R dram address    
        vIO32WriteFldAlign(TDDC_07, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, C_TDDC_MV_RD_STR_ADR); // TDDC Read mv address for Tempral candidate

        // Print TDDC Debug LOG
        vIO32WriteFldAlign(SW_TDDC_REG_01, 1, TDDC_DEBUG_LOG); 

        LOG(0, "TDDC Init OK size < 0x%x\n", prPool->u4Size);
    }
    
	check_map = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);    
	complete_map = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);
    m_pDownY32_L = (PEL*)x_mem_alloc(depth_width*depth_height); 
    m_pDownY32_R = (PEL*)x_mem_alloc(depth_width*depth_height);		
    m_pDownY32_L_ISR = (PEL*)x_mem_alloc(depth_width*depth_height); 
    m_pDownY32_R_ISR = (PEL*)x_mem_alloc(depth_width*depth_height);	
	DE_buf_L = (DVEC*)x_mem_alloc(sizeof(DVEC)*depth_width*depth_height);	
	DE_buf_R = (DVEC*)x_mem_alloc(sizeof(DVEC)*depth_width*depth_height);	    
	DE_buf_L_ISR = (DVEC*)x_mem_alloc(sizeof(DVEC)*depth_width*depth_height);	
	DE_buf_R_ISR = (DVEC*)x_mem_alloc(sizeof(DVEC)*depth_width*depth_height);	
	DE_buf_L_PRE = (DVEC*)x_mem_alloc(sizeof(DVEC)*depth_width*depth_height);	
    DE_buf_L_INV = (DVEC*)x_mem_alloc(sizeof(DVEC)*depth_width*depth_height);	
	final_depth_buf_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	    
	final_depth_buf_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	    
	render_depth_buf_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	    
	render_depth_buf_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	    
	occ_map_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	occ_map_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	non_occ_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	non_occ_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	occ_diff_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	occ_diff_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	hole_buf_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	hole_buf_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	
	osd_sm_buf_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	osd_sm_buf_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	

	osd_map_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	osd_map_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	
	
    #ifdef FPGA_INDIRECT_ACCESS    
    pGetMvData = (UINT32*)x_mem_alloc(TDDC_MV_DRAM_SIZE);                
    pSetMvData = (UINT32*)x_mem_alloc(TDDC_MV_DRAM_SIZE);     
    pGetCidDataL = (UINT32*)x_mem_alloc(TDDC_CID_DRAM_SIZE); 
    pGetCidDataR = (UINT32*)x_mem_alloc(TDDC_CID_DRAM_SIZE);                
    #endif

    if ((m_pDownY32_L == NULL) || (m_pDownY32_R == NULL) || 
        (m_pDownY32_L_ISR == NULL) || (m_pDownY32_R_ISR == NULL) || 
        (DE_buf_L == NULL) || (DE_buf_R == NULL) ||        
        (DE_buf_L_ISR == NULL) || (DE_buf_R_ISR == NULL) ||
        (DE_buf_L_PRE == NULL) || (DE_buf_L_INV == NULL) ||
        (final_depth_buf_L == NULL) || (final_depth_buf_R == NULL) || 
        (render_depth_buf_L == NULL) || (render_depth_buf_R == NULL) ||
        (occ_map_L == NULL) || (occ_map_R == NULL) || 
        (non_occ_L == NULL) || (non_occ_R == NULL) || 
        (occ_diff_L == NULL) || (occ_diff_R == NULL) || 
        (hole_buf_L == NULL) || (hole_buf_R == NULL) || 
        (osd_sm_buf_L == NULL) || (osd_sm_buf_R == NULL) || 
        #ifdef FPGA_INDIRECT_ACCESS            
        (pGetMvData == NULL) || (pSetMvData == NULL) || 
        (pGetCidDataL == NULL) || (pGetCidDataR == NULL) || 
        #endif    
        (check_map == NULL) || (complete_map == NULL))
    {
        LOG(0, "TDDC Alloc memory fail !!!\n");
        return;
    }
    else
    {        
        LOG(0, "TDDC Alloc memory OK !!!\n");
    }
    
	for (i=0; i<depth_width*depth_height; i++)
	{
		DE_buf_L[i].x = (i%512)-256;//-512;
        DE_buf_L[i].y = (i%48)-24;//-32;
		DE_buf_R[i].x = (i%512)-256;//-512;
        DE_buf_R[i].y = (i%48)-24;//-32;        

		DE_buf_L_PRE[i].x = (i%512)-256;//-512;
        DE_buf_L_PRE[i].y = (i%48)-24;//-32;
		DE_buf_L_INV[i].x = (i%512)-256;//-512;
        DE_buf_L_INV[i].y = (i%48)-24;//-32;        
        
		DE_buf_L_ISR[i].x = (i%512)-256;//-512;
        DE_buf_L_ISR[i].y = (i%48)-24;//-32;
		DE_buf_R_ISR[i].x = (i%512)-256;//-512;
        DE_buf_R_ISR[i].y = (i%48)-24;//-32;        
	}
    
    x_memset(m_pDownY32_L, 0, sizeof(PEL)*depth_width*depth_height);
    x_memset(m_pDownY32_R, 0, sizeof(PEL)*depth_width*depth_height);    
    x_memset(m_pDownY32_L_ISR, 0, sizeof(PEL)*depth_width*depth_height);
    x_memset(m_pDownY32_R_ISR, 0, sizeof(PEL)*depth_width*depth_height);
    x_memset(final_depth_buf_L,	0, sizeof(INT32)*depth_width*depth_height);
    x_memset(final_depth_buf_R,	0, sizeof(INT32)*depth_width*depth_height);
    x_memset(render_depth_buf_L, 128, sizeof(INT32)*depth_width*depth_height);
    x_memset(render_depth_buf_R, 128, sizeof(INT32)*depth_width*depth_height);
    x_memset(occ_map_L, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(occ_map_R, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(non_occ_L, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(non_occ_R, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(occ_diff_L, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(occ_diff_R, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(hole_buf_L, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(hole_buf_R, 0, sizeof(INT32)*depth_width*depth_height);

    vDrvTddcInitISR();

    u1TddcInitFlag = SV_TRUE;
}


void vDrvTddcDebugInk(void)
{
    UINT32 x, y, ink;    
    UINT32 u4DebugIndex = IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DEBUG_INDEX);    
    UINT32 u4DebugW = depth_width;
    UINT32 u4DebugH = depth_height;

	ink = IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DV_INK);
      
    if (u4DebugIndex)
    {               
        if (u4DebugIndex == 1)
        {                       
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = m_pDownY32_L[x+y*depth_width];
                    render_depth_buf_R[x+y*u4DebugW] = (255-m_pDownY32_R[x+y*depth_width]);
                }
            }
        }
        else if (u4DebugIndex == 2)
        {                      
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = DE_buf_L[x+y*depth_width].blk*8+128;
                    render_depth_buf_R[x+y*u4DebugW] = -(DE_buf_R[x+y*depth_width].blk*8)+128;
                }
            }
        }        
        else if (u4DebugIndex == 3)
        {                      
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = DE_buf_L[x+y*depth_width].x+128;
                    render_depth_buf_R[x+y*u4DebugW] = DE_buf_R[x+y*depth_width].x+128;
                }
            }
        }
        else if (u4DebugIndex == 4)  
        {         
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = hole_buf_L[x+y*depth_width]+128;
                    render_depth_buf_R[x+y*u4DebugW] = hole_buf_R[x+y*depth_width]+128;
                }
            }      
        }
        else if (u4DebugIndex == 5)  
        {               
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = (occ_map_L[x+y*depth_width]==255)?128:(255-ink);
                    render_depth_buf_R[x+y*u4DebugW] = (occ_map_R[x+y*depth_width]==255)?128:ink;
                }
            }
        }
        else if (u4DebugIndex == 6)  
        {              
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = non_occ_L[x+y*depth_width]+128;
                    render_depth_buf_R[x+y*u4DebugW] = non_occ_R[x+y*depth_width]+128;
                }
            }
        }    
        else if (u4DebugIndex == 7)  
        {              
            ink = IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DV_INK);
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = ink;
                    render_depth_buf_R[x+y*u4DebugW] = ink;
                }
            }
        }
        else if (u4DebugIndex == 8)
        {
            x = IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_MV_INK_INDEX);            
            render_depth_buf_L[x] = ink;
            render_depth_buf_R[x] = ink;
        }		
        else if (u4DebugIndex == 9)  
        {              
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = osd_map_L[x+y*depth_width];
                    render_depth_buf_R[x+y*u4DebugW] = 255-osd_map_R[x+y*depth_width];
                }
            }
        }    
    }        
}

void vDrvTddcSetData(void)
{
    UINT32 x, y;    
    UINT32 u4Width = depth_width;
    UINT32 u4Height = depth_height;
    UINT32 u4DramWidth = (((u4Width*2*2)+63)>>6)<<6;    
    UINT8* pbDramData = (UINT8*)TDDC_VIR(gTddcCfg.u4MvAddrIn[CUR_FRM]);
    DVEC *stMvL, *stMvR;

    //LOG(2, "Ttd W %d  Tddc W %d H %d \n", u4Width, u4TddcWidth, u4Height);

    #ifdef FPGA_INDIRECT_ACCESS 
    pbDramData = (UINT8*)pSetMvData;
    #endif
    
    if (u1TddcInitFlag == SV_TRUE)
    {               
        vDrvTddcDebugInk();        

        for (y=0; y < u4Height; y++)
        {
            for (x=0; x < u4Width; x++)
            {                
                stMvL = (DVEC*)(pbDramData+y*u4DramWidth+(x/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+(x%TDDC_MV_PACK)*TDDC_MV_SIZE);
                stMvR = (DVEC*)(pbDramData+y*u4DramWidth+(x/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+TDDC_MV_PACK_SIZE+(x%TDDC_MV_PACK)*TDDC_MV_SIZE);
                stMvL->x = render_depth_buf_L[x+y*u4Width]-128;
                stMvL->y = 0;                
                stMvL->blk = DE_buf_L[x+y*u4Width].blk;
                stMvR->x = render_depth_buf_R[x+y*u4Width]-128;
                stMvR->y = 0;                
                stMvR->blk = DE_buf_R[x+y*u4Width].blk;
            }
    	}		
        
        #ifndef FPGA_INDIRECT_ACCESS 
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDramData, TDDC_MV_DRAM_SIZE);
        #else
        vDrvSetFpgaData(pSetMvData, TDDC_VIR(gTddcCfg.u4MvAddrIn[CUR_FRM]), TDDC_MV_DRAM_SIZE);
        #endif
        
        // Update TTD read MV & CID read address        
        if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_ENABLE))
        {
            vDrvTtdSetReadAddr(gTddcCfg.u4MvAddrIn[CUR_FRM], gTddcCfg.u4CidAddrL[CUR_FRM], gTddcCfg.u4CidAddrR[CUR_FRM]);
        }
    }        
}

void vDrvTddcDsoPatch(void)
{    
    UINT32 x, y;
    UINT32 u4TtdW = IO32ReadFldAlign(TTD_02, TTD_IMG_WIDTH);     
    UINT32 u4TddcW = IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_WIDTH)/TDDC_DS;    
    UINT32 u4TddcH = IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_HEIGHT)/TDDC_DS;     
    UINT32 u4TtdDramW, u4TddcDramW;    
    UINT8 *pbDSoDataL_0, *pbDSoDataR_0, *pbDSoDataL_1, *pbDSoDataR_1;   

    // Config TDDC DSO input address
    if (u4TtdW < (TDDC_DV_WIDTH_LIMIT/2))
    {                          
        u4TtdW = IO32ReadFldAlign(TTD_04, TTD_DS4_H_MODE) ? (u4TtdW/4) : (u4TtdW/8);
        u4TddcDramW = ((u4TddcW+63)>>6)<<6;
        u4TtdDramW = ((u4TtdW+63)>>6)<<6;
        
        pbDSoDataL_0 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrL[CUR_FRM]);    
        pbDSoDataR_0 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrR[CUR_FRM]);
        pbDSoDataL_1 = (UINT8*)x_mem_alloc(u4TddcDramW*u4TddcH);
        pbDSoDataR_1 = (UINT8*)x_mem_alloc(u4TddcDramW*u4TddcH);
        
        if ((pbDSoDataL_1 == NULL) || (pbDSoDataR_1 == NULL))
        {   
            if (pbDSoDataL_1)   x_mem_free(pbDSoDataL_1);
            if (pbDSoDataR_1)   x_mem_free(pbDSoDataR_1);
                
            LOG(0, "Alloc dram size %d failed  \n", u4TddcDramW*u4TddcH);
            return;
        }
        
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDSoDataL_0, u4TtdDramW*u4TddcH);            
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDSoDataR_0, u4TtdDramW*u4TddcH);  

        LOG(2, "Ttd W %d Dram %d  Tddc W %d Dram %d \n", u4TtdW, u4TtdDramW, u4TddcW, u4TddcDramW);
        
        for (y=0; y < u4TddcH; y++)
        {
            for (x=0; x < u4TtdW; x++)
            {             
                pbDSoDataL_1[y*u4TddcDramW+x] = pbDSoDataL_0[u4TtdDramW*y+x];
                pbDSoDataR_1[y*u4TddcDramW+x] = pbDSoDataR_0[u4TtdDramW*y+x];
            }
            // black data 
            for (x=u4TtdW; x < u4TddcW; x++)
            {             
                pbDSoDataL_1[y*u4TddcDramW+x] = 0;
                pbDSoDataR_1[y*u4TddcDramW+x] = 0;
            }
        }      

        x_memcpy(pbDSoDataL_0, pbDSoDataL_1, u4TddcDramW*u4TddcH);
        x_memcpy(pbDSoDataR_0, pbDSoDataR_1, u4TddcDramW*u4TddcH);

        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDSoDataL_0, u4TddcDramW*u4TddcH);            
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDSoDataR_0, u4TddcDramW*u4TddcH); 

        x_mem_free(pbDSoDataL_1);
        x_mem_free(pbDSoDataR_1);
    }
}

void vDrvTddcHwReset(void)
{    
    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, 1, MJC_TDDC_RST);    
    vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, 0, MJC_TDDC_RST);
}

void vDrvTddcFwTriggerSet(void)
{     
    static UINT32 u4BusyCnt = 0;
    UINT8 u1StateCur = IO32ReadFldAlign(TDDC_STATUS1, RD_TDDC_CUR_STATE);  
    
    LOG(6, "TDDC State %d Cal %d Mv %d SA %d\n",
        IO32ReadFldAlign(TDDC_STATUS1, RD_TDDC_CUR_STATE),
        IO32ReadFldAlign(TDDC_STATUS1, RD_TDDC_CAL_READY),
        IO32ReadFldAlign(TDDC_STATUS1, RD_MV_READY),
        IO32ReadFldAlign(TDDC_STATUS1, RD_SA_READY));     

    if (u1StateCur != 0)
    {    
        u4BusyCnt++;
        
        if ((IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_TRIGGER_CHK) == SV_TRUE)
			&& (u4BusyCnt > IO32ReadFldAlign(SW_TDDC_REG_0D, TDDC_WAIT_CNT))) 
        {            
            vDrvTddcHwReset();
            LOG(1, "TDDC state stop %d !! Reset \n", u1StateCur);
			return;
        }           
        
        LOG(4, "TDDC not ready %d => Cnt %d \n", u1StateCur, u4BusyCnt);
        return;
    }

    // DSO patch when width is small
    //vDrvTddcDsoPatch();

    // Switch to next frame and clear state
	u4BufIndex = u4BufIndex ? 0 : 1;
	u4BusyCnt = 0; 

    // TTD write next frame
    vDrvTtdSetWriteAddr(gTddcCfg.u4DsoAddrL[NEXT_FRM], gTddcCfg.u4DsoAddrR[NEXT_FRM], 
                    gTddcCfg.u4CidAddrL[NEXT_FRM], gTddcCfg.u4CidAddrR[NEXT_FRM]);

    // TDDC use current frame to do DV estimation   
    vDrvTddcSetAddr(gTddcCfg.u4DsoAddrL[CUR_FRM], gTddcCfg.u4DsoAddrR[CUR_FRM], gTddcCfg.u4MvAddrOut[0]);
    
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DC_CAL) == SV_TRUE)
    {
        vDrvTddcFwTiggerHw(2, 1);
    }
    else if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_DET) == SV_TRUE)
    {
        vDrvTddcFsDetDump();    
        vDrvTddcFwTiggerHw(1, 1);
    }
}
    
void vDrvTddcFwTrigger(void)
{
    static UINT32 u4Count = 0;
    static UINT32 u4FwEnable = 0xFF;
    static UINT32 u4TriCount = 0, u4TestCount = 0;

    if (IO32ReadFldAlign(TDDC_00, C_TDDC_EN) == SV_FALSE)
    {
        return;
    }

    vStatusLog("Trigger", 0);

    if ((IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_WIDTH) < TDDC_DV_WIDTH_LIMIT) || (DvCount > TDDC_DV_MAX))
    {          
        LOG(5, "Depth : Width  %d  Height  %d  Total  %d \n", depth_width, depth_height, DvCount);
        return;    
    }

    u4TriCount = (u4TriCount < 7200) ? (u4TriCount+1) : 0;

    if (u4TriCount == 0)
    {        
        u4TestCount++;
        LOG(1, "TDDC OK count %d \n", u4TestCount);
    }
    
    u4Count = (u4Count < IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_TRIGGER_LOOP)) ? (u4Count+1) : 0; 
       
    if (u4Count != 0) 
    {        
        LOG(6, "Loop count %d / %d \n", (u4Count-1), IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_TRIGGER_LOOP));
        return;
    }

    if (u4FwEnable != IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_ENABLE))
    {                
        u4FwEnable = IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_ENABLE);
        vIO32WriteFldAlign(TDDC_00, !u4FwEnable, C_TDDC_HW_TRIG_EN);

        if (!u4FwEnable)
        {
            vDrvTddcSetAddr(gTddcCfg.u4DsoAddrL[0], gTddcCfg.u4DsoAddrR[0], gTddcCfg.u4MvAddrOut[0]);             
            vDrvTtdSetReadAddr(gTddcCfg.u4MvAddrOut[0], gTddcCfg.u4CidAddrL[0], gTddcCfg.u4CidAddrR[0]);
            vDrvTtdSetWriteAddr(gTddcCfg.u4DsoAddrL[0], gTddcCfg.u4DsoAddrR[0], 
                                gTddcCfg.u4CidAddrL[0], gTddcCfg.u4CidAddrR[0]);      
        }
    }    
   
    if (u4FwEnable == SV_TRUE)
    {           
        vDrvTddcFwTriggerSet();            
    }        
}
   
void vDrvTddcUpdateParam(void)
{
    static UINT32 u4HomoMo[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
    static UINT8 u1MoCnt = 0;
    static UINT32 u4LBoxDet = 0;
    UINT32 u4TddcWidth = IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_WIDTH);
    UINT32 u4TddcHeigh = IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_HEIGHT);
    UINT32 u4TddcUp, u4TddcDn, u4TddcLf, u4TddcRt;
    
    u4HomoMo[u1MoCnt] = u4DrvDIGetFusionMotionLevel();
    u1MoCnt = ((u1MoCnt+1)&0x3); 
    
    depth_width = ((u4TddcWidth+TDDC_DS-1)/TDDC_DS+TDDC_MB_SIZE-1)/TDDC_MB_SIZE;
    depth_height = ((u4TddcHeigh+TDDC_DS-1)/TDDC_DS+TDDC_MB_SIZE-1)/TDDC_MB_SIZE;    

    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_LBOX_DET))
    {
        u4TddcUp = (_rMJCLboxSta.u2PosUp > (u4TddcHeigh/4)) ? (u4TddcHeigh/4) : _rMJCLboxSta.u2PosUp;
        u4TddcLf = (_rMJCLboxSta.u2PosLf > (u4TddcWidth/4)) ? (u4TddcWidth/4) : _rMJCLboxSta.u2PosLf;
        u4TddcDn = (_rMJCLboxSta.u2PosDn < (u4TddcHeigh*3/4)) ? (u4TddcHeigh*3/4) : _rMJCLboxSta.u2PosDn;
        u4TddcRt = (_rMJCLboxSta.u2PosRt < (u4TddcWidth*3/4)) ? (u4TddcWidth*3/4) : _rMJCLboxSta.u2PosRt;

        vIO32WriteFldMulti(TDDCDS_11, P_Fld(u4TddcUp, C_LPBOX_UP) 
                                    | P_Fld(u4TddcDn, C_LPBOX_DN)); 
        vIO32WriteFldMulti(TDDCDS_12, P_Fld(u4TddcLf, C_LPBOX_LF) 
                                    | P_Fld(u4TddcRt, C_LPBOX_RT)); 
        u4LBoxDet = 1;
    }
    else
    {
        if (u4LBoxDet == 1)
        {            
            vIO32WriteFldMulti(TDDCDS_11, P_Fld(0, C_LPBOX_UP) 
                                        | P_Fld(u4TddcHeigh-1, C_LPBOX_DN)); 
            vIO32WriteFldMulti(TDDCDS_12, P_Fld(0, C_LPBOX_LF) 
                                        | P_Fld(u4TddcWidth-1, C_LPBOX_RT)); 

            u4LBoxDet = 0;
        }
    }

    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_STILL_DET))
    {
        if ((u4HomoMo[0] == 0) && (u4HomoMo[1] == 0) 
            && (u4HomoMo[2] == 0) && (u4HomoMo[3] == 0))
        {       
            vIO32WriteFldMulti(TDDCDS_03, P_Fld(0, C_PREV_COST) | P_Fld(8, C_RAND_COST)
                | P_Fld(3, C_TMPR_COST) | P_Fld(2, C_SPTL_COST)); 
        }
        else
        {
            vIO32WriteFldMulti(TDDCDS_03, P_Fld(1, C_PREV_COST) | P_Fld(4, C_RAND_COST)
                | P_Fld(2, C_TMPR_COST) | P_Fld(1, C_SPTL_COST)); 
        }
    }

    // copy data for firmware
    x_memcpy(m_pDownY32_L, m_pDownY32_L_ISR, depth_width*depth_height);
    x_memcpy(m_pDownY32_R, m_pDownY32_R_ISR, depth_width*depth_height);    
    x_memcpy(DE_buf_L, DE_buf_L_ISR, sizeof(DVEC)*depth_width*depth_height);
    x_memcpy(DE_buf_R, DE_buf_R_ISR, sizeof(DVEC)*depth_width*depth_height);
}

void vDrvTddcProc(void)
{          
    vSystemLog();
    
    if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_ENABLE) == SV_TRUE) 
    {                
        if (u1TddcInitFlag != SV_TRUE)
        {                
            vDrvTTDSet3DDepthCtrl(SV_OFF);
        }        
        
        if ((IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DC_CAL) == SV_TRUE) 
            && (DvCount <= TDDC_DV_MAX))
        {              
            vTimeLog("Proc Loop    time ");        
            
            vDrvTddcUpdateParam();            

            vTimeLog("Update        time ");

            vDrvTddcOccProcessNew(m_pDownY32_L, m_pDownY32_R, DE_buf_L, DE_buf_R, 
                occ_map_L, occ_map_R, non_occ_L, non_occ_R, occ_diff_L, occ_diff_R);

            vTimeLog("OccProc       time ");
            
            vDrvTddcHoleFill_L(hole_buf_L, DE_buf_L, occ_map_L, occ_diff_L);                
            vDrvTddcHoleFill_R(hole_buf_R, DE_buf_R, occ_map_R, occ_diff_R);    
            
            vTimeLog("HoleFill      time ");

            vDrvTddcOsdDetect(hole_buf_R, DE_buf_R, occ_map_R, occ_diff_R);    
            
            vTimeLog("OsdDetect     time ");
            
            vDrvTddcFallBack();
            
            vTimeLog("Fallback      time ");
            
            vDrvTddcTmprSmooth(final_depth_buf_L, final_depth_buf_R, hole_buf_L, hole_buf_R);

            vTimeLog("TmpSmooth     time ");
            
            vDrvTddcDepthBlend(render_depth_buf_L, final_depth_buf_L, 
                occ_map_L, render_depth_buf_R, final_depth_buf_R, occ_map_R); 
            
            vTimeLog("DepthBlend    time ");
            
            vDrvTddcSetData();
            
            vTimeLog("Dumpin        time ");      
            
            vDrvTddcFsNavi(); 
            
            vTimeLog("FsNavi        time ");
		}
        
        if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_SUPPORT_FS_DET) == SV_TRUE)
        {
            vDrvTddcFsNaviReslut();

            vTimeLog("NaviReslut    time ");
        }
    }    
}

void vDrvTddcSetCtrl(UINT8 u1OnOff)
{
    UINT32 u4DcCtrl, u4FsDet;    
    UINT32 u4Loop = (PANEL_GetVClkMax() < 100) ? 0 : 1;    
    
    u1IsrFlag = 0;   
    FsNaviCnt = 0;
    
    u4DcCtrl = IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_ECO_IC);
    u4FsDet = IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_SUPPORT_FS_DET) 
                && IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_ECO_IC); 
    
    LOG(2, "Tddc Set Ctrl %d\n", u1OnOff);

    if (u1OnOff == SV_OFF)
    {        
        vIO32WriteFldAlign(TTDDM_02, u4FsDet, TTD_DRAM_WT_DSO_EN);
        vIO32WriteFldAlign(TTDDM_03, SV_OFF, TTD_DRAM_WT_CID_EN);   
        vIO32WriteFldAlign(TTDDM_07, SV_OFF, TTD_DRAM_RD_MV_EN); 
        vIO32WriteFldAlign(TTDDM_08, SV_OFF, TTD_DRAM_RD_CID_EN); 
        
        vIO32WriteFldMulti(TTD_02, P_Fld(u4FsDet, MJC_PRE_EN) 
                                    | P_Fld(u4FsDet, LR_FREERUN_EN));  
        vIO32WriteFldMulti(TDDC_03, P_Fld(u4FsDet, C_TDDC_DRAM_WT_MV_EN)
                                    | P_Fld(u4FsDet, C_TDDC_DRAM_RD_MV_EN)
                                    | P_Fld(u4FsDet, C_TDDC_DRAM_RD_SA_EN));   
        vIO32WriteFldAlign(TTD_04, SV_OFF, TTD_3DC_EN);        
        
        vIO32WriteFldMulti(SW_TDDC_REG_00, P_Fld(u4FsDet, TDDC_FW_ENABLE)            
                                    | P_Fld(u4Loop ? 3 : 1, TDDC_TRIGGER_LOOP));    
        vIO32WriteFldMulti(SW_TDDC_REG_01, P_Fld(u4FsDet, TDDC_FS_DET)        
                                    | P_Fld(SV_OFF, TDDC_DC_CAL));  
        
        vIO32WriteFldAlign(TDDC_00, u4FsDet, C_TDDC_EN);          
    }
    else
    {        
        vIO32WriteFldAlign(TTDDM_02, u4DcCtrl, TTD_DRAM_WT_DSO_EN);
        vIO32WriteFldAlign(TTDDM_03, u4DcCtrl, TTD_DRAM_WT_CID_EN);  
        vIO32WriteFldAlign(TTDDM_07, u4DcCtrl, TTD_DRAM_RD_MV_EN); 
        vIO32WriteFldAlign(TTDDM_08, u4DcCtrl, TTD_DRAM_RD_CID_EN);         
        
        vIO32WriteFldMulti(TTD_02, P_Fld(SV_OFF, MJC_PRE_EN) 
                                    | P_Fld(SV_OFF, LR_FREERUN_EN));  
        vIO32WriteFldMulti(TDDC_03, P_Fld(u4DcCtrl, C_TDDC_DRAM_WT_MV_EN)
                                    | P_Fld(u4DcCtrl, C_TDDC_DRAM_RD_MV_EN)
                                    | P_Fld(u4DcCtrl, C_TDDC_DRAM_RD_SA_EN));     
        vIO32WriteFldAlign(TTD_04, SV_ON, TTD_3DC_EN);

        vIO32WriteFldMulti(SW_TDDC_REG_00, P_Fld(u4DcCtrl, TDDC_FW_ENABLE)            
                                    | P_Fld(u4Loop, TDDC_TRIGGER_LOOP));    
        vIO32WriteFldMulti(SW_TDDC_REG_01, P_Fld(SV_OFF, TDDC_FS_DET)        
                                    | P_Fld(u4DcCtrl, TDDC_DC_CAL));        
        
        vIO32WriteFldAlign(TDDC_00, u4DcCtrl, C_TDDC_EN);      
    }   
}
