/*----------------------------------------------------------------------------*
 * Copyright Statement:                                                       *
 *                                                                            *
 *   This software/firmware and related documentation ("MediaTek Software")   *
 * are protected under international and related jurisdictions'copyright laws *
 * as unpublished works. The information contained herein is confidential and *
 * proprietary to MediaTek Inc. Without the prior written permission of       *
 * MediaTek Inc., any reproduction, modification, use or disclosure of        *
 * MediaTek Software, and information contained herein, in whole or in part,  *
 * shall be strictly prohibited.                                              *
 * MediaTek Inc. Copyright (C) 2010. All rights reserved.                     *
 *                                                                            *
 *   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND     *
 * AGREES TO THE FOLLOWING:                                                   *
 *                                                                            *
 *   1)Any and all intellectual property rights (including without            *
 * limitation, patent, copyright, and trade secrets) in and to this           *
 * Software/firmware and related documentation ("MediaTek Software") shall    *
 * remain the exclusive property of MediaTek Inc. Any and all intellectual    *
 * property rights (including without limitation, patent, copyright, and      *
 * trade secrets) in and to any modifications and derivatives to MediaTek     *
 * Software, whoever made, shall also remain the exclusive property of        *
 * MediaTek Inc.  Nothing herein shall be construed as any transfer of any    *
 * title to any intellectual property right in MediaTek Software to Receiver. *
 *                                                                            *
 *   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its *
 * representatives is provided to Receiver on an "AS IS" basis only.          *
 * MediaTek Inc. expressly disclaims all warranties, expressed or implied,    *
 * including but not limited to any implied warranties of merchantability,    *
 * non-infringement and fitness for a particular purpose and any warranties   *
 * arising out of course of performance, course of dealing or usage of trade. *
 * MediaTek Inc. does not provide any warranty whatsoever with respect to the *
 * software of any third party which may be used by, incorporated in, or      *
 * supplied with the MediaTek Software, and Receiver agrees to look only to   *
 * such third parties for any warranty claim relating thereto.  Receiver      *
 * expressly acknowledges that it is Receiver's sole responsibility to obtain *
 * from any third party all proper licenses contained in or delivered with    *
 * MediaTek Software.  MediaTek is not responsible for any MediaTek Software  *
 * releases made to Receiver's specifications or to conform to a particular   *
 * standard or open forum.                                                    *
 *                                                                            *
 *   3)Receiver further acknowledge that Receiver may, either presently       *
 * and/or in the future, instruct MediaTek Inc. to assist it in the           *
 * development and the implementation, in accordance with Receiver's designs, *
 * of certain softwares relating to Receiver's product(s) (the "Services").   *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MediaTek Inc. with respect  *
 * to the Services provided, and the Services are provided on an "AS IS"      *
 * basis. Receiver further acknowledges that the Services may contain errors  *
 * that testing is important and it is solely responsible for fully testing   *
 * the Services and/or derivatives thereof before they are used, sublicensed  *
 * or distributed. Should there be any third party action brought against     *
 * MediaTek Inc. arising out of or relating to the Services, Receiver agree   *
 * to fully indemnify and hold MediaTek Inc. harmless.  If the parties        *
 * mutually agree to enter into or continue a business relationship or other  *
 * arrangement, the terms and conditions set forth herein shall remain        *
 * effective and, unless explicitly stated otherwise, shall prevail in the    *
 * event of a conflict in the terms in any agreements entered into between    *
 * the parties.                                                               *
 *                                                                            *
 *   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and    *
 * cumulative liability with respect to MediaTek Software released hereunder  *
 * will be, at MediaTek Inc.'s sole discretion, to replace or revise the      *
 * MediaTek Software at issue.                                                *
 *                                                                            *
 *   5)The transaction contemplated hereunder shall be construed in           *
 * accordance with the laws of Singapore, excluding its conflict of laws      *
 * principles.  Any disputes, controversies or claims arising thereof and     *
 * related thereto shall be settled via arbitration in Singapore, under the   *
 * then current rules of the International Chamber of Commerce (ICC).  The    *
 * arbitration shall be conducted in English. The awards of the arbitration   *
 * shall be final and binding upon both parties and shall be entered and      *
 * enforceable in any court of competent jurisdiction.                        *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *
 * Description:
 *         The main routine of boot loader
 *
 *---------------------------------------------------------------------------*/

//---------------------------------------------------------------------
// include files
//---------------------------------------------------------------------
#include "loader_if.h"
#include "x_serial.h"
#include "x_drv_cli.h"

//---------------------------------------------------------------------
// Config
//---------------------------------------------------------------------
#ifndef LOADER_VERSION_MAJOR
#define LOADER_VERSION_MAJOR                0
#endif /* LOADER_VERSION_MAJOR */

#ifndef LOADER_VERSION_MINOR
#define LOADER_VERSION_MINOR                1
#endif /* LOADER_VERSION_MINOR */

#ifdef NDEBUG   // release version
#define BOOT_WAIT_TIME                  100         // 0.1 second
#else           // debug version
#define BOOT_WAIT_TIME                  2000        // 3 second
#endif


//---------------------------------------------------------------------
// Constant definitions
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// External declarations
//---------------------------------------------------------------------
extern void TEST_Function(UINT32 fgBool);

//---------------------------------------------------------------------
// Private functions
//---------------------------------------------------------------------


//---------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------

/*----------------------------------------------------------------------------
 * Function: CMain()
 *
 * Description:
 *      The entry point of single thread environment.
 *
 * Inputs:
 *      pv_mem_addr: The start address of available memory
 *      z_mem_size: Size in byte of available memory
 *      pv_arg: Optional argument, not used
 *
 * Outputs: -
 *
 * Returns: -
 *---------------------------------------------------------------------------*/
VOID CMain()
{
    LDR_DATA_T   *prLdrData;
#ifdef CC_FPGA
    PFN_JumpAddr pfnJump;
#endif
    Printf("\nMT5387 Boot Loader v%u.%u\n", LOADER_VERSION_MAJOR, LOADER_VERSION_MINOR);
#ifndef CC_FPGA
    // Initialize some setting at the beginning.
    LDR_PreInit();

    // Detect if user wants to enter CLI mode
    if (LDR_IsEnterCli(BOOT_WAIT_TIME))
    {
        CLI_Init();
        CLI_Input(NULL);
    }

    // Get prLdrData
    prLdrData = LDR_GetLdrData();

    // Initialize basic loader environment. (eeprom, 8032, ...etc.)
    LDR_EnvInit(prLdrData);

    // Check USB upgrade flag and do USB upgrade if positive
    LDR_CheckUpgrade(prLdrData);

    // Query boot/standby decision by eeprom and wakeup reason.
    if (LDR_IsGoBootup(prLdrData) == 0)
    {
        // Enter standby if the decision is standby from above data structure.
        LDR_EnterStandby(prLdrData);
    }

    // Initialize boot up settings (GPIO, LED, ...etc.)
    LDR_UpSetup(prLdrData);

    // Logo display on panel
    LDR_PanelLogo(prLdrData);
#else
    Printf("\nLoading TV Image in FPGA environment.\n");
    x_memcpy((void *)0x10000, (void *)0x28000000 + 0x28000, (UINT32)0x500000); //assume loader will be smaller than 128KB
    pfnJump = (PFN_JumpAddr)(0x10000);
    pfnJump();
#endif

    // Try Bootup with loading image to dram.
    if (LDR_ImageDecompress(prLdrData) == 0)
    {
        // jump to work.
        LDR_ImageJump(prLdrData);
        // never return if jump.
    }

    // Check USB upgrade flag and do USB upgrade if positive
    if (LDR_ForceUpgrade(prLdrData) == 0)
    {
        // Try Bootup with loading image to dram.
        if (LDR_ImageDecompress(prLdrData) == 0)
        {
            // jump to work.
            LDR_ImageJump(prLdrData);
            // never return if jump.
        }
    }

    // UART change to transparent mode.
    SerTransparent();

    // if boot failed, enter cli directly.
    CLI_Init();
    CLI_Input(NULL);

    // never return.
    TEST_Function(0);
}

