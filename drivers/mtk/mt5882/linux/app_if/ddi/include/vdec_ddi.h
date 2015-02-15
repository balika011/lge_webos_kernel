/******************************************************************************
 *   DTV LABORATORY, LG ELECTRONICS INC., SEOUL, KOREA
 *   Copyright(c) 2006 by LG Electronics Inc.
 *
 *   All rights reserved. No part of this work may be reproduced, stored in a
 *   retrieval system, or transmitted by any means without prior written
 *   permission of LG Electronics Inc.
 *****************************************************************************/

/** @file 		vdec_ddi.h
 *
 *  header of Mpeg2 Video Decoder DDI Fuction.
 *
 *  @author		Oh, Kyong-Joo (kjoh@lge.com)
 *  @version	1.0
 *  @date		2007.8.1     porting on GNSS platform
 *  @date		2007.12.27   Y2DTV
 *  @note		Additional information.
 *  @see
 */

/******************************************************************************
 	Header File Guarder
******************************************************************************/
#ifndef _VDEC_DDI_H_
#define _VDEC_DDI_H_

/******************************************************************************
    ���� ���� ���(Control Constants)
******************************************************************************/

/******************************************************************************
	#include ���ϵ� (File Inclusions)
******************************************************************************/
#include <osa_api.h>
#ifndef WIN32_SIM // Modified by dilly97: fix include
#include "common.h"
#else
#include "common_win32.h"
#endif

#include "video_ddi.h"

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 	��� ����(Constant Definitions)
******************************************************************************/

/******************************************************************************
    ��ũ�� �Լ� ���� (Macro Definitions)
******************************************************************************/

#define HD1920   0
#define HD1280   1
#define SD720    2

#define VDEC_NORMAL_PLAY_SPEED 1000


#if (PLATFORM_TYPE == BCM_PLATFORM)
#define STREAMCHANGECOUNTERMAX      3
#define STREAMCHANGECOUNTERMIN      0
#define CHANGEMAXFREERUNCOUNTER     120
#define FREERUNDELAYCOUNTER         3600
#endif

#define	VDEC_MAX_CH_NUM			2

/******************************************************************************
	�� ���� (Type Definitions)
******************************************************************************/

/**
 * VDEC status
 */
typedef enum
{
	VDEC_NOT_INIT	= 0,
	VDEC_INIT,
	VDEC_PLAYING,
	VDEC_STOPPED,
	VDEC_FREEZED,
	VDEC_NO_DATA,
	VDEC_RUNNING,
	VDEC_GET_SYNC
} VDEC_STATE_T;

/**
 * codec type
 */
typedef enum
{
	VDEC_CODEC_NONE	= 0,
	VDEC_CODEC_MPEG1,
	VDEC_CODEC_MPEG2,
	VDEC_CODEC_MPEG4_PART2,
	VDEC_CODEC_H264_MPEG4AVC,
	VDEC_CODEC_AVS,
	VDEC_CODEC_VC1,
	VDEC_CODEC_DIVX,
	VDEC_CODEC_MIN = VDEC_CODEC_NONE,
	VDEC_CODEC_MAX = VDEC_CODEC_DIVX,	
} VDEC_CODEC_T;

/**
 * Drip type ; I frame
 */
typedef enum
{
	VDEC_DRIP_I_FRAME	= 0,
	VDEC_DRIP_NORMAL	= 1
} VDEC_DRIP_TYPE_T;

/**
 * constant about Good video
 */
typedef enum
{
	VDEC_BAD_VIDEO		= 0,
	VDEC_GOOD_VIDEO		= 1
} VDEC_VIDEO_STATUS_T;

/**
 * video�� size, bitrate���� ����� �� ��쿡 ���̴µ� �ַ� �޸� MAP���ýÿ� ���δ�.
 */
typedef enum {
	VDEC_HDR_V_SIZE                    = 0,		/**< vertical size */
	VDEC_HDR_H_SIZE                    = 1,		/**< horizontal size */
	VDEC_HDR_VIDEO_FORMAT              = 2,		/**< video format */
	VDEC_HDR_FRAME_RATE                = 3,		/**< frame rate */
	VDEC_HDR_ASPECT_RATIO              = 4,		/**< aspect ratio */
	VDEC_HDR_BIT_RATE                  = 5,		/**< bit rate*/
	VDEC_HDR_PROGRESSIVE_SEQUENCE      = 6,		/**< progressive sequence */
	VDEC_HDR_PICTURE_TYPE              = 7,		/**< picture type */
	VDEC_HDR_TEMP_REF                  = 8,		/**< temporal reference */
	VDEC_HDR_PICTURE_STRUCT            = 9,		/**< picture reference */
	VDEC_HDR_TOP_FF                    = 10,	/**< top field first */
	VDEC_HDR_REPT_FF                   = 11,	/**< repeat field first */
	VDEC_HDR_PROGRESSIVE_FRAME         = 12,	/**< progressive frame */
	VDEC_HDR_INVALID                   = 13
} VDEC_HDR_T;

typedef void (*PFN_VDEC_INFO_TO_DVR_T) (UINT8 vdec_ch, UINT8 pic_type);


#if 0
/**
 * userdata�� DCC type.
 * video_api.h���� ���� ��.
 */
typedef enum
{
	DCC_UNKNOWN       	= 0x0,
	DCC_EIA708        	= 0x1,	/**< EIA-708 Format */
	DCC_DVS157        	= 0x2,	/**< DVS-157 Format */
	DCC_EIA708nDVS157 	= 0x3,	/**< EIA-708 + DVS-157  Format */
	DCC_TEST1         	= 0x6,
	DCC_TEST2         	= 0xa
} DCC_TYPE_STATE;
#endif

/**
 * userdata�� DCC type.
 * VIDEO_API.h���� ���� ��.
 *
 * @ingroup ka_vdec_types
 */
typedef enum
{
	DCC_UNKNOWN       	= 0x0,
	DCC_EIA708        	= 0x1,	/**< EIA-708 Format */
	DCC_DVS157        	= 0x2,	/**< DVS-157 Format */
	DCC_EIA708_DVS157 	= 0x3,	/**< EIA-708 + DVS-157  Format */
	DCC_TEST1         	= 0x6,
	DCC_TEST2         	= 0xa,
	DCC_EIA608			= 0x8
} VDEC_DCC_TYPE_T;

/**
 * DTVCC Service type.
 *
 * @see DDI Implementation Guide
*/
typedef enum
{
	DTVCC_SERVICE1		= 0x1,
	DTVCC_SERVICE2		= 0x2,
	DTVCC_SERVICE3		= 0x3,
	DTVCC_SERVICE4		= 0x4,
	DTVCC_SERVICE5		= 0x5,
	DTVCC_SERVICE6		= 0x6,
	DTVCC_SERVICE_INVALID	= 0x7
} VDEC_DCC_SERVICE_NUM_T;

/**
 * VDEC input video DTV format.
 *
 * @see
*/
typedef enum VDEC_DTV_FORMAT
{
	DTV1920X1080		= 0,
	DTV1440X1080		= 1,
	DTV1280X1080		= 2,
	DTV1280X720 		= 3,
	DTV960X720 			= 4,
	DTV720X576			= 5,
	DTV704X576			= 6,
	DTV640X576			= 7,
	DTV544X576			= 8,
	DTV480X576			= 9,
	DTV352X576			= 10,
	DTV352X288			= 11,
	DTV768X576			= 12,
	DTV528X576			= 13,
	DTV768X480			= 14,
	DTV720X480			= 15,
	DTV704X480			= 16,
	DTV640X480			= 17,
	DTV544X480			= 18,
	DTV528X480			= 19,
	DTV480X480			= 20,
	DTV352X480			= 21,
	DTV352X240			= 22,
	DTV_FORMAT_INVALID	= 23
} VDEC_DTV_FORMAT_T;

/**
 * AFD Mode Information
 *
 */
typedef enum VDEC_AFD_MODE
{
	VDEC_AFD_4_3_1000		= 0x08,// AFD_43_SAME
	VDEC_AFD_4_3_1001		= 0x09,// AFD_43_TO_43
	VDEC_AFD_4_3_1010		= 0x0a,// AFD_43_TO_169
	VDEC_AFD_4_3_1011		= 0x0b,// AFD_43_TO_149
	VDEC_AFD_4_3_1100		= 0x0c,// AFD_43_TO_RSVD
	VDEC_AFD_4_3_1101		= 0x0d,// AFD_43_TO_43SP149
	VDEC_AFD_4_3_1110		= 0x0e,// AFD_43_TO_169SP149
	VDEC_AFD_4_3_1111		= 0x0f,// AFD_43_TO_SP43
	VDEC_AFD_4_3_INVALID	= 0x00,// AFD_43_INVALID
	VDEC_AFD_16_9_1000		= 0x18,// AFD_169_SAME
	VDEC_AFD_16_9_1001		= 0x19,// AFD_169_TO_43
	VDEC_AFD_16_9_1010		= 0x1a,// AFD_169_TO_169
	VDEC_AFD_16_9_1011		= 0x1b,// AFD_169_TO_149
	VDEC_AFD_16_9_1100		= 0x1c,// AFD_169_TO_RSVD
	VDEC_AFD_16_9_1101		= 0x1d,// AFD_169_TO_43SP149
	VDEC_AFD_16_9_1110		= 0x1e,// AFD_169_TO_169SP149
	VDEC_AFD_16_9_1111		= 0x1f,// AFD_169_TO_SP43
	VDEC_AFD_16_9_INVALID	= 0x10,// AFD_169_INVALID
} VDEC_AFD_MODE_T;


/**
 * Pciture type
 *
 */
typedef enum VDEC_PIC_TYPE
{
	VDEC_SEQ_HEADER		= 0x00, // sequence header
	VDEC_I_PIC		    = 0x01, // I picture type
	VDEC_P_PIC		    = 0x02, // P picture type
	VDEC_B_PIC		    = 0x03, // B picture type
} VDEC_PIC_TYPE_T;

/**
 * Video decoder status
 *
 */
typedef struct VDEC_DECODER_STATUS
{
	VDEC_STATE_T		vdecState;
	VDEC_CODEC_T		codecType;
	BOOLEAN				bLipSyncAV;
	BOOLEAN				bStillPicture;
	BOOLEAN				bDripDecoding;
	UINT32				esBufDataSize;	   /* depth in bytes of the compressed buffer */
	UINT32				decodedQueueDepth; /* the number of decoded pictures currently ready to be displayed */
	UINT32				nMBDecErrorRate;   /* Rate of MB Dec Error at one frame, nMBDecErrorRate = Err MBs / Tatal MBs */
	UINT32				bPtsMatched;	   /* Indicate PTS matched */
} VDEC_DECODER_STATUS_T;

/**
 * Video decoder PTS
 *
 */
typedef struct VDEC_PTS
{
	UINT32				decodingPicturePTS;	   /* PTS of current decoding picture */
	UINT32				displayPicturePTS;     /* PTS of current displaying picture */
} VDEC_PTS_T;

/**
 * Decoding speed of video decoder
 *
 */
typedef enum VDEC_DECODING_SPEED
{
    VDEC_DECODE_SPEED_PAUSE  =    0,
    VDEC_DECODE_SPEED_SLOW   =  500,
    VDEC_DECODE_SPEED_NORMAL = 1000,
}VDEC_DECODING_SPEED_T;

/**
 * Vdec decode mode.
 */
typedef enum	VDEC_DECODE_MODE
{
	VDEC_DECODE_ALL,
	VDEC_DECODE_IP,
	VDEC_DECODE_I
} VDEC_DECODE_MODE_T;

#if (PLATFORM_TYPE == LG_PLATFORM)
/* [L8] kangsik.yoon (2011/04/13)
 * vdec internal buffer flush
 */
typedef enum	VDEC_FLUSH_MODE
{
	VDEC_FLUSH_CPB,
	VDEC_FLUSH_CMD,
	VDEC_FLUSH_CPB_CMD,
	VDEC_FLUSH_ALL
} VDEC_FLUSH_MODE_T;
#endif
/** UI Messaging System�� ���Ǿ����� Callback �Լ��� ���� ����.
 *	API_UTIL_InitUIMessagingSystem �Լ� ȣ�� �� ���Ǿ�����.
 */
#define  MAX_VDEC2UI_MSG_PARAM 	4
typedef void (*DDI_VDEC_MHP_AFD_CALLBACK_PTR_T) (UINT8 afdValue);




/******************************************************************************
	�Լ� ���� (Function Declaration)
******************************************************************************/
/**
 * Initialize VDEC.
 * Create semaphore and thread (process message, process AFD)
 * @return API_OK, API_ERROR when error
 */
DTV_STATUS_T DDI_VDEC_Initialize( void );

/**
 * start VDEC.
 * call GNSS gmPE_Start().
 * @param port [in] deprecated, set 0
 * @param codec [in] video codec to set
 * @return API_OK, API_ERROR when error
 */
DTV_STATUS_T DDI_VDEC_Start( UINT8 port, VDEC_CODEC_T codec);

/**
 * stop VDEC.
 * @param port [in] deprecated, set 0
 * @return API_OK, API_ERROR when error
 * @see gmPE_Stop()
 */
DTV_STATUS_T DDI_VDEC_Stop( UINT8 vPort );

/**
 * Pause VDEC.
 * just do not pass to display, decoding is going on.
 *
 * @param port [in] deprecated, set 0
 * @param frz [in] freeze: 1, melt: 0
 * @return API_OK, API_ERROR when error
 */
DTV_STATUS_T DDI_VDEC_Freeze(	UINT8 port, UINT8 frz );

/**
 * Start still-picture(I-Frame) decoding.
 * MHEG I-frame function use this function.
 *
 * @param port [in] deprecated, set 0
 * @param pDripData [in] ptr in system memory containing I-frame data
 * @param dripSize [in] byte size of I-frame
 * @return API_OK, API_ERROR when error
 */
DTV_STATUS_T DDI_VDEC_StartDripDec( UINT8 vPort, UINT8* pDripData, UINT32 dripSize, VDEC_DRIP_TYPE_T dripType, VDEC_CODEC_T codecType );
/**
 * Stop still-picture(I-Frame) decoding.
 * MHEG I-frame function use this function.
 *
 * @param port [in] deprecated, set 0
 * @return API_OK, API_ERROR when error
 */
DTV_STATUS_T DDI_VDEC_StopDripDec( UINT8 vPort );

/**
 * Get MPEG decoding information.
 * V size, H size, frame rate, aspect ratio, progressive.
 *
 * @param port [in] deprecated, set 0
 * @param infonum [in] info. number to get
 * @return information value
 */
SINT32 DDI_VDEC_GetVideoInfo( UINT8 port, VDEC_HDR_T infonum );

/**
 * Gets video decoder status.
 * It contains whether decoder start or not, lip sync status.
 *
 * @param vPort [IN] specifies a video port
 * @param pStatus [OUT] pointer to the variable to store the video decoder status
 * @return  DTV_STATUS_T
 **/
DTV_STATUS_T DDI_VDEC_GetDecoderStatus(UINT8 vPort, VDEC_DECODER_STATUS_T *pStatus);

/**
 * Synchronize AV.
 * @param port [in] deprecated, set 0
 * @param onOff [in] on off
 * @return API_OK, API_ERROR when error
 */
DTV_STATUS_T DDI_VDEC_SyncAV(UINT8 port , UINT8 onOff);

/**
 * return AFD value
 * @param port [in] deprecated, set 0
 * @return AFD value
 * @see VDEC_DecodeAFD()
 */
VDEC_AFD_MODE_T DDI_VDEC_GetAFD(UINT8 port);

/**
 * function telling if DTV signal is good or not.
 * It must be called periodically by CM.
 * @param port [in] deprecated, set 0
 * @return TRUE if good, or FALSE
 * @see DDI_VDEC_SetLatestDTVSigStatus()
 */
BOOLEAN DDI_VDEC_IsDTVSigStatusGood( UINT8 vPort );

/**
 * set good video forcely regardless of decoding state.
 * It is called when EPF mode.
 * @param port [in] deprecated, set 0
 * @param GoodorBad [in] Good/Bad
 * @see DDI_VDEC_IsDTVSigStatusGood()
 */
void DDI_VDEC_SetLatestDTVSigStatus( UINT8 vPort, VDEC_VIDEO_STATUS_T GoodorBad );

/**
 * DDI fuction for starting to gether user data.
 *
 * @param port [in] deprecated, set 0
 * @return DTV_STATUS_T
 * @see DDI_VDEC_StopUserData()
 */
DTV_STATUS_T DDI_VDEC_StartUserData( UINT8 port );

/**
 * DDI fuction for stopping to gether user data.
 *
 * @param port [in] deprecated, set 0
 * @return DTV_STATUS_T
 * @see DDI_VDEC_StartUserData()
 */
DTV_STATUS_T DDI_VDEC_StopUserData( UINT8 port );

void DDI_VDEC_InitCntCheck(void);

/**
 * Sets the decoding speed of the VDEC.
 * The normal decoding speed value for input parameter is 1000.
 * Please use the following value VDEC_MORMAL_PLAY_SPEED when you input the speed value.
 *
 * @param vPort [IN] specifies a video port.
 * @param speed [IN] decoding speed(#define VDEC_NORMAL_PLAY_SPEED 1000)
 * @return  API_OK if success, else API_ERROR
 **/
DTV_STATUS_T DDI_VDEC_SetDecodingSpeed(UINT8 vPort, VDEC_DECODING_SPEED_T speed);

/**
 * Registers callback function for DVR middleware.
 * DVR middleware need to know the picture type information for current decoding video.
 * Picture_type is as follows.
 * - sequence header
 * - I picture type
 * - P picture type
 * - B picture type
 * After VDEC decodes a frame, notify DVR middleware the picture type(I, P, B) and vdec_ch.
 * After the sequence header is detected, notify DVR middleware the picture type(sequence header) and vdec_ch.
 *
 * @param fnCbFn [IN] callback function
 * @return void
 */
void DDI_VDEC_RegisterDVRCallback(PFN_VDEC_INFO_TO_DVR_T pfnDVRCallback);

/**
 * Frame advance is a feature to single-step decoding frame by frame.
 * It means that 1 frame forward mode(1 frame decofing and stop).
 * It is dependent on the video decoder HW performance.
 * It HW does not support some speed mode, this function returns NOT_SUPPORTED error for that speed mode.
 *
 * @param bFrameAdvanceOn [IN] specifies Frame Advance mode
 * @return  DTV_STATUS_T
 **/
DTV_STATUS_T DDI_VDEC_SetFrameAdvance(UINT8 vPort, BOOLEAN bFrameAdvanceOn);



/**
 * Frame advance is a feature to single-step decoding frame by frame. but it doesn't wait any response
 * It means that 1 frame forward mode(1 frame decofing and stop).
 * It is dependent on the video decoder HW performance.
 * It HW does not support some speed mode, this function returns NOT_SUPPORTED error for that speed mode.
 *
 * @param bFrameAdvanceOn [IN] specifies Frame Advance mode
 * @return  DTV_STATUS_T
 **/
#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_VDEC_SetStillPicture(UINT8 vPort, BOOLEAN bStillPicture);
#endif //#if (PLATFORM_TYPE == MTK_PLATFORM)
#if (PLATFORM_TYPE == MTK_PLATFORM)
DTV_STATUS_T DDI_VDEC_SetFrameAdvanceNoSync(UINT8 vPort, BOOLEAN bFrameAdvanceOn);
#endif

/**
 * Reset video decoder. This flushes the video decoder
 *
 * @param vPort [IN] specifies a video port
 * @return  DTV_STATUS_T
 **/
DTV_STATUS_T DDI_VDEC_Reset(UINT8 vPort);

#if (PLATFORM_TYPE == LG_PLATFORM)
/* [L8] kangsik.yoon (2011/04/13)
 * vdec internal buffer flush
 */
DTV_STATUS_T DDI_VDEC_Flush(UINT8 vPort, VDEC_FLUSH_MODE_T vFlushMode);
#endif
/**
 * Get Current PTS of video decoder.
 *
 * @param vPort [IN] specifies a video port
 * @param pVdecPTS [OUT] PTS of video decoder
 * @return  DTV_STATUS_T
 **/
DTV_STATUS_T DDI_VDEC_GetCurPTS(UINT8 vPort, VDEC_PTS_T *pVdecPTS);




/**
 * DDI fuction for smooth fast forward
 *
 * @param  decodeMode [IN] enable/disable  drop field.
 * @return void
 */
void	DDI_VDEC_SetDecodeMode(VDEC_DECODE_MODE_T decodeMode);

/**
 * DDI fuction for getting total number of display pictures since Start.
 * This function is used to determine if uploaded 1 frame is decoded.
 * @param void
 * @return BOOLEAN
 */
BOOLEAN		DDI_VDEC_IsFrameAdvanced(void);



#if (PLATFORM_TYPE == BCM_PLATFORM)

/**
 * DDI fuction for smooth fast forward
 *
 * @param  bEnable [IN] enable/disable  drop field.
 * @return DTV_STATUS
 */
DTV_STATUS DDI_VDEC_BCM_SetDropFieldMode(BOOLEAN bEnable);


/**
 * I-Frame Decoding Function.
 *
 * This function will not display the decoded image in the video plane.
 * Instead the decoded image will be saved through file.
 * And also you don't have to stop VideoDecoder in order to use this function.
 * In other words, this function is independent of Main VDEC.
 *
 * @param	vPort			[IN]	video port
 * @param	codec			[IN]	video codec
 * @return  if succeeded - API_OK, else - API_ERROR
 */
DTV_STATUS_T DDI_VDEC_BCM_StartDripDec( UINT8 vPort, UINT8* dripData, UINT32 dripSize, VDEC_DRIP_TYPE_T dripType, VDEC_CODEC_T codecType );

/**
 * Stop I-Frame Decoding Function.
 *
 * This function will not display the decoded image in the video plane.
 * Instead the decoded image will be saved through file.
 * And also you don't have to stop VideoDecoder in order to use this function.
 * In other words, this function is independent of Main VDEC.
 *
 * @param	vPort			[IN]	video port
 * @param	codec			[IN]	video codec
 * @return  if succeeded - API_OK, else - API_ERROR
 */
DTV_STATUS_T DDI_VDEC_BCM_StopDripDec( UINT8 vPort );

/**
 * DDI fuction for AFD Callback
 *
 * @param  pfnMsgCbFn [IN] callback function.
 * @return void
 */
void DDI_VDEC_BCM_MHPAFDCallBack( DDI_VDEC_MHP_AFD_CALLBACK_PTR_T pfnMsgCbFn);

#endif

/**
 * VDEC debug menu.
 *
 * @param void
 * @return void
 */
void DDI_VDEC_DebugMenu(void);

/**
 * Existence of the DVS157 data.
 *
 * @author
 * @param	void		[IN]
 * @return	BOOLEAN : TRUE if there are DVS157 data
 */
BOOLEAN DDI_VDEC_IsDVS157ExistCnt(void);

/**
 * Existence of the EIA708 data.
 *
 * @author
 * @param	serviceNum	[IN] VDEC_DCC_SERVICE_NUM_T
 * @return	BOOLEAN : TRUE if there are EIA708 data
 */
BOOLEAN DDI_VDEC_IsDCCExistCnt(VDEC_DCC_SERVICE_NUM_T serviceNum);

/**
 * Set 0 on the EIA708 data Existence flag of All Services and DVS157 data Existence flag.
 *
 * @author
 * @param	void		[IN]
 * @return	void
 */
void DDI_VDEC_ResetExistCnt(void);

/**
 * decrease EIA708 data & DVS157 data Existence flag of all services.
 *
 * @author
 * @param	void		[IN]
 * @return	void
 */
void DDI_VDEC_MonitoringDCC(void);

/**
 * ä�� ü���� ���Ŀ� dcc type, field�� ���� ���� �ʱ�ȭ
 * Vdec_Start �Ǹ鼭 �ʱ�ȭ �ϴµ��� �ұ��ϰ� �̹� ä���� data�� ������ ��
 *
 * @author
 * @param	void		[IN]
 * @return	void
 */
 /* skystone_FT_NewJersey */
void DDI_VDEC_ResetAllDCCType( void );

/**
 * �� Picture hdr�ȿ� user data type�� �������� �� �� ����.
 * EIA608(EIA708), DVS 157�߿� �Ѵ� �Դ� �� ���߿� �ϳ��� �Դ��� ���� ����.
 *
 * @return DCC_TYPE_STATE : all dcc type
 * @remarks DDI_VDEC_UnknownDCCDatafromUserData���� ����.
 */
VDEC_DCC_TYPE_T DDI_VDEC_GetAllDCCType( void );

/**
 * reset count value for CC stability
 *
 * @author
 * @param	void
 * @return	void
 */
void DDI_VDEC_InitCntCheck( void );

/**
 * DCC Type Setting.(Unknown, SCTE21(EIA608 or EIA708), SCTE20(DVS157), UI���� ��Ʈ��.
 *
 * @author
 * @param 	_bDCCType	[IN]	dcc type
 * @return 	void
 */
void DDI_VDEC_SetDCCType( VDEC_DCC_TYPE_T dccType );

/**
 * enable userdata rx
 *
 * 1. prevent the repeat call of this function.
 * 3. If this is the first call, then conintue
 * 4. If not, return.
 * 5. Send USRDATA_EN event to UsrDataRxTask.
 * 6. Initialize all userdata queues variables.
 * 7. clear usrdata temperary buffer.
 * 8. initialize all userdata related hdall registers.
 * 9. start ACC encoder.
 * 10. enable userdata interrupt.
 *
 * @author
 * @param	void	[IN]
 * @return	DTV_STATUS_T
 */
DTV_STATUS_T DDI_VDEC_EnableUserDataRx(void);
/**
 * disable userdata rx
 * reverse order of EnableUserDataRx routine.
 *
 * 1. make pointer of userdata rx routine NULL.
 * 2. check is this the last possible disable call for userdata.
 * 3. if no, return immediately.
 * 4. if yes, continue.
 * 5. send EVENT_USRDATA_DIS to UsrDataRxTask to make ready to terminate
 *	   its usrdata rx operation.
 * 6. disable UserData Interrupt of hdall.
 * 7. stop ACC Encoder operation.
 * 8. initialize all variables for usrdata queues.
 *
 * @author
 * @param	void	[IN]
 * @return	DTV_STATUS_T
 */
DTV_STATUS_T DDI_VDEC_DisableUserDataRx(void);
/**
 * Start send userdata to DCC Engine
 * Obtain the callback for userdata transmission.
 *
 * @author
 * @param	pfnUserDataRx		[IN]	callback function is called whenever the userdata slot is received
 * @param	pfnUserDataOverflow	[IN]	callback function is called whenever the userdata buffer is overflow
 * @return	DTV_STATUS_T
 */
DTV_STATUS_T DDI_VDEC_Start708Rx(
	BOOLEAN	(*pfnUserDataRx)(void),			/* callback function is called whenever the userdata slot is received */
	BOOLEAN	(*pfnUserDataOverflow)(void) 	/* callback function is called whenever the userdata buffer is overflow */
);
/**
 * stop send userdata to DCC engine
 * Delete the callback function pointer for userdata transmission
 *
 * @author
 * @param	void	[IN]
 * @return	DTV_STATUS_T
 */
DTV_STATUS_T DDI_VDEC_Stop708Rx( void );
/**
 * start parsing the 608 in 708
 *
 * @author
 * @param	qid4ACCTx		[IN]	QID for 608Data Sending
 * @param	msgID4Field1	[IN]	Message ID for field 1
 * @param	msgID4Field2	[IN]	Message ID for field 2
 * @param	srcID4ACCTx		[IN]	Message Source ID for Main Picture
 * @return	DTV_STATUS_T
 */
DTV_STATUS_T DDI_VDEC_Start608Parsing(
	OSA_MSGQ_T	qid4ACCTx, 			// QID for 608Data Sending
	UINT32		msgID4Field1, 		// Message ID for field 1
	UINT32		msgID4Field2,		// Message ID for field 2
	UINT32		srcID4ACCTx		// Message Source ID for  608Data Sending
);

/**
 * stop parsing the 608 in 708
 *
 * @author
 * @param	void	[IN]
 * @return	DTV_STATUS_T
 */
DTV_STATUS_T DDI_VDEC_Stop608Parsing( void );
/**
 * Obtain the stored userdata .
 * This function copy back the received userdata to DCC
 *
 * @author
 * @param	ret_Size	[OUT]	buffer Size
 * @param	buffer		[OUT]	buffer pointer to save data
 * @return	DTV_STATUS_T
 */
DTV_STATUS_T DDI_VDEC_GetUserData(
	UINT16	*pRetSize,
	void	*pBuffer
);

void DDI_VDEC_RegisterPicInfoCallback(void);
#define MARK	__FILE__, __func__, __LINE__

#ifdef __cplusplus
}
#endif

#endif /*_VDEC_DDI_H_ */
