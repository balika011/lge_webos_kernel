/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2006 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file 		senc_ddi.h
 *
 *  header of System Encoder DDI Fuction.
 *
 *  @author		Hwang Hayun (hwang.hayun@lge.com)
 *  @version	0.1
 *  @date		2011.08.10
 *  @note		Additional information.
 *  @see
 */

#ifndef _SENC_DDI_H_
#define _SENC_DDI_H_
/*----------------------------------------------------------------------------------------
	Control Constants
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    File Inclusions
----------------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif
/*----------------------------------------------------------------------------------------
	Constant Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
	Macro Definitions
----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------
    Type Definitions
----------------------------------------------------------------------------------------*/
typedef enum DDI_SENC_CH
{
    SENC_NORMAL_CH,
    SENC_AUDIO_CH,
    SENC_VIDEO_CH,

}DDI_SENC_CH_T;

typedef enum DDI_SENC_VQUALITY
{
    SENC_VQUALITY_LOW			= 0,
	SENC_VQUALITY_MID_LOW		= 1,
	SENC_VQUALITY_MIDDLE		= 2,
	SENC_VQUALITY_MID_HIGH		= 3,
	SENC_VQUALITY_HIGH			= 4,

	SENC_VQUALITY_MIN		    = SENC_VQUALITY_LOW,
	SENC_VQUALITY_MAX		    = SENC_VQUALITY_HIGH

}DDI_SENC_VQUALITY_T;


/*----------------------------------------------------------------------------------------
	Extern Function Prototype Declaration 
----------------------------------------------------------------------------------------*/

DTV_STATUS_T DDI_SENC_StartTSDownload(void);

DTV_STATUS_T DDI_SENC_StopTSDownload(void);

DTV_STATUS_T DDI_SENC_GetTSDownloadStream(UINT8 **ppTSStreamBuffer, UINT32 *pStreamSize);

DTV_STATUS_T DDI_SENC_StartSWTSDownload(DDI_SENC_CH_T chType);

DTV_STATUS_T DDI_SENC_StopSWTSDownload(void);

DTV_STATUS_T DDI_SENC_GetSWTSDownloadStream(UINT8 **ppTSStreamBuffer, UINT32 *pStreamSize);

DTV_STATUS_T DDI_SENC_SetVideoQuality(DDI_SENC_VQUALITY_T videoQuality);

DTV_STATUS_T DDI_SENC_SetVideoQP(UINT32 vencQP);

DTV_STATUS_T DDI_SENC_SetVideoGOP(UINT32 vencGOP);
    
void DDI_SENC_SetQPGOPValue(UINT32 qp, UINT32 gop);


/*----------------------------------------------------------------------------------------
	Extern Variables
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Extern Includes
----------------------------------------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif

//EOF
