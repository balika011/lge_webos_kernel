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
#include "vdec_hal_if_common.h"
#include "vdec_hw_common.h"
#include "x_typedef.h"
#include "x_ckgen.h"
#include "drv_config.h"
#include "x_debug.h"

#ifdef CONFIG_TV_DRV_VFY 
#include "vdec_debug.h"
#endif // CONFIG_TV_DRV_VFY

#if (CONFIG_DRV_LINUX_DATA_CONSISTENCY)
#include <config/arch/chip_ver.h>
#endif

#if CONFIG_SECTION_BUILD_LINUX_KO
#include <mach/irqs.h>
//#include <mach/mt8530.h>

#if (CONFIG_DRV_VERIFY_SUPPORT)
#include "vdec_verify_mpv_prov.h"
#endif


#define MUSB_BASE3                        (IO_VIRT + 0xF000)
#define MUSB_ANAPHYBASE                    (0x700)

#define M_REG_AUTOPOWER                      0x80
#define M_REG_AUTOPOWER_DRAMCLK         0x01
#define M_REG_AUTOPOWER_ON                     0x02
#define M_REG_AUTOPOWER_PHYCLK             0x04
#define MUSB_BASE                       (IO_VIRT + 0xE000)
#define MUSB_PHYBASE                           (0x400)
#define MUSB_MISCBASE                     (0x600)

#define MGC_PHY_Read32(_pBase, r)      \
    *((volatile UINT32 *)(((UINT32)_pBase) + (MUSB_PHYBASE)+ (r)))
#define MGC_PHY_Write32(_pBase, r, v)  \
    (*((volatile UINT32 *)((((UINT32)_pBase) + MUSB_PHYBASE)+ (r))) = v)
#define MGC_MISC_Read32(_pBase, r)     \
    *((volatile UINT32 *)(((UINT32)_pBase) + (MUSB_MISCBASE)+ (r)))
#define MGC_MISC_Write32(_pBase, r, v) \
    (*((volatile UINT32 *)(((UINT32)_pBase) + (MUSB_MISCBASE)+ (r))) = v)
#define MGC_AnaPhy_Read32(_pBase, _offset) \
    (*((volatile UINT32 *)(((UINT32)_pBase) + MUSB_ANAPHYBASE + _offset)))
#define MGC_AnaPhy_Write32(_pBase, _offset, _data) \
    (*((volatile UINT32 *)(((UINT32)_pBase) + MUSB_ANAPHYBASE + _offset)) = _data)
#endif

#define VDEC_VLD_USE_USB  0
// **************************************************************************
// Function : INT32 i4VDEC_HAL_Common_Init(UINT32 u4ChipID);
// Description : Turns on video decoder HAL
// Parameter : u4ChipID
// Return      : >0: init OK.
//                  <0: init failed
// **************************************************************************
INT32 i4VDEC_HAL_Common_Init(UINT32 u4ChipID)
{
#if (!CONFIG_DRV_FPGA_BOARD)

    // reset common SRAM
    vVDecWriteVLD(0, RW_VLD_RESET_COMMOM_SRAM, 0x00000000);
    vVDecWriteVLD(0, RW_VLD_RESET_COMMOM_SRAM, 0x00000100);
    vVDecWriteVLD(0, RW_VLD_RESET_COMMOM_SRAM, 0x00000000);
    vVDecWriteVLD(1, RW_VLD_RESET_COMMOM_SRAM, 0x00000000);
    vVDecWriteVLD(1, RW_VLD_RESET_COMMOM_SRAM, 0x00000100);
    vVDecWriteVLD(1, RW_VLD_RESET_COMMOM_SRAM, 0x00000000);
#endif

    return HAL_HANDLE_OK;
}

#if (!CONFIG_DRV_FPGA_BOARD) && (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
void vVDEC_HAL_CLK_Set(UINT32 u4CodeType)
{
    vVDecSetVldMcClk(0,u4CodeType);
}
#endif

#if VDEC_REMOVE_UNUSED_FUNC
// **************************************************************************
// Function : INT32 i4VDEC_HAL_Common_Uninit(void);
// Description : Turns off video decoder HAL
// Parameter : void
// Return      : >0: uninit OK.
//                  <0: uninit failed
// **************************************************************************
INT32 i4VDEC_HAL_Common_Uninit(void)
{
    return HAL_HANDLE_OK;
}
#endif

#if 0
/******************************************************************************
* Global Function
******************************************************************************/
void MPV_PllInit(void)
{
       UINT32 u4DramClk;
       UINT16 u2MS, u2NS, u2Counter;
       UINT8 u1Band;
       UINT8 ucDelay;

       u4DramClk = BSP_GetClock(CAL_SRC_DMPLL, &u1Band, &u2MS, &u2NS,
            &u2Counter);

       UTIL_Printf("DRAM CLK: %d\n", u4DramClk);
/*
	// set vpll test_gnd = 1, bs = 4, bw = 2, gs = 2, cpi = 8 (default)
	// 148.5MHz
	*((UINT32*)0x2000D030) = (UINT32)0x00082528;
*/
	ucDelay = 0;
	*((UINT32*)0x2000D03c) = (*((UINT32*)0x2000D03c) & 0xC0FFC0FF) | ((ucDelay<<24)|(ucDelay<<8));
	*((UINT32*)0x2000D040) = (*((UINT32*)0x2000D040) & 0xFFFFC0FF) | (ucDelay<<8);

	// set mdec_ck = ck_vpll (default)
	*((UINT32*)0x2000D048) = (*((UINT32*)0x2000D048) & 0xFF00FFFF) | 0x00010000;
	*((UINT32*)0x20007054) = (*((UINT32*)0x20007054) & 0xFFFF000F) | 0x0000FFF0;


}
#endif

#if VDEC_REMOVE_UNUSED_FUNC
// **************************************************************************
// Function : UINT32 u4VDEC_HAL_Common_GetHWResourceNum(UINT32 *pu4BSNum, UINT32 *pu4VLDNum);
// Description :Get hardware resource number
// Parameter : pu4BSNum : Pointer to barrel shifter number of every VLD
//                 pu4VLDNum : Pointer to VLD number
// Return      : None
// **************************************************************************
void vVDEC_HAL_Common_GetHWResourceNum(UINT32 *pu4BSNum, UINT32 *pu4VLDNum)
{
    *pu4BSNum = 2;
    *pu4VLDNum = 2;
  
    return;
}
#endif

// **************************************************************************
// Function : void vDEC_HAL_COMMON_SetVLDPower(UINT32 u4VDecID, BOOL fgOn);
// Description :Turn on or turn off VLD power
// Parameter : u4VDecID : video decoder hardware ID
//                 fgOn : Flag to vld power on or off
// Return      : None
// **************************************************************************
void vDEC_HAL_COMMON_SetVLDPower(UINT32 u4VDecID, BOOL fgOn)
{
    if (fgOn)
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_PWRSAVE, 0);
    }
    else
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_PWRSAVE, 1);
    }
    return;
}


// **************************************************************************
// Function : void vDEC_HAL_COMMON_PowerOn(UINT32 u4VDecID, BOOL fgOn);
// Description :Turn on or turn off VLD power
// Parameter : u4VDecID : video decoder hardware ID
//                 fgOn : Flag to vld power on or off
// Return      : None
// **************************************************************************
void vDEC_HAL_COMMON_PowerOn(void)
{    
#if 0
    CKGEN_AgtOnClk(e_CLK_VDEC);
    CKGEN_AgtOnClk(e_CLK_MC);
#endif    
    return;
}

// **************************************************************************
// Function : void vDEC_HAL_COMMON_PowerOff(UINT32 u4VDecID, BOOL fgOn);
// Description :Turn on or turn off VLD power
// Parameter : u4VDecID : video decoder hardware ID
//                 fgOn : Flag to vld power on or off
// Return      : None
// **************************************************************************
void vDEC_HAL_COMMON_PowerOff(void)
{    
#if 0
    CKGEN_AgtOffClk(e_CLK_VDEC);
    CKGEN_AgtOffClk(e_CLK_MC);
#endif    
    return;
}



INT32 i4VDEC_HAL_Dram_Busy( UINT32 u4ChipID, UINT32 u4StartAddr, UINT32 u4Offset)
{

    vWriteReg(0x7210, (u4StartAddr << 4));
    vWriteReg(0x7214, (u4Offset << 4));
    vWriteReg(0x7104, 0x0);    
    vWriteReg(0x7218, 0x8e0f110d);    
    return 0;
}

INT32 i4VDEC_HAL_Dram_Busy_Off( UINT32 u4ChipID, UINT32 u4StartAddr, UINT32 u4Offset)
{

    vWriteReg(0x7210, (u4StartAddr << 4));
    vWriteReg(0x7214, (u4Offset << 4));
    //vWriteReg(0x7104, 0x0);    
    vWriteReg(0x7218, 0x860f110d);    
    return 0;
}


#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) && CONFIG_DRV_FTS_SUPPORT)
INT32 vDEC_HAL_COMMON_ReadLBDResult(UINT32 ucMpvId, UINT32* u4YUpbound, 
    UINT32* u4YLowbound, UINT32* u4CUpbound, UINT32* u4CLowbound)
{
    UINT32 u4YResult, u4CResult;

    
    vVDECReadLetetrBoxDetResult(ucMpvId, &u4YResult, &u4CResult);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
    *u4YUpbound = (u4YResult&0xFFF);
    *u4YLowbound = ((u4YResult >> 16)&0xFFF);
    *u4CUpbound = (u4CResult&0xFFF);
    *u4CLowbound = ((u4CResult >> 16)&0xFFF);
#else
    *u4YUpbound = (u4YResult&0x7FF);
    *u4YLowbound = ((u4YResult >> 16)&0x7FF);
    *u4CUpbound = (u4CResult&0x7FF);
    *u4CLowbound = ((u4CResult >> 16)&0x7FF);
#endif
    return 0;
}
#endif

#if BANDWIDTH_MEASURE
void vVDEC_HAL_BANDWID_MEAS_DumpReg(UINT32 u4VDecID)
{
	UTIL_Printf("[BANDWIDTH_MEASURE]:%d,%d,%d,%d,%d,%d,%d\n",
				u4VDecReadMC(u4VDecID,0x770),
				u4VDecReadMC(u4VDecID,0x8b8),
				u4VDecReadMC(u4VDecID,0x28),
				u4VDecReadMC(u4VDecID,0x2c),
				u4VDecReadMC(u4VDecID,0x9e0),
				u4VDecReadMC(u4VDecID,0x9e4),
		        (u4VDecReadMISC(u4VDecID,0xf0)&1));
}
#endif
#if PARA8_BEHAVIOR_TEST
void vVDEC_HAL_PARA8_BEHAVIOR_DumpReg(UINT32 u4VDecID)
{
	//UTIL_Printf("NBM_DLE_NUM,ESA_REQ_DATA_NUM,MC_REQ_DATA_NUM,MC_MBX,MC_MBY,CYC_SYS,INTRA_CNT,LAT_BUF_BYPASS,CYCLE_DRAM,Y_BLK_CNT,C_BLK_CNT,WAIT_CNT,PARA8_BEHAVIOR\n");
	UTIL_Printf("[PARA8_BEHAVIOR]:%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
				u4VDecReadMC(u4VDecID,0x770),
				u4VDecReadMC(u4VDecID,0x8b8),
				u4VDecReadMC(u4VDecID,0xa28),
				u4VDecReadMC(u4VDecID,0x28),
				u4VDecReadMC(u4VDecID,0x2c),
				u4VDecReadMC(u4VDecID,0x9e0),
				u4VDecReadMC(u4VDecID,0x9e4),
		        (u4VDecReadMISC(u4VDecID,0xf0)&1),
		        u4VDecReadMC(u4VDecID,0x778),
		        u4VDecReadMC(u4VDecID,0x9e8),
		        u4VDecReadMC(u4VDecID,0x9ec),
		        u4VDecReadMC(u4VDecID,0x9f0),
		        u4VDecReadMC(u4VDecID,0x998));
}
#endif

#if PERFORMANCE_TEST
// **************************************************************************
// Function : void v4VDEC_Get_Performance_Reg(UINT32 u4VDecID);
// Description :Read reg in interrupt
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void v4VDEC_Get_Performance_Reg(UINT32 u4VDecID)
{
    extern UINT32 NBM_DLE_NUM;
    extern UINT32 ESA_REQ_DATA_NUM;
    extern UINT32 MC_REQ_DATA_NUM;
    extern UINT32 MC_MBX;
    extern UINT32 MC_MBY;
    extern UINT32 CYC_SYS;
    extern UINT32 INTRA_CNT;
    extern UINT32 LAT_BUF_BYPASS;
    extern UINT32 Y_BLK_CNT;
    extern UINT32 C_BLK_CNT;
    extern UINT32 WAIT_CNT;    
    extern UINT32 REQ_CNT;    
    extern UINT32 MC_DLE_NUM;       
    extern UINT32 CYCLE_DRAM;           
    extern UINT32 BITRATE_PIC;       
    NBM_DLE_NUM = u4VDecReadMC(u4VDecID, 0x770);
    ESA_REQ_DATA_NUM = u4VDecReadMC(u4VDecID, 0x8b8);
    MC_REQ_DATA_NUM = u4VDecReadMC(u4VDecID, 0xa28);
    MC_MBX = u4VDecReadMC(u4VDecID, 0x28);
    MC_MBY = u4VDecReadMC(u4VDecID, 0x2C);
    CYC_SYS = u4VDecReadMC(u4VDecID, 0x9e0);
    INTRA_CNT = u4VDecReadMC(u4VDecID, 0x9e4);
    LAT_BUF_BYPASS = u4VDecReadVLDTOP(u4VDecID, 0xf0);
    Y_BLK_CNT = u4VDecReadMC(u4VDecID, 0x9e8);
    C_BLK_CNT = u4VDecReadMC(u4VDecID, 0x9ec);
    WAIT_CNT = u4VDecReadMC(u4VDecID, 0x9f0);
    REQ_CNT =  u4VDecReadMC(u4VDecID, 0x7b4);
    MC_DLE_NUM = u4VDecReadMC(u4VDecID, 0x774);
    CYCLE_DRAM = u4VDecReadMC(u4VDecID, 0x778);
//    BITRATE_PIC = u4VDecReadMC(u4VDecID, 0x9ec);
}

// **************************************************************************
// Function : void v4VDEC_Dump_Performance_Reg(UINT32 u4VDecID);
// Description :Dump reg after interrupt
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void v4VDEC_Dump_Performance_Reg()
{
    extern UINT32 NBM_DLE_NUM;
    extern UINT32 ESA_REQ_DATA_NUM;
    extern UINT32 MC_REQ_DATA_NUM;
    extern UINT32 MC_MBX;
    extern UINT32 MC_MBY;
    extern UINT32 CYC_SYS;
    extern UINT32 INTRA_CNT;
    extern UINT32 LAT_BUF_BYPASS;
    extern UINT32 Y_BLK_CNT;
    extern UINT32 C_BLK_CNT;
    extern UINT32 WAIT_CNT;    
    extern UINT32 REQ_CNT;    
    extern UINT32 MC_DLE_NUM;       
    extern UINT32 CYCLE_DRAM;           
    extern UINT32 BITRATE_PIC;      

    
    UTIL_Printf("NBM_DLE_NUM=%d,ESA_REQ_DATA_NUM=%d,MC_REQ_DATA_NUM=%d\n",
        NBM_DLE_NUM,ESA_REQ_DATA_NUM,MC_REQ_DATA_NUM);

    UTIL_Printf("MC_MBX=%d,MC_MBY=%d,CYC_SYS=%d\n",
        MC_MBX,MC_MBY,CYC_SYS);

    UTIL_Printf("INTRA_CNT=%d,LAT_BUF_BYPASS=%d,Y_BLK_CNT=%d\n",
        INTRA_CNT,LAT_BUF_BYPASS,Y_BLK_CNT);

    UTIL_Printf("C_BLK_CNT=%d,WAIT_CNT=%d,REQ_CNT=%d\n",
        C_BLK_CNT,WAIT_CNT,REQ_CNT);
    
    UTIL_Printf("MC_DLE_NUM=%d,CYCLE_DRAM=%d,BITRATE_PIC=%d\n",
        MC_DLE_NUM,CYCLE_DRAM,BITRATE_PIC);

    //,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n");

    //clear this value
    NBM_DLE_NUM = 0xa5;
    ESA_REQ_DATA_NUM = 0xa5;
    MC_REQ_DATA_NUM =  0xa5;
    MC_MBX = 0xa5;
    MC_MBY = 0xa5;
    CYC_SYS = 0xa5;
    INTRA_CNT = 0xa5;    
    LAT_BUF_BYPASS = 0xa5;   
    Y_BLK_CNT = 0xa5;    
    C_BLK_CNT = 0xa5;
    WAIT_CNT = 0xa5;
    REQ_CNT = 0xa5;
    MC_DLE_NUM = 0xa5;
    CYCLE_DRAM = 0xa5;
    BITRATE_PIC = 0xa5;
    
}

#endif

#if (VDEC_SUPPORT_IOMMU)
void vDEC_HAL_COMMON_PrintfMMUStatus(void)
{
#if 1
     UINT32 u4RegVal = 0;
     u4RegVal = u4VDecReadIOMMU (0, RW_IOMMU_CFG0);
     UTIL_Printf("[VDEC_VFY] RW_IOMMU_CFG0 value is %x . \n",u4RegVal);
     u4RegVal = u4VDecReadIOMMU (0, RW_IOMMU_CFG1);
     UTIL_Printf("[VDEC_VFY] RW_IOMMU_CFG1 value is %x . \n",u4RegVal);
     u4RegVal = u4VDecReadIOMMU (0, RW_IOMMU_CFG2);
     UTIL_Printf("[VDEC_VFY] RW_IOMMU_CFG2 value is %x . \n",u4RegVal);
     u4RegVal = u4VDecReadIOMMU (0, RW_IOMMU_CFG3);
     UTIL_Printf("[VDEC_VFY] RW_IOMMU_CFG3 value is %x . \n",u4RegVal);
	 u4RegVal = u4VDecReadIOMMU (0, RW_IOMMU_CFG4);
     UTIL_Printf("[VDEC_VFY] RW_IOMMU_CFG4 value is %x . \n",u4RegVal);
#endif
}
void vDEC_HAL_COMMON_IOMMUInit(ENUM_VDEC_FMT_T eVDecType)
{
       if (VDEC_FMT_VP8 == eVDecType)
       {
           vVDecWriteIOMMU(0, RW_IOMMU_CFG0, 0xFF);
           vVDecWriteIOMMU(0, RW_IOMMU_CFG1, u4HalGetTTB1());
           vVDecWriteIOMMU(0, RW_IOMMU_CFG2, 0x001b0013);
           vVDecWriteIOMMU(0, RW_IOMMU_CFG3, 0x0000001d);
           vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0600000a);
           x_thread_delay(2);
           vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0010000a);
           x_thread_delay(20);
       	}
	    else if(VDEC_FMT_VP6 == eVDecType)
    	{
            vVDecWriteIOMMU(0, RW_IOMMU_CFG0, 0xFF);
    	    vVDecWriteIOMMU(0, RW_IOMMU_CFG1, u4HalGetTTB1());
    	    vVDecWriteIOMMU(0, RW_IOMMU_CFG2, 0x001d001b);
            vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0600000a);
            x_thread_delay(2);
            vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0010000a);
    	    x_thread_delay(20);

    	}
		else
    	{
            vVDecWriteIOMMU(0, RW_IOMMU_CFG0, 0xF);
    	    vVDecWriteIOMMU(0, RW_IOMMU_CFG1, u4HalGetTTB1());
    	    vVDecWriteIOMMU(0, RW_IOMMU_CFG2, 0x001d001b);
            vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0600000a);
            x_thread_delay(2);
            vVDecWriteIOMMU(0, RW_IOMMU_CFG4, 0x0010000a);
    	    x_thread_delay(20);
    	}

}
void vDEC_HAL_COMMON_SetMMUUsrTable(void)
{
}
void vDEC_HAL_COMMON_MMUFlush(void)
{
	UINT32 u4VdeRegVal;
    u4VdeRegVal = u4VDecReadIOMMU(0, RW_IOMMU_CFG4);
    vVDecWriteIOMMU(0, RW_IOMMU_CFG4, u4VdeRegVal|0x80000000);
	while (u4VDecReadIOMMU(0, RW_IOMMU_CFG8)&0x20000000);
#if 0
	UINT32 u4PngRegVal;                
	u4PngRegVal = IOMMU_READ32 (REG_RW_IOMMU_CFG4, IOMMU_PNG);	//
    IOMMU_WRITE32(REG_RW_IOMMU_CFG4, IOMMU_PNG,    u4PngRegVal|0x80000000);////////8, for self fire
    #if (CONFIG_CHIP_VER_CURR >CONFIG_CHIP_VER_MT8560)
        while (IOMMU_READ32 (REG_RW_IOMMU_CFG8, IOMMU_PNG)&0x20000000);
   #else 
	while (IOMMU_READ32 (REG_RW_IOMMU_CFG4, IOMMU_PNG)&0x10000000);
    #endif 
#endif
}
void vCheckAddress(UINT32 u4Addr)
{
    if(!(u4Addr & (~0x3FFFFFFF)))
    {
        UTIL_Printf("!!!Address %x Fail Before Programming!!!\n",u4Addr);
        ASSERT(0);
    }
}
#if (CONFIG_CHIP_VER_CURR >CONFIG_CHIP_VER_MT8560)
void vDEC_HAL_COMMON_MMUEnable(void)
{
#if 0
        UINT32 u4Val =0;
	u4Val = PngReadREG(0x4E00, 0x04);
	PngWriteREG(0x4E00,0x04,u4Val|2); 		///FOR PNG to MMU enable
#endif
}
#endif
#endif
