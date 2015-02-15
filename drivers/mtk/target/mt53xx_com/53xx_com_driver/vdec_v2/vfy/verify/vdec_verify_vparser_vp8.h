#ifndef _VDEC_VERIFY_VPARSER_VP8_H_
#define _VDEC_VERIFY_VPARSER_VP8_H_

#include "x_typedef.h"

void vVerInitVP8(UINT32 u4InstID);
UINT32 u4VerVParserVP8(UINT32 u4InstID, BOOL fgInquiry);
void vVerifyVDecSetVP8Info(UINT32 u4InstID);
void vVerVP8VDecEnd(UINT32 u4InstID);
void vVerVP8DecEndProc(UINT32 u4InstID);

#endif // _PR_EMU_H_

