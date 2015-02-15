#include "vdec_verify_common.h"
#include "vdec_verify_mpv_prov.h"
#include "vdec_vp8webp_vfy_if.h"


void * _rVdecInfo;

typedef void (*PFN_VDEC_EVENT_NOTIFY)(
    UINT32 u4Arg1,
    UINT32 u4Arg2,
    UINT32 u4Arg3,
    UINT32 u4Arg4
);

typedef struct
{
    UINT32 u4Pts;
    UCHAR u1PicType;
    UCHAR u1EsId;
}VDEC_PICTYPE_CB_T;


typedef struct
{
    UINT32 u4Rptr;
    UINT32 u4Pts;
    UINT16 u2TemporalRef;
    UCHAR ucDataType;
    UCHAR ucPicType;
    UCHAR ucPicStruct;
    UCHAR ucRepFirstFld;
    UCHAR ucTopFldFirst;
    UCHAR ucDataLen;
    UCHAR ucEsId;
    BOOL fgProgressiveFrm;
}MPV_USERDATA_CB_T;


typedef struct
{
    UINT32  u4DecErrNs;
    UCHAR u1EsId;
}VDEC_DECERR_CB_T;

typedef struct
{
    UINT32 u4Status;
    UCHAR u1EsId;
}VDEC_STATUS_CB_T;

typedef void (*PFN_VDEC_NOT_SUPPORT_NFY_FCT) (
    UCHAR              ucEsId,
    BOOL                 fgSupport
);

typedef void (*PFN_VDEC_TRICK_PTS_CB)
(
    UINT8 u1SrcId,
    UINT32 u4Pts,
    BOOL fgAgain
);

typedef void (*PFN_VDEC_STATUS_CB)
(
    VDEC_STATUS_CB_T* prStatus
);

typedef void (*PFN_VDEC_PIC_TYPE_CB)
(
    VDEC_PICTYPE_CB_T* prPicType
);

typedef void (*PFN_VDEC_RENDER_PTS_CB)
(
    UINT32 u4Pts,
    UINT8 u1AttachedSrcId
);


typedef void (*PFN_VDEC_DECERR_CB)
(
    VDEC_DECERR_CB_T* prDecErr
);


typedef void (*PFN_MPV_USERDATA_CB)
(
	MPV_USERDATA_CB_T* prUsrData
);

/**
 *  Display color mode.
 */
typedef enum
{
    AYCBCR8888_DIRECT32,						///< AYCbCr display mode, 32 bit per pixel, for OSD
    ARGB8888_DIRECT32,
    Y_CBCR420_SEP16,							///< Y/CbCr seperate 420 display mode, 16 bit per pixel, for video plane
    Y_CBCR422_SEP16,							///< Y/CbCr separate 422 display mode, 16 bit per pixel, for video plane
    Y_CBCR444_YCSEP24,
    ARGB4444_DIRECT16,
    ARGB1555_DIRECT16,
    RGB565_DIRECT16,
    COLOR_MODE_MAX
} ENUM_IMG_DISP_COLOR_MODE;

/**
 *  Rotate degree.
 */
typedef enum
{
#ifdef PHOTO_EMP_DDI_CONFIG
    CLOCKWISE_0,
    CLOCKWISE_90,
    CLOCKWISE_180,
    CLOCKWISE_270,
    CLOCKWISE_0_WITH_FLIP,					///<no rotation, with flip */
    CLOCKWISE_90_WITH_FLIP,					///<clockwise  90 degrees, with flip */
    CLOCKWISE_180_WITH_FLIP,					///<clockwise 180 degrees, with flip */
    CLOCKWISE_270_WITH_FLIP,					///<clockwise 270 degrees, with flip */
    PHASE_MAX
#else
	CLOCKWISE_0			 		= 0,
	CLOCKWISE_0_WITH_FLIP	 	= 1,
	CLOCKWISE_180			 	= 2,
	CLOCKWISE_180_WITH_FLIP 	= 3,
	CLOCKWISE_270_WITH_FLIP 	= 4,
	CLOCKWISE_90			 	= 5,
	CLOCKWISE_90_WITH_FLIP  	= 6,
	CLOCKWISE_270			 	= 7,
	PHASE_MAX
#endif
}ENUM_IMG_ROTATE_PHASE;

typedef struct
{
    UINT8* pu1Addr;								///< [IN] start address
    UINT32 u4CbCrOffset;							///< [IN] address offset for Y/CbCr seperate mode
    UINT32 u4Depth;								///< [IN] number of bits per pixel  for Y component.
    UINT32 u4Pitch;								///< [IN] pitch
    UINT32 u4TotalLine;							///< [IN] total height
    UINT32 u4StartX;								///< [IN] start point X
    UINT32 u4StartY;								///< [IN] start point Y
    UINT32 u4Width;								///< [IN] region width
    UINT32 u4Height;								///< [IN] region height
    UCHAR  ucPicStruct;                            //1, TOP_FLD_PIC   2,BTM_FLD_PIC  3,FRM_PIC
    UINT32 u4DecRatio;
}IMG_REGION_T;

typedef struct
{
    ENUM_IMG_DISP_COLOR_MODE eDispColorMode;	///< [IN] color mode
    ENUM_IMG_ROTATE_PHASE eRotateDegree;		///< [IN] rotate degree
    IMG_REGION_T rSrcRegion;						///< [IN] source information
    IMG_REGION_T rDstRegion;						///< [IN] destination information
    BOOL bKeepRatio;							///< [IN] TRUE: keep original image ratio, FALSE: fit u4DstW, u4DstH
    BOOL bKeepOriResolution;                   ///< [IN] TRUE: keep original image resolution, FALSE: enlarge to u4DstW, u4DstH
    IMG_REGION_T* prDstRegion;					///< [OUT] destination result
}IMG_BUF_PARAM_T;

typedef void (*PFN_JPG_EVENT_NOTIFY)(
    UINT32 u4ImgID,
    VOID *pv_tag,
    VOID *pv_data,
    UINT32 u4State
);

typedef enum
{
    IMAGE_JPEG,				///< image format
    IMAGE_PNG,
    IMAGE_WEBP,
    IMAGE_MPO,
    IMAGE_PROGRESSIVE_JPEG,
    IMAGE_MAX
} ENUM_IMG_FORMAT;

typedef struct
{
    UINT32 u4SrcStartX;								///< [IN] start point X
    UINT32 u4SrcStartY;								///< [IN] start point Y
    UINT32 u4SrcWidth;								///< [IN] region width
    UINT32 u4SrcHeight;								///< [IN] region height
}IMG_SRC_REGION_T;

typedef struct
{
    UCHAR ucFbgId;
    UINT32 u4RingOffset;
}IMG_VDEC_T;

typedef struct
{
    ENUM_IMG_FORMAT eFormat;					///< [IN] Format of Image.
    UINT8* pu1InBuffer;							///< [IN] compressed data start pointer.Null for using buffer notify mechanism
    UINT64 u8BufSize;							///< [IN] the size of the compressed data. 0 for using buffer notify mechanism
    IMG_REGION_T rDstRegion;						///< [IN] destination information, use internal buffer if pu1Addr == NULL, use default w,h if w,h=0
    BOOL bKeepRatio;							///< [IN] TRUE: keep original image ratio, FALSE: fit u4DstW, u4DstH
    ENUM_IMG_DISP_COLOR_MODE eDispColorMode;	///< [IN] destination color mode
    ENUM_IMG_ROTATE_PHASE eRotateDegree;		///< [IN] rotate degree
    IMG_REGION_T* prDecOut;						///< [IN/OUT] the decompressed image info. Valid: !=NULL, prepare by middleware
    IMG_VDEC_T rMjpg;                              ///used for getting working buffer from MPEG (MJPEG only)
    IMG_SRC_REGION_T rSrcRegion;
    UINT32 u4DecColorMode;
    BOOL fgThumbnailDec;                        ///< [IN] decode thumbnail or full image
    BOOL bIsVitualOutputAddr;
#ifdef CC_FUJI3D_SUPPORT
    BOOL fgMPFParsing;                        ///< [IN] parsing MPF header
#endif
} IMG_PARAM_T;

void *  _VDEC_GetEsInfoKeep(UCHAR ucEsId)
{
		return 0;
}

void *  _VDEC_GetEsInfo(UCHAR ucEsId)
{
		return 0;
}

void VDEC_Termint(void)
{

}


void VDEC_SyncStc(UCHAR ucEsId, UCHAR ucMode, UCHAR ucStcId)
{

}


BOOL  VDEC_StopVideoThumbnail2(UCHAR ucEsId)
{
		return true;
}

void VDEC_Stop(UCHAR ucEsId)
{

}

void VDEC_StartPTS(UCHAR ucEsId, BOOL fgEnCalPTS, UINT32 u4Pts, void * prPTSInfo)
{

}

void VDEC_SetWptr(UINT8 u1VdecId, UINT32 u4Wptr)
{

}


BOOL  VDEC_SetWaitPvrSps(UCHAR ucEsId, UINT8 u1PVRWaitSps)
{
	return true;
}

INT32 VDEC_SetVdecNotSupportNfy(UCHAR ucEsId, const PFN_VDEC_NOT_SUPPORT_NFY_FCT  prVdecNtyNotSupport)
{
	return 0;
}

BOOL  VDEC_SetTotalTime(UCHAR ucEsId, UINT32 u4TotalTime)
{
	return true;
}

void VDEC_SetTPTFlag(UCHAR ucEsId, BOOL fgIsTPT)
{

}

void VDEC_SetSrcId(UCHAR ucEsId, UINT8 u1SwdmxInstId )
{

}

BOOL  VDEC_SetRenderPts(UCHAR ucEsId, UINT32 u4Pts)
{
	return true;
}


BOOL VDEC_SetRenderPicCount(UCHAR ucEsId, BOOL fgRenderPicCnt, UINT32 u4Count)
{
	return true;
}

BOOL  VDEC_SetRenderFromPtsAutoPauseStartPts(UCHAR ucEsId, UINT32 u4Pts, BOOL fgAutoPause, BOOL fgStartPts)
{
	return true;
}

BOOL  VDEC_SetRenderFromPts(UCHAR ucEsId, UINT32 u4Pts)
{
	return true;
}

BOOL  VDEC_SetRenderFromPos(UCHAR ucEsId, BOOL fgRenderFromPos,
    UINT64 u8Pos, INT32 i4TempRef, UINT16 u2DecodingOrder)
{
	return true;
}

BOOL VDEC_SetRenderFromFirstPic(UCHAR ucEsId,
    BOOL fgRenderFromFirstPic, UINT64 u8LastRenderPos)
{
	return true;
}

BOOL  VDEC_SetRenderBackStep(UCHAR ucEsId, BOOL fgRenderBackStep, UINT64 u8Pos,
    INT32 i4TempRef, UINT16 u2DecodingOrder)
{
	return true;
}

void VDEC_SetParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{

}


void VDEC_SetMMParam(UCHAR ucEsId, UINT32 u4Type, UINT32 u4Param1, UINT32 u4Param2, UINT32 u4Param3)
{
}

BOOL  VDEC_SetLastTime(UCHAR ucEsId, UINT32 u4LastTime)
{
	return true;
}


void VDEC_SetH264PVR(UCHAR ucEsId, UINT32 u4PVRMode)
{
}

EXTERN BOOL VDEC_SetGetSeqHdrOnlyEnd(UCHAR ucEsId, BOOL fgPowerOff, UCHAR ucMpvId)
{
	return true;
}

BOOL VDEC_SetGetSeqHdrOnlyBegin(UCHAR ucEsId,
    ENUM_VDEC_FMT_T eFmt, BOOL *pfgPowerOff, UCHAR *ucMpvId)
{
	return true;
}

BOOL  VDEC_SetFileSize(UCHAR ucEsId, UINT64 u8FileSize)
{
	return true;
}

INT32 VDEC_SetDecNfy(UCHAR ucEsId, const void * prDecNfyInfo)
{
	return 0;
}

VOID VDEC_SetABInfo(UCHAR ucEsId, void  *prABInfo)
{
}

UCHAR VDEC_SendEs(const void *prPesInfo)
{
	return 0;
}

BOOL VDEC_ReleaseDispQ(UCHAR ucEsId)
{
	return true;
}

BOOL  VDEC_RegTrickPtsCb(UCHAR ucEsId, PFN_VDEC_TRICK_PTS_CB pfnTrickPtsCb, UINT8 u1AttachedSrcId)
{
	return true;
}

BOOL VDEC_RegStatusCb(PFN_VDEC_STATUS_CB pfnStatusCb)
{
	return true;
}

BOOL  VDEC_RegRenderPtsCb(UCHAR ucEsId, PFN_VDEC_RENDER_PTS_CB pfnRenderPtsCb, UINT8 u1AttachedSrcId)
{
	return true;
}

BOOL VDEC_RegPicTypeCb(PFN_VDEC_PIC_TYPE_CB pfnPicTypeCb)
{
	return true;
}

BOOL  VDEC_RegDecErrCb(PFN_VDEC_DECERR_CB pfnDecErrCb, UINT32 u4ErrDuration)
{
	return true;
}

void VDEC_RePlayVdec(UCHAR ucEsId)
{
}

BOOL VDEC_QueryInfo(UCHAR ucEsId, void* prHdrInfo)
{
	return true;
}

BOOL VDEC_PrsSeqHdr(UCHAR ucEsId, UCHAR eFmt,
    void* prPesInfo, void* prSeqInfo)
{
	return true;
}

void VDEC_Play_I_Frame(UCHAR ucEsId, UCHAR eFmt)
{

}

BOOL VDEC_PlayOneFrm(UCHAR ucEsId, UCHAR eFmt)
{
	return true;
}

void VDEC_PlayNotDisplay(UCHAR ucEsId, BOOL fgNotDisplay)
{
}

void VDEC_PlayMM(UCHAR ucEsId, BOOL fgPlayMM)
{
}

BOOL VDEC_Play(UCHAR ucEsId, UCHAR eFmt)
{
	return true;
}

void VDEC_Pause(UCHAR ucEsId)
{
}

BOOL VDEC_Notify(UCHAR ucEsId,UCHAR notifyStatus)
{
	return true;
}

void VDEC_Init(void)
{
}

BOOL  VDEC_GetVideoThumbnail3(UCHAR ucEsId, UINT16 u2CompId, BOOL bIsOnePass,
    void* prDstInfo,  BOOL fgKeepRation)
{
	return true;
}

BOOL  VDEC_GetVideoThumbnail2(UCHAR ucEsId,
    void* prDstInfo,
    UINT32 u4WorkingBufAddr, UINT32 u4WorkingBufSize,
    BOOL fgKeepRation)
{
	return true;
}

BOOL  VDEC_GetVideoThumbnail(UCHAR ucEsId, UINT16 u2CompId, BOOL bIsOnePass,
    void* prDstInfo, void* prSrcInfo, BOOL fgKeepRation)
{
}

VOID VDEC_GetQueueInfo(UCHAR ucEsId, UINT16* pu2QueueSize,UINT16* pu2MaxQueueSize)
{
}

BOOL VDEC_GetGnlInfo(UCHAR ucEsId, void* prGnlInfo)
{
	return true;
}

void VDEC_GetDecStatus(UCHAR ucEsId, BOOL* pfgLock
    , BOOL* pfgTopFldFirst, BOOL* pfgRepFirstFld, BOOL* pfgProgressiveFrm
    , UCHAR* pucPicType, UCHAR* pucPicStruture, UCHAR* pucTemporalRef
    , UINT32* pu4ReceiveNs, UINT32* pu4DecOkNs)
{

}

UINT32 VDEC_GetAvgPicSize(UCHAR ucEsId)
{
	return 0;
}

void VDEC_GenPTS(UCHAR ucEsId, UCHAR ucFbgId, UCHAR ucFbId)
{
}

void VDEC_FbgReleaseHdlr(UCHAR ucEsId)
{
}

void VDEC_FbgChgHdlr(UCHAR ucEsId, UCHAR ucFbgId, UINT32 u4FBStart, UINT32 u4FBEnd)
{
}

void VDEC_ClrMMParam(UCHAR ucEsId, UINT32 u4Type)
{
}

BOOL VDEC_ChkDivXBonding(VOID)
{
	return true;
}

BOOL VDEC_ChkCodecCap(UCHAR ucEsId, UCHAR eFmt)
{
	return true;
}

void MPV_Termint(void)
{
}

void MPV_StopUserData(UCHAR ucEsId)
{
}

void MPV_StopCc(UCHAR ucEsId)
{
}

void MPV_Stop(UCHAR ucEsId)
{
}

void MPV_StartUserData(UCHAR ucEsId)
{
}

BOOL MPV_ShowLogo(VOID)
{
	return true;
}

void MPV_SetRR(UCHAR ucEsId, UINT32 u4Ratio)
{
}

void MPV_SetMcMode(UCHAR ucEsId, BOOL fgMcPipe, BOOL fgMcFastAddr, BOOL fgMcNewFsm)
{
}

INT32 MPV_SetIFrameDec(UCHAR ucEsId, const void* prIFrameBuff)
{
	return 0;
}


INT32 MPV_SetIFrameBuf(UCHAR ucEsId, const void* prIFrameBuff)
{
	return 0;
}

void MPV_SetEcSetting(UCHAR ucEsId, UINT32 u4Mode)
{
}

void MPV_SetEC(UCHAR ucEsId, UCHAR ucECMethod, UINT32 u4ErrThrsd,
                                BOOL fgDetectRefLost)
{
}

void MPV_SetDbkRegion(UCHAR ucEsId, UINT32 u4XStart, UINT32 u4XEnd,
                                                UINT32 u4YStart, UINT32 u4YEnd)
{
}

void MPV_SetDbkPara2(UCHAR ucEsId, UINT32 u4P1, UINT32 u4P2,UINT32 u4P3,UINT32 u4P4, UINT32 u4P5,
                                       UINT32 u4P6, UINT32 u4P7, UINT32 u4P8,UINT32 u4P9,UINT32 u4P10)
{
}

void MPV_SetDbkPara1(UCHAR ucEsId, UINT32 u4P1, UINT32 u4P2,UINT32 u4P3,UINT32 u4P4, UINT32 u4P5,
                                      UINT32 u4P6, UINT32 u4P7, UINT32 u4P8,UINT32 u4P9,UINT32 u4P10)
{
}

BOOL MPV_SetDbkLevel(UCHAR ucEsId, UINT16 u2Level) // 0 ~ 100
{
	return true;
}

void MPV_SetDbkInfo(UCHAR ucEsId, BOOL fgDbkEnable,
    UINT16 u2XStart, UINT16 u2YStart, UINT16 u2Width, UINT16 u2Height)

{
}

void MPV_SetDbk(BOOL fgEnable, BOOL fgEngrMode)
{
}

void MPV_SetDBkThrsd(UCHAR ucEsId, BOOL fgDisable, UINT32 u4QpAvgThrsd, UINT32 u4DbkRatioThrsd,
                          UINT32 u4QpAvgThrsdP, UINT32 u4DbkRatioThrsdP, UINT32 u4MvThrsd)
{
}

INT32 MPV_SetCcNfyFct(UCHAR ucEsId, const void* prCcNfyInfo)
{
	return 0;
}

void MPV_Reset(void)
{
}

BOOL  MPV_RegUserDataCb(PFN_MPV_USERDATA_CB pfUserDataCb)
{
	return true;
}

void MPV_QueryStatus(void)
{
}

void MPV_PlayCc(UCHAR ucEsId)
{
}

void MPV_Play(UCHAR ucEsId)
{
}

void MPV_Pause(UCHAR ucEsId)
{
}

BOOL MPV_IsDisplay(UCHAR ucEsId)
{
	return true;
}

void MPV_Init(void)
{
}

BOOL MPV_GetUserData(UCHAR ucEsId, UCHAR* pucUserDataTmpBuf, UINT32 u4Rptr, UINT32 u4DataLen)
{
	return true;
}

UINT32 MPV_GetDecMode(UCHAR ucEsId)
{
	return 0;
}

VOID MPV_GetDbkLevel(UCHAR ucEsId, UINT16 *pu2Level) // 0 ~ 100
{
}

void MPV_GetDbkInfo(UCHAR ucEsId, BOOL* pfgDbkEnable,
    UINT16* pu2XStart, UINT16* pu2YStart, UINT16* pu2Width, UINT16* pu2Height)
{
}

INT32 MPV_GetCc(UCHAR ucEsId, void* prCcData)
{
	return 0;
}

INT32 MPV_FreeIFrameBuf(UCHAR ucEsId)
{
	return 0;
}

INT32 MPV_FreeCcBuf(UCHAR ucEsId)
{
	return 0;
}

void MPV_FlushEsmQ(UCHAR ucEsId, BOOL fgAbortDecode)
{
}

INT32 MPV_FlushCcBuf(UCHAR ucEsId)
{
	return 0;
}

void MPV_CleanRRDbk(UCHAR ucEsId)
{
}

INT32 MPV_AllocIFrameBuf(UCHAR ucEsId, void* prIFrameBuff)
{
	return 0;
}

INT32 MPV_AllocCcBuf(UCHAR ucEsId, UINT32 u4CcBufSize)
{
	return 0;
}

BOOL VDEC_IsExistDtvIFrmCase(VOID)
{
    return TRUE;
}

void VDEC_SetMVCCmpare(UCHAR ucEsId, BOOL fgParam)
{
    ;
}

BOOL  VDEC_SetRenderFromPtsAutoPause(UCHAR ucEsId, UINT32 u4Pts, BOOL fgAutoPause)
{
    return TRUE;
}

BOOL _VDEC_Vp8WebPDecStart(UINT32 u4DecId,WEBP_DEC_MODE eDecMode,
    WEBP_DECIN_PARAM_T *prDecInParam,WEBP_DECOUT_PARAM_T *prDecOutParam)
{
    return TRUE;
}
BOOL _VDEC_Vp8WebPDecStop(UINT32 u4DecId)
{
    return TRUE;
}
UINT32 _VDEC_Vp8WebPGetProgress(UINT32 u4DecId)
{
    return 0;
}
BOOL _VDEC_Vp8WebPInit(UINT32 u4DecId,PFN_EWBP_EVENT_NOTIFY pfnCb,
	UINT32 u4BufStart,UINT32 u4BufSize)
{
    return TRUE;
}

typedef enum
{
   TIME_STAMP_SWDMX_PLAY_START,
   TIME_STAMP_SWDMX_PLAY_DONE,
   TIME_STAMP_SWDMX_SEEK_START,
   TIME_STAMP_SWDMX_ABREPEAT_START,
   TIME_STAMP_SWDMX_PAUSE_START,
   TIME_STAMP_SWDMX_PAUSE_DONE,
   TIME_STAMP_SWDMX_RESUME_START,
   TIME_STAMP_SWDMX_RESUME_DONE,
   TIME_STAMP_SWDMX_STOP_START,
   TIME_STAMP_SWDMX_STOP_DONE,

   TIME_STAMP_VDEC_PLAY_START,
   TIME_STAMP_VDEC_PLAY_DONE,
   TIME_STAMP_VDEC_FRAME_FIRST,
   TIME_STAMP_VDEC_PAUSE_START,
   TIME_STAMP_VDEC_PAUSE_DONE,
   TIME_STAMP_VDEC_RESUME_START,
   TIME_STAMP_VDEC_RESUME_DONE,
   TIME_STAMP_VDEC_STOP_START,
   TIME_STAMP_VDEC_STOP_DONE,

   TIME_STAMP_AUD_PLAY_START,
   TIME_STAMP_AUD_PLAY_DONE,
   TIME_STAMP_AUD_FRAME_FIRST,
   TIME_STAMP_AUD_CALLBACK_FIRST,
   TIME_STAMP_AUD_AOUT_ENABLE,
   TIME_STAMP_AUD_PAUSE_START,
   TIME_STAMP_AUD_PAUSE_DONE,
   TIME_STAMP_AUD_RESUME_START,
   TIME_STAMP_AUD_RESUME_DONE,
   TIME_STAMP_AUD_STOP_START,
   TIME_STAMP_AUD_STOP_DONE,

   TIME_STAMP_FEEDER_REQ_FIRST,
   TIME_STAMP_FEEDER_ACK_FIRST,

   TIME_STAMP_VDP_FRAME_FIRST,
   TIME_STAMP_VDP_DISPLAY_FIRST,
   TIME_STAMP_VDP_SEEK_DONE,
   TIME_STAMP_VDP_ABREPEAT_DONE,
   TIME_STAMP_MAX
}E_TIME_STAMP_TYPE;

VOID MMPlaybackSpeedTestAddTimeStamp(E_TIME_STAMP_TYPE eType)
{
    //MMPlaybackSpeedTestAddTimeStamp_cmd(eType);
    return;
}

FILE *fopen(const UINT8 *filename, const UINT8 *mode)
{
    return (FILE*)0;
}
INT32 fread(void *ptr, UINT32 size, UINT32 n, FILE *stream)
{
    return 0;
}
INT32 fwrite(void *ptr, UINT32 size, UINT32 n, FILE *stream)
{
    return 0;
}
INT32 fseek(FILE *stream, INT32 offset, INT32 whence)
{
    return 0;
}

INT32 fclose(FILE *stream)
{
    return 0;
}

INT64 ftell(FILE *stream)
{
    return 0;
}

INT32 _t32_HostBreakPT(INT32 i4TargetFlag, CHAR *szHostCommand)
{
    return 0;
}

FILE *FILE_Open(const UINT8 *filename, const UINT8 *mode)
{
    return (FILE*)0;
}

UINT32 fb_osd_get_buffer_mode(UINT32 u4_plane)
{
    ASSERT(0);
    return 0;
}

UINT32 fb_osd_get_dfb_addr(void)
{
    ASSERT(0);
    return 0;
}

UINT32 fb_osd_get_dfb_size(void)
{
    ASSERT(0);
    return 0;
}

UINT32 fb_osd_get_fb_addr(void)
{
    ASSERT(0);
    return 0;
}

UINT32 fb_osd_get_fb_size(void)
{
    ASSERT(0);
    return 0;
}


