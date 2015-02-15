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
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file dmx_cryptkey_mw.c
 *  DMX crypt key management middleware
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "dmx_if.h"
#include "fvr_if.h"
#include "dmx_mw.h"
#include "x_lint.h"

LINT_EXT_HEADER_BEGIN

#include "x_assert.h"
#include "x_hal_5381.h"
#include "x_pvr.h"
#include "x_os.h"

#include "x_debug.h"

LINT_EXT_HEADER_END

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
#define DMX_MAX_NUM_CA_KEY              12
#define   DMX_DTV_CA_KEY_START            0     // for key which will be changed at run-time 
#define   DMX_DTV_CA_KEY_END              3
#define   DMX_MAX_NUM_DTV_CA_KEY        (DMX_DTV_CA_KEY_END - DMX_DTV_CA_KEY_START + 1)
#define   DMX_MM_CA_KEY_START             4     // for key which won't be changed at run-time 
#define   DMX_MM_CA_KEY_END               11 
#define   DMX_MAX_NUM_MM_CA_KEY         (DMX_MM_CA_KEY_END - DMX_MM_CA_KEY_START + 1)

#define DMX_MAX_NUM_FVR_KEY             4
#define   DMX_FVR_KEY_START               0
#define   DMX_FVR_KEY_END                 3

#define TOTAL_NUM_PID_INDEX         (DMX_NUM_PID_INDEX+FVR_NUM_PID_INDEX)


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
typedef struct
{
    BOOL    fgEven;
    UINT8   u1Idx;
} MW_DMX_CRYPT_KEYX_T; 

typedef struct
{
    BOOL fgEvenValid;
    BOOL fgOddValid;
    DESC_ALG_T eDescAlg;            //Descramble Algorithm
    MW_DMX_CRYPT_KEY_UNION_T uEvenKeySet;
    MW_DMX_CRYPT_KEY_UNION_T uOddKeySet;
} MW_DMX_CRYPT_KEY_SLOT_T;


//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Import functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------

static MW_DMX_CRYPT_KEY_SLOT_T _arMwDmxCAKey[DMX_MAX_NUM_CA_KEY];
static MW_DMX_CRYPT_KEY_SLOT_T _arMwDmxFvrKey[DMX_MAX_NUM_FVR_KEY];

static MW_DMX_CRYPT_KEYX_T _arPid2CAKeyIdxMap[TOTAL_NUM_PID_INDEX];
static INT32 _arCAEvenKeyIdxRef[DMX_MAX_NUM_DTV_CA_KEY];
static INT32 _arCAOddKeyIdxRef[DMX_MAX_NUM_DTV_CA_KEY];

static HANDLE_T _hDmxCryptMutex = (HANDLE_T)NULL;
//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

static INT32 _MwDmxCryptCompareAESKey(MW_DMX_CRYPT_KEY_UNION_T *puAESKey1, MW_DMX_CRYPT_KEY_UNION_T *puAESKey2)
{
    INT32 i, result = 0;

    for (i = (MW_DMX_AES_KEY_LENGTH - 1); i >= 0; i--)
    {
        if (puAESKey1->rAESKeySet.au4Key[i] != puAESKey2->rAESKeySet.au4Key[i])
        {
            result = -1;
            break;
        }
    }

    if (result >= 0)
    {
        for (i = (MW_DMX_AES_IV_KEY_LENGTH - 1); i >= 0; i--)
        {
            if (puAESKey1->rAESKeySet.au4Iv[i] != puAESKey2->rAESKeySet.au4Iv[i])
            {
                result = -1;
                break;
            }
        }
    }

    return result;
}

//-----------------------------------------------------------------------------
/** _MwDmxCryptFindKeySlot
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxCryptFindKeySlot(MW_DMX_CRYPT_KEY_SLOT_T *prKeySlot, 
                            UINT8 u1StartIdx, UINT8 u1TotalNum,
                            MW_DMX_CRYPT_KEY_INFO_T *prKeyInfo,
                            UINT8 *pu1ReturnKeyIdx)
{
    UINT8 i;
    UINT8 u1FreeIdx = u1TotalNum;
    MW_DMX_CRYPT_KEY_UNION_T *puCryptKey2;
    BOOL fgFound = FALSE, fgValid = FALSE;

    if ((prKeyInfo == NULL) || (prKeySlot == NULL) || (pu1ReturnKeyIdx == NULL))
    {
        return MW_DMX_CRYPT_INVALID_KEYX;
    }

    /* search valid key */
    for (i = u1StartIdx; i < u1TotalNum; i++)
    {
        fgValid = (prKeyInfo->fgEven) ? prKeySlot[i].fgEvenValid : prKeySlot[i].fgOddValid;

        /* find emptry minimum key set or emptry minimum key slot with same alg */
        if ((!prKeySlot[i].fgEvenValid && !prKeySlot[i].fgOddValid) ||
           (!fgValid && (prKeySlot[i].eDescAlg == prKeyInfo->eDescAlg)))
        {
            if (i < u1FreeIdx)
            {
                u1FreeIdx = i;
            }
        }

        if (fgValid && (prKeySlot[i].eDescAlg == prKeyInfo->eDescAlg))
        {
            puCryptKey2 = (prKeyInfo->fgEven) ? &(prKeySlot[i].uEvenKeySet) : &(prKeySlot[i].uOddKeySet);
            switch (prKeyInfo->eDescAlg)
            {
                case DESC_ALG_AES_128_ECB:
                case DESC_ALG_AES_128_CBC:
                    if (0 == _MwDmxCryptCompareAESKey(prKeyInfo->puCryptKey, puCryptKey2))
                    {
                        fgFound = TRUE;
                    }
                    break;
                
                default:
                    break;
            }

            if (fgFound) {
                break;
            }
        }
    }

    if (fgFound)
    {
        *pu1ReturnKeyIdx = i;
    }
    else if (u1FreeIdx < u1TotalNum)
    {
        *pu1ReturnKeyIdx = u1FreeIdx;
    }

    return fgFound;
}

//-----------------------------------------------------------------------------
/** _MwDmxCryptFindStaticKeySlot
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxCryptFindStaticKeySlot(MW_DMX_CRYPT_KEY_SLOT_T *prKeySlot,
                                        UINT8 u1StartIdx, UINT8 u1TotalNum, 
                                        UINT8 *pu1ReturnKeyIdx)
{
    UINT8 i;
    BOOL fgFound = FALSE;

    if ((prKeySlot == NULL) || (pu1ReturnKeyIdx == NULL))
    {
        return MW_DMX_CRYPT_INVALID_KEYX;
    }

    /* search valid key */
    for (i = u1StartIdx; i < u1TotalNum; i++)
    {
        if (!prKeySlot[i].fgEvenValid && !prKeySlot[i].fgOddValid)
        {
            *pu1ReturnKeyIdx = i;
            fgFound = TRUE;
            break;
        }
    }

    return fgFound;
}

//-----------------------------------------------------------------------------
/** _MwDmxCryptSaveKey
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxCryptSaveKey(MW_DMX_CRYPT_KEY_SLOT_T *prKeySlot, UINT8 u1TotalNum,
                                MW_DMX_CRYPT_KEY_UNION_T *puCryptKey, UINT8 u1KeyIdx, BOOL fgEven)
{
    if ((prKeySlot == NULL) || (puCryptKey == NULL) || (u1KeyIdx >= u1TotalNum))
    {
        return FALSE;
    }
   
    if (fgEven)
    { 
        prKeySlot[u1KeyIdx].uEvenKeySet = *puCryptKey; 
    }
    else
    { 
        prKeySlot[u1KeyIdx].uOddKeySet = *puCryptKey;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MwDmxCryptResetKeySlot
 */
//-----------------------------------------------------------------------------
static BOOL _MwDmxCryptResetKeySlot(MW_DMX_CRYPT_KEY_SLOT_T *prKeySlot)
{
    if (prKeySlot == NULL)
    {
        return FALSE;
    }

    prKeySlot->fgEvenValid = FALSE; 
    prKeySlot->fgOddValid = FALSE; 
    prKeySlot->eDescAlg = DESC_ALG_NONE; 
    
    x_memset((void *)&(prKeySlot->uEvenKeySet), 0, sizeof(prKeySlot->uEvenKeySet));
    x_memset((void *)&(prKeySlot->uOddKeySet), 0, sizeof(prKeySlot->uOddKeySet));
    
    return TRUE;
}

//-----------------------------------------------------------------------------
// Inter-file functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_AllocateCAKeySlot
 *
 *  Allocate a key index for DTV and MM play. 
 *  If MWDMX_CRYPTKEY_FLAG_UPDATE_REF is enabled, the key index reference will 
 *  be updated.  If key index reference is 0, the key index which has been allocated 
 *  will be invalidated. This design is used for the case that crypt keys are 
 *  changed periodically. In current design, there is no key index for MM play. 
 *  Thus, the flags which are MWDMX_CRYPTKEY_FLAG_FVR_PIDX and MWDMX_CRYPTKEY_FLAG_UPDATE_REF, 
 *  and u1Pidx parameter are useless for MM play.
 *
 *
 *  @param  prKeyInfo                   The key information
 *  @param  u4Flags                     Function flags
 *            MWDMX_CRYPTKEY_FLAG_NONE        None
 *            MWDMX_CRYPTKEY_FLAG_MM_PLAY     Allocate key index for MM play
 *            MWDMX_CRYPTKEY_FLAG_FVR_PIDX    The given u1Pidx is a pidx of FVR
 *            MWDMX_CRYPTKEY_FLAG_UPDATE_REF  Update key index reference
 *  @param  u1Pidx                      The corresponding Pidx
 *  @param  pu1ReturnIdx                The pointer which points to the returned key index
 *
 *  @retval TRUE                        Succeed
 *  @retval FALSE                       Fail
 *
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_CRYPT_AllocateCAKeySlot(MW_DMX_CRYPT_KEY_INFO_T *prKeyInfo,
                                    UINT32 u4Flags, UINT8 u1Pidx,
                                    UINT8 *pu1ReturnIdx)
{
    UINT8 u1KeyIdx = MW_DMX_CRYPT_INVALID_KEYX; 
    UINT8 u1StartIdx, u1TotalNum;
    BOOL fgFound, fgRet = FALSE;
    //INT32 i;

    if (u4Flags & MWDMX_CRYPTKEY_FLAG_STATIC_IDX)
    {
        if (pu1ReturnIdx == NULL)
        {
            return FALSE;
        }
    }
    else if ((prKeyInfo == NULL) || (pu1ReturnIdx == NULL))
    {
        return FALSE;
    }

#if 0
    Printf("\n");
    for (i = 0; i < TOTAL_NUM_PID_INDEX; i++)
    {
        Printf("%u ", _arPid2CAKeyIdxMap[i].u1Idx);
    }
    Printf("\n");

    Printf("\n");
    for (i = 0; i < DMX_MAX_NUM_DTV_CA_KEY; i++)
    {
        Printf("%u ", _arCAEvenKeyIdxRef[i]);
    }
    Printf("\n");

    Printf("\n");
    for (i = 0; i < DMX_MAX_NUM_DTV_CA_KEY; i++)
    {
        Printf("%u ", _arCAOddKeyIdxRef[i]);
    }
    Printf("\n");
#endif

    if (u4Flags & MWDMX_CRYPTKEY_FLAG_MM_PLAY) 
    {
        u1StartIdx = DMX_MM_CA_KEY_START;
        u1TotalNum = DMX_MAX_NUM_MM_CA_KEY;
    }
    else
    {
        if (u4Flags & MWDMX_CRYPTKEY_FLAG_FVR_PIDX)
        {
            u1Pidx += DMX_NUM_PID_INDEX;
        }

        if (u1Pidx >= TOTAL_NUM_PID_INDEX)
        {
            return FALSE;
        }

        u1StartIdx = DMX_DTV_CA_KEY_START; 
        u1TotalNum = DMX_MAX_NUM_DTV_CA_KEY;
    }
    
    VERIFY(x_sema_lock(_hDmxCryptMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
    
    if (u4Flags & MWDMX_CRYPTKEY_FLAG_STATIC_IDX)
    {
        if (_MwDmxCryptFindStaticKeySlot(_arMwDmxCAKey, u1StartIdx, u1TotalNum, &u1KeyIdx))
        {
            _arMwDmxCAKey[u1KeyIdx].fgEvenValid = TRUE;
            _arMwDmxCAKey[u1KeyIdx].fgOddValid = TRUE;
            *pu1ReturnIdx = u1KeyIdx;
            fgRet = TRUE;
            Printf("%s - allocated static keyIdx %u\n", __FUNCTION__, u1KeyIdx);
        }
    }
    else
    {
        fgFound = _MwDmxCryptFindKeySlot(_arMwDmxCAKey,
                                        u1StartIdx, u1TotalNum,
                                        prKeyInfo, &u1KeyIdx);

        if (u1KeyIdx != MW_DMX_CRYPT_INVALID_KEYX)
        {
            if (!(u4Flags & MWDMX_CRYPTKEY_FLAG_MM_PLAY) && (u4Flags & MWDMX_CRYPTKEY_FLAG_UPDATE_REF))
            {
                MW_DMX_CRYPT_KEYX_T  u1KeyType;
                INT32 *pu4CAKeyIdxRef1, *pu4CAKeyIdxRef2;
            
                pu4CAKeyIdxRef1 = (prKeyInfo->fgEven) ? &_arCAEvenKeyIdxRef[0] : &_arCAOddKeyIdxRef[0];
                pu4CAKeyIdxRef1[u1KeyIdx]++;

                u1KeyType = _arPid2CAKeyIdxMap[u1Pidx];
                if (u1KeyType.u1Idx == MW_DMX_CRYPT_INVALID_KEYX)
                {
                    u1KeyType.u1Idx = u1KeyIdx; 
                    u1KeyType.fgEven = prKeyInfo->fgEven; 
                    _arPid2CAKeyIdxMap[u1Pidx] = u1KeyType;
                }
                else if ((u1KeyType.u1Idx != u1KeyIdx) || (u1KeyType.fgEven != prKeyInfo->fgEven))
                {
                    pu4CAKeyIdxRef2 = (u1KeyType.fgEven) ? &_arCAEvenKeyIdxRef[0] : &_arCAOddKeyIdxRef[0];
                    if (pu4CAKeyIdxRef2[u1KeyType.u1Idx] > 0)
                    {
                        pu4CAKeyIdxRef2[u1KeyType.u1Idx]--;
                        if (pu4CAKeyIdxRef2[u1KeyType.u1Idx] <= 0)
                        {
                            pu4CAKeyIdxRef2[u1KeyType.u1Idx] = 0;
                            //invalid key slot
                            if (u1KeyType.fgEven) {
                                _arMwDmxCAKey[u1KeyType.u1Idx].fgEvenValid = FALSE;
                            }
                            else 
                            {
                                _arMwDmxCAKey[u1KeyType.u1Idx].fgOddValid = FALSE;
                            }
                            _arMwDmxCAKey[u1KeyType.u1Idx].eDescAlg = DESC_ALG_NONE;
                        }
                    }
                   
                    /* updaet new map */ 
                    u1KeyType.u1Idx = u1KeyIdx; 
                    u1KeyType.fgEven = prKeyInfo->fgEven; 
                    _arPid2CAKeyIdxMap[u1Pidx] = u1KeyType;
                }
            }  // end of MWDMX_CRYPTKEY_FLAG_UPDATE_REF
        
            *pu1ReturnIdx = u1KeyIdx;

            if (prKeyInfo->fgEven) {
                _arMwDmxCAKey[u1KeyIdx].fgEvenValid = TRUE;
            }
            else
            {
                _arMwDmxCAKey[u1KeyIdx].fgOddValid = TRUE;
            }
            _arMwDmxCAKey[u1KeyIdx].eDescAlg = prKeyInfo->eDescAlg;

            if (!fgFound)
            {
                fgRet = _MwDmxCryptSaveKey(_arMwDmxCAKey, DMX_MAX_NUM_CA_KEY, 
                                        prKeyInfo->puCryptKey, u1KeyIdx, prKeyInfo->fgEven);
            }
            else
            {
                fgRet = TRUE;
            }
        }
        else
        {
            fgRet = FALSE;
        }
    }  //MWDMX_CRYPTKEY_FLAG_STATIC_IDX
    
    VERIFY(x_sema_unlock(_hDmxCryptMutex) == OSR_OK);
    
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_ResetCAKeySlotByKeyIdx
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_CRYPT_ResetCAKeySlotByKeyIdx(UINT8 u1KeyIdx)
{
    INT32 i;
    BOOL fgRet;

    if (u1KeyIdx >= DMX_MAX_NUM_CA_KEY)
    {
        return FALSE;
    }
    
    VERIFY(x_sema_lock(_hDmxCryptMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
    
    fgRet = _MwDmxCryptResetKeySlot(&_arMwDmxCAKey[u1KeyIdx]);

    if (fgRet)
    {
        for (i = 0; i < TOTAL_NUM_PID_INDEX; i++)
        {
            if (u1KeyIdx == _arPid2CAKeyIdxMap[i].u1Idx)
            {
                _arPid2CAKeyIdxMap[i].u1Idx = MW_DMX_CRYPT_INVALID_KEYX; 
            }
        }

        if (u1KeyIdx <= DMX_DTV_CA_KEY_END)
        { 
            _arCAEvenKeyIdxRef[u1KeyIdx] = 0;
            _arCAOddKeyIdxRef[u1KeyIdx] = 0;
        }
    }
    
    VERIFY(x_sema_unlock(_hDmxCryptMutex) == OSR_OK);
    
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_ResetCAKeySlotByPidx
 *
 * In current design, this API is only useful to reset Pidx of DTV excluding FVR
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_CRYPT_ResetCAKeySlotByPidx(UINT8 u1Pidx, BOOL fgFvrPidx)
{
    INT32 *pu4CAKeyIdxRef;
    UINT8 u1KeyIdx;
    BOOL fgEven;

    if (fgFvrPidx)
    {
        u1Pidx += DMX_NUM_PID_INDEX;
    }

    if (u1Pidx >= TOTAL_NUM_PID_INDEX)
    {
        return FALSE;
    }
    
    VERIFY(x_sema_lock(_hDmxCryptMutex, X_SEMA_OPTION_WAIT) == OSR_OK);

    u1KeyIdx = _arPid2CAKeyIdxMap[u1Pidx].u1Idx;
    if (u1KeyIdx > DMX_DTV_CA_KEY_END)
    {
        VERIFY(x_sema_unlock(_hDmxCryptMutex) == OSR_OK);
        return TRUE;
    }

    fgEven = _arPid2CAKeyIdxMap[u1Pidx].fgEven;
    pu4CAKeyIdxRef = (fgEven) ?_arCAEvenKeyIdxRef : _arCAOddKeyIdxRef;

    if (pu4CAKeyIdxRef[u1KeyIdx] > 0)
    {
        pu4CAKeyIdxRef[u1KeyIdx]--;
        if (pu4CAKeyIdxRef[u1KeyIdx] <= 0)
        {
            pu4CAKeyIdxRef[u1KeyIdx] = 0;
            if (fgEven)
            {
                _arMwDmxCAKey[u1KeyIdx].fgEvenValid = FALSE; 
                x_memset((void *)&(_arMwDmxCAKey[u1KeyIdx].uEvenKeySet), 0, sizeof(_arMwDmxCAKey[u1KeyIdx].uEvenKeySet));

                _arPid2CAKeyIdxMap[u1Pidx].fgEven = FALSE;
                _arPid2CAKeyIdxMap[u1Pidx].u1Idx = MW_DMX_CRYPT_INVALID_KEYX;
                Printf("%s - invalidate Even reference\n", __FUNCTION__);
            }
            else
            {
                _arMwDmxCAKey[u1KeyIdx].fgOddValid = FALSE; 
                x_memset((void *)&(_arMwDmxCAKey[u1KeyIdx].uOddKeySet), 0, sizeof(_arMwDmxCAKey[u1KeyIdx].uOddKeySet));
            
                _arPid2CAKeyIdxMap[u1Pidx].fgEven = FALSE;
                _arPid2CAKeyIdxMap[u1Pidx].u1Idx = MW_DMX_CRYPT_INVALID_KEYX;
                
                Printf("%s - invalidate Odd reference\n", __FUNCTION__);
            }
    
            if ((_arCAEvenKeyIdxRef[u1KeyIdx] == 0) && (_arCAOddKeyIdxRef[u1KeyIdx] == 0))
            {
                _arMwDmxCAKey[u1KeyIdx].eDescAlg = DESC_ALG_NONE; 
                
                Printf("%s - invalidate All reference\n", __FUNCTION__);
            }
        }
        //Printf("%s - Even ref %d\n", __FUNCTION__, _arCAEvenKeyIdxRef[u1KeyIdx]);
        //Printf("%s - Odd ref %d\n", __FUNCTION__, _arCAOddKeyIdxRef[u1KeyIdx]);
    } 
    
    VERIFY(x_sema_unlock(_hDmxCryptMutex) == OSR_OK);
    
    return TRUE;
}


//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_ResetAllCAKeySlots
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_CRYPT_ResetAllCAKeySlots(BOOL fgMMplay)
{
    INT32 i;

    VERIFY(x_sema_lock(_hDmxCryptMutex, X_SEMA_OPTION_WAIT) == OSR_OK);

    if (fgMMplay)
    {
        for (i = DMX_MM_CA_KEY_START; i <= DMX_MM_CA_KEY_END; i++)
        {
            _MwDmxCryptResetKeySlot(&_arMwDmxCAKey[i]);
        }
    }
    else
    {
        for (i = 0; i < TOTAL_NUM_PID_INDEX; i++)
        {
            _arPid2CAKeyIdxMap[i].fgEven = FALSE;
            _arPid2CAKeyIdxMap[i].u1Idx = MW_DMX_CRYPT_INVALID_KEYX;
        }

        x_memset(_arCAEvenKeyIdxRef, 0, sizeof(_arCAEvenKeyIdxRef));
        x_memset(_arCAOddKeyIdxRef, 0, sizeof(_arCAOddKeyIdxRef));

        for (i = DMX_DTV_CA_KEY_START; i <= DMX_DTV_CA_KEY_END; i++)
        {
            _MwDmxCryptResetKeySlot(&_arMwDmxCAKey[i]);
        }
    }
    
    VERIFY(x_sema_unlock(_hDmxCryptMutex) == OSR_OK);
    
    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_AllocateFvrKeySlot
 *  for record encryption
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_CRYPT_AllocateFvrKeySlot(MW_DMX_CRYPT_KEY_INFO_T *prKeyInfo, UINT8 *pu1ReturnIdx)
{
    UINT8 u1KeyIdx = MW_DMX_CRYPT_INVALID_KEYX; 
    BOOL fgFound, fgRet = FALSE;

    if ((prKeyInfo == NULL) || (pu1ReturnIdx == NULL))
    {
        return FALSE;
    }
    
    fgFound = _MwDmxCryptFindKeySlot(_arMwDmxFvrKey, 
                                    DMX_FVR_KEY_START, DMX_MAX_NUM_FVR_KEY, 
                                    prKeyInfo, &u1KeyIdx);

    if (u1KeyIdx != MW_DMX_CRYPT_INVALID_KEYX)
    {
        *pu1ReturnIdx = u1KeyIdx;
        
        if (prKeyInfo->fgEven) {
            _arMwDmxFvrKey[u1KeyIdx].fgEvenValid = TRUE;
        }
        else
        {
            _arMwDmxFvrKey[u1KeyIdx].fgOddValid = TRUE;
        }
        
        _arMwDmxFvrKey[u1KeyIdx].eDescAlg = prKeyInfo->eDescAlg;

        if (!fgFound)
        {
            fgRet = _MwDmxCryptSaveKey(_arMwDmxFvrKey, DMX_MAX_NUM_FVR_KEY, 
                                    prKeyInfo->puCryptKey, u1KeyIdx, prKeyInfo->fgEven);
        }
        else
        {
            fgRet = TRUE;
        }
    }
    else
    {
        fgRet = FALSE;
    }
    
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_ResetFvrKeySlot
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_CRYPT_ResetFvrKeySlot(UINT8 u1KeyIdx)
{
    BOOL fgRet;

    if (u1KeyIdx >= DMX_MAX_NUM_FVR_KEY)
    {
        return FALSE;
    }
    
    VERIFY(x_sema_lock(_hDmxCryptMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
    
    fgRet = _MwDmxCryptResetKeySlot(&_arMwDmxFvrKey[u1KeyIdx]);

    VERIFY(x_sema_unlock(_hDmxCryptMutex) == OSR_OK);
    
    return fgRet;
}

//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_ResetAllFvrKeySlots
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_CRYPT_ResetAllFvrKeySlots(void)
{
    INT32 i;

    VERIFY(x_sema_lock(_hDmxCryptMutex, X_SEMA_OPTION_WAIT) == OSR_OK);
 
    for (i = 0; i < DMX_MAX_NUM_FVR_KEY; i++)
    {
        _MwDmxCryptResetKeySlot(&_arMwDmxFvrKey[i]);
    }
    
    VERIFY(x_sema_unlock(_hDmxCryptMutex) == OSR_OK);
    
    return TRUE;
}

//-----------------------------------------------------------------------------
/** _MW_DMX_CRYPT_Init
 */
//-----------------------------------------------------------------------------
BOOL _MW_DMX_CRYPT_Init(void)
{
    if (!_hDmxCryptMutex)
    {
        VERIFY(x_sema_create(&_hDmxCryptMutex, X_SEMA_TYPE_MUTEX, X_SEMA_STATE_UNLOCK) == OSR_OK);
    
    }

    _MW_DMX_CRYPT_ResetAllCAKeySlots(FALSE);    // reset DTV
    _MW_DMX_CRYPT_ResetAllCAKeySlots(TRUE);     // reset MM
    
    _MW_DMX_CRYPT_ResetAllFvrKeySlots();

    return TRUE;
}
