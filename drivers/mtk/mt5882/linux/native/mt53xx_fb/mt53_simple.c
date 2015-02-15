#if defined(ANDROID) || defined(ANDROID_TWO_WORLDS)
//#define __TEST_BY_OSD__
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/export.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "fbm_drvif.h"
#include "srm_drvif.h"
#include "vdp_drvif.h"
#include "vdp_if.h"
#include "b2r_if.h"
#include "vdo_if.h"
#include "drv_tdtv_drvif.h"
#ifdef __TEST_BY_OSD__
#include "gfx_drvif.h"
#include "osd_drvif.h"
#endif
#include "mt53_simple.h"

#define INIT_LIST_HEAD(X) {.prev = &X, .next = &X}

#define MAX_CHANNEL_NS 2 

typedef BOOL (*ConsumeListHandle)(SIMPLE_BUFFER_T *prBuffer);
typedef struct CONSUME_BUFFER_T CONSUME_BUFFER_T;

struct CONSUME_BUFFER_T
{
    struct list_head link;
	SIMPLE_BUFFER_T rBuffer;
};

static LIST_HEAD(rConsumeList);
static spinlock_t rConsumeLock;
static atomic_t rConsumeCount;
static wait_queue_head_t rConsumeQueue;
static BOOL fgMT53SimpleInit = FALSE;

static UCHAR aucFbgId[MAX_CHANNEL_NS] = {FBM_FBG_ID_UNKNOWN, FBM_FBG_ID_UNKNOWN}; 
static UINT32 au4LastScrPitch[MAX_CHANNEL_NS] = {0, 0};
static UINT32 au4LastScrWidth[MAX_CHANNEL_NS] = {0, 0};
static UINT32 au4LastScrHeight[MAX_CHANNEL_NS] = {0, 0};


static inline CONSUME_BUFFER_T *ConsumeList_GetElement(SIMPLE_BUFFER_T *prSimBuf)
{
    CONSUME_BUFFER_T *pos;
	ASSERT((prSimBuf != NULL));

    list_for_each_entry(pos, &rConsumeList, link)
    {
        if(prSimBuf->u4Addr == pos->rBuffer.u4Addr && prSimBuf->u4Size == pos->rBuffer.u4Size)
        {
            return pos;
        }
    }

	return NULL;
}

static inline BOOL ComsumeList_AddElement(SIMPLE_BUFFER_T *prSimBuf)
{
    CONSUME_BUFFER_T *prConsumeBuf;	
    ASSERT((prSimBuf != NULL));

	prConsumeBuf = kmalloc(sizeof(CONSUME_BUFFER_T), GFP_KERNEL);
	prConsumeBuf->rBuffer = *prSimBuf;
	list_add_tail(&prConsumeBuf->link, &rConsumeList);
	atomic_add(1, &rConsumeCount);

	return TRUE;
}

static inline BOOL ComsumeList_RemoveElement(SIMPLE_BUFFER_T *prSimBuf)
{
    CONSUME_BUFFER_T *prConsumeBuf;	
    ASSERT((prSimBuf != NULL));

	if((prConsumeBuf = ConsumeList_GetElement(prSimBuf)) == NULL)
	{
	    return FALSE;
	}
	atomic_sub(1, &rConsumeCount);
	list_del(&prConsumeBuf->link);
	kfree(prConsumeBuf);
	
	return TRUE;
}

static inline BOOL ComsumeList_IsExistElement(SIMPLE_BUFFER_T *prSimBuf)
{
    ASSERT((prSimBuf != NULL));
	
    return ((ConsumeList_GetElement(prSimBuf) == NULL) ? FALSE : TRUE);
}

BOOL ComsumeList_HandleElement(SIMPLE_BUFFER_T *prSimBuf, ConsumeListHandle pfnHandle)
{
    BOOL ret;
	ASSERT((prSimBuf != NULL));
	ASSERT((pfnHandle != NULL));
	
    spin_lock(&rConsumeLock);
	ret = pfnHandle(prSimBuf);
	spin_unlock(&rConsumeLock);

	return ret;
}

VOID ComsumeList_WaitRemoveElement(SIMPLE_BUFFER_T *prSimBuf)
{
    wait_event_interruptible(rConsumeQueue, 
		(ComsumeList_HandleElement(prSimBuf, ComsumeList_IsExistElement) == FALSE));
}

VOID ComsumeList_WaitHasElement(VOID)
{
    wait_event_interruptible(rConsumeQueue,((atomic_read(&rConsumeCount) > 0)));
}

INT32 ConsumeList_Ioctl(UINT32 i4Cmd, UINT32 u4Arg)
{
    SIMPLE_BUFFER_T rSimBuf;
	
    if(i4Cmd == SIMPLE_ADD_SIMPLEBUFFER || i4Cmd == SIMPLE_REMOVE_SIMPLEBUFFER 
		|| i4Cmd == SIMPLE_WAIT_SIMPLEBUFFER_REMOVE)
	{
        if (copy_from_user(&rSimBuf, (void *)u4Arg, sizeof(SIMPLE_BUFFER_T)))
        {
            printk("%s(%d) - copy_from_user() fail\n", __FUNCTION__, __LINE__);
            return -EFAULT;
        }	
	}

    switch(i4Cmd)
    {
    case SIMPLE_ADD_SIMPLEBUFFER:
		{
			ComsumeList_HandleElement(&rSimBuf, ComsumeList_AddElement);
			wake_up_interruptible_all(&rConsumeQueue);
		}
	    break;
    case SIMPLE_REMOVE_SIMPLEBUFFER:
		{
			ComsumeList_HandleElement(&rSimBuf, ComsumeList_RemoveElement);
			wake_up_interruptible_all(&rConsumeQueue);
		}
	    break;
    case SIMPLE_WAIT_SIMPLEBUFFER_REMOVE:
		{
			ComsumeList_WaitRemoveElement(&rSimBuf);
		}
	    break;
	case SIMPLE_POLL_CONSUME:
		{
			ComsumeList_WaitHasElement();
		}
	    break;	
	default:
		{
			printk("%s(%d) - invalid cmd \n", __FUNCTION__, __LINE__);
		}
	    break;
    }	

	return 0;
}

static UCHAR VDPConvert_PathInit(UCHAR u4ChannelID, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pitch)
{
    UCHAR ucFbgType = FBM_FBG_ID_UNKNOWN, ucFbgId = FBM_FB_ID_UNKNOWN;
    FBM_SEQ_HDR_T* prFbmSeqHdr = NULL;
    //VDP_REGION_T rRegion = {0};
    FBM_CREATE_FBG_PAR_T rPar;
    
    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));

	if(u4ChannelID == 1)
	{
	    printk("call SRM_SetTvMode(SRM_TV_MODE_TYPE_PIP);");
	    SRM_SetTvMode(SRM_TV_MODE_TYPE_PIP);
	}
    ucFbgType = FBM_SelectGroupType(u4Width, u4Height);
    FBM_SetColorMode(FBM_CM_420);
    ucFbgId = FBM_CreateGroupExt(ucFbgType, FBM_VDEC_JPEG, u4Width, u4Height, &rPar);
    if(ucFbgId == FBM_FBG_ID_UNKNOWN)
    {
        printk("%s(%d)  Create Fbg Fail!! input resolution %dx%d\n", __FUNCTION__, __LINE__, u4Width, u4Height);
        return FBM_FBG_ID_UNKNOWN;
    }

    prFbmSeqHdr = FBM_GetFrameBufferSeqHdr(ucFbgId);
    if (!prFbmSeqHdr)
    {
        FBM_ReleaseGroup(ucFbgId);
        printk("%s(%d)  Create Fbg Fail!! input resolution %dx%d\n", __FUNCTION__, __LINE__, u4Width, u4Height);
        return FBM_FBG_ID_UNKNOWN;
    }
    
    prFbmSeqHdr->u2HSize = u4Width;
    prFbmSeqHdr->u2VSize = u4Height;
    prFbmSeqHdr->u2OrgHSize = prFbmSeqHdr->u2HSize;
    prFbmSeqHdr->u2OrgVSize = prFbmSeqHdr->u2VSize;
    prFbmSeqHdr->u2LineSize = u4Pitch ? (((u4Pitch + 15) >> 4) << 4) : (((u4Width + 15) >> 4) << 4);
    prFbmSeqHdr->fgProgressiveSeq = TRUE;
    prFbmSeqHdr->fgRasterOrder = TRUE;
    prFbmSeqHdr->fg422Mode = FALSE;
    prFbmSeqHdr->ucFrmRatCod = MPEG_FRAME_RATE_30;

    bApiVideoSetVideoSrc(u4ChannelID, u4ChannelID ? SV_VS_DT2 : SV_VS_DT1);
    VDP_SetEnable(u4ChannelID, 1);
    VDP_SetMode(u4ChannelID, VDP_MODE_BG);
    VDP_SetInput(u4ChannelID, 0, 0);
    FBM_SetPlayMode(ucFbgId, FBM_FBG_MM_MODE);
    FBM_FbgChgNotify(ucFbgId, 0);

    FBM_SetFrameBufferFlag(ucFbgId, FBM_FLAG_SEQ_CHG_SPEEDUP);

    return ucFbgId;
}


static INT32 VDPConvert_PathRelease(UCHAR u4ChannelID)
{
    VDP_SetEnable(u4ChannelID, 0);

    if (aucFbgId[u4ChannelID] != FBM_FBG_ID_UNKNOWN)
    {
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, 0);
        FBM_ReleaseGroup(aucFbgId[u4ChannelID]);
        aucFbgId[u4ChannelID] = FBM_FBG_ID_UNKNOWN;
		if(u4ChannelID == 1)
		{
		    printk("call SRM_SetTvMode(SRM_TV_MODE_TYPE_NORMAL);");
			SRM_SetTvMode(SRM_TV_MODE_TYPE_NORMAL);
		}
    }
    return 0;
}

#ifdef __TEST_BY_OSD__
static UINT32 VDPConvert_GetEmptyBuffer(UINT32 u4ChannelID)
{
#ifdef __TEST_BY_OSD__
    return GFX_GetTmpBuf();
#else
    return 0;
#endif
}
#endif

static VOID VDPConvert_ColorCvt(UINT32 u4SrcAddr, UINT32 u4Width, UINT32 u4Height, 
        UINT32 u4Pitch, UINT32 u4ColorMode, UINT32 u4DstYAddr, UINT32 u4DstCAddr)
{
#ifdef __TEST_BY_OSD__
	//from YV12 to aycbcr8888
	RZ_JPG_SCL_PARAM_SET_T rJpgParam;

	x_memset((void *)&rJpgParam, 0, sizeof(RZ_OSD_SCL_PARAM_SET_T));
	
	rJpgParam.u4IsRsIn	  = 1; //block/raster in
	rJpgParam.u4YSrcBase1	= (u4SrcAddr);
	rJpgParam.u4YSrcBase2	= (u4SrcAddr);

	rJpgParam.u4YSrcBufLen	= u4Pitch;
	rJpgParam.u4CSrcBufLen	= u4Pitch >> 1; 

	rJpgParam.u4YSrcHOffset = 0;
	rJpgParam.u4YSrcVOffset = 0;
	rJpgParam.u4YSrcW	  = u4Width;
	rJpgParam.u4YSrcH	  = u4Height;

	rJpgParam.u4CbSrcBase1	= (u4SrcAddr + u4Pitch*u4Height);
	rJpgParam.u4CbSrcBase2	= (u4SrcAddr + u4Pitch*u4Height);	 
	rJpgParam.u4CbSrcHOffset	  = 0;
	rJpgParam.u4CbSrcVOffset	  = 0;
	rJpgParam.u4CbSrcW	  = u4Width >> 1; 
	rJpgParam.u4CbSrcH	  = u4Height >> 1; 

	rJpgParam.u4CrSrcBase1	= (u4SrcAddr + u4Pitch*u4Height + u4Pitch*u4Height/4);
	rJpgParam.u4CrSrcBase2	= (u4SrcAddr + u4Pitch*u4Height + u4Pitch*u4Height/4);	
	rJpgParam.u4CrSrcHOffset = 0;
	rJpgParam.u4CrSrcVOffset = 0;
	rJpgParam.u4CrSrcW = u4Width >> 1;
	rJpgParam.u4CrSrcH = u4Height >> 1;

	rJpgParam.u4IsRsOut = 1;	//block/raster out
	rJpgParam.u4OutSwap = 0;	  
	rJpgParam.u4ColorComp = 7; //y,cb,cr exists

	rJpgParam.u4YTgBase   = (u4DstYAddr);
	rJpgParam.u4CTgBase   = (u4DstYAddr);

	rJpgParam.u4IsJpgSameaddr = 0;
	rJpgParam.u4YTgHOffset = 0;
	rJpgParam.u4YTgVOffset = 0;
	rJpgParam.u4YTgBufLen = u4Pitch << 2;
	rJpgParam.u4CTgBufLen = u4Pitch << 2;

	rJpgParam.u4IsVdo2Osd = 1;	   // output in OSD format ?
	rJpgParam.u4YTgCM = E_RZ_OSD_DIR_CM_AYCbCr8888;
	rJpgParam.u4YTgW = u4Width;
	rJpgParam.u4YTgH = u4Height;		
	rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_444;
	rJpgParam.u4CTgW = u4Width;
	rJpgParam.u4CTgH = u4Height;

	rJpgParam.u4IsFstBl = 1;
	rJpgParam.u4IsLstBl = 1;
	rJpgParam.u4JpgVfacY = 1; 
	rJpgParam.u4JpgVfacCb = 0;
	rJpgParam.u4JpgVfacCr = 0;	  
	rJpgParam.fgBlassign = TRUE; //indicates to directly assign input/output address

	IMGRZ_Lock();
	IMGRZ_ReInit();
	//src virtual address, dst fbm address
	rJpgParam.u4RzIOMMU_MMUTableAddr = HalCpuGetPhyPgd();
	IMGRZ_Cfg_MMU_IO((UINT32) E_FIRST_RESIZER, (BOOL) 1, (BOOL) 0);

	IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
	IMGRZ_Scale(&rJpgParam);
	IMGRZ_Flush();
	IMGRZ_Wait();
	IMGRZ_Unlock();
#else
    //from YV12 to yc420
    RZ_JPG_SCL_PARAM_SET_T rJpgParam;

    x_memset((void *)&rJpgParam, 0, sizeof(RZ_OSD_SCL_PARAM_SET_T));

    rJpgParam.u4IsRsIn	  = 1; //block/raster in
    rJpgParam.u4YSrcBase1	= (u4SrcAddr);
    rJpgParam.u4YSrcBase2	= (u4SrcAddr);

    rJpgParam.u4YSrcBufLen	= u4Pitch;
    rJpgParam.u4CSrcBufLen	= u4Pitch >> 1; 

    rJpgParam.u4YSrcHOffset = 0;
    rJpgParam.u4YSrcVOffset = 0;
    rJpgParam.u4YSrcW	  = u4Width;
    rJpgParam.u4YSrcH	  = u4Height;

    rJpgParam.u4CbSrcBase1	= (u4SrcAddr + u4Pitch*u4Height + u4Pitch*u4Height/4);
    rJpgParam.u4CbSrcBase2	= (u4SrcAddr + u4Pitch*u4Height + u4Pitch*u4Height/4);    
    rJpgParam.u4CbSrcHOffset	  = 0;
    rJpgParam.u4CbSrcVOffset	  = 0;
    rJpgParam.u4CbSrcW	  = u4Width >> 1; 
    rJpgParam.u4CbSrcH	  = u4Height >> 1; 

    rJpgParam.u4CrSrcBase1	= (u4SrcAddr + u4Pitch*u4Height);
    rJpgParam.u4CrSrcBase2	= (u4SrcAddr + u4Pitch*u4Height);	
    rJpgParam.u4CrSrcHOffset = 0;
    rJpgParam.u4CrSrcVOffset = 0;
    rJpgParam.u4CrSrcW = u4Width >> 1;
    rJpgParam.u4CrSrcH = u4Height >> 1;

    rJpgParam.u4IsRsOut = 1;	//block/raster out
    rJpgParam.u4OutSwap = 0;
    rJpgParam.u4ColorComp = 7; //y,cb,cr exists

    rJpgParam.u4YTgBase   = u4DstYAddr;
    rJpgParam.u4CTgBase   = u4DstCAddr;

    rJpgParam.u4IsJpgSameaddr = 0;
    rJpgParam.u4YTgHOffset = 0;
    rJpgParam.u4YTgVOffset = 0;
    rJpgParam.u4YTgBufLen = u4Pitch;
    rJpgParam.u4CTgBufLen = u4Pitch;

    rJpgParam.u4IsVdo2Osd = 0;	   // output in OSD format ?
    rJpgParam.u4YTgCM = 0;
    rJpgParam.u4YTgW = u4Width;
    rJpgParam.u4YTgH = u4Height;
    rJpgParam.u4OutMode = E_RZ_VDO_OUTMD_420;
    rJpgParam.u4CTgW = u4Width >> 1;
    rJpgParam.u4CTgH = u4Height >> 1;

    rJpgParam.u4IsFstBl = 1;
    rJpgParam.u4IsLstBl = 1;
    rJpgParam.u4JpgVfacY = 1; 
    rJpgParam.u4JpgVfacCb = 0;
    rJpgParam.u4JpgVfacCr = 0;
    rJpgParam.fgBlassign = TRUE; //indicates to directly assign input/output address

    IMGRZ_Lock();
    IMGRZ_ReInit();    
    rJpgParam.u4RzIOMMU_MMUTableAddr = HalCpuGetPhyPgd();
    IMGRZ_Cfg_MMU_IO((BOOL) 1, (BOOL) 0);
    IMGRZ_SetScaleOpt(E_RZ_INOUT_TYPE_JPGMD);
    IMGRZ_Scale(&rJpgParam);
    IMGRZ_Flush();
    IMGRZ_Wait();
    IMGRZ_Unlock();

#endif

    return ;
}

static BOOL VDPConvert_FBFlip(UINT32 u4ChannelID, UINT32 u4ScrAddr, UINT32 u4SrcWidth, UINT32 u4ScrHeight, UINT32 u4SrcPitch, UINT32 u4ColorMode,
	    UINT32 u4SrcClipX, UINT32 u4SrcClipY, UINT32 u4SrcClipWidth, UINT32 u4SrcClipHeight,
	    UINT32 u4DisplayX, UINT32 u4DisplayY, UINT32 u4DisplayWidth, UINT32 u4DisplayHeight)
{
#ifdef __TEST_BY_OSD__
    static BOOL fgInit = FALSE;
    static UINT32 u4RgnList, u4Region;
	UINT32 u4OSDPitch = ((u4SrcWidth * 4 + 15) >> 4) << 4;
	UINT32 u4DstAddr;

	u4DstAddr = VDPConvert_GetEmptyBuffer(u4ChannelID);
	if (u4SrcPitch == 0)
		u4SrcPitch = ((u4SrcWidth + 15) >> 4) << 4;	
	
    if(fgInit == FALSE)
    {
	    OSD_PLA_Enable(OSD_PLANE_2, FALSE);
	    OSD_RGN_Create(&u4Region, u4SrcWidth, u4ScrHeight, (void *)u4DstAddr, OSD_CM_AYCBCR8888_DIRECT32, u4OSDPitch, 0, 0, u4SrcWidth, u4SrcWidth);
	    OSD_RGN_Set(u4Region, OSD_RGN_MIX_SEL, OSD_BM_PIXEL);
	    OSD_RGN_LIST_Create(&u4RgnList);
	    OSD_RGN_Insert(u4RgnList, u4Region);
	    OSD_PLA_FlipTo(OSD_PLANE_2, u4RgnList);

	    fgInit = TRUE;
    }

	printk("[%s:%d][0x%x, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d]\n", __FUNCTION__, __LINE__, 
		u4ScrAddr, u4SrcWidth, u4ScrHeight, u4SrcPitch, u4ColorMode, u4SrcClipX, u4SrcClipY, u4SrcClipWidth, u4SrcClipHeight, u4DisplayX,
		u4DisplayY, u4DisplayWidth, u4DisplayHeight);
	printk("[%s:%d][0x%x, %d]\n", __FUNCTION__, __LINE__, u4DstAddr, u4OSDPitch);

    VDPConvert_ColorCvt(u4ScrAddr, u4SrcWidth, u4ScrHeight, u4SrcPitch, u4ColorMode, u4DstAddr, u4DstAddr);

	OSD_RGN_Set(u4Region, OSD_RGN_BMP_W, u4SrcWidth);
	OSD_RGN_Set(u4Region, OSD_RGN_BMP_H, u4ScrHeight);
	OSD_RGN_Set(u4Region, OSD_RGN_DISP_W, u4SrcWidth);
	OSD_RGN_Set(u4Region, OSD_RGN_DISP_H, u4ScrHeight);
	OSD_RGN_Set(u4Region, OSD_RGN_OUT_W, u4SrcWidth);
	OSD_RGN_Set(u4Region, OSD_RGN_OUT_H, u4ScrHeight);
    //OSD_RGN_Set(u4Region, OSD_RGN_COLORMODE, u4ColorMode);
	OSD_RGN_Set(u4Region, OSD_RGN_BMP_PITCH, u4OSDPitch);
    OSD_SC_Scale(OSD_SCALER_2, 1, u4SrcWidth, u4ScrHeight, u4SrcWidth, u4ScrHeight);
	OSD_PLA_Enable(OSD_PLANE_2, TRUE);
#else
    {
        UCHAR ucFbId, ucFbgId;
        UINT32 u4DstYAddr, u4DstCAddr;
        FBM_PIC_HDR_T* prFbmPicHdr = NULL;
        UINT32 u4PanelWidth, u4PanelHeight;
        VDP_REGION_T rRegion = {0};

        ASSERT((u4SrcPitch % 32 == 0));
        ASSERT((u4SrcPitch != 0));
        if (u4ChannelID >= MAX_CHANNEL_NS)
        {
            ASSERT(u4ChannelID < MAX_CHANNEL_NS);
            return FALSE;
        }
        if (aucFbgId[u4ChannelID] == FBM_FBG_ID_UNKNOWN)
        {
            aucFbgId[u4ChannelID] = VDPConvert_PathInit(u4ChannelID, u4SrcWidth, u4ScrHeight, u4SrcPitch);
        }
        else if ((au4LastScrPitch[u4ChannelID] != u4SrcPitch) ||
			     (au4LastScrWidth[u4ChannelID] != u4SrcWidth) || 
                 (au4LastScrHeight[u4ChannelID] != u4ScrHeight))
        {
            VDPConvert_PathRelease(u4ChannelID);
            aucFbgId[u4ChannelID] = VDPConvert_PathInit(u4ChannelID, u4SrcWidth, u4ScrHeight, u4SrcPitch);
        }
        
        ucFbgId = aucFbgId[u4ChannelID];
        
        if (ucFbgId == (UCHAR)FBM_FB_ID_UNKNOWN)
        {
            printk("%s(%d) ucFbgId Create Fail.\n", __FUNCTION__, __LINE__);
            VDPConvert_PathRelease(ucFbgId); 
            return FALSE;
        }
    
        ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)500);
        if (ucFbId == (UCHAR)FBM_FB_ID_UNKNOWN)
        {
            printk("%s(%d) Get empty frame buffer failed.\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
    
        FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &u4DstYAddr, &u4DstCAddr);
        prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
        if ((prFbmPicHdr == NULL) || (u4DstYAddr == 0) || (u4DstCAddr == 0))
        {
            printk("%s(%d) Alloc frame buffer failed.\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
    
        //call xinwei supported function to color space convert src buffer to b2r buffer u4DstYAddr, u4DstCAddr 
        VDPConvert_ColorCvt(u4ScrAddr, u4SrcWidth, u4ScrHeight, u4SrcPitch, u4ColorMode, u4DstYAddr, u4DstCAddr);

        au4LastScrPitch[u4ChannelID] = u4SrcPitch;
        au4LastScrWidth[u4ChannelID] = u4SrcWidth;
        au4LastScrHeight[u4ChannelID] = u4ScrHeight;		
       
        prFbmPicHdr->fgProgressiveFrm = TRUE;
        prFbmPicHdr->fgRepFirstFld = FALSE;
        prFbmPicHdr->fgTopFldFirst = FALSE;
        prFbmPicHdr->u4PTS = 1;
    
        //Display
        FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);
    
        // Unmute B2R
        VDP_SetMode(u4ChannelID, VDP_MODE_NORMAL);
	
        rRegion.u4X = u4SrcClipX * 10000 / u4SrcWidth;
        rRegion.u4Y = u4SrcClipY * 10000 / u4ScrHeight;
        rRegion.u4Width = u4SrcClipWidth * 10000 / u4SrcWidth;
        rRegion.u4Height = u4SrcClipHeight * 10000 / u4ScrHeight;
        VDP_SetSrcRegion(u4ChannelID, 0, rRegion);

        u4PanelWidth = (UINT32) 1920;//OSD_BASE_GetDisplayWidth();
        u4PanelHeight = (UINT32) 1080;//OSD_BASE_GetDisplayHeight();		
        
        rRegion.u4X = u4DisplayX * 10000 / u4PanelWidth;
        rRegion.u4Y = u4DisplayY * 10000 / u4PanelHeight;
        rRegion.u4Width = u4DisplayWidth * 10000 / u4PanelWidth;
        rRegion.u4Height = u4DisplayHeight * 10000 / u4PanelHeight;
        VDP_SetOutRegion(u4ChannelID, 0, rRegion);
    }
#endif
    return TRUE;
}



static INT32 VDPConvert_FlipToB2R(VDPC_INFO_T *prInfo)
{
    UINT32 u4BmpAddr, u4BmpWidth, u4BmpHeight, u4BmpPitch, u4ColorMode;
	UINT32 u4SrcClipX, u4SrcClipY, u4SrcClipWidth, u4SrcClipHeight;
	UINT32 u4DisplayX, u4DisplayY, u4DisplayWidth, u4DisplayHeight;
    UINT32 u4ChannelID;
    BOOL  fgRet = TRUE;
	
	ASSERT((prInfo != NULL));
    if (!prInfo || (prInfo->u4ChannelID >= MAX_CHANNEL_NS))
    {
	    printk("%s(%d) - bad argument \n", __FUNCTION__, __LINE__);
	    return -1;
    }

	u4BmpAddr = prInfo->rSrcBuffer.rBuffer.u4Addr;
	u4BmpWidth = prInfo->rSrcBuffer.u4Width;
	u4BmpHeight = prInfo->rSrcBuffer.u4Height;
	u4BmpPitch = prInfo->rSrcBuffer.u4Pitch;
	u4ColorMode = prInfo->rSrcBuffer.u4ColorMode;

	u4SrcClipX = prInfo->u4SrcClipX;
	u4SrcClipY = prInfo->u4SrcClipY;
	u4SrcClipWidth = prInfo->u4SrcClipWidth;
	u4SrcClipHeight = prInfo->u4SrcClipHeight;

	u4DisplayX = prInfo->u4DisplayX;
	u4DisplayY = prInfo->u4DisplayY;
	u4DisplayWidth = prInfo->u4DisplayWidth;
	u4DisplayHeight = prInfo->u4DisplayHeight;
	
    u4ChannelID = prInfo->u4ChannelID;
	
	if((u4SrcClipX + u4SrcClipWidth) > u4BmpWidth || (u4SrcClipY + u4SrcClipHeight) > u4BmpHeight)
	{
	    printk("%s(%d) - bad argument \n", __FUNCTION__, __LINE__);
	    return -1;
	}

	fgRet = VDPConvert_FBFlip(u4ChannelID, u4BmpAddr, u4BmpWidth, u4BmpHeight, u4BmpPitch, u4ColorMode, u4SrcClipX, u4SrcClipY, u4SrcClipWidth,
		u4SrcClipHeight, u4DisplayX, u4DisplayY, u4DisplayWidth, u4DisplayHeight);

	return fgRet ? 0 : -1;
}

INT32 VDPConvert_Ioctl(UINT32 i4Cmd, UINT32 u4Arg)
{
    VDPC_INFO_T rInfo;

	if(i4Cmd == SIMPLE_VDP_CONVERT || i4Cmd == SIMPLE_VDP_STOP_CVT)
	{
        if (copy_from_user(&rInfo, (void *)u4Arg, sizeof(VDPC_INFO_T)))
        {
            printk("%s(%d) - copy_from_user() fail\n", __FUNCTION__, __LINE__);
            return -EFAULT;
        }	

		if(i4Cmd == SIMPLE_VDP_CONVERT)
		{
		    return VDPConvert_FlipToB2R(&rInfo);
		}
		else if(i4Cmd == SIMPLE_VDP_STOP_CVT)
		{
		    return VDPConvert_PathRelease(rInfo.u4ChannelID);
		}
	}

	return 0;
}

/*
static int MT53Simple_Open(struct inode *inode, struct file *file)
{
    UN_USED(inode);
	UN_USED(file);
	printk("MT53Simple_Open  .....\n");
	return 0;
}

static int MT53Simple_Release(struct inode *inode, struct file *file)
{
    UN_USED(inode);
	UN_USED(file);
	printk("MT53Simple_Release  .....\n");
	return 0;
}


static long MT53Simple_Ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	UN_USED(file);

	printk("MT53Simple_Ioctl test .....\n");

	if(cmd >= SIMPLE_CONSUME_CMD_START && cmd <= SIMPLE_CONSUME_CMD_END)
	{
	    return ConsumeList_Ioctl(cmd, arg);
	}

	if(cmd >= SIMPLE_VDP_CMD_START && cmd <= SIMPLE_VDP_CMD_END)
	{
        return VDPConvert_Ioctl(cmd, arg); 
	}

	if(cmd == SIMPLE_TEST_IO)
	{
	    printk("MT53Simple_Ioctl test \n");
	}

	return 0;
}

static struct file_operations rMT53SimpleOps = {
    .owner = THIS_MODULE,
	.open = MT53Simple_Open,
	.release = MT53Simple_Release,
	.unlocked_ioctl = MT53Simple_Ioctl
};

static struct miscdevice rMiscSimpleDev = {
    .minor = MISC_DYNAMIC_MINOR,
	.name = "simple",
	.fops = &rMT53SimpleOps,
	.mode = S_IRUGO | S_IWUGO
};

VOID Mt53Simple_InitDevice(VOID)
{
    INT32 ret;

	printk("call Mt53Simple_InitDevice !!! \n");

    if(TRUE == fgMT53SimpleInit)
    {
        return;
    }

	fgMT53SimpleInit = TRUE;
	
	spin_lock_init((&rConsumeLock));
    atomic_set(&rConsumeCount, 0);
    init_waitqueue_head(&rConsumeQueue);


	ret = misc_register(&rMiscSimpleDev);
	if(ret < 0)
	{
	    printk("unable register simple device !!! \n");
	}
	
}
*/


INT32 mt53simple_init(void)
{
    if(TRUE == fgMT53SimpleInit)
    {
        return 0;
    }

	fgMT53SimpleInit = TRUE;
	
	spin_lock_init((&rConsumeLock));
    atomic_set(&rConsumeCount, 0);
    init_waitqueue_head(&rConsumeQueue);

	return 0;
}

INT32 mt53simple_ioctl(UINT32 u4Arg)
{
	SIMPLE_CMD_T rCmd;
	
	if (copy_from_user(&rCmd, (void *)u4Arg, sizeof(SIMPLE_CMD_T)))
	{
		printk("%s(%d) - copy_from_user() fail\n", __FUNCTION__, __LINE__);
		return -EFAULT;
	}

	if(rCmd.u4Cmd >= SIMPLE_CONSUME_CMD_START && rCmd.u4Cmd <= SIMPLE_CONSUME_CMD_END)
	{
	    return ConsumeList_Ioctl(rCmd.u4Cmd, rCmd.u4Arg);
	}

	if(rCmd.u4Cmd >= SIMPLE_VDP_CMD_START && rCmd.u4Cmd <= SIMPLE_VDP_CMD_END)
	{
        return VDPConvert_Ioctl(rCmd.u4Cmd, rCmd.u4Arg); 
	}

	if(rCmd.u4Cmd == SIMPLE_TEST_IO)
	{
	    printk("mt53simple_ioctl test \n");
	}

	return 0;
}

#endif

