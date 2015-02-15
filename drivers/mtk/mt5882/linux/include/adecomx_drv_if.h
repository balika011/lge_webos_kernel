/********************************************************************************************
 *     LEGAL DISCLAIMER 
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES 
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED 
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS 
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, 
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY 
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK 
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION 
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *     
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH 
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, 
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE 
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
 *     
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS 
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.  
 ************************************************************************************************/
/*
*
* $Author: p4admin $
* $Modtime: $
* $Revision: #1 $
*
*/

// *********************************************************************
// Memo
// *********************************************************************
/*
*/

#ifndef _ADECOMX_MOD_H_
#define _ADECOMX_MOD_H_


#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
 *                              Definition
 *===========================================================================*/

typedef enum {
    ADEXOMX_UNKNOWN,
    
    ADEXOMX_CREATE      = _IOWR('c', 0, long),
    ADEXOMX_DECODE      = _IOWR('c', 1, long),
    ADEXOMX_FLUSH       = _IOWR('c', 2, long),    
    ADEXOMX_GETPARAM    = _IOWR('c', 3, long),
    ADEXOMX_SETPARAM    = _IOWR('c', 4, long),
    ADEXOMX_RELEASE     = _IOWR('c', 5, long),
	ADEXOMX_FILLBUFFER	= _IOWR('c', 6, long),
	ADEXOMX_EMPTYBUFFER = _IOWR('c', 7, long),

    ADEXOMX_COMMAND_MAX = _IOWR('c', 53, long)
} ADEXOMX_COMMAND;

typedef enum
{
	ADECOMX_GET_TYPE_MAX
} ADECOMX_GET_TYPE_T;

typedef enum
{
	ADECOMX_AUDIO_FORMAT_UNKNOWN_AUDIO_FORMAT  = 0,       ///< Unknown audio format
		ADECOMX_AUDIO_FORMAT_MP3 = (1<<0), ///MP3
		ADECOMX_AUDIO_FORMAT_AC3 = (1<<1), ///AC3
		ADECOMX_AUDIO_FORMAT_WMA = (1<<2), ///WMA
		ADECOMX_AUDIO_FORMAT_WMAPRO = (1<<3), ///WMAPRO
		ADECOMX_AUDIO_FORMAT_AAC = (1<<4), ///AAC
		ADECOMX_AUDIO_FORMAT_DTS = (1<<5), ///DTS
		ADECOMX_AUDIO_FORMAT_MPEG = (1<<6), ///MPEG
		ADECOMX_AUDIO_FORMAT_FLAC = (1<<7), ///FLAC
		ADECOMX_AUDIO_FORMAT_VORBIS = (1<<8), ///VORBIS
		ADECOMX_AUDIO_FORMAT_LPCM = (1<<9), ///LPCM
		ADECOMX_AUDIO_FORMAT_ADPCM = (1<<10), ///ADPCM
		ADECOMX_AUDIO_FORMAT_PCM = (1<<11), ///PCM
		ADECOMX_AUDIO_FORMAT_AMRNB = (1<<12), ///AMR-NB
		ADECOMX_AUDIO_FORMAT_AMRWB = (1<<13), ///AMR-WB
} ADECOMX_AUDIO_FORMAT_T;

typedef struct _ADECOMX_BUF_T
{
    unsigned int u4SA;          ///< [IN]base address of buffer
    unsigned int u4Size;        ///< [IN]buffer size
    unsigned int u4Offset;      ///< [Out]buffer consume offset, the AP can check if the input buffer has been empty or output buffer has been full by checking (u4Offset == u4Size)
    unsigned int u4Timestamp;   ///< store timestamp 
    unsigned int u4Flags;       ///< store flags
} ADECOMX_BUF_T;

typedef enum
{
    ADECOMX_SET_TYPE_WMA_PARAM,
    ADECOMX_SET_TYPE_WMAPRO_PARAM,
    ADECOMX_SET_TYPE_AAC_PARAM,
	ADECOMX_SET_TYPE_DTS_PARAM,
	ADECOMX_SET_TYPE_FLAC_PARAM,
	ADECOMX_SET_TYPE_VORBIS_PARAM,
	ADECOMX_SET_TYPE_LPCM_PARAM,
	ADECOMX_SET_TYPE_ADPCM_PARAM,
	ADECOMX_SET_TYPE_PCM_PARAM,
    ADECOMX_SET_TYPE_MAX
} ADECOMX_SET_TYPE_T;

typedef enum _ADECOMX_MRESULT_T
{
    ADECOMX_MRESULT_OK = 0,           ///< OK
    ADECOMX_MRESULT_FAIL,             ///< Fail
    ADECOMX_MRESULT_FATAL,            ///< Fatal error to stop.
    ADECOMX_MRESULT_MAX = 0x0FFFFFFF
} ADECOMX_MRESULT_T;

/*=============================================================================
 *                             Function Declaration
 *===========================================================================*/

typedef struct _PAR_ADECOMX_CREATE {
    ADECOMX_MRESULT_T          eRet;               // OUT
    ADECOMX_AUDIO_FORMAT_T     eAudioFormat;       // IN
} PAR_ADECOMX_CREATE;

typedef struct _PAR_ADECOMX_DECODE {
    ADECOMX_MRESULT_T           eRet;               // OUT
    ADECOMX_BUF_T               *prInputBuf;        // IN
    ADECOMX_BUF_T               *prOutputBuf;       // IN
} PAR_ADECOMX_DECODE;

typedef struct _PAR_ADECOMX_GETPARAM {
    ADECOMX_MRESULT_T           eRet;               // OUT
    ADECOMX_GET_TYPE_T          eType;              // IN
    void                        *pvInParam;         // IN
    void                        *pvOutParam;        // OUT
} PAR_ADECOMX_GETPARAM;

typedef struct _PAR_ADECOMX_SETPARAM {
    ADECOMX_MRESULT_T           eRet;               // OUT
    ADECOMX_SET_TYPE_T          eType;              // IN
    void                        *pvInParam;         // IN
    void                        *pvOutParam;        // OUT
} PAR_ADECOMX_SETPARAM;

typedef struct __ADECOMX_SET_WMA_PARAM_T
{
    UINT16 u2Channels;
    UINT32 u4SampleRate;
    UINT32 u4BytesPerSec;
    UINT16 u2BlockAlign;
    UINT16 u2EncodeOption;
} ADECOMX_SET_WMA_PARAM_T;

typedef struct __ADECOMX_SET_WMAPRO_PARAM_T
{
    UINT16 u2Channels;
    UINT32 u4SampleRate;
    UINT32 u4BytesPerSec;
    UINT16 u2BlockAlign;
    UINT16 u2ValidBits;
    UINT32 u4ChannelMask;
    UINT32 u4AdvEncodeOpt2;
    UINT16 u2EncodeOption;
    UINT16 u2AdvEncodeOpt;
} ADECOMX_SET_WMAPRO_PARAM_T;

typedef struct __ADECOMX_SET_AAC_PARAM_T
{
    UINT16 u2Channels;
    UINT32 u4SampleRateIndex;
} ADECOMX_SET_AAC_PARAM_T;

typedef struct __ADECOMX_SET_DTS_PARAM_T
{
    UINT8 u1BigEndian; //0: little, 1: big
} ADECOMX_SET_DTS_PARAM_T;

typedef struct __ADECOMX_SET_FLAC_PARAM_T
{
    UINT32   u4TotalSamples;
    UINT32   u4MinFrameSize;
    UINT32   u4MaxFrameSize;
    UINT32   u4Samplerate;
    UINT16   u2MinBlkSize;
    UINT16   u2MaxBlkSize;
    UINT32   u4ChannelNum;
    UINT8    u1BPS;
} ADECOMX_SET_FLAC_PARAM_T;

typedef enum _ADECOMX_LPCM_TYPE_T
{
    LPCM_AUDIO_PCMModeLinear = 0,  /**< Linear PCM encoded data */
    //LPCM_AUDIO_ADPCMModeMS,        /**ADPCM-MS*/ 
    //LPCM_AUDIO_ADPCMModeIMA,       /**ADPCM-IMA*/
    LPCM_AUDIO_PCMModeALaw,        /**< A law PCM encoded data (G.711) */ 
    LPCM_AUDIO_PCMModeMULaw,       /**< Mu law PCM encoded data (G.711)  */ 
    LPCM_AUDIO_PCMModeMax = 0x7FFFFFFF
} ADECOMX_LPCM_TYPE_T;

typedef struct __ADECOMX_SET_LPCM_PARAM_T
{
    ADECOMX_LPCM_TYPE_T eLPCMType;
    UINT32   u4SampleRate;
    UINT32   u4Channels;
    UINT32   u4BlockAlign;   
    UINT32   u4BitsPerSample;  
    UINT8    u1BigEndian;
    UINT8    u1SignedData;
} ADECOMX_SET_LPCM_PARAM_T;

typedef enum _ADECOMX_ADPCM_TYPE_T
{
    LPCM_AUDIO_ADPCMModeMS = 0,        /**< A law PCM encoded data (G.711) */ 
    LPCM_AUDIO_ADPCMModeIMA,       /**< Mu law PCM encoded data (G.711)  */ 
    LPCM_AUDIO_ADPCMModeMax = 0x7FFFFFFF
} ADECOMX_ADPCM_TYPE_T;

typedef struct __ADECOMX_SET_ADPCM_PARAM_T
{
    ADECOMX_ADPCM_TYPE_T eADPCMType;
    UINT32   u4Channels;
    UINT32   u4BitsPerSample;
    UINT32   u4SampleRate;
    UINT32   u4BlockAlign;   
    UINT32   u4FrameSize;  
} ADECOMX_SET_ADPCM_PARAM_T;

typedef struct __ADECOMX_SET_PCM_PARAM_T
{
    UINT32   u4SampleRate;
    UINT32   u4Channels;  
    UINT32   u4BitsPerSample;  
    UINT8    u1BigEndian;
    UINT8    u1SignedData;
    UINT8    u1InterLeaved;
} ADECOMX_SET_PCM_PARAM_T;

#ifdef __cplusplus
}
#endif

#endif //#ifndef _ADECOMX_MOD_H_

