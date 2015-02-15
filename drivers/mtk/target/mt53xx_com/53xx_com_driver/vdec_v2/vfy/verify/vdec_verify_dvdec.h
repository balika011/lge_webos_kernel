#ifndef _VDEC_VERIFY_DV_H_
#define _VDEC_VERIFY_DV_H_

//#include "vdec_info_dv.h"

//extern TDvInfo        _tDvInfo_8520;

#define dUtPhyDramAddr(dwAddr)     (dwAddr)

//#define DV_UNKNOW	0xFE
#define DV_CMD_RESULT_DONEXT 0
#define DV_CMD_RESULT_DONEXT_BREAK 1

extern UINT32 dwDVDecodeDone_8520;
extern UINT32 dwDVDecodeTimeOut_8520;
extern UINT32 dwDVDecodeOther_8520;

extern void vInitDvDec_8520(UINT32 u4InstID);

#endif // _PR_EMU_H_
