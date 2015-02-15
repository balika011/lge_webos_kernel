/* ========================================================================== */
/*                                                                            */
/*   control.h                                                               */
/*   (c) 2001 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#ifndef _CTRL_H_
#define _CTRL_H_

typedef enum{
    DYN_BL_DISABLE,
    DYN_BL_ENABLE
}DBL_EN_CTRL;

extern void vAutoBacklightInit(void);
extern void vAutoBacklightISR(void);
extern UINT32 vAutoBacklightProc(void);
extern void vDynBacklightCtrlEnable(DBL_EN_CTRL value);

#endif /* _CTRL_H_ */
