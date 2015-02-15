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
 
#ifndef VENC_VERIFY_H
#define VENC_VERIFY_H

#include "x_typedef.h"

#define VENC_RAVEN_FRMENCSRC_CNT      50
#define VENC_RAVEN_RESULT_CNT         50

#define VENC_RAVEN_INPUTBUF_CNT   1
#define VENC_RAVEN_SRCBUF_CNT     (1+2)

typedef struct
{

} VENC_FRMENCSRC_T;

typedef struct
{
    UINT32              u4OutputBuf;
    UINT32              u4RamCodeBuf;
    UINT32              u4RCInfoBuf;
    UINT32              au4SrcBufY[VENC_RAVEN_SRCBUF_CNT];
    UINT32              au4SrcBufC[VENC_RAVEN_SRCBUF_CNT];
    UINT32              au4SrcBufV[VENC_RAVEN_SRCBUF_CNT];
    UINT32              au4ColMvInfoBuf[2];
} VENC_RAVEN_HW_BUF_T;

typedef struct
{
    //UINT32              au4InputBufY[VENC_RAVEN_INPUTBUF_CNT];
    //UINT32              au4InputBufU[VENC_RAVEN_INPUTBUF_CNT];
    //UINT32              au4InputBufV[VENC_RAVEN_INPUTBUF_CNT];
    
    UINT32              u4ConvertBuf;

    UINT32              u4MmrBuf;
    UINT32              u4GoldenBuf;
    UINT32              u4SpsBuf;
    UINT32              u4PpsBuf;
    UINT32              u4CrcBuf;
    UINT32              u4ListBuf;
} VENC_RAVEN_VFY_BUF_T;

typedef struct
{
    VENC_RAVEN_HW_BUF_T         rHwBuf;

    VENC_RAVEN_VFY_BUF_T        rVfyBuf;


    VENC_RAVEN_H246_MMR_PARAM   rMmr;

    UINT32                      u4SrcBufCurrIdx;
    UINT32                      u4GoldenFileRdOft;
    UINT32                      u4CrcFileRdOft;

    BOOL                        fgWaitSrcBufFree;

    UINT32                      u4FrmEncSrcReadIdx;
    UINT32                      u4FrmEncSrcWriteIdx;
    VENC_FRMENCSRC_T            rFrmEncSrc[VENC_RAVEN_FRMENCSRC_CNT];

    UINT32                      u4ResultReadIdx;
    UINT32                      u4ResultWriteIdx;
    VENC_RESULT_T               rResult[VENC_RAVEN_RESULT_CNT];

    UINT32                      u4OutputBufReadOft;
    UINT32                      u4OutputBufWriteOft;

    UINT32                      u4SrcW;
    UINT32                      u4SrcH;
    UINT32                      u4SrcAlignW;
    UINT32                      u4SrcAlignH;

    UINT32                      u4RealEncFrmCnt;
    UINT32                      u4GoldenChkFrmCnt;

    BOOL                        fgChkGolden;
    BOOL                        fgChkCrc;

    UINT32                      u4FrmIdx;

    UINT32                      u4FileListOft;
    BOOL                        fgFileListEnd;
    UINT32                      u4FileHeadCnt;
} VENC_RAVEN_VFY_INFO_T;

#endif //VENC_VERIFY_H



