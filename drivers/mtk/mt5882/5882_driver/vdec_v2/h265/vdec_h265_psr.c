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
 * $RCSfile: vdec_h265_psr.c,v $
 * $Revision: #1 $
 *---------------------------------------------------------------------------*/

#define DEFINE_IS_LOG VDEC_IsLog
#include "x_debug.h"
#include "x_hal_5381.h"
#include "x_assert.h"
#include "x_os.h"
#include "x_mid.h"
#include "vdec_if.h"
#include "vdec_drvif.h"
#include "vdec_info_h265.h"
#include "vdec_h265_psr.h"
#include "../lae/vdec_lae.h"




BOOL fgInitRPS[6] = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};//temp solution
BOOL fgInitScalingList[6] = {FALSE,FALSE,FALSE,FALSE,FALSE,FALSE};
H265_RPS_Data *prH265RPS[MAX_ES_NUM][H265_MAX_SPS_NUM][H265_MAX_RPS_NUM];
INT32 *prPPSScalingListDeltaCoef[MAX_ES_NUM][H265_MAX_PPS_NUM][SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM];
INT32 *prSPSScalingListDeltaCoef[MAX_ES_NUM][H265_MAX_SPS_NUM][SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM];

extern const UINT32 H265_scalingListSize[4];
extern const UINT32 H265_scalingListSizeX[4];
extern const UINT32 H265_scalingListNum[SCALING_LIST_SIZE_NUM];

const INT32  H265_eTTable[4] = {0,3,1,2};

/// coefficient scanning type used in ACS
enum COEFF_SCAN_TYPE
{
  SCAN_DIAG = 0,         ///< up-right diagonal scan
  SCAN_HOR,              ///< horizontal first scan
  SCAN_VER               ///< vertical first scan
};

// scanning order table
#define     MAX_CU_DEPTH            7                           // log2(LCUSize)
#define     MAX_CU_SIZE             (1<<(MAX_CU_DEPTH))         // maximum allowable size of CU
#define     MAX_LAST_SCAN_SIZE      (0x3FFF0)

INT32  H265_aucConvertToBit[MAX_CU_SIZE+1];
UINT32* H265_auiSigLastScan[3][MAX_CU_DEPTH];
UINT32 *gH265LastScan = NULL;
const UINT32 H265_sigLastScan8x8[3][4] =
{
  {0, 2, 1, 3},
  {0, 1, 2, 3},
  {0, 2, 1, 3}
};

UINT32 H265_sigLastScanCG32x32[64];

INT32 vVDecH265InitROM(UINT32 u4InstID)
{
    INT32 i,j,c;
    INT32 i4Rdptr = 0,i4Size = 0;
    UNUSED(u4InstID);
    for(i=0; i<MAX_CU_SIZE+1 ; i++)
    {
        H265_aucConvertToBit[i] = -1;
    }
    
    c=0;
    for ( i=4; i<MAX_CU_SIZE; i*=2 )
    {
        H265_aucConvertToBit[i] = c;
        c++;
    }
    H265_aucConvertToBit[i] = c;

    if(gH265LastScan == NULL)
    {
        gH265LastScan = (UINT32*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(MAX_LAST_SCAN_SIZE, 1024));
        if(gH265LastScan == NULL)
        {
            LOG(0,"[H265]Allocate last scan buffer fail \n");
            return (INT32)E_VDEC_FAIL;
        }
        x_memset((void *)gH265LastScan,0,MAX_LAST_SCAN_SIZE);
        
        //Init Last Scan
        for(i = 0; i < 3; i++)
        {
            for(j = 0; j < MAX_CU_DEPTH; j++)
            {
                H265_auiSigLastScan[i][j] = NULL;
            }
        }
    }
    else
    {
        return (INT32)E_VDEC_OK;
    }
    
    c = 2;
    for ( i = 0; i < MAX_CU_DEPTH; i++ )
    {
        i4Size = c*c;
        #if 0
        if (H265_auiSigLastScan[0][i] == NULL){ H265_auiSigLastScan[0][i] = (UINT32*)vmalloc(c*c*sizeof(UINT32)); } 
        if (H265_auiSigLastScan[1][i] == NULL){ H265_auiSigLastScan[1][i] = (UINT32*)vmalloc(c*c*sizeof(UINT32)); }
        if (H265_auiSigLastScan[2][i] == NULL){ H265_auiSigLastScan[2][i] = (UINT32*)vmalloc(c*c*sizeof(UINT32)); } 
        #else
        if (H265_auiSigLastScan[0][i] == NULL) { H265_auiSigLastScan[0][i] = (UINT32*)(gH265LastScan + i4Rdptr); } 
        i4Rdptr += i4Size;
        if (H265_auiSigLastScan[1][i] == NULL) { H265_auiSigLastScan[1][i] = (UINT32*)(gH265LastScan + i4Rdptr); }
        i4Rdptr += i4Size;
        if (H265_auiSigLastScan[2][i] == NULL) { H265_auiSigLastScan[2][i] = (UINT32*)(gH265LastScan + i4Rdptr); } 
        i4Rdptr += i4Size;
        #endif
        vVDecH265InitSigLastScan( H265_auiSigLastScan[0][i], H265_auiSigLastScan[1][i], H265_auiSigLastScan[2][i], c, c);
        c <<= 1;
    }

    return (INT32)E_VDEC_OK;
}


void vVDecH265InitSigLastScan(UINT32* pBuffD, UINT32* pBuffH, UINT32* pBuffV, INT32 iWidth, INT32 iHeight)
{
      const UINT32  uiNumScanPos  = (UINT32) iWidth * iWidth;
      UINT32 uiNextScanPos = 0;
      UINT32 uiScanLine;
      UINT32 initBlkPos,offsetY,offsetX,offsetD,offsetScan;
      UINT32 uiCnt = 0;
      INT32 blkX, blkY;
      INT32 iX, iY, x, y;

      
      if( iWidth < 16 )
      {
        UINT32* pBuffTemp = pBuffD;
        if( iWidth == 8 )
        {
          pBuffTemp = H265_sigLastScanCG32x32;
        }

        for(uiScanLine = 0; uiNextScanPos < uiNumScanPos; uiScanLine++ )
        {
          INT32    iPrimDim  = (INT32) uiScanLine;
          INT32    iScndDim  = 0;
          while( iPrimDim >= iWidth )
          {
            iScndDim++;
            iPrimDim--;
          }
          while( iPrimDim >= 0 && iScndDim < iWidth )
          {
            pBuffTemp[ uiNextScanPos ] = iPrimDim * iWidth + iScndDim ;
            uiNextScanPos++;
            iScndDim++;
            iPrimDim--;
          }
        }
      }


      if( iWidth > 4 )
      {
        UINT32 uiNumBlkSide = iWidth >> 2;
        UINT32 uiNumBlks    = uiNumBlkSide * uiNumBlkSide;
        UINT32 log2Blk      = (UINT32)((INT32)H265_aucConvertToBit[ uiNumBlkSide ] + 1);
        UINT32 uiBlk;

        for( uiBlk = 0; uiBlk < uiNumBlks; uiBlk++ )
        {
          uiNextScanPos   = 0;
          initBlkPos = H265_auiSigLastScan[ SCAN_DIAG ][ log2Blk ][ uiBlk ];

          if( iWidth == 32 )
          {
            initBlkPos = H265_sigLastScanCG32x32[ uiBlk ];
          }
          offsetY    = initBlkPos / uiNumBlkSide;
          offsetX    = initBlkPos - offsetY * uiNumBlkSide;
          offsetD    = 4 * ( offsetX + offsetY * iWidth );
          offsetScan = 16 * uiBlk;
          for( uiScanLine = 0; uiNextScanPos < 16; uiScanLine++ )
          {
            INT32    iPrimDim  = (INT32)uiScanLine;
            INT32    iScndDim  = 0;
            while( iPrimDim >= 4 )
            {
              iScndDim++;
              iPrimDim--;
            }
            while( iPrimDim >= 0 && iScndDim < 4 )
            {
              pBuffD[ uiNextScanPos + offsetScan ] = iPrimDim * iWidth + iScndDim + offsetD;
              uiNextScanPos++;
              iScndDim++;
              iPrimDim--;
            }
          }
        }
      }

      if( iWidth > 2 )
      {
        UINT32 numBlkSide = iWidth >> 2;
        for(blkY=0; blkY < numBlkSide; blkY++)
        {
          for(blkX=0; blkX < numBlkSide; blkX++)
          {
            UINT32 offset = blkY * 4 * iWidth + blkX * 4;
            for(y=0; y < 4; y++)
            {
              for(x=0; x < 4; x++)
              {
                pBuffH[uiCnt] = y*iWidth + x + offset;
                uiCnt ++;
              }
            }
          }
        }

        uiCnt = 0;
        for(blkX=0; blkX < numBlkSide; blkX++)
        {
          for(blkY=0; blkY < numBlkSide; blkY++)
          {
            UINT32 offset = blkY * 4 * iWidth + blkX * 4;
            for(x=0; x < 4; x++)
            {
              for(y=0; y < 4; y++)
              {
                pBuffV[uiCnt] = y*iWidth + x + offset;
                uiCnt ++;
              }
            }
          }
        }
      }
      else
      {
        for(iY=0; iY < iHeight; iY++)
        {
          for(iX=0; iX < iWidth; iX++)
          {
            pBuffH[uiCnt] = iY*iWidth + iX;
            uiCnt ++;
          }
        }

        uiCnt = 0;
        for(iX=0; iX < iWidth; iX++)
        {
          for(iY=0; iY < iHeight; iY++)
          {
            pBuffV[uiCnt] = iY*iWidth + iX;
            uiCnt ++;
          }
        }    
      }
}

INT32 vVDecH265InitRPS(UINT32 u4InstID)
{
    //H265_RPS_Data ***prRPS = NULL;
    H265_RPS_Data *p4RPSData = NULL;
    INT32 i4SpsIdx,i4RpsIdx;

    if(fgInitRPS[u4InstID] == FALSE)
    {
        for(i4SpsIdx = 0; i4SpsIdx < H265_MAX_SPS_NUM; i4SpsIdx ++)
        {
            for(i4RpsIdx = 0; i4RpsIdx < H265_MAX_RPS_NUM; i4RpsIdx++)
            {
                //prRPS[i4SpsIdx][i4RpsIdx] = NULL;
                prH265RPS[u4InstID][i4SpsIdx][i4RpsIdx] = NULL;
            }
        }
        fgInitRPS[u4InstID] = TRUE;
    }
    else
    {
        return (INT32)E_VDEC_OK;
    }
    
    p4RPSData = (H265_RPS_Data*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory((H265_MAX_SPS_NUM*H265_MAX_RPS_NUM*sizeof(H265_RPS_Data)), 1024));
    if(p4RPSData == NULL)
    {
        LOG(0,"[H265-ERR] RPS allocate memory faile\n");
        return (INT32)E_VDEC_FAIL;
    }
    else
    {
        x_memset((void *)p4RPSData,0,(H265_MAX_SPS_NUM*H265_MAX_RPS_NUM*sizeof(H265_RPS_Data)));
        LOG(0,"[H265] RPS allocate memory OK [0x%x]\n",(UINT32)p4RPSData);
    }
    
    for(i4SpsIdx = 0; i4SpsIdx < H265_MAX_SPS_NUM; i4SpsIdx ++)
    {
        for(i4RpsIdx = 0; i4RpsIdx < H265_MAX_RPS_NUM; i4RpsIdx++)
        {
            //prRPS[i4SpsIdx][i4RpsIdx] = p4RPSData ++;
            prH265RPS[u4InstID][i4SpsIdx][i4RpsIdx] = p4RPSData ++;
        }
    }
    
    return (INT32)E_VDEC_OK;
}


INT32 vVDecH265InitScalingList(UINT32 u4InstID)
{
    UINT32 u4ScalingListTotalSize = 0,u4ScalingListSize = 0;
    INT32 *pi4ScalingList = NULL;
    //INT32 ****pi4PPSScalingList = NULL;
    //INT32 ****pi4SPSScalingList = NULL;
    INT32 i4PPSIdx,i4SPSIdx,i4SizeIdx,i4ListIdx;
    if(fgInitScalingList[u4InstID] == FALSE)
    {
        fgInitScalingList[u4InstID] = TRUE;
    }
    else
    {
        return (INT32)E_VDEC_OK;
    }
    u4ScalingListTotalSize = (H265_MAX_SPS_NUM + H265_MAX_PPS_NUM)*SCALING_LIST_UNIT_SIZE;
    pi4ScalingList = (INT32*)VIRTUAL((UINT32)BSP_AllocAlignedDmaMemory(u4ScalingListTotalSize, 1024));
    if(pi4ScalingList == NULL)
    {
        LOG(0,"[H265-ERR] Scaling List allocate memory faile\n");
        return (INT32)E_VDEC_FAIL;
    }
    x_memset((void*)pi4ScalingList,0,u4ScalingListTotalSize);

    //PPS scaling list init
    for(i4PPSIdx = 0; i4PPSIdx < H265_MAX_PPS_NUM; i4PPSIdx++)
    {
        for(i4SizeIdx = 0; i4SizeIdx < SCALING_LIST_SIZE_NUM; i4SizeIdx++)
        {
            for(i4ListIdx = 0; i4ListIdx < H265_scalingListNum[i4SizeIdx]; i4ListIdx++)
            {
                //pi4PPSScalingList[i4PPSIdx][i4SizeIdx][i4ListIdx] = (INT32*)((UINT32)pi4ScalingList + u4ScalingListSize);
                prPPSScalingListDeltaCoef[u4InstID][i4PPSIdx][i4SizeIdx][i4ListIdx] = (INT32*)((UINT32)pi4ScalingList + u4ScalingListSize);
                u4ScalingListSize += (MAX_MATRIX_COEF_NUM < H265_scalingListSize[i4SizeIdx])? (MAX_MATRIX_COEF_NUM * sizeof(INT32)) : (H265_scalingListSize[i4SizeIdx] * sizeof(INT32));
            }
        }
    }

    //SPS scaling list init
    for(i4SPSIdx = 0; i4SPSIdx < H265_MAX_SPS_NUM; i4SPSIdx++)
    {
        for(i4SizeIdx = 0; i4SizeIdx < SCALING_LIST_SIZE_NUM; i4SizeIdx++)
        {
            for(i4ListIdx = 0; i4ListIdx < H265_scalingListNum[i4SizeIdx]; i4ListIdx++)
            {
                //pi4SPSScalingList[i4SPSIdx][i4SizeIdx][i4ListIdx] = (INT32*)((UINT32)pi4ScalingList + u4ScalingListSize);
                prSPSScalingListDeltaCoef[u4InstID][i4SPSIdx][i4SizeIdx][i4ListIdx] = (INT32*)((UINT32)pi4ScalingList + u4ScalingListSize);
                u4ScalingListSize += (MAX_MATRIX_COEF_NUM < H265_scalingListSize[i4SizeIdx])? (MAX_MATRIX_COEF_NUM * sizeof(INT32)): (H265_scalingListSize[i4SizeIdx] * sizeof(INT32));
            }
        }
    }

    return (INT32)E_VDEC_OK;
    
}

/************************************************************/
/**/
/*u4VDecID : HW ID (0:VLD0; 1:VLD1; 2:LAE0; 3:LAE1)*/
/*return : ucNalType,ucLayerId,ucNuhTempId*/
/*fgForbiddenBit : 0 valid; 1 invalid*/
/*************************************************************/
BOOL fgVDecH265NalUnitHeader(UINT32 u4VDecID,UCHAR *ucNalType,UCHAR *ucLayerId,UCHAR *ucNuhTempId)
{
    BOOL fgForbiddenBit;
    fgForbiddenBit = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);
    *ucNalType = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 6);
    *ucLayerId = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 6);
    *ucNuhTempId = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 3) - 1;
    return fgForbiddenBit;
}

/************************************************************/
/**/
/*u4InstID : Instant ID*/
/*u4VDecID : HW ID (0:VLD0; 1:VLD1; 2:LAE0; 3:LAE1)*/
/**/
/*************************************************************/

INT32 i4VDecH265SPSPsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue)
{
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    H265_SPS_Data *prH265_SPS_DataInst = NULL;
    H265_PTL_Data rTmpPTL;
    UINT32 u4Code,i,u4Idx;
    UINT32 u4TmpMaxTLayersMinus1,u4SeqParameterSetId,u4TmpVPSId;
    UINT32 u4AddCUDepth;
    BOOL bTmpTemporalIdNestingFlag;
    ASSERT(prH265DrvDecInfo!=NULL);
    ASSERT(pi4RetValue!=NULL);
    *pi4RetValue = SPS_OK;

    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;

    //video_parameter_set_id
    u4TmpVPSId = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 4);//u(4)
    //sps_max_sub_layers_minus1
    u4TmpMaxTLayersMinus1 = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 3);//u(4)
    //temporal_id_nesting_flag
    bTmpTemporalIdNestingFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(4)
    if( u4TmpMaxTLayersMinus1==0 )
    {
        if(bTmpTemporalIdNestingFlag != 1)
        {
            // sps_temporal_id_nesting_flag must be 1 when sps_max_sub_layers_minus1 is 0
            LOG(0,"[VDEC-H265-ERR] layer count conflict with temporal id\n");
            *pi4RetValue = SPS_LAYER_CONFLICT;
        }
    }

    rTmpPTL.bProfilePresentFlag = 1;
    vVDecH265PTL_Rbsp(u4InstID,u4VDecID,&rTmpPTL,u4TmpMaxTLayersMinus1);

    u4SeqParameterSetId = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
    if(u4SeqParameterSetId < H265_MAX_SPS_NUM)
    {
        prH265_SPS_DataInst = &(prH265DrvDecInfo->arH265SPS[u4SeqParameterSetId]);
        prH265_SPS_DataInst->bSPSValid = FALSE;// FALSE until set completely
        prH265_SPS_DataInst->u4SPSId = u4SeqParameterSetId;
        prH265_SPS_DataInst->u4VPSId = u4TmpVPSId;
        prH265_SPS_DataInst->u4MaxTLayersMinus1 = u4TmpMaxTLayersMinus1;
        prH265_SPS_DataInst->bTemporalIdNestingFlag = bTmpTemporalIdNestingFlag;
        prH265_SPS_DataInst->u4SeqParameterSetId = u4SeqParameterSetId;
        prH265DecPrm->ucSemlessSPSIdx = u4SeqParameterSetId;
        //prH265_SPS_DataInst->rSPS_PTL = rTmpPTL;
        x_memcpy(&prH265_SPS_DataInst->rSPS_PTL,&rTmpPTL,sizeof(H265_PTL_Data));
    }
    else
    {
        prH265DecPrm->ucSemlessSPSIdx = 0xFF;
        LOG(0,"[VDEC-H265-ERR] SPS ID invalid\n");
        *pi4RetValue = SPS_ID_INVLID;
        return SPS_ID_INVLID;
    }

    vVDecH265InitSPS(u4InstID,prH265_SPS_DataInst);
    vVDecH265InitPicInfo(u4InstID,prH265DrvDecInfo);

    prH265_SPS_DataInst->u4ChromaFormatIdc = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)

    if( prH265_SPS_DataInst->u4ChromaFormatIdc == 3 )
    {
        prH265_SPS_DataInst->bSeparateColourPlaneFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);
        if (prH265_SPS_DataInst->bSeparateColourPlaneFlag)
        {
            LOG(0,"[VDEC-H265-ERR] Clour plan flag invalid\n");
        }
    }

    if ( 0 == prH265_SPS_DataInst->u4ChromaFormatIdc )
    {
        prH265_SPS_DataInst->u4SubWidthC = 1;
        prH265_SPS_DataInst->u4SubHeightC = 1;
    } 
    else if ( 1 == prH265_SPS_DataInst->u4ChromaFormatIdc ) 
    {
        prH265_SPS_DataInst->u4SubWidthC = 2;
        prH265_SPS_DataInst->u4SubHeightC = 2;
    } 
    else if ( 2 == prH265_SPS_DataInst->u4ChromaFormatIdc ) 
    {
        prH265_SPS_DataInst->u4SubWidthC = 2;
        prH265_SPS_DataInst->u4SubHeightC = 1;
    } 
    else if ( 3 == prH265_SPS_DataInst->u4ChromaFormatIdc ) 
    {
        prH265_SPS_DataInst->u4SubWidthC = 1;
        prH265_SPS_DataInst->u4SubHeightC = 1;
    }
    else 
    {
        LOG(0,"[VDEC-H265-ERR] u4ChromaFormatIdc invalid\n");
    }

    
    prH265_SPS_DataInst->u4PicWidthInLumaSamples = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
    prH265_SPS_DataInst->u4PicHeightInLumaSamples = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);    //ue(v)   
    prH265_SPS_DataInst->bConformanceWindowFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    if ( prH265_SPS_DataInst->bConformanceWindowFlag )
    {
        prH265_SPS_DataInst->u4ConfWinLeftOffset = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
        prH265_SPS_DataInst->u4ConfWinRightOffset = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
        prH265_SPS_DataInst->u4ConfWinTopOffset = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
        prH265_SPS_DataInst->u4ConfWinBottomOffset = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
    }

    prH265_SPS_DataInst->u4FrameCropLeftOffset = prH265_SPS_DataInst->u4ConfWinLeftOffset * prH265_SPS_DataInst->u4SubWidthC;
    prH265_SPS_DataInst->u4FrameCropRightOffset = prH265_SPS_DataInst->u4ConfWinRightOffset * prH265_SPS_DataInst->u4SubWidthC;
    prH265_SPS_DataInst->u4FrameCropTopOffset = prH265_SPS_DataInst->u4ConfWinTopOffset * prH265_SPS_DataInst->u4SubHeightC;
    prH265_SPS_DataInst->u4FrameCropBottomOffset = prH265_SPS_DataInst->u4ConfWinBottomOffset * prH265_SPS_DataInst->u4SubHeightC;

    prH265_SPS_DataInst->u4BitDepthLumaMinus8 = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
    prH265_SPS_DataInst->u4QpBDOffsetY = (UINT32) 6*(prH265_SPS_DataInst->u4BitDepthLumaMinus8+8);

    prH265_SPS_DataInst->u4BitDepthChromaMinus8 = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
    prH265_SPS_DataInst->u4QpBDOffsetC = (UINT32) 6*(prH265_SPS_DataInst->u4BitDepthChromaMinus8+8);

    // only support 0, 2 (8,10 bit)
    if (prH265_SPS_DataInst->u4BitDepthLumaMinus8 == 0 && prH265_SPS_DataInst->u4BitDepthChromaMinus8 == 0)
    {         
        prH265DecPrm->ucMain10Mode = HEVC_8BIT_MODE;
    }
    else if(prH265_SPS_DataInst->u4BitDepthLumaMinus8 == 2 && prH265_SPS_DataInst->u4BitDepthChromaMinus8 == 2)
    {
        prH265DecPrm->ucMain10Mode = HEVC_COMPACT_MODE;
    }
    else
    {
        LOG(0,"[VDEC-H265-ERR] bit depth not support\n");
        *pi4RetValue = SPS_BITDEPTH_NOT_SUPPORT;
        return SPS_BITDEPTH_NOT_SUPPORT;
    }

    prH265_SPS_DataInst->u4Log2MaxPicOrderCntLsbMinus4 = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)

    u4Code= u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    for( i = 0; i <= prH265_SPS_DataInst->u4MaxTLayersMinus1; i++)
    {
        prH265_SPS_DataInst->u4MaxDecPicBuffering[i] = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID) + 1;//ue(v)
        prH265_SPS_DataInst->u4NumReorderPics[i] = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
        prH265_SPS_DataInst->u4MaxLatencyIncrease[i] = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)     
        if  ( 1 != u4Code )
        {
            for ( i++; i <= prH265_SPS_DataInst->u4MaxTLayersMinus1; i++)
            {
                 prH265_SPS_DataInst->u4MaxDecPicBuffering[i] = prH265_SPS_DataInst->u4MaxDecPicBuffering[0];
                 prH265_SPS_DataInst->u4NumReorderPics[i] = prH265_SPS_DataInst->u4NumReorderPics[0];
                 prH265_SPS_DataInst->u4MaxLatencyIncrease[i] = prH265_SPS_DataInst->u4MaxLatencyIncrease[0];
            }
            break;
        }
    }

    prH265_SPS_DataInst->u4Log2MinCodingBlockSizeMinus3 = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
    prH265_SPS_DataInst->u4Log2DiffMaxMinCodingBlockSize = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
    prH265_SPS_DataInst->u4MaxCUWidth = 1<<(prH265_SPS_DataInst->u4Log2MinCodingBlockSizeMinus3 + 3 + prH265_SPS_DataInst->u4Log2DiffMaxMinCodingBlockSize);
    prH265_SPS_DataInst->u4MaxCUHeight =  1<<(prH265_SPS_DataInst->u4Log2MinCodingBlockSizeMinus3 + 3 + prH265_SPS_DataInst->u4Log2DiffMaxMinCodingBlockSize);

    prH265_SPS_DataInst->u4Log2MinTransformBlockSizeMinus2 = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);    //ue(v)
    prH265_SPS_DataInst->u4Log2DiffMaxMinTtransformBlockSize = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);  //ue(v)
    prH265_SPS_DataInst->u4MaxTrSize = (1<<(prH265_SPS_DataInst->u4Log2MinTransformBlockSizeMinus2+2 + prH265_SPS_DataInst->u4Log2DiffMaxMinTtransformBlockSize  ) );

    prH265_SPS_DataInst->u4QuadtreeTUMaxDepthInter = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID) + 1;  //ue(v)
    prH265_SPS_DataInst->u4QuadtreeTUMaxDepthIntra = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID) + 1;  //ue(v)
    
    while( ( prH265_SPS_DataInst->u4MaxCUWidth >> prH265_SPS_DataInst->u4Log2DiffMaxMinCodingBlockSize ) > ( 1 << ( prH265_SPS_DataInst->u4Log2MinTransformBlockSizeMinus2+2 + u4AddCUDepth )  ) )
    {
        u4AddCUDepth++;
    }
    
    prH265_SPS_DataInst->u4MaxCUDepth = u4AddCUDepth + prH265_SPS_DataInst->u4Log2DiffMaxMinCodingBlockSize;
    prH265_SPS_DataInst->bScalingListFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)

    if( prH265_SPS_DataInst->bScalingListFlag )
    {
        prH265_SPS_DataInst->bScalingListPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
        if (prH265_SPS_DataInst->bScalingListPresentFlag )
        {
             //parse scaling list
            if ( SPS_OK != vVDecH265SL_Rbsp(u4InstID,u4VDecID, &(prH265_SPS_DataInst->rSPS_ScalingList)) )
            {
                *pi4RetValue = SPS_SL_SYNTAX_ERROR;
                return SPS_SL_SYNTAX_ERROR;
            }
        }
    }

    prH265_SPS_DataInst->bUseAMP = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    prH265_SPS_DataInst->bUseSAO = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    prH265_SPS_DataInst->bUsePCM = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    if ( prH265_SPS_DataInst->bUsePCM )
    {
        prH265_SPS_DataInst->u4PCMBitDepthLumaMinus1 = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 4);//u(4)
        prH265_SPS_DataInst->u4PCMBitDepthChromaMinus1 = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 4);//u(4)

        prH265_SPS_DataInst->u4PCMLog2LumaMinSizeMinus3 = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
        prH265_SPS_DataInst->u4PCMLog2LumaMaxSize = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
        prH265_SPS_DataInst->u4PCMLog2LumaMaxSize += prH265_SPS_DataInst->u4PCMLog2LumaMinSizeMinus3+3;

        prH265_SPS_DataInst->bPCMFilterDisableFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    }

    //parse short term RPS
    prH265_SPS_DataInst->u4NumShortTermRefPicSets = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
    if (prH265_SPS_DataInst->u4NumShortTermRefPicSets > 64)
    {
        prH265_SPS_DataInst->u4NumShortTermRefPicSets = 64;
    }

    for(u4Idx = 0; u4Idx < prH265_SPS_DataInst->u4NumShortTermRefPicSets; u4Idx++)
    {   
        if ( SPS_OK != vVDecH265RPS_Rbsp(u4InstID,u4VDecID, prH265_SPS_DataInst, prH265_SPS_DataInst->pShortTermRefPicSets[u4Idx],&(prH265DrvDecInfo->rH265PicInfo),u4Idx) )
        {
            *pi4RetValue = SPS_RPS_SYNTAX_ERROR;
            return SPS_RPS_SYNTAX_ERROR; 
        }
    }

    //parse Long term RPS
    prH265_SPS_DataInst->bLongTermRefsPresent = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    if (prH265_SPS_DataInst->bLongTermRefsPresent)
    {
        prH265_SPS_DataInst->u4NumLongTermRefPicSPS = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
        for(u4Idx = 0; u4Idx < prH265_SPS_DataInst->u4NumLongTermRefPicSPS; u4Idx++)
        {
            prH265_SPS_DataInst->u4LtRefPicPocLsbSps[u4Idx] = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, prH265_SPS_DataInst->u4Log2MaxPicOrderCntLsbMinus4 + 4);//u(BitsForPOC)
            prH265_SPS_DataInst->bUsedByCurrPicLtSPSFlag[u4Idx] = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
        }
    }

    prH265_SPS_DataInst->u4NumRefFrames = prH265_SPS_DataInst->u4NumLongTermRefPicSPS + prH265DrvDecInfo->rH265PicInfo.i4MaxStrNumNegPosPics;
    prH265_SPS_DataInst->bTMVPFlagsPresent = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    prH265_SPS_DataInst->bUseStrongIntraSmoothing = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    prH265_SPS_DataInst->bVuiParametersPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)

    if ( prH265_SPS_DataInst->bVuiParametersPresentFlag )
    {
         vVDecH265VUI_Rbsp(u4InstID,u4VDecID,&(prH265_SPS_DataInst->rVUI), prH265_SPS_DataInst->u4MaxTLayersMinus1+1);
    }

    prH265_SPS_DataInst->bSPSExtensionFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    vVDEC_HAL_H265_TrailingBits(0, u4VDecID);
    
    prH265_SPS_DataInst->bSPSValid = TRUE;

    return SPS_OK;
}

/************************************************************/
/**/
/*u4InstID : Instant ID*/
/*u4VDecID : HW ID (0:VLD0; 1:VLD1; 2:LAE0; 3:LAE1)*/
/**/
/*************************************************************/
void vVDecH265PTL_Rbsp(UINT32 u4InstID, UINT32 u4VDecID,pH265_PTL_Data pPTL,INT32 i4MaxNumSubLayersMinus1)
{
    INT32 i4tmp = 0,i4tmp2 = 0;
    INT32 i = 0;
    INT32 i4Idx  = 0;

    if (pPTL->bProfilePresentFlag)
    {
        pPTL->generalPTL.u4ProfileSpace = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 2);//u(2)
        pPTL->generalPTL.bTierFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
        pPTL->generalPTL.u4ProfileIdc= u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 5);//u(5)
        for( i = 0; i < 32; i++)
        {
            pPTL->generalPTL.bProfileCompatibilityFlag[i] = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
        }

        pPTL->generalPTL.bProgressiveSourceFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
        pPTL->generalPTL.bInterlacedSourceFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
        pPTL->generalPTL.bNonPackedConstraintFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
        pPTL->generalPTL.bFrameOnlyConstraintFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)

        i4tmp = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 32);   //u(44)
        i4tmp2 = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 12);
        
        if ( i4tmp ||i4tmp2 )
        {                    
            LOG(0,"[VDEC-H265-ERR] Reserved 44bit[0] conflict @ %d\n",__LINE__);
        }
        
    }

    pPTL->generalPTL.u4LevelIdc= u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 8);//u(8)

    
    for(i4Idx = 0; i4Idx < i4MaxNumSubLayersMinus1; i4Idx++)
    {
        pPTL->bSubLayerProfilePresentFlag[i4Idx] = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
        pPTL->bSubLayerLevelPresentFlag[i4Idx] = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
    }

    if ( 0 < i4MaxNumSubLayersMinus1 )
    {
        for(i4Idx = i4MaxNumSubLayersMinus1; i4Idx < 8; i4Idx++)
        {
            i4tmp = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 2);//u(2)
            if (i4tmp)
            {
                LOG(0,"[VDEC-H265-ERR] Reserved 2bit conflict\n");
            }
        }
    }

    // sub level 
    for(i4Idx = 0; i4Idx < i4MaxNumSubLayersMinus1; i4Idx++)
    {
        if( pPTL->bProfilePresentFlag && pPTL->bSubLayerProfilePresentFlag[i4Idx] )
        {
            pPTL->subLayerPTL[i4Idx].u4ProfileSpace = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 2);//u(2)
            pPTL->subLayerPTL[i4Idx].bTierFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
            pPTL->subLayerPTL[i4Idx].u4ProfileIdc= u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 5);//u(5)

            for( i = 0; i < 32; i++)
            {
                pPTL->subLayerPTL[i4Idx].bProfileCompatibilityFlag[i] = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)

            }

            pPTL->subLayerPTL[i4Idx].bProgressiveSourceFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
            pPTL->subLayerPTL[i4Idx].bInterlacedSourceFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
            pPTL->subLayerPTL[i4Idx].bNonPackedConstraintFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
            pPTL->subLayerPTL[i4Idx].bFrameOnlyConstraintFlag = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)

            i4tmp = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 32);   //u(44)
            i4tmp2 = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 12);
            
            if ( i4tmp ||i4tmp2 )
            {                    
                LOG(0,"[VDEC-H265-ERR] Reserved 44bit[1] conflict @ %d\n",__LINE__);
            }
       }
       
       if( pPTL->bSubLayerLevelPresentFlag[i4Idx] )
       {
            pPTL->subLayerPTL[i4Idx].u4LevelIdc= u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 8);//u(8)

       }
    }

//Temp still image check
#if 0
    if(pPTL->generalPTL.u4ProfileIdc == 3 || pPTL->generalPTL.bProfileCompatibilityFlag[3] == 1)
    {
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.fgIsStillPicture = true;
    }
#endif

}

/************************************************************/
/**/
/*u4InstID : Instant ID*/
/************************************************************/
void vVDecH265InitSPS(UINT32 u4InstID,H265_SPS_Data *prSPS)
{
    INT32 i;
    INT32 i4sizeId,i4listId;
    INT32 i4RpsIdx;
    //INT32 ****pi4SPSScalingList = NULL;
    
    prSPS->u4MaxCUWidth = 32;
    prSPS->u4MaxCUHeight = 32;
    prSPS->u4MaxCUDepth = 3;
    prSPS->u4PCMLog2LumaMaxSize = 5;
    prSPS->u4PCMLog2LumaMinSizeMinus3 = 4;
    prSPS->u4ChromaFormatIdc = 1;
    prSPS->u4BitDepthLumaMinus8 = 0;
    prSPS->u4BitDepthChromaMinus8 = 0;
    prSPS->u4PCMBitDepthLumaMinus1 = 7;
    prSPS->u4PCMBitDepthChromaMinus1= 7;
    prSPS->u4BitDepthLumaMinus8 = 0;
    prSPS->u4BitDepthChromaMinus8 = 0;
    prSPS->u4Log2MaxPicOrderCntLsbMinus4 = 4;
    prSPS->u4MaxTrSize = 32;
    prSPS->u4NumLongTermRefPicSPS = 0;
    prSPS->u4NumShortTermRefPicSets = 0;
    prSPS->u4NumRefFrames = 0;

    for ( i = 0; i < MAX_TLAYER; i++ ) 
    {
        prSPS->u4MaxLatencyIncrease[i] = 0;
        prSPS->u4MaxDecPicBuffering[i] = 1;
        prSPS->u4NumReorderPics[i] = 0;
    }
    
    // init scaling list
    if(prSPS->u4SPSId >= H265_MAX_SPS_NUM)
    {
        LOG(0,"[VDEC-H265-ERROR] Sps ID too large, reallocate scaling list arry!\n");
        ASSERT(0);
    }
    
    for( i4sizeId = 0; i4sizeId < SCALING_LIST_SIZE_NUM; i4sizeId++)
    {
        for( i4listId = 0; i4listId < H265_scalingListNum[i4sizeId]; i4listId++)
        {
            //prSPS->rSPS_ScalingList.pScalingListDeltaCoef[i4sizeId][i4listId] = pi4SPSScalingList[prSPS->u4SPSId][i4sizeId][i4listId];
            prSPS->rSPS_ScalingList.pScalingListDeltaCoef[i4sizeId][i4listId] = prSPSScalingListDeltaCoef[u4InstID][prSPS->u4SPSId][i4sizeId][i4listId];
        }
    }
    prSPS->rSPS_ScalingList.pScalingListDeltaCoef[SCALING_LIST_32x32][3] = prSPS->rSPS_ScalingList.pScalingListDeltaCoef[SCALING_LIST_32x32][1];
    prSPS->bSL_Init = 1;

    //assign RPS array
    for(i4RpsIdx = 0; i4RpsIdx < H265_MAX_RPS_NUM; i4RpsIdx++)
    {
        prSPS->pShortTermRefPicSets[i4RpsIdx] =  prH265RPS[u4InstID][prSPS->u4SPSId][i4RpsIdx];
    }

}

// *********************************************************************
// Function    : void vHEVCVerifyInitPicInfo(UINT32 u4InstID)
// Description : init pic info
// Parameter   : None
// Return      : None
// *********************************************************************
void vVDecH265InitPicInfo(UINT32 u4InstID,H265_DRV_INFO_T* prH265DrvDecInfo)
{
    INT32 i;
    ASSERT(prH265DrvDecInfo!=NULL);
    prH265DrvDecInfo->rH265PicInfo.u4SliceCnt = 0;
    prH265DrvDecInfo->rH265PicInfo.u4IqSramAddrAccCnt = 0;
    prH265DrvDecInfo->rH265PicInfo.u4PicWidthInCU = 0;
    prH265DrvDecInfo->rH265PicInfo.u4PicHeightInCU = 0;
    
    //_rH265PicInfo[u4InstID].i4StrNumDeltaPocs = 0;
    //_rH265PicInfo[u4InstID].i4StrNumNegPosPics = 0;
    prH265DrvDecInfo->rH265PicInfo.i4MaxStrNumNegPosPics = 0;
    prH265DrvDecInfo->rH265PicInfo.i4NumLongTerm = 0;
    prH265DrvDecInfo->rH265PicInfo.i4NumLongTermSps = 0;

    prH265DrvDecInfo->rH265PicInfo.i4DpbLTBuffCnt = 0;
    prH265DrvDecInfo->rH265PicInfo.i4RefListNum = 0;
    prH265DrvDecInfo->rH265PicInfo.bLowDelayFlag = 0;

    for(i = 0; i<16;i++)
    {
        prH265DrvDecInfo->rH265PicInfo.i4PocDiffList0[i] = 0;
        prH265DrvDecInfo->rH265PicInfo.i4PocDiffList1[i] = 0;
        prH265DrvDecInfo->rH265PicInfo.i4LongTermList0[i] = 0;
        prH265DrvDecInfo->rH265PicInfo.i4LongTermList1[i] = 0;
        prH265DrvDecInfo->rH265PicInfo.i4BuffIdList0[i] = 0;
        prH265DrvDecInfo->rH265PicInfo.i4BuffIdList1[i] = 0;
        prH265DrvDecInfo->rH265PicInfo.i4DpbLTBuffId[i] = 0;
        prH265DrvDecInfo->rH265PicInfo.i4List0DecOrderNo[i] = 0;
        prH265DrvDecInfo->rH265PicInfo.i4List1DecOrderNo[i] = 0;
    }
}

UINT32  vVDecH265SL_Rbsp(UINT32 u4InstID, UINT32 u4VDecID, H265_SL_Data* scalingList)
{
    UINT32 sizeId, listId;
    BOOL scalingListPredModeFlag;
    INT32 i4Code,refListId,listSize;
    
    //for each size
    for(sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++)
    {
        for(listId = 0; listId <  H265_scalingListNum[sizeId]; listId++)
        {
            scalingList->bScalingListPredModeFlag[sizeId][listId] = u4VDEC_HAL_H265_GetRealBitStream(0, u4VDecID, 1);//u(1)
            scalingListPredModeFlag = ( scalingList->bScalingListPredModeFlag[sizeId][listId] ) ? 1 : 0;
            if( 1 != scalingListPredModeFlag ) //Copy Mode
            {
            
                i4Code  = u4VDEC_HAL_H265_UeCodeNum(0, u4VDecID);//ue(v)
                if ((INT32)listId-i4Code >= 0 && (INT32)listId-i4Code < H265_scalingListNum[sizeId] )
                {
                    scalingList->u4RefMatrixId[sizeId][listId] = (UINT32)((INT32)listId-i4Code);
                }
                else if ( (INT32)listId-i4Code >= H265_scalingListNum[sizeId] )
                {
                    return SPS_SL_SYNTAX_ERROR;
                }
                else
                {
                    return SPS_SL_SYNTAX_ERROR;
                }

                if( sizeId > SCALING_LIST_8x8 )
                {
                    scalingList->i4ScalingListDC[sizeId][listId] = ((listId == scalingList->u4RefMatrixId[sizeId][listId])? 16 : scalingList->i4ScalingListDC[sizeId][scalingList->u4RefMatrixId[sizeId][listId]] );
                }

                refListId = scalingList->u4RefMatrixId[sizeId][listId]; 
                listSize = (MAX_MATRIX_COEF_NUM < H265_scalingListSize[sizeId])? MAX_MATRIX_COEF_NUM : H265_scalingListSize[sizeId];
                x_memcpy(scalingList->pScalingListDeltaCoef[sizeId][listId],( (listId == refListId)? 
                    vVDecH265GetSLDefaultAddress(sizeId, refListId) : scalingList->pScalingListDeltaCoef[sizeId][refListId]) , sizeof(INT32)*listSize);

           }
           else
           {   //DPCM Mode
                INT32 i,coefNum = min_hevc(MAX_MATRIX_COEF_NUM,(INT32)H265_scalingListSize[sizeId]);
                INT32 nextCoef = SCALING_LIST_START_VALUE;
                UINT32* scan  = (sizeId == 0) ? H265_auiSigLastScan [SCAN_DIAG] [1] :  H265_sigLastScanCG32x32;
                INT32* dst = scalingList->pScalingListDeltaCoef[sizeId][listId];

                if( sizeId > SCALING_LIST_8x8 )
                {
                    i4Code  = i4VDEC_HAL_H265_SeCodeNum(0, u4VDecID);//se(v)
                    scalingList->i4ScalingListDC[sizeId][listId] = i4Code+8;
                    nextCoef = scalingList->i4ScalingListDC[sizeId][listId];
                }

                for(i = 0; i < coefNum; i++)
                {
                    i4Code = i4VDEC_HAL_H265_SeCodeNum(0, u4VDecID);//se(v)
                    nextCoef = (nextCoef + i4Code + 256 ) % 256;
                    dst[scan[i]]= nextCoef;
                }
           }
        }
    }

    return SPS_OK;
}

UINT32  vVDecH265RPS_Rbsp(UINT32 u4InstID,UINT32 u4VDecID,H265_SPS_Data* pSPS,H265_RPS_Data* pRPS,VDEC_INFO_H265_PIC_INFO_T* prPic,UINT32 u4Idx)
{
    UINT32 u4Code, bit, refIdc;
    INT32 refIndex;
    BOOL interRPSPred, used;
    H265_RPS_Data* refRPS = NULL;
    
    INT32 k = 0;
    INT32 k0 = 0;
    INT32 k1 = 0;
    INT32 deltaRPS, deltaPOC, i, j;
    INT32 temp, numNegPics;
    INT32 prev, poc;

    //reset pRPS
    pRPS->u4NumberOfNegativePictures = 0;
    pRPS->u4NumberOfPositivePictures = 0;
    pRPS->u4NumberOfPictures = 0;
    pRPS->u4NumberOfLongtermPictures = 0;
    pRPS->u4DeltaRIdxMinus1 = 0;
    pRPS->u4DeltaRPS = 0;
    pRPS->bInterRPSPrediction = 0;
    pRPS->u4NumRefIdc = 0;

    if ( 0 < u4Idx )
    {
        pRPS->bInterRPSPrediction = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    }
    else 
    {
        pRPS->bInterRPSPrediction = 0;
    }
    interRPSPred = pRPS->bInterRPSPrediction;

    if(interRPSPred)
    {
        if( u4Idx ==  pSPS->u4NumShortTermRefPicSets )
        {
            u4Code = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        }
        else
        {
            u4Code = 0;
        }

        if (u4Code > u4Idx-1)
        {
            return SPS_RPS_SYNTAX_ERROR;
        }
        
        refIndex = u4Idx -1 - u4Code;
        if ( refIndex > u4Idx -1 || refIndex < 0 )
        {
            return SPS_RPS_SYNTAX_ERROR;
        }
        refRPS = pSPS->pShortTermRefPicSets[refIndex];

        if ( refRPS==NULL )
        {
            return SPS_RPS_SYNTAX_ERROR;
        }

        bit = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        u4Code= u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        deltaRPS = (1 - (bit<<1)) * (u4Code + 1); // delta_RPS

        prPic->i4StrNumDeltaPocs = refRPS->u4NumberOfNegativePictures+refRPS->u4NumberOfPositivePictures;

        for ( i = 0; i <= (refRPS->u4NumberOfNegativePictures+refRPS->u4NumberOfPositivePictures); i++ )
        {
            bit = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
            refIdc = bit;
            if (refIdc == 0) 
            {
                bit = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
                refIdc = bit<<1; //second bit is "1" if refIdc is 2, "0" if refIdc = 0.
            }
            if (refIdc == 1 || refIdc == 2)
            {
                deltaPOC = deltaRPS + (( i < (refRPS->u4NumberOfNegativePictures+refRPS->u4NumberOfPositivePictures) )? refRPS->i4DeltaPOC[i]  : 0);
                pRPS->i4DeltaPOC[k] = deltaPOC;
                pRPS->bUsed[k] = ( refIdc == 1 );
                
                if (deltaPOC < 0) 
                {
                    k0++;
                }
                else
                {
                    k1++;
                }
                k++;
            }
            pRPS->u4RefIdc[i] = refIdc;
        }

        pRPS->u4NumRefIdc = refRPS->u4NumberOfPictures + 1;
        pRPS->u4NumberOfPictures = k;
        pRPS->u4NumberOfNegativePictures = k0;
        pRPS->u4NumberOfPositivePictures = k1;

        // sort in increasing order (smallest first)
        for( j=1; j < pRPS->u4NumberOfPictures; j++)
        { 
            deltaPOC = pRPS->i4DeltaPOC[j];
            used = pRPS->bUsed[j];
             for ( k=j-1; k >= 0; k--)
            {
                temp = pRPS->i4DeltaPOC[k];
                if (deltaPOC < temp)
                {
                    pRPS->i4DeltaPOC[k+1] = temp;
                    pRPS->bUsed[k+1] = pRPS->bUsed[k];
                    pRPS->i4DeltaPOC[k] = deltaPOC;
                    pRPS->bUsed[k] = used;
                }
            }
        }
        // flip the negative values to largest first
        numNegPics = pRPS->u4NumberOfNegativePictures;
        for( j=0, k=numNegPics-1; j < numNegPics>>1; j++, k--)
        { 
            deltaPOC = pRPS->i4DeltaPOC[j];
            used = pRPS->bUsed[j];
            pRPS->i4DeltaPOC[j] = pRPS->i4DeltaPOC[k];
            pRPS->bUsed[j] = pRPS->bUsed[k] ;
            pRPS->i4DeltaPOC[k] = deltaPOC;
            pRPS->bUsed[k] = used;
        }

    }
    else
    {

        pRPS->u4NumberOfNegativePictures = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        pRPS->u4NumberOfPositivePictures = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        
        prPic->i4StrNumNegPosPics = pRPS->u4NumberOfNegativePictures + pRPS->u4NumberOfPositivePictures;
        prPic->i4MaxStrNumNegPosPics = (prPic->i4MaxStrNumNegPosPics < prPic->i4StrNumNegPosPics)?
                                                                prPic->i4StrNumNegPosPics : prPic->i4MaxStrNumNegPosPics;
        if ( prPic->i4StrNumNegPosPics > 32 ){
            return SPS_RPS_SYNTAX_ERROR;
        }
        
        prev = 0;
        for( j=0 ; j < pRPS->u4NumberOfNegativePictures; j++)
        {
            u4Code = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
            poc = prev-u4Code-1;
            prev = poc;
            pRPS->i4DeltaPOC[j] = poc;
            pRPS->bUsed[j] = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }
        prev = 0;
        for( j=pRPS->u4NumberOfNegativePictures; j < pRPS->u4NumberOfNegativePictures + pRPS->u4NumberOfPositivePictures; j++)
        {
            u4Code = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
            poc = prev+u4Code+1;
            prev = poc;
            pRPS->i4DeltaPOC[j] = poc;
            pRPS->bUsed[j] = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }

        pRPS->u4NumberOfPictures = pRPS->u4NumberOfNegativePictures + pRPS->u4NumberOfPositivePictures;
    }

    return SPS_OK;
}

void vVDecH265VUI_Rbsp( UINT32 u4InstID,UINT32 u4VDecID,H265_VUI_Data* VUI,UINT32 MaxTLayers)
{
    ASSERT(VUI!=NULL);
    VUI->bAspectRatioInfoPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    if (VUI->bAspectRatioInfoPresentFlag)
    {
        VUI->i4AspectRatioIdc = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 8);//u(8)
        if (VUI->i4AspectRatioIdc  == 255)
        {
            VUI->i4SarWidth= u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 16);//u(16)
            VUI->i4SarHeight = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 16);//u(16)
        }
    }

    VUI->bOverscanInfoPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    if (VUI->bOverscanInfoPresentFlag)
    {
        VUI->bOverscanAppropriateFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    }

    VUI->bVideoSignalTypePresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    if (VUI->bVideoSignalTypePresentFlag )
    {
        VUI->i4VideoFormat = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 3);//u(3)
        VUI->bVideoFullRangeFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        VUI->bColourDescriptionPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        if (VUI->bColourDescriptionPresentFlag )
        {
            VUI->i4ColourPrimaries = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 8);//u(8)
            VUI->i4TransferCharacteristics = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 8);//u(8)
            VUI->i4MatrixCoefficients = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 8);//u(8)
        }
    }

    VUI->bChromaLocInfoPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    if (VUI->bChromaLocInfoPresentFlag )
    {
        VUI->i4ChromaSampleLocTypeTopField = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        VUI->i4ChromaSampleLocTypeBottomField = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
    }

    VUI->bNeutralChromaIndicationFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    VUI->bFieldSeqFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
   
    VUI->bFrameFieldInfoPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    VUI->bDefaultDisplayWindowEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)

    if (VUI->bDefaultDisplayWindowEnabledFlag )
    {
        VUI->i4DefaultDisplayWinLeftOffset = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        VUI->i4DefaultDisplayWinRightOffset = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        VUI->i4DefaultDisplayWinTopOffset = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        VUI->i4DefaultDisplayWinBottomOffset = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
    }

    VUI->bTimingInfoPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    if( VUI->bTimingInfoPresentFlag )
    {
        VUI->u4NumUnitsInTick = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 32);//u(32)
        VUI->u4TimeScale = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 32);//u(32)
        VUI->bPocProportionalToTimingFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        if(VUI->bPocProportionalToTimingFlag)
        {
            VUI->i4NumTicksPocDiffOneMinus1 = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        }
        VUI->bHrdParametersPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        if( VUI->bHrdParametersPresentFlag )
        {
          vVDecH265HRD_Rbsp(u4InstID,u4VDecID,&VUI->rHdrParameters, 1, MaxTLayers -1 );
        }
    }

     VUI->bBitstreamRestrictionFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    if ( VUI->bBitstreamRestrictionFlag )
    {
        VUI->bTilesFixedStructureFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        VUI->bMotionVectorsOverPicBoundariesFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        VUI->bRestrictedRefPicListsFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        
        VUI->i4MinSpatialSegmentationIdc = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        VUI->i4MaxBytesPerPicDenom = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        VUI->i4MaxBitsPerMinCuDenom = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        VUI->i4Log2MaxMvLengthHorizontal = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        VUI->i4Log2MaxMvLengthVertical = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)

    }
}


void vVDecH265HRD_Rbsp( UINT32 u4InstID,UINT32 u4VDecID,H265_HRD_Data* HRD,BOOL bCommonInfPresentFlag ,UINT32 MaxTLayersMinus1)
{
    INT32 i, j, nalOrVcl;
    
    if( bCommonInfPresentFlag )
    {
        HRD->bNalHrdParametersPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        HRD->bVclHrdParametersPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)

        if( HRD->bNalHrdParametersPresentFlag || HRD->bVclHrdParametersPresentFlag )
        {
            HRD->bSubPicCpbParamsPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
            if( HRD->bSubPicCpbParamsPresentFlag )
            {
                HRD->u4TickDivisorMinus2 = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 8);//u(8)
                HRD->u4DuCpbRemovalDelayLengthMinus1 = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 5);//u(5)
                HRD->bSubPicCpbParamsInPicTimingSEIFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
                HRD->u4DpbOutputDelayDuLengthMinus1 = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 5);//u(5)
            }
            HRD->u4BitRateScale = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 4);//u(4)
            HRD->u4CpbSizeScale = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 4);//u(4)
            if( HRD->bSubPicCpbParamsPresentFlag)
            {
                HRD->u4DucpbSizeScale = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 4);//u(4)
            }
            HRD->u4InitialCpbRemovalDelayLengthMinus1 = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 5);//u(5)
            HRD->u4AuCpbRemovalDelayLengthMinus1 = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 5);//u(5)
            HRD->u4DpbOutputDelayLengthMinus1 = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 5);//u(5)
        }
    }


    for( i = 0; i <= MaxTLayersMinus1; i ++ )
    {
        HRD->rSubLayerHRD[i].bFixedPicRateFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        if( 1 != HRD->rSubLayerHRD[i].bFixedPicRateFlag )
        {
            HRD->rSubLayerHRD[i].bFixedPicRateWithinCvsFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }
        else
        {
            HRD->rSubLayerHRD[i].bFixedPicRateWithinCvsFlag = 1;
        }
        HRD->rSubLayerHRD[i].bLowDelayHrdFlag = 0;
        HRD->rSubLayerHRD[i].u4CpbCntMinus1 = 0;

        if( HRD->rSubLayerHRD[i].bFixedPicRateWithinCvsFlag )
        {
            HRD->rSubLayerHRD[i].u4ElementalDurationInTcMinus1 = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        }
        else
        { 
            HRD->rSubLayerHRD[i].bLowDelayHrdFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }
            
        if ( 1 != HRD->rSubLayerHRD[i].bLowDelayHrdFlag )
        {
            HRD->rSubLayerHRD[i].u4CpbCntMinus1 = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        }

        for( nalOrVcl = 0; nalOrVcl < 2; nalOrVcl ++ )
        {
            if( ( ( nalOrVcl == 0 ) && ( HRD->bNalHrdParametersPresentFlag ) ) ||
                ( ( nalOrVcl == 1 ) && ( HRD->bVclHrdParametersPresentFlag ) ) )
            {
                for( j = 0; j <= (HRD->rSubLayerHRD[i].u4CpbCntMinus1); j ++ )
                {
                    HRD->rSubLayerHRD[i].u4BitRateValueMinus1[j][nalOrVcl] = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
                    HRD->rSubLayerHRD[i].u4CpbSizeValueMinus1[j][nalOrVcl] = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
                    if( HRD->bSubPicCpbParamsPresentFlag )
                    {
                        //[notice] spec code confilction !?
                        HRD->rSubLayerHRD[i].u4DuBitRateValueMinus1[j][nalOrVcl] = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
                        HRD->rSubLayerHRD[i].u4DucpbSizeValueMinus1[j][nalOrVcl] = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
                    }
                    HRD->rSubLayerHRD[i].bCbrFlag[j][nalOrVcl]  = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
                }
            }
        }
    }
    
}


INT32 vVDecH265PPSPsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue)
{
    UINT32 u4PicParameterSetId;
    INT32 i;
    H265_SPS_Data *prH265_SPS_DataInst;
    H265_PPS_Data *prH265_PPS_DataInst;

    u4PicParameterSetId = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
    if(u4PicParameterSetId < H265_MAX_PPS_NUM)
    {
        prH265_PPS_DataInst = &(prH265DrvDecInfo->arH265PPS[u4PicParameterSetId]);
        prH265_PPS_DataInst->u4PicParameterSetId = u4PicParameterSetId;
        prH265_PPS_DataInst->bPPSValid = FALSE;// FALSE until set completely
    }
    else
    {
        LOG(0,"[VDEC-H265-ERR] PPS id [%d] invalid\n",u4PicParameterSetId);
        return PPS_ID_SYNTAX_ERROR;
    }

    prH265_PPS_DataInst->u4SeqParameterSetId = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
    if ( prH265_PPS_DataInst->u4SeqParameterSetId < H265_MAX_SPS_NUM)
    {
        prH265_SPS_DataInst = &(prH265DrvDecInfo->arH265SPS[prH265_PPS_DataInst->u4SeqParameterSetId]);
    }
    else 
    {
        //error handle, choose one valid sps info
        for(i = 0; i < H265_MAX_SPS_NUM; i++)
        {
            if(prH265DrvDecInfo->arH265SPS[i].bSPSValid)
            {
                prH265_SPS_DataInst = &(prH265DrvDecInfo->arH265SPS[i]);
                break;
            }
        }

        if(i >= H265_MAX_SPS_NUM)
        {
            LOG(0,"[VDEC-H265-ERR] No valid SPS info\n");
            return PPS_NO_SPS_INFO_ERROR;
        }
        else
        {
            LOG(0,"[VDEC-H265-ERR] use SPS[%d] replace SPS[%d] for error SPS ID\n",(i - 1),prH265_PPS_DataInst->u4SeqParameterSetId);
        }
        
    }
    UNUSED(prH265_SPS_DataInst);

    vVDecH265InitPPS(u4InstID,prH265_PPS_DataInst);

    prH265_PPS_DataInst->bDependentSliceSegmentsEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bOutputFlagPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->u4NumExtraSliceHeaderBits = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 3);//u(3)

    prH265_PPS_DataInst->bSignHideFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bCabacInitPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)

    prH265_PPS_DataInst->u4NumRefIdxL0DefaultActiveMinus1 = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
    if ( prH265_PPS_DataInst->u4NumRefIdxL0DefaultActiveMinus1 > 14 )
    {
        return PPS_REF_NUM_ERROR;
    }
    prH265_PPS_DataInst->u4NumRefIdxL1DefaultActiveMinus1 = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
    if ( prH265_PPS_DataInst->u4NumRefIdxL1DefaultActiveMinus1 > 14 )
    {
        return PPS_REF_NUM_ERROR;
    }

    prH265_PPS_DataInst->i4PicInitQPMinus26 = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)

    prH265_PPS_DataInst->bConstrainedIntraPredFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bTransformSkipEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bCuQPDeltaEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    if( prH265_PPS_DataInst->bCuQPDeltaEnabledFlag )
    {
        prH265_PPS_DataInst->u4DiffCuQPDeltaDepth = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
    }
    else
    {
        prH265_PPS_DataInst->u4DiffCuQPDeltaDepth = 0;
    }

    prH265_PPS_DataInst->i4PPSCbQPOffset = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)
    if ( prH265_PPS_DataInst->i4PPSCbQPOffset < -12 || prH265_PPS_DataInst->i4PPSCbQPOffset > 12 )
    {
        return PPS_QP_ERROR;
    }
    prH265_PPS_DataInst->i4PPSCrQPOffset = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)
    if ( prH265_PPS_DataInst->i4PPSCrQPOffset < -12 || prH265_PPS_DataInst->i4PPSCrQPOffset > 12 )
    {
        return PPS_QP_ERROR;
    }

    prH265_PPS_DataInst->bPPSSliceChromaQpFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bWPPredFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bWPBiPredFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bTransquantBypassEnableFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bTilesEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bEntropyCodingSyncEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)

    if( prH265_PPS_DataInst->bTilesEnabledFlag )
    {
        prH265_PPS_DataInst->u4NumColumnsMinus1 = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        prH265_PPS_DataInst->u4NumRowsMinus1 = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        prH265_PPS_DataInst->bUniformSpacingFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)

        if( 1 != prH265_PPS_DataInst->bUniformSpacingFlag )
        {   
            for( i = 0; i < prH265_PPS_DataInst->u4NumColumnsMinus1; i++)
            { 
                prH265_PPS_DataInst->u4ColumnWidthMinus1[i] = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
            }

            for( i = 0; i < prH265_PPS_DataInst->u4NumRowsMinus1; i++)
            { 
                prH265_PPS_DataInst->u4RowHeightMinus1[i] = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
            }
            
        }
        
        if( 0 != prH265_PPS_DataInst->u4NumColumnsMinus1 || 0 != prH265_PPS_DataInst->u4NumRowsMinus1 )
        {
            prH265_PPS_DataInst->bLoopFilterAcrossTilesEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }
    }

    prH265_PPS_DataInst->bLoopFilterAcrossSlicesEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bDeblockingFilterControlPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    if( prH265_PPS_DataInst->bDeblockingFilterControlPresentFlag )
    {
        prH265_PPS_DataInst->bDeblockingFilterOverrideEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        prH265_PPS_DataInst->bPicDisableDeblockingFilterFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        if( 1 != prH265_PPS_DataInst->bPicDisableDeblockingFilterFlag )
        {
            prH265_PPS_DataInst->i4DeblockingFilterBetaOffsetDiv2 = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)
            prH265_PPS_DataInst->i4DeblockingFilterTcOffsetDiv2 = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)
        }
    }

    prH265_PPS_DataInst->bPPSScalingListPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    if( prH265_PPS_DataInst->bPPSScalingListPresentFlag )
    {
        if ( SPS_OK != vVDecH265SL_Rbsp(u4InstID,u4VDecID, &(prH265_PPS_DataInst->rPPS_ScalingList)) )
        {
            return PPS_SCL_ERROR;
        }
    }

    prH265_PPS_DataInst->bListsModificationPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->u4Log2ParallelMergeLevelMinus2 = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
    prH265_PPS_DataInst->bSliceHeaderExtensionPresentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    prH265_PPS_DataInst->bPPSExtensionFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    vVDEC_HAL_H265_TrailingBits(0,u4VDecID);

    prH265_PPS_DataInst->bPPSValid = TRUE;
    return PPS_OK;

}

void vVDecH265InitPPS(UINT32 u4InstID,H265_PPS_Data *prPPS)
{
    INT32 i4SizeId,i4ListId;

    for( i4SizeId = 0; i4SizeId < SCALING_LIST_SIZE_NUM; i4SizeId++)
    {
        for( i4ListId = 0; i4ListId < H265_scalingListNum[i4SizeId]; i4ListId++)
        {
            //prPPS->rPPS_ScalingList.pScalingListDeltaCoef[i4SizeId][i4ListId] = pi4PPSScalingList[prPPS->u4PicParameterSetId][i4SizeId][i4ListId];
            prPPS->rPPS_ScalingList.pScalingListDeltaCoef[i4SizeId][i4ListId] = prPPSScalingListDeltaCoef[u4InstID][prPPS->u4PicParameterSetId][i4SizeId][i4ListId];
        }
    }
    prPPS->rPPS_ScalingList.pScalingListDeltaCoef[SCALING_LIST_32x32][3] = prPPS->rPPS_ScalingList.pScalingListDeltaCoef[SCALING_LIST_32x32][1];
    prPPS->bSL_Init = 1; 
    prPPS->u4NumColumnsMinus1 = 0;
    prPPS->u4NumRowsMinus1 = 0;
    prPPS->bLoopFilterAcrossTilesEnabledFlag = 1;
}

INT32 i4VDecH265SlimSlicePsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue)
{
    UINT32 u4NalType = 0;
    UINT32 u4Code;
    H265_SPS_Data *prH265_SPS_DataInst;
    H265_PPS_Data *prH265_PPS_DataInst;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    *pi4RetValue = SLICE_OK;
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    prH265_Slice_Hdr_DataInst = &(prH265DrvDecInfo->rH265SliceHdr);
    prH265DecPrm->prSliceHdr = prH265_Slice_Hdr_DataInst;
    //prH265DecPrm->prFBufInfo = &(prH265DrvDecInfo->arH265FbInfo[0]);

    u4NalType = prH265DecPrm->u4NalType;
    prH265_Slice_Hdr_DataInst->u4NalType = u4NalType;
    prH265_Slice_Hdr_DataInst->bFirstSliceSegmentInPic = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)

    if( prH265_Slice_Hdr_DataInst->bFirstSliceSegmentInPic )
    {
        prH265_Slice_Hdr_DataInst->bDeblockingFilterDisableFlag = 0;
        prH265_Slice_Hdr_DataInst->bDeblockingFilterOverrideFlag = 1;
    }

    if ( 16 <= u4NalType && 23>= u4NalType  )
    {      //IRAP picture
        u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        UNUSED(u4Code);
    }
    prH265_Slice_Hdr_DataInst->u4PPSID = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)

    if((prH265_Slice_Hdr_DataInst->u4PPSID < H265_MAX_PPS_NUM)
        && (prH265DrvDecInfo->arH265PPS[prH265_Slice_Hdr_DataInst->u4PPSID].bPPSValid)
        && (prH265DrvDecInfo->arH265SPS[prH265DrvDecInfo->arH265PPS[prH265_Slice_Hdr_DataInst->u4PPSID].u4SeqParameterSetId].bSPSValid))
    {
        prH265_SPS_DataInst = &(prH265DrvDecInfo->arH265SPS[prH265DrvDecInfo->arH265PPS[prH265_Slice_Hdr_DataInst->u4PPSID].u4SeqParameterSetId]);
        prH265_PPS_DataInst = &(prH265DrvDecInfo->arH265PPS[prH265_Slice_Hdr_DataInst->u4PPSID]);
        prH265DecPrm->prSPS = prH265_SPS_DataInst;
        prH265DecPrm->prPPS = prH265_PPS_DataInst;
    }
    else
    {
        LOG(0,"[VDEC-H265-ERROR-%d] Slice Hdr PPS Num[%d] err \n",u4InstID,prH265_Slice_Hdr_DataInst->u4PPSID);
        if((prH265_Slice_Hdr_DataInst->u4PPSID < H265_MAX_PPS_NUM) && prH265DrvDecInfo->arH265PPS[prH265_Slice_Hdr_DataInst->u4PPSID].bPPSValid)// given PPS id is valid
        {
            prH265_PPS_DataInst = &(prH265DrvDecInfo->arH265PPS[prH265_Slice_Hdr_DataInst->u4PPSID]);
            prH265DecPrm->prPPS = prH265_PPS_DataInst;
        } 
        else if(prH265DrvDecInfo->arH265PPS[0].bPPSValid) // PPS id = 0 is valid
        {
            LOG(0,"[VDEC-H265-ERROR]Force PPS[0] replace PPS[%d] \n",prH265_Slice_Hdr_DataInst->u4PPSID);
            prH265_PPS_DataInst = &(prH265DrvDecInfo->arH265PPS[0]);
            prH265DecPrm->prPPS = prH265_PPS_DataInst;
        } 
        else 
        {
            LOG(0,"[VDEC-H265-ERROR]No valid PPS info \n");
            *pi4RetValue = SLICE_PPS_ID_ERROR;
            return SLICE_PPS_ID_ERROR;
        }
        
        if ((prH265_Slice_Hdr_DataInst->u4PPSID < H265_MAX_PPS_NUM) &&
            (prH265DrvDecInfo->arH265PPS[prH265_Slice_Hdr_DataInst->u4PPSID].u4SeqParameterSetId < H265_MAX_SPS_NUM) &&
            prH265DrvDecInfo->arH265SPS[prH265DrvDecInfo->arH265PPS[prH265_Slice_Hdr_DataInst->u4PPSID].u4SeqParameterSetId].bSPSValid)
        { 
            prH265_SPS_DataInst = &(prH265DrvDecInfo->arH265SPS[prH265DrvDecInfo->arH265PPS[prH265_Slice_Hdr_DataInst->u4PPSID].u4SeqParameterSetId]);
            prH265DecPrm->prSPS = prH265_SPS_DataInst;
        }
        else if(prH265DrvDecInfo->arH265SPS[0].bSPSValid)// SPS id = 0 is valid
        {
            LOG(0,"[VDEC-H265-ERROR]Force SPS[0] \n");
            prH265_SPS_DataInst = &(prH265DrvDecInfo->arH265SPS[0]);
            prH265DecPrm->prSPS = prH265_SPS_DataInst;
        }
        else 
        {
            LOG(0,"[VDEC-H265-ERROR]No valid SPS info \n");
            *pi4RetValue = SLICE_PPS_ID_ERROR;
            return SLICE_PPS_ID_ERROR;
        }
    }

    return *pi4RetValue;
}

INT32 i4VDecH265RestOfSlicePsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue)
{
    INT32 i4BitsSliceSegmentAddress,i4NumCTUs;
    UINT32 u4Code,i,u4NalType;
    INT32 i4NumRpsCurrTempList = 0x00;
    UCHAR ucFBufId = 0xFF,ucSeqInfoChg = 0x00;
    BOOL bSAOEnabled,bDBFEnabled,fgNeedAllocateFBuf = FALSE;
    H265_RPS_Data *pSliceRPS;
    H265_SPS_Data *prH265_SPS_DataInst;
    H265_PPS_Data *prH265_PPS_DataInst;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;

    *pi4RetValue = SLICE_OK;
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    prH265_SPS_DataInst = prH265DecPrm->prSPS;
    prH265_PPS_DataInst = prH265DecPrm->prPPS;
    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    u4NalType = prH265DecPrm->u4NalType;
    
    
    vVDecH265InitSliceHdr(u4InstID,prH265DrvDecInfo);
    vVDecH265InitPicInfo(u4VDecID,prH265DrvDecInfo);

    if(1 != prH265_Slice_Hdr_DataInst->bFirstSliceSegmentInPic && prH265_PPS_DataInst->bDependentSliceSegmentsEnabledFlag)
    {
        prH265_Slice_Hdr_DataInst->bDependentSliceSegmentFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
    } 
    else 
    {
        prH265_Slice_Hdr_DataInst->bDependentSliceSegmentFlag = 0;
    }
    
    i4NumCTUs = ((prH265_SPS_DataInst->u4PicWidthInLumaSamples+prH265_SPS_DataInst->u4MaxCUWidth-1)/prH265_SPS_DataInst->u4MaxCUWidth)*
                       ((prH265_SPS_DataInst->u4PicHeightInLumaSamples+prH265_SPS_DataInst->u4MaxCUHeight-1)/prH265_SPS_DataInst->u4MaxCUHeight);

    //calculation of SliceSegmentAddress
    i4BitsSliceSegmentAddress = 0;
    while(i4NumCTUs > (1 << i4BitsSliceSegmentAddress))
    {
        i4BitsSliceSegmentAddress++;
    }

    if(1 != prH265_Slice_Hdr_DataInst->bFirstSliceSegmentInPic)
    {
        prH265_Slice_Hdr_DataInst->u4SliceSegmentAddress  = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID,i4BitsSliceSegmentAddress);//u(v)
    }

    if ( 0 == prH265_Slice_Hdr_DataInst->bDependentSliceSegmentFlag )
    {
        for ( i=0; i < prH265_PPS_DataInst->u4NumExtraSliceHeaderBits; i++)
        {
            u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }
        
        prH265_Slice_Hdr_DataInst->u4SliceType = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        VDecPrintf(u4InstID,VDEC_LOG_PICINFO,"Current PicType %d With HW [%d]\n",prH265_Slice_Hdr_DataInst->u4SliceType,u4VDecID);

        if((prH265DecPrm->ucSkipMode == IMode) && (prH265_Slice_Hdr_DataInst->u4SliceType != HEVC_I_SLICE))
        {
            *pi4RetValue = SLICE_ST_DROP;
            return SLICE_ST_DROP;
        }
        
        if (prH265_Slice_Hdr_DataInst->u4SliceType >= H265_Slice_Type_MAX) 
        {
            if ( 16 <= u4NalType && 23>= u4NalType  )
            {
                prH265_Slice_Hdr_DataInst->u4SliceType = HEVC_I_SLICE;
            } 
            else
            {
                prH265_Slice_Hdr_DataInst->u4SliceType = HEVC_B_SLICE;
            }
        }
        
        if ( prH265_PPS_DataInst->bOutputFlagPresentFlag )
        {
            prH265_Slice_Hdr_DataInst->bPicOutputFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }

        ////////RW_HEVC_SLICE_0 register setting
        if ( prH265_Slice_Hdr_DataInst->bFirstSliceSegmentInPic )
        {
            UINT32 u4SliceReg0;
            BOOL fgIDRflag;
            fgIDRflag = (u4NalType ==NAL_UNIT_CODED_SLICE_IDR) || (u4NalType ==NAL_UNIT_CODED_SLICE_IDR_N_LP);
            u4SliceReg0 = (( 0 & 0xfffff) << 0) |
                            ((i4BitsSliceSegmentAddress & 0x1f) << 20) |
                            ((prH265_Slice_Hdr_DataInst->u4SliceType & 0x3) << 28) |
                            ((prH265_Slice_Hdr_DataInst->bDependentSliceSegmentFlag & 0x1) << 30) |
                            ((fgIDRflag & 0x1) << 31); 
            vVDecWriteHEVCVLD(u4VDecID, RW_HEVC_SLICE_0, u4SliceReg0);
        }

        if (prH265_SPS_DataInst->bSeparateColourPlaneFlag)
        {
            prH265_Slice_Hdr_DataInst->u4ColourPlaneID = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID,2);//u(2)
        }

        //Calc POC
        vVDecH265CalcCurSlicePOC(u4InstID,u4VDecID,prH265DrvDecInfo);
        //Parse short term RPS
        vVDecH265PrepareShortTermRPS(u4InstID,u4VDecID,prH265DrvDecInfo);
        //Parse long term RPS
        vVDecH265PrepareLongTermRPS(u4InstID,u4VDecID,prH265DrvDecInfo);

        pSliceRPS = prH265_Slice_Hdr_DataInst->pShortTermRefPicSets;
        if ( u4NalType == NAL_UNIT_CODED_SLICE_BLA
            || u4NalType == NAL_UNIT_CODED_SLICE_BLANT
            || u4NalType == NAL_UNIT_CODED_SLICE_BLA_N_LP ) 
        {
            // In the case of BLA picture types, rps data is read from slice header but ignored
            pSliceRPS = &(prH265_Slice_Hdr_DataInst->rLocalRPS);
            pSliceRPS->u4NumberOfNegativePictures = 0;
            pSliceRPS->u4NumberOfPositivePictures = 0;
            pSliceRPS->u4NumberOfLongtermPictures = 0;
            pSliceRPS->u4NumberOfPictures = 0;
            prH265_Slice_Hdr_DataInst->pShortTermRefPicSets = pSliceRPS;
        }

        //Check whether get valid RPS
        ASSERT(prH265_Slice_Hdr_DataInst->pShortTermRefPicSets != NULL);
        #ifdef CC_DUAL_CORE_DEC
        if(prH265DecPrm->ucVDecStatus == REAL_DECODE)
        {
            VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-%d] Real decode flow \n",u4InstID);
        #endif
        
            #if (defined (CC_MT5890)) //Oryx E1 HW limitation workaround
            if((prH265_SPS_DataInst->u4BitDepthLumaMinus8 == 0) && (prH265DecPrm->fgIsUFOMode)
                && (prH265_PPS_DataInst->bTilesEnabledFlag) && !IS_IC_5861() && (!prH265DecPrm->ucDualCoreMode))
            {
                LOG(2,"Force 8bit tile UFO to nonUFO\n");
                prH265DecPrm->fgIsUFOMode = FALSE;
            }
            #endif
            
            //Allocate FBG
            if(i4VDecH265AllocateFBG(u4InstID,prH265DecPrm) == E_VDEC_FAIL)
            {
                //error handle
                *pi4RetValue = ALLOC_FBG_FAIL;
                return ALLOC_FBG_FAIL;
            }
            
            if(i4VDecH265PrepareRefList(u4InstID,prH265DecPrm) != E_VDEC_OK)
            {
                //error handle        
            }
            
            //Prepare decode info
            //Check whether need allocate free fbidx
            if(( (prH265DecPrm->prCurrFBufInfo == NULL) ||
                (prH265DecPrm->prCurrFBufInfo->ucFBufStatus == FRAME) ||
                (prH265DecPrm->prSliceHdr->i4POC != prH265DecPrm->rLastInfo.i4LastPOC)))
            {
                fgNeedAllocateFBuf = TRUE;
            }
            else if(prH265DecPrm->ucSkipMode == IMode)
            {
                fgNeedAllocateFBuf = TRUE;
            }
            
            if(fgNeedAllocateFBuf)
            {
                if((ucFBufId = ucVDecH265AllocateFBInfo(u4InstID,prH265DecPrm)) != FBUF_ID_UNKNOWN)
                {
                    prH265DecPrm->ucDecFBufId = ucFBufId;
                    
                    //Get FBM
                    if(prH265DecPrm->ucPreFbId == FBM_FB_ID_UNKNOWN)
                    {
                        //prH265DecPrm->ucFbId = FBM_GetEmptyFrameBuffer(prH265DecPrm->ucFbgId, prH265DecPrm->u4WaitFbTime);
                        if((prH265DecPrm->ucPreFbId = FBM_GetEmptyFrameBuffer(prH265DecPrm->ucFbgId, VDEC_H265_GET_FB_TIMEOUT)) == FBM_FB_ID_UNKNOWN)
                        {
                            *pi4RetValue = GET_FB_TIMEOUT;
                            return *pi4RetValue;                        
                        }
                    }
                    //Get Empty FBM sucess.
                    prH265DecPrm->ucFbId = prH265DecPrm->ucPreFbId;
                }
                else
                {
                    LOG(1,"Allocate FBuf Fail \n");
                }
            }
            
            ASSERT(ucFBufId < prH265DecPrm->ucMaxFBufNum);
            
            vVDecH265SetCurrFBInfo(u4InstID,prH265DrvDecInfo);
            ucSeqInfoChg = ucVDecH265SendSeqChgInfo(u4InstID,prH265DecPrm);
            *pi4RetValue = (ucSeqInfoChg & (FRAMERATE_CHG | RESOLUTION_CHG)) ? SLICE_RES_CHG : *pi4RetValue;
            prH265DecPrm->ucSeqChgMode = 0x00;
            
            //Mark ref-nal as ref property
            //No matter SREF/LREF, Ref property will be re-assign in Apply and Set Ref API
            if(!IsH265NonRefSlice(u4NalType))
            {
                vVDecH265SetRefFBuf(prH265DecPrm->prFBufInfo,ucFBufId,SREF_PIC);
            }
        #ifdef CC_DUAL_CORE_DEC
        }
        else if(prH265DecPrm->ucVDecStatus == LAE_DECODE)//Get LAE decode ref info
        {
            prH265DecPrm->rLaeInfo.u4PicH = prH265DecPrm->prSPS->u4PicHeightInLumaSamples;
            prH265DecPrm->rLaeInfo.u4PicW = prH265DecPrm->prSPS->u4PicWidthInLumaSamples;
            // Get LAE buffer
            //fgLAEAllocateInfoBuf(u4InstID,VDEC_FMT_H265,&(prH265DecPrm->rLaeInfo));
            do
            {
                ucGetBufferForLaeDec(u4InstID,&(prH265DecPrm->rLaeInfo));
                VDecPrintf(u4InstID,VDEC_LOG_LAEDEC,"[H265-TEST-%d] LAE Decode Flow address 0x%x,0x%x\n",u4InstID,prH265DecPrm->rLaeInfo.u4CabacAddr,prH265DecPrm->rLaeInfo.u4ECAddr);
                if((prH265DecPrm->rLaeInfo.u4CabacAddr == 0) || (prH265DecPrm->rLaeInfo.u4ECAddr == 0))
                {
                    *pi4RetValue = SLICE_ST_DROP;
                    return SLICE_ST_DROP;
                }
            }while( (prH265DecPrm->prSPS->u4PicHeightInLumaSamples != prH265DecPrm->rLaeInfo.u4PicH) 
                 || (prH265DecPrm->prSPS->u4PicWidthInLumaSamples != prH265DecPrm->rLaeInfo.u4PicW));
            //error handle
            
            vVDEC_HAL_H265_SetLAEDecAddr(u4VDecID,PHYSICAL(prH265DecPrm->rLaeInfo.u4CabacAddr),PHYSICAL(prH265DecPrm->rLaeInfo.u4ECAddr));
            vVDEC_HAL_Set_ErrConceal(u4InstID,u4VDecID);
        }
        #endif
        
        if (1 == prH265_SPS_DataInst->bUseSAO)
        {
            prH265_Slice_Hdr_DataInst->bSaoEnabledFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
            prH265_Slice_Hdr_DataInst->bSaoEnabledFlagChroma = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }
        
        if ( u4NalType == NAL_UNIT_CODED_SLICE_IDR ||u4NalType == NAL_UNIT_CODED_SLICE_IDR_N_LP )
        {
            prH265_Slice_Hdr_DataInst->bTMVPFlagsPresent = 0;
        }
    
        if ( prH265_Slice_Hdr_DataInst->u4SliceType != HEVC_I_SLICE )// not I slice
        {
            u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
            if (1 == u4Code)
            {
                u4Code = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
                prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_0] = u4Code + 1;
                if ( prH265_Slice_Hdr_DataInst->u4SliceType == HEVC_B_SLICE )
                {
                    u4Code = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
                    prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_1] = u4Code + 1;
                }
                else
                {
                    prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_1] = 0;
                }
            }
            else
            {
                prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_0] = prH265_PPS_DataInst->u4NumRefIdxL0DefaultActiveMinus1+1;
                if(prH265_Slice_Hdr_DataInst->u4SliceType == HEVC_B_SLICE)
                {
                    prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_1] = prH265_PPS_DataInst->u4NumRefIdxL1DefaultActiveMinus1+1;
                }
                else
                {
                    prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_1] = 0;
                }
            }
        }
        
        for( i=0; i < prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfNegativePictures 
            +prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfPositivePictures
            +prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfLongtermPictures; i++) 
        {
            if(prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->bUsed[i]) 
            {
                i4NumRpsCurrTempList++;
            }
        }

        if ( prH265_Slice_Hdr_DataInst->u4SliceType != HEVC_I_SLICE )// not I slice
        {
            ////////RW_HEVC_SLICE_2 register setting
            if ( prH265_Slice_Hdr_DataInst->bFirstSliceSegmentInPic )
            {
                UINT32 u4SliceReg2;
                u4SliceReg2 = (((prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_0]-1) & 0xf) << 0) |
                               (((prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_1]-1) & 0xf) << 4);
                vVDecWriteHEVCVLD(u4VDecID, RW_HEVC_SLICE_2, u4SliceReg2);  
            }

            if ( prH265_PPS_DataInst->bListsModificationPresentFlag && i4NumRpsCurrTempList > 1 )
            {
                ////////RW_HEVC_REF_PIC_LIST_MOD register setting
                if ( prH265_Slice_Hdr_DataInst->bFirstSliceSegmentInPic )
                {
                    UINT32 u4RefListMode;
                    INT32 i4Length = 0;
                    INT32 i4NumRpsCurrTempList0 = i4NumRpsCurrTempList;
                    if ( i4NumRpsCurrTempList0 > 1 )
                    {
                            i4Length = 1;
                            i4NumRpsCurrTempList0 --;
                    }
                    while ( i4NumRpsCurrTempList0 >>= 1)
                    {
                        i4Length ++;
                    }
                    u4RefListMode = ((i4NumRpsCurrTempList & 0xf) << 0) | ((i4Length & 0x7) << 4);
                    vVDecWriteHEVCVLD(u4VDecID, RW_HEVC_REF_PIC_LIST_MOD, u4RefListMode);  
                    /////////////////////////////////////////////////////////////////////
                    vVDecH265Ref_Pic_List_Modification(u4InstID,u4VDecID);
                    /////////////////////////////////////////////////////////////////////
                }

            }

            if ( prH265_Slice_Hdr_DataInst->u4SliceType == HEVC_B_SLICE )
            {
                prH265_Slice_Hdr_DataInst->bMvdL1ZeroFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
            }

            prH265_Slice_Hdr_DataInst->bCabacInitFlag = 0; //default
            if ( 1 == prH265_PPS_DataInst->bCabacInitPresentFlag )
            {
                prH265_Slice_Hdr_DataInst->bCabacInitFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
            }
            if ( 1 == prH265_Slice_Hdr_DataInst->bTMVPFlagsPresent )
            {
                if ( prH265_Slice_Hdr_DataInst->u4SliceType == HEVC_B_SLICE )
                {
                    prH265_Slice_Hdr_DataInst->bColFromL0Flag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
                }
                else
                {
                    prH265_Slice_Hdr_DataInst->bColFromL0Flag = 1;
                }
                if ( (1 == prH265_Slice_Hdr_DataInst->bColFromL0Flag && 1 < prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_0]) ||
                     (0 == prH265_Slice_Hdr_DataInst->bColFromL0Flag && 1 < prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_1]) )
                {
                    prH265_Slice_Hdr_DataInst->u4ColRefIdx = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
                }
                else 
                {
                    prH265_Slice_Hdr_DataInst->u4ColRefIdx  = 0;
                }
            }
            if ( (1 == prH265_PPS_DataInst->bWPPredFlag && prH265_Slice_Hdr_DataInst->u4SliceType == HEVC_P_SLICE) ||
                 (1 == prH265_PPS_DataInst->bWPBiPredFlag && prH265_Slice_Hdr_DataInst->u4SliceType == HEVC_B_SLICE) )
            {     
                /////////////////////////////////////////////////////////////////////
                vVDEC_HAL_H265_PredWeightTable(u4VDecID);
                /////////////////////////////////////////////////////////////////////
            }
            prH265_Slice_Hdr_DataInst->u4FiveMinusMaxNumMergeCand = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        }

        u4Code = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)
        prH265_Slice_Hdr_DataInst->i4SliceQp = u4Code + prH265_PPS_DataInst->i4PicInitQPMinus26+26;

        if ( 1 == prH265_PPS_DataInst->bPPSSliceChromaQpFlag )
        {
            prH265_Slice_Hdr_DataInst->i4SliceQpDeltaCb = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)
            prH265_Slice_Hdr_DataInst->i4SliceQpDeltaCr = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)
        }
        if (1 == prH265_PPS_DataInst->bDeblockingFilterControlPresentFlag)
        {
            if (1 == prH265_PPS_DataInst->bDeblockingFilterOverrideEnabledFlag)
            {
                prH265_Slice_Hdr_DataInst->bDeblockingFilterOverrideFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
            } 
            else 
            {
                prH265_Slice_Hdr_DataInst->bDeblockingFilterOverrideFlag = 0;
            }
            if (1 == prH265_Slice_Hdr_DataInst->bDeblockingFilterOverrideFlag)
            {
                prH265_Slice_Hdr_DataInst->bDeblockingFilterDisableFlag = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
                if (0 == prH265_Slice_Hdr_DataInst->bDeblockingFilterDisableFlag)
                {
                    prH265_Slice_Hdr_DataInst->i4DeblockingFilterBetaOffsetDiv2 = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)
                    prH265_Slice_Hdr_DataInst->i4DeblockingFilterTcOffsetDiv2 = i4VDEC_HAL_H265_SeCodeNum(0,u4VDecID);//se(v)
                }
            }
            else
            {
                prH265_Slice_Hdr_DataInst->bDeblockingFilterDisableFlag = prH265_PPS_DataInst->bPicDisableDeblockingFilterFlag;
                prH265_Slice_Hdr_DataInst->i4DeblockingFilterBetaOffsetDiv2 = prH265_PPS_DataInst->i4DeblockingFilterBetaOffsetDiv2;
                prH265_Slice_Hdr_DataInst->i4DeblockingFilterTcOffsetDiv2 = prH265_PPS_DataInst->i4DeblockingFilterTcOffsetDiv2;
            }
        }
        else
        {
            prH265_Slice_Hdr_DataInst->bDeblockingFilterDisableFlag = 0;
            prH265_Slice_Hdr_DataInst->i4DeblockingFilterBetaOffsetDiv2 = 0;
            prH265_Slice_Hdr_DataInst->i4DeblockingFilterTcOffsetDiv2 = 0;
        }

        bSAOEnabled = ( 0 == prH265_SPS_DataInst->bUseSAO ) ? (0):(prH265_Slice_Hdr_DataInst->bSaoEnabledFlag || prH265_Slice_Hdr_DataInst->bSaoEnabledFlagChroma);
        bDBFEnabled = ( 0 == prH265_Slice_Hdr_DataInst->bDeblockingFilterDisableFlag );
        if ( 1 == prH265_PPS_DataInst->bLoopFilterAcrossSlicesEnabledFlag && (bSAOEnabled ||bDBFEnabled ))
        {
            u4Code= u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }
        else
        {
            u4Code = prH265_PPS_DataInst->bLoopFilterAcrossSlicesEnabledFlag;
        }
        prH265_Slice_Hdr_DataInst->bLoopFilterAcrossSlicesEnabledFlag = u4Code;
    }
    
    //Parse num_entry_point_offsets
    if ( 1 == prH265_PPS_DataInst->bTilesEnabledFlag || 1 == prH265_PPS_DataInst->bEntropyCodingSyncEnabledFlag )
    {
        prH265_Slice_Hdr_DataInst->u4NumEntryPointOffsets = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        if ( 0 < prH265_Slice_Hdr_DataInst->u4NumEntryPointOffsets )
        {
            prH265_Slice_Hdr_DataInst->u4OffsetLenMinus1 = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        }
        for ( i = 0; i < prH265_Slice_Hdr_DataInst->u4NumEntryPointOffsets ; i++ )
        {
            u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID,prH265_Slice_Hdr_DataInst->u4OffsetLenMinus1 + 1);//u(v)
        }
    }
    
    if ( 1 == prH265_PPS_DataInst->bSliceHeaderExtensionPresentFlag )
    {
        prH265_Slice_Hdr_DataInst->u4SliceHeaderExtensionLength = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
        for ( i = 0; i < prH265_Slice_Hdr_DataInst->u4SliceHeaderExtensionLength ; i++ )
        {
            u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID,8);//u(8)
        }
    }
    
    //Set register prepare trigger decode
    vVDecH265SetRegister(u4InstID,u4VDecID,prH265DecPrm);
    
    if(prH265DecPrm->ucDualCoreMode == DUAL_CORE_MODE)
    {
        VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-TEST-%d] Mcore setting begin\n",u4InstID);
        vVDecWriteHEVCVLD(VDEC_VLD1, RW_HEVC_SLICE_0, u4VDecReadHEVCVLD(u4VDecID,RW_HEVC_SLICE_0));
        if ( prH265_Slice_Hdr_DataInst->u4SliceType != HEVC_I_SLICE )
        {
            vVDecWriteHEVCVLD(VDEC_VLD1, RW_HEVC_SLICE_2, u4VDecReadHEVCVLD(u4VDecID,RW_HEVC_SLICE_2)); 
            vVDecWriteHEVCVLD(VDEC_VLD1, RW_HEVC_REF_PIC_LIST_MOD, u4VDecReadHEVCVLD(u4VDecID,RW_HEVC_REF_PIC_LIST_MOD));  
        }
        vVDecH265SetRegister(u4InstID,VDEC_VLD1,prH265DecPrm);

        //Check lae buffer available
        if(IsH265SliceNalUnit(prH265DecPrm->u4NalType))
        {
            ucGetBufferForMcoreDec(u4InstID,&(prH265DecPrm->rLaeInfo));
            UNUSED(fgMCoreMatchLAE(u4InstID,prH265DrvDecInfo));
            VDecPrintf(u4InstID,VDEC_LOG_LAEDEC,"Get lae decQ 0x%x,0x%x @ rdptr 0x%x \n",prH265DecPrm->rLaeInfo.u4CabacAddr,prH265DecPrm->rLaeInfo.u4ECAddr,prH265DecPrm->rLaeInfo.u4PesRdptr);
        }
        vVDecSetMCoreLaeBuf(u4InstID,PHYSICAL(prH265DecPrm->rLaeInfo.u4CabacAddr),PHYSICAL(prH265DecPrm->rLaeInfo.u4ECAddr));
    }
    else if(prH265DecPrm->ucDualCoreMode == M10TILE_DUAL_CORE_MODE)
    {
        //Check lae buffer available
        if(IsH265SliceNalUnit(prH265DecPrm->u4NalType))
        {
            ucGetBufferForMcoreDec(u4InstID,&(prH265DecPrm->rLaeInfo));
            VDecPrintf(u4InstID,VDEC_LOG_LAEDEC,"Get from lae decQ address 0x%x,0x%x \n",prH265DecPrm->rLaeInfo.u4CabacAddr,prH265DecPrm->rLaeInfo.u4ECAddr);
        }
        vVDecSetMCoreLaeBuf(u4InstID,PHYSICAL(prH265DecPrm->rLaeInfo.u4CabacAddr),PHYSICAL(prH265DecPrm->rLaeInfo.u4ECAddr));
    }
    
    
    vVDEC_HAL_H265_TrailingBits(0,u4VDecID);
    VDecPrintf(u4InstID,VDEC_LOG_INWIND,"[H265-TEST-%d] Input window[%d] is 0x%x error type 0x%08x\n",u4InstID,u4VDecID,u4VDecHEVCVLDGetBitS(0,u4VDecID,0),u4VDEC_HAL_H265_GetErrMsg(u4VDecID));
    return *pi4RetValue;
}


void vVDecH265SlicePsr(UINT32 u4InstID,UINT32 u4VDecID, H265_DRV_INFO_T* prH265DrvDecInfo,INT32* pi4RetValue)
{
    INT32 i4Ret = SLICE_OK;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    if((prH265DecPrm->fgWaitFirstIFrm) && !IsH265RAPSlice(prH265DecPrm->u4NalType))
    {
        *pi4RetValue = (INT32)SLICE_ST_DROP;
        return ;
    }
    else
    {
        prH265DecPrm->fgWaitFirstIFrm = FALSE;
    }
    
    i4VDecH265SlimSlicePsr(u4InstID,u4VDecID,prH265DrvDecInfo,&i4Ret);
    if(i4Ret != SLICE_OK)
    {
        LOG(0,"[H265-ERR-%d] Parse milestone 1 error 0x%x\n",u4InstID,i4Ret);
        *pi4RetValue = (INT32)E_VDEC_PARSE_NOT_READY;
        return ;
    }
    
    i4VDecH265RestOfSlicePsr(u4InstID,u4VDecID,prH265DrvDecInfo,&i4Ret);
    if(i4Ret != SLICE_OK)
    {
        if(i4Ret == GET_FB_TIMEOUT)
        {
            *pi4RetValue = (INT32)GET_FB_TIMEOUT;
        }
        else if(i4Ret == ALLOC_FBG_FAIL)
        {
            LOG(2,"[ERR-%d] Allocate FBG Fail 0x%x\n",u4InstID,i4Ret);
            *pi4RetValue = (INT32)ALLOC_FBG_FAIL;
        }
        else if(i4Ret == SLICE_RES_CHG)
        {
            *pi4RetValue = (INT32)E_VDEC_OK_RES_CHG;
        }
        else
        {
            LOG(2,"[ERR-%d] Parse milestone 2 error 0x%x\n",u4InstID,i4Ret);
            *pi4RetValue = (INT32)SLICE_ST_DROP;
        }
        
    }
}

void vVDecH265InitSliceHdr(UINT32 u4InstID,H265_DRV_INFO_T* prH265DrvDecInfo)
{
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;
    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;

    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;

    if(IsH265RAPSlice(prH265DecPrm->u4NalType))
    {
        if((prH265DecPrm->u4NalType == NAL_UNIT_CODED_SLICE_BLA)   ||
           (prH265DecPrm->u4NalType == NAL_UNIT_CODED_SLICE_BLANT) |
           (prH265DecPrm->u4NalType == NAL_UNIT_CODED_SLICE_BLA_N_LP) ||
           (prH265DecPrm->u4NalType == NAL_UNIT_CODED_SLICE_IDR) ||
           (prH265DecPrm->u4NalType == NAL_UNIT_CODED_SLICE_IDR_N_LP) )
        {
            prH265_Slice_Hdr_DataInst->bNoRaslOutputFlag = 1;
        }
        else if(prH265DecPrm->u4NalType == NAL_UNIT_CODED_SLICE_CRA)
        {
            if(prH265DecPrm->bFirstSliceInSequence)
            {
                prH265_Slice_Hdr_DataInst->bNoRaslOutputFlag = TRUE;
                prH265DecPrm->fgNoRaslOutput = TRUE;
            }
            else if(prH265_Slice_Hdr_DataInst->bFirstSliceSegmentInPic )
            {
                prH265_Slice_Hdr_DataInst->bNoRaslOutputFlag = FALSE;
            }
        }
        else
        {
            prH265_Slice_Hdr_DataInst->bNoRaslOutputFlag = FALSE;
        }

        if(!prH265DecPrm->bFirstSliceInSequence)
        {
            prH265DecPrm->fgNoRaslOutput = FALSE;
        }
        
    }

    prH265_Slice_Hdr_DataInst->u4NumOfLongTermPics = 0;
    prH265_Slice_Hdr_DataInst->u4NumOfLongTermSPS = 0;
    prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_0] = 0;
    prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_1] = 0;
    prH265_Slice_Hdr_DataInst->i4NumRefIdx[REF_PIC_LIST_C] = 0;
    prH265_Slice_Hdr_DataInst->u4ColRefIdx = 0;

    prH265_Slice_Hdr_DataInst->bColFromL0Flag = 1;
    prH265_Slice_Hdr_DataInst->bTMVPFlagsPresent = 1;

    prH265_Slice_Hdr_DataInst->i4SliceQpDeltaCb = 0;
    prH265_Slice_Hdr_DataInst->i4SliceQpDeltaCr = 0;

    prH265_Slice_Hdr_DataInst->i4POC = 0;
    prH265_Slice_Hdr_DataInst->i4POCLsb = 0;
    prH265_Slice_Hdr_DataInst->i4POCMsb = 0;
    prH265_Slice_Hdr_DataInst->bCabacInitFlag = 0;
    prH265_Slice_Hdr_DataInst->u4FiveMinusMaxNumMergeCand = MRG_MAX_NUM_CANDS;
    prH265DecPrm->bFirstSliceInSequence = FALSE;

    prH265_Slice_Hdr_DataInst->rLocalRPS.u4NumberOfNegativePictures = 0;
    prH265_Slice_Hdr_DataInst->rLocalRPS.u4NumberOfPositivePictures = 0;
    prH265_Slice_Hdr_DataInst->rLocalRPS.u4NumberOfPictures = 0;
    prH265_Slice_Hdr_DataInst->rLocalRPS.u4NumberOfLongtermPictures = 0;
    prH265_Slice_Hdr_DataInst->rLocalRPS.u4DeltaRIdxMinus1 = 0;
    prH265_Slice_Hdr_DataInst->rLocalRPS.u4DeltaRPS = 0;
    prH265_Slice_Hdr_DataInst->rLocalRPS.bInterRPSPrediction = 0;
    prH265_Slice_Hdr_DataInst->rLocalRPS.u4NumRefIdc = 0;
}


void vVDecH265CalcCurSlicePOC(UINT32 u4InstID,UINT32 u4VDecID,H265_DRV_INFO_T* prH265DrvDecInfo)
{
    UINT32 u4NalType,u4NuhTemporalId;
    INT32 iPOClsb,iMaxPOClsb, iPrevPOClsb, iPrevPOCmsb, iPOCmsb;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst;
    H265_SPS_Data *prH265_SPS_DataInst;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;

    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    u4NalType = prH265DecPrm->u4NalType;
    u4NuhTemporalId = prH265DecPrm->u4NuhTemporalId;
    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    prH265_SPS_DataInst = prH265DecPrm->prSPS;
    
    if( u4NalType == NAL_UNIT_CODED_SLICE_IDR || u4NalType == NAL_UNIT_CODED_SLICE_IDR_N_LP )// IDR picture
    { 

        prH265_Slice_Hdr_DataInst->i4POC = 0;
        //prH265_Slice_Hdr_DataInst->pShortTermRefPicSets = &(prH265_Slice_Hdr_DataInst->rLocalRPS);
        //prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfNegativePictures = 0;
        //prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfPositivePictures = 0;
        //prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfLongtermPictures = 0;
        //prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfPictures = 0;
        prH265_Slice_Hdr_DataInst->i4POCLsb = 0;
        prH265_Slice_Hdr_DataInst->i4POCMsb = 0;

        if ( 0 == u4NuhTemporalId )
        {
            prH265DecPrm->i4PrePOC = 0;
            prH265DecPrm->i4PrePOCLsb = 0;
            prH265DecPrm->i4PrePOCMsb = 0;     
        }

    } 
    else
    {

        prH265_Slice_Hdr_DataInst->i4POCLsb = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID,prH265_SPS_DataInst->u4Log2MaxPicOrderCntLsbMinus4 + 4);//u(v)

        // slice POC calculation
        // But algorithm is not align with SPEC  mtk40343 ???????
        
        iPOClsb = prH265_Slice_Hdr_DataInst->i4POCLsb;
        iMaxPOClsb = 1<< (prH265_SPS_DataInst->u4Log2MaxPicOrderCntLsbMinus4+4);

        iPrevPOClsb = prH265DecPrm->i4PrePOCLsb;
        iPrevPOCmsb = prH265DecPrm->i4PrePOCMsb;

        VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-TEST-%d] Lsb %d Prev %d Max %d\n",u4InstID,iPOClsb,iPrevPOClsb,iMaxPOClsb);
        if( ( iPOClsb  <  iPrevPOClsb ) && ( ( iPrevPOClsb - iPOClsb )  >=  ( iMaxPOClsb / 2 ) ) ) 
        {
            iPOCmsb = iPrevPOCmsb + iMaxPOClsb;
        }
        else if( (iPOClsb  >  iPrevPOClsb )  && ( (iPOClsb - iPrevPOClsb )  >  ( iMaxPOClsb / 2 ) ) )
        {
            iPOCmsb = iPrevPOCmsb - iMaxPOClsb;
        }
        else
        {
            iPOCmsb = iPrevPOCmsb;
        }

        if ( u4NalType == NAL_UNIT_CODED_SLICE_BLA
          || u4NalType == NAL_UNIT_CODED_SLICE_BLANT
          || u4NalType == NAL_UNIT_CODED_SLICE_BLA_N_LP ) 
        {
          // For BLA picture types, POCmsb is set to 0.
            iPOCmsb = 0;
        }
    
        if ( u4NalType == NAL_UNIT_CODED_SLICE_CRA && prH265_Slice_Hdr_DataInst->bNoRaslOutputFlag )
        {
            iPOCmsb = 0;
        }
        
        prH265_Slice_Hdr_DataInst->i4POCLsb = iPOClsb;
        prH265_Slice_Hdr_DataInst->i4POCMsb = iPOCmsb;
        // get POC
        prH265_Slice_Hdr_DataInst->i4POC = (iPOCmsb+iPOClsb);
        if ( 0 == u4NuhTemporalId )
        {
            prH265DecPrm->i4PrePOC = prH265_Slice_Hdr_DataInst->i4POC;
            prH265DecPrm->i4PrePOCLsb = iPOClsb;
            prH265DecPrm->i4PrePOCMsb = iPOCmsb;
        }
        VDecPrintf(u4InstID,VDEC_LOG_DECINFO,"[H265-TEST-%d] Msb&Lsb [%d,%d]CurrPOC = %d\n",u4InstID,iPOCmsb,iPOClsb,prH265_Slice_Hdr_DataInst->i4POC);
    }
}


void vVDecH265PrepareShortTermRPS(UINT32 u4InstID,UINT32 u4VDecID,H265_DRV_INFO_T* prH265DrvDecInfo)
{
    UINT32 u4NalType,u4Code;
    INT32 i4NumBits;
    H265_RPS_Data *pSliceRPS;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst;
    H265_SPS_Data *prH265_SPS_DataInst;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;

    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    u4NalType = prH265DecPrm->u4NalType;
    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    prH265_SPS_DataInst = prH265DecPrm->prSPS;

    if( u4NalType == NAL_UNIT_CODED_SLICE_IDR || u4NalType == NAL_UNIT_CODED_SLICE_IDR_N_LP )// IDR picture
    {
        prH265_Slice_Hdr_DataInst->pShortTermRefPicSets = &(prH265_Slice_Hdr_DataInst->rLocalRPS);
        prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfNegativePictures = 0;
        prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfPositivePictures = 0;
        prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfLongtermPictures = 0;
        prH265_Slice_Hdr_DataInst->pShortTermRefPicSets->u4NumberOfPictures = 0;
    }
    else
    {
        u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        if ( 0 == u4Code )//! short_term_ref_pic_set_sps_flag
        {
            pSliceRPS = &(prH265_Slice_Hdr_DataInst->rLocalRPS);
            /////////////////////////////////////////////////////////////////////
             if ( SPS_OK != vVDecH265RPS_Rbsp(u4InstID,u4VDecID,prH265_SPS_DataInst, pSliceRPS,&(prH265DrvDecInfo->rH265PicInfo),prH265_SPS_DataInst->u4NumShortTermRefPicSets ))
             {
                 return ; 
             }
            /////////////////////////////////////////////////////////////////////
            prH265_Slice_Hdr_DataInst->pShortTermRefPicSets = pSliceRPS;

        }
        else
        {
            //calculation of BitsForShortermRPSidx
            i4NumBits = 0;
            while ((1 << i4NumBits) < prH265_SPS_DataInst->u4NumShortTermRefPicSets)
            {
                i4NumBits++;
            }
            if ( i4NumBits > 0 ){
                u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID,i4NumBits);//u(v)
                if ( u4Code >= prH265_SPS_DataInst->u4NumShortTermRefPicSets )
                {
                     u4Code = 0;
                }
            }
            else 
            {
                u4Code = 0;
            }
            prH265_Slice_Hdr_DataInst->pShortTermRefPicSets = prH265_SPS_DataInst->pShortTermRefPicSets[u4Code];
            pSliceRPS = prH265_Slice_Hdr_DataInst->pShortTermRefPicSets;
        }
    }
}


void vVDecH265PrepareLongTermRPS(UINT32 u4InstID,UINT32 u4VDecID,H265_DRV_INFO_T* prH265DrvDecInfo)
{
    UINT32 u4NalType,u4Code;
    INT32 i4NumBits,i,j;
    INT32 iOffset, iNumLtrpInSPS, iPrevDeltaMSB, iDeltaPocMSBCycleLT, iPOClsb,iPocLsbLt, iPocLTCurr;
    INT32 i4NumOfLtrp;
    BOOL bDeltaFlag = 0;
    H265_RPS_Data *pSliceRPS;
    H265_Slice_Hdr_Data *prH265_Slice_Hdr_DataInst;
    H265_SPS_Data *prH265_SPS_DataInst;
    VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm;

    prH265DecPrm = &prH265DrvDecInfo->rH265DecParam;
    u4NalType = prH265DecPrm->u4NalType;
    prH265_Slice_Hdr_DataInst = prH265DecPrm->prSliceHdr;
    prH265_SPS_DataInst = prH265DecPrm->prSPS;
    pSliceRPS = prH265_Slice_Hdr_DataInst->pShortTermRefPicSets;
    
    if( u4NalType == NAL_UNIT_CODED_SLICE_IDR || u4NalType == NAL_UNIT_CODED_SLICE_IDR_N_LP )// IDR picture
    {
        
    }
    else
    {
        if ( 1 == prH265_SPS_DataInst->bLongTermRefsPresent )
        {
            iOffset = pSliceRPS->u4NumberOfNegativePictures + pSliceRPS->u4NumberOfPositivePictures;
            iNumLtrpInSPS = 0;
            
            i4NumOfLtrp = 0;
            if ( 0 <  prH265_SPS_DataInst->u4NumLongTermRefPicSPS )
            {
                prH265_Slice_Hdr_DataInst->u4NumOfLongTermSPS = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
                iNumLtrpInSPS = prH265_Slice_Hdr_DataInst->u4NumOfLongTermSPS;
                i4NumOfLtrp += iNumLtrpInSPS;
                pSliceRPS->u4NumberOfLongtermPictures = i4NumOfLtrp;
            }
            
            prH265_Slice_Hdr_DataInst->u4NumOfLongTermPics = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)
            i4NumOfLtrp += prH265_Slice_Hdr_DataInst->u4NumOfLongTermPics;
            pSliceRPS->u4NumberOfLongtermPictures = i4NumOfLtrp;

            //calculation of BitsForLtrpInSPS
            i4NumBits = 0;
            while (prH265_SPS_DataInst->u4NumLongTermRefPicSPS > (1 << i4NumBits))
            {
                i4NumBits++;
            }
            
            //_rH265PicInfo[u4InstID].i4NumLongTerm = i4NumOfLtrp;
            //_rH265PicInfo[u4InstID].i4NumLongTermSps = iNumLtrpInSPS;
            prH265DrvDecInfo->rH265PicInfo.i4NumLongTerm = i4NumOfLtrp;
            prH265DrvDecInfo->rH265PicInfo.i4NumLongTermSps = iNumLtrpInSPS;

            iPrevDeltaMSB = 0, iDeltaPocMSBCycleLT = 0;
            for ( i = iOffset+pSliceRPS->u4NumberOfLongtermPictures-1, j = 0; j < i4NumOfLtrp; i--, j++)
            {
                iPocLsbLt = 0;   
                if ( j < prH265_Slice_Hdr_DataInst->u4NumOfLongTermSPS )
                {
                    u4Code = 0;
                    if ( 0 < i4NumBits )
                    {
                        u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID,i4NumBits);//u(v)
                    }    
                    iPocLsbLt = prH265_SPS_DataInst->u4LtRefPicPocLsbSps[u4Code];
                    pSliceRPS->bUsed[i] = prH265_SPS_DataInst->bUsedByCurrPicLtSPSFlag[u4Code];
                }
                else
                {
                    u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID,prH265_SPS_DataInst->u4Log2MaxPicOrderCntLsbMinus4+4);//u(v)
                    iPocLsbLt = u4Code;
                    u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
                    pSliceRPS->bUsed[i] = u4Code;
                }
                u4Code = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
                if ( 1 == u4Code )//SBPresentFlag
                {
                    u4Code = u4VDEC_HAL_H265_UeCodeNum(0,u4VDecID);//ue(v)

                    bDeltaFlag = 0;
                    //First LTRP || First LTRP from SH  
                    if( (  i == iOffset+pSliceRPS->u4NumberOfLongtermPictures-1) || ( i == iOffset+(i4NumOfLtrp-iNumLtrpInSPS)-1) )
                    {
                        bDeltaFlag = 1;
                    }
                    if( 1==bDeltaFlag )
                    {
                        iDeltaPocMSBCycleLT = u4Code;
                    } else
                    {
                        iDeltaPocMSBCycleLT = u4Code + iPrevDeltaMSB;              
                    }
                    iPocLTCurr = prH265_Slice_Hdr_DataInst->i4POC -iDeltaPocMSBCycleLT * (1 << (prH265_SPS_DataInst->u4Log2MaxPicOrderCntLsbMinus4+4)) 
                                                - iPOClsb + iPocLsbLt;
                    pSliceRPS->i4POC[i] = iPocLTCurr;
                    pSliceRPS->i4DeltaPOC[i] = iPocLTCurr-prH265_Slice_Hdr_DataInst->i4POC;
                    pSliceRPS->bCheckLTMSB[i] = 1;
                }
                else
                {
                    pSliceRPS->i4POC[i] = iPocLsbLt;
                    pSliceRPS->i4DeltaPOC[i] = iPocLsbLt-prH265_Slice_Hdr_DataInst->i4POC;
                    pSliceRPS->bCheckLTMSB[i] = 0;
                    if(i == iOffset+(i4NumOfLtrp-iNumLtrpInSPS)-1)
                    {
                        iDeltaPocMSBCycleLT = 0;
                    }
                }
                iPrevDeltaMSB = iDeltaPocMSBCycleLT;
            }

            iOffset += pSliceRPS->u4NumberOfLongtermPictures;
            pSliceRPS->u4NumberOfPictures = iOffset;
        }

        if (1==prH265_SPS_DataInst->bTMVPFlagsPresent)
        {
            prH265_Slice_Hdr_DataInst->bTMVPFlagsPresent = u4VDEC_HAL_H265_GetRealBitStream(0,u4VDecID, 1);//u(1)
        }
        else
        {
            prH265_Slice_Hdr_DataInst->bTMVPFlagsPresent = 0;
        }
    }
}


