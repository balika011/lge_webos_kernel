/* ========================================================================== */
/*                                                                            */
/*   bl_util.h                                                                 */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

#ifndef BL_UTIL_H
#define BL_UTIL_H

#include "x_typedef.h"

#define IIR_COEF_INPUT_THROUGH			255
#define IIR_NO_EFFECT_COEF				IIR_COEF_INPUT_THROUGH 

#define BOUND(x,min,max)	( ((x) > (max)) ? (max) : ( ((x) < (min)) ? (min) : (x) ) )
#define BOUND_MAX(x,max)	( ((x) > (max)) ? (max) : (x) )
#define BOUND_MIN(x,min)	( ((x) < (min)) ? (min) : (x) )

/*****************************************************************************/

typedef struct{
	INT32 th1;
	INT32 th2;
	INT32 th3;
	INT32 th4;
}ST_PARAM_UTIL_MATH_STEP_FUNC_TH;
 
typedef struct{
	INT32  l;
	INT32  m;
	INT32  h;
}ST_PARAM_UTIL_MATH_STEP_FUNC_OFST;

/*****************************************************************************/

extern INT32 util_math_step_func( const INT32 status,
								  const ST_PARAM_UTIL_MATH_STEP_FUNC_TH *th,
								  const ST_PARAM_UTIL_MATH_STEP_FUNC_OFST *ofst );
								  
extern UINT16 util_math_iir_high_precision_ex( INT16 a_n, UINT16 x_n, UINT32 *yh_n );

extern UINT32 util_math_iir( const UINT8 a_n, const UINT32 x_n, const UINT32 y_n_1 );

#endif
