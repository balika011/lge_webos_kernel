/* ========================================================================== */
/*                                                                            */
/*   bl_util.c                                                                 */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "bl_util.h"
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define IIR_ADJUST_FOR_ACCURACY_64			64
#define IIR_ADJUST_FOR_ACCURACY_16			16
#define IIR_ADJUST_FOR_ACCURACY_256			256
#define IIR_ADJUST_FOR_ACCURACY_4096		4096

#define IIR_BASE_4096		4096

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static variables
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Global functions
//-----------------------------------------------------------------------------

INT32 util_math_step_func( const INT32 status,
						   const ST_PARAM_UTIL_MATH_STEP_FUNC_TH *th,
						   const ST_PARAM_UTIL_MATH_STEP_FUNC_OFST *ofst )
{
	INT32 ret;
	BOOL param_is_ok = (BOOL)( (th != NULL) && (ofst != NULL) &&  (th->th1 <= th->th2) && (th->th2 <= th->th3) && (th->th3 <= th->th4) );
	
	if( param_is_ok != (BOOL)FALSE ){
		if(status < th->th1){
			ret = ofst->l;
		}else if(status < th->th2){
			// min(ofst->l, ofst->m) < offset < max(ofst->l, ofst->m)
			// (th->th2 - th->th1) cannot be 0.
			ret = ofst->l +  ( ( (ofst->m - ofst->l) * (status - th->th1) ) / (th->th2 - th->th1) );
		}else if(status < th->th3){
			ret = ofst->m;
		}else if(status < th->th4){
			// min(ofst->m, ofst->h) < offset < max(ofst->m, ofst->h)
			// (th->th4 - th->th3) cannot be 0.
			ret = ofst->m +  ( ( (ofst->h - ofst->m) * (status - th->th3) ) / (th->th4 - th->th3) );
		}else{
			ret = ofst->h;
		}
	}else{
		ret = 0;
	}
	
	return ret;
}

/**
 *  SWToDo It's better to replace util_math_iir_high_precision() to this function.
 *  High-Precesion IIR Filter.(for 10 bit input) 
 *  basic function          : y_n  = ( (a_n + 1) * x_n            + (255 -a_n) * y_n-1            ) / 256
 *  high accuracy function  : yh_n = ( (a_n + 1) * x_n * (64*256) + (255 -a_n) * y_n-1 * (64*256) ) / 256
 *                          :      =   (a_n + 1) * x_n * 64       + (255 -a_n) * yh_n-1 / 256
 *                          : y_n  = yh_n / (64*256)
 *  @param  : a_n		: coefficient( must be -1~255 )
 *  @param  : x_n		: input ( must be 0~1023 )
 *  @param  : yh_n		: pointer to high-precision internal state of IIR. ( 0~1023 * 64 * 256 = 0~0x3ff << 14 = 24bit )
 *  @return : y_n		: current ( Internal state of IIR. 0~1023)
 *  @author : Tomokazu Ueyama
 */
UINT16 util_math_iir_high_precision_ex( INT16 a_n, UINT16 x_n, UINT32 *yh_n )
{
	x_n = BOUND_MAX( x_n, 1023 );
	a_n = BOUND( a_n, -1, 255 );
	*yh_n = ((UINT32)(a_n + 1) * (UINT32)x_n * IIR_ADJUST_FOR_ACCURACY_64) + (((UINT32)(255 - a_n) * *yh_n) >> 8);
	return (UINT16)( *yh_n / (IIR_ADJUST_FOR_ACCURACY_64 * IIR_ADJUST_FOR_ACCURACY_256 ) );
}

/**
 *  IIR Filter. If "x_n" or "y_n_1" is greater than 2^24, "y_n" may be overflow.
 *  y_n = ( (a_n + 1) * x_n + (255 -a_n) * y_n-1 ) / 256
 *  @param  : a_n		: coefficient
 *  @param  : x_n		: input
 *  @param  : y_n_1		: previous  ( Internal state of IIR. )
 *  @return : y_n		: current   ( Internal state of IIR. )
 *  @author : Kentaro Hashimoto
 */
UINT32 util_math_iir( const UINT8 a_n, const UINT32 x_n, const UINT32 y_n_1 )
{
	return ( ( ((UINT32)a_n + 1) * x_n ) + ( (255 - (UINT32)a_n) * y_n_1 ) ) >> 8 ;
}

