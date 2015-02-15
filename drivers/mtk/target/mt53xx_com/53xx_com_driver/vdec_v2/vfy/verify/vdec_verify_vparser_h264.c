#include "vdec_verify_mpv_prov.h"
#include "vdec_hal_if_common.h"
#include "vdec_hal_if_mpeg.h"
#include "vdec_hal_if_wmv.h"
#include "vdec_hal_if_h264.h"
#include "vdec_verify_file_common.h"
#include "vdec_verify_vparser_h264.h"
#include "vdec_verify_common.h"
#include "x_debug.h"

#if (!CONFIG_DRV_LINUX)
#include <string.h>
#include <stdio.h>
#include <math.h>
#endif

void vParseSliceHeader(UINT32 u4InstID, UINT32 u4VDecID);  
void vSlice_layer_without_partition_nonIDR(UINT32 u4InstID, UINT32 u4VDecID);
void vSlice_layer_without_partition_IDR(UINT32 u4InstID, UINT32 u4VDecID);
void vVerifySEI_Rbsp(UINT32 u4InstID, UINT32 u4VDecID);
void vVerifySeq_Par_Set_Rbsp(UINT32 u4InstID, UINT32 u4VDecID);
void vVerifyPic_Par_Set_Rbsp(UINT32 u4InstID, UINT32 u4VDecID);
void vInterpretFilmGrainCharacteristicsInfo(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm);
void vVerifyRef_Pic_List_Reordering(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_H264_SLICE_HDR_T *prSliceHdr);
void vVerifyDec_Ref_Pic_Marking(UINT32 u4InstID,  UINT32 u4VDecID, VDEC_INFO_H264_SLICE_HDR_T *prSliceHdr);
void vVerifyInitSPS(VDEC_INFO_H264_SPS_T *prSPS);
void vVerifyInitSliceHdr(VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm);
void vVerifyHrdParameters(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_H264_HRD_PRM_T *tHrdPara);
void vInterpretBufferingPeriodInfo(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm);


BOOL fgChkRefInfo(UINT32 u4InstID, UINT32 u4FBufIdx, UINT32 u4RefType);
void vVDecSetPRefPicListReg(UINT32 u4FBufInfo, UINT32 u4ListIdx);
void vVDecSetBRefPicListReg(UINT32 u4FBufInfo, UINT32 u4ListIdx);
void vInsertRefPicList(UINT32 u4InstID, VDEC_INFO_H264_REF_PIC_LIST_T *ptRefPicList, INT32 iCurrPOC, UINT32 u4RefPicListInfo);
void vVDecSetCurrPOC(UINT32 u4InstID);


CHAR quant_intra_default[16] = {
6,13,20,28,
13,20,28,32,
20,28,32,37,
28,32,37,42
};

CHAR quant_inter_default[16] = {
10,14,20,24,
14,20,24,27,
20,24,27,30,
24,27,30,34
};

CHAR quant8_intra_default[64] = {
6,10,13,16,18,23,25,27,
10,11,16,18,23,25,27,29,
13,16,18,23,25,27,29,31,
16,18,23,25,27,29,31,33,
18,23,25,27,29,31,33,36,
23,25,27,29,31,33,36,38,
25,27,29,31,33,36,38,40,
27,29,31,33,36,38,40,42
};

CHAR quant8_inter_default[64] = {
9,13,15,17,19,21,22,24,
13,13,17,19,21,22,24,25,
15,17,19,21,22,24,25,27,
17,19,21,22,24,25,27,28,
19,21,22,24,25,27,28,30,
21,22,24,25,27,28,30,32,
22,24,25,27,28,30,32,33,
24,25,27,28,30,32,33,35
};


// *********************************************************************
// Function    : void vErrInfo(UINT32 u4Type)
// Description : error handler
// Parameter   : None
// Return      : None
// *********************************************************************
void vErrInfo(UINT32 u4Type)
{
	switch(u4Type)
	{
		case OUT_OF_FILE:
		break;
		case VER_FORBIDEN_ERR:
		break;
		case DEC_INIT_FAILED:
		break;
		default:
		break;
	}
}


// *********************************************************************
// Function    : void   vPrepareRefPiclist(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm)
// Description : check pic type to send P_0, B_0, B_1
// Parameter   : None
// Return      : None
// *********************************************************************
void vPrepareRefPiclist(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm)
{
#if defined(MVC_VFY_SUPPORT) 
	vVDEC_HAL_H264_MVC_Switch(u4InstID, _ucMVCType[u4InstID] == 2);
#endif

#ifdef REDEC    
	if( _u4ReDecCnt[u4InstID] == 0)
#endif      
	{
		vVerifyPrepareFBufInfo(u4InstID, tVerMpvDecPrm);    
	}
}


// *********************************************************************
// Function : void AssignQuantParam(void)
// Description : 
// Parameter : 
// Return    : 
// *********************************************************************
void vAssignQuantParam(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm)
{
  	INT32 i;
	CHAR *ptList[8];

	for(i=0; i<8; i++)
	{
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = FALSE;
	}

	if((!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgPicScalingMatrixPresentFlag) 
	&& (!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgSeqScalingMatrixPresentFlag))
	{
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingMatrixPresentFlag = FALSE;	
		// do nothing
	}
	else
	{
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingMatrixPresentFlag = TRUE;
		if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgSeqScalingMatrixPresentFlag) // check sps first
		{
			for(i=0; i<8; i++)
			{
				ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = TRUE;
				if(i<6)
				{
  					if(!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgSeqScalingListPresentFlag[i]) // fall-back rule A
  					{
    					if((i==0) || (i==3))
    					{
      						ptList[i] =  (i==0) ? quant_intra_default:quant_inter_default;
      						vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
    					}
    					else
    					{
      						ptList[i] =  ptList[i-1];
      						ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i-1];
							vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
    					}
  					}
  					else // fall-back rule A
  					{
    					if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgUseDefaultScalingMatrix4x4Flag[i])
    					{
      						ptList[i] = (i<3) ? quant_intra_default:quant_inter_default;
      						vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
    					}
    					else
    					{
    						ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = TRUE;
      						ptList[i] = ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->cScalingList4x4[i];
      						vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);           
    					}
  					}
				}
				else
				{
  					if(!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgSeqScalingListPresentFlag[i] 
    				|| ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgUseDefaultScalingMatrix8x8Flag[i-6]) // fall-back rule A
  					{
    					ptList[i] = (i==6) ? quant8_intra_default:quant8_inter_default;
    					vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
  					}
  					else
  					{          
  						ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = TRUE;
    					ptList[i] = ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->cScalingList8x8[i-6];
    					vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
  					}
				}
			}
		}

		if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgPicScalingMatrixPresentFlag) // then check pps
		{
			for(i=0; i<8; i++)
			{
				if(i<6)
				{
					if(!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgPicScalingListPresentFlag[i]) // fall-back rule A
				  	{
				    	if((i==0) || (i==3))
					    {              
					    	if(!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgSeqScalingMatrixPresentFlag)
					      	{
					        	ptList[i] = (i==0) ? quant_intra_default:quant_inter_default;
					        	//ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = TRUE;
					        	vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
					      	}
					    }
				    	else
					    {
					      	ptList[i] = ptList[i-1];
					      	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i-1];
					        vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
					    }
				  	}
				  	else
				  	{
				    	if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgUseDefaultScalingMatrix4x4Flag[i])
					    {
					      	ptList[i] = (i<3) ? quant_intra_default:quant_inter_default;
					      	//ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = TRUE;
					      	vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
					    }
				    	else
					    {
					      	ptList[i] = ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->cScalingList4x4[i];
					      	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = TRUE;
					      	vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
					    }
				  	}
				}
				else
				{
			  		if(!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgPicScalingListPresentFlag[i]) // fall-back rule B
				  	{
				    	if(!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgSeqScalingMatrixPresentFlag)
					    {
					      	ptList[i] = (i==6) ? quant8_intra_default:quant8_inter_default;
					      	//ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = TRUE;
					      	vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
					    }
				  	}
			  		else if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgUseDefaultScalingMatrix8x8Flag[i-6])
				  	{
				    	ptList[i] = (i==6) ? quant8_intra_default:quant8_inter_default;
				    	//ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = TRUE;
				    	vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);
				  	}
			  		else
				  	{
				    	ptList[i] = ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->cScalingList8x8[i-6];
				    	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[i] = TRUE;
				    	vVDEC_HAL_H264_WriteScalingList(u4InstID,i, ptList[i]);       
				  	}
				}
			}
		}
	}
}


// *********************************************************************
// Function    : void vVerifyFlushBufRefInfo(UINT32 u4InstID)
// Description : flush DPB Ref info
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyFlushBufRefInfo(UINT32 u4InstID)
{
	UINT32 i;

	_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.u4MaxLongTermFrameIdx = 0xffffffff;

	for(i=0; i<17; i++)
	{
		_ptFBufInfo[u4InstID][i].fgNonExisting = FALSE;        
		_ptFBufInfo[u4InstID][i].ucFBufRefType = NREF_PIC;
		_ptFBufInfo[u4InstID][i].ucTFldRefType = NREF_PIC;
		_ptFBufInfo[u4InstID][i].ucBFldRefType = NREF_PIC;    
		_ptFBufInfo[u4InstID][i].u4LongTermFrameIdx = 0xffffffff;
		_ptFBufInfo[u4InstID][i].u4TFldLongTermFrameIdx = 0xffffffff;
		_ptFBufInfo[u4InstID][i].u4BFldLongTermFrameIdx = 0xffffffff;
	}
	for(i=0; i<3; i++)
	{        
		_ptRefPicList[u4InstID][i].u4RefPicCnt = 0;
	} 
}


// *********************************************************************
// Function    : void vVerifyVDecSetPicInfo(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm)
// Description : Set Pic related info before reordering
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyVDecSetPicInfo(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm)
{	
	if (VDEC_LAE_ST == ucCurrStatus)
	{
		//BOOL fgDiff;
		//UINT32 u4CurrPicNum, u4Temp;
		UTIL_Printf("LAE Set info start\n");	
		/*u4CurrPicNum = (fgIsFrmPic(u4InstID))? _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum : ((_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum << 1) +1);
		vVDEC_HAL_H264_InitPRefList(u4InstID, fgIsFrmPic(u4InstID), _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4MaxFrameNum, u4CurrPicNum);
		u4Temp = 0;
		vSetupPRefPicList(u4InstID, &u4Temp, 0, 1);
		vSetupPRefPicList(u4InstID, &u4Temp, 4, 5);
		u4Temp = 0;
		fgDiff = FALSE;
		vSetupBRefPicList(u4InstID, &u4Temp, 0, 1, &fgDiff);
		vSetupBRefPicList(u4InstID, &u4Temp, 4, 5, &fgDiff);*/
		vPrepareRefPiclist(u4InstID, ptVerMpvDecPrm);
		vVDecSetPRefPicList(u4InstID);
		vVDecSetBRefPicList(u4InstID);
		vAssignQuantParam(u4VDecID, ptVerMpvDecPrm);
		vVDecWriteMCORETOP(u4VDecID, RW_MCORE_CABAC0, PHYSICAL(_pucBSCABACBuff[0]));
		vVDecWriteMCORETOP(u4VDecID, RW_MCORE_EC0, PHYSICAL(_pucECInfoBuff[0]));
		UTIL_Printf("LAE Set info end\n");
	}
	else if (VDEC_CORE1_ST == ucCurrStatus)
	{		
	#ifdef VDEC_SIM_DUMP
		UTIL_Printf("Set Core1 part\n");
	#endif
		//copy avc param from core0
		x_memcpy(&_tVerMpvDecPrm[1],&_tVerMpvDecPrm[2],sizeof(VDEC_INFO_DEC_PRM_T));
		//copy  fb info
		x_memcpy(&_ptFBufInfo[1],&_ptFBufInfo[2],(sizeof(VDEC_INFO_H264_FBUF_INFO_T) * 17));
		//copy current fb info
		_ptCurrFBufInfo[1] = _ptCurrFBufInfo[2];    
		//copy
		x_memcpy(&_ptRefPicList[1],&_ptRefPicList[2],sizeof(VDEC_INFO_H264_REF_PIC_LIST_T));
		x_memcpy(&_arPRefPicListInfo[1], &_arPRefPicListInfo[2], sizeof(VDEC_INFO_H264_P_REF_PRM_T));
		x_memcpy(&_arBRefPicListInfo[1], &_arBRefPicListInfo[2], sizeof(VDEC_INFO_H264_B_REF_PRM_T));
		vVDecSetPRefPicList(u4VDecID);
		vVDecSetBRefPicList(u4VDecID);
		vAssignQuantParam(u4VDecID, ptVerMpvDecPrm);
		ucCurrStatus = VDEC_NORMAL_ST;
	#ifdef VDEC_SIM_DUMP
		UTIL_Printf("Set Core1 part done\n");
	#endif
	}
	else
	{
	#ifdef VDEC_SIM_DUMP
		UTIL_Printf("Set Core0 part\n");
	#endif
		if (0 == _u4DualCoreEnable[0])
		{
			vPrepareRefPiclist(u4InstID, ptVerMpvDecPrm);
		}
		else
		{
			//copy avc param from core0
			x_memcpy(&_tVerMpvDecPrm[0],&_tVerMpvDecPrm[2],sizeof(VDEC_INFO_DEC_PRM_T));
			//copy  fb info
			x_memcpy(&_ptFBufInfo[0],&_ptFBufInfo[2],(sizeof(VDEC_INFO_H264_FBUF_INFO_T) * 17));
			//copy current fb info
			_ptCurrFBufInfo[0] = _ptCurrFBufInfo[2];    
			//copy
			x_memcpy(&_ptRefPicList[0],&_ptRefPicList[2],sizeof(VDEC_INFO_H264_REF_PIC_LIST_T));
			x_memcpy(&_arPRefPicListInfo[0], &_arPRefPicListInfo[2], sizeof(VDEC_INFO_H264_P_REF_PRM_T));
			x_memcpy(&_arBRefPicListInfo[0], &_arBRefPicListInfo[2], sizeof(VDEC_INFO_H264_B_REF_PRM_T));
		}
		vVDecSetPRefPicList(u4InstID);
		vVDecSetBRefPicList(u4InstID);
		vAssignQuantParam(u4VDecID, ptVerMpvDecPrm);	
	#ifdef VDEC_SIM_DUMP
		UTIL_Printf("Set Core0 part done\n");
	#endif
	}
	
	vVDEC_HAL_H264_SetSPSAVLD(u4VDecID, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS);
	vVDEC_HAL_H264_SetPPSAVLD(u4VDecID,  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgUserScalingMatrixPresentFlag,
	&(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgUserScalingListPresentFlag[0]), ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS);
	vVDEC_HAL_H264_SetSHDRAVLD1(u4VDecID, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr);  
}


// *********************************************************************
// Function    : void   vVerifyPrepareFBufInfo(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm)
// Description : check pic type to send P_0, B_0, B_1
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyPrepareFBufInfo(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm)
{
	tVerMpvDecPrm->u4PicW = (tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicWidthInMbsMinus1 + 1) << 4;
	tVerMpvDecPrm->u4PicH = (2 - tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgFrameMbsOnlyFlag)*(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicHeightInMapUnitsMinus1 + 1) << 4; //32x
	tVerMpvDecPrm->u4PicBW = tVerMpvDecPrm->u4PicW;
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.u4RealPicH = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicHeightInMapUnitsMinus1 << 4;  // original real size

	if((tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicW != tVerMpvDecPrm->u4PicW) || (tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicH != tVerMpvDecPrm->u4PicH))
	{
		vPartitionDPB(u4InstID);      
	}

//if(tVerMpvDecPrm->prSliceHdr->fgNoOutputOfPriorPicsFlag) // clear all prior pic in DPB
	if(fgIsIDRPic(u4InstID))
	{
		tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum = 0xffffffff;
	//vFlushDPB(u4InstID, tVerMpvDecPrm, FALSE);
		if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgNoOutputOfPriorPicsFlag)
		{
			vFlushDPB(u4InstID, tVerMpvDecPrm, FALSE);
		}
		else
		{
			vFlushDPB(u4InstID, tVerMpvDecPrm, TRUE);
		}
	} 

	if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgGapsInFrameNumValueAllowedFlag)
	{
		vFillFrameNumGap(u4InstID, tVerMpvDecPrm);
	}

// Find a empty fbuf 
	if((_ptCurrFBufInfo[u4InstID]->ucFBufStatus == NO_PIC)
	|| (_ptCurrFBufInfo[u4InstID]->ucFBufStatus & tVerMpvDecPrm->ucPicStruct))
	{
		vAllocateFBuf(u4InstID, tVerMpvDecPrm, TRUE);   
	}
	
	//UTIL_Printf("before fbufstatus %d, picstruct %d\n", _ptCurrFBufInfo[u4InstID]->ucFBufStatus, _tVerMpvDecPrm[u4InstID].ucPicStruct);
	_ptCurrFBufInfo[u4InstID]->ucFBufStatus |= _tVerMpvDecPrm[u4InstID].ucPicStruct;
	//UTIL_Printf("fbufstatus %d, picstruct %d\n", _ptCurrFBufInfo[u4InstID]->ucFBufStatus, _tVerMpvDecPrm[u4InstID].ucPicStruct);

	if(tVerMpvDecPrm->ucPicStruct & TOP_FIELD)
	{
		_ptCurrFBufInfo[u4InstID]->u4TFldPara = ((fgIsFrmPic(u4InstID)? 0 : 1) << 19) + ((fgIsFrmPic(u4InstID) && tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgMbAdaptiveFrameFieldFlag) << 18);
	}
	if(tVerMpvDecPrm->ucPicStruct & BOTTOM_FIELD)
	{
		_ptCurrFBufInfo[u4InstID]->u4BFldPara = ((fgIsFrmPic(u4InstID)? 0 : 1) << 19) + ((fgIsFrmPic(u4InstID) && tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgMbAdaptiveFrameFieldFlag) << 18);
	}
}


// *********************************************************************
// Function    : void vVDecSetPRefPicList(UINT32 u4InstID)
// Description : Set P ref pic list by Pic Num
// Parameter   : None
// Return      : None
// *********************************************************************
void vVDecSetPRefPicList(UINT32 u4InstID)
{
	INT32 i;
	UINT32 u4AddTop;
	UINT32 u4AddBot;
	UINT32 u4Temp;
	UINT32 u4CurrPicNum;
	UINT32 u4Idx;

	_ptCurrFBufInfo[u4InstID]->u4FrameNum = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum;
	u4CurrPicNum = (fgIsFrmPic(u4InstID))? _ptCurrFBufInfo[u4InstID]->u4FrameNum : ((_ptCurrFBufInfo[u4InstID]->u4FrameNum << 1) +1);
	_ptCurrFBufInfo[u4InstID]->i4PicNum = u4CurrPicNum;
	_ptRefPicList[u4InstID][0].u4RefPicCnt = 0;
	_ptRefPicList[u4InstID][1].u4RefPicCnt = 0;  
	_ptRefPicList[u4InstID][4].u4RefPicCnt = 0;
	_ptRefPicList[u4InstID][5].u4RefPicCnt = 0;

	for(i=0; i<_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
	{
		if(_tVerMpvDecPrm[u4InstID].ucPicStruct == TOP_FIELD)
		{
			u4AddTop = 1;
			u4AddBot = 0;
		}
		else if(_tVerMpvDecPrm[u4InstID].ucPicStruct == BOTTOM_FIELD)
		{
			u4AddTop = 0;
			u4AddBot = 1;
		}
		else
		{
			u4AddTop = 0;
			u4AddBot = 0;
		}

		if(fgChkRefInfo(u4InstID, i, SREF_PIC))
		{        
			if(_ptFBufInfo[u4InstID][i].u4FrameNum > _ptCurrFBufInfo[u4InstID]->u4FrameNum)
			{
			  	_ptFBufInfo[u4InstID][i].i4FrameNumWrap = _ptFBufInfo[u4InstID][i].u4FrameNum - _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4MaxFrameNum;
			}
			else
			{
			  	_ptFBufInfo[u4InstID][i].i4FrameNumWrap = _ptFBufInfo[u4InstID][i].u4FrameNum;
			}
			if(fgIsFrmPic(u4InstID))
			{
			  	_ptFBufInfo[u4InstID][i].i4PicNum = _ptFBufInfo[u4InstID][i].i4FrameNumWrap;
			}
			else
			{
			  	_ptFBufInfo[u4InstID][i].i4PicNum = (_ptFBufInfo[u4InstID][i].i4FrameNumWrap<<1) + 1;
			}

			if(_ptFBufInfo[u4InstID][i].ucTFldRefType == SREF_PIC)
			{
			  	_ptFBufInfo[u4InstID][i].i4TFldPicNum = (_ptFBufInfo[u4InstID][i].i4FrameNumWrap << 1) + u4AddTop;
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][0], i, SREF_PIC + (0 <<8) + ( i<<16));          
			}
			if(_ptFBufInfo[u4InstID][i].ucBFldRefType == SREF_PIC)
			{
			  	_ptFBufInfo[u4InstID][i].i4BFldPicNum = (_ptFBufInfo[u4InstID][i].i4FrameNumWrap << 1) + u4AddBot;
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][1], i, SREF_PIC + (1 <<8) + ( i<<16));          
			}
		}
		else if(fgChkRefInfo(u4InstID, i, LREF_PIC))
		{        
			_ptFBufInfo[u4InstID][i].i4LongTermPicNum = _ptFBufInfo[u4InstID][i].u4LongTermFrameIdx;

			if(_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC)
			{
			  	u4Idx = _ptFBufInfo[u4InstID][i].u4TFldLongTermFrameIdx;
			  	_ptFBufInfo[u4InstID][i].i4TFldLongTermPicNum = (u4Idx << 1) + u4AddTop;
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][4], i, LREF_PIC + (6 <<8) + ( i<<16));          
			} 
			if( _ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC)
			{
			  	_ptFBufInfo[u4InstID][i].i4BFldLongTermPicNum = (_ptFBufInfo[u4InstID][i].u4BFldLongTermFrameIdx << 1) + u4AddBot;        
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][5], i, LREF_PIC + (7 <<8) + ( i<<16));          
			}        
		}      
	}

	vVDEC_HAL_H264_InitPRefList(u4InstID, fgIsFrmPic(u4InstID), _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4MaxFrameNum, u4CurrPicNum);
	//fprintf(_tRecFileInfo.fpFile, "frame num = %d\n", _ptCurrFBufInfo->u4FrameNum);    
	u4Temp = 0;
	vSetupPRefPicList(u4InstID, &u4Temp, 0, 1);
	vSetupPRefPicList(u4InstID, &u4Temp, 4, 5);

#if defined(MVC_VFY_SUPPORT) 
	if(_ucMVCType[u4InstID] == 2)
	{
		vAppendInterviewRefPicList(u4InstID, &u4Temp, 0);
	}
#endif  
}


// *********************************************************************
// Function    : void vSetupPRefPicList(UINT32 u4InstID, UINT32 *pu4RefIdx, UINT32 u4TFldListIdx, UINT32 u4BFldListIdx)
// Description : Setup Ref Pic List
// Parameter   : None
// Return      : None
// *********************************************************************
void vSetupPRefPicList(UINT32 u4InstID, UINT32 *pu4RefIdx, UINT32 u4TFldListIdx, UINT32 u4BFldListIdx)
{
	INT32 i ;
	UINT32 u4TotalFBuf;  
	UINT32 u4DpbBaseOffset;  
	
	VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo;
	prPRefPicListInfo = &_arPRefPicListInfo[u4InstID];
	u4DpbBaseOffset = 0;  
#if defined(MVC_VFY_SUPPORT) 
	u4DpbBaseOffset = (_ucMVCType[u4InstID] == 2)? 
	                         _tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum << 1
	                         : 0;
#endif
//VDEC_INFO_H264_P_REF_PRM_T rPRefPicListInfo;
	u4TotalFBuf = (_ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt >= _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt)?
                 	_ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt : _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt;
	if(fgIsFrmPic(u4InstID))
	{
		u4TFldListIdx = (_ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt >= _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt)?
	                       		u4TFldListIdx : u4BFldListIdx;
	}
	for(i=0; i<u4TotalFBuf; i++)
	{
		if(fgIsFrmPic(u4InstID))
		{
			prPRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4TFldListIdx].u4FBufIdx[i];
			prPRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
			prPRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPicNum;
			prPRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPOC;
			prPRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4PicNum;
			prPRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
			prPRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
			prPRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPicNum;
			prPRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPOC;
			prPRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4BFldPara;
			prPRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4Addr;
			prPRefPicListInfo->u4FBufInfo = FRAME + (i << 8) + (pu4RefIdx[0] << 16);
			prPRefPicListInfo->u4ListIdx = u4TFldListIdx;
			prPRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4TFldPara;
			#ifdef Y_C_SEPERATE
			prPRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YStartAddr;
			prPRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CStartAddr;
			prPRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YLenOffset;
			prPRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CLenOffset;
			#endif
			prPRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
			vVDEC_HAL_H264_SetPRefPicListReg(u4InstID, prPRefPicListInfo);
			//vVDecSetPRefPicListReg(FRAME + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);         
			pu4RefIdx[0] ++;
		}
		else if(_tVerMpvDecPrm[u4InstID].ucPicStruct == TOP_FIELD)
		{
			if(i < _ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt)
			{
				prPRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4TFldListIdx].u4FBufIdx[i];
				prPRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				prPRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				prPRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPOC;
				prPRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4PicNum;
				prPRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				prPRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				prPRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				prPRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPOC;
				prPRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4BFldPara;
				prPRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4Addr;
				prPRefPicListInfo->u4FBufInfo = TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
				prPRefPicListInfo->u4ListIdx = u4TFldListIdx;
				prPRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4TFldPara;
				#ifdef Y_C_SEPERATE
				prPRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YStartAddr;
				prPRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CStartAddr;
				prPRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YLenOffset;
				prPRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CLenOffset;
				#endif
				prPRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;				
				vVDEC_HAL_H264_SetPRefPicListReg(u4InstID, prPRefPicListInfo);
				//vVDecSetPRefPicListReg(TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
				pu4RefIdx[0] ++;
			}
			if(i < _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt)
			{
				prPRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4BFldListIdx].u4FBufIdx[i];
				prPRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				prPRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				prPRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPOC;
				prPRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4PicNum;
				prPRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				prPRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				prPRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				prPRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPOC;
				prPRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4BFldPara;
				prPRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4Addr;
				prPRefPicListInfo->u4FBufInfo = BOTTOM_FIELD + (i << 8)  + (pu4RefIdx[0] << 16);
				prPRefPicListInfo->u4ListIdx = u4BFldListIdx;
				prPRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4TFldPara;
				#ifdef Y_C_SEPERATE
				prPRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YStartAddr;
				prPRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CStartAddr;
				prPRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YLenOffset;
				prPRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CLenOffset;
				#endif
				prPRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				vVDEC_HAL_H264_SetPRefPicListReg(u4InstID, prPRefPicListInfo);
				//vVDecSetPRefPicListReg(BOTTOM_FIELD + (i << 8)  + (pu4RefIdx[0] << 16), u4BFldListIdx);
				pu4RefIdx[0] ++;
			} 
		}
		else if(_tVerMpvDecPrm[u4InstID].ucPicStruct == BOTTOM_FIELD)
		{
			if(i < _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt)
			{
				prPRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4BFldListIdx].u4FBufIdx[i];
				prPRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				prPRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				prPRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPOC;
				prPRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4PicNum;
				prPRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				prPRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				prPRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				prPRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPOC;
				prPRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4BFldPara;
				prPRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4Addr;
				prPRefPicListInfo->u4FBufInfo = BOTTOM_FIELD + (i << 8)  + (pu4RefIdx[0] << 16);
				prPRefPicListInfo->u4ListIdx = u4BFldListIdx;
				prPRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4TFldPara;
				#ifdef Y_C_SEPERATE
				prPRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YStartAddr;
				prPRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CStartAddr;
				prPRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YLenOffset;
				prPRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CLenOffset;
				#endif
				prPRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				vVDEC_HAL_H264_SetPRefPicListReg(u4InstID, prPRefPicListInfo);
				//vVDecSetPRefPicListReg(BOTTOM_FIELD + (i << 8)  + (pu4RefIdx[0] << 16), u4BFldListIdx);
				pu4RefIdx[0] ++;
			}
			if(i < _ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt)
			{
				prPRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4TFldListIdx].u4FBufIdx[i];
				prPRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				prPRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				prPRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4BFldPOC;
				prPRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4PicNum;
				prPRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				prPRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				prPRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				prPRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].i4TFldPOC;
				prPRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4BFldPara;
				prPRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4Addr;
				prPRefPicListInfo->u4FBufInfo = TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
				prPRefPicListInfo->u4ListIdx = u4TFldListIdx;
				prPRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4TFldPara;
				#ifdef Y_C_SEPERATE
				prPRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YStartAddr;
				prPRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CStartAddr;
				prPRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4YLenOffset;
				prPRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prPRefPicListInfo->ucFBufIdx].u4CLenOffset;
				#endif
				prPRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				vVDEC_HAL_H264_SetPRefPicListReg(u4InstID, prPRefPicListInfo);
				//vVDecSetPRefPicListReg(TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
				pu4RefIdx[0] ++;
			} 
		}
	}  
}


// *********************************************************************
// Function    : void vVDecSetBRefPicList(UINT32 u4InstID)
// Description : Set Ref Pic List for P slice
// Parameter   : None
// Return      : None
// *********************************************************************
void vVDecSetBRefPicList(UINT32 u4InstID)
{
	INT32 i;
	UINT32 u4Temp;
	INT32 iCurrPOC;
	UINT32 u4TotalRPIdx;
	BOOL fgDiff;
	VDEC_INFO_H264_POC_PRM_T rPOCInfo;
	vVDecSetCurrPOC(u4InstID);   
	if(fgIsFrmPic(u4InstID))
	{
		iCurrPOC = _ptCurrFBufInfo[u4InstID]->i4POC;
	}
	else
	{
		iCurrPOC = (_tVerMpvDecPrm[u4InstID].ucPicStruct == TOP_FIELD)? _ptCurrFBufInfo[u4InstID]->i4TFldPOC : _ptCurrFBufInfo[u4InstID]->i4BFldPOC;
	}
	rPOCInfo.ucPicStruct = _tVerMpvDecPrm[u4InstID].ucPicStruct;
	rPOCInfo.fgIsFrmPic = fgIsFrmPic(u4InstID);
	rPOCInfo.i4BFldPOC = _ptCurrFBufInfo[u4InstID]->i4BFldPOC;
	rPOCInfo.i4POC = _ptCurrFBufInfo[u4InstID]->i4POC;
	rPOCInfo.i4TFldPOC = _ptCurrFBufInfo[u4InstID]->i4TFldPOC;
	vVDEC_HAL_H264_SetPOC( u4InstID, &rPOCInfo);
	_ptRefPicList[u4InstID][0].u4RefPicCnt = 0;  
	_ptRefPicList[u4InstID][1].u4RefPicCnt = 0;  
	_ptRefPicList[u4InstID][2].u4RefPicCnt = 0;  
	_ptRefPicList[u4InstID][3].u4RefPicCnt = 0;  
	_ptRefPicList[u4InstID][4].u4RefPicCnt = 0;  
	_ptRefPicList[u4InstID][5].u4RefPicCnt = 0;  
	for(i=0; i<_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
	{     
		if(fgChkRefInfo(u4InstID, i, SREF_PIC))
		{
		// Avoid non-existing pic into ref pic list when POC type = 0
			if((_ptFBufInfo[u4InstID][i].ucTFldRefType == SREF_PIC)) 
		//            && !((_tVerMpvDecPrm.SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicOrderCntType == 0) && (_ptFBufInfo[i].fgNonExisting)))
			{
			  	// B0
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][0], iCurrPOC, SREF_PIC + (2 <<8) + (i<<16));
			  	// B1
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][2], iCurrPOC, SREF_PIC + (4 <<8) + (i<<16));      
			}
			if((_ptFBufInfo[u4InstID][i].ucBFldRefType == SREF_PIC) 
		    	&& !((_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicOrderCntType == 0) && (_ptFBufInfo[u4InstID][i].fgNonExisting)))
			{
			  	// B0
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][1], iCurrPOC, SREF_PIC + (3 <<8) + (i <<16));
			  	// B1
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][3], iCurrPOC, SREF_PIC + (5 <<8) + (i <<16));    
			}
		}
		else if(fgChkRefInfo(u4InstID, i, LREF_PIC))
		{
			if(_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC)
			{
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][4], iCurrPOC, LREF_PIC + (8 <<8) + (i <<16));
			}
			if(_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC)
			{
			  	vInsertRefPicList(u4InstID, &_ptRefPicList[u4InstID][5], iCurrPOC, LREF_PIC + (9 <<8) + (i <<16)); 
			}        
		}      
	}
	vVDEC_HAL_H264_InitBRefList(u4InstID);

	//vWriteAVCVLD(RW_AVLD_RESET_PIC_NUM, RESET_PIC_NUM);
	u4Temp = 0;
	fgDiff = FALSE;
	//fprintf(_tRecFileInfo.fpFile,"B0 \n");  
	vSetupBRefPicList(u4InstID, &u4Temp, 0, 1, &fgDiff);
	vSetupBRefPicList(u4InstID, &u4Temp, 4, 5, &fgDiff);

	if(fgIsFrmPic(u4InstID))
	{
		u4TotalRPIdx = (_ptRefPicList[u4InstID][0].u4RefPicCnt < _ptRefPicList[u4InstID][1].u4RefPicCnt)? _ptRefPicList[u4InstID][1].u4RefPicCnt : _ptRefPicList[u4InstID][0].u4RefPicCnt;
		u4TotalRPIdx += (_ptRefPicList[u4InstID][4].u4RefPicCnt < _ptRefPicList[u4InstID][5].u4RefPicCnt)? _ptRefPicList[u4InstID][5].u4RefPicCnt : _ptRefPicList[u4InstID][4].u4RefPicCnt;
	}
	else
	{
		u4TotalRPIdx = _ptRefPicList[u4InstID][0].u4RefPicCnt + _ptRefPicList[u4InstID][1].u4RefPicCnt + _ptRefPicList[u4InstID][4].u4RefPicCnt + _ptRefPicList[u4InstID][5].u4RefPicCnt;
	}

	// in field pic, if B0 & B1 identical, switch the 1st 2 items
	if(u4TotalRPIdx > 1 && (!fgDiff))
	{
		vVDEC_HAL_H264_B1ListSwap(u4InstID, fgIsFrmPic(u4InstID));
	}

#if defined(MVC_VFY_SUPPORT) 
	if(_ucMVCType[u4InstID] == 2)
	{
		vAppendInterviewRefPicList(u4InstID, &u4Temp, 1);
		vAppendInterviewRefPicList(u4InstID, &u4Temp, 2);
	}
#endif  
}


// *********************************************************************
// Function    : void vSetupBRefPicList(UINT32 u4InstID, UINT32 *pu4RefIdx, UINT32 u4TFldListIdx, UINT32 u4BFldListIdx)
// Description : Setup Ref Pic List
// Parameter   : None
// Return      : None
// *********************************************************************
void vSetupBRefPicList(UINT32 u4InstID, UINT32 *pu4RefIdx, UINT32 u4TFldListIdx, UINT32 u4BFldListIdx, BOOL *fgDiff)
{
	INT32 i ;
	UINT32 u4TotalFBuf;
	UINT32 u4Cnt[2];
	BOOL fgIsDiff;
	//VDEC_INFO_H264_B_REF_PRM_T rBRefPicListInfo;
	UINT32 u4DpbBaseOffset;  

	VDEC_INFO_H264_B_REF_PRM_T *prBRefPicListInfo;
	prBRefPicListInfo = &_arBRefPicListInfo[u4InstID];
	u4DpbBaseOffset = 0;  
#if defined(MVC_VFY_SUPPORT) 
	u4DpbBaseOffset = (_ucMVCType[u4InstID] == 2)? 
	                         _tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum << 1
	                         : 0;
#endif

	u4Cnt[0]=pu4RefIdx[0];  
	u4Cnt[1]=pu4RefIdx[0]; 

	u4TotalFBuf = (_ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt >= _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt)?
	                 _ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt : _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt;
	if(fgIsFrmPic(u4InstID))
	{
		u4TFldListIdx = (_ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt >= _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt)?
	                       	u4TFldListIdx : u4BFldListIdx;
	}  

	for(i=0; i<u4TotalFBuf; i++)
	{
		if(fgIsFrmPic(u4InstID))
		{
			prBRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4TFldListIdx].u4FBufIdx[i];
			prBRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
			prBRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPicNum;
			prBRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPOC;
			prBRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4PicNum;    
			prBRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
			prBRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
			prBRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPicNum;
			prBRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPOC;
			//prBRefPicListInfo->u4DramPicSize = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize;
			prBRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4BFldPara;
			prBRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4Addr;
		#if VDEC_H264_REDUCE_MV_BUFF
			prBRefPicListInfo->u4FBufMvStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4MvStartAddr;
		#else
			prBRefPicListInfo->u4FBufMvStartAddr = (prBRefPicListInfo->u4FBufYStartAddr  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize * 3) >>1));
		#endif
			prBRefPicListInfo->u4FBufInfo = FRAME + (i << 8) + (pu4RefIdx[0] << 16);
			prBRefPicListInfo->u4ListIdx = u4TFldListIdx;
			prBRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4TFldPara;
			if(prBRefPicListInfo->u4ListIdx < 4)
			{
				prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx + 2;
			}
			else
			{
				prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx;
			}
			prBRefPicListInfo->ucFBufIdx1 = _ptRefPicList[u4InstID][prBRefPicListInfo->u4ListIdx1].u4FBufIdx[i];
			prBRefPicListInfo->u4FBufYStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4Addr;
			prBRefPicListInfo->i4LongTermPicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4LongTermPicNum;
			prBRefPicListInfo->i4PicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4PicNum;
			prBRefPicListInfo->i4TFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4TFldPOC;
			prBRefPicListInfo->i4BFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPOC;
			prBRefPicListInfo->u4TFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4TFldPara;
			prBRefPicListInfo->u4BFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4BFldPara;
		#if VDEC_H264_REDUCE_MV_BUFF      
			prBRefPicListInfo->u4FBufMvStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4MvStartAddr;
		#else
			//prBRefPicListInfo->u4DramPicSize1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize;
			//prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((prBRefPicListInfo->u4DramPicSize1 * 3) >>1));
			prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize * 3) >>1));
		#endif
			#ifdef Y_C_SEPERATE
			prBRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YStartAddr;
			prBRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CStartAddr;
			prBRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YLenOffset;
			prBRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CLenOffset;
			prBRefPicListInfo->u4YStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YStartAddr;
			prBRefPicListInfo->u4CStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CStartAddr;
			prBRefPicListInfo->u4YLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YLenOffset;
			prBRefPicListInfo->u4CLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CLenOffset;
			#endif
			prBRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
			prBRefPicListInfo->ucFBufIdx1 += u4DpbBaseOffset;
			if (VDEC_LAE0 == u4InstID)
			{
				fgIsDiff = bVDEC_HAL_H264_LAE_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
			}
			else
			{
				fgIsDiff = bVDEC_HAL_H264_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
			}
			//vVDecSetBRefPicListReg(FRAME + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
			if((!fgDiff[0]) && fgIsDiff)
			{
				fgDiff[0] = TRUE;
			}
			pu4RefIdx[0] ++;
		}
		else if(_tVerMpvDecPrm[u4InstID].ucPicStruct == TOP_FIELD)
		{
			if(i < _ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt)
			{
				prBRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4TFldListIdx].u4FBufIdx[i];
				prBRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				prBRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				prBRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPOC;
				prBRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4PicNum;    
				prBRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				prBRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				prBRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				prBRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPOC;
				//prBRefPicListInfo->u4DramPicSize = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize;
				prBRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4BFldPara;
				prBRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4Addr;
			#if VDEC_H264_REDUCE_MV_BUFF
				prBRefPicListInfo->u4FBufMvStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4MvStartAddr;
			#else
				prBRefPicListInfo->u4FBufMvStartAddr = (prBRefPicListInfo->u4FBufYStartAddr  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize * 3) >>1));
			#endif
				prBRefPicListInfo->u4FBufInfo = TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
				prBRefPicListInfo->u4ListIdx = u4TFldListIdx;
				prBRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4TFldPara;
				if(prBRefPicListInfo->u4ListIdx < 4)
				{
				  	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx + 2;
				}
				else
				{
				  	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx;
				}
				prBRefPicListInfo->ucFBufIdx1 = _ptRefPicList[u4InstID][prBRefPicListInfo->u4ListIdx1].u4FBufIdx[i];
				prBRefPicListInfo->u4FBufYStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4Addr;;
				prBRefPicListInfo->i4LongTermPicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4LongTermPicNum;
				prBRefPicListInfo->i4PicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPicNum;;
				prBRefPicListInfo->i4TFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4TFldPOC;
				prBRefPicListInfo->i4BFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPOC;
				prBRefPicListInfo->u4TFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4TFldPara;
				prBRefPicListInfo->u4BFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4BFldPara;
			#if VDEC_H264_REDUCE_MV_BUFF
				prBRefPicListInfo->u4FBufMvStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4MvStartAddr;
			#else
				//prBRefPicListInfo->u4DramPicSize1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize;
				//prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((prBRefPicListInfo->u4DramPicSize1 * 3) >>1));
				prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize * 3) >>1));
			#endif
				#ifdef Y_C_SEPERATE
				prBRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YStartAddr;
				prBRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CStartAddr;
				prBRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YLenOffset;
				prBRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CLenOffset;
				prBRefPicListInfo->u4YStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YStartAddr;
				prBRefPicListInfo->u4CStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CStartAddr;
				prBRefPicListInfo->u4YLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YLenOffset;
				prBRefPicListInfo->u4CLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CLenOffset;
				#endif
				prBRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				prBRefPicListInfo->ucFBufIdx1 += u4DpbBaseOffset;
				if (VDEC_LAE0 == u4InstID)
				{
					fgIsDiff = bVDEC_HAL_H264_LAE_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
				}
				else
				{
					fgIsDiff = bVDEC_HAL_H264_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
				}
				//vVDecSetBRefPicListReg(TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
				if(u4TFldListIdx < 4) // Short-term only
				{
					prBRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4TFldListIdx+2].u4FBufIdx[i];
					prBRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
					prBRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPicNum;
					prBRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPOC;
					prBRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4PicNum;    
					prBRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
					prBRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
					prBRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPicNum;
					prBRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPOC;
					//prBRefPicListInfo->u4DramPicSize = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize;
					prBRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4BFldPara;
					prBRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4Addr;
				#if VDEC_H264_REDUCE_MV_BUFF
					prBRefPicListInfo->u4FBufMvStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4MvStartAddr;
				#else
					prBRefPicListInfo->u4FBufMvStartAddr = (prBRefPicListInfo->u4FBufYStartAddr  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize * 3) >>1));
				#endif
					prBRefPicListInfo->u4FBufInfo = TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
					prBRefPicListInfo->u4ListIdx = u4TFldListIdx+2;
					prBRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4TFldPara;
			  		if(prBRefPicListInfo->u4ListIdx < 4)
				  	{
				    	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx + 2;
				  	}
			  		else
				  	{
				    	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx;
				  	}
				  	prBRefPicListInfo->ucFBufIdx1 = _ptRefPicList[u4InstID][prBRefPicListInfo->u4ListIdx1].u4FBufIdx[i];
				  	prBRefPicListInfo->u4FBufYStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4Addr;;
				  	prBRefPicListInfo->i4LongTermPicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4LongTermPicNum;
				  	prBRefPicListInfo->i4PicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPicNum;;
				  	prBRefPicListInfo->i4TFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4TFldPOC;
				  	prBRefPicListInfo->i4BFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPOC;
				  	prBRefPicListInfo->u4TFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4TFldPara;
				  	prBRefPicListInfo->u4BFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4BFldPara;
				#if VDEC_H264_REDUCE_MV_BUFF
			  		prBRefPicListInfo->u4FBufMvStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4MvStartAddr;
				#else          
			  		//prBRefPicListInfo->u4DramPicSize1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize;
			  		//prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((prBRefPicListInfo->u4DramPicSize1 * 3) >>1));
			  		prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize * 3) >>1));
				#endif
					#ifdef Y_C_SEPERATE
					prBRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YStartAddr;
					prBRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CStartAddr;
					prBRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YLenOffset;
					prBRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CLenOffset;
					prBRefPicListInfo->u4YStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YStartAddr;
					prBRefPicListInfo->u4CStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CStartAddr;
					prBRefPicListInfo->u4YLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YLenOffset;
					prBRefPicListInfo->u4CLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CLenOffset;
					#endif
			  		prBRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
			  		prBRefPicListInfo->ucFBufIdx1 += u4DpbBaseOffset;
					if (VDEC_LAE0 == u4InstID)
					{
						fgIsDiff = bVDEC_HAL_H264_LAE_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
					}
					else
					{
			  			fgIsDiff = bVDEC_HAL_H264_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
					}
			  		//vVDecSetBRefPicListReg(TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx+2);
				}

				if((!fgDiff[0]) && fgIsDiff)
				{
			  		fgDiff[0] = TRUE;
				}
				pu4RefIdx[0] ++;
				u4Cnt[0] ++;
			}
			if(i < _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt)
			{
				prBRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4BFldListIdx].u4FBufIdx[i];
				prBRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				prBRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				prBRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPOC;
				prBRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4PicNum;    
				prBRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				prBRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				prBRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				prBRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPOC;
				//prBRefPicListInfo->u4DramPicSize = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize;
				prBRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4BFldPara;
				prBRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4Addr;
			#if VDEC_H264_REDUCE_MV_BUFF
				//prBRefPicListInfo->u4FBufMvStartAddr = prBRefPicListInfo->u4FBufMvStartAddr;
				prBRefPicListInfo->u4FBufMvStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4MvStartAddr;
			#else
				prBRefPicListInfo->u4FBufMvStartAddr = (prBRefPicListInfo->u4FBufYStartAddr  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize * 3) >>1));
			#endif
				prBRefPicListInfo->u4FBufInfo = BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
				prBRefPicListInfo->u4ListIdx = u4BFldListIdx;
				prBRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4TFldPara;
				if(prBRefPicListInfo->u4ListIdx < 4)
				{
				  	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx + 2;
				}
				else
				{
				  	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx;
				}
				prBRefPicListInfo->ucFBufIdx1 = _ptRefPicList[u4InstID][prBRefPicListInfo->u4ListIdx1].u4FBufIdx[i];
				prBRefPicListInfo->u4FBufYStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4Addr;;
				prBRefPicListInfo->i4LongTermPicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4LongTermPicNum;
				prBRefPicListInfo->i4PicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPicNum;;
				prBRefPicListInfo->i4TFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4TFldPOC;
				prBRefPicListInfo->i4BFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPOC;
				prBRefPicListInfo->u4TFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4TFldPara;
				prBRefPicListInfo->u4BFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4BFldPara;
			#if VDEC_H264_REDUCE_MV_BUFF
				prBRefPicListInfo->u4FBufMvStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4MvStartAddr;
			#else
				//prBRefPicListInfo->u4DramPicSize1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize;
				//prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((prBRefPicListInfo->u4DramPicSize1 * 3) >>1));
				prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize * 3) >>1));
			#endif
				#ifdef Y_C_SEPERATE
				prBRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YStartAddr;
				prBRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CStartAddr;
				prBRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YLenOffset;
				prBRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CLenOffset;
				prBRefPicListInfo->u4YStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YStartAddr;
				prBRefPicListInfo->u4CStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CStartAddr;
				prBRefPicListInfo->u4YLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YLenOffset;
				prBRefPicListInfo->u4CLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CLenOffset;
				#endif
				prBRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				prBRefPicListInfo->ucFBufIdx1 += u4DpbBaseOffset;    
				if (VDEC_LAE0 == u4InstID)
				{
					fgIsDiff = bVDEC_HAL_H264_LAE_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
				}
				else
				{
					fgIsDiff = bVDEC_HAL_H264_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
				}
				//vVDecSetBRefPicListReg(BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx);
				if(u4BFldListIdx < 4) // Short-term only
				{
				  	prBRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4BFldListIdx+2].u4FBufIdx[i];
				  	prBRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				  	prBRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				  	prBRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPOC;
				  	prBRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4PicNum;    
				  	prBRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				  	prBRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				  	prBRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				  	prBRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPOC;
				  	//prBRefPicListInfo->u4DramPicSize = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize;
				  	prBRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4BFldPara;
				  	prBRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4Addr;
				#if VDEC_H264_REDUCE_MV_BUFF
					prBRefPicListInfo->u4FBufMvStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4MvStartAddr;
				#else
				  	prBRefPicListInfo->u4FBufMvStartAddr = (prBRefPicListInfo->u4FBufYStartAddr  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize * 3) >>1));
				#endif
				  	prBRefPicListInfo->u4FBufInfo = BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
				  	prBRefPicListInfo->u4ListIdx = u4BFldListIdx+2;
				  	prBRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4TFldPara;
				  	if(prBRefPicListInfo->u4ListIdx < 4)
				  	{
				    	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx + 2;
				  	}
				  	else
				  	{
				    	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx;
				  	}
					prBRefPicListInfo->ucFBufIdx1 = _ptRefPicList[u4InstID][prBRefPicListInfo->u4ListIdx1].u4FBufIdx[i];
					prBRefPicListInfo->u4FBufYStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4Addr;;
					prBRefPicListInfo->i4LongTermPicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4LongTermPicNum;
					prBRefPicListInfo->i4PicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPicNum;;
					prBRefPicListInfo->i4TFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4TFldPOC;
					prBRefPicListInfo->i4BFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPOC;
					prBRefPicListInfo->u4TFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4TFldPara;
					prBRefPicListInfo->u4BFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4BFldPara;
				#if VDEC_H264_REDUCE_MV_BUFF
				  	prBRefPicListInfo->u4FBufMvStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4MvStartAddr;
				#else
				  	//prBRefPicListInfo->u4DramPicSize1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize;
				  	//prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((prBRefPicListInfo->u4DramPicSize1 * 3) >>1));
				  	prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize * 3) >>1));
				#endif
					#ifdef Y_C_SEPERATE
					prBRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YStartAddr;
					prBRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CStartAddr;
					prBRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YLenOffset;
					prBRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CLenOffset;
					prBRefPicListInfo->u4YStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YStartAddr;
					prBRefPicListInfo->u4CStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CStartAddr;
					prBRefPicListInfo->u4YLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YLenOffset;
					prBRefPicListInfo->u4CLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CLenOffset;
					#endif
				  	prBRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				  	prBRefPicListInfo->ucFBufIdx1 += u4DpbBaseOffset;
					if (VDEC_LAE0 == u4InstID)
					{
						fgIsDiff = bVDEC_HAL_H264_LAE_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
					}
					else
					{
				  		fgIsDiff = bVDEC_HAL_H264_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
					}
				  	//vVDecSetBRefPicListReg(BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx+2);
				}

				if((!fgDiff[0]) && fgIsDiff)
				{
			  		fgDiff[0] = TRUE;
				}
				pu4RefIdx[0] ++;
				u4Cnt[1] ++;        
			}
		}
		else if(_tVerMpvDecPrm[u4InstID].ucPicStruct == BOTTOM_FIELD)
		{
			if(i < _ptRefPicList[u4InstID][u4BFldListIdx].u4RefPicCnt)
			{
				prBRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4BFldListIdx].u4FBufIdx[i];
				prBRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				prBRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				prBRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPOC;
				prBRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4PicNum;    
				prBRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				prBRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				prBRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				prBRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPOC;
				//prBRefPicListInfo->u4DramPicSize = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize;
				prBRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4BFldPara;
				prBRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4Addr;
			#if VDEC_H264_REDUCE_MV_BUFF
				prBRefPicListInfo->u4FBufMvStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4MvStartAddr;
			#else
				prBRefPicListInfo->u4FBufMvStartAddr = (prBRefPicListInfo->u4FBufYStartAddr  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize * 3) >>1));
			#endif
				prBRefPicListInfo->u4FBufInfo = BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
				prBRefPicListInfo->u4ListIdx = u4BFldListIdx;
				prBRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4TFldPara;
				if(prBRefPicListInfo->u4ListIdx < 4)
				{
				  	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx + 2;
				}
				else
				{
				  	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx;
				}
				prBRefPicListInfo->ucFBufIdx1 = _ptRefPicList[u4InstID][prBRefPicListInfo->u4ListIdx1].u4FBufIdx[i];
				prBRefPicListInfo->u4FBufYStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4Addr;;
				prBRefPicListInfo->i4LongTermPicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4LongTermPicNum;
				prBRefPicListInfo->i4PicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPicNum;;
				prBRefPicListInfo->i4TFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4TFldPOC;
				prBRefPicListInfo->i4BFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPOC;
				prBRefPicListInfo->u4TFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4TFldPara;
				prBRefPicListInfo->u4BFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4BFldPara;
			#if VDEC_H264_REDUCE_MV_BUFF
				prBRefPicListInfo->u4FBufMvStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4MvStartAddr;
			#else
				//prBRefPicListInfo->u4DramPicSize1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize;
				//prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((prBRefPicListInfo->u4DramPicSize1 * 3) >>1));
				prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize * 3) >>1));
			#endif
				#ifdef Y_C_SEPERATE
				prBRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YStartAddr;
				prBRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CStartAddr;
				prBRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YLenOffset;
				prBRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CLenOffset;
				prBRefPicListInfo->u4YStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YStartAddr;
				prBRefPicListInfo->u4CStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CStartAddr;
				prBRefPicListInfo->u4YLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YLenOffset;
				prBRefPicListInfo->u4CLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CLenOffset;
				#endif
				prBRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				prBRefPicListInfo->ucFBufIdx1 += u4DpbBaseOffset;    
				if (VDEC_LAE0 == u4InstID)
				{
					fgIsDiff = bVDEC_HAL_H264_LAE_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
				}
				else
				{
					fgIsDiff = bVDEC_HAL_H264_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
				}
				//vVDecSetBRefPicListReg(BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx);
				if(u4BFldListIdx < 4) // Short-term only
				{
				  	prBRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4BFldListIdx+2].u4FBufIdx[i];
				  	prBRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				  	prBRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				  	prBRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPOC;
				  	prBRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4PicNum;    
				  	prBRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				  	prBRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				  	prBRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				  	prBRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPOC;
				  	//prBRefPicListInfo->u4DramPicSize = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize;
				  	prBRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4BFldPara;
				  	prBRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4Addr;
				#if VDEC_H264_REDUCE_MV_BUFF
					prBRefPicListInfo->u4FBufMvStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4MvStartAddr;
				#else
				  	prBRefPicListInfo->u4FBufMvStartAddr = (prBRefPicListInfo->u4FBufYStartAddr  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize * 3) >>1));
				#endif
				  	prBRefPicListInfo->u4FBufInfo = BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
				  	prBRefPicListInfo->u4ListIdx = u4BFldListIdx+2;
				  	prBRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4TFldPara;
				  	if(prBRefPicListInfo->u4ListIdx < 4)
					{
					    prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx + 2;
					}
				  	else
				  	{
				    	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx;
				  	}
					prBRefPicListInfo->ucFBufIdx1 = _ptRefPicList[u4InstID][prBRefPicListInfo->u4ListIdx1].u4FBufIdx[i];
					prBRefPicListInfo->u4FBufYStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4Addr;;
					prBRefPicListInfo->i4LongTermPicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4LongTermPicNum;
					prBRefPicListInfo->i4PicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPicNum;;
					prBRefPicListInfo->i4TFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4TFldPOC;
					prBRefPicListInfo->i4BFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPOC;
					prBRefPicListInfo->u4TFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4TFldPara;
					prBRefPicListInfo->u4BFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4BFldPara;
				#if VDEC_H264_REDUCE_MV_BUFF
				  	prBRefPicListInfo->u4FBufMvStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4MvStartAddr;
				#else
				  //prBRefPicListInfo->u4DramPicSize1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize;
				  //prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((prBRefPicListInfo->u4DramPicSize1 * 3) >>1));
				  	prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize * 3) >>1));
				#endif
					#ifdef Y_C_SEPERATE
					prBRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YStartAddr;
					prBRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CStartAddr;
					prBRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YLenOffset;
					prBRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CLenOffset;
					prBRefPicListInfo->u4YStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YStartAddr;
					prBRefPicListInfo->u4CStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CStartAddr;
					prBRefPicListInfo->u4YLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YLenOffset;
					prBRefPicListInfo->u4CLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CLenOffset;
					#endif
				  	prBRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				  	prBRefPicListInfo->ucFBufIdx1 += u4DpbBaseOffset;
					if (VDEC_LAE0 == u4InstID)
					{
						fgIsDiff = bVDEC_HAL_H264_LAE_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
					}
					else
					{
				  		fgIsDiff = bVDEC_HAL_H264_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
					}
				  //vVDecSetBRefPicListReg(BOTTOM_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4BFldListIdx+2);
				}

				if((!fgDiff[0]) && fgIsDiff)
				{
			  		fgDiff[0] = TRUE;
				}
				pu4RefIdx[0] ++;
				u4Cnt[1] ++;        
			}      
			if(i < _ptRefPicList[u4InstID][u4TFldListIdx].u4RefPicCnt)
			{
				prBRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4TFldListIdx].u4FBufIdx[i];
				prBRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				prBRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				prBRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPOC;
				prBRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4PicNum;    
				prBRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				prBRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				prBRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				prBRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPOC;        
				//prBRefPicListInfo->u4DramPicSize = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize;
				prBRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4BFldPara;
				prBRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4Addr;
			#if VDEC_H264_REDUCE_MV_BUFF
				prBRefPicListInfo->u4FBufMvStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4MvStartAddr;
			#else
				prBRefPicListInfo->u4FBufMvStartAddr = (prBRefPicListInfo->u4FBufYStartAddr  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize * 3) >>1));
			#endif
				prBRefPicListInfo->u4FBufInfo = TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
				prBRefPicListInfo->u4ListIdx = u4TFldListIdx;
				prBRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4TFldPara;
				if(prBRefPicListInfo->u4ListIdx < 4)
				{
				  	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx + 2;
				}
				else
				{
				  	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx;
				}
				prBRefPicListInfo->ucFBufIdx1 = _ptRefPicList[u4InstID][prBRefPicListInfo->u4ListIdx1].u4FBufIdx[i];
				prBRefPicListInfo->u4FBufYStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4Addr;;
				prBRefPicListInfo->i4LongTermPicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4LongTermPicNum;
				prBRefPicListInfo->i4PicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPicNum;;
				prBRefPicListInfo->i4TFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4TFldPOC;
				prBRefPicListInfo->i4BFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPOC;
				prBRefPicListInfo->u4TFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4TFldPara;
				prBRefPicListInfo->u4BFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4BFldPara;
			#if VDEC_H264_REDUCE_MV_BUFF
				prBRefPicListInfo->u4FBufMvStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4MvStartAddr;
			#else
				//prBRefPicListInfo->u4DramPicSize1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize;
				//prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((prBRefPicListInfo->u4DramPicSize1 * 3) >>1));
				prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize * 3) >>1));
			#endif
				#ifdef Y_C_SEPERATE
				prBRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YStartAddr;
				prBRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CStartAddr;
				prBRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YLenOffset;
				prBRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CLenOffset;
				prBRefPicListInfo->u4YStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YStartAddr;
				prBRefPicListInfo->u4CStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CStartAddr;
				prBRefPicListInfo->u4YLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YLenOffset;
				prBRefPicListInfo->u4CLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CLenOffset;
				#endif
				prBRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				prBRefPicListInfo->ucFBufIdx1 += u4DpbBaseOffset;
				if (VDEC_LAE0 == u4InstID)
				{
					fgIsDiff = bVDEC_HAL_H264_LAE_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
				}
				else
				{
					fgIsDiff = bVDEC_HAL_H264_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
				}
				//vVDecSetBRefPicListReg(TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx);
				if(u4TFldListIdx < 4) // Short-term only
				{
				  	prBRefPicListInfo->ucFBufIdx = _ptRefPicList[u4InstID][u4TFldListIdx+2].u4FBufIdx[i];
				  	prBRefPicListInfo->i4BFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldLongTermPicNum;
				  	prBRefPicListInfo->i4BFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPicNum;
				  	prBRefPicListInfo->i4BFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4BFldPOC;
				  	prBRefPicListInfo->i4PicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4PicNum;    
				  	prBRefPicListInfo->i4LongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4LongTermPicNum;
				  	prBRefPicListInfo->i4TFldLongTermPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldLongTermPicNum;
				  	prBRefPicListInfo->i4TFldPicNum = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPicNum;
				  	prBRefPicListInfo->i4TFldPOC = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].i4TFldPOC;
				  	//prBRefPicListInfo->u4DramPicSize = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize;
				  	prBRefPicListInfo->u4BFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4BFldPara;
				  	prBRefPicListInfo->u4FBufYStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4Addr;
				#if VDEC_H264_REDUCE_MV_BUFF
					prBRefPicListInfo->u4FBufMvStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4MvStartAddr;
				#else
				  	prBRefPicListInfo->u4FBufMvStartAddr = (prBRefPicListInfo->u4FBufYStartAddr  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4DramPicSize * 3) >>1));
				#endif
				  	prBRefPicListInfo->u4FBufInfo = TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16);
				  	prBRefPicListInfo->u4ListIdx = u4TFldListIdx+2;
				  	prBRefPicListInfo->u4TFldPara = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4TFldPara;
				  	if(prBRefPicListInfo->u4ListIdx < 4)
				  	{
				    	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx + 2;
				  	}
				  	else
				  	{
				    	prBRefPicListInfo->u4ListIdx1 = prBRefPicListInfo->u4ListIdx;
				  	}
				  	prBRefPicListInfo->ucFBufIdx1 = _ptRefPicList[u4InstID][prBRefPicListInfo->u4ListIdx1].u4FBufIdx[i];
				  	prBRefPicListInfo->u4FBufYStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4Addr;;
				  	prBRefPicListInfo->i4LongTermPicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4LongTermPicNum;
				  	prBRefPicListInfo->i4PicNum1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPicNum;;
				  	prBRefPicListInfo->i4TFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4TFldPOC;
				  	prBRefPicListInfo->i4BFldPOC1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].i4BFldPOC;
				  	prBRefPicListInfo->u4TFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4TFldPara;
				  	prBRefPicListInfo->u4BFldPara1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4BFldPara;
				#if VDEC_H264_REDUCE_MV_BUFF
				 	prBRefPicListInfo->u4FBufMvStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4MvStartAddr;
				#else
				  	//prBRefPicListInfo->u4DramPicSize1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize;
				  	//prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((prBRefPicListInfo->u4DramPicSize1 * 3) >>1));
				  	prBRefPicListInfo->u4FBufMvStartAddr1 = (prBRefPicListInfo->u4FBufYStartAddr1  + ((_ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4DramPicSize * 3) >>1));
				#endif
					#ifdef Y_C_SEPERATE
					prBRefPicListInfo->u4YStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YStartAddr;
					prBRefPicListInfo->u4CStartAddr = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CStartAddr;
					prBRefPicListInfo->u4YLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4YLenOffset;
					prBRefPicListInfo->u4CLenOffset = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx].u4CLenOffset;
					prBRefPicListInfo->u4YStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YStartAddr;
					prBRefPicListInfo->u4CStartAddr1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CStartAddr;
					prBRefPicListInfo->u4YLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4YLenOffset;
					prBRefPicListInfo->u4CLenOffset1 = _ptFBufInfo[u4InstID][prBRefPicListInfo->ucFBufIdx1].u4CLenOffset;
					#endif
				  	prBRefPicListInfo->ucFBufIdx += u4DpbBaseOffset;
				  	prBRefPicListInfo->ucFBufIdx1 += u4DpbBaseOffset;
					if (VDEC_LAE0 == u4InstID)
					{
						fgIsDiff = bVDEC_HAL_H264_LAE_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
					}
					else
					{
				  		fgIsDiff = bVDEC_HAL_H264_SetBRefPicListReg(u4InstID, prBRefPicListInfo);
					}
				  	//vVDecSetBRefPicListReg(TOP_FIELD + (i << 8) + (pu4RefIdx[0] << 16), u4TFldListIdx+2);
				}
				if((!fgDiff[0]) && fgIsDiff)
				{
			  		fgDiff[0] = TRUE;
				}
				pu4RefIdx[0] ++;
				u4Cnt[0] ++;        
			}
		}
	} 
}


// *********************************************************************
// Function    : void vPartitionDPB(UINT32 u4InstID)
// Description : Set VDec related parameters
// Parameter   : None
// Return      : None
// *********************************************************************
void vPartitionDPB(UINT32 u4InstID)
{
	INT32 i, j;
	UINT32 u4DramPicSize;
	UINT32 u4DramPicArea;
	UINT32 u4YLen, u4CLen;
	
	if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIs16BitMode 
		|| _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsReorderMode
		|| _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsCompactMode)
	{
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFrames + 1;
	}
	else
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = DPB_SZ / ((_tVerMpvDecPrm[u4InstID].u4PicW * _tVerMpvDecPrm[u4InstID].u4PicH) * 7 / 4);
	UTIL_Printf("[VDEC_VER] MaxFBufNum = 0x%x\n", _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum);
	if(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFrames > _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum)
	{
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFrames;
		UTIL_Printf("==============================================================================\n");
		UTIL_Printf("[VDEC-H264-VFY] DPB Buffer smaller than file needed,should remalloc DPB buffer!\n");
		UTIL_Printf("[VDEC_H264_VFY] DPB Size need about 0x%x B\n",(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFrames + 1)*(_tVerMpvDecPrm[u4InstID].u4PicW * _tVerMpvDecPrm[u4InstID].u4PicH));
		UTIL_Printf("==============================================================================\n");
	}
#if (!defined(MVC_VFY_SUPPORT))
	if(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum > 17)
	{
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = 17;
	}
#else
	if(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum > 7)
	{
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = 7;
	}
	//for some special file,ref num may more than spec define. 
	if(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum < _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFrames)
	{
		UTIL_Printf("[VDEC-MVC-VFY]Re-Assigne MVC refnum\n");
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFrames + 1;
	}    
	if((_ucMVCType[u4InstID] == 1))
	{
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum ++;
	}  
#endif

#if defined(MVC_VFY_SUPPORT)
	// Real pic size w=16x, h=32x  
	u4DramPicSize = ((((_tVerMpvDecPrm[u4InstID].u4PicW + 15) >> 4) * ((_tVerMpvDecPrm[u4InstID].u4PicH + 31) >> 5)) << 9);
	// 1 pic area = Y + CbCr +MV
	u4DramPicArea = ((((u4DramPicSize * 7) >> 2) + 511) >> 9)<< 9;    
	
	if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum * u4DramPicArea > DPB_SZ)
	{
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		VDEC_ASSERT(0);
	}
#else
	//For DDR3
	// Real pic size w=64x, h=32x  
	u4DramPicSize = ((((_tVerMpvDecPrm[u4InstID].u4PicW + 15) >> 4) * ((_tVerMpvDecPrm[u4InstID].u4PicH + 31) >> 5)) << 9);
	//For Swap mode
	// 1 pic area = Y + CbCr +MV
	u4DramPicArea = ((((u4DramPicSize * 7) >> 2) + 4095) >> 12)<< 12;    

	if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIs16BitMode 
		|| _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsReorderMode)
	{
		u4DramPicSize = u4DramPicSize << 1;
	}
	if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsCompactMode)
	{
		u4DramPicSize = u4DramPicSize*5/4;
	}
	
	if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum * u4DramPicArea > DPB_SZ)
	{
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		UTIL_Printf("[VDEC_VER] H264 DPB Size Not Enough!!!!!\n");
		VDEC_ASSERT(0);
	}
#endif

	UTIL_Printf("[H264] Inst%d, u4DramPicSize:0x%x, u4DramPicArea:0x%x\n", u4InstID, u4DramPicSize, u4DramPicArea);
	u4YLen = (u4DramPicSize >> 8);
	u4CLen = (u4YLen >> 1);
	UTIL_Printf("[H264] Inst%d, u4YLen:0x%x, u4CLen:0x%x\n", u4InstID, u4YLen, u4CLen);
	for(i=0; i<_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
	{
		_ptFBufInfo[u4InstID][i].u4W = _tVerMpvDecPrm[u4InstID].u4PicW;
		_ptFBufInfo[u4InstID][i].u4H = _tVerMpvDecPrm[u4InstID].u4PicH;    
		_ptFBufInfo[u4InstID][i].u4DramPicSize = u4DramPicSize;
		_ptFBufInfo[u4InstID][i].u4DramPicArea = u4DramPicArea;    		
		_ptFBufInfo[u4InstID][i].u4Addr = ((UINT32)_pucDPB[u4InstID]) + (i * u4DramPicArea);
		
		//UTIL_Printf("[H264] Inst%d, buf %d use frame Buffer 0x%x, pa 0x%x\n", 
		//u4InstID, i, _ptFBufInfo[u4InstID][i].u4Addr,
		//PHYSICAL(_ptFBufInfo[u4InstID][i].u4Addr));

	#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)//14/9/2010 mtk40343
		if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum <= 8)
		{
	 		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsReduceMVBuffer = TRUE;
		}
	#endif

	#if VDEC_H264_REDUCE_MV_BUFF
	#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)//14/9/2010 mtk40343
		if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum <= 8)
	#else
		if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsReduceMVBuffer == 1)
	#endif
		{        
			if (u4InstID == 0 || u4InstID == 2)
			{
		  		_ptFBufInfo[u4InstID][i].u4MvStartAddr = (UINT32) _pucAVCMVBuff_Main[i];
			}	
	    	else
		    {
		     	_ptFBufInfo[u4InstID][i].u4MvStartAddr = (UINT32) _pucAVCMVBuff_Sub[i];         
		    }
			UTIL_Printf("[H264] Inst%d, buf %d use reduced B MV Buffer 0x%x\n", u4InstID, i, _ptFBufInfo[u4InstID][i].u4MvStartAddr);
		}
		else
	#endif
		 {
		 	#ifdef Y_C_SEPERATE
			_ptFBufInfo[u4InstID][i].u4MvStartAddr = (UINT32) _pucAVCMVBuff_Main[i];
			UTIL_Printf("[H264] Inst%d, buf %d use large B MV Buffer 0x%x\n", u4InstID, i, _ptFBufInfo[u4InstID][i].u4MvStartAddr);
			#else
		    _ptFBufInfo[u4InstID][i].u4MvStartAddr = _ptFBufInfo[u4InstID][i].u4Addr + ((u4DramPicSize * 3) >> 1);	    
		    UTIL_Printf("[H264] Inst%d, buf %d use large B MV Buffer 0x%x\n", u4InstID, i, _ptFBufInfo[u4InstID][i].u4MvStartAddr);
			#endif
		}		
	}
	#ifdef Y_C_SEPERATE
	j =  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum -1;
	_ptFBufInfo[u4InstID][0].u4YStartAddr =  (UINT32)_pucDPB[u4InstID];
	_ptFBufInfo[u4InstID][0].u4YLenStartAddr = _ptFBufInfo[u4InstID][0].u4YStartAddr + u4DramPicSize;
	_ptFBufInfo[u4InstID][0].u4YLenOffset = u4DramPicSize;
	_ptFBufInfo[u4InstID][0].u4YLen = u4YLen;
	UTIL_Printf("[H264] Inst%d, buf %d use Ystart 0x%x, pa 0x%x, YLenStart 0x%x, YLenOffset 0x%x\n", 
			u4InstID, 0, _ptFBufInfo[u4InstID][0].u4YStartAddr,
			PHYSICAL(_ptFBufInfo[u4InstID][0].u4YStartAddr),
			_ptFBufInfo[u4InstID][0].u4YLenStartAddr,
			_ptFBufInfo[u4InstID][0].u4YLenOffset);
	for (i = 1; i < _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
	{
		_ptFBufInfo[u4InstID][i].u4YStartAddr = ((((_ptFBufInfo[u4InstID][i-1].u4YLenStartAddr + u4YLen) + 8191) >> 13) << 13);
		_ptFBufInfo[u4InstID][i].u4YLenStartAddr = _ptFBufInfo[u4InstID][i].u4YStartAddr + u4DramPicSize;
		_ptFBufInfo[u4InstID][i].u4YLenOffset = u4DramPicSize;
		_ptFBufInfo[u4InstID][i].u4YLen = u4YLen;
		UTIL_Printf("[H264] Inst%d, buf %d use Ystart 0x%x, pa 0x%x, YLenStart 0x%x, YLenOffset 0x%x\n", 
			u4InstID, i, _ptFBufInfo[u4InstID][i].u4YStartAddr,
			PHYSICAL(_ptFBufInfo[u4InstID][i].u4YStartAddr),
			_ptFBufInfo[u4InstID][i].u4YLenStartAddr,
			_ptFBufInfo[u4InstID][i].u4YLenOffset);
	}
	_ptFBufInfo[u4InstID][0].u4CStartAddr = ((((_ptFBufInfo[u4InstID][j].u4YLenStartAddr + u4YLen) + 8191) >> 13) << 13);
	_ptFBufInfo[u4InstID][0].u4CLenStartAddr = _ptFBufInfo[u4InstID][0].u4CStartAddr + (u4DramPicSize >> 1);
	_ptFBufInfo[u4InstID][0].u4CLenOffset = (u4DramPicSize >> 1);
	_ptFBufInfo[u4InstID][0].u4CLen = u4CLen;
	UTIL_Printf("[H264] Inst%d, buf %d use Cstart 0x%x, pa 0x%x, CLenStart 0x%x, CLenOffset 0x%x\n", 
			u4InstID, 0, _ptFBufInfo[u4InstID][0].u4CStartAddr,
			PHYSICAL(_ptFBufInfo[u4InstID][0].u4CStartAddr),
			_ptFBufInfo[u4InstID][0].u4CLenStartAddr,
			_ptFBufInfo[u4InstID][0].u4CLenOffset);
	for (i = 1; i < _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
	{
		_ptFBufInfo[u4InstID][i].u4CStartAddr = ((((_ptFBufInfo[u4InstID][i-1].u4CLenStartAddr + u4CLen) + 8191) >> 13) << 13);
		_ptFBufInfo[u4InstID][i].u4CLenStartAddr = _ptFBufInfo[u4InstID][i].u4CStartAddr + (u4DramPicSize >> 1);
		_ptFBufInfo[u4InstID][i].u4CLenOffset = (u4DramPicSize >> 1);
		_ptFBufInfo[u4InstID][0].u4CLen = u4CLen;
		UTIL_Printf("[H264] Inst%d, buf %d use Cstart 0x%x, pa 0x%x, CLenStart 0x%x, CLenOffset 0x%x\n", 
			u4InstID, i, _ptFBufInfo[u4InstID][i].u4CStartAddr,
			PHYSICAL(_ptFBufInfo[u4InstID][i].u4CStartAddr),
			_ptFBufInfo[u4InstID][i].u4CLenStartAddr,
			_ptFBufInfo[u4InstID][i].u4CLenOffset);
	}
	#endif
	// current reset to 0 when DPB partition.
	_ptCurrFBufInfo[u4InstID] = &_ptFBufInfo[u4InstID][0];
}


// *********************************************************************
// Function    : void   vFillFrameNumGap(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm)
// Description : add the frame num for the gap
// Parameter   : None
// Return      : None
// *********************************************************************
void vFillFrameNumGap(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm)
{
	UINT32 u4CurrFrameNum;
	UINT32 u4UnusedShortTermFrameNum;
	char strMessage[256];
	INT32 tmp1 = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[0];
	INT32 tmp2 = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[1];
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[0] = 0;
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[1] = 0;

	u4UnusedShortTermFrameNum = (tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum + 1) % tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4MaxFrameNum;
	u4CurrFrameNum = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum;

	if((tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum != tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum) 
	&& (u4CurrFrameNum != u4UnusedShortTermFrameNum))
	{
		sprintf(strMessage, "%s", "!!!!  Fill frame num gap works  !!!!!\n");
		fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tRecFileInfo[u4InstID]);
		//fprintf(_tRecFileInfo.fpFile, "!!!!  Fill frame num gap works  !!!!!\n");
		while (u4CurrFrameNum != u4UnusedShortTermFrameNum)
		{
			// Create a new frame pic
			vAllocateFBuf(u4InstID, tVerMpvDecPrm, FALSE);
			_ptCurrFBufInfo[u4InstID]->ucFBufStatus = FRAME;
			_ptCurrFBufInfo[u4InstID]->i4PicNum = u4UnusedShortTermFrameNum;
			_ptCurrFBufInfo[u4InstID]->u4FrameNum = u4UnusedShortTermFrameNum;
			_ptCurrFBufInfo[u4InstID]->fgNonExisting = TRUE;
			tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgAdaptiveRefPicMarkingModeFlag = 0;
			if (tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicOrderCntType == 0)
			{
				_ptCurrFBufInfo[u4InstID]->i4POC = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastPOC;      
				_ptCurrFBufInfo[u4InstID]->i4TFldPOC = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastTFldPOC;
				_ptCurrFBufInfo[u4InstID]->i4BFldPOC = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastBFldPOC;
			}
			else
			{
				vVDecSetCurrPOC(u4InstID);
			}
			// Check if out of the Ref Frames
			vVerifySlidingWindowProce(u4InstID);
			_ptCurrFBufInfo[u4InstID]->ucFBufRefType = SREF_PIC;
			_ptCurrFBufInfo[u4InstID]->ucTFldRefType = SREF_PIC;
			_ptCurrFBufInfo[u4InstID]->ucBFldRefType = SREF_PIC;        
			tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum = u4UnusedShortTermFrameNum;
			u4UnusedShortTermFrameNum = (u4UnusedShortTermFrameNum + 1) % tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4MaxFrameNum;
		}    
	}
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[0] = tmp1;
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[1] = tmp2;
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum = u4CurrFrameNum;
}


// *********************************************************************
// Function    : void vAllocateFBuf(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm, BOOL fgFillCurrFBuf)
// Description : Allocate decoding frm buff in DPB
// Parameter   : 
// Return      : None
// *********************************************************************
void vAllocateFBuf(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm, BOOL fgFillCurrFBuf)
{
	INT32 i;
	INT32 iMinPOC;
	UINT32 u4MinPOCFBufIdx = 0;

	// Check if DPB full
	iMinPOC = 0x7fffffff;
	for(i=0; i<_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
	{
		if(_ptFBufInfo[u4InstID][i].ucFBufStatus == NO_PIC)
		{
			iMinPOC = 0x7fffffff;
			u4MinPOCFBufIdx = i;        
			break;
		}
		// miew: need to take care of field empty
		else if((iMinPOC > _ptFBufInfo[u4InstID][i].i4POC) && fgIsNonRefFBuf(u4InstID, i))
		{
			iMinPOC = _ptFBufInfo[u4InstID][i].i4POC;
			u4MinPOCFBufIdx = i;
		}
	}  
	// No empty DPB, 1 FBuf output
	if(_ptFBufInfo[u4InstID][u4MinPOCFBufIdx].ucFBufStatus != NO_PIC)
	{
		vVerifyClrFBufInfo(u4InstID, u4MinPOCFBufIdx);
	}
	_tVerMpvDecPrm[u4InstID].ucDecFBufIdx = u4MinPOCFBufIdx;
	// Only new alloc needs to update current fbuf idx
	vSetCurrFBufIdx(u4InstID, _tVerMpvDecPrm[u4InstID].ucDecFBufIdx);
	if(fgFillCurrFBuf)
	{
		//vFilledFBuf(_u4VDecID, _pucDecWorkBuf, _ptCurrFBufInfo->u4DramPicSize);
#ifdef DOWN_SCALE_SUPPORT
		//vFilledFBuf(_u4VDecID, _pucVDSCLBuf, _ptCurrFBufInfo->u4DramPicSize);
#endif
	}
}


// *********************************************************************
// Function    : BOOL fgChkRefInfo(UINT32 u4InstID, UINT32 u4FBufIdx, UINT32 u4RefType)
// Description : Check if reference picture should be insered to ref pic list
// Parameter   : None
// Return      : None
// *********************************************************************
BOOL fgChkRefInfo(UINT32 u4InstID, UINT32 u4FBufIdx, UINT32 u4RefType)
{
	if(fgIsFrmPic(u4InstID))
	{
		// According to spec 8.2.4.2.1 
		// NOTE: A non-pared reference fiedl is not used for inter prediction for decoding a frame.
		if((_ptFBufInfo[u4InstID][u4FBufIdx].ucTFldRefType == u4RefType) && (_ptFBufInfo[u4InstID][u4FBufIdx].ucBFldRefType == u4RefType))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		if((_ptFBufInfo[u4InstID][u4FBufIdx].ucTFldRefType == u4RefType) || (_ptFBufInfo[u4InstID][u4FBufIdx].ucBFldRefType == u4RefType))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}    
	}
}


// *********************************************************************
// Function    : void vInsertRefPicList(UINT32 u4InstID, VDEC_INFO_H264_REF_PIC_LIST_T *ptRefPicList, INT32 iCurrPOC, UINT32 u4RefPicListInfo)
// Description : Instert Short ref pic list item
// Parameter   : None
// Return      : None
// *********************************************************************
void vInsertRefPicList(UINT32 u4InstID, VDEC_INFO_H264_REF_PIC_LIST_T *ptRefPicList, INT32 iCurrPOC, UINT32 u4RefPicListInfo)
{
	INT32 j;
	UCHAR ucRefType; // 1-> Short 2-> Long
	UCHAR bListType; // 0-> P, 1-> B_0, 2->B_1
	UCHAR ucFBufIdx;
	INT32 iComp0 = 0;
	INT32 iComp1 = 0;
	UINT32 u4Temp;
	BOOL fgSwitch;

	ucRefType = u4RefPicListInfo & 0xf;   
	// 0:P_T, 1:P_B, 2:B0_T, 3:B0_B, 4:B1_T, 5:B1_B, 6:P_T_L, 7:P_B_L, 8:B_T_L, 9:B_B_L,  
	bListType = (u4RefPicListInfo >> 8) & 0xf;  
	ucFBufIdx = (u4RefPicListInfo >> 16) & 0xff;  

	if(ucRefType == SREF_PIC)
	{
		// 1st: Insert the current to the last idx
		ptRefPicList->u4FBufIdx[ptRefPicList->u4RefPicCnt] = ucFBufIdx;
		// 2nd: shift Shortterm ref pic        
		for(j=ptRefPicList->u4RefPicCnt - 1; j>=0; j--)
		{
			fgSwitch = FALSE;
			if(bListType == 0)
			{
				if(_ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].i4TFldPicNum > _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].i4TFldPicNum)
				{
				  	fgSwitch = TRUE;
				}
			}
			else if(bListType == 1)
			{
				if(_ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].i4BFldPicNum > _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].i4BFldPicNum)
				{
				  	fgSwitch = TRUE;
				}
			}
			else if((bListType == 2) || (bListType == 3))
			{
				iComp0 = (bListType == 2)? _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].i4TFldPOC : _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].i4BFldPOC;
				iComp1 = (bListType == 2)? _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].i4TFldPOC : _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].i4BFldPOC;
				if((fgIsFrmPic(u4InstID)? (((iComp0 < iCurrPOC) &&
			                             (iComp1 < iCurrPOC) &&
			                             (iComp1 > iComp0))
			                             ||
			                             ((iComp0 >= iCurrPOC) &&
			                              (iComp1 < iComp0))
			                             ||
			                             ((iComp0 >= iCurrPOC) &&
			                            (iComp1 < iCurrPOC))) 
			                            :
			                            (((iComp0 <= iCurrPOC) &&
			                             (iComp1 <= iCurrPOC) &&
			                             (iComp1 > iComp0))
			                             ||
			                              ((iComp0 > iCurrPOC) &&
			                              (iComp1 < iComp0))
			                             ||
			                             ((iComp0 > iCurrPOC) &&
			                             (iComp1 <= iCurrPOC)))) )
				{
			  		fgSwitch = TRUE;
				}
			}
			else if((bListType == 4) || (bListType == 5))
			{
				iComp0 = (bListType == 4)? _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].i4TFldPOC : _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].i4BFldPOC;
				iComp1 = (bListType == 4)? _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].i4TFldPOC : _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].i4BFldPOC;
				if(((iComp0 <= iCurrPOC) &&
			     	(iComp1 > iComp0))
			   		||
			   		((iComp0 > iCurrPOC) &&          
			     	(iComp1 > iCurrPOC) && 
			     	(iComp1 < iComp0))
			   		||
			   		((iComp0 <= iCurrPOC) &&
			    	(iComp1 > iCurrPOC)))            
				{
			  		fgSwitch = TRUE;
				}
			}
			if(fgSwitch)
			{
				u4Temp = ptRefPicList->u4FBufIdx[j+1];
				ptRefPicList->u4FBufIdx[j+1] = ptRefPicList->u4FBufIdx[j];
				ptRefPicList->u4FBufIdx[j] = u4Temp;
			}
		}
		ptRefPicList->u4RefPicCnt ++;  
	}
	else if(ucRefType == LREF_PIC)
	{
		ptRefPicList->u4FBufIdx[ptRefPicList->u4RefPicCnt] = ucFBufIdx;        
		for(j=(INT32)(ptRefPicList->u4RefPicCnt - 1); j>=0; j--)
		{
			if(bListType == 6)
			{
				iComp0 = _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].i4TFldLongTermPicNum;
				iComp1 = _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].i4TFldLongTermPicNum;
			}
			else if(bListType == 7)
			{
				iComp0 = _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].i4BFldLongTermPicNum;
				iComp1 = _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].i4BFldLongTermPicNum;
			}
			else if(bListType == 8)
			{
				iComp0 = _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].u4TFldLongTermFrameIdx;
				iComp1 = _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].u4TFldLongTermFrameIdx;
			}
			else if(bListType == 9)
			{
				iComp0 = _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j]].u4BFldLongTermFrameIdx;
				iComp1 = _ptFBufInfo[u4InstID][ptRefPicList->u4FBufIdx[j+1]].u4BFldLongTermFrameIdx;
			}            
			if(iComp1 < iComp0)
			{
				u4Temp = ptRefPicList->u4FBufIdx[j+1];
				ptRefPicList->u4FBufIdx[j+1] = ptRefPicList->u4FBufIdx[j];
				ptRefPicList->u4FBufIdx[j] = u4Temp;
			}
		}
		ptRefPicList->u4RefPicCnt ++;
	}
}


// *********************************************************************
// Function    : void vVDecSetCurrPOC(UINT32 u4InstID)
// Description : Set POC : Picture order count for display order
// Parameter   : None
// Return      : None
// Note  :JM decode_POC
// *********************************************************************
void vVDecSetCurrPOC(UINT32 u4InstID)
{
	INT32 iPrevPOCMsb;
	INT32 iPrevPOCLsb;    
	INT32 iMaxPicOrderCntLsb;
	INT32 iPrevFrameNumOffset;
	INT32 iAbsFrameNum;
	INT32 iPicOrderCntCycleCnt = 0;
	INT32 iFrameNumInPicOrderCntCycle = 0;
	INT32 iExpectedDeltaPerPicOrderCnt = 0;
	INT32 iExpectedDeltaPerPicOrderCntCycle;
	INT32 i;
	INT32 iTemp;

	VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm;

	tVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];
	iMaxPicOrderCntLsb = 1 << (tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4Log2MaxPicOrderCntLsbMinus4 + 4);

	switch(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicOrderCntType)
	{
		case 0:
			if(fgIsIDRPic(u4InstID))
			{
				iPrevPOCMsb = 0;
				iPrevPOCLsb = 0;        
			}
			else
			{
				if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.fgLastMmco5)
				{
				  	if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.ucLastPicStruct != BOTTOM_FIELD)
				  	{
				    	iPrevPOCMsb = 0;
				    	iPrevPOCLsb = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastRefTFldPOC;        
				  	}
				  	else
				  	{
				    	iPrevPOCMsb = 0;
				    	iPrevPOCLsb = 0;        
				  	}
				}
				else
				{
				    iPrevPOCMsb = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastRefPOCMsb;        
				    iPrevPOCLsb = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastRefPOCLsb;        
				}
			}
	// Calculate POCMsb
			if((tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntLsb <  iPrevPOCLsb) && 
 				((iPrevPOCLsb - tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntLsb) >= (iMaxPicOrderCntLsb >> 1)))
			{
				tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntMsb = iPrevPOCMsb + iMaxPicOrderCntLsb;
			}
			else if((tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntLsb >  iPrevPOCLsb) && 
 					((tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntLsb - iPrevPOCLsb) > (iMaxPicOrderCntLsb >> 1)))
			{
				tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntMsb = iPrevPOCMsb - iMaxPicOrderCntLsb;
			}
			else
			{
				tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntMsb = iPrevPOCMsb;
			}
			if((!tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgFieldPicFlag) || (!tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgBottomFieldFlag))
			{
				_ptCurrFBufInfo[u4InstID]->i4TFldPOC = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntMsb + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntLsb;
			}
			if((!tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgFieldPicFlag))
			{
				_ptCurrFBufInfo[u4InstID]->i4BFldPOC = _ptCurrFBufInfo[u4InstID]->i4TFldPOC + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCntBottom;
			}
			else if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgBottomFieldFlag)
			{
				_ptCurrFBufInfo[u4InstID]->i4BFldPOC = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntMsb + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntLsb;
			}
			break;
		case 1:
			if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.fgLastMmco5)
			{
				iPrevFrameNumOffset = 0;
			}
			else
			{
				iPrevFrameNumOffset = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastFrameNumOffset;
			}
			if(fgIsIDRPic(u4InstID))
			{
				tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset = 0;
			}
			else if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum > tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum)
			{
				tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset = iPrevFrameNumOffset + (INT32)tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4MaxFrameNum;
			}
			else
			{
				tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset = iPrevFrameNumOffset;
			}
			if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFramesInPicOrderCntCycle != 0)
			{
				iAbsFrameNum = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset + (INT32)tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum;
			}
			else
			{
				iAbsFrameNum = 0; 
			}
			if(_u4NalRefIdc[u4InstID] == 0 && iAbsFrameNum > 0)
			{
				iAbsFrameNum --;
			}
			if(iAbsFrameNum > 0)
			{
				iPicOrderCntCycleCnt = (iAbsFrameNum - 1)/tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFramesInPicOrderCntCycle;
				iFrameNumInPicOrderCntCycle = (iAbsFrameNum - 1)%tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFramesInPicOrderCntCycle;        
			}
			iExpectedDeltaPerPicOrderCntCycle = 0;
			for(i=0; i<tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFramesInPicOrderCntCycle; i++)
			{
				iExpectedDeltaPerPicOrderCntCycle += tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->i4OffsetForRefFrame[i];
			}
			if(iAbsFrameNum > 0)
			{
				iExpectedDeltaPerPicOrderCnt = (INT32)iPicOrderCntCycleCnt * iExpectedDeltaPerPicOrderCntCycle;
				for(i=0; i<=iFrameNumInPicOrderCntCycle; i++)
				{
				  	iExpectedDeltaPerPicOrderCnt = iExpectedDeltaPerPicOrderCnt + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->i4OffsetForRefFrame[i];
				}
			}
			else
			{
				iExpectedDeltaPerPicOrderCnt = 0;
			}
			if(_u4NalRefIdc[u4InstID] == 0)
			{
				iExpectedDeltaPerPicOrderCnt = iExpectedDeltaPerPicOrderCnt + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->i4OffsetForNonRefPic;
			}
			if(!tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgFieldPicFlag)
			{
				_ptCurrFBufInfo[u4InstID]->i4TFldPOC = iExpectedDeltaPerPicOrderCnt + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[0];
				iTemp = _ptCurrFBufInfo[u4InstID]->i4TFldPOC + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->i4OffsetForTopToBottomField;
				_ptCurrFBufInfo[u4InstID]->i4BFldPOC = iTemp + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[1];
				_ptCurrFBufInfo[u4InstID]->i4BFldPOC = _ptCurrFBufInfo[u4InstID]->i4TFldPOC + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->i4OffsetForTopToBottomField + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[1];
			}
			else if(!tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgBottomFieldFlag)
			{
				_ptCurrFBufInfo[u4InstID]->i4TFldPOC = iExpectedDeltaPerPicOrderCnt + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[0];
			}
			else
			{
				_ptCurrFBufInfo[u4InstID]->i4BFldPOC = iExpectedDeltaPerPicOrderCnt + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->i4OffsetForTopToBottomField + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[0];      
			}
			break;
		case 2:
			if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.fgLastMmco5)
			{
				iPrevFrameNumOffset = 0;
			}
			else
			{
				iPrevFrameNumOffset = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastFrameNumOffset;
			}      
			if(fgIsIDRPic(u4InstID))
			{
				tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset = 0;
			}
			else if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum > tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum)
			{
				tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset = iPrevFrameNumOffset + (INT32)tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4MaxFrameNum;
			}
			else
			{
				tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset = iPrevFrameNumOffset;
			}
			if(fgIsIDRPic(u4InstID))
			{
				// Use iAbsFrameNum as tempPicOrderCnt
				iAbsFrameNum = 0;
			}
			else if(_u4NalRefIdc[u4InstID] == 0)
			{
				iAbsFrameNum = ((tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum)  << 1) + 1;
			}
			else
			{
				iAbsFrameNum = ((tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset + tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum)  << 1);        
			}
			if(!tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgFieldPicFlag)
			{
				_ptCurrFBufInfo[u4InstID]->i4TFldPOC = iAbsFrameNum;
				_ptCurrFBufInfo[u4InstID]->i4BFldPOC = iAbsFrameNum;
			}
			else if(!tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgBottomFieldFlag)
			{
				_ptCurrFBufInfo[u4InstID]->i4TFldPOC = iAbsFrameNum;
			}
			else
			{
				_ptCurrFBufInfo[u4InstID]->i4BFldPOC = iAbsFrameNum;
			}     
			break;
		default:
			break;
	}
	_ptCurrFBufInfo[u4InstID]->i4POC = (_ptCurrFBufInfo[u4InstID]->i4TFldPOC < _ptCurrFBufInfo[u4InstID]->i4BFldPOC)? _ptCurrFBufInfo[u4InstID]->i4TFldPOC : _ptCurrFBufInfo[u4InstID]->i4BFldPOC;
}


// *********************************************************************
// Function    : void vVerifySlidingWindowProce(UINT32 u4InstID)
// Description : marking the decoded ref pic with sliding window method
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifySlidingWindowProce(UINT32 u4InstID)
{
	INT32 i;
	INT32 iMinFrameNumWrap;
	INT32 i4FrameNumWrap;
	UINT32 u4NumShortTerm;
	UINT32 u4NumLongTerm;
	UINT32 u4MinFBufIdx = 0;

	// If the curr pic is the 2nd field, follow the 1st field's ref info
	if((_ptCurrFBufInfo[u4InstID]->ucFBufStatus == FRAME) && (_tVerMpvDecPrm[u4InstID].ucPicStruct == BOTTOM_FIELD) && (_ptCurrFBufInfo[u4InstID]->ucTFldRefType == SREF_PIC))
	{
		_ptCurrFBufInfo[u4InstID]->ucBFldRefType = SREF_PIC;
	}
	else if((_ptCurrFBufInfo[u4InstID]->ucFBufStatus == FRAME) && (_tVerMpvDecPrm[u4InstID].ucPicStruct == TOP_FIELD) && (_ptCurrFBufInfo[u4InstID]->ucBFldRefType == SREF_PIC))
	{
		_ptCurrFBufInfo[u4InstID]->ucTFldRefType = SREF_PIC;    
	}
	else
	{
		i = 0;
		iMinFrameNumWrap = 0xfffffff;
		u4NumShortTerm = 0;
		u4NumLongTerm = 0;  
		// Remove 1 SREF pic for a new ref pic
		for(i=0; i<_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
		{
			if((_ptFBufInfo[u4InstID][i].ucTFldRefType == SREF_PIC) || (_ptFBufInfo[u4InstID][i].ucBFldRefType == SREF_PIC))
			{
				i4FrameNumWrap = (_ptFBufInfo[u4InstID][i].u4FrameNum > _ptCurrFBufInfo[u4InstID]->u4FrameNum)? (_ptFBufInfo[u4InstID][i].u4FrameNum -_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4MaxFrameNum) : _ptFBufInfo[u4InstID][i].u4FrameNum;
				if(iMinFrameNumWrap > i4FrameNumWrap)
				{
				  	iMinFrameNumWrap =  i4FrameNumWrap;
				  	u4MinFBufIdx = i;        
				}
				u4NumShortTerm ++;     
			}
			if((_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC) || (_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC))
			{
				u4NumLongTerm ++;
			}
		}
		// Since current pic should be ref pic, the condition should be modified as "larger" only
		// but the current one not set as ref pic at this time,
		if((u4NumShortTerm + u4NumLongTerm) >= ((_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFrames > 0)? _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->u4NumRefFrames : 1))
		{
			// Remove the smallet FrameNumWrap item
			_ptFBufInfo[u4InstID][u4MinFBufIdx].ucFBufRefType = NREF_PIC;
			_ptFBufInfo[u4InstID][u4MinFBufIdx].ucTFldRefType = NREF_PIC;
			_ptFBufInfo[u4InstID][u4MinFBufIdx].ucBFldRefType = NREF_PIC;    
		}
	}
}


// *********************************************************************
// Function    : void vSetCurrFBufIdx(UINT32 u4InstID, UINT32 u4DecFBufIdx)
// Description : Set Curr FBuf index
// Parameter   : None
// Return      : None
// *********************************************************************
void vSetCurrFBufIdx(UINT32 u4InstID, UINT32 u4DecFBufIdx)
{
	_ptCurrFBufInfo[u4InstID] = &_ptFBufInfo[u4InstID][u4DecFBufIdx];  
	_pucDecWorkBuf[u4InstID] = (UCHAR *)(_ptCurrFBufInfo[u4InstID]->u4Addr);
}

//BOOL fgFirstInit = TRUE;
// *********************************************************************
// Function    : void vSearchRealPic(UINT32 u4InstID)
// Description : Search for the real pic then to dec
// Parameter   : None
// Return      : None
// *********************************************************************
void vSearchRealPic(UINT32 u4InstID)
{
	UINT32 u4Temp;
	BOOL fgForbidenZeroBits;
	UINT32 u4Bits;
	VDEC_INFO_H264_BS_INIT_PRM_T rH264BSInitPrm;
	
	do
	{
		//_u4CurrPicStartAddr[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
		if (1 == _u4DualCoreEnable[u4InstID])
		{
			rH264BSInitPrm.fgDualCoreEn = 1;
		}
	#ifdef BARREL2_THREAD_SUPPORT
		VERIFY (x_sema_lock(_ahVDecEndSema[u4InstID], X_SEMA_OPTION_WAIT) == OSR_OK);
	#endif
		if (_u4CurrPicStartAddr[1] != 0)
		{
	        printk("[INFO] update Current rptr here !\n");
	        _u4CurrPicStartAddr[u4InstID]  = _u4CurrPicStartAddr[1];
	        _u4CurrPicStartAddr[1] = 0;  //reset for looping until real picture
    	} 
		else 
		{
        	_u4CurrPicStartAddr[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
    	}
		rH264BSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
		rH264BSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
		rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID] + _u4CurrPicStartAddr[u4InstID];
	#ifndef  RING_VFIFO_SUPPORT
		rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
	#else
		//	rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
		rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
	#endif
		rH264BSInitPrm.u4PredSa = /*PHYSICAL*/((UINT32)_pucPredSa[u4InstID]);
		i4VDEC_HAL_H264_InitVDecHW(u4InstID);
		i4VDEC_HAL_H264_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rH264BSInitPrm);

	#ifdef BARREL2_THREAD_SUPPORT
		VERIFY (x_sema_unlock(_ahVDecEndSema[u4InstID]) == OSR_OK);
	#endif
	//why shift those bit after init bs ???????????????
	//make non-align before search start code.marked by mtk40343 6/19/2012
	//u4Temp = u4VDEC_HAL_H264_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, u4Bits);
	#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
		u4Temp = u4VDEC_HAL_H264_GetStartCode(_u4BSID[u4InstID], u4InstID) & 0xff;
	#else
	#ifdef MVC_DEBUG
		UINT32 u4Test;
		UINT32 _u4CurrPicStartAddr1[2];
		u4Test = u4VDEC_HAL_H264_GetStartCode_8530(_u4BSID[u4InstID], u4InstID) & 0xff;        
		u4Temp = u4Test & 0xff;
	#ifdef MVC_DEBUG//add for debug read pointer,compare with bsinfo.txt
		_u4CurrPicStartAddr1[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
		UTIL_Printf("InstID (%d) NalType (%d) offset (%d,%d) Dram (0x%x) u4Test(0x%x)\n",u4InstID,(u4Temp & 0xff),_u4CurrPicStartAddr[u4InstID],_u4CurrPicStartAddr1[u4InstID],*(UINT32 *)(_u4CurrPicStartAddr1[u4InstID] + (UINT32)_pucVFifo[u4InstID] - 4),u4Test);
	#endif
	#else
		u4Temp = u4VDEC_HAL_H264_GetStartCode_8530(_u4BSID[u4InstID], u4InstID) & 0xff;
	#endif
	#endif

		fgForbidenZeroBits = ((u4Temp >> 7) & 0x01); // bit 31
		if(fgForbidenZeroBits != 0)
		{
			vErrInfo(VER_FORBIDEN_ERR);
		}
		_u4NalRefIdc[u4InstID] = ((u4Temp >> 5) & 0x3); // bit 30,29
		_ucNalUnitType[u4InstID] = (u4Temp & 0x1f); // bit 28,27,26,25,24

	#if defined(MVC_VFY_SUPPORT)
		if((_ucMVCType[u4InstID] == 1) && (_ucNalUnitType[u4InstID] == MVC_PREFIX_NAL))
		{
		 	vPrefix_Nal_Unit_Rbsp_Verify(u4InstID);
		}
		else if((_ucMVCType[u4InstID] == 2) && (_ucNalUnitType[u4InstID] == SLICE_EXT))
		{
		 	vPrefix_Nal_Unit_Rbsp_Verify(u4InstID);
	#if MVC_PATCH_1         
		 	_ucNalUnitType[u4InstID] = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgIdrFlag? NON_IDR_SLICE : IDR_SLICE;
	#else             
		 	_ucNalUnitType[u4InstID] = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgNonIdrFlag? NON_IDR_SLICE : IDR_SLICE;
	#endif
		}
		else if((_ucMVCType[u4InstID] == 2) && ((_ucNalUnitType[u4InstID] == NON_IDR_SLICE) || (_ucNalUnitType[u4InstID] == IDR_SLICE)))
		{
			_ucNalUnitType[u4InstID] = 0xff;
		}
	#endif    
		u4Temp = u4VDEC_HAL_H264_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);

	#ifdef DEBUG_LOG
		UTIL_Printf("=============_ucNalUnitType %d=========\n", _ucNalUnitType[u4InstID]);
	#endif
		if (1 == _u4DualCoreEnable[u4InstID])
		{		
		#ifdef VDEC_SIM_DUMP
			UTIL_Printf("[H264]Init Core1\n");
		#endif
			i4VDEC_HAL_H264_InitVDecHW(VDEC_VLD1);
			i4VDEC_HAL_H264_InitBarrelShifter(_u4BSID[u4InstID], VDEC_VLD1, &rH264BSInitPrm);
			u4Temp = u4VDEC_HAL_H264_GetStartCode_8530(_u4BSID[u4InstID], VDEC_VLD1) & 0xff;
			fgForbidenZeroBits = ((u4Temp >> 7) & 0x01); // bit 31
			if(fgForbidenZeroBits != 0)
			{
			  	vErrInfo(VER_FORBIDEN_ERR);
			}
			_u4NalRefIdc[1] = ((u4Temp >> 5) & 0x3); // bit 30,29
			_ucNalUnitType[1] = (u4Temp & 0x1f); // bit 28,27,26,25,24
			u4Temp = u4VDEC_HAL_H264_ShiftGetBitStream(_u4BSID[u4InstID], VDEC_VLD1, 0);
		#ifdef VDEC_SIM_DUMP
			UTIL_Printf("[H264]Init Core1 Done\n");
		#endif
		}
		switch(_ucNalUnitType[u4InstID])
		{
			case NON_IDR_SLICE:
			case IDR_SLICE:
	    		if(_u4BSID[u4InstID] != 0)
			    {
					_u4CurrPicStartAddr[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
					rH264BSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
					rH264BSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
					rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID] + _u4CurrPicStartAddr[u4InstID];
			    #ifndef  RING_VFIFO_SUPPORT
			      	rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
			    #else
					//	  rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
				  	rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
			    #endif
			      	rH264BSInitPrm.u4PredSa = /*PHYSICAL*/((UINT32)_pucPredSa[u4InstID]);
			      	i4VDEC_HAL_H264_InitBarrelShifter(0, u4InstID, &rH264BSInitPrm);
			      	u4VDEC_HAL_H264_ShiftGetBitStream(0, u4InstID, u4Bits);
			    }	
				if (1 == _u4DualCoreEnable[u4InstID])
				{
					ucCurrStatus = VDEC_CORE0_ST;
				}
	    		vSlice_layer_without_partition_nonIDR(u4InstID, VDEC_VLD0);			
				if (1 == _u4DualCoreEnable[u4InstID])
				{
					ucCurrStatus = VDEC_CORE1_ST;
					vSlice_layer_without_partition_nonIDR(VDEC_VLD1,VDEC_VLD1);
				}
	    		break;
			case VER_SEI:
			#if defined(MVC_VFY_SUPPORT)
	 			if(_ucMVCType[u4InstID] != 2)
			#endif
	    		vVerifySEI_Rbsp(u4InstID, VDEC_VLD0);
	    		break;
			case SPS:
	    		if(_ucMVCType[u4InstID] != 2)    	 
	    		{
	    			vVerifySeq_Par_Set_Rbsp(u4InstID, VDEC_VLD0);
					if (1 == _u4DualCoreEnable[u4InstID])
					{
						memcpy(_rH264SPS[1],_rH264SPS[0],sizeof(VDEC_INFO_H264_SPS_T)*32);
					}
	    		}
	   			 break;      
		#if defined(MVC_VFY_SUPPORT)
			case SUB_SPS:
	 			if(_ucMVCType[u4InstID] == 2)
				{
	      			vSubset_Seq_Parameter_Set_Rbsp_Verify(u4InstID);
		 		}
	    		break;      
		#endif        
			case PPS:
				vVerifyPic_Par_Set_Rbsp(u4InstID, VDEC_VLD0);
				if (1 == _u4DualCoreEnable[u4InstID])
				{
					memcpy(_rH264PPS[1],_rH264PPS[0],sizeof(VDEC_INFO_H264_PPS_T)*256);
				}
				break;         
			default:
				break;
		}
	}while(!((_ucNalUnitType[u4InstID] == NON_IDR_SLICE) || (_ucNalUnitType[u4InstID] == IDR_SLICE)));
}


// *********************************************************************
// Function    : UINT32 vAVCLAEParseSliceHeader(UINT32 u4InstID)
// Description : LAE part search for the real pic then to dec
// Parameter   : None
// Return      : None
// *********************************************************************
UINT32 vAVCLAEParseSliceHeader(UINT32 u4InstID)
{
	UINT32 u4Temp, u4Ret, u4LAEID;
	BOOL fgForbidenZeroBits;
	UINT32 u4Bits;
	VDEC_INFO_H264_BS_INIT_PRM_T rH264BSInitPrm;

	u4LAEID = (u4InstID == 0) ? VDEC_LAE0 : VDEC_LAE1;
#ifdef DEBUG_LOG
	UTIL_Printf("==============Usage LAE HW %d===================\n",u4LAEID);
#endif
	do
	{
		//_u4LAEParseStartAddr[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
		rH264BSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
		rH264BSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
		rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID] + _u4LAEParseStartAddr[u4InstID];
	#ifndef  RING_VFIFO_SUPPORT
		rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
	#else
		//	rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
		rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
	#endif
		rH264BSInitPrm.u4PredSa = ((UINT32)_pucPredSa[u4InstID]);	

		i4VDEC_HAL_H264_InitVDecHW(u4LAEID);
		i4VDEC_HAL_H264_InitBarrelShifter(_u4BSID[u4InstID], u4LAEID, &rH264BSInitPrm);
		u4Temp = u4VDEC_HAL_H264_GetStartCode_8530(_u4BSID[u4InstID], u4LAEID) & 0xff;

		fgForbidenZeroBits = ((u4Temp >> 7) & 0x01); // bit 31
		if(fgForbidenZeroBits != 0)
		{
			vErrInfo(VER_FORBIDEN_ERR);
		}
		_u4NalRefIdc[u4LAEID] = ((u4Temp >> 5) & 0x3); // bit 30,29
		_ucNalUnitType[u4LAEID] = (u4Temp & 0x1f); // bit 28,27,26,25,24
	  		
		u4Temp = u4VDEC_HAL_H264_ShiftGetBitStream(_u4BSID[u4InstID], u4LAEID, 0);
	#ifdef DEBUG_LOG
		UTIL_Printf("=============_ucNalUnitType %d=========\n", _ucNalUnitType[u4LAEID]);
	#endif
		switch(_ucNalUnitType[u4LAEID])
		{
			case NON_IDR_SLICE:
			case IDR_SLICE:			
				vSlice_layer_without_partition_nonIDR(u4LAEID, u4LAEID);				
				break;
			case VER_SEI:				
				vVerifySEI_Rbsp(u4LAEID, u4LAEID);				
				break;
			case SPS:				
				vVerifySeq_Par_Set_Rbsp(u4LAEID, u4LAEID);				
				break;		
			case PPS:				
				vVerifyPic_Par_Set_Rbsp(u4LAEID, u4LAEID);				
				break;		   
			default:
				break;
		}		
		_u4LAEParseStartAddr[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(_u4BSID[u4InstID], u4LAEID, (UINT32)_pucVFifo[u4InstID], &u4Bits);	
	}while(!((_ucNalUnitType[u4LAEID] == NON_IDR_SLICE) || (_ucNalUnitType[u4LAEID] == IDR_SLICE)));
}


// *********************************************************************
// Function    : BOOL fgAVCLAEDecIsFinish(UINT32 u4InstID)
// Description : Check if LAE part decode finish
// Parameter   : None
// Return      : None
// *********************************************************************
BOOL fgAVCLAEDecIsFinish(UINT32 u4InstID, unsigned long start_time)
{
	UINT32 u4Timeout = 0;
	UINT32 u4Loop = 0;
	BOOL fgDecFinish = FALSE;
    UINT32 u4RetryDelay = 100000;	
	
    while(!fgDecFinish)
    {
        if(((u4VDecReadMCORETOP(u4InstID, 0xC)>>24) & 0x1))
        {
            fgDecFinish = TRUE;
            break;
        }
      
        if ( ( jiffies - start_time > 1700 ) )
        {
            printk("\n!!!!!!LAE Polling int timeout !!!!!!\n\n");
            break;
        }
          
        while (u4RetryDelay > 0 )
        {
            u4RetryDelay--;
        }
        u4RetryDelay = 100000;
     }
	
	return fgDecFinish;
}


// *********************************************************************
// Function    : void vSlice_layer_without_partition_nonIDR(UINT32 u4InstID)
// Description : Handle nonIDR slice header
// Parameter   : None
// Return      : None
// *********************************************************************
void vSlice_layer_without_partition_nonIDR(UINT32 u4InstID, UINT32 u4VDecID)
{
	vParseSliceHeader(u4InstID, u4VDecID);  
}


// *********************************************************************
// Function    : void vSlice_layer_without_partition_IDR(UINT32 u4InstID)
// Description : Handle IDR slice header
// Parameter   : None
// Return      : None
// *********************************************************************
void vSlice_layer_without_partition_IDR(UINT32 u4InstID, UINT32 u4VDecID)
{
	vParseSliceHeader(u4InstID, u4VDecID);    
}

// *********************************************************************
// Function    : void vVerifySeq_Par_Set_Rbsp(UINT32 u4InstID)
// Description : Handle picture parameter set header
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifySeq_Par_Set_Rbsp(UINT32 u4InstID, UINT32 u4VDecID)
{
	UINT32 u4Temp;
	UINT32 i;
	UINT32 u4SeqParameterSetId;
	VDEC_INFO_H264_SPS_T *prSPS = NULL;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]Parse SPS Start\n");
#endif
	if(u4InstID == VDEC_LAE0)
    {
        _u4BSID[u4InstID] = 0;
    }
	u4Temp = (u4VDEC_HAL_H264_GetBitStreamShift(_u4BSID[u4InstID], u4VDecID, 24) >> 8);
#if defined(MVC_VFY_SUPPORT)  
	if((u4Temp & 0x700) > 0)  // [11:8]
#else
	if((u4Temp & 0xf00) > 0)  // [11:8]
#endif  
	{
	#if (!(CONFIG_DRV_VERIFY_SUPPORT && defined(MVC_VFY_SUPPORT)))
		//*fprintf(_tRecFileInfo.fpFile, "forbiden error in Seq_Par_Set_Rbsp\n");
		sprintf(_bTempStr1[u4InstID], "%s", "err in SPS Forbiden Zero\\n\\0");
		vErrMessage(u4InstID, (CHAR *)_bTempStr1[u4InstID]);
		return;
	#endif
	}

	// 1st
	u4SeqParameterSetId = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("seq_parameter_set_id 0x%.8x\n", u4SeqParameterSetId);
#endif
	if(u4SeqParameterSetId < 32)
	{	  
		prSPS = &_rH264SPS[u4InstID][u4SeqParameterSetId];
		prSPS->fgSPSValid = TRUE;

		prSPS->u4ProfileIdc = (u4Temp >> 16);                              // [23:16]
		prSPS->fgConstrainedSet0Flag = (u4Temp >> 15) & 0x1;      // [15]
		prSPS->fgConstrainedSet1Flag = (u4Temp >> 14) & 0x1;      // [14]
		prSPS->fgConstrainedSet2Flag = (u4Temp >> 13) & 0x1;      // [13]
		prSPS->fgConstrainedSet3Flag = (u4Temp >> 12) & 0x1;      // [12]
		prSPS->fgConstrainedSet4Flag = (u4Temp >> 11) & 0x1;      // [11] According to the latest spec AD007
		prSPS->u4LevelIdc = (u4Temp & 0xff);                              // [7:0]
		if(prSPS->fgConstrainedSet3Flag && (prSPS->u4LevelIdc == 11))
		{
			// should be 1b
			prSPS->u4LevelIdc = 10;
		}
		prSPS->u4SeqParameterSetId = u4SeqParameterSetId;
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.rLastInfo.ucLastSPSId = prSPS->u4SeqParameterSetId;
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("profile_idc 0x%.8x\n", prSPS->u4ProfileIdc);
		UTIL_Printf("Constrained_Set_Flag %d %d %d %d\n",prSPS->fgConstrainedSet0Flag,prSPS->fgConstrainedSet1Flag,prSPS->fgConstrainedSet2Flag,prSPS->fgConstrainedSet3Flag);
		UTIL_Printf("profile_idc 0x%.8x\n", prSPS->u4LevelIdc); 
	#endif
	}
	else
	{
		sprintf(_bTempStr1[u4InstID], "%s", "err in SPS Num\\n\\0");
		vErrMessage(u4InstID, (CHAR *)_bTempStr1[u4InstID]);    
		//*fprintf(_tRecFileInfo.fpFile, "seq_parameter_set_id error in Seq_Par_Set_Rbsp\n");
		//return;
		UTIL_Printf("Error: SPS(%d) invalid (%d).............................................\n",u4InstID,u4SeqParameterSetId);
	}
	vVerifyInitSPS(prSPS);
	// 2nd
	if((prSPS->u4ProfileIdc == FREXT_HP) 
	|| (prSPS->u4ProfileIdc == FREXT_Hi10P) 
	|| (prSPS->u4ProfileIdc == FREXT_Hi422)
	|| (prSPS->u4ProfileIdc == FREXT_Hi444)
#if  defined(MVC_VFY_SUPPORT)     
	|| (prSPS->u4ProfileIdc == 118)    
	|| (prSPS->u4ProfileIdc == 128)
#endif   
	)
	{
		prSPS->u4ChromaFormatIdc = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("chroma_format_idc 0x%.8x\n", prSPS->u4ChromaFormatIdc);
	#endif
		if(prSPS->u4ChromaFormatIdc == 3)
		{
			prSPS->fgResidualColorTransformFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		#ifdef DEBUG_LOG
			UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
			UTIL_Printf("residual_color_transform_flag %d\n", prSPS->fgResidualColorTransformFlag);
		#endif
		}
		prSPS->u4BitDepthLumaMinus8 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("bit_depth_luma_minus8 0x%.8x\n", prSPS->u4BitDepthLumaMinus8);
	#endif
		prSPS->u4BitDepthChromaMinus8 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("bit_depth_chroma_minus8 0x%.8x\n", prSPS->u4BitDepthChromaMinus8);
	#endif
		prSPS->fgQpprimeYZeroTransformBypassFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("apprime_y_zero_transform_bypass_flag %d\n", prSPS->fgQpprimeYZeroTransformBypassFlag);
	#endif
		prSPS->fgSeqScalingMatrixPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("seq_scaling_matrix_present_flag %d\n", prSPS->fgSeqScalingMatrixPresentFlag);
	#endif
		if(prSPS->fgSeqScalingMatrixPresentFlag)
		{
			for(i = 0; i < 8; i ++)
			{
				prSPS->fgSeqScalingListPresentFlag[i] = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
			#ifdef DEBUG_LOG
				UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
				UTIL_Printf("seq_scaling_list_present_flag[%d] %d\n", i,prSPS->fgSeqScalingListPresentFlag[i]);
			#endif
				if(prSPS->fgSeqScalingListPresentFlag[i])
				{
				  	if(i < 6)
				  	{
				    	vVDEC_HAL_H264_ScalingList(_u4BSID[u4InstID], u4VDecID, prSPS->cScalingList4x4[i],16, &prSPS->fgUseDefaultScalingMatrix4x4Flag[i]);
				  	}
				  	else
				  	{
				    	vVDEC_HAL_H264_ScalingList(_u4BSID[u4InstID], u4VDecID, prSPS->cScalingList8x8[i-6],64, &prSPS->fgUseDefaultScalingMatrix8x8Flag[i-6]);
				  	}
				}
			}
		}
	}

	prSPS->u4Log2MaxFrameNumMinus4 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	prSPS->u4MaxFrameNum = 2 << (prSPS->u4Log2MaxFrameNumMinus4 + 4 - 1);  //prSPS->u4MaxFrameNum = pow(2,prSPS->u4Log2MaxFrameNumMinus4 + 4);  
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("log2_max_frame_num_minus4 0x%.8x\n", prSPS->u4Log2MaxFrameNumMinus4);
#endif
	prSPS->u4PicOrderCntType = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("pic_order_cnt_type 0x%.8x\n", prSPS->u4PicOrderCntType);
#endif
	if(prSPS->u4PicOrderCntType == 0)
	{
		prSPS->u4Log2MaxPicOrderCntLsbMinus4 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("log2_max_pic_order_cnt_lsb_minus4 0x%.8x\n", prSPS->u4Log2MaxPicOrderCntLsbMinus4);
	#endif
	}
	else if(prSPS->u4PicOrderCntType == 1)
	{
		prSPS->fgDeltaPicOrderAlwaysZeroFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("delta_pic_order_always_zero_flag 0x%.8x\n", prSPS->fgDeltaPicOrderAlwaysZeroFlag);
	#endif
		prSPS->i4OffsetForNonRefPic = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("offset_for_non_ref_pic 0x%.8x\n", prSPS->i4OffsetForNonRefPic);
	#endif
		prSPS->i4OffsetForTopToBottomField = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("offset_for_top_to_bottom_field 0x%.8x\n", prSPS->i4OffsetForTopToBottomField);
	#endif
		prSPS->u4NumRefFramesInPicOrderCntCycle = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("num_ref_frames_in_pic_order_cnt_cycle 0x%.8x\n", prSPS->u4NumRefFramesInPicOrderCntCycle);
	#endif
		for(i=0 ; i<prSPS->u4NumRefFramesInPicOrderCntCycle; i++)
		{
			prSPS->i4OffsetForRefFrame[i] = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
			//*fprintf(_tRecFileInfo.fpFile, "offset_for_ref_frame[%d] %d\n", i, prSPS->i4OffsetForRefFrame[i]);
		}
	}
	prSPS->u4NumRefFrames = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("num_ref_frames 0x%.8x\n", prSPS->u4NumRefFrames);
#endif
	prSPS->fgGapsInFrameNumValueAllowedFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("gaps_in_frame_num_value_allowed_flag %d\n", prSPS->fgGapsInFrameNumValueAllowedFlag);
#endif
	prSPS->u4PicWidthInMbsMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("pic_width_in_mbs_minus1 0x%.8x\n", prSPS->u4PicWidthInMbsMinus1);
#endif
	prSPS->u4PicHeightInMapUnitsMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("pic_height_in_map_units_minus1 0x%.8x\n", prSPS->u4PicHeightInMapUnitsMinus1);
#endif
	prSPS->fgFrameMbsOnlyFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	if((prSPS->u4ProfileIdc >= 77) // upper than Main Profile
		&& ((prSPS->u4LevelIdc <= 20) || (prSPS->u4LevelIdc >= 42)))
	{
		prSPS->fgFrameMbsOnlyFlag = 1;
	}
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("frame_mbs_only_flag %d\n", prSPS->fgFrameMbsOnlyFlag);	
#endif
	prSPS->fgMbAdaptiveFrameFieldFlag = FALSE;
	if(!prSPS->fgFrameMbsOnlyFlag)
	{
		prSPS->fgMbAdaptiveFrameFieldFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("mb_adaptive_frame_field_flag %d\n", prSPS->fgMbAdaptiveFrameFieldFlag);
	#endif
	}
	prSPS->fgDirect8x8InferenceFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	if((prSPS->u4ProfileIdc >= 77) // upper than Main Profile
		&& (prSPS->u4LevelIdc >= 30))
	{
		prSPS->fgDirect8x8InferenceFlag = 1;
	}  
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
	_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsReduceMVBuffer = prSPS->fgDirect8x8InferenceFlag;
	//UTIL_Printf("reduction flag = %d \n",_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsReduceMVBuffer);
#endif
	//*fprintf(_tRecFileInfo.fpFile, "direct_8x8_inference_flag %d\n", prSPS->fgDirect8x8InferenceFlag);

	prSPS->fgFrameCroppingFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("frame_cropping_flag %d\n", prSPS->fgFrameCroppingFlag);
#endif

	if(prSPS->fgFrameCroppingFlag)
	{
		prSPS->u4FrameCropLeftOffset = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("frame_crop_left_offset 0x%.8x\n", prSPS->u4FrameCropLeftOffset);
	#endif

		prSPS->u4FrameCropRightOffset = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("frame_crop_right_offset 0x%.8x\n", prSPS->u4FrameCropRightOffset);
	#endif

		prSPS->u4FrameCropTopOffset = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("frame_crop_top_offset 0x%.8x\n", prSPS->u4FrameCropTopOffset);
	#endif

		prSPS->u4FrameCropBottomOffset = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("frame_crop_bottom_offset 0x%.8x\n", prSPS->u4FrameCropBottomOffset);
	#endif
	}
	else
	{
		prSPS->u4FrameCropLeftOffset = 0;
		prSPS->u4FrameCropRightOffset = 0;
		prSPS->u4FrameCropTopOffset = 0;
		prSPS->u4FrameCropBottomOffset = 0;
	}

	prSPS->fgVuiParametersPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("vui_parameters_present_flag %d\n", prSPS->fgVuiParametersPresentFlag);
#endif
#if 1
	if(prSPS->fgVuiParametersPresentFlag)
	{
		prSPS->rVUI.fgAspectRatioInfoPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		if(prSPS->rVUI.fgAspectRatioInfoPresentFlag)
		{
			prSPS->rVUI.u4AspectRatioIdc = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8);
			if(prSPS->rVUI.u4AspectRatioIdc == 255) //Extended_SAR
			{
				prSPS->rVUI.u4SarWidth = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 16);
				prSPS->rVUI.u4SarHeight = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 16);
			}
		}
		prSPS->rVUI.fgOverscanInfoPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		if(prSPS->rVUI.fgOverscanInfoPresentFlag)
		{
			prSPS->rVUI.fgOverscanAppropriateFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		}
		prSPS->rVUI.fgVideoSignalTypePresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		if(prSPS->rVUI.fgVideoSignalTypePresentFlag)
		{
			prSPS->rVUI.u4VideoFormat = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 3); 
			prSPS->rVUI.fgVideoFullRangeFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
			prSPS->rVUI.fgColourDescriptionPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
			if(prSPS->rVUI.fgColourDescriptionPresentFlag)
			{
				prSPS->rVUI.u4ColourPrimaries = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8); 
				prSPS->rVUI.u4TransferCharacteristics = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8); 
				prSPS->rVUI.u4MatrixCoefficients = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8); 
			}
		}
		prSPS->rVUI.fgChromaLocationInfoPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		if(prSPS->rVUI.fgChromaLocationInfoPresentFlag)
		{
			prSPS->rVUI.u4ChromaSampleLocTypeTopField = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID); 
			prSPS->rVUI.u4ChromaSampleLocTypeBottomField = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID); 
		}
		prSPS->rVUI.fgTimingInfoPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		if(prSPS->rVUI.fgTimingInfoPresentFlag)
		{
			prSPS->rVUI.u4NumUnitsInTick = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 32); 
			prSPS->rVUI.u4TimeScale = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 32); 
			prSPS->rVUI.fgFixedFrameRateFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		}
		prSPS->rVUI.fgNalHrdParametersPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		if(prSPS->rVUI.fgNalHrdParametersPresentFlag)
		{
			vVerifyHrdParameters(u4InstID, u4VDecID, &prSPS->rVUI.tNalHrdParameters);
		}
		prSPS->rVUI.fgVclHrdParametersPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		if(prSPS->rVUI.fgVclHrdParametersPresentFlag)
		{
			vVerifyHrdParameters(u4InstID, u4VDecID, &prSPS->rVUI.tVclHrdParameters);
		}
		if(prSPS->rVUI.fgNalHrdParametersPresentFlag || prSPS->rVUI.fgVclHrdParametersPresentFlag)
		{
			prSPS->rVUI.fgLowDelayHrdFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		}
		prSPS->rVUI.fgPicStructPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		prSPS->rVUI.fgBitstreamRestrictionFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		if(prSPS->rVUI.fgBitstreamRestrictionFlag)
		{
			prSPS->rVUI.fgMotionVectorsOverPicBoundariesFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
			prSPS->rVUI.u4MaxBytesPerPicDenom = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
			prSPS->rVUI.u4MaxBitsPerMbDenom = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
			prSPS->rVUI.u4Log2MaxMvLengthHorizontal = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
			prSPS->rVUI.u4Log2MaxMvLengthVertical = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
			prSPS->rVUI.u4NumReorderFrames = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
			prSPS->rVUI.u4MaxDecFrameBuffering = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
		}
	}
#endif

#if  defined(MVC_VFY_SUPPORT)   
	if(_ucMVCType[u4InstID] != 2)
#endif 
	{
		vVDEC_HAL_H264_TrailingBits(_u4BSID[u4InstID], u4VDecID);
	}

	prSPS->fgSPSValid = TRUE;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]Parse SPS End\n");
#endif
}


// *********************************************************************
// Function    : void vVerifyPic_Par_Set_Rbsp(UINT32 u4InstID)
// Description : Handle picture parameter set header
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyPic_Par_Set_Rbsp(UINT32 u4InstID, UINT32 u4VDecID)
{
	UINT32 i;
	UINT32 u4PPSID;
	VDEC_INFO_H264_SPS_T *prSPS;
	VDEC_INFO_H264_PPS_T *ptPPS;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]Parse PPS Start\n");
#endif
	if(u4InstID == VDEC_LAE0)
    {
        _u4BSID[u4InstID] = 0;
    }
	//ptPPS->fgPPSValid = FALSE;
	u4PPSID = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	if(u4PPSID < 256)
	{
		ptPPS = &_rH264PPS[u4InstID][u4PPSID];
		ptPPS->fgPPSValid = FALSE; // FALSE until set completely
	#ifdef DEBUG_LOG
		UTIL_Printf("pic_parameter_set_id 0x%.8x\n", u4PPSID);
	#endif
	}
	else
	{
		//*fprintf(_tRecFileInfo.fpFile, "pic_parameter_set_id error in Pic_Par_Set_Rbsp\n");
		sprintf(_bTempStr1[u4InstID], "%s", "err in PPS Num err\\n\\0");
		vErrMessage(u4InstID, (CHAR *)_bTempStr1[u4InstID]);    
		UTIL_Printf("Error: PPS(%d) invalid (%d).............................................\n",u4InstID,u4PPSID);
		return;
	}

	ptPPS->u4SeqParameterSetId = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	prSPS = &_rH264SPS[u4InstID][ptPPS->u4SeqParameterSetId];
	if(prSPS->fgSPSValid)
	{
	#ifdef DEBUG_LOG
		UTIL_Printf("seq_parameter_set_id 0x%.8x\n", ptPPS->u4SeqParameterSetId);
	#endif
	}
	else
	{
	#ifdef DEBUG_LOG
		UTIL_Printf("invalid seq_parameter_set_id 0x%.8x in Pic_Par_Set\n", ptPPS->u4SeqParameterSetId);
	#endif
	//return;
	}

	ptPPS->fgEntropyCodingModeFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("entropy_coding_mode_flag %d\n", ptPPS->fgEntropyCodingModeFlag);
#endif

	ptPPS->fgPicOrderPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("pic_order_present_flag %d\n", ptPPS->fgPicOrderPresentFlag);
#endif

	ptPPS->u4NumSliceGroupsMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("num_slice_groups_minus1 0x%.8x\n", ptPPS->u4NumSliceGroupsMinus1);
#endif

	if(ptPPS->u4NumSliceGroupsMinus1 > 255)
	{
		vVDecOutputDebugString("///Warning!!! num_slice_groups_minus1 size isn't enough ///\n");
	}

	if(ptPPS->u4NumSliceGroupsMinus1 > 0)
	{
		ptPPS->u4SliceGroupMapType = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
		UTIL_Printf("slice_group_map_type 0x%.8x\n", ptPPS->u4SliceGroupMapType);
#endif
		if(ptPPS->u4SliceGroupMapType == 0)
		{
			for(i = 0; i <= ptPPS->u4NumSliceGroupsMinus1; i++)
			{
				ptPPS->u4RunLengthMinus1[i] = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
				//*fprintf(_tRecFileInfo.fpFile, "run_length_minus1[%d] 0x%.8x\n", i, ptPPS->u4RunLengthMinus1[i]);
			}
		}
		else if(ptPPS->u4SliceGroupMapType == 2)
		{
			for(i = 0; i < ptPPS->u4NumSliceGroupsMinus1; i++)
			{
				ptPPS->u4TopLeft[i] = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
				ptPPS->u4BottomRight[i] = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
				//*fprintf(_tRecFileInfo.fpFile, "top_left[i] 0x%.8x, bottom_right[i] 0x%.8x\n", ptPPS->u4TopLeft[i], ptPPS->u4BottomRight[i]);
			}
		}
		else if((ptPPS->u4SliceGroupMapType == 3) ||
		        	(ptPPS->u4SliceGroupMapType == 4) ||
		     	(ptPPS->u4SliceGroupMapType == 5))
		{
			ptPPS->fgSliceGroupChangeDirectionFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
			//*fprintf(_tRecFileInfo.fpFile, "pic_order_present_flag %d\n", ptPPS->fgSliceGroupChangeDirectionFlag);      
			ptPPS->u4SliceGroupChangeRateMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
			//*fprintf(_tRecFileInfo.fpFile, "slice_group_change_rate_minus1 0x%.8x\n", ptPPS->u4SliceGroupChangeRateMinus1);      
		}
		else if(ptPPS->u4SliceGroupMapType == 6)
		{
			ptPPS->u4PicSizeInMapUnitsMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
			//*fprintf(_tRecFileInfo.fpFile, "pic_size_in_map_units_minus1 0x%.8x\n", ptPPS->u4PicSizeInMapUnitsMinus1);            
			for(i = 0; i <= ptPPS->u4PicSizeInMapUnitsMinus1; i++)
			{
				ptPPS->pu4SliceGroupId[i] = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
				//*fprintf(_tRecFileInfo.fpFile, "slice_group_id[%d] 0x%.8x\n", i, ptPPS->pu4SliceGroupId[i]);                    
			}
		}
	}

	ptPPS->u4NumRefIdxL0ActiveMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("num_ref_idx_l0_active_minus1 0x%.8x\n", ptPPS->u4NumRefIdxL0ActiveMinus1);
#endif
	ptPPS->u4NumRefIdxL1ActiveMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("num_ref_idx_l1_active_minus1 0x%.8x\n", ptPPS->u4NumRefIdxL1ActiveMinus1);
#endif

	ptPPS->fgWeightedPredFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("weighted_pred_flag %d\n", ptPPS->fgWeightedPredFlag);
#endif
	ptPPS->u4WeightedBipredIdc = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 2); 
#ifdef DEBUG_LOG
	UTIL_Printf("weighted_bipred_idc 0x%.8x\n", ptPPS->u4WeightedBipredIdc);
#endif

	ptPPS->i4PicInitQpMinus26 = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
	//*fprintf(_tRecFileInfo.fpFile, "pic_init_qp_minus26 0x%.8x\n", ptPPS->i4PicInitQpMinus26);
	ptPPS->i4PicInitQsMinus26 = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
	//*fprintf(_tRecFileInfo.fpFile, "pic_init_qs_minus26 0x%.8x\n", ptPPS->i4PicInitQsMinus26);

	ptPPS->i4ChromaQpIndexOffset = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
	//*fprintf(_tRecFileInfo.fpFile, "chroma_qp_index_offset 0x%.8x\n", ptPPS->i4ChromaQpIndexOffset);
	ptPPS->fgDeblockingFilterControlPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("deblocking_filter_control_present_flag %d\n", ptPPS->fgDeblockingFilterControlPresentFlag);
#endif
	ptPPS->fgConstrainedIntraPredFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("constrained_intra_pred_flag %d\n", ptPPS->fgConstrainedIntraPredFlag);
#endif
	ptPPS->fgRedundantPicCntPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("redundant_pic_cnt_present_flag %d\n", ptPPS->fgRedundantPicCntPresentFlag);
#endif

	if(bVDEC_HAL_H264_IsMoreRbspData(_u4BSID[u4InstID], u4VDecID))
	{
		ptPPS->fgTransform8x8ModeFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		//*fprintf(_tRecFileInfo.fpFile, "transform_8x8_mode_flag %d\n", ptPPS->fgTransform8x8ModeFlag);
		ptPPS->fgPicScalingMatrixPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		//*fprintf(_tRecFileInfo.fpFile, "pic_scaling_matrix_present_flag %d\n", ptPPS->fgPicScalingMatrixPresentFlag);
		if(ptPPS->fgPicScalingMatrixPresentFlag)
		{
			for(i=0; i<((ptPPS->fgTransform8x8ModeFlag << 1) + 6); i++)
			{
				ptPPS->fgPicScalingListPresentFlag[i] = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
				//*fprintf(_tRecFileInfo.fpFile, "pic_scaling_list_present_flag[%d] %d\n", i, ptPPS->fgPicScalingListPresentFlag[i]);
				if(ptPPS->fgPicScalingListPresentFlag[i])
				{
				  	if(i < 6)
				  	{
				    	vVDEC_HAL_H264_ScalingList(_u4BSID[u4InstID], u4VDecID, ptPPS->cScalingList4x4[i],16, &ptPPS->fgUseDefaultScalingMatrix4x4Flag[i]);
				  	}
				  	else
				  	{
				    	vVDEC_HAL_H264_ScalingList(_u4BSID[u4InstID], u4VDecID, ptPPS->cScalingList8x8[i-6], 64, &ptPPS->fgUseDefaultScalingMatrix8x8Flag[i-6]);
				  	}
				}
			}
		}
		ptPPS->i4SecondChromaQpIndexOffset = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
		//*fprintf(_tRecFileInfo.fpFile, "second_chroma_qp_index_offset 0x%.8x\n", ptPPS->i4SecondChromaQpIndexOffset);
	}
	else
	{
		ptPPS->fgTransform8x8ModeFlag = 0;
		ptPPS->fgPicScalingMatrixPresentFlag = FALSE;
		ptPPS->i4SecondChromaQpIndexOffset = ptPPS->i4ChromaQpIndexOffset;
	}
	vVDEC_HAL_H264_TrailingBits(_u4BSID[u4InstID], u4VDecID);

	ptPPS->fgPPSValid = TRUE;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]Parse PPS End\n");
#endif
}


// *********************************************************************
// Function    : void vSearchRealPic(UINT32 u4InstID)
// Description : Search for the real pic then to dec
// Parameter   : None
// Return      : None
// *********************************************************************
void vParseSliceHeader(UINT32 u4InstID, UINT32 u4VDecID)
{
	VDEC_INFO_H264_SLICE_HDR_T *prSliceHdr;
	VDEC_INFO_DEC_PRM_T  *ptVerMpvDecPrm;
	UINT32 u4PPSID;
	UINT32 u4OriBSID = _u4BSID[u4InstID];
	UINT32 u4Temp;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]Parse IDR/Non-IDR Start\n");
#endif

	_u4BSID[u4InstID] = 0;

	u4Temp = u4VDEC_HAL_H264_ShiftGetBitStream(_u4BSID[u4InstID], u4VDecID, 0);
	ptVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];
	prSliceHdr = &_rH264SliceHdr[u4InstID];
	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr = prSliceHdr;

	prSliceHdr->u4FirstMbInSlice = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("first_mb_in_slice 0x%.8x\n", prSliceHdr->u4FirstMbInSlice);
#endif
	prSliceHdr->u4SliceType = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	//*fprintf(_tRecFileInfo.fpFile, "slice_type 0x%.8x\n", prSliceHdr->u4SliceType);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("[vdec-h264] slice type = %d\n",prSliceHdr->u4SliceType);
#endif
	if(prSliceHdr->u4SliceType >= 5)
	{
		//prSliceHdr->u4SliceType -= 5;
	}

	u4PPSID = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("PPSID %d\n", u4PPSID);
#endif
	if((u4PPSID < 256)
		&& (_rH264PPS[u4InstID][u4PPSID].fgPPSValid)
		&& (_rH264SPS[u4InstID][_rH264PPS[u4InstID][u4PPSID].u4SeqParameterSetId].fgSPSValid))
	{
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS = &_rH264PPS[u4InstID][u4PPSID];
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS = &_rH264SPS[u4InstID][_rH264PPS[u4InstID][u4PPSID].u4SeqParameterSetId];
#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("pic_parameter_set_id 0x%.8x\n", u4PPSID);
#endif
	}
	else
	{
		sprintf(_bTempStr1[u4InstID], "%s", "err in Slice Hdr PPS Num err\\n\\0");
		vErrMessage(u4InstID, (CHAR *)_bTempStr1[u4InstID]);        
		UTIL_Printf("Error(%d) PPsID(%d).................\n",u4InstID,u4PPSID);
	}

	vVerifyInitSliceHdr(ptVerMpvDecPrm);
	prSliceHdr->u4FrameNum = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4Log2MaxFrameNumMinus4 + 4);	
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("u4FrameNum 0x%.8x\n", prSliceHdr->u4FrameNum);
#endif
	if(fgIsIDRPic(u4InstID)) //IDR picture
	{
	//ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgIDRPic = true;
	//*fprintf(_tRecFileInfo.fpFile, "in IDR, frame_num 0x%.8x\n", prSliceHdr->u4FrameNum);
	}
	else // not IDR picture
	{
	//ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.fgIDRPic = false;
	//*fprintf(_tRecFileInfo.fpFile, "in non-IDR, frame_num 0x%.8x\n", prSliceHdr->u4FrameNum);
	}

	prSliceHdr->fgFieldPicFlag = FALSE;
	prSliceHdr->fgBottomFieldFlag = FALSE;
	if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgFrameMbsOnlyFlag)
	{
		ptVerMpvDecPrm->ucPicStruct = FRAME;
	}
	else
	{
		prSliceHdr->fgFieldPicFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("field_pic_flag %d\n", prSliceHdr->fgFieldPicFlag);
	#endif
		if(prSliceHdr->fgFieldPicFlag)
		{
			prSliceHdr->fgBottomFieldFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		#ifdef DEBUG_LOG
			UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
			UTIL_Printf("bottom_field_flag %d\n", prSliceHdr->fgBottomFieldFlag);
		#endif
			ptVerMpvDecPrm->ucPicStruct = (prSliceHdr->fgBottomFieldFlag) ? BOTTOM_FIELD : TOP_FIELD;            
		}
		else
		{
			ptVerMpvDecPrm->ucPicStruct = FRAME;           
		}
	}
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("pic struct %d\n", ptVerMpvDecPrm->ucPicStruct);
#endif

	if(fgIsIDRPic(u4InstID)) //IDR picture
	{
		prSliceHdr->u4IdrPicId = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("idr_pic_id 0x%.8x\n", prSliceHdr->u4IdrPicId);
	#endif
	}
	if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicOrderCntType == 0)
	{
		prSliceHdr->i4PicOrderCntLsb = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4Log2MaxPicOrderCntLsbMinus4 + 4);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("pic_order_cnt_lsb 0x%.8x\n", prSliceHdr->i4PicOrderCntLsb);
	#endif
		if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgPicOrderPresentFlag && (!prSliceHdr->fgFieldPicFlag))
		{
			prSliceHdr->i4DeltaPicOrderCntBottom = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
		}
		else
		{
			prSliceHdr->i4DeltaPicOrderCntBottom = 0;
		}
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("delta_pic_order_cnt_bottom 0x%.8x\n", prSliceHdr->i4DeltaPicOrderCntBottom);       
	#endif
	}

	if((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicOrderCntType == 1) && (!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgDeltaPicOrderAlwaysZeroFlag))
	{
		prSliceHdr->i4DeltaPicOrderCnt[0] = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("delta_pic_order_cnt[0] 0x%.8x\n", prSliceHdr->i4DeltaPicOrderCnt[0]);
	#endif
		if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgPicOrderPresentFlag && (!prSliceHdr->fgFieldPicFlag))
		{
			prSliceHdr->i4DeltaPicOrderCnt[1] = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
			UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
			UTIL_Printf("delta_pic_order_cnt[1] 0x%.8x\n", prSliceHdr->i4DeltaPicOrderCnt[1]);
	#endif
		}
	}
	else
	{
		if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->u4PicOrderCntType == 1)
		{
			prSliceHdr->i4DeltaPicOrderCnt[0] = 0;
			prSliceHdr->i4DeltaPicOrderCnt[1] = 0;
		#ifdef DEBUG_LOG
			UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
			UTIL_Printf("delta_pic_order_cnt[0] 0x%.8x\n", prSliceHdr->i4DeltaPicOrderCnt[0]);
			UTIL_Printf("delta_pic_order_cnt[1] 0x%.8x\n", prSliceHdr->i4DeltaPicOrderCnt[1]);
		#endif
		}
	}

	if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgRedundantPicCntPresentFlag)
	{
		prSliceHdr->u4RedundantPicCnt = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("redundant_pic_cnt 0x%.8x\n", prSliceHdr->u4RedundantPicCnt);
	#endif
	}
	if(fgIsBSlice(prSliceHdr->u4SliceType))
	{
		prSliceHdr->fgDirectSpatialMvPredFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("direct_spatial_mv_pred_flag %d\n", prSliceHdr->fgDirectSpatialMvPredFlag);
	#endif
	}
	if(fgIsPSlice(prSliceHdr->u4SliceType) || fgIsBSlice(prSliceHdr->u4SliceType))
	{
		prSliceHdr->fgNumRefIdxActiveOverrideFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("num_ref_idx_active_override_flag %d\n", prSliceHdr->fgNumRefIdxActiveOverrideFlag);
	#endif
		if(prSliceHdr->fgNumRefIdxActiveOverrideFlag)
		{
			prSliceHdr->u4NumRefIdxL0ActiveMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
		#ifdef DEBUG_LOG
			UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
			UTIL_Printf("num_ref_idx_l0_active_minus1 0x%.8x\n", prSliceHdr->u4NumRefIdxL0ActiveMinus1);
		#endif
			if(fgIsBSlice(prSliceHdr->u4SliceType))
			{
				prSliceHdr->u4NumRefIdxL1ActiveMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
		#ifdef DEBUG_LOG
				UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
				UTIL_Printf("num_ref_idx_l1_active_minus1 0x%.8x\n", prSliceHdr->u4NumRefIdxL1ActiveMinus1);
		#endif
			}
		}
	}
	if(!fgIsBSlice(prSliceHdr->u4SliceType))
	{
		prSliceHdr->u4NumRefIdxL1ActiveMinus1 = 0;
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("u4NumRefIdxL1ActiveMinus1 %d\n", prSliceHdr->u4NumRefIdxL1ActiveMinus1);
	#endif
	}

	vVerifyVDecSetPicInfo(u4InstID, u4VDecID, ptVerMpvDecPrm);

	vVerifyRef_Pic_List_Reordering(u4InstID, u4VDecID, prSliceHdr);

	if((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgWeightedPredFlag && fgIsPSlice(prSliceHdr->u4SliceType))
		|| ((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->u4WeightedBipredIdc == 1) && fgIsBSlice(prSliceHdr->u4SliceType)))
	{
		vVDEC_HAL_H264_PredWeightTable(u4VDecID);
	}

	if(_u4NalRefIdc[u4InstID] != 0)
	{
		prSliceHdr->fgMmco5 = FALSE;      
		vVerifyDec_Ref_Pic_Marking(u4InstID, u4VDecID, prSliceHdr);
	}

	if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgEntropyCodingModeFlag && (!fgIsISlice(prSliceHdr->u4SliceType)))
	{
		prSliceHdr->u4CabacInitIdc = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("cabac_init_idc 0x%.8x\n", prSliceHdr->u4CabacInitIdc);   
	#endif
	}
	else
	{
		prSliceHdr->u4CabacInitIdc = 0;
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("cabac_init_idc 0x%.8x\n", prSliceHdr->u4CabacInitIdc);   
	#endif
	}

	prSliceHdr->i4SliceQpDelta = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
	UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
	UTIL_Printf("slice_qp_delta 0x%.8x\n", prSliceHdr->i4SliceQpDelta);
#endif

	if((prSliceHdr->u4SliceType == SI_Slice) || (prSliceHdr->u4SliceType == SP_Slice))
	{
		if(prSliceHdr->u4SliceType == SP_Slice)
		{
			prSliceHdr->fgSpForSwitchFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		#ifdef DEBUG_LOG
			UTIL_Printf("sp_for_switch_flag %d\n", prSliceHdr->fgSpForSwitchFlag);
		#endif
		}
		prSliceHdr->i4SliceQsDelta = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("slice_qs_delta 0x%.8x\n", prSliceHdr->i4SliceQsDelta);
	#endif
	}

	if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->fgDeblockingFilterControlPresentFlag)
	{
		prSliceHdr->u4DisableDeblockingFilterIdc = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
		UTIL_Printf("disable_deblocking_filter_idc 0x%.8x\n", prSliceHdr->u4DisableDeblockingFilterIdc);
	#endif
		if(prSliceHdr->u4DisableDeblockingFilterIdc != 1)
		{
			prSliceHdr->i4SliceAlphaC0OffsetDiv2 = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
		#ifdef DEBUG_LOG
			UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
			UTIL_Printf("slice_alpha_c0_offset_div2 0x%.8x\n", prSliceHdr->i4SliceAlphaC0OffsetDiv2);
		#endif
			prSliceHdr->i4SliceBetaOffsetDiv2 = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
		#ifdef DEBUG_LOG
			UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
			UTIL_Printf("slice_beta_offset_div2 0x%.8x\n", prSliceHdr->i4SliceBetaOffsetDiv2);
		#endif
		}
		else
		{
			prSliceHdr->i4SliceAlphaC0OffsetDiv2 = 0;
			prSliceHdr->i4SliceBetaOffsetDiv2 = 0;
		#ifdef DEBUG_LOG
			UTIL_Printf("[H264] Inst%d, Input Window: 0x%08x\n", u4VDecID, u4VDecReadAVCVLD(u4VDecID, RO_AVLD_BARL));
			UTIL_Printf("slice_alpha_c0_offset_div2 0x%.8x\n", prSliceHdr->i4SliceAlphaC0OffsetDiv2);
			UTIL_Printf("slice_beta_offset_div2 0x%.8x\n", prSliceHdr->i4SliceBetaOffsetDiv2);
		#endif
		}
	}

	if((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->u4NumSliceGroupsMinus1 > 0) && 
	(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->u4SliceGroupMapType >= 3) &&
	(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->u4SliceGroupMapType <= 5))
	{
		//*fprintf(_tRecFileInfo.fpFile, "in slice_group_change_cycle\n");
		prSliceHdr->u4SliceGroupChangeCycle = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		//*fprintf(_tRecFileInfo.fpFile, "slice_beta_offset_div2 0x%.8x\n", prSliceHdr->u4SliceGroupChangeCycle);    
	}
	_u4BSID[u4InstID] = u4OriBSID;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]Parse IDR/Non-IDR Done\n");
#endif
}


// *********************************************************************
// Function    : void vVerifyRef_Pic_List_Reordering(UINT32 u4InstID,  UINT32 u4VDecID, VDEC_INFO_H264_SLICE_HDR_T *prSliceHdr)
// Description : ref pic List0 & List1 reordering
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyRef_Pic_List_Reordering(UINT32 u4InstID,  UINT32 u4VDecID, VDEC_INFO_H264_SLICE_HDR_T *prSliceHdr)
{
	if(!fgIsISlice(prSliceHdr->u4SliceType))
	{
		vVDEC_HAL_H264_Reording(u4VDecID);
	}
}

// *********************************************************************
// Function    : void vVerifyDec_Ref_Pic_Marking(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_H264_SLICE_HDR_T *prSliceHdr)
// Description : mark ref pic
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyDec_Ref_Pic_Marking(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_H264_SLICE_HDR_T *prSliceHdr)
{   
	UINT32 u4Cnt;

	if(u4InstID == VDEC_LAE0)
    {
        _u4BSID[u4InstID] = 0;
    }
	if(fgIsIDRPic(u4InstID))
	{
		prSliceHdr->fgNoOutputOfPriorPicsFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
		UTIL_Printf("no_output_of_prior_pics_flag %d\n", prSliceHdr->fgNoOutputOfPriorPicsFlag);
#endif
		prSliceHdr->fgLongTermReferenceFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
#ifdef DEBUG_LOG
		UTIL_Printf("long_term_reference_flag %d\n", prSliceHdr->fgLongTermReferenceFlag);
#endif
		if(prSliceHdr->fgLongTermReferenceFlag)
		{
		//  u4RefPicCntFrameIdx = 0xff;
		//  u4MaxLongTermFrameIdx = 0;
		}
		else
		{
		//  u4RefPicCntFrameIdx = 0;
		//  u4MaxLongTermFrameIdx = 0xff;
		}
	}
	else
	{
		prSliceHdr->fgAdaptiveRefPicMarkingModeFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	#ifdef DEBUG_LOG
		UTIL_Printf("adaptive_ref_pic_marking_mode_flag %d\n", prSliceHdr->fgAdaptiveRefPicMarkingModeFlag);
	#endif
		if(prSliceHdr->fgAdaptiveRefPicMarkingModeFlag)
		{
		  	u4Cnt = 0;
		  	do
		  	{
		    	prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
		    	if((prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] == 1))
			    {
			      	prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] |= (u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID) << 8);  
				  #ifdef DEBUG_LOG
			      	UTIL_Printf("differenc_of_pic_nums_minus1 0x%.8x\n", prSliceHdr->u4DifferencOfPicNumsMinus1);
				  #endif
				}
		    	else if(prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] == 2)
			    {
			      	prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] |= (u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID) << 8);  
				#ifdef DEBUG_LOG
				  	UTIL_Printf("long_term_pic_num 0x%.8x\n", prSliceHdr->u4LongTermPicNum);
				#endif
			    }
		    	else if(prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] == 3)
			    {
			      	prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] |= (u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID) << 8);  
			      	prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] |= (u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID) << 16);         
				#ifdef DEBUG_LOG
				  	UTIL_Printf("long_term_frame_idx 0x%.8x\n", prSliceHdr->u4LongTermFrameIdx);
				#endif
			    }
		    	else if(prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] == 4)
			    {
			      	prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] |= (u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID) << 8);                     
				#ifdef DEBUG_LOG
				  	UTIL_Printf("max_long_term_frame_idx_plus1 0x%.8x\n", prSliceHdr->u4MaxLongTermFrameIdxPlus1);
				#endif
				}
		    	else if(prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] == 5)
			    {
			      	prSliceHdr->fgMmco5 = TRUE;
			    }
		    	else if(prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] == 6)
			    {
			      	prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] |= (u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID) << 8);           
				#ifdef DEBUG_LOG
				  	UTIL_Printf("long_term_frame_idx 0x%.8x\n", prSliceHdr->u4LongTermFrameIdx);
				#endif
				}        
		    	u4Cnt ++;
		  }while(prSliceHdr->u4MemoryManagementControlOperation[u4Cnt-1] != 0);
		}
	}
}


// *********************************************************************
// Function    : void vVerifyInitSPS(VDEC_INFO_H264_SPS_T *prSPS)
// Description : Init SPS related fields
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyInitSPS(VDEC_INFO_H264_SPS_T *prSPS)
{
	INT32 i;

	prSPS->u4ChromaFormatIdc = 1;
	prSPS->u4BitDepthLumaMinus8 = 0;
	prSPS->u4BitDepthChromaMinus8 = 0;
	prSPS->fgQpprimeYZeroTransformBypassFlag = FALSE;  
	prSPS->fgSeqScalingMatrixPresentFlag = FALSE;
	for(i=0; i<8; i++)
	{
		prSPS->fgSeqScalingListPresentFlag[i] = FALSE;  
	}
}


// *********************************************************************
// Function    : void vVerifyHrdParameters(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_H264_HRD_PRM_T *tHrdPara)
// Description : 
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyHrdParameters(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_H264_HRD_PRM_T *tHrdPara)
{
	UINT32 u4SchedSelIdx;
	
	tHrdPara->u4CpbCntMinus1 = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	tHrdPara->u4BitRateScale = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 4);
	tHrdPara->u4CpbSizeScale = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 4);  
	for(u4SchedSelIdx=0; u4SchedSelIdx<=  tHrdPara->u4CpbCntMinus1; u4SchedSelIdx++)
	{
		tHrdPara->u4BitRateValueMinus1[u4SchedSelIdx] = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
		tHrdPara->u4CpbSizeValueMinus1[u4SchedSelIdx] = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
		tHrdPara->fgCbrFlag[u4SchedSelIdx] = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
	}
	tHrdPara->u4InitialCpbRemovalDelayLengthMinus1 = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 5);  
	tHrdPara->u4CpbRemovalDelayLengthMinus1 = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 5);  
	tHrdPara->u4DpbOutputDelayLengthMinus1 = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 5);  
	tHrdPara->u4TimeOffsetLength = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 5);  
}


// *********************************************************************
// Function    : void vVerifyInitSliceHdr()
// Description : 
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyInitSliceHdr(VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm)
{
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[0] = 0;
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4DeltaPicOrderCnt[1] = 0;
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4RedundantPicCnt = 0;  
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4DisableDeblockingFilterIdc = 0;  
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4SliceAlphaC0OffsetDiv2 = 0;    
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4SliceBetaOffsetDiv2 = 0;    
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4NumRefIdxL0ActiveMinus1 = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->u4NumRefIdxL0ActiveMinus1;
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4NumRefIdxL1ActiveMinus1 = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prPPS->u4NumRefIdxL1ActiveMinus1;
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx = 0xffffffff; 
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermPicNum = 0;  
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgDirectSpatialMvPredFlag = FALSE;
	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4IdrPicId = 0;
}


// *********************************************************************
// Function    : void vVerifySEI_Rbsp(UINT32 u4InstID)
// Description : SEI parameter set header
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifySEI_Rbsp(UINT32 u4InstID, UINT32 u4VDecID)
{
	UINT32 u4PayloadType = 0;
	INT32 u4PayloadSize = 0;
	UINT32 u4Offset = 1;
	UCHAR bTmpByte = 0;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]Parse SEI Start\n");
#endif
	if(u4InstID == VDEC_LAE0)
    {
        _u4BSID[u4InstID] = 0;
    }
	do
	{
	// sei_message();
		u4PayloadType = 0;
		while ((bTmpByte = (u4VDEC_HAL_H264_GetBitStreamShift(_u4BSID[u4InstID], u4VDecID, 8) >> 24)) == 0xFF)
		{
			u4PayloadType += 255;
		}
		u4PayloadType += bTmpByte;   // this is the last UCHAR

		u4PayloadSize = 0;
		while ((bTmpByte = (u4VDEC_HAL_H264_GetBitStreamShift(_u4BSID[u4InstID], u4VDecID, 8) >> 24)) == 0xFF)
		{
			u4PayloadSize += 255;
		}
		u4PayloadSize += bTmpByte;   // this is the last UCHAR

		switch ( u4PayloadType )     // sei_payload( type, size );
		{
			case  SEI_BUFFERING_PERIOD:
				vInterpretBufferingPeriodInfo(u4InstID, u4VDecID, &_tVerMpvDecPrm[u4InstID]);
				break;
			case  SEI_FILM_GRAIN_CHARACTERISTICS:
				vInterpretFilmGrainCharacteristicsInfo(u4InstID, u4VDecID, &_tVerMpvDecPrm[u4InstID]);
				break;
			default:        
		//vInitVDecHW();
				while(u4PayloadSize > 0)
				{
				  	bTmpByte = u4VDEC_HAL_H264_ShiftGetBitStream(_u4BSID[u4InstID], u4VDecID, 8);
				  	u4PayloadSize --;
				}
				break;
		}
		u4Offset += u4PayloadSize;
		vVDEC_HAL_H264_TrailingBits(_u4BSID[u4InstID], u4VDecID);
	} while(bVDEC_HAL_H264_IsMoreRbspData(_u4BSID[u4InstID], u4VDecID));    // more_rbsp_data()  msg[offset] != 0x80
// ignore the trailing bits rbsp_trailing_bits();  
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("[H264]Parse SEI End\n");
#endif
}


// *********************************************************************
// Function    : void vInterpretFilmGrainCharacteristicsInfo(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm)
// Description : SEI Film Grain parameter set header
// Parameter   : VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm
// Return      : None
// *********************************************************************
void vInterpretFilmGrainCharacteristicsInfo(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm)
{
	UINT32 c,i,j,k;
	UINT32 u4Temp;
	if(u4InstID == VDEC_LAE0)
    {
        _u4BSID[u4InstID] = 0;
    }
	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgFilmGrainCharacteristicsCancelFlag  = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID); // Used to shift 1-bit
	#ifdef DEBUG_LOG
	UTIL_Printf("film_grain_characteristics_cancel_flag = %d\n", ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgFilmGrainCharacteristicsCancelFlag);
	#endif
	if(!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgFilmGrainCharacteristicsCancelFlag)
	{
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4ModelId = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 2);
		#ifdef DEBUG_LOG
		UTIL_Printf("model_id = %d\n", ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4ModelId);
		#endif
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgSeparateColourDescriptionPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
		#ifdef DEBUG_LOG
		UTIL_Printf("separate_colour_description_present_flag = %d\n", ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgSeparateColourDescriptionPresentFlag);
		#endif
		if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgSeparateColourDescriptionPresentFlag)
		{
			ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4FilmGrainBitDepthLumaMinus8 = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 3);
			ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4FilmGrainBitDepthChromaMinus8 = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 3);
			ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgFilmGrainFullRangeFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
			ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4FilmGrainColourPrimaries = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8);
			ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4FilmGrainTransferCharacteristics = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8);
			ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4FilmGrainMatrixCoefficients = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8);
		}
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4BlendingModeId = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 2);
		#ifdef DEBUG_LOG
		UTIL_Printf("blending_mode_id = %d\n", ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4BlendingModeId);
		#endif
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4Log2ScaleFactor = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 4);
		#ifdef DEBUG_LOG
		UTIL_Printf("log2_scale_factor = %d\n", ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4Log2ScaleFactor);
		#endif
		for(c=0; c<3; c++)
		{
			ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgCompModelPresentFlag[c] = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4VDecID);
			#ifdef DEBUG_LOG
		UTIL_Printf("comp_model_present_flag[%d] = %d\n", c, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgCompModelPresentFlag[c] );
		#endif
		}
		for(c=0; c<3; c++)
		{
			if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgCompModelPresentFlag[c])
			{
				ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumIntensityIntervalsMinus1[c] = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8);
				#ifdef DEBUG_LOG
				UTIL_Printf("num_intensity_intervals_minus1[%d] = %d\n", c, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumIntensityIntervalsMinus1[c] );
				#endif
				ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumModelValuesMinus1[c] = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 3);
				#ifdef DEBUG_LOG
				UTIL_Printf("num_model_values_minus1[%d] = %d\n", c, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumModelValuesMinus1[c] );
				#endif
				for(i=0; i<256; i++) // Initialize
				{
				  	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue[(c << 9) + (i << 1)] = 0;
				  	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue[(c << 9) + (i << 1) + 1] = 0;
				}
				for(i=0; i<=ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumIntensityIntervalsMinus1[c]; i++)        
				{
				 	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalLowerBound[c][i] = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8);
					#ifdef DEBUG_LOG
				  UTIL_Printf("intensity_interval_lower_bound[%d][%d] = %d\n", c, i, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalLowerBound[c][i]);
				  #endif
				  	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalUpperBound[c][i] = u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, 8);
				  #ifdef DEBUG_LOG
				 UTIL_Printf("intensity_interval_upper_bound[%d][%d] = %d\n", c, i, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalUpperBound[c][i]);
				  #endif
				  	for(j=0; j<=ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumModelValuesMinus1[c]; j++)        //0,1,2
				  	{
					    u4Temp = i4VDEC_HAL_H264_SeCodeNum(_u4BSID[u4InstID], u4VDecID);
						#ifdef DEBUG_LOG
					    UTIL_Printf("comp_model_value[%d][%d][%d] = %d\n", c, i, j, u4Temp);
					    #endif
					    if(j == 0)
					    {
					      	for(k=ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalLowerBound[c][i]; k<=ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalUpperBound[c][i]; k++)
					      	{
					        	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue[(c << 9) + (k << 1)] = (u4Temp & 0xff);
					      	}
					    }
				    	else if(j == 1)
				    	{
				      		for(k=ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalLowerBound[c][i]; k<=ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalUpperBound[c][i]; k++)
				      		{
				        		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue[(c << 9) + (k << 1) + 1] = (ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue[(c << 9) + (k << 1) + 1] & 0xf0) + (u4Temp & 0xf);
				      		}
				    	}
				    	else if(j == 2)
				    	{
				      		for(k=ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalLowerBound[c][i]; k<=ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4IntensityIntervalUpperBound[c][i]; k++)
				      		{
				        		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue[(c << 9) + (k << 1) + 1] = (ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue[(c << 9) + (k << 1) + 1] & 0xf) + ((u4Temp & 0xf) << 4);
				      		}
				    	}
				  }
				}
			}      
		}
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4FilmGrainCharacteristicsRepetitionPeriod = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);      
		#ifdef DEBUG_LOG
	UTIL_Printf("film_grain_characteristics_repetition_period = %d\n", ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4FilmGrainCharacteristicsRepetitionPeriod);
	#endif
	}
}


// *********************************************************************
// Function    : void vInterpretBufferingPeriodInfo(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm)
// Description : SEI Film Grain parameter set header
// Parameter   : VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm
// Return      : None
// *********************************************************************
void vInterpretBufferingPeriodInfo(UINT32 u4InstID, UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm)
{
	UINT32 u4SPSID;
	//UINT32 u4InitCpbRemovalDelay;
	//UINT32 u4InitCpbRemovalDelayOffset;
	INT32 k;
	if(u4InstID == VDEC_LAE0)
    {
        _u4BSID[u4InstID] = 0;
    }
	u4SPSID = u4VDEC_HAL_H264_UeCodeNum(_u4BSID[u4InstID], u4VDecID);
	if(_rH264SPS[u4InstID][u4SPSID].fgSPSValid)
	{
		if(( (!ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr)
		  || !ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgNoOutputOfPriorPicsFlag) 
		&&  (ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS != (&_rH264SPS[u4InstID][u4SPSID])))
		{
			vVerifyFlushBufRefInfo(u4InstID);
		}
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS = &_rH264SPS[u4InstID][u4SPSID];
		if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgVuiParametersPresentFlag)
		{
			if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->rVUI.fgNalHrdParametersPresentFlag)
			{
				for (k=0; k<ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->rVUI.tNalHrdParameters.u4CpbCntMinus1+1; k++)
				{
				  /*u4InitCpbRemovalDelay        =*/ u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->rVUI.tNalHrdParameters.u4InitialCpbRemovalDelayLengthMinus1 + 1);
				  /*u4InitCpbRemovalDelayOffset =*/ u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->rVUI.tNalHrdParameters.u4InitialCpbRemovalDelayLengthMinus1 + 1);
				}
			}

			if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->rVUI.fgVclHrdParametersPresentFlag)
			{
				for (k=0; k<ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->rVUI.tVclHrdParameters.u4CpbCntMinus1+1; k++)
				{
				  /*u4InitCpbRemovalDelay        =*/ u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->rVUI.tVclHrdParameters.u4InitialCpbRemovalDelayLengthMinus1 + 1);
				  /*u4InitCpbRemovalDelayOffset =*/ u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4VDecID, ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->rVUI.tVclHrdParameters.u4InitialCpbRemovalDelayLengthMinus1 + 1);
				}
			}
		}
	}
}


#if  defined(MVC_VFY_SUPPORT) 
// *********************************************************************
// Function    : void vSetupInterViewRefPicList(UINT32 u4InstID, UINT32 u4BaseViewDpbId, VDEC_INFO_H264_FBUF_INFO_T* ptInterViewDpbInfo, UCHAR ucRefListId, UINT32 *pu4RefIdx)
// Description : Setup Ref Pic List
// Parameter   : None
// Return      : None
// *********************************************************************
void vSetupInterViewRefPicList(UINT32 u4InstID, UINT32 u4BaseViewDpbId, VDEC_INFO_H264_FBUF_INFO_T* ptInterViewDpbInfo, UCHAR ucRefListId, UINT32 *pu4RefIdx)
{
	VDEC_INFO_H264_DEC_PRM_T *prVDecH264DecPrm;
	VDEC_INFO_H264_P_REF_PRM_T *prPRefPicListInfo;

	prVDecH264DecPrm = &_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm;

	prPRefPicListInfo = &_arPRefPicListInfo[u4InstID];

	if(u4BaseViewDpbId >= 17)
	{
		return;
	}
	prPRefPicListInfo->ucFBufIdx = u4BaseViewDpbId;
	prPRefPicListInfo->u4FBufInfo = _tVerMpvDecPrm[u4InstID].ucPicStruct + (pu4RefIdx[0] << 16);
	prPRefPicListInfo->i4TFldPOC = ptInterViewDpbInfo->i4TFldPOC;
	prPRefPicListInfo->i4BFldPOC = ptInterViewDpbInfo->i4BFldPOC;
	prPRefPicListInfo->u4TFldPara = ptInterViewDpbInfo->u4TFldPara;
	prPRefPicListInfo->u4BFldPara = ptInterViewDpbInfo->u4BFldPara;
	prPRefPicListInfo->u4FBufYStartAddr = ptInterViewDpbInfo->u4YStartAddr;
	prPRefPicListInfo->u4FBufCAddrOffset = ptInterViewDpbInfo->u4CAddrOffset;
	prPRefPicListInfo->u4FBufMvStartAddr = ptInterViewDpbInfo->u4MvStartAddr;
	prPRefPicListInfo->u4ViewId = ptInterViewDpbInfo->u4ViewId;
	if(ucRefListId == 0)
	{
		vVDEC_HAL_H264_SetInterViewPRefPicListReg(u4InstID, prPRefPicListInfo);
		pu4RefIdx[0] ++;
	}
	else if(ucRefListId == 1)
	{
		vVDEC_HAL_H264_SetInterViewB0RefPicListReg(u4InstID, prPRefPicListInfo);
	}
	else if(ucRefListId == 2)
	{
		vVDEC_HAL_H264_SetInterViewB1RefPicListReg(u4InstID, prPRefPicListInfo);
		pu4RefIdx[0] ++;
	}
}


// *********************************************************************
// Function    : void vAppendInterviewRefPicList(UINT32 u4InstID,  UINT32* pu4RefIdx, UINT32 u4PicListIdx)
// Description : Setup Ref Pic List
// Parameter   : None
// Return      : None
// *********************************************************************
void vAppendInterviewRefPicList(UINT32 u4InstID,  UINT32* pu4RefIdx, UINT32 u4PicListIdx)
{
	VDEC_INFO_H264_DEC_PRM_T *prVDecH264DecPrm;
	INT32 i4Idx;
	UINT32 u4VOIdx;
	UCHAR ucNumRefListNum;
	VDEC_INFO_H264_LAST_INFO_T *prBaseLastInfo;
	VDEC_INFO_H264_FBUF_INFO_T *prBaseLastDpbInfo;

	prVDecH264DecPrm = &_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm;

//u4PicListIdx=0, means P0, 1, means B0, 2, means P1
	u4VOIdx = MAX_MVC_VIEW_ID;
	for(i4Idx=prVDecH264DecPrm->prSPS->rMvcSPS.ucNumViewsMinus1; i4Idx>=0; i4Idx--)
	{
		if(prVDecH264DecPrm->prSPS->rMvcSPS.aucViewId[i4Idx] == prVDecH264DecPrm->rMvcExtInfo.u4ViewId)
		{
		    u4VOIdx = i4Idx;
		    break;
		}
	}

	prBaseLastInfo = &(_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.rLastInfo);
	if((u4VOIdx < MAX_MVC_VIEW_ID) && (prBaseLastInfo->ucLastDpbId < 17))
	{
		ucNumRefListNum = 0;
		//prBaseLastDpbInfo = &(((H264_DRV_INFO_T *)(VDecGetViewEsInfo(BASE_VIEW_ID)->prVDecDrvInfo))->arH264FbInfo[prBaseLastInfo->ucLastDpbId]);
		prBaseLastDpbInfo = &_rH264PrevFbInfo[0];
		if(prVDecH264DecPrm->rMvcExtInfo.fgAnchorPicFlag)
		{
		    if(((u4PicListIdx == 0) || (u4PicListIdx == 1)) && prVDecH264DecPrm->prSPS->rMvcSPS.aucNumAnchorRefsL0[u4VOIdx])
		    {
		        ucNumRefListNum = prVDecH264DecPrm->prSPS->rMvcSPS.aucNumAnchorRefsL0[u4VOIdx];
		    }
		    else if((u4PicListIdx == 2) && prVDecH264DecPrm->prSPS->rMvcSPS.aucNumAnchorRefsL1[u4VOIdx])
		    {
		        ucNumRefListNum = prVDecH264DecPrm->prSPS->rMvcSPS.aucNumAnchorRefsL1[u4VOIdx];
		    }
		    
		    if(ucNumRefListNum)
		    {
		        for(i4Idx=0; i4Idx< ucNumRefListNum; i4Idx++)
		        {
		            if((((u4PicListIdx == 0) || (u4PicListIdx == 1)) && (prBaseLastInfo->u4LastViewId == prVDecH264DecPrm->prSPS->rMvcSPS.aucAnchorRefL0[u4VOIdx][i4Idx]))
		                || ((u4PicListIdx == 2) && (prBaseLastInfo->u4LastViewId  == prVDecH264DecPrm->prSPS->rMvcSPS.aucAnchorRefL1[u4VOIdx][i4Idx])))
		            {
		                // Set this one to ref pic list
		                vSetupInterViewRefPicList(u4InstID, prBaseLastInfo->ucLastDpbId, prBaseLastDpbInfo, u4PicListIdx, pu4RefIdx);
		                break;
		            }
		        }
		    }
		}
		else // !fgAnchorPicFlag
		{
		    if(((u4PicListIdx == 0) || (u4PicListIdx == 1)) && prVDecH264DecPrm->prSPS->rMvcSPS.aucNumNonAnchorRefsL0[u4VOIdx])
		    {
		        ucNumRefListNum = prVDecH264DecPrm->prSPS->rMvcSPS.aucNumNonAnchorRefsL0[u4VOIdx];
		    }
		    else if((u4PicListIdx == 2) && prVDecH264DecPrm->prSPS->rMvcSPS.aucNumNonAnchorRefsL1[u4VOIdx])
		    {
		        ucNumRefListNum = prVDecH264DecPrm->prSPS->rMvcSPS.aucNumNonAnchorRefsL1[u4VOIdx];
		    }
		    
		    if(ucNumRefListNum)
		    {
		        for(i4Idx=0; i4Idx< ucNumRefListNum; i4Idx++)
		        {
		            if((((u4PicListIdx == 0) || (u4PicListIdx == 1)) && (prBaseLastInfo->u4LastViewId  == prVDecH264DecPrm->prSPS->rMvcSPS.aucNonAnchorRefL0[u4VOIdx][i4Idx]))
		                || ((u4PicListIdx == 2) && (prBaseLastInfo->u4LastViewId  == prVDecH264DecPrm->prSPS->rMvcSPS.aucNonAnchorRefL1[u4VOIdx][i4Idx])))
		            {
		                // Set this one to ref pic list
		                vSetupInterViewRefPicList(u4InstID, prBaseLastInfo->ucLastDpbId, prBaseLastDpbInfo, u4PicListIdx, pu4RefIdx);
		                break;
		            }
		        }
		    }
		}  
	}

//    if (_u4FileCnt[1] >0x09 && (_ucMVCType[u4InstID] == 2) && (_tVerMpvDecPrm[1].SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgNonIdrFlag == 0))
	if ((_ucMVCType[u4InstID] == 2) && (_tVerMpvDecPrm[1].SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgNonIdrFlag == 0))
	{
		if ((_tVerMpvDecPrm[u4InstID].ucPicStruct == BOTTOM_FIELD) ||(_tVerMpvDecPrm[u4InstID].ucPicStruct == TOP_FIELD))
		{
			for(i4Idx=pu4RefIdx[0]; i4Idx< _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum*2; i4Idx++)
		  	{
		    // Set this one to ref pic list
		    	vSetupInterViewRefPicList(u4InstID, prBaseLastInfo->ucLastDpbId, prBaseLastDpbInfo, u4PicListIdx, pu4RefIdx);
		  	}
		}
		else
		{
			for(i4Idx=pu4RefIdx[0]; i4Idx< _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i4Idx++)
		  	{
		    // Set this one to ref pic list
		    	vSetupInterViewRefPicList(u4InstID, prBaseLastInfo->ucLastDpbId, prBaseLastDpbInfo, u4PicListIdx, pu4RefIdx);
		  	}
		}
	}
}


// *********************************************************************
// Function    : void vPrefix_Nal_Unit_Rbsp(UINT32 u4BSID, UINT32 u4VDecID, H264_DRV_INFO_T *prH264DrvDecInfo)
// Description : Handle picture parameter set header
// Parameter   : None
// Return      : None
// *********************************************************************
void vPrefix_Nal_Unit_Rbsp_Verify(UINT32 u4InstID)
{
	VDEC_INFO_DEC_PRM_T  *ptVerMpvDecPrm;

	_u4BSID[u4InstID] = 0;

	ptVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];
	ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgSvcExtensionFlag = bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4InstID);

	if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgSvcExtensionFlag) // SVC
	{
	// do nothing
		vVDEC_HAL_H264_TrailingBits(_u4BSID[u4InstID], u4InstID);
	}
	else // MVC
	{
#if MVC_PATCH_1               
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgIdrFlag =  bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4InstID);
#else              
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgNonIdrFlag =  bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4InstID);
#endif
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.ucPriorityId =  u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4InstID, 6);
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.u4ViewId =  u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4InstID, 10);
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.ucTemporalId =  u4VDEC_HAL_H264_GetRealBitStream(_u4BSID[u4InstID], u4InstID, 3);
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgAnchorPicFlag =  bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4InstID);
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgInterViewFlag =  bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4InstID);
		ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.fgReservedOneBit =  bVDEC_HAL_H264_GetBitStreamFlg(_u4BSID[u4InstID], u4InstID);
	}
}

#define H264PsrRangeChk(u4ChkRange, u4MaxRange) if(u4ChkRange > u4MaxRange) VDEC_ASSERT(0);
// *********************************************************************
// Function    : void vPic_Par_Set_Rbsp(void)
// Description : Handle picture parameter set header
// Parameter   : None
// Return      : None
// *********************************************************************
void vSubset_Seq_Parameter_Set_Rbsp_Verify(UINT32 u4VDecID)
{
	VDEC_INFO_DEC_PRM_T  *ptVerMpvDecPrm;
	VDEC_INFO_H264_SPS_T *prSPS;

	BOOL fgAdditionalExtension2Flag;
	UINT32 u4I;
	UINT32 u4J;
	UINT32 u4K;
	UCHAR u4BSID;

	_u4BSID[u4VDecID] = 0;
	u4BSID = _u4BSID[u4VDecID];
	ptVerMpvDecPrm = &_tVerMpvDecPrm[u4VDecID];

	vVerifySeq_Par_Set_Rbsp(u4VDecID, u4VDecID);
	if(ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.ucLastSPSId < 32)
	{
		prSPS = &_rH264SPS[u4VDecID][ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.ucLastSPSId];
		if((prSPS->u4ProfileIdc == 118) || (prSPS->u4ProfileIdc == 128))
		{
		    prSPS->fgBitEqualToOne =  bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		    
		    //Seq_ParameterSet_Mvc_Extension            
		    prSPS->rMvcSPS.ucNumViewsMinus1 = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		    H264PsrRangeChk(prSPS->rMvcSPS.ucNumViewsMinus1, MAX_MVC_VIEW_ID);
		    for(u4I=0; u4I<=prSPS->rMvcSPS.ucNumViewsMinus1; u4I++)
		    {
		        prSPS->rMvcSPS.aucViewId[u4I] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		    }
		    for(u4I=1; u4I<=prSPS->rMvcSPS.ucNumViewsMinus1; u4I++)
		    {
		        prSPS->rMvcSPS.aucNumAnchorRefsL0[u4I] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        H264PsrRangeChk(prSPS->rMvcSPS.aucNumAnchorRefsL0[u4I], MAX_MVC_REF_FRM_NUM);
		        for(u4J=0; u4J<prSPS->rMvcSPS.aucNumAnchorRefsL0[u4I]; u4J++)
		        {
		            prSPS->rMvcSPS.aucAnchorRefL0[u4I][u4J] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        }
		        prSPS->rMvcSPS.aucNumAnchorRefsL1[u4I] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        H264PsrRangeChk(prSPS->rMvcSPS.aucNumAnchorRefsL1[u4I], MAX_MVC_REF_FRM_NUM);
		        for(u4J=0; u4J<prSPS->rMvcSPS.aucNumAnchorRefsL1[u4I]; u4J++)
		        {
		            prSPS->rMvcSPS.aucAnchorRefL1[u4I][u4J] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        }
		    }
		    for(u4I=1; u4I<=prSPS->rMvcSPS.ucNumViewsMinus1; u4I++)
		    {
		        prSPS->rMvcSPS.aucNumNonAnchorRefsL0[u4I] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        H264PsrRangeChk(prSPS->rMvcSPS.aucNumNonAnchorRefsL0[u4I], MAX_MVC_REF_FRM_NUM);
		        for(u4J=0; u4J<prSPS->rMvcSPS.aucNumNonAnchorRefsL0[u4I]; u4J++)
		        {
		            prSPS->rMvcSPS.aucNonAnchorRefL0[u4I][u4J] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        }
		        prSPS->rMvcSPS.aucNumNonAnchorRefsL1[u4I] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        H264PsrRangeChk(prSPS->rMvcSPS.aucNumNonAnchorRefsL1[u4I], MAX_MVC_REF_FRM_NUM);
		        for(u4J=0; u4J<prSPS->rMvcSPS.aucNumNonAnchorRefsL1[u4I]; u4J++)
		        {
		            prSPS->rMvcSPS.aucNonAnchorRefL1[u4I][u4J] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        }
		    }
		    prSPS->rMvcSPS.ucNumLevelValuesSignalledMinus1 = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		    H264PsrRangeChk(prSPS->rMvcSPS.aucNumNonAnchorRefsL1[u4I], MAX_MVC_VIEW_ID);
		    for(u4I=0; u4I<=prSPS->rMvcSPS.ucNumLevelValuesSignalledMinus1; u4I++)
		    {
		        prSPS->rMvcSPS.aucLevelIdc[u4I] = u4VDEC_HAL_H264_GetRealBitStream(u4BSID, u4VDecID, 8);
		        prSPS->rMvcSPS.au2NumApplicableOpsMinus1[u4I] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        for(u4J=0; u4J<=prSPS->rMvcSPS.au2NumApplicableOpsMinus1[u4I]; u4J++)
		        {
		            prSPS->rMvcSPS.aucApplicableOpTemporalId[u4I][u4J] = u4VDEC_HAL_H264_GetRealBitStream(u4BSID, u4VDecID, 3);
		            prSPS->rMvcSPS.au2ApplicableOpNumTargetViewsMinus1[u4I][u4J] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		            H264PsrRangeChk(prSPS->rMvcSPS.aucNumNonAnchorRefsL1[u4I], MAX_MVC_APPICABLE_OP_NUM);
		            for(u4K=0; u4K<=prSPS->rMvcSPS.au2ApplicableOpNumTargetViewsMinus1[u4I][u4J]; u4K++)
		            {
		                prSPS->rMvcSPS.au2ApplicableOpTargetViewsId[u4I][u4J][u4K] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		            }
		            prSPS->rMvcSPS.au2ApplicableOpNumViewsMinus1[u4I][u4J] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        }
		    }

		    prSPS->fgMvcVuiParametersPresentFlag = bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		    if(prSPS->fgMvcVuiParametersPresentFlag)
		    {
		        //mvc_vui_parameters_Extension
		        prSPS->rMvcVUI.u4NumOpsMinus1 = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		        H264PsrRangeChk(prSPS->rMvcSPS.aucNumNonAnchorRefsL1[u4I], MAX_MVC_APPICABLE_OP_NUM);
		        for(u4I=0; u4I<=prSPS->rMvcVUI.u4NumOpsMinus1; u4I++)
		        {
		            prSPS->rMvcVUI.ucTemporalId[u4I] = u4VDEC_HAL_H264_GetRealBitStream(u4BSID, u4VDecID, 3);
		            prSPS->rMvcVUI.ucNumTargetOutputViewsMinus1[u4I] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);
		            for(u4J=0; u4J<=prSPS->rMvcSPS.au2NumApplicableOpsMinus1[u4I]; u4J++)
		            {
		                prSPS->rMvcVUI.aucViewId[u4I][u4J] = u4VDEC_HAL_H264_UeCodeNum(u4BSID, u4VDecID);                        
		            }                    
		            prSPS->rMvcVUI.fgTimingInfoPresentFlag[u4I] = bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		            if(prSPS->rMvcVUI.fgTimingInfoPresentFlag[u4I] )
		            {
		                prSPS->rMvcVUI.u4NumUnitsInTick[u4I] = u4VDEC_HAL_H264_GetRealBitStream(u4BSID, u4VDecID, 32);
		                prSPS->rMvcVUI.u4TimeScale[u4I] = u4VDEC_HAL_H264_GetRealBitStream(u4BSID, u4VDecID, 32);
		                prSPS->rMvcVUI.fgFixedFrameRateFlag[u4I] = bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		            }
		            prSPS->rMvcVUI.fgNalHrdParametersPresentFlag[u4I] = bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		            if(prSPS->rMvcVUI.fgNalHrdParametersPresentFlag[u4I])
		            {
		                vVerifyHrdParameters(u4VDecID, u4VDecID, &prSPS->rVUI.tNalHrdParameters);
		            }
		            prSPS->rMvcVUI.fgVclHrdParametersPresentFlag[u4I] = bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		            if(prSPS->rMvcVUI.fgVclHrdParametersPresentFlag[u4I])
		            {
		                vVerifyHrdParameters(u4VDecID, u4VDecID, &prSPS->rVUI.tNalHrdParameters);
		            }
		            if(prSPS->rMvcVUI.fgNalHrdParametersPresentFlag[u4I] || prSPS->rMvcVUI.fgVclHrdParametersPresentFlag[u4I])
		            {
		                prSPS->rMvcVUI.fgLowDelayHrdFlag[u4I] = bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		            }
		            prSPS->rMvcVUI.fgPicStructPresetFlag[u4I] = bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		        }
		    }
		    
		    fgAdditionalExtension2Flag = bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		    if(fgAdditionalExtension2Flag)
		    {
		        while(bVDEC_HAL_H264_IsMoreRbspData(u4BSID, u4VDecID))
		        {
		            fgAdditionalExtension2Flag = bVDEC_HAL_H264_GetBitStreamFlg(u4BSID, u4VDecID);
		        }
		    }
		}
	}
	vVDEC_HAL_H264_TrailingBits(u4BSID, u4VDecID);
}
#endif

