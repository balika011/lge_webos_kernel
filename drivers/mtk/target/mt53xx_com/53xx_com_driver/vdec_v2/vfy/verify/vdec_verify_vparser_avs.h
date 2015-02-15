#ifndef _VDEC_VERIFY_VPARSER_AVS_H_
#define _VDEC_VERIFY_VPARSER_AVS_H_

#include "x_typedef.h"

void vVerInitAVS(UINT32 u4InstID);
UINT32 u4VerVParserAVS(UINT32 u4InstID, BOOL fgInquiry);
void vVerifyVDecSetAVSInfo(UINT32 u4InstID);
void vVerAVSVDecEnd(UINT32 u4InstID);
void vVerAVSDecEndProc(UINT32 u4InstID);

#if AVS_CRC_COMPARE_SUPORT
void vAVS_CRCGoldenLoad(UINT32 u4InstID);
void vAVS_CheckCRCResult(UINT32 u4InstID, BOOL* fgNextFrameExist);
#endif
#endif // _PR_EMU_H_

