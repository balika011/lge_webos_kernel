/*****************************************************************************************//**
 * Copyright (c) by TPV Technology.
 * All rights reserved.
 *
 * @file        imamb_p.h
 * @brief       The header file for provided APIs for the complete Ambilight lib.
 * @author      Chen Hsuan Han
 * @remark      This file contains API prototypes and common types that are provided by the Ambilight lib.
 ********************************************************************************************/
/** @mainpage Ambilight 2K14 API Specification for MTK Platform
	@author Chen Hsuan Han
	@version 0.25
	@date 2014-12-04
	
	@section Introduction
	This document describes the API Specification for the Ambilight 2k14 for Philips 2K14 TV 6K and below series. The target platforms includes MT5391 and MT5580.

	@section Interfaces
	All provided interfaces are described in imamb_p.h, and required interfaces are described in imamb_r.h
*/

#ifndef _IO_MTAMB_H
#define _IO_MTAMB_H

/*********************************************************************************************
 * Includes
 ********************************************************************************************/

//#include "../minternal/mcommon_drv/mtypedef.h"
#include "u_common.h"

/*********************************************************************************************
 * Defines
 ********************************************************************************************/
#define IP_VERSION_MAMB	"00.00.00.00"


/*********************************************************************************************
 * Structs and enums
 ********************************************************************************************/

/** @brief Ambilight Mode */
typedef enum  {
    DRVAMB_AMBILIGHT_MODE_OFF          = 0,
    DRVAMB_AMBILIGHT_MODE_STATIC       = 1,
    DRVAMB_AMBILIGHT_MODE_EXTERNAL	= 2,    /*Color of pushed pixels is in Gamma Light domain*/
    DRVAMB_AMBILIGHT_EXTERNAL_EXPERT   = 3,	/*Color of pushed pixels is in linear Light domain*/	
    DRVAMB_AMBILIGHT_MODE_DYNAMIC      = 4,
    DRVAMB_AMBILIGHT_MODE_MONO         = 5,
    DRVAMB_AMBILIGHT_MODE_LOUNGE       = 6,
    DRVAMB_AMBILIGHT_MODE_FACTORY      = 7
} mamb_AmbilightMode_e;

/** @brief Ambilight Lounge Mode Mode */
typedef enum  {
    DRVAMB_AMBILIGHT_LOUNGEMOODMODE_NORMAL   = 0,
    DRVAMB_AMBILIGHT_LOUNGEMOODMODE_DEMO     = 1
}mamb_AmbilightLoungeMoodMode_e;

/** @brief Ambilight Topology */
typedef enum  {
    DRVAMB_AMBILIGHT_TOPOLOGY_NONE      = 0,
    DRVAMB_AMBILIGHT_TOPOLOGY_MONO      = 1,	   /* Value used when the Ambilight topology is 1 global control .							  */
    DRVAMB_AMBILIGHT_TOPOLOGY_2_SIDED   = 2,    /* Value used when the Ambilight topology is 2 sided control: Left and right.			  */
    DRVAMB_AMBILIGHT_TOPOLOGY_3_SIDED   = 3,    /* Value used when the Ambilight topology is 3 sided control: Left, top and right.		  */
    DRVAMB_AMBILIGHT_TOPOLOGY_4_SIDED	 = 4  /* Value used when the Ambilight topology is 4 sided control: Left, top, right and bottom. */
}mamb_AmbilightTopology_e;

/** @brief Ambilight Feature */
typedef enum  {
    DRVAMB_AMBILIGHT_FEATURE_IMMERSIONLEVEL              = 0,
    DRVAMB_AMBILIGHT_FEATURE_BRIGHTNESS                  = 1,
    DRVAMB_AMBILIGHT_FEATURE_SATURATION                  = 2,
    DRVAMB_AMBILIGHT_FEATURE_COLORBOOST                  = 3,	 
    DRVAMB_AMBILIGHT_FEATURE_MOTION_AMBILIGHT_LEVEL      = 4,
    DRVAMB_AMBILIGHT_FEATURE_SHOPBOOST                   = 5,
    DRVAMB_AMBILIGHT_FEATURE_ANDROID_LIGHT_COUPLING      = 6
}mamb_AmbilightFeature_e;


/** @brief Ambilight Led Align */
typedef enum  {
    DRVAMB_AMBILIGHT_LEDALIGNED_UNKNOWN        = 0,
    DRVAMB_AMBILIGHT_LEDALIGNED_ALIGNED        = 1,
    DRVAMB_AMBILIGHT_LEDALIGNED_INITIALISED    = 2
}mamb_AmbilightLedAlign_e;

/** @brief Ambilight Setting Save Status */
typedef enum  {
    DRVAMB_AMBILIGHT_SETTINGSSAVED_OK          = 0,
    DRVAMB_AMBILIGHT_SETTINGSSAVED_BUSY        = 1,
    DRVAMB_AMBILIGHT_SETTINGSSAVED_FAILED      = 2
}mamb_AmbilightSettingsSavedStatus_e;

/** @brief Ambilight Color */
typedef struct 
{
    int red;
    int green;
    int blue; 
} mamb_AmbilightColor_t;

/** @brief Ambilight Lounge Mode Setting */
typedef struct 
{
    int hue;
    int saturation;
    int brightness;
    int deltaHue;
    int deltaSaturation;
    int deltaBrightness;
    int speed;
    mamb_AmbilightLoungeMoodMode_e mode;
} mamb_AmbilightLoungeMoodSettings_t;

/** @brief Ambilight Matrix */
typedef struct 
{
    int   Rr;
    int   Rb;
    int   Rg;

    int   Gr;
    int   Gb;
    int   Gg;

    int   Br;
    int   Bb;
    int   Bg;
} mamb_AmbilightMatrix_t;


/** @brief Ambilight Setting Factor */
typedef struct 
{
    int redFactor;
    int greenFactor;
    int blueFactor; 
} mamb_AmbilightFactor_t;



/*********************************************************************************************
 * Exported functions prototypes
 ********************************************************************************************/
extern void Mamb_Init(UINT8 *pbData);
extern void Mamb_ReInit(UINT8 *pbData);

extern void Mamb_setMode(mamb_AmbilightMode_e ambilightmode );
extern void Mamb_getMode(mamb_AmbilightMode_e *ambilightmode );
extern void Mamb_setMute(BOOL mute );
extern void Mamb_getMute( BOOL * is_muted );
extern void Mamb_setLoungeMood( mamb_AmbilightLoungeMoodSettings_t * ambilightloungemood );
extern void Mamb_getLoungeMood( mamb_AmbilightLoungeMoodSettings_t * ambilightloungemood );
extern void Mamb_setFeatureLevel(mamb_AmbilightFeature_e ambilightfeature, int level );
extern void Mamb_getFeatureLevel(mamb_AmbilightFeature_e ambilightfeature, int * level );
extern void Mamb_setWallColor(mamb_AmbilightColor_t *wallcolor );
extern void Mamb_getWallColor(mamb_AmbilightColor_t *wallcolor );
extern void Mamb_setStaticColorRGB(mamb_AmbilightColor_t * color );
extern void Mamb_getStaticColorRGB(mamb_AmbilightColor_t * color );
extern void Mamb_setSunsetDuration(int msec );
extern void Mamb_getSunsetDuration( int * msec );
extern void Mamb_getConfiguration(mamb_AmbilightTopology_e * topology , int * no_of_vertpixels , int * no_of_horpixels , int * no_of_boards , int * version);
extern void Mamb_setPixel(int pixel, mamb_AmbilightColor_t * pixelcolors );
extern void Mamb_getPixel(int  pixel, mamb_AmbilightColor_t * pixelcolors );
extern void Mamb_setPixelArray(int  size, mamb_AmbilightColor_t * pixelcolorsarray );
extern void Mamb_getPixelArray(int  size, mamb_AmbilightColor_t * pixelcolorsarray );
extern void Mamb_getMeasuredPixelArray( int  size, mamb_AmbilightColor_t * pixelcolorsarray );
extern void Mamb_getProcessedPixelArray( int  size, mamb_AmbilightColor_t * pixelcolorsarray );
extern void Mamb_getNbrOfPixelsPerboard ( int board, int * no_of_pixels );
extern void Mamb_setFactoryMatrixboard ( int board, int led_index, mamb_AmbilightMatrix_t * matrix, mamb_AmbilightFactor_t * factors );
extern void Mamb_getFactoryMatrixboard ( int board, int led_index, mamb_AmbilightMatrix_t * matrix, mamb_AmbilightFactor_t * factors );
extern void Mamb_setFactoryBrightnessboard (  int board, int led_index,  mamb_AmbilightFactor_t * factors );
extern void Mamb_getFactoryBrightnessboard ( int board, int led_index, mamb_AmbilightFactor_t * factors );
extern void Mamb_saveSettings( void );
extern void Mamb_checkSettingsSaved( int * retval );
extern void Mamb_getFactoryNbrOfLedsPerboard ( int board, int * no_of_leds );
extern void Mamb_getFactoryLedHwOption ( int * led_hw_option );

#endif // _IO_MTAMB_H/


