#ifndef _VDEC_VERIFY_VDEC_H_
#define _VDEC_VERIFY_VDEC_H_

#include "x_typedef.h"

extern void vVDecVerifyThread(void *param_array);
extern void vSecBSVerifyThread(void *param_array);
extern void vVDecClockSelect(void);
extern void vDrmaBusySet (UINT32  u4InstID);
extern void vDrmaBusyOff (UINT32  u4InstID);

#if defined(MVC_VFY_SUPPORT) 
#define fgIsDepView() (_ucNalUnitType == H264_SLICE_EXT)
#endif

#endif // _PR_EMU_H_

