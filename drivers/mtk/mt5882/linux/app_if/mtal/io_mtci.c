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
 * $RCSfile: io_mtci.c,v $
 * $Revision: #1 $
 * $Date: 2015/02/15 $
 * $Author: p4admin $
 *
 * Description: Peripherals, such as NOR/NAND/SIF/RTC/PWM/GPIO (and others), are centralized in
 *                   this file
 *---------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "mtci.h"
#include <linux/mtal_ioctl.h>
#include "pcmcia_if.h"

#include "cb_low.h"  // For callback function
//for LGE
#define LGE_CI
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error1.1\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error1.2\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error2.1\n");      \
        return MTR_ERR_INV;                                          \
    }     

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ntsc argument error2.2\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error3\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error4.1\n");      \
        return MTR_ERR_INV;                                          \
    }       

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error4.2\n");      \
        return MTR_ERR_INV;                                          \
    } 
    
#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error6\n");     \
        return MTR_ERR_INV;                                          \
    }                                                       \

#define COPY_TO_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       size))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error7\n");      \
        return MTR_ERR_INV;                                          \
    }     
#define COPY_FROM_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,    \
                       size))          \
    {                                                       \
        MTCI_PRINT("mtal_ioctl_ci argument error8\n");      \
        return MTR_ERR_INV;                                          \
    } 
//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
#define MAX_CIS_MAP_SIZE                256
#define TIMEOUT                         2000
#ifndef DATA_CH_BUF_SIZE
#define DATA_CH_BUF_SIZE 65536
#endif
//-----------------------------------------------------------------------------
// Static Variables
//-----------------------------------------------------------------------------
static UINT8 * pu1CurCisMap = NULL;
static UINT16 u2addrcor;
static UINT8 u1cor;
static HANDLE_T hMtciSemaHandle = (HANDLE_T)NULL;
static BOOL bDriverInited = FALSE;
static UINT8* pu8GlobalBuffer = NULL;

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

/** MTCI_Init()  Common Interface Initialization Fucntion
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_Init (VOID) 
{
    MTCI_PRINT("_MTCI_Init\n");
	
    PCMCIA_Init();
    
    pu1CurCisMap=kmalloc(MAX_CIS_MAP_SIZE,GFP_KERNEL);
    if(!pu1CurCisMap)
    {
        MTCI_PRINT("pu1CurCisMap alloc memory fail\n");
        return MTR_NOT_OK;
    }

    pu8GlobalBuffer = kmalloc(DATA_CH_BUF_SIZE*sizeof(UINT8),GFP_KERNEL);
    if ( pu8GlobalBuffer == NULL )
    {
        MTCI_PRINT("pu1CurCisMap alloc memory fail\n");
        return MTR_NOT_OK;
    }

    PCMCIA_EnableIsr();
    bDriverInited = true;
    return MTR_OK;
}

/** MTCI_Close()  Common Interface Close Fucntion
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_Close (VOID) 
{
    MTCI_PRINT("_MTCI_Close\n");

    /*
     * Not Implement here, no need
    */
    
    return MTR_OK;
}

/** MTCI_Restart()  Common Interface Restart CAM Fucntion
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_Restart (VOID) 
{
    MTCI_PRINT("_MTCI_Restart\n");

    /*
     * Implement here
    */

    PCMCIA_Restart();
    
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTCI_DectectCard()  Common Interface Card Detection Function
 *  @retval TRUE card inserted ,FALSE No Card 
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_DectectCard (VOID) 
{
    static BOOL bCamDetectLogPrinted = FALSE;
    if ( bDriverInited == FALSE )
    {
        return MTR_OK;
    }
    
    if (PCMCIA_DetectCard())
    {
        if ( bCamDetectLogPrinted == FALSE )
        {
            MTCI_PRINT("!!!!!!!!!!!CAM Been Connected!!!!!!!!!\n");
        }
        bCamDetectLogPrinted = TRUE;
        return MTR_CARD_DETECTED;
    }
    else
    {
        if ( bCamDetectLogPrinted == TRUE )
        {
            MTCI_PRINT("!!!!!!!!!!!CAM Been Removed!!!!!!!!!\n");
        }
        bCamDetectLogPrinted = FALSE;
        return MTR_OK;
    }
}

//-----------------------------------------------------------------------------
/** MTCI_Reset()  Resets CI Device (PCMCIA reset), also called card/CAM reset
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_Reset (unsigned long arg)
{
    UINT8 fgReset;

    fgReset = (UINT8)arg;
    
    MTCI_PRINT("_MTCI_Reset\n");
    
    PCMCIA_Reset(fgReset);
    
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTCI_CheckCIS()  Check CIS(Card Information Structure)
 * read CIS and check if it is compatible or not 
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_CheckCIS (VOID)
{
    UINT32 i;
    UINT32 u4TimeCount= 0 ;
    UINT16 addrcor;
    UINT8 cor;
    MTCI_PRINT("_MTCI_CheckCIS\n");

    if ( pu1CurCisMap == NULL )
    {
        return MTR_NOT_OK;
    }

    // Reset CAM
    x_thread_delay(200);
    PCMCIA_Reset(1);
    x_thread_delay(10);
    PCMCIA_Reset(0);
    
    while( (PCMCIA_TestCardReady()!=1) )
    {
        if ( ++u4TimeCount > TIMEOUT)
        {
            MTCI_PRINT("Ready pin is low... (card not reset?) \n");
			//for one DIBOLO CAM(from LG) issue
			x_thread_delay(200);
            PCMCIA_Reset(1);
            x_thread_delay(10);
            PCMCIA_Reset(0);
            break;
        }
        x_thread_delay(5);
    }

    PCMCIA_PcmciaToPod( FALSE );
    x_memset((void*)pu1CurCisMap, 0, MAX_CIS_MAP_SIZE);
    for( i = 0 ; i < MAX_CIS_MAP_SIZE ; i ++)
    {
        pu1CurCisMap[i] = PCMCIA_ReadCor( i *2 );
    }
    
    if (PCMCIA_CheckCIS(pu1CurCisMap, &addrcor, &cor) != 1)
    {
        MTCI_PRINT("_MTCI_CheckCIS Failed!\n");
        return MTR_NOT_OK;
    }
    u2addrcor = addrcor;
    u1cor = cor;
#if 0
    printf("CIS ======================================:\n");
    {
        int i = 0 ; 
        for ( i = 0 ; i < 255  ; i++ )
        {
            printf("0x%x\n", pu1CurCisMap[i]);
            if ( i %16 ==0 )
            {
                printf("\n");
            }
        }
        printf("\n");
    }
#endif

    PCMCIA_PcmciaEnableTS(FALSE);

    MTCI_PRINT("_MTCI_CheckCIS OK!\n");
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTCI_CheckCIPLUSCapability()  Check CI+ capability
 * read CI+ capability from CIS 
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_CheckCIPLUSCapability (VOID)
{
    UINT8 ui1CIPlusCap = 0;

    MTCI_PRINT("_MTCI_CheckCIPLUSCapability\n");
     
    if (PCMCIA_CheckCIPLUSCapability(&ui1CIPlusCap))
    {
        MTCI_PRINT("_MTCI_CheckCIPLUSCapability Fail\n");
        return 0;
    }

    MTCI_PRINT("_MTCI_CheckCIPLUSCapability %d\n", ui1CIPlusCap);
    
    return ui1CIPlusCap;
}
static MT_RESULT_T  _MTCI_ReadCIPLUSVersion (unsigned long arg)
{
    UINT32 ui4CIPlusVersion = 0;
	UINT32* pu4Version;
    pu4Version = (UINT32*)arg;
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    MTCI_PRINT("_MTCI_ReadCIPLUSVersion\n");
	#ifdef LGE_CI
    if (PCMCIA_ReadCIPLUSVersion(&ui4CIPlusVersion))
    {
        MTCI_PRINT("_MTCI_ReadCIPLUSVersion Fail\n");
        return 0;
    }
	#endif
	USR_SPACE_ACCESS_VALIDATE_ARG(pu4Version, (sizeof(UINT32)));
	COPY_TO_USER_ARG(pu4Version, ui4CIPlusVersion,UINT32);
    MTCI_PRINT("2-2_MTCI_ReadCIPLUSVersion %d\n", *pu4Version);
    return MTR_OK;
}
static MT_RESULT_T  _MTCI_ReadCIPLUSCiprof(unsigned long arg)
{
    UINT32 ui4CIPlusCiprof = 0;
	UINT32* pu4Profile;
    MTCI_PRINT("_MTCI_ReadCIPLUSCiprof\n");
     pu4Profile = (UINT32*)arg;
	USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
	#ifdef LGE_CI
    if (PCMCIA_ReadCIPLUSCiprof(&ui4CIPlusCiprof))
    {
        MTCI_PRINT("PCMCIA_ReadCIPLUSCiprof Fail\n");
        return 0;
    }
	#endif
	USR_SPACE_ACCESS_VALIDATE_ARG(pu4Profile, (sizeof(UINT32)));
	COPY_TO_USER_ARG(pu4Profile, ui4CIPlusCiprof,UINT32);
    MTCI_PRINT("2-2_MTCI_ReadCIPLUSCiprof %d\n", *pu4Profile);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTCI_WriteCor()  Write COR(Configuration Option Register) data into COR
 *  address
 *  @param u2CorAddress, Size beginning from u4Addr
 *  @param u1CorValue, Memory buffer storing check sum value
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_WriteCor (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
	
    UINT16 u2CorAddress;
    UINT8 u1CorValue;
	
    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg,rArg);

    u2CorAddress = (UINT16)rArg.ai4Arg[0];
    u1CorValue = (UINT8)rArg.ai4Arg[1];

    PCMCIA_PcmciaToPod( FALSE );

    if ( (u2CorAddress == 0xFFFF) && (u1CorValue == 0xFF))
    {
        u2CorAddress = u2addrcor;
        u1CorValue = u1cor;
    }
    MTCI_PRINT("PCMCIA_WriteCor : %d %d\n", u2CorAddress, u1CorValue);
    if (PCMCIA_WriteCor(u2CorAddress, u1CorValue))
    {
        MTCI_PRINT(" _MTCI_WriteCor Failed! u2CorAddress = %d, u1CorValue = %d\n",u2CorAddress,u1CorValue);
        return MTR_NOT_OK;
    }

    MTCI_PRINT(" _MTCI_WriteCor OK ! u2CorAddress = %d, u1CorValue = %d\n",u2CorAddress,u1CorValue);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTCI_Read()  Read data from CAM
 *  @param pu1Data, data 
 *  @param pu2DataLen, data length
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_Read ( unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;	
    UINT8* pu1Data;
    UINT16* pu2DataLen;
    UINT16 pku2Datalen = 65535;// nego buffsize need up to 65535  ,for some CI+ issue//DATA_CH_BUF_SIZE;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg, rArg);

    pu1Data =(UINT8* )rArg.ai4Arg[0];
    pu2DataLen =(UINT16*)rArg.ai4Arg[1];

    PCMCIA_SetPodCiAccessMode(POD_DATA_CHANNEL);
    PCMCIA_PcmciaToPod(TRUE);
    x_memset(pu8GlobalBuffer, 0, DATA_CH_BUF_SIZE*sizeof(UINT8));

    if (PCMCIA_Read(POD_SDA, pu8GlobalBuffer, &pku2Datalen) )
    {
        MTCI_PRINT("_MTCI_Read Failed2!\n");
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu2DataLen, UINT16);
    COPY_TO_USER_ARG(pu2DataLen, pku2Datalen, UINT16);

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Data,UINT8);
    COPY_TO_USER_ARG_K(pu1Data,pu8GlobalBuffer[0],(pku2Datalen*sizeof(UINT8)));

    MTCI_PRINT("_MTCI_Read OK!\n");
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** MTCI_Write()  Write data into CAM
 *  @param pu1Data, data 
 *  @param pu2DataLen, data length
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_Write (unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
	
    UINT8* pu1Data;
    UINT16* pu2DataLen;
    UINT16   u2Datalen = 0;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg,rArg);

    pu1Data = (UINT8*)rArg.ai4Arg[0];
    pu2DataLen = (UINT16*)rArg.ai4Arg[1];

    USR_SPACE_ACCESS_VALIDATE_ARG(pu2DataLen, UINT16);
    COPY_FROM_USER_ARG(pu2DataLen, u2Datalen,UINT16);
	 
    PCMCIA_SetPodCiAccessMode(POD_DATA_CHANNEL);
    PCMCIA_PcmciaToPod(TRUE);
    x_memset(pu8GlobalBuffer, 0, DATA_CH_BUF_SIZE*sizeof(UINT8));

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Data, UINT8);

    COPY_FROM_USER_ARG_K(pu1Data, pu8GlobalBuffer[0], (u2Datalen*sizeof(UINT8)));

    if (PCMCIA_Write(POD_SDA, pu8GlobalBuffer, u2Datalen ))
    {
        MTCI_PRINT("_MTCI_Write Failed2 ! u2Datalen = %d\n", u2Datalen);
        return MTR_NOT_OK;
    }

    MTCI_PRINT("_MTCI_Write OK ! u2Datalen = %d\n", u2Datalen);    
    return MTR_OK;
} 

//-----------------------------------------------------------------------------
/** _MTCI_NegoBuf()  negotiation buffer size
 *  @param pu2BufSize, negotiation Buffer Size
 *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_NegoBuf (unsigned long arg)
{
    UINT16 u2BufSize;
    UINT16* pu2BufSize;

    pu2BufSize = (UINT16*)arg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    if (PCMCIA_NegotiatedBufSize(&u2BufSize))
    {
        MTCI_PRINT("_MTCI_NegoBuf Failed ! pu2BufSize = %d\n", (int)*pu2BufSize);
        return MTR_NOT_OK;
    }
    
    USR_SPACE_ACCESS_VALIDATE_ARG(pu2BufSize, (sizeof(UINT16)));
    COPY_TO_USER_ARG(pu2BufSize, u2BufSize,UINT16);
    PCMCIA_PcmciaToPod( TRUE );
    PCMCIA_PcmciaEnableTS(TRUE);
    
    MTCI_PRINT("_MTCI_NegoBuf OK ! pu2BufSize = %d\n", (int)*pu2BufSize);
    return MTR_OK;
}

//-----------------------------------------------------------------------------
/** _MTCI_ReadDAStatus()  Read DA status
 *  @param pu2BufSize, Buffer size 
 *  @retval TRUE DA=1 (data avaliable), FALSE DA=0 (Not avaliable)
 */
//-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_ReadDAStatus (VOID)
{
    POD_IO_STATE_T eIoState;
    UINT8 u1DaFrSts = 0;

    eIoState = POD_SDA;
    PCMCIA_SetPodCiAccessMode(POD_DATA_CHANNEL);
    PCMCIA_PcmciaToPod(TRUE);

    PCMCIA_GetDaFrIntStatus(POD_DATA_CHANNEL, &u1DaFrSts);

    if ( u1DaFrSts & (1<<7) )
    {
        return MTR_DA_HIGH;
    }
    else
    {
        return MTR_OK;
    }
}



static MT_RESULT_T _MTCI_ReadReg(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg = {{0}};
    UINT8* pu1Data;
    UINT8* pku1Data =NULL;
    UINT16* pu2DataLen;
    UINT16   u2Datalen = 0;
    UINT32   u4RegAddr;

    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg, rArg);

    u4RegAddr =(UINT32)rArg.ai4Arg[0];
    pu1Data =(UINT8* )rArg.ai4Arg[1];
    pu2DataLen =(UINT16*)rArg.ai4Arg[2];
    USR_SPACE_ACCESS_VALIDATE_ARG(pu2DataLen, UINT16);
    COPY_FROM_USER_ARG(pu2DataLen, u2Datalen,UINT16);

    PCMCIA_SetPodCiAccessMode(POD_DATA_CHANNEL);
    PCMCIA_PcmciaToPod(TRUE);
    pku1Data = kmalloc((u2Datalen)*sizeof(UINT8),GFP_KERNEL);
    if (!pku1Data)
    {
        MTCI_PRINT("_MTCI_ReadReg Failed 1!\n");
        return MTR_PARAMETER_ERROR;
    }
    if ( PCMCIA_ReadReg(POD_SDA, u4RegAddr, pku1Data, u2Datalen) )
    {
        MTCI_PRINT("_MTCI_ReadReg Failed 2!\n");
        kfree(pku1Data);
        return MTR_NOT_OK;
    }

    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Data,UINT8);
    COPY_TO_USER_ARG_K(pu1Data,pku1Data[0],((u2Datalen)*sizeof(UINT8)));

    kfree(pku1Data);

    return MTR_OK;
}
static MT_RESULT_T _MTCI_WriteReg(unsigned long arg)
{
    MTAL_IOCTL_3ARG_T rArg = {{0}};
    
    UINT8*   pu1Data;
    //UINT16* pu2DataLen;
    UINT16   u2Datalen;    
    UINT8*   pku1Data = NULL;     
    UINT32   u4RegAddr;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(arg);
    COPY_FROM_USER_3ARG(arg,rArg);

    u4RegAddr = (UINT32)rArg.ai4Arg[0];
    pu1Data = (UINT8*)rArg.ai4Arg[1];
    u2Datalen = (UINT16)rArg.ai4Arg[2];
     
    PCMCIA_SetPodCiAccessMode(POD_DATA_CHANNEL);
    PCMCIA_PcmciaToPod(TRUE);
    pku1Data = kmalloc(u2Datalen*sizeof(UINT8),GFP_KERNEL);

    if (!pku1Data)
    {
        MTCI_PRINT("_MTCI_WriteReg Failed 1!\n");
        return MTR_PARAMETER_ERROR;
    }
    USR_SPACE_ACCESS_VALIDATE_ARG(pu1Data, UINT8);
     
    COPY_FROM_USER_ARG_K(pu1Data, pku1Data[0], (u2Datalen*sizeof(UINT8)));
    
    if ( PCMCIA_WriteReg(POD_SDA, u4RegAddr, pku1Data, u2Datalen ) )
    {    
        MTCI_PRINT("_MTCI_WriteReg Failed 2!\n");
        kfree(pku1Data);
        return MTR_NOT_OK;
    }


    MTCI_PRINT("_MTCI_WriteReg OK!\n");
    kfree(pku1Data);
    
    return MTR_OK;
}
static MT_RESULT_T _MTCI_ReadCor(unsigned long arg)
{
    UINT16 u2CorAddress;

    u2CorAddress = arg;

//    MTCI_PRINT("_MTCI_ReadCor!\n");

    return PCMCIA_ReadCor(u2CorAddress);
}
static MT_RESULT_T _MTCI_TestCardReady(VOID)
{
    if ( PCMCIA_TestCardReady() )
    {
        return MTR_CARD_READY;
    }
    else
    {
        return MTR_OK;
    }
}
static MT_RESULT_T _MTCI_EnableSlot(unsigned long arg)
{
    UINT8 u1Enable;

    u1Enable = (UINT8)arg;

    MTCI_PRINT("_MTCI_EnableSlot : %d!\n", u1Enable);

    PCMCIA_PcmciaToPod(u1Enable);
    
    return MTR_OK;
}

static MT_RESULT_T _MTCI_DetectPodActive(VOID)
{
    if ( PCMCIA_DetectPodActive() )
    {
        return MTR_POD_CI_ACTIVE;
    }
    else
    {
        return MTR_OK;
    }
}

static MT_RESULT_T _MTCI_PcmciaEnableTs(unsigned long arg)
{
    MTAL_IOCTL_2ARG_T rArg;
    UINT8 u1Enable;
    UINT8 u1FromCA;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg,rArg);

    u1Enable = (UINT8)rArg.ai4Arg[0];
    u1FromCA = (UINT8)rArg.ai4Arg[1];

    if ( u1FromCA )
    {
        if ( !PCMCIA_DetectCard() )
        {
            MTCI_PRINT("_MTCI_PcmciaEnableTs : Card no exist !\n");
            return 0;
        }
    }

    PCMCIA_PcmciaEnableTS(u1Enable);

    MTCI_PRINT("_MTCI_PcmciaEnableTs OK! %d %d\n", u1Enable, u1FromCA);
    return MTR_OK;
}
static MT_RESULT_T _MTCI_ResetCam(VOID)
{
    MTCI_PRINT("_MTCI_ResetCam\n");
    #ifdef LGE_CI
    PCMCIA_SetPodCiAccessMode(POD_DATA_CHANNEL);
    PCMCIA_ResetChannel();
    #else
    PCMCIA_ResetCam();
    #endif
    return MTR_OK;
}
static MT_RESULT_T _MTCI_SetCardDetectOnOff(unsigned long arg)
{
    // Not Implemented....
    return MTR_OK;
}

static MT_RESULT_T _MTCK_SetBypass(unsigned long arg)
{
    UINT8 u1Bypass;

    u1Bypass = (UINT8)arg;

    PCMCIA_SetBypass(u1Bypass);

    MTCI_PRINT("_MTCK_SetBypass : %d\n", u1Bypass);
    return MTR_OK;
}

 //-----------------------------------------------------------------------------
  /** MTCI_BypassOnOff()  Turn On/Off Bypass mode
   * When Bypass Mode On,  TS will run from Demodulator to Demuxer directly 
   *  (ie. bypass CI device)
   * otherwise Bypass Mode Off,  TS will run through CI device (ie. including CAM 
   *  under I/O mode) 
   *  @param fgOnOff, TRUE is on, and FALSE is off
   *  @retval MTR_OK successful,otherwise MTR_NOT_OK.
   */
  //-----------------------------------------------------------------------------
static  MT_RESULT_T  _MTCI_BypassOnOff (unsigned long arg)
{
	BOOL fgOnOff;
  
	fgOnOff = (BOOL)arg;
  
	MTCI_PRINT(" - fgOnOff = %d\n",
		fgOnOff);
  
	//add driver implementation code here
	PCMCIA_PcmciaEnableTS(fgOnOff);
	  
	return MTR_OK;
}
  


 //-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
 /** _MTCI_ExternalDemodOnOff()  Swtich TS to external demod or internal
  *  @param pu2BufSize, Buffer size
  *  @retval MTR_OK DA=1 (data avaliable), MTR_NOT_OK DA=0 (Not avaliable)
  */
 //-----------------------------------------------------------------------------
static MT_RESULT_T  _MTCI_ExternalDemodOnOff (unsigned long arg)
{
    BOOL fgOnOff;
    fgOnOff = (BOOL)arg;
    MTCI_PRINT("_MTCI_ExternalDemodOnOff = %d\n",fgOnOff);

    if (fgOnOff)
    {
        PCMCIA_ExternalDemodOnOff(TRUE);
    }
    else
    {
        PCMCIA_ExternalDemodOnOff(FALSE);
    }
    return MTR_OK;
}

#if 0//for build warning
static MT_RESULT_T  _MTCI_SetTsClkRate(unsigned long arg)
{
    UINT32   u4ClkRate;
    u4ClkRate = (UINT32)arg;

    PCMCIA_ExternalIcSetTsClkRate(u4ClkRate);
    return MTR_OK;
}
#endif

/*********************************************************************/
// MTAL for MTK MW
static MT_RESULT_T _MTCI_SetPodciAccessMode(unsigned long arg)
{
    POD_ACCESS_MODE_T eMode;

    eMode = (POD_ACCESS_MODE_T)arg;

    MTCI_PRINT("_MTCI_SetPodciAccessMode : %d!\n", eMode);

    PCMCIA_SetPodCiAccessMode(eMode);
    
    return MTR_OK;
}

static INT32 _MTCI_PcmciaToPod(unsigned long arg)
{
    BOOL fgPcmciaToPod;

    fgPcmciaToPod = (BOOL)arg;

    MTCI_PRINT("_MTCI_PcmciaToPod : %d!\n", fgPcmciaToPod);

    return PCMCIA_PcmciaToPod(fgPcmciaToPod);
}

static MT_RESULT_T _MTCI_EnableIsr(VOID)
{
    MT_RESULT_T i4Result = MTR_OK;

    i4Result = PCMCIA_EnableIsr();
	MTCI_PRINT("_MTCI_EnableIsr \n");

    return i4Result;
}

static MT_RESULT_T _MTCI_AutoSetTsPath(VOID)
{
    PCMCIA_AutoSetTsRoutingPath();
	MTCI_PRINT("_MTCI_AutoSetTsPath\n");
    return MTR_OK;
}

static MT_RESULT_T _MTCI_CiCamWorkaround(unsigned long arg)
{
	 MT_RESULT_T i4Result = MTR_OK;
   MTAL_IOCTL_2ARG_T rArg; 
   INT32 eCiCamWorkaround;
	 void* vParam;
	 USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
   COPY_FROM_USER_2ARG(arg,rArg);
	 eCiCamWorkaround =(INT32 )rArg.ai4Arg[0];
	 vParam =(void*)rArg.ai4Arg[1];

   PCMCIA_CiCamWorkaround(eCiCamWorkaround,  vParam);

   return i4Result;
 }
static MT_RESULT_T _MTCI0_Init(VOID)
{
    MT_RESULT_T i4Result = MTR_OK;
	  i4Result = _MTCI_Init();
    return i4Result;
}


static MT_RESULT_T _MTCI0_SetBypass(unsigned long arg)
{
	BOOL fgBypass;
	
	fgBypass = (BOOL)arg;
	
	MTCI_PRINT("_MTCI0_SetBypass : %d!\n", fgBypass);
	
	PCMCIA_SetBypass( fgBypass);
		
	return MTR_OK;
}

static void _Ci_RegIsr(UINT16  ui2_ireq_status)
{
	_CB_PutEvent(CB_MTAL_CI_TRIGGER, sizeof(UINT16), &ui2_ireq_status);
}

static MT_RESULT_T _MTCI0_RegIsr(unsigned long arg)
{
	UINT16 u2IntNo;
	u2IntNo = (UINT16)arg;
	PCMCIA_RegIsr(u2IntNo,  _Ci_RegIsr, NULL);
	return MTR_OK;
}

#ifdef CC_SUPPORT_STR
static void _Ci_RegFastBootCB(UINT8 ui1_fb_status)
{
    MTCI_PRINT("_CI_IO: ui1_fb_status = %d\n",ui1_fb_status);
    _CB_PutEvent(CB_MTAL_CI_FAST_BOOT_CB_TRIGGER, sizeof(UINT8), &ui1_fb_status);
}

static MT_RESULT_T _MTCI0_RegFastBootCB(unsigned long arg)
{
    MTCI_PRINT("_CI_IO: _MTCI0_RegFastBootCB\n");
    PCMCIA_RegFastBootCB(_Ci_RegFastBootCB);
    return MTR_OK;
}
#endif

#ifdef CC_DUAL_TUNER_SUPPORT
static MT_RESULT_T _MTCI0_SetDualTsPath(unsigned long arg)
{
    INT32 u4Ret = MTR_OK;
    PCMCIA_DUAL_TS_TYPE eTsType;
    

    eTsType = (PCMCIA_DUAL_TS_TYPE)arg;

    MTCI_PRINT("_MTCI0_SetDualTsPath : %d\n", eTsType);

    u4Ret = PCMCIA_SetDualTsPath(eTsType);
    
    return u4Ret;
}
#endif

#ifdef CC_MT5890
static MT_RESULT_T _MTCI0_SetCamVer(unsigned long arg)
{
    INT32 u4Ret = MTR_OK;
    CAM_VERSION_TYPE_T eCamVer;

    eCamVer = (CAM_VERSION_TYPE_T)arg;

    MTCI_PRINT("_MTCI0_SetCamVer: %d\n", eCamVer);

    u4Ret = PCMCIA_SetCamVer(eCamVer);
    return u4Ret;
}
#endif

static MT_RESULT_T _MTCI0_SetBypassTsRouting(unsigned long arg)
{
	BOOL fgBypass;
		
	fgBypass = (BOOL)arg;
		
	MTCI_PRINT("_MTCI0_SetBypassTsRouting : %d!\n", fgBypass);
		
	PCMCIA_SetBypassTsRouting( fgBypass);
			
	return MTR_OK;
}


static MT_RESULT_T _MTCI_SetTsPath(unsigned long arg)
{
    BOOL fgExternalDemod;
    BOOL fgThroughCard;
    MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(arg);
    COPY_FROM_USER_2ARG(arg,rArg);

    fgExternalDemod = (BOOL)rArg.ai4Arg[0];
    fgThroughCard   = (BOOL)rArg.ai4Arg[1];
    
    PCMCIA_SetTsPath(fgExternalDemod, fgThroughCard);
    
    return MTR_OK;
}

#if 0//for warning to error,'_MTCI_SetTsClkRate' defined but not used
static MT_RESULT_T  _MTCI_SetTsClkRate(unsigned long arg)
{
    UINT32   u4ClkRate;
    u4ClkRate = (UINT32)arg;

    PCMCIA_ExternalIcSetTsClkRate(u4ClkRate);
    return MTR_OK;
}
#endif



int mtal_ioctl_mtci(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg)
{
    int result = MTR_OK;
    
    if ( hMtciSemaHandle == (HANDLE_T)NULL )
    {
        if ( x_sema_create(&hMtciSemaHandle, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) != 0 )
        {
            return MTR_NOT_OK;
        }
    }

    x_sema_lock(hMtciSemaHandle, X_SEMA_OPTION_WAIT);

    switch (cmd) 
    {
        case MTAL_IO_CI_INIT:
            MTCI_PRINT("invoke _MTCI_Init begin\n");
            result = _MTCI_Init();
            break;
        case MTAL_IO_CI_CLOSE:
            MTCI_PRINT("invoke _MTCI_Close begin\n");
            result = _MTCI_Close();
            break;
        case MTAL_IO_CI_RESTART:
            MTCI_PRINT("invoke _MTCI_Restart begin\n");
            result = _MTCI_Restart();
            break;
        case MTAL_IO_CI_DETECTCARD:
            result = _MTCI_DectectCard();
            break;
        case MTAL_IO_CI_RESET:
            MTCI_PRINT("invoke _MTCI_Reset begin\n");
            result = _MTCI_Reset(arg);
            break;
        case MTAL_IO_CI_CHECKCIS:
            MTCI_PRINT("invoke _MTCI_CheckCIS begin\n");
            result = _MTCI_CheckCIS();
            break;
        case MTAL_IO_CI_CHECKCIPLUSCAP:
            MTCI_PRINT("invoke _MTCI_CheckCIPLUSCapability begin\n");
            result = _MTCI_CheckCIPLUSCapability();
            break;
		case MTAL_IO_CI_READCIPLUSVER:
            MTCI_PRINT("invoke _MTCI_ReadCIPLUSVersion begin\n");
            result = _MTCI_ReadCIPLUSVersion(arg);
            break;
		case MTAL_IO_CI_READCIPLUSPROF:
            MTCI_PRINT("invoke _MTCI_ReadCIPLUSCiprof begin\n");
            result = _MTCI_ReadCIPLUSCiprof(arg);
            break;
        case MTAL_IO_CI_WRITECOR:
            MTCI_PRINT("invoke _MTCI_WriteCor begin\n");
            result = _MTCI_WriteCor(arg);
            break;
        case MTAL_IO_CI_READ:
            MTCI_PRINT("invoke _MTCI_Read begin\n");
            result = _MTCI_Read(arg);
            break;
        case MTAL_IO_CI_WRITE:
            MTCI_PRINT("invoke _MTCI_Write begin\n");
            result = _MTCI_Write(arg);
            break;
        case MTAL_IO_CI_NEGOBUF:
            MTCI_PRINT("invoke _MTCI_NegoBuf begin\n");
            result = _MTCI_NegoBuf(arg);
            break;
        case MTAL_IO_CI_READDASTATUS:
            MTCI_PRINT("invoke _MTCI_ReadDAStatus begin\n");
            result = _MTCI_ReadDAStatus();
            break;
        case MTAL_IO_CI_READREG:
            MTCI_PRINT("invoke _MTCI_ReadReg begin\n");
            result = _MTCI_ReadReg(arg);
            break;
        case MTAL_IO_CI_WRITEREG:
            MTCI_PRINT("invoke _MTCI_WriteReg begin\n");
            result = _MTCI_WriteReg(arg);
            break;
        case MTAL_IO_CI_READCOR:
            MTCI_PRINT("invoke _MTCI_ReadCor begin\n");
            result = _MTCI_ReadCor(arg);
            break;
        case MTAL_IO_CI_TESTCARDREADY:
            MTCI_PRINT("invoke _MTCI_TestCardReady begin\n");
            result = _MTCI_TestCardReady();
            break;
        case MTAL_IO_CI_ENABLESLOT:
            MTCI_PRINT("invoke _MTCI_EnableSlot begin\n");
            result = _MTCI_EnableSlot(arg);
            break;
        case MTAL_IO_CI_DETECTPODACTIVE:
            MTCI_PRINT("invoke _MTCI_DetectPodActive begin\n");
            result = _MTCI_DetectPodActive();
            break;
        case MTAL_IO_CI_PCMCIAENABLETS:
            MTCI_PRINT("invoke _MTCI_PcmciaEnableTs begin\n");
            result = _MTCI_PcmciaEnableTs(arg);
            break;
        case MTAL_IO_CI_RESETCAM:
            MTCI_PRINT("invoke _MTCI_ResetCam begin\n");
            result = _MTCI_ResetCam();
            break;
        case MTAL_IO_CI_SETCARDDETECTONOFF:
            MTCI_PRINT("invoke _MTCI_SetCardDetectOnOff begin\n");
            result = _MTCI_SetCardDetectOnOff(arg);
            break;
        case MTAL_IO_CI_SETBYPASS:
            MTCI_PRINT("invoke _MTCK_SetBypass begin\n");
            result = _MTCK_SetBypass(arg);
            break;
		case MTAL_IO_CI_BYPASSONOFF:
	        MTCI_PRINT("invoke _MTCI_BypassOnOff begin\n");
            result = _MTCI_BypassOnOff(arg);
            break;
        case MTAL_IO_CI_EXTERNALDEMOD:
            MTCI_PRINT("invoke _MTCI_ExternalDemodOnOff begin\n");
            result = _MTCI_ExternalDemodOnOff(arg);
            break;
        case MTAL_IO_CI_SETTSPATH:
            MTCI_PRINT("invoke _MTCI_SetTsPath begin\n");
            result = _MTCI_SetTsPath(arg);
            break;

/////////////////////////////////////////////////////////////////
//////// MTAL For MTK MW
		case MTAL_IO_CI_INIT0:
			result = _MTCI0_Init();
			break;
#ifdef CC_DUAL_TUNER_SUPPORT
        case MTAL_IO_CI_DUALTUNERTSPATH:
            result = _MTCI0_SetDualTsPath(arg);
            break;
#endif
        case MTAL_IO_CI_SETPODCIMODE:
            result = _MTCI_SetPodciAccessMode(arg);
			break;
        case MTAL_IO_CI_PCMCIATOPOD:
            result = _MTCI_PcmciaToPod(arg);
			break;
        case MTAL_IO_CI_ENABLEISR:
            result = _MTCI_EnableIsr();
			break;
        case MTAL_IO_CI_AUTOSETTSPATH:
            result = _MTCI_AutoSetTsPath();
			break;
		case MTAL_IO_CI_CAMWORKAROUND:
			result = _MTCI_CiCamWorkaround(arg);
			break;
		case MTAL_IO_CI_SetBypass0:
			result = _MTCI0_SetBypass(arg);
			break;	
		case MTAL_IO_CI_SetBypassTsRouting0:
			result = _MTCI0_SetBypassTsRouting(arg);
			break;
		case MTAL_IO_CI_RegIsr:
			result = _MTCI0_RegIsr(arg);
			break;
#ifdef CC_SUPPORT_STR			
        case MTAL_IO_CI_FAST_BOOT_CB:
            result = _MTCI0_RegFastBootCB(arg);
            break;
#endif
#ifdef CC_MT5890
        case MTAL_IO_CI_SETCAMVER:
            result = _MTCI0_SetCamVer(arg);
            break;
#endif
/////////////////////////////////////////////////////////////////

        default:
            result = MTR_NOT_OK;
            break;
    }

    x_sema_unlock(hMtciSemaHandle);
    
    return result;
}
 

