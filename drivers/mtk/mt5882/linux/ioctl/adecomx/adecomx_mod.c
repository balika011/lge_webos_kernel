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

#if defined(LINUX_TURNKEY_SOLUTION) && defined(CC_ENABLE_ADECOMX)

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include <linux/cdev.h>
#include <linux/fs.h>          //For file_operations
#include "x_hal_5381.h"
#include "x_typedef.h"
#include "x_os.h"
#include "x_util.h"
#include "x_printf.h"
#include "x_assert.h"
#include "x_debug.h"
#include "x_common.h"

#include "adecomx_drv_if.h"
#include "OMX_Core.h"
#include <linux/cb_data.h>
#include <linux/version.h>

#include <asm/uaccess.h>
#include "aud_if.h"

#include <linux/time.h>		// For Error Handling

LINT_EXT_HEADER_BEGIN
//#define DEFINE_IS_LOG	AOMX_IsLog
#include "x_debug.h"
LINT_EXT_HEADER_END

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
#define USE_UNLOCK_IOCTL
#endif

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg, type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        Printf("[ADECOMX][ERROR] (L:%d) adecomx_mod argument error\n", __LINE__);     \
        return -1;                                          \
    }

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel, type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(type)))          \
    {                                                       \
        Printf("[ADECOMX][ERROR] (L:%d) adecomx_mod argument error\n", __LINE__);      \
        return -1;                                          \
    }

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel, type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        Printf("[ADECOMX][ERROR] (L:%d) adecomx_mod argument error\n", __LINE__);      \
        return -1;                                          \
    }

typedef struct 
{    
    BOOL    fgOccupied;
    //Decoder information
    AUD_FMT_T eFmt;    
    BOOL    fgPlay;
    BOOL    fgEOS;
    //Input buffer information
    UINT32 u4InputBufSA;
    UINT32 u4InputBufSZ;
    UINT32 u4InputBufRP;
    UINT32 u4InputBufWP;
    UINT32 u4TotalInputSize;
    //Output buffer information
    UINT32 u4OutputBufSA;
    UINT32 u4OutputBufSZ;
    UINT32 u4OutputBufRP;
    UINT32 u4OutputBufWP;
    UINT32 u4TotalOutputSize;

	////////////////////////////////////////
	// Decoder Interface
	//Header Insertion
	UINT32 u4AudHdrOffset;
	UINT32 u4AudHdrSize;
	BOOL   fgEnableAdtsInsert;
    BOOL   fgEnableVorbisHdrInsert;

	//AAC
	UINT8 u1AacChNo; //Default: 2
	UINT8 u1AacSampleRateIdx; //Default: 3 //48K
	UINT8 u1AAC_ADTS[7+1]; // 1 for alignment

    //OggS
    UINT8   u1VORBIS_OggS[27+255];
    UINT32  u4VorbisPage; 

	//Input data buffer
	void*	pAdecInBuf;
	UINT32  u4InBufSize;

	BOOL	fgInitBuffer;
} ADECOMX_HANDLE_T;

#define ADECOMX_INST_MAX  (1)

//---------------------------------------------------------------------------
// Extern functions
//---------------------------------------------------------------------------
extern UINT32 u4GetAFIFOStart(UINT8 uDecIndex);
extern UINT32 u4GetAFIFOEnd(UINT8 uDecIndex);
extern UINT32 u4GetABufPnt(UINT8 uDecIndex);
extern void vSetAWritePnt(UINT8 uDecIndex, UINT32 u4WritePointer);

extern UINT32 u4GetUploadFIFOStart(void);
extern UINT32 u4GetUploadFIFOEnd(void);
extern UINT32 u4GetUploadWritePnt(void);
extern void vSetUploadReadPtr(UINT32 u4ReadPtr);

extern void DSP_FlushInvalidateDCacheFree(UINT32 u4Addr, UINT32 u4Len);

extern void AUD_LineInCtrlEnable(UINT8 u1DecId, BOOL fgEnable);
extern UINT32 u4ReadInputUnderrun(UINT8 u1DecId);
extern void AUD_DspMutePcmOut(BOOL fgEnable);
extern void AUD_DspFastDecode(BOOL fgEnable);
extern BOOL fgDecoderRealPlay(UINT8 u1DecId);
extern void vResetUploadFlag(void);
extern void GST_AOMX_DumpAudioData(UINT8 u1DecId, const char *addr, UINT32 size, BOOL fgnew);
//---------------------------------------------------------------------------
// Public functions
//---------------------------------------------------------------------------

#define FILL_BUFFER_TIME_OUT_US		300 * 1000 	// 300 ms
#define EMPTY_BUFFER_TIME_OUT_US	300 * 1000 	// 300 ms


static BOOL _fgInited = FALSE;
static int _u4AdecOmxOpenCnt = 0;
UINT32 u4ADecOmxDbgMsk = 0;
INT32  u4ADecOmxInitBufSize = -1;
	
typedef enum
{
	ADECOMX_DUMP_DATA						= (1 << 0),
	ADECOMX_SHOW_INPUT_COPY_LOG 			= (1 << 1),
	ADECOMX_SHOW_OUTPUT_COPY_LOG			= (1 << 2),
	ADECOMX_DISABLE_FAST_DECODE 			= (1 << 3),
	ADECOMX_ENABLE_PCM_OUTPUT				= (1 << 4),
	ADECOMX_DISABLE_VORBIS_HEADER_INSERT	= (1 << 5),
	ADECOMX_DISABLE_AAC_HEADER_INSERT		= (1 << 6),
} ADEC_DRV_DBG_MASK_T;


ADECOMX_HANDLE_T _arAdecOmx[ADECOMX_INST_MAX];

static UINT32 u4AudOmxFillFIFO(ADECOMX_HANDLE_T *prAdecOmx, UINT32 u4From, UINT32 u4Size)
{
    UINT32 u4FifoSA, u4FifoEA;
    UINT32 u4FifoRp, u4FifoWp;
    INT32 i4FifoFree, i4Copied = 0;
	UINT32 u4Ret;

	u4FifoSA = prAdecOmx->u4InputBufSA;
	u4FifoEA = prAdecOmx->u4InputBufSA + prAdecOmx->u4InputBufSZ;
	u4FifoWp = prAdecOmx->u4InputBufWP;
	u4FifoRp = VIRTUAL(u4GetABufPnt(AUD_DEC_MAIN));

    if (u4FifoRp >= u4FifoWp)
    {
        // |xxxxxxxx|--------------|xxxxxxxxxxxxxx|
        // SA       Wp             Rp             EA
        //          |--u4FifoFree--|
        if (u4FifoRp == u4FifoWp)
        {
            // if Wp == Rp == SA, it is the first time write FIFO
            // else if Wp == Rp, means FIFO is pure empty
            // it should not be FIFO full case, since we allways avoid filling FIFO to full
            i4FifoFree = u4FifoEA - u4FifoSA - 8; // - 8 to avoid Wp == Rp after updated
            //Printf("[ADECOMX] u4AudOmxFillFIFO: first time write or empty.\n");
        }
        else
        {
            i4FifoFree = u4FifoRp - u4FifoWp - 8; // - 8 to avoid Wp == Rp after updated
        }

        if (i4FifoFree <= 0)
        {
            // copy none
            i4Copied = 0;
        }
        else if (i4FifoFree >= (INT32)u4Size)
        {
            // copy all
            i4Copied = u4Size;
        }
        else
        {
            // copy part
            i4Copied = i4FifoFree;
        }

        if (i4Copied > 0)
        {
            x_memcpy((void*)u4FifoWp, (const void*)u4From, i4Copied);        
            //copy_from_user(u4FifoWp, (void __user *)u4From, i4Copied);                

            // flush cache
            DSP_FlushInvalidateDCacheFree(u4FifoWp, i4Copied);

            u4FifoWp += i4Copied;
        }
    }
    else if (u4FifoRp < u4FifoWp)
    {
        // |--------|xxxxxxxxxxxxxx|--------------|
        // SA       Rp             Wp             EA
        //                         |--u4FifoFree--|

        if (u4FifoRp > u4FifoSA)
        {
            i4FifoFree = u4FifoEA - u4FifoWp;
        }
        else
        {
            // Rp == SA, - 8 to avoid Wp == Rp after updated
            i4FifoFree = u4FifoEA - u4FifoWp - 8;
        }

        if (i4FifoFree <= 0)
        {
            // copy none
            i4Copied = 0;        
        }
        else if (i4FifoFree >= (INT32)u4Size)
        {
            // copy all
            i4Copied = u4Size;
        }
        else
        {
            // copy part
            i4Copied = i4FifoFree;
        }

        if (i4Copied > 0)
        {
            x_memcpy((void*)u4FifoWp, (const void*)u4From, i4Copied);        
            //copy_from_user(u4FifoWp, (void __user *)u4From, i4Copied);

            // flush cache
            DSP_FlushInvalidateDCacheFree(u4FifoWp, i4Copied);

            u4FifoWp += i4Copied;
        }

        // check if need roll back, make sure Wp != EA
        if (u4FifoWp == u4FifoEA)
        {
            u4FifoWp = u4FifoSA;
        }

        // check if need copy part 2
        if ((i4Copied < u4Size) && (u4FifoRp > u4FifoSA))
        {
            INT32 i4Copied2;

            // |--------|xxxxxxxxxxxxxx|--------------|
            // SA       Rp             Wp             EA
            // |--here--|

            i4FifoFree = u4FifoRp - u4FifoSA - 8;

            if (i4FifoFree <= 0)
            {
                // copy none
                i4Copied2 = 0;
            }
            else if (i4FifoFree >= (INT32)(u4Size - i4Copied))
            {
                // copy all
                i4Copied2 = u4Size - i4Copied;
            }
            else
            {
                // copy part
                i4Copied2 = i4FifoFree;
            }

            if (i4Copied2 > 0)
            {
                x_memcpy((void*)u4FifoSA, (const void*)(u4From + i4Copied), i4Copied2);
                //copy_from_user(u4FifoSA, (void __user *)(u4From + i4Copied), i4Copied2);

                // flush cache
                DSP_FlushInvalidateDCacheFree(u4FifoSA, i4Copied2);

                u4FifoWp += i4Copied2;
                i4Copied += i4Copied2;
            }
        }
    }

    // check if need roll back
    if (u4FifoWp == u4FifoEA)
    {
        u4FifoWp = u4FifoSA;
    }
	
	if (prAdecOmx->fgInitBuffer) 
	{
		if (prAdecOmx->u4TotalInputSize > u4ADecOmxInitBufSize)
		{
			prAdecOmx->fgInitBuffer = FALSE;
			Printf("Afifo got enough data (%d). Start playback!\n", prAdecOmx->u4TotalInputSize);
			vSetAWritePnt(AUD_DEC_MAIN, PHYSICAL(u4FifoWp));			
		}
	} 
	else 
	{
	    // set new Wptr
	    vSetAWritePnt(AUD_DEC_MAIN, PHYSICAL(u4FifoWp));
	}

    //Update Wptr
    prAdecOmx->u4InputBufRP = u4FifoRp;
    prAdecOmx->u4InputBufWP = u4FifoWp;
    prAdecOmx->u4TotalInputSize += i4Copied;

    if (u4ADecOmxDbgMsk & ADECOMX_SHOW_INPUT_COPY_LOG)
    {
        Printf("[Input] i4Filled: %d Total: %d\n", i4Copied, prAdecOmx->u4TotalInputSize);
    }

	if(u4ADecOmxDbgMsk & ADECOMX_DUMP_DATA)
	{
		void *tmp = (void *)x_mem_alloc(i4Copied);
		if (tmp) {
			u4Ret = copy_from_user(tmp, (void __user *)(u4From), i4Copied);
			GST_AOMX_DumpAudioData(AUD_DEC_MAIN, (const char *) tmp, i4Copied, FALSE);
			x_mem_free(tmp);
		}
	}
    
    return i4Copied;
}

int _adecomx_outputbuf_copy(ADECOMX_HANDLE_T *prAdecOmx, ADECOMX_BUF_T *prOutputBuf)
{    
    UINT32 u4From, u4MaxSize;
    INT32 i4Filled = 0, i4OutputAvail;
    UINT32 u4OutputSA, u4OutputEA, u4OutputRp, u4OutputWp;
	UINT32 u4Ret;

    u4OutputSA = prAdecOmx->u4OutputBufSA;
    u4OutputEA = prAdecOmx->u4OutputBufSA + prAdecOmx->u4OutputBufSZ;
    u4OutputRp = prAdecOmx->u4OutputBufRP;
    u4OutputWp = VIRTUAL(u4GetUploadWritePnt());

    u4From = prOutputBuf->u4SA + prOutputBuf->u4Offset;
    u4MaxSize = prOutputBuf->u4Size - prOutputBuf->u4Offset;

    if (u4ADecOmxDbgMsk & ADECOMX_SHOW_OUTPUT_COPY_LOG)
    {
        Printf("[Output] u4SA: %x u4Offset: %d u4Size: %d u4OutputSA: %x u4OutputEA: %x u4OutputRp: %x u4OutputWp: %x\n",
                prOutputBuf->u4SA, prOutputBuf->u4Offset, prOutputBuf->u4Size, u4OutputSA, u4OutputEA, u4OutputRp, u4OutputWp);
    }

    if (u4OutputWp == u4OutputRp)
    {
        // Wp == Rp, think the buffer is empty
        // DSP will not write buffer to full
    }
    else if (u4OutputWp > u4OutputRp)
    {
        // |--------|xxxxxxxxxxxxxx|--------------|
        // SA       Rp             Wp             EA
        //          |-----here-----|

        i4OutputAvail = u4OutputWp - u4OutputRp;

        if (i4OutputAvail < 0)
        {
            // get none
            i4Filled = 0;
        }
        else if (i4OutputAvail < u4MaxSize)
        {
            // get all decoded samples
            i4Filled = i4OutputAvail;
        }
        else
        {
            // get part
            i4Filled = u4MaxSize;
        }

        if (i4Filled > 0)
        {
            // invalidate cache
            DSP_FlushInvalidateDCacheFree(u4OutputRp, i4Filled);

            // copy PCM samples from decoder output buffer to OpenMAX output buffer
            //x_memcpy((void*)u4From, (const void*)u4OutputRp, i4Filled);
            u4Ret = copy_to_user((void __user *)u4From, (void *)u4OutputRp, i4Filled);

            u4OutputRp += i4Filled;
        }
    }
    else if (u4OutputWp < u4OutputRp)
    {
        // |xxxxxxxx|--------------|xxxxxxxxxxxxxx|
        // SA       Wp             Rp             EA
        //                         |-----here-----|

        i4OutputAvail = u4OutputEA - u4OutputRp;

        if (i4OutputAvail < 0)
        {
            // get none
            i4Filled = 0;
        }
        else if (i4OutputAvail < u4MaxSize)
        {
            // get all decoded samples
            i4Filled = i4OutputAvail;
        }
        else
        {
            // get part
            i4Filled = u4MaxSize;
        }

        if (i4Filled > 0)
        {
            // invalidate cache
            DSP_FlushInvalidateDCacheFree(u4OutputRp, i4Filled);

            // copy PCM samples from decoder output buffer to OpenMAX output buffer
            //x_memcpy((void*)u4From, (const void*)u4OutputRp, i4Filled);
            u4Ret = copy_to_user((void __user *)u4From, (void *)u4OutputRp, i4Filled);

            u4OutputRp += i4Filled;
        }

        // check if need roll back, make sure Rp != EA
        if (u4OutputRp == u4OutputEA)
        {
            u4OutputRp = u4OutputSA;
        }

        if ((i4Filled < u4MaxSize) && (u4OutputWp > u4OutputSA)) // avoid case u4OutputWp == u4OutputSA
        {
            INT32 i4Filled2;

            // |xxxxxxxx|--------------|xxxxxxxxxxxxxx|
            // SA       Wp             Rp             EA
            // |--here--|

            i4OutputAvail = u4OutputWp - u4OutputSA;

            if (i4OutputAvail < 0)
            {
                // get none
                i4Filled2 = 0;
            }
            else if (i4OutputAvail < (u4MaxSize - i4Filled))
            {
                // get all decoded samples
                i4Filled2 = i4OutputAvail;
            }
            else
            {
                // get part
                i4Filled2 = u4MaxSize - i4Filled;
            }

            if (i4Filled2 > 0)
            {
                // invalidate cache
                DSP_FlushInvalidateDCacheFree(u4OutputSA, i4Filled2);

                // copy PCM samples from decoder output buffer to OpenMAX output buffer
                //x_memcpy((void*)(u4From + i4Filled), (const void*)u4OutputSA, i4Filled2);
                u4Ret = copy_to_user((void __user *)(u4From + i4Filled), (void *)u4OutputSA, i4Filled2);

                u4OutputRp += i4Filled2;
                i4Filled   += i4Filled2;
            }
        }
    }

    // check if need roll back, make sure Rp != EA
    if (u4OutputRp == u4OutputEA)
    {
        u4OutputRp = u4OutputSA;
    }

    // set new read pointer
    vSetUploadReadPtr(PHYSICAL(u4OutputRp));

    // update new read pointer
    prAdecOmx->u4OutputBufRP = u4OutputRp;
    prAdecOmx->u4OutputBufWP = u4OutputWp;
    prOutputBuf->u4Offset += i4Filled;
    prAdecOmx->u4TotalOutputSize += i4Filled;

    if (u4ADecOmxDbgMsk & ADECOMX_SHOW_OUTPUT_COPY_LOG)
    {
        Printf("[Output] i4Filled: %d Total: %d\n", i4Filled, prAdecOmx->u4TotalOutputSize);
    }

    return i4Filled;
}

int adecomx_create(ADECOMX_HANDLE_T *prAdecOmx, unsigned long arg)
{
    PAR_ADECOMX_CREATE rCreate;
    AUD_FMT_T eFmt;

    Printf("[ADECOMX] %s\n", __FUNCTION__);
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_ADECOMX_CREATE);
    COPY_FROM_USER_ARG(arg, rCreate, PAR_ADECOMX_CREATE);

    switch(rCreate.eAudioFormat)
    {
        case ADECOMX_AUDIO_FORMAT_MP3:
            eFmt = AUD_FMT_MP3;
            break;
		case ADECOMX_AUDIO_FORMAT_WMA:
			eFmt = AUD_FMT_WMA;
			break;
		case ADECOMX_AUDIO_FORMAT_WMAPRO:
			eFmt = AUD_FMT_WMAPRO;
			break;
		case ADECOMX_AUDIO_FORMAT_AAC:
			eFmt = AUD_FMT_AAC;
			break;
        case ADECOMX_AUDIO_FORMAT_DTS:
            eFmt = AUD_FMT_DTS;
            break;
        case ADECOMX_AUDIO_FORMAT_MPEG:
            eFmt = AUD_FMT_MPEG;
            break;
        case ADECOMX_AUDIO_FORMAT_AC3:
            eFmt = AUD_FMT_AC3;
            break;
        case ADECOMX_AUDIO_FORMAT_FLAC:
            eFmt = AUD_FMT_FLAC;
            break;
        case ADECOMX_AUDIO_FORMAT_VORBIS:
            eFmt = AUD_FMT_VORBISDEC;
            break;
        case ADECOMX_AUDIO_FORMAT_LPCM:
            eFmt = AUD_FMT_LPCM;
            break;
        case ADECOMX_AUDIO_FORMAT_ADPCM:
            eFmt = AUD_FMT_LPCM;
            break;
        case ADECOMX_AUDIO_FORMAT_PCM:
            eFmt = AUD_FMT_LPCM;
            break;
        case ADECOMX_AUDIO_FORMAT_AMRNB:
            eFmt = AUD_FMT_AMR;
            break;
        case ADECOMX_AUDIO_FORMAT_AMRWB:
            eFmt = AUD_FMT_AWB;
            break;
        default:
            eFmt = AUD_FMT_UNKNOWN; 
            break;
    }
    if(eFmt == AUD_FMT_UNKNOWN)
    {
        Printf("[ADECOMX][ERROR} %s(L:%d) not support format(%d)\n", __FUNCTION__, __LINE__, rCreate.eAudioFormat);
        return -1;
    }

    //Driver related operation

		//Disable upload path
		AUD_DspSetUploadMode(0);
		AUD_DspUploadDataEnable();		  
		vResetUploadFlag(); 	   

        //Stop all decoder first
        AUD_DSPCmdStop(AUD_DEC_MAIN);
        AUD_DSPCmdStop(AUD_DEC_AUX);
#ifdef CC_MT5391_AUD_3_DECODER
        AUD_DSPCmdStop(AUD_DEC_THIRD);
#endif
#ifdef CC_AUD_4_DECODER_SUPPORT
        AUD_DSPCmdStop(AUD_DEC_4TH);
#endif

        //Load decode code
        AUD_SetDecType(AUD_DEC_MAIN, AUD_STREAM_FROM_MEMORY, eFmt);

    //Update prAdexOmx related variable
    prAdecOmx->eFmt = eFmt;
    prAdecOmx->fgPlay = FALSE;
    prAdecOmx->fgEOS = FALSE;
    
    prAdecOmx->u4InputBufSA  = (UINT32)(VIRTUAL(u4GetAFIFOStart(AUD_DEC_MAIN)));
    prAdecOmx->u4InputBufSZ = (UINT32)(VIRTUAL(u4GetAFIFOEnd(AUD_DEC_MAIN))) - prAdecOmx->u4InputBufSA;
    prAdecOmx->u4InputBufRP = prAdecOmx->u4InputBufWP = prAdecOmx->u4InputBufSA;
    prAdecOmx->u4TotalInputSize = 0;

    prAdecOmx->u4OutputBufSA = (UINT32)(VIRTUAL(u4GetUploadFIFOStart()));
    prAdecOmx->u4OutputBufSZ = (UINT32)(VIRTUAL(u4GetUploadFIFOEnd())) - prAdecOmx->u4OutputBufSA;
    prAdecOmx->u4OutputBufRP = prAdecOmx->u4OutputBufWP = prAdecOmx->u4OutputBufSA;
    prAdecOmx->u4TotalOutputSize = 0;

    prAdecOmx->fgEnableAdtsInsert = (u4ADecOmxDbgMsk & ADECOMX_DISABLE_AAC_HEADER_INSERT) ? FALSE : TRUE;	
    prAdecOmx->fgEnableVorbisHdrInsert = (u4ADecOmxDbgMsk & ADECOMX_DISABLE_VORBIS_HEADER_INSERT) ? FALSE : TRUE;
	
	prAdecOmx->u1AacChNo = 2;
	prAdecOmx->u1AacSampleRateIdx = 4;
	prAdecOmx->u4AudHdrSize = 0;
	prAdecOmx->u4AudHdrOffset = 0;

	prAdecOmx->u4VorbisPage = 0;	

	prAdecOmx->pAdecInBuf = NULL;
	prAdecOmx->u4InBufSize = 0;

	// For DTS only, buffer some data to avoid sound break;
	prAdecOmx->fgInitBuffer =  (prAdecOmx->eFmt == AUD_FMT_DTS) ? TRUE : FALSE;
	if (u4ADecOmxInitBufSize == -1) 
	{
		u4ADecOmxInitBufSize = prAdecOmx->u4InputBufSZ / 4;
	}

	if(u4ADecOmxDbgMsk & ADECOMX_DUMP_DATA)
	{
		GST_AOMX_DumpAudioData(AUD_DEC_MAIN, 0, 0, TRUE);
	}

    rCreate.eRet = ADECOMX_MRESULT_OK;
    COPY_TO_USER_ARG(arg, rCreate, PAR_ADECOMX_CREATE);

    return 0;
}

int adecomx_DoEmptyBuffer(ADECOMX_HANDLE_T *prAdecOmx, unsigned long arg)
{
    PAR_ADECOMX_DECODE rDecode;
    ADECOMX_BUF_T rInputBuf;
	UINT32 u4SendSize = 0;
	//struct timeval tvStart, tvNow;
	ADECOMX_MRESULT_T eRet = ADECOMX_MRESULT_OK;
	UINT32 u4Ret;
	
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_ADECOMX_DECODE);
    COPY_FROM_USER_ARG(arg, rDecode, PAR_ADECOMX_DECODE);

    if(rDecode.prInputBuf)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(rDecode.prInputBuf, ADECOMX_BUF_T);
        COPY_FROM_USER_ARG(rDecode.prInputBuf, rInputBuf, ADECOMX_BUF_T);
    }
    else
    {
		Printf("[ADECOMX] %s(L:%d) prInputBuf null\n", __FUNCTION__, __LINE__);
		return -1;
    }

	if (rDecode.prInputBuf && (rInputBuf.u4Flags & 1))
	{
		prAdecOmx->fgEOS = TRUE;
		Printf("[ADECOMX] OMX_BUFFERFLAG_EOS!!!\n");
	}

    if(rDecode.prInputBuf && (rInputBuf.u4Offset != rInputBuf.u4Size))		// Copy input data into temp buffer
	{
		prAdecOmx->pAdecInBuf = x_mem_alloc(rInputBuf.u4Size);
		if (!prAdecOmx->pAdecInBuf) {
			Printf("[ADECOMX] %s(L:%d) Allocate memory fail!!!\n", __FUNCTION__, __LINE__);
			return -1;
		}
		u4Ret = copy_from_user(prAdecOmx->pAdecInBuf, 
						   (void __user *)(rInputBuf.u4SA + rInputBuf.u4Offset), 
						   rInputBuf.u4Size - rInputBuf.u4Offset);
		if (u4Ret < 0)
		{
			Printf("[ADECOMX] %s(L:%d) copy_from_user fail!!!\n", __FUNCTION__, __LINE__);
			return -1;
		}
		prAdecOmx->u4InBufSize = rInputBuf.u4Size - rInputBuf.u4Offset;
	}


    if (!prAdecOmx->fgPlay)
    {
        //Disable Line-In
        AUD_LineInCtrlEnable(AUD_DEC_MAIN,FALSE);

		AUD_DspMutePcmOut((u4ADecOmxDbgMsk & ADECOMX_ENABLE_PCM_OUTPUT) ? FALSE : TRUE);
       	//AUD_DspFastDecode((u4ADecOmxDbgMsk & ADECOMX_DISABLE_FAST_DECODE) ? FALSE : TRUE);
        
        //Set AV Sync mode as free-run
        AUD_SetAvSynMode(AUD_DEC_MAIN, AV_SYNC_FREE_RUN); //TODO - backup
        
        //Reset AFIFO write ptr
        prAdecOmx->u4InputBufWP = prAdecOmx->u4InputBufSA;
        vSetAWritePnt(AUD_DEC_MAIN, PHYSICAL(prAdecOmx->u4InputBufSA));

        //Enable Upload Path
        AUD_DspSetUploadMode(8);
        AUD_DspUploadDataEnable();

        //Bypass post-proc
        //AUD_DspBypassPostProcess(AUD_DECMAIN, 0x2000);

        //DDT line-mode
        //AUD_DspDdtDmxRfMode(0,0,FALSE);
        
        //Speaker Config
        //AUD_DspSpeakerOutputConfig(0,0);  //TODO - backup
        AUD_DSPCmdPlay(AUD_DEC_MAIN);

        prAdecOmx->fgPlay = TRUE;

        Printf("[ADECOMX] %s(L:%d) start play\n", __FUNCTION__, __LINE__);
    }

    //Handle InputBuf
    //do_gettimeofday(&tvStart);
    while (u4SendSize < prAdecOmx->u4InBufSize) 	//There are still input data, while loop
    {
    	if ((prAdecOmx->eFmt == AUD_FMT_AAC) &&
			(prAdecOmx->fgEnableAdtsInsert) &&
			(rInputBuf.u4Offset == 0) &&
			(prAdecOmx->u4AudHdrSize == 0))
    	{
			if (!((((UINT8*)prAdecOmx->pAdecInBuf)[0] == 0xFF) && 
				   ((((UINT8*)prAdecOmx->pAdecInBuf)[1] & 0xF0) == 0xF0))) 
			{
				   
				// Insert ADTS Header
				
				prAdecOmx->u1AAC_ADTS[0] = 0xFF;
				prAdecOmx->u1AAC_ADTS[1] = 0xF9;
				prAdecOmx->u1AAC_ADTS[2] = (1<<6) | ((prAdecOmx->u1AacSampleRateIdx<<2) & 0x3C) | ((prAdecOmx->u1AacChNo>>2) & 0x1);
				prAdecOmx->u1AAC_ADTS[3] = ((prAdecOmx->u1AacChNo & 0x3)<<6) | (((rInputBuf.u4Size + 7)>>11)&0x3);
				prAdecOmx->u1AAC_ADTS[4] = (((rInputBuf.u4Size + 7)>> 3) & 0xFF);
				prAdecOmx->u1AAC_ADTS[5] = (((rInputBuf.u4Size + 7)<<5) & 0xE0) | ((0x7FF>>6)&0x1F);
				prAdecOmx->u1AAC_ADTS[6] = ((0x7FF<<2)&0xFC);
				
				prAdecOmx->u4AudHdrOffset = 0;
				prAdecOmx->u4AudHdrSize = 7;
			}
		}

        if ((prAdecOmx->eFmt == AUD_FMT_VORBISDEC) &&
            (prAdecOmx->fgEnableVorbisHdrInsert) &&
            (rInputBuf.u4Offset == 0) &&
            (prAdecOmx->u4AudHdrSize == 0))
        {
        	if (!((((UINT8*)(prAdecOmx->pAdecInBuf))[0] == 0x4F) && 
			  	(((UINT8*)(prAdecOmx->pAdecInBuf))[1]  == 0x67) && 
			  	(((UINT8*)(prAdecOmx->pAdecInBuf))[2]  == 0x67) && 
			  	(((UINT8*)(prAdecOmx->pAdecInBuf))[3]  == 0x53))) 
        	{
	            UINT8   u1Temp;
	            UINT32  u4EmptySize1;  

	            // Insert Vorbis Header
	            prAdecOmx->u1VORBIS_OggS[0] = 0x4F;
	            prAdecOmx->u1VORBIS_OggS[1] = 0x67;
	            prAdecOmx->u1VORBIS_OggS[2] = 0x67;
	            prAdecOmx->u1VORBIS_OggS[3] = 0x53;
	            //"Version" must be 0x00
	            prAdecOmx->u1VORBIS_OggS[4] = 0x00;
	            //"Type": =1 continue page, =2 BOS, =4 BOE
	            prAdecOmx->u1VORBIS_OggS[5] = 0x00;
	            //"Granule=8, S/N=4, Page no=4, CRC=4" bytes
	            for (u1Temp = 6; u1Temp < 26; u1Temp++)
	            {
	                prAdecOmx->u1VORBIS_OggS[u1Temp] = 0x00;
	            }
	            //Set "Page" Information
	            prAdecOmx->u1VORBIS_OggS[18] = (UINT8)(prAdecOmx->u4VorbisPage & 0xFF);
	            prAdecOmx->u1VORBIS_OggS[19] = (UINT8)((prAdecOmx->u4VorbisPage>>8) & 0xFF);
	            prAdecOmx->u1VORBIS_OggS[20] = (UINT8)((prAdecOmx->u4VorbisPage>>16) & 0xFF);
	            prAdecOmx->u1VORBIS_OggS[21] = (UINT8)((prAdecOmx->u4VorbisPage>>24) & 0xFF);
	            prAdecOmx->u4VorbisPage += 1;

	            if ((rInputBuf.u4Size) <= 0xFF)
	            {
	                u1Temp = 1;       // Only one segment
	                prAdecOmx->u1VORBIS_OggS[26] = 0x01;
	                prAdecOmx->u1VORBIS_OggS[27] = rInputBuf.u4Size;
	            }
	            else
	            {
	                u1Temp = 1;       // Need accumulate segments number
	                u4EmptySize1 = rInputBuf.u4Size;
	                while (u4EmptySize1 > 0xFF)
	                {
	                    u4EmptySize1 -= 0xFF;
	                    // Set Segments Table
	                    prAdecOmx->u1VORBIS_OggS[26+u1Temp] = 0xFF; // [26+1] ~ [26+255]
	                    u1Temp += 1;
	                }
	                // "Segments Number"=1
	                prAdecOmx->u1VORBIS_OggS[26] = u1Temp;
	                // Last Segment of "Segment Table"
	                prAdecOmx->u1VORBIS_OggS[26+u1Temp] = u4EmptySize1;
	            }

	            prAdecOmx->u4AudHdrOffset = 0;
	            prAdecOmx->u4AudHdrSize = 27+u1Temp;
							
	        }
			else	// If there is one packet with ogg header, we think all the packet with ogg header
			{
				prAdecOmx->fgEnableVorbisHdrInsert = FALSE;
			}
        }

        if (prAdecOmx->u4AudHdrSize)
		{
            UINT32 u4HeaderSendSize = 0;
			if (prAdecOmx->eFmt == AUD_FMT_AAC)
			{	
				u4HeaderSendSize = u4AudOmxFillFIFO(prAdecOmx, (UINT32)(prAdecOmx->u1AAC_ADTS) + prAdecOmx->u4AudHdrOffset, prAdecOmx->u4AudHdrSize);       
			}

            if (prAdecOmx->eFmt == AUD_FMT_VORBISDEC)
            {
                u4HeaderSendSize = u4AudOmxFillFIFO(prAdecOmx, (UINT32)(prAdecOmx->u1VORBIS_OggS) + prAdecOmx->u4AudHdrOffset, prAdecOmx->u4AudHdrSize);    
            }

            prAdecOmx->u4AudHdrOffset += u4HeaderSendSize;
            prAdecOmx->u4AudHdrSize -= u4HeaderSendSize;
		}
        
        // copy input data from the rInputBuf to prAdecOmx memory address
		if (!prAdecOmx->u4AudHdrSize)
		{
			u4SendSize += u4AudOmxFillFIFO(prAdecOmx, (UINT32)prAdecOmx->pAdecInBuf + u4SendSize, prAdecOmx->u4InBufSize - u4SendSize);
		}

		if (u4SendSize < prAdecOmx->u4InBufSize)
		{
			x_thread_delay(5);	// Afifo is full. Let's slow down
		}
		/*
		do_gettimeofday(&tvNow);
		if (((tvNow.tv_sec - tvStart.tv_sec) * 1000000 + (tvNow.tv_usec - tvStart.tv_usec)) > EMPTY_BUFFER_TIME_OUT_US)
		{
			Printf("[ADECOMX] Empty Buffer Time Out!\n");
			eRet = ADECOMX_MRESULT_FAIL;
			break;
		}
		*/
    }

	rInputBuf.u4Offset += u4SendSize;
	
	if (prAdecOmx->pAdecInBuf) {
		x_mem_free(prAdecOmx->pAdecInBuf);
		prAdecOmx->pAdecInBuf = NULL;
		prAdecOmx->u4InBufSize = 0;
	}

	COPY_TO_USER_ARG(rDecode.prInputBuf, rInputBuf, ADECOMX_BUF_T);
 	rDecode.eRet = eRet;
	COPY_TO_USER_ARG(arg, rDecode, PAR_ADECOMX_DECODE);

	return 0;

}

int adecomx_DoFillBuffer(ADECOMX_HANDLE_T *prAdecOmx, unsigned long arg)
{
    ADECOMX_BUF_T rOutputBuf;
	PAR_ADECOMX_DECODE rDecode;
	UINT32 u4RetryCount = 0;
	struct timeval tvStart, tvNow;
	ADECOMX_MRESULT_T eRet = ADECOMX_MRESULT_OK;
	UINT32 u4PreInputBufRP = prAdecOmx->u4InputBufRP;
	
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_ADECOMX_DECODE);
    COPY_FROM_USER_ARG(arg, rDecode, PAR_ADECOMX_DECODE);

    if(rDecode.prOutputBuf)
    {
        USR_SPACE_ACCESS_VALIDATE_ARG(rDecode.prOutputBuf, ADECOMX_BUF_T);
        COPY_FROM_USER_ARG(rDecode.prOutputBuf, rOutputBuf, ADECOMX_BUF_T);
    }
    else
    {
        Printf("[ADECOMX] %s(L:%d) prOutputBuf null\n", __FUNCTION__, __LINE__);
		return -1;
	}

	do_gettimeofday(&tvStart);
    while (rOutputBuf.u4Offset != rOutputBuf.u4Size)
	{
		int i4Filled = _adecomx_outputbuf_copy(prAdecOmx, &rOutputBuf);
		if (i4Filled < 0)
		{
			Printf("[ADECOMX][ERROR] %s(L:%d) _adecomx_outputbuf_copy fail\n", __FUNCTION__, __LINE__);
			return -1;
		}

		if (i4Filled == 0) {
			x_thread_delay(2);		// TODO: Refine this time?
			if (prAdecOmx->fgEOS) {
				prAdecOmx->u4InputBufRP = VIRTUAL(u4GetABufPnt(AUD_DEC_MAIN));
				if (u4PreInputBufRP == prAdecOmx->u4InputBufRP)
				{
					u4RetryCount++;
					//Printf("[ADECOMX] EOS Received. Afifo Rp(%x) Wp(%x) empty retry = %d\n", 
					//		prAdecOmx->u4InputBufRP, prAdecOmx->u4InputBufWP, u4RetryCount);
				} 
				else 
				{
					u4RetryCount = 0;
				}

				// TODO: Need to Refine this time?
				if (u4RetryCount == 25)	// 50ms, it's a long time...Now we are sure everything is done~
				{
					Printf("[ADECOMX] EOS Received. Afifo Empty. Notify EOS!\n");
					rOutputBuf.u4Flags = 1;
					break;
				}
				u4PreInputBufRP = prAdecOmx->u4InputBufRP;
			}
		}
		
		do_gettimeofday(&tvNow);
		if (((tvNow.tv_sec - tvStart.tv_sec) * 1000000 + (tvNow.tv_usec - tvStart.tv_usec)) > FILL_BUFFER_TIME_OUT_US)
		{
			// Though timed out, don't notify error. For there maybe no input data
			Printf("[ADECOMX] Fill Buffer Time Out!\n");
			break;
		}
	}

	COPY_TO_USER_ARG(rDecode.prOutputBuf, rOutputBuf, ADECOMX_BUF_T);
	rDecode.eRet = eRet;
	COPY_TO_USER_ARG(arg, rDecode, PAR_ADECOMX_DECODE);

	return 0;
}

int adecomx_decode(ADECOMX_HANDLE_T *prAdecOmx, unsigned long arg)
{
	Printf("[ADECOMX] adecomx_decode is obsolete. Please use DoEmptyBuffer and DoFillBuffer instead.\n");
	return 0;
}

int adecomx_flush(ADECOMX_HANDLE_T *prAdecOmx, unsigned long arg)
{
    Printf("[ADECOMX] %s(fgPlay: %d)\n", __FUNCTION__, prAdecOmx->fgPlay);
    
    if (prAdecOmx->fgPlay)
    {
        //Stop Decoder
        //Disable Upload Path
        AUD_DspSetUploadMode(0);
        AUD_DspUploadDataEnable();
        vResetUploadFlag();                
        AUD_DSPCmdStop(AUD_DEC_MAIN);
        
        //Reset AFIFO write ptr
        prAdecOmx->u4InputBufRP = prAdecOmx->u4InputBufWP = prAdecOmx->u4InputBufSA;        
        prAdecOmx->u4OutputBufRP = prAdecOmx->u4OutputBufWP = prAdecOmx->u4OutputBufSA;
        prAdecOmx->fgEOS = FALSE;        
        vSetAWritePnt(AUD_DEC_MAIN, PHYSICAL(prAdecOmx->u4InputBufSA));

        //Enable Upload Path
        AUD_DspSetUploadMode(8);
        AUD_DspUploadDataEnable();

        //Play Decoder
        AUD_DSPCmdPlay(AUD_DEC_MAIN);
    }

    //TODO - update status
    return 0;
}

int adecomx_getparam(ADECOMX_HANDLE_T *prAdecOmx, unsigned long arg)
{
    PAR_ADECOMX_GETPARAM rGetparam;

    Printf("[ADECOMX] %s\n", __FUNCTION__);
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_ADECOMX_GETPARAM);
    COPY_FROM_USER_ARG(arg, rGetparam, PAR_ADECOMX_GETPARAM);
    
    switch (rGetparam.eType)
    {
        default:
            rGetparam.eRet = ADECOMX_MRESULT_FAIL;
            break;
    }

    COPY_TO_USER_ARG(arg, rGetparam, PAR_ADECOMX_GETPARAM);

    return 0;
}

int adecomx_setparam(ADECOMX_HANDLE_T *prAdecOmx, unsigned long arg)
{
    PAR_ADECOMX_SETPARAM rSetparam;

    Printf("[ADECOMX] %s\n", __FUNCTION__);
    
    USR_SPACE_ACCESS_VALIDATE_ARG(arg, PAR_ADECOMX_SETPARAM);
    COPY_FROM_USER_ARG(arg, rSetparam, PAR_ADECOMX_SETPARAM);
    
    switch (rSetparam.eType)
    {
        case ADECOMX_SET_TYPE_WMA_PARAM:
            if (rSetparam.pvInParam)
            {
                AUD_WMA_CFG_T rWMAInfo = {0};
            
                ADECOMX_SET_WMA_PARAM_T rParam;
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, ADECOMX_SET_WMA_PARAM_T);
                COPY_FROM_USER_ARG(rSetparam.pvInParam, rParam, ADECOMX_SET_WMA_PARAM_T);

                rWMAInfo.u2HdrSrc = 1;
                rWMAInfo.u4Pktsz = rParam.u2BlockAlign;
                rWMAInfo.u2Numch = rParam.u2Channels;
                rWMAInfo.u4Sampersec = rParam.u4SampleRate;
                rWMAInfo.u4Bpersec = rParam.u4BytesPerSec;
                rWMAInfo.u4Blocksz = rParam.u2BlockAlign;
                rWMAInfo.u2Encoderopt = rParam.u2EncodeOption;

                AUD_DspWMAHeader(AUD_DEC_MAIN, &rWMAInfo);
            }
            rSetparam.eRet = ADECOMX_MRESULT_OK;                
            break;
        case ADECOMX_SET_TYPE_WMAPRO_PARAM:
            if (rSetparam.pvInParam)
            {
                AUD_WMA_CFG_T rWMAPROInfo = {0};
            
                ADECOMX_SET_WMAPRO_PARAM_T rParam;
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, ADECOMX_SET_WMAPRO_PARAM_T);
                COPY_FROM_USER_ARG(rSetparam.pvInParam, rParam, ADECOMX_SET_WMAPRO_PARAM_T);

                rWMAPROInfo.u2HdrSrc = 1;
                rWMAPROInfo.u4Pktsz = rParam.u2BlockAlign;
                rWMAPROInfo.u2Numch = rParam.u2Channels;
                rWMAPROInfo.u4Sampersec = rParam.u4SampleRate;
                rWMAPROInfo.u4Bpersec = rParam.u4BytesPerSec;
                rWMAPROInfo.u4Blocksz = rParam.u2BlockAlign;
                rWMAPROInfo.u2Encoderopt = rParam.u2EncodeOption;
                rWMAPROInfo.u4ChannelMask = rParam.u4ChannelMask;
                rWMAPROInfo.u2BitsPerSample = rParam.u2ValidBits;
                rWMAPROInfo.u2AdvEncoderopt = rParam.u2AdvEncodeOpt;
                rWMAPROInfo.u4AdvEncoderopt2= rParam.u4AdvEncodeOpt2;
                
                AUD_DspWMAProHeader(AUD_DEC_MAIN, &rWMAPROInfo);                    
            }                
            rSetparam.eRet = ADECOMX_MRESULT_OK;
            break;
		case ADECOMX_SET_TYPE_AAC_PARAM:
            if (rSetparam.pvInParam)
            {
                ADECOMX_SET_AAC_PARAM_T rParam;
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, ADECOMX_SET_AAC_PARAM_T);
                COPY_FROM_USER_ARG(rSetparam.pvInParam, rParam, ADECOMX_SET_AAC_PARAM_T);
				
				prAdecOmx->u1AacChNo = rParam.u2Channels;
				prAdecOmx->u1AacSampleRateIdx = rParam.u4SampleRateIndex;
				
				Printf("[ADECOMX] Set AAC Param : SampleRateIndex(%d), Channels(%d)\n", 
					prAdecOmx->u1AacSampleRateIdx, prAdecOmx->u1AacChNo);
            }                
            rSetparam.eRet = ADECOMX_MRESULT_OK;			
			break;
        case ADECOMX_SET_TYPE_DTS_PARAM:
            if (rSetparam.pvInParam)
            {
                ADECOMX_SET_DTS_PARAM_T rParam;
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, ADECOMX_SET_DTS_PARAM_T);
                COPY_FROM_USER_ARG(rSetparam.pvInParam, rParam, ADECOMX_SET_DTS_PARAM_T);

                AUD_SetDTSInfo((rParam.u1BigEndian==1) ? DEC_BIG_ENDIAN : DEC_LITTLE_ENDIAN);                
            }                
            rSetparam.eRet = ADECOMX_MRESULT_OK;            
            break;
        case ADECOMX_SET_TYPE_FLAC_PARAM:
            if (rSetparam.pvInParam)
            {
                ADECOMX_SET_FLAC_PARAM_T rParam;
				AUD_FLAC_INFO_T rFLACInfo = {0};
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, ADECOMX_SET_FLAC_PARAM_T);
                COPY_FROM_USER_ARG(rSetparam.pvInParam, rParam, ADECOMX_SET_FLAC_PARAM_T);

                rFLACInfo.u2BPS = (UINT16)rParam.u1BPS;
                rFLACInfo.u2MaxBlkSz = rParam.u2MaxBlkSize;
                rFLACInfo.u2MinBlkSz = rParam.u2MinBlkSize;
                rFLACInfo.u2ChannelNum = (UINT16)rParam.u4ChannelNum;
                rFLACInfo.u4MaxFrmSz = rParam.u4MaxFrameSize;
                rFLACInfo.u4MinFrmSz = rParam.u4MinFrameSize;
                rFLACInfo.u4SmpRate = rParam.u4Samplerate;

                AUD_DspFLACHeader(AUD_DEC_MAIN, &rFLACInfo);               
            }   
            rSetparam.eRet = ADECOMX_MRESULT_OK;   
            break;
        case ADECOMX_SET_TYPE_LPCM_PARAM:
            if (rSetparam.pvInParam)
            {
                ADECOMX_SET_LPCM_PARAM_T rParam;
				AUD_LPCM_INFO_T rLPCMInfo = {0};
                UINT32  u4Tmp = 0;
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, ADECOMX_SET_LPCM_PARAM_T);
                COPY_FROM_USER_ARG(rSetparam.pvInParam, rParam, ADECOMX_SET_LPCM_PARAM_T);
                
                // Sample Frequency
                u4Tmp = (UINT32)(rParam.u4SampleRate / 1000);
                switch (u4Tmp)
                {
                    case 48:
                        rLPCMInfo.eSampleFreq = FS_48K;
                        break;
                    case 32:
                        rLPCMInfo.eSampleFreq = FS_32K;
                        break;
                    case 44:
                        rLPCMInfo.eSampleFreq = FS_44K;
                        break;
                    case 8:
                        rLPCMInfo.eSampleFreq = FS_8K;
                        break;
                    case 11:
                        rLPCMInfo.eSampleFreq = FS_11K;
                        break;
                    case 12:
                        rLPCMInfo.eSampleFreq = FS_12K;
                        break;
                    case 16:
                        rLPCMInfo.eSampleFreq = FS_16K;
                        break;
                    case 22:
                        rLPCMInfo.eSampleFreq = FS_22K;
                        break;
                    case 24:
                        rLPCMInfo.eSampleFreq = FS_24K;
                        break;
                    case 5:
                        rLPCMInfo.eSampleFreq = FS_5K;
                        break;
                    case 6:
                        rLPCMInfo.eSampleFreq = FS_6K;
                        break;
                    case 88:
                        rLPCMInfo.eSampleFreq = FS_88K;
                        break;
                    case 176:
                        rLPCMInfo.eSampleFreq = FS_176K;
                        break;
                    case 192:
                        rLPCMInfo.eSampleFreq = FS_192K;
                        break;
                    default:
                        rLPCMInfo.eSampleFreq = FS_48K;
                        break;
                }
                // LPCM Type
                u4Tmp = (UINT32)(rParam.eLPCMType);
                switch (u4Tmp)
                {
                    case 0:
                        rLPCMInfo.eLPcmType = LPCM_TYPE_NORAML;
                        break;
                    case 1:
                        rLPCMInfo.eLPcmType = LPCM_TYPE_A_LAW;
                        break;
                    case 2:
                        rLPCMInfo.eLPcmType = LPCM_TYPE_U_LAW;
                        break;
                    default:
                        rLPCMInfo.eLPcmType = LPCM_TYPE_NORAML;
                        break;
                }
                rLPCMInfo.ui2_num_ch = (UINT16)rParam.u4Channels;
                rLPCMInfo.ui2_bits_per_sample = (UINT16)rParam.u4BitsPerSample;
                if (rLPCMInfo.ui2_bits_per_sample == 16)
                {
                    rLPCMInfo.ui2_bits_per_sample = 8;
                }
                rLPCMInfo.ui2_block_align = (UINT16)rParam.u4BlockAlign;
                rLPCMInfo.b_big_endian = (rParam.u1BigEndian == 0) ? TRUE : FALSE;
                rLPCMInfo.b_signed_data = (rParam.u1SignedData == 0) ? TRUE : FALSE;
                rLPCMInfo.b_info_not_specific = FALSE;
                Printf("[ADECOMX] LPCM Info: %d   %d   %d    %d    %d    %d\n", rLPCMInfo.eSampleFreq, rLPCMInfo.eLPcmType,
                    rLPCMInfo.ui2_num_ch, rLPCMInfo.ui2_bits_per_sample, rLPCMInfo.b_big_endian, rLPCMInfo.b_signed_data);

                AUD_LPcmSetting(AUD_DEC_MAIN, &rLPCMInfo);               
            }   
            rSetparam.eRet = ADECOMX_MRESULT_OK;   
            break;
        case ADECOMX_SET_TYPE_ADPCM_PARAM:
            if (rSetparam.pvInParam)
            {
                ADECOMX_SET_ADPCM_PARAM_T rParam;
				AUD_LPCM_INFO_T rLPCMInfo = {0};
                UINT32  u4Tmp = 0;
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, ADECOMX_SET_ADPCM_PARAM_T);
                COPY_FROM_USER_ARG(rSetparam.pvInParam, rParam, ADECOMX_SET_ADPCM_PARAM_T);                
                
                // Sample Frequency
                u4Tmp = (UINT32)(rParam.u4SampleRate / 1000);
                switch (u4Tmp)
                {
                    case 48:
                        rLPCMInfo.eSampleFreq = FS_48K;
                        break;
                    case 32:
                        rLPCMInfo.eSampleFreq = FS_32K;
                        break;
                    case 44:
                        rLPCMInfo.eSampleFreq = FS_44K;
                        break;
                    case 8:
                        rLPCMInfo.eSampleFreq = FS_8K;
                        break;
                    case 11:
                        rLPCMInfo.eSampleFreq = FS_11K;
                        break;
                    case 12:
                        rLPCMInfo.eSampleFreq = FS_12K;
                        break;
                    case 16:
                        rLPCMInfo.eSampleFreq = FS_16K;
                        break;
                    case 22:
                        rLPCMInfo.eSampleFreq = FS_22K;
                        break;
                    case 24:
                        rLPCMInfo.eSampleFreq = FS_24K;
                        break;
                    case 5:
                        rLPCMInfo.eSampleFreq = FS_5K;
                        break;
                    case 6:
                        rLPCMInfo.eSampleFreq = FS_6K;
                        break;
                    case 88:
                        rLPCMInfo.eSampleFreq = FS_88K;
                        break;
                    case 176:
                        rLPCMInfo.eSampleFreq = FS_176K;
                        break;
                    case 192:
                        rLPCMInfo.eSampleFreq = FS_192K;
                        break;
                    default:
                        rLPCMInfo.eSampleFreq = FS_48K;
                        break;
                }
                // ADPCM Type
                u4Tmp = (UINT32)(rParam.eADPCMType);
                switch (u4Tmp)
                {
                    case 0:
                        rLPCMInfo.eLPcmType = LPCM_TYPE_MS_ADPCM;
                        break;
                    case 1:
                        rLPCMInfo.eLPcmType = LPCM_TYPE_IMA_ADPCM;
                        break;
                    default:
                        rLPCMInfo.eLPcmType = LPCM_TYPE_NORAML;
                        break;
                }
                rLPCMInfo.ui2_num_ch = (UINT16)rParam.u4Channels;
                rLPCMInfo.ui2_bits_per_sample = (UINT16)rParam.u4BitsPerSample;
                if (rLPCMInfo.ui2_bits_per_sample == 16)
                {
                    rLPCMInfo.ui2_bits_per_sample = 8;
                }
                rLPCMInfo.ui2_block_align = (UINT16)rParam.u4BlockAlign;
                //rLPCMInfo.b_big_endian = (rParam.u1BigEndian == 0) ? TRUE : FALSE;
                //rLPCMInfo.b_signed_data = (rParam.u1SignedData == 0) ? TRUE : FALSE;
                //rLPCMInfo.b_info_not_specific = FALSE;
                Printf("[ADECOMX] ADPCM Info: %d   %d   %d    %d    %d\n", rLPCMInfo.eSampleFreq, rLPCMInfo.eLPcmType,
                    rLPCMInfo.ui2_num_ch, rLPCMInfo.ui2_bits_per_sample, rLPCMInfo.ui2_block_align);

                AUD_LPcmSetting(AUD_DEC_MAIN, &rLPCMInfo);               
            }   
            rSetparam.eRet = ADECOMX_MRESULT_OK;
            break;
        case ADECOMX_SET_TYPE_PCM_PARAM:
            if (rSetparam.pvInParam)
            {
                ADECOMX_SET_PCM_PARAM_T rParam;
				AUD_LPCM_INFO_T rLPCMInfo = {0};
                UINT32  u4Tmp = 0;
                USR_SPACE_ACCESS_VALIDATE_ARG(rSetparam.pvInParam, ADECOMX_SET_ADPCM_PARAM_T);
                COPY_FROM_USER_ARG(rSetparam.pvInParam, rParam, ADECOMX_SET_ADPCM_PARAM_T);                
                
                // Sample Frequency
                u4Tmp = (UINT32)(rParam.u4SampleRate / 1000);
                switch (u4Tmp)
                {
                    case 48:
                        rLPCMInfo.eSampleFreq = FS_48K;
                        break;
                    case 32:
                        rLPCMInfo.eSampleFreq = FS_32K;
                        break;
                    case 44:
                        rLPCMInfo.eSampleFreq = FS_44K;
                        break;
                    case 8:
                        rLPCMInfo.eSampleFreq = FS_8K;
                        break;
                    case 11:
                        rLPCMInfo.eSampleFreq = FS_11K;
                        break;
                    case 12:
                        rLPCMInfo.eSampleFreq = FS_12K;
                        break;
                    case 16:
                        rLPCMInfo.eSampleFreq = FS_16K;
                        break;
                    case 22:
                        rLPCMInfo.eSampleFreq = FS_22K;
                        break;
                    case 24:
                        rLPCMInfo.eSampleFreq = FS_24K;
                        break;
                    case 5:
                        rLPCMInfo.eSampleFreq = FS_5K;
                        break;
                    case 6:
                        rLPCMInfo.eSampleFreq = FS_6K;
                        break;
                    case 88:
                        rLPCMInfo.eSampleFreq = FS_88K;
                        break;
                    case 176:
                        rLPCMInfo.eSampleFreq = FS_176K;
                        break;
                    case 192:
                        rLPCMInfo.eSampleFreq = FS_192K;
                        break;
                    default:
                        rLPCMInfo.eSampleFreq = FS_48K;
                        break;
                }
                rLPCMInfo.eLPcmType = LPCM_TYPE_NORAML;
                rLPCMInfo.ui2_num_ch = (UINT16)rParam.u4Channels;
                rLPCMInfo.ui2_bits_per_sample = (UINT16)rParam.u4BitsPerSample;
                if (rLPCMInfo.ui2_bits_per_sample == 16)
                {
                    rLPCMInfo.ui2_bits_per_sample = 8;
                }
                rLPCMInfo.b_big_endian = (rParam.u1BigEndian == 0) ? TRUE : FALSE;
                rLPCMInfo.b_signed_data = (rParam.u1SignedData == 0) ? TRUE : FALSE;
                rLPCMInfo.b_info_not_specific = FALSE;
                Printf("[ADECOMX] PCM Info: %d   %d   %d    %d    %d\n", rLPCMInfo.eSampleFreq, rLPCMInfo.eLPcmType,
                    rLPCMInfo.ui2_num_ch, rLPCMInfo.ui2_bits_per_sample, rLPCMInfo.b_big_endian);

                AUD_LPcmSetting(AUD_DEC_MAIN, &rLPCMInfo);               
            }   
            rSetparam.eRet = ADECOMX_MRESULT_OK;
            break;
        default:
            rSetparam.eRet = ADECOMX_MRESULT_FAIL;
            break;
    }

    COPY_TO_USER_ARG(arg, rSetparam, PAR_ADECOMX_SETPARAM);

    return 0;
}

extern BOOL skip_create_feed_stream;

//-----------------------------------------------------------------------------
// adecomx_open
//-----------------------------------------------------------------------------
int adecomx_open(struct inode *inode, struct file *pfile)
{
    UINT32 i;

    Printf("[ADECOMX] %s (_u4AdecOmxOpenCnt=%d)\n", __FUNCTION__, _u4AdecOmxOpenCnt);
    
    if(!_fgInited)
    {
        Printf("[ADECOMX] %s(L:%d) init\n", __FUNCTION__, __LINE__);
        _fgInited = TRUE;
    }

    for (i = 0; i < ADECOMX_INST_MAX; i++)
    {
        if (!_arAdecOmx[i].fgOccupied)
        {
            Printf("[ADECOMX] %s(L:%d) dec%d is occupied\n", __FUNCTION__, __LINE__, i);
            _arAdecOmx[i].fgOccupied = TRUE;
            break;
        }
    }
    if(i >= ADECOMX_INST_MAX)
    {
        Printf("[ADECOMX][ERROR] %s(L:%d) inst full\n", __FUNCTION__, __LINE__);
        return -1;
    }

    pfile->private_data = &_arAdecOmx[i];
    if (0 == _u4AdecOmxOpenCnt)
    {
        skip_create_feed_stream = TRUE;
    }

    _u4AdecOmxOpenCnt ++;

    return 0;
}

//-----------------------------------------------------------------------------
// adecomx_release
//-----------------------------------------------------------------------------
int adecomx_release(struct inode *inode, struct file *pfile)
{
    ADECOMX_HANDLE_T *prAdecOmx;

    Printf("[ADECOMX] %s (_u4AdecOmxOpenCnt=%d)\n", __FUNCTION__, _u4AdecOmxOpenCnt);

    if(!_fgInited)
    {
        Printf("[ADECOMX][ERROR] %s(L:%d) init\n", __FUNCTION__, __LINE__);
        _fgInited = TRUE;
        return -1;
    }
    if(!pfile)
    {
        Printf("[ADECOMX][ERROR] %s(L:%d) pfile null\n", __FUNCTION__, __LINE__);
        return -1;
    }
    prAdecOmx = (ADECOMX_HANDLE_T*)pfile->private_data;
    if(!prAdecOmx)
    {
        Printf("[ADECOMX][ERROR] %s(L:%d) prAdecOmx null\n", __FUNCTION__, __LINE__);
        return -1;
    }

    _u4AdecOmxOpenCnt --;

    if (0 == _u4AdecOmxOpenCnt)
    {
        skip_create_feed_stream = FALSE;
        // do something?
    }

    if(prAdecOmx->fgPlay)
    {
        Printf("[ADECOMX] %s(L:%d) fgPlay set to false\n", __FUNCTION__, __LINE__);
        
        //Disable Upload Path      
        AUD_DspSetUploadMode(0);
        AUD_DspUploadDataEnable();
        vResetUploadFlag();  		

        AUD_DSPCmdStop(AUD_DEC_MAIN);
        AUD_DspMutePcmOut(FALSE);
        AUD_DspFastDecode(FALSE);
        prAdecOmx->fgPlay = FALSE;
    }

    prAdecOmx->fgOccupied = FALSE;
    //prAdecOmx->eFmt = AUD_FMT_UNKNOWN;
    pfile->private_data = NULL;

    return 0;
}

//-----------------------------------------------------------------------------
// adecomx_ioctl
//-----------------------------------------------------------------------------
int adecomx_ioctl(struct inode *inode,
                struct file *pfile, 
                unsigned int cmd,
                 unsigned long arg)
{
    INT32  i4Ret = 0;
    ADECOMX_HANDLE_T *prAdecOmx = (ADECOMX_HANDLE_T *)pfile->private_data;

    //Printf("[ADECOMX] %s\n", __FUNCTION__);
    
    if(!prAdecOmx)
    {
        Printf("[ADECOMX][ERROR] %s(L:%d) prAdecOmx(private_data) is not set\n", __FUNCTION__, __LINE__, cmd);    
        return -1;
    }

    switch (cmd)
    {
        case ADEXOMX_CREATE:
            i4Ret = adecomx_create(prAdecOmx, arg);
            break;
        case ADEXOMX_DECODE:
            i4Ret = adecomx_decode(prAdecOmx, arg);
            break;
        case ADEXOMX_FLUSH:
            i4Ret = adecomx_flush(prAdecOmx, arg);
            break;
        case ADEXOMX_GETPARAM:
            i4Ret = adecomx_getparam(prAdecOmx, arg);
            break;
        case ADEXOMX_SETPARAM:
            i4Ret = adecomx_setparam(prAdecOmx, arg);
            break;
		case ADEXOMX_EMPTYBUFFER:
			i4Ret = adecomx_DoEmptyBuffer(prAdecOmx, arg);
			break;
		case ADEXOMX_FILLBUFFER:
			i4Ret = adecomx_DoFillBuffer(prAdecOmx, arg);
			break;
        default:
            Printf("[ADECOMX][ERROR] %s(L:%d) cmd(%d) not support\n", __FUNCTION__, __LINE__, cmd);
            i4Ret = -1;
            break;
    }

    return i4Ret;    
}

//-----------------------------------------------------------------------------
// adecomx_unlocked_ioctl
//-----------------------------------------------------------------------------
#ifdef USE_UNLOCK_IOCTL
long adecomx_unlocked_ioctl(struct file *pfile, unsigned int cmd, unsigned long param)
{
    //Printf("[ADECOMX] %s\n", __FUNCTION__);

    return (long)adecomx_ioctl(0, pfile, cmd, param);
}
#endif

//-----------------------------------------------------------------------------
// adecomx_fops => device driver structure
//-----------------------------------------------------------------------------
struct file_operations adecomx_fops = {
    .owner              = THIS_MODULE,
    .open               = adecomx_open,
#ifdef USE_UNLOCK_IOCTL
    .unlocked_ioctl     = adecomx_unlocked_ioctl,
#else
    .ioctl              = adecomx_ioctl,
#endif
    .release            = adecomx_release,
};

void adecomx_query(void)
{
    int i;
    Printf("[ADECOMX Common]\n");
    Printf("  _fgInited: %d\n", _fgInited);
    Printf("  _u4AdecOmxOpenCnt: %d\n", _u4AdecOmxOpenCnt);
    Printf("  u4ADecOmxDbgMsk: 0x%08x\n", u4ADecOmxDbgMsk);
    Printf("  u4ADecOmxInitBufSize : 0x%08x\n", u4ADecOmxInitBufSize);
	
    for (i=0;i<ADECOMX_INST_MAX;i++)
    {
        Printf("[ADECOMX Dec%d]\n", i);
        Printf("  fgOccipied: %d\n", _arAdecOmx[i].fgOccupied);
        Printf("  eFmt: %d\n", _arAdecOmx[i].eFmt);
        Printf("  fgPlay: %d\n", _arAdecOmx[i].fgPlay);
        Printf("  fgEOS: %d\n", _arAdecOmx[i].fgEOS);
        Printf("  u4InputBufSA: 0x%08x\n", _arAdecOmx[i].u4InputBufSA);
        Printf("  u4InputBufSZ: 0x%08x\n", _arAdecOmx[i].u4InputBufSZ);
        Printf("  u4InputBufRP: 0x%08x\n", _arAdecOmx[i].u4InputBufRP);
        Printf("  u4InputBufWP: 0x%08x\n", _arAdecOmx[i].u4InputBufWP);
        Printf("  u4TotalInputSize: %d\n", _arAdecOmx[i].u4TotalInputSize);
        Printf("  u4OutputBufSA: 0x%08x\n", _arAdecOmx[i].u4OutputBufSA);
        Printf("  u4OutputBufSZ: 0x%08x\n", _arAdecOmx[i].u4OutputBufSZ);
        Printf("  u4OutputBufRP: 0x%08x\n", _arAdecOmx[i].u4OutputBufRP);
        Printf("  u4OutputBufWP: 0x%08x\n", _arAdecOmx[i].u4OutputBufWP);
        Printf("  u4TotalOutputSize: %d\n", _arAdecOmx[i].u4TotalOutputSize);
    }    
}

#endif

