/* ========================================================================== */
/*                                                                            */
/*   bl_ctrl.h                                                              */
/*   (c) 2001 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

#ifndef _BACKLIGHT_CTRL_H_
#define _BACKLIGHT_CTRL_H_

#define ECO_NORMAL          0
#define ECO_DECREASE1       1
#define ECO_DECREASE2       2
#define ECO_BLACKOUT        3
#define ECO_PICTUREFRAME    4

typedef struct _DYN_BL_MEMBER_T
{
    HANDLE_T    h_dynBlCtrlThread;
    HANDLE_T    h_dynBlTimer;
    HANDLE_T    h_dynBlThreadSema;  
    HANDLE_T    h_blSregMutex;
    
    UINT8 dbl_event_ui_change;
    UINT8 dbl_event_timer_timeout;
    UINT8 *userBackLight;      // MTK UserBackLight Level
    UINT8 *ecoMode;            // MTK EcoMode
    UINT16 temperature;
}DYN_BL_MEMBER_T;

extern UINT8 vAPIGetDimmingValue(void);
extern void vAPIBacklightRegSet(void);
extern DYN_BL_MEMBER_T* t_dyn_bl_ext(void);
extern UINT8 vAPIGetPowerSavingStatus(void);

#endif /* _BACKLIGHT_CTRL_H_ */
