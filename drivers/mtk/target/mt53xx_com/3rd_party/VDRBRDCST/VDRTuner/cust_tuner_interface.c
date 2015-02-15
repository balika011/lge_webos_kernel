/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Copyright (c) 2008, MediaTek, Inc.
 * All rights reserved.
 *
 * Unauthorized use, practice, perform, copy, distribution, reproduction,
 * or disclosure of this information in whole or in part is prohibited.
 *-----------------------------------------------------------------------------
 *
 * $Author: p4admin $
 * $Date: 2015/02/15 $
 * $RCSfile: tuner_interface.c,v $
 * $Revision: #1 $
 *
 *---------------------------------------------------------------------------*/

/** @file tuner_interface.c
 *  Implement primitive tuner driver interface.
 */

#include "tuner_interface_if.h"
#include "x_typedef.h"
#include "cust_tunerType.h"
#include "tunerDebug.h"

typedef struct 
{
    UINT8                id;
    ITUNER_FCT_T*       fct;
} CUST_TUNER_FCT_MAP_T;

#if CUST_TUNER_TYPE==CUST_TUNER_LG_TDVWH810F
EXTERN ITUNER_FCT_T piTunerFct_TDVWH810F ;
#endif

#if CUST_TUNER_TYPE==CUST_TUNER_SS_DTVS203
EXTERN ITUNER_FCT_T piTunerFct_DTVS203 ;//defined in pd_tuner_dtvs203.c
#endif


#if CUST_TUNER_TYPE==CUST_TUNER_SONY
EXTERN ITUNER_FCT_T piTunerFct_SONY ;//defined in pd_tuner_sony.c
#endif

#if (CUST_TUNER_TYPE==CUST_TUNER_NXP_TDA18273)||(CUST_TUNER_TYPE==CUST_TUNER_NXPLIST)
EXTERN ITUNER_FCT_T piTunerFct_TDA18273 ;
#endif

#if (CUST_TUNER_TYPE==CUST_TUNER_SS_DTVS20C)||(CUST_TUNER_TYPE==CUST_TUNER_NXPLIST)
EXTERN ITUNER_FCT_T piTunerFct_DTVS20C ;
#endif

#if (CUST_TUNER_TYPE==CUST_TUNER_SILICON_TUNERA)
EXTERN ITUNER_FCT_T piTunerFct_TUNERA ;
#endif

#if (CUST_TUNER_TYPE==CUST_TUNER_SHARP_VA1G2CD)
EXTERN ITUNER_FCT_T piTunerFct_VA1G2CD ;//defined in pd_tuner_va1g2cd.c
#endif

#if (CUST_TUNER_TYPE==CUST_TUNER_TCL_F20WT)
EXTERN ITUNER_FCT_T piTunerFct_F20WT ;
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SILABS_SI2158)
EXTERN ITUNER_FCT_T piTunerFct_SI2158 ;
#endif

#if (CUST_TUNER_TYPE==CUST_TUNER_SILABS_SI2157)
EXTERN ITUNER_FCT_T piTunerFct_SI2157 ;
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_Mxl_MXL601)
EXTERN ITUNER_FCT_T piTunerFct_MXL601 ;
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_Mxl_MXL661)
EXTERN ITUNER_FCT_T piTunerFct_MXL661 ;
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_NXP_TDA18274)
EXTERN ITUNER_FCT_T piTunerFct_TDA18274 ;
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SONY_RA217)
EXTERN ITUNER_FCT_T piTunerFct_RA217 ;
#endif

#if (CUST_TUNER_TYPE==CUST_TUNER_SONY_RA231)
EXTERN ITUNER_FCT_T piTunerFct_RA231 ;
#endif

#if (CUST_TUNER_TYPE==CUST_TUNER_SONY_RB221)
EXTERN ITUNER_FCT_T piTunerFct_RB221 ;
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SONY_RE227)
EXTERN ITUNER_FCT_T piTunerFct_RE227 ;
#endif

static CUST_TUNER_FCT_MAP_T cust_tuner_map[]=
{
/*{tunertype defined in cust_tunerType.h,function table defined in pd_tuner_xx.h */
{0,NULL},//Not used
#if CUST_TUNER_TYPE==CUST_TUNER_LG_TDVWH810F
{CUST_TUNER_LG_TDVWH810F,&piTunerFct_TDVWH810F},
#endif
#if CUST_TUNER_TYPE==CUST_TUNER_SS_DTVS203
{CUST_TUNER_SS_DTVS203,&piTunerFct_DTVS203},
#endif
#if CUST_TUNER_TYPE==CUST_TUNER_SONY
{CUST_TUNER_SONY,&piTunerFct_SONY},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_NXP_TDA18273)||(CUST_TUNER_TYPE==CUST_TUNER_NXPLIST)
{CUST_TUNER_NXP_TDA18273,&piTunerFct_TDA18273},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SS_DTVS20C)||(CUST_TUNER_TYPE==CUST_TUNER_NXPLIST)
{CUST_TUNER_SS_DTVS20C,&piTunerFct_DTVS20C},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SILICON_TUNERA)
{CUST_TUNER_SILICON_TUNERA,&piTunerFct_TUNERA},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SHARP_VA1G2CD)
{CUST_TUNER_SHARP_VA1G2CD,&piTunerFct_VA1G2CD},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_TCL_F20WT)
{CUST_TUNER_TCL_F20WT,&piTunerFct_F20WT},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SILABS_SI2158)
{CUST_TUNER_SILABS_SI2158,&piTunerFct_SI2158},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SILABS_SI2157)
{CUST_TUNER_SILABS_SI2157,&piTunerFct_SI2157},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_Mxl_MXL601)
{CUST_TUNER_Mxl_MXL601,&piTunerFct_MXL601},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_Mxl_MXL661)
{CUST_TUNER_Mxl_MXL661,&piTunerFct_MXL661},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_NXP_TDA18274)
{CUST_TUNER_NXP_TDA18274,&piTunerFct_TDA18274},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SONY_RA217)
{CUST_TUNER_SONY_RA217,&piTunerFct_RA217},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SONY_RA231)
{CUST_TUNER_SONY_RA231,&piTunerFct_RA231},
#endif

#if (CUST_TUNER_TYPE==CUST_TUNER_SONY_RB221)
{CUST_TUNER_SONY_RB221,&piTunerFct_RB221},
#endif
#if (CUST_TUNER_TYPE==CUST_TUNER_SONY_RE227)
{CUST_TUNER_SONY_RE227,&piTunerFct_RE227},
#endif

};
//#endif

ITUNER_FCT_T* GetCustTunerFuncTable(UINT16 id){
    
	UINT8 u1FctTblNum=sizeof(cust_tuner_map)/sizeof(cust_tuner_map[0]);
	ITUNER_FCT_T* pTunerFunc=NULL;
	UINT8 ii=0;
	mcDBG_MSG_INFO(("Total %d func table exist\n",u1FctTblNum-1)); 
	
	if(u1FctTblNum==1)
	{
	    mcDBG_MSG_ERR(("Func table is NULL!Please check your SW setting\n"));
	}
	else if(u1FctTblNum==2)
	{
	   pTunerFunc=cust_tuner_map[1].fct;
	   mcDBG_MSG_INFO(("Tuner Type=0x%02X\n",cust_tuner_map[1].id));
	}
	else //u1FctTblNum>2
	 {
	  for(ii=0;ii<u1FctTblNum;ii++)
	    {
	     if(cust_tuner_map[ii].id==id)
		 {
		  pTunerFunc=cust_tuner_map[ii].fct;
		  mcDBG_MSG_INFO(("TunerAll tunertype\n"));
		  break;
		 }
	   }
	 }
	 return pTunerFunc;
}

