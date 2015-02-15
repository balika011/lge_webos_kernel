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
/*
*
*
* \par Project
*    MT8520
*
* \par Description
*    Jpeg Decoder HAL interface definition.
*
* \par Author_Name
*    CK Hu
*
* \par Last_Changed
* $Author: p4admin $
* $Modtime: $
* $Revision: #1 $
*
*/

// *********************************************************************
// Memo
// *********************************************************************
/*
*/

#include "x_hal_5381.h"
#include "x_bim.h"
#include "jdec_hal.h"
#include "jdec_hal_errcode.h"
#include "jdec_hal_if.h"
#include "jdec_com_jfif.h"
#include "x_printf.h"
#include "jdec_debug.h"
UINT8 _fgEnableRegisterLog = 0;
BOOL  _fgForceLogEnable = FALSE;
BOOL _fgEnableSimLog = FALSE;
BOOL _debug_decode_done_log = FALSE;
#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#else
#include "x_mem_phase2.h"
#endif
#if JDEC_HAL_SUPPORT_MB
#include <asm/system.h>
#endif

#define VLD0_BASE   JPG_VLD_BASE

// In 8520 first version,
// Jpeg Huffman table and non-zero hiscan can not access DRAM over 16M,
// In ECO version should fix this bug
//#define JDEC_HAL_DRAM_16M_LIMIT 0 // verify in 8530

/*
const static UINT32 u4HwJdecVldOffset[HW_JDEC_VLD_NUM] = {
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    VLD1_REG_OFFSET
#else
    VLD0_REG_OFFSET
#endif
};


const static UINT32 u4HwJdecMcOffset[HW_JDEC_MC_NUM] = {
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    MC1_REG_OFFSET
#else
    MC0_REG_OFFSET
#endif
};

*/
#if JDEC_HW_LOG_ENABLE
UINT32 _au4JdecHwCmd[1024][2];
UINT32 _u4JdecHwCmdIdx = 0;
#endif
 
#define JDEC_CBCR_ALIGN_WITH_Y 1
INLINE void vHwJdecWrite32(UINT32 u4HwId,UINT32 addr,UINT32 value)
{ 
#if 0	
    //*(volatile UINT32 *)(IO_BASE + u4HwJdecVldOffset[u4HwId] + addr ) = value;
    //if(addr != 0x3D8 && addr != 0x118)
    JDEC_SIM_HAL_LOG("WriteREG(VLD_REG_OFST + 4*%d (%x), 32h'%x)\n", (addr/4), addr, value);
    #if JDEC_HW_LOG_ENABLE
    if (_u4JdecHwCmdIdx == 0)
    {
        if (addr == _au4JdecHwCmd[1023][0])
        {
            return;
        }
    }
    else
    {
        if (addr == _au4JdecHwCmd[_u4JdecHwCmdIdx-1][0])
        {
            return;
        }
    }
    _au4JdecHwCmd[_u4JdecHwCmdIdx][0] = addr;
    _au4JdecHwCmd[_u4JdecHwCmdIdx][1] = value;
    _u4JdecHwCmdIdx ++;
    if (_u4JdecHwCmdIdx > 1023)
    {
        _u4JdecHwCmdIdx = 0;
    }
    #endif  
 #else 
    JDEC_SIM_HAL_LOG("RISCWrite(JPGDEC_VLD_ADDR + 4*%d (%x), 32h'%x)\n", (addr/4), addr, value);
   	IO_WRITE32(VLD0_BASE, (addr), value); 
 #endif   
  
}


INLINE UINT32 u4HwJdecRead32(UINT32 u4HwId,UINT32 addr)
{
	#if 0
    #if JDEC_HW_LOG_ENABLE
    JDEC_SIM_HAL_LOG("ReadREG(VLD_REG_OFST + 4*%d (%x))\n", (addr/4), addr);
    
    if (_u4JdecHwCmdIdx == 0)
    {
        if ((addr |((UINT32)0x1 << 31))  != _au4JdecHwCmd[1023][0])
        {
            _au4JdecHwCmd[_u4JdecHwCmdIdx][0] = addr | ((UINT32)0x1 << 31);
            _au4JdecHwCmd[_u4JdecHwCmdIdx][1] = 0;
            _u4JdecHwCmdIdx ++;
            if (_u4JdecHwCmdIdx > 1023)
            {
                _u4JdecHwCmdIdx = 0;
            }
        }
    }
    else
    {
        if ((addr |((UINT32)0x1 << 31)) != _au4JdecHwCmd[_u4JdecHwCmdIdx-1][0])
        {
            _au4JdecHwCmd[_u4JdecHwCmdIdx][0] = addr | ((UINT32)0x1 << 31);
            _au4JdecHwCmd[_u4JdecHwCmdIdx][1] = 0;
            _u4JdecHwCmdIdx ++;
            if (_u4JdecHwCmdIdx > 1023)
            {
                _u4JdecHwCmdIdx = 0;
            }
        }
    }
    
    #endif
    return *(volatile UINT32 *)(IO_BASE + u4HwJdecVldOffset[u4HwId] + addr);
   #else   
      JDEC_SIM_HAL_LOG("RISCReaad(JPGDEC_VLD_ADDR + 4*%d (%x))\n", (addr/4), addr);
     return  IO_READ32(VLD0_BASE, (addr));
   #endif  
}


//INLINE void vHwJdecMcWrite32(UINT32 u4HwId,UINT32 addr,UINT32 value)
//{
////
    //*(volatile UINT32 *)(IO_BASE + u4HwJdecMcOffset[u4HwId] + addr ) = value;
//    IO_WRITE32(VLD0_BASE, (addr), u4Value);
//}


//INLINE UINT32 u4HwJdecMcRead32(UINT32 u4HwId,UINT32 addr)
//{
//    return *(volatile UINT32 *)(IO_BASE + u4HwJdecMcOffset[u4HwId] + addr);
//}
UINT32 _u4CurMcuRow_num =0;
void vHwJdecPrintRegister(UINT32 u4HwId)
{
    UINT32 u4Base = 0xB0;
    UINT32 u4Temp = 0;
	/*if(!_fgForceLogEnable)
	{
      if(!_fgEnableRegisterLog)
		 return;
	}*/
    UTIL_Printf("Enter %s, mcu row[%d]!\n", __FUNCTION__,_u4CurMcuRow_num);
	//UTIL_Printf("0xf0008048 =0x%x\n",*(volatile UINT32 *)(0xf0008048));
    //UTIL_Printf("0xf000804c =0x%x\n",*(volatile UINT32 *)(0xf000804c));
    for (u4Temp = 0; u4Temp < 252; u4Temp ++)
    {
        if ((u4Temp % 4) == 0)
        {
            UTIL_Printf("[0x%8x]",0xF003E000+u4Base + u4Temp * 4);
        }
        UTIL_Printf("0x%8x   ",u4HwJdecRead32(u4HwId,(u4Base + u4Temp * 4)));
        if ((u4Temp % 4) == 3)
        {
            UTIL_Printf("\n");
        }
    }
    #if 0//JDEC_IO_MMU_TEST
    UTIL_Printf("[0xFD05F0C0]0x%x , 0x%x, 0x%x, 0x%x \n", JPEG_MMU_READ32(0x0),JPEG_MMU_READ32(0x4),JPEG_MMU_READ32(0x8),JPEG_MMU_READ32(0xC));
	UTIL_Printf("[0xFD05F0D0]0x%x , 0x%x, 0x%x, 0x%x \n", JPEG_MMU_READ32(0x10),JPEG_MMU_READ32(0x14),JPEG_MMU_READ32(0x18),JPEG_MMU_READ32(0x1C));
	UTIL_Printf("[0xFD05F0E0]0x%x , 0x%x, 0x%x, 0x%x \n", JPEG_MMU_READ32(0x20),JPEG_MMU_READ32(0x24),JPEG_MMU_READ32(0x28),JPEG_MMU_READ32(0x2C));
	UTIL_Printf("[0xFD05F0F0]0x%x , 0x%x, 0x%x, 0x%x \n", JPEG_MMU_READ32(0x30),JPEG_MMU_READ32(0x34),JPEG_MMU_READ32(0x38),JPEG_MMU_READ32(0x3C));
    #if 0
    UTIL_Printf("[0xFD05F0C0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x0, IOMMU_JPG),IOMMU_READ32(0x4, IOMMU_JPG),IOMMU_READ32(0x8, IOMMU_JPG),IOMMU_READ32(0xC, IOMMU_JPG));
	UTIL_Printf("[0xFD05F0D0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x10, IOMMU_JPG),IOMMU_READ32(0x14, IOMMU_JPG),IOMMU_READ32(0x18, IOMMU_JPG),IOMMU_READ32(0x1C, IOMMU_JPG));
	UTIL_Printf("[0xFD05F0E0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x20, IOMMU_JPG),IOMMU_READ32(0x24, IOMMU_JPG),IOMMU_READ32(0x28, IOMMU_JPG),IOMMU_READ32(0x2C, IOMMU_JPG));
	UTIL_Printf("[0xFD05F0F0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x30, IOMMU_JPG),IOMMU_READ32(0x34, IOMMU_JPG),IOMMU_READ32(0x38, IOMMU_JPG),IOMMU_READ32(0x3C, IOMMU_JPG));
    #endif
    #endif

}


 void vHwJdecPrintRegister_ex(UINT32 u4HwId)
{
#if 0
    UINT32 u4Base = 0x220;
    UINT32 u4Temp = 0;

    UTIL_Printf("Enter %s!\n", __FUNCTION__);
/*
	u4Base = 0xb0;

	for (u4Temp = 0; u4Temp < 28; u4Temp ++)
	{
		   if ((u4Temp % 4) == 0)
		   {
			   UTIL_Printf("[0x%8x]",0xF003E000+u4Base + u4Temp * 4);
		   }
		   UTIL_Printf("0x%8x	",u4HwJdecRead32(u4HwId,(u4Base + u4Temp * 4)));
		   if ((u4Temp % 4) == 3)
		   {
			   UTIL_Printf("\n");
		   }
	}
	*/
	u4Base = 0x220;
    for (u4Temp = 0; u4Temp < 8; u4Temp ++)
    {
        if ((u4Temp % 4) == 0)
        {
            UTIL_Printf("[0x%8x]",0xF003E000+u4Base + u4Temp * 4);
        }
        UTIL_Printf("0x%8x   ",u4HwJdecRead32(u4HwId,(u4Base + u4Temp * 4)));
        if ((u4Temp % 4) == 3)
        {
            UTIL_Printf("\n");
        }
    }
    UTIL_Printf("[0xf003e278] 0x%8x\n",u4HwJdecRead32(u4HwId,0x278));
	UTIL_Printf("[0xf003e144] 0x%8x\n",u4HwJdecRead32(u4HwId,0x144));
	
    #if JDEC_IO_MMU_TEST
    UTIL_Printf("[0xFD05F0C0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x0, IOMMU_JPG),IOMMU_READ32(0x4, IOMMU_JPG),IOMMU_READ32(0x8, IOMMU_JPG),IOMMU_READ32(0xC, IOMMU_JPG));
	UTIL_Printf("[0xFD05F0D0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x10, IOMMU_JPG),IOMMU_READ32(0x14, IOMMU_JPG),IOMMU_READ32(0x18, IOMMU_JPG),IOMMU_READ32(0x1C, IOMMU_JPG));
	UTIL_Printf("[0xFD05F0E0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x20, IOMMU_JPG),IOMMU_READ32(0x24, IOMMU_JPG),IOMMU_READ32(0x28, IOMMU_JPG),IOMMU_READ32(0x2C, IOMMU_JPG));
	UTIL_Printf("[0xFD05F0F0]0x%x , 0x%x, 0x%x, 0x%x \n", IOMMU_READ32(0x30, IOMMU_JPG),IOMMU_READ32(0x34, IOMMU_JPG),IOMMU_READ32(0x38, IOMMU_JPG),IOMMU_READ32(0x3C, IOMMU_JPG));
    #endif
#endif	

} 
void vHwJdecVLDPowerOn(UINT32 u4HwId,BOOL fgPowerOn)
{
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(fgPowerOn)
        vHwJdecWrite32(u4HwId,RW_VLD_PWRSAVE,0);
    else
      {
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
        vHwJdecWrite32(u4HwId,RW_VLD_PWRSAVE,1);
#else
        vHwJdecWrite32(u4HwId,RW_VLD_PWRSAVE,3);mb();
        #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8561) 
        #if JDEC_IO_MMU_TEST
        JDEC_VERY_HAL_LOG("JDEC_MMU ENABLE------------------\n");
        vHwJdecWrite32(u4HwId,RW_VLD_PWRSAVE,0x10013);mb();
        #endif
        #endif
      }
#endif
       //JDEC_VERY_HAL_LOG("u4HwJdecRead32(RW_VLD_PWRSAVE)  =0x%x\n",u4HwJdecRead32(u4HwId,RW_VLD_PWRSAVE));
}


void vHwJdecHwReset(UINT32 u4HwId)
{
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560) 
    UINT32 u4Reg;
#endif
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
   vHwJdecWrite32(u4HwId,WO_VLD_SW_RESET, (VLD_SW_RESET_CLEAR|VLD_SW_RESET_RESET |VLD_LARB_RESET | VLD_ASYNC_FIFO_RST | VLD_IOMMU_RST));
#else
    vHwJdecWrite32(u4HwId,WO_VLD_SW_RESET, VLD_SW_RESET_RESET);
#endif

#if JDEC_HAL_SUPPORT_MB
    mb();
#endif
    x_thread_delay(2);
    vHwJdecWrite32(u4HwId,WO_VLD_SW_RESET, VLD_SW_RESET_CLEAR);mb();
}

#if JDEC_MM_MODE_ENABLE
void vHwJdecHwResetMM(UINT32 u4HwId)
{
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560) 
    UINT32 u4Reg;
#endif
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
	vHwJdecWrite32(u4HwId,WO_VLD_SW_RESET, (VLD_SW_RESET_CLEAR|VLD_SW_RESET_RESET |VLD_LARB_RESET | VLD_ASYNC_FIFO_RST));
#else
    vHwJdecWrite32(u4HwId,WO_VLD_SW_RESET, VLD_SW_RESET_RESET);
#endif
    mb();
    vHwJdecWrite32(u4HwId,WO_VLD_SW_RESET, VLD_SW_RESET_CLEAR);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520) 

  #if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560) 
    u4Reg = u4HwJdecRead32(u4HwId,RW_VLD_AMSR);
    u4Reg |= VLD_CPFE_EN;
    u4Reg &= 0xFFFFFFFB;
    vHwJdecWrite32(u4HwId,RW_VLD_AMSR,u4Reg);
#endif
#endif
}
#endif

void vHwPrintChkSum(u4HwId)
{
    UINT32 u4ImgReszCheckSum;
    UTIL_Printf("vHwPrintChkSum\n");
    UTIL_Printf("Bs dram in chksum:0x%x\n",u4HwJdecRead32(u4HwId,  VLDREG_JPEG_BS_DDRAM_IN_CHKSUM));    
    UTIL_Printf("input window chksum:0x%x\n",u4HwJdecRead32(u4HwId,  VLDREG_JPEG_INPUT_CHKSUM));
    UTIL_Printf("huffdec chksum:0x%x\n",u4HwJdecRead32(u4HwId,  VLDREG_JPEG_HUFFDEC_CHKSUM));
    UTIL_Printf("VLD out chksum:0x%x\n", u4HwJdecRead32(u4HwId,  VLDREG_JPEG_VLD_OUT_CHKSUM));
    UTIL_Printf( "IDCT chksum:0x%x\n",u4HwJdecRead32(u4HwId,  VLDREG_JPEG_IDCT_OUT_CHKSUM));
    UTIL_Printf("WINTF cksum:0x%x\n",u4HwJdecRead32(u4HwId,  VLDREG_JPEG_WINTF_CHKSUM));
    UTIL_Printf("coeff dram in chksum:0x%x\n",u4HwJdecRead32(u4HwId,  VLDREG_JPEG_COEFF_DRAM_IN_CHKSUM));
    UTIL_Printf("nz in chksum:0x%x\n",u4HwJdecRead32(u4HwId,  VLDREG_JPEG_NZ_IN_CHKSUM));
    UTIL_Printf("nz out chksum:0x%x\n",u4HwJdecRead32(u4HwId, VLDREG_JPEG_NZ_OUT_CHKSUM));

           UTIL_Printf("[ImgreszVfy] Hw Checksum Wr 0x%x Rd 0x%x 0x%x,  0x1D8:0x%x\n",
                IO_READ32(0xF0000000+ 0x20000, 0x0E4),
                IO_READ32(0xF0000000+ 0x20000, 0x1E4),
                IO_READ32(0xF0000000+ 0x20000, 0x1F4),
                IO_READ32(0xF0000000+ 0x20000, 0x1D8));
}
void vHwJdecHwCheckDram(UINT32 u4HwId)
{ 
#if 0
  UINT32 u4Tem1 =0;
  UINT32 u4Temp2 =0;
  u4Tem1 = *(volatile UINT32 *)(IO_BASE + 0x520d8);
  u4Temp2 = *(volatile UINT32 *)(IO_BASE + 0x52138);
  if(((u4Tem1& 0x88888888) != 0)  ||
  	 ((u4Temp2& 0x88888888)  != 0)
  	)
  {
   //vHwJdecPrintRegister(u4HwId);
   //x_thread_delay(1000);
   //ASSERT(0);
   UTIL_Printf("[JDEC_HW] dram compare result 1 \n");
  }
#endif
}

void vHwJdecSRAMClear(UINT32 u4HwId)
{
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_SRAM_CLEAR,0);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_SRAM_CLEAR,0x100);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_SRAM_CLEAR,0);mb();
}


void vHwJdecSetVLDWaitTimeoutValue(UINT32 u4HwId,UINT32 u4TimeoutValue)
{
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_WAITT,u4TimeoutValue);
}


void vHwJdecSetDecodeModeBaseline(UINT32 u4HwId)
{
    UINT32 u4Reg;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Reg = u4HwJdecRead32(u4HwId,RW_VLD_JPG_PRG_MODE);
    u4Reg &= ~DECODE_MODE_PROG;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_PRG_MODE,u4Reg);
}


void vHwJdecSetDecodeModeProgressive(UINT32 u4HwId)
{
    UINT32 u4Reg;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Reg = u4HwJdecRead32(u4HwId,RW_VLD_JPG_PRG_MODE);
    u4Reg |= DECODE_MODE_PROG;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_PRG_MODE,u4Reg);
}


void vHwJdecSetAhAl(UINT32 u4HwId,UINT32 Ah,UINT32 Al)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_AH_AL,((Ah & 0xF)<<8) | (Al & 0xF));
}


void vHwJdecSetSsSe(UINT32 u4HwId,UINT32 Ss,UINT32 Se)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_SS_SE,((Ss & 0x7F) << 8) | (Se & 0x3F));
}


void vHwJdecSetDcAc(UINT32 u4HwId,UINT32 Ah,UINT32 Ss)
{
    UINT32 u4Reg;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Reg = u4HwJdecRead32(u4HwId,RW_VLD_JPG_PRG_MODE);
    u4Reg &= ~(JPG_DC_FIRST | JPG_DC_REFINE | JPG_AC_FIRST | JPG_AC_REFINE);
    if (Ah == 0)
    {
        if (Ss == 0)
            u4Reg |= JPG_DC_FIRST;
        else
            u4Reg |= JPG_AC_FIRST;
    }
    else
    {
        if (Ss == 0)
            u4Reg |= JPG_DC_REFINE;
        else
            u4Reg |= JPG_AC_REFINE;
    }
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_PRG_MODE,u4Reg);
}

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
void vHwJdecForNewEnhance(UINT32 u4HwId,BOOL fgOpenEOB,BOOL fgOpenCoef)
{
    UINT32 u4Reg;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Reg = u4HwJdecRead32(u4HwId,RW_VLD_JPG_PRG_MODE);
    u4Reg &= ~(EOB_AUTO_FILL|COEF_DEF_INPUT);

    if(fgOpenEOB)
        u4Reg |= EOB_AUTO_FILL;
    if(fgOpenCoef)
        u4Reg |= COEF_DEF_INPUT;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_PRG_MODE,u4Reg);
}
#endif
void vHwJdecSetLastScan(UINT32 u4HwId,BOOL fgLastScan)
{
    UINT32 u4Reg;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Reg = u4HwJdecRead32(u4HwId,RW_VLD_JPG_PRG_MODE);
	if(fgLastScan)
        u4Reg |= PROG_MODE_LAST_SCAN;
    else
        u4Reg &= ~PROG_MODE_LAST_SCAN;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_PRG_MODE,u4Reg);
}


void vHwJdecSetNoneZeroHistoryBuf(UINT32 u4HwId,UINT32 u4NoneZeroHistoryBuf)
{
#if !JDEC_IO_MMU_TEST
    JDEC_VERY_HAL_LOG("Enter %s-- u4NoneZeroHistoryBuf[0x%x] None MMU!\n", __FUNCTION__,u4NoneZeroHistoryBuf);
    u4NoneZeroHistoryBuf = u4NoneZeroHistoryBuf ? PHYSICAL(u4NoneZeroHistoryBuf):0;
#else 
    JDEC_VERY_HAL_LOG("Enter %s--MMU![0x%x]\n", __FUNCTION__,u4NoneZeroHistoryBuf);
#endif
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    if (BSP_GetIcVersion() < IC_VER_E)
    {
        VERIFY(u4NoneZeroHistoryBuf < 0x1000000);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_NZ_HIST,u4NoneZeroHistoryBuf);
    }
    else
    {
        UINT32 u4Temp = 0;

        // Set bit 23~0 of address
        vHwJdecWrite32(u4HwId, RW_VLD_JPG_NZ_HIST,(u4NoneZeroHistoryBuf & 0xFFFFFF));

        // set bit 29~24 of address
        u4Temp = u4HwJdecRead32(u4HwId,RW_MBSTART_DCAC_SWITCH);
        u4Temp = (u4Temp & 0xFFC0FFFF) | ((u4NoneZeroHistoryBuf & 0x3F000000) >> 8);


    }
#else
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_NZ_HIST,u4NoneZeroHistoryBuf);
#endif
}


void vHwJdecSetOutputNoneZeroHistoryBufOnly(UINT32 u4HwId,BOOL fgNzBufOnly)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(fgNzBufOnly)
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_PRG_NWB,VLD_PROG_BUFF_NZ_ONLY);
    else
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_PRG_NWB,VLD_PROG_BUFF_ALL);
}
#if JDEC_MM_MODE_ENABLE
void vHwJdecSeMCURowNum(UINT32 u4HwId,UINT32 u4StartRow, UINT32 u4EndRow)
{
    UINT32 u4Val =0;
	JDEC_VERY_HAL_LOG("Enter %s, u4StartRow=0x%x,u4EndRow=0x%x,u4Val=0x%x\n", __FUNCTION__,u4StartRow,u4EndRow,u4Val);
    u4StartRow = (u4StartRow & 0x1FFF)<<16;
    u4EndRow = (u4EndRow & 0x1FFF);
    u4Val = u4StartRow| u4EndRow|0x80000000;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_CLR_ST_MRK,u4Val);
	JDEC_VERY_HAL_LOG("Enter %s Seted RW_VLD_JPG_CLR_ST_MRK=0x%x\n",__FUNCTION__, u4HwJdecRead32(u4HwId,RW_VLD_JPG_CLR_ST_MRK));
}

void  vHwJDecSetMMpassMode(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
#if 0
     vHwJdecWrite32(u4HwId, RW_VLD_JPG_NEW_BASE, 1);
	JDEC_VERY_HAL_LOG("[JDEC] 0x3C4=0x%x\n", u4HwJdecRead32(u4HwId,RW_VLD_JPG_NEW_BASE));
#endif	
}

INT32 i4HwJdecSetMMPassPitchPerRow(UINT32 u4HwId,UINT32 u4Comp0PitchPerRow,UINT32 u4Comp1PitchPerRow,UINT32 u4Comp2PitchPerRow,
	                                                      JDEC_JFIF_SOS_T *prScanInfo, BOOL fgLastScan)
{
    UINT32 u4Value;
    UINT32 u4CompIdx;
    UINT32 u4Comp0Pitch = 0;
    UINT32 u4Comp1Pitch = 0;
    UINT32 u4Comp2Pitch = 0;
    UINT32 u4Coffe0Pitch = 0;
    UINT32 u4Coffe1Pitch = 0;
    UINT32 u4Coffe2Pitch = 0;
	UINT32 u4CompMCUROW0Pitch = 0;
    UINT32 u4CompMCUROW1Pitch = 0;
    UINT32 u4CompMCUROW2Pitch = 0;
	JDEC_VERY_HAL_LOG("Enter %s u1CompInScan=0x%x ,u4Comp0PitchPerRow=0x%x u4Comp1PitchPerRow=0x%x u4Comp2PitchPerRow=0x%x, fgLastScan= 0x%x\n", __FUNCTION__,
	                    prScanInfo->u1CompInScan, u4Comp0PitchPerRow,u4Comp1PitchPerRow,u4Comp2PitchPerRow,fgLastScan);
    if(prScanInfo->u1CompInScan == 1)
    {
        u4CompIdx = prScanInfo->au1CompIdx[0];
        if (u4CompIdx > 2)
        {
            ASSERT(0);
         }
         switch(u4CompIdx)
      	 {
      	   case 0: //only y component
      	       u4Comp0Pitch = u4Comp0PitchPerRow;
			   u4CompMCUROW0Pitch= u4Comp0PitchPerRow;
			   u4CompMCUROW1Pitch= u4Comp0PitchPerRow;
			   u4CompMCUROW2Pitch= u4Comp0PitchPerRow;
	        break;
      	   case 1: //only cb component
#if JDEC_CBCR_ALIGN_WITH_Y 
			   u4Comp0Pitch = u4Comp0PitchPerRow;
               u4CompMCUROW0Pitch= u4Comp0PitchPerRow;
			   u4CompMCUROW1Pitch= u4Comp0PitchPerRow;
			   u4CompMCUROW2Pitch= u4Comp0PitchPerRow;
#else
		       u4Comp0Pitch = u4Comp1PitchPerRow;
			   u4CompMCUROW0Pitch= u4Comp1PitchPerRow;
			   u4CompMCUROW1Pitch= u4Comp1PitchPerRow;
			   u4CompMCUROW2Pitch= u4Comp1PitchPerRow;
#endif 
	           break;
	        case 2: //only cr component
#if JDEC_CBCR_ALIGN_WITH_Y 
			    u4Comp0Pitch = u4Comp0PitchPerRow;
			   u4CompMCUROW0Pitch= u4Comp0PitchPerRow;
			   u4CompMCUROW1Pitch= u4Comp0PitchPerRow;
			   u4CompMCUROW2Pitch= u4Comp0PitchPerRow;
#else
		         u4Comp0Pitch = u4Comp2PitchPerRow;
			   u4CompMCUROW0Pitch= u4Comp2PitchPerRow;
			   u4CompMCUROW1Pitch= u4Comp2PitchPerRow;
			   u4CompMCUROW2Pitch= u4Comp2PitchPerRow;
#endif
	         break;
      	 }
          u4Comp1Pitch =  0;
          u4Comp2Pitch =  0;
     }
     else
     {
#if JDEC_CBCR_ALIGN_WITH_Y
          u4Comp0Pitch = u4Comp0PitchPerRow;
          u4Comp1Pitch = u4Comp0PitchPerRow;
          u4Comp2Pitch = u4Comp0PitchPerRow;
		  u4CompMCUROW0Pitch = u4Comp0PitchPerRow;
          u4CompMCUROW1Pitch = u4Comp0PitchPerRow;
          u4CompMCUROW2Pitch = u4Comp0PitchPerRow;
#else
          u4Comp0Pitch = u4Comp0PitchPerRow;
          u4Comp1Pitch = u4Comp1PitchPerRow;
          u4Comp2Pitch = u4Comp2PitchPerRow;
		  u4CompMCUROW0Pitch = u4Comp0PitchPerRow;
          u4CompMCUROW1Pitch = u4Comp1PitchPerRow;
          u4CompMCUROW2Pitch = u4Comp2PitchPerRow;
 #endif		  
     }
     vHwJdecWrite32(u4HwId,RW_VLD_JPG_IDCT_WIDTH01,
                          ((u4Comp0Pitch>>2) & 0x3FFF)
                          | (((u4Comp1Pitch>>2) & 0x3FFF) << 14));

    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0);
    u4Value = (u4Value & (~0x3FFFF)) | ((u4Comp2Pitch>>2) & 0x3FFF);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0,u4Value);
#if 1
{
   UINT32 u4Value =0x710;
   vHwJdecWrite32(u4HwId, RW_VLD_MM_ROW_SIZE_COMP0, u4CompMCUROW0Pitch*8/4);
   vHwJdecWrite32(u4HwId, RW_VLD_MM_ROW_SIZE_COMP1, u4CompMCUROW1Pitch*8/4);
   vHwJdecWrite32(u4HwId, RW_VLD_MM_ROW_SIZE_COMP2, u4CompMCUROW2Pitch*8/4);
   vHwJdecWrite32(u4HwId, RW_VLD_JPG_NEW_BASE, u4Value);
   JDEC_VERY_HAL_LOG("[JDEC] 0x3C4=0x%x\n", u4HwJdecRead32(u4HwId,RW_VLD_JPG_NEW_BASE));
}
#endif 

   {
        if (!fgLastScan)
        {
            u4Comp0PitchPerRow = u4Comp0PitchPerRow * 2;
            u4Comp1PitchPerRow = u4Comp1PitchPerRow * 2;
            u4Comp2PitchPerRow = u4Comp2PitchPerRow * 2;
        }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)		
       if(prScanInfo->u1CompInScan == 1)
       {
           u4CompIdx = prScanInfo->au1CompIdx[0];
           if (u4CompIdx > 2)
           {
               ASSERT(0);
            }
           switch(u4CompIdx)
           {
            case 0: //only y component
         	   u4Coffe0Pitch = u4Comp0PitchPerRow;
   	          break;
         	case 1: //only cb component
   		      u4Coffe0Pitch = u4Comp1PitchPerRow;
   	          break;
   	       case 2: //only cr component
   		      u4Coffe0Pitch = u4Comp2PitchPerRow;
   	       break;
            }
             u4Coffe1Pitch =  0;
             u4Coffe2Pitch =  0;
        }
        else
#endif
        {
             u4Coffe0Pitch = u4Comp0PitchPerRow;
             u4Coffe1Pitch = u4Comp1PitchPerRow;
             u4Coffe2Pitch = u4Comp2PitchPerRow;
        }   
       u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0);
        u4Value = (u4Value & (~(0x3FFFFUL<<14))) | (((u4Coffe0Pitch>>2) & 0x3FFFUL)<<14);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0,u4Value);

        JDEC_VERY_HAL_LOG("[JDEC] For 0x194 u4Coffe1Pitch=0x%x,u4Coffe2Pitch=0x%x\n", u4Coffe1Pitch,u4Coffe2Pitch);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEFF_WIDTH12,
                              ((u4Coffe1Pitch>>2) & 0x3FFF)
                              | (((u4Coffe2Pitch>>2) & 0x3FFF) <<14));
    }
	return S_JDEC_HAL_OK;
}
#endif


void vHwJdecSetInputBuffer(UINT32 u4HwId,UINT32 u4InBufSa,UINT32 u4InBufSz)
{    JDEC_VERY_HAL_LOG("Enter %s!,u4InBufSa[0x%x], u4InBufSz[0x%x]\n", __FUNCTION__,u4InBufSa,u4InBufSz);
#if !JDEC_IO_MMU_TEST
    u4InBufSa = u4InBufSa ? PHYSICAL(u4InBufSa):0;
#endif
    vHwJdecWrite32(u4HwId,RW_VLD_VSTART,u4InBufSa>>6); mb();
    vHwJdecWrite32(u4HwId,RW_VLD_VEND,(u4InBufSa+u4InBufSz)>>6);mb();
}


INT32 i4HwJdecWaitVldFetchOk(UINT32 u4HwId)
{
	UINT32 u4TimeAccu = 0;
        JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
	while (! (u4HwJdecRead32(u4HwId,RO_VLD_FETCHOK) & VLD_FETCH_OK))
	{
        
		//x_thread_delay(5);
		u4TimeAccu ++;
		if (u4TimeAccu >= 1000000)
		{
		     UTIL_Printf("initial fetch FAIL.\n");
		     vHwJdecPrintRegister(0);
		     
		     //ASSERT(0);
		     u4TimeAccu = 0;
			 return E_JDEC_HAL_FAIL;
		}
        
	}
	return S_JDEC_HAL_OK;
}

#if DEBUG_MMMMM_ON
UINT32 *_pi4JpegWindow[14]={NULL};
UINT32  _u4WindowNum[14] = {0};
UINT32  _ui4ScanNumber;
#endif

UINT32 u4HwJdecGetBitS(UINT32 u4HwId,UINT32 u4ShiftBit)
{
  UINT32 u4RegVal;
  UINT32 u4ShiftBitL = (u4ShiftBit & 0x1F);
  UINT32 u4ShiftBitH = ((u4ShiftBit>>5) & 0x3);
  UINT32 u4I;
  JDEC_VERY_HAL_LOG("Enter %s,u4ShiftBit[0x%x]!\n", __FUNCTION__,u4ShiftBit); 
  #if DEBUG_MMMMM_ON
  if(_pi4JpegWindow[_ui4ScanNumber] != NULL)
  {
    //ASSERT(0);
  }
  else
  {
     _pi4JpegWindow[_ui4ScanNumber]= x_alloc_vmem(3 *sizeof(UINT32));
  }
   _u4WindowNum[_ui4ScanNumber] = 3;
  #endif
#if DEBUG_MMMMM_ON 
		_pi4JpegWindow[_ui4ScanNumber][0] =  u4HwJdecRead32(u4HwId,0xF0);
#endif   

  // Skip groups of 32 bits
  for(u4I=0; u4I<u4ShiftBitH; u4I++)
  { 
	  u4HwJdecRead32(u4HwId,RO_VLD_BARL + (32<< 2)); mb();  
  }
#if DEBUG_MMMMM_ON
	   _pi4JpegWindow[_ui4ScanNumber][1] =  u4HwJdecRead32(u4HwId,0xF0);
#endif	 
  u4RegVal = u4HwJdecRead32(u4HwId,RO_VLD_BARL + (u4ShiftBitL<< 2)); mb();
#if DEBUG_MMMMM_ON
  _pi4JpegWindow[_ui4ScanNumber][2] =	u4HwJdecRead32(u4HwId,0xF0);
#endif
  return(u4RegVal);
}
UINT32 u4HwJdecGetVLDWritePtr(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    return u4HwJdecRead32(u4HwId,RO_VLD_HW_WRITE_POINT);
}

UINT32 u4HwJdecGetVLDReadPtr(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    return u4HwJdecRead32(u4HwId,RW_VLD_HW_READ_POINT);
}

INT32 i4HwJdecSetBarrelShifterAddr(UINT32 u4HwId,UINT32 u4Addr,UINT32 u4Bits)
{ 
    JDEC_VERY_HAL_LOG("Enter %s!u4Addr=0x%x,u4Bits=0x%x\n", __FUNCTION__,u4Addr, u4Bits);
#if !JDEC_IO_MMU_TEST
    u4Addr = u4Addr ? PHYSICAL(u4Addr):0;
#endif	
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
if((u4HwJdecRead32(u4HwId,RO_VLD_FETCHOK) & VLD_INIT_VALID_FLG))
{
    UINT32 u4TryCnt =0;
    while (!(u4HwJdecRead32(u4HwId,RO_VIDEO_BUFFER_CTRL) & 1))
    {
       u4TryCnt++;
   	   if(u4TryCnt >=10000)
   	   {
   	     if(!_fgEnableRegisterLog)
		 {
	       _fgEnableRegisterLog = 2;
		 }
         vHwJdecPrintRegister(0);
		 if(2==_fgEnableRegisterLog)
		 {
	       _fgEnableRegisterLog = 0;
		 }
		 UTIL_Printf("wait VLD_INIT_VALID_FLG FAIL.\n");
		 break;
       }
    }
	ASSERT(u4TryCnt<10000);
}
#endif

#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560) 
    if ((u4HwJdecRead32(u4HwId, 0xF4) & 0x8000) > 0)
    {
        UINT32 u4Temp = 0;

        u4Temp = u4HwJdecRead32(u4HwId, 0xF4);
        while (1)
        {
            if ((u4Temp & 0x1) > 0)
                break;
            u4Temp = u4HwJdecRead32(u4HwId, 0xF4);
        }
    }
#endif
    vHwJdecWrite32(u4HwId,RW_VLD_RPTR,u4Addr & (~0xF)); 	mb();// Set VLD read pointer

//Reset async fifo
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
    vHwJdecWrite32(u4HwId,WO_VLD_SW_RESET,VLD_ASYNC_FIFO_RST);mb();
    vHwJdecWrite32(u4HwId,WO_VLD_SW_RESET,VLD_SW_RESET_CLEAR);mb();
#endif

    vHwJdecWrite32(u4HwId,RW_VLD_PROC,VLD_INIFET); mb();// Init fetch

    {
        INT32 i4Ret;
        i4Ret = i4HwJdecWaitVldFetchOk(u4HwId);
        if(i4Ret<0)
            return i4Ret;
    }    

    vHwJdecWrite32(u4HwId,RW_VLD_PROC,VLD_INIBR); mb(); // Init barrel shifter
    //check initial fetch pre-status.
   #if 1
    {
      UINT32 u4TryCnt =0;
	  UINT32 u4FetchReg =0;
	  UINT32 uBufCtrlReg =0;
	  u4FetchReg=(u4HwJdecRead32(u4HwId,RO_VLD_FETCHOK) & 4);
	  uBufCtrlReg =(u4HwJdecRead32(u4HwId,RO_VIDEO_BUFFER_CTRL) & 4);
      while (!u4FetchReg || uBufCtrlReg)
      {
         UTIL_Printf("Error RO_VLD_FETCHOK ===[0x%8x]\n",u4HwJdecRead32(u4HwId,RO_VLD_FETCHOK));
		 UTIL_Printf("Error RO_VIDEO_BUFFER_CTRL ===[0x%8x]\n",u4HwJdecRead32(u4HwId,RO_VIDEO_BUFFER_CTRL));
		 u4TryCnt++;
      	 if(u4TryCnt >=10000)
      	 {      
            vHwJdecPrintRegister(0);   
			x_thread_delay(10000);
        	break;
         }	
		  u4FetchReg=(u4HwJdecRead32(u4HwId,RO_VLD_FETCHOK) & 4);
	      uBufCtrlReg =(u4HwJdecRead32(u4HwId,RO_VIDEO_BUFFER_CTRL) & 4);
      }      
	  ASSERT(u4TryCnt<10000);
    }
   #endif
    u4HwJdecGetBitS(u4HwId,((u4Addr & 0xF) << 3) + u4Bits);

    return S_JDEC_HAL_OK;
}

//extern INT32 x_dbg_stmt (const CHAR*  ps_format, ...);

INT32 i4HwJdecSetVLDSwWritePointer(UINT32 u4HwId,BOOL fgEnable,UINT32 u4Addr)
{
    JDEC_VERY_HAL_LOG("Enter %s wtptr[0x%x]!\n", __FUNCTION__,u4Addr);
#if !JDEC_IO_MMU_TEST
    u4Addr = u4Addr ? PHYSICAL(u4Addr) : 0;
#endif
    vHwJdecWrite32(u4HwId,RW_VLD_SW_WP,u4Addr);
    return S_JDEC_HAL_OK;
}


void vHwJdecSetVLDBarrelShifterToJpegMode(UINT32 u4HwId,BOOL fgJpegMode)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(fgJpegMode)
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_SWITCH,1);
    else
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_SWITCH,0);
}


void vHwJdecSetMiscReg(UINT32 u4HwId)
{
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_PRG_MISC1,0xFFFFFFFF);
#else
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
      //Reserved [31:16] for ECO  & [9] for debugg
      UINT32 u4RegVal;
	  JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
      u4RegVal = u4HwJdecRead32(u4HwId,RW_VLD_JPG_PRG_MISC1);
      u4RegVal |= 0x00000100;
      UTIL_Printf("imgrz hw id:1\n");
      vHwJdecWrite32(u4HwId, RW_VLD_JPG_PRG_MISC1, u4RegVal);mb();
   #elif (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    // select image resizer 0 for test
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_PRG_MISC1, 0x00000100);mb();
#else
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_PRG_MISC1, 0xFFFF011F);mb();
#endif
#endif
}


INT32 i4HwJdecCalculateOutputBufAddr(UINT32 u4HwId,UINT32 u4CurrMCUNum,
                                                   JDEC_HAL_OUTPUT_ADDR_T *prOutputAddr,
                                                   JDEC_JFIF_SOF_T *prSofInfo,
                                                   JDEC_JFIF_SOS_T *prScanInfo,
                                                   JDEC_HAL_OUTPUT_INFO_T *prOutputInfo,
                                                   UINT32 u4OutCoefYBufSa,
                                                   UINT32 u4OutCoefCbBufSa,
                                                   UINT32 u4OutCoefCrBufSa,
                                                   BOOL fgReinit,
                                                   BOOL fgIsMultiCollectMode,
                                                   BOOL fgLastScan,
                                                   JDEC_HAL_DEC_MODE_T eDecodingMode
#if JDEC_SUPPORT_SLICED_COEF_BUFFER
                                                   ,HANDLE_T hY
                                                   ,HANDLE_T hCb
                                                   ,HANDLE_T hCr
                                                   ,BOOL fgSlicedCoef
#endif
                                                   )
{
    UINT32 u4CompIdx=0xFF;
    JDEC_VERY_HAL_LOG("Enter %s!-modifed\n", __FUNCTION__);
#if JDEC_MM_MODE_ENABLE
	if(eDecodingMode== JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS)
	{
	    UINT32 u4Factor =1;
	    if(prScanInfo->u1CompInScan == 1)
        {
            u4CompIdx = prScanInfo->au1CompIdx[0];
            if (u4CompIdx > 2)
            {
                ASSERT(0);
            }
            prOutputAddr->u4MCUWidth = prOutputAddr->au4MCUWidth[u4CompIdx];
            prOutputAddr->u4MCUHeight = prOutputAddr->au4MCUHeight[u4CompIdx];		
			u4Factor = prSofInfo->arSofComp[u4CompIdx].u1VSampFactor;
		    //prOutputAddr->u4MCUy = prOutputInfo->u4DecodedMCURow * prSofInfo->arSofComp[u4CompIdx].u1VSampFactor;
		} 
		else
        {
            prOutputAddr->u4MCUWidth = prOutputAddr->u4CombMCUWidth;
            prOutputAddr->u4MCUHeight = prOutputAddr->u4CombMCUHeight;
			u4CurrMCUNum = prOutputInfo->u4DecodedMCURow *prOutputAddr->u4MCUWidth;
        }
		prOutputAddr->u4MCUy = prOutputInfo->u4DecodedMCURow;
        prOutputAddr->u4MCUx =0;
        prOutputAddr->u4YBufSa = prOutputInfo->u4OutYBufSa;
        prOutputAddr->u4CbBufSa = prOutputInfo->u4OutCbBufSa;
        prOutputAddr->u4CrBufSa = prOutputInfo->u4OutCrBufSa;
        if(prScanInfo->u1CompInScan != 1)
        {
        #if JDEC_CBCR_ALIGN_WITH_Y
            prOutputAddr->u4YBufSa  += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutYBufPitch*8;
            prOutputAddr->u4CbBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutYBufPitch*8;
            prOutputAddr->u4CrBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutYBufPitch*8;
		#else
            prOutputAddr->u4YBufSa  += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutYBufPitch*8;
            prOutputAddr->u4CbBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutCbBufPitch*8;
            prOutputAddr->u4CrBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutCrBufPitch*8;

		#endif
        } 
		else
        {
     #if JDEC_CBCR_ALIGN_WITH_Y 
            prOutputAddr->u4YBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutYBufPitch*(prSofInfo->arSofComp[0].u1VSampFactor * 8);
            prOutputAddr->u4CbBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutYBufPitch*(prSofInfo->arSofComp[1].u1VSampFactor * 8);
            prOutputAddr->u4CrBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutYBufPitch*(prSofInfo->arSofComp[2].u1VSampFactor * 8);
	 #else
            prOutputAddr->u4YBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutYBufPitch*(prSofInfo->arSofComp[0].u1VSampFactor * 8);
            prOutputAddr->u4CbBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutCbBufPitch*(prSofInfo->arSofComp[1].u1VSampFactor * 8);
            prOutputAddr->u4CrBufSa += (prOutputAddr->u4MCUy)*prOutputInfo->u4OutCrBufPitch*(prSofInfo->arSofComp[2].u1VSampFactor * 8);
	 #endif
        }  
    	JDEC_VERY_HAL_LOG("Enter %s MMode u4CompIdx[0x%x], u4MCUy[0x%x]!\n", __FUNCTION__,u4CompIdx, prOutputAddr->u4MCUy);   
        JDEC_VERY_HAL_LOG("Enter %s MMode prOutputAddr->u4YBufSa [0x%x]!\n", __FUNCTION__,prOutputAddr->u4YBufSa );   
        JDEC_VERY_HAL_LOG("Enter %s MMode prOutputAddr->u4CbBufSa [0x%x]\n", __FUNCTION__,prOutputAddr->u4CbBufSa );   		
		JDEC_VERY_HAL_LOG("Enter %s MMode prOutputAddr->u4CrBufSa [0x%x]!\n", __FUNCTION__,prOutputAddr->u4CrBufSa);   
		//JDEC_VERY_HAL_LOG("Enter %s MMode prOutputInfo->ui4McuRowNumber [0x%x]!\n", __FUNCTION__,prOutputInfo->ui4McuRowNumber);   
		return 0;
	}
	else 
#endif 	
    if((u4CurrMCUNum == 0) || fgReinit)
    {
        if(prScanInfo->u1CompInScan == 1)
        {
            u4CompIdx = prScanInfo->au1CompIdx[0];
            if (u4CompIdx > 2)
            {
                ASSERT(0);
            }
            prOutputAddr->u4MCUWidth = prOutputAddr->au4MCUWidth[u4CompIdx];
            prOutputAddr->u4MCUHeight = prOutputAddr->au4MCUHeight[u4CompIdx];
        } 
        else
        {
            prOutputAddr->u4MCUWidth = prOutputAddr->u4CombMCUWidth;
            prOutputAddr->u4MCUHeight = prOutputAddr->u4CombMCUHeight;
        }

        if(u4CurrMCUNum == 0)
        {
            prOutputAddr->u4MCUx = 0;
            prOutputAddr->u4MCUy = 0;
        } else
        {
            prOutputAddr->u4MCUy = u4CurrMCUNum / prOutputAddr->u4MCUWidth;
            prOutputAddr->u4MCUx = u4CurrMCUNum - prOutputAddr->u4MCUy * prOutputAddr->u4MCUWidth;
        }
    }

    if (fgLastScan) /* Add by pingzhao ,if not last scan ,no need to calculate IDCT buffer*/
    {
    if(prOutputAddr->fgAddrModified)
        prOutputAddr->u4AddrModifiedMCUy = prOutputAddr->u4MCUy;

    // Calculate IDCT buffer address
    if((prOutputAddr->u4MCUx == 0) || fgReinit || prOutputAddr->fgAddrModified)
    {
        prOutputAddr->u4YBufSa = prOutputInfo->u4OutYBufSa;
        prOutputAddr->u4CbBufSa = prOutputInfo->u4OutCbBufSa;
        prOutputAddr->u4CrBufSa = prOutputInfo->u4OutCrBufSa;

        if(prScanInfo->u1CompInScan == 1)
        {
            prOutputAddr->u4YBufSa += (prOutputAddr->u4MCUy-prOutputAddr->u4AddrModifiedMCUy)*prOutputInfo->u4OutYBufPitch*8;
            prOutputAddr->u4CbBufSa += (prOutputAddr->u4MCUy-prOutputAddr->u4AddrModifiedMCUy)*prOutputInfo->u4OutCbBufPitch*8;
            prOutputAddr->u4CrBufSa += (prOutputAddr->u4MCUy-prOutputAddr->u4AddrModifiedMCUy)*prOutputInfo->u4OutCrBufPitch*8;
        } else
        {
            prOutputAddr->u4YBufSa += (prOutputAddr->u4MCUy-prOutputAddr->u4AddrModifiedMCUy)*prOutputInfo->u4OutYBufPitch*(prSofInfo->arSofComp[0].u1VSampFactor * 8);
            prOutputAddr->u4CbBufSa += (prOutputAddr->u4MCUy-prOutputAddr->u4AddrModifiedMCUy)*prOutputInfo->u4OutCbBufPitch*(prSofInfo->arSofComp[1].u1VSampFactor * 8);
            prOutputAddr->u4CrBufSa += (prOutputAddr->u4MCUy-prOutputAddr->u4AddrModifiedMCUy)*prOutputInfo->u4OutCrBufPitch*(prSofInfo->arSofComp[2].u1VSampFactor * 8);
        }

        prOutputAddr->fgAddrModified = FALSE;
    } else
    {
        if(prScanInfo->u1CompInScan == 1)
        {
            prOutputAddr->u4YBufSa += 8;
            prOutputAddr->u4CbBufSa += 8;
            prOutputAddr->u4CrBufSa += 8;
        } else
        {       
                prOutputAddr->u4YBufSa += (prSofInfo->arSofComp[0].u1CompWidth);
                prOutputAddr->u4CbBufSa += (prSofInfo->arSofComp[1].u1CompWidth);
                prOutputAddr->u4CrBufSa += (prSofInfo->arSofComp[2].u1CompWidth);
            }
        }
    }
   else  //modified by lusun for multicollect mcu row mode.Prevente  addr is 0
    {
        prOutputAddr->u4YBufSa = prOutputInfo->u4OutYBufSa;
        prOutputAddr->u4CbBufSa = prOutputInfo->u4OutCbBufSa;
        prOutputAddr->u4CrBufSa = prOutputInfo->u4OutCrBufSa;       
     }
    if(fgReinit)
    {
        if(prScanInfo->u1CompInScan == 1)
        {
            prOutputAddr->u4YBufSa += prOutputAddr->u4MCUx * 8;
            prOutputAddr->u4CbBufSa += prOutputAddr->u4MCUx * 8;
            prOutputAddr->u4CrBufSa += prOutputAddr->u4MCUx * 8;
        } else
        {
            prOutputAddr->u4YBufSa += prOutputAddr->u4MCUx * (prSofInfo->arSofComp[0].u1HSampFactor << 3);
            prOutputAddr->u4CbBufSa += prOutputAddr->u4MCUx * (prSofInfo->arSofComp[1].u1HSampFactor << 3);
            prOutputAddr->u4CrBufSa += prOutputAddr->u4MCUx * (prSofInfo->arSofComp[2].u1HSampFactor << 3);
        }
    }

    if (fgIsMultiCollectMode)
    {
        // Calculate Coefficient buffer address    
        if((prOutputAddr->u4MCUx == 0) || fgReinit)
	    {
	        prOutputAddr->u4YCoeffBufSa = u4OutCoefYBufSa;
	        prOutputAddr->u4CbCoeffBufSa = u4OutCoefCbBufSa;
	        prOutputAddr->u4CrCoeffBufSa = u4OutCoefCrBufSa;

	        if(prScanInfo->u1CompInScan == 1)
	        {
	            prOutputAddr->u4YCoeffBufSa += prOutputAddr->u4MCUy * prOutputAddr->u4CombMCUWidth
	                                    * (prSofInfo->arSofComp[0].u1HSampFactor * 8)
	                                    * 8
	                                    * 2;

	            prOutputAddr->u4CbCoeffBufSa += prOutputAddr->u4MCUy * prOutputAddr->u4CombMCUWidth
	                                     * (prSofInfo->arSofComp[1].u1HSampFactor * 8)
	                                     * 8
	                                     * 2;

	            prOutputAddr->u4CrCoeffBufSa += prOutputAddr->u4MCUy * prOutputAddr->u4CombMCUWidth
	                                     * (prSofInfo->arSofComp[2].u1HSampFactor * 8)
	                                     * 8
	                                     * 2;
	        } else
	        {
	            prOutputAddr->u4YCoeffBufSa += prOutputAddr->u4MCUy * prOutputAddr->u4MCUWidth
	                                    * (prSofInfo->arSofComp[0].u1HSampFactor * 8)
	                                    * (prSofInfo->arSofComp[0].u1VSampFactor * 8)
	                                    * 2;

	            prOutputAddr->u4CbCoeffBufSa += prOutputAddr->u4MCUy * prOutputAddr->u4MCUWidth
	                                     * (prSofInfo->arSofComp[1].u1HSampFactor * 8)
	                                     * (prSofInfo->arSofComp[1].u1VSampFactor * 8)
	                                     * 2;

	            prOutputAddr->u4CrCoeffBufSa += prOutputAddr->u4MCUy * prOutputAddr->u4MCUWidth
	                                     * (prSofInfo->arSofComp[2].u1HSampFactor * 8)
	                                     * (prSofInfo->arSofComp[2].u1VSampFactor * 8)
	                                     * 2;
	        }
	    } 
        else
         {
             if(prScanInfo->u1CompInScan == 1)
             {
                 prOutputAddr->u4YCoeffBufSa += 8
                                              * 8
                                              * 2;
     
                 prOutputAddr->u4CbCoeffBufSa += 8
                                               * 8
                                               * 2;
     
                 prOutputAddr->u4CrCoeffBufSa += 8
                                               * 8
                                               * 2;
             } else
             {           
                     prOutputAddr->u4YCoeffBufSa += prSofInfo->arSofComp[0].u2CompCofSize;
     
                     prOutputAddr->u4CbCoeffBufSa += prSofInfo->arSofComp[1].u2CompCofSize;
     
                     prOutputAddr->u4CrCoeffBufSa += prSofInfo->arSofComp[2].u2CompCofSize;
            }
         }
        if(fgReinit)
	    {
	        if(prScanInfo->u1CompInScan == 1)
	        {
	            prOutputAddr->u4YCoeffBufSa += prOutputAddr->u4MCUx * 8
	                                         * 8
	                                         * 2;

	            prOutputAddr->u4CbCoeffBufSa += prOutputAddr->u4MCUx * 8
	                                          * 8
	                                          * 2;

	            prOutputAddr->u4CrCoeffBufSa += prOutputAddr->u4MCUx * 8
	                                          * 8
	                                          * 2;
	        } else
	        {
	            prOutputAddr->u4YCoeffBufSa += prOutputAddr->u4MCUx * (prSofInfo->arSofComp[0].u1HSampFactor * 8)
	                                         * 8
	                                         * 2;

	            prOutputAddr->u4CbCoeffBufSa += prOutputAddr->u4MCUx * (prSofInfo->arSofComp[1].u1HSampFactor * 8)
	                                          * 8
	                                          * 2;

	            prOutputAddr->u4CrCoeffBufSa += prOutputAddr->u4MCUx * (prSofInfo->arSofComp[2].u1HSampFactor * 8)
	                                          * 8
	                                          * 2;
	        }
	    }
    }
    else
    {
        prOutputAddr->u4YCoeffBufSa = u4OutCoefYBufSa;
        prOutputAddr->u4CbCoeffBufSa = u4OutCoefCbBufSa;
        prOutputAddr->u4CrCoeffBufSa = u4OutCoefCrBufSa;
    }
    if ((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT)||
        (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE))
    {
    prOutputAddr->u4MCUx++;
    if(prOutputAddr->u4MCUx == prOutputAddr->u4MCUWidth)
    {
        prOutputAddr->u4MCUx = 0;
        prOutputAddr->u4MCUy++;
    }
    }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    if ((eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT) ||
        (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE))
    {
        //prOutputAddr->u4MCUy ++;
    }
#endif

    return S_JDEC_HAL_OK;
}


void vHwJdecSetOutputBufAddr(UINT32 u4HwId,UINT32 u4Comp0Addr,UINT32 u4Comp1Addr,UINT32 u4Comp2Addr,
                                           JDEC_JFIF_SOS_T *prScanInfo)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Comp0Addr = u4Comp0Addr ? PHYSICAL(u4Comp0Addr) : 0;
    u4Comp1Addr = u4Comp1Addr ? PHYSICAL(u4Comp1Addr) : 0;
    u4Comp2Addr = u4Comp2Addr ? PHYSICAL(u4Comp2Addr) : 0;

    if(prScanInfo->u1CompInScan == 1)
    {
        switch(prScanInfo->au1CompIdx[0])
        {
            case 0:
                vHwJdecWrite32(u4HwId,RW_VLD_JPG_WR_ADD0,u4Comp0Addr>>2);
                break;
            case 1:
                vHwJdecWrite32(u4HwId,RW_VLD_JPG_WR_ADD0,u4Comp1Addr>>2);
                break;
            case 2:
                vHwJdecWrite32(u4HwId,RW_VLD_JPG_WR_ADD0,u4Comp2Addr>>2);
                break;
        }
    } else
    {
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_WR_ADD0,u4Comp0Addr>>2);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_WR_ADD1,u4Comp1Addr>>2);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_WR_ADD2,u4Comp2Addr>>2);mb();
    }
}


void vHwJdecSetOutputCoefBufAddr(UINT32 u4HwId,UINT32 u4Comp0Addr,UINT32 u4Comp1Addr,UINT32 u4Comp2Addr,
                                               JDEC_JFIF_SOS_T *prScanInfo)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
#if !JDEC_IO_MMU_TEST
    u4Comp0Addr = u4Comp0Addr ? PHYSICAL(u4Comp0Addr) : 0;
    u4Comp1Addr = u4Comp1Addr ? PHYSICAL(u4Comp1Addr) : 0;
    u4Comp2Addr = u4Comp2Addr ? PHYSICAL(u4Comp2Addr) : 0;
#endif
    if(prScanInfo->u1CompInScan == 1)
    {
        switch(prScanInfo->au1CompIdx[0])
        {
            case 0:
                vHwJdecWrite32(u4HwId,RW_VLD_JPG_RD_ADD0,u4Comp0Addr>>2);
                break;
            case 1:
                vHwJdecWrite32(u4HwId,RW_VLD_JPG_RD_ADD0,u4Comp1Addr>>2);
                break;
            case 2:
                vHwJdecWrite32(u4HwId,RW_VLD_JPG_RD_ADD0,u4Comp2Addr>>2);
                break;
        }
    } else
    {
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_RD_ADD0,u4Comp0Addr>>2);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_RD_ADD1,u4Comp1Addr>>2);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_RD_ADD2,u4Comp2Addr>>2);mb();
    }
}
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
INT32 i4HwJdecSetCoefMCUSize(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo)
{
    UINT32 u4CompIdx;
    UINT32 u4Value = 0;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if((prSofComp == NULL) || (prScanInfo == NULL))
        return E_JDEC_HAL_FAIL;

    if(prScanInfo->u1CompInScan == 1)
    {
        u4Value = (1 << 8) + (1 << 4) + 1;
    }
    else
    {
        u4CompIdx = prScanInfo->au1CompIdx[0];
        u4Value += prSofComp[u4CompIdx].u1HSampFactor;
        u4CompIdx = prScanInfo->au1CompIdx[1];
        u4Value += (prSofComp[u4CompIdx].u1HSampFactor << 4);
        u4CompIdx = prScanInfo->au1CompIdx[2];
        u4Value += (prSofComp[u4CompIdx].u1HSampFactor << 8);
    }
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_MCU_COEF_SIZE,u4Value);
    return S_JDEC_HAL_OK;
        
}
#endif
INT32 i4HwJdecSetCoefPitch(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo, JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo
#if JDEC_SUPPORT_SLICED_COEF_BUFFER
                             ,JDEC_HAL_DEC_MODE_T eDecodingMode
#endif
)
{

    UINT32 au4CoefPitch[3] = {0,0,0};
    UINT32 u4Index;
    UINT32 u4MCUPerWidth = 0;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4MCUPerWidth = (prPicInfo->u2ImageWidth + prSofComp[0].u1HSampFactor * 8 - 1) / (prSofComp[0].u1HSampFactor * 8);
    for(u4Index =0; u4Index < 3; u4Index++)
    {
        au4CoefPitch[u4Index] = u4MCUPerWidth * prSofComp[u4Index].u1HSampFactor * prSofComp[u4Index].u1VSampFactor
                                              * 128 / 4;
    }

    if(prScanInfo->u1CompInScan == 1)
    {
        switch(prScanInfo->au1CompIdx[0])
        {
            case 0:
                vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEF_PITCH_0,au4CoefPitch[prScanInfo->au1CompIdx[0]]/ (prSofComp[prScanInfo->au1CompIdx[0]].u1VSampFactor));
                break;
            case 1:
                vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEF_PITCH_0,au4CoefPitch[prScanInfo->au1CompIdx[0]]/ (prSofComp[prScanInfo->au1CompIdx[0]].u1VSampFactor));
                break;
            case 2:
                vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEF_PITCH_0,au4CoefPitch[prScanInfo->au1CompIdx[0]]/ (prSofComp[prScanInfo->au1CompIdx[0]].u1VSampFactor));
                break;
        } 
    }
    else
    {
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEF_PITCH_0,au4CoefPitch[0]);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEF_PITCH_1,au4CoefPitch[1]);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEF_PITCH_2,au4CoefPitch[2]);mb();
    }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)	
#if JDEC_SUPPORT_SLICED_COEF_BUFFER
    if  (eDecodingMode == JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT)
    	{
            vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEF_PITCH_0,0);
            vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEF_PITCH_1,0);
            vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEF_PITCH_2,0);mb();
    	}
#endif
#endif
    return S_JDEC_HAL_OK;
}
/*
void vHwJdecSetReadWriteOutputBufMode(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    vHwJdecMcWrite32(u4HwId,RW_MC_ADDRSWAP,0);
#endif
    vHwJdecMcWrite32(u4HwId,RW_MC_PS_DRAM_MODE,3);
}
*/

void vHwJdecInitScan(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART,0); mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART,VLD_RES_MKR_FND); mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART,0); mb();
}


void vHwJdecRestart(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART,VLD_MCU_RESTART); mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART,0); mb();
}


void vHwJdecCleanInterBuf(UINT32 u4HwId,BOOL fgClean)
{   
#if 0
	JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);

	if(fgClean)
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_FAST_DEC,VLD_CLEAN_INTERNAL_BUFFER);
    else
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_FAST_DEC,VLD_KEEP_INTERNAL_BUFFER);
#endif	
}


void vHwJdecSetCurrMCUNum(UINT32 u4HwId,UINT32 u4CurrMCUNum)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_MCU_COUNT,u4CurrMCUNum);
}


void vHwJdecDecodeMCU(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_START,VLD_DECODE_MCU);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_START,VLD_DECODE_MCU_CLEAR);mb();
}


BOOL fgHwJdecFillQTBLRdy(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4HwJdecRead32(u4HwId,RW_VLD_JPG_PRG_MISC1) & 0x1)
        return TRUE;

    return FALSE;
}

BOOL fgHwJdecIsDecodeFinish(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4HwJdecRead32(u4HwId,RW_VLD_JPG_RMK_AND_FINISH) & VLD_MCU_DECODE_FINISH)
        return TRUE;

    return FALSE;
}


BOOL fgHwJdecIsFoundMarker(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(u4HwJdecRead32(u4HwId,RW_VLD_JPG_RMK_AND_FINISH) & VLD_RECEIVE_MARKER)
        return TRUE;

    return FALSE;
}


UINT32 u4HwJdecGetFoundMarker(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    return (u4HwJdecRead32(u4HwId,RW_VLD_JPG_RMK_AND_FINISH) >> 8) & 0xFF;
}


INT32 i4HwJdecSetBlockPostition(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo,JDEC_JFIF_SOS_T *prScanInfo)
{
    UINT32 u4Block;
    UINT32 u4CompNum;
    UINT32 u4X,u4Y;
	UINT32 u4BlockXPosList = 0, u4BlockYPosList = 0;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(prPicInfo == NULL)
        return E_JDEC_HAL_FAIL;

    u4Block = 0;
    if(prScanInfo->u1CompInScan == 1)
    {
    } else
    {
        if (prPicInfo->u1NumComponents > 3)
        {
            ASSERT(0);
        }
        for(u4CompNum=0; u4CompNum<prPicInfo->u1NumComponents; u4CompNum++)
            for(u4Y=0; u4Y<prPicInfo->arSofComp[u4CompNum].u1VSampFactor; u4Y++)
                for(u4X=0; u4X<prPicInfo->arSofComp[u4CompNum].u1HSampFactor; u4X++)
                {
                    u4BlockXPosList |= (u4X & 0x7) << (u4Block * 3);
                    u4BlockYPosList |= (u4Y & 0x7) << (u4Block * 3);
                    u4Block++;
                }
    }

    vHwJdecWrite32(u4HwId,RW_VLD_JPG_X_IN_MCU,u4BlockXPosList);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_Y_IN_MCU,u4BlockYPosList);mb();

    return S_JDEC_HAL_OK;
}


INT32 i4HwJdecSetBlkAndDcAcTblList(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo)
{
    UINT32 u4Block;
    UINT32 u4Count;
    UINT32 u4CompNum;
    UINT32 u4CompIdx;
    UINT32 au4CompBlkNum;
    UINT32 u4DcHuffTblList = 0,u4AcHuffTblList = 0;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if((prSofComp == NULL) || (prScanInfo == NULL))
		return E_JDEC_HAL_FAIL;

    u4Block = 0;
    for(u4CompNum = 0; (u4CompNum<prScanInfo->u1CompInScan) && (u4CompNum<3); u4CompNum++)
    {
        u4CompIdx = prScanInfo->au1CompIdx[u4CompNum];
        if (u4CompIdx > 2)
        {
            ASSERT(0);
        }

        if(prScanInfo->u1CompInScan == 1)
            au4CompBlkNum = 1;
        else
            au4CompBlkNum = prSofComp[u4CompIdx].u1HSampFactor * prSofComp[u4CompIdx].u1VSampFactor;

        for (u4Count = 0; u4Count < au4CompBlkNum; u4Count++)
        {
            u4DcHuffTblList |= (prScanInfo->au1DcId[u4CompIdx]) << (u4Block * 2);
            u4AcHuffTblList |= (prScanInfo->au1AcId[u4CompIdx]) << (u4Block * 2);
	        u4Block++;
        }
	}

    vHwJdecWrite32(u4HwId,RW_VLD_JPG_BLK_DC_TBL,u4DcHuffTblList);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_BLK_AC_TBL,u4AcHuffTblList); mb();

	return S_JDEC_HAL_OK;
}


INT32 i4HwJdecSetBlkAndCompMembership(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo)
{
    UINT32 u4Block;
    UINT32 u4Count;
    UINT32 u4CompNum;
    UINT32 u4CompIdx;
    UINT32 au4CompBlkNum;
    UINT32 u4MembershipList = 0;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if((prSofComp == NULL) || (prScanInfo == NULL))
		return E_JDEC_HAL_FAIL;

    u4Block = 0;
    for(u4CompNum = 0; (u4CompNum<prScanInfo->u1CompInScan) && (u4CompNum<3); u4CompNum++)
    {
        if(prScanInfo->u1CompInScan == 1)
            au4CompBlkNum = 1;
        else
        {
            u4CompIdx = prScanInfo->au1CompIdx[u4CompNum];
            au4CompBlkNum = prSofComp[u4CompIdx].u1HSampFactor * prSofComp[u4CompIdx].u1VSampFactor;
        }

	    for (u4Count = 0; u4Count < au4CompBlkNum; u4Count++)
	    {
            u4MembershipList |= u4CompNum << (u4Block * 2);
	        u4Block++;
	    }
	}

    vHwJdecWrite32(u4HwId,RW_VLD_JPG_MEMBERSHIP,u4MembershipList);

	return S_JDEC_HAL_OK;
}


INT32 i4HwJdecSetQuantTable(UINT32 u4HwId,JDEC_JFIF_DQT_T *prQTblInfo)
{
    #if  JDEC_QTABLE_RISC_MODE
    INT32 i4Index;
    UINT32 *rQtable0Addr;
    UINT32 *rQtable1Addr;
    #else
	INT32 i4Ret;
    UINT32 u4Dummy;
    #endif
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(prQTblInfo == NULL)
		return E_JDEC_HAL_FAIL;

    //if(prQTblInfo->u1NumQ > 2)
		//return E_JDEC_HAL_FAIL;

#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    // Fix init barrel shiter write register Data Abort.
    vHwJdecWrite32(u4HwId,RW_VLD_RDY_SWTICH, READY_TO_RISC_1);
#endif
    // Set V FIFO start/end address to full range.
    vHwJdecWrite32(u4HwId,RW_VLD_VSTART,0);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_VEND,~0);mb();

   #if  JDEC_QTABLE_RISC_MODE
       rQtable0Addr = (void *)(&(prQTblInfo->aau1Qtbl[0][0]));

       vHwJdecWrite32(u4HwId,RW_IQ_TABLE_INDEX,0);mb();
     	for (i4Index = 0; i4Index < 16; i4Index++)
        {  
            vHwJdecWrite32(u4HwId,RW_IQ_TABLE_DATA_IN,(UINT32)*(rQtable0Addr + i4Index));mb();
        }
#else
     i4Ret = i4HwJdecSetBarrelShifterAddr(u4HwId,(UINT32)(&(prQTblInfo->aau1Qtbl[0][0])),0);
    if(i4Ret<0)
        return i4Ret;
    u4Dummy = u4HwJdecRead32(u4HwId,RW_VLD_TABLIM); // Load intra Quantization Matrix

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
     // wait fill Q-table ready
     while(TRUE)
     	{
     	  if(fgHwJdecFillQTBLRdy(u4HwId))
	    break;
     	}
#endif
#endif
    
#if  JDEC_QTABLE_RISC_MODE
           rQtable1Addr = (void *)(&(prQTblInfo->aau1Qtbl[1][0]));

           vHwJdecWrite32(u4HwId,RW_IQ_TABLE_INDEX,0x40);mb();
            for (i4Index = 0; i4Index < 16; i4Index++)
            {
                vHwJdecWrite32(u4HwId,RW_IQ_TABLE_DATA_IN,(UINT32)*(rQtable1Addr + i4Index));mb();
            }
#else
    i4Ret = i4HwJdecSetBarrelShifterAddr(u4HwId,(UINT32)(&(prQTblInfo->aau1Qtbl[1][0])),0);
    if(i4Ret<0)
        return i4Ret;
    u4Dummy = u4HwJdecRead32(u4HwId,RW_VLD_TABLNIM); // Load non-intra Quantization Matrix

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) 
     // wait fill Q-table ready
     while(TRUE)
     	{
     	  if(fgHwJdecFillQTBLRdy(u4HwId))
	    break;
     	}
#endif

    (void)u4Dummy;
#endif

	return S_JDEC_HAL_OK;
}


INT32 i4HwJdecSetCurrUsedQuantTable(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo,JDEC_JFIF_SOS_T *prScanInfo)
{
    UINT32 u4QtblUsed = 0;
    UINT32 u4CompNum;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if((prPicInfo == NULL) || (prScanInfo == NULL))
		return E_JDEC_HAL_FAIL;

    if (prScanInfo->u1CompInScan > 3)
    {
        ASSERT(0);
    }
    for(u4CompNum=0; u4CompNum<prScanInfo->u1CompInScan; u4CompNum++)
        u4QtblUsed |= (prPicInfo->arSofComp[prScanInfo->au1CompIdx[u4CompNum]].u1QuantTblNo) <<
                      (u4CompNum<<1);

    vHwJdecWrite32(u4HwId,RW_VLD_JPG_Q_TBL,u4QtblUsed);

	return S_JDEC_HAL_OK;
}


INT32 i4HwJdecSetPitchPerRow(UINT32 u4HwId,UINT32 u4Comp0PitchPerRow,UINT32 u4Comp1PitchPerRow,UINT32 u4Comp2PitchPerRow,
	                                                      JDEC_JFIF_SOS_T *prScanInfo,BOOL fgCoeffPitch, BOOL fgLastScan)
{
    UINT32 u4Value;
    UINT32 u4CompIdx;
    UINT32 u4Comp0Pitch = 0;
    UINT32 u4Comp1Pitch = 0;
    UINT32 u4Comp2Pitch = 0;
    UINT32 u4Coffe0Pitch = 0;
    UINT32 u4Coffe1Pitch = 0;
    UINT32 u4Coffe2Pitch = 0;
	JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(prScanInfo->u1CompInScan == 1)
    {
        u4CompIdx = prScanInfo->au1CompIdx[0];
        if (u4CompIdx > 2)
        {
            ASSERT(0);
         }
        switch(u4CompIdx)
      	 {
      	   case 0: //only y component
      	       u4Comp0Pitch = u4Comp0PitchPerRow;
	   break;
      	   case 1: //only cb component
		u4Comp0Pitch = u4Comp1PitchPerRow;
	   break;
	   case 2: //only cr component
		u4Comp0Pitch = u4Comp2PitchPerRow;
	   break;
      	 }
          u4Comp1Pitch =  0;
          u4Comp2Pitch =  0;
     }
     else
     {
          u4Comp0Pitch = u4Comp0PitchPerRow;
          u4Comp1Pitch = u4Comp1PitchPerRow;
          u4Comp2Pitch = u4Comp2PitchPerRow;
     }
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_IDCT_WIDTH01,
                          ((u4Comp0Pitch>>2) & 0x3FFF)
                          | (((u4Comp1Pitch>>2) & 0x3FFF) << 14));

    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0);
    u4Value = (u4Value & (~0x3FFFF)) | ((u4Comp2Pitch>>2) & 0x3FFF);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0,u4Value);

    if(fgCoeffPitch)
    {
        if (!fgLastScan)
        {
            u4Comp0PitchPerRow = u4Comp0PitchPerRow * 2;
            u4Comp1PitchPerRow = u4Comp1PitchPerRow * 2;
            u4Comp2PitchPerRow = u4Comp2PitchPerRow * 2;
        }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)		
       if(prScanInfo->u1CompInScan == 1)
       {
           u4CompIdx = prScanInfo->au1CompIdx[0];
           if (u4CompIdx > 2)
           {
               ASSERT(0);
            }
           switch(u4CompIdx)
           {
              case 0: //only y component
         	   u4Coffe0Pitch = u4Comp0PitchPerRow;
   	       break;
         	case 1: //only cb component
   		   u4Coffe0Pitch = u4Comp1PitchPerRow;
   	       break;
   	       case 2: //only cr component
   		   u4Coffe0Pitch = u4Comp2PitchPerRow;
   	       break;
            }
             u4Coffe1Pitch =  0;
             u4Coffe2Pitch =  0;
        }
        else
#endif
        {
             u4Coffe0Pitch = u4Comp0PitchPerRow;
             u4Coffe1Pitch = u4Comp1PitchPerRow;
             u4Coffe2Pitch = u4Comp2PitchPerRow;
        }        
        u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0);
        u4Value = (u4Value & (~(0x3FFFFUL<<14))) | (((u4Coffe0Pitch>>2) & 0x3FFFUL)<<14);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0,u4Value);


        vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEFF_WIDTH12,
                              ((u4Coffe1Pitch>>2) & 0x3FFF)
                              | (((u4Coffe2Pitch>>2) & 0x3FFF) <<14));
    }

	return S_JDEC_HAL_OK;
}


//////////////////////////////////////////////////////////////
//
// Progressive Related Function
//
//////////////////////////////////////////////////////////////
INT32 i4HwJdecSetCoefPitchPerRow(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo)
{
    UINT32 u4CompId;
    UINT32 au4CompPitchPerRow[3] = { 0, 0, 0};
    UINT32 u4Value;
    UINT32 u4MaxHSampleFactor = 0;
    UINT32 u4McuPerRow = 0;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(prPicInfo == NULL)
		return E_JDEC_HAL_FAIL;

    for(u4CompId=0; u4CompId<3; u4CompId++)
    {
        if (prPicInfo->arSofComp[u4CompId].u1HSampFactor >= u4MaxHSampleFactor)
        {
            u4MaxHSampleFactor = prPicInfo->arSofComp[u4CompId].u1HSampFactor;
        }
    }
    u4McuPerRow = (prPicInfo->u2ImageWidth + u4MaxHSampleFactor * 8 - 1)/(u4MaxHSampleFactor * 8);

#if 0
    for(u4CompId=0; u4CompId<3; u4CompId++)
    {
        UINT32 u4WidthPerMCU = (prPicInfo->arSofComp[u4CompId].u1HSampFactor * 8);
        au4CompPitchPerRow[u4CompId] = ((prPicInfo->u2ImageWidth +
                                          u4WidthPerMCU - 1) /
                                          u4WidthPerMCU) * u4WidthPerMCU; // Align to u4WidthPerMCU

        au4CompPitchPerRow[u4CompId] = ((au4CompPitchPerRow[u4CompId] + 15) / 16) * 16; // Align to 16's
        //au4CompPitchPerRow[u4CompId] *= prPicInfo->arSofComp[u4CompId].u1HSampFactor;
        au4CompPitchPerRow[u4CompId] = au4CompPitchPerRow[u4CompId] * 2;
    }
#else  
    for(u4CompId=0; u4CompId<3; u4CompId++)
    {
        au4CompPitchPerRow[u4CompId] = u4McuPerRow * prPicInfo->arSofComp[u4CompId].u1HSampFactor * 8 * 2;
    }
#endif    

    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0);
    u4Value = (u4Value & (~(0x3FFFFUL<<14))) | (((au4CompPitchPerRow[0]>>2) & 0x3FFFUL)<<14);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_IDCT_WIDTH2_COEFF_WIDTH0,u4Value); mb();


    vHwJdecWrite32(u4HwId,RW_VLD_JPG_COEFF_WIDTH12,
                          ((au4CompPitchPerRow[1]>>2) & 0x3FFF)
                          | (((au4CompPitchPerRow[2]>>2) & 0x3FFF) <<14));mb();

	return S_JDEC_HAL_OK;
}


INT32 i4HwJdecSetDcAcNeededList(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo)
{
    UINT32 u4Block;
    UINT32 u4Count;
    UINT32 u4CompNum;
    UINT32 u4CompIdx;
    UINT32 au4CompBlkNum;
    UINT32 u4DcNeededList = 0;
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    UINT32 u4AcNeededList = 0;
#endif
    UINT32 u4Value;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if((prSofComp == NULL) || (prScanInfo == NULL))
		return E_JDEC_HAL_FAIL;

    u4Block = 0;
    for(u4CompNum = 0; (u4CompNum<prScanInfo->u1CompInScan) && (u4CompNum<3); u4CompNum++)
    {
        if(prScanInfo->u1CompInScan == 1)
            au4CompBlkNum = 1;
        else
        {
            u4CompIdx = prScanInfo->au1CompIdx[u4CompNum];
            au4CompBlkNum = prSofComp[u4CompIdx].u1HSampFactor * prSofComp[u4CompIdx].u1VSampFactor;
        }

	    for (u4Count = 0; u4Count < au4CompBlkNum; u4Count++)
	    {
	        u4DcNeededList |= (1<<u4Block);
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
	        u4AcNeededList |= (1<<u4Block);
#endif
	        u4Block++;
	    }
	}

    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_DC_AC_NEEDED);
    u4Value = (u4Value & (~0x3FF)) | (u4DcNeededList & 0x3FF);
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    u4Value = (u4Value & (~(0x3FF<<10))) | ((u4AcNeededList & 0x3FF)<<10);
#endif
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_DC_AC_NEEDED,u4Value);

	return S_JDEC_HAL_OK;
}


INT32 i4HwJdecSetBlockCount(UINT32 u4HwId,JDEC_SOF_COMP_T *prSofComp,JDEC_JFIF_SOS_T *prScanInfo)
{
    UINT32 u4CompNum;
    UINT32 u4CompIdx;
    UINT32 u4BlockCount = 0;
    UINT32 u4Value;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if((prSofComp == NULL) || (prScanInfo == NULL))
		return E_JDEC_HAL_FAIL;

    for(u4CompNum = 0; (u4CompNum<prScanInfo->u1CompInScan) && (u4CompNum<3); u4CompNum++)
    {
        if(prScanInfo->u1CompInScan == 1)
            u4BlockCount = 1;
        else
        {
            u4CompIdx = prScanInfo->au1CompIdx[u4CompNum];
            u4BlockCount += prSofComp[u4CompIdx].u1HSampFactor * prSofComp[u4CompIdx].u1VSampFactor;
        }
    }

    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_DC_AC_NEEDED);
    u4Value = (u4Value & (~(0xF<<24))) | ((u4BlockCount & 0xF)<<24);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_DC_AC_NEEDED,u4Value);

	return S_JDEC_HAL_OK;
}


INT32 i4HwJdecSetEOBRUN(UINT32 u4HwId,UINT32 u4EOBRUN)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_WR_EOB_RUN,u4EOBRUN);

	return S_JDEC_HAL_OK;
}


UINT32 u4HwJdecGetEOBRUN(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    return u4HwJdecRead32(u4HwId,RO_VLD_JPG_RD_EOB_RUN);
}


INT32 i4HwJdecProcRestart(UINT32 u4HwId)
{
    UINT32 u4Value;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecRestart(u4HwId);
    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_RMK_AND_FINISH);
    if(!(u4Value & VLD_RECEIVE_MARKER)) // Not found any marker
    {
        // TODO:
    } else if(((u4Value>>8) & 0xFF) != 0xD7) // Found marker, but not RST marker.
    {
        // TODO:
    }

	return S_JDEC_HAL_OK;
}


//////////////////////////////////////////////////////////////
//
// Huffman Table Related Function
//
//////////////////////////////////////////////////////////////
void vHwJdecResetDcPrediction(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART,0); mb();
	vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART,VLD_RES_DC_PRED); mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART,0);mb();
}


INT32 iHwJdecCalcDerivedHuffTable(UINT8 *au1Bits,UINT8 *au1HuffVal,JDEC_HAL_DERIVED_HUFF_TABLE_T *prDerivedHuffTbl)
{
	//int numsymbols;
	INT32	i4BitLen, i4Symbol, i4LookBits, i4Ctr, i4Size, i4Loop1, i4Loop2;
	INT16	i2Bits;

	INT8	ai1HuffSize[257];
	UINT16	au2HuffCode[257];
	UINT16	u2Code;
    //JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if((au1Bits == NULL) || (au1HuffVal == NULL) || (prDerivedHuffTbl == NULL))
        return E_JDEC_HAL_FAIL;

	/* Note that huffsize[] and huffcode[] are filled in code-length order,
	 * paralleling the order of the symbols themselves in htbl->huffval[].
	 */

	x_memset(ai1HuffSize, 0, sizeof(ai1HuffSize));
	x_memset(au2HuffCode, 0, sizeof(au2HuffCode));
	/* Figure C.1: make table of Huffman code length for each symbol */

	i4Symbol = 0;
	for (i4BitLen = 1; i4BitLen <= 16; i4BitLen++)
	{
		i2Bits = (INT16)au1Bits[i4BitLen];

		if ((i4Symbol + i2Bits) > 256) /* protect against table overrun */
		{
			// Error handler:
			// set to nearest value
			i2Bits = 255 - i4Symbol;	/* p + i = 255 */
		}
        if (i4Symbol > 256)
        {
             UTIL_Printf("ERROR0: i4Symbol =0x%x\n",i4Symbol);
			 return E_JDEC_HAL_FAIL;            
        }
		while (i2Bits--)  // safe while
		{
   		   if (i4Symbol > 256)
           {
               UTIL_Printf("ERROR1: i4Symbol =0x%x\n",i4Symbol);
   			   return E_JDEC_HAL_FAIL;            
           }
			ai1HuffSize[i4Symbol++] = (INT8) i4BitLen;
		}
	}
	if (i4Symbol > 256)
	{
	   UTIL_Printf("ERROR2: i4Symbol =0x%x\n",i4Symbol);
	   return E_JDEC_HAL_FAIL;			 
	}

	ai1HuffSize[i4Symbol] = 0;
	//numsymbols = p;

	/* Figure C.2: generate the codes themselves */
	/* We also validate that the counts represent a legal Huffman code tree. */

	i4Symbol	= 0;
	u2Code		= 0;
	i4Size		= ai1HuffSize[0];

#define BIG_LOOP 5000

	for (i4Loop1 = 0; i4Loop1 < BIG_LOOP; i4Loop1++)
	{
        if (i4Symbol > 256)
        {
            break;
        }
		if (ai1HuffSize[i4Symbol])
		{
			for (i4Loop2 = 0; i4Loop2 < BIG_LOOP; i4Loop2++)
			{
				if (((INT32) ai1HuffSize[i4Symbol]) == i4Size)
				{
					au2HuffCode[i4Symbol++] = u2Code;
					u2Code++;
                    if (i4Symbol > 256)
                    {
                        break;
                    }
				}
				else
				{
					break;
				}
			}
			/* code is now 1 more than the last code used for codelength si; but
			* it must still fit in si bits, since no code is allowed to be all ones.
			*/

			if (((INT32) u2Code) >= (((INT32) 1) << i4Size))
			{
				// Error handler:
				// set to 0
				u2Code = 0;
			}

			u2Code <<= 1;
			i4Size++;
		}// huffsize[9]
		else
		{
			break;
		}
	}// BIG_LOOP

	/* Figure F.15: generate decoding tables for bit-sequential decoding */

	i4Symbol = 0;
	for (i4BitLen = 1; i4BitLen <= 16; i4BitLen++)
	{
		if (au1Bits[i4BitLen])
		{
			/* valoffset[l] = huffval[] index of 1st symbol of code length l,
			 * minus the minimum code of length l
			 */
			prDerivedHuffTbl->ai4Valoffset[i4BitLen] =
				(INT32)i4Symbol - (INT32)au2HuffCode[i4Symbol];
			i4Symbol += au1Bits[i4BitLen];
            // For klocwork issuse
            if ((i4Symbol > 257) || (i4Symbol < 1))
            {
                UTIL_Printf("ERROR3: i4Symbol =0x%x\n",i4Symbol);
			    return E_JDEC_HAL_FAIL;
               // ASSERT(0);
            }
			prDerivedHuffTbl->ai4Maxcode[i4BitLen] =
				(INT32)au2HuffCode[i4Symbol - 1];		/* maximum code of length l */
		}
		else
		{
			prDerivedHuffTbl->ai4Valoffset[i4BitLen] = 0;
				/* clear to zero if no codes of this length */
			prDerivedHuffTbl->ai4Maxcode[i4BitLen] = -1;
				/* -1 if no codes of this length */
		}
	}
	prDerivedHuffTbl->ai4Maxcode[17] = (INT32)0xFFFFF;
		/* ensures i2Jpeg_huff_decode terminates */

	/* Compute lookahead tables to speed up decoding.
	 * First we set all the table entries to 0, indicating "too long";
	 * then we iterate through the Huffman codes that are short enough and
	 * fill in all the entries that correspond to bit sequences starting
	 * with that code.
	 */

	for (i4Loop1 = 0; i4Loop1 < (1 << D_HUFF_LOOKAHEAD_BITS); i4Loop1++)
	{
		prDerivedHuffTbl->ai4LookNbits[i4Loop1] = 0;
	}

	i4Symbol = 0;
	for (i4BitLen = 1; i4BitLen <= D_HUFF_LOOKAHEAD_BITS; i4BitLen++)
	{
		for (i2Bits = 1; i2Bits <= (INT16) au1Bits[i4BitLen]; i2Bits++, i4Symbol++)
		{
			/* i4BitLen = current code's length, i4Symbol = its index in huffcode[] & huffval[]. */
			/* Generate left-justified code followed by all possible bit sequences */
			i4LookBits = (INT32)au2HuffCode[i4Symbol] << (D_HUFF_LOOKAHEAD_BITS - i4BitLen);
			for (i4Ctr = 1 << (D_HUFF_LOOKAHEAD_BITS - i4BitLen); i4Ctr > 0; i4Ctr--)
			{
			    if(i4LookBits >((1 << D_HUFF_LOOKAHEAD_BITS)-1))
			    {
			      UTIL_Printf("ERROR4: i4LookBits =0x%x\n",i4LookBits);
				  return E_JDEC_HAL_FAIL;			 
			    }
				prDerivedHuffTbl->ai4LookNbits[i4LookBits] = i4BitLen;
				prDerivedHuffTbl->au1LookSym[i4LookBits] = au1HuffVal[i4Symbol];
				i4LookBits++;
			}
		}
	}

    return S_JDEC_HAL_OK;
}


// load ac look ahead table
INT32 iHwJdecLoadAcLookAheadTable(UINT32 u4HwId,UINT8 u1HuffTblNo,JDEC_HAL_DERIVED_HUFF_TABLE_T *prDerivedHuffTbl)
{
	INT32 i4LookAhead;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(prDerivedHuffTbl == NULL)
        return E_JDEC_HAL_FAIL;

    vHwJdecWrite32(u4HwId,RW_VLD_JPG_LAHUFF_IDX,(UINT32)u1HuffTblNo);
    

	for (i4LookAhead = 0; i4LookAhead < 64; i4LookAhead++)
	{
		vHwJdecWrite32(u4HwId,RW_VLD_JPG_LAHUFF_DATA,
			((prDerivedHuffTbl->ai4LookNbits[i4LookAhead] << 8) |
			(UINT32)prDerivedHuffTbl->au1LookSym[i4LookAhead]));mb();
	}

	vHwJdecResetDcPrediction(u4HwId);

	return S_JDEC_HAL_OK;
}


// maxcode and valoffset
INT32 i4HwJdecSetBitsTable(UINT32 u4HwId,INT32 *ai4Value)
{
	INT32 i4Index;

	if(ai4Value == NULL)
        return E_JDEC_HAL_FAIL;

	for (i4Index = 1; i4Index <= 16; i4Index++)
	{
        vHwJdecWrite32(u4HwId,WO_VLD_JPG_BITS_TBL,(UINT32)(ai4Value[i4Index]));mb();
	}

	return S_JDEC_HAL_OK;
}

// for 8530, internal huffman table
INT32 i4HwJdecSetHuffTable(UINT32 u4HwId,BOOL fgDC, UINT32 u4TblIdx,UINT8 *au1Huffval)
{
    INT32 i4Index = 0;
    UINT32 u4Value = 0;

    //JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
   // JDEC_VERY_HAL_LOG("HuffVal Dram Address:%x!\n", (UINT32)au1Huffval);
    
    if ((au1Huffval == NULL) || (u4TblIdx > 3))
    {
        return E_JDEC_HAL_FAIL;
    }
    if (fgDC)
    {
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_FILL_HSRAM,(u4HwJdecRead32(u4HwId, RW_VLD_JPG_FILL_HSRAM) & 0xF660)|DC_TABLE|(u4TblIdx << 8));mb();
    }
    else
    {
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_FILL_HSRAM,(u4HwJdecRead32(u4HwId, RW_VLD_JPG_FILL_HSRAM) & 0xF660)|AC_TABLE|(u4TblIdx << 8));mb();
    }
    for (i4Index = 0; i4Index < 256; i4Index +=4)
    {
        u4Value = ((UINT32)au1Huffval[i4Index + 3] << 24) |
                  ((UINT32)au1Huffval[i4Index + 2] << 16) |
                  ((UINT32)au1Huffval[i4Index + 1] << 8)|
                  ((UINT32)au1Huffval[i4Index + 0]);
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_HUFF_VAL, u4Value);mb();
    }
    return 0;
}

// Huffman Value address
INT32 i4HwJdecSetHuffAddr(UINT32 u4HwId,UINT32 u4DcHuff, UINT32 u4AcHuff)
{
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if((u4DcHuff == 0) || (u4AcHuff == 0))
        return E_JDEC_HAL_FAIL;

	if ((u4DcHuff & 15) || (u4AcHuff & 15))
		return E_JDEC_HAL_FAIL;

    u4DcHuff = u4DcHuff ? PHYSICAL(u4DcHuff) : 0;
    u4AcHuff = u4AcHuff ? PHYSICAL(u4AcHuff) : 0;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_HUFF_TBL,u4DcHuff);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_HUFF_TBL,u4AcHuff | VLD_AC_HUFF_FLAG);mb();
	return S_JDEC_HAL_OK;
}


INT32 i4HwJdecTransHuffTblToHwFormat(UINT8 *pu1DstAddr, UINT8 *au1Huffval)
{
	UINT8 *pu1Huff;
	INT32 i4Index;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if((pu1DstAddr == NULL) || (au1Huffval == NULL))
        return E_JDEC_HAL_FAIL;

	pu1Huff = pu1DstAddr;

	for (i4Index = 0; i4Index < 256; i4Index += 4, pu1Huff += 4)
	{
		// every 128 bits, write only 32 bits data, rest 96 bits fill zero
		//x_memcpy(pu1Huff, &au1Huffval[i4Index], 4);
		pu1Huff[0] = au1Huffval[i4Index + 0];
		pu1Huff[1] = au1Huffval[i4Index + 1];
		pu1Huff[2] = au1Huffval[i4Index + 2];
		pu1Huff[3] = au1Huffval[i4Index + 3];
		//x_memset(pu1Huff + 4, 0, 12);
	}

	return S_JDEC_HAL_OK;
}

void vHwJdecSwitchToBusClock(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_HSRAM_INIT,SRAM_CLOCK_DISABLE);mb();
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_HSRAM_INIT,FILL_ENABLE);mb();
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_HSRAM_INIT,(SRAM_CLOCK_ENABLE | FILL_ENABLE));mb();
}

void vHwJdecSwitchToEngineClock(UINT32 u4HwId)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_HSRAM_INIT,FILL_ENABLE);mb();
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_HSRAM_INIT,SRAM_CLOCK_DISABLE);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_HSRAM_INIT,SRAM_CLOCK_ENABLE);mb();
}



INT32 i4HwJdecSetHuffmanTable(UINT32 u4HwId,JDEC_JFIF_DHT_T *prHuffmanTable,
                              UINT8 *pu1HwDcHuffTbl,UINT8 *pu1HwAcHuffTbl)
{
    UINT32 u4DcHuffTblNum = 4;//prHuffmanTable->u4NumDcTbl;
    UINT32 u4AcHuffTblNum = 4;//prHuffmanTable->u4NumAcTbl;
    UINT32 u4TblNum;
    JDEC_HAL_DERIVED_HUFF_TABLE_T *prDerivedDcHuffmanTable[4] ={NULL};
    JDEC_HAL_DERIVED_HUFF_TABLE_T *prDerivedAcHuffmanTable[4] ={NULL};
    INT32 i4Ret;
    UINT32 u4I;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    if((pu1HwDcHuffTbl == NULL) || (pu1HwAcHuffTbl == NULL))
		return E_JDEC_HAL_FAIL;
#endif    

    // Allocate temp buffer.
    for(u4I=0; u4I<4; u4I++)
    {

	    prDerivedDcHuffmanTable[u4I] = (JDEC_HAL_DERIVED_HUFF_TABLE_T *) x_hw_mem_alloc(sizeof(JDEC_HAL_DERIVED_HUFF_TABLE_T),32,FALSE);
        prDerivedAcHuffmanTable[u4I] = (JDEC_HAL_DERIVED_HUFF_TABLE_T *) x_hw_mem_alloc(sizeof(JDEC_HAL_DERIVED_HUFF_TABLE_T),32,FALSE);
    }


    // Derive DC Huffman Table
    for(u4TblNum=0; u4TblNum<u4DcHuffTblNum; u4TblNum++)
    {  
	   i4Ret=   iHwJdecCalcDerivedHuffTable(
            prHuffmanTable->arDcTbl[u4TblNum].au1Bits,
            prHuffmanTable->arDcTbl[u4TblNum].au1HuffVal,
            prDerivedDcHuffmanTable[u4TblNum]);
	   if(i4Ret!=S_JDEC_HAL_OK)
	   {
  	    for(u4I=0; u4I<4; u4I++)
        {
            x_hw_mem_free((void*)prDerivedDcHuffmanTable[u4I], FALSE);
            x_hw_mem_free((void*)prDerivedAcHuffmanTable[u4I], FALSE);
			return i4Ret;
         }
	   }
    }
    // Derive AC Huffman Table
    for(u4TblNum=0; u4TblNum<u4AcHuffTblNum; u4TblNum++)
    {
      i4Ret= iHwJdecCalcDerivedHuffTable(
            prHuffmanTable->arAcTbl[u4TblNum].au1Bits,
            prHuffmanTable->arAcTbl[u4TblNum].au1HuffVal,
            prDerivedAcHuffmanTable[u4TblNum]);
	  if(i4Ret!=S_JDEC_HAL_OK)
	  {
  	    for(u4I=0; u4I<4; u4I++)
        {
            x_hw_mem_free((void*)prDerivedDcHuffmanTable[u4I], FALSE);
            x_hw_mem_free((void*)prDerivedAcHuffmanTable[u4I], FALSE);
			return i4Ret;
         }
	  	}
    }

    
    #if CONFIG_DRV_LINUX_DATA_CONSISTENCY
    {
        
        for(u4TblNum=0; u4TblNum<u4DcHuffTblNum; u4TblNum++)
        {
            #if !JDEC_USE_MAP_UNMAP
            BSP_FlushDCacheRange((UINT32)(&prHuffmanTable->arDcTbl[u4TblNum]), sizeof(JDEC_JFIF_DHT_T));
            //BSP_FlushDCacheRange((UINT32)(&prHuffmanTable->arAcTbl[u4TblNum]), sizeof(JDEC_JFIF_DHT_T));
            BSP_FlushDCacheRange((UINT32)prDerivedDcHuffmanTable[u4TblNum], sizeof(JDEC_HAL_DERIVED_HUFF_TABLE_T));
            #endif
        }

        for (u4TblNum = 0; u4TblNum < u4AcHuffTblNum; u4TblNum ++)
        {
            #if !JDEC_USE_MAP_UNMAP
            BSP_FlushDCacheRange((UINT32)(&prHuffmanTable->arAcTbl[u4TblNum]), sizeof(JDEC_JFIF_DHT_T));
            BSP_FlushDCacheRange((UINT32)prDerivedAcHuffmanTable[u4TblNum], sizeof(JDEC_HAL_DERIVED_HUFF_TABLE_T));
            #endif
        }
     //HalFlushInvalidateDCache();
    }
    //BSP_FlushDCacheRange();
    #endif

    // For 8530 
    //vHwJdecSwitchToBusClock(u4HwId);
    // Set AC Look Ahead Table to HW.
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8530)
    for(u4TblNum=0; u4TblNum<u4AcHuffTblNum; u4TblNum++)
        iHwJdecLoadAcLookAheadTable(u4HwId,u4TblNum,prDerivedAcHuffmanTable[u4TblNum]);
#endif


#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
     vHwJdecWrite32(u4HwId,RW_VLD_JPG_LAHUFF_IDX,0x0); mb();
#endif
    // Set DC Maxcode Table
    for(u4TblNum=0; u4TblNum<4; u4TblNum++)
        i4HwJdecSetBitsTable(u4HwId,prDerivedDcHuffmanTable[u4TblNum]->ai4Maxcode);
    // Set DC Valoffset Table
    for(u4TblNum=0; u4TblNum<4; u4TblNum++)
        i4HwJdecSetBitsTable(u4HwId,prDerivedDcHuffmanTable[u4TblNum]->ai4Valoffset);
    // Set AC Maxcode Table
    for(u4TblNum=0; u4TblNum<4; u4TblNum++)
        i4HwJdecSetBitsTable(u4HwId,prDerivedAcHuffmanTable[u4TblNum]->ai4Maxcode);
    // Set AC Valoffset Table
    for(u4TblNum=0; u4TblNum<4; u4TblNum++)
        i4HwJdecSetBitsTable(u4HwId,prDerivedAcHuffmanTable[u4TblNum]->ai4Valoffset);

    // Free temp buffer.
    for(u4I=0; u4I<4; u4I++)
    {
       x_hw_mem_free((void*)prDerivedDcHuffmanTable[u4I], FALSE);
       x_hw_mem_free((void*)prDerivedAcHuffmanTable[u4I], FALSE);
    }
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8520) 
    // new huffman table address
    // DC huffman table
    for (u4TblNum = 0; u4TblNum < 4; u4TblNum ++)
        i4HwJdecSetHuffTable(u4HwId, TRUE, u4TblNum, prHuffmanTable->arDcTbl[u4TblNum].au1HuffVal);

    // AC huffman table
    for (u4TblNum = 0; u4TblNum < 4; u4TblNum ++)
        i4HwJdecSetHuffTable(u4HwId, FALSE,u4TblNum, prHuffmanTable->arAcTbl[u4TblNum].au1HuffVal);

    //vHwJdecSwitchToEngineClock(u4HwId);

#else
    // Transfer Huffman Table to HW format Huffman Table
    for(u4TblNum=0; u4TblNum<4; u4TblNum++)
        i4HwJdecTransHuffTblToHwFormat(pu1HwDcHuffTbl+(u4TblNum<<8),
                                       prHuffmanTable->arDcTbl[u4TblNum].au1HuffVal);
    for(u4TblNum=0; u4TblNum<4; u4TblNum++)
        i4HwJdecTransHuffTblToHwFormat(pu1HwAcHuffTbl+(u4TblNum<<8),
                                       prHuffmanTable->arAcTbl[u4TblNum].au1HuffVal);
    // Set HW format Huffman Table Address
    if((i4Ret = i4HwJdecSetHuffAddr(u4HwId,(UINT32)pu1HwDcHuffTbl,(UINT32)pu1HwAcHuffTbl)) < 0)
        return i4Ret;
#endif    

	return S_JDEC_HAL_OK;
}


//////////////////////////////////////////////////////////////
//
// Picture Mode (MCU Row Mode) Related Function
//
//////////////////////////////////////////////////////////////
void vHwJdecSetOutputBank0BufAddr(UINT32 u4HwId,UINT32 u4Comp0Addr,UINT32 u4Comp1Addr,UINT32 u4Comp2Addr)
{
    UINT32 u4Value;
    JDEC_VERY_HAL_LOG("Enter %s![0x%x][0x%x][0x%x]\n", __FUNCTION__,u4Comp0Addr,u4Comp1Addr,u4Comp2Addr);
    u4Comp0Addr = u4Comp0Addr ? PHYSICAL(u4Comp0Addr) : 0;
    u4Comp1Addr = u4Comp1Addr ? PHYSICAL(u4Comp1Addr) : 0;
    u4Comp2Addr = u4Comp2Addr ? PHYSICAL(u4Comp2Addr) : 0;

    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH);
    u4Value |= JPG_ROW_DEC_WR_ADDR;
    u4Value &= ~(JPG_ROW_DEC_WR_BANK1_ADDR);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH,u4Value);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_ROW_DEC_COMP0_ADDR,u4Comp0Addr/4);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_ROW_DEC_COMP1_ADDR,u4Comp1Addr/4);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_ROW_DEC_COMP2_ADDR,u4Comp2Addr/4);mb();
}


void vHwJdecSetOutputBank1BufAddr(UINT32 u4HwId,UINT32 u4Comp0Addr,UINT32 u4Comp1Addr,UINT32 u4Comp2Addr)
{
    UINT32 u4Value;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Comp0Addr = u4Comp0Addr ? PHYSICAL(u4Comp0Addr) : 0;
    u4Comp1Addr = u4Comp1Addr ? PHYSICAL(u4Comp1Addr) : 0;
    u4Comp2Addr = u4Comp2Addr ? PHYSICAL(u4Comp2Addr) : 0;

    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH);
    u4Value |= JPG_ROW_DEC_WR_ADDR;
    u4Value |= JPG_ROW_DEC_WR_BANK1_ADDR;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH,u4Value);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_ROW_DEC_COMP0_ADDR,u4Comp0Addr/4); mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_ROW_DEC_COMP1_ADDR,u4Comp1Addr/4); mb(); 
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_ROW_DEC_COMP2_ADDR,u4Comp2Addr/4); mb();
}


void vHwJdecSetMCUWidth(UINT32 u4HwId,UINT32 u4Comp0MCUWidth,UINT32 u4Comp1MCUWidth,UINT32 u4Comp2MCUWidth)
{
    UINT32 u4Value;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    //set each component MCU width
    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH);
    u4Value |= JPG_ROW_DEC_WR_ADDR;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH,u4Value);mb();
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8555)
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_ROW_DEC_COMP0_ADDR_OFST,u4Comp0MCUWidth/4);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_ROW_DEC_COMP1_ADDR_OFST,u4Comp1MCUWidth/4);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_ROW_DEC_COMP2_ADDR_OFST,u4Comp2MCUWidth/4);mb();
#endif
}


void vHwJdecSetPictureSize(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo)
{
    UINT32 u4MCUsPerRow;
    UINT32 u4TotalMCURows;
    UINT32 u4WidthPerMCU = prPicInfo->arSofComp[0].u1HSampFactor * 8;
    UINT32 u4HeightPerMCU = prPicInfo->arSofComp[0].u1VSampFactor * 8;
    JDEC_VERY_HAL_LOG("Enter %s,u1HSampFactor:u1VSampFactor[0x%x:0x%x]\n", __FUNCTION__,
		prPicInfo->arSofComp[0].u1HSampFactor,
		prPicInfo->arSofComp[0].u1VSampFactor);
    u4MCUsPerRow = (prPicInfo->u2ImageWidth + u4WidthPerMCU - 1) / u4WidthPerMCU;
    u4TotalMCURows = (prPicInfo->u2ImageHeight + u4HeightPerMCU - 1) / u4HeightPerMCU;

    //set picture width and height
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_PIC_SIZE,(u4MCUsPerRow<<16)+u4TotalMCURows);
}

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
void vHwJdecSetPictureSizeForProg(UINT32 u4HwId,JDEC_JFIF_SOF_T *prPicInfo,JDEC_JFIF_SOS_T *prSosInfo)
{
    UINT32 u4MCUsPerRow;
    UINT32 u4TotalMCURows;
    UINT32 u4CompId;
    UINT32 au4MCUWidth[3] = {0,0,0};
    UINT32 au4MCUHeight[3] = {0,0,0};
    UINT32 u4CombMCUWidth;
    UINT32 u4CombMCUHeight;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    for(u4CompId = 0; u4CompId< prPicInfo->u1NumComponents; u4CompId++)
    {
        UINT32 u4WidthPerMCU = (prPicInfo->arSofComp[0].u1HSampFactor /
                                prPicInfo->arSofComp[u4CompId].u1HSampFactor) * 8;
        UINT32 u4HeightPerMCU = (prPicInfo->arSofComp[0].u1VSampFactor /
                                 prPicInfo->arSofComp[u4CompId].u1VSampFactor) * 8;

        au4MCUWidth[u4CompId] = (prPicInfo->u2ImageWidth + u4WidthPerMCU - 1) / u4WidthPerMCU;
        au4MCUHeight[u4CompId] = (prPicInfo->u2ImageHeight + u4HeightPerMCU - 1) / u4HeightPerMCU;
    }

    u4CombMCUWidth = (prPicInfo->u2ImageWidth + prPicInfo->arSofComp[0].u1HSampFactor * 8 - 1) /
                     (prPicInfo->arSofComp[0].u1HSampFactor * 8);
    u4CombMCUHeight = (prPicInfo->u2ImageHeight + prPicInfo->arSofComp[0].u1VSampFactor * 8 - 1) /
                      (prPicInfo->arSofComp[0].u1VSampFactor * 8);

    if(prSosInfo->u1CompInScan == 1)
    {
        UINT32 u4CompIdx = prSosInfo->au1CompIdx[0];
        u4MCUsPerRow = au4MCUWidth[u4CompIdx];
        u4TotalMCURows = au4MCUHeight[u4CompIdx];
    } else
    {
        u4MCUsPerRow = u4CombMCUWidth;
        u4TotalMCURows = u4CombMCUHeight;
    }
    //set picture width and height
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_PIC_SIZE,(u4MCUsPerRow<<16)+u4TotalMCURows);
}
#endif

void vHwJdecSetRestartInterval(UINT32 u4HwId,UINT32 u4RestartInterval)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520) 
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART_INTERVAL,((u4RestartInterval-1) & 0xFFFF));
#else
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_RESTART_INTERVAL,((u4RestartInterval-1) & 0xFFFF)| RST_ENABLE);
#endif
}


void vHwJdecSetDecodeMode(UINT32 u4HwId,JDEC_HAL_DEC_MODE_T eDecodeMode)
{
    UINT32 u4Value;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Value = u4HwJdecRead32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH);
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    u4Value &= ~(JPG_ROW_DEC_MCU_LEVEL | JPG_ROW_DEC_MCU_ROW_LEVEL | JPG_ROW_DEC_PIC_LEVEL | JPG_ROW_DEC_SCAN_LEVEL);
#else
    u4Value &= ~(JPG_ROW_DEC_MCU_LEVEL | JPG_ROW_DEC_MCU_ROW_LEVEL | JPG_ROW_DEC_PIC_LEVEL);
#endif

    switch(eDecodeMode)
    {
        case JDEC_HAL_DEC_MODE_BASELINE_MCU:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE:
            u4Value |= JPG_ROW_DEC_MCU_LEVEL;
            vHwJdecWrite32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH,u4Value);
            break;
        case JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW:
            u4Value |= JPG_ROW_DEC_MCU_ROW_LEVEL;
            vHwJdecWrite32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH,u4Value);
            break;
        case JDEC_HAL_DEC_MODE_BASELINE_PIC:
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)  
        case JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT:
#endif           
            u4Value |= JPG_ROW_DEC_PIC_LEVEL;
            vHwJdecWrite32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH,u4Value);
            break;
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE:
            u4Value |= JPG_ROW_DEC_MCU_ROW_LEVEL;
            vHwJdecWrite32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH,u4Value);
            break;
 #if JDEC_MM_MODE_ENABLE
	case JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT_MULTI_PASS: 
		{
			  u4Value &= (~JPG_ROW_DEC_WR_ADDR);
		}
 #endif		 
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT:
        case JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE:
            u4Value |= JPG_ROW_DEC_PIC_LEVEL;
            vHwJdecWrite32(u4HwId,RW_VLD_JPG_MB_ROW_DEC_SWITCH,u4Value);
            break;
#endif
        default:
            ASSERT(0);
    }
}


void vHwJdecDecodePicture(UINT32 u4HwId)
{
    //trigger jpeg picture level decode
#if 0
    JDEC_VERY_HAL_LOG("Enter %s and dump triger register!\n", __FUNCTION__);     
	if(!_fgEnableRegisterLog)
	{
          _fgEnableRegisterLog = 2;
	}
    vHwJdecPrintRegister(u4HwId);
	if(2==_fgEnableRegisterLog)
	{
	   _fgEnableRegisterLog = 0;
	}
	JDEC_VERY_HAL_LOG("-----------------------------------!\n", __FUNCTION__);  
#else
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);  
  if(_debug_decode_done_log)
    vHwJdecPrintRegister(u4HwId);
#endif
    vHwJdecWrite32(u4HwId,RW_VLD_STA_JPG_DEC_MB_ROW,JPG_ROW_DEC_FIRST_ROW);mb();
    vHwJdecWrite32(u4HwId,RW_VLD_STA_JPG_DEC_MB_ROW,0);mb();
}


BOOL fgHwJdecIsDecodePictureFinish(UINT32 u4HwId)
{
    BOOL fgRet;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    fgRet = ((u4HwJdecRead32(u4HwId,RW_VLD_JPG_ROW_PIC_DEC_FINISH) & JPG_ROW_DEC_PIC_FINISH) == JPG_ROW_DEC_PIC_FINISH);
    return fgRet;
}


void vHwJdecDecodeMCURow(UINT32 u4HwId,BOOL fgFirstRow)
{
     JDEC_VERY_HAL_LOG("Enter %s----------------!\n", __FUNCTION__);
    //trigger jpeg picture level decode
    if(_debug_decode_done_log)
     vHwJdecPrintRegister(u4HwId);
#if 0	
 	{
 	   UTIL_Printf("====decode mcu row\n");
	   vHwJdecPrintRegister_ex(u4HwId);
    }
#endif
    while(u4HwJdecRead32(u4HwId,0x144)&0x2)
    {
      	UTIL_Printf("[0xf003e144] 0x%8x\n",u4HwJdecRead32(u4HwId,0x144));
    }
    if(fgFirstRow)
    {
        vHwJdecWrite32(u4HwId,RW_VLD_STA_JPG_DEC_MB_ROW,JPG_ROW_DEC_FIRST_ROW); mb();
    }
    else
    {
        vHwJdecWrite32(u4HwId,RW_VLD_STA_JPG_DEC_MB_ROW,JPG_ROW_DEC_NEXT_ROW); mb();
    }

    vHwJdecWrite32(u4HwId,RW_VLD_STA_JPG_DEC_MB_ROW,0);mb();
}


BOOL fgHwJdecIsDecodeMCURowFinish(UINT32 u4HwId)
{
    BOOL fgRet;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    fgRet = ((u4HwJdecRead32(u4HwId,RW_VLD_JPG_ROW_PIC_DEC_FINISH) & JPG_ROW_DEC_MCU_ROW_FINISH) == JPG_ROW_DEC_MCU_ROW_FINISH);
    return fgRet;
}


//////////////////////////////////////////////////////////////
//
// Re-initialization
//
//////////////////////////////////////////////////////////////
INT32 i4HwJdecGetHwStatus(UINT32 u4HwId,JDEC_HAL_HW_STATUS_T *prHwStatus)
{
    UINT32 u4Reg;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Reg = u4HwJdecRead32(u4HwId,RO_VLD_JPG_BITS_LEFT);
    prHwStatus->u4BitsLeftRead = u4Reg & 0xFF;
    prHwStatus->u4BufPtrRead = (u4Reg >> 8) & 0xFF;
    prHwStatus->u4ShiftMag = (u4Reg >> 16) & 0xFF;
    prHwStatus->u4MkrRm2 = (u4Reg >> 24) & 0x1;
    prHwStatus->u4JpegBufRead = u4HwJdecRead32(u4HwId,RO_VLD_JPG_INTERNAL_BUF);
    prHwStatus->u4RestartMkr = u4HwJdecRead32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA3) & 0xFFFF;
    u4Reg = u4HwJdecRead32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA4);
    prHwStatus->u4LastDcValue0 = u4Reg & 0xFFFF;
    prHwStatus->u4LastDcValue1 = (u4Reg >> 16) & 0xFFFF;
    prHwStatus->u4LastDcValue2 = u4HwJdecRead32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA5);

	return S_JDEC_HAL_OK;
}


INT32 i4HwJdecSetHwStatus(UINT32 u4HwId,JDEC_HAL_HW_STATUS_T *prHwStatus)
{
    UINT32 u4Reg;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Reg = prHwStatus->u4BitsLeftRead & 0xFF;
    u4Reg |= (prHwStatus->u4BufPtrRead & 0xFF) << 8;
    u4Reg |= (prHwStatus->u4ShiftMag & 0xFF) << 16;
    u4Reg |= (prHwStatus->u4MkrRm2 & 0x1) << 24;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA1,u4Reg);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA2,prHwStatus->u4JpegBufRead);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA3,(prHwStatus->u4RestartMkr & 0xFFFF) << 16);
    u4Reg = prHwStatus->u4LastDcValue0 & 0xFFFF;
    u4Reg |= (prHwStatus->u4LastDcValue1 & 0xFFFF) << 16;
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA4,u4Reg);
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA5,prHwStatus->u4LastDcValue2);mb();

    // Trigger load inter data
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA_TRIGGER,1); mb();
    vHwJdecWrite32(u4HwId,RW_VLD_JPG_LOAD_INTER_DATA_TRIGGER,0); mb();

	return S_JDEC_HAL_OK;
}


void vHwJdecSetNewBaseline(UINT32 u4HwId, BOOL fgEnable)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if (fgEnable)
    {
        vHwJdecWrite32(u4HwId, RW_VLD_JPG_NEW_BASE, NEW_BASELINE_ENABLE);
    }
    else
    {
        vHwJdecWrite32(u4HwId, RW_VLD_JPG_NEW_BASE, NEW_BASELINE_DISABLE);
    }
}
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
void vHwJDecSetNewProgressive(UINT32 u4HwId, BOOL fgMultiEnable, BOOL fgEnhanceEnable)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    if(fgMultiEnable || fgEnhanceEnable)
    {
        if(fgMultiEnable)
            vHwJdecWrite32(u4HwId, RW_VLD_JPG_NEW_BASE, NEW_PROGRESSIVE_MULTI_COLLECT);
        else
            vHwJdecWrite32(u4HwId, RW_VLD_JPG_NEW_BASE, NEW_PROGRESSIVE_ENHANCE);
    }
    else
    {
        vHwJdecWrite32(u4HwId, RW_VLD_JPG_NEW_BASE, NEW_BASELINE_DISABLE);
    }
}
#endif

void vHwJdecSetInterrupt(UINT32 u4HwId, BOOL fgEnable)
{
    JDEC_VERY_HAL_LOG("Enter %s enable[%d]!\n", __FUNCTION__,fgEnable);
    if (fgEnable)
    {
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_SWITCH, (u4HwJdecRead32(u4HwId, RW_VLD_JPG_SWITCH) & ~VLD_INT_ENABLE)| VLD_INT_ENABLE);
    }
    else
    {
        vHwJdecWrite32(u4HwId,RW_VLD_JPG_SWITCH, (u4HwJdecRead32(u4HwId, RW_VLD_JPG_SWITCH) & 0xFFEF));
    }
}
extern BOOL fgDisableErrConceal;

INT32 i4HwJdecGetErrorStatus(UINT32 u4HwId)
{

#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)
    UINT32 u4Reg = 0;
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)

    if(!fgDisableErrConceal)
  	{
  	   return 0;
  	}
#endif
    u4Reg = u4HwJdecRead32(u4HwId, RW_VLD_JPG_RESTART_INTERVAL);
    if (u4Reg & (0x1 << 16))
        return (u4HwJdecRead32(u4HwId, RW_VLD_JPG_ERROR_STATUS)/* & 0x31F*/);
    else
        return (u4HwJdecRead32(u4HwId, RW_VLD_JPG_ERROR_STATUS) & 0xFFFFFFDF/* & 0x31F*/);
#endif

    return 0;
}

INT32 i4JDECReadVldRPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits, UINT32 u4VFIFOSa, UINT32 u4VFIFOEa,UINT32 *pu4Bytes)
{
    UINT32 u4DramRptr;
    UINT32 u4SramRptr, u4SramWptr,u4SramCount;
    UINT32 u4SramDataSz;
    UINT32 u4ByteAddr;
    UINT32 u4TimeCount = 0;
    INT32 i4Ret = S_JDEC_HAL_OK;
      JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    // HW issue, wait for read pointer stable
    //while (!(u4HwJdecRead32(u4VDecID,RO_VIDEO_BUFFER_CTRL) & 1)); //?
    while (TRUE)
    {
        
        if (u4HwJdecRead32(u4VDecID,RO_VIDEO_BUFFER_CTRL) & 1)
            break;
        else
        {
            u4TimeCount ++;
            if (u4TimeCount > 1000000)
            {
                UTIL_Printf("[JDEC_HAL]Wait rd ptr stable fail \n");
                i4Ret = E_JDEC_HAL_FAIL;
                break;
            }
        }
        
    }
    
    if (i4Ret == E_JDEC_HAL_FAIL)
        return E_JDEC_HAL_FAIL;
    
    u4DramRptr = u4HwJdecRead32(u4VDecID, RW_VLD_HW_READ_POINT + (u4BSID << 10));
    // sram_ab
    u4SramRptr = u4HwJdecRead32(u4VDecID, RW_VLD_BS_RW_POINT + (u4BSID << 10)) & 0xf;
    // sram_aa
    u4SramWptr = (u4HwJdecRead32(u4VDecID, RW_VLD_BS_RW_POINT + (u4BSID << 10)) >> 8) & 0xf;
    // sram_ctr
    u4SramCount = (u4HwJdecRead32(u4VDecID,RW_VLD_BS_RW_POINT + (u4BSID << 10)) >> 24)& 0x3;

    // calculate line number
    if (u4SramWptr > u4SramRptr)
    {
        u4SramDataSz = u4SramWptr - u4SramRptr;
    }
    else
    {
        u4SramDataSz = 16 - (u4SramRptr - u4SramWptr);
    }
  
    *pu4Bits = u4HwJdecRead32(u4VDecID, VLD_BS_SUM + (u4BSID << 10)) & 0x3f;
  
    
    //u4ByteAddr = u4DramRptr - (u4SramDataSz + 3)* 4 + (*pu4Bits / 8);
    /* u4DramRptr : DRAM read pointer
       u4SramDataSz * 16 - u4SramCount * 4 : byte left in SRAM
       3 * 4 : barrel shifter byte
       pu4Bits/8 : sum in barrel shifter
     */
    u4ByteAddr = u4DramRptr - u4SramDataSz * 16 + u4SramCount * 4 - 3 * 4 + *pu4Bits/8 ;
    
    *pu4Bits &= 0x7; // arithmetical compliment for pu4Bits/8 
    
    if (u4ByteAddr >= u4VFIFOSa)
    {
    u4ByteAddr -= ((UINT32)u4VFIFOSa);
    }
    else
    {
        u4ByteAddr = u4VFIFOEa - (u4VFIFOSa - u4ByteAddr) - u4VFIFOSa;
    }
    
    *pu4Bytes = u4ByteAddr;
    //return (u4ByteAddr);
    return i4Ret;
}
#if (CONFIG_CHIP_VER_CURR > CONFIG_CHIP_VER_MT8530)

void vJDECErrorConceal(UINT32 u4HwId,BOOL fgGT64En, BOOL fgRstMakEn, BOOL fgErrCodeLenEn)
{
    UINT32 u4Value = 0;
    JDEC_VERY_HAL_LOG("Enter %s, fgGT64En=0x%x,fgRstMakEn=0x%x,fgErrCodeLenEn=0x%x, !\n", __FUNCTION__,fgGT64En,fgRstMakEn,fgErrCodeLenEn);

    u4Value = u4HwJdecRead32(u4HwId, RW_VLD_JPG_ERROR_CONCEAL);
    if (fgGT64En)
    {
        u4Value = (u4Value & (~EC_GT64_EN)) | EC_GT64_EN;
    }
    else
    {
        u4Value = u4Value & (~EC_GT64_EN);
    }
    if (fgRstMakEn)
    {
        u4Value = (u4Value & (~EC_RSTMAK_EN)) | EC_RSTMAK_EN;
    }
    else
    {
        u4Value = u4Value & (~EC_RSTMAK_EN);
    }

    if (fgErrCodeLenEn)    
    {        
        u4Value = (u4Value & (~EC_BEC_LEN_EN) )| EC_BEC_LEN_EN;    
     }    
    else    
    {        
        u4Value = u4Value & (~EC_BEC_LEN_EN);
    }

    vHwJdecWrite32(u4HwId, RW_VLD_JPG_ERROR_CONCEAL, u4Value);
    
}
#else
#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8530)
void vJDECErrorConceal(UINT32 u4HwId,BOOL fgGT64En, BOOL fgRstMakEn)
{
    UINT32 u4Value = 0;

    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Value = u4HwJdecRead32(u4HwId, RW_VLD_JPG_ERROR_CONCEAL);
    if (fgGT64En)
    {
        u4Value = (u4Value & (~EC_GT64_EN)) | EC_GT64_EN;
    }
    else
    {
        u4Value = u4Value & (~EC_GT64_EN);
    }
    if (fgRstMakEn)
    {
        u4Value = (u4Value & (~EC_RSTMAK_EN)) | EC_RSTMAK_EN;
    }
    else
    {
        u4Value = u4Value & (~EC_RSTMAK_EN);
    }

    vHwJdecWrite32(u4HwId, RW_VLD_JPG_ERROR_CONCEAL, u4Value);
    
}

#endif
#endif


void vJDECSTOPPicMode(UINT32 u4HwId)
{
    UINT32 u4Value = 0;
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
#if 0	
    u4Value = u4HwJdecRead32(u4HwId, RW_VLD_JPG_RESTART);
    u4Value = (u4Value & (~VLD_JPG_STOP_EN))| VLD_JPG_STOP_EN;
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_RESTART,u4Value);
#else
     //need confirm with DE
    u4Value = u4HwJdecRead32(u4HwId, RW_VLD_JPG_DEC_BREAK);
    u4Value = u4Value |0x11; 
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_DEC_BREAK,u4Value);

#endif
}

BOOL fgJDECGetStopReady(UINT32 u4HwId)
{
    UINT32 u4Value = 0;
	JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
#if 0		
    u4Value = u4HwJdecRead32(u4HwId, RW_VLD_JPG_RESTART);
    if ((u4Value & VLD_JPG_STOP_RDY) > 0)
        return TRUE;
    else
        return FALSE;
#else
    u4Value = u4HwJdecRead32(u4HwId, RW_VLD_JPG_DEC_STOP);
    u4Value = (u4Value >>28) & 0x3;
    if (u4Value ==0x3)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#endif

}
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
void vHwJdecSetBurstMode(UINT32 u4HwId, BOOL fgBrstEnable, BOOL fg422Enable)
{
    UINT32 u4Value = 0;
	    JDEC_VERY_HAL_LOG("Enter %s ,fgBrstEnable[%d],fg422Enable[%d] !\n", __FUNCTION__);
    u4Value = u4HwJdecRead32(u4HwId, RW_VLD_JPG_BST_OUT_MODE);
    if (fgBrstEnable)
    {
        u4Value |= JPG_ROW_BST_OUT_ON;
	if(fg422Enable)
	    u4Value |= JPG_BST_FORMAT_SEL_422;
        
    }
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_BST_OUT_MODE, u4Value);
}
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
INT32 i4HwJdecSetVideoOutMode(UINT32 u4HwId, BOOL fgPadding16Byte, BOOL fgVideoOutput,JDEC_JFIF_SOF_T *prSofInfo)
{
    UINT32 u4Reg;
	    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Reg = u4HwJdecRead32(u4HwId, RW_VLD_JPG_BST_OUT_MODE);
    u4Reg |= JPG_ROW_BST_OUT_ON;
    if(fgPadding16Byte)
        u4Reg |= JPG_VIDEO_FORMAT_PADDING;
    if(fgVideoOutput)
        u4Reg |= JPG_VIDEO_FORMAT_OUTPUT;
    
    if ((prSofInfo->arSofComp[0].u1HSampFactor == 2)&&(prSofInfo->arSofComp[1].u1HSampFactor == 1))
      {
        if((prSofInfo->arSofComp[0].u1VSampFactor == 2)&&(prSofInfo->arSofComp[1].u1VSampFactor == 1)) //420 format
        {
        	JDEC_VERY_HAL_LOG("Enter %s 420!\n", __FUNCTION__);
            u4Reg &= ~JPG_BST_FORMAT_SEL_422;
        }
        else if ((prSofInfo->arSofComp[0].u1VSampFactor == 1)&&(prSofInfo->arSofComp[1].u1VSampFactor == 1)) //2*1 422 format
        {
            u4Reg |= JPG_BST_FORMAT_SEL_422;
            JDEC_VERY_HAL_LOG("Enter %s 422!\n", __FUNCTION__);
            
        }
        else
            return E_JDEC_HAL_FAIL; 
      }
    else
      {
        return E_JDEC_HAL_FAIL; 
      }
     vHwJdecWrite32(u4HwId, RW_VLD_JPG_BST_OUT_MODE, u4Reg);

  return S_JDEC_HAL_OK;
}

void vHwJdecSetPicturePixelSize (UINT32 u4HwId, UINT32 u4PicPixelSize)
{    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    vHwJdecWrite32(u4HwId, RW_VLD_JPG_PIC_PIX_WIDTH, u4PicPixelSize);
}
#endif

void vJDECPollingFsh(UINT32 u4HwId)
{
#if 0    
    UINT32 u4Value = 0;


    //x_thread_delay(100);
    while(1)
    {
        u4Value = u4HwJdecRead32(u4HwId,RO_VIDEO_BUFFER_CTRL);
        if (u4Value & 0x1)
            break;
        x_thread_delay(5);
    }
#endif    
}

void vJDECReleaseCPU(UINT32 u4HwId)
{
    UINT32 u4Value = u4HwJdecRead32(u4HwId, 0x84);
    JDEC_VERY_HAL_LOG("Enter %s!\n", __FUNCTION__);
    u4Value |= 0x800;

    vHwJdecWrite32(u4HwId, 0x84, u4Value);
    
}
