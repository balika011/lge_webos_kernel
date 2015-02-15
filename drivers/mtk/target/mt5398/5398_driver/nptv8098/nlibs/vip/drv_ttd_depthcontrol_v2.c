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
#ifdef CC_MT5398
#include "hw_mjc.h"
#endif
#include "hw_sw.h"
#include "hw_ycproc.h"
#include "nptv_debug.h"
#include "drv_ttd.h"
#include "drv_video.h"
#include "drv_tdtv_drvif.h"
#include "drv_scaler_drvif.h"
#include "drv_meter.h"
#include "drv_mjc.h"
#include "fbm_drvif.h"
#include "drv_tdtv_feature.h"

#define TDDC_DS_DRAM_SIZE       0x9000  //(240*135) => (256*135) = 0x8700
#define TDDC_CID_DRAM_SIZE      0x1000  //(60*34)   => (64*34) = 0x880
#define TDDC_MV_DRAM_SIZE       0x3000  //(60*34*2)*2 => (64*34*2)*2 = 0x2200
#define TDDC_DRAM_TOTAL_SIZE    ((TDDC_DS_DRAM_SIZE*4+TDDC_CID_DRAM_SIZE+TDDC_MV_DRAM_SIZE*2)*2)
#define TDDC_MV_SIZE			(2)
#define TDDC_MV_PACK			(4)
#define TDDC_MV_PACK_SIZE		(TDDC_MV_SIZE*TDDC_MV_PACK)
#define TDDC_MV_PACK_LR_SIZE	(TDDC_MV_PACK_SIZE*2)

#define ClipInt(p)  ((p > 255) ? (255) : ((p < 0) ? 0 : p))
#define Diff(a, b)  (((a)>(b))?((a)-(b)):((b)-(a)))
#define CLIP(val, min, max) ((val>=max) ? max : ((val<=min) ? min : val)) 

#define TDDC_VIR(addr)  (gTddcCfg.u4VirAddrBase+addr)
#define TDDC_PHY(addr)  (gTddcCfg.u4PhyAddrBase+addr)
#define TDDC_VIR_TO_PHY(addr)  (addr-gTddcCfg.u4VirAddrBase+gTddcCfg.u4PhyAddrBase)

typedef struct 
{
    signed y:6;
    signed x:10;
} DVEC;

typedef struct
{
    UINT32 u4DsoAddrL[4];
    UINT32 u4DsoAddrR[4]; 
    UINT32 u4CidAddrL; 
    UINT32 u4CidAddrR; 
    UINT32 u4MvAddrIn;
    UINT32 u4MvAddrOut[2];     // 0:LR 1:RL
    UINT32 u4VirAddrBase;
    UINT32 u4PhyAddrBase;
} TDDC_DRAM_T;


#define TDDC_DS                 (8)
#define TDDC_MB_SIZE            (4)
#define TDDC_DV_WIDTH_LIMIT     (512+32)
#define TDDC_DV_HEIGHT_LIMIT    (64)

static TDDC_DRAM_T gTddcCfg;
static UINT32 u4BufIndex;
static UINT8 u1IsrFlag = 0;
static UINT8 u1TddcInitFlag = SV_FALSE;
static INT32 shift_avg_H;
static INT32 shift_avg_V;
static INT32 occ_count_L, occ_count_R;
static INT32 depth_width, depth_height;
static INT32 m_mbSize = 4;
static INT32 local_gain, global_gain;
static INT32 AutoC_Out_value = 0;
static INT32 final_shift_H=0;
static INT32 dv_max_count = 0;

static PEL *m_pDownY32_L = NULL;   	    //Dram2 : CID
static PEL *m_pDownY32_R = NULL;   	    //Dram2 : CID
static PEL *m_pDownY32_L_ISR = NULL;    //Dram2 : CID
static PEL *m_pDownY32_R_ISR = NULL;    //Dram2 : CID
static INT32 *check_map = NULL;
static DVEC * DE_buf_L = NULL; 		    //Dram3 : MV Write
static DVEC * DE_buf_R = NULL; 		    //Dram3 : MV Write
static DVEC * DE_buf_L_ISR = NULL; 		//Dram3 : MV Write
static DVEC * DE_buf_R_ISR = NULL; 		//Dram3 : MV Write
static DVEC * DE_buf_L_PRE = NULL; 		
static DVEC * DE_buf_L_INV = NULL;
static INT32 * depth_buf_L = NULL;          
static INT32 * depth_buf_R = NULL;            
static INT32 * filter_buf_L = NULL;         
static INT32 * filter_buf_R = NULL;     
static INT32 * final_depth_buf_L = NULL;     
static INT32 * final_depth_buf_R = NULL;        
static INT32 * render_depth_buf_L = NULL;   //Dram4 : MV Read
static INT32 * render_depth_buf_R = NULL;   //Dram4 : MV Read
static INT32 * occ_map_L = NULL;             
static INT32 * occ_map_R = NULL;    
static INT32 * non_occ_L = NULL;              
static INT32 * non_occ_R = NULL; 

HAL_TIME_T TimeStart, TimeNow, TimeDelta;

INT32  expf_Lut[96]={
1024,1023,1020,1016,1010,1002,992,981,969,954,939,922,904,884,864,842, 
 820, 797, 773, 749, 724, 698,673,647,621,595,569,544,518,493,469,445,
 421, 398, 375, 354, 332, 312,292,273,255,238,221,206,191,177,163,150,
 139, 127, 117, 107,  98,  89, 81, 74, 67, 61, 55, 50, 45, 41, 36, 33, 
  29,  26,  23,  21,  18,  16, 15, 13, 11, 10,  9,  8,  7,  6,  5,  5, 
   4,   3,   3,   3,   2,   2,  2,  1,  1,  1,  1,  1,  1,  1,  0,  0}; 


EXTERN void HalFlushInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
EXTERN void HalIOMMUFlushInvalidateDCacheMultipleLine(UINT32 u4VirAddr, UINT32 u4PhyAddr, UINT32 u4Size);

EXTERN UINT8 fgIsSupport120HzPanel(void);

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

static void vTimeLogStart(void)
{        
    HAL_GetTime(&TimeStart);                 
}

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
        Printf("DSO L 0x%08x 0x%08x 0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4DsoAddrL[0]), TDDC_PHY(gTddcCfg.u4DsoAddrL[1]), 
            TDDC_PHY(gTddcCfg.u4DsoAddrL[2]), TDDC_PHY(gTddcCfg.u4DsoAddrL[3]));        
        Printf("    R 0x%08x 0x%08x 0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4DsoAddrR[0]), TDDC_PHY(gTddcCfg.u4DsoAddrR[1]), 
            TDDC_PHY(gTddcCfg.u4DsoAddrR[2]), TDDC_PHY(gTddcCfg.u4DsoAddrR[3]));        
        Printf("CID   0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4CidAddrL), TDDC_PHY(gTddcCfg.u4CidAddrR));        
        Printf("MV    0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4MvAddrOut[0]), TDDC_PHY(gTddcCfg.u4MvAddrOut[1]));
        
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

static void vErrorLog(UINT8* string)
{    
    static UINT32 u4Count = 0;

    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_ERR_LOG))
    {
        u4Count++;
        Printf("%s \n", string);
    }    
    
    if (u4Count > 256)
    {
        u4Count = 0;
        vIO32WriteFldAlign(SW_TDDC_REG_01, 0, TDDC_ERR_LOG);
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
*/

/* =========================================================================================== */
static void IIR8(INT32 *cur, INT32 new_in, INT32 *final, INT32 weight)//weight8 IIR cur (8.4) new_in(8.0) final(8.0)
{
	*cur=(((8-weight)*(*cur)+(weight*new_in*16))*1);
	if(*cur>=0)	*cur=(*cur+4)/8;
	else		*cur=(*cur-4)/8;

	if(*cur>=0)	*final=(*cur+8)/16;
	else		*final=(*cur-8)/16;
}

/* =========================================================================================== 
static void IIR16(INT32 *cur, INT32 new_in, INT32 *final, INT32 weight)//weight16 IIR cur (8.5) new_in(8.0) final(8.0)
{
	*cur=(((16-weight)*(*cur)+(weight*new_in*32))*1);
	if(*cur>=0)	*cur=(*cur+8)/16;
	else		*cur=(*cur-8)/16;

	if(*cur>=0)	*final=(*cur+16)/32;
	else		*final=(*cur-16)/32;
}
*/


static void OccProcess(PEL *Cin_L, PEL *Cin_R, DVEC *level_L, DVEC *level_R, INT32 *occ_map_L, INT32 *occ_map_R,
                        INT32 *non_occ_L, INT32 *non_occ_R, INT32 shift_H, INT32 shift_V)
{
    INT32 i, x, y;
	INT32 L_dp, R_dp, L_disp, R_disp;
    INT32 diff = TDDC_DS*m_mbSize/2;
    INT32 mod = TDDC_DS*m_mbSize;
    INT32 OccDVerror = IO32ReadFldAlign(SW_TDDC_REG_06, TDDC_OCC_DV_ERROR);   
    INT32 OccLRerror = IO32ReadFldAlign(SW_TDDC_REG_06, TDDC_OCC_LR_ERROR);        
    occ_count_L=0; 
    occ_count_R=0;

    i = 0;
    
	for (y=0; y<depth_height; y++)	// LRC L map
	{
    	for (x=0; x<depth_width; x++)
        {		
    		L_dp = (level_L[i].x>=0) ? (level_L[i].x+diff) : (level_L[i].x-diff);
    		L_disp = L_dp/mod;
    		
    		if((x+L_disp)>=0 && (x+L_disp)<=depth_width-1)
    		{   //has correspondence
    			if ((ABS(level_L[i].x+level_R[i+L_disp].x) <= OccDVerror   
                    && level_R[i+L_disp].x!=-512 && level_L[i].x!=-512)
					&&ABS(Cin_L[i]-Cin_R[i+L_disp]) <= OccLRerror               
			        && y< depth_height-2)
    			{   //LR perfact match
    				occ_map_L[i] = 255;		non_occ_L[i] =  level_L[i].x;
    			}
    			else if ((ABS(level_L[i].x+level_R[i+L_disp].x) <= OccDVerror   
                    && level_R[i+L_disp].x!=-512 && level_L[i].x!=-512)
			        && y>= depth_height-2)
    			{   //LR perfact match
    				occ_map_L[i] = 255;		non_occ_L[i] =  level_L[i].x;
    			}
    			else
    			{   //unknown
    				occ_count_L++;
    				occ_map_L[i] =0;	non_occ_L[i] = -128;
    			}
    		}
    		else
    		{   //out of boundary
    			occ_map_L[i] =0;	non_occ_L[i] = -128;
    		}
            i++;
    	}
	}
    
    i = 0;
    
	for (y=0; y<depth_height; y++)	// LRC R map
	{
    	for (x=0; x<depth_width; x++)
        {		
    		R_dp = (level_R[i].x>=0) ? (level_R[i].x+diff) : (level_R[i].x-diff);
    		R_disp = R_dp/mod;

    		if ((x+R_disp)>=0 && (x+R_disp)<=depth_width-1)
    		{   //has correspondence
    			if ((ABS(level_R[i].x+level_L[i+R_disp].x) <= OccDVerror 
    				&& level_R[i].x!=-512&&level_L[i+R_disp].x!=-512)
                    && ABS(Cin_R[i]-Cin_L[i+R_disp]) <= OccLRerror 
                    && y< depth_height-2)
    			{   //LR perfact match
    				occ_map_R[i] = 255; 	non_occ_R[i] =  level_R[i].x;
    			}
    			else if ((ABS(level_R[i].x+level_L[i+R_disp].x) <= OccDVerror 
    			    && level_R[i].x!=-512&&level_L[i+R_disp].x!=-512)
                    && y>= depth_height-2)
    			{   //LR perfact match
    				occ_map_R[i] = 255; 	non_occ_R[i] =  level_R[i].x;
    			}
    			else
    			{   //unknown
    				occ_count_R++;
    				occ_map_R[i] =0;		non_occ_R[i] = -128;
    			}
    		}
    		else
    		{   //out of boundary
    		    occ_map_R[i] =0;		non_occ_R[i] = -128;
    		}
            i++;
    	}
	}
}
/* =========================================================================================== */
static void HoleFill_L(INT32 *fi_im, DVEC *level, INT32 *occ_map)
{
	INT32 width = depth_width;
	INT32 height = depth_height;
	INT32 i, x, y;
	INT32 count=0;
	INT32 sum_neighbor=0;	
	
	for (i=0; i<width*height; i++)
	{
        check_map[i] = (occ_map[i]!=255) ? 0 : 1;
	}
    
	//UL filling
	for (y=0;y<height;y++)
	{
    	for (x=0;x<width;x++)
    	{	
    		if (check_map[x+y*width]==0)
    		{
    			if (check_map[x+(y-1)*width]==1||check_map[(x-1)+y*width]==1)
    			{
    				count=0;
    				sum_neighbor=0;
                    
    				if (x-1>=0	 &&check_map[x-1+ y   *width]!=0)	
                        {   sum_neighbor+=fi_im[x-1+ y   *width];   count++;    }//L
    				if (x+1<width &&check_map[x+1+ y   *width]!=0)	
                        {   sum_neighbor+=level[x+1+ y   *width].x; count++;    }//R
    				if (y-1>=0	 &&check_map[x+  (y-1)*width]!=0)	
                        {   sum_neighbor+=fi_im[x+  (y-1)*width];   count++;    }//U
    				if (y+1<height&&check_map[x+  (y+1)*width]!=0)	
                        {   sum_neighbor+=level[x+  (y+1)*width].x; count++;    }//D
    				
    				if(count==0)	
                    {
                        check_map[x+y*width]=0;
    				}
    				else
                    {   
                        fi_im[x+y*width]=sum_neighbor/count;    
                        check_map[x+y*width]=1;
                    }
    			}
    		}
    		else
    		{
    			fi_im[x+y*width]=level[x  +y*width].x;	check_map[x+y*width]=1;
    		}	
    	}
	}
    
	//DR filling
	for (y=height-1;y>=0;y--)
	{
    	for (x=width-1;x>=0;x--)
    	{	
    		if (check_map[x+y*width]==0)
    		{
    			if (check_map[x+(y+1)*width]==1||check_map[(x+1)+y*width]==1)
    			{
    				count=0;
    				sum_neighbor=0;
                    
    				if (x-1>=0	 &&check_map[x-1+ y   *width]!=0)	
                        {   sum_neighbor+=fi_im[x-1+ y   *width];   count++;    }//L
    				if (x+1<width &&check_map[x+1+ y   *width]!=0)	
                        {   sum_neighbor+=fi_im[x+1+ y   *width];   count++;    }//R
    				if (y-1>=0	 &&check_map[x+  (y-1)*width]!=0)	
                        {   sum_neighbor+=fi_im[x+  (y-1)*width];   count++;    }//U
    				if (y+1<height&&check_map[x+  (y+1)*width]!=0)	
                        {   sum_neighbor+=fi_im[x+  (y+1)*width];   count++;    }//D
    				
    				if (count==0)	
    				{
                        check_map[x+y*width]=0;
    				}
                    else
                    {   
                        fi_im[x+y*width]=sum_neighbor/count;    
                        check_map[x+y*width]=1;
                    }
    			}
    		}
    	}
	}
}

/* =========================================================================================== */
static void HoleFill_R(INT32 *fi_im, DVEC *level, INT32 *occ_map)
{
	INT32 width  = depth_width;
	INT32 height = depth_height;	
	INT32 i, x, y;
	INT32 count=0;
	INT32 sum_neighbor=0;	
	
	for (i=0; i<width*height; i++)
	{
        check_map[i]= (occ_map[i]!=255)	? 0 : 1;
	}
    
	//UL filling
	for (y=0;y<height;y++)
	{
    	for (x=width-1;x>=0;x--)
    	{	
    		if (check_map[x+y*width]==0)
    		{
    			if (check_map[x+(y-1)*width]==1||check_map[(x+1)+y*width]==1)
    			{	
    				count=0;
    				sum_neighbor=0;
                    
    				if (x-1>=0	 &&check_map[x-1+ y   *width]!=0)	
                        {   sum_neighbor+=level[x-1+ y   *width].x; count++;    }//L
    				if (x+1<width &&check_map[x+1+ y   *width]!=0)	
                        {   sum_neighbor+=fi_im[x+1+ y   *width];   count++;    }//R
    				if (y-1>=0	 &&check_map[x+  (y-1)*width]!=0)	
                        {   sum_neighbor+=fi_im[x+  (y-1)*width];   count++;    }//U
    				if (y+1<height&&check_map[x+  (y+1)*width]!=0)	
                        {   sum_neighbor+=level[x+  (y+1)*width].x; count++;    }//D
    				
    				if (count==0)
                    {
                        check_map[x+y*width]=0;
    				}
    				else
                    {   
                        fi_im[x+y*width]=sum_neighbor/count;    
                        check_map[x+y*width]=1;
                    }
    			}
    		}
    		else
    		{
    			fi_im[x+y*width]=level[x  +y*width].x;	
                check_map[x+y*width]=1;
    		}	
    	}
	}
    
	//DR filling
	for (y=height-1;y>=0;y--)
	{
    	for (x=0;x<width;x++)
    	{	
    		if (check_map[x+y*width]==0)
    		{
    			if (check_map[x+(y+1)*width]==1||check_map[(x-1)+y*width]==1)
    			{	
    				count=0;
    				sum_neighbor=0;
                    
    				if (x-1>=0	 &&check_map[x-1+ y   *width]!=0)	
                        {   sum_neighbor+=fi_im[x-1+ y   *width];   count++;    }//L
    				if (x+1<width &&check_map[x+1+ y   *width]!=0)	
                        {   sum_neighbor+=fi_im[x+1+ y   *width];   count++;    }//R
    				if (y-1>=0	 &&check_map[x+  (y-1)*width]!=0)	
                        {   sum_neighbor+=fi_im[x+  (y-1)*width];   count++;    }//U
    				if (y+1<height&&check_map[x+  (y+1)*width]!=0)	
                        {   sum_neighbor+=fi_im[x+  (y+1)*width];   count++;    }//D
    				
    				if (count==0)	
                    {
                        check_map[x+y*width]=0;
    				}
    				else
                    {   
                        fi_im[x+y*width]=sum_neighbor/count;    
                        check_map[x+y*width]=1;
                    }
    			}
    		}
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

        AutoC_Global_Fac = final_shift_H + (INT32)(final_shift_H*global_gain/64) + (g_u1ProtrudeFromScreen-128);
        AutoC_Local_Fac1 = SearchLocalMax;// + (INT32)(SearchLocalMax*local_gain/64);
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
/* =========================================================================================== */
static void DepthBlend(INT32 *render_depth_buf_L, INT32 *final_depth_buf_L,INT32 *cur_depth_buf_L, 
						INT32 *render_depth_buf_R, INT32 *final_depth_buf_R,INT32 *cur_depth_buf_R)
{    
    INT32 i, x, y, addr;
    INT32 check_L, check_R;
    INT32 occ_avg, local_dibr_gain, total_shift_H;    
    static INT32 cur_dibr_gain=0;        
    static INT32 cur_dibr_gain_IIR=0;
    static INT32 total_shift_H_IIR=0;    
    INT32 total_cnt_L=0;
    INT32 total_cnt_R=0;
    INT32 total_shift_L=0;
    INT32 total_shift_R=0;
    INT32 OccThrLow = IO32ReadFldAlign(SW_TDDC_REG_03, TDDC_OCC_THR_LOW);     
    INT32 OccThrRange = IO32ReadFldAlign(SW_TDDC_REG_03, TDDC_OCC_THR_RANGE);
    INT32 fg_plan = IO32ReadFldAlign(SW_TDDC_REG_07, TDDC_ZDV_PLAN);
    INT32 bg_weight = IO32ReadFldAlign(SW_TDDC_REG_07, TDDC_BG_WEIGHT);
    INT32 fg_weight = IO32ReadFldAlign(SW_TDDC_REG_07, TDDC_FG_WEIGHT);    
    INT32 all_occ_en = IO32ReadFldAlign(SW_TDDC_REG_06, TDDC_OCC_ALL_GLOBAL);
    INT32 i4NewDV, i4Value;
    UINT32 PxlGain = IO32ReadFldAlign(SW_AUTO_CONV_06, AUTO_CONV_PXL_GAIN);

    g_u1AutoConvEnable = (UINT8)IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_ENABLE);

    #ifdef CC_FLIP_MIRROR_SUPPORT
    if(u1GetFlipMirrorConfig() == SYS_MIRROR_CONFIG_ON)
    {
        for (i=0; i<depth_width*depth_height; i++)
        {
            final_depth_buf_R[i] = (cur_depth_buf_L[i]+3*final_depth_buf_R[i]+2)/4;
            final_depth_buf_L[i] = (cur_depth_buf_R[i]+3*final_depth_buf_L[i]+2)/4;
        }
    }    
    else
    #endif
    {
        for (i=0; i<depth_width*depth_height; i++)
        {
            final_depth_buf_L[i] = (cur_depth_buf_L[i]+3*final_depth_buf_L[i]+2)/4;
            final_depth_buf_R[i] = (cur_depth_buf_R[i]+3*final_depth_buf_R[i]+2)/4;
        }
    }
    
	for(y=0; y<depth_height-2; y++)
	{
		check_L=0;	check_R=0;
        
		for(x=0; x<depth_width; x++)
		{
			addr=x+y*depth_width;
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
		}
        
		for(x=depth_width-1;x>=0;x--)
		{
			addr=x+y*depth_width;
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
	
	total_shift_H = (total_shift_L/total_cnt_L-total_shift_R/total_cnt_R)/2;//r2: sta_fw_shift_H

    if (IO32ReadFldAlign(SW_TDDC_REG_04, TDDC_GLOBAL_IIR_EN) == 0)
        final_shift_H = (final_shift_H*3+total_shift_H+2)/4;
    else
        IIR8(&total_shift_H_IIR, total_shift_H, &final_shift_H, 1);
    
    occ_avg = (occ_count_L+occ_count_R)/2;

    vIO32WriteFldAlign(SW_TDDC_REG_08, total_shift_H, TOTAL_SHIFT_H);         
    vIO32WriteFldAlign(SW_TDDC_REG_08, final_shift_H, FINAL_SHIFT_H);    
	vIO32WriteFldAlign(SW_TDDC_REG_05, occ_count_L, TDDC_OCC_CNT_L);
	vIO32WriteFldAlign(SW_TDDC_REG_05, occ_count_R, TDDC_OCC_CNT_R);
    
    if (occ_avg > (OccThrLow+OccThrRange))                              
        local_dibr_gain = 0;
    else if (occ_avg > OccThrLow) 
        local_dibr_gain = MAX((OccThrRange-occ_avg+OccThrLow), 0)*32/OccThrRange;  
    else                                                        
        local_dibr_gain = 32;

    if (IO32ReadFldAlign(SW_TDDC_REG_04, TDDC_NEW_IIR_EN) == 0)
        cur_dibr_gain = (3*cur_dibr_gain+local_dibr_gain+2)/4;
    else
        IIR8(&cur_dibr_gain_IIR, local_dibr_gain, &cur_dibr_gain, 1);

	vIO32WriteFldAlign(SW_TDDC_REG_04, local_dibr_gain, TDDC_FB_GAIN);
	vIO32WriteFldAlign(SW_TDDC_REG_04, cur_dibr_gain, TDDC_FB_GAIN_IIR);

    if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_MANUAL_GAIN))
    {
        local_gain = IO32ReadFldAlign(SW_TDDC_REG_02, TDDC_LOCAL_GAIN_UI)-0x20;
    }
    else
    {   // local gain fallback by OCC   
        local_gain = cur_dibr_gain*((INT32)IO32ReadFldAlign(SW_TDDC_REG_02, TDDC_LOCAL_GAIN_UI)-0x20)/32;
    }
    
    global_gain = (INT32)IO32ReadFldAlign(SW_TDDC_REG_02, TDDC_GLOBAL_GAIN_UI)-0x20;
        
    AutoConversion(final_depth_buf_L);

    if(IO32ReadFldAlign(SW_AUTO_CONV_00, AUTO_CONV_ADAP_GAIN_ENABLE) == SV_TRUE)
        AdaptiveGlobalGain();                

    for(i=0; i<depth_width*depth_height; i++)
    {
        i4Value = ClipInt(-(final_depth_buf_L[i]-final_shift_H)*(local_gain)/64-final_shift_H*global_gain/64+128+(g_u1ProtrudeFromScreen-128));
        i4NewDV = final_depth_buf_L[i]-2*(i4Value-128);
        if (i4NewDV < -2*g_i4dLimMax)   // L-eye image safety
        {
            i4Value = ClipInt(final_depth_buf_L[i]/2 + g_i4dLimMax + 128);
        }
        else if (i4NewDV > -2*g_i4dLimMin)
        {
            i4Value = ClipInt(final_depth_buf_L[i]/2 + g_i4dLimMin + 128);
        }

        #ifdef CC_FLIP_MIRROR_SUPPORT
        if(u1GetFlipMirrorConfig() == SYS_MIRROR_CONFIG_ON)
        {
            if(g_u1AutoConvEnable)        
                render_depth_buf_R[i] = i4Value - AutoC_Out_value*PxlGain;//AutoC_Final_value;            
            else
                render_depth_buf_R[i] = i4Value;
        }    
        else
        #endif
        {
            if(g_u1AutoConvEnable)        
                render_depth_buf_L[i] = i4Value - AutoC_Out_value*PxlGain;//AutoC_Final_value;            
            else
                render_depth_buf_L[i] = i4Value;
        }

        i4Value = ClipInt(-(final_depth_buf_R[i]+final_shift_H)*(local_gain)/64+final_shift_H*global_gain/64+128+(128-g_u1ProtrudeFromScreen));
        i4NewDV = final_depth_buf_R[i]-2*(i4Value-128);
        if (i4NewDV > 2*g_i4dLimMax)   // R-eye image safety
        {
            i4Value = ClipInt(final_depth_buf_R[i]/2 - g_i4dLimMax + 128);
        }
        else if (i4NewDV < 2*g_i4dLimMin)
        {
            i4Value = ClipInt(final_depth_buf_R[i]/2 - g_i4dLimMin + 128);
        }

        
        #ifdef CC_FLIP_MIRROR_SUPPORT
        if(u1GetFlipMirrorConfig() == SYS_MIRROR_CONFIG_ON)
        {
            if(g_u1AutoConvEnable)           
                render_depth_buf_L[i] = i4Value + AutoC_Out_value*PxlGain;//AutoC_Final_value;
            else
                render_depth_buf_L[i] = i4Value;
        }    
        else
        #endif
        {
            if(g_u1AutoConvEnable)           
                render_depth_buf_R[i] = i4Value + AutoC_Out_value*PxlGain;//AutoC_Final_value;
            else
                render_depth_buf_R[i] = i4Value;

        }
    }
}

/* =========================================================================================== */
static void DilterBiltInt(INT32 *Dout, PEL *Cin, INT32 *Din)
{
	INT32 width  = depth_width;
	INT32 height = depth_height;
    INT32 x, y;
	INT32 xmin, ymin, xmid, ymid, xmax, ymax;
	INT32 d0;
	INT32 weightTotal;
	INT32 weight00,weight01,weight02;
	INT32 weight10,weight11,weight12;
	INT32 weight20,weight21,weight22;

    for(y = 0; y < height; y++)
    for(x = 0; x < width; x++)
    {
		ymin = MIN(MAX(0, y-1),height-3); ymid=MIN(y,height-3);	ymax = MIN(height-3, y+1);
		xmin = MAX(0, x-1);	xmid=x;	xmax = MIN( width-1, x+1);
		d0 = 0;//reset value
		weightTotal = 0;

 		weight00 = expf_Lut[MIN(Diff(Cin[xmin+ymin*width], Cin[x+y*width]), 95)];
 		weight01 = expf_Lut[MIN(Diff(Cin[xmin+ymid*width], Cin[x+y*width]), 95)];
 		weight02 = expf_Lut[MIN(Diff(Cin[xmin+ymax*width], Cin[x+y*width]), 95)];
 		weight10 = expf_Lut[MIN(Diff(Cin[xmid+ymin*width], Cin[x+y*width]), 95)];
 		weight11 = expf_Lut[MIN(Diff(Cin[xmid+ymid*width], Cin[x+y*width]), 95)];
 		weight12 = expf_Lut[MIN(Diff(Cin[xmid+ymax*width], Cin[x+y*width]), 95)];
 		weight20 = expf_Lut[MIN(Diff(Cin[xmax+ymin*width], Cin[x+y*width]), 95)];
 		weight21 = expf_Lut[MIN(Diff(Cin[xmax+ymid*width], Cin[x+y*width]), 95)];
 		weight22 = expf_Lut[MIN(Diff(Cin[xmax+ymax*width], Cin[x+y*width]), 95)];
		d0 += (Din[xmin+ymin*width]*weight00);	weightTotal +=weight00;
		d0 += (Din[xmin+ymid*width]*weight01);	weightTotal +=weight01;
		d0 += (Din[xmin+ymax*width]*weight02);	weightTotal +=weight02;
		d0 += (Din[xmid+ymin*width]*weight10);	weightTotal +=weight10;
		d0 += (Din[xmid+ymid*width]*weight11);	weightTotal +=weight11;
		d0 += (Din[xmid+ymax*width]*weight12);	weightTotal +=weight12;
		d0 += (Din[xmax+ymin*width]*weight20);	weightTotal +=weight20;
		d0 += (Din[xmax+ymid*width]*weight21);	weightTotal +=weight21;
		d0 += (Din[xmax+ymax*width]*weight22);	weightTotal +=weight22;
		
		if(weightTotal==0)
			Dout[x+y*width] = Din[x+y*width];
		else
			Dout[x+y*width] = MIN(d0/weightTotal,255);
	}	
}

void DepthDown(INT32 *down_depth_buf_L, INT32 *cur_depth_buf_L, 
		INT32 *down_depth_buf_R, INT32 *cur_depth_buf_R, INT32 c_h_down, INT32 c_v_down)
{
	INT32 x, y;
	INT32 x1, x2, x3;
	INT32 y1, y2, y3;
    INT32 w_down, h_down;

    w_down = (depth_width+1)/2;
    h_down = (depth_height+1)/2;
        
	if((c_h_down==1)&&(c_v_down==0))
	{
		for(y=0;y<depth_height;y++)
		for( x=0;x<w_down;x++)
		{
			x1 = MAX(0, 2*x-1);	x2=2*x; x3 = MIN(depth_width-1, 2*x+1);
			//int addr=x+y*depth_width;
			down_depth_buf_L[x+y*depth_width]=(cur_depth_buf_L[x1+y*depth_width]+2*cur_depth_buf_L[x2+y*depth_width]+cur_depth_buf_L[x3+y*depth_width]+2)/4;
			down_depth_buf_R[x+y*depth_width]=(cur_depth_buf_R[x1+y*depth_width]+2*cur_depth_buf_R[x2+y*depth_width]+cur_depth_buf_R[x3+y*depth_width]+2)/4;
		}
	}
	else if((c_h_down==0)&&(c_v_down==1))
	{
		for(y=0;y<h_down;y++)
		for(x=0;x<depth_width;x++)
		{
			y1 = MAX(0, 2*y-1);	y2=2*y; y3 = MIN(depth_height-1, 2*y+1);
			//int addr=x+y*depth_width;
			down_depth_buf_L[x+y*depth_width]=(cur_depth_buf_L[x+y1*depth_width]+2*cur_depth_buf_L[x+y2*depth_width]+cur_depth_buf_L[x+y3*depth_width]+2)/4;
			down_depth_buf_R[x+y*depth_width]=(cur_depth_buf_R[x+y1*depth_width]+2*cur_depth_buf_R[x+y2*depth_width]+cur_depth_buf_R[x+y3*depth_width]+2)/4;
		}
	}
	else if((c_h_down==1)&&(c_v_down==1))
	{
		for(y=0;y<h_down;y++)
		for(x=0;x<depth_width;x++)
		{
			y1 = MAX(0, 2*y-1);	y2=2*y; y3 = MIN(depth_height-1, 2*y+1);
			//int addr=x+y*depth_width;
			down_depth_buf_L[x+y*depth_width]
			    =(cur_depth_buf_L[x+y1*depth_width]+2*cur_depth_buf_L[x+y2*depth_width]+cur_depth_buf_L[x+y3*depth_width]+2)/4;
			down_depth_buf_R[x+y*depth_width]
                =(cur_depth_buf_R[x+y1*depth_width]+2*cur_depth_buf_R[x+y2*depth_width]+cur_depth_buf_R[x+y3*depth_width]+2)/4;
		}
        
        for(y=0;y<h_down;y++)
        for(x=0;x<w_down;x++)
        {
            x1 = MAX(0, 2*x-1); x2=2*x; x3 = MIN(depth_width-1, 2*x+1);
            //int addr=x+y*depth_width;
            down_depth_buf_L[x+y*w_down]
                =(down_depth_buf_L[x1+y*depth_width]+2*down_depth_buf_L[x2+y*depth_width]+down_depth_buf_L[x3+y*depth_width]+2)/4;
            down_depth_buf_R[x+y*w_down]
                =(down_depth_buf_R[x1+y*depth_width]+2*down_depth_buf_R[x2+y*depth_width]+down_depth_buf_R[x3+y*depth_width]+2)/4;
        }
	}
}

void vDrvTddcFwTiggerHw(UINT8 u1TrigNum)
{        
    static UINT8 u1State = 0xFF;
    UINT8 u1StateCur;
    
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_TRIGGER_CHK) == SV_TRUE) 
    {
        u1StateCur = IO32ReadFldAlign(TDDC_STATUS1, RD_TDDC_CUR_STATE);    

        if ((u1StateCur == u1State) && (u1StateCur != 0))
        {            
            vIO32WriteFldAlign(TDDC_00, SV_OFF, C_TDDC_EN); 
            vIO32WriteFldAlign(TDDC_00, SV_ON, C_TDDC_EN); 
            vErrorLog("TDDC state stop ");
        }

        u1State = u1StateCur;

        if (u1StateCur != 0)
        {
            vErrorLog("TDDC HW not ready !! Wait !! ");
            return;
        }
        
    }
    
    vIO32WriteFldAlign(TDDC_00, u1TrigNum, C_TDDC_TRIG_NUM); 
    vIO32WriteFldAlign(TDDC_00, 0, C_TDDC_FW_TRIG);          
    vIO32WriteFldAlign(TDDC_00, 1, C_TDDC_FW_TRIG); 
    vIO32WriteFldAlign(TDDC_00, 0, C_TDDC_FW_TRIG); 
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
#if SUPPORT_3D_FS_DET

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
        
#endif
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

    vStatusLog("Fs Detect", 6);
    
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

    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_DET_LOG))
    {
        if (x_cnt[0] > fsnavi_fs_fcr_thr) 
        {
            LOG(2, "FS Det: FS       %d %d %d\n", x_cnt[0], x_cnt[1], x_cnt[2]);
        }
        else if ((x_cnt[1] > fsnavi_2d_fcr_hi_thr) 
            || ((x_cnt[0] <= fsnavi_2d_fcr_slp_st) && (x_cnt[1] > fsnavi_2d_fcr_lo_thr))
            || ((x_cnt[0] > fsnavi_2d_fcr_slp_st) && (x_cnt[1] > fsnavi_2d_fcr_lo_thr+(x_cnt[0]-fsnavi_2d_fcr_slp_st)*fsnavi_2d_fcr_slp))) 
        {
            LOG(2, "FS Det: 2D       %d %d %d\n", x_cnt[0], x_cnt[1], x_cnt[2]);
        }
        else 
        {
            LOG(2, "FS Det: Unknown  %d %d %d\n", x_cnt[0], x_cnt[1], x_cnt[2]);
        }   
    }
}

void vDrvTddcSetPicSize(UINT16 u2Width, UINT16 u2Height)
{   
    static UINT16 u2TddcW = 0, u2TddcH = 0;
    UINT32 u4TotalDvBlock;
    
    if (IO32ReadFldAlign(TTD_02, SRC_3D_TYPE) == SRC_3D_TYPE_LI)
    {
        u2Height /= 2;
    }
    
    if ((u2Height <= 540) 
        && (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_ENABLE)))
    {
        vIO32WriteFldAlign(TTD_04, 1, TTD_DS4_V_MODE);
        u2Height *= 2;
    }
    else
    {
        vIO32WriteFldAlign(TTD_04, 0, TTD_DS4_V_MODE);
    }
    
    if ((u2Width <= 960)
        && (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_ENABLE)))
    {        
        vIO32WriteFldAlign(TTD_04, 1, TTD_DS4_H_MODE);
        u2Width *= 2;        
    }
    else
    {
        vIO32WriteFldAlign(TTD_04, 0, TTD_DS4_H_MODE);
    }

    if (u2Width < TDDC_DV_WIDTH_LIMIT)
    {
        u2Width = TDDC_DV_WIDTH_LIMIT;
    }

    if (u2Height < TDDC_DV_HEIGHT_LIMIT)
    {
        u2Height = TDDC_DV_HEIGHT_LIMIT;
    }

    if ((u2TddcW != u2Width) || (u2TddcH != u2Height))
    {
        u2TddcW = u2Width;
        u2TddcH = u2Height;
        
        depth_width = ((u2Width+7)/8+m_mbSize-1)/m_mbSize;
        depth_height = ((u2Height+7)/8+m_mbSize-1)/m_mbSize;
        
        u4TotalDvBlock = depth_width*depth_height*2;
        u4TotalDvBlock = IO32ReadFldAlign(TTD_04, TTD_DS4_V_MODE) ? (u4TotalDvBlock>>1) : u4TotalDvBlock;    
        u4TotalDvBlock = IO32ReadFldAlign(TTD_04, TTD_DS4_H_MODE) ? (u4TotalDvBlock>>1) : u4TotalDvBlock;    
            
        vIO32WriteFldMulti(TDDC_00, P_Fld(u2Width, C_TDDC_IMG_WIDTH) | P_Fld(u2Height, C_TDDC_IMG_HEIGHT));
        vIO32WriteFldMulti(TDDCDS_11, P_Fld(0, C_LPBOX_UP) | P_Fld(u2Height-1, C_LPBOX_DN)); 

        // PR Mode DV size bigger than 2K will SRAM read failed 
        if ((BSP_GetIcVersion() == IC_VER_5398_AA) && (u4TotalDvBlock > 2048))
        {
            vIO32WriteFldAlign(TTDIR_05, 0, IR_DISPT_SCALAR);
        }
        else
        {            
            vIO32WriteFldAlign(TTDIR_05, 0x200, IR_DISPT_SCALAR);
        }
        
        LOG(3, "Depth : Width  %d  Height  %d  Total  %d \n", depth_width, depth_height, u4TotalDvBlock);
    }
}

void vDrvTddcInitFw(void)
{
    vIO32WriteFldMulti(SW_TDDC_REG_00, P_Fld(0, TDDC_TRIGGER_LOOP) 
                                    | P_Fld(1, TDDC_MV_PATCH)
                                    | P_Fld(0xFFFF, TDDC_MV_INK_INDEX));    
    
    vIO32WriteFldMulti(SW_TDDC_REG_01, P_Fld(0, TDDC_DEBUG_INDEX)
                                    | P_Fld(1, TDDC_STILL_DET)
                                    | P_Fld(0, TDDC_LBOX_DET)
                                    | P_Fld(1, TDDC_FS_SRC_AUTO)
                                    | P_Fld(1, TDDC_TRIGGER_CHK));     
    
    vIO32WriteFldMulti(SW_TDDC_REG_02, P_Fld(0x80, TDDC_LOCAL_GAIN_UI) 
                                    | P_Fld(0x80, TDDC_GLOBAL_GAIN_UI));     

    vIO32WriteFldMulti(SW_TDDC_REG_03,P_Fld(0, TDDC_OCC_THR_LOW)
                                    | P_Fld(0x140, TDDC_OCC_THR_RANGE));     

    vIO32WriteFldMulti(SW_TDDC_REG_04,P_Fld(0, TDDC_NEW_IIR_EN)
                                    | P_Fld(0, TDDC_GLOBAL_IIR_EN));

	vIO32WriteFldMulti(SW_TDDC_REG_06,P_Fld(8, TDDC_OCC_DV_ERROR)
									| P_Fld(32, TDDC_OCC_LR_ERROR)
									| P_Fld(1, TDDC_OCC_ALL_GLOBAL));	
	
	vIO32WriteFldMulti(SW_TDDC_REG_07,P_Fld(8, TDDC_FG_WEIGHT)
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
//Auto Conversion
	vIO32WriteFldMulti(SW_AUTO_CONV_00, P_Fld(150, AUTO_CONV_GLOBAL_TH_0)
									| P_Fld(50, AUTO_CONV_GLOBAL_TH_1)
									| P_Fld(60, AUTO_CONV_GLOBAL_TH_2)
									| P_Fld(1, AUTO_CONV_IIR_EN)
	    							| P_Fld(1, AUTO_CONV_GLOBAL_CURVE_MODE)
    								| P_Fld(1, AUTO_CONV_LOCAL_CURVE_MODE)
                                    | P_Fld(1, AUTO_CONV_BLACKBAR_ENABLE));  
    vIO32WriteFldMulti(SW_AUTO_CONV_01, P_Fld(120, AUTO_CONV_LOCAL_TH_0)
                                    | P_Fld(110, AUTO_CONV_LOCAL_TH_1)
                                    | P_Fld(100, AUTO_CONV_LOCAL_TH_2)
                                    | P_Fld(90, AUTO_CONV_LOCAL_TH_3));
    vIO32WriteFldMulti(SW_AUTO_CONV_02, P_Fld(64, AUTO_CONV_LOCAL_BLKTH_0)
                                    | P_Fld(96, AUTO_CONV_LOCAL_BLKTH_1)
                                    | P_Fld(128, AUTO_CONV_LOCAL_BLKTH_2)
                                    | P_Fld(30, AUTO_CONV_LOCAL_BLKTH_LN));
    vIO32WriteFldAlign(SW_AUTO_CONV_03, 0x01, AUTO_CONV_IIR_STRENTH);
    vIO32WriteFldMulti(SW_AUTO_CONV_05, P_Fld(0, AUTO_CONV_GLOBAL_PXL)
                                    | P_Fld(0, AUTO_CONV_LOCAL_PXL)
                                    | P_Fld(0, AUTO_CONV_FINAL_PXL)
                                    | P_Fld(0x80, AUTO_CONV_DEBUG_PXL));
    vIO32WriteFldMulti(SW_AUTO_CONV_06, P_Fld(2, AUTO_CONV_CHANGE_THD)
                                    | P_Fld(1, AUTO_CONV_PXL_GAIN)
                                    | P_Fld(10, AUTO_CONV_RESET_FRAME));
    vIO32WriteFldMulti(SW_AUTO_CONV_08, P_Fld(0x40, AUTO_CONV_GB_CRV_GAIN)
                                    | P_Fld(0x40, AUTO_CONV_LC_CRV_GAIN)
                                    | P_Fld(0x10, AUTO_CONV_GLOBAL_LIMIT)
                                    | P_Fld(0x10, AUTO_CONV_LOCAL_LIMIT));
    vIO32WriteFldMulti(SW_AUTO_CONV_09, P_Fld(160, AUTO_CONV_LOCAL_BLKTH_3)
                                    | P_Fld(192, AUTO_CONV_LOCAL_BLKTH_4)
                                    | P_Fld(80, AUTO_CONV_LOCAL_TH_4)
                                    | P_Fld(70, AUTO_CONV_LOCAL_TH_5));      
    vIO32WriteFldMulti(SW_AUTO_CONV_0A, P_Fld(60, ADAP_GBL_GAIN_TH_1)
                                    | P_Fld(90, ADAP_GBL_GAIN_TH_2)
                                    | P_Fld(120, ADAP_GBL_GAIN_TH_3));      
    vIO32WriteFldMulti(SW_AUTO_CONV_0B, P_Fld(0x80, ADAP_GBL_GAIN_RATIO_1)      
                                        | P_Fld(0x80, ADAP_GBL_GAIN_RATIO_2)
                                        | P_Fld(0x80, ADAP_GBL_GAIN_RATIO_3));
}

void vDrvTddcGetData(void)
{
    UINT32 x, y;    
    UINT32 u4MvDramWidth = ((((depth_width*2*2)+63)>>6)<<6);    
    UINT32 u4CidDramWidth = (((depth_width+63)>>6)<<6);
    UINT32 u4DsoAddrL, u4DsoAddrR, u4DvAddr;
    UINT8* pbMvData = (UINT8*)TDDC_VIR(gTddcCfg.u4MvAddrOut[0]);    
    UINT8* pbCidDataL = (UINT8*)TDDC_VIR(gTddcCfg.u4CidAddrL);
    UINT8* pbCidDataR = (UINT8*)TDDC_VIR(gTddcCfg.u4CidAddrR);
    DVEC *stMvL, *stMvR;    

    vStatusLog("Data    ", 4);

    HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbMvData, u4MvDramWidth*depth_height);    

    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DC_CAL) == SV_TRUE) 
    {        
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbCidDataL, u4CidDramWidth*depth_height);
        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbCidDataR, u4CidDramWidth*depth_height);

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
    }
    
    if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_DET) == SV_TRUE)          
    {
        u1IsrFlag = 1;
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
        if (fgIsSupport120HzPanel())
        {
            if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_SRC_INV) == SV_FALSE)
            {
                u4DsoAddrL = gTddcCfg.u4DsoAddrL[(u4BufIndex+1)&0x3];
                u4DsoAddrR = gTddcCfg.u4DsoAddrL[(u4BufIndex+2)&0x3];
            }
            else
            {
                u4DsoAddrL = gTddcCfg.u4DsoAddrR[(u4BufIndex+1)&0x3];
                u4DsoAddrR = gTddcCfg.u4DsoAddrR[(u4BufIndex+2)&0x3];
            }
        }
        else
        {            
            u4DsoAddrL = gTddcCfg.u4DsoAddrR[(u4BufIndex+2)&0x3];
            u4DsoAddrR = gTddcCfg.u4DsoAddrL[(u4BufIndex+3)&0x3];
        }
        
        u4DvAddr = gTddcCfg.u4MvAddrOut[1];    
        
        vDrvTddcSetAddr(u4DsoAddrL, u4DsoAddrR, u4DvAddr);
        vDrvTddcFwTiggerHw(1);
    }    
}

void vDrvTddcGetDataInv(void)
{
    UINT32 x, y;    
    UINT32 u4MvDramWidth = ((((depth_width*2*2)+63)>>6)<<6);    
    UINT8* pbMvData = (UINT8*)TDDC_VIR(gTddcCfg.u4MvAddrOut[1]);    
    DVEC *stMvL;        

    vStatusLog("DataInv ", 5);

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
}

void vDrvTddcISR(UINT16 u2Vector)
{
    UINT8 u1StaInt;
    ASSERT(u2Vector == VECTOR_TDDC);
    u1StaInt = IO32ReadFldAlign(MJC_STA_SYS_00, MJC_STA_TDDC_INT);

    vStatusLog("ISR     ", 3);
    
    if (u1StaInt == 0x1) 
    {       
        if (u1TddcInitFlag == SV_TRUE) 
        {
            if (u1IsrFlag == 0)
            {                
                vTimeLogStart();
                vDrvTddcGetData();                
                vTimeLog("ISR1       time ");
            }
            else 
            {                
                vTimeLogStart();
                vDrvTddcGetDataInv();                
                vTimeLog("ISR2       time ");
                   
                vDrvTddcFsDetect();
                vTimeLog("FS Detect  time ");
            }
        }
    }

    if ((u1StaInt) && (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_NOT_CLEAN_ISR) == 0))
    {
        vIO32WriteFldAlign(MJC_SYS_INTR_CTRL, u1StaInt, MJC_TDDC_INT_CLEAR);
        vIO32WriteFldAlign(MJC_SYS_INTR_CTRL, 0x0, MJC_TDDC_INT_CLEAR);
    }
}

void vDrvTddcInitISR(void)
{
    x_os_isr_fct    pfnOldIsr;
    
    vIO32WriteFldAlign(MJC_SYS_INTR_CTRL, 0, MJC_TDDC_INT_MASK);    

    if (x_reg_isr(VECTOR_TDDC, vDrvTddcISR, &pfnOldIsr) != OSR_OK)
    {
        LOG(0, "Error: Fail to register TDDC ISR!\n");
        return;
    }

    LOG(0, "TDDC ISR registered!\n");
}

void vDrvTddcInit(UINT32 img_width, UINT32 img_height)
{    
    INT32 i;        
    FBM_POOL_T* prPool, *prPool2;
    
    if (u1TddcInitFlag != SV_FALSE)
    {        
        LOG(0, "TDDC already init\n");
        return;
    }
    
    vDrvTddcInitFw();

    depth_width = ((img_width+7)/8+m_mbSize-1)/m_mbSize;
    depth_height = ((img_height+7)/8+m_mbSize-1)/m_mbSize;    
    dv_max_count = depth_width*depth_height;

    prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_PQ_3DC, NULL);
    prPool2 = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_TOTAL2, NULL);

    if (prPool->u4Size > TDDC_DRAM_TOTAL_SIZE)
    {   
        // Physical Dram Address
        gTddcCfg.u4PhyAddrBase = (prPool->u4Addr & 0x0FFFFFFF);   
        gTddcCfg.u4DsoAddrL[0] = 0;     
        gTddcCfg.u4DsoAddrR[0] = gTddcCfg.u4DsoAddrL[0] + TDDC_DS_DRAM_SIZE;        
        gTddcCfg.u4DsoAddrL[1] = gTddcCfg.u4DsoAddrR[0] + TDDC_DS_DRAM_SIZE;   
        gTddcCfg.u4DsoAddrR[1] = gTddcCfg.u4DsoAddrL[1] + TDDC_DS_DRAM_SIZE;        
        gTddcCfg.u4DsoAddrL[2] = gTddcCfg.u4DsoAddrR[1] + TDDC_DS_DRAM_SIZE;   
        gTddcCfg.u4DsoAddrR[2] = gTddcCfg.u4DsoAddrL[2] + TDDC_DS_DRAM_SIZE;
        gTddcCfg.u4DsoAddrL[3] = gTddcCfg.u4DsoAddrR[2] + TDDC_DS_DRAM_SIZE;   
        gTddcCfg.u4DsoAddrR[3] = gTddcCfg.u4DsoAddrL[3] + TDDC_DS_DRAM_SIZE;
        gTddcCfg.u4CidAddrL = gTddcCfg.u4DsoAddrR[3] + TDDC_DS_DRAM_SIZE;
        gTddcCfg.u4CidAddrR = gTddcCfg.u4CidAddrL + TDDC_CID_DRAM_SIZE;
        gTddcCfg.u4MvAddrOut[0] = gTddcCfg.u4CidAddrR + TDDC_CID_DRAM_SIZE;        
        gTddcCfg.u4MvAddrOut[1] = gTddcCfg.u4MvAddrOut[0] + TDDC_MV_DRAM_SIZE;
        gTddcCfg.u4MvAddrIn = gTddcCfg.u4MvAddrOut[1] + TDDC_MV_DRAM_SIZE;
        
        // Virtual Dram Address        
        if (prPool->u4Addr >= prPool2->u4Addr)    
        {
            gTddcCfg.u4VirAddrBase = (UINT32)BSP_MapReservedMem((void *)prPool->u4Addr, prPool->u4Size);
        }
        else
        {
            gTddcCfg.u4VirAddrBase = VIRTUAL(prPool->u4Addr);
        }
        
        vIO32WriteFldAlign(MJC_PR_03, (prPool->u4Addr>>28), MJC_PR_ADDR_MSB);
        vIO32WriteFldAlign(TTDDM_01, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, TTD_DSO_STR_ADR_L);    // TTD write predown L dram address
        vIO32WriteFldAlign(TTDDM_02, TDDC_PHY(gTddcCfg.u4DsoAddrR[0])>>4, TTD_DSO_STR_ADR_R);    // TTD write predown R dram address
        vIO32WriteFldAlign(TTDDM_03, TDDC_PHY(gTddcCfg.u4CidAddrL)>>4, TTD_CID_STR_ADR_L);
        vIO32WriteFldAlign(TTDDM_04, TDDC_PHY(gTddcCfg.u4CidAddrR)>>4, TTD_CID_STR_ADR_R);    
        vIO32WriteFldAlign(TTDDM_05, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, TTD_DRAM_ADR_LOW);     // TTD dram Low boundary          
        vIO32WriteFldAlign(TTDDM_06, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, TTD_DRAM_ADR_HIGH);   // TTD dram High boundary 
        vIO32WriteFldAlign(TTDDM_07, TDDC_PHY(gTddcCfg.u4MvAddrIn)>>4, TTD_MV_STR_ADR);          // TTD mv dram address 
        vIO32WriteFldMulti(TTDDM_01, P_Fld(0, CID_FRM_WT_NUM) | P_Fld(0, DSO_FRM_WT_NUM));

        vIO32WriteFldAlign(TDDC_03, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, C_TDDC_MV_WT_STR_ADR); // TDDC write mv output address
        vIO32WriteFldAlign(TDDC_04, TDDC_PHY(gTddcCfg.u4MvAddrIn)>>4, C_TDDC_MV_END_ADR);
        vIO32WriteFldAlign(TDDC_05, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, C_TDDC_SA_STR_ADR_L);   // TDDC Read predown L dram address
        vIO32WriteFldAlign(TDDC_06, TDDC_PHY(gTddcCfg.u4DsoAddrR[0])>>4, C_TDDC_SA_STR_ADR_R);   // TDDC Read predown R dram address    
        vIO32WriteFldAlign(TDDC_07, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, C_TDDC_MV_RD_STR_ADR); // TDDC Read mv address for Tempral candidate

        LOG(0, "\n======== TDDC Dram Config =========\n");
        LOG(0, "POOL  0x%08x POOL2 0x%08x \n", prPool->u4Addr, prPool2->u4Addr);   
        LOG(0, "Size  0x%08x ", prPool->u4Size);
        LOG(0, "DSO L 0x%08x 0x%08x 0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4DsoAddrL[0]), TDDC_PHY(gTddcCfg.u4DsoAddrL[1]), 
            TDDC_PHY(gTddcCfg.u4DsoAddrL[2]), TDDC_PHY(gTddcCfg.u4DsoAddrL[3]));        
        LOG(0, "    R 0x%08x 0x%08x 0x%08x 0x%08x\n", 
            TDDC_PHY(gTddcCfg.u4DsoAddrR[0]), TDDC_PHY(gTddcCfg.u4DsoAddrR[1]), 
            TDDC_PHY(gTddcCfg.u4DsoAddrR[2]), TDDC_PHY(gTddcCfg.u4DsoAddrR[3]));        
        LOG(0, "CID   0x%08x ~ 0x%08x \n", 
            TDDC_PHY(gTddcCfg.u4CidAddrL), TDDC_PHY(gTddcCfg.u4CidAddrR));        
        LOG(0, "MV    0x%08x \n", 
            TDDC_PHY(gTddcCfg.u4MvAddrOut[0]), TDDC_PHY(gTddcCfg.u4MvAddrOut[1]));
    }
    else
    {
        LOG(0, "TDDC init failed => No memory\n");
        return;
    }
    
	check_map = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);
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
	depth_buf_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	        
	depth_buf_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	        
	filter_buf_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	        
	filter_buf_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	        
	final_depth_buf_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	    
	final_depth_buf_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	    
	render_depth_buf_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	    
	render_depth_buf_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	    
	occ_map_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	occ_map_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	non_occ_L = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            
	non_occ_R = (INT32*)x_mem_alloc(sizeof(INT32)*depth_width*depth_height);	            

    if ((m_pDownY32_L == NULL) || (m_pDownY32_R == NULL) || 
        (m_pDownY32_L_ISR == NULL) || (m_pDownY32_R_ISR == NULL) || 
        (DE_buf_L == NULL) || (DE_buf_R == NULL) ||        
        (DE_buf_L_ISR == NULL) || (DE_buf_R_ISR == NULL) ||
        (DE_buf_L_PRE == NULL) || (DE_buf_L_INV == NULL) ||
        (depth_buf_L == NULL) || (depth_buf_R == NULL) || 
        (filter_buf_L == NULL) || (filter_buf_R == NULL) ||
        (final_depth_buf_L == NULL) || (final_depth_buf_R == NULL) || 
        (render_depth_buf_L == NULL) || (render_depth_buf_R == NULL) ||
        (occ_map_L == NULL) || (occ_map_R == NULL) || 
        (non_occ_L == NULL) || (non_occ_R == NULL) || (check_map == NULL))
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
    x_memset(depth_buf_L, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(depth_buf_R, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(filter_buf_L, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(filter_buf_R, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(final_depth_buf_L,	0, sizeof(INT32)*depth_width*depth_height);
    x_memset(final_depth_buf_R,	0, sizeof(INT32)*depth_width*depth_height);
    x_memset(render_depth_buf_L, 128, sizeof(INT32)*depth_width*depth_height);
    x_memset(render_depth_buf_R, 128, sizeof(INT32)*depth_width*depth_height);
    x_memset(occ_map_L, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(occ_map_R, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(non_occ_L, 0, sizeof(INT32)*depth_width*depth_height);
    x_memset(non_occ_R, 0, sizeof(INT32)*depth_width*depth_height);

    vDrvTddcInitISR();

    u1TddcInitFlag = SV_TRUE;
}


void vDrvTddcDebugInk(void)
{
    UINT32 x, y, ink;    
    UINT32 u4DebugIndex = IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DEBUG_INDEX);    
    UINT32 u4DebugW = IO32ReadFldAlign(TTD_04, TTD_DS4_H_MODE) ? ((depth_width+1)/2) : depth_width;
    UINT32 u4DebugH = IO32ReadFldAlign(TTD_04, TTD_DS4_V_MODE) ? ((depth_height+1)/2) : depth_height;
    UINT32 u4DsH = IO32ReadFldAlign(TTD_04, TTD_DS4_H_MODE) ? 2 : 1;
    UINT32 u4DsV = IO32ReadFldAlign(TTD_04, TTD_DS4_V_MODE) ? 2 : 1;
      
    if (u4DebugIndex)
    {               
        if (u4DebugIndex == 1)
        {                       
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = m_pDownY32_L[x*u4DsH+y*u4DsV*depth_width];
                    render_depth_buf_R[x+y*u4DebugW] = (255-m_pDownY32_R[x*u4DsH+y*u4DsV*depth_width]);
                }
            }
        }
        else if (u4DebugIndex == 2)
        {                      
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = DE_buf_L[x*u4DsH+y*u4DsV*depth_width].x+128;
                    render_depth_buf_R[x+y*u4DebugW] = DE_buf_R[x*u4DsH+y*u4DsV*depth_width].x+128;
                }
            }
        }
        else if (u4DebugIndex == 3)  
        {         
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = depth_buf_L[x*u4DsH+y*u4DsV*depth_width]+128;
                    render_depth_buf_R[x+y*u4DebugW] = depth_buf_R[x*u4DsH+y*u4DsV*depth_width]+128;
                }
            }      
        }
        else if (u4DebugIndex == 4)  
        {   
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = filter_buf_L[x*u4DsH+y*u4DsV*depth_width]+128;
                    render_depth_buf_R[x+y*u4DebugW] = filter_buf_R[x*u4DsH+y*u4DsV*depth_width]+128;
                }
            }
        }        
        else if (u4DebugIndex == 5)  
        {               
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = (occ_map_L[x*u4DsH+y*u4DsV*depth_width]==255)?128:255;
                    render_depth_buf_R[x+y*u4DebugW] = (occ_map_R[x*u4DsH+y*u4DsV*depth_width]==255)?128:0;
                }
            }
        }
        else if (u4DebugIndex == 6)  
        {              
            for (y=0; y<u4DebugH; y++)
            {
                for (x=0; x<u4DebugW; x++)
                {
                    render_depth_buf_L[x+y*u4DebugW] = non_occ_L[x*u4DsH+y*u4DsV*depth_width]+128;
                    render_depth_buf_R[x+y*u4DebugW] = non_occ_R[x*u4DsH+y*u4DsV*depth_width]+128;
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
            ink = IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DV_INK);
            render_depth_buf_L[x] = ink;
            render_depth_buf_R[x] = ink;
        }
    }        
}


void vDrvTddcSetData(void)
{
    UINT32 x, y, z, xx, yy;    
    UINT32 u4TddcWidth = IO32ReadFldAlign(TTD_04, TTD_DS4_H_MODE) ? ((depth_width+1)/2) : depth_width;
    UINT32 u4Width = IO32ReadFldAlign(TTD_02, TTD_IMG_WIDTH)/32;
    UINT32 u4Height = IO32ReadFldAlign(TTD_04, TTD_DS4_V_MODE) ? ((depth_height+1)/2) : depth_height;
    UINT32 u4DramWidth = (((u4Width*2*2)+63)>>6)<<6;    
    UINT32 u4WidthAlign = ((u4Width+3)>>2)<<2;    
    UINT8* pbDramData = (UINT8*)TDDC_VIR(gTddcCfg.u4MvAddrIn);
    DVEC *stMvL, *stMvR;

    //LOG(2, "Ttd W %d  Tddc W %d H %d \n", u4Width, u4TddcWidth, u4Height);
      
    if (u1TddcInitFlag == SV_TRUE)
    {               
        vDrvTddcDebugInk();        

        //vIO32WriteFldAlign(TTDDM_07, 0, HW_AUTO_DMA_READ_EN);
        
        if (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_MV_PATCH))
        {
            for (y=0; y < u4Height; y++)
            {
                for (x=0; x < u4Width; x++)
                {                
                    z = x+y*u4Width;
                    xx = (z%u4WidthAlign);
                    yy = (z/u4WidthAlign);
                    stMvL = (DVEC*)(pbDramData+yy*u4DramWidth+(xx/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+(xx%TDDC_MV_PACK)*TDDC_MV_SIZE);
                    stMvR = (DVEC*)(pbDramData+yy*u4DramWidth+(xx/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+TDDC_MV_PACK_SIZE+(xx%TDDC_MV_PACK)*TDDC_MV_SIZE);

                    stMvL->x = render_depth_buf_L[x+y*u4TddcWidth]-128;
                    stMvR->x = render_depth_buf_R[x+y*u4TddcWidth]-128;                    
                    stMvL->y = 0;
                    stMvR->y = 0;
                }
            }         
        }
        else
        {
            for (y=0; y < u4Height; y++)
            {
                for (x=0; x < u4Width; x++)
                {                
                    stMvL = (DVEC*)(pbDramData+y*u4DramWidth+(x/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+(x%TDDC_MV_PACK)*TDDC_MV_SIZE);
                    stMvR = (DVEC*)(pbDramData+y*u4DramWidth+(x/TDDC_MV_PACK)*TDDC_MV_PACK_LR_SIZE+TDDC_MV_PACK_SIZE+(x%TDDC_MV_PACK)*TDDC_MV_SIZE);
                    stMvL->x = render_depth_buf_L[x+y*u4Width]-128;
                    stMvL->y = 0;
                    stMvR->x = render_depth_buf_R[x+y*u4Width]-128;
                    stMvR->y = 0;
                }
            }        
        }

        HalFlushInvalidateDCacheMultipleLine_Ex((UINT32)pbDramData, TDDC_MV_DRAM_SIZE);        

        // Bug : cen problem        
        //vIO32WriteFldAlign(TTDDM_07, 1, FW_DMA_READ_TRIG);
        //vIO32WriteFldAlign(TTDDM_07, 0, FW_DMA_READ_TRIG);
        
        // Workaround : auto dma read        
        //vIO32WriteFldAlign(TTDDM_07, 1, HW_AUTO_DMA_READ_EN);
    }        
}


void vDrvTddcFwTriggerDC(void)
{    
    UINT32 x, y;
    UINT32 u4TtdW = IO32ReadFldAlign(TTD_02, TTD_IMG_WIDTH);     
    UINT32 u4TddcW = IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_WIDTH)/8;    
    UINT32 u4TddcH = IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_HEIGHT)/8;     
    UINT32 u4TtdDramW, u4TddcDramW;    
    UINT32 u4DsoAddrL, u4DsoAddrR, u4DvAddr;
    UINT8 *pbDSoDataL_0, *pbDSoDataR_0, *pbDSoDataL_1, *pbDSoDataR_1;    

    vStatusLog("Trigger DC", 1);
    
    // Config TTD DSO output address
    vIO32WriteFldAlign(TTDDM_01, TDDC_PHY(gTddcCfg.u4DsoAddrL[u4BufIndex])>>4, TTD_DSO_STR_ADR_L);   
    vIO32WriteFldAlign(TTDDM_02, TDDC_PHY(gTddcCfg.u4DsoAddrR[u4BufIndex])>>4, TTD_DSO_STR_ADR_R);   

    // Config TDDC DSO input address
    if (u4TtdW < (TDDC_DV_WIDTH_LIMIT/2))
    {                          
        u4TtdW = IO32ReadFldAlign(TTD_04, TTD_DS4_H_MODE) ? (u4TtdW/4) : (u4TtdW/8);
        u4TddcDramW = ((u4TddcW+63)>>6)<<6;
        u4TtdDramW = ((u4TtdW+63)>>6)<<6;
        
        pbDSoDataL_0 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrL[(u4BufIndex+3)&0x3]);    
        pbDSoDataR_0 = (UINT8*)TDDC_VIR(gTddcCfg.u4DsoAddrR[(u4BufIndex+3)&0x3]);
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
    
    u4DsoAddrL = gTddcCfg.u4DsoAddrL[(u4BufIndex+3)&0x3];
    u4DsoAddrR = gTddcCfg.u4DsoAddrR[(u4BufIndex+3)&0x3];
    u4DvAddr = gTddcCfg.u4MvAddrOut[0];
        
    vDrvTddcSetAddr(u4DsoAddrL, u4DsoAddrR, u4DvAddr);    
    vDrvTddcFwTiggerHw(2);
}
    
void vDrvTddcFwTriggerFsDet(void)
{        
    UINT32 u4DsoAddrL, u4DsoAddrR, u4DvAddr;
    
    vStatusLog("Trigger Fs", 2);

    LOG(5, "TDDC State %d Cal %d Mv %d SA %d\n",
        IO32ReadFldAlign(TDDC_STATUS1, RD_TDDC_CUR_STATE),
        IO32ReadFldAlign(TDDC_STATUS1, RD_TDDC_CAL_READY),
        IO32ReadFldAlign(TDDC_STATUS1, RD_MV_READY),
        IO32ReadFldAlign(TDDC_STATUS1, RD_SA_READY)); 
    
    LOG(3, "MJC Ori %d Ind %d \n",
        IO32ReadFldAlign(MJC_STA_FM_03, MJC_STA_MC_ORI_EN_HW),
        IO32ReadFldAlign(MJC_STA_FM_02, MJC_STA_MEMC_NUM_HW));
    
    vDrvTddcFsDetDump();    
    
    // Config TTD DSO output address
    vIO32WriteFldAlign(TTDDM_01, TDDC_PHY(gTddcCfg.u4DsoAddrL[u4BufIndex])>>4, TTD_DSO_STR_ADR_L);   
    vIO32WriteFldAlign(TTDDM_02, TDDC_PHY(gTddcCfg.u4DsoAddrR[u4BufIndex])>>4, TTD_DSO_STR_ADR_R);  

    if (fgIsSupport120HzPanel())
    {       
        if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_SRC_AUTO) == SV_TRUE)
        {
            // After Reset TTD freerun, first frame is right
            if (IO32ReadFldAlign(MJC_STA_FM_03, MJC_STA_MC_ORI_EN_HW) == 0)
            {          
                vIO32WriteFldAlign(SW_TDDC_REG_01, 0, TDDC_FS_SRC_INV); 
            }
            else
            {            
                vIO32WriteFldAlign(SW_TDDC_REG_01, 1, TDDC_FS_SRC_INV); 
            }
        }
        
        if ((u4BufIndex & 1) == 0)
        {                
            if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_SRC_INV) == SV_FALSE)
            {                
                u4DsoAddrL = gTddcCfg.u4DsoAddrL[(u4BufIndex+2)&0x3];
                u4DsoAddrR = gTddcCfg.u4DsoAddrL[(u4BufIndex+3)&0x3];
            }
            else
            {                
                u4DsoAddrL = gTddcCfg.u4DsoAddrR[(u4BufIndex+2)&0x3];
                u4DsoAddrR = gTddcCfg.u4DsoAddrR[(u4BufIndex+3)&0x3];
            }
        }
        else
        {
            return;
        }
    }
    else
    {                    
        u4DsoAddrL = gTddcCfg.u4DsoAddrL[(u4BufIndex+3)&0x3];
        u4DsoAddrR = gTddcCfg.u4DsoAddrR[(u4BufIndex+3)&0x3];
    }
    
    u4DvAddr = gTddcCfg.u4MvAddrOut[0];
        
    vDrvTddcSetAddr(u4DsoAddrL, u4DsoAddrR, u4DvAddr);    
    vDrvTddcFwTiggerHw(1);
}

void vDrvTddcFwTrigger(void)
{
    static UINT32 u4Count = 0;
    static UINT32 u4FwEnable = 0xFF;

    if (IO32ReadFldAlign(TDDC_00, C_TDDC_EN) == SV_FALSE)
    {
        return;
    }

    vStatusLog("Trigger", 0);

    if (IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_WIDTH) < TDDC_DV_WIDTH_LIMIT)
    {          
        LOG(2, "TDDC Width too small %d  !!\n", IO32ReadFldAlign(TDDC_00, C_TDDC_IMG_WIDTH));
        return;    
    }

    u4Count = (u4Count < IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_TRIGGER_LOOP)) ? (u4Count+1) : 0; 
       
    if (u4Count != 0) 
    {        
        LOG(5, "Loop count %d < %d \n", u4Count, IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_TRIGGER_LOOP));
        return;
    }

    if (u4FwEnable != IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_ENABLE))
    {                
        u4FwEnable = IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FW_ENABLE);
        vIO32WriteFldAlign(TDDC_00, !u4FwEnable, C_TDDC_HW_TRIG_EN);
        //vIO32WriteFldAlign(TTDDM_07, !u4FwEnable, HW_AUTO_DMA_READ_EN);  
        vIO32WriteFldAlign(TTDDM_07, SV_ON, HW_AUTO_DMA_READ_EN);  

        if (!u4FwEnable)
        {
            vDrvTddcSetAddr(gTddcCfg.u4DsoAddrL[0], gTddcCfg.u4DsoAddrR[0], gTddcCfg.u4MvAddrOut[0]); 
            vIO32WriteFldAlign(TTDDM_01, TDDC_PHY(gTddcCfg.u4DsoAddrL[0])>>4, TTD_DSO_STR_ADR_L);   
            vIO32WriteFldAlign(TTDDM_02, TDDC_PHY(gTddcCfg.u4DsoAddrR[0])>>4, TTD_DSO_STR_ADR_R);   
            vIO32WriteFldAlign(TTDDM_07, TDDC_PHY(gTddcCfg.u4MvAddrOut[0])>>4, TTD_MV_STR_ADR); 
        }
    }    

    if ((IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_DET)) 
        && (IO32ReadFldAlign(SW_TDDC_REG_00, TDDC_FS_DET_RST)))
    {        
        LOG(2, "n\n==> Reset TTD Free run 1 -> 0\n");
        vIO32WriteFldAlign(SW_TDDC_REG_00, 0, TDDC_FS_DET_RST);
        vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, 1, MJC_SVP_RST);  
        vIO32WriteFldAlign(MJC_SYS_CLK_CTRL, 0, MJC_SVP_RST);  
    }   
   
    if (u4FwEnable == SV_TRUE)
    {           
        u1IsrFlag = 0;        
        u4BufIndex = (u4BufIndex+1)&0x3;
        
        if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_DC_CAL) == SV_TRUE) 
        {
            vDrvTddcFwTriggerDC();
        }
        else if (IO32ReadFldAlign(SW_TDDC_REG_01, TDDC_FS_DET) == SV_TRUE)
        {
            vDrvTddcFwTriggerFsDet();
        }
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
    
    depth_width = ((u4TddcWidth+7)/8+m_mbSize-1)/m_mbSize;
    depth_height = ((u4TddcHeigh+7)/8+m_mbSize-1)/m_mbSize;

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
            && (dv_max_count >= (depth_width*depth_height)))
        {            
            //vStatusLog("Proc  In", 9);

            vTimeLogStart();
            
            vDrvTddcUpdateParam();            

            vTimeLog("Update     time ");
            
            OccProcess(m_pDownY32_L, m_pDownY32_R, DE_buf_L, DE_buf_R, 
                occ_map_L, occ_map_R, non_occ_L, non_occ_R, shift_avg_H, shift_avg_V);

            vTimeLog("OccProc    time ");
            
            HoleFill_L(depth_buf_L, DE_buf_L, occ_map_L);
            HoleFill_R(depth_buf_R, DE_buf_R, occ_map_R);	
            
            vTimeLog("HoleFill   time ");
            
            DilterBiltInt(filter_buf_L, m_pDownY32_L, depth_buf_L);
            DilterBiltInt(filter_buf_R, m_pDownY32_R, depth_buf_R);
            
            vTimeLog("DilterBilt time ");
            
            DepthBlend(render_depth_buf_L, final_depth_buf_L, filter_buf_L, 
    			render_depth_buf_R, final_depth_buf_R, filter_buf_R);

            vTimeLog("DepthBlend time ");
            
            DepthDown(render_depth_buf_L, render_depth_buf_L, render_depth_buf_R, render_depth_buf_R,
                IO32ReadFldAlign(TTD_04, TTD_DS4_H_MODE), IO32ReadFldAlign(TTD_04, TTD_DS4_V_MODE));
            
            vTimeLog("DepthDown  time ");
            
            vDrvTddcSetData();
            
            vTimeLog("Dumpin     time ");    
            
            //vStatusLog("Proc  Out", 0xA);
        }
    }    
}

void vDrvTddcSetCtrl(UINT8 u1OnOff)
{
    UINT32 u4TDTVMJCMode = u4DrvTDTVMJCModeQuery();
    
    u1IsrFlag = 0;
    
    LOG(2, "Tddc Set Ctrl %d\n", u1OnOff);

    if (u1OnOff == SV_OFF)
    {
        vIO32WriteFldAlign(TTDDM_03, SV_OFF, TTD_DRAM_WT_CID_EN);          
        vIO32WriteFldAlign(TTDDM_07, SV_OFF, TTD_DRAM_RD_DMAP_EN);        
        vIO32WriteFldAlign(TTD_04, SV_OFF, TTD_3DC_EN);        
        vIO32WriteFldAlign(SW_TDDC_REG_01, SV_OFF, TDDC_DC_CAL);           
        
        #if SUPPORT_3D_FS_DET                
            vIO32WriteFldAlign(TTDDM_02, SV_ON, TTD_DRAM_WT_DSO_EN);
            vIO32WriteFldMulti(TDDC_03, P_Fld(SV_ON, C_TDDC_DRAM_WT_MV_EN)
                                        | P_Fld(SV_ON, C_TDDC_DRAM_RD_MV_EN)
                                        | P_Fld(SV_ON, C_TDDC_DRAM_RD_SA_EN));   
            vIO32WriteFldAlign(TTD_02, SV_ON, LR_FREERUN_EN);
            vIO32WriteFldAlign(SW_TDDC_REG_00, 1, TDDC_TRIGGER_LOOP); 
            vIO32WriteFldAlign(SW_TDDC_REG_01, SV_ON, TDDC_FS_DET);
            vIO32WriteFldAlign(SW_TDDC_REG_00, SV_ON, TDDC_FW_ENABLE);            
            vIO32WriteFldAlign(TDDC_00, SV_ON, C_TDDC_EN);             
            vIO32WriteFldAlign(SW_TDDC_REG_00, 1, TDDC_FS_DET_RST);
        #else
            vIO32WriteFldAlign(TTDDM_02, SV_OFF, TTD_DRAM_WT_DSO_EN);            
            vIO32WriteFldMulti(TDDC_03, P_Fld(SV_OFF, C_TDDC_DRAM_WT_MV_EN)
                                        | P_Fld(SV_OFF, C_TDDC_DRAM_RD_MV_EN)
                                        | P_Fld(SV_OFF, C_TDDC_DRAM_RD_SA_EN));              
            vIO32WriteFldAlign(TTD_02, SV_OFF, LR_FREERUN_EN);
            vIO32WriteFldAlign(SW_TDDC_REG_00, 0, TDDC_TRIGGER_LOOP); 
            vIO32WriteFldAlign(SW_TDDC_REG_01, SV_OFF, TDDC_FS_DET);
            vIO32WriteFldAlign(SW_TDDC_REG_00, SV_OFF, TDDC_FW_ENABLE);            
            vIO32WriteFldAlign(TDDC_00, SV_OFF, C_TDDC_EN); 
        #endif
    }
    else
    {        
        vIO32WriteFldAlign(TTDDM_02, SV_ON, TTD_DRAM_WT_DSO_EN);
        vIO32WriteFldAlign(TTDDM_03, SV_ON, TTD_DRAM_WT_CID_EN);  
        vIO32WriteFldAlign(TTDDM_07, SV_ON, TTD_DRAM_RD_DMAP_EN);
        vIO32WriteFldMulti(TDDC_03, P_Fld(SV_ON, C_TDDC_DRAM_WT_MV_EN)
                                    | P_Fld(SV_ON, C_TDDC_DRAM_RD_MV_EN)
                                    | P_Fld(SV_ON, C_TDDC_DRAM_RD_SA_EN));        
        vIO32WriteFldAlign(TTD_02, SV_OFF, LR_FREERUN_EN);
        
        if (TD_MJC_OUT(u4TDTVMJCMode) == E_TD_OUT_3D_FS)
        {
            vIO32WriteFldAlign(SW_TDDC_REG_00, 1, TDDC_TRIGGER_LOOP); 
        }
        else
        
        {
        	vIO32WriteFldAlign(SW_TDDC_REG_00, 0, TDDC_TRIGGER_LOOP); 
        }
        
        vIO32WriteFldAlign(TTD_04, SV_ON, TTD_3DC_EN);
        vIO32WriteFldAlign(SW_TDDC_REG_01, SV_ON, TDDC_DC_CAL);        
        vIO32WriteFldAlign(SW_TDDC_REG_01, SV_OFF, TDDC_FS_DET);
        vIO32WriteFldAlign(SW_TDDC_REG_00, SV_ON, TDDC_FW_ENABLE);        
        vIO32WriteFldAlign(TDDC_00, SV_ON, C_TDDC_EN); 
    }   
}
