/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/
#ifndef _GRAPHICS3D_IF_C_
#define _GRAPHICS3D_IF_C_

#include <linux/string.h>
#include <linux/file.h>

#include "x_hal_5381.h"

#include "x_drv_cli.h"
#include "x_serial.h"
#include "x_printf.h"
#include "x_os.h"

#include "x_assert.h"
#include "x_util.h"
#include "x_stl_lib.h"
#include "x_common.h"
#include "x_bim.h"
#include "x_rand.h"
#include "drvcust_if.h"

#include "gfx3d_vfy_if.h"
#include "gfx3d_vfy_sys.h"

#include "x_hal_926.h"
#include "x_typedef.h"
#include "x_printf.h"
#include "x_debug.h"

#include "x_kmem.h"

#include <linux/fs.h>
#include "x_fm.h"
#include <asm/unistd.h>


/*-----------------------------------------------------------------------------
                    function extension
 ----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
                    data define
 ----------------------------------------------------------------------------*/
G3D_EMU_CONFIG_T eG3dEmuConfig;
G3D_EMU_RESULT_T eG3dEmuResult;

CHAR     ucConfigPathName [256] = "D:\\g3d\0";
CHAR     ucConfigFileName [256] = "config.txt\0";
CHAR     ucConfigStringName [256];
UINT32   u4PatternCnt =0;
CHAR    compname[10][128];
CHAR    goldname[10][128];
UINT32  cmpcnt = 0;
UINT32  goldcnt = 0;    
BOOL    fgMakeFileParsed = FALSE;
INT32   u4DumpMemCount[10];
INT32   u4DumpRegCount[10];
CHAR    *pPPPhyMemBuff = NULL;

CHAR    *pGPDTEBuffer = NULL;
CHAR    *pPPDTEBuffer = NULL;
CHAR    *pPP1DTEBuffer = NULL;

UINT32   u4MaxDiff = 0;  

UINT32   u4RegRecord [MAX_REG_REC_CNT][MAX_REG_REC_IDX][2];
UINT32   u4RecordCnt = 0;

CHAR    *pcStaticTotalBuffer = NULL;
CHAR    *pHexFileBuffer = NULL;
CHAR	*pConfigBuffer = NULL;   
CHAR	*pG3dEmuList = NULL;

CHAR    *pMakeFileBuffer = NULL;
CHAR    *pGPPhyMemBuff = NULL;

BOOL    fgIsPPOutBuff = FALSE;
CHAR    *pPPOutBuffAddr = NULL;
UINT32  u4PPOutBuffBase = 0x0;


#if TEST_G3D_INTERRUPT
static x_os_isr_fct _pfnOldIsr;
static HANDLE_T _hG3dIsrSema = 0;
static HANDLE_T _hG3dPpIsrSema = 0;
static HANDLE_T _hG3dPp1IsrSema = 0;
#endif


#if G3D_SUPPORT_INTERNAL_MMU
static UINT32 u4GPDTE = 0;
static UINT32 u4PPDTE = 0;

#if G3D_SUPPORT_MP
static UINT32 u4PP1DTE = 0;
#if G3D_SUPPORT_MALIMP4
static UINT32 u4PP2DTE = 0;
static UINT32 u4PP3DTE = 0;
#endif //G3D_SUPPORT_MALIMP4
#endif //G3D_SUPPORT_MP

#endif //G3D_SUPPORT_INTERNAL_MMU

UINT32 eG3DMemLayOut [2][10][2] =
{          
	/*	448 M availabe total  in VFY environment */
    {  
        //Base Addr                                                                          
        {MEM_DATA_BASE_ADDR ,                    0x0}, 
        //List Addr
        {MEM_DATA_BASE_ADDR,                   (1024*1024)},
        //Config Addr   
        {MEM_DATA_BASE_ADDR+(1024*1024),       (1024*1024)}, 
        //Make Addr
        {MEM_DATA_BASE_ADDR+(1024*1024)*2,     (1024*1024)}, 
        //GP Table                                                    
        {MEM_DATA_BASE_ADDR+(1024*1024)*3,     (1024*1024)*4}, 
        //PP Table                                                    
        {MEM_DATA_BASE_ADDR+(1024 * 1024)*7,   (1024*1024)*4}, 
        //Hex Addr
        {MEM_DATA_BASE_ADDR+(1024 * 1024)*11,  (1024*1024)*140}, 
        //GP Physical                              
        {MEM_DATA_BASE_ADDR+(1024 * 1024)*151,  (1024*1024)*140},
        //PP Physical
        {MEM_DATA_BASE_ADDR+(1024 * 1024)*291,  (1024*1024)*140}, 
    },

	/*	144 M availabe total  in Emu environment */
    {  
        //Base Addr                                                                          
        {MEM_DATA_BASE_ADDR ,                    0x0}, 
        //List Addr
        {MEM_DATA_BASE_ADDR,                   (1024*1024)},
        //Config Addr   
        {MEM_DATA_BASE_ADDR+(1024*1024),       (1024*1024)}, 
        //Make Addr
        {MEM_DATA_BASE_ADDR+(1024*1024)*2,     (1024*1024)}, 
        //GP Table                                                    
        {MEM_DATA_BASE_ADDR+(1024*1024)*3,     (1024*1024)*4}, 
        //PP Table                                                    
        {MEM_DATA_BASE_ADDR+(1024 * 1024)*7,   (1024*1024)*4}, 
        //Hex Addr
        {MEM_DATA_BASE_ADDR+(1024 * 1024)*11,  (1024*1024)*40}, 
        //GP Physical                              
        {MEM_DATA_BASE_ADDR+(1024 * 1024)*51,  (1024*1024)*40},
        //PP Physical
        {MEM_DATA_BASE_ADDR+(1024 * 1024)*91,  (1024*1024)*40}, 
    }
};

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/
    
/*-----------------------------------------------------------------------------
                    code start
 ----------------------------------------------------------------------------*/


void vMaliForceResetGP(void)
{
    UINT32 regval;
    UINT32 u4waitcnt = 0;
    UINT32 u4RetryCnt = 1000;

    Printf("GP FORCE RESET!!!\n");

    regval = u4MaliReadReg(0x0020);
    regval |= (0x1 << 9);
    vMaliWriteReg(0x0020, regval);

    while(u4waitcnt < u4RetryCnt)
    {
        x_thread_delay(2);
        if (u4MaliReadReg(0x0024)  &  (0x1<<12))
        {
          break;
        }
        u4waitcnt++;
    }

    if (u4waitcnt  >= u4RetryCnt)
    Printf("GP FORCE RESET FAIL!!!\n");

    regval = u4MaliReadReg(0x0020);
    regval |= (0x1 << 5);
    vMaliWriteReg(0x0020, regval);

      
}


//Gazelle reset MFG top
static void vMaliResetMFGTop(void)
{
	INT32  i4regval = 0;
	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x0));
    i4regval = i4regval | (0xc0000000);
	vWriteReg(MALI_TOP_OFFSET + 0x0, i4regval);	
	x_thread_delay(20);
	i4regval = i4regval & (0x3fffffff);
	vWriteReg(MALI_TOP_OFFSET + 0x0, i4regval);
	return;
}

void vMaliResetHw(void)
{
	//GP software reset:
	//write 0020[10]=1
	//wait reset complete interrupt 0030[19]
	INT32  regval = 0;
	UINT32 u4waitcnt = 0;
	UINT32 u4RetryCnt = 10;

	HalFlushInvalidateDCache();
	
	regval = u4MaliReadReg(0x0020);
	regval |= (0x1<<10);
	vMaliWriteReg(0x0020, regval);
	u4waitcnt = 0;
	while(u4waitcnt < u4RetryCnt)
	{
	  x_thread_delay(2);
	  if (u4MaliReadReg(0x0030)	&  (0x1<<19))
	  {
		   break;
	  }
	  u4waitcnt++;
	}
	
	//PP0 software reset:
	//write 900c[7]=1
	//wait reset complete interrupt 902c[12]
	vMaliWriteReg(0x9028, 0xFFF);

	regval = u4MaliReadReg(0x900c);
	regval |= (0x1<<7);
	vMaliWriteReg(0x900c, regval);
	u4waitcnt = 0;
	while(u4waitcnt < u4RetryCnt)
	{
	  x_thread_delay(2);
	  if (u4MaliReadReg(0x0902c)  &	(0x1<<12))
	  {
		   break;
	  }
	  u4waitcnt++;
					 
	}

	#if G3D_SUPPORT_MP
    //PP1 software reset:
	//write b00c[7]=1
	//wait reset complete interrupt b02c[12]
	vMaliWriteReg(0xb028, 0xFFF);

	regval = u4MaliReadReg(0xb00c);
	regval |= (0x1<<7);
	vMaliWriteReg(0xb00c, regval);
	u4waitcnt = 0;
	while(u4waitcnt < u4RetryCnt)
	{
	  x_thread_delay(2);
	  if (u4MaliReadReg(0x0b02c)  &	(0x1<<12))
	  {
		   break;
	  }
	  u4waitcnt++;
					 
	}

#if G3D_SUPPORT_MALIMP4
	//PP2 software reset:
	//write b00c[7]=1
	//wait reset complete interrupt b02c[12]
	vMaliWriteReg(0xd028, 0xFFF);

	regval = u4MaliReadReg(0xd00c);
	regval |= (0x1<<7);
	vMaliWriteReg(0xd00c, regval);
	u4waitcnt = 0;
	while(u4waitcnt < u4RetryCnt)
	{
	  x_thread_delay(2);
	  if (u4MaliReadReg(0x0d02c)  & (0x1<<12))
	  {
		   break;
	  }
	  u4waitcnt++;
					 
	}


	//PP3 software reset:
	//write b00c[7]=1
	//wait reset complete interrupt b02c[12]
	vMaliWriteReg(0xf028, 0xFFF);

	regval = u4MaliReadReg(0xf00c);
	regval |= (0x1<<7);
	vMaliWriteReg(0xf00c, regval);
	u4waitcnt = 0;
	while(u4waitcnt < u4RetryCnt)
	{
	  x_thread_delay(2);
	  if (u4MaliReadReg(0x0f02c)  & (0x1<<12))
	  {
		   break;
	  }
	  u4waitcnt++;
					 
	}
#endif
	
    #endif

	//no need reset MMU, MMU already setting @ beginning,
	
}


UINT32 u4StrToInt (CHAR *adr1)
{    
    UINT32 value, idx, i;
    UINT32 oft = 0;

    value=0; idx=0;
    for(i=(8-strlen(adr1));i<8;i++)
    {
        if(adr1[idx]=='0') oft = 0;
        else if(adr1[idx]=='1') oft = 1;
        else if(adr1[idx]=='2') oft = 2;
        else if(adr1[idx]=='3') oft = 3;
        else if(adr1[idx]=='4') oft = 4;
        else if(adr1[idx]=='5') oft = 5;
        else if(adr1[idx]=='6') oft = 6;
        else if(adr1[idx]=='7') oft = 7;
        else if(adr1[idx]=='8') oft = 8;
        else if(adr1[idx]=='9') oft = 9;
        else if(adr1[idx]=='a' || adr1[idx]=='A' ) oft = 10;
        else if(adr1[idx]=='b' || adr1[idx]=='B' ) oft = 11;
        else if(adr1[idx]=='c' || adr1[idx]=='C' ) oft = 12;
        else if(adr1[idx]=='d' || adr1[idx]=='D' ) oft = 13;
        else if(adr1[idx]=='e' || adr1[idx]=='E' ) oft = 14;
        else if(adr1[idx]=='f' || adr1[idx]=='F' ) oft = 15;
        idx++;
        value = value + ((oft & 0xf) << ((7-i)*4));
    }

    return value;
}

void vIrqAnalysis(void)
{
     UINT32 u4Gp_24, u4Gp_68;
     UINT32 u4Pp_20, u4Pp_2c;
      
     u4Gp_24 = u4MaliGPReadReg(0x24);
     u4Gp_68 = u4MaliGPReadReg(0x68);
     u4Pp_20 = u4MaliPPReadReg(0x1020); 
     u4Pp_2c = u4MaliPPReadReg(0x102c);         
}

#if 0
static void vPrintCheckSumRegister(void)
{
	INT32  i4regval = 0;
	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x40));	
	Printf("======vPrintCheckSumRegister======\n");
	Printf("[0x40]:0x%x \n",i4regval);

	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x44)); 
	Printf("[0x44]:0x%x \n",i4regval);

	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x48)); 
	Printf("[0x48]:0x%x \n",i4regval);

    i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x4C)); 
    Printf("[0x4C]:0x%x \n",i4regval);

	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x50)); 
	Printf("[0x50]:0x%x \n",i4regval);

	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x54)); 
	Printf("[0x54]:0x%x \n",i4regval);

	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x58)); 
	Printf("[0x58]:0x%x \n",i4regval);

	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x5C)); 
	Printf("[0x5C]:0x%x \n",i4regval);

    i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x60)); 
	Printf("[0x60]:0x%x \n",i4regval);

	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x64)); 
	Printf("[0x64]:0x%x \n",i4regval);

	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x68)); 
	Printf("[0x68]:0x%x \n",i4regval);

	i4regval = (u4ReadReg(MALI_TOP_OFFSET + 0x6C)); 
	Printf("[0x6C]:0x%x \n",i4regval);

    //temp
	i4regval = u4MaliReadReg(MALI400_REG_BASE_L2 + 0x1C); 
	Printf("MALI400_REG_BASE_L2 Enable:0x%x \n",i4regval);

	return;
}
#endif

void vDumpIntStatus( void)
{
    /*Show Mali IRQ Status*/
    #define GPU_DUMP_REG( addr )    printf( "%s (0x%x) = 0x%x\n" , #addr, addr, u4MaliReadReg(addr))
    /*GP*/
    GPU_DUMP_REG( (MALI400_REG_BASE_GP+0x30) );
    /*GP MMU*/
    GPU_DUMP_REG( (MALI400_REG_BASE_MMU_GP+0x20) );
    /*PP0*/
    GPU_DUMP_REG( (MALI400_REG_BASE_PP0+0x102c) );
    /*PP0 MMU*/
    GPU_DUMP_REG( (MALI400_REG_BASE_MMU_PP0+0x20) );
    /*PP1*/
    GPU_DUMP_REG( (MALI400_REG_BASE_PP1+0x102c) );
    /*PP1 MMU*/
    GPU_DUMP_REG( (MALI400_REG_BASE_MMU_PP1+0x20) );
    Printf("\r\n");

}

void vPrintRegister (G3D_CMP_STATUS fgWaitIrqDone)
{
     UINT32 u4RegVal;
     UINT32 u4Reg;
     UINT32 i;
     UINT32 u4LastRecCnt = u4RecordCnt-1;

     if (u4RecordCnt == 0)
     {
         //Printf("Dump : NO Register Record\n");
         return;
     }
     else
     {
        Printf("Dump Registers: \n");
     }
     
     for (i=0; i < MAX_REG_REC_IDX; i++)
     {
          u4Reg      = u4RegRecord[u4LastRecCnt][i][0];
          u4RegVal = u4RegRecord[u4LastRecCnt][i][1];

          if (u4Reg == 0x1024)
            Printf("L2_RD[ 0x%08X ] : 0x%x \n", u4Reg, u4RegVal);
          else
          if (u4Reg == 0x102C)
            Printf("L2_WR[ 0x%08X ] : 0x%x \n", u4Reg, u4RegVal);
          else
          if (u4Reg == 0x004C)
            Printf("GP_FR[ 0x%08X ] : 0x%x \n", u4Reg, u4RegVal);
          else
          if (u4Reg == 0x908C)
            Printf("PP_FR[ 0x%08X ] : 0x%x \n", u4Reg, u4RegVal);
          else
            Printf("REG[ 0x%08X ] : 0x%x \n", u4Reg, u4RegVal);
     }     
     Printf("Dump End\n");
}

void vDumpRegister( void )
{
	UINT32 u4RegVal = 0;
	UINT32 u4Reg = 0;
	UINT32 i = 0;

	if ( u4RecordCnt > MAX_REG_REC_CNT)
	{
		Printf("Dump Register Over Array Size 1\n");
		ASSERT(0);
	}

	//GP Status
	Printf("Dump Registers: \n");
	for (u4Reg = 0x24; u4Reg <= 0x30; u4Reg+=4)
	{
		u4RegVal = u4MaliReadReg(u4Reg);
		//Printf("0x%08X : 0x%x \n", (MALI_REG_OFFSET+u4Reg), u4RegVal);
		u4RegRecord[u4RecordCnt][i][0] = u4Reg;
		u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
		i++;
	}

	for (u4Reg = 0x3C; u4Reg <= 0x58; u4Reg+=4)
	{
		u4RegVal = u4MaliReadReg(u4Reg);
		//Printf("0x%08X : 0x%x \n", (MALI_REG_OFFSET+u4Reg), u4RegVal);
		u4RegRecord[u4RecordCnt][i][0] = u4Reg;
		u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
		i++;

	}

	u4Reg = 0x68;
	u4RegVal = u4MaliReadReg(u4Reg);
	//Printf("0x%08X : 0x%x \n", (MALI_REG_OFFSET+0x68), u4RegVal);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	u4Reg = 0x6c;
	u4RegVal = u4MaliReadReg(u4Reg);
	//Printf("0x%08X : 0x%x \n", (MALI_REG_OFFSET+0x6c), u4RegVal);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	u4Reg = 0x94;
	u4RegVal = u4MaliReadReg(u4Reg);
	//Printf("0x%08X : 0x%x \n", (MALI_REG_OFFSET+0x94), u4RegVal);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	u4Reg = 0xa0;
	u4RegVal = u4MaliReadReg(u4Reg);
	//Printf("0x%08X : 0x%x \n", (MALI_REG_OFFSET+0xa0), u4RegVal);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	u4Reg = 0xa4;
	u4RegVal = u4MaliReadReg(u4Reg);
	//Printf("0x%08X : 0x%x \n", (MALI_REG_OFFSET+0xa4), u4RegVal);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	//PP Status
	for (u4Reg = 0x9000; u4Reg < 0x9010; u4Reg+=4)
	{
		u4RegVal = u4MaliReadReg(u4Reg);
		//Printf("0x%08X : 0x%08x \n", (MALI_REG_OFFSET+u4Reg), u4RegVal);
		u4RegRecord[u4RecordCnt][i][0] = u4Reg;
		u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
		i++;
	}     

	for (u4Reg = 0x9020; u4Reg < 0x9030; u4Reg+=4)
	{
		u4RegVal = u4MaliReadReg(u4Reg);
		//Printf("0x%08X : 0x%08x \n", (MALI_REG_OFFSET+u4Reg), u4RegVal);
		u4RegRecord[u4RecordCnt][i][0] = u4Reg;
		u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
		i++;
	}     

	u4Reg = 0x9050;
	u4RegVal = u4MaliReadReg(u4Reg);
	//Printf("0x%08X : 0x%08x \n", (MALI_REG_OFFSET+0x09050), u4RegVal);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	u4Reg = 0x9060;
	u4RegVal = u4MaliReadReg(u4Reg);
	//Printf("0x%08X : 0x%08x \n", (MALI_REG_OFFSET+0x09060), u4RegVal);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	for (u4Reg = 0x9080; u4Reg < 0x9090; u4Reg+=4)
	{
		u4RegVal = u4MaliReadReg(u4Reg);
		//Printf("0x%08X : 0x%08x \n", (MALI_REG_OFFSET+u4Reg), u4RegVal);
		u4RegRecord[u4RecordCnt][i][0] = u4Reg;
		u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
		i++;
	}     
	for (u4Reg = 0x90a0; u4Reg < 0x90b0; u4Reg+=4)
	{
		u4RegVal = u4MaliReadReg(u4Reg);
		//Printf("0x%08X : 0x%08x \n", (MALI_REG_OFFSET+u4Reg), u4RegVal);
		u4RegRecord[u4RecordCnt][i][0] = u4Reg;
		u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
		i++;
	}

	for (u4Reg = 0xE040; u4Reg < 0xE070; u4Reg+=4)
	{
		u4RegVal = u4MaliReadReg(u4Reg);
		//Printf("0x%08X : 0x%08x \n", (MALI_REG_OFFSET+u4Reg), u4RegVal);
		u4RegRecord[u4RecordCnt][i][0] = u4Reg;
		u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
		i++;
	}    

#if WATCH_PERF_COUNT        
	u4Reg = 0x1024;
	u4RegVal = u4MaliReadReg(u4Reg);           
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	u4Reg = 0x102C;          
	u4RegVal = u4MaliReadReg(u4Reg);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	u4Reg = 0x004C;  
	u4RegVal = u4MaliReadReg(u4Reg);
	//Printf("0x%08X : 0x%08x \n", (MALI_REG_OFFSET+u4Reg), u4RegVal);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;

	u4Reg = 0x908C;  
	u4RegVal = u4MaliReadReg(u4Reg);
	//Printf("0x%08X : 0x%08x \n", (MALI_REG_OFFSET+u4Reg), u4RegVal);
	u4RegRecord[u4RecordCnt][i][0] = u4Reg;
	u4RegRecord[u4RecordCnt][i][1] = u4RegVal;
	i++;
	  
#endif
	//Printf("Dump End\n");

	if ( i > MAX_REG_REC_IDX)
	{
		Printf("Dump Register Over Array Size 2\n");
		ASSERT(0);
	}

	u4RecordCnt++;
}


BOOL MaliWaitIrq(void)
{
	BOOL fgIRQ_GP, fgIRQ_Pp,fgIRQ_Pp1,fgIRQ_Pp2,fgIRQ_Pp3,fgIRQ_GpMmu, fgIRQ_PpMmu,fgIRQ_Pp1Mmu,fgIRQ_Pp2Mmu,fgIRQ_Pp3Mmu;
	BOOL  fgIsGPComplete;
	INT32 u4waitcnt = 0;
	UINT32 u4Gp_Irq_Raw, u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status;
	UINT32 u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status;
	#if 0
	UINT32 u4Pp1_Irq_Raw, u4Pp1_Irq_Clear, u4Pp1_Irq_Mask, u4Pp1_Irq_Status;
	UINT32 u4Pp2_Irq_Raw, u4Pp2_Irq_Clear, u4Pp2_Irq_Mask, u4Pp2_Irq_Status;
	UINT32 u4Pp3_Irq_Raw, u4Pp3_Irq_Clear, u4Pp3_Irq_Mask, u4Pp3_Irq_Status;
	#endif
	
	fgIRQ_GP = fgIRQ_Pp =fgIRQ_Pp1 =fgIRQ_Pp2 =fgIRQ_Pp3 = fgIRQ_GpMmu = fgIRQ_PpMmu =fgIRQ_Pp1Mmu =fgIRQ_Pp2Mmu =fgIRQ_Pp3Mmu = FALSE;
	u4waitcnt = 0;
	
	while (1)
	{
		//irq_gp = 0x68[0]
		//GPU status IRQ
		fgIRQ_GP = ((u4MaliReadReg(0x68)& 0x1) == 0x1)? TRUE : FALSE;
		//Vertex Shader end of command list
		//fgIRQ_GP = ((u4MaliReadReg(0x24) & 0x1)==0x1)? TRUE : FALSE;
		//irq_pp_0 = 0x9008[6]
		fgIRQ_Pp = (u4MaliReadReg(0x9008) & (0x1 << 6))? TRUE : FALSE;			
		fgIRQ_Pp1 = (u4MaliReadReg(0xB008) & (0x1 << 6))? TRUE : FALSE;  
		fgIRQ_Pp2 = (u4MaliReadReg(0xD008) & (0x1 << 6))? TRUE : FALSE; 		
		fgIRQ_Pp3 = (u4MaliReadReg(0xF008) & (0x1 << 6))? TRUE : FALSE; 

		fgIRQ_GpMmu =  (u4MaliReadReg(0x3020) & (0x3))? TRUE : FALSE;

		fgIRQ_PpMmu =  (u4MaliReadReg(0x4020) & (0x3))? TRUE : FALSE;
		fgIRQ_Pp1Mmu =	(u4MaliReadReg(0x5020) & (0x3))? TRUE : FALSE;
		fgIRQ_Pp2Mmu =	(u4MaliReadReg(0x6020) & (0x3))? TRUE : FALSE;
		fgIRQ_Pp3Mmu =	(u4MaliReadReg(0x7020) & (0x3))? TRUE : FALSE;

		u4Gp_Irq_Raw = u4MaliReadReg(0x24);

		/* for F204-plbu_scissor02_elements_mmu can not refer fgIsGPComplete as interrupt is GP_IRQ_HANG */
		fgIsGPComplete = (((u4Gp_Irq_Raw & 0x00707bff) == 0x1) || ((u4Gp_Irq_Raw & 0x00707bff) == 0x2) || ((u4Gp_Irq_Raw & 0x00707bff) == 0x3) );


		if ( (fgIRQ_GP) || fgIRQ_Pp || fgIRQ_Pp1|| fgIRQ_Pp2|| fgIRQ_Pp3|| fgIRQ_GpMmu
		|| fgIRQ_PpMmu || fgIRQ_Pp1Mmu || fgIRQ_Pp2Mmu || fgIRQ_Pp3Mmu )
		{							   
			vDumpIntStatus();
			u4Gp_Irq_Raw = u4MaliReadReg(0x24); 
			u4Gp_Irq_Clear = u4MaliReadReg(0x28); 
			u4Gp_Irq_Mask = u4MaliReadReg(0x2c); 
			u4Gp_Irq_Status = u4MaliReadReg(0x30);
			u4Pp_Irq_Raw = u4MaliReadReg(0x9020); 
			u4Pp_Irq_Clear = u4MaliReadReg(0x9024);
			u4Pp_Irq_Mask = u4MaliReadReg(0x9028);
			u4Pp_Irq_Status = u4MaliReadReg(0x902c);
			Printf( "GP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Gp_Irq_Raw, u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status);
			Printf( "PP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status);							   

			return TRUE;
		}

		x_thread_delay(10);
		u4waitcnt++;

		if (u4waitcnt > 1000)
		{
			Printf( "Wait IRQ Fail -1 !!!!\n");
			u4Gp_Irq_Raw = u4MaliReadReg(0x24); 
			u4Gp_Irq_Clear = u4MaliReadReg(0x28); 
			u4Gp_Irq_Mask = u4MaliReadReg(0x2c); 
			u4Gp_Irq_Status = u4MaliReadReg(0x30);
			u4Pp_Irq_Raw = u4MaliReadReg(0x9020); 
			u4Pp_Irq_Clear = u4MaliReadReg(0x9024);
			u4Pp_Irq_Mask = u4MaliReadReg(0x9028);
			u4Pp_Irq_Status = u4MaliReadReg(0x902c);
			Printf( "GP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Gp_Irq_Raw, u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status);
			Printf( "PP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status);								   
			vDumpIntStatus();
			return FALSE ;
		}
	}				


	u4Gp_Irq_Raw = u4MaliReadReg(0x24); 
	u4Gp_Irq_Clear = u4MaliReadReg(0x28); 
	u4Gp_Irq_Mask = u4MaliReadReg(0x2c); 
	u4Gp_Irq_Status = u4MaliReadReg(0x30);
	u4Pp_Irq_Raw = u4MaliReadReg(0x9020); 
	u4Pp_Irq_Clear = u4MaliReadReg(0x9024);
	u4Pp_Irq_Mask = u4MaliReadReg(0x9028);
	u4Pp_Irq_Status = u4MaliReadReg(0x902c);
	Printf( "GP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Gp_Irq_Raw, u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status);
	Printf( "PP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status);								 

}

BOOL MaliWaitIrqWithMaskAndVal(unsigned int mask, unsigned int val)
{
	BOOL fgIRQ_GP, fgIRQ_Pp,fgIRQ_Pp1,fgIRQ_Pp2,fgIRQ_Pp3,fgIRQ_GpMmu, fgIRQ_PpMmu,fgIRQ_Pp1Mmu,fgIRQ_Pp2Mmu,fgIRQ_Pp3Mmu;
	BOOL  fgIsGPComplete;
	INT32 u4waitcnt = 0;

	UINT32 u4Gp_Irq_Raw, u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status;
	UINT32 u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status;
	#if 0
	UINT32 u4Pp1_Irq_Raw, u4Pp1_Irq_Clear, u4Pp1_Irq_Mask, u4Pp1_Irq_Status;
	UINT32 u4Pp2_Irq_Raw, u4Pp2_Irq_Clear, u4Pp2_Irq_Mask, u4Pp2_Irq_Status;
	UINT32 u4Pp3_Irq_Raw, u4Pp3_Irq_Clear, u4Pp3_Irq_Mask, u4Pp3_Irq_Status;
	#endif
	
	fgIRQ_GP = fgIRQ_Pp =fgIRQ_Pp1 =fgIRQ_Pp2 =fgIRQ_Pp3 = fgIRQ_GpMmu = fgIRQ_PpMmu =fgIRQ_Pp1Mmu =fgIRQ_Pp2Mmu =fgIRQ_Pp3Mmu = FALSE;
    Printf("MaliTbWaitIrqWithMaskAndVal Start ...mask(0x%x) value(0x%x)\r\n", mask , val );
    while(1)
	{		 
		fgIRQ_GP = (u4MaliReadReg(0x68) == 0x1)? TRUE : FALSE;
		fgIRQ_Pp = (u4MaliReadReg(0x9008) & (0x1 << 6))? TRUE : FALSE;		  
		fgIRQ_Pp1 = (u4MaliReadReg(0xB008) & (0x1 << 6))? TRUE : FALSE;  
		fgIRQ_Pp2 = (u4MaliReadReg(0xD008) & (0x1 << 6))? TRUE : FALSE;		  
		fgIRQ_Pp3 = (u4MaliReadReg(0xF008) & (0x1 << 6))? TRUE : FALSE; 
		fgIRQ_GpMmu =  (u4MaliReadReg(0x3020) & (0x3))? TRUE : FALSE;
		fgIRQ_PpMmu =  (u4MaliReadReg(0x4020) & (0x3))? TRUE : FALSE;
		fgIRQ_Pp1Mmu =  (u4MaliReadReg(0x5020) & (0x3))? TRUE : FALSE;
		fgIRQ_Pp2Mmu =  (u4MaliReadReg(0x6020) & (0x3))? TRUE : FALSE;
		fgIRQ_Pp3Mmu =  (u4MaliReadReg(0x7020) & (0x3))? TRUE : FALSE;

		if (mask & GP_IRQ_EN)
		{
			u4Gp_Irq_Raw = u4MaliReadReg(0x24);
			fgIsGPComplete = (((u4Gp_Irq_Raw & 0x00707bff) == 0x1) || ((u4Gp_Irq_Raw & 0x00707bff) == 0x2) || ((u4Gp_Irq_Raw & 0x00707bff) == 0x3) );
		}
		else
		{
			fgIRQ_GP = TRUE;
			fgIsGPComplete = TRUE;
		}                             

		if (mask & PP0_IRQ_EN)
		{
		}
		else if(!(mask & PP_BDC_IRQ_EN))
		{
			fgIRQ_Pp = TRUE;
		}

		if (mask & GPMMU_IRQ_EN)                          
		{
		}
		else
		{
			fgIRQ_GpMmu = TRUE;
		}                          

		if (mask & PP0MMU_IRQ_EN)
		{
		}
		else
		{
			fgIRQ_PpMmu = TRUE;
		}


		if (mask & PP1_IRQ_EN)
		{
		}
		else if(!(mask & PP_BDC_IRQ_EN))
		{
			fgIRQ_Pp1 = TRUE;
		}

		if (mask & PP1MMU_IRQ_EN)
		{
		}
		else
		{
			fgIRQ_Pp1Mmu = TRUE;
		}

		if (mask & PP2_IRQ_EN)
		{
		}
		else if(!(mask & PP_BDC_IRQ_EN))
		{
			fgIRQ_Pp2 = TRUE;
		}

		if (mask & PP2MMU_IRQ_EN)
		{
		}
		else
		{
			fgIRQ_Pp2Mmu = TRUE;
		}		  

		if (mask & PP3_IRQ_EN)
		{
		}
		else if(!(mask & PP_BDC_IRQ_EN))
		{
			fgIRQ_Pp3 = TRUE;
		}

		if (mask & PP3MMU_IRQ_EN)
		{
		}
		else
		{
			fgIRQ_Pp3Mmu = TRUE;
		}

		/* for benchmark & SLT pattern only check GP & PP0 */
		if ( fgIRQ_GP && fgIRQ_Pp ) /*&& fgIRQ_Pp1 && fgIRQ_Pp2 && fgIRQ_Pp3 && fgIRQ_GpMmu
		&& fgIRQ_PpMmu && fgIRQ_Pp1Mmu && fgIRQ_Pp2Mmu && fgIRQ_Pp3Mmu)*/ 
		{
			vDumpIntStatus();
			return TRUE ;
		}

		x_thread_delay(10);
		u4waitcnt++;
		if (u4waitcnt > 1000)
		{
			Printf( "Wait IRQ Fail -2 !!!!\n");
			u4Gp_Irq_Raw = u4MaliReadReg(0x24); 
			u4Gp_Irq_Clear = u4MaliReadReg(0x28); 
			u4Gp_Irq_Mask = u4MaliReadReg(0x2c); 
			u4Gp_Irq_Status = u4MaliReadReg(0x30);
			u4Pp_Irq_Raw = u4MaliReadReg(0x9020); 
			u4Pp_Irq_Clear = u4MaliReadReg(0x9024);
			u4Pp_Irq_Mask = u4MaliReadReg(0x9028);
			u4Pp_Irq_Status = u4MaliReadReg(0x902c);
			Printf( "GP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Gp_Irq_Raw, u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status);
			Printf( "PP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status);
			vDumpIntStatus();
			return FALSE;
		}
	}

    Printf("MaliTbWaitIrqWithMaskAndVa: IRQ wait error. mask(0x%x) value(0x%x)\r\n", mask , val);
	
}


#if TEST_G3D_INTERRUPT
static VOID _Gfx3dIsr(UINT16 u2Vector)
{
    if(u2Vector == MALI450_VECTOR_GFX3D_GP)
    {
        UINT32 u4Gp_Irq_Raw;
        UINT32 u4Gp_Irq_Clear;        
        UINT32 u4Gp_Irq_Mask;                
        UINT32 u4Gp_Irq_Status;  
        UNUSED(u4Gp_Irq_Raw);        UNUSED(u4Gp_Irq_Clear);        UNUSED(u4Gp_Irq_Mask);        UNUSED(u4Gp_Irq_Status);
        u4Gp_Irq_Raw = u4MaliReadReg(0x24); 
        u4Gp_Irq_Clear = u4MaliReadReg(0x28); 
        u4Gp_Irq_Mask = u4MaliReadReg(0x2c); 
        u4Gp_Irq_Status = u4MaliReadReg(0x30);
        IO_WRITE32(0xF0040000, 0x28, 0xffffffff);        
        Printf("[G3D] GP interrupts!\n");          
        Printf("[G3D] GP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", 
        	u4Gp_Irq_Raw, u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status);     
        x_sema_unlock(_hG3dIsrSema);
    }
    else if(u2Vector == MALI450_VECTOR_GFX3D_PP)
    {
        UINT32 u4Pp_Irq_Raw;
        UINT32 u4Pp_Irq_Clear;        
        UINT32 u4Pp_Irq_Mask;                
        UINT32 u4Pp_Irq_Status;            
        UNUSED(u4Pp_Irq_Raw);        UNUSED(u4Pp_Irq_Clear);        UNUSED(u4Pp_Irq_Mask);        UNUSED(u4Pp_Irq_Status);        
        //HAL_GetTime(&_rTimePPFinish);        
        //HAL_GetDeltaTime(&_rActiveTime, &_rTimeGPTrigger, &_rTimePPFinish);        
        u4Pp_Irq_Raw = u4MaliReadReg(0x9020); 
        u4Pp_Irq_Clear = u4MaliReadReg(0x9024);
        u4Pp_Irq_Mask = u4MaliReadReg(0x9028);
        u4Pp_Irq_Status = u4MaliReadReg(0x902c);
        IO_WRITE32(0xF0049000, 0x24, 0xffffffff);    
        Printf("[G3D] PP interrupts!\n");            
        Printf("[G3D] PP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", 
        	u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status);                       
        //Printf("[G3D] Total frame time: %d sec %d micro!\n", _rActiveTime.u4Seconds, _rActiveTime.u4Micros);                    
        x_sema_unlock(_hG3dPpIsrSema);        
    }
	
	else if(u2Vector == MALI450_VECTOR_GFX3D_PP1)
	{
		UINT32 u4Pp_Irq_Raw;
		UINT32 u4Pp_Irq_Clear;		  
		UINT32 u4Pp_Irq_Mask;				 
		UINT32 u4Pp_Irq_Status; 		   
		UNUSED(u4Pp_Irq_Raw);		 UNUSED(u4Pp_Irq_Clear);		UNUSED(u4Pp_Irq_Mask);		  UNUSED(u4Pp_Irq_Status);		  
		//HAL_GetTime(&_rTimePPFinish);		 
		//HAL_GetDeltaTime(&_rActiveTime, &_rTimeGPTrigger, &_rTimePPFinish); 	   
		u4Pp_Irq_Raw = u4MaliReadReg(0xB020); 
		u4Pp_Irq_Clear = u4MaliReadReg(0xB024);
		u4Pp_Irq_Mask = u4MaliReadReg(0xB028);
		u4Pp_Irq_Status = u4MaliReadReg(0xB02c);
		IO_WRITE32(0xF004B000, 0x24, 0xffffffff);	 
        //#ifndef __MODEL_slt__
		Printf("[G3D] PP1 interrupts!\n");			  
		Printf("[G3D] PP1 IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", 
			u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status);						 
		//Printf("[G3D] Total frame time: %d sec %d micro!\n", _rActiveTime.u4Seconds, _rActiveTime.u4Micros);					
        //#endif        
		x_sema_unlock(_hG3dPp1IsrSema); 	   
	}
	
    else if(u2Vector == MALI450_VECTOR_GFX3D_GPMMU)
    {
        IO_WRITE32(0xF0043000, 0x18, 0xffffffff);        
        Printf("[G3D] GP MMU interrupts!\n");    
    }
    else if(u2Vector == MALI450_VECTOR_GFX3D_PPMMU)
    {
        IO_WRITE32(0xF0044000, 0x18, 0xffffffff);            
        Printf("[G3D] PP MMU interrupts!\n");    
    }
	
	else if(u2Vector == MALI450_VECTOR_GFX3D_PP1MMU)
    {
        IO_WRITE32(0xF0045000, 0x18, 0xffffffff);            
        Printf("[G3D] PP1 MMU interrupts!\n");    
    }
    else
    {
        Printf("[G3D] Unhook interrupts!\n");    
        ASSERT(0);
    }
}


BOOL fgMaliWaitIRQ (CHAR *pline)
{
   CHAR line[256];
   CHAR val0[24], val1[24];
   CHAR str0[100],str1[100];
   INT32  r, value;
   BOOL fgIRQ_GP, fgIRQ_Pp, fgIRQ_GpMmu, fgIRQ_PpMmu;
   INT32 u4waitcnt = 0;
   UINT32 u4Gp_Irq_Raw;
   UINT32 u4IrqCheck, u4IrqFlag;
   BOOL  fgIsGPComplete;
   UINT32 u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status;
   UINT32 u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status;
   strcpy(line, pline);
               
   HalFlushInvalidateDCache();
   
      val0[0] = 0x0;
      r = sscanf(line, " wait %s %s %s", str0, str1, val0);
      if(strcmp(str1, "irq")==0)
      {
            Printf( "\nwait(mali_%s_risc==1);\n\n", str1);

           fgIRQ_GP = fgIRQ_Pp = fgIRQ_GpMmu = fgIRQ_PpMmu = FALSE;

            //if(x_sema_lock_timeout(_hG3dIsrSema, 5000) != OSR_OK)
			if(x_sema_lock_timeout(_hG3dIsrSema, 30000) != OSR_OK)
			{
				u4Gp_Irq_Raw = u4MaliReadReg(0x24); 
				u4Gp_Irq_Clear = u4MaliReadReg(0x28); 
				u4Gp_Irq_Mask = u4MaliReadReg(0x2c); 
				u4Gp_Irq_Status = u4MaliReadReg(0x30);
				Printf( "GP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Gp_Irq_Raw, u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status);
			}
			if(x_sema_lock_timeout(_hG3dPpIsrSema, 30000) != OSR_OK)
			{
				u4Pp_Irq_Raw = u4MaliReadReg(0x9020); 
				u4Pp_Irq_Clear = u4MaliReadReg(0x9024);
				u4Pp_Irq_Mask = u4MaliReadReg(0x9028);
				u4Pp_Irq_Status = u4MaliReadReg(0x902c);
				Printf( "PP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status);															  
			}	
			
			if(x_sema_lock_timeout(_hG3dPp1IsrSema, 30000) != OSR_OK)
			{
				u4Pp_Irq_Raw = u4MaliReadReg(0xB020); 
				u4Pp_Irq_Clear = u4MaliReadReg(0xB024);
				u4Pp_Irq_Mask = u4MaliReadReg(0xB028);
				u4Pp_Irq_Status = u4MaliReadReg(0xB02c);
				Printf( "PP1 IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status);															   
			}
			
	  }
      else if(strcmp(str1, "clk")==0)
      {
        Printf( "\nrepeat(%s) @(%s mali_%s);\n\n", val0, str0, str1);
        value = 0;
        if ( val0[0] != 0x0)
           value = u4StrToInt(val0);

        if (value == 0)
               value = 20;
        
         x_thread_delay(value);
      }
      else if(strcmp(str0, "irq")==0)
      { 
      
		//if(x_sema_lock_timeout(_hG3dIsrSema, 5000) != OSR_OK)
		if(x_sema_lock_timeout(_hG3dIsrSema, 30000) != OSR_OK)
		{
		
			u4Gp_Irq_Raw = u4MaliReadReg(0x24); 
			u4Gp_Irq_Clear = u4MaliReadReg(0x28); 
			u4Gp_Irq_Mask = u4MaliReadReg(0x2c); 
			u4Gp_Irq_Status = u4MaliReadReg(0x30);
			Printf( "GP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Gp_Irq_Raw, u4Gp_Irq_Clear, u4Gp_Irq_Mask, u4Gp_Irq_Status);

		}
		if(x_sema_lock_timeout(_hG3dPpIsrSema, 30000) != OSR_OK)
		{
		
			u4Pp_Irq_Raw = u4MaliReadReg(0x9020); 
			u4Pp_Irq_Clear = u4MaliReadReg(0x9024);
			u4Pp_Irq_Mask = u4MaliReadReg(0x9028);
			u4Pp_Irq_Status = u4MaliReadReg(0x902c);
			Printf( "PP IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status); 															

		} 

		if(x_sema_lock_timeout(_hG3dPp1IsrSema, 30000) != OSR_OK)
		{
		
			u4Pp_Irq_Raw = u4MaliReadReg(0xB020); 
			u4Pp_Irq_Clear = u4MaliReadReg(0xB024);
			u4Pp_Irq_Mask = u4MaliReadReg(0xB028);
			u4Pp_Irq_Status = u4MaliReadReg(0xB02c);
			Printf( "PP1 IRQ: Raw=0x%x, Clear = 0x%x, Mask=0x%x, Status = 0x%x \n", u4Pp_Irq_Raw, u4Pp_Irq_Clear, u4Pp_Irq_Mask, u4Pp_Irq_Status);															   

		}	  			 
         if(r==1)
         {
            Printf(  "\n$display(\"Waiting for IRQ %s ...\");", val0);
            Printf(  "\nwait((IRQ & 'h%s));\n\n", val0);
         }
         else 
         if(r==2)
         {
               Printf(  "\n$display(\"Waiting for IRQs %s == %s ...\");", val0, val1);
               Printf(  "\nwait(((IRQ & 'h%s)=='h%s));\n\n", val0, val1);
         }
      }                  

      return TRUE;
}

#else

BOOL fgMaliWaitIRQ (CHAR *pline)
{
	CHAR line[256];
	CHAR val0[24], val1[24];
	CHAR str0[100],str1[100];
	INT32  r, value;
	UINT32 u4IrqCheck, u4IrqFlag;

	strcpy(line, pline);
	
	HalFlushInvalidateDCache();
	
	memset( (void*) (str0), 0, 100);
	memset( (void*) (str1), 0, 100);
	val0[0] = 0x0;
	r = sscanf(line, " wait %s %s %s", str0, str1, val0);
	if(strcmp(str1, "irq")==0)
	{
		Printf( "\nwait(mali_%s_risc==1);\n\n", str1);
		if(MaliWaitIrq())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		
	}
	else if(strcmp(str1, "clk")==0)
	{
		Printf( "\nrepeat(%s) @(%s mali_%s);\n\n", val0, str0, str1);
		value = 0;
		if ( val0[0] != 0x0)
		value = u4StrToInt(val0);

		if (value == 0)
		value = 20;

		x_thread_delay(value);
	}
	else if(strcmp(str0, "irq")==0)
	{ 

		r = sscanf(line, "wait irq %s %s\n\r", val0, val1);
		u4IrqCheck = u4StrToInt(val0);
		u4IrqFlag = u4StrToInt(val1);
		if(MaliWaitIrqWithMaskAndVal(u4IrqCheck,u4IrqFlag))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}

		if(r==1)
		{
			Printf(  "\n$display(\"Waiting for IRQ %s ...\");", val0);
			Printf(  "\nwait((IRQ & 'h%s));\n\n", val0);
		}
		else 
		if(r==2)
		{
			//Printf(  "\n$display(\"Waiting for IRQs %s == %s ...\");", val0, val1);
			//Printf(  "\nwait(((IRQ & 'h%s)=='h%s));\n\n", val0, val1);
		}
	}                  

	return TRUE;
}

#endif


INT32 i4Graphics3D_Drv_Init(void)
{
	// Set GRA3D clock to vdec pll
	//IO_WRITE32(0xF000D000, 0x314, 0xD);  
	#if 0
	//Gazelle FPGA 
	IO_WRITE32(IO_BASE, MALI_TOP_OFFSET + 0x14, 0xfc32ffff);   
	IO_WRITE32(IO_BASE, MALI_TOP_OFFSET + 0x1c, 0xfc32ffff);   
	IO_WRITE32(IO_BASE, MALI_TOP_OFFSET + 0x14, 0xfc3301ff);   
	IO_WRITE32(IO_BASE, MALI_TOP_OFFSET + 0x1c, 0xfc3301ff); 
	#endif
	//Printf("i4Graphics3D_Drv_Emu-1\r\n");

	/* G3D core clock selection */
    IO_WRITE32(IO_BASE, 0xd3d4, 0x00000001);//G3DPLL
	//Printf("i4Graphics3D_Drv_Emu-2\r\n");

#if TEST_G3D_INTERRUPT
	if(_hG3dIsrSema == 0)
	{
	
	    printf("TEST ISR=========\n");				  
		//VERIFY(x_reg_isr(MALI450_VECTOR_GFX3D_GP, _Gfx3dIsr, &_pfnOldIsr) == OSR_OK);
		INT32 i4Result = x_reg_isr(MALI450_VECTOR_GFX3D_GP, _Gfx3dIsr, &_pfnOldIsr);
		if(i4Result != OSR_OK)
		{
			printf("MALI450_VECTOR_GFX3D_GP NG i4Result is %d\n",i4Result);				  

		}
		else
		{	
		    x_sema_create(&_hG3dIsrSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK);
			printf("MALI450_VECTOR_GFX3D_GP OK\n");				  

		}
		//PP0
		if(x_reg_isr(MALI450_VECTOR_GFX3D_PP, _Gfx3dIsr, &_pfnOldIsr) != OSR_OK)
		{
			printf("MALI450_VECTOR_GFX3D_PP NG\n"); 			  
		
		}
		else
		{	
			x_sema_create(&_hG3dPpIsrSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK);
			printf("MALI450_VECTOR_GFX3D_PP OK\n"); 			  
		
		}
		//PP1
		if(x_reg_isr(MALI450_VECTOR_GFX3D_PP1, _Gfx3dIsr, &_pfnOldIsr) != OSR_OK)
		{
			printf("MALI450_VECTOR_GFX3D_PP1 NG\n"); 			  
		
		}
		else
		{	
			x_sema_create(&_hG3dPp1IsrSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK);
			printf("MALI450_VECTOR_GFX3D_PP1 OK\n"); 			  
		
		}
		//GP MMU
		if(x_reg_isr(MALI450_VECTOR_GFX3D_GPMMU, _Gfx3dIsr, &_pfnOldIsr) != OSR_OK) 
		{
			printf("MALI450_VECTOR_GFX3D_GPMMU NG\n");			  
		
		}
		else
		{	
			printf("MALI450_VECTOR_GFX3D_GPMMU OK\n");			  	
		}
		//PP0 MMU
		if(x_reg_isr(MALI450_VECTOR_GFX3D_PPMMU, _Gfx3dIsr, &_pfnOldIsr) != OSR_OK)
		{
			printf("MALI450_VECTOR_GFX3D_PPMMU NG\n");			  
		
		}
		else
		{	
			printf("MALI450_VECTOR_GFX3D_PPMMU OK\n");				
		}

 		//PP1 MMU
		if(x_reg_isr(MALI450_VECTOR_GFX3D_PP1MMU, _Gfx3dIsr, &_pfnOldIsr) != OSR_OK) 
		{
			printf("MALI450_VECTOR_GFX3D_PP1MMU NG\n");			  
		
		}
		else
		{	
			printf("MALI450_VECTOR_GFX3D_PP1MMU OK\n");				
		}

		#if 0
		VERIFY(x_reg_isr(MALI450_VECTOR_GFX3D_PP, _Gfx3dIsr, &_pfnOldIsr) == OSR_OK);
		VERIFY(x_reg_isr(MALI450_VECTOR_GFX3D_GPMMU, _Gfx3dIsr, &_pfnOldIsr) == OSR_OK);	 
		VERIFY(x_reg_isr(MALI450_VECTOR_GFX3D_PPMMU, _Gfx3dIsr, &_pfnOldIsr) == OSR_OK);
		VERIFY(x_reg_isr(MALI450_VECTOR_GFX3D_PP1, _Gfx3dIsr, &_pfnOldIsr) == OSR_OK);
		VERIFY(x_reg_isr(MALI450_VECTOR_GFX3D_PP1MMU, _Gfx3dIsr, &_pfnOldIsr) == OSR_OK); 
		x_sema_create(&_hG3dPp1IsrSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK);  
		x_sema_create(&_hG3dIsrSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK);
		x_sema_create(&_hG3dPpIsrSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK);	 		
        #endif
		
	}
#endif

	return 0;
}

INT32 i4Graphics3D_Drv_Uninit(void)
{
   return 0;
}

INT32 i4Graphics3D_Drv_Stop( UINT32              u4CmdBufHandle)
{
   return 0;
}


UINT32 M2P(UINT32 u4Mali)
{
    if(eG3dEmuConfig.fgIsMaliPPTest)
    {
    	if(eG3dEmuConfig.fgIsHexTypeGP)
    	{
			return u4Mali + (INT32) (PHYSICAL( (UINT32) pGPPhyMemBuff));
			eG3dEmuConfig.fgIsHexTypeGP = FALSE;
    	}
		else
		{
			return u4Mali + (INT32) (PHYSICAL( (UINT32) pPPPhyMemBuff));  	
		}
		//return u4Mali + (INT32) ( (UINT32) pPPPhyMemBuff);  		
    }
    else
    {  
		//fgIsMaliGPTest and benchmark and MP pattern use GP MMU table 
		//benchmark pattern, GP and PP mapping with the same table
		//return u4Mali + (INT32)( (UINT32) pGPPhyMemBuff);	
    	if(eG3dEmuConfig.fgIsHexTypePP)
    	{
			return u4Mali + (INT32) (PHYSICAL( (UINT32) pPPPhyMemBuff));
			eG3dEmuConfig.fgIsHexTypePP = FALSE;
    	}
		else
		{
			return u4Mali + (INT32) (PHYSICAL( (UINT32) pGPPhyMemBuff)); 
		}		
    }
}

#if G3D_SUPPORT_INTERNAL_MMU

//DTE, PTE must be Physical addr for GPU to access
BOOL InitMaliGPMMU(UINT32 u4DTE, UINT32 u4PTE, UINT32 u4MemBase, UINT32 u4MemSize)
{
    UINT32 i;
    vMaliWriteReg(0x3008, 0x1);      
    vMaliWriteReg(0x3000, u4DTE + NON_CACHEABLE_DRAM_OFFSET);
    x_memset((void*)((VIRTUAL)(u4DTE) + NON_CACHEABLE_DRAM_OFFSET), 0, 4096);
    for(i = 0 ; i < (u4MemSize - 1) / MMU_UNIT + 1; i++)
    {
        *((UINT32*)(VIRTUAL)(u4DTE) + i + NON_CACHEABLE_DRAM_OFFSET) = ((u4PTE + NON_CACHEABLE_DRAM_OFFSET) + (i * 0x1000)) | 0x1;
    }
    for(i = 0; i < 1024 * ((u4MemSize - 1) / MMU_UNIT + 1); i++)
    {
        *((UINT32*)(VIRTUAL)(u4PTE) + i + NON_CACHEABLE_DRAM_OFFSET) = ((u4MemBase + NON_CACHEABLE_DRAM_OFFSET) + (i * 0x1000)) | 0x7;
        // for deal with 0x1100000 only
        /*
        if(((u4MemBase + (i * 0x1000)) | 0x7) > 0x10000000)
        {
            *((UINT32*)u4PTE + i) = ((u4MemBase + (i * 0x1000)) | 0x7) + 0x800000 - 0x10000000;
        }  
        */
    }
    vMaliWriteReg(0x3008, 0x0);    
    return TRUE;
}

BOOL InitMaliPPMMU(UINT32 u4DTE, UINT32 u4PTE, UINT32 u4MemBase, UINT32 u4MemSize)
{
    UINT32 i;
    vMaliWriteReg(0x4008, 0x1);      
    vMaliWriteReg(0x4000, (u4DTE + NON_CACHEABLE_DRAM_OFFSET));
    x_memset((void*)((VIRTUAL)(u4DTE) + NON_CACHEABLE_DRAM_OFFSET), 0, 4096);
    for(i = 0 ; i < (u4MemSize - 1) / MMU_UNIT + 1; i++)
    {
        *((UINT32*)(VIRTUAL)(u4DTE) + i + NON_CACHEABLE_DRAM_OFFSET) = ((u4PTE + NON_CACHEABLE_DRAM_OFFSET) + (i * 0x1000)) | 0x1;
    }
    for(i = 0; i < 1024 * ((u4MemSize - 1) / MMU_UNIT + 1); i++)
    {
        *((UINT32*)(VIRTUAL)(u4PTE) + i + NON_CACHEABLE_DRAM_OFFSET) = ((u4MemBase + NON_CACHEABLE_DRAM_OFFSET) + (i * 0x1000)) | 0x7;
        // for deal with 0x1100000 only
        /*
        if(((u4MemBase + (i * 0x1000)) | 0x7) > 0x10000000)
        {
            *((UINT32*)u4PTE + i) = ((u4MemBase + (i * 0x1000)) | 0x7) + 0x800000 - 0x10000000;
        }  
        */
    }
    vMaliWriteReg(0x4008, 0x0);    
    return TRUE;
}


BOOL InitMaliPP1MMU(UINT32 u4DTE, UINT32 u4PTE, UINT32 u4MemBase, UINT32 u4MemSize)
{
    UINT32 i;
    vMaliWriteReg(0x5008, 0x1);      
    vMaliWriteReg(0x5000, (u4DTE + NON_CACHEABLE_DRAM_OFFSET));
    x_memset((void*)((VIRTUAL)(u4DTE) + NON_CACHEABLE_DRAM_OFFSET), 0, 4096);
    for(i = 0 ; i < (u4MemSize - 1) / MMU_UNIT + 1; i++)
    {
        *((UINT32*)(VIRTUAL)(u4DTE) + i + NON_CACHEABLE_DRAM_OFFSET) = ((u4PTE + NON_CACHEABLE_DRAM_OFFSET) + (i * 0x1000)) | 0x1;
    }
    for(i = 0; i < 1024 * ((u4MemSize - 1) / MMU_UNIT + 1); i++)
    {
        *((UINT32*)(VIRTUAL)(u4PTE) + i + NON_CACHEABLE_DRAM_OFFSET) = ((u4MemBase + NON_CACHEABLE_DRAM_OFFSET) + (i * 0x1000)) | 0x7;
        // for deal with 0x1100000 only
        /*
        if(((u4MemBase + (i * 0x1000)) | 0x7) > 0x10000000)
        {
            *((UINT32*)u4PTE + i) = ((u4MemBase + (i * 0x1000)) | 0x7) + 0x800000 - 0x10000000;
        }  
        */
    }
    vMaliWriteReg(0x5008, 0x0);    
    return TRUE;
}


void ResetInitGPMMU(void)
{

	u4GPDTE = (UINT32)pGPDTEBuffer ;
  	printf("u4GPDTE : 0x%x!!\n", u4GPDTE );	

	/* GP MMU*/
	//writereg 02000008 00000006
	vMaliWriteReg(MALI400_REG_BASE_MMU_GP+8, 0x00000006); //force reset
	//writereg 02000008 00000001
	vMaliWriteReg(MALI400_REG_BASE_MMU_GP+8, 0x00000001); //MMU DISABLE PAGING
	//writereg 02000000 0084a000
	vMaliWriteReg(MALI400_REG_BASE_MMU_GP+0, (PHYSICAL)(u4GPDTE)); 
	//writereg 02000008 00000004
	vMaliWriteReg(MALI400_REG_BASE_MMU_GP+8, 0x4); //zap the entire page table cache
	//writereg 02000008 00000000
	vMaliWriteReg(MALI400_REG_BASE_MMU_GP+8, 0x0); //MMU ENABLE PAGING

	InitMaliGPMMU(u4GPDTE, u4GPDTE + 4096, (UINT32)pGPPhyMemBuff, _GP_PHYSICAK_SIZE);

}

void ResetInitPPMMU(void)
{
	u4PPDTE = (UINT32)pPPDTEBuffer;
  	printf("u4PPDTE : 0x%x!!\n", u4PPDTE );	

	/*PP MMU */
	//writereg 12000008 00000006	
	//vMaliWriteReg(MALI400_REG_BASE_MMU_PP0 + 8, 6);
	//writereg 12000008 00000001
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP0 + 8, 1);
	//writereg 12000000 0084a000
	if(eG3dEmuConfig.fgIsMaliPPTest ||
	   eG3dEmuConfig.fgIsMaliGPTest)
	{
	    vMaliWriteReg(MALI400_REG_BASE_MMU_PP0 + 0, (PHYSICAL)(u4PPDTE));
	}
	else
	{
	    vMaliWriteReg(MALI400_REG_BASE_MMU_PP0 + 0, (PHYSICAL)(u4GPDTE));
	}
	//writereg 12000008 00000004
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP0 + 8, 4);
	//writereg 12000008 00000000
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP0 + 8, 0);
	//# MMU end	
	if(eG3dEmuConfig.fgIsMaliPPTest ||
	   eG3dEmuConfig.fgIsMaliGPTest)
	{
	    InitMaliPPMMU(u4PPDTE, u4PPDTE + 4096, (UINT32)pPPPhyMemBuff, _PP_PHYSICAK_SIZE);   
	}
	else
	{  
	    /* for benchmark pattern PP use the same MMU table as GP used */
	    InitMaliPPMMU(u4GPDTE, u4GPDTE + 4096, (UINT32)pGPPhyMemBuff, _GP_PHYSICAK_SIZE); 
	}
	
}

#if G3D_SUPPORT_MP
void ResetInitPP1MMU(void)
{
	u4PP1DTE = (UINT32)pPPDTEBuffer ;
  	printf("u4PP1DTE : 0x%x!!\n", u4PP1DTE );	
	
	/*PP MMU */
	//writereg 12000008 00000006	
	//vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 8, 6);
	//writereg 12000008 00000001
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 8, 1);
	//writereg 12000000 0084a000
	//vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 0, u4PP1DTE);
	if(eG3dEmuConfig.fgIsMaliPPTest ||
	   eG3dEmuConfig.fgIsMaliGPTest)
	{
	    vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 0, u4PPDTE);
	}
	else
	{
	    vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 0, u4GPDTE);
	}
	//writereg 12000008 00000004
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 8, 4);
	//writereg 12000008 00000000
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 8, 0);
	//# MMU end	
	if(eG3dEmuConfig.fgIsMaliPPTest ||
	   eG3dEmuConfig.fgIsMaliGPTest)
	{
	    InitMaliPPMMU(u4PP1DTE, u4PP1DTE + 4096, (UINT32)pPPPhyMemBuff, _PP_PHYSICAK_SIZE);   
	}
	else
	{
	    /* for benchmark pattern PP use the same MMU table as GP used */
	    InitMaliPPMMU(u4GPDTE, u4GPDTE + 4096, (UINT32)pGPPhyMemBuff, _GP_PHYSICAK_SIZE); 
	}
}

#if G3D_SUPPORT_MALIMP4
void ResetInitPP2MMU(void)
{
	u4PP2DTE = (UINT32)pPPDTEBuffer ;
  	printf("u4PP2DTE : 0x%x!!\n", u4PP2DTE );	
	
	/*PP MMU */
	//writereg 12000008 00000006	
	//vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 8, 6);
	//writereg 12000008 00000001
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP2 + 8, 1);
	//writereg 12000000 0084a000
	//vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 0, u4PP1DTE);
	if(eG3dEmuConfig.fgIsMaliPPTest ||
	   eG3dEmuConfig.fgIsMaliGPTest)
	{
	    vMaliWriteReg(MALI400_REG_BASE_MMU_PP2 + 0, u4PP2DTE);
	}
	else
	{
	    vMaliWriteReg(MALI400_REG_BASE_MMU_PP2 + 0, u4GPDTE);
	}
	//writereg 12000008 00000004
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP2 + 8, 4);
	//writereg 12000008 00000000
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP2 + 8, 0);
	//# MMU end	
	if(eG3dEmuConfig.fgIsMaliPPTest ||
	   eG3dEmuConfig.fgIsMaliGPTest)
	{
	    InitMaliPPMMU(u4PP2DTE, u4PP2DTE + 4096, (UINT32)pPPPhyMemBuff, _PP_PHYSICAK_SIZE);   
	}
	else
	{
	    /* for benchmark pattern PP use the same MMU table as GP used */
	    InitMaliPPMMU(u4GPDTE, u4GPDTE + 4096, (UINT32)pGPPhyMemBuff, _GP_PHYSICAK_SIZE); 
	}
}

void ResetInitPP3MMU(void)
{
	u4PP3DTE = (UINT32)pPPDTEBuffer ;
  	printf("u4PP3DTE : 0x%x!!\n", u4PP3DTE );	
	
	/*PP MMU */
	//writereg 12000008 00000006	
	//vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 8, 6);
	//writereg 12000008 00000001
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP3 + 8, 1);
	//writereg 12000000 0084a000
	//vMaliWriteReg(MALI400_REG_BASE_MMU_PP1 + 0, u4PP1DTE);
	if(eG3dEmuConfig.fgIsMaliPPTest ||
	   eG3dEmuConfig.fgIsMaliGPTest)
	{
	    vMaliWriteReg(MALI400_REG_BASE_MMU_PP3 + 0, u4PP2DTE);
	}
	else
	{
	    vMaliWriteReg(MALI400_REG_BASE_MMU_PP3 + 0, u4GPDTE);
	}
	//writereg 12000008 00000004
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP3 + 8, 4);
	//writereg 12000008 00000000
	vMaliWriteReg(MALI400_REG_BASE_MMU_PP3 + 8, 0);
	//# MMU end	
	if(eG3dEmuConfig.fgIsMaliPPTest ||
	   eG3dEmuConfig.fgIsMaliGPTest)
	{
	    InitMaliPPMMU(u4PP3DTE, u4PP3DTE + 4096, (UINT32)pPPPhyMemBuff, _PP_PHYSICAK_SIZE);   
	}
	else
	{
	    /* for benchmark pattern PP use the same MMU table as GP used */
	    InitMaliPPMMU(u4GPDTE, u4GPDTE + 4096, (UINT32)pGPPhyMemBuff, _GP_PHYSICAK_SIZE); 
	}
}
#endif //G3D_SUPPORT_MALIMP4

#endif //G3D_SUPPORT_MP

#endif //G3D_SUPPORT_INTERNAL_MMU

UINT32 u4ConfigAddToRealAddr(UINT32 value)
{
    UINT32 new_adr0 = 0;

    if( (value>>16) == 0 )           new_adr0 = value & 0xFFF;
    else if( (value>>16) == 0x0100 ) new_adr0 = 0x1000 + (value & 0xFFF );
    else if( (value>>16) == 0x0101 ) new_adr0 = 0x10000 + (value & 0xFFF );
    else if( (value>>16) == 0x0102 ) new_adr0 = 0x11000 + (value & 0xFFF );

    else if( (value>>16) == 0x0200 ) new_adr0 = 0x3000 + (value & 0xFFF );
	
    else if( (value>>16) == 0x2000 ) new_adr0 = 0x2000 + (value & 0xFFF );
    else if( (value>>16) == 0x1000 ) new_adr0 = 0x8000 + (value & 0x1FFF);
    else if( (value>>16) == 0x1001 ) new_adr0 = 0xA000 + (value & 0x1FFF);
    else if( (value>>16) == 0x1002 ) new_adr0 = 0xC000 + (value & 0x1FFF);
    else if( (value>>16) == 0x1003 ) new_adr0 = 0xE000 + (value & 0x1FFF);
	
    else if( (value>>16) == 0x1004 ) new_adr0 = 0x28000 + (value & 0x1FFF);
    else if( (value>>16) == 0x1005 ) new_adr0 = 0x2A000 + (value & 0x1FFF);
    else if( (value>>16) == 0x1006 ) new_adr0 = 0x2C000 + (value & 0x1FFF);
    else if( (value>>16) == 0x1007 ) new_adr0 = 0x2E000 + (value & 0x1FFF);
	
    else if( (value>>16) == 0x0200 ) new_adr0 = 0x3000 + (value & 0xFFF );
    else if( (value>>16) == 0x1200 ) new_adr0 = 0x4000 + (value & 0xFFF );
    else if( (value>>16) == 0x1201 ) new_adr0 = 0x5000 + (value & 0xFFF );
    else if( (value>>16) == 0x1202 ) new_adr0 = 0x6000 + (value & 0xFFF );
    else if( (value>>16) == 0x1203 ) new_adr0 = 0x7000 + (value & 0xFFF );
    else if( (value>>16) == 0x1204 ) new_adr0 = 0x1C000 + (value & 0xFFF );
    else if( (value>>16) == 0x1205 ) new_adr0 = 0x1D000 + (value & 0xFFF );
    else if( (value>>16) == 0x1206 ) new_adr0 = 0x1E000 + (value & 0xFFF );
    else if( (value>>16) == 0x1207 ) new_adr0 = 0x1F000 + (value & 0xFFF );

    else if( (value>>16) == 0x8000|| (value>>16) == 0x8001 ||(value>>16) == 0x8002) new_adr0 = (value & 0x3FFFF );

    return new_adr0;
}

/* and "check_hex pp" in config.txt before load_mem or dump_mem of PP hex file for some MP pattern */
static void MaliCheckHexTypeProc(CHAR *pline)
{
	CHAR line[256];
	CHAR str0[100];
	INT32  r;	
  	printf("MaliCheckHexProc\n");	
	strcpy(line, pline);
	memset( (void*) (str0), 0, 100);

	r = sscanf(line, "check_hex %s\n\r", str0);
	
  	Printf("check hex: str0=%s\n",str0);	
	if(strcmp(str0, "pp")==0)
	{
        eG3dEmuConfig.fgIsHexTypePP = TRUE;
	}
	else if(strcmp(str0, "gp")==0)
	{
        eG3dEmuConfig.fgIsHexTypeGP = TRUE;
	}
	
	return;

}

static void MaliCheckRegProc(CHAR *pline)
{
	CHAR line[256];
	CHAR val0[24];
	CHAR adr0[100],str1[100];
	INT32  r, value;
	INT32 new_adr,reg_read,regval;
	
  	printf("MaliCheckRegProc\n");	

	strcpy(line, pline);

	memset( (void*) (adr0), 0, 100);
	memset( (void*) (str1), 0, 100);
	memset( (void*) (val0), 0, 23);

	val0[0] = 0x0;
	r = sscanf(line, "check_reg %s %s %s\n\r", adr0,str1,val0);
	
  	Printf("adr0=%s,str1=%s,val0=%s\n",adr0,str1,val0);	
	if(strcmp(str1, "=")==0)
	{
	
		value = u4StrToInt(adr0);
		new_adr = u4ConfigAddToRealAddr(value);
		regval = u4StrToInt(val0); 	
		reg_read = u4MaliReadReg(new_adr);
		if(reg_read!= regval)
		{
			//comparing result not refer 0x2c INT status register,
			if(0x2c != new_adr)
			{
				eG3dEmuResult.eCmpRegResult = G3D_CMP_STATUS_FAIL;
			}
		}
		else
		{
			eG3dEmuResult.eCmpRegResult = G3D_CMP_STATUS_SUCCESS;
		}
	}
	return;

}



BOOL fgG3DBufGetLine(CHAR *pBuf, UINT32 *pu4PrsSz, CHAR *pline, UINT32 u4RealSz)
{
    CHAR *pucLine;
    CHAR ucCurr;
    UINT32 idx;
    
    while(*pu4PrsSz < u4RealSz) 
    {  
        pucLine = ((CHAR*) pBuf) + (*pu4PrsSz);
        for (idx = 0; idx < 256; idx++)
        {
            ucCurr = *pucLine;
            if (ucCurr == '\n')
            {
                (*pu4PrsSz) ++;
                break;
            }

            (*pline) = ucCurr;
            (*pu4PrsSz) ++;
            pucLine++;
            pline++;
        }
        (*pline) = '\0';
        //Printf("line is : %s\n", pline);
        return TRUE;
    }
    return FALSE;
}

BOOL  fgIsValueEqual(UINT32 golden, UINT32 result, BOOL *fgCheck)
{
    UINT32 g0, g1, g2, g3, g4, g5, g6, g7;
    UINT32 r0,  r1, r2,  r3, r4,  r5, r6,  r7;

    if (golden != result)
    {
        g0 = (golden >> 28) & 0xf;
        g1 = (golden >> 24) & 0xf;
        g2 = (golden >> 20) & 0xf;
        g3 = (golden >> 16) & 0xf;
        g4 = (golden >> 12) & 0xf;
        g5 = (golden >>  8) & 0xf;
        g6 = (golden >>  4) & 0xf;
        g7 = (golden         ) & 0xf;   

        r0 = (result >> 28) & 0xf;
        r1 = (result >> 24) & 0xf;
        r2 = (result >> 20) & 0xf;
        r3 = (result >> 16) & 0xf;
        r4 = (result >> 12) & 0xf;
        r5 = (result >>  8) & 0xf;
        r6 = (result >>  4) & 0xf;
        r7 = (result         ) & 0xf;

        if (
          (  fgCheck[0]==TRUE && g0 != r0)  ||
          (  fgCheck[1]==TRUE && g1 != r1)  ||
          (  fgCheck[2]==TRUE && g2 != r2)  ||
          (  fgCheck[3]==TRUE && g3 != r3)  ||
          (  fgCheck[4]==TRUE && g4 != r4)  ||
          (  fgCheck[5]==TRUE && g5 != r5)  ||
          (  fgCheck[6]==TRUE && g6 != r6)  ||
          (  fgCheck[7]==TRUE && g7 != r7)  )
        {
           return FALSE;
        }

        return TRUE;
    }

     return TRUE;
}

UINT32  u4DiffMemory(UINT32 golden, UINT32 result, BOOL *fgCheck)
{
    UINT32 g0, g1, g2, g3, g4, g5, g6, g7;
    UINT32 r0,  r1, r2,  r3, r4,  r5, r6,  r7;
    UINT32 u4D, u4Temp;

    if (golden != result)
    {
        g0 = (golden >> 28) & 0xf;
        g1 = (golden >> 24) & 0xf;
        g2 = (golden >> 20) & 0xf;
        g3 = (golden >> 16) & 0xf;
        g4 = (golden >> 12) & 0xf;
        g5 = (golden >>  8) & 0xf;
        g6 = (golden >>  4) & 0xf;
        g7 = (golden         ) & 0xf;   

        r0 = (result >> 28) & 0xf;
        r1 = (result >> 24) & 0xf;
        r2 = (result >> 20) & 0xf;
        r3 = (result >> 16) & 0xf;
        r4 = (result >> 12) & 0xf;
        r5 = (result >>  8) & 0xf;
        r6 = (result >>  4) & 0xf;
        r7 = (result         ) & 0xf;


       u4D = 0;
       if  (  fgCheck[0]==TRUE && g0 != r0) 
       {             
            u4Temp = (g0 > r0)? (g0-r0) : (r0-g0);
            u4D = u4Temp << 4;
       }       
       if  (  fgCheck[1]==TRUE && g1 != r1)
       {
           u4Temp = (g1 > r1)? (g1-r1) : (r1-g1);
           u4D += u4Temp;
       }

       if (u4D > 10)
       {
          Printf("This is diff is large: %d\n", u4D);
       }
       if (u4D > u4MaxDiff)
       {
           Printf("This is Max diff : %d\n", u4D);
           u4MaxDiff = u4D;
       }

       u4D = 0;
       if   (  fgCheck[2]==TRUE && g2 != r2)
       {
                u4Temp = (g2 > r2)? (g2-r2) : (r2-g2);
            u4D = u4Temp << 4;
       }
       if   (  fgCheck[3]==TRUE && g3 != r3)
       {
           u4Temp = (g3 > r3)? (g3-r3) : (r3-g3);
           u4D += u4Temp;       
       }

       if (u4D > 10)
       {
          Printf("This is diff is large: %d\n", u4D);
       }
       if (u4D > u4MaxDiff)
       {
           Printf("This is Max diff : %d\n", u4D);
           u4MaxDiff = u4D;
       }


       u4D = 0;
       if   (  fgCheck[4]==TRUE && g4 != r4)
       {
           u4Temp = (g4 > r4)? (g4-r4) : (r4-g4);
            u4D = u4Temp << 4;
       }
       if   (  fgCheck[5]==TRUE && g5 != r5)
       {
           u4Temp = (g5 > r5)? (g5-r5) : (r5-g5);
           u4D += u4Temp;       
       }
       if (u4D > 10)
       {
          Printf("This is diff is large: %d\n", u4D);
       }
       if (u4D > u4MaxDiff)
       {
           Printf("This is Max diff : %d\n", u4D);
           u4MaxDiff = u4D;
       }

       u4D = 0;
       if   (  fgCheck[6]==TRUE && g6 != r6)
       {
           u4Temp = (g6 > r6)? (g6-r6) : (r6-g6);
            u4D = u4Temp << 4;
       }
       if   (  fgCheck[7]==TRUE && g7 != r7)  
       {
           u4Temp = (g7 > r7)? (g7-r7) : (r7-g7);
           u4D += u4Temp;       
       }
       if (u4D > 10)
       {
          Printf("This is diff is large: %d\n", u4D);
       }
       if (u4D > u4MaxDiff)
       {
           Printf("This is Max diff : %d\n", u4D);
           u4MaxDiff = u4D;
       }
       
    }

     return TRUE;
}

INT32 vG3DEmuParseMemHex (CHAR *pMemName, UINT32 u4Offset, INT32 hex_action, UINT32 u4RdCount, UINT32 u4CompCnt, UINT32 u4CompAddr)
{
    CHAR str0 [256];
    CHAR mem_addr [256];
    UINT32 value;
    CHAR val0 [10];
    UINT32  r, i, j;
    CHAR *pMemBuf = NULL;
    INT32  iFd;
    UINT32 u4PrsSz = 0;
    UINT32 u4RealMemSize;
    UINT32 u4MemLength;
    UINT32 u4MemSz;
    CHAR line [256];    
    BOOL   fgCheck[8];
    UINT32 addr =0; 

	//Enzhu Added Begin
	UINT32 BeginAddr = 0;
	UINT32 ui4Idx = 0 ;
    //Enzhu Added End
	
    UINT32 reg_addr;
    BOOL fgret;
    UINT32 cnt = 0;
    UINT32 mem_value = 0;
    INT32  u4SkipLine = u4RdCount / 16;
    INT32  u4CompLine = u4CompCnt / 16;
    BOOL   fgOpenSuccess = FALSE;

    BOOL    fgCmpSuccess = TRUE;
    strcpy(str0, pMemName);
    Printf("pMemName:\n%s\n", str0);


    pMemBuf = (CHAR*)  (pHexFileBuffer);

    if (pMemBuf == NULL)
    {
       Printf("[WARNING] Allocate Memory for MemHex File Fail!!!!\n ");
       //ASSERT(0);
       //return FALSE;
       return G3D_CMP_STATUS_OPEN_FAIL;
    }

	//x_memset(pMemBuf, 0x0, _HEX_FILE_SIZE);

#if T32_HOST
	u4RealMemSize= _HEX_FILE_SIZE;
	u4MemLength = _HEX_FILE_SIZE;
	fgOpenSuccess = fgGPUReadPCFile((void*) pMemBuf, _HEX_FILE_SIZE, pMemName, 0, &u4RealMemSize, &u4MemLength);
	//u4RealMemSize = u4MemLength = x_strlen(pMemName);    
#elif USB_G3D_VFY   
	fgOpenSuccess = fgReadFile((void*) pMemBuf, _HEX_FILE_SIZE, pMemName, 0, &iFd, &u4RealMemSize, &u4MemLength);
#else
	fgOpenSuccess = fgG3DFATSFsReadFile(pMemName,(void*)pMemBuf,0,_HEX_FILE_SIZE,&u4RealMemSize,&u4MemLength,&iFd);
#endif
	
    if (fgOpenSuccess == FALSE)
    {
       Printf("[WARNING] Open MemHex Fail: %s\n ", pMemName);
       return G3D_CMP_STATUS_OPEN_FAIL;
    }

	HalFlushInvalidateDCache();

    switch(hex_action)
    {
              case HEX_LOAD:
                  Printf("LoadMem: \n");
                  break;
              case HEX_COMP_MEM:
                  Printf("CompMem: \n");                
                  break;
              case HEX_COMP_REG:
                    reg_addr = u4ConfigAddToRealAddr(addr);
                  Printf("CompReg: \n");
                  break;

              default:
                  break;
    }
        
    u4MemSz = 0;

	
	//printf("u4CompLine= %d,u4PrsSz=%d,u4RealMemSize=%d\n",u4CompLine,u4PrsSz,u4RealMemSize);
    while( (u4PrsSz < u4RealMemSize) && (u4CompLine >0) ) 
    {                
        fgret = fgG3DBufGetLine(pMemBuf, &u4PrsSz, line, u4RealMemSize);
        if (!fgret)
        {
			printf("fgG3DBufGetLine line = %s \n",line);
            break;
        }
		
                            
        if (line[0] == '#')
        {
            //This is comment line. Do nothing.
        }
        else
        //if (line[0] == '0')
        {
            u4CompLine--;
            
            if (u4SkipLine > 0)
            {
                u4SkipLine --;
            }
            else
            {
            
            //Get Addr
            for (i=0; i < strlen(line); i++)
            {
                if (line[i] != ':' && line[i] != ' ')
                {
                    mem_addr[i] = line[i];
                }
                else
                {
                    mem_addr[i] = '\0';
                    break;
                }
            }

            addr = u4StrToInt(mem_addr);
            addr = addr + u4Offset;

			//Enzhu Added Begin
			ui4Idx++;
			if(1 == ui4Idx )
			{
				BeginAddr = addr;	
				Printf("HEX_LOAD_MEM: BeginAddr = 0x%08x \n",BeginAddr);
			}
			//Enzhu Added End
			
            while(line[i] == ':' || line[i] == ' ')
            {        
                i++;
            }                        
            
            cnt = 0;
            for (j=i; j < strlen(line); )
            {
              for (r=0; r < 8; r++)
              {
                 //0x2E(".")  0x58("X")  0x78("x")
                 if (line[j] != 0x2E && line[j] != 0x58 && line[j] !=0x78)
                 {
                     val0[r] = line[j];
                     fgCheck[r] = TRUE;
                 }
                 else
                 {
                    val0[r] = '0';
                    fgCheck[r] = FALSE;
                 }
                 
                j++;
              }
              val0[8] = '\0';
              value = u4StrToInt(val0);
              //Printf("0x%08x ", value);              

              switch(hex_action)
              {
              case HEX_LOAD:                  
                  if ( (cnt == 0) &&  ((addr &0x3FFFFF) == 0x0) )
                  {
                      Printf("HEX_LOAD_MEM: Mem[0x%08x] = 0x%08x \n", addr + (cnt*4), value);
                  }
				  
                  #if G3D_SUPPORT_INTERNAL_MMU           
           	      vMaliWriteMem(M2P(addr + (cnt*4)), value);
				  #else
				  vMaliWriteMem((addr + (cnt*4)), value);
				  #endif
				  
                  break;
              case HEX_COMP_MEM:
			  	
                  if ( (u4CompAddr == 0x0) ||
                       (u4CompAddr != 0x0 &&  ( (addr + (cnt*4)) >= u4CompAddr ) ) )
                  {                      
					  
                      #if G3D_SUPPORT_INTERNAL_MMU		   
                      mem_value = u4MaliReadMem(M2P(addr + (cnt*4)));
					  #else				  
                      mem_value = u4MaliReadMem((addr) + (cnt*4));
				      #endif
					  
                      if (fgIsValueEqual (value, mem_value, fgCheck)  != TRUE)
                      {
                          Printf("[WARNING] CompMem Mismatch: [Addr]0x%08x : [Golden]0x%08x -- [Read]0x%08x\n", addr+ (cnt*4), value, mem_value);
                          //u4DiffMemory(value, mem_value, fgCheck);
                          fgCmpSuccess = FALSE;                      
                          u4CompLine = 0;
						  /* print all mismatch addr*/
                          //break;
                      }
					  else
					  {
						  if ( (cnt == 0) &&  ((addr &0xFFFFF) == 0x0) )
						  {
							  Printf("[Addr]0x%08x][Golden]0x%08x vs. Read[0x%08x] \n",addr+ (cnt*4),value,mem_value);					
						  }

					  }
                  }
                  else
                  {
                      //Skip Comp Mem
                      if ( (cnt == 0) &&  ((addr &0x3FFFF) == 0x0) )
                      {
                          Printf("HEX_COMP_MEM: Mem[0x%08x] : Skip\n", addr + (cnt*4));
                      }
                      else
                      if ( (cnt == 0) &&  ((addr &0xFFF) == 0x0) )
                      {
                         Printf("HEX_COMP_MEM: .. : Skip\n", addr + (cnt*4));
                      }
                  }
                  break;
              case HEX_COMP_REG:                
                  reg_addr = u4ConfigAddToRealAddr(addr);

                  if ( (cnt == 0) &&  ((reg_addr &0x3FFFF) == 0x0) )
                  {
                      Printf("HEX_COMP_REG: Reg[0x%08x] = ", reg_addr + (cnt*4));
                  }
                  else
                  if ( (cnt == 0) &&  ((addr &0xFFF) == 0x0) )
                  {
                      Printf("HEX_COMP_REG: ..\n");
                  }

                  mem_value = u4MaliReadReg(reg_addr + (cnt*4));
                  
                  if ( (cnt == 0) &&  ((reg_addr &0x3FFFF) == 0x0) )
                  {
                      Printf("0x%08x vs. gold = [0x%08x] \n", mem_value, value);      
                  }

                  if (fgIsValueEqual (value, mem_value, fgCheck)  != TRUE)
                  {
                      Printf("[WARNING] CompReg Mismatch: [Addr]0x%08x : [Golden]0x%08x -- [Read]0x%08x\n", addr+ (cnt*4), value, mem_value);

                      fgCmpSuccess = FALSE;
                      //ASSERT(0);
                      //return FALSE;
                  }
                  break;

              default:
                  break;
              }
              cnt++;

              if (cnt==4)
              {
                  break;
              }

              while((line[j] == ':' || line[j] == ' ') && j < strlen(line) )
              {        
                j++;
              }
            }        
            //Printf("\n");            
            }
        }                                            
    } //     

    //Enzhu Added Begin
    if(hex_action == HEX_LOAD)
    {	
		HalFlushInvalidateDCache();
	}
    //Enzhu Added End

    if ((hex_action == HEX_COMP_MEM) || (hex_action == HEX_COMP_REG))
    {
        if (fgCmpSuccess)
        {
            Printf("Comapre Finish : HEX_LAST_ADR: ADDR= 0x%08x\n", addr); 
            return G3D_CMP_STATUS_SUCCESS;
        }
        else
        {           
            Printf("Comapre Fail : HEX_LAST_ADR: ADDR= 0x%08x\n", addr); 
            return G3D_CMP_STATUS_FAIL;
        }
    }

    return G3D_CMP_STATUS_UNKNOW;
}

BOOL fgRealChar(char p)
{
    if (p == 0x5F || p == 0x2E 
        || (p >= 0x30 && p <= 0x39)
        || (p >= 0x41 && p <= 0x5A)
        || (p >= 0x61 && p <= 0x7A) )
    {
        return TRUE;
    }

    return FALSE;
}

BOOL vG3DEmuParseMakefile (CHAR *pMakeName, CHAR *dumpname, INT32 u4CompType)
{
    CHAR str0 [256];    
    UINT32 i, j;
    CHAR *pMakeBuf = NULL;
    INT32  iFd;
    UINT32 u4PrsSz = 0;
    UINT32 u4RealSize;
    UINT32 u4Length;
    UINT32 u4Sz;
    CHAR line [256];        
    BOOL fgret;
    BOOL     fgDiffDump = FALSE;
    BOOL     fgDiffResult = FALSE;
    BOOL     fgKeepSearch = TRUE;
    INT32 i4StartIdx = 0;
    BOOL  fgOpenSuccess = FALSE;

    cmpcnt = 0;
    goldcnt = 0;    


    strcpy(str0, pMakeName);
    Printf("%s\n", str0);

    pMakeBuf = (CHAR*) (pMakeFileBuffer);	
	
	Printf("pMakeBuf: 0x%08x\n _MAKE_FILE_SIZE_=%d\n ", pMakeBuf,_MAKE_FILE_SIZE_);

    if (pMakeBuf == NULL)
    {
       Printf("[WARNING] Allocate Memory for Make File Fail: %s\n ", pMakeName);
       //ASSERT(0);
       return FALSE;
    }
	
#if T32_HOST
	x_memset(pMakeBuf, (int)'\0', _MAKE_FILE_SIZE_);
	u4RealSize= _MAKE_FILE_SIZE_;
	u4Length = _MAKE_FILE_SIZE_; 
	fgOpenSuccess = fgGPUReadPCFile((void*) pMakeBuf, _MAKE_FILE_SIZE_, pMakeName, 0, &u4RealSize, &u4Length);		
#elif USB_G3D_VFY   
	fgOpenSuccess = fgReadFile((void*) pMakeBuf, _MAKE_FILE_SIZE_, pMakeName, 0, &iFd, &u4RealSize, &u4Length);
#else
	fgOpenSuccess = fgG3DFATSFsReadFile(pMakeName,(void*) pMakeBuf,0,_MAKE_FILE_SIZE_,&u4RealSize,&u4Length,&iFd);
#endif


    if (fgOpenSuccess == FALSE)
    {
       Printf("[WARNING] Open Make Fail: %s\n ", pMakeName);
       //ASSERT(0);
       return FALSE;
    }

    
    u4Sz = 0;
    while( u4PrsSz < u4RealSize) 
    {
        fgret = fgG3DBufGetLine(pMakeBuf, &u4PrsSz, line, u4RealSize);
        if (!fgret)
            break;

		//Printf("line:%s\n", line);

        if (line[0] == 'D' && line[1] == 'U' && line[2] == 'M' && line[3] == 'P' && line[4] == 'S')
        {
            i4StartIdx = 0;
            for (i=5; i < strlen(line); i++)
            {
                if (line[i] == '=')
                {
                   i4StartIdx = i+1;
                   break;
                }
                else
                if (line[i] == '+' && line[i+1] == '=')
                {
                   i4StartIdx = i+2;
                   break;
                }
            }
                        
            j = 0;
            for (i=i4StartIdx; i < strlen(line); i++)
            {
                if ( (line[i] == 0x0 || line[i] == '\n' || line[i] == '\r' || line[i] == '#')  &&  j > 0 )
                {
                    goldname[goldcnt][j] = 0x0;

                    if (  (goldname[goldcnt][j-4] == '.')
                        && (goldname[goldcnt][j-3] == 'h')
                        && (goldname[goldcnt][j-2] == 'e')
                        && (goldname[goldcnt][j-1] == 'x') )
                    {
                       //do nothing.
                    }
                    else
                    {
                       //concate .hex
                       strcat(goldname[goldcnt], "_result.hex");
                    }
                    goldcnt++;
                    j = 0;
                    break;
                }
                else
                if ( line[i] == ' ')
                {
                   if ( j > 0 )
                   {
                      goldname[goldcnt][j] = 0x0;

                      if (  (goldname[goldcnt][j-4] == '.')
                        && (goldname[goldcnt][j-3] == 'h')
                        && (goldname[goldcnt][j-2] == 'e')
                        && (goldname[goldcnt][j-1] == 'x') )
                      {
                         //do nothing.
                      }
                      else
                      {
                         //concate .hex
                         strcat(goldname[goldcnt], "_result.hex");
                      }
                      goldcnt++;
                      j = 0;
                   }
                }
                else
                if (fgRealChar( line[i] ))
                {
                  goldname[goldcnt][j] = line[i];
                  j++;
                }
            }      

            if (j > 0)
            {
                    goldname[goldcnt][j] = 0x0;
                    
                    if (  (goldname[goldcnt][j-4] == '.')
                        && (goldname[goldcnt][j-3] == 'h')
                        && (goldname[goldcnt][j-2] == 'e')
                        && (goldname[goldcnt][j-1] == 'x') )
                    {
                       //do nothing.
                    }
                    else
                    {
                       //concate .hex
                       strcat(goldname[goldcnt], "_result.hex");
                    }
                    goldcnt++;
                    j = 0;
            }
            
        }
        else
        if (line[0] == 'I' && line[1] == 'M' && line[2] == 'A' && line[3] == 'G' && line[4] == 'E' && line[5] == 'S' && line[6] == '=')
        {
            j = 0;
            for (i=7; i < strlen(line); i++)
            {
                if (line[i] == 0x0 || line[i] == '\n' || line[i] == '\r' || ((line[i] == ' ') && ( j > 0 )))
                {
                    goldname[goldcnt][j] = 0x0;

                    if (  (goldname[goldcnt][j-4] == '.')
                        && (goldname[goldcnt][j-3] == 'h')
                        && (goldname[goldcnt][j-2] == 'e')
                        && (goldname[goldcnt][j-1] == 'x') )
                    {
                       //do nothing.
                    }
                    else
                    {
                       //concate .hex
                       strcat(goldname[goldcnt], "_result.hex");
                    }
                    goldcnt++;
                    j = 0;
                    break;
                }
                else
                {
                  goldname[goldcnt][j] = line[i];
                  j++;
                }
            }      

            if (j > 0)
            {
                    goldname[goldcnt][j] = 0x0;
                    
                    if (  (goldname[goldcnt][j-4] == '.')
                        && (goldname[goldcnt][j-3] == 'h')
                        && (goldname[goldcnt][j-2] == 'e')
                        && (goldname[goldcnt][j-1] == 'x') )
                    {
                       //do nothing.
                    }
                    else
                    {
                       //concate .hex
                       strcat(goldname[goldcnt], "_result.hex");
                    }
                    goldcnt++;
                    j = 0;
            }
        }
        else
        if (line[0] == 'D' &&
            line[1] == 'I' &&
            line[2] == 'F' &&
            line[3] == 'F' &&
            line[4] == '_' &&
            line[5] == 'D' &&
            line[6] == 'U' &&
            line[7] == 'M' &&
            line[8] == 'P' )
        {
             i4StartIdx = 0;
                   for (i=9; i < strlen(line); i++)
                   {
                        if (line[i] == '=')
                        {
                           i4StartIdx = i+1;
                           break;
                        }
                    }
                          
            fgDiffDump = TRUE;
                  fgDiffResult = FALSE;
            fgKeepSearch = FALSE;
            j=0;
            for (i=i4StartIdx; i < strlen(line); i++)
            {
                if (line[i] == 0x0 || line[i] == '\n' || line[i] == '\r' )
                {
                    if (j > 0)
                    {
                        compname[cmpcnt][j] = 0x0;              
                           cmpcnt++;
                    }
                    j = 0;
                    break;
                }
                else
                if (line[i] == ' ')
                {               
                    if (j > 0)
                    {                     
                      compname[cmpcnt][j] = 0x0;
                      j = 0;
                      cmpcnt++;
                    }
                }
                else            
                if (line[i] == '\\')
                {
                    //do nothing.
                    fgKeepSearch = TRUE;
                }
                else
                if (fgRealChar( line[i] ))
                {
                  compname[cmpcnt][j] = line[i];
                  j++;
                }
            }

            if (j > 0)
                     {
                         compname[cmpcnt][j] = 0x0;             
                cmpcnt++;
                    }
        }
        else
        if (line[0] == 'D' &&
            line[1] == 'I' &&
            line[2] == 'F' &&
            line[3] == 'F' &&
            line[4] == '_' &&
            line[5] == 'R' &&
            line[6] == 'E' &&
            line[7] == 'S' &&
            line[8] == 'U' &&
            line[9] == 'L' &&
            line[10] == 'T' )
        {       
            //line[11] = ' '
            //line[12] = '='
            //line[13] = ' '
            i4StartIdx = 0;
            for (i=11; i < strlen(line); i++)
                    {
                        if (line[i] == '=')
                        {
                           i4StartIdx = i+1;
                           break;
                        }
                     }
                          
            fgDiffDump = FALSE;
                     fgDiffResult = TRUE;
            fgKeepSearch = FALSE;
            j=0;
            for (i=i4StartIdx; i < strlen(line); i++)
            {
                if (line[i] == 0x0 || line[i] == '\n' || line[i] == '\r' )
                {
                    if (j > 0)
                    {
                     goldname[goldcnt][j] = 0x0;
                     goldcnt++;
                     j = 0;
                    }
                    break;
                }
                else
                if (line[i] == ' ')
                {                   
                    if (j > 0)
                    {                     
                      goldname[goldcnt][j] = 0x0;
                      j=0;
                      goldcnt++;
                    }
                }
                else            
                if (line[i] == '\\')
                {
                    //do nothing.
                    fgKeepSearch = TRUE;
                }
                else
                if (fgRealChar( line[i] ))
                {
                  goldname[goldcnt][j] = line[i];
                  j++;
                }
            }

            if (j > 0)
                     {
                         goldname[goldcnt][j] = 0x0;
                goldcnt++;
                j = 0;
                    }
        }
        else
        if (fgDiffDump == TRUE)
        {       
            fgKeepSearch = FALSE;
            j=0;
            for (i=0; i < strlen(line); i++)
            {
                if (line[i] == 0x0 || line[i] == '\n' || line[i] == '\r' )
                {
                    if (j > 0)
                    {
                        compname[cmpcnt][j] = 0x0;                  
                        cmpcnt++;
                    }
                    j = 0;
                    fgDiffDump = fgKeepSearch;
                    break;
                }
                else
                if (line[i] == ' ')
                {                   
                    if ( j > 0 )
                    {                     
                      compname[cmpcnt][j] = 0x0;
                      cmpcnt++;
                      j = 0;
                    }
                }
                else            
                if (line[i] == '\\')
                {
                    //do nothing.
                    fgKeepSearch = TRUE;
                }
                else
                if (fgRealChar( line[i] ))
                {
                  compname[cmpcnt][j] = line[i];
                  j++;
                }
            }

            if (j > 0)
                     {
                         compname[cmpcnt][j] = 0x0;
                cmpcnt++;
                j = 0;
                    }
        }
        else
        if (fgDiffResult == TRUE)
        {       
            fgKeepSearch = FALSE;
            j=0;
            for (i=0; i < strlen(line); i++)
            {
                if (line[i] == 0x0 || line[i] == '\n' || line[i] == '\r' )
                {
                    if (j > 0)
                    {
                      goldname[goldcnt][j] = 0x0;
                      goldcnt++;
                    }
                    j=0;
                    fgDiffResult = fgKeepSearch;
                    break;
                }
                else
                if (line[i] == ' ')
                {               
                    if (j > 0)
                    {                     
                      goldname[goldcnt][j] = 0x0;                     
                      goldcnt++;
                      j=0;
                    }
                }
                else            
                if (line[i] == '\\')
                {
                    //do nothing.
                    fgKeepSearch = TRUE;
                }
                else
                if (fgRealChar( line[i] ))
                {
                  goldname[goldcnt][j] = line[i];
                  j++;
                }
            }

            if (j > 0)
                     {
                        goldname[goldcnt][j] = 0x0;                   
               goldcnt++;
               j = 0;
                    }
        }                                       
    } //    


    return TRUE;
}


void  vIsMemCmpPass (void)
{
    INT32 eMemCmpResult = ( eG3dEmuResult.eCmpAllMemResult | eG3dEmuResult.eCmpMemResult | eG3dEmuResult.eCmpPpMemResult );
    INT32 eRegCmpResult = ( eG3dEmuResult.eCmpRegResult | eG3dEmuResult.eCmpGpRegResult | eG3dEmuResult.eCmpPpRegResult );

    if ( eMemCmpResult >= G3D_CMP_STATUS_FAIL)
    {
        eG3dEmuResult.eCmpMemPass = G3D_CMP_STATUS_FAIL;
    }
    else
    if ( eMemCmpResult == (G3D_CMP_STATUS_SUCCESS)  || eMemCmpResult == (G3D_CMP_STATUS_SUCCESS | G3D_CMP_STATUS_OPEN_FAIL))
    {
        eG3dEmuResult.eCmpMemPass = G3D_CMP_STATUS_SUCCESS;        
    }
    else
    {
        eG3dEmuResult.eCmpMemPass = G3D_CMP_STATUS_UNKNOW;
    }

    if ( eRegCmpResult >= G3D_CMP_STATUS_FAIL )
    {
        eG3dEmuResult.eCmpRegPass = G3D_CMP_STATUS_FAIL;
    }
    else
    if ( eRegCmpResult == (G3D_CMP_STATUS_SUCCESS)  || eRegCmpResult == (G3D_CMP_STATUS_SUCCESS | G3D_CMP_STATUS_OPEN_FAIL))
    {
        eG3dEmuResult.eCmpRegPass = G3D_CMP_STATUS_SUCCESS;
    }
    else
    {
        eG3dEmuResult.eCmpRegPass = G3D_CMP_STATUS_UNKNOW;
    }    
}


void vGfx3DTestResult(BOOL fgWaitIrqDone)
{
	if (fgWaitIrqDone == G3D_CMP_STATUS_SUCCESS) 
	   {
			if (eG3dEmuResult.eCmpMemPass == G3D_CMP_STATUS_SUCCESS)
			{			  
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_SUCCESS)
					   Printf("[Mali-Test-Result] [ %s ]: IRQ - MEM - REG ==>", ucConfigStringName);
				 else
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_FAIL)
					   Printf("[Mali-Test-Result] [ %s ]: IRQ - MEM - FFF ==>", ucConfigStringName);
				 else
					   Printf("[Mali-Test-Result] [ %s ]: IRQ - MEM - XXX ==>", ucConfigStringName);
			}
			else
			if (eG3dEmuResult.eCmpMemPass == G3D_CMP_STATUS_FAIL)
			{
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_SUCCESS)
					   Printf("[Mali-Test-Result] [ %s ]: IRQ - FFF - REG ==>", ucConfigStringName);
				 else
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_FAIL)
					   Printf("[Mali-Test-Result] [ %s ]: IRQ - FFF - FFF ==>", ucConfigStringName);
				 else
					   Printf("[Mali-Test-Result] [ %s ]: IRQ - FFF - XXX ==>", ucConfigStringName);
			}
			else
			{
				if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_SUCCESS)
					   Printf("[Mali-Test-Result] [ %s ]: IRQ - XXX - REG ==>", ucConfigStringName);
				 else
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_FAIL)
					   Printf("[Mali-Test-Result] [ %s ]: IRQ - XXX - FFF ==>", ucConfigStringName);
				 else
					   Printf("[Mali-Test-Result] [ %s ]: IRQ - XXX - XXX ==>", ucConfigStringName);
			}
	   }
	   else
	   if (fgWaitIrqDone == G3D_CMP_STATUS_FAIL)
	   {   
		   if (eG3dEmuResult.eCmpMemPass == G3D_CMP_STATUS_SUCCESS)
			{			  
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_SUCCESS)
					   Printf("[Mali-Test-Result] [ %s ]: FFF - MEM - REG ==>", ucConfigStringName);
				 else
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_FAIL)
					   Printf("[Mali-Test-Result] [ %s ]: FFF - MEM - FFF ==>", ucConfigStringName);
				 else
					   Printf("[Mali-Test-Result] [ %s ]: FFF - MEM - XXX ==>", ucConfigStringName);
			}
			else
			if (eG3dEmuResult.eCmpMemPass == G3D_CMP_STATUS_FAIL)
			{
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_SUCCESS)
					   Printf("[Mali-Test-Result] [ %s ]: FFF - FFF - REG ==>", ucConfigStringName);
				 else
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_FAIL)
					   Printf("[Mali-Test-Result] [ %s ]: FFF - FFF - FFF ==>", ucConfigStringName);
				 else
					   Printf("[Mali-Test-Result] [ %s ]: FFF - FFF - XXX ==>", ucConfigStringName);
			}
			else
			{
				if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_SUCCESS)
					   Printf("[Mali-Test-Result] [ %s ]: FFF - XXX - REG ==>", ucConfigStringName);
				 else
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_FAIL)
					   Printf("[Mali-Test-Result] [ %s ]: FFF - XXX - FFF ==>", ucConfigStringName);
				 else
					   Printf("[Mali-Test-Result] [ %s ]: FFF - XXX - XXX ==>", ucConfigStringName);
			}		
	   }
	   else
	   {
			if (eG3dEmuResult.eCmpMemPass == G3D_CMP_STATUS_SUCCESS)
			{			  
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_SUCCESS)
					   Printf("[Mali-Test-Result] [ %s ]: XXX - MEM - REG ==>", ucConfigStringName);
				 else
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_FAIL)
					   Printf("[Mali-Test-Result] [ %s ]: XXX - MEM - FFF ==>", ucConfigStringName);
				 else
					   Printf("[Mali-Test-Result] [ %s ]: XXX - MEM - XXX ==>", ucConfigStringName);
			}
			else
			if (eG3dEmuResult.eCmpMemPass == G3D_CMP_STATUS_FAIL)
			{
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_SUCCESS)
					   Printf("[Mali-Test-Result] [ %s ]: XXX - FFF - REG ==>", ucConfigStringName);
				 else
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_FAIL)
					   Printf("[Mali-Test-Result] [ %s ]: XXX - FFF - FFF ==>", ucConfigStringName);
				 else
					   Printf("[Mali-Test-Result] [ %s ]: XXX - FFF - XXX ==>", ucConfigStringName);
			}
			else
			{
				if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_SUCCESS)
					   Printf("[Mali-Test-Result] [ %s ]: XXX - XXX - REG ==>", ucConfigStringName);
				 else
				 if (eG3dEmuResult.eCmpRegPass == G3D_CMP_STATUS_FAIL)
					   Printf("[Mali-Test-Result] [ %s ]: XXX - XXX - FFF ==>", ucConfigStringName);
				 else
					   Printf("[Mali-Test-Result] [ %s ]: XXX - XXX - XXX ==>", ucConfigStringName);
			}
	
			 
	   }
	   
	   Printf("\n");
	   //GP Reg
	   if (eG3dEmuResult.eCmpGpRegResult == G3D_CMP_STATUS_SUCCESS)
			Printf("Gp_reg: OK ");
	   else
	   if (eG3dEmuResult.eCmpGpRegResult == G3D_CMP_STATUS_FAIL)
			Printf("Gp_reg: NG ");
	   else
			Printf("Gp_reg: NA ");

	   Printf("\n");
	   //PP Reg
	   if (eG3dEmuResult.eCmpPpRegResult == G3D_CMP_STATUS_SUCCESS)
			Printf("Pp_reg: OK ");
	   else
	   if (eG3dEmuResult.eCmpPpRegResult == G3D_CMP_STATUS_FAIL)
			Printf("Pp_reg: NG ");	 
	   else
			Printf("Pp_reg: NA ");
	   
	   Printf("\n");	   
	   //Dump Reg
	   if (eG3dEmuResult.eCmpRegResult == G3D_CMP_STATUS_SUCCESS)
			Printf("Nm_reg: OK ");
	   else
	   if (eG3dEmuResult.eCmpRegResult == G3D_CMP_STATUS_FAIL)
			Printf("Nm_reg: NG ");	 
	   else
			Printf("Nm_reg: NA ");
	
	   Printf("\n");
	   //AllMem
	   if (eG3dEmuResult.eCmpAllMemResult == G3D_CMP_STATUS_SUCCESS)
			Printf("Al_mem: OK ");
	   else
	   if (eG3dEmuResult.eCmpAllMemResult == G3D_CMP_STATUS_FAIL)
			Printf("Al_mem: NG ");
	   else
			Printf("Al_mem: NA ");
	   
	   Printf("\n");
	   //PpMem
	   if (eG3dEmuResult.eCmpPpMemResult == G3D_CMP_STATUS_SUCCESS)
			Printf("Pp_mem: OK ");
	   else
	   if (eG3dEmuResult.eCmpPpMemResult == G3D_CMP_STATUS_FAIL)
			Printf("Pp_mem: NG ");
	   else
			Printf("Pp_mem: NA ");

	
	   Printf("\n");
	   //Mem
	   if (eG3dEmuResult.eCmpMemResult == G3D_CMP_STATUS_SUCCESS)
			Printf("Nm_mem: OK ");
	   else
	   if (eG3dEmuResult.eCmpMemResult == G3D_CMP_STATUS_FAIL)
			Printf("Nm_mem: NG ");	 
	   else
			Printf("Nm_mem: NA ");
	   
	   Printf("\n\n");


}

#if WATCH_PERF_COUNT   
static void vG3DEnablePerfCount(void)
{
	// L2 Cache Perf Enable
	vMaliWriteReg(0x101c, 0x1);
	vMaliWriteReg(0x1020, 0xA);
	vMaliWriteReg(0x1028, 0xB);
	
	//GP Perf Enable
	vMaliWriteReg(0x003C, 0x1);
	vMaliWriteReg(0x0044, 0x1);
	
	//PP Perf Enable
	vMaliWriteReg(0x9080, 0x1);
	vMaliWriteReg(0x9084, 0x0);
	return;
}
#endif

G3D_CMP_STATUS G3DSLTResult(void)
{
	return eG3dEmuResult.eCmpAllMemResult;
}

void vG3DEmuParseConfig ( void *pConfig, UINT32 u4ConfigSz, CHAR *pucPathName, BOOL fgRedo)
{
   //CHAR config[500] = {'\0'};
   CHAR cmd[256],line[256];
   CHAR adr0[50], adr1[50];
   CHAR val0[24], val1[24], val2[24];
   INT32  once, new_adr, new_adr0, new_adr1,clear_value;
   INT32  r, value, idx;
   INT32  addr0, addr1;//, len;
   INT32  line_number=0; 
   CHAR str0[100],str1[100];
   INT32 argc = 2;
   CHAR dumpname [256];   
   UINT32 u4PrsSz = 0;
   INT32  regval;
   UINT32 u4offset = 0;
   CHAR load_filename [256];
   CHAR make_filename [256];   
   CHAR fullgold_filename [256];
   INT32 i;
   BOOL  fgCompared = FALSE;
   CHAR *pch;
   BOOL   fgResult = TRUE;
   G3D_CMP_STATUS  fgWaitIrqDone = G3D_CMP_STATUS_UNKNOW;
   G3D_CMP_STATUS  eCompareResult = G3D_CMP_STATUS_UNKNOW;
   UINT32 u4DumpRegCnt = 0;
   UINT32 u4DumpRegRang[20][2];

   x_memset( (void*) (&eG3dEmuResult), 0, sizeof(G3D_EMU_RESULT_T));

   fgMakeFileParsed = FALSE;
   u4RecordCnt = 0;

   for (i=0; i<10; i++)
   {
       u4DumpMemCount[i] = 0;
       u4DumpRegCount[i] = 0;
   }
   
   if(argc==2)
   {
      // config translation
      Printf("*************************\n");
      Printf(" MALI to MTK Integration \n");
      Printf("*************************\n");  
      
      Printf("converting config.txt...\n");
      
      once=0;
      while(u4PrsSz < u4ConfigSz) 
      {    		 		 
		 memset( (void*) (line), 0, 256); 		 
		 memset( (void*) (cmd), 0, 256); 
		 memset( (void*) (str0), 0, 100);
		 memset( (void*) (str1), 0, 100);
		 memset( (void*) (val0), 0, 24);
		 memset( (void*) (val1), 0, 24);
		 memset( (void*) (val2), 0, 24);

         fgG3DBufGetLine((CHAR*) pConfig, &u4PrsSz, line, u4ConfigSz);               
         line_number = line_number + 1;
         
         r = sscanf(line, "%s\r\n", cmd);
	     //Printf("cmd is : %s\n", cmd);
         
         //if(c!=NULL)
         {
            if(r==1)
            { 

               if(strcmp(cmd, "force")==0)
               {
                  r = sscanf(line, " force %s", str0);
                  if(strcmp(str0, "gp_reset")==0)
                  {
                      Printf("reset gp_reset\n");
                      x_thread_delay(10);
                      regval = u4MaliReadReg(0x0020);
                      regval |= (0x1<<5);
                      vMaliWriteReg(0x0020, regval);
                      x_thread_delay(10);
                  }
                  else
                  if(strcmp(str0, "pp_reset")==0)
                  {
                      Printf("reset pp_reset\n");                  
                      x_thread_delay(10);
                      regval = u4MaliReadReg(0x900c);
                      regval |= (0x1<<5);
                      vMaliWriteReg(0x900c, regval);
                      x_thread_delay(10);
                  }
               }
               else
               if(strcmp(cmd, "reset")==0)
               {
                   printf("reset cmd\n");
				   /* reset in top layer for l2 pattern pass, L2 also need reset */
                   #if 1
				   vMaliResetMFGTop();
				   #else
				   vMaliResetHw();
				   #endif
				   
                   #if G3D_SUPPORT_INTERNAL_MMU
				   ResetInitGPMMU();
				   ResetInitPPMMU();
				   #if G3D_SUPPORT_MP
				   ResetInitPP1MMU();	
				   #if G3D_SUPPORT_MALIMP4
				   ResetInitPP2MMU();
				   ResetInitPP3MMU();
				   #endif
				   #endif
                   #endif

				   //disable L2 cache
				   //vMaliWriteReg(MALI400_REG_BASE_L2 + 0x1C, 0x0);

				   /*enable watchdog for F204-plbu_scissor02_elements_mmu */
				   #if 0
				   vMaliWriteReg(0x00A0, 0x0);			   
				   vMaliWriteReg(0x9060, 0x0);
				   /* timeout setting */
				   vMaliWriteReg(0x00A4, 0x100);
				   #endif
               }  
               else if(strcmp(cmd, "wait")==0)
               {
                    #if 0
					/* for pattern F206-apb-interrupt_16 */
					if (eG3dEmuConfig.fgIsGPWatchDogDisable)
					{
						Printf("Disable GP Watch Dog TimeeOut\n");
						vMaliWriteReg(0x00A0, 0x1);
					}

					if (eG3dEmuConfig.fgIsPPWatchDogDisable)
					{
						Printf("Disable PP Watch Dog TimeeOut\n");
						vMaliWriteReg(0x9060, 0x1);
					}
					#endif 
					
                   if ( fgMaliWaitIRQ(line) )
                    fgWaitIrqDone = G3D_CMP_STATUS_SUCCESS;
                   else                     
                    fgWaitIrqDone = G3D_CMP_STATUS_FAIL;
               }

			   else if(strcmp(cmd, "check_reg")==0)
			   {
					MaliCheckRegProc(line);
			   }
			   /* mtk added internal command for GP & PP used different mmu table */
			   else if(strcmp(cmd, "check_hex")==0)
			   {
				   MaliCheckHexTypeProc(line);
			   }
			   
               else if(strcmp(cmd, "clear_mem")==0)
               {
                  r = sscanf(line, " clear_mem %s %s %s", str0, str1, val0);
				  
				  addr0 = u4StrToInt(str0);
				  addr1 = u4StrToInt(str1);  			  
				  clear_value = u4StrToInt(val0); 				  
				  x_memset((void*)VIRTUAL(M2P(addr0)),clear_value,(addr1-addr0));
                  
                  if(r==2)
                     Printf( "    top.clear_mem('h%s, 'h%s, 0);\n", str0, str1);
                  else
                     Printf( "    top.clear_mem('h%s, 'h%s, 'h%s);\n", str0, str1, val0);
               }
               else if(strcmp(cmd, "writereg")==0)
               {
                  r = sscanf(line, " writereg %s %s", adr0, val0);                                    
                  
                  value=0;    idx=0;                  
                  value = u4StrToInt(adr0);
                  new_adr = u4ConfigAddToRealAddr(value);
                  regval = u4StrToInt(val0);          
					
                  if (new_adr == 0x8104 || new_adr == 0x8204 || new_adr == 0x8304)
                  {
                      fgIsPPOutBuff = TRUE;			  
					  //regval = (pPPPhyMemBuff + regval);						  
                      pPPOutBuffAddr = (CHAR*)regval;                    
                      Printf( "PP Output Buffer =  0x%08x\n", regval);
                  }
				  
				  //for PP1
				  else if (new_adr == 0xA104 || new_adr == 0xA204 || new_adr == 0xA304)
                  {
                      fgIsPPOutBuff = TRUE;
                      pPPOutBuffAddr = (CHAR*)regval;                      
                      Printf( "PP1 Output Buffer =  0x%08x\n", regval);;
                  }

				  
                  if (new_adr == 0x4000)
                  {                
				       //regval = (pPPPhyMemBuff + regval);	
					   
                       //uPOutBuffBase = (u4ReadMem( (regval + 0x1000)) & 0x0FFFFFF0);
                       Printf( "PP Output Base =  0x%08x\n", regval);
                  }
                  
                  //Printf( "writereg 0x%08x : 0x%08x\n", new_adr, regval);
                  vMaliWriteReg(new_adr, regval);
               }
               else if(strcmp(cmd, "dump_mem")==0)
               {
                  vPrintRegister(fgWaitIrqDone );
                  r = sscanf(line, " dump_mem %x %x %s %s", (unsigned int*) &addr0, (unsigned int *) &addr1, str0, str1);
                  //len = addr1 - addr0;
                  Printf( "Dump Memory Start\n");
                  
                  if(r==2)
                  {
                     ;//Printf( "    mali_testbench.DUT.u_dramc.SaveMemory(`STDOUT, 32'h%08x, 32'h%08x );\n", addr0, len);      
                  }
                  else if(r==4)
                  {
                     if(strcmp(str0, ">")==0) 
                     {
                        Printf( " str0 is > \n");           
                     }
                     else if(strcmp(str0, ">>")==0) 
                     {
                        Printf( " str0 is >> \n");           
                     }

                     new_adr0 = addr0;
                     new_adr1 = addr1;
                     //new_adr0 = u4ConfigAddToRealAddr(addr0);
                     //new_adr1 = u4ConfigAddToRealAddr(addr1);                     
                  
                     Printf("Dump_mem: 0x%08x ~ 0x%08x", new_adr0, new_adr1);
                     Printf( " str1 is %s \n", str1);
                     strcpy (dumpname, str1);

                     strcpy(make_filename, pucPathName);
                     strcat(make_filename, "Makefile");
                     if (fgMakeFileParsed == FALSE)
                     {
                       fgResult = vG3DEmuParseMakefile(make_filename, dumpname, HEX_COMP_MEM);
                       fgMakeFileParsed = TRUE;

                     }

                     fgCompared = FALSE;
                     if (cmpcnt >0 && goldcnt > 0)
                     {
                         for (i=0; i < cmpcnt; i++)
                         {
                            if ( strcmp(dumpname, compname[i]) == 0)
                            {
                               strcpy(fullgold_filename, pucPathName);
                               strcat(fullgold_filename, goldname[i]);
                               eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_MEM, u4DumpMemCount[i], (new_adr1 - new_adr0), 0x0));
                               eG3dEmuResult.eCmpMemResult |= eCompareResult;
                               fgCompared = TRUE;
                               u4DumpMemCount[i] += (new_adr1 - new_adr0);
                               break;
                           }
                         }
                     }
                     else
                     if (cmpcnt == 0 && goldcnt > 0)
                     {
                        pch = strrchr(str1, '.');
                        for (i=0; i < (pch - str1); i++)
                        {
                           dumpname[i] = str1[i];
                        }
                        dumpname[i] = 0x0;
                        strcat(dumpname, "_result.hex");
                        
                         for (i=0; i < goldcnt; i++)
                         {
                            if ( strcmp(dumpname, goldname[i]) == 0)
                            {
                               strcpy(fullgold_filename, pucPathName);
                               strcat(fullgold_filename, goldname[i]);
                               eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_MEM, u4DumpMemCount[i], (new_adr1 - new_adr0), 0x0));
                               eG3dEmuResult.eCmpMemResult |= eCompareResult;
                               fgCompared = TRUE;
                               u4DumpMemCount[i] += (new_adr1 - new_adr0);
                               break;
                           }
                         }
                     }
                     else
                     if (cmpcnt > 0 && goldcnt == 0)
                     {
                        Printf("Mismatch\n");
                     }
                     
                     if (fgCompared == FALSE)
                     {
                          strcpy(fullgold_filename, pucPathName);	  
						  if(eG3dEmuConfig.fgIsMaliGPTest
						  ||eG3dEmuConfig.fgIsMaliPPTest)
						  {				  
						      printf("fgIsMaliBaseTest strcat result.hex\n");
                              strcat(fullgold_filename, "result.hex");	
						  }
						  else
						  {			  
						      printf("strcat str1\n");
                              strcat(fullgold_filename, str1);
						  }
                          eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_MEM, u4DumpMemCount[0], (new_adr1 - new_adr0), 0x0));
                           eG3dEmuResult.eCmpMemResult |= eCompareResult;
                          u4DumpMemCount[0] += (new_adr1 - new_adr0);
                     }
                  }
                  Printf( " Dump Memory End\n");
               }
               else if(strcmp(cmd, "dump_written_mem")==0)
               {
                  vPrintRegister(fgWaitIrqDone);
                  
                  r = sscanf(line, " dump_written_mem %x %x %s %s", (unsigned int*)&addr0, (unsigned int*) &addr1, str0, str1);                  
                  if(r<=0)
                  {
                     r = sscanf(line, " dump_written_mem %s %s", str0, str1);
                     if(strcmp(str0, ">")==0)
                        Printf( "file_tmp = $fopen(\"%s\", \"w\");\n", str1);                                                
                     else if(strcmp(str0, ">>")==0)
                        Printf( "file_tmp = $fopen(\"%s\", \"a\");\n", str1);
                     else
                        break;
                        
                     new_adr0 = 0;
                     new_adr1 = 0;                  
                     strcpy (dumpname, str1);
                     // result.hex
                     goldname[0][0] = 'r';
                     goldname[0][1] = 'e';
                     goldname[0][2] = 's';
                     goldname[0][3] = 'u';
                     goldname[0][4] = 'l';
                     goldname[0][5] = 't';
                     goldname[0][6] = '.';
                     goldname[0][7] = 'h';
                     goldname[0][8] = 'e';
                     goldname[0][9] = 'x';
                     goldname[0][10] = 0x0;
                     strcpy(fullgold_filename, pucPathName);
                     strcat(fullgold_filename, goldname[0]);

                     if (!eG3dEmuConfig.fgIsCompareAllMem)
                     {                      
                         eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_MEM, 0, 0x80000000, ((UINT32) pPPOutBuffAddr)+(u4PPOutBuffBase)));
                         eG3dEmuResult.eCmpPpMemResult |= eCompareResult;
                     }
                     else
                     {
                         eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_MEM, 0, 0x80000000, 0));
                          eG3dEmuResult.eCmpAllMemResult |= eCompareResult;
                     }
                     
                     //vG3DEmuCompareHex (make_filename, dumpname, HEX_COMP_MEM, pucPathName, (new_adr1 - new_adr0));
                     
                  }
                  else if(r==4)
                  {
                  #if 0
                     if(strcmp(str0, ">")==0) 
                        Printf( "  file_tmp = $fopen(\"%s\", \"w\");\n", str1);                        
                     else if(strcmp(str0, ">>")==0) 
                        Printf( "  file_tmp = $fopen(\"%s\", \"a\");\n", str1);    
                      else
                        break;

                     value=0;
                     value = u4StrToInt(adr0);
                     new_adr0 = u4ConfigAddToRealAddr(value);
                     value = u4StrToInt(adr1);
                     new_adr1 = u4ConfigAddToRealAddr(value);

                     strcpy (dumpname, str1);
                     fgResult = vG3DEmuCompareHex (make_filename, dumpname, HEX_COMP_MEM, pucPathName, (new_adr1 - new_adr0), (UINT32) pPPOutBuffAddr);
                   #endif                     
                  }
               }
               else if(strcmp(cmd, "dump_reg")==0)
               {               
                  r = sscanf(line, " dump_reg %s %s %s %s", adr0, adr1, str0, str1);                  
                  value=0;
                  value = u4StrToInt(adr0);
                  new_adr0 = u4ConfigAddToRealAddr(value);
                  value = u4StrToInt(adr1);
                  new_adr1 = u4ConfigAddToRealAddr(value);
                  
                  Printf("Dump_reg: 0x%08x ~ 0x%08x\n", new_adr0, new_adr1);

                  u4DumpRegRang[ u4DumpRegCnt ] [0] = new_adr0;
                  u4DumpRegRang[ u4DumpRegCnt ] [1] = new_adr1;
                  u4DumpRegCnt++;

                  if (eG3dEmuConfig.fgIsMaliGPTest ||
				  	  eG3dEmuConfig.fgIsMaliPPTest)
             {
	                  //if(r==4)          
					  printf("Dump_reg: 0x%x ~ 0x%x\n", new_adr0, new_adr1);
                  {
                     if(strcmp(str0, ">")==0) 
                     {
                        Printf( " str0 is > \n");           
                     }
                     else if(strcmp(str0, ">>")==0) 
                     {
                        Printf( " str0 is >> \n");
                     }
						 else
						 {
							continue;
						 }

	                     Printf( " str1 is %s \n", str1);
	                     
	                     strcpy (dumpname, str1);

	                     strcpy(make_filename, pucPathName);
	                     strcat(make_filename, "Makefile");
	                     if (fgMakeFileParsed == FALSE)
	                     {
	                         fgResult = vG3DEmuParseMakefile(make_filename, dumpname, HEX_COMP_REG);
	                         fgMakeFileParsed = TRUE;
	                     }
						 
	                     Printf( "cmpcnt=%d, goldcnt=%d\n",cmpcnt,goldcnt);

	                     fgCompared = FALSE;
	                     if (cmpcnt >0 && goldcnt > 0)
	                     {
	                         for (i=0; i < cmpcnt; i++)
	                         {
	                            if ( strcmp(dumpname, compname[i]) == 0)
	                            {                                       
	                                  strcpy(fullgold_filename, pucPathName);
	                                  strcat(fullgold_filename, goldname[i]);
									   
									  Printf("goldname[%d] = %s\n",i,goldname[i]);
									   
	                                  eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_REG, u4DumpRegCount[i], (new_adr1 - new_adr0), 0x0));
	                                  eG3dEmuResult.eCmpRegResult |= eCompareResult;
	                                  fgCompared = TRUE;
	                                  u4DumpRegCount[i] += (new_adr1 - new_adr0);
	                                  break;
	                             }
	                        }
	                    }
	                    else
	                    if ( cmpcnt == 0 && goldcnt > 0 )
	                    {
	                        pch = strrchr(str1, '.');
	                        for (i=0; i < (pch - str1); i++)
	                        {
	                           dumpname[i] = str1[i];
	                        }
	                        dumpname[i] = 0x0;
	                        strcat(dumpname, "_result.hex");
	                        
	                         for (i=0; i < goldcnt; i++)
	                         {
	                            if ( strcmp(dumpname, goldname[i]) == 0)
	                            {
	                               strcpy(fullgold_filename, pucPathName);
	                               strcat(fullgold_filename, goldname[i]);
								   
								   Printf("goldname[%d] = %s\n",i,goldname[i]);
								   
	                               eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_REG, u4DumpRegCount[i], (new_adr1 - new_adr0), 0x0));
	                               eG3dEmuResult.eCmpRegResult |= eCompareResult;
	                               fgCompared = TRUE;
	                               u4DumpRegCount[i] += (new_adr1 - new_adr0);
	                               break;
	                           }
	                         }
	                    }
	                    else
	                    if ( cmpcnt > 0 && goldcnt == 0 )
	                    {
	                        Printf("Mismatch\n");
	                    }
	                        
	                     
	                    if (fgCompared == FALSE)
	                    {
	                        strcpy(fullgold_filename, pucPathName);
	                        //strcat(fullgold_filename, "result.hex");//str1
	                        strcat(fullgold_filename, str1);
							//strcat(fullgold_filename, "_result.hex");// temple solution for some pattern read makefile failed
	                        eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_REG, u4DumpMemCount[0], (new_adr1 - new_adr0), 0x0));
	                        eG3dEmuResult.eCmpRegResult |= eCompareResult;
	                        u4DumpRegCount[0] += (new_adr1 - new_adr0);
	                    }
	                 }
	          }
	          else
	          {
	                  if(r==2)
	                  {
	                      if (new_adr0 < 0x1000)
	                      {
	                         sprintf(str1, "GP0_status.hex");
	                         Printf( " str1 is  %s\n", str1);
	                         strcpy(fullgold_filename, pucPathName);
	                         strcat(fullgold_filename, str1);
	                         
	                         eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_REG, u4DumpMemCount[0], (new_adr1 - new_adr0), new_adr0));
	                         eG3dEmuResult.eCmpGpRegResult |= eCompareResult;
	                         u4DumpRegCount[0] += (new_adr1 - new_adr0);
	                       }
	                      else
	                      if (new_adr0 >= 0x8000 && new_adr0 < 0xA000)
	                      {
	                         sprintf(str1, "PP0_status.hex");
	                         Printf( " str1 is  %s\n", str1);
	                         strcpy(fullgold_filename, pucPathName);
	                         strcat(fullgold_filename, str1);
	                         eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_REG, u4DumpMemCount[0], (new_adr1 - new_adr0), new_adr0));
	                         eG3dEmuResult.eCmpPpRegResult |= eCompareResult;
	                         u4DumpRegCount[0] += (new_adr1 - new_adr0);                         
	                      }
	                  }
	                  else
	                  if(r==4)
	                  {
	                  
					      printf("Dump_reg: 0x%x ~ 0x%x\n", new_adr0, new_adr1);
	                     if ( (strcmp(str0, ">")==0) || (strcmp(str0, ">>")==0) )
	                     {                     
	                         Printf( " str1 is %s \n", str1);                        
	                         strcpy(fullgold_filename, pucPathName);
	                         strcat(fullgold_filename, str1);

	                         eCompareResult = (vG3DEmuParseMemHex (fullgold_filename, 0, HEX_COMP_REG, u4DumpMemCount[0], (new_adr1 - new_adr0), new_adr0));
	                         eG3dEmuResult.eCmpRegResult |= eCompareResult;
	                         u4DumpRegCount[0] += (new_adr1 - new_adr0);
	                     }
	                  }                  
	          }
               }
               else if(strcmp(cmd, "outstanding")==0)
               {
                  r = sscanf(line, " outstanding %s %s %s", val0, val1, val2 );
                  Printf( "$display(\"Setting outstanding transactions support: %s reads, %s writes, XX total\");\n", val0, val1, val2);
                  Printf( "RISCWrite(`SVOAHB_00_ADDR + 'hE004, 'h%s%s);\n", val0, val1);
               }
               else if(strcmp(cmd, "quit")==0 && !once)
               {
                  //Printf( "$finish;\n");
                  //once=1;
               }
               // Memory convertion
               else if(strcmp(cmd, "load_mem")==0)
               {
#if WATCH_PERF_COUNT   
				  vG3DEnablePerfCount();
#endif     

                  val0[0] = 0x0;
                  r = sscanf(line, " load_mem %s %s", str0, val0);
                                   
                  strcpy( load_filename, pucPathName);
                  strcat(load_filename, str0);

                  u4offset = 0;
                  if (val0[0] != 0x0)
                  {
                      u4offset= u4StrToInt(val0);
                  }

                  if (fgRedo == FALSE)
                      fgResult = vG3DEmuParseMemHex (load_filename, u4offset, HEX_LOAD, 0, 0xfffffff, 0x0);

                  if(strcmp(str0, "mc1.hex")==0)
					{   

						if (eG3dEmuConfig.fgIsGPWatchDogDisable)
						{
							Printf("Disable GP Watch Dog TimeeOut\n");
							vMaliWriteReg(0x00A0, 0x1);
						}

						if (eG3dEmuConfig.fgIsPPWatchDogDisable)
						{
							Printf("Disable PP Watch Dog TimeeOut\n");
							vMaliWriteReg(0x9060, 0x1);
						}

						Printf("Set TimeOut to 0xFFFFFFFF\n");
						vMaliWriteReg(0x00A4, 0xFFFFFFFF);
						vMaliWriteReg(0x9064, 0xFFFFFFFF);

						Printf("R/W Outstanding: %d, %d\n", eG3dEmuConfig.u4ReadStand, eG3dEmuConfig.u4WriteStand);
						regval = u4MaliReadReg(0xE00C);
						regval |= ( (eG3dEmuConfig.u4ReadStand << 4) | (eG3dEmuConfig.u4WriteStand) );
						vMaliWriteReg(0xE00c, regval);
					}   
               }
               
             } // r=1
   
          }  // c != NULL  
       }  //end of while
 
    Printf("}\n");
    } //end of if
  
	vIsMemCmpPass();
	vGfx3DTestResult(fgWaitIrqDone);
	Printf("\n");
   
} //end of func.


  

INT32 i4Graphics3D_Drv_RegRW(void)
{
   INT32  i4Idx = 0;
   INT32  i4WrData = 0;
   INT32  i4RdData = 0;  
   while (1)
   {
	   for ( i4Idx = 0; i4Idx < 256; i4Idx++)
	   {
	        if ( (i4Idx & 0xf) == 0x0)
	        {
	            Printf("i4Idx = %d\n", i4Idx);
	        }
	        i4RdData = u4MaliReadReg(4);
	        Printf("i4RdData = %d\n", i4RdData);
	        if (i4WrData != i4RdData)
	        {
	            Printf("Mali W != R!!\n");
	        }        
	   }
   }

}

static void vG3dStaticMemCfg(void)
{

#if G3D_VFY_ENV
	pG3dEmuList =     (CHAR*)(VIRTUAL)(eG3DMemLayOut [0][G3D_LIST_ADR_IDX][0]);
	pConfigBuffer =   (CHAR*)(VIRTUAL)(eG3DMemLayOut [0][G3D_CONF_ADR_IDX][0]);
	pMakeFileBuffer = (CHAR*)(VIRTUAL)(eG3DMemLayOut[0][G3D_MAKE_ADR_IDX][0]);
	pGPDTEBuffer  =   (CHAR*)eG3DMemLayOut [0][G3D_GPTB_ADR_IDX][0];
	pPPDTEBuffer =    (CHAR*)eG3DMemLayOut [0][G3D_PPTB_ADR_IDX][0];
	pHexFileBuffer	= (CHAR*)(VIRTUAL)(eG3DMemLayOut [0][G3D_HEXF_ADR_IDX][0]);	
	pGPPhyMemBuff =   (CHAR*)eG3DMemLayOut [0][G3D_GPPH_ADR_IDX][0];
	pPPPhyMemBuff =   (CHAR*)eG3DMemLayOut [0][G3D_PPPH_ADR_IDX][0];
#else
	pG3dEmuList =     (CHAR*)(VIRTUAL)(eG3DMemLayOut [1][G3D_LIST_ADR_IDX][0]);
	pConfigBuffer =   (CHAR*)(VIRTUAL)(eG3DMemLayOut [1][G3D_CONF_ADR_IDX][0]);
	pMakeFileBuffer = (CHAR*)(VIRTUAL)(eG3DMemLayOut[1][G3D_MAKE_ADR_IDX][0]);
	pGPDTEBuffer  =   (CHAR*)eG3DMemLayOut [1][G3D_GPTB_ADR_IDX][0];
	pPPDTEBuffer =    (CHAR*)eG3DMemLayOut [1][G3D_PPTB_ADR_IDX][0];
	pHexFileBuffer	= (CHAR*)(VIRTUAL)(eG3DMemLayOut [0][G3D_HEXF_ADR_IDX][0]);	
	pGPPhyMemBuff =   (CHAR*)eG3DMemLayOut [1][G3D_GPPH_ADR_IDX][0];
	pPPPhyMemBuff =   (CHAR*)eG3DMemLayOut [1][G3D_PPPH_ADR_IDX][0];
#endif

#if 0
	printf("pGPPhyMemBuff is 0x%x,mapping(0x%x)\n",pGPPhyMemBuff,VIRTUAL((UINT32)pGPPhyMemBuff));
	printf("pPPPhyMemBuff is 0x%x,mapping(0x%x)\n",pPPPhyMemBuff,VIRTUAL((UINT32)pPPPhyMemBuff));
	printf("pGPDTEBuffer is 0x%x,mapping(0x%x)\n",pGPDTEBuffer,VIRTUAL((UINT32)pGPDTEBuffer));
	printf("pPPDTEBuffer is 0x%x,mapping(0x%x)\n",pPPDTEBuffer,VIRTUAL((UINT32)pPPDTEBuffer));
#endif

	x_memset((pG3dEmuList), 0x0, G3D_ALLOC_MEM_SIZE);
}


void vG3DEmuCfgInit(void)
{
	//x_memset( (void*) (&eG3dEmuConfig), 0, sizeof(G3D_EMU_CONFIG_T));
	//Printf("vG3DEmuCfgInit-1\r\n");

	eG3dEmuConfig.fgIsMaliGPTest = FALSE;
	eG3dEmuConfig.fgIsMaliPPTest = FALSE;
	eG3dEmuConfig.fgIsHexTypeGP  = FALSE;
	eG3dEmuConfig.fgIsHexTypePP  = FALSE;	
	eG3dEmuConfig.fgIsAxiBrdgWROutStand = AXI_BRDG_WR_OUTSTAND_TEST;
	eG3dEmuConfig.fgIsGPWatchDogDisable = GP_WATCH_DOG_DISABLE;
	eG3dEmuConfig.fgIsPPWatchDogDisable = PP_WATCH_DOG_DISABLE;
	eG3dEmuConfig.fgIsCompareAllMem = COMPARE_ALL_WRITTEN_MEM;
	eG3dEmuConfig.fgIsCompareGPStatus = COMPARE_GP_STATUS;
	eG3dEmuConfig.fgIsComparePPStatus = COMPARE_PP_STATUS; 
	eG3dEmuConfig.fgIsAxiStopTest = AXI_STOP_TEST; 

}

//extern unsigned long get_drvmem_mem(void);
INT32 i4G3DAllocMemory(void)
{	
	//Printf("i4G3DAllocMemory--1!!!\r\n");
    #ifdef CONFIG_TV_DRV_VFY
	pcStaticTotalBuffer = (CHAR*)BSP_AllocVfyReserveMemory(0,G3D_ALLOC_MEM_SIZE,32);
	#else
	pcStaticTotalBuffer = (CHAR*)MEM_DATA_BASE_ADDR; //use channel B memory
	#endif
	Printf("pcStaticTotalBuffer PA : 0x%x !!\n", (INT32) (PHYSICAL ( (UINT32) pcStaticTotalBuffer)) );
	vG3dStaticMemCfg(); 
    return 1;

}

INT32 i4Graphics3D_Drv_Emu(void)
{

#if USB_G3D_VFY
    #if defined(__MODEL_slt__)		
    CHAR	ucEmuListName [256] = "/basic/slt_test_pattern/Mali450/Mali450_patttern_list.bin";		
    #else		
	CHAR    ucEmuListName [256] = "/mnt/usb_0/Mali450/Gazelle_pattern_list.txt";
    #endif 
#else 	
    CHAR	ucEmuListName [256] = "W:\\GPU_Mali450_Pattern\\Gazelle_pattern_list.txt";
#endif

	INT32    i4EmuFd = 0;
	UINT32  u4EmuReadSize = 0;
	UINT32  u4EmuFileSize = 0;
	UINT32  line_number = 0;
	UINT32  u4PrsSz = 0;
	CHAR    line[256];
	CHAR    cmd[256];   
	CHAR    str0 [256];
	INT32    i4ConfigFd = 0;
	UINT32  u4ConfigReadSize = 0;
	UINT32  u4ConfigFileSize = 0;
	BOOL     fgRet = FALSE;
	INT32    r;
	CHAR    ucTemp[20] = {'\0'};
	//CHAR    strDumpFileName [256];
	UINT32  u4ReadStandStart, u4WriteStandStart;
	BOOL     fgRedo = FALSE;
	
	Printf("i4Graphics3D_Drv_Emu\r\n");

	i4Graphics3D_Drv_Init();

	vG3DEmuCfgInit();

	i4G3DAllocMemory();
	
#if T32_HOST
	x_memset((pG3dEmuList), (int)'\0', _LIST_FILE_SIZE_);
	u4EmuReadSize = _LIST_FILE_SIZE_;
	u4EmuFileSize = _LIST_FILE_SIZE_;

	fgRet = fgGPUReadPCFile((pG3dEmuList), _LIST_FILE_SIZE_, &ucEmuListName[0], 0, &u4EmuReadSize,&u4EmuFileSize);
#elif USB_G3D_VFY
	fgRet = fgReadFile(pG3dEmuList, _LIST_FILE_SIZE_, &ucEmuListName[0], 0, &i4EmuFd, &u4EmuReadSize, &u4EmuFileSize);
#else
	fgRet = fgG3DFATSFsReadFile(&ucEmuListName[0],pG3dEmuList,0,_LIST_FILE_SIZE_,&u4EmuReadSize,&u4EmuFileSize,&i4EmuFd);
#endif

   if (fgRet == 0)
   {
		Printf("[WARNING] READ Emul List FILEERROR!!!\r\n");
		//return 0;
		goto FINISHED;
   }

   u4PrsSz = 0;
   u4PatternCnt = 0;   
   
   while(u4PrsSz < u4EmuReadSize) 
   {         
       fgRedo = FALSE;
	   
	   memset( (void*) (line), 0, 256); 
	   memset( (void*) (cmd), 0, 256); 
	   memset( (void*) (str0), 0, 256);


	   fgG3DBufGetLine((CHAR*) pG3dEmuList, &u4PrsSz, line, u4EmuReadSize);
       line_number = line_number + 1;

       r = sscanf(line, "%s %s\r\n", cmd, str0);         

       if(r==1 && strcmp(cmd, "@")==0)
       { 
          //This line end of pattern list.
          Printf("End of Pattern List\n");
		  //vPrintCheckSumRegister();
          break;
       }
       else
       if(strcmp(cmd, "#")==0)
       { 
          //This line is comment. Do nothing.
          Printf("Skip Pattern\n");
       }
	   else
       if(strcmp(cmd, "")==0)
       { 
          //This line is comment. Do nothing.
          Printf("Skip NULL row!!!\n");
       }
       else
       {                         
             if (strcmp(cmd, "P")==0)
             {
				eG3dEmuConfig.fgIsMaliPPTest = TRUE;			
				eG3dEmuConfig.fgIsMaliGPTest = FALSE;
				strcpy(cmd, str0);
             }
			 else if(strcmp(cmd, "G")==0)
			 {
				 eG3dEmuConfig.fgIsMaliGPTest = TRUE;		 
				 eG3dEmuConfig.fgIsMaliPPTest = FALSE;			 
				 strcpy(cmd, str0);

			 }
             else
             {
				 eG3dEmuConfig.fgIsMaliPPTest = FALSE;			 
				 eG3dEmuConfig.fgIsMaliGPTest = FALSE;
             } 
			 
             Printf("PathName is : %s\n", cmd);
             strcpy(ucConfigPathName, cmd); 
#if T32_HOST
             sprintf(ucTemp, "\\");
#else
			 sprintf(ucTemp, "/");
#endif
             strcat(ucConfigPathName, ucTemp);
             //strcat(ucConfigPathName, "\\");
       
             strcpy(ucConfigStringName, ucConfigPathName);
             strcat(ucConfigStringName, ucConfigFileName);

             Printf("[3D] Parsing Config = %s\n", ucConfigStringName); 

             if (eG3dEmuConfig.fgIsAxiBrdgWROutStand)
             {
                 u4ReadStandStart = 13;
                 u4WriteStandStart = 0;
             }
             else
             {
                 u4ReadStandStart = MAX_READ_OUT_STAND -1 ;
                 u4WriteStandStart = MAX_WRITE_OUT_STAND - 1;
             }

             for (eG3dEmuConfig.u4ReadStand = u4ReadStandStart; eG3dEmuConfig.u4ReadStand < MAX_READ_OUT_STAND; eG3dEmuConfig.u4ReadStand++)
             {
                 for (eG3dEmuConfig.u4WriteStand = u4WriteStandStart; eG3dEmuConfig.u4WriteStand < MAX_WRITE_OUT_STAND; eG3dEmuConfig.u4WriteStand++)
                 {

#if T32_HOST
						x_memset(pConfigBuffer, (int)'\0', _CONFIG_FIE_SIZE_);
						u4ConfigFileSize = _CONFIG_FIE_SIZE_;
						u4ConfigReadSize = _CONFIG_FIE_SIZE_;
						fgRet = fgGPUReadPCFile(pConfigBuffer, _CONFIG_FIE_SIZE_, &ucConfigStringName[0], 0, &u4ConfigReadSize, &u4ConfigFileSize);
						
#elif USB_G3D_VFY   
						fgRet = fgReadFile(pConfigBuffer, _CONFIG_FIE_SIZE_, &ucConfigStringName[0], 0, &i4ConfigFd, &u4ConfigReadSize, &u4ConfigFileSize);
#else
						fgRet = fgG3DFATSFsReadFile(&ucConfigStringName[0],pConfigBuffer,0,_CONFIG_FIE_SIZE_,&u4ConfigReadSize,&u4ConfigFileSize,&i4ConfigFd);
#endif

                    if (!fgRet)
                    {
                         Printf("Read %s file FAIL!!\n", ucConfigFileName);
                    }

                    fgIsPPOutBuff = FALSE;
                    pPPOutBuffAddr = NULL;
                    u4PPOutBuffBase = 0x0;
                    vG3DEmuParseConfig ( pConfigBuffer, u4ConfigReadSize, &ucConfigPathName[0], fgRedo);
                    fgRedo = FALSE;
                    //x_thread_delay(10);
                    u4PatternCnt++;
                }                 
             }
        }
   }


FINISHED:
	
   #ifdef CONFIG_TV_DRV_VFY 
   BSP_FreeVfyReserveMemory((UINT32)pcStaticTotalBuffer);
   #endif

   
   return 0;
}


#endif
