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

#ifndef __DRV_GFX3D_VFY_H
#define __DRV_GFX3D_VFY_H

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
//#include "x_graphics3d.h"

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

//the below two macros must be oppisite
//either load data via ICE or load date via USB Host
#define T32_HOST     0
#define USB_G3D_VFY  1

/* some pattern such as:
F000_read_writer_reg 
F000-MMU-APB 
F013-MMU_GP_IRQ
need disable to pass */
#define TEST_G3D_INTERRUPT (1)

#define NON_CACHEABLE_DRAM_OFFSET (0x0)
#define TEST_DRAM_CHB (0)   

#if 0
#if TEST_DRAM_CHB    
CHAR *pGPPhyMemBuff = (CHAR*)0x47a00000; // 15M;  NULL;
#else
CHAR *pGPPhyMemBuff = (CHAR*)0x07a00000; // 15M;  NULL;
#endif

#if TEST_DRAM_CHB  
CHAR *pPPPhyMemBuff = (CHAR*)0x47a00000;
#else
CHAR *pPPPhyMemBuff = (CHAR*)0x07a00000;
#endif
#endif

#if T32_HOST
enum
{
	E_T32_NOP,
	E_T32_EXEC,
	E_T32_RESTART,
	E_T32_EXIT,
	E_T32_EXEC_RELOAD_SYM,
	E_T32_DETECT_HOST,
	E_T32_END
};
extern INT32 T32_HostExec(INT32 i4TargetFlag, const CHAR *szFormat, ...);
#endif



typedef struct
{
  INT32 handle;
  INT32 offset;
  UINT32 filesize;
}
FILE;

typedef enum 
{
    G3D_CMP_STATUS_UNKNOW = 0,
    G3D_CMP_STATUS_OPEN_FAIL =1,                        
    G3D_CMP_STATUS_SUCCESS = 2,                        
    G3D_CMP_STATUS_FAIL = 4,                       
    G3D_CMP_STATUS_MAX = 32,
} G3D_CMP_STATUS;

typedef struct
{
    BOOL  fgIsMaliBaseTest;
    BOOL  fgIsAxiBrdgWROutStand;
    BOOL  fgIsGPWatchDogDisable;
    BOOL  fgIsPPWatchDogDisable;
    BOOL  fgIsCompareAllMem;
    BOOL  fgIsCompareGPStatus;
    BOOL  fgIsComparePPStatus;
    BOOL  fgIsAxiStopTest;
    UINT32  u4ReadStand;
    UINT32  u4WriteStand;
} G3D_EMU_CONFIG_T;

typedef struct
{    
    G3D_CMP_STATUS  eCmpMemResult;
    G3D_CMP_STATUS  eCmpPpMemResult;
    G3D_CMP_STATUS  eCmpAllMemResult;    
    G3D_CMP_STATUS  eCmpRegResult;
    G3D_CMP_STATUS  eCmpGpRegResult;
    G3D_CMP_STATUS  eCmpPpRegResult;
    
    G3D_CMP_STATUS  eCmpMemPass;
    G3D_CMP_STATUS  eCmpRegPass;
    
    BOOL                      fgIsGpIrqDone;
    BOOL                      fgIsPpIrqDone;
} G3D_EMU_RESULT_T;


/// Init Graphics3D driver.
/// Call this function by system initialization function when system boot up.
/// \return If return value < 0, it's failed.
extern INT32 i4Graphics3D_Drv_Init(void);


/// Uninit Graphics3D driver.
/// Call this function by system uninitialization function when system shut down.
/// \return If return value < 0, it's failed.
extern INT32 i4Graphics3D_Drv_Uninit(void);

/// Send Stop Command to Cmd Buffer.
/// It's FF Command, use to Stop current command and clean the cmd buffer.
/// \return If return value < 0, it's failed.
extern INT32 i4Graphics3D_Drv_Stop(
    UINT32              u4CmdBufHandle);

extern INT32 i4Graphics3D_Drv_Emu(void);

extern INT32 i4Graphics3D_Drv_RegRW(void);

#endif // __DRV_GFX3D_VFY_H
