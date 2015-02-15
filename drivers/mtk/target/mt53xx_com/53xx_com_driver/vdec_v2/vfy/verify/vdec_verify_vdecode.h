#ifndef _VDEC_VERIFY_VDECODE_H_
#define _VDEC_VERIFY_VDECODE_H_

#include "vdec_info_common.h"


void vMpvPlay(UINT32 u4InstID,UCHAR ucVldID);
void vVerifyVDecIsrInit(UINT32 u4InstID);
void vVerifyVDecIsrStop(UINT32 u4InstID);
void vVDecInit(UINT32 u4InstID);
void vVDecDeInit(UINT32 u4InstID);
extern void vDrmaBusySet(UINT32  u4InstID);
UCHAR ucVDecAllocateVld(UINT32 u4InstID);


#endif

