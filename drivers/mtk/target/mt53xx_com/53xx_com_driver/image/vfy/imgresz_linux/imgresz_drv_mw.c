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


#include "x_assert.h"
#include "x_os.h"
#include "imgresz_drv_x.h"
#include "drv_imgresz.h"

/*-----------------------------------------------------------------------------
                    data declarations
 ----------------------------------------------------------------------------*/

INT32 i4ImgReszMwGetTicket(IMGRESZ_TICKET_T *pTicket)
{
    return i4ImgResz_Drv_GetTicket((IMGRESZ_DRV_TICKET_T *)pTicket);
}

INT32 i4ImgReszMwGetState(IMGRESZ_TICKET_T *pTicket, IMGRESZ_SCALE_STATE_T *pState)
{
    return i4ImgResz_Drv_GetState((IMGRESZ_DRV_TICKET_T *)pTicket, (IMGRESZ_DRV_SCALE_STATE_T *)pState);
}

INT32 i4ImgReszMwReleaseTicket(IMGRESZ_TICKET_T *pTicket)
{
    return i4ImgResz_Drv_ReleaseTicket((IMGRESZ_DRV_TICKET_T *)pTicket);
}

INT32 i4ImgReszMwSetScaleMode(IMGRESZ_TICKET_T *pTicket, IMGRESZ_SCALE_MODE eScaleMode)
{
    return i4ImgResz_Drv_SetScaleMode((IMGRESZ_DRV_TICKET_T *)pTicket, (IMGRESZ_DRV_SCALE_MODE) eScaleMode);
}

INT32 i4ImgReszMwSetSrcBufInfo(IMGRESZ_TICKET_T *pTicket, IMGRESZ_SRC_BUF_INFO_T *pSrcBufInfo)
{
    return i4ImgResz_Drv_SetSrcBufInfo((IMGRESZ_DRV_TICKET_T *)pTicket, (IMGRESZ_DRV_SRC_BUF_INFO_T *)pSrcBufInfo);
}

INT32 i4ImgReszMwSetDstBufInfo(IMGRESZ_TICKET_T *pTicket, IMGRESZ_DST_BUF_INFO_T *pDstBufInfo)
{
    return i4ImgResz_Drv_SetDstBufInfo((IMGRESZ_DRV_TICKET_T *)pTicket, (IMGRESZ_DRV_DST_BUF_INFO_T *)pDstBufInfo);
}

INT32 i4ImgReszMwSetPartialBufInfo(IMGRESZ_TICKET_T *pTicket, IMGRESZ_PARTIAL_INFO_T *pPartialBufInfo)
{
    return i4ImgResz_Drv_SetPartialBufInfo((IMGRESZ_DRV_TICKET_T *)pTicket, (IMGRESZ_DRV_PARTIAL_INFO_T *)pPartialBufInfo);
}

INT32 i4ImgReszMwDoScale(IMGRESZ_TICKET_T *pTicket, IMGRESZ_DO_SCALE_T *pDoScale)
{
    return i4ImgResz_Drv_DoScale((IMGRESZ_DRV_TICKET_T *)pTicket, (IMGRESZ_DRV_DO_SCALE_T *)pDoScale);
}


INT32 i4ImgReszMwStopScale(IMGRESZ_TICKET_T *pTicket)
{
    return i4ImgResz_Drv_StopScale((IMGRESZ_DRV_TICKET_T *)pTicket);
}

INT32 i4ImgReszMw_Get (UINT16 u2CompId, UINT32 u4GetCommand, void* pvGetInfo, UINT32 u4GetInfoSize)
{
    INT32 i4Ret = 1;

    switch(u4GetCommand)
    {
        case IMG_RESZ_GET_TYPE_GET_TICKET:
            i4Ret = i4ImgReszMwGetTicket((IMGRESZ_TICKET_T *)pvGetInfo);
            break;
        case IMG_RESZ_GET_TYPE_SCALING_STATE:
            {
                IMGRESZ_TICKET_T rTicket;
                rTicket.u4Ticket = u2CompId;
                i4Ret = i4ImgReszMwGetState(&rTicket, (IMGRESZ_SCALE_STATE_T *)pvGetInfo);
            }
            break;
        default:
            ASSERT(0);
            break;
    }
    return i4Ret;
}

INT32 i4ImgReszMw_Set (UINT16 u2CompId, UINT32 u4SetCommand, void* pvSetInfo, UINT32 u4SetInfoSize)
{
    INT32 i4Ret = 1;

    switch(u4SetCommand)
    {
        case IMG_RESZ_SET_TYPE_RELEASE_TICKET:
            {
                IMGRESZ_TICKET_T rTicket;
                rTicket.u4Ticket = u2CompId;
                i4Ret = i4ImgReszMwReleaseTicket(&rTicket);
            }
            break;
#if 0
        case IMG_RESZ_SET_TYPE_RESET_IMGRESZ:
            //do nothing now, will do resizer when do scale
            break;
#endif
        case IMG_RESZ_SET_TYPE_SET_SCALE_MODE:
            {
                IMGRESZ_TICKET_T rTicket;
                rTicket.u4Ticket = u2CompId;
                i4Ret = i4ImgReszMwSetScaleMode(&rTicket, (IMGRESZ_SCALE_MODE)(UINT32)pvSetInfo);
            }
            break;
        case IMG_RESZ_SET_TYPE_SET_SRC_BUF_INFO:
            {
                IMGRESZ_TICKET_T rTicket;
                rTicket.u4Ticket = u2CompId;
                i4Ret = i4ImgReszMwSetSrcBufInfo(&rTicket, (IMGRESZ_SRC_BUF_INFO_T *)pvSetInfo);
            }
            break;
        case IMG_RESZ_SET_TYPE_SET_DST_BUF_INFO:
            {
                IMGRESZ_TICKET_T rTicket;
                rTicket.u4Ticket = u2CompId;
                i4Ret = i4ImgReszMwSetDstBufInfo(&rTicket, (IMGRESZ_DST_BUF_INFO_T *)pvSetInfo);
            }
            break;
        case IMG_RESZ_SET_TYPE_SET_PARTIAL_BUF_INFO:
            {
                IMGRESZ_TICKET_T rTicket;
                rTicket.u4Ticket = u2CompId;
                i4Ret = i4ImgReszMwSetPartialBufInfo(&rTicket, (IMGRESZ_PARTIAL_INFO_T *)pvSetInfo);
            }
            break;
        case IMG_RESZ_SET_TYPE_SET_DO_SCAL:
            {
                IMGRESZ_TICKET_T rTicket;
                rTicket.u4Ticket = u2CompId;
                i4Ret = i4ImgReszMwDoScale(&rTicket, (IMGRESZ_DO_SCALE_T *)pvSetInfo);
            }
            break;
        case IMG_RESZ_SET_TYPE_SET_STOP:
            {
                IMGRESZ_TICKET_T rTicket;
                rTicket.u4Ticket = u2CompId;
                i4Ret = i4ImgReszMwStopScale(&rTicket);
            }
            break;
        default:
            ASSERT(0);
            break;
    }
    return i4Ret;
}

