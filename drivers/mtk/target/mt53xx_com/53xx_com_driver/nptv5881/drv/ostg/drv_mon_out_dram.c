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
 * $RCSfile: drv_scaler.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file drv_scaler.c
 *  Brief of file drv_scaler.c.
 *  Details of file drv_scaler.c (optional).
 */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_lint.h"
LINT_EXT_HEADER_BEGIN

#include "hw_mon_out.h"
#include "hw_scpos.h"
#include "drv_scpos.h"
#include "drv_scaler.h"
#include "drv_scaler_drvif.h"
#include "scpos_debug.h"
#include "fbm_drvif.h"
#include "srm_drvif.h"
#include "x_assert.h"
#include "drv_mon_out_predown.h"
#include "drv_mon_out.h"



LINT_EXT_HEADER_END

//static UINT32 _au4MonOutForceWriteOff = 0;
VDP_MONOUT_PRM_T _arMonOutPrm;

//static function
static SCALER_DRAM_INIT_STATE _eMonOutGetDramInitState(void);
static void _vMonOutSetDramInitState(SCALER_DRAM_INIT_STATE state);
static void _vMonOutUpdatePageNum(void);
static void _vMonOutToggleForceOff(void);
static void _vMonOutUpdateLinePitch(void);
void vDrvMonOutSrmUpdate(void);

#if 0
void vMonOutForceWriteOff(UINT8 u1OnOff) 
{
    CRIT_STATE_T csState;

    csState = x_crit_start();    
    _au4MonOutForceWriteOff = u1OnOff;
    x_crit_end(csState);   
}
#endif
void vMonOutWriteRelatchEn(UINT32 u4OnOff)
{
    UINT8 value;
    value = (u4OnOff==SV_ON)?1:0;

	SCPIP_WRITE_FLD(DRAM_MON_00, value, DRAM_MON_00_DA_WRITE_EN_RELATCH);	
}
void vMonOutSetDramLinePitch(UINT32 u4Pitch)
{
	SCPIP_WRITE_FLD(DRAM_MON_00, u4Pitch, DRAM_MON_00_DA_DRAM_LINE_PITCH);	
}

void vMonOutSetBufNum(UINT32 u4BufNum)
{
    if (u4BufNum < 1 || u4BufNum > 3)
        return;

    SCPIP_WRITE_FLD(DRAM_MON_00, u4BufNum-1, DRAM_MON_00_DA_FRAME_NUM);
}
UINT32 u4MonOutGetBufNum(void)
{
    UINT32 u4FrameNum;

    u4FrameNum = SCPIP_READ_FLD(DRAM_MON_00,DRAM_MON_00_DA_FRAME_NUM) + 1;

    return u4FrameNum;    
}

static SCALER_DRAM_INIT_STATE _eMonOutGetDramInitState(void)
{
    return _arMonOutPrm.eDramInitState;
}

static void _vMonOutSetDramInitState(SCALER_DRAM_INIT_STATE state)
{
    _arMonOutPrm.eDramInitState = state;
}

/**
 *	@remy porting 
 *  Turn off write enable immediately and set counter.
 *  Write enable will not be turned on before counter clear to 0.
 */
static void _vMonOutToggleForceOff(void)
{
	CRIT_STATE_T csState; 

    LOG(3, "_vMonOutToggleForceOff: Disable Write Relatch\n"); 
	    
    csState = x_crit_start(); 
    _arMonOutPrm.u4CntToRstDramPrm = (_arMonOutPrm.u4CntToRstDramPrm < 2)? 2: _arMonOutPrm.u4CntToRstDramPrm;

    // when turn write relatch ????
    vMonOutWriteRelatchEn(SV_OFF);
    x_crit_end(csState);
    _vMonOutWriteCtrlOnOff(SV_OFF);
}

/**
 * @remy porting
**/
static void _vMonOutSrmUpdateDramInfo(FBM_POOL_T* prPool)
{
    //update scaler dram info
    _arMonOutPrm.rFbmInfo.u4Addr = prPool->u4Addr;
    _arMonOutPrm.rFbmInfo.u4Width = prPool->u4Width;
    _arMonOutPrm.rFbmInfo.u4Height = prPool->u4Height;
    _arMonOutPrm.rFbmInfo.u4FbmSize = prPool->u4Size; 

    //FBM always 444 10b, need to review ???
    _arMonOutPrm.rFbmInfo.fg10bit = SV_FALSE;
    _arMonOutPrm.rFbmInfo.fg422 = SV_FALSE;

    LOG(3, "[SRM Callback]Set Monitor Out Buffer: addr=0x%x, w=%d, h=%d\n", 
            (UINT32)_arMonOutPrm.rFbmInfo.u4Addr, 
            (UINT32)_arMonOutPrm.rFbmInfo.u4Width, 
            (UINT32)_arMonOutPrm.rFbmInfo.u4Height);
}

/**
 *	@remy porting 
**/
static void _vMonOutUpdatePageNum(void)
{
    SCALER_DRAM_ADDR_T* dramAddrInfo;
    SCALER_FBM_POOL_T* fbmInfo;
    UINT32 u4FbmWidth;
    UINT32 u4FbmHeight;
    UINT32 u4FbTotalSize;
    UINT8 bIs10Bit;
    UINT8 bIs422;
    UINT32 u4FrameNum;
    UINT32 u4UsedPage;
    UINT32 u4Padding;
    
    dramAddrInfo = &_arMonOutPrm.rDramAddrInfo;
    fbmInfo = &_arMonOutPrm.rFbmInfo;    
    u4FbmWidth = fbmInfo->u4Width;
    u4FbmHeight = fbmInfo->u4Height;
    u4FrameNum = dramAddrInfo->FrameNum;    
    u4FbTotalSize = (fbmInfo->u4FbmSize) >> 11;  //total page number from FBM
    
    bIs10Bit = fbmInfo->fg10bit;
    bIs422 = fbmInfo->fg422;
    
    dramAddrInfo->MsbPageNum = ((u4FbmWidth + 255) / 256) * (((u4FbmHeight* u4FrameNum) + 7) / 8);
    if(bIs10Bit == SV_TRUE)
    {
        dramAddrInfo->LsbPageNum = (((u4FbmWidth + 255) / 256) * (((u4FbmHeight* u4FrameNum) + 255) / 256)) * 8;
    }
    else
    {
        dramAddrInfo->LsbPageNum = 0;
    }
    
    if(bIs10Bit == SV_TRUE && bIs422 == SV_TRUE)    //422 10 bit
    {
        u4UsedPage = (dramAddrInfo->MsbPageNum * 2) + (dramAddrInfo->LsbPageNum * 2);
        u4Padding  = 8;
    }
    else if(bIs10Bit == SV_FALSE && bIs422 == SV_TRUE)    //422 8 bit
    {
        u4UsedPage = (dramAddrInfo->MsbPageNum * 2);
        u4Padding  = 8;
    }
    else if(bIs10Bit == SV_TRUE && bIs422 == SV_FALSE)    //444 10 bit
    {
        u4UsedPage = (dramAddrInfo->MsbPageNum * 3) + (dramAddrInfo->LsbPageNum * 3);
        u4Padding  = 12;
    }
    else if(bIs10Bit == SV_FALSE && bIs422 == SV_FALSE)    //444 8 bit
    {
        u4UsedPage = (dramAddrInfo->MsbPageNum * 3);
        u4Padding  = 12;
    }
    else
    {
        u4UsedPage = 0;
        u4Padding  = 0;
    }

    LOG(1, "FBM width (%d) height (%d)\n", u4FbmWidth, fbmInfo->u4Height);
    LOG(1, "FBM allocated Page Num(%d) Used Page Num by Calculation(%d) Padding (%d)\n", u4FbTotalSize, u4UsedPage, u4Padding);
    LOG(1, "MSB Page Num(%d) x %d\n", dramAddrInfo->MsbPageNum, (bIs422==1)?2:3);
    if(bIs10Bit == SV_TRUE)
    {
        LOG(1, "LSB Page Num(%d) x %d\n", dramAddrInfo->LsbPageNum, (bIs422==1)?2:3);
    }
    if(u4FbTotalSize < u4UsedPage + u4Padding)
    {
        LOG(1, "FBM buffer to small!!!\n");
    }
}

/**
 *	@remy porting 
**/
static void _vMonOutCalBufferAddress(void)
{
    SCALER_DRAM_ADDR_T* dramAddrInfo;
    SCALER_FBM_POOL_T* fbmInfo;
    UINT32 u4MsbTotalPage;
    UINT32 u4LsbTotalPage;

    dramAddrInfo = &_arMonOutPrm.rDramAddrInfo;
    fbmInfo= &_arMonOutPrm.rFbmInfo;
    u4MsbTotalPage = dramAddrInfo->MsbPageNum;
    u4LsbTotalPage = dramAddrInfo->LsbPageNum;

    //1. Low bound
    dramAddrInfo->LowAddr = fbmInfo->u4Addr >> 11;
    //2. Y Msb
    dramAddrInfo->YMsbAddr = fbmInfo->u4Addr >> 11; 
    //3. Y Lsb = Y Msb + Msb Total Page
    dramAddrInfo->YLsbAddr = MAKE_ALIGN_TO((dramAddrInfo->YMsbAddr + u4MsbTotalPage), 3, (((dramAddrInfo->LowAddr)%8)+2)%8);
    //4. U Msb = Y Lsb + Lsb Total Page
    dramAddrInfo->UMsbAddr = MAKE_ALIGN_TO((dramAddrInfo->YLsbAddr + u4LsbTotalPage), 3, (((dramAddrInfo->LowAddr)%8)+4)%8);
    //5. U Lsb = U Msb + Msb Total Page
    dramAddrInfo->ULsbAddr = MAKE_ALIGN_TO((dramAddrInfo->UMsbAddr + u4MsbTotalPage), 3, (((dramAddrInfo->LowAddr)%8)+6)%8);
    //6. V Msb & V Lsb
    if(fbmInfo->fg422 == SV_FALSE)  //444
    {
        dramAddrInfo->VMsbAddr = MAKE_ALIGN_TO((dramAddrInfo->ULsbAddr + u4LsbTotalPage), 3, (((dramAddrInfo->LowAddr)%8)+0)%8);
        dramAddrInfo->VLsbAddr = MAKE_ALIGN_TO((dramAddrInfo->VMsbAddr + u4MsbTotalPage), 3, (((dramAddrInfo->LowAddr)%8)+2)%8);
    }
    else    //422
    {
        dramAddrInfo->VMsbAddr = dramAddrInfo->UMsbAddr;
        dramAddrInfo->VLsbAddr = dramAddrInfo->ULsbAddr;
    }
    //8. High bound
    //dramAddrInfo->HighAddr = MAKE_ALIGN_TO(((fbmInfo->u4Addr+fbmInfo->u4FbmSize)>>11), 3, (((dramAddrInfo->LowAddr)%8)+4)%8);
    dramAddrInfo->HighAddr = (fbmInfo->u4Addr + fbmInfo->u4FbmSize)>>11; // FIXME
    LOG(1,"Update Monitor Out Dram Info\n");
    LOG(1,"==========================\n");    
    LOG(1,"YMSB(0x%x) YLSB(0x%x)\n",dramAddrInfo->YMsbAddr, dramAddrInfo->YLsbAddr);
    LOG(1,"UMSB(0x%x) ULSB(0x%x)\n",dramAddrInfo->UMsbAddr, dramAddrInfo->ULsbAddr);
    LOG(1,"VMSB(0x%x) VLSB(0x%x)\n",dramAddrInfo->VMsbAddr, dramAddrInfo->VLsbAddr);
    LOG(1,"Low (0x%x) High(0x%x)\n",dramAddrInfo->LowAddr,  dramAddrInfo->HighAddr);    
    LOG(1,"==========================\n");    
}

/**
 *	@remy porting 
**/
static void _vMonOutUpdateFrameNumber(void)
{
    UINT32 u4FrameNum;
    
    // How about Mirror Flip
    //need to review ?
     u4FrameNum = 3;       

  _arMonOutPrm.rDramAddrInfo.FrameNum = u4FrameNum;
  
}

/**
 *	@remy porting 
**/
static void _vMonOutUpdateLinePitch(void)
{
    SCALER_FBM_POOL_T* fbmInfo;
    UINT16 width;
    MONOUTPREDOWN_INFO_T* monOutPdsInfo;
    fbmInfo= &_arMonOutPrm.rFbmInfo;
    monOutPdsInfo = getMonOutPDSInfo(); 

    if ((monOutPdsInfo->u4OutputWidth!= 0) && (fbmInfo->u4Width == 0))
    {
        width = 0;
        LOG(0,"fbm not no allocate dram to tve\n");
    }
    else
    {
        width = MIN(monOutPdsInfo->u4OutputWidth, fbmInfo->u4Width);
    }

    // need to review ??
    //calculate dram write picth  (in cobra, line pitch change to 256 align)
    _arMonOutPrm.u4DramLinePitch = DIVIDE_WITH_CARRIER((UINT16)width,256);
    LOG(1, "Line Pitch(%d) Width (%d)\n", _arMonOutPrm.u4DramLinePitch, width);
}

/**
 *	@remy porting 
**/
void vMonOutUpdateDramPrm(const SCALER_FBM_POOL_T* prFbmPool)
{
    
    //1. update frame number to write
    _vMonOutUpdateFrameNumber();
    //2. update Msb & Lsb Page Number
    _vMonOutUpdatePageNum();
    //3. calculate Dram base address & Low/High limit
    _vMonOutCalBufferAddress();
    //4. update line pitch
    _vMonOutUpdateLinePitch();
    
}
/**
 *	@remy porting 
**/
void vMonOutUpdateDramReg(void)
{
    SCALER_DRAM_ADDR_T* dramInfo;
        
    dramInfo = &_arMonOutPrm.rDramAddrInfo;
    
    if(dramInfo->YMsbAddr == SCPIP_READ_FLD(DRAM_MON_04, DRAM_MON_04_DA_ADDR_BASE_MSB_Y) &&
       dramInfo->YLsbAddr == SCPIP_READ_FLD(DRAM_MON_05, DRAM_MON_05_DA_ADDR_BASE_LSB_Y) &&
       dramInfo->UMsbAddr == SCPIP_READ_FLD(DRAM_MON_06, DRAM_MON_06_DA_ADDR_BASE_MSB_U) &&
       dramInfo->ULsbAddr == SCPIP_READ_FLD(DRAM_MON_07, DRAM_MON_07_DA_ADDR_BASE_LSB_U) &&
       dramInfo->VMsbAddr == SCPIP_READ_FLD(DRAM_MON_08, DRAM_MON_08_DA_ADDR_BASE_MSB_V) &&
       dramInfo->VLsbAddr == SCPIP_READ_FLD(DRAM_MON_09, DRAM_MON_09_DA_ADDR_BASE_LSB_V) &&
       dramInfo->HighAddr  == SCPIP_READ_FLD(DRAM_MON_0A, DRAM_MON_0A_DA_WADDR_HI_LIMIT) &&
       dramInfo->LowAddr == SCPIP_READ_FLD(DRAM_MON_0B, DRAM_MON_0B_DA_WADDR_LO_LIMIT) &&
       dramInfo->FrameNum == u4MonOutGetBufNum())
    {
        //all the same. just return
        return;
    }
 
    //1. turn off dram write & set counter
    _vMonOutToggleForceOff();

    //2. update base address
    SCPIP_WRITE_FLD(DRAM_MON_04, dramInfo->YMsbAddr , DRAM_MON_04_DA_ADDR_BASE_MSB_Y);
    SCPIP_WRITE_FLD(DRAM_MON_05, dramInfo->YLsbAddr , DRAM_MON_05_DA_ADDR_BASE_LSB_Y);
    SCPIP_WRITE_FLD(DRAM_MON_06, dramInfo->UMsbAddr , DRAM_MON_06_DA_ADDR_BASE_MSB_U);
    SCPIP_WRITE_FLD(DRAM_MON_07, dramInfo->ULsbAddr , DRAM_MON_07_DA_ADDR_BASE_LSB_U);
    SCPIP_WRITE_FLD(DRAM_MON_08, dramInfo->VMsbAddr , DRAM_MON_08_DA_ADDR_BASE_MSB_V);
    SCPIP_WRITE_FLD(DRAM_MON_09, dramInfo->VLsbAddr , DRAM_MON_09_DA_ADDR_BASE_LSB_V);
    SCPIP_WRITE_FLD(DRAM_MON_0A, dramInfo->HighAddr , DRAM_MON_0A_DA_WADDR_HI_LIMIT);
    SCPIP_WRITE_FLD(DRAM_MON_0B, dramInfo->LowAddr , DRAM_MON_0B_DA_WADDR_LO_LIMIT);
    //3. update frame number
    vMonOutSetBufNum(dramInfo->FrameNum);
    //4. set Dram line pitch
    vMonOutSetDramLinePitch(_arMonOutPrm.u4DramLinePitch);

    //5. set this means dram register has been inited.
    if(_eMonOutGetDramInitState() == SCALER_DRAM_INIT_STATE_SRM_INITED)
    {
        _vMonOutSetDramInitState(SCALER_DRAM_INIT_STATE_REG_INITED);
    }
}

/**
 *   @remy porting  
 *  monitor out  control call back
 *
 * @param void
 * @return void
 */
void vDrvMonOutSrmUpdate(void)
{
#ifdef CC_SCPOS_EN
    FBM_POOL_T* prPool;

    //_vMonOutWriteCtrlOnOff(SV_OFF);

    //1.get Scaler  Pool info, change FBM_POOL_TYPE_SCPOS_PIP to ....???
    prPool = FBM_GetPoolInfoAuto((UCHAR)FBM_POOL_TYPE_TVE, NULL);

    //2. pool is valid, update dram info.
    if (prPool != NULL)
    {
         _vMonOutSrmUpdateDramInfo(prPool);

        // need this ????
        if(_eMonOutGetDramInitState() == SCALER_DRAM_INIT_STATE_NOT_INIT)
        {
            _vMonOutSetDramInitState(SCALER_DRAM_INIT_STATE_SRM_INITED);
        }

        vMonOutUpdateDramPrm(&_arMonOutPrm.rFbmInfo);
        vMonOutUpdateDramReg();
    }
    else
    {
            LOG(0, "vDrvMonOutSrmDispCb: fail to get pool\n");
    }

    //3. fire scaler mode change ????

 
#endif
}


