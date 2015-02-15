
/******************************************************************************
*[File]				pq_gamma.h
*[Version]			v0.1
*[Revision Date]	2009-07-01
*[Author]			cheah
*[Description]
*    Header file of Sony PQ Gamma
*[Copyright]
*    Copyright (C) 2009 Sony All Rights Reserved.
******************************************************************************/

#ifndef PQ_GAMMA_H
#define PQ_GAMMA_H


/*-----------------------------------------------------------------------------
	include files
-----------------------------------------------------------------------------*/
#include "../sony_drv_inc/pq_gamma_if.h" /* header for sony external src */


/*-----------------------------------------------------------------------------
	Defines
-----------------------------------------------------------------------------*/
//#define PQ_DBG_ON 		/* general PQ debug log */
//#define PQ_DBG_GAMMA_ON	/* gamma interpolation debug log (10->256->1024) */

#ifdef PQ_DBG_ON
	#include "../x_inc/x_os.h"
	extern INT32 x_dbg_stmt(const CHAR*  ps_format, ...);
	
	#define PQ_DBG(x...)	x_dbg_stmt(x);
	
	/* debug log with time-stamp */
	#define PQ_DBGT(x...)	pq_GetTimeReference();	\
							x_dbg_stmt(x);

	#ifdef PQ_DBG_GAMMA_ON
	#define PQ_DBG_GAMMA(x...) PQ_DBG(x)
	#else
	#define PQ_DBG_GAMMA(x...)
	#endif

#else
	#define PQ_DBG(x...)
	#define PQ_DBGT(x...)
	#define PQ_DBG_GAMMA(x...)
#endif /* PQ_DBG_ON */


#ifdef CC_SONY_GAMMA
	#define SONYSET(x)	(x);
	#define SONYGET(x)	(x);
#else
	#define SONYSET(x)
	#define SONYGET(x)	(0);
#endif


#define BOUND_MIN(x,min)	(((x) < (min)) ? (min) : (x))
#define BOUND_MAX(x,max)	(((x) > (max)) ? (max) : (x))
#define BOUND(x,min,max)	(((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))


/* bit field defines */
#define b0		0x0001
#define b1		0x0002
#define b2		0x0004
#define b3		0x0008
#define b4		0x0010
#define b5		0x0020
#define b6		0x0040
#define b7		0x0080
#define b8		0x0100
#define b9		0x0200
#define b10		0x0400
#define b11		0x0800
#define b12		0x1000
#define b13		0x2000
#define b14		0x4000
#define b15		0x8000

/* byte field defines */
#define BYTE_LO 0x00FF
#define BYTE_HI 0xFF00


/*-----------------------------------------------------------------------------
	Enum, typedef 
-----------------------------------------------------------------------------*/
#ifdef CC_SONY_GAMMA

typedef enum{
	LUMA_LVL_0,
	LUMA_LVL_1,
	LUMA_LVL_2,
	LUMA_LVL_3,
	LUMA_LVL_4,
	LUMA_LVL_5,
	LUMA_LVL_6,
	LUMA_LVL_7,
	LUMA_LVL_8,
	LUMA_LVL_9,
	LUMA_LVL_10,
	NUM_OF_LUMA_LVL
}Luma_Lvl_t;

typedef enum{
	DELTA_GAIN,
	FINAL_GAIN,
	NUM_OF_AUTO_GAIN
}Auto_Gain_t;

typedef struct Luma_RGB_T
{
	//UINT16 idx;	//for debug use only
	UINT16 Luma_R, Luma_G, Luma_B;
} Luma_RGB_T;

typedef struct WBS_NVM_ENV_T
{
    WB_Index_t CLRIDX;
	WB_Lvl_t   WBLVL;
	Gamma_RGB_t GAMMA;
} WBS_NVM_ENV_T;

/*-----------------------------------------------------------------------------
	Structures 
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
	Public functions prototype   (for Sony internal src)
-----------------------------------------------------------------------------*/

#ifdef PQ_DBG_ON
void pq_GetTimeReference(void);		/* debug log with time stamp */
void Sony_ListPQNVM(void);			/* Sony PQ-NVM allocation check */
#endif

/* -- E C S (no callback)------------------- */

/* Gamma */
extern void Sony_GammaSelectLvl (UINT16 write_data);
extern void Sony_GammaSetLvl (Gamma_RGB_t Gamma, Luma_Lvl_t level, UINT16 write_data);
extern void Sony_GammaPatternRefresh(Luma_RGB_T* Luma_ptr);
extern BOOL Sony_GammaSetup_Read (void);
extern Luma_Lvl_t Sony_GammaSelectLvl_Read (void);
extern UINT16 Sony_GammaReadLvl (Gamma_RGB_t Gamma, Luma_Lvl_t level);
extern Luma_RGB_T Sony_GammaPatternRGB_Read (void);


/* for design evaluation only */
extern UINT16 Sony_GammaGetSampleIndex (void);
extern UINT16 Sony_GammaGetSample (Gamma_RGB_t Gamma);

/* ------------------------------- */

/* White balance */
extern void Sony_WBSetData (WB_Index_t color_idx, WB_Lvl_t wb_lvl, Gamma_RGB_t gamma, UINT16 write_data);
extern BOOL Sony_WBSetup_Read (void);
extern WB_Lvl_t Sony_WBReadLvl (void);
extern WB_Index_t Sony_WBReadColor (void);
extern UINT16 Sony_WBReadData (WB_Index_t color_idx, WB_Lvl_t wb_lvl, Gamma_RGB_t gamma);
/* ------------------------------- */

/* Auto White balance */
UINT16 Sony_AutoWB_GetGain (Auto_Gain_t gain, Gamma_RGB_t Gamma);


#endif /* CC_SONY_GAMMA */

#endif /* PQ_GAMMA_H */

