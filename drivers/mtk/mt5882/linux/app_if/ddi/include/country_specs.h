/******************************************************************************
 *	 DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *	 Copyright(c) 2008 by LG Electronics Inc.
 *
 *	 All rights reserved. No part of this work may be reproduced, stored in a
 *	 retrieval system, or transmitted by any means without prior written
 *	 permission of LG Electronics Inc.
 *****************************************************************************/

/** @file country_specs.h
 *
 * This header file assigns and initializes country specification arrays.
 *
 *	@author 	David (DONG-IL) Lee
 *	@version	1.0
 *	@date		2008.05.08
 *	@note		This file shall be included only in country.h
 *	@see		ISO 3166-1 (Exception: UK <- GB)
 */
#ifndef WIN32
#ifndef _COUNTRY_H_
	#error	NOTE: This file should be included only in country.h
#endif
#endif

#ifndef	_COUNTRY_SPECS_H_
#define	_COUNTRY_SPECS_H_
/*	country code,	code name,	patpmtsdt_map,	mheg5,	ci,	oad,	lcn,	vhf,	nordig,	dtv,	acap,	dvr_ready,	ttx_mode,	acms,	dAboost,	mono,	i_ii_save,	init_menu_lang,	satellite,	cable_digital,	cable_analog,	acc,	dcc,	bluetooth,	wss_support,	attx,	dttx,	ttx_subt,	dvb_subt, adcc, psip , si, 	_reserved,	country_order,	full_name	*/
/*								 code_name																*/
/*								 ||,  patpmtsdt_map														*/
/*								 ||,  |,mheg5															*/
/*								 ||,  |,|,ci															*/
/*								 ||,  |,|,|,oad															*/
/*								 ||,  |,|,|,|,lcn														*/
/*								 ||,  |,|,|,|,|,vhf														*/
/*								 ||,  |,|,|,|,|,|,nordig												*/
/*								 ||,  |,|,|,|,|,|,|,dtv													*/
/*								 ||,  |,|,|,|,|,|,|,|,acap												*/
/*								 ||,  |,|,|,|,|,|,|,|,|,dvr_ready										*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,ttx_mode										*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,acms										*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,dAboost									*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,mono									*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,i_ii_save								*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,init_menu_lang						*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,satellite						*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,cable_digital					*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,cable_analog					*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,acc							*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,dcc						*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,exclusive_dtv_cadtv				*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,wss_support			*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,attx				*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,dttx				*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,ttx_subt		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,dvb_subt		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,adcc		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,psip		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,si		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,ci_plus	*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,counrty_order							*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,	| |						 	 |full_name */
/*								 vv,  v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,	v v							 v			*/
RMM_COUNTRY_ELMT_T aCountryList_EU[] = {
	{RMM_COUNTRY_CODE_UK,		"GB",{0,1,1,1,1,0,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,0,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_UK, "United Kingdom"},
	{RMM_COUNTRY_CODE_FR,		"FR",{0,0,1,0,1,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_FR, "France"},
	{RMM_COUNTRY_CODE_DE,		"DE",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_DE, "Germany"},
	{RMM_COUNTRY_CODE_IT,		"IT",{0,0,1,0,1,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_IT, "Italy"},
	{RMM_COUNTRY_CODE_ES,		"ES",{1,0,1,1,0,0,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_ES, "Spain"},
	{RMM_COUNTRY_CODE_SE,		"SE",{0,0,1,1,1,1,1,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_SE, "Sweden"},
	{RMM_COUNTRY_CODE_FI,		"FI",{0,0,1,1,1,1,1,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_FI, "Finland"},
	{RMM_COUNTRY_CODE_AT,		"AT",{1,0,1,1,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_AT, "Austria"},
	{RMM_COUNTRY_CODE_BE,		"BE",{1,0,1,1,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_BE, "Belgium"},
	{RMM_COUNTRY_CODE_CZ,		"CZ",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_CZ, "Czech Republic"},
	{RMM_COUNTRY_CODE_DK,		"DK",{0,0,1,1,1,1,1,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_DK, "Denmark"},
	{RMM_COUNTRY_CODE_GR,		"GR",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_GR, "Greece"},
	{RMM_COUNTRY_CODE_LU,		"LU",{1,0,1,1,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_LU, "Luxembourg"},
	{RMM_COUNTRY_CODE_NL,		"NL",{1,0,1,1,1,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_NL, "Netherlands"},
	{RMM_COUNTRY_CODE_BG,		"BG",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_BG, "Bulgaria"},
	{RMM_COUNTRY_CODE_HR,		"HR",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_HR, "Croatia"},
	{RMM_COUNTRY_CODE_HU,		"HU",{1,0,1,0,1,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_HU, "Hungary"},
	{RMM_COUNTRY_CODE_IE,		"IE",{0,1,1,1,1,1,1,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_IE, "Ireland"},
	{RMM_COUNTRY_CODE_NO,		"NO",{0,0,1,0,1,1,1,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_NO, "Norway"},
	{RMM_COUNTRY_CODE_PL,		"PL",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_PL, "Poland"},
	{RMM_COUNTRY_CODE_PT,		"PT",{1,0,1,0,1,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_PT, "Portugal"},
	{RMM_COUNTRY_CODE_RO,		"RO",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_RO, "Romania"},
	{RMM_COUNTRY_CODE_RU,		"RU",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_RU, "Russian Federation"},
	{RMM_COUNTRY_CODE_RS,		"RS",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_RS, "Serbia"},
	{RMM_COUNTRY_CODE_SI,		"SI",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_SI, "Slovenia"},
	{RMM_COUNTRY_CODE_CH,		"CH",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_CH, "Switzerland"},
	{RMM_COUNTRY_CODE_TR,		"TR",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_TR, "Turkey"},
	{RMM_COUNTRY_CODE_MA,		"MA",{1,0,1,0,1,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_MA, "Morocco"},
	{RMM_COUNTRY_CODE_UA,		"UA",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_UA, "Ukraine"},
	{RMM_COUNTRY_CODE_KZ,		"KZ",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_KZ, "Kazakhstan"},
	{RMM_COUNTRY_CODE_LT,		"LT",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_LT, "Lithuania"},
	{RMM_COUNTRY_CODE_EE,		"EE",{1,0,1,0,1,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_EE, "Estonia"},//090213
	{RMM_COUNTRY_CODE_LV,		"LV",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_LV, "Latvia"},
	{RMM_COUNTRY_CODE_SK,		"SK",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_SK, "Slovakia"},
	{RMM_COUNTRY_CODE_AL,		"AL",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_AL, "Albania"},
	{RMM_COUNTRY_CODE_BA,		"BA",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_BA, "Bosnia and Herzegovina"},
	{RMM_COUNTRY_CODE_BY,		"BY",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER_BY, "Belarus"},
	{RMM_COUNTRY_CODE___,		"__",{1,0,1,0,0,1,0,1,0,1,0,1,1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,0,0,1,1}, (UINT8)RMM_COUNTRY_ORDER___, "__"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
/*								 ||,  patpmtsdt_map														*/
/*								 ||,  |,mheg5															*/
/*								 ||,  |,|,ci															*/
/*								 ||,  |,|,|,oad															*/
/*								 ||,  |,|,|,|,lcn														*/
/*								 ||,  |,|,|,|,|,vhf														*/
/*								 ||,  |,|,|,|,|,|,nordig												*/
/*								 ||,  |,|,|,|,|,|,|,dtv													*/
/*								 ||,  |,|,|,|,|,|,|,|,acap												*/
/*								 ||,  |,|,|,|,|,|,|,|,|,dvr_ready										*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,ttx_mode										*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,acms										*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,dAboost									*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,mono									*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,i_ii_save								*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,init_menu_lang						*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,satellite						*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,cable_digital					*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,cable_analog					*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,acc							*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,dcc						*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,exclusive_dtv_cadtv				*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,wss_support			*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,attx				*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,dttx				*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,ttx_subt		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,dvb_subt		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,adcc		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,psip		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,si		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,ci_plus	*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,counrty_order							*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,	| |						 	 |full_name */
/*								 vv,  v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,	v v							 v			*/
RMM_COUNTRY_ELMT_T aCountryList_KR[] = {
	{RMM_COUNTRY_CODE_KR,		"KR",{0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Korea, Republic of"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_US[] = {
	{RMM_COUNTRY_CODE_US,		"US",{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "United States"},
	{RMM_COUNTRY_CODE_CA,		"CA",{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Canada"},
	{RMM_COUNTRY_CODE_MX,		"MX",{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Mexico"},
	{RMM_COUNTRY_CODE_PH,		"PH",{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Philippines"},
	{RMM_COUNTRY_CODE_HN,		"HN",{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Honduras"},	
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
//zhangze 0718 for China city option
RMM_COUNTRY_ELMT_T aCountryList_CN[] = {
	{RMM_COUNTRY_CODE_CN,   	"CN",{1,0,1,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "China"},
	{RMM_COUNTRY_CODE_HK,		"HK",{0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Hong Kong"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
/*zhangze 20101120 added Hong Kong group*/
//zhangze 0718 for China city option
RMM_COUNTRY_ELMT_T aCountryList_HK[] = {
	{RMM_COUNTRY_CODE_CN,		"CN",{1,0,1,0,0,1,0,1,0,0,0,0,1,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "China"},
	{RMM_COUNTRY_CODE_HK,		"HK",{0,1,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Hong Kong"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_TW[] = {
	{RMM_COUNTRY_CODE_TW,		"TW",{1,0,0,1,0,0,0,1,0,0,0,0,1,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Taiwan"},
	{RMM_COUNTRY_CODE_CO,		"CO",{1,0,0,1,0,0,0,1,0,0,0,0,1,0,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Colombia"},
	{RMM_COUNTRY_CODE_PA,		"PA",{1,0,0,1,0,0,0,1,0,0,0,0,1,0,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Panama"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_BR[] = {
	{RMM_COUNTRY_CODE_BR,		"BR",{1,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Brazil"},
	{RMM_COUNTRY_CODE_CL,		"CL",{1,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Chile"},
	{RMM_COUNTRY_CODE_PE,		"PE",{1,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Peru"},
	{RMM_COUNTRY_CODE_AR,		"AR",{1,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Argentina"},
	{RMM_COUNTRY_CODE_EC,		"EC",{1,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Ecuador"},
#if 0	//�߳��� ���� ���� ����.
	{RMM_COUNTRY_CODE_VE,		"VE",{1,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Venezuela"},
	{RMM_COUNTRY_CODE_CR,		"CR",{1,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Costarica"},
	{RMM_COUNTRY_CODE_PY,		"PY",{1,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Paraguay"},
	{RMM_COUNTRY_CODE_BO,		"BO",{1,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Bolivia"},
#endif
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_JP[] = {
	{RMM_COUNTRY_CODE_JP,		"JP",{0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,1,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Japan"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_XA[] = {
	{RMM_COUNTRY_CODE_XA,		"XA",{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "[VIRTUAL] A-CSA"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_IL[] = {
	{RMM_COUNTRY_CODE_IL,		"IL",{1,0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Israel"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };

#ifndef ENABLE_AJJA_GROUP
RMM_COUNTRY_ELMT_T aCountryList_AU[] = {
#ifdef INCLUDE_SUPPORT_AUMHEG
	{RMM_COUNTRY_CODE_AU,		"AU",{0,1,0,1,1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Australia"},
#else
	{RMM_COUNTRY_CODE_AU,		"AU",{0,0,0,1,1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Australia"},
#endif
	{RMM_COUNTRY_CODE_NZ,		"NZ",{0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "New Zealand"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_SG[] = {
	{RMM_COUNTRY_CODE_SG,		"SG",{1,0,0,1,1,0,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Singapore"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_ZA[] = {
	{RMM_COUNTRY_CODE_ZA,		"ZA",{1,1,0,1,0,0,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "South Africa"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_VN[] = {
	{RMM_COUNTRY_CODE_VN,		"VN",{1,0,0,1,0,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Viet Nam"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_XB[] = {
	{RMM_COUNTRY_CODE_XB,		"XB",{0,0,0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "[VIRTUAL] A-ASIA"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_ID[] = {
	{RMM_COUNTRY_CODE_ID,		"ID",{1,0,0,1,0,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Indonesia"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_MY[] = {
	{RMM_COUNTRY_CODE_MY,		"MY",{1,1,0,1,1,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Malaysia"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_IR[] = {
	{RMM_COUNTRY_CODE_IR,		"IR",{1,0,0,1,1,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Iran"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
#else
/*								 ||,  patpmtsdt_map														*/
/*								 ||,  |,mheg5															*/
/*								 ||,  |,|,ci															*/
/*								 ||,  |,|,|,oad															*/
/*								 ||,  |,|,|,|,lcn														*/
/*								 ||,  |,|,|,|,|,vhf														*/
/*								 ||,  |,|,|,|,|,|,nordig												*/
/*								 ||,  |,|,|,|,|,|,|,dtv													*/
/*								 ||,  |,|,|,|,|,|,|,|,acap												*/
/*								 ||,  |,|,|,|,|,|,|,|,|,dvr_ready										*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,ttx_mode										*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,acms										*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,dAboost									*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,mono									*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,i_ii_save								*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,init_menu_lang						*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,satellite						*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,cable_digital					*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,cable_analog					*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,acc							*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,dcc						*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,exclusive_dtv_cadtv				*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,wss_support			*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,attx				*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,dttx				*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,ttx_subt		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,dvb_subt		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,adcc		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,psip		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,si		*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,ci_plus	*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,counrty_order							*/
/*								 ||,  |,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,|,	| |						 	 |full_name */
/*								 vv,  v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,v,	v v							 v			*/

RMM_COUNTRY_ELMT_T aCountryList_AJ[] = {
	{RMM_COUNTRY_CODE_AU,		"AU",{0,0,0,1,1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Australia"},
	{RMM_COUNTRY_CODE_NZ,		"NZ",{0,1,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "New Zealand"},
	{RMM_COUNTRY_CODE_SG,		"SG",{1,0,0,1,1,0,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Singapore"},
	{RMM_COUNTRY_CODE_VN,		"VN",{1,0,0,1,0,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Viet nam"},
	{RMM_COUNTRY_CODE_ID,		"ID",{1,0,0,1,0,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Indonesia"},
	{RMM_COUNTRY_CODE_MY,		"MY",{1,1,0,1,1,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Malaysia"},
	{RMM_COUNTRY_CODE_MM,		"MM",{1,0,0,1,0,0,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Myanmar"},
	{RMM_COUNTRY_CODE_LK,		"LK",{1,0,0,1,0,0,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Sri Lanka"},
	{RMM_COUNTRY_CODE_IN,		"IN",{0,0,0,0,0,0,0,0,0,0,2,1,0,0,1,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "India"},
	{RMM_COUNTRY_CODE_TH,		"TH",{0,0,0,0,0,0,0,0,0,0,2,1,0,0,1,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Thailand"},
	{RMM_COUNTRY_CODE_XB,		"XB",{0,0,0,0,0,0,0,0,0,0,2,1,0,0,1,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "[VIRTUAL] A-ASIA"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
RMM_COUNTRY_ELMT_T aCountryList_JA[] = {
	{RMM_COUNTRY_CODE_ZA,		"ZA",{1,1,0,1,0,0,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "South Africa"},
	{RMM_COUNTRY_CODE_IR,		"IR",{1,0,0,1,1,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Iran"},
	{RMM_COUNTRY_CODE_DZ,		"DZ",{1,0,0,0,0,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Algeria"},
	{RMM_COUNTRY_CODE_TN,		"TN",{1,0,0,0,0,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Tunisia"},
	{RMM_COUNTRY_CODE_PK,		"PK",{0,0,0,0,0,0,0,0,0,0,2,1,0,0,1,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "Pakistan"},
	{RMM_COUNTRY_CODE_XD,		"XD",{0,0,0,0,0,0,0,0,0,0,2,1,0,0,1,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "[VIRTUAL] A-ASIA"},
	{RMM_COUNTRY_CODE_XC,		"XC",{1,0,0,0,0,1,0,1,0,0,0,1,1,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "[VIRTUAL] D-ASIA"},
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, NULL} };
#endif
RMM_COUNTRY_ELMT_T aCountryList_ZZ[] = {
	{RMM_COUNTRY_CODE_ZZ,		NULL,{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, (UINT8)RMM_COUNTRY_ORDER_UNKNOWN, "[VIRTUAL] invalid group (terminator)"} };

//zhangze 0718 for China city option
RMM_CITY_ELMT_T aCityList_CN[] = {
#ifdef INCLUDE_1RF_CN
	{RMM_CITY_CODE_CNBJ,   "CNBJ",		"China Beijing"},
	{RMM_CITY_CODE_CNCC,  "CHANGCHUN",	"China Changchun Cable"},
	{RMM_CITY_CODE_CNCS,   "CHANGSHA",	"China Changsha PN"},
	{RMM_CITY_CODE_CNCD,   "CHENGDU",	"China Chengdu XW"},
	{RMM_CITY_CODE_CNDL,   "DALIAN", 	"China Dalian TT"},
	{RMM_CITY_CODE_CNFZ,   "FUZHOU", 	"China Fuzhou Cable"},
	{RMM_CITY_CODE_CNGZ,   "GUANGZHOU",	"China Guangzhou PN"},
	{RMM_CITY_CODE_CNHB,   "HEBEI",		"China Hebei PN"},
	{RMM_CITY_CODE_CNHF,   "HEFEI",		"China Hefei Cable"},
	{RMM_CITY_CODE_CNJL,   "JILIN",		"China JiLin PN"},//yinxianghua	 20091117
	{RMM_CITY_CODE_CNNB,   "NINGBO", 	"China Ningbo Cable"},
	{RMM_CITY_CODE_CNSY,   "SHENYANG",	"China Shenyang Cable"},
	{RMM_CITY_CODE_CNWH,   "WUHAN",		"China Wuhan GD"},
	{RMM_CITY_CODE_CNXA,   "XIAN",		"China Xian PN"},
	{RMM_CITY_CODE_CNXM,   "XIAMEN", 	"China Xiamen Cable"},
	{RMM_CITY_CODE_CNZZ,   "ZHENGZHOU",	"China Zhengzhou Cable"},//yinxianghua  20091203
	{RMM_CITY_CODE_CNZH,   "ZHUHAI", 	"China Zhuhai Cable"},
	//END
	{RMM_CITY_CODE_CNO,	   "CNO",		"China Others"},//xiangchen.yang 20111031 . reopen 20111226 add NoSelection
	{RMM_CITY_CODE_ZZ,		NULL,		NULL}

#else
	{RMM_CITY_CODE_CNBJ,	  "CNBJ",		"China Beijing"},
	//add by lihuifeng for cityoption 2009.10.06
	{RMM_CITY_CODE_CNCC,  	  "CHANGCHUN",  "China Changchun Cable"},
	{RMM_CITY_CODE_CNCS,	  "CHANGSHA",	"China Changsha PN"},
	{RMM_CITY_CODE_CNCD,	  "CHENGDU",	"China Chengdu XW"},
	{RMM_CITY_CODE_CNDL,	  "DALIAN",		"China Dalian TT"},
	{RMM_CITY_CODE_CNFZ,	  "FUZHOU",		"China Fuzhou Cable"},
	{RMM_CITY_CODE_CNGZ,	  "GUANGZHOU",	"China Guangzhou PN"},
	{RMM_CITY_CODE_CNHB,	  "HEBEI",		"China Hebei PN"},
	{RMM_CITY_CODE_CNHF,	  "HEFEI",		"China Hefei Cable"},
	{RMM_CITY_CODE_CNJL,	  "JILIN",		"China JiLin PN"},//yinxianghua   20091117
	{RMM_CITY_CODE_CNNB,	  "NINGBO",		"China Ningbo Cable"},
	{RMM_CITY_CODE_CNSY,	  "SHENYANG",	"China Shenyang Cable"},
	{RMM_CITY_CODE_CNWH,	  "WUHAN",		"China Wuhan GD"},
	{RMM_CITY_CODE_CNXA,	  "XIAN",		"China Xian PN"},
	{RMM_CITY_CODE_CNXM,	  "XIAMEN",		"China Xiamen Cable"},
	{RMM_CITY_CODE_CNZZ,	  "ZHENGZHOU",	"China Zhengzhou Cable"},//yinxianghua  20091203
	{RMM_CITY_CODE_CNZH,	  "ZHUHAI",		"China Zhuhai Cable"},
	//END
	{RMM_CITY_CODE_CNO,	   	  "CNO",		"China Others"}, //xiangchen.yang 20111031. reopen 20111226 add NoSelection
	{RMM_CITY_CODE_ZZ,		  NULL,			NULL}
#endif
};

#endif	/* _COUNTRY_SPECS_H_ */
