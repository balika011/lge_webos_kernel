#ifdef SYS_MHL_SUPPORT
#ifdef CC_MHL_HDMI_SWITCH

extern void vHDMIMHL_Init(void);


extern void vMhl_MainLoop(void);


extern void OsSleep(clock_time_t delay);

extern void EnterCriticalSection(void);

extern void LeaveCriticalSection(void);



#endif
#endif


