/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 1999 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file pql_ddi.h
 *
 *  PQL include header.
 *
 *  @author	Youm, Seung-Jun(sjyoum@lge.com)
 *  @version	1.0
 *  @date		2006.08.30
 *  @see
 */


/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef	__PQL_DDI_H_
#define	__PQL_DDI_H_

/******************************************************************************
    ���� ���� ���(Control Constants)
******************************************************************************/

/******************************************************************************
	#include ���ϵ� (File Inclusions)
******************************************************************************/
#include "common.h"
#include "osa_modeldef.h"
#include "branches.h"
#include "pql.h"
#include "pql_db.h"
#include "pql_dc2plus.h"
#include "pql_debug.h"
#include "pql_soc.h"
#include "pql_color.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
	��� ����
	(Constant Definitions)
*******************************************************************************/

//#define ADCCAL_BYCSC

#define PQL_SELECT_ON	1

#define PQL_ON					(1)
#define PQL_OFF					(0)

#define PQL_OK					(0)
#define PQL_NOT_OK				(-1)

#define PQL_SUCCESS(e)      ((e) == PQL_OK)
#define PQL_ERROR(e)        ((e) != PQL_OK)




/**
* Input Type & Format Type Definition for PQL
*/
/* PQL_INPUT_TYPE */
#define PQL_INPUT_DTV			0		/* Digital_RF */
#define PQL_INPUT_ANA			1		/* Analog_RF/CVBS/S-video  */
#define PQL_INPUT_COMP			2		/* Component */
#define PQL_INPUT_HDMI			3		/* HDMI/DVIDTV/DVIPC */
#define PQL_INPUT_RGBPC			4		/* RGB_PC  */
#define PQL_INPUT_DIVX			5		/* DIVX */
#define PQL_INPUT_PVR			6		/* PVR*/
#define PQL_INPUT_IPTV			7		/* IPTV*/
#define PQL_SOURCE_MAX			8		/* Max source in this system */



#define PQL_ALL_THE_SAME_INDEX			0
#define PQL_ALL_THE_SAME_INDEX_MAX		1
#define PQL_NA_INDEX_MAX				0
#define PQL_END_OF_ARRAY				0xFF

#define PQL_INPUT_SRC_FMT_T			UINT32

#define PQL_LSHIFT_DATA(val, _bits, _pos)	((val & _bits) << _pos)
#define PQL_DATA_MASK(_bits, _pos)			((_bits) << _pos)
#define PQL_RSHIFT_DATA(val, _bits, _pos)	((val & PQL_DATA_MASK(_bits, _pos)) >> _pos)


#define PQL_3DMODE_POS				30
#define PQL_3DMODE_BITS				0x00000003
#define PQL_SET_3DMODE(_srcfmt,_idx)	\
	_srcfmt = ((_srcfmt \
						& ~(PQL_DATA_MASK(PQL_3DMODE_BITS, PQL_3DMODE_POS))) \
						| (PQL_LSHIFT_DATA(_idx, PQL_3DMODE_BITS, PQL_3DMODE_POS)))

#define PQL_GET_3DMODE(_srcfmt)		PQL_RSHIFT_DATA(_srcfmt, PQL_3DMODE_BITS, PQL_3DMODE_POS)

#define PQL_3D_IN_FMT_POS			26
#define PQL_3D_IN_FMT_BITS			0x0000000f
#define PQL_SET_3D_IN_FMT(_srcfmt,_idx)	\
	_srcfmt = ((_srcfmt \
						& ~(PQL_DATA_MASK(PQL_3D_IN_FMT_BITS, PQL_3D_IN_FMT_POS))) \
						| (PQL_LSHIFT_DATA(_idx, PQL_3D_IN_FMT_BITS, PQL_3D_IN_FMT_POS)))

#define PQL_GET_3D_IN_FMT(_srcfmt)		PQL_RSHIFT_DATA(_srcfmt, PQL_3D_IN_FMT_BITS, PQL_3D_IN_FMT_POS)


#define PQL_SHOPMODE_POS			25
#define PQL_SHOPMODE_BITS			0x00000001
#define PQL_SET_SHOPMODE(_srcfmt,_idx)	\
	_srcfmt = ((_srcfmt \
						& ~(PQL_DATA_MASK(PQL_SHOPMODE_BITS, PQL_SHOPMODE_POS))) \
						| (PQL_LSHIFT_DATA(_idx, PQL_SHOPMODE_BITS, PQL_SHOPMODE_POS)))
#define PQL_GET_SHOPMODE(_srcfmt)		PQL_RSHIFT_DATA(_srcfmt, PQL_SHOPMODE_BITS, PQL_SHOPMODE_POS)

#define PQL_MAIN_SUB_POS			23
#define PQL_MAIN_SUB_BITS			0x00000003
#define PQL_SET_MAIN_SUB(_srcfmt,_idx)	\
	_srcfmt = ((_srcfmt \
						& ~(PQL_DATA_MASK(PQL_MAIN_SUB_BITS, PQL_MAIN_SUB_POS))) \
						| (PQL_LSHIFT_DATA(_idx, PQL_MAIN_SUB_BITS, PQL_MAIN_SUB_POS)))
#define PQL_GET_MAIN_SUB(_srcfmt)		PQL_RSHIFT_DATA(_srcfmt, PQL_MAIN_SUB_BITS, PQL_MAIN_SUB_POS)

#define PQL_WRDY_POS			16
#define PQL_WRDY_BITS			0x00000001
#define PQL_SET_WRDY(_srcfmt,_idx)	\
	_srcfmt = ((_srcfmt \
						& ~(PQL_DATA_MASK(PQL_WRDY_BITS, PQL_WRDY_POS))) \
						| (PQL_LSHIFT_DATA(_idx, PQL_WRDY_BITS, PQL_WRDY_POS)))
#define PQL_GET_WRDY(_srcfmt)		PQL_RSHIFT_DATA(_srcfmt, PQL_WRDY_BITS, PQL_WRDY_POS)

#define PQL_INPUT_TYPE_POS			8
#define PQL_INPUT_TYPE_BITS			0x000000ff
#define PQL_SET_INPUT_TYPE(_srcfmt,_idx)	\
	_srcfmt = ((_srcfmt \
						& ~(PQL_DATA_MASK(PQL_INPUT_TYPE_BITS, PQL_INPUT_TYPE_POS))) \
						| (PQL_LSHIFT_DATA(_idx, PQL_INPUT_TYPE_BITS, PQL_INPUT_TYPE_POS)))
#define PQL_GET_INPUT_TYPE(_srcfmt)		PQL_RSHIFT_DATA(_srcfmt, PQL_INPUT_TYPE_BITS, PQL_INPUT_TYPE_POS)

#define PQL_RES_INDEX_POS			0
#define PQL_RES_INDEX_BITS			0x000000ff
#define PQL_SET_RES_INDEX(_srcfmt,_idx)	\
	_srcfmt = ((_srcfmt \
						& ~(PQL_DATA_MASK(PQL_RES_INDEX_BITS, PQL_RES_INDEX_POS))) \
						| (PQL_LSHIFT_DATA(_idx, PQL_RES_INDEX_BITS, PQL_RES_INDEX_POS)))
#define PQL_GET_RES_INDEX(_srcfmt)		PQL_RSHIFT_DATA(_srcfmt, PQL_RES_INDEX_BITS, PQL_RES_INDEX_POS)

#define PQL_INDEXTBL_START_INDEX				0
#define PQL_INDEXTBL_TRID_START_INDEX			4
#define PQL_3D_IN_FMT_NUM						4
#define PQL_INDEXTBL_TRID_WRDY_OFFSET			(PQL_3D_IN_FMT_NUM*2)
#define PQL_INDEXTBL_SHOP_MODE_OFFSET			1
#define PQL_INDEXTBL_SUB_MODE_OFFSET			0
#define PQL_INDEXTBL_WRDY_OFFSET				2

//[090908-clearcut] DCNR
#define DCNR_DNR_LUT_NUM 16             // Dnr ���� LUT�� entry ��
#define DCNR_HISTO_BIN_NUM 4                     // Histogram ���� ���� �� ���� BIN�� �� (x)
#define DCNR_HISTO_THRD 64800            // �˰��� ���� ���� �Ǵ��� ���� Threshold
#define DCNR_FIR_TAB_NUM 5


#define NUM_HISTO_TYPE 								7				// Histogram Type ����


/*******************************************************************************
	��ũ�� ����
	(Macro Definitions)
*******************************************************************************/
#define IF_PQLFMT_IS_INTERLACE(x) if(	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_DTV_HD_I  || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_I_DVB || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_240I_50HZ || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_SD_I_DVB	\
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_ANA \
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_COMP && \
										(	PQL_GET_RES_INDEX(x)==PQL_COMP_HD_I  || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_SD_I_PAL\
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_I  || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_PVR && \
										(	PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_I  || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_IPTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_I  || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_I_PAL \
										)	\
									)	\
								)

#define IF_PQLFMT_IS_PC(x) if(	\
								(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_RGBPC && \
									(	PQL_GET_RES_INDEX(x)==PQL_RGBPC_VGA || \
										PQL_GET_RES_INDEX(x)==PQL_RGBPC_SVGA || \
										PQL_GET_RES_INDEX(x)==PQL_RGBPC_XGA || \
										PQL_GET_RES_INDEX(x)==PQL_RGBPC_WXGA || \
										PQL_GET_RES_INDEX(x)==PQL_RGBPC_UXGA || \
										PQL_GET_RES_INDEX(x)==PQL_RGBPC_FHD	\
									)	\
							   	) || \
								(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
									(	PQL_GET_RES_INDEX(x)==PQL_DVIPC_VGA  || \
										PQL_GET_RES_INDEX(x)==PQL_DVIPC_SVGA || \
										PQL_GET_RES_INDEX(x)==PQL_DVIPC_XGA || \
										PQL_GET_RES_INDEX(x)==PQL_DVIPC_WXGA || \
										PQL_GET_RES_INDEX(x)==PQL_DVIPC_UXGA || \
										PQL_GET_RES_INDEX(x)==PQL_DVIPC_FHD \
									)	\
								)	\
							)

#define IF_PQLFMT_ISNOT_PC(x) if(	\
									!(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_RGBPC && \
										(	PQL_GET_RES_INDEX(x)==PQL_RGBPC_VGA || \
											PQL_GET_RES_INDEX(x)==PQL_RGBPC_SVGA || \
											PQL_GET_RES_INDEX(x)==PQL_RGBPC_XGA || \
											PQL_GET_RES_INDEX(x)==PQL_RGBPC_WXGA || \
											PQL_GET_RES_INDEX(x)==PQL_RGBPC_UXGA || \
											PQL_GET_RES_INDEX(x)==PQL_RGBPC_FHD	\
										)	\
								   	) || \
									!(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_DVIPC_VGA  || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_SVGA || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_XGA || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_WXGA || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_UXGA || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_FHD \
										)	\
									)	\
								)

#define IF_PQLFMT_IS_HDMI(x) if(	(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_HDMIAV_FHD  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_FHD_PAL  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_FHD  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_FHD_PAL  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I_PAL \
										)	\
									)	\
								)

/* Including HDMI PC (DVI PC) formats */
#define IF_PQLFMT_IS_HDMI_ALL(x) if((PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_HDMIAV_FHD  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_FHD_PAL  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_FHD  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_FHD_PAL  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_VGA  || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_SVGA || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_XGA || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_WXGA || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_UXGA || \
											PQL_GET_RES_INDEX(x)==PQL_DVIPC_FHD \
										)	\
									)	\
								)


#define IF_PQLFMT_IS_HDMI_HD(x) if(	(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_HDMIAV_FHD  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_FHD_PAL  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_FHD  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_FHD_PAL  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_FHD || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_FHD_PAL  || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_P_PAL\
										)	\
									)	\
								)

#define IF_PQLFMT_IS_DVIDTV(x) if(	(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_DVIDTV_FHD || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_FHD_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_I_PAL\
										)	\
									)	\
								)

#define IF_PQLFMT_IS_DTV_HD(x) if(	(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_DTV_FHD  || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_FHD_DVB  || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_I_DVB || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_P_DVB || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_I_DVB\
										)	\
									)	\
								)

#define IF_PQLFMT_IS_COMP(x) if(	(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_COMP && \
											(	PQL_GET_RES_INDEX(x)==PQL_COMP_FHD  || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_HD_I || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_HD_P || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_FHD_PAL || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_HD_I_PAL || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_HD_P_PAL || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_SD_I || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_SD_P || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_SD_I_PAL|| \
												PQL_GET_RES_INDEX(x)==PQL_COMP_SD_P_PAL\
											)	\
										)	\
									)

#define IF_PQLFMT_IS_COMP_HD(x) if(	(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_COMP && \
											(	PQL_GET_RES_INDEX(x)==PQL_COMP_FHD  || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_HD_I || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_HD_P || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_FHD_PAL || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_HD_I_PAL || \
												PQL_GET_RES_INDEX(x)==PQL_COMP_HD_P_PAL\
											)	\
										)	\
									)

#define IF_PQLFMT_IS_PVR_ANA(x) if(	(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_PVR && \
												(	PQL_GET_RES_INDEX(x)==PQL_PVRANA_RF	|| \
													PQL_GET_RES_INDEX(x)==PQL_PVRANA_RF_PAL || \
													PQL_GET_RES_INDEX(x)==PQL_PVRANA_AV || \
													PQL_GET_RES_INDEX(x)==PQL_PVRANA_AV_PAL || \
													PQL_GET_RES_INDEX(x)==PQL_PVRANA_SVID || \
													PQL_GET_RES_INDEX(x)==PQL_PVRANA_SVID_PAL || \
													PQL_GET_RES_INDEX(x)==PQL_PVRANA_SCARTRGB\
												)	\
											)	\
										)


#define IF_PQLFMT_IS_PVR_HD(x) if(	(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_PVR && \
												(	PQL_GET_RES_INDEX(x)==PQL_PVRDTV_FHD|| \
													PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_I || \
													PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_P || \
													PQL_GET_RES_INDEX(x)==PQL_PVRDTV_FHD_PAL || \
													PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_I_PAL || \
													PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_P_PAL\
												)	\
											)	\
										)


#define IF_PQLFMT_IS_IPTV_HD(x) if(	(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_IPTV && \
												(	PQL_GET_RES_INDEX(x)==PQL_IPTV_FHD|| \
													PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_I || \
													PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_P || \
													PQL_GET_RES_INDEX(x)==PQL_IPTV_FHD_PAL || \
													PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_I_PAL || \
													PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_P_PAL\
												)	\
											)	\
										)

#define IF_PQLFMT_IS_SD(x) if(	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_DTV_SD_P  || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_SD_P_DVB || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_240I_50HZ || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_SD_I_DVB	\
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_ANA \
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_COMP && \
										(	PQL_GET_RES_INDEX(x)==PQL_COMP_SD_P  || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_SD_I_PAL\
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_P  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_P  || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_P  || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_PVR && \
										(	PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_I  || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_IPTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_I  || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_I_PAL \
										)	\
									)	\
								)

#define IF_PQLFMT_IS_PAL(x) if((PQL_GET_RES_INDEX(x) == PQL_DTV_SD_P_DVB) \
				|| (PQL_GET_RES_INDEX(x) == PQL_DTV_SD_I_DVB) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_RF_PAL) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_RF_SECAM) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_RF_PAL60) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_RF_PAL_M) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_RF_PAL_N) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_CVBS_PAL) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_CVBS_SECAM) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_CVBS_PAL60) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_CVBS_PAL_M) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_CVBS_PAL_N) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_SVID_PAL) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_SVID_SECAM) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_SVID_PAL60) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_SVID_PAL_M) \
				 || (PQL_GET_RES_INDEX(x) == PQL_ANA_SVID_PAL_N) \
				 || (PQL_GET_RES_INDEX(x) == PQL_COMP_SD_P_PAL) \
				 || (PQL_GET_RES_INDEX(x) == PQL_COMP_SD_I_PAL) \
				 || (PQL_GET_RES_INDEX(x) == PQL_HDMIAV_SD_P_PAL) \
				 || (PQL_GET_RES_INDEX(x) == PQL_HDMIAV_SD_I_PAL) \
				 || (PQL_GET_RES_INDEX(x) == PQL_DVIDTV_SD_P_PAL) \
				 || (PQL_GET_RES_INDEX(x) == PQL_DVIDTV_SD_I_PAL) \
				 )

#define IF_PQLFMT_IS_EBU(x) if(	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_DTV_SD_P_DVB || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_240I_50HZ || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_SD_I_DVB	\
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_COMP && \
										(	PQL_GET_RES_INDEX(x)==PQL_COMP_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_SD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_IPTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_ANA && \
										(	PQL_GET_RES_INDEX(x)==PQL_ANA_RF_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_RF_SECAM || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_RF_PAL60 || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_RF_PAL_M || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_RF_PAL_N || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_CVBS_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_CVBS_SECAM || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_CVBS_PAL60 || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_CVBS_PAL_M || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_CVBS_PAL_N || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_SVID_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_SVID_SECAM || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_SVID_PAL60 || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_SVID_PAL_M || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_SVID_PAL_N || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_SCARTRGB \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_PVR && \
										(	PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_PVRANA_RF_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_PVRANA_AV_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_PVRANA_SVID_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_PVRANA_SCARTRGB \
										)	\
									)	\
								)

#define IF_PQLFMT_IS_SMPTE(x) if(	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_DTV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_SD_I	\
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_COMP && \
										(	PQL_GET_RES_INDEX(x)==PQL_COMP_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_SD_I \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_SD_I \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_IPTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_SD_I \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_ANA && \
										(	PQL_GET_RES_INDEX(x)==PQL_ANA_RF || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_RF_NTSC443 || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_CVBS || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_CVBS_NTSC443 || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_SVID || \
											PQL_GET_RES_INDEX(x)==PQL_ANA_SVID_NTSC443 \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_RGBPC && \
										(	PQL_GET_RES_INDEX(x)==PQL_RGBPC_VGA || \
											PQL_GET_RES_INDEX(x)==PQL_RGBPC_SVGA || \
											PQL_GET_RES_INDEX(x)==PQL_RGBPC_XGA \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DIVX && \
										(	PQL_GET_RES_INDEX(x)==PQL_DIVX_VGA || \
											PQL_GET_RES_INDEX(x)==PQL_DIVX_SVGA || \
											PQL_GET_RES_INDEX(x)==PQL_DIVX_XGA \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_PVR && \
										(	PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_P || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_SD_I || \
											PQL_GET_RES_INDEX(x)==PQL_PVRANA_RF || \
											PQL_GET_RES_INDEX(x)==PQL_PVRANA_AV || \
											PQL_GET_RES_INDEX(x)==PQL_PVRANA_SVID \
										)	\
									)	\
								)

#define IF_PQLFMT_IS_BT709(x) if(	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_DTV_FHD || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_FHD_DVB || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_P_DVB || \
											PQL_GET_RES_INDEX(x)==PQL_DTV_HD_I_DVB \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_COMP && \
										(	PQL_GET_RES_INDEX(x)==PQL_COMP_FHD || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_FHD_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_SD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_COMP_SD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_HDMI && \
										(	PQL_GET_RES_INDEX(x)==PQL_HDMIAV_FHD || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_FHD || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_FHD || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_FHD_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_FHD_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_DVIDTV_HD_I_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_FHD_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_HDMIAV_RGB_HD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_IPTV && \
										(	PQL_GET_RES_INDEX(x)==PQL_IPTV_FHD || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_FHD_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_IPTV_HD_I_PAL \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_RGBPC && \
										(	PQL_GET_RES_INDEX(x)==PQL_RGBPC_WXGA || \
											PQL_GET_RES_INDEX(x)==PQL_RGBPC_UXGA || \
											PQL_GET_RES_INDEX(x)==PQL_RGBPC_FHD \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DIVX && \
										(	PQL_GET_RES_INDEX(x)==PQL_DIVX_WXGA || \
											PQL_GET_RES_INDEX(x)==PQL_DIVX_UXGA || \
											PQL_GET_RES_INDEX(x)==PQL_DIVX_FHD \
										)	\
									) ||	\
									(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_PVR && \
										(	PQL_GET_RES_INDEX(x)==PQL_PVRDTV_FHD || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_P || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_I || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_FHD_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_P_PAL || \
											PQL_GET_RES_INDEX(x)==PQL_PVRDTV_HD_I_PAL \
										)	\
									)	\
								)


#define IF_PQLFMT_IS_DTV(x) if(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DTV)

#define IF_PQLFMT_IS_ANA(x) if(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_ANA)

#define IF_PQLFMT_IS_SIMPLE_MENU_TYPE(x) if(PQL_GET_INPUT_TYPE(x)== PQL_INPUT_DIVX)

#define PQL_ADC_COEFF_MAX			3	/* Coeff_ADC�� Data ���� ����� ��ȭ���Ѿ� ��.*/
#define PQL_ADC_COEFF_MIN			0
#define PQL_ADC_COEFF_ADAPT_MAX	3	/* Coeff_ADC_Adapt�� Data ���� ����� ��ȭ���Ѿ� ��.*/
#define PQL_ADC_COEFF_ADAPT_MIN	0

/*******************************************************************************
    �� ����
    (Type Definitions)
*******************************************************************************/

/**
 * xd struc for pdp
 */

typedef struct
{
	UINT16 (*puGeneralFmterAddr)[2];
	UINT16 sizeofFmterAddr;
	UINT8 (*puGeneralXdpV4Addr)[4];
	UINT16 sizeofXdpV4Addr;
	UINT8 (*puGeneralXdpV5Addr)[4];
	UINT16 sizeofXdpV5Addr;
	UINT16 *pGeneralFmterCoeff;
	UINT8 *pGeneralXdpV4Coeff;
	UINT8 *pGeneralXdpV5Coeff;
	UINT16 sizeofpGeneralFmterCoeff;
	UINT16 sizeofpGeneralXdpV4Coeff;
	UINT16 sizeofpGeneralXdpV5Coeff;
} PQL_XDP_GENERAL_COEFF_T;



/**
 * color temperature ��.
 *
 * @see
 */
typedef struct PQL_COLOR_TEMP
{
	UINT8	rgbGain[3];			/**< Gain. 0:Red, 1:Green, 2:Blue*/
	UINT8	rgbOffset[3];		/**< Offset. 0:Red, 1:Green, 2:Blue*/

}PQL_COLOR_TEMP_T;


/**
 * white balace ��  for PQ Color Temperature Algorithm
 * (Cool/Norm/Warm/Warm1)
 *
 * @see
 */
typedef struct PQL_WB_DATA
{
	PQL_COLOR_TEMP_T	wbCool;				/**< white balance Cool data*/
	PQL_COLOR_TEMP_T	wbNorm;				/**< white balance Normal data*/
	PQL_COLOR_TEMP_T	wbWarm;				/**< white balance Warm data*/
	PQL_COLOR_TEMP_T	wbWarm1;			/**< white balance Warm1 data*/
}PQL_WB_DATA_T;


/**
 * white balace �� from NVRAM (Factory Mode)
 * (Cool/Norm/Warm)
 *
 * @see
 */
typedef struct PQL_WB_DATA_FATORY
{
	PQL_COLOR_TEMP_T	wbCool;				/**< white balance Cool data*/
	PQL_COLOR_TEMP_T	wbNorm;				/**< white balance Normal data*/
	PQL_COLOR_TEMP_T	wbWarm;				/**< white balance Warm data*/
}PQL_WB_FACTORY_DATA_T;


/**
 * color temperature struc.
 *
 * @see
 */
typedef enum {
	PQL_WB_RED		= 0,				/**< white balance Red data*/
	PQL_WB_GRN		= 1,				/**< white balance Green data*/
	PQL_WB_BLU		= 2,				/**< white balance Blue data*/

} PQL_WB_COLOR_T;

/**
 * CM : Output CSC coefficient struct.
 *
 * @see
 */
typedef struct PQL_CM_OUT_CS
{
	//Output CSC Setting Val.
	float fColorStdMatrix1[3];
	float fColorStdMatrix2[3];
	float fColorStdMatrix3[3];
	UINT8 uColorStdForRev ;
}PQL_CM_OUT_CS_T;

/**
 * PQL Process Path.
 * Main Path(Main External Input, Main Format converter, Main Post Processing)��\n
 * Sub Path (Sub External Input, Sub Format converter, Sub Post Processing, DENC)��
 * ������ �� ����Ѵ�.
 * @ingroup
 */
typedef enum {
	PQL_PATH_MAIN		= 0,		/* main path */
	PQL_PATH_SUB,				/* sub path */
	PQL_PATH_VCR,				/* vcr out path */
	PQL_PATH_MAXN
} PQL_PATH_T;

/**
 * CM : CM Level Adjust struct.
 *
 * @see
 */
typedef struct PQL_CM_LEVEL_ADJ
{
    UINT16 pqlLevelYGain;
    UINT16 pqlLevelYOfst;
    UINT16 pqlLevelCGain;
}PQL_CM_LEVEL_ADJ_T;

/**
 * True Motion Level Enumeration.
 *
 * @see
 */

typedef enum{
	PQL_FRC_TRUM_OFF =0,
	PQL_FRC_TRUM_LOW,
	PQL_FRC_TRUM_HIGH,
	PQL_FRC_TRUM_USER,
}PQL_FRC_TRUM_INDEX_T;

/**
 * SH : Sharpness UI value struct.
 *
 * @see
 */
typedef struct PQL_SHARP_UI_VALUE{
	UINT8	nSharpnessCtrlType;
	float	fSharpnessUi;
	float	fHSharpnessUi;
	float	fVSharpnessUi;
	UINT8	edgeEnhancerUi;
	UINT8	superResUi;
	PQL_FRC_TRUM_INDEX_T trumIdx;
}PQL_SHARP_UI_VALUE_T;

/**
 * DC : Dynamic Contrast Level Enumeration.
 *
 * @see
 */
typedef enum {
	  PQL_DC_OFF	  = 0,
	  PQL_DC_LOW,
	  PQL_DC_MED,
	  PQL_DC_HIGH
} PQL_DC_LEVEL_T;


/**
 * DYNAMICCOLOR : Dynamic Color Level Enumeration.
 *
 * @see
 */
typedef enum {
	  PQL_FC_OFF	  = 0,
	  PQL_FC_LOW,
	  PQL_FC_MED,
	  PQL_FC_HIGH
	  
} PQL_DYNAMICCOLOR_LEVEL_T;


/**
 * SR : SR Level Enumeration.
 *
 * @see
 */
typedef enum {
	  PQL_SR_OFF	  = 0,
	  PQL_SR_LOW,
	  PQL_SR_MED,
	  PQL_SR_HIGH,
} PQL_SR_LEVEL_T;

/**
 * NR : NR Level Enumeration.
 *
 * @see
 */
typedef enum {
	  PQL_NR_OFF	  = 0,
	  PQL_NR_LOW,
	  PQL_NR_MEDIUM,
	  PQL_NR_HIGH,
	  PQL_NR_AUTO,
	  PQL_NR_DISABLE
} PQL_NR_LEVEL_T;


/**
 * BLACK : Black Level Control Setting Value Enumeration.
 *
 * @see
 */
typedef enum {
	PQL_BLACK_LOW		= 0,			/**< Black Level Low	*/
	PQL_BLACK_HIGH, 					/**< Black Level High	*/
	PQL_BLACK_AUTO, 					/**< Black Level Auto	*/
} PQL_BLACK_T;

/**
 * PC : PC Control UI value Structure.
 *
 * @see
 */
typedef struct PQL_PC_UI_VALUE{
	float contrastUi;
	float brightnessUi;
	float colorUi;
	float tintUi;
	float contrastCentreUi;
}PQL_PC_UI_VALUE_T;

 /**
 * Loading UI matching table val in case of EyeQ.
 *
 * @see
 */
 typedef enum {
		 PQL_EYEQ_CONTRAST	 = 0x0000,
		 PQL_EYEQ_BRIGHTNESS,
		 PQL_EYEQ_COLOR,
		 PQL_EYEQ_TINT,
		 PQL_EYEQ_SHARPNESS,
		 PQL_EYEQ_BACKLIGHT
 } PQL_EYEQ_DB_MEMBER_T;

 /**
 * Loading UI matching Offset table val in case of KIND.
 *
 * @see
 */
 typedef enum {
		 PQL_KIND_CONTRAST_OFFSET	 = 0x0000,
		 PQL_KIND_BRIGHTNESS_OFFSET,
		 PQL_KIND_COLOR_OFFSET,
		 PQL_KIND_TINT_OFFSET,
		 PQL_KIND_SHARPNESS_OFFSET
 } PQL_KIND_DB_MEMBER_T;

 /**
  * Loading DC2P DynamicContrast value parameters table val.
  *
  * @see
  */
  typedef enum {
		  PQL_DIMMING_APL	= 0x0000,
		  PQL_DIMMING_APLMATCH_DIMRANGE	= 0x0001,
		  PQL_DIMMING_UIPOINT	= 0x0002,
		  PQL_DIMMING_UIMATCH_DIMRANGE	= 0x0003
  } PQL_DIMMING_DB_MEMBER_T;

#define PQL_EYECARE_CURVE_POINT_NUM	7

/**
*	 EyeCare control parameter.
*
*/
typedef struct  {
	SINT32 	 nRawApl[PQL_EYECARE_CURVE_POINT_NUM];
	SINT32 	 nDimRangeWithRawApl[PQL_EYECARE_CURVE_POINT_NUM];
} PQL_EYECARE_DIMMING_DATA_T;

 /**
 *	 EyeCare Total parameter to control dimming.
 *
 */
 typedef struct {
	 //PQL_EYECARE_DIMMING_DATA_T  stPcEyecareHighCtrlData ;
	 //PQL_EYECARE_DIMMING_DATA_T  stPcEyecareLowCtrlData ;
	 PQL_EYECARE_DIMMING_DATA_T  stPcEyecareCtrlData ;
 }PQL_EYECARE_DIMMING_DB_T;



#define PQL_UI_DIMMING_MAX_CURVE_CTRL 6

/**
*	Clear Plus Dimming Curve Table.
*
*/
typedef struct {
	SINT32	nUiBackLightCtrlPoint[PQL_UI_DIMMING_MAX_CURVE_CTRL] ;
	SINT32	nClearPlusBacklightVal[PQL_UI_DIMMING_MAX_CURVE_CTRL];
}PQL_CLEARP_DIMMING_DB_T;


/**
 * CM : Color Standard Setting Value Enumeration.
 *
 * @see
 */
 typedef enum {
	PQL_CS_SD=0,
	PQL_CS_HD,
	PQL_CS_MAX
} PQL_CS_T;




/**
 * AVDEC : Analog Video Decoder Input Mode Enumeration.
 *
 * @see
 */
typedef enum
{
	PQL_AVDEC_MODE_NTSC,
	PQL_AVDEC_MODE_PAL,
	PQL_AVDEC_MODE_PAL_N,
	PQL_AVDEC_MODE_PAL_M,
	PQL_AVDEC_MODE_SECAM,
	PQL_AVDEC_MODE_NTSC_443,
	PQL_AVDEC_MODE_PAL_60,
	PQL_AVDEC_MODE_UNKNOWN_525,
	PQL_AVDEC_MODE_UNKNOWN_625,
	PQL_AVDEC_MODE_UNKNOWN,
}PQL_AVDECODER_VIDEO_MODE_T;

/**
 * Edge Enhance Level Enumeration.
 *
 * @see
 */
typedef enum {
	  PQL_EDGE_ENHANCE_OFF	  = 0,
	  PQL_EDGE_ENHANCE_LOW,
	  PQL_EDGE_ENHANCE_HIGH
} PQL_EDGE_ENHANCE_UI_LEVEL_T;

/**
 * Edge Enhance UI Value.
 *
 * @see
 */

typedef struct PQL_UI_EDGE_ENHANCE
{
	PQL_EDGE_ENHANCE_UI_LEVEL_T eeLevel;
	PQL_FRC_TRUM_INDEX_T trumIdx;
}PQL_EDGE_ENHANCE_UI_T;


/**
 * Gamma Mode.
 */
typedef enum {
	PQL_GAMMA_LOW	= 0,	// LCD : 1.95, PDP : BT.709
	PQL_GAMMA_MID	= 1,	// LCD : 2.20, PDP : 2.20
	PQL_GAMMA_HIGH	= 2,	// LCD : 2.35, PDP : 2.35
	PQL_GAMMA_MODE_MAX	= 3
} PQL_GAMMA_MODE_T;

/**
 * Gamma Setting Block
 */
typedef enum
{
	PQL_GAMMA_BLOCK_MAIN_SOC	=0,		// Main SOC
	PQL_GAMMA_BLOCK_FRC			=1,		// FRC
	PQL_GAMMA_BLOCK_TCON		=2,		// T-Con
	PQL_GAMMA_BLOCK_MAX			=3
} PQL_GAMMA_BLOCK_T;


/**
 * Gamma Setting DATA
 */
typedef struct
{
	PQL_GAMMA_MODE_T	eGammaMode;
	PQL_GAMMA_BLOCK_T	eGammaBlock;
} PQL_GAMMA_SET_INFO_T;


/**
 * Define PQL DB Version.
 * PQL DB ���� ����ϴ� �Ʒ� index �� ���� �Ǹ� version �߰� �Ǿ����.
 * NC40 ������ Dasy ���� Recording Contents �� PQL DB ������ ���� �ϱ� ���� ����.
 */
typedef enum {
	PQL_DB_INDEX_NC40=0,
}PQL_DB_INDEX_VER_T;


/**
 * DTV fmt type.
 *
 * 2008.10.25 PSI��û���� ����.
 * 2009.06.19 GP2�� �ű� Index �����.
 * @see .
*/
typedef enum {
/* PQL_INPUT_DTV			: Digital_RF + Digital_PVR 						*/
	PQL_DTV_FHD				= 0 ,	/*0 HD Progressive :	1080p	*/
	PQL_DTV_HD_I			,		/*1 HD Interlace : 	1080i	*/
	PQL_DTV_HD_P			,		/*2 HD Progressive :	 720p	*/
	PQL_DTV_SD_P			,		/*3 SD Progressive :	 480p	*/
	PQL_DTV_SD_I			,		/*4 SD Interlace :	 480i	*/
	PQL_DTV_FHD_DVB			,		/*5 HD Progressive :	1080p	*/
	PQL_DTV_HD_I_DVB		,		/*6 HD Interlace : 	1080i	*/
	PQL_DTV_HD_P_DVB		,		/*7 HD Progressive :	 720p	*/
	PQL_DTV_SD_P_DVB		,		/*8 SD Progressive :	 576p	*/
	PQL_DTV_SD_I_DVB		,		/*9 SD Interlace : 	 576i	*/
	PQL_DTV_240I_50HZ		,		/*10	*/
	PQL_PICWIZARD			,		/*11	*/
	PQL_DTV_INDEX_MAX		,

}PQL_INPUT_DTV_T;

/**
 * Analog Source fmt type.
 *
 * @see .
*/
typedef enum {
/* PQL_INPUT_ANA		: Analog_RF/CVBS/S-video				*/
	PQL_ANA_RF				= 0,	/*0 Analog_RF + NTSC		*/
	PQL_ANA_RF_PAL			,		/*1 Analog_RF + PAL		*/
	PQL_ANA_RF_SECAM		,		/*2 Analog_RF + SECAM		*/
	PQL_ANA_RF_PAL60		,		/*3 Analog_RF + PAL60		*/
	PQL_ANA_RF_NTSC443		,		/*4 Analog_RF + NTSC443		*/
	PQL_ANA_RF_PAL_M		,		/*5 Analog_RF + PAL_M			*/
	PQL_ANA_RF_PAL_N		,		/*6 Analog_RF + PAL_N			*/
	PQL_ANA_RF_MAX			= PQL_ANA_RF_PAL_N,
	PQL_ANA_CVBS			= PQL_ANA_RF_PAL_N + 1,		/*7 Analog_CVBS + NTSC		*/
	PQL_ANA_CVBS_PAL		,		/*8 Analog_CVBS + PAL		*/
	PQL_ANA_CVBS_SECAM		,		/*9 Analog_CVBS + SECAM		*/
	PQL_ANA_CVBS_PAL60		,		/*10 Analog_CVBS + PAL60		*/
	PQL_ANA_CVBS_NTSC443	,		/*11 Analog_CVBS + NTSC443	*/
	PQL_ANA_CVBS_PAL_M		,		/*12 Analog_CVBS + PAL_M		*/
	PQL_ANA_CVBS_PAL_N		,		/*13 Analog_CVBS + PAL_N		*/
	PQL_ANA_CVBS_MAX		= PQL_ANA_CVBS_PAL_N - PQL_ANA_CVBS + 1,
	PQL_ANA_SVID			= PQL_ANA_CVBS_PAL_N+1,		/*14 Analog_SVID + NTSC		*/
	PQL_ANA_SVID_PAL		,		/*15 Analog_SVID + PAL		*/
	PQL_ANA_SVID_SECAM		,		/*16 Analog_SVID + SECAM		*/
	PQL_ANA_SVID_PAL60		,		/*17 Analog_SVID + PAL60		*/
	PQL_ANA_SVID_NTSC443	,		/*18 Analog_SVID + NTSC443	*/
	PQL_ANA_SVID_PAL_M		,		/*19 Analog_SVID + PAL_M	*/
	PQL_ANA_SVID_PAL_N		,		/*20 Analog_SVID + PAL_N	*/
	PQL_ANA_SVID_MAX		= PQL_ANA_SVID_PAL_N - PQL_ANA_SVID + 1,
	PQL_ANA_SCARTRGB		= PQL_ANA_SVID_PAL_N+1,		/*21 Analog_SVID + SCART_RGB	*/
	PQL_ANA_SCARTRGB_MAX	= PQL_ANA_SCARTRGB	,
	PQL_ANA_INDEX_MAX		= PQL_ANA_SCARTRGB+1,
}PQL_INPUT_ANA_T;

/**
 * Component fmt type.
 *
 * @see .
*/
typedef enum {
/* PQL_INPUT_COMP		: Component							*/
	PQL_COMP_FHD				= 0,	/*0 HD Interlace :  1080P	*/
	PQL_COMP_HD_I				,		/*1 HD Interlace :  1080i	*/
	PQL_COMP_HD_P				,		/*2 HD Progressive : 720p	*/
	PQL_COMP_SD_P				,		/*3 SD Progressive : 480p	*/
	PQL_COMP_SD_I				,		/*4 SD Interlace :   480i	*/
	PQL_COMP_FHD_PAL			,		/*5 HD Progressive :1080P	*/
	PQL_COMP_HD_I_PAL			,		/*6 HD Interlace :  1080i	*/
	PQL_COMP_HD_P_PAL			,		/*7 HD Progressive : 720p	*/
	PQL_COMP_SD_P_PAL			,		/*8 SD Progressive : 576p	*/
	PQL_COMP_SD_I_PAL			,		/*9 SD Interlace :   576i	*/
	PQL_COMP_INDEX_MAX		,
}PQL_INPUT_COMP_T;

/**
 * HDMI DVI fmt type.
 *
 * @see .
*/
typedef enum {
/* PQL_INPUT_HDMI	: HDMI_AV										*/
	PQL_HDMIAV_FHD			= 0,	/*0 HD Progressive : 1080P	*/
	PQL_HDMIAV_HD_I			,		/*1 HD Interlace : 1080i	*/
	PQL_HDMIAV_HD_P			,		/*2 HD Progressive : 720p	*/
	PQL_HDMIAV_SD_P			,		/*3 SD Progressive : 480p	*/
	PQL_HDMIAV_SD_I			,		/*4 SD Interlace : 480i		*/
	PQL_HDMIAV_FHD_PAL		,		/*5 HD Progressive : 1080P	*/
	PQL_HDMIAV_HD_I_PAL		,		/*6 HD Interlace : 1080i	*/
	PQL_HDMIAV_HD_P_PAL		,		/*7 HD Progressive : 720p	*/
	PQL_HDMIAV_SD_P_PAL		,		/*8 SD Progressive : 576p	*/
	PQL_HDMIAV_SD_I_PAL		,		/*9 SD Interlace : 576i		*/
	PQL_HDMIAV_MAX			= PQL_HDMIAV_SD_I_PAL + 1,		/*10 */
	PQL_HDMIAV_RGB_FHD		= PQL_HDMIAV_SD_I_PAL + 1	,		/*0 HD Progressive : 1080P	*/
	PQL_HDMIAV_RGB_HD_I 		,		/*1 HD Interlace : 1080i	*/
	PQL_HDMIAV_RGB_HD_P 		,		/*2 HD Progressive : 720p	*/
	PQL_HDMIAV_RGB_SD_P 		,		/*3 SD Progressive : 480p	*/
	PQL_HDMIAV_RGB_SD_I 		,		/*4 SD Interlace : 480i 	*/
	PQL_HDMIAV_RGB_FHD_PAL		,		/*5 HD Progressive : 1080P	*/
	PQL_HDMIAV_RGB_HD_I_PAL 	,		/*6 HD Interlace : 1080i	*/
	PQL_HDMIAV_RGB_HD_P_PAL 	,		/*7 HD Progressive : 720p	*/
	PQL_HDMIAV_RGB_SD_P_PAL 	,		/*8 SD Progressive : 576p	*/
	PQL_HDMIAV_RGB_SD_I_PAL 	,		/*9 SD Interlace : 576i 	*/
	PQL_HDMIAV_RGB_MAX			= PQL_HDMIAV_RGB_SD_I_PAL - PQL_HDMIAV_RGB_FHD + 1,		/*10 */
	/* PQL_INPUT_DVIPC	: DVI_PC											*/
	PQL_DVIDTV_FHD 			= PQL_HDMIAV_RGB_SD_I_PAL + 1,		/*10 */
	PQL_DVIDTV_HD_I			,		/*11 */
	PQL_DVIDTV_HD_P 		,		/*12 */
	PQL_DVIDTV_SD_P			,		/*13 */
	PQL_DVIDTV_SD_I			,		/*14 */
	PQL_DVIDTV_FHD_PAL		,		/*15 */
	PQL_DVIDTV_HD_I_PAL	 	,		/*16 */
	PQL_DVIDTV_HD_P_PAL		,		/*17 */
	PQL_DVIDTV_SD_P_PAL		,		/*18 */
	PQL_DVIDTV_SD_I_PAL	 	,		/*19 */
	PQL_DVIDTV_MAX			= PQL_DVIDTV_SD_I_PAL - PQL_DVIDTV_FHD + 1,		/*10 */
	PQL_DVIPC_VGA			= PQL_DVIDTV_SD_I_PAL + 1,		/*20 640x480			*/
	PQL_DVIPC_SVGA			,		/*21 800x600			*/
	PQL_DVIPC_XGA			,		/*22 1280x768			*/
	PQL_DVIPC_WXGA			,		/*23 1366x768			*/
	PQL_DVIPC_UXGA			,		/*24 1600x1200			*/
	PQL_DVIPC_FHD			,		/*25 */
	PQL_DVIPC_MAX			= PQL_DVIPC_FHD - PQL_DVIPC_VGA + 1,		/*6 */
	PQL_HDMI_INDEX_MAX		= PQL_DVIPC_FHD + 1,		/*26 */
}PQL_INPUT_HDMI_T;

#ifndef INCLUDE_VCS
/**
 * VCS fmt type.
 *
 * @see .
*/
typedef enum {
/* PQL_INPUT_VCS	: VCS										*/
	PQL_VCS_VGA			= 0,	/*0 640x480			*/
	PQL_VCS_SVGA			,	/*1 800x600			*/
	PQL_VCS_XGA				,	/*2 1280x768			*/
	PQL_VCS_WXGA			,	/*3 1366x768			*/
	PQL_VCS_UXGA			,	/*4 1600x1200			*/
	PQL_VCS_FHD				,	/*5	*/
	PQL_VCS_INDEX_MAX		,
}PQL_INPUT_VCS_T;
#endif

/**
 * RGB PC fmt type.
 *
 * @see .
*/
typedef enum {
/* PQL_INPUT_RGBPC	: RGB_PC 										*/
	PQL_RGBPC_VGA			= 0,	/*0 640x480			*/
	PQL_RGBPC_SVGA			,		/*1 800x600			*/
	PQL_RGBPC_XGA			,		/*2 1280x768			*/
	PQL_RGBPC_WXGA			,		/*3 1366x768			*/
	PQL_RGBPC_UXGA			,		/*4 1600x1200			*/
	PQL_RGBPC_FHD			,		/*5	*/
	PQL_RGBPC_INDEX_MAX		,
}PQL_INPUT_RGBPC_T;

/**
 * DIVX fmt type.
 *
 * @see .
*/
typedef enum {
/* PQL_INPUT_DIVX	: Divx MOVIE (through USB, BT)					*/
	PQL_PHOTO_VGA			= 0,	/*0 640x480			*/
	PQL_PHOTO_SVGA			,		/*1 800x600			*/
	PQL_PHOTO_XGA			,		/*2 1280x768			*/
	PQL_PHOTO_WXGA			,		/*3 1366x768			*/
	PQL_PHOTO_UXGA			,		/*4 1600x1200			*/
	PQL_PHOTO_FHD			,		/*5	*/
	PQL_PHOTO_MAX			= PQL_PHOTO_FHD + 1,
	PQL_DIVX_VGA			= PQL_PHOTO_FHD + 1,	/*6 640x480			*/
	PQL_DIVX_SVGA			,		/*7 800x600			*/
	PQL_DIVX_XGA			,		/*8 1280x768			*/
	PQL_DIVX_WXGA			,		/*9 1366x768			*/
	PQL_DIVX_UXGA			,		/*10 1600x1200			*/
	PQL_DIVX_FHD			,		/*11	*/
	PQL_DIVX_MAX			= PQL_DIVX_FHD - PQL_DIVX_VGA + 1,

	#ifdef INCLUDE_VCS
	PQL_VCS_VGA				= PQL_DIVX_FHD+1,	/*12 640x480			*/
	PQL_VCS_SVGA			,	/*13 800x600			*/
	PQL_VCS_XGA				,	/*14 1280x768			*/
	PQL_VCS_WXGA			,	/*15 1366x768			*/
	PQL_VCS_UXGA			,	/*16 1600x1200			*/
	PQL_VCS_FHD				,	/*17 */
	PQL_VCS_MAX				= PQL_VCS_FHD - PQL_VCS_VGA +1,
	PQL_DIVX_INDEX_MAX		= PQL_VCS_FHD + 1,
	#else
	PQL_DIVX_INDEX_MAX		= PQL_DIVX_FHD + 1,
	#endif
}PQL_INPUT_DIVX_T;

/**
 * PVR fmt type.
 *
 * @see .
*/
typedef enum {
/* PQL_INPUT_PVRDTV	: PVR_Digital Source					*/
	PQL_PVRDTV_FHD			= 0,	/*0 HD Progressive : 1080P	*/
	PQL_PVRDTV_HD_I			,		/*1 HD Interlace : 1080i		*/
	PQL_PVRDTV_HD_P			,		/*2 HD Progressive : 720p	*/
	PQL_PVRDTV_SD_P			,		/*3 SD Progressive : 480p	*/
	PQL_PVRDTV_SD_I			,		/*4 SD Interlace : 480i		*/
	PQL_PVRDTV_FHD_PAL		,		/*5 etc				*/
	PQL_PVRDTV_HD_I_PAL		,		/*6 etc				*/
	PQL_PVRDTV_HD_P_PAL		,		/*7 etc				*/
	PQL_PVRDTV_SD_P_PAL		,		/*8 etc				*/
	PQL_PVRDTV_SD_I_PAL		,		/*9 etc				*/
	PQL_PVRDTV_MAX			= PQL_PVRDTV_SD_I_PAL - PQL_PVRDTV_FHD + 1,
/* PQL_INPUT_PVRANA1	:  PVR_Analog RF, CVBS					*/
	PQL_PVRANA_RF			= PQL_PVRDTV_SD_I_PAL + 1,		/*10 PVR_RF + NTSC		*/
	PQL_PVRANA_RF_PAL		,		/*11 PVR_RF + PAL		*/
	PQL_PVRANA_AV			,		/*12 PVR_AV + NTSC		*/
	PQL_PVRANA_AV_PAL		,		/*13 PVR_AV + PAL		*/
	PQL_PVRANA_SVID			,		/*14 PVR_SVID		*/
	PQL_PVRANA_SVID_PAL		,		/*15 PVR_SVID + PAL	*/
	PQL_PVRANA_SCARTRGB		,		/*16 PVR_CVBS + NTSC		*/
	PQL_PVRANA_MAX			= PQL_PVRANA_SCARTRGB - PQL_PVRANA_RF + 1,
	PQL_PVR_INDEX_MAX				= PQL_PVRANA_SCARTRGB + 1,
}PQL_INPUT_PVR_T;

/**
 * IPTV fmt type.
 *
 * @see .
*/
typedef enum {
/* PQL_INPUT_IPTV	:  IPTV Source							*/
	PQL_IPTV_FHD			= 0,	/*0 HD Interlace :  1080P	*/
	PQL_IPTV_HD_I			,		/*1 HD Interlace :  1080i	*/
	PQL_IPTV_HD_P			,		/*2 HD Progressive : 720p	*/
	PQL_IPTV_SD_P			,		/*3 SD Progressive : 480p	*/
	PQL_IPTV_SD_I			,		/*4 SD Interlace :   480i	*/
	PQL_IPTV_FHD_PAL		,		/*5 HD Progressive :1080P	*/
	PQL_IPTV_HD_I_PAL		,		/*6 HD Interlace :  1080i	*/
	PQL_IPTV_HD_P_PAL		,		/*7 HD Progressive : 720p	*/
	PQL_IPTV_SD_P_PAL		,		/*8 SD Progressive : 576p	*/
	PQL_IPTV_SD_I_PAL		,		/*9 SD Interlace :   576i	*/
	PQL_IPTV_INDEX_MAX			,
}PQL_INPUT_IPTV_T;

 /**
 * Define width and height for In/Out Size.
 *
 * @see
 */
typedef struct PQL_SIZE{
	UINT16	inWidth;				/*input width*/
	UINT16	inHeight;				/*input height*/
	UINT16	outWidth;				/*output width*/
	UINT16	outHeight;				/*output height*/
}PQL_SIZE_T;

/**
* pql status.
*
* @see
*/
typedef struct PQL_STATUS{
	BOOLEAN					pqlPath[PQL_PATH_MAXN];					/* path selection */
	PQL_INPUT_SRC_FMT_T		pqlSrcFmt[PQL_PATH_MAXN];				/* src and fmt */
	PQL_SIZE_T				pqlSize[PQL_PATH_MAXN];					/* size*/
}PQL_STATUS_T;


/**
 * Getting coefficient ID and Max value from pqldb, that is not dependent on the chip specific algorithm.
 *
 * @see .
 */
typedef struct PQL_COEFF_MEMBER {

    UINT8    uPqlCoeffID;      /**< model name*/
    UINT8    uPqlCoeffIDMax;          /**< DB ver.*/

} PQL_COEFF_MEMBER_T;

/**
 * Getting coefficient ID and Max value from pqldb  for new Index structure.
 *
 * @see .
 */
typedef struct PQL_INDEX_MEMBER {
    UINT8   uPqlMainCoeffID;	/* Main Path Index Id				*/

    BOOLEAN bValidSubCoeffID;	/* valid status for Sub Index			*/
    UINT8   uPqlSubCoeffID;		/* Sub Path Index Id				*/

    BOOLEAN bValidBECoeffID;	/* valid status for BackEnd Chip Index*/
    UINT8   uPqlBECoeffID;		/* BackEnd Chip Index Id			*/

	UINT8	uPqlCoeffIDMax;		/* Max Id for Main					*/
} PQL_INDEX_MEMBER_T;


/**
* PQL data for control film(Cinema).
*/
typedef struct PQL_FILM_CTRL
{
	BOOLEAN bGameMode;
	BOOLEAN bFilmMode;
}PQL_FILM_CTRL_T;

/**
 * < Expert Control > Color Standard Setting Menu member.
 */
typedef enum {
	PQL_EXP_CS_SD,									/**< SD Color Standard  */
    PQL_EXP_CS_HD,									/**< HD Color Standard  */
    PQL_EXP_XVYCC_SD,								/**< xvYCC SD Color Standard  */
    PQL_EXP_XVYCC_HD,								/**< xvYCC HD Color Standard  */
} PQL_COLORSTANDARD_T;

/**
* PQL data for color gamut
*/
typedef struct PQL_COLORGAMUT_CTRL
{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T pqlStatus;
	BOOLEAN uxvYCCStatus;
	UINT8 uQuantRangeStatus;
	UINT8 uxvYCCCoeffId;
	UINT8 uUixvYCC;
	UINT8 uUiGamma;	// [jaehan.kim] 2010/01/29		PDP only!  THX ȭ�� �䱸 ���� : Gamma ����õ� Gamut Standard �� CSC ����
	//xvYCC meta data
	//xvYCC GBD Color Space
	UINT8 uColorGamut;

	//20091026, sjyoum, gamut���ý� xvYcc �� colorstd�� ���ÿ� �����ϱ� ���� �߰���.
	PQL_COLORSTANDARD_T uXvYccColorStd;

	PQL_BLACK_T         uUiBlackLevel;//20101013 jaeyoung.cho USB BLACK LEVEL������ ���� �߰���(LOW/HIGH/AUTO)
}PQL_COLORGAMUT_CTRL_T;


/**
* PQL data for color standard
*/
typedef struct PQL_CSC_LEVEL_CTRL
{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T pqlStatus;
	//for blackLevel
	UINT8 uUI_BlackLevel;				/** UI Color Standard Selection */
	UINT8 uBlackLevelInput;				/** Current Input */
}PQL_CSC_LEVEL_CTRL_T;

/**
 * HDMI : Color Space Domain
 *
 * @see
 */
typedef enum
{
	PQL_HDMI_RGB = 0,
	PQL_HDMI_YCBCR,
	PQL_HDMI_XVYCC,
	PQL_DVI_RGB,
	PQL_HDMI_UNKNOWN,
}PQL_HDMI_COLOR_DOMAIN_T;

/**
 * HDMI : Quantization Range
 *
 * @see
 */
typedef enum
{
	PQL_HDMI_QUANT_RANGE_DEFAULT = 0,
	PQL_HDMI_QUANT_RANGE_LIMITEDRANGE,
	PQL_HDMI_QUANT_RANGE_FULLRANGE,
	PQL_HDMI_QUANT_RANGE_RESERVED,
}PQL_HDMI_QUANTIZATION_RANGE_T;


/**
 * PQL_CM_REGION_T
 */
typedef enum {
	PQL_CM_MGT	= 0,
	PQL_CM_RED,
	PQL_CM_YEL,
	PQL_CM_GRN,
	PQL_CM_CYN,
	PQL_CM_BLU,
	PQL_CM_FT,
	PQL_CM_BS,
	PQL_CM_ICC,
	PQL_CM_IHC
}PQL_CM_REGIONID_T;


/**
 * CM : Expert Color Mangement struct.
 *
 * @see
 */
typedef struct PQL_CM_EXPCOLOR_PARAM{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T pqlStatus;
	PQL_CM_REGIONID_T	pqlCmRegionID;			/**< �����ϰ��� �ϴ� ���� ID	*/
	//UINT8	pqlCmCoeffID;					/**< DB Table Index	*/
	SINT8	uiExpSatGainValue;				/**< UI Sat Gain Value */
	SINT8	uiExpHueGainValue;				/**< UI Hue Gain Value */
	SINT8	uiExpLuminanceValue;					/**< UI Luminance Value */
	UINT8	uiFreshColorValue;				/**< UI Sat Gain Value */
}PQL_CM_EXPCOLOR_PARAM_T;

/**
 * CM : Fresh Color struct.
 *
 * @see
 */
typedef struct PQL_CM_FRESHCOLOR_PARAM{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T pqlStatus;
	UINT8	uiFreshColorValue;				/**< UI Sat Gain Value */
}PQL_CM_FRESHCOLOR_PARAM_T;

/**
 * CM : Skin Color struct.
 *
 * @see
 */
typedef struct PQL_CM_SKINCOLOR_PARAM{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T *pstPqlStatus;
	SINT8	uiSkinColorValue;				/**< UI Skin Color Value */
	UINT8	uiFreshColorValue;				/**< UI Sat Gain Value */
}PQL_CM_SKINCOLOR_PARAM_T;


/**
 * CM : Grass Color struct.
 *
 * @see
 */
typedef struct PQL_CM_GRASSCOLOR_PARAM{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T *pstPqlStatus;
	SINT8	uiGrassColorValue;				/**< UI Grass Color Value */
	UINT8	uiFreshColorValue;				/**< UI Sat Gain Value */
}PQL_CM_GRASSCOLOR_PARAM_T;

/**
 * CM : Blue Sky Color struct.
 *
 * @see
 */
typedef struct PQL_CM_BLUESKYCOLOR_PARAM{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T *pstPqlStatus;
	SINT8	uiBlueSkyColorValue;			/**< UI Skin Color Value */
	UINT8	uiFreshColorValue;				/**< UI Sat Gain Value */
}PQL_CM_BLUESKYCOLOR_PARAM_T;

/**
*	define FRESHCOLOR checksum.
*/
typedef enum PQL_COLOR {
	PQL_COLOR_SKIN,
	PQL_COLOR_GRASS,
	PQL_COLOR_SKY,
	PQL_COLOR_FRESH
} PQL_COLOR_T;

/**
 * CM : ui Color struct.
 *
 * @see
 */
typedef struct PQL_COLOR_VALUE {
	SINT8 s8SkinColor;
	SINT8 s8GrassColor;
	SINT8 s8SkyColor;
	UINT8 u8FreshColor;
} PQL_COLOR_VALUE_T;


/**
 * CM : Fresh Color struct.
 *
 * @see
 */
typedef struct PQL_CM_DYNAMICWHITE_PARAM{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T *pstPqlStatus;
	UINT8	uiDynamicWhiteValue;				/**< UI Dynamic White Value */
}PQL_CM_DYNAMICWHITE_PARAM_T;

/**
 * DC2P : Dynamic Contrast struct.
 *
 * @see
 */
typedef struct PQL_DC2P_CTRL{
	PQL_PATH_T pqlPath;
	PQL_STATUS_T stPqlStatus;
#if (PLATFORM_TYPE == MTK_PLATFORM)
	// coeff id

	UINT8	uDc2pDynContCoeffId;	/**< Coeff Index(PQL_COEFF_DC2P_DYNCONT) */
	UINT8	uDc2pAplInfoCeffId;		/**< Coeff Index(PQL_COEFF_DC2P_APL) */
	UINT8	uDc2pCgainCoeffId;		/**< Coeff Index(PQL_COEFF_DC2P_CGAIN) */
	UINT8	uDc2pSkinCoeffId;		/**< Coeff Index(PQL_COEFF_DC2P_SKIN) */
	UINT8	uDc2pYBitPlusCoeffId;	/**< Coeff Index(PQL_COEFF_DC2P_YBITPLUS) */
#endif
	// ui value
	UINT8	uUiFreshContrast;	/**< UI Fresh Contrast Value */
	BOOLEAN	bStoreMode;				/**< Store mode */
}PQL_DC2P_CTRL_T;

/**
* PQL data for anlog Video Decoder.
*/
typedef struct PQL_ANDECODER_CTRL
{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T pqlStatus;
}PQL_ANDECODER_CTRL_T;

/**
* PQL data for control BLE/WLE
*/
typedef struct PQL_BLEWLE_CTRL
{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T pqlStatus;
	BOOLEAN bBlewleMode;
}PQL_BLEWLE_CTRL_T;		// [vivakjh]  2008/12/29	PDP BLE/WLE(Black/White Level Extension)����.

/**
 * NR : NR Setting Info Structure.
 *
 * @see
 */
typedef struct PQL_NR_SETTING_INFO
{
	UINT8 uIsProgressive;
	BOOLEAN bNROnOffCtrl;
	BOOLEAN bIsDTVtiming;
	BOOLEAN bIsPCtiming;
} PQL_NR_SETTING_INFO_T;


//by KWON_0618
/**
 * UI Mpeg NR control struct.
 *
 * @see
 */
typedef struct PQL_NR_MPEGNR_PARAM{
	PQL_PATH_T pqlPath;
	//pqlstatus
	PQL_STATUS_T pstPqlStatus;
	PQL_NR_LEVEL_T	uMpegNRValue;				/**< UI MPeg NR Value */
}PQL_NR_MPEGNR_PARAM_T;

/**
 * UI NR control struct.
 *
 * @see
 */
typedef struct PQL_NR_CTRL_PARAM{
	PQL_PATH_T pqlPath;
	PQL_STATUS_T pstPqlStatus;
	PQL_NR_LEVEL_T	uNRValue;				/**< UI NR Value */
	PQL_NR_SETTING_INFO_T stSettingInfo;
}PQL_NR_CTRL_PARAM_T;


/**
 * SR control struct.
 *
 * @see
 */
typedef struct PQL_SR_PARAM{
	PQL_PATH_T pqlPath;
	PQL_STATUS_T stPqlStatus;
	PQL_SR_LEVEL_T	uSRValue;		/**< UI SR Value */
}PQL_SR_PARAM_T;


/**
 * PQL_INDEXTBL_FLAG.
 */
typedef enum {
	PQL_IDXAR_ATTR_NA		= 0x01,	//Not Applicable
	PQL_IDXAR_ATTR_EACH_RES	= 0x02,	//Using Each Resolution
	PQL_IDXAR_ATTR_ALL_THE_SAME	= 0x04,	//Using ALL The Same Index
	PQL_IDXAR_ATTR_ALL			= 0x07,	//Using ALL
}PQL_IDXAR_ATTR_T;


/**
 * PQL_INDEXTBL_COMPONENT_T.
 */
typedef struct PQL_INDEXTBL_COMPONENT {
	UINT8 uPqlIdxArAttr;
	UINT8 uPqlSizeOfIdxArray;
	UINT8 *puPqlIdxArray;
#ifndef _EMUL_WIN
} __attribute__((packed)) PQL_INDEXTBL_COMPONENT_T;
#else
}PQL_INDEXTBL_COMPONENT_T;;
#endif
/**
 * PQL_INDEXTBL_T.
 */
typedef struct PQL_INDEXTBL {
	PQL_INDEXTBL_COMPONENT_T *pPqlSrcInput[PQL_SOURCE_MAX];
}PQL_INDEXTBL_T;

/**
 * PQL_FUNC_IDXTBL_DET_FACTOR_T.
 */
typedef struct PQL_FUNC_IDXTBL_DET_FACTOR {
	UINT8	uPql3DMode;
	UINT8	uPql3DInputFormat;

	UINT8	uPqlShopMode;
	UINT8	uPqlMainOrSub;
	UINT8	uPqlWirelessRdy;

	UINT8	uPqlInputType;
	UINT8	uPqlResIndex;

}PQL_FUNC_IDXTBL_DET_FACTOR_T;

/**
 * PQL_DB_IDXTBL_DET_FACTOR.
 */
typedef struct PQL_DB_IDXTBL_DET_FACTOR {
	PQL_FUNC_IDXTBL_DET_FACTOR_T *pstCurInputStatus;
	UINT32 pqlIdCoeff;
	//�ű� �߰� ���
	//common
	UINT8	nPqlBlkIdx;
	UINT8	nPqlBlkItemsIdx;
	UINT8	nPqlInputItemsIdx;
	//for Store Option
	UINT8	nStoreOptionIdx;
	//for 3D
	UINT8	nPqlTridIdx;

	//final
	UINT8	nPqlResIndex;
	PQL_INDEX_MEMBER_T	*pstPqlCoeffIdIdx;
}PQL_DB_IDXTBL_DET_FACTOR_T;


/**
 * Match setting value to chip ( or driver )'s characteristic.
*/
typedef struct {

	float	fRGBGainVal[3];		/* 0.6160 ~ 1.0000 ~ 1.1260 : step = 0.002	*/
	float	dwRGBGain[3];		/* 0.6160 ~ 1.0000 ~ 1.1260				*/

	float	fRGBOffsetVal[3];		/* -64 ~ 0 ~ 63 : step = 1bit				*/
	float	dwRGBOffset[3];		/* -0.1600 ~ 0.0000 ~ 0.1575				*/

} PQL_ADC_ADAPT_T;

/**
 * Match OSD value to setting value.
*/
typedef struct {

	float	fRGBGainOSD[3];		/* 0 ~ 192 ~ 255 : default value is 192.	*/
	float	fRGBGainVal[3];		/* 0.616 ~ 1.000 ~ 1.126 : step = 0.002	*/

	float	fRGBOffsetOSD[3];	/* 0 ~ 64 ~ 127 : default value is 64.		*/
	float	fRGBOffsetVal[3];		/* -64 ~ 0 ~ 63 : step = 1bit			*/

} PQL_ADC_CTRL_T;



/**
 * color temperature information
 * this will be used for getting ct_csc's coefficients
*/
typedef struct PQL_GAIN_OFFSET
{
	float fGainR;
	float fGainG;
	float fGainB;
	float fOffsetR;
	float fOffsetG;
	float fOffsetB;
} PQL_GAIN_OFFSET_T;

/**
 * YCDelay : CM Level Adjust struct.
 *
 * @see
 */
typedef struct PQL_YC_DELAY_ADJ
{
    UINT8 pqlTvdYDelay;
    UINT8 pqlTvdCDelay;
    UINT8 pqlColorYDelay;
    UINT8 pqlColorCbDelay;
    UINT8 pqlColorCrDelay;
}PQL_YC_DELAY_ADJ_T;


/**
 * PQ_Gain_Table�� UID ���� ��ҿ� Tool Option �� �� return ��.
 * @note BEST > GOOD > MISMATCH �̾�� ��.
 */

typedef enum {
	PQ_UID_ELEMENT_MISMATCH		= 0,
	PQ_UID_ELEMENT_GOOD_MATCH	= 1,
	PQ_UID_ELEMENT_BEST_MATCH	= 2
} PQ_Tbl_ELEMENT_MATCH_T;

/**
 * Gamma Block Position : Before or After Color Temperature block, dependent on the platform design
 *
 * @see
 */
typedef enum PQL_EXP_GAMMA_BLOCK_POSITION
{
	PQL_EXP_GAMMA_BEFORE_COLORTEMP = 0,
	PQL_EXP_GAMMA_AFTER_COLORTEMP,
}PQL_EXP_GAMMA_BLOCK_POSITION_T;

/**
 * Gamma Block for Expert Mode : Main SOC, FRC, T-Con
 *
 * @see
 */
typedef enum PQL_EXP_GAMMA_BLOCK
{
	PQL_EXP_GAMMA_MAINSOC = 0,
	PQL_EXP_GAMMA_FRC,
	PQL_EXP_GAMMA_TCON,
}PQL_EXP_GAMMA_BLOCK_T;


/*------------------------------------------------------------------------------
	XD Plasma 3.0 ����	[vivakjh] 2009/09/17
>> START ----------------------------------------------------------------------*/
/**
*	XD-Plasma 3.0  Color Param stucture.
*/
typedef struct PQL_XDP_COLOR_PARAM {
	PQL_PATH_T pqlPath;
	PQL_STATUS_T pqlStatus;
	UINT8 uPqlXDPColorCtrlIndex;
	UINT8 uiDynamicColorVal;				/**< UI Value */

	//XD Plasma 3.0 Color
	UINT8 *pstXDPColorTable;
	//3D Formatter XD Plasma Fresh White
	UINT16 *pstFmterXDPColorTable;
}PQL_XDP_COLOR_PARAM_T;


/**
*	XD-Plasma 3.0  Fresh White Param stucture.
*/
typedef struct PQL_XDP_FW_PARAM {
	PQL_PATH_T pqlPath;
	PQL_STATUS_T pqlStatus;
	UINT8 uPqlXDPFWCtrlIndex;
	UINT8 uiDynamicColVal;				/**< UI Value */

	//XD Plasma Fresh White
	UINT8 *pstXDPFWTable;
	//3D Formatter XD Plasma Fresh White
	UINT16 *pstFmterXDPFWTable;
}PQL_XDP_FW_PARAM_T;


/**
*	XD-Plasma 3.0  Sharpness Param stucture.
*/
typedef struct PQL_XDP_SHP_PARAM {
	PQL_PATH_T pqlPath;
	PQL_STATUS_T pqlStatus;
	UINT8 uPqlXDPShpCtrlIndex;
	UINT8 uiEdgeEnhanceValue;				/**< UI Value */

	//XD Plasma Fresh White
	UINT8 *pstXDPShpTable;
	//3D Formatter XD Plasma Fresh White
	UINT16 *pstFmterXDPShpTable;
}PQL_XDP_SHP_PARAM_T;

/*Luminance senor lux dimming mapping curve*/
#define PQL_EYEQ_LUX_DIM_CURVE_MAX 7

typedef enum PQL_EYEQ_MODE{
	PQL_EYEQ_INTELLIGENT_MODE = 0,
	PQL_EYEQ_ENERGYSAVE_MODE,
}PQL_EYEQ_MODE_T;

typedef struct {
	SINT32 u16Lux[PQL_EYEQ_LUX_DIM_CURVE_MAX];
	SINT32 u16Dimming[PQL_EYEQ_LUX_DIM_CURVE_MAX];
}PQ_LUX_DIM_CURVE_T;

typedef struct {
	PQ_LUX_DIM_CURVE_T stIntelligentMode;
	PQ_LUX_DIM_CURVE_T stEnergySavingMode;
}PQ_LUX_DIM_MODE_T;

/* >> END ----------------------------------------------------------------------*/


/*******************************************************************************
	Extern ���������� �Լ� prototype ����
	(Extern Variables & Function Prototype Declarations)
*******************************************************************************/

/******************************************************************************
	�Լ� ���� (Function Declaration)
******************************************************************************/

/* Functions for PQL module(drivers/pql/) internally */
/*** pqldb_ddi.c *****************************************************************/
DTV_STATUS_T PQL_GetSocCoeffID(UINT8 pqlPort, PQL_STATUS_T pqlStatus, \
											PQL_SOC_COEFF_ID_T pqlIdCoeff, PQL_INDEX_MEMBER_T *pPqlIndexMember);



/***************************************************************************/
/**************** 	LG DDI 		********************************************/
/***************************************************************************/

void DDI_PQL_Init(void);

BOOLEAN DDI_PQL_GetSceneChangeStatus(void);

PQL_WB_FACTORY_DATA_T DDI_PQL_GetDftWBData(void);
float DDI_PQL_GetWBGainFromAdjWB(UINT8 u8UIvalue);
UINT8 DDI_PQL_GetDftSubBrightness(void);

/* Sharpness */
DTV_STATUS_T DDI_PQL_SetSharpByUserParam(PQL_SHARP_UI_VALUE_T sPqUiVal, PQL_STATUS_T pqlStatus);

/* NR */
DTV_STATUS_T DDI_PQL_SetNRByUserControl(PQL_STATUS_T pqlStatus, PQL_NR_LEVEL_T pqlNrLevel);
/* xvYCC */
BOOLEAN DDI_PQL_GetxvYCCStatus(void);
UINT8 DDI_PQL_GetQuantRangeStatus(void);
/* Timing Change */
void DDI_PQL_SetForTimmingChange(PQL_STATUS_T pqlStatus);

/* PC */
void DDI_PQL_GetAdjSubBrightCont( PQL_SUB_CONTBRI_T *pAdjSubBriCont );
void DDI_PQL_SetAdjSubBrightCont( PQL_SUB_CONTBRI_T adjSubBriCont );

/* Video DDI */
PQL_AVDECODER_VIDEO_MODE_T DDI_PQL_GetColorSystem(void);
DTV_STATUS_T DDI_PQL_GetHDMIColorDomain(PQL_HDMI_COLOR_DOMAIN_T *pqlHDMIColorDomain);
DTV_STATUS_T DDI_PQL_GetHDMIQuantRange(PQL_HDMI_QUANTIZATION_RANGE_T *pPqlHDMIQuantRange);

/* check auto wb tick time */
UINT8 DDI_PQL_CheckAutoWbTickTime(void);

/* Set Test Pattern Status */
void DDI_PQL_SetTestPatternStatus(BOOLEAN bOnOff);
DTV_STATUS_T DDI_PQL_SetColorTemp(PQL_COLOR_TEMP_T cscPcCtData);
DTV_STATUS_T DDI_PQL_SetFactoryStatus(BOOLEAN bFactoryStatus);

/* Eye Q Sensor Gain */
PQ_Tbl_ELEMENT_MATCH_T DDI_PQL_CheckUidElementMatch( UINT32 sourceUid, UINT32 targetUid, UINT32 mask, UINT32 pos);
UINT32 DDI_PQL_GetPQTblIndex(UINT32 pqUid);
UINT32 DDI_PQL_GetEyeQSensorGain(UINT32 uToolType, UINT32 uModuleInch);
void DDI_PQL_GetEyeQLuxDimCurve(UINT32 uToolType, UINT32 uModuleInch, PQL_EYEQ_MODE_T EyeqMode, PQ_LUX_DIM_CURVE_T* stEyeQLuxDimCurve);
UINT32 DDI_PQL_GetEyeQSensorInitValue(UINT32 uToolType, UINT32 uModuleInch);
UINT32 DDI_PQL_GetEyeQSensorADCGain(UINT32 uToolType, UINT32 uModuleInch);
float DDI_PQL_GetEyeQSensorStrength(UINT32 uToolType, int sensorVal);
void DDI_PQL_SetSensorType(UINT8 sensorType);



UINT16 DDI_PQL_GetEyeQTblId(UINT32 idx);
UINT32 DDI_PQL_GetEyeQTblGain(UINT32 idx);
UINT32 DDI_PQL_GetEyeQTblSize(void);

void DDI_PQL_InitGammaBlockInfo(void);
PQL_EXP_GAMMA_BLOCK_T DDI_PQL_GetExpGammaBlock(void);
PQL_EXP_GAMMA_BLOCK_POSITION_T DDI_PQL_GetExpGammaBlockPosition(void);
UINT32 DDI_PQL_GetHistToal(PQL_DC2P_RAWDATA_INFO_T* histData);
DTV_STATUS_T DDI_PQL_SetFreshWhiteData(UINT8 uFreshColorUIVal);


/***************************************************************************/
/**************** 	DB DDI	(defined in pql_ddi_db.c)	********************/
/***************************************************************************/

void DDI_PQL_DB_InitDB(PQL_CONFIG_T tempPql);
void DDI_PQL_DB_InitDBIndex(PQL_CONFIG_T tempPql);
void DDI_PQL_DB_GetCoeffParam(UINT32 pqlIdCoeff, PQL_INPUT_SRC_FMT_T pqlSrcFmt, PQL_INDEX_MEMBER_T *pPqlIndexMember);
const UINT8 * DDI_PQL_DB_GetDbString(void);

UINT8 DDI_PQL_DB_GetDBEyeQOSDValue( PQL_EYEQ_DB_MEMBER_T memberID, UINT8 index, UINT16 uEyeQStep);
SINT32 DDI_PQL_DB_GetDBDimmingCtrl(PQL_DIMMING_DB_MEMBER_T  memberID, UINT8 index, UINT8 uCurveNum);
SINT32 DDI_PQL_DB_GetDBDimmingAPLCtrl(PQL_DIMMING_DB_MEMBER_T memberID,UINT8 index,UINT8 uiValue,UINT8 uCurveNum);
void DDI_PQL_DB_GetDBEyecareDimmingCtrl(UINT8 index, PQL_EYECARE_DIMMING_DB_T *pDataBase);
void DDI_PQL_DB_GetDBClearPlusDimCtrl(UINT8 index, PQL_CLEARP_DIMMING_DB_T *pClearpDimTbl);
#ifdef INCLUDE_MOTION_EYECARE
void DDI_PQL_DB_GetDBMotionDimmingCtrl(UINT8 index, PQL_MOTION_CTRL_T *pMotionCtrl, PQL_MOTION_DIMMING_DATA_T *pMotionHighApl, PQL_MOTION_DIMMING_DATA_T *pMotionLowApl);
#endif

DTV_STATUS_T DDI_PQL_DB_GetBrightContFor10pWB( PQL_PC_UI_VALUE_T pcUiVal, UINT8 pcId, float *pfContrast, float *pfBrightness );
DTV_STATUS_T DDI_PQL_DB_GetCorrectValOfWBGainOffset(UINT8 index, PQL_WB_CORRECT_DATA_T *pWBCorrectVal);
BOOLEAN DDI_PQL_DB_CheckCoeffIdValid( UINT32 pqlIdCoeff );



/***************************************************************************/
/**************** 	DC2P DDI	(defined in pql_dc2plus.c)	********************/
/***************************************************************************/

void DDI_PQL_DC2P_EnhanceContrast( UINT8 pqlPath, PQL_STATUS_T pqlStatus, PQL_DC2P_RAWDATA_INFO_T* pHistDc2p, UINT32 nHistTotal);
SINT32 DDI_PQL_DC2P_InterpolVal4DC2P(SINT32 inVal, SINT32 tableIn1, SINT32 tableIn2, SINT32 tableOut1, SINT32 tableOut2);
SINT32 DDI_PQL_DC2P_GetOutValByInterpol4DC2P(SINT32 inVal, SINT32 *pInTable, SINT32 *pOutTable, UINT8 min, UINT8 max);
void DDI_PQL_DC2P_CtrlFreshContrast(UINT8 pqlPath,PQL_STATUS_T pqlStatus,UINT8 uUIFreshContrast);
void DDI_PQL_DC2P_DisableFreshContrast(UINT8 pqlPath);
void DDI_PQL_DC2P_SetFlagId(BOOLEAN bUpdateFlag);
void DDI_PQL_DC2P_SetStoreCheck(BOOLEAN bStoreCheck);
void DDI_PQL_DC2P_SetContrastVal(UINT8 contrast, UINT8 pqlPath);
void DDI_PQL_DC2P_InitFreshContrastVal(UINT8 pqlPath);
UINT8 DDI_PQL_DC2P_GetHistogramType (UINT16 *puCurrentBin);
BOOLEAN DDI_PQL_DC2P_IsSceneChanged(UINT16 *puPqlBin);
void DDI_PQL_DC2P_SetAPSMotionGain(UINT16 uMotionGain);

/***************************************************************************/
/****************PQL Engine Color DDI(defined in pql_color.c)***************/
/***************************************************************************/
int DDI_PQLENGINE_SetClearWhite(PQL_CM_DYNAMICWHITE_PARAM_T *pstClearWhite);
int DDI_PQLENGINE_DynamicClearWhite(PQL_DC2P_RAWDATA_INFO_T histoData, UINT8 histoOnOff);
/***************************************************************************/
/****************PQL Engine NoiseReduction DDI(defined in pql_NR.c)*********/
/***************************************************************************/
void DDI_PQLENGINE_DynamicNoiseReduction(void);
void DDI_PQLENGINE_SetDynamicNR(PQL_NR_CTRL_PARAM_T stNRCtrlParam);
float DDI_PQLENGINE_SetDCGainByNoiseLevel(UINT8 uNoiseLevel);
float DDI_PQLENGINE_GetDCGainByNoiseLevel(void);


/***************************************************************************/
/**************** 	SOC DDI (defined in pql_soc.c)	************************/
/***************************************************************************/

/* Picture Control */
DTV_STATUS_T DDI_PQL_SOC_SetPictureControl(PQL_PC_UI_VALUE_T sPcUiVal, UINT8  sPqCoeffId, PQL_PATH_T pqlPath);
DTV_STATUS_T DDI_PQL_SOC_SetColorTemp(PQL_COLOR_TEMP_T cscPcCtData);

/* Sharpness */
DTV_STATUS_T DDI_PQL_SOC_SetSuperResParam(PQL_SR_PARAM_T *pstSRParam);
DTV_STATUS_T DDI_PQL_SOC_SetSharpByUserParam(PQL_SHARP_UI_VALUE_T sPqUiVal, PQL_STATUS_T pqlStatus);
DTV_STATUS_T DDI_PQL_SOC_SetSharpSystemParam(PQL_STATUS_T pqlStatus, PQL_PATH_T pqlPath);
DTV_STATUS_T DDI_PQL_SOC_SetSharpByEdgeEnhance(PQL_EDGE_ENHANCE_UI_T sUiEdEnhan, PQL_PATH_T pqlPath, PQL_STATUS_T pqlStatus);
DTV_STATUS_T DDI_PQL_SetSharpByUserControl(PQL_SHARP_UI_VALUE_T	 sPqUiVal, PQL_STATUS_T pqlStatus);


/* DC */
DTV_STATUS_T DDI_PQL_SOC_Init(void);
DTV_STATUS_T DDI_PQL_SOC_SetDefaultBySystem(void);
DTV_STATUS_T  DDI_PQL_SOC_InitDCChipDataTypeInfo(void);
#if 1
PQL_DC2P_RAWDATA_SUPPORT_INFO_T DDI_PQL_SOC_GetDCChipDataTypeInfo(void);
#else
DTV_STATUS_T DDI_PQL_SOC_GetDCChipDataTypeInfo(UINT32 *pChipDataType);
#endif
DTV_STATUS_T DDI_PQL_SOC_GetDCChipRawData(PQL_DC2P_RAWDATA_SUPPORT_INFO_T *pHistoSupportInfo, PQL_DC2P_RAWDATA_INFO_T *pHistoInfo);
UINT32 		 DDI_PQL_SOC_GetMotionValFrHisto(PQL_DC2P_RAWDATA_INFO_T *pHistoInfo);

DTV_STATUS_T DDI_PQL_SOC_SetDCFreshContrast(PQL_DC2P_CTRL_T *pCtrlInfo);
DTV_STATUS_T DDI_PQL_SOC_SetDCFreshContrastLUT(SINT32	*pAccLUT);
DTV_STATUS_T DDI_PQL_SOC_SetAdaptiveCGain(SINT32 *pCGainParam);	//added by request of cho04
#if(PLATFORM_TYPE == LG_PLATFORM)
DTV_STATUS_T DDI_PQL_SOC_SetSceneChange(BOOLEAN bIsSceneChange); // 201226 kyungryun.lee added for DCR Scene Change
#endif

/* Dynamic Color */
DTV_STATUS_T DDI_PQL_SOC_InitFreshColorParam(PQL_CM_FRESHCOLOR_PARAM_T *pPqlFreshColorParam);
DTV_STATUS_T DDI_PQL_SOC_SetFreshColor(PQL_CM_FRESHCOLOR_PARAM_T pqlFreshColorParam);

DTV_STATUS_T DDI_PQL_SOC_SetSkinColor(PQL_CM_SKINCOLOR_PARAM_T *pstSkinColor);
DTV_STATUS_T DDI_PQL_SOC_SetGrassColor(PQL_CM_GRASSCOLOR_PARAM_T *pstGrassColor);
DTV_STATUS_T DDI_PQL_SOC_SetBlueSkyColor(PQL_CM_BLUESKYCOLOR_PARAM_T *pstBSkyColor);
//DTV_STATUS_T DDI_PQL_SOC_SetDynamicWhite(PQL_CM_DYNAMICWHITE_PARAM_T *pstDynamicWhite);
DTV_STATUS_T DDI_PQL_SOC_Set3DColorCorrection(BOOLEAN bOnOff);

/* Noise Reduction */
DTV_STATUS_T DDI_PQL_SOC_SetNRByUserParam(PQL_NR_CTRL_PARAM_T *pstNrCtrlParam);
DTV_STATUS_T DDI_PQL_SOC_SetNRSystemParam(PQL_STATUS_T pqlStatus, PQL_PATH_T pqlPath);
DTV_STATUS_T DDI_PQL_SOC_SetMPEGNRParam(PQL_NR_MPEGNR_PARAM_T *pstMpegNRParam);	//KWON_0618 Parameter change
DTV_STATUS_T DDI_PQL_SOC_GetNoiseLevel(UINT16* uLevel); // Choonghoon Park Dynamic NR
DTV_STATUS_T DDI_PQL_SOC_GetMotionLevel(UINT32* uLevel); // Choonghoon Park Dynamic NR

/* Deinterlace */
DTV_STATUS_T DDI_PQL_SOC_SetDeinterParam(UINT8 *pMDCoeffId, PQL_PATH_T pqlPath, PQL_STATUS_T pqlStatus);
DTV_STATUS_T DDI_PQL_SOC_SetFilmMode(PQL_FILM_CTRL_T *pCtrlData, PQL_STATUS_T pqlStatus, PQL_PATH_T  pqlPath);

/* Blacklevel */
DTV_STATUS_T DDI_PQL_SOC_SetBlackLevel(PQL_CSC_LEVEL_CTRL_T *pstUserBlackLevelCtrl);

/* Gamma */
DTV_STATUS_T DDI_PQL_SOC_SetGamma(PQL_PATH_T pqlPath, UINT32 *pRedGammaTable, UINT32 *pGreenGammaTable, UINT32 *pBlueGammaTable);

/* Color Gamut */
DTV_STATUS_T DDI_PQL_SOC_SetColorGamut(PQL_COLORGAMUT_CTRL_T *pqlColorGamut);
DTV_STATUS_T DDI_PQL_SOC_SetxvYCCData(PQL_COLORGAMUT_CTRL_T *pqlColorGamut);

/* Color Filter */
DTV_STATUS_T DDI_PQL_SOC_SetColorFilter(UINT8 uSelectColorFilter);

/* Color Manage */
DTV_STATUS_T DDI_PQL_SOC_InitExpColorParam(PQL_CM_EXPCOLOR_PARAM_T *pPqlExpColorParam);
DTV_STATUS_T DDI_PQL_SOC_SetExpColorParam(UINT8 pqlCMCtrl, PQL_CM_EXPCOLOR_PARAM_T pqlExpColorParam);

/* Demo Mode */
DTV_STATUS_T DDI_PQL_SOC_SetSplitDemo( BOOLEAN bOnOff);

/* Game Mode */
DTV_STATUS_T DDI_PQL_SOC_SetGameMode(BOOLEAN bGameMode);


#if(PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_PQL_SOC_MTK_SetDCFreshContrastSkinLUT(SINT32 *pAccLUT);
DTV_STATUS_T DDI_PQL_SOC_MTK_SetLevelAdj(/*PQL_CM_LEVEL_ADJ_T pqlLevelParam,*/ PQL_STATUS_T pqlStatus, PQL_PATH_T pqlPath);
DTV_STATUS_T PQL_MTK_SetColorCurve(UINT8 pqlColorIndex, UINT8 * pqlColorParam);
DTV_STATUS_T PQL_MTK_SetColorGain(PQL_PC_UI_VALUE_T sPcUiVal, UINT8  sPqCoeffId, PQL_PATH_T pqlPath, SINT32 nColorGain);
DTV_STATUS_T DDI_PQL_SOC_MTK_SetColorGain(/*PQL_PC_UI_VALUE_T sPcUiVal, UINT8 sPqCoeffId,*/ PQL_PATH_T pqlPath, SINT32 nColorGain);
DTV_STATUS_T DDI_PQL_SOC_MTK_SetYCDelay(/*PQL_YC_DELAY_ADJ_T pqlDelayVal,*/ PQL_STATUS_T pqlStatus, PQL_PATH_T pqlPath);
DTV_STATUS_T PQL_MTK_SetAdaptiveCGain(SINT32 *pCGainParam);
DTV_STATUS_T PQL_MTK_SetBlackLevelBypass(void);
DTV_STATUS_T DDI_PQL_SOC_MTK_GetFilmModeInfo(UINT8 *FilmStatus);
DTV_STATUS_T DDI_PQL_SOC_MTK_SetColorWindow(CHIP_COLORWINDOW_T* pColorWindowSetting);
DTV_STATUS_T DDI_PQL_SOC_MTK_SetYBitPlusSetting(UINT8 uUIFreshContrast, PQL_DC2P_YBITPLUS_DB_T stDC2PYBitPlusData);

DTV_STATUS_T DDI_PQL_SOC_MTK_OPC_GetStatus(UINT8* uGain, UINT8* uTargetPwm, UINT8* uCurPwm);
DTV_STATUS_T DDI_PQL_SOC_MTK_OPC_SetGain(UINT8 uGain);
DTV_STATUS_T DDI_PQL_SOC_MTK_OPC_SetParam(CHIP_OPC_PARAM_T* pOPCSetting);
DTV_STATUS_T DDI_PQL_SOC_MTK_OPC_Enable(BOOLEAN bOnOff);
DTV_STATUS_T DDI_PQL_SOC_MTK_OPC_SetAPPWM(UINT8 u1APPWM);
#endif


#if(PLATFORM_TYPE == LG_PLATFORM)
DTV_STATUS_T DDI_PQL_SOC_L9_SetColorGain(PQL_PATH_T pqlPath, SINT32 nColorGain);
#endif


/* XD Plasma PDP Only */
// [ vivakjh] 2010/08/13 XD Plasma 2.0 Spec  ������. DTV_STATUS_T DDI_PQL_SetXDPCC(UINT8 nUiValue, PQL_PATH_T pqlPath, PQL_STATUS_T pqlStatus);			//[vivakjh] 2009/02/23  PDP XD Plasma ����.
DTV_STATUS_T DDI_PQL_SetXDPFreshWhite(UINT8 nUiValue, PQL_PATH_T pqlPath, PQL_STATUS_T pqlStatus);	//[vivakjh] 2009/02/23  PDP XD Plasma ����.
DTV_STATUS_T DDI_PQL_SetXDPColor(UINT8 nUiValue, PQL_PATH_T pqlPath, PQL_STATUS_T pqlStatus);			//[vivakjh] 2009/09/19  PDP XD Plasma 3.0 ����.
DTV_STATUS_T DDI_PQL_SetXDPSharpness(UINT8 nUiValue, PQL_PATH_T pqlPath, PQL_STATUS_T pqlStatus);		//[vivakjh] 2009/02/23  PDP XD Plasma ����.
void DDI_PQL_ReloadXDPlasmaData(void);
DTV_STATUS_T DDI_PQL_SetFreshWhiteData(UINT8 uFreshColorUIVal);
DTV_STATUS_T DDI_PQL_GetAShpIACurve(PQL_PATH_T pqlPath, PQL_STATUS_T pqlStatus, UINT8 uHistoType, PQL_ASHP_IA_CURVE_CTRL_T *pstAShpIaCurve);
UINT8 		 DDI_PQL_GetAPSContrast(PQL_PATH_T pqlPath, PQL_STATUS_T pqlStatus);
UINT8 		 DDI_PQL_GetCurrentAPSContrast(void);
void 		 DDI_PQL_SetAdjAPSContrast(UINT8 uAdjAPSContrast);
DTV_STATUS_T DDI_PQL_SetGamma(PQL_PATH_T pqlPath, PQL_GAMMA_BLOCK_T eGammaBlock, UINT32 *pRedGammaTable, UINT32 *pGreenGammaTable, UINT32 *pBlueGammaTable);
//DTV_STATUS_T DDI_PQL_SetSuperResParam(PQL_SR_PARAM_T *pstSRParam);
void 		DDI_PQL_SetCurrentAdaptiveWBVal(UINT8 nWBValue);
void 		DDI_PQL_SetCurrentAdaptiveWBOnOff(BOOLEAN bWBmode);
DTV_STATUS_T DDI_PQL_SetGameMode(BOOLEAN bGameMode);


#ifdef __cplusplus
}
#endif

#endif	/* __PQL_DDI_H_ */
