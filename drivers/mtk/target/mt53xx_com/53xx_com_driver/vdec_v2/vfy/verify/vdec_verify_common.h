#ifndef _VDEC_VERIFY_COMMON_H_
#define _VDEC_VERIFY_COMMON_H_

#include "vdec_info_common.h"


void vOutputPOCData(UINT32 dwDecOrder);
UCHAR ucVDecGetMinPOCFBuf(UINT32 u4InstID,VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm, BOOL fgWithEmpty);
void vVerifyClrFBufInfo(UINT32 u4InstID, UINT32 u4FBufIdx);
void vFlushDPB(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm, BOOL fgWithOutput);


#endif


