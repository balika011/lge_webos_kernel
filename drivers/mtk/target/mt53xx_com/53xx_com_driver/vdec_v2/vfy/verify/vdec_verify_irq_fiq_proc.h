#ifndef _VDEC_VERIFY_IRQ_FIQ_PROC_H_
#define _VDEC_VERIFY_IRQ_FIQ_PROC_H_

#include "x_typedef.h"

void vVDec0IrqProc(UINT16 u2Vector);
void vVDec1IrqProc(UINT16 u2Vector);
void vVDecIOMMUIrqProc(UINT16 u2Vector);
void vVDecLae0IrqProc(UINT16 u2Vector);
#endif

