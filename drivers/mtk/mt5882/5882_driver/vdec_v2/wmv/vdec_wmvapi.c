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
 *-----------------------------------------------------------------------------
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: vdec_wmvapi.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

/** @file vdec_wmvapi.c
 *  This file contains implementation of exported APIs of VDEC.
 */

#include "x_lint.h"
#include "vdec_drvif.h"
#include "vdec_wmvpsr.h"
#include "vdec_wmvdec.h"
#include "vdec_wmvutil.h"
#include "vdec_wmvhw.h"
#include "vdec_wmvapi.h"
LINT_EXT_HEADER_BEGIN
#include "x_hal_926.h"
#include "x_hal_5381.h"
#include "x_chip_id.h"
#include "x_common.h"
#include "x_typedef.h"
LINT_EXT_HEADER_END


#ifndef AVC_IRQ_DISABLE
#define WMVVLD_VECTOR 24
extern HANDLE_T hVdecWMVFinSema;
extern UCHAR ucVLDISREsId;
#endif

//global variable
#define MAX_ES_NUM VDEC_MAX_ES
#define ES_ID_0 0

static const UINT8 _u1vopFirstFieldType[8] =
    { IVOP, IVOP, PVOP, PVOP, BVOP, BVOP, BIVOP, BIVOP };
static const UINT8 _u1vopSecondFieldType[8] =
    { IVOP, PVOP, IVOP, PVOP, BVOP, BIVOP, BVOP, BIVOP };
#define WMV_SC_FRAME        0x10D
#define WMV_SC_ENTRY        0x10E
#define WMV_SC_SEQ          0x10F
#define WMV_SC_FIELD        0x10C
extern void reset_pic_hdr_bits(UCHAR ucEsId);

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
VDEC_WMV_INFO_T *prWMVDecInfo[MAX_ES_NUM] = {NULL};
#else
VDEC_WMV_INFO_T rWMVDecInfo[MAX_ES_NUM];
#endif


/** .
*@param .
 */
VDEC_HDLR _VDEC_WMVGetHdlr(UCHAR ucEsId)
{
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(ucEsId<MAX_ES_NUM)
    {
        return  prWMVDecInfo[ucEsId];
    }
    else
    {
        LOG(0, "ES ID is out of range: %d, force ES0 !!!!!!\n", ucEsId);
        ASSERT(0);
        return prWMVDecInfo[ES0];  // tmp solution to shut up Klocwork
    }  
#else
    if(ucEsId<MAX_ES_NUM)
    {
        return (VDEC_HDLR)&rWMVDecInfo[ucEsId];
    }
    else
    {
        LOG(0, "ES ID is out of range: %d, force ES0 !!!!!!\n", ucEsId);
        ASSERT(0);
        return (VDEC_HDLR)&rWMVDecInfo[ES0];  // tmp solution to shut up Klocwork
    }
#endif
}


/** .
*@param .
 */
INT32 _VDEC_WMVInit(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec;

#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
    if(prWMVDecInfo[ucEsId] == NULL)
    {
        prWMVDecInfo[ucEsId] = (VDEC_WMV_INFO_T*)x_mem_alloc_virtual(sizeof(VDEC_WMV_INFO_T));
        if(prWMVDecInfo[ucEsId] == NULL)
        {
            LOG(0,"Allocate driver info struct Fail\n");
            ASSERT(0);
        }
    }
#endif     

    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);

    x_memset((void*)prVdec, 0, sizeof(VDEC_WMV_INFO_T));
    prVdec->rOther.u4LastClustPos=0xFFFFFFFF;
    prVdec->rOther.u1FbgId = FBM_FBG_ID_UNKNOWN;
    prVdec->rOther.fgFirstFrame = TRUE;
    //_WMVInit(hVdec, pi4RetValue); we need wmv type, move it to header parsing
    prVdec->rOther.fgNewMode = TRUE;
    prVdec->rOther.fgBindingByte = FALSE;
    prVdec->rOther.u1BindingByte = 0x00;
     prVdec->rOther.fgVC1NoCalPts = FALSE;
    prVdec->rOther.fgNoFrameRate = FALSE;
    prVdec->rOther.u4VC1Pts_0 = 0;
    prVdec->rOther.u4VC1Pts_n = 0;
    prVdec->rOther.u4PtsIdx = 0;
    prVdec->rOther.fgEncrypted = FALSE;
    return E_VDEC_OK;
}

/** .
*@param .
 */
INT32 _VDEC_WMVHdrPrs(UCHAR ucVdecId, UCHAR ucEsId)
{
    INT32 i4RetValue;
    VDEC_WMV_INFO_T* prVdec;

    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);

    LOG(7,"HdrPrs lock[%d]\n",ucEsId);
    _VDEC_LockVld(ucEsId,0);//maiyou1
    
#ifdef WMV_MINST
    vRestoreVDec(ucEsId, FALSE);
#endif
    
    _WMVHdrPrs(ucEsId,&i4RetValue);
    if (i4RetValue == E_VDEC_SKIPFRAME) 
    {
        INT32 i4RetValue2;
#ifdef CC_WMV_EMULATION
        //LOG(0,"Frame %d : Skip Frame .........................................\n", rWMVDecInfo[0].rOther.u4CurrPicNo);
#endif
#ifdef DYNAMIC_MEM_ALLOC_DRV_INFO
        LOG(4,"Frame %d : Skip Frame .........................................\n", prWMVDecInfo[0]->rOther.u4CurrPicNo);
#else
        LOG(4,"Frame %d : Skip Frame .........................................\n", rWMVDecInfo[0].rOther.u4CurrPicNo);
#endif
        if (prVdec)
            prVdec->rOther.u4CurrPicNo++;
        _WMVSWRst(ucEsId,&i4RetValue2);
        
        LOG(7,"HdrPrs unlock[%d]\n",ucEsId);
        _VDEC_UnlockVld(ucEsId,0);//maiyou1
        
        return i4RetValue;
    }
    
#ifdef WMV_MINST
    vSaveVDec(ucEsId);
#endif

    LOG(7,"HdrPrs unlock[%d]\n",ucEsId);
    _VDEC_UnlockVld(ucEsId,0);//maiyou1
    
    return i4RetValue;
}

/** .
*@param .
 */
INT32 _VDEC_WMVDecStart(UCHAR ucVdecId, UCHAR ucEsId)
{
    INT32 i4RetValue;

    LOG(7,"DecStart lock[%d]\n",ucEsId);
    _VDEC_LockVld(ucEsId,0);//maiyou1
    
#ifdef WMV_MINST
    vRestoreVDec(ucEsId, TRUE);
#endif
    
    _WMVDecStart(ucEsId,&i4RetValue);
    
#ifdef VDEC_DEC_REG_QUERY
    VDEC_PrintDecReg( ucVdecId, e_DECODE_START_REG_DUMP );
#endif // VDEC_DEC_REG_QUERY

    return i4RetValue;
}

/** .
*@param .
 */
INT32 _VDEC_WMVDecFin(UCHAR ucVdecId, UCHAR ucEsId)
{
    INT32 i4RetValue;
    _WMVDecFin(ucEsId,&i4RetValue);
    if (i4RetValue == E_VDEC_DISP_NOT_READY) {
        INT32 i4RetValue2;
        _WMVSWRst(ucEsId,&i4RetValue2);
    }

#ifdef WMV_MINST
    //vSaveVDec(ucEsId);
#endif

    LOG(7,"DecFin unlock[%d]\n",ucEsId);
    _VDEC_UnlockVld(ucEsId,0);//maiyou1

#ifdef VDEC_DEC_REG_QUERY
    VDEC_PrintDecReg( ucVdecId, e_DECODE_DONE_REG_DUMP );
#endif // VDEC_DEC_REG_QUERY

    return i4RetValue;
}


VOID _VDEC_WMVSWRst(UCHAR ucVdecId, UCHAR ucEsId)
{
    INT32 i4RetValue;

    _VDEC_LockVld(ucEsId,0);  // megaa 20100730: power on/off patch

    _WMVSWRst(ucEsId,&i4RetValue);

    _VDEC_UnlockVld(ucEsId,0);  // megaa 20100730: power on/off patch
}


VOID _VDEC_WMVStop(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec;

    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);
    //VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    
    if (!prVdec)
        return;
        
#ifdef USE_FBM
    if (prVdec->rOther.u1FbgId != FBM_FBG_ID_UNKNOWN)
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, ucEsId);
        FBM_WaitUnlockFrameBuffer(prVdec->rOther.u1FbgId,20);    // block until display unlock
        FBM_ReleaseGroup(prVdec->rOther.u1FbgId);
        //prVdec->rOther.u1FbgId = FBM_FBG_ID_UNKNOWN;
    }
#endif

//clear decoder information
    //_WMVStop();
    x_memset((void*)prVdec, 0, sizeof(VDEC_WMV_INFO_T));
#ifdef USE_FBM
    prVdec->rOther.u1FbgId = FBM_FBG_ID_UNKNOWN;
#endif
    prVdec->rOther.fgFirstFrame = TRUE;

#if 0
    _VDEC_LockFlushMutex(ucEsId);

    if (prVdecEsInfo->fgFlushEsmQ)
    {
        _VDEC_FlushEs(ucEsId);
        prVdecEsInfo->fgFlushEsmQ = FALSE;
        //Printf("_MPV_StopDecES prVdecEsInfo->fgFlushEsmQ = FALSE\n");
        _VDEC_UnlockFlushSema(ucEsId);
    }
    _VDEC_ClearEsInfo(ucEsId);

    _VDEC_UnlockFlushMutex(ucEsId);
#endif

}


VOID _VDEC_WMVGetPesStruct(UCHAR ucEsId, void **ppvPesInfo)
{
    VDEC_WMV_INFO_T* prVdec;
    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);
    *ppvPesInfo = (void*)(&prVdec->rPesInfo);
    //return E_VDEC_OK;
}


VOID _VDEC_WMVGetDispPic(UCHAR ucEsId, void* pvInfo)
{
    VDEC_WMV_INFO_T* prVdec;
    VDEC_DISP_PIC_INFO_T* prDispInfo;
    
    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);
    prDispInfo = (VDEC_DISP_PIC_INFO_T*)pvInfo;

    if (!prVdec || !prDispInfo)
        return;
        
    prDispInfo->u4YSize = prVdec->rSeqHdrR.u4PicWidthDec * prVdec->rSeqHdrR.u4PicHeightDec;
    prDispInfo->u4CSize = prDispInfo->u4YSize >> 1;

    prDispInfo->u4W = prVdec->rOther.rPicInfo.dwW;
    prDispInfo->u4H = prVdec->rOther.rPicInfo.dwH;

    prDispInfo->u4W_Cmp = prVdec->rSeqHdrR.u4PicWidthCmp;
    prDispInfo->u4H_Cmp = prVdec->rSeqHdrR.u4PicHeightCmp;

    prDispInfo->u4CurrentPicNo = prVdec->rOther.u4CurrPicNo;
    
    prDispInfo->pvYAddr = (void*)prVdec->rOther.rVSyncPrm.dwPtrY;
    prDispInfo->pvCAddr = (void*)prVdec->rOther.rVSyncPrm.dwPtrC;

    prDispInfo->i4CurrentTemporalField = prVdec->rPicLayer.i4CurrentTemporalField;
    prDispInfo->u1FrameCodingMode = prVdec->rPicLayer.u1FrameCodingMode;
    //wmv crc
#if defined(CC_WMV_EMULATION) || defined(MULTIMEDIA_AUTOTEST_SUPPORT) || defined(VDEC_CRC_TEST)
#if defined(VDEC_CRC_TEST) && !defined(MULTIMEDIA_AUTOTEST_SUPPORT) && !defined(CC_WMV_EMULATION)
    if(VDEC_IsTestCRC(ucEsId))
#endif
    {
        prDispInfo->u4Ycrc[0]=prVdec->rOther.rCrcValue[0][0][0];
        prDispInfo->u4Ycrc[1]=prVdec->rOther.rCrcValue[0][0][1];
        prDispInfo->u4Ycrc[2]=prVdec->rOther.rCrcValue[0][0][2];
        prDispInfo->u4Ycrc[3]=prVdec->rOther.rCrcValue[0][0][3];
        prDispInfo->u4Ccrc[0]=prVdec->rOther.rCrcValue[0][1][0];
        prDispInfo->u4Ccrc[1]=prVdec->rOther.rCrcValue[0][1][1];
        prDispInfo->u4Ccrc[2]=prVdec->rOther.rCrcValue[0][1][2];
        prDispInfo->u4Ccrc[3]=prVdec->rOther.rCrcValue[0][1][3];
        prDispInfo->u1PicStruct=3;
        
        if(prVdec->rPicLayer.u1FrameCodingMode==INTERLACEFIELD)
        {
            prDispInfo->u4Ycrc[4]=prVdec->rOther.rCrcValue[1][0][0];
            prDispInfo->u4Ycrc[5]=prVdec->rOther.rCrcValue[1][0][1];
            prDispInfo->u4Ycrc[6]=prVdec->rOther.rCrcValue[1][0][2];
            prDispInfo->u4Ycrc[7]=prVdec->rOther.rCrcValue[1][0][3];
            prDispInfo->u4Ccrc[4]=prVdec->rOther.rCrcValue[1][1][0];
            prDispInfo->u4Ccrc[5]=prVdec->rOther.rCrcValue[1][1][1];
            prDispInfo->u4Ccrc[6]=prVdec->rOther.rCrcValue[1][1][2];
            prDispInfo->u4Ccrc[7]=prVdec->rOther.rCrcValue[1][1][3];
            prDispInfo->u1PicStruct=2;
        }
    }
#endif
#if defined(MULTIMEDIA_AUTOTEST_SUPPORT) || defined(VDEC_CRC_TEST)
#if defined(VDEC_CRC_TEST) && !defined(MULTIMEDIA_AUTOTEST_SUPPORT)
    if(VDEC_IsTestCRC(ucEsId))
#endif
    {
        VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
        UINT32 u4CrcIndex=0;
        prVdecEsInfo->u4CrcResult=0;
        for(u4CrcIndex=0;u4CrcIndex<8;u4CrcIndex++)
        {
            prVdecEsInfo->u4CrcResult^=prDispInfo->u4Ycrc[u4CrcIndex];
            prVdecEsInfo->u4CrcResult^=prDispInfo->u4Ccrc[u4CrcIndex];
            
            if (u4CrcIndex == 3 && prVdec->rPicLayer.u1FrameCodingMode != INTERLACEFIELD)
                break;
        }
    }
#endif
}

VOID _VDEC_WMVSetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{    
    VDEC_WMV_INFO_T* prVdec;
    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);
    
    if (u4Type == 0) //set resolution
    {
        if (prVdec)
        {
            prVdec->rSeqHdrR.u4PicWidthSrc = (UINT32)u4Param1;
            prVdec->rSeqHdrR.u4PicHeightSrc = (UINT32)u4Param2;
        }
    }
    else if (u4Type ==1) //set sequence header
    {
        if (prVdec)
        {
            prVdec->rOther.i4SeqHdrDataLen = u4Param1;
            prVdec->rOther.i4SeqHdrData1 = u4Param2;
        }
    }
#ifdef CC_WMV_EMULATION
    else if(u4Type==17)
    {
    
    }
#endif
    else if (u4Type == 18)  // ASF + WVC1 binding type
    {
        prVdec->rOther.fgBindingByte = TRUE;
        prVdec->rOther.u1BindingByte = *((UINT8*)u4Param1);
        LOG(3, "WMV setup binding byte = %02x\n", prVdec->rOther.u1BindingByte);
    }
	
    else if (u4Type == 19)  // encrypted mode
    {
        prVdec->rOther.fgEncrypted = (BOOL)u4Param1;
        LOG(3, "!!!!!!!!!!!!! Set fgEncrypted !!!\n");
    }
    else
    {
        //do nothing
    }
}

#ifdef VDEC_TIME_PROFILE
VOID _VDEC_WMVGetParam(UCHAR ucEsId, UINT32 u4Type, UINT32* pu4Param1, UINT32* pu4Param2, UINT32* pu4Param3)
{
    VDEC_WMV_INFO_T* prVdec;
    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);

    ASSERT(ucEsId < VDEC_MAX_ES);
    ASSERT(pu4Param1 !=NULL);
    ASSERT(pu4Param2 !=NULL);
    ASSERT(pu4Param3 !=NULL);

    if (u4Type == (UINT32)WMV_TIME_PROFILE)
    {
        if ((prVdec->rPicLayer.u1FrameCodingMode == PROGRESSIVE || prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFRAME)
            || (prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFIELD && prVdec->rPicLayer.i4CurrentField != 0) )
        {
            *pu4Param1 = prVdec->rOther.u1FbgId;
            *pu4Param2 = prVdec->rOther.u4CurDecMs;       //hw dec
        }
        else
        {
            *pu4Param1 = 0xFF;
        }
    }
    else if (u4Type == (UINT32)WMV_TIME_PROFILE2)
    {
        switch (prVdec->rPicLayer.u1PicType)
        {
        case IVOP:
            *pu4Param2 = 2;
            break;
        case PVOP:
            *pu4Param2 = 0;
            break;
        case BVOP:
        case BIVOP:
            *pu4Param2 = 1;
            break;
        default:
            *pu4Param2 = 3;
            break;
        }
        if (prVdec->rPicLayer.u1FrameCodingMode == PROGRESSIVE || prVdec->rPicLayer.u1FrameCodingMode == INTERLACEFRAME)
        {
            *pu4Param3 = 1;
        }
        else
        {
            if (prVdec->rPicLayer.i4CurrentField == 0)
            {
                *pu4Param3 = 0;
            }
            else
            {
                *pu4Param3 = 1;
            }
        }
    }
    else if (u4Type == (UINT32)WMV_TIME_PROFILE3)
    {
        *pu4Param1 = 0;
        *pu4Param2 = 0;
        *pu4Param3 = prVdec->rOther.u4CurDramCycle;
    }
    else if (u4Type == (UINT32)WMV_DISP_IDX)
    {
    }
}
#endif

#if defined(VDEC_TIME_PROFILE) || defined(CC_MAPLE_CUST_DRV)
BOOL _VDEC_WMVIsPic(UCHAR ucEsId, VDEC_PES_INFO_T *prPes)
{
    return TRUE;
}
#endif

BOOL _VDEC_IsSupportVc1(void)
{
#ifdef CC_WMV_EMULATION
    return TRUE;
#else
    return IS_SupportVC1();
#endif
}

void _VDEC_WMVUpdDmxRptr(UCHAR ucEsId)
{
    VDEC_WMV_INFO_T* prVdec;
    //UINT32 u4CurRptr;
    //UINT32 u4CurRptrBits;

    
    ASSERT(ucEsId < MAX_ES_NUM);

    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);
    
    //_VDEC_LockVld(ucEsId,0);  // megaa 20100730: power on/off patch

    //u4CurRptr = dwVldRPtr(ucEsId,&u4CurRptrBits);

    //_VDEC_UnlockVld(ucEsId,0);  // megaa 20100730: power on/off patch

    if (prVdec->rPesInfo.u1SliceCount || prVdec->rPesInfo.fgNotUpdateDmxRp)
    {
        return;
    }
    /*u4CurRptr -= (WPTR_OFFSET + RPTR_ALIGN);

    u4CurRptr += prVdec->rPesInfo.u4FifoStart;

    if(u4CurRptr < prVdec->rPesInfo.u4FifoStart)
    {
        u4CurRptr += (prVdec->rPesInfo.u4FifoEnd - prVdec->rPesInfo.u4FifoStart);
    }
    */
    //u4CurRptr += prVdec->rPesInfo.u4FifoStart;
    //LOG(6,"current read pointer : %x %x fifo start : %x\n",(u4CurRptr-prVdec->rPesInfo.u4FifoStart),u4CurRptr,prVdec->rPesInfo.u4FifoStart);

    //update dmx pointer
    //LOG(0,"UpdDmxRptr lock[%d]\n",ucEsId);
    //_VDEC_LockVld(ucEsId,0);//maiyou1
#ifndef CC_WMV_EMULATION
    if (!VDEC_SetRptr(prVdec->rPesInfo.ucEsId, prVdec->rPesInfo.u4VldReadPtr, prVdec->rPesInfo.u4VldReadPtr))
        //if (!VDEC_SetRptr(0, (u4CurRptr) , (u4CurRptr)))
    {
        LOG(0, "WMV ES(%d) VDEC_SetRptr Fail\n", prVdec->rPesInfo.ucEsId);
    }
#endif
    //LOG(0,"UpdDmxRptr unlock[%d]\n",ucEsId);
    //_VDEC_UnlockVld(ucEsId,0);//maiyou1
}

void _VDEC_WMVSetDmxWptr(UCHAR ucEsId, UINT32 u4Wptr)
{
    //VDEC_ES_INFO_KEEP_T *prVdecEsInfoKeep;
    VDEC_WMV_INFO_T* prVdec;
    
    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);

    ASSERT(ucEsId < MAX_ES_NUM);
    //LOG(0, "UpdWptr 0x%x\n", u4Wptr);
    //prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(0);

    /*if (!prVdecEsInfoKeep->fgDmxPartialPes)
    {
        prVdec->rOther.u4WPtr = (u4Wptr + 0xf) & (~0xf);
        prVdec->rOther.u4WPtr += WPTR_OFFSET;
        //LOG(5,"W: 0x%x\n", prEsInfo->u4VldWptr);
    }
    else
    {
        prVdec->rOther.u4WPtr = (u4Wptr & (~0xf));
    }*/

    prVdec->rOther.u4WPtr = (u4Wptr & (~0xf));
    //LOG(0,"SetDmxWptr lock[%d]\n",ucEsId);
    //_VDEC_LockVld(ucEsId,0);//maiyou1
    
    //_VDEC_UpdateVldWMVWPtr(ucEsId,prVdec->rOther.u4WPtr);

    //LOG(0,"SetDmxWptr unlock[%d]\n",ucEsId);ucEsId
    //_VDEC_UnlockVld(ucEsId,0);//maiyou1
}


//skip ES if return TRUE
BOOL _VDEC_WMVEsFlwCtrl(UCHAR ucEsId, UCHAR* pucVldId, INT8 i1EsBufStatus, UCHAR ucSkipMode)
{
#ifndef VDEC_COMMON_ISR
#ifndef AVC_IRQ_DISABLE
    x_os_isr_fct pfnOldIsr;
#endif
#endif
    VDEC_ES_INFO_T *prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
    UINT8 u1PicType = 0;
    UCHAR uFbId;
    //static UINT32 u4IframeCount =0;
    //static UINT32 u4BIframeCount =0;
    VDEC_WMV_INFO_T* prVdec;
    UINT8 u1FrameCodingMode = 0;
    
    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);

    UNUSED(u1PicType);
    ASSERT(ucEsId < MAX_ES_NUM);
    ASSERT(pucVldId !=NULL);
    ASSERT(prVdecEsInfo != NULL);

    //for trick mode
    /*if(prVdec->rPesInfo.fgGop)
    {
        vSetFRefBuf(ucEsId,0);
        vSetBRefBuf(ucEsId,1);
    }
    */
    if(prVdecEsInfo==NULL || prVdec==NULL)
    {
       return TRUE;
    }

    if (prVdecEsInfo->fgSendedEos)
    {
        LOG(6, "SendedEos already\n");
        *pucVldId = VDEC_MAX_VLD;
        return TRUE;
    }

    prVdec->rOther.i4SetPos = (prVdec->rPesInfo.u4VldReadPtr - prVdec->rPesInfo.u4FifoStart);
    prVdec->rDram.pu1VFifo = (void*)prVdec->rPesInfo.u4FifoStart;
    prVdec->rDram.pu1FifoEnd = (void*)prVdec->rPesInfo.u4FifoEnd;

#ifndef CC_WMV_EMULATION
    if (prVdecEsInfo != NULL && prVdec->rPesInfo.u4VideoType != DMX_VIDEO_VC1)
        //if(prVdecEsInfo != NULL)
    {
        prVdec->rSeqHdrR.u4PicWidthSrc =  (UINT32)prVdecEsInfo->rMMSeqInfo.u2_width;
        prVdec->rSeqHdrR.u4PicHeightSrc = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_height;
        prVdec->rOther.i4SeqHdrDataLen = (UINT32)prVdecEsInfo->rMMSeqInfo.u2SequenceHeaderSize;
        prVdec->rOther.i4SeqHdrData1 = (UINT32)prVdecEsInfo->rMMSeqInfo.u4SequenceHeader;
    }
#else
    // vparser use _VdecVParserWMV to set decoder parameters
    if (prVdec->rPesInfo.ucPicType == 0xff &&prVdec->rPesInfo.fgEos)
    {
        return FALSE;
    }
#endif
    //LOG(0,">>>>>>>The WMV3 SequenceHeader is 0x%x\n",prVdec->rOther.i4SeqHdrData1);

    if (prVdec->rPesInfo.u4VideoType == DMX_VIDEO_VC1)
    {
        prVdec->rSeqLayer.fgVC1 = TRUE;
        prVdec->rOther.i4CodecVersion = WMVA;
    }
    else if (prVdec->rPesInfo.u4VideoType == DMX_VIDEO_WMV7)
    {
        prVdec->rSeqLayer.fgVC1 = FALSE;
        prVdec->rOther.i4CodecVersion = WMV1;
    }
    else if (prVdec->rPesInfo.u4VideoType == DMX_VIDEO_WMV8)
    {
        prVdec->rSeqLayer.fgVC1 = FALSE;
        prVdec->rOther.i4CodecVersion = WMV2;
    }
    else if (prVdec->rPesInfo.u4VideoType == DMX_VIDEO_WMV9)
    {
        prVdec->rSeqLayer.fgVC1 = FALSE;
        prVdec->rOther.i4CodecVersion = WMV3;
    }
    else
    {
        ASSERT(0);
        prVdec->rSeqLayer.fgVC1 = FALSE;
        prVdec->rOther.i4CodecVersion = UNKNOW_TYPE;
    }

    /* Check PTS and calculate framerate to be compared with the parsed framerate.  If the
       difference between them is larger than a threshold, turn on the fgVC1NoCalPts flag
       so that the original PTS passed from demux module will be used. */
    if ((prVdec->rPesInfo.u4VideoType == DMX_VIDEO_VC1) && (prVdec->rPesInfo.u4PTS != 0) && prVdec->rPesInfo.fgDtsValid)
    {
        UINT32 u4idx = prVdec->rOther.u4PtsIdx;
        UINT32 u4DeltPTS = 0;

        if (u4idx == 0)
        {
            prVdec->rOther.u4VC1Pts_0 = prVdec->rPesInfo.u4PTS;
        }
        if (u4idx == VC1_REC_PTS_CNT)
        {
            prVdec->rOther.u4VC1Pts_n = prVdec->rPesInfo.u4PTS;
            
            LOG(3,"The %dth frame not 0 PTS is %d,   The first frame not 0 PTS is %d\n",
                VC1_REC_PTS_CNT, prVdec->rOther.u4VC1Pts_n, prVdec->rOther.u4VC1Pts_0);
            u4DeltPTS = (prVdec->rOther.u4VC1Pts_n - prVdec->rOther.u4VC1Pts_0) / VC1_REC_PTS_CNT;
            LOG(3,"According the framerate cal the delpts is %d,the vdec cal the delpts is %d\n",
                prVdec->rOther.u4DeltaPTS, u4DeltPTS);
            if (u4DeltPTS && 
                (VDEC_ABS(prVdec->rOther.u4DeltaPTS - u4DeltPTS) > VC1_FRMRATE_DIF_THRESHOLD) && 
                !prVdec->rOther.fgNoFrameRate)
            {
                prVdec->rOther.fgVC1NoCalPts = TRUE;
                LOG(3,"!!!!!! VC1 seq cal wrong frame rate\n");
            }
        }
        if (u4idx <= VC1_REC_PTS_CNT)
        prVdec->rOther.u4PtsIdx++;
    }

    //LOG(0,"EsFlwCtrl lock[%d]\n",ucEsId);
    _VDEC_LockVld(ucEsId,0);//maiyou1
    
    if (prVdec->rOther.fgFirstFrame) //init state
    {
        LOG(6,"hello, it's first frame\n");
        
        //_VDEC_LockVld(ucEsId,0);//maiyou1
        
        _WMVInit(ucEsId);
        vInitVParser(ucEsId);
        vInitVParserWMV(ucEsId);
        vPowerOnVDec();

#ifndef AVC_IRQ_DISABLE
        // reg ISR
        #ifdef VDEC_COMMON_ISR
        _VDEC_RegIsr(WMVVLD_VECTOR, _WMVVLDIsr);
        #else
        if (x_reg_isr(WMVVLD_VECTOR, _WMVVLDIsr, &pfnOldIsr) != OSR_OK)
        {
            ASSERT(0);
        }
        #endif
        if (hVdecWMVFinSema == 0)
        {
            VERIFY(x_sema_create(&hVdecWMVFinSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
        }
        //vIrqOn(INT_VLD);
#endif

        prVdec->rOther.fgDumpSimPat = FALSE;
        prVdec->rOther.u4BPRawFlag = 0;
		if(((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos) > ((UINT32)prVdec->rDram.pu1FifoEnd))
        {
        	fgVldInitBarrelShifter(ucEsId, (((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos) - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
        }
		else
		{
			fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos);
		}
        //fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos);
        prVdec->rOther.u4Datain = dwGetBitStream(ucEsId,0);
        if (prVdec->rOther.i4CodecVersion != WMVA)
        {
            _getESInfo(ucEsId);
            if (prVdec->rOther.i4CodecVersion == WMV3)
            {
                SetupMultiResParams(ucEsId);
                prVdec->rOther.i4VprErr = decodeVOLHead_WMV3(ucEsId);
            }
            else if ((prVdec->rOther.i4CodecVersion == WMV1) || (prVdec->rOther.i4CodecVersion == WMV2))
            {
                prVdec->rOther.i4VprErr = decodeVOLHead_WMV12(ucEsId);
                //prVdec->rPicLayer.u1PicType = dwGetHi(ucEsId, iNumBitsFrameType);
                //u1PicType = prVdec->rPicLayer.u1PicType;
            }
            vInitVdoMem(ucEsId);
        }
        prVdec->rOther.fgFirstFrame = FALSE;
    }
    else
    {
#ifdef WMV_MINST
        vRestoreVDec(ucEsId, FALSE);
#endif
    }

	if(prVdec->rPesInfo.u2SkipVidCount>0)
	{
        prVdec->rPicLayer.u1PicType=SKIPFRAME;
		_VDEC_UnlockVld(ucEsId,0);//maiyou1
		if(ucSkipMode == IMode)
        {
		   return TRUE;
        }
        else
        {
           return FALSE;
        }
    }

    if (prVdec->rOther.i4CodecVersion == WMV3)
    {
        if(((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos) > ((UINT32)prVdec->rDram.pu1FifoEnd))
        {
        	fgVldInitBarrelShifter(ucEsId, (((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos) - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
        }
		else
		{
			fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos);
		}
        
        if (prVdec->rSeqLayer.fgSeqFrameInterpolation)
        {
            dwGetHi(ucEsId, 1);
        }
        dwGetHi(ucEsId, 2);

        if (prVdec->rSMProfile.fgPreProcRange)
        {
            prVdec->rEntryPoint.i4RangeState = dwGetHi(ucEsId, 1);
        }

        prVdec->rPicLayer.u1PrevPicType = prVdec->rPicLayer.u1PicType;
        // Picture coding type
        if (1 == dwGetHi(ucEsId, 1))
            prVdec->rPicLayer.u1PicType = PVOP;
        else
        {
            if (prVdec->rSMProfile.i4NumBFrames == 0)
            {
                prVdec->rPicLayer.u1PicType = IVOP;
            }
            else
            {
                if (dwGetHi(ucEsId, 1) == 1)
                    prVdec->rPicLayer.u1PicType = IVOP;
                else
                    prVdec->rPicLayer.u1PicType = BVOP;
            }
        }
        u1PicType = prVdec->rPicLayer.u1PicType;
    }
    else if ((prVdec->rOther.i4CodecVersion == WMV1) || (prVdec->rOther.i4CodecVersion == WMV2))
    {
        int iNumBitsFrameType;
		if(((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos) > ((UINT32)prVdec->rDram.pu1FifoEnd))
        {
        	fgVldInitBarrelShifter(ucEsId, (((UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos) - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
        }
		else
		{
			fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos);
		}
       // fgVldInitBarrelShifter(ucEsId,(UINT32)prVdec->rDram.pu1VFifo + prVdec->rOther.i4SetPos);
        prVdec->rOther.u4Datain = dwGetBitStream(ucEsId,0);
        iNumBitsFrameType = (prVdec->rOther.i4CodecVersion == WMV2) ? 1 : 2;
        prVdec->rPicLayer.u1PrevPicType = prVdec->rPicLayer.u1PicType;
        prVdec->rPicLayer.u1PicType = dwGetHi(ucEsId, iNumBitsFrameType);
        u1PicType = prVdec->rPicLayer.u1PicType;
    }
    else if (prVdec->rOther.i4CodecVersion == WMVA)
    {
        UINT32 u4StartCode;
        UINT32 u4Addr;
        UINT8* pucBuf;
        u4Addr = (UINT32)prVdec->rPesInfo.au1PicInfo;
        pucBuf = (UCHAR*)u4Addr;
        u4StartCode = pucBuf[0];
        u4StartCode |= 0x100;
        switch (u4StartCode)
        {
        case WMV_SC_SEQ:
            prVdec->rOther.u1CountingIPFrame = 0;
            break;
        case WMV_SC_FRAME:
        case WMV_SC_FIELD:
#ifdef USE_FBM
            if (prVdec->rOther.u1FbgId == FBM_FBG_ID_UNKNOWN)
            {
                //LOG(0,"EsFlwCtrl 1 unlock[%d]\n",ucEsId);
#ifdef WMV_MINST
                vSaveVDec(ucEsId);
#endif
                _VDEC_UnlockVld(ucEsId,0);//maiyou1
                //vCheckUnlockVld(ucEsId, fgFirstFrame);
                return TRUE;
            }
#endif
			
            //fgVldInitBarrelShifter(ucEsId,prVdec->rPesInfo.u4VldReadPtr+4);
        if((prVdec->rPesInfo.u4VldReadPtr + 4) > ((UINT32)prVdec->rDram.pu1FifoEnd))
        {
        	fgVldInitBarrelShifter(ucEsId, ((prVdec->rPesInfo.u4VldReadPtr + 4) - ((UINT32)prVdec->rDram.pu1FifoEnd) + ((UINT32)prVdec->rDram.pu1VFifo)));
        }
		else
		{
			fgVldInitBarrelShifter(ucEsId,prVdec->rPesInfo.u4VldReadPtr+4);
		}
            prVdec->rOther.u4Datain = dwGetBitStream(ucEsId,0);
            reset_pic_hdr_bits(ucEsId);
            if (prVdec->rPicLayer.i4CurrentTemporalField == 1)
            {
                u1FrameCodingMode = INTERLACEFIELD;
            }
            else
            {
                if (prVdec->rSeqLayer.fgInterlacedSource)
                {
                    if (0 == dwGetHi(ucEsId, 1))
                    {
                        u1FrameCodingMode = PROGRESSIVE;
                    }
                    else
                    {
                        if (0 == dwGetHi(ucEsId, 1))
                        {
                            u1FrameCodingMode = INTERLACEFRAME;
                        }
                        else
                        {
                            u1FrameCodingMode = INTERLACEFIELD;
                        }
                    }
                }
                else
                {
                    u1FrameCodingMode = PROGRESSIVE;
                }
            }
            if (u1FrameCodingMode == PROGRESSIVE || u1FrameCodingMode == INTERLACEFRAME)
            {
                if (0 == dwGetHi(ucEsId, 1))
                    u1PicType = PVOP;
                else if (0 == dwGetHi(ucEsId, 1))
                    u1PicType = BVOP;
                else if (0 == dwGetHi(ucEsId, 1))
                    u1PicType = IVOP;
                else if (0 == dwGetHi(ucEsId, 1))
                    u1PicType = BIVOP;
                else
                    u1PicType = SKIPFRAME;
            }
            else if (u1FrameCodingMode == INTERLACEFIELD)
            {
                if (prVdec->rPicLayer.i4CurrentTemporalField == 0)
                {
                    UINT32 u4Code = dwGetHi(ucEsId, 3);
                    u1PicType = _u1vopFirstFieldType[u4Code];
                    prVdec->rOther.u1SecondFieldType = _u1vopSecondFieldType[u4Code];
                    prVdec->rPicLayer.u1PrevPicType = u1PicType;
                }
                else
                {
                    u1PicType = prVdec->rOther.u1SecondFieldType;
                }
            }
            // disable fgVC1NoCalPts mechanism for non progressive source
            if (u1FrameCodingMode != PROGRESSIVE)
            {
                prVdec->rOther.fgVC1NoCalPts = FALSE;
                prVdec->rOther.u4PtsIdx = 0xFFFFFFFF;
            }
            break;
        default:
            //LOG(0,"EsFlwCtrl 2 unlock[%d]\n",ucEsId);
#ifdef WMV_MINST
            vSaveVDec(ucEsId);
#endif
            _VDEC_UnlockVld(ucEsId,0);//maiyou1
            //vCheckUnlockVld(ucEsId, fgFirstFrame);
            return FALSE;
            //break;
        }
#if 1
        if (u4StartCode == WMV_SC_FRAME || u4StartCode==WMV_SC_FIELD)
        {
            if ((u1PicType ==IVOP || u1PicType==PVOP) && !prVdec->rEntryPoint.fgClosedEntryPoint)
            {
                if (prVdec->rOther.u1CountingIPFrame < 2)
                {
                    prVdec->rOther.u1CountingIPFrame++;
                    if (prVdec->rOther.u1CountingIPFrame == 2)
                    {
                        prVdec->rPicLayer.i4TemporalRef = 0;
#if 0  // do not overwrite the parsed flag for DTV00310840
                        prVdec->rEntryPoint.fgClosedEntryPoint = TRUE;
#endif
                    }
                }
            }
            if (!prVdec->rEntryPoint.fgClosedEntryPoint && u1PicType ==BVOP && prVdec->rOther.fgChangeResolution && prVdec->rOther.u1CountingIPFrame < 2)
            {
                prVdec->rOther.fgSkipFrame = TRUE;
                //u1PicType = SKIPFRAME;
                //vCheckUnlockVld(ucEsId, fgFirstFrame);
                //return TRUE;
            }
        }
#endif
    }



    if ((prVdecEsInfo != NULL) && IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I))
    {
        if (prVdecEsInfo->ucFbgId != FBM_FBG_ID_UNKNOWN)
        {
            FBM_InvalidateRefFrameBuffer(prVdecEsInfo->ucFbgId);
        }
        prVdec->rOther.u4NextIPTS = 0xFFFFFFFF;  // set it to the largest possible value
        CLR_DECFLAG(prVdecEsInfo, DEC_FLG_WAIT_NEXT_I);

        /*if (IS_DECFLAG_SET(prVdecEsInfo, DEC_FLG_2ND_FLD_PIC))
        {
            TOGGLE_DECFLAG(prVdecEsInfo, DEC_FLG_2ND_FLD_PIC);
        }*/
    }

    // DTV00310840
    #ifdef ENABLE_MULTIMEDIA
    if (prVdecEsInfo != NULL&&prVdec->rOther.i4CodecVersion == WMVA && u1PicType == BVOP &&prVdecEsInfo->eContainerType!=SWDMX_FMT_VC1_ES)
	#endif
    {
        UCHAR ucFRefFbId;
        UCHAR ucBRefFbId;
        BOOL fgSkip = FALSE;

        if (prVdec->rOther.u1FbgId != FBM_FBG_ID_UNKNOWN)
        {
            FBM_GetRefFrameBuffer(prVdec->rOther.u1FbgId, &ucFRefFbId, &ucBRefFbId);
            if (prVdec->rEntryPoint.fgClosedEntryPoint)
            {
                if (ucBRefFbId == FBM_FB_ID_UNKNOWN)
                {
                    fgSkip = TRUE;
                }
            }
            else
            {
                if (ucBRefFbId == FBM_FB_ID_UNKNOWN || ucFRefFbId == FBM_FB_ID_UNKNOWN)
                {
                    fgSkip = TRUE;
                }
            }
        }
        
        if (fgSkip)
        {
#ifdef WMV_MINST
            vSaveVDec(ucEsId);
#endif
            _VDEC_UnlockVld(ucEsId,0);//maiyou1
            
            return TRUE;
        }
    }
    #ifdef ENABLE_MULTIMEDIA
	if(prVdecEsInfo != NULL&&prVdecEsInfo->eContainerType==SWDMX_FMT_VC1_ES)
	#endif
	{
		if(prVdec->rOther.u4CurrPicNo==0)
		{
		   prVdec->rOther.u1LastPlayMode=ucSkipMode;
		}
		else
		{
		   if(ucSkipMode!=prVdec->rOther.u1LastPlayMode)
           {
		      prVdec->rOther.u4LastPTS=STC_GetStcValue(prVdecEsInfo->ucStcId);
           }
		}
	}

	prVdec->rOther.u1LastPlayMode=ucSkipMode;
	
    if (ucSkipMode == IMode)
    {
        //LOG(0,"I mode\n");

        //LOG(0,"EsFlwCtrl 3,4 unlock[%d]\n",ucEsId);
#ifdef WMV_MINST
        vSaveVDec(ucEsId);
#endif
        _VDEC_UnlockVld(ucEsId,0);//maiyou1
        //vCheckUnlockVld(ucEsId, fgFirstFrame);

        if(u1FrameCodingMode == INTERLACEFIELD && prVdec->rPicLayer.i4CurrentTemporalField == 1)
        {
            //do nothing.
        }
        else
        {
            uFbId=FBM_GetDecFrameBuffer(prVdecEsInfo->ucFbgId);
            if(uFbId!=FBM_FB_ID_UNKNOWN)
            {
               vWmvPrntFbmStatus(prVdecEsInfo->ucFbgId,uFbId,FBM_FB_STATUS_EMPTY,__LINE__);
               FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId,uFbId,FBM_FB_STATUS_EMPTY);
            }
        }


        if(prVdec->rOther.i4CodecVersion == WMVA 
            && u1FrameCodingMode == INTERLACEFIELD 
            &&prVdec->rPicLayer.i4CurrentTemporalField != 0 
            && prVdec->rPicLayer.u1PrevPicType==IVOP 
            && u1PicType==PVOP)
        {
            return FALSE;
        }
        else if (u1PicType!=IVOP)
        {
            // Update DMX read ptr when drop frame
            // If common part have handle it, remove it from here.
            _VDEC_WMVUpdDmxRptr(ucEsId);
            
            return TRUE;
        }
        else
        {
            //LOG(0,"I frame\n");
            return FALSE;
        }
    }
    else if (ucSkipMode == IPMode)
    {
        //LOG(0,"IP mode\n");
        
        //LOG(0,"EsFlwCtrl 5,6 unlock[%d]\n",ucEsId);
#ifdef WMV_MINST
        vSaveVDec(ucEsId);
#endif
        _VDEC_UnlockVld(ucEsId,0);//maiyou1
        //vCheckUnlockVld(ucEsId, fgFirstFrame);
        
        if (u1PicType == BVOP || u1PicType == BIVOP)
        {
            uFbId=FBM_GetDecFrameBuffer(prVdecEsInfo->ucFbgId);
            if(uFbId!=FBM_FB_ID_UNKNOWN)
            {
               vWmvPrntFbmStatus(prVdecEsInfo->ucFbgId,uFbId,FBM_FB_STATUS_EMPTY,__LINE__);
               FBM_SetFrameBufferStatus(prVdecEsInfo->ucFbgId,uFbId,FBM_FB_STATUS_EMPTY);
            }
            if (prVdec->rSeqLayer.fgInterlacedSource)  // DTV00084284
            {
                if (prVdec->rOther.i4CodecVersion == WMVA && u1FrameCodingMode == INTERLACEFIELD)
                {
                    prVdec->rPicLayer.i4CurrentTemporalField ^= 1;  //toggle field
                    prVdec->rPicLayer.i4CurrentField ^= 1;
                }
            }

            // Update DMX read ptr when drop frame
            // If common part have handle it, remove it from here.
            _VDEC_WMVUpdDmxRptr(ucEsId);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    
    //LOG(0,"EsFlwCtrl 7 unlock[%d]\n",ucEsId);
#ifdef WMV_MINST
    vSaveVDec(ucEsId);
#endif
    _VDEC_UnlockVld(ucEsId,0);//maiyou1
    //vCheckUnlockVld(ucEsId, fgFirstFrame);
    
    return FALSE;
}

VOID _VDEC_WMVStartPTS(UCHAR ucEsId, BOOL fgEnCalPTS, UINT32 u4Pts, UINT32 u4PtsDiff)
{
    VDEC_WMV_INFO_T* prVdec;

    ASSERT(ucEsId < MAX_ES_NUM);
    prVdec = (VDEC_WMV_INFO_T*)_VDEC_WMVGetHdlr(ucEsId);

    prVdec->rOther.fgEnCalPTS= fgEnCalPTS;
    prVdec->rOther.fgFirstDispFrame = fgEnCalPTS;
    if (fgEnCalPTS)
    {
        //prVdec->rOther.u4LastPTS = u4Pts;
        //prVdec->rOther.u4DeltaPTS= u4PtsDiff;
    }
}

VOID _VDEC_WMVSetIsr(UCHAR ucEsId)
{
    #ifndef VDEC_COMMON_ISR
    x_os_isr_fct pfnOldIsr;
    #endif

    if(ucEsId<MAX_ES_NUM)
    {
        ucVLDISREsId = ucEsId;  // megaa 20100326
    }
#ifndef AVC_IRQ_DISABLE
    // reg ISR
    #ifdef VDEC_COMMON_ISR
    _VDEC_RegIsr(WMVVLD_VECTOR, _WMVVLDIsr);
    #else
    if (x_reg_isr(WMVVLD_VECTOR, _WMVVLDIsr, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    }
    #endif
    if (hVdecWMVFinSema == 0)
    {
        VERIFY(x_sema_create(&hVdecWMVFinSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
    }
#else
    #ifdef VDEC_COMMON_ISR
    _VDEC_RegIsr(WMVVLD_VECTOR, _VDEC_FakeVldIsr);
    #else
    if (x_reg_isr(WMVVLD_VECTOR, NULL, &pfnOldIsr) != OSR_OK)
    {
        ASSERT(0);
    }
    #endif
#endif
    UNUSED(ucEsId);
}

VOID vWmvPrntFbmStatus(UINT8 u1FbgId, UCHAR ucFbId, UINT8 u1NewState, UINT32 u4Line)
{
#ifdef WMV_PRINT_FBM_STATUS
    UCHAR u1state;
    u1state = FBM_GetFrameBufferStatus(u1FbgId,ucFbId);
    LOG(0,"WMV set FBM status : %d %d to %d (#%d)\n",ucFbId,u1state,u1NewState, u4Line);
#else
    UNUSED(u1FbgId);
    UNUSED(ucFbId);
    UNUSED(u1NewState);
    UNUSED(u4Line);
#endif
}

#ifdef CC_SUPPORT_VDEC_PREPARSE
INT32 _VDEC_WmvPrePrsHdr(UCHAR ucMpvId, UCHAR ucEsId)
{
   VDEC_ES_INFO_T* prVdecEsInfo;
   VDEC_ES_INFO_KEEP_T* prVdecEsInfoKeep;
   UINT32 u4Addr;
   UCHAR *pucBuf;
   UINT32 u4StartCode;
   UINT32 u4Temp;
   INT32  i4RetValue = E_VDEC_OK;
   BOOL fgProgressiveSeq,fgInterlacedSource;

   VDEC_WMV_INFO_T* prVdec = (VDEC_WMV_INFO_T*)vWMVGetHdlr(ucEsId);
   prVdecEsInfo = _VDEC_GetEsInfo(ucEsId);
   prVdecEsInfoKeep = _VDEC_GetEsInfoKeep(ucEsId);

   if (prVdecEsInfo != NULL && prVdec->rPesInfo.u4VideoType != DMX_VIDEO_VC1 && prVdecEsInfo->fgMMSeqInfoValid)
   {
       prVdecEsInfo->u2OrgHSize =  (UINT32)prVdecEsInfo->rMMSeqInfo.u2_width;
       prVdecEsInfo->u2OrgVSize = (UINT32)prVdecEsInfo->rMMSeqInfo.u2_height;
       prVdecEsInfo->fgProgressive = TRUE;
   }
   else
   {
#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
       u4Addr = (UINT32)prVdecEsInfoKeep->au1SeqInfo;
#else
       u4Addr = (UINT32)prVdecEsInfoKeep->rPesInfo.au1PicInfo;
#endif
       pucBuf = (UCHAR*)u4Addr;

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
	   u4StartCode = (pucBuf[0] << 24) | (pucBuf[1] << 16) | (pucBuf[2] << 8) | (pucBuf[3]);
       LOG(5, "u4StartCode: %02x %02x %02x %02x\n", pucBuf[0], pucBuf[1], pucBuf[2], pucBuf[3]);
#else
       u4StartCode = pucBuf[0];
       u4StartCode |= 0x100;
#endif
       if(WMV_SC_SEQ !=u4StartCode)
       {
           LOG(0,"WMV VC1 First DATA error %X!!!\n",u4StartCode);
           return E_VDEC_PARSE_NOT_READY;
       }

#if defined(CC_TRUSTZONE_SUPPORT) && defined(CC_SVP_SUPPORT)
       u4Addr +=4;//skip 00 00 01 0F
#else
       u4Addr =prVdec->rPesInfo.u4VldReadPtr + 4;//skip 00 00 01 0F
#endif
       u4Addr +=2;//skip first 16 bits
       pucBuf = (UCHAR*)VIRTUAL(u4Addr);

       u4Temp = (UINT32)pucBuf[0];
       u4Temp = (u4Temp << 4) | ((pucBuf[1] >> 4) &0x0f);
       prVdec->rSeqHdrR.u4MaxCodedWidth=u4Temp;

       u4Temp = (UINT32)((pucBuf[1] << 8)&0x0f00);
       u4Temp = u4Temp | pucBuf[2];
       prVdec->rSeqHdrR.u4MaxCodedHeight = u4Temp & 0xfff;//12bits

       LOG(5,"u4MaxCodedWidth=%d\n",prVdec->rSeqHdrR.u4MaxCodedWidth );
       LOG(5,"u4MaxCodedHeight=%d\n",prVdec->rSeqHdrR.u4MaxCodedHeight );

       fgInterlacedSource = (pucBuf[3] & 0x40)>>6;
       fgProgressiveSeq = (pucBuf[3] & 0x4) >>2;

       if(fgProgressiveSeq)
        {
           prVdecEsInfo->fgProgressive = TRUE;//progressive
        }
       else
        {
           if (!fgInterlacedSource)
            {
                prVdecEsInfo->fgProgressive = TRUE;//progressive
            }
            else
            {
                prVdecEsInfo->fgProgressive = FALSE;//interlaced
            }
        }
       
       prVdecEsInfo->u2OrgHSize = 2* prVdec->rSeqHdrR.u4MaxCodedWidth + 2;
       prVdecEsInfo->u2OrgVSize = 2* prVdec->rSeqHdrR.u4MaxCodedHeight + 2;
   }

    switch(prVdecEsInfo->rMMSeqInfo.e_frame_rate)
    {
      case VDEC_SRC_FRAME_RATE_23_976: /* 24000/1001 (23.976...) */
          prVdecEsInfo->u4FrameRate = 2397;
          break;
      case VDEC_SRC_FRAME_RATE_24:
          prVdecEsInfo->u4FrameRate = 2400;
          break;
      case VDEC_SRC_FRAME_RATE_25:
          prVdecEsInfo->u4FrameRate = 2500;
          break;
      case VDEC_SRC_FRAME_RATE_29_97: /* 30000/1001 (29.97...) */
          prVdecEsInfo->u4FrameRate = 2997;
          break;
      case VDEC_SRC_FRAME_RATE_30:
          prVdecEsInfo->u4FrameRate = 3000;
          break;
      case VDEC_SRC_FRAME_RATE_50:
          prVdecEsInfo->u4FrameRate = 5000;
          break;
      case VDEC_SRC_FRAME_RATE_59_94: /* 60000/1001 (59.94...) */
          prVdecEsInfo->u4FrameRate = 5994;
          break;
      case VDEC_SRC_FRAME_RATE_60:
          prVdecEsInfo->u4FrameRate = 6000;
          break;
#if defined(SUPPORT_DISP_WEB_VIDEO_SMOOTHER)
      case VDEC_SRC_FRAME_RATE_12:
         prVdecEsInfo->u4FrameRate = 1200;
         break;
      case VDEC_SRC_FRAME_RATE_15:
         prVdecEsInfo->u4FrameRate = 1500;
         break;
       case VDEC_SRC_FRAME_RATE_20:
          prVdecEsInfo->u4FrameRate = 2000;
          break;
#endif
      default:
          prVdecEsInfo->u4FrameRate = 0;
          break;
    }
    return i4RetValue;
}
#endif
//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------



