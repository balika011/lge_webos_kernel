
#ifndef TZMACRO_H
#define TZMACRO_H

#define MODE_MON            0x16
#define MODE_SVC            0x13
#define MODE_IRQ            0x12

//if you want enable tz on 4 core,please unmark below line
//#define TZ_SUPPORT_4_CORE

#if defined(CC_MT5882)
#define TZ_SUPPORT_4_CORE
#endif

#if defined(TZ_SUPPORT_4_CORE)
#define CPU_NUMS            4
#define N_THREADS           8
#else
#define CPU_NUMS            2
#define N_THREADS           4
#endif

#define N_DRIVERS           1

#define STACK_SIZE          0x2000
#define STACK_SIZE_SHIFT    13

// stack checkpoint
#define TZ_CKPT1            0xAAAAAAAA
#define TZ_CKPT2            0xBBBBBBBB

#endif  /* TZMACRO_H */
