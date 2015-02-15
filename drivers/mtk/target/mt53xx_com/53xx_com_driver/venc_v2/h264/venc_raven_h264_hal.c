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
 * $RCSfile: venc_raven_h264_hal.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file venc_raven_h264_hal.c
 *  
 */


//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "x_assert.h"
#include "x_os.h"
#include "x_bim.h"
#include "x_ckgen.h"
#include "hw_ckgen.h"
#include "x_debug.h"

#include "venc_raven_define.h"
#include "venc_raven_h264_hal.h"

//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define RAVEN_RESET 1

#define VENC_H264_VECTOR        VECTOR_VENC

#define VENC_INTR_SPSINT            (1)
#define VENC_INTR_PPSINT            (2)
#define VENC_INTR_FRMINT            (3)
#define VENC_INTR_FIFOFULL          (4)

//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------
static HANDLE_T    _hVencIsrEvent = 0;
static BOOL        _fgVencIsrReg = FALSE;
static BOOL        _fgVencFifoFull = FALSE;

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------
static INT32 i4Venc_Hal_CreateIsrEventGroup(void)
{
    CHAR  szBuf[16] = "VENCISR";

    INT32 i4Ret = x_ev_group_create(&_hVencIsrEvent, szBuf, 0);
    if (OSR_OK != i4Ret)
    {
        ASSERT(0);
    }

    return VENC_RAVEN_OK;
}

static INT32 i4Venc_Hal_DeleteIsrEventGroup(void)
{
    x_ev_group_delete(_hVencIsrEvent);

    return VENC_RAVEN_OK;
}

static INT32 i4Venc_Hal_SetIsrEvent(UINT32 u4SetEvent)
{
    INT32 i4Ret;
    
    LOG(7,"[raven] set event %d\n", u4SetEvent);
    
    i4Ret = x_ev_group_set_event(_hVencIsrEvent, u4SetEvent, X_EV_OP_OR);
    if (OSR_OK != i4Ret)
    {
        ASSERT(0);
    }

    return VENC_RAVEN_OK;
}

static INT32 i4Venc_Hal_WaitIsrEvent(UINT32 u4WaitEvent)
{
    INT32 i4Ret;
    UINT32 u4GetEvent = VENC_INVALID_DW;

    //printf("[raven] start wait event %d\n", u4WaitEvent);
    
    i4Ret = x_ev_group_wait_event(_hVencIsrEvent, u4WaitEvent, &u4GetEvent, X_EV_OP_OR_CONSUME);
    if (OSR_OK != i4Ret)
    {
        ASSERT(0);
    }

    //printf("[raven] end wait event %d\n", u4WaitEvent);

    return VENC_RAVEN_OK;
}

VOID vVenc_Hal_IsrHandle(UINT16 u2Vector)
{
    UINT32 u4VencIsrValue = 0;
    UINT32 u4SetEvent = 0;

    LOG(7,"[raven] ISR %d\n", u2Vector);
           
    if (VENC_H264_VECTOR != u2Vector)
    {
        return ;
    }

    u4VencIsrValue = i4Venc_Hw_VENC_IRQ_STATUS();
    i4Venc_Hw_VENC_IRQ_ACK(u4VencIsrValue);
        
    VERIFY(BIM_ClearIrq(u2Vector));

    if (0 == u4VencIsrValue)
    {
        ASSERT(0);
    }

    if (0x10 & u4VencIsrValue)
    {
        ASSERT(0);  //pause Isr
    }
    else if (0x8 & u4VencIsrValue)
    {
        u4SetEvent = VENC_INTR_FIFOFULL;
        _fgVencFifoFull = TRUE;
    }
    else if (0x4 & u4VencIsrValue)
    {
        u4SetEvent = VENC_INTR_FRMINT;
    }
    else if (0x2 & u4VencIsrValue)
    {
        u4SetEvent = VENC_INTR_PPSINT;
    }
    else if (0x1 & u4VencIsrValue)
    {
        u4SetEvent = VENC_INTR_SPSINT;
    }
    else
    {
        LOG(7,"[raven] u4VencIsrValue 0x%x, ASSERT ERROR\n", u4VencIsrValue);
        //ASSERT(0);
        while(1);
    }

    i4Venc_Hal_SetIsrEvent(u4SetEvent);
}

static INT32 i4Venc_Hal_RegIsr(void)
{
    x_os_isr_fct pfnOldIsr;

    if (FALSE == _fgVencIsrReg)
    {
        //printf("[raven] VENC_H264_VECTOR = %d\n",VENC_H264_VECTOR);
        if (OSR_OK != x_reg_isr(VENC_H264_VECTOR, vVenc_Hal_IsrHandle, &pfnOldIsr))
        {
            ASSERT(0);
        }
        //printf("[raven] Creat ISR OK\n");
        _fgVencIsrReg = TRUE;
    }

    return VENC_RAVEN_OK;
}

static INT32 i4Venc_Hal_UnregIsr(void)
{
#if 0
    x_os_isr_fct pfnOldIsr;
    
    if (TRUE == _fgVencIsrReg)
    {
        if (OSR_OK != x_reg_isr(VENC_H264_VECTOR, NULL, &pfnOldIsr))
        {
            ASSERT(0);
        }
    }
    
    _fgVencIsrReg = FALSE;
#endif
    return VENC_RAVEN_OK;
}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------
INT32 i4Venc_Hal_Init(void)
{
    i4Venc_Hal_RegIsr();

    i4Venc_Hal_CreateIsrEventGroup();

    return 0;
}

INT32 i4Venc_Hal_Uninit(void)
{
    i4Venc_Hal_DeleteIsrEventGroup();

    i4Venc_Hal_UnregIsr();

    return 0;
}

INT32 i4Venc_Hal_Stop(void)
{
    //clear all Interrupt
    i4Venc_Hw_VENC_IRQ_ACK(0x1F);

    i4Venc_Hal_Uninit();

    return (0);
}

INT32 i4Venc_Hal_Power(BOOL fgPowerOn)
{
    i4Venc_Hw_VENC_CLK_SETTING(fgPowerOn);
    //vIO32Write4B(CKGEN_VENC_CKCFG, VENC_CK_SEL);

    return (0);
}

INT32 i4Venc_Hal_HwReset(void)
{
    VENC_SW_HRST_N_T rParam;

    LOG(7,"[raven] start reset Hw_VENC_SW_HRST_N\n");
    
    rParam.u1SW_HRST_N = 0x0;
    i4Venc_Hw_VENC_SW_HRST_N(&rParam);

    i4Venc_Hw_RD_VENC_SW_HRST_N();

#if RAVEN_RESET
    rParam.u1SW_HRST_N = 0x0;
    i4Venc_Hw_VENC_SW_HRST_N(&rParam);

    {
        UINT32 u4Loop = 0;
        UINT32 u4Cnt = 0;
        UINT32 u4Reg = 0;
        while(1)
        {
            u4Loop = 100;
            u4Reg = *((UINT32*)(0xF003A000)); // read register VENC_HW_MODE_VIDEO_HW_MODE_SEL xiaolei.li
            if (0 == (u4Reg >> 28))
            {
                break;
            }

            //ASSERT(0);

            rParam.u1SW_HRST_N = 0x0;
            i4Venc_Hw_VENC_SW_HRST_N(&rParam);

            //x_thread_delay(1);
            while(u4Loop)
            {
                u4Loop --;
            }

            u4Cnt ++;
        }

        if (0 < u4Cnt)
        {
            LOG(7,"[raven] Hw_VENC_SW_HRST_N write 0x0 cnt %d\n", u4Cnt);
        }
    }
#endif

#if RAVEN_RESET
    rParam.u1SW_HRST_N = 0x1;
    i4Venc_Hw_VENC_SW_HRST_N(&rParam);

    {
        UINT32 u4Loop = 0;
        UINT32 u4Cnt = 0;
        UINT32 u4Reg = 0;
        while(1)
        {
            u4Loop = 100;
            u4Reg = *((UINT32*)(0xF003A000)); // read register VENC_HW_MODE_VIDEO_HW_MODE_SEL xiaolei.li
            if (1 == (u4Reg >> 28))
            {
                break;
            }

            //ASSERT(0);
            
            while(u4Loop)
            {
                u4Loop --;
            }

            rParam.u1SW_HRST_N = 0x1;
            i4Venc_Hw_VENC_SW_HRST_N(&rParam);

            u4Cnt ++;
        }

        if (0 < u4Cnt)
        {
            LOG(7,"[raven] Hw_VENC_SW_HRST_N write 0x1 cnt %d\n", u4Cnt);
        }
    }
#endif

    LOG(7,"[raven] end reset Hw_VENC_SW_HRST_N\n");

    return 0;
}

INT32 i4Venc_Hal_SetRamCodeBuf(UINT32 u4RamCodeAddr)
{
#ifdef VENC_NEW_EMU
    if (0 == u4RamCodeSrc)
    {
        x_memcpy(u4RamCodeAddr, VENC_IRAM_CODE, VENC_IRAM_LEN);
    }
#endif

    i4Venc_Hw_VENC_RC_CODE_DRAM_ADDR(u4RamCodeAddr);

    return 0;
}

INT32 i4Venc_Hal_SetRCInfoBuf(UINT32 u4RCInfoBufAddr)
{
    i4Venc_Hw_VENC_RC_INFO_DRAM_ADDR(u4RCInfoBufAddr);

    return 0;
}

INT32 i4Venc_Hal_SetOutputBuf(UINT32 u4BufAddr, UINT32 u4BufSize)
{
    i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_SIZE(u4BufSize);
    i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_ADDR(u4BufAddr);

    return 0;
}

INT32 i4Venc_Hal_Enc(UINT32 u4EncType, VENC_RAVEN_ENC_PARAM_T* prParam)
{
    VENC_CODEC_CTRL_T       rCodecCtrl = {0};
    VENC_IRQ_MODE_SEL_T     rIrqModeSel = {0};
    VENC_CE_T               rCe = {0};
    #ifdef REG_DUMP
    static UINT32 u4FrmIdx = 0;
    #endif
    
    _fgVencFifoFull = FALSE;

    i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_SIZE(prParam->u4OutputBufSize);
    i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_ADDR(prParam->u4OutputBufAddr);

    i4Venc_Hw_VENC_HW_MODE_SEL(&(prParam->rVENC_HW_MODE_SEL));

    rIrqModeSel.u1BS_DRAM_FULL_IRQ_ENABLE = 1;
    rIrqModeSel.u1ENC_FRM_IRQ_ENABLE = 1;
    rIrqModeSel.u1ENC_PPS_IRQ_ENABLE = 1;
    rIrqModeSel.u1ENC_SPS_IRQ_ENABLE = 1;
    rIrqModeSel.u1PAUSE_FRM_IRQ_ENABLE = 1;
    i4Venc_Hw_VENC_IRQ_MODE_SEL(&rIrqModeSel);

    rCe.u1CE = 1;
    i4Venc_Hw_VENC_CE(&rCe);

    if (VENC_RAVEN_ENC_SPS == u4EncType)
    {
        rCodecCtrl.u1ENC_SPS = 1;
        i4Venc_Hw_VENC_CODEC_CTRL(&rCodecCtrl);
        
        #ifdef REG_DUMP
        i4Venc_Hal_SeqLelRegDump();
        i4Venc_Hal_SPSEncRegDump();
        #endif
        
        i4Venc_Hal_WaitIsrEvent(VENC_INTR_SPSINT);
    }
    else if (VENC_RAVEN_ENC_PPS == u4EncType)
    {
        rCodecCtrl.u1ENC_PPS = 1;
        i4Venc_Hw_VENC_CODEC_CTRL(&rCodecCtrl);

        #ifdef REG_DUMP
        i4Venc_Hal_PPSEncRegDump();
        #endif
        
        i4Venc_Hal_WaitIsrEvent(VENC_INTR_PPSINT);
    }
    else if (VENC_RAVEN_ENC_FRM == u4EncType)
    {
        rCodecCtrl.u1ENC_FRM = 1;
        i4Venc_Hw_VENC_CODEC_CTRL(&rCodecCtrl);
        
        #ifdef REG_DUMP
        i4Venc_Hal_FrmLelRegDump(u4FrmIdx);
        u4FrmIdx ++;
        #endif
        
        i4Venc_Hal_WaitIsrEvent(VENC_INTR_FRMINT | VENC_INTR_FIFOFULL);
    }
    else if (VENC_RAVEN_ENC_RELEASE_BS_DRAM == u4EncType)
    {
        rCodecCtrl.u1RELEASE_BS_DRAM = 1;
        i4Venc_Hw_VENC_CODEC_CTRL(&rCodecCtrl);
        i4Venc_Hal_WaitIsrEvent(VENC_INTR_FRMINT | VENC_INTR_FIFOFULL);
    }
    else
    {
        ASSERT(0);
    }

    rCe.u1CE = 0;
    i4Venc_Hw_VENC_CE(&rCe);

    prParam->fgOutputBufFull = _fgVencFifoFull;

    return 0;
}

INT32 i4Venc_Hal_SeqMmrSet(VENC_RAVEN_SEQ_MMR_PARAM_T* prParam)
{
    if (0 == prParam->rVENC_RATECONTROL_INFO_1.u1RC_FPS)
    {
        LOG(0, "[venc] i4Venc_Hal_SeqMmrSet 0, default to fps 30\n");
        prParam->rVENC_RATECONTROL_INFO_1.u1RC_FPS = 30;
    }

    i4Venc_Hw_VENC_ENCODER_INFO_0(&(prParam->rVENC_ENCODER_INFO_0));
    i4Venc_Hw_VENC_ENCODER_INFO_1(&(prParam->rVENC_ENCODER_INFO_1));
    i4Venc_Hw_VENC_ENCODER_INFO_2(&(prParam->rVENC_ENCODER_INFO_2));
    i4Venc_Hw_VENC_IMAGE_INFO_0(&(prParam->rVENC_IMAGE_INFO_0));
    i4Venc_Hw_VENC_IMAGE_INFO_1(&(prParam->rVENC_IMAGE_INFO_1));
    i4Venc_Hw_VENC_H264_ENC_INFO_0(&(prParam->rVENC_H264_ENC_INFO_0));
    i4Venc_Hw_VENC_RATECONTROL_INFO_0(&(prParam->rVENC_RATECONTROL_INFO_0));
    i4Venc_Hw_VENC_RATECONTROL_INFO_1(&(prParam->rVENC_RATECONTROL_INFO_1));
    i4Venc_Hw_VENC_AVSR_ANRP_INFO_1(&(prParam->rVENC_AVSR_ANRP_INFO_1));
    
    return 0;
}

INT32 i4Venc_Hal_FrmMmrSet(VENC_RAVEN_FRM_MMR_PARAM_T* prParam)
{
    if (0 == prParam->rVENC_RATECONTROL_INFO_1.u1RC_FPS)
    {
        LOG(0, "[venc] i4Venc_Hal_FrmMmrSet 0\n");
        prParam->rVENC_RATECONTROL_INFO_1.u1RC_FPS = 30;
    }

    i4Venc_Hw_VENC_ENCODER_INFO_3(&(prParam->rVENC_ENCODER_INFO_3));
    i4Venc_Hw_VENC_RATECONTROL_INFO_0(&(prParam->rVENC_RATECONTROL_INFO_0));
    i4Venc_Hw_VENC_RATECONTROL_INFO_1(&(prParam->rVENC_RATECONTROL_INFO_1));

    i4Venc_Hw_VENC_ENCODER_INFO_4(prParam->u4FrmCnt);
    i4Venc_Hw_VENC_ENCODER_INFO_5(prParam->u4FrmIdx);
    i4Venc_Hw_VENC_ENCODER_INFO_6(prParam->u4LastIDRFrmIdx);
    
    i4Venc_Hw_VENC_FRM_CUR_Y_DRAM_ADDR(prParam->u4FrmCurYAddr);
    i4Venc_Hw_VENC_FRM_CUR_UV_DRAM_ADDR(prParam->u4FrmCurCAddr);
    i4Venc_Hw_VENC_FRM_CUR_V_DRAM_ADDR(prParam->u4FrmCurVAddr);
    
    i4Venc_Hw_VENC_FRM_REF_Y_DRAM_ADDR(prParam->u4FrmRefYAddr);
    i4Venc_Hw_VENC_FRM_REF_UV_DRAM_ADDR(prParam->u4FrmRefCAddr);
    
    i4Venc_Hw_VENC_FRM_REC_Y_DRAM_ADDR(prParam->u4FrmRecYAddr);
    i4Venc_Hw_VENC_FRM_REC_UV_DRAM_ADDR(prParam->u4FrmRecCAddr);
    
    i4Venc_Hw_VENC_LOAD_COL_INFO_DRAM_ADDR(prParam->u4ColLoadAddr);
    i4Venc_Hw_VENC_SAVE_COL_INFO_DRAM_ADDR(prParam->u4ColSaveAddr);
    
    i4Venc_Hw_VENC_RC_INFO_DRAM_ADDR(prParam->u4RcInfoAddr);

    return 0;
}

INT32 i4Venc_Hal_VUISet(VENC_RAVEN_VUI_PARAM_T* prVUI)
{
    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_0(&(prVUI->rVENC_VUI_PARAMETER_INFO_0_T));

    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_1(&(prVUI->rVENC_VUI_PARAMETER_INFO_1_T));

    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_2(&(prVUI->rVENC_VUI_PARAMETER_INFO_2_T));

    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_3(prVUI->u4VUI_num_units_in_tick);

    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_4(prVUI->u4VUI_time_scale);

    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_5(&(prVUI->rVENC_VUI_PARAMETER_INFO_5_T));

    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_6(&(prVUI->rVENC_VUI_PARAMETER_INFO_6_T));

    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_7(&(prVUI->rVENC_VUI_PARAMETER_INFO_7_T));

    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_8(&(prVUI->rVENC_VUI_PARAMETER_INFO_8_T));

    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_9(&(prVUI->rVENC_VUI_PARAMETER_INFO_9_T));

    return 0;
}

INT32 i4Venc_Hal_GetResult(VENC_RESULT_INFO_T* pResultInfo)
{
    pResultInfo->u4BitstreamStartAddr = i4Venc_Hw_VENC_BITSTREAM_BUF_DRAM_ADDR_GET();
    pResultInfo->u4BitstreamByteCnt = i4Venc_Hw_VENC_PIC_BITSTREAM_BYTE_CNT();
  
    return 0;
}

INT32 i4Venc_Hal_GetCrc(VENC_RAVEN_CRC_T* prCrc)
{
    prCrc->u4BsCrc = i4Venc_Hw_VENC_CRC_BSDMA();
    prCrc->u4CurLumaCrc = i4Venc_Hw_VENC_CRC_CUR_LUMA();
    prCrc->u4RefLumaCrc = i4Venc_Hw_VENC_CRC_REF_LUMA();
    prCrc->u4CurChromaCrc = i4Venc_Hw_VENC_CRC_CUR_CHROMA();
    prCrc->u4RefChromaCrc = i4Venc_Hw_VENC_CRC_REF_CHROMA();
    prCrc->u4RdComvCrc = i4Venc_Hw_VENC_CRC_RD_COMV();
    prCrc->u4SvComvCrc = i4Venc_Hw_VENC_CRC_SV_COMV();
    prCrc->u4RecFrmCrc = i4Venc_Hw_VENC_CRC_REC_FRM();

    return 0;
}

INT32 i4Venc_Hal_Start(VOID)
{
    i4Venc_Hal_Init();
    i4Venc_Hal_Power(TRUE);
    i4Venc_Hal_HwReset();

    //to do: register configuration here
    //register setting in vuiset, seqmmr set & frmmmr set
    
    return 0;
}

INT32 i4Venc_Hal_SetAspectRatioIdc(VENC_RAVEN_VUI_PARAM_T* prVUI)
{
    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_0_ASPECT_RATIO_IDC(&(prVUI->rVENC_VUI_PARAMETER_INFO_0_T));
    i4Venc_Hw_VENC_VUI_PARAMETER_INFO_2(&(prVUI->rVENC_VUI_PARAMETER_INFO_2_T));

    return 0;
}

