/* ========================================================================== */
/*                                                                            */
/*   opt_bl_ctrl.h                                                               */
/*   (c) 2009 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#ifndef _OPT_BL_CTRL_H_
#define _OPT_BL_CTRL_H_

extern UINT8 calc_backlight_ctrl_opt_bl_iir( void );
extern void calc_backlight_ctrl_opt_sensor_init_param( void );
extern UINT8 calc_backlight_ctrl_opt_bl_coef( void );
extern void opt_bl_ctrl_main(void);


#endif /* _OPT_BL_CTRL_H_ */
