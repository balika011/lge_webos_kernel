#include "mtphoto4k.h"
#include "x_os.h"
#include "x_timer.h"
#include "drv_scaler_drvif.h"
#include "b2r_if.h"
#include "mute_if.h"
#include <linux/mtal_ioctl.h>
#include <linux/vmalloc.h>

#include "imgrz_if.h"
#include "imgrz_drvif.h"
#include "gfx_if.h"
#include "gfx_drvif.h"
#include "b2r_drvif.h"

#include "mtgfx.h"
#include "io_mtphoto4k.h"

UINT32 u4ImgzTestOption0 = 0;
UINT32 u4ImgzTestOption1 = 0;
UINT32 u4ImgzTestOption2 = 0;
//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------
#define MTPHOTO4K_PRINT printk
#define MTPHOTO4K_DEBUG 0

#define USR_SPACE_ACCESS_VALIDATE_2ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_2ARG_T)))              \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error1\n");     \
        return MTR_ERR_INV;                                          \
    }                                                     

#define USR_SPACE_ACCESS_VALIDATE_3ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_3ARG_T)))              \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error2\n");     \
        return MTR_ERR_INV;                                          \
    }                                                    
    
#define USR_SPACE_ACCESS_VALIDATE_4ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_4ARG_T)))              \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error3\n");   \
        return MTR_ERR_INV;                                          \
    }   
#define COPY_FROM_USER_2ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_2ARG_T)))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error4\n");      \
        return MTR_ERR_INV;                                          \
    }     

#define COPY_FROM_USER_3ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_3ARG_T)))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error5\n");      \
        return -1;                                          \
    }

#define COPY_FROM_USER_4ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_4ARG_T)))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error6\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_ARG(arg,type)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(type)))              \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error7\n");     \
        return MTR_ERR_INV;                                          \
    }                                                  

#define COPY_TO_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       sizeof(type)))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error8\n");      \
        return MTR_ERR_INV;                                          \
    }       

#define COPY_FROM_USER_ARG(ArgUsr, ArgKernel,type)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,    \
                       sizeof(type)))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error9\n");      \
        return MTR_ERR_INV;                                          \
    } 
    
#define USR_SPACE_ACCESS_VALIDATE_ARG_K(arg,size)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   size))              \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error10\n");     \
        return MTR_ERR_INV;                                          \
    }                                                      

#define COPY_TO_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_to_user((void __user *)ArgUsr, &ArgKernel,   \
                       size))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error11\n");      \
        return MTR_ERR_INV;                                          \
    }     
#define COPY_FROM_USER_ARG_K(ArgUsr, ArgKernel,size)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,    \
                       size))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error12\n");      \
        return MTR_ERR_INV;                                          \
    } 
#define COPY_FROM_USER_5ARG(ArgUsr, ArgKernel)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(MTAL_IOCTL_5ARG_T)))          \
        {                                                       \
            MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error13\n");      \
            return -1;                                          \
        }
    
#define USR_SPACE_ACCESS_VALIDATE_5ARG(arg)                 \
        if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                       sizeof(MTAL_IOCTL_5ARG_T)))              \
        {                                                       \
            MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error14\n");   \
            return MTR_ERR_INV;                                          \
        }  

#define COPY_FROM_USER_6ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_6ARG_T)))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error13\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_6ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_6ARG_T)))              \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error14\n");   \
        return MTR_ERR_INV;                                          \
    }   

#define COPY_FROM_USER_7ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_7ARG_T)))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error15\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_7ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_7ARG_T)))              \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error16\n");   \
        return MTR_ERR_INV;                                          \
    }   

#define COPY_FROM_USER_8ARG(ArgUsr, ArgKernel)              \
    if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                       sizeof(MTAL_IOCTL_8ARG_T)))          \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error17\n");      \
        return -1;                                          \
    }

#define USR_SPACE_ACCESS_VALIDATE_8ARG(arg)                 \
    if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                   sizeof(MTAL_IOCTL_8ARG_T)))              \
    {                                                       \
        MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error18\n");   \
        return MTR_ERR_INV;                                          \
    }   

#define COPY_FROM_USER_9ARG(ArgUsr, ArgKernel)              \
        if (copy_from_user(&ArgKernel, (void __user *)ArgUsr,   \
                           sizeof(MTAL_IOCTL_9ARG_T)))          \
        {                                                       \
            MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error19\n");      \
            return -1;                                          \
        }
    
#define USR_SPACE_ACCESS_VALIDATE_9ARG(arg)                 \
        if (!access_ok(VERIFY_READ, (void __user *)arg,         \
                       sizeof(MTAL_IOCTL_9ARG_T)))              \
        {                                                       \
            MTPHOTO4K_PRINT("mtal_ioctl_mtphoto4k argument error20\n");   \
            return MTR_ERR_INV;                                          \
        }   




// B2R
#define B2R_WIDTH_4K2K              (3840)  
#define B2R_HEIGHT_4K2K             (2160)
#define B2R_WIDTH_FHD               (1920)  
#define B2R_HEIGHT_FHD              (1080)

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

typedef struct {
    MTPHOTO4K_RGB_TYPE_T eType;
    UINT32  u32PhotoAddress;
    UINT32  u32PhotoSize;
    UINT32  u32PhotoPitch;
    UINT32  u32X;
    UINT32  u32Y;
    UINT32  u32Width;
    UINT32  u32Height;
    UINT8   u8VideoPath;
}PhotoSourceInfo_T;

typedef struct {
    UINT32  u32DestAddrY;
    UINT32  u32DestAddrU;
    UINT32  u32DestAddrV;
    UINT32  u32X;
    UINT32  u32Y;
    UINT32  u32Width;
    UINT32  u32Height;
}DestAddrInfo_T;

typedef struct {
    UCHAR   fbg_id;
    UCHAR   fb_id;
    UINT32  width;
    UINT32  height;
    UINT32  y_addr;
    UINT32  c_addr;
    UINT32  y_virt_addr;
    UINT32  c_virt_addr;
}VdpBuffer_T;

#define VDP_FB_NUM      2
#define VDP_ID_OFFSET   0x80

VdpBuffer_T g_VdpBuffer[VDP_FB_NUM] = {{0xFF}, {0xFF}};
#define B2R_BUFFER_NUM  2
#define B2R_NEXT_ID     0
#define B2R_CURRENT_ID  1

#define GFX_HW_MAX_HEIGHT   2048

// B2R
static unsigned char g_fbg_id = FBM_FBG_ID_UNKNOWN;
static UCHAR g_fb_id[B2R_BUFFER_NUM] = {0xFF, 0xFF};
static UCHAR g_fbg_scroll_id = FBM_FBG_ID_UNKNOWN;

static  BOOL bPause = FALSE;

MTPHOTO4K_SCROLL_B2R_CALLBACK_TAG_T     t_scroll_tag    = {0};
MTPHOTO4K_FADE_B2R_CALLBACK_TAG_T       t_fade_tag      = {0};
//-----------------------------------------------------------------------------
// Static Variables
//-----------------------------------------------------------------------------
static HANDLE_T hMtPhoto4kSemaHandle = (HANDLE_T)NULL;

static DestAddrInfo_T stDestInfo = {0, 0, 0};
static PhotoSourceInfo_T stSourceInfo = {0,0,0,0,0,0};
static UINT8* pPhotoBuffer = NULL;

//static HAL_TIME_T TimeStart, TimeNow, TimeDelta;

//-----------------------------------------------------------------------------
// External Funciton 
//-----------------------------------------------------------------------------
EXTERN void HalFlushInvalidateDCache(void);
EXTERN UINT32 HalGetMMUTableAddress(void);
EXTERN INT32 _CB_PutEvent(CB_FCT_ID_T eFctId, INT32 i4TagSize, void *pvTag);
EXTERN void vDrvSetFade(UINT8 bPath, UINT8 bValue, UINT8 bBg_Color);

//-----------------------------------------------------------------------------
// Funciton Define
//-----------------------------------------------------------------------------
//#define IMG_DUMP
#ifdef IMG_DUMP
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/param.h>

static INT32 _IMG_DumpData(char *filePath, const UINT8 *buf, UINT32 u4Width, UINT32 u4Height, UINT32 u4Pitch)
{
	mm_segment_t oldfs;
	struct file *filep;
	size_t ret, count;
	char fileName[256];

	memset(fileName, 0, 256 * sizeof(char));

	count = u4Pitch * u4Height;
	

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	if(filePath == NULL)
	{
		sprintf(fileName, "/tmp/usb/sda/sda/IMG_K_%ld_%ld_%ld.bin", u4Width, u4Height, u4Pitch);
		
		filep = filp_open(fileName, (O_CREAT | O_WRONLY | O_TRUNC), 0);		
	}
	else
	{
		//use the path
		filep = filp_open(filePath, (O_CREAT | O_WRONLY | O_TRUNC), 0);		
	}
	
	printk("[DUMP]filename=%s, buf=0x%x,count=0x%x\n", fileName, (UINT32)buf, (UINT32)count);
	
	if (IS_ERR(filep))
	{
		set_fs(oldfs);
		printk("[DUMP] filp_open error %x!\n", filep);
		return -1;
	}
	ret = filep->f_op->write(filep, (VOID*)buf, count, &filep->f_pos);
	if (ret != count)
	{
		printk("[DUMP] f_op->write error ret=%x!\n", ret);
		return -1;
	}
	printk("[DUMP]buf=0x%x,size=0x%x, ret=0x%x\n", (UINT32)buf, (UINT32)count, (UINT32)ret);
//	x_thread_delay(300);	
	filp_close(filep, 0);
	set_fs(oldfs);

	return 0;
}
#endif

void _MTPHOTO4K_Notify(void *pv_data, UINT32 u4State)
{
    MTPHOTO4K_CALLBACK_INFO_T t_cb_info = {0};

    t_cb_info.u4Condition = u4State;
    t_cb_info.u4Param = (UINT32)pv_data;

    MTPHOTO4K_PRINT("CB_MTAL_PHOTO4K_NFY [u4State=%d]\n", u4State);        
    _CB_PutEvent(CB_MTAL_PHOTO4K_NFY, sizeof(MTPHOTO4K_CALLBACK_INFO_T), &t_cb_info);
}

static VOID _video_resize_phase(
        UINT32 YSrc, 
        UINT32 Csrc, 
        UINT32 SrcX,
        UINT32 SrcY, 
        UINT32 SrcPitch, 
        UINT32 SrcWidth, 
        UINT32 SrcHeight,
        UINT32 YDst, 
        UINT32 CDst, 
        UINT32 DstX, 
        UINT32 DstY, 
        UINT32 DstPitch,
        UINT32 DstWidth, 
        UINT32 DstHeight)
{
    RZ_VDO_SCL_PARAM_SET_T rSclParam;

    IMGRZ_Lock_Ex(E_FIRST_RESIZER);
    IMGRZ_ReInit_Ex(E_FIRST_RESIZER);

    x_memset((void *) &rSclParam, 0, sizeof(RZ_VDO_SCL_PARAM_SET_T));

    // set scaling parameters
    rSclParam.u4SrcSwap = 0;
    rSclParam.u4IsRsIn = 1;
    rSclParam.u4YSrcBase = YSrc;
    rSclParam.u4YSrcBufLen = SrcPitch;
    rSclParam.u4CSrcBufLen = SrcPitch;
    rSclParam.u4YSrcHOffset = SrcX;
    rSclParam.u4YSrcVOffset = SrcY;
    rSclParam.u4YSrcW = SrcWidth;
    rSclParam.u4YSrcH = SrcHeight;

    rSclParam.u4CSrcBase = Csrc;
    rSclParam.u4CSrcHOffset = SrcX >> 1;
    rSclParam.u4CSrcVOffset = SrcY;
    rSclParam.u4CSrcW = SrcWidth >> 1;
    rSclParam.u4CSrcH = SrcHeight;

    rSclParam.u4OutSwap = 0;
    rSclParam.u4IsRsOut = 1;
    rSclParam.u4OutMode = 1;
    rSclParam.u4IsVdo2Osd = 0;

    rSclParam.u4YTgBase = YDst;
    rSclParam.u4YTgCM = 0;
    rSclParam.u4YTgBufLen = DstPitch;
    rSclParam.u4YTgHOffset = DstX;
    rSclParam.u4YTgVOffset = DstY;
    rSclParam.u4YTgW = DstWidth;
    rSclParam.u4YTgH = DstHeight;

    rSclParam.u4CTgBase = CDst;
    rSclParam.u4CTgW = DstWidth >> 1;
    rSclParam.u4CTgH = DstHeight;
    rSclParam.u4VdoCbCrSwap = 0;
    rSclParam.u4ABlend = (UINT32) 0x80;

    IMGRZ_SetScaleOpt_Ex(E_FIRST_RESIZER, E_RZ_INOUT_TYPE_VDOMD);
    IMGRZ_Scale_Ex(E_FIRST_RESIZER, (void *) (&rSclParam));

    IMGRZ_Flush_Ex(E_FIRST_RESIZER);
    IMGRZ_Wait_Ex(E_FIRST_RESIZER);
    IMGRZ_Unlock_Ex(E_FIRST_RESIZER);
}

static VOID _set_gfx_mmu_cfg(BOOL bEnable, BOOL bSrcMmu, BOOL bDstMmu)
{
    GFX_MMU_T       t_mmu;

    t_mmu.u4_init           = bEnable;
    t_mmu.u4_enable         = 0x1;
    t_mmu.u4_op_md          = 0x0;
    t_mmu.u4_src_rw_mmu     = bSrcMmu;
    t_mmu.u4_dst_rw_mmu     = bDstMmu;
    t_mmu.u4_vgfx_ord       = 0x0;
    t_mmu.u4_vgfx_slva      = 0x0;
    t_mmu.u4_pgt            = 0x0;
    _GFX_MMU_Set_Cfg(&t_mmu);
}
static VOID _gfx_hw_blt(
    UINT8*  pu1Src, 
    UINT32  u4SrcX,
    UINT32  u4SrcY,
    UINT32  u4SrcPitch, 
    UINT8*  pu1Dst,
    UINT32  u4DstX,
    UINT32  u4DstY,
    UINT32  u4DstPitch,
    UINT32  u4Switch,
    UINT32  u4Width,
    UINT32  u4Height,
    UINT32 u4ColorMode)
{
    GFX_SetSrc(pu1Src, u4ColorMode, u4SrcPitch);
    GFX_SetDst(pu1Dst, u4ColorMode, u4DstPitch);         
    GFX_SetBltOpt(u4Switch, 0, 0);
    GFX_BitBlt(u4SrcX, u4SrcY, u4DstX, u4DstY, u4Width, u4Height);      
}

INT32 _mt53fb_rotate(MTPHOTO4K_VDP_ROTATE_T *prRotate)
{
    INT32 i4Ret = 0;
    INT32 i4Sx, i4Sy, i4Dx, i4Dy;
    UINT32 u4Switch = 0;
    UINT32 i, j, n, m;
    UINT8 *pu1SrcBase, *pu1DstBase;

    GFX_Lock();
    _set_gfx_mmu_cfg(TRUE, FALSE, TRUE);

    if (prRotate->u4RotateOp & VDP_ROTATE_90)
    { 
        GFX_SetSrc((UINT8 *) prRotate->u4Src, CM_RGB_CLUT8, prRotate->u4SrcPitch);
        GFX_SetDst((UINT8 *) prRotate->u4Dst, CM_RGB_CLUT8, prRotate->u4DstPitch);

        i4Sx = (INT32) prRotate->u4SrcX;
        i4Sy = (INT32) prRotate->u4SrcY;
        i4Dx = (INT32) prRotate->u4DstX + prRotate->u4Height - 1;
        i4Dy = (INT32) prRotate->u4DstY;

        // rotated by 90" clock-wise
        GFX_SetHoriToVertLineOpt(0);
        
        for (i = 0; i < prRotate->u4Height; i++)
        {
            n = prRotate->u4Width / GFX_HW_MAX_HEIGHT;
            m = prRotate->u4Width % GFX_HW_MAX_HEIGHT;

            for(j = 0; j < n; j++)
            {
                GFX_HoriToVertLine((UINT32)i4Sx + GFX_HW_MAX_HEIGHT * j, (UINT32)i4Sy, (UINT32)i4Dx,
                        (UINT32) i4Dy + GFX_HW_MAX_HEIGHT * j, GFX_HW_MAX_HEIGHT);                    
            }

            if(m > 0)
            {
                GFX_HoriToVertLine((UINT32)i4Sx + GFX_HW_MAX_HEIGHT * n, (UINT32)i4Sy, (UINT32)i4Dx,
                        (UINT32)i4Dy + GFX_HW_MAX_HEIGHT * n, m);       
            }
            
            i4Sy++;
            i4Dx--;
        } // ~for
    }
    else if (prRotate->u4RotateOp & VDP_ROTATE_270)
    { 
        GFX_SetSrc((UINT8 *) prRotate->u4Src, CM_RGB_CLUT8, prRotate->u4SrcPitch);
        GFX_SetDst((UINT8 *) prRotate->u4Dst, CM_RGB_CLUT8, prRotate->u4DstPitch);
        // Rotating Y
        i4Sx = (INT32) prRotate->u4SrcX;
        i4Sy = (INT32) prRotate->u4SrcY;
        i4Dx = (INT32) prRotate->u4DstX;
        i4Dy = (INT32) prRotate->u4DstY + prRotate->u4Width - 1;

        // rotated by 90" counter-clock-wise
        GFX_SetHoriToVertLineOpt(1);

        for (i = 0; i < prRotate->u4Height; i++)
        {
            n = prRotate->u4Width / GFX_HW_MAX_HEIGHT;
            m = prRotate->u4Width % GFX_HW_MAX_HEIGHT;
            i4Dy = prRotate->u4DstY;

            for(j = 1; j <= n; j++)
            {
                GFX_HoriToVertLine((UINT32)i4Sx + prRotate->u4Width - GFX_HW_MAX_HEIGHT * j, (UINT32)i4Sy, (UINT32)i4Dx,
                        (UINT32)i4Dy + GFX_HW_MAX_HEIGHT * j, GFX_HW_MAX_HEIGHT);                    
            }

            if(m > 0)
            {
                i4Dy = (INT32) prRotate->u4DstY + prRotate->u4Width - 1;
                GFX_HoriToVertLine((UINT32)i4Sx, (UINT32)i4Sy, (UINT32)i4Dx, (UINT32)i4Dy, m);       
            }
            
            i4Sy++;
            i4Dx++;
        }
    }
    else if(prRotate->u4RotateOp & VDP_ROTATE_180)
    {
        // To use mirror&flip to do rotate 180  
        UINT32  u4BltHeight = 0;
        UINT8*  pu1TmpBuf = NULL;

        if (prRotate->u4RotateOp & VDP_ROTATE_FLIP)
        {
            u4Switch = D_GFXFLAG_FLIP;
        }
        else
        {
            u4Switch = D_GFXFLAG_MIRROR | D_GFXFLAG_FLIP;
        }
        
        n = prRotate->u4Height / GFX_HW_MAX_HEIGHT;
        m = prRotate->u4Height % GFX_HW_MAX_HEIGHT;
        if(m > 0)
        {
            n++;
        }
        
        pu1SrcBase = (UINT8 *)( prRotate->u4Src + (prRotate->u4SrcY ) * prRotate->u4SrcPitch + prRotate->u4SrcX );          
        pu1DstBase =  (UINT8 *) (prRotate->u4Dst +  (prRotate->u4DstY + prRotate->u4Height) * prRotate->u4DstPitch + prRotate->u4DstX);

        for(i = 0; i < n; i++)
        {
            if ((m > 0) && (i == n - 1))
            {
                u4BltHeight = m;
                pu1TmpBuf = pu1DstBase - m * prRotate->u4DstPitch - i * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch;
            }
            else
            {
                u4BltHeight = GFX_HW_MAX_HEIGHT;
                pu1TmpBuf = pu1DstBase - ( i + 1 ) * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch;
            }
            _gfx_hw_blt(pu1SrcBase + i * GFX_HW_MAX_HEIGHT * prRotate->u4SrcPitch,
                        0,
                        0,
                        prRotate->u4SrcPitch,
                        pu1TmpBuf,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        u4Switch,
                        prRotate->u4Width,
                        u4BltHeight,
                        CM_RGB_CLUT8);        
        }
    }
    else if (prRotate->u4RotateOp & VDP_ROTATE_FLIP)
    {
        // To use mirror&flip to do rotate 180  
        UINT32  u4BltHeight = 0;

        u4Switch = D_GFXFLAG_MIRROR;
        
        n = prRotate->u4Height / GFX_HW_MAX_HEIGHT;
        m = prRotate->u4Height % GFX_HW_MAX_HEIGHT;
        if(m > 0)
        {
            n++;
        }
        
        pu1SrcBase = (UINT8 *)( prRotate->u4Src + (prRotate->u4SrcY ) * prRotate->u4SrcPitch + prRotate->u4SrcX );          
        pu1DstBase =  (UINT8 *)( prRotate->u4Dst + (prRotate->u4DstY ) * prRotate->u4DstPitch + prRotate->u4DstX );

        for(i = 0; i < n; i++)
        {
            if ((m > 0) && (i == n - 1))
            {
                u4BltHeight = m;
            }
            else
            {
                u4BltHeight = GFX_HW_MAX_HEIGHT;
            }
            _gfx_hw_blt(pu1SrcBase + i * GFX_HW_MAX_HEIGHT * prRotate->u4SrcPitch,
                        0,
                        0,
                        prRotate->u4SrcPitch,
                        pu1DstBase + i * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        u4Switch,
                        prRotate->u4Width,
                        u4BltHeight,
                        CM_RGB_CLUT8);        
        }
    }
    
    GFX_Flush();
    GFX_Wait();
    GFX_Unlock();

    if (prRotate->u4RotateOp & VDP_ROTATE_270)
    {
        UINT32  ui4DstPitch = ((prRotate->u4Width << 1) + 0xf) & (~0xf);

        //resize to width x height/2, and then rotate to height/2 x width
        //---CbCr rotate
        _video_resize_phase(
                prRotate->u4Src + prRotate->u4SrcY * prRotate->u4SrcPitch,
                prRotate->u4Src + prRotate->u4SrcCbCrOffset + prRotate->u4SrcY * prRotate->u4SrcPitch, 
                prRotate->u4SrcX,
                0, 
                prRotate->u4SrcPitch, 
                prRotate->u4Width,
                prRotate->u4Height,
                GFX_GetTmpBuf() + ui4DstPitch * (prRotate->u4Height >> 1),
                GFX_GetTmpBuf(), 
                0, 
                0, 
                ui4DstPitch,
                (prRotate->u4Width << 1), 
                prRotate->u4Height >> 1);

        GFX_Lock();
        GFX_SetSrc((UINT8 *)GFX_GetTmpBuf(), CM_ARGB4444_DIRECT16, ui4DstPitch);
        GFX_SetDst((UINT8 *)(prRotate->u4Dst + prRotate->u4DstCbCrOffset), CM_ARGB4444_DIRECT16, prRotate->u4DstPitch);

        // Rotating CbCr
        prRotate->u4Height >>= 1;
        i4Sx = (INT32) 0;
        i4Sy = (INT32) 0;
        i4Dx = (INT32) prRotate->u4DstX >> 1;
        i4Dy = (INT32) prRotate->u4DstY + prRotate->u4Width - 1;

        // rotated by 90" counter-clock-wise
        GFX_SetHoriToVertLineOpt(1);

        for (i = 0; i < prRotate->u4Height; i++)
        {
            n = prRotate->u4Width / GFX_HW_MAX_HEIGHT;
            m = prRotate->u4Width % GFX_HW_MAX_HEIGHT;
            i4Dy = prRotate->u4DstY;

            for(j = 1; j <= n; j++)
            {
                GFX_HoriToVertLine((UINT32) i4Sx + prRotate->u4Width - GFX_HW_MAX_HEIGHT * j, (UINT32) i4Sy, (UINT32) i4Dx,
                        (UINT32) i4Dy + GFX_HW_MAX_HEIGHT * j, GFX_HW_MAX_HEIGHT);                    
            }

            if(m > 0)
            {
                i4Dy = (INT32) prRotate->u4DstY + prRotate->u4Width - 1;
                GFX_HoriToVertLine((UINT32)i4Sx, (UINT32)i4Sy, (UINT32)i4Dx, (UINT32)i4Dy, m);       
            }   

            i4Sy++;
            i4Dx++;
        }

        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();

        prRotate->u4Height <<= 1;
    }
    else if (prRotate->u4RotateOp & VDP_ROTATE_90)
    {
        UINT32   u4DstPitch = ((prRotate->u4Width << 1) + 0xf) & (~0xf);

        //---CbCr rotate
        _video_resize_phase(prRotate->u4Src + prRotate->u4SrcY * prRotate->u4SrcPitch,
                prRotate->u4Src + prRotate->u4SrcCbCrOffset + prRotate->u4SrcY * prRotate->u4SrcPitch, 
                prRotate->u4SrcX,
                0, 
                prRotate->u4SrcPitch, 
                prRotate->u4Width,
                prRotate->u4Height,
                GFX_GetTmpBuf() + u4DstPitch*(prRotate->u4Height >> 1),
                GFX_GetTmpBuf(), 
                0, 
                0, 
                u4DstPitch,
                (prRotate->u4Width << 1), prRotate->u4Height >> 1);

        GFX_Lock();
        GFX_SetSrc((UINT8 *) GFX_GetTmpBuf(), CM_ARGB4444_DIRECT16, u4DstPitch);
        GFX_SetDst((UINT8 *)(prRotate->u4Dst + prRotate->u4DstCbCrOffset), CM_ARGB4444_DIRECT16, prRotate->u4DstPitch);

        // Rotating CbCr
        prRotate->u4Height >>= 1;
        i4Sx = (INT32) 0;
        i4Sy = (INT32) 0;
        i4Dx = ((INT32) prRotate->u4DstX >> 1) + prRotate->u4Height - 1;
        i4Dy = (INT32) prRotate->u4DstY;

        // rotated by 90" clock-wise
        GFX_SetHoriToVertLineOpt(0);

        for (i = 0; i < prRotate->u4Height; i++)
        {
            n = prRotate->u4Width / GFX_HW_MAX_HEIGHT;
            m = prRotate->u4Width % GFX_HW_MAX_HEIGHT;

            for(j = 0; j < n; j++)
            {
                GFX_HoriToVertLine((UINT32) i4Sx + GFX_HW_MAX_HEIGHT * j, (UINT32) i4Sy, (UINT32) i4Dx,
                        (UINT32) i4Dy + GFX_HW_MAX_HEIGHT * j, GFX_HW_MAX_HEIGHT);                    
            }

            if(m > 0)
            {
                GFX_HoriToVertLine((UINT32) i4Sx + GFX_HW_MAX_HEIGHT * n, (UINT32) i4Sy, (UINT32) i4Dx,
                        (UINT32) i4Dy + GFX_HW_MAX_HEIGHT * n, m);       
            }

            i4Sy++;
            i4Dx--;
        }

        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();

        prRotate->u4Height <<= 1;
    }
    else if(prRotate->u4RotateOp & VDP_ROTATE_180)
    {
        // To use mirror&flip to do rotate 180  
        UINT32  u4BltHeight = 0;
        UINT8*  pu1TmpBuf = NULL;
        
        if (prRotate->u4RotateOp & VDP_ROTATE_FLIP)
        {
            u4Switch = D_GFXFLAG_FLIP;
        }
        else
        {
            u4Switch = D_GFXFLAG_MIRROR | D_GFXFLAG_FLIP;
        }
        
        n = prRotate->u4Height / GFX_HW_MAX_HEIGHT;
        m = prRotate->u4Height % GFX_HW_MAX_HEIGHT;
        if (m > 0)
        {
            n++;
        }
        
        pu1SrcBase = (UINT8 *)( prRotate->u4Src + prRotate->u4SrcCbCrOffset + (prRotate->u4SrcY ) * prRotate->u4SrcPitch + (prRotate->u4SrcX));          
        pu1DstBase =  (UINT8 *) (prRotate->u4Dst +  prRotate->u4DstCbCrOffset + (prRotate->u4DstY + prRotate->u4Height) * prRotate->u4DstPitch + (prRotate->u4DstX));

        GFX_Lock();
        
        for(i = 0; i < n; i++)
        {
            if ((m > 0) && (i == n - 1))
            {
                u4BltHeight = m;
                pu1TmpBuf = pu1DstBase - m * prRotate->u4DstPitch - i * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch;
            }
            else
            {
                u4BltHeight = GFX_HW_MAX_HEIGHT;
                pu1TmpBuf = pu1DstBase - ( i + 1 ) * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch;
            }
            _gfx_hw_blt(pu1SrcBase + i * GFX_HW_MAX_HEIGHT * prRotate->u4SrcPitch,
                        0,
                        0,
                        prRotate->u4SrcPitch,
                        pu1TmpBuf,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        u4Switch,
                        prRotate->u4Width >> 1,
                        u4BltHeight,
                        CM_ARGB4444_DIRECT16);             
        }

        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();
        
    }
    else if(prRotate->u4RotateOp & VDP_ROTATE_FLIP)
    {
        // To use mirror&flip to do rotate 180  
        UINT32  u4BltHeight = 0;
        
        u4Switch = D_GFXFLAG_MIRROR;
        
        n = prRotate->u4Height / GFX_HW_MAX_HEIGHT;
        m = prRotate->u4Height % GFX_HW_MAX_HEIGHT;
        if (m > 0)
        {
            n++;
        }
        
        pu1SrcBase = (UINT8 *)( prRotate->u4Src + prRotate->u4SrcCbCrOffset + (prRotate->u4SrcY ) * prRotate->u4SrcPitch + (prRotate->u4SrcX));          
        pu1DstBase =  (UINT8 *)( prRotate->u4Dst + prRotate->u4DstCbCrOffset + (prRotate->u4DstY ) * prRotate->u4DstPitch + (prRotate->u4DstX));

        GFX_Lock();
        
        for(i = 0; i < n; i++)
        {
            if ((m > 0) && (i == n - 1))
            {
                u4BltHeight = m;
            }
            else
            {
                u4BltHeight = GFX_HW_MAX_HEIGHT;
            }
            _gfx_hw_blt(pu1SrcBase + i * GFX_HW_MAX_HEIGHT * prRotate->u4SrcPitch,
                        0,
                        0,
                        prRotate->u4SrcPitch,
                        pu1DstBase + i * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        u4Switch,
                        prRotate->u4Width >> 1,
                        u4BltHeight,
                        CM_ARGB4444_DIRECT16);             
        }

        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();
        
    }

    if ((prRotate->u4RotateOp & (VDP_ROTATE_90 | VDP_ROTATE_270)) && 
        (prRotate->u4RotateOp & VDP_ROTATE_FLIP))
    {
        UINT32  u4BltHeight = 0;
        UINT8*  pu1TmpBuf = NULL;
        
        u4Switch = D_GFXFLAG_FLIP;

        n = prRotate->u4Width / GFX_HW_MAX_HEIGHT;
        m = prRotate->u4Width % GFX_HW_MAX_HEIGHT;
        if (m > 0)
        {
            n++;
        }

        GFX_Lock();
        pu1SrcBase = (UINT8 *)( prRotate->u4Dst + prRotate->u4DstY * prRotate->u4DstPitch + (prRotate->u4DstX)); 
        pu1DstBase =  (UINT8 *) (GFX_GetTmpBuf() + (prRotate->u4DstY + prRotate->u4Width) * prRotate->u4DstPitch + (prRotate->u4DstX));
        
        for(i = 0; i < n; i++)
        {
            if ((m > 0) && (i == n - 1))
            {
                u4BltHeight = m;
                pu1TmpBuf = pu1DstBase - m * prRotate->u4DstPitch - i * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch;
            }
            else
            {
                u4BltHeight = GFX_HW_MAX_HEIGHT;
                pu1TmpBuf = pu1DstBase - ( i + 1 ) * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch;
            }
            
            _set_gfx_mmu_cfg(TRUE, TRUE, FALSE);
            _gfx_hw_blt((UINT8*)pu1SrcBase + i * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        pu1TmpBuf,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        u4Switch,
                        prRotate->u4Height,
                        u4BltHeight,
                        CM_RGB_CLUT8);
            _gfx_hw_blt((UINT8*)(pu1SrcBase + prRotate->u4DstCbCrOffset + i * GFX_HW_MAX_HEIGHT * prRotate->u4DstPitch),
                        0,
                        0,
                        prRotate->u4DstPitch,
                        pu1TmpBuf + prRotate->u4DstCbCrOffset,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        u4Switch,
                        prRotate->u4Height>>1,
                        u4BltHeight,
                        CM_ARGB4444_DIRECT16);
        }
        
        pu1SrcBase = (UINT8 *)(GFX_GetTmpBuf() + prRotate->u4DstY * prRotate->u4DstPitch + (prRotate->u4DstX >> 1));
        pu1DstBase =  (UINT8 *)( prRotate->u4Dst + prRotate->u4DstY * prRotate->u4DstPitch + (prRotate->u4DstX >> 1)); 
        
        for(i = 0; i < n; i++)
        {
            if ((m > 0) && (i == n - 1))
            {
                u4BltHeight = m;
            }
            else
            {
                u4BltHeight = GFX_HW_MAX_HEIGHT;
            }
            
            _set_gfx_mmu_cfg(TRUE, FALSE, TRUE);
            _gfx_hw_blt((UINT8*)pu1SrcBase + i * prRotate->u4DstPitch * GFX_HW_MAX_HEIGHT,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        pu1DstBase + i * prRotate->u4DstPitch * GFX_HW_MAX_HEIGHT,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        0,
                        prRotate->u4Height,
                        u4BltHeight,
                        CM_RGB_CLUT8);

            _gfx_hw_blt(pu1SrcBase + i * prRotate->u4DstPitch * GFX_HW_MAX_HEIGHT + prRotate->u4DstCbCrOffset,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        pu1DstBase + i * prRotate->u4DstPitch * GFX_HW_MAX_HEIGHT + prRotate->u4DstCbCrOffset,
                        0,
                        0,
                        prRotate->u4DstPitch,
                        0,
                        prRotate->u4Height>>1,
                        u4BltHeight,
                        CM_ARGB4444_DIRECT16);
        }

        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();
    }
    
    return i4Ret;
}


INT32 _mt53_fb_vdp_stretchblit(MTPHOTO4K_STRETCH_BLIT_T * pr_vdp_stretch_blit)
{
    INT32 i4Ret = 0;
    UINT8   ui1Step = 1, i;
    UINT32  src_h, dest_h, src_offset_h, dest_offset_h;
    
    MTGFX_MMU_SET_T       t_mmu = {0};
  
#if 1 //def CC_GFX_MMU
    t_mmu.u4_init          =GFX_INIT_MMU_VAL;
    t_mmu.u4_enable        =0x1;
#else
    t_mmu.u4_init          =GFX_INIT_MMU_VAL;
    t_mmu.u4_enable        =0x0;
#endif
    t_mmu.u4_cmprss        =0x0;
    t_mmu.u4_src_rw_mmu    =0x1;
    t_mmu.u4_dst_rw_mmu    =0x0;//dst buffer is physical
    t_mmu.u4_vgfx_ord      =0x0;
    t_mmu.u4_pgt           =0x0;

    ui1Step = (pr_vdp_stretch_blit->src_clip_h> pr_vdp_stretch_blit->dest_clip_h) ? 
            ((pr_vdp_stretch_blit->src_clip_h + GFX_HW_MAX_HEIGHT -1) / GFX_HW_MAX_HEIGHT) : 
            ((pr_vdp_stretch_blit->dest_clip_h + GFX_HW_MAX_HEIGHT -1 )/ GFX_HW_MAX_HEIGHT);
    
    src_h = pr_vdp_stretch_blit->src_clip_h / ui1Step;
    dest_h = pr_vdp_stretch_blit->dest_clip_h / ui1Step;

    src_offset_h = src_h;
    dest_offset_h = dest_h;

    for (i = 0; i < ui1Step; i++)
    {
        if (i == ui1Step - 1)
        {
            src_h = pr_vdp_stretch_blit->src_clip_h - src_h * i;
            dest_h = pr_vdp_stretch_blit->dest_clip_h - dest_h * i;
        }
        //Y
        GFX_Lock();
        if(GFX_INIT_MMU_VAL == t_mmu.u4_init)
        {
            GFX_MMU_Set_Cfg(&t_mmu);
        }
        GFX_SetSrc((UINT8 *)pr_vdp_stretch_blit->src_buffer_Y_addr, CM_RGB_CLUT8, pr_vdp_stretch_blit->src_buffer_Y_pitch);
        GFX_SetDst((UINT8 *)pr_vdp_stretch_blit->dest_buffer_Y_addr, CM_RGB_CLUT8, pr_vdp_stretch_blit->dest_buffer_Y_pitch);
        GFX_SetBltOpt(0, 0, 0);
        GFX_StretchBlt(pr_vdp_stretch_blit->src_clip_x,
                       pr_vdp_stretch_blit->src_clip_y + i * src_offset_h,
                       pr_vdp_stretch_blit->src_clip_w,
                       src_h,
                       pr_vdp_stretch_blit->dest_clip_x,
                       pr_vdp_stretch_blit->dest_clip_y + i * dest_offset_h,
                       pr_vdp_stretch_blit->dest_clip_w,
                       dest_h);
        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();

        //CbCr
        GFX_Lock();
        if(GFX_INIT_MMU_VAL == t_mmu.u4_init)
        {
            GFX_MMU_Set_Cfg(&t_mmu);
        }
        GFX_SetSrc((UINT8 *)pr_vdp_stretch_blit->src_buffer_C_addr, CM_ARGB4444_DIRECT16, pr_vdp_stretch_blit->src_buffer_Y_pitch);
        GFX_SetDst((UINT8 *)pr_vdp_stretch_blit->dest_buffer_C_addr, CM_ARGB4444_DIRECT16, pr_vdp_stretch_blit->dest_buffer_Y_pitch);
        GFX_SetBltOpt(0, 0, 0);
		
		if(stSourceInfo.eType == YUV_422_SEP)
		{
			GFX_StretchBlt(pr_vdp_stretch_blit->src_clip_x>>1,
						   pr_vdp_stretch_blit->src_clip_y + i * src_offset_h,
						   pr_vdp_stretch_blit->src_clip_w>>1,
						   src_h,
						   pr_vdp_stretch_blit->dest_clip_x>>1,
						   pr_vdp_stretch_blit->dest_clip_y + i * dest_offset_h,
						   pr_vdp_stretch_blit->dest_clip_w>>1,
						   dest_h);
		}
		else if(stSourceInfo.eType == YUV_420_SEP)
		{
			GFX_StretchBlt(pr_vdp_stretch_blit->src_clip_x>>1,
						   pr_vdp_stretch_blit->src_clip_y + i * src_offset_h,
						   pr_vdp_stretch_blit->src_clip_w>>1,
						   src_h>>1,
						   pr_vdp_stretch_blit->dest_clip_x>>1,
						   pr_vdp_stretch_blit->dest_clip_y + i * dest_offset_h,
						   pr_vdp_stretch_blit->dest_clip_w>>1,
						   dest_h>>1);

		}
        GFX_Flush();
        GFX_Wait();
        GFX_Unlock();
    }
    return i4Ret;

}

static BOOL _Panel_Is_4K2K(VOID)
{
#if (defined(CC_SUPPORT_4K2K) || defined(CC_SUPPORT_4K2K_PHOTO))
    
    UINT32  u4Width, u4Height;

    u4Width = PANEL_GetPanelPhysicalWidth();
    u4Height = PANEL_GetPanelPhysicalHeight();

    
    if (u4Width == B2R_WIDTH_4K2K && u4Height == B2R_HEIGHT_4K2K)
    {
        MTPHOTO4K_PRINT("{%s} Is4K2KPanel=1.\n", __FUNCTION__);
        return TRUE;
    }
    else
    {
        MTPHOTO4K_PRINT("{%s} Is4K2KPanel=0.\n", __FUNCTION__);
        return TRUE;
    }
#else

    return FALSE;

#endif
}

static BOOL _need_rotate_buffer(MTPHOTO4K_ROTATION_T eRotate)
{
    if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90 ||
        eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270 ||
        eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90_WITH_FLIP ||
        eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270_WITH_FLIP)
    {
        return TRUE;
    }

    return FALSE;
}
static UINT32 _get_rotate_flag(MTPHOTO4K_ROTATION_T eRotate)
{
    UINT32  u4ROtateOp = 0;
    
    if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_0_WITH_FLIP ||
        eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90_WITH_FLIP ||
        eRotate == MTPHOTO4K_ROTATION_CLOCKWS_180_WITH_FLIP ||
        eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270_WITH_FLIP)
    {
        u4ROtateOp |= VDP_ROTATE_FLIP;
    }

    if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90 ||
        eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90_WITH_FLIP)
    {
        u4ROtateOp |= VDP_ROTATE_90;
    }
    if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_180 ||
        eRotate == MTPHOTO4K_ROTATION_CLOCKWS_180_WITH_FLIP)
    {
        u4ROtateOp |= VDP_ROTATE_180;
    }
    if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270 ||
        eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270_WITH_FLIP)
    {
        u4ROtateOp |= VDP_ROTATE_270;
    }

    return u4ROtateOp;
        
}


static void vRgbtoYuv422_SW(PhotoSourceInfo_T* pSourceInfo, DestAddrInfo_T* pDestAddrInfo)
{
    UINT32 i, u4Size;
    INT32 R0, G0, B0, R1, G1, B1;
    UINT8* pRgbSrc;    
    UINT8* pDestY = (UINT8*)(pDestAddrInfo->u32DestAddrY);
    UINT8* pDestC = (UINT8*)(pDestAddrInfo->u32DestAddrU);
    
    MTPHOTO4K_PRINT("===== vRgbtoYuv422_SW ======\n");
    
    if ((pSourceInfo == NULL) || (pDestY == NULL) || (pDestC == NULL))
    {
        MTPHOTO4K_PRINT("RgbtoYuv422 NULL pointer !!\n");
        return;
    }

    // width must align 2
    u4Size = (pSourceInfo->u32Width/2)*pSourceInfo->u32Height;
    pRgbSrc = (UINT8*)(pSourceInfo->u32PhotoAddress);
     
    switch (pSourceInfo->eType)
    {
        case RGB_TYPE_RGBA8888:    
            for (i=0; i<u4Size; i++)
            {   
                pRgbSrc++;    
                B0 = *pRgbSrc++;
                G0 = *pRgbSrc++;
                R0 = *pRgbSrc++;
                pRgbSrc++;
                B1 = *pRgbSrc++;
                G1 = *pRgbSrc++;
                R1 = *pRgbSrc++;
                
                *pDestY++ = 16  + (( 186*R0 + 627*G0 +  63*B0)/1024); // Y0                  
                *pDestY++ = 16  + (( 186*R1 + 627*G1 +  63*B1)/1024); // Y1                
                *pDestC++ = 128 + (( 448*R0 - 407*G0 -  41*B0)/1024); // V0         
                *pDestC++ = 128 + ((-103*R0 - 345*G0 + 448*B0)/1024); // U0        
            }
            break;
        case RGB_TYPE_ARGB8888:    
            for (i=0; i<u4Size; i++)
            {   
                B0 = *pRgbSrc++;
                G0 = *pRgbSrc++;
                R0 = *pRgbSrc++;
                pRgbSrc++;
                B1 = *pRgbSrc++;
                G1 = *pRgbSrc++;
                R1 = *pRgbSrc++;                
                pRgbSrc++;    
                
                *pDestY++ = 16  + (( 186*R0 + 627*G0 +  63*B0)/1024); // Y0                  
                *pDestY++ = 16  + (( 186*R1 + 627*G1 +  63*B1)/1024); // Y1                
                *pDestC++ = 128 + (( 448*R0 - 407*G0 -  41*B0)/1024); // V0         
                *pDestC++ = 128 + ((-103*R0 - 345*G0 + 448*B0)/1024); // U0        
            }
            break;
        case RGB_TYPE_ABGR8888:    
            for (i=0; i<u4Size; i++)
            {   
                R0 = *pRgbSrc++;
                G0 = *pRgbSrc++;
                B0 = *pRgbSrc++;
                pRgbSrc++;
                R1 = *pRgbSrc++;
                G1 = *pRgbSrc++;
                B1 = *pRgbSrc++;                
                pRgbSrc++;    
                
                *pDestY++ = 16  + (( 186*R0 + 627*G0 +  63*B0)/1024); // Y0                  
                *pDestY++ = 16  + (( 186*R1 + 627*G1 +  63*B1)/1024); // Y1                
                *pDestC++ = 128 + (( 448*R0 - 407*G0 -  41*B0)/1024); // V0         
                *pDestC++ = 128 + ((-103*R0 - 345*G0 + 448*B0)/1024); // U0        
            }
            break;
        case RGB_TYPE_RGB888:    
            for (i=0; i<u4Size; i++)
            {   
                B0 = *pRgbSrc++;
                G0 = *pRgbSrc++;
                R0 = *pRgbSrc++;
                B1 = *pRgbSrc++;
                G1 = *pRgbSrc++;
                R1 = *pRgbSrc++;
                
                *pDestY++ = 16  + (( 186*R0 + 627*G0 +  63*B0)/1024); // Y0                  
                *pDestY++ = 16  + (( 186*R1 + 627*G1 +  63*B1)/1024); // Y1                
                *pDestC++ = 128 + (( 448*R0 - 407*G0 -  41*B0)/1024); // V0         
                *pDestC++ = 128 + ((-103*R0 - 345*G0 + 448*B0)/1024); // U0        
            }
            break;        
        case RGB_TYPE_ARGB4444:
        case RGB_TYPE_RGB565:
        default:
            MTPHOTO4K_PRINT("not support now !!\n");
    }
}


static INT32 _mt53_fb_b2r_flip(UINT8 u1BufNum, UINT32 u4Show, UINT8 u1VideoPath, MTPHOTO4K_ROTATION_T eRotate)
{
    UINT32 y_addr, c_addr;
    unsigned char ucFbgId, ucFbId;
    FBM_PIC_HDR_T* prFbmPicHdr; 
    UINT32  u4FbDispWidth, u4FbDispHeight;
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING 
    UINT32  u4FbMapAddr = 0;
#endif
    BOOL                        bRotateBuf = FALSE;
    UINT32                      u4TmpBufPitch = 0, u4TmpBufHeight = 0;
    UINT8*                      pTmpBuffer = NULL;
    MTPHOTO4K_STRETCH_BLIT_T    tBlt = {0};

    if (_Panel_Is_4K2K())
    {
        u4FbDispWidth = B2R_WIDTH_4K2K;
        u4FbDispHeight = B2R_HEIGHT_4K2K;
    }
    else
    {
        u4FbDispWidth = B2R_WIDTH_FHD;
        u4FbDispHeight = B2R_HEIGHT_FHD;
    }

    ucFbgId = g_fbg_id;
    ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);

    if (ucFbId == FBM_FB_ID_UNKNOWN)
    {
        printk("{_mt53_fb_b2r_flip} FBM_GetEmptyFrameBuffer return FBM_FB_ID_UNKNOWN.\n");
        return MTR_FBM_ERROR;
    }
    
    if (u4Show) 
    {               
        FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &y_addr, &c_addr);
        if (y_addr == 0 || c_addr == 0)
        {
            printk("{_mt53_fb_b2r_flip1} FBM_GetFrameBufferAddr return null address.\n");
            return MTR_FBM_ERROR;
        }

        MTPHOTO4K_PRINT("test driver ko.\n");

#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING 
        u4FbMapAddr = (UINT32)BSP_MapReservedMem((void*)y_addr, u4FbDispWidth * u4FbDispHeight);
        if(!u4FbMapAddr)
        {
            MTPHOTO4K_PRINT("4k2k photo fb Y dynamic map fail\n");
            ASSERT(0);
        }

        u4FbMapAddr = (UINT32)BSP_MapReservedMem((void*)c_addr, u4FbDispWidth * u4FbDispHeight);
        if(!u4FbMapAddr)
        {
            MTPHOTO4K_PRINT("4k2k photo fb C dynamic map fail\n");
            ASSERT(0);
        }
#endif        
        stDestInfo.u32DestAddrY = (UINT32)VIRTUAL(y_addr);
        stDestInfo.u32DestAddrU = (UINT32)VIRTUAL(c_addr); 

        if (u4FbDispHeight > GFX_HW_MAX_HEIGHT)
        {
            GFX_Memset((UINT8*)y_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0);
            GFX_Memset((UINT8*)(y_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0);
            GFX_Memset((UINT8*)c_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0x80);
            GFX_Memset((UINT8*)(c_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0x80); 
        }
        else
        {
            GFX_Memset((UINT8*)y_addr, u4FbDispWidth, u4FbDispHeight, 0);
            GFX_Memset((UINT8*)c_addr, u4FbDispWidth, u4FbDispHeight, 0x80);
        }

        if ((stSourceInfo.eType == YUV_422_SEP || stSourceInfo.eType == YUV_420_SEP)
            && eRotate != MTPHOTO4K_ROTATION_CLOCKWS_0)
        {
            MTPHOTO4K_VDP_ROTATE_T  tRotate = {0};
            
            bRotateBuf = _need_rotate_buffer(eRotate);

            tBlt.src_buffer_Y_addr = stSourceInfo.u32PhotoAddress;
            tBlt.src_buffer_C_addr = stSourceInfo.u32PhotoAddress + stSourceInfo.u32PhotoSize/2;
            tBlt.src_buffer_Y_width = 0;//stSourceInfo.u32PhotoPitch;
            tBlt.src_buffer_Y_height = 0;//stSourceInfo.u32Height;
            tBlt.src_buffer_Y_pitch = stSourceInfo.u32PhotoPitch;
            tBlt.src_clip_x = stSourceInfo.u32X;
            tBlt.src_clip_y = stSourceInfo.u32Y;
            tBlt.src_clip_w = stSourceInfo.u32Width;
            tBlt.src_clip_h = stSourceInfo.u32Height;

            tBlt.dest_buffer_Y_addr = y_addr;
            tBlt.dest_buffer_C_addr = c_addr;
            tBlt.dest_buffer_Y_width = 0;
            tBlt.dest_buffer_Y_height = 0;
            tBlt.dest_buffer_Y_pitch = u4FbDispWidth;   
            tBlt.dest_clip_x = 0;
            tBlt.dest_clip_y = 0;
            tBlt.dest_clip_w = stDestInfo.u32Width;
            tBlt.dest_clip_h = stDestInfo.u32Height;
            
            _mt53_fb_vdp_stretchblit(&tBlt);

            u4TmpBufPitch = bRotateBuf ? u4FbDispHeight : u4FbDispWidth;
            u4TmpBufPitch = (((u4TmpBufPitch + 15) >> 4) << 4);
            u4TmpBufHeight = bRotateBuf ? u4FbDispWidth : u4FbDispHeight;

            pTmpBuffer = (UINT8*)vmalloc(2*u4TmpBufPitch*u4TmpBufHeight);
            if (pTmpBuffer == NULL)
            {
                printk("vmalloc tmp buffer fail.\n");
                return MTR_OUT_OF_MEMORY;
            }
            printk("memset it.\n");
            x_memset(pTmpBuffer, 0, 2*u4TmpBufPitch*u4TmpBufHeight);

            tRotate.u4Src = y_addr;
            tRotate.u4SrcCbCrOffset = c_addr - y_addr;
            tRotate.eSrcCm = CM_YCbYCr422_DIRECT16;
            tRotate.u4SrcPitch = u4FbDispWidth;
            tRotate.u4SrcX = 0;
            tRotate.u4SrcY = 0;
            tRotate.u4Dst = (UINT32)pTmpBuffer;
            tRotate.u4DstCbCrOffset = u4TmpBufPitch*u4TmpBufHeight;
            tRotate.eDstCm = CM_YCbYCr422_DIRECT16;
            tRotate.u4DstPitch = u4TmpBufPitch;
            tRotate.u4DstX = 0;
            tRotate.u4DstY = 0;
            tRotate.u4Width = tBlt.dest_clip_w;
            tRotate.u4Height = tBlt.dest_clip_h;
            tRotate.u4RotateOp = _get_rotate_flag(eRotate);

            _mt53fb_rotate(&tRotate);
        }

        if (stSourceInfo.eType == YUV_422_SEP || stSourceInfo.eType == YUV_420_SEP)
        {
            MTPHOTO4K_STRETCH_BLIT_T tBlt = {0};

            if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_0)
            {
                tBlt.src_buffer_Y_addr = stSourceInfo.u32PhotoAddress;
                tBlt.src_buffer_C_addr = stSourceInfo.u32PhotoAddress + stSourceInfo.u32PhotoSize/2;
                tBlt.src_buffer_Y_width = 0;//stSourceInfo.u32PhotoPitch;
                tBlt.src_buffer_Y_height = 0;//stSourceInfo.u32Height;
                tBlt.src_buffer_Y_pitch = stSourceInfo.u32PhotoPitch;

                tBlt.src_clip_x = stSourceInfo.u32X;
                tBlt.src_clip_y = stSourceInfo.u32Y;
                tBlt.src_clip_w = stSourceInfo.u32Width;
                tBlt.src_clip_h = stSourceInfo.u32Height;
				//_IMG_DumpData("/tmp/usb/sda/sda/y.bin", tBlt.src_buffer_Y_addr, tBlt.src_clip_w, tBlt.src_clip_h, tBlt.src_buffer_Y_pitch);
				//_IMG_DumpData("/tmp/usb/sda/sda/c.bin", tBlt.src_buffer_C_addr, tBlt.src_clip_w, tBlt.src_clip_h, tBlt.src_buffer_Y_pitch);
				
				MTPHOTO4K_PRINT("## addr y 0x%x, addr c= 0x%x, width=%d, height=%d, size=%d, pitch=%d, colormode=%d\n", 
					tBlt.src_buffer_Y_addr, tBlt.src_buffer_C_addr, 
					tBlt.src_clip_w, tBlt.src_clip_h, stSourceInfo.u32PhotoSize, 
					tBlt.src_buffer_Y_pitch,stSourceInfo.eType);
            }
            else
            {
                tBlt.src_buffer_Y_addr = (UINT32)pTmpBuffer;
                tBlt.src_buffer_C_addr = (UINT32)(pTmpBuffer + u4TmpBufPitch*u4TmpBufHeight);
                tBlt.src_buffer_Y_width = 0;//stSourceInfo.u32PhotoPitch;
                tBlt.src_buffer_Y_height = 0;//stSourceInfo.u32Height;
                tBlt.src_buffer_Y_pitch = u4TmpBufPitch;

                tBlt.src_clip_x = 0;
                tBlt.src_clip_y = 0;
                tBlt.src_clip_w = bRotateBuf ? stDestInfo.u32Height : stDestInfo.u32Width;
                tBlt.src_clip_h = bRotateBuf ? stDestInfo.u32Width : stDestInfo.u32Height;

                if (u4FbDispHeight > GFX_HW_MAX_HEIGHT)
                {
                    GFX_Memset((UINT8*)y_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0);
                    GFX_Memset((UINT8*)(y_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0);
                    GFX_Memset((UINT8*)c_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0x80);
                    GFX_Memset((UINT8*)(c_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0x80);  
                }
                else
                {
                    GFX_Memset((UINT8*)y_addr, u4FbDispWidth, u4FbDispHeight, 0);
                    GFX_Memset((UINT8*)c_addr, u4FbDispWidth, u4FbDispHeight, 0x80);
                }
            }
            tBlt.dest_buffer_Y_addr = y_addr;
            tBlt.dest_buffer_C_addr = c_addr;
            tBlt.dest_buffer_Y_width = u4FbDispWidth;
            tBlt.dest_buffer_Y_height = u4FbDispHeight;
            tBlt.dest_buffer_Y_pitch = u4FbDispWidth;

            tBlt.dest_clip_x = stDestInfo.u32X;
            tBlt.dest_clip_y = stDestInfo.u32Y;
            tBlt.dest_clip_w = stDestInfo.u32Width;
            tBlt.dest_clip_h = stDestInfo.u32Height;

            HalFlushInvalidateDCacheMultipleLine(tBlt.src_buffer_Y_addr, tBlt.src_buffer_Y_pitch * tBlt.src_clip_h);
            HalFlushInvalidateDCacheMultipleLine(tBlt.src_buffer_C_addr, tBlt.src_buffer_Y_pitch * tBlt.src_clip_h);
			
            _mt53_fb_vdp_stretchblit(&tBlt);

            //HalFlushInvalidateDCache(); 
            HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(y_addr), u4FbDispWidth * u4FbDispHeight);
            HalFlushInvalidateDCacheMultipleLine((UINT32)VIRTUAL(c_addr), u4FbDispWidth * u4FbDispHeight);
			
            if(pTmpBuffer)
            {
                vfree(pTmpBuffer);
                pTmpBuffer = NULL;
            } 
        }
        else
        {   
            HalFlushInvalidateDCacheMultipleLine(stSourceInfo.u32PhotoAddress, 4*B2R_WIDTH_4K2K*B2R_HEIGHT_4K2K);
            vRgbtoYuv422_SW(&stSourceInfo, &stDestInfo);
            //HalFlushInvalidateDCache(); 
            HalFlushInvalidateDCacheMultipleLine(stDestInfo.u32DestAddrY, u4FbDispWidth * u4FbDispHeight);
            HalFlushInvalidateDCacheMultipleLine(stDestInfo.u32DestAddrU, u4FbDispWidth * u4FbDispHeight);
        }
#ifdef CC_JPEG_FBM_DYNAMIC_MAPPING 
        BSP_UnMapReservedMem((void *)stDestInfo.u32DestAddrY);
        BSP_UnMapReservedMem((void *)stDestInfo.u32DestAddrU);
#endif        
    }
    else
    {
        //clean screen
        FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &y_addr, &c_addr);
        if (y_addr == 0 || c_addr == 0)
        {
            printk("{_mt53_fb_b2r_flip2} FBM_GetFrameBufferAddr return null address.\n");
            return MTR_FBM_ERROR;
        }
        if (u4FbDispHeight > GFX_HW_MAX_HEIGHT)
        {
            GFX_Memset((UINT8*)y_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0);
            GFX_Memset((UINT8*)(y_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0);
            GFX_Memset((UINT8*)c_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0x80);
            GFX_Memset((UINT8*)(c_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0x80);  
        }
        else
        {
            GFX_Memset((UINT8*)y_addr, u4FbDispWidth, u4FbDispHeight, 0);
            GFX_Memset((UINT8*)c_addr, u4FbDispWidth, u4FbDispHeight, 0x80);
        }
    }

    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
    
    if (prFbmPicHdr == NULL)
    {
         MTPHOTO4K_PRINT("[%s] FBM_GetFrameBufferPicHdr failed.\n", __FUNCTION__);
         return MTR_FBM_ERROR;
    }
            
    prFbmPicHdr->ucPicStruct = (UCHAR)MPEG_PIC_FRAME;
    prFbmPicHdr->fgProgressiveFrm = TRUE;
    prFbmPicHdr->fgRepFirstFld = FALSE;
    prFbmPicHdr->fgTopFldFirst = FALSE;
    
    FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE); //default  
    FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY);  

    // Display
    FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_DISPLAYQ);
    
    // Unmute B2R
    VDP_SetMode(u1VideoPath, VDP_MODE_NORMAL);        
	
	MTPHOTO4K_PRINT("[%s] ucFbgId = %d\n", __FUNCTION__, ucFbgId); 		

    return MTR_OK;
}

static void _mt53_fb_b2r_setPitch(UINT32  u4FbWidth, UINT32 u4FbHeight, UINT32  u4FbPitch)
{
    FBM_SEQ_HDR_T* fbm_seq_hdr;
    if (u4FbPitch != 0 && g_fbg_scroll_id != FBM_FBG_ID_UNKNOWN)
    {
        fbm_seq_hdr = FBM_GetFrameBufferSeqHdr(g_fbg_scroll_id);
        if (fbm_seq_hdr == NULL)
        {
            return;
        }
        fbm_seq_hdr->u2HSize = u4FbWidth;
        fbm_seq_hdr->u2VSize = u4FbHeight;
        fbm_seq_hdr->u2OrgHSize = fbm_seq_hdr->u2HSize;
        fbm_seq_hdr->u2OrgVSize = fbm_seq_hdr->u2VSize;
        fbm_seq_hdr->u2LineSize = u4FbPitch;
        fbm_seq_hdr->fgProgressiveSeq = TRUE;
        fbm_seq_hdr->fgRasterOrder = TRUE;
        fbm_seq_hdr->fg422Mode = TRUE;
        fbm_seq_hdr->ucFrmRatCod = MPEG_FRAME_RATE_15; 
    
        FBM_SetFrameBufferFlag(g_fbg_scroll_id, FBM_FLAG_SEQ_CHG);
    }
}


static int _mt53_fb_b2r_install(UINT32 u4VDecFmt, UINT32  u4FbWidth, UINT32 u4FbHeight, UINT32  u4FbPitch)
{
    unsigned char fbg_type, fbg_id;
    FBM_SEQ_HDR_T* fbm_seq_hdr;
    FBM_CREATE_FBG_PAR_T rPar;
    FBM_POOL_T      *ptFbm;

    if (u4FbPitch == 0 && g_fbg_id != FBM_FBG_ID_UNKNOWN) 
    {    
        MTPHOTO4K_PRINT("[%s] FBG Already init \n ", __FUNCTION__);
        return MTR_OK;
    }

    if (u4FbPitch != 0 && g_fbg_scroll_id != FBM_FBG_ID_UNKNOWN)
    {
        return MTR_OK;
    }
   
    x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
    rPar.u1VdecId       = (UINT8)stSourceInfo.u8VideoPath;
    rPar.u1AppMode      = FBM_FBG_APP_MTIMAGE;
    rPar.fgExternalAddr = ((u4FbPitch == 0) ? FALSE : TRUE);
    rPar.u1RequestFbNs  = ((u4FbPitch == 0) ? 2 : 1);

	MTPHOTO4K_PRINT("[%s] vdecId=%d \n", __FUNCTION__, rPar.u1VdecId);

    fbg_type = FBM_SelectGroupType(u4FbWidth, u4FbHeight);
      
    FBM_SetColorMode(FBM_CM_420);
    fbg_id = FBM_CreateGroupExt(fbg_type, u4VDecFmt, u4FbWidth, u4FbHeight, &rPar);
    
    if(fbg_id == FBM_FBG_ID_UNKNOWN)
    {
        MTPHOTO4K_PRINT("[%s] FBM_Create_Group %d x %d Fail \n", __FUNCTION__, u4FbWidth, u4FbHeight);
        return MTR_FBM_ERROR;
    }

    if (rPar.fgExternalAddr)
    {
        ptFbm = FBM_GetPoolInfo(FBM_POOL_TYPE_DMX);
        FBM_SetFrameBufferLayout(fbg_id, 0, ptFbm->u4Addr, TRUE);
        ptFbm = FBM_GetPoolInfo(FBM_POOL_TYPE_FEEDER);
        FBM_SetFrameBufferLayout(fbg_id, 0, ptFbm->u4Addr, FALSE);
    }
    
    FBM_SetPlayMode(fbg_id, FBM_FBG_MM_MODE);
    FBM_SetSyncStc(fbg_id, 0, stSourceInfo.u8VideoPath); //none
    FBM_FbgChgNotify(fbg_id, stSourceInfo.u8VideoPath);

    if (u4FbPitch == 0)
    {
        u4FbPitch = ((u4FbWidth + 15) >> 4) << 4;
    }

    fbm_seq_hdr = FBM_GetFrameBufferSeqHdr(fbg_id);
    fbm_seq_hdr->u2HSize = u4FbWidth;
    fbm_seq_hdr->u2VSize = u4FbHeight;
    fbm_seq_hdr->u2OrgHSize = fbm_seq_hdr->u2HSize;
    fbm_seq_hdr->u2OrgVSize = fbm_seq_hdr->u2VSize;
    fbm_seq_hdr->u2LineSize = u4FbPitch;
    fbm_seq_hdr->fgProgressiveSeq = TRUE;
    fbm_seq_hdr->fgRasterOrder = TRUE;
    fbm_seq_hdr->fg422Mode = FALSE;
    #if defined(CC_MT5882)
    fbm_seq_hdr->ucFrmRatCod = MPEG_FRAME_RATE_15;
    #else
    fbm_seq_hdr->ucFrmRatCod = MPEG_FRAME_RATE_30; 
    #endif
    
    FBM_SetFrameBufferFlag(fbg_id, FBM_FLAG_SEQ_CHG);

    if (rPar.fgExternalAddr)
    {
        g_fbg_scroll_id = fbg_id;
    }
    else
    {
        g_fbg_id = fbg_id;
		MTPHOTO4K_PRINT("[%s] g_fbg_id=%d.\n", __FUNCTION__, g_fbg_id); 		
    }
    
    return MTR_OK;
}


static INT32 _mt53_fb_b2r_uninstall(UINT8 u1VideoPath)
{
	MTPHOTO4K_PRINT("[%s] begin release fbm, g_fbg_id=%d, path=%d.\n", __FUNCTION__, g_fbg_id, u1VideoPath);	
    FBM_ReleaseGroup(g_fbg_id);
//    FBM_ReleaseGroup(g_fbg_scroll_id);
    FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, u1VideoPath);
    
    g_fbg_id = FBM_FBG_ID_UNKNOWN;
    g_fbg_scroll_id = FBM_FBG_ID_UNKNOWN;

    g_fb_id[B2R_CURRENT_ID] = FBM_FB_ID_UNKNOWN;
    g_fb_id[B2R_NEXT_ID]    = FBM_FB_ID_UNKNOWN;
	MTPHOTO4K_PRINT("[%s] out release fbm.\n", __FUNCTION__);	

    return 0;
}

static HANDLE_T    hSema = NULL_HANDLE;

static INT32 _MTPHOTO4K_Init(UINT32 u4Arg)
{
    VDP_REGION_T rRegion = {0, 0, 10000, 10000};
    VDP_OVERSCAN_REGION_T rORegion = {0, 0, 0, 0};
    MT_RESULT_T ret;
    UINT8 u1VideoPath;
    UINT32      u4FbDispWidth, u4FbDispHeight;
    #ifdef CC_B2R_RM_SUPPORT
    B2R_RM_T    t_b2r_src = {0};
    #endif

    MTAL_IOCTL_2ARG_T rArg;
    USR_SPACE_ACCESS_VALIDATE_2ARG(u4Arg);
    COPY_FROM_USER_2ARG(u4Arg,rArg);

    u1VideoPath = rArg.ai4Arg[0];
	
	MTPHOTO4K_PRINT("[%s] u1VideoPath=%d !!!\n", __FUNCTION__, u1VideoPath);

    stSourceInfo.u8VideoPath = u1VideoPath;

    if (_Panel_Is_4K2K())
    {
        u4FbDispWidth = B2R_WIDTH_4K2K;
        u4FbDispHeight = B2R_HEIGHT_4K2K;
    }
    else
    {
        u4FbDispWidth = B2R_WIDTH_FHD;
        u4FbDispHeight = B2R_HEIGHT_FHD;
    }
	bApiVideoSetVideoSrc(u1VideoPath, SV_VS_DT1);

    ret = _mt53_fb_b2r_install(FBM_VDEC_JPEG, u4FbDispWidth, u4FbDispHeight, 0);
    if (ret < MTR_OK)
    {
        return ret;
    }
    
    ret = _mt53_fb_b2r_flip(0, 0, u1VideoPath, MTPHOTO4K_ROTATION_CLOCKWS_0); 
    if (ret < MTR_OK)
    {
        return ret;
    }
        
    VDP_SetEnable(u1VideoPath, 1);
    VDP_SetMode(u1VideoPath, VDP_MODE_BG);
   // VDP_SetInput(u1VideoPath, 0, 0);  
    VDP_SetSrcRegion(u1VideoPath, 1, rRegion); 
    VDP_SetOutRegion(u1VideoPath, 1, rRegion); 
    VDP_SetOverScan(u1VideoPath,  rORegion);

    #ifdef CC_B2R_RM_SUPPORT
    t_b2r_src.u2HActive = u4FbDispWidth;
    t_b2r_src.u2VActive = u4FbDispHeight;
    t_b2r_src.u2BlockMode = 0;
    B2R_Source_Connect(t_b2r_src);
    #endif

#if defined(CC_MT5890)
	if(!IS_IC_5861())
#endif
    {   
//        VDP_B2rSwitch(u1VideoPath, B2R_1);
    }

    if (hSema == NULL_HANDLE)
    {
        INT32 i4_ret;
        i4_ret = x_sema_create(&hSema, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK);
        if (i4_ret != OSR_OK)
        {
            MTPHOTO4K_PRINT("[%s] x_sema_create error, ret=%d!!!\n", __FUNCTION__, i4_ret);
            return MTR_NOT_OK;
        }
    }

/*
    if (NULL == pPhotoBuffer)
    {
        pPhotoBuffer = (UINT8*)vmalloc(4*B2R_WIDTH_4K2K*B2R_HEIGHT_4K2K);
    }
    
    if (NULL == pPhotoBuffer) 
    {
        MTPHOTO4K_PRINT("[%s] Alloc buffer error!!!\n", __FUNCTION__);
        bApiVideoSetVideoSrc(0, SV_VS_MAX);
        VDP_SetEnable(0, 0);
        return -1;
    }
*/
    return MTR_OK;   
}

static INT32 _MTPHOTO4K_SetDisplayInfo(UINT32 u4Arg)
{
    MTAL_IOCTL_8ARG_T rArg;
    MTPHOTO4K_RGB_TYPE_T eType;
    MTPHOTO4K_DISPLAY_REGION_T  stDispRegion, stSrcRegion;
    MTPHOTO4K_DISPLAY_REGION_T* pDispRegion, *pSrcRegion;
    UINT8* pBufferAddr;
    UINT32 u4BufferSize, u4BufferPitch;
    UINT8  u1VideoPath;
    UINT32  u4FbDispWidth, u4FbDispHeight;
    MTPHOTO4K_ROTATION_T    eRotate;
    VDP_REGION_T rRegion = {0, 0, 10000, 10000};
    
    USR_SPACE_ACCESS_VALIDATE_8ARG(u4Arg);
    COPY_FROM_USER_8ARG(u4Arg,rArg);
    eType = rArg.ai4Arg[0];
    
    pBufferAddr = (UINT8*)(rArg.ai4Arg[1]);
    u4BufferSize = rArg.ai4Arg[2];
    u4BufferPitch = rArg.ai4Arg[3];
    
    pDispRegion = (MTPHOTO4K_DISPLAY_REGION_T*)rArg.ai4Arg[4];
    USR_SPACE_ACCESS_VALIDATE_ARG(pDispRegion, MTPHOTO4K_DISPLAY_REGION_T);
    COPY_FROM_USER_ARG(pDispRegion, stDispRegion, MTPHOTO4K_DISPLAY_REGION_T);

    pSrcRegion = (MTPHOTO4K_DISPLAY_REGION_T*)rArg.ai4Arg[5];
    USR_SPACE_ACCESS_VALIDATE_ARG(pSrcRegion, MTPHOTO4K_DISPLAY_REGION_T);
    COPY_FROM_USER_ARG(pSrcRegion, stSrcRegion, MTPHOTO4K_DISPLAY_REGION_T);

    u1VideoPath = rArg.ai4Arg[6];
    MTPHOTO4K_PRINT("_MTPHOTO4K_SetDisplayInfo videoPath=%d.\n", u1VideoPath);
    eRotate = rArg.ai4Arg[7];

    if (_Panel_Is_4K2K())
    {
        u4FbDispWidth = B2R_WIDTH_4K2K;
        u4FbDispHeight = B2R_HEIGHT_4K2K;
    }
    else
    {
        u4FbDispWidth = B2R_WIDTH_FHD;
        u4FbDispHeight = B2R_HEIGHT_FHD;
    }

    if (stDispRegion.u4x > u4FbDispWidth || (stDispRegion.u4x + stDispRegion.u4Width) > u4FbDispWidth ||
        stDispRegion.u4y > u4FbDispHeight || (stDispRegion.u4y + stDispRegion.u4Height) > u4FbDispHeight)
    {
        MTPHOTO4K_PRINT("[%s]The destination size is over frame buffer size !!\n", __FUNCTION__);
        return MTR_ERR_INV;
    }

    if (eType != YUV_422_SEP && eType != YUV_420_SEP)
    {
        if (pPhotoBuffer == NULL)
        {
            MTPHOTO4K_PRINT("[%s %d] pPhotoBuffer is NULL.\n");
            return MTR_NOT_OK;
        }
        USR_SPACE_ACCESS_VALIDATE_ARG_K(pBufferAddr, u4BufferSize);
        COPY_FROM_USER_ARG_K(pBufferAddr, pPhotoBuffer[0], u4BufferSize);
    }
    
    //X offset should be even for 422
    stDestInfo.u32X= stDispRegion.u4x & (~1U);
    stDestInfo.u32Y= stDispRegion.u4y;
    stDestInfo.u32Width= stDispRegion.u4Width;
    stDestInfo.u32Height= stDispRegion.u4Height;

    stSourceInfo.u32X = stSrcRegion.u4x & (~1U);
    stSourceInfo.u32Y = stSrcRegion.u4y;
    stSourceInfo.u32Width = stSrcRegion.u4Width;
    stSourceInfo.u32Height = stSrcRegion.u4Height;
    stSourceInfo.eType = eType;
    stSourceInfo.u32PhotoSize = u4BufferSize;
    stSourceInfo.u32PhotoPitch = u4BufferPitch;
    stSourceInfo.u8VideoPath = u1VideoPath;
    
    VDP_SetSrcRegion(u1VideoPath, 1, rRegion); 
    VDP_SetOutRegion(u1VideoPath, 1, rRegion);
    
    if (eType == YUV_422_SEP || eType == YUV_420_SEP)
    {
        stSourceInfo.u32PhotoAddress = (UINT32)pBufferAddr;
        
        return _mt53_fb_b2r_flip(1, TRUE, u1VideoPath, eRotate);
    }
    else
    { 
        stSourceInfo.u32PhotoAddress = (UINT32)pPhotoBuffer;
        return MTR_OK;
    }
}

static INT32 _MTPHOTO4K_Show(UINT32 u4Arg)
{
    UINT8 u1BufNum;
    UINT32 u4Show;
    MTAL_IOCTL_2ARG_T rArg;

    USR_SPACE_ACCESS_VALIDATE_2ARG(u4Arg);
    COPY_FROM_USER_2ARG(u4Arg,rArg);
    u1BufNum = rArg.ai4Arg[0];
    u4Show = rArg.ai4Arg[1];    
    
    return _mt53_fb_b2r_flip(u1BufNum, u4Show, stSourceInfo.u8VideoPath, MTPHOTO4K_ROTATION_CLOCKWS_0);        
}

static INT32 _MTPHOTO4K_DeInit( VOID)
{    
    VDP_REGION_T rRegion = {0, 0, 0, 0};
    
    VDP_SetOutRegion(stSourceInfo.u8VideoPath, 0, rRegion);
	VDP_SetMode(stSourceInfo.u8VideoPath, VDP_MODE_BG);
	VDP_SetEnable(stSourceInfo.u8VideoPath, 0);
    bApiVideoSetVideoSrc(stSourceInfo.u8VideoPath, SV_VS_MAX);

    _mt53_fb_b2r_uninstall(stSourceInfo.u8VideoPath); 
/*
    if(pPhotoBuffer)
    {
        vfree((void *)pPhotoBuffer);
        pPhotoBuffer = NULL;
    }
     */   
    return MTR_OK;
}
static INT32 _MTPHOTO4K_Prepare(UINT32 u4Arg)
{
    MTAL_IOCTL_7ARG_T           rArg;
    MTPHOTO4K_DISPLAY_REGION_T  stDispRegion, stSrcRegion;
    MTPHOTO4K_DISPLAY_REGION_T* pDispRegion, *pSrcRegion;
    UINT8*                      pBufferAddr;
    UINT32                      u4BufferPitch, u4BufferSize;
    UINT8                       u1VideoPath;
    UINT32                      y_addr, c_addr;
    UCHAR                       ucFbgId, ucFbId;
    FBM_PIC_HDR_T*              prFbmPicHdr; 
    UINT32                      u4FbDispWidth, u4FbDispHeight;
    BOOL                        bRotateBuf = FALSE;
    UINT32                      u4TmpBufPitch, u4TmpBufHeight;
    UINT8*                      pTmpBuffer = NULL;
    MTPHOTO4K_ROTATION_T        eRotate;
    MTPHOTO4K_STRETCH_BLIT_T    tBlt = {0};
    VDP_REGION_T rRegion = {0, 0, 10000, 10000};
    
    USR_SPACE_ACCESS_VALIDATE_7ARG(u4Arg);
    COPY_FROM_USER_7ARG(u4Arg,rArg);

    u1VideoPath = rArg.ai4Arg[0];
    pBufferAddr = (UINT8*)(rArg.ai4Arg[1]);
    u4BufferSize = rArg.ai4Arg[2];
    u4BufferPitch = rArg.ai4Arg[3];
    
    pDispRegion = (MTPHOTO4K_DISPLAY_REGION_T*)rArg.ai4Arg[5];
    USR_SPACE_ACCESS_VALIDATE_ARG(pDispRegion, MTPHOTO4K_DISPLAY_REGION_T);
    COPY_FROM_USER_ARG(pDispRegion, stDispRegion, MTPHOTO4K_DISPLAY_REGION_T);

    pSrcRegion = (MTPHOTO4K_DISPLAY_REGION_T*)rArg.ai4Arg[4];
    USR_SPACE_ACCESS_VALIDATE_ARG(pSrcRegion, MTPHOTO4K_DISPLAY_REGION_T);
    COPY_FROM_USER_ARG(pSrcRegion, stSrcRegion, MTPHOTO4K_DISPLAY_REGION_T);

    MTPHOTO4K_PRINT("_MTPHOTO4K_Prepare videoPath=%d.\n", u1VideoPath);
    eRotate = rArg.ai4Arg[6];

    if (_Panel_Is_4K2K())
    {
        u4FbDispWidth = B2R_WIDTH_4K2K;
        u4FbDispHeight = B2R_HEIGHT_4K2K;
    }
    else
    {
        u4FbDispWidth = B2R_WIDTH_FHD;
        u4FbDispHeight = B2R_HEIGHT_FHD;
    }
    
    if (stDispRegion.u4x > u4FbDispWidth || (stDispRegion.u4x + stDispRegion.u4Width) > u4FbDispWidth ||
        stDispRegion.u4y > u4FbDispHeight || (stDispRegion.u4y + stDispRegion.u4Height) > u4FbDispHeight)
    {
        MTPHOTO4K_PRINT("[%s]The destination size is over frame buffer size !!\n", __FUNCTION__);
        return MTR_ERR_INV;
    }

    //X offset should be even for 422
    stDestInfo.u32X= stDispRegion.u4x & (~1U);
    stDestInfo.u32Y= stDispRegion.u4y;
    stDestInfo.u32Width= stDispRegion.u4Width;
    stDestInfo.u32Height= stDispRegion.u4Height;

    stSourceInfo.u32X = stSrcRegion.u4x & (~1U);
    stSourceInfo.u32Y = stSrcRegion.u4y;
    stSourceInfo.u32Width = stSrcRegion.u4Width;
    stSourceInfo.u32Height = stSrcRegion.u4Height;
    stSourceInfo.u32PhotoAddress = (UINT32)pBufferAddr;
    stSourceInfo.u32PhotoSize = u4BufferSize;
    stSourceInfo.u32PhotoPitch = u4BufferPitch;
    stSourceInfo.u8VideoPath = u1VideoPath;

    ucFbgId = g_fbg_id;
    ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);    
             
    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &y_addr, &c_addr);
    stDestInfo.u32DestAddrY = (UINT32)VIRTUAL(y_addr);
    stDestInfo.u32DestAddrU = (UINT32)VIRTUAL(c_addr); 

    if (u4FbDispHeight > GFX_HW_MAX_HEIGHT)
    {
        GFX_Memset((UINT8*)y_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0);
        GFX_Memset((UINT8*)(y_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0);
        GFX_Memset((UINT8*)c_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0x80);
        GFX_Memset((UINT8*)(c_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0x80);  
    }
    else
    {
        GFX_Memset((UINT8*)y_addr, u4FbDispWidth, u4FbDispHeight, 0);
        GFX_Memset((UINT8*)c_addr, u4FbDispWidth, u4FbDispHeight, 0x80);
    }

    if (eRotate != MTPHOTO4K_ROTATION_CLOCKWS_0)
    {
        MTPHOTO4K_VDP_ROTATE_T  tRotate = {0};
        
        bRotateBuf = _need_rotate_buffer(eRotate);

        tBlt.src_buffer_Y_addr = stSourceInfo.u32PhotoAddress;
        tBlt.src_buffer_C_addr = stSourceInfo.u32PhotoAddress + stSourceInfo.u32PhotoSize/2;
        tBlt.src_buffer_Y_width = 0;//stSourceInfo.u32PhotoPitch;
        tBlt.src_buffer_Y_height = 0;//stSourceInfo.u32Height;
        tBlt.src_buffer_Y_pitch = stSourceInfo.u32PhotoPitch;
        tBlt.src_clip_x = stSourceInfo.u32X;
        tBlt.src_clip_y = stSourceInfo.u32Y;
        tBlt.src_clip_w = stSourceInfo.u32Width;
        tBlt.src_clip_h = stSourceInfo.u32Height;

        tBlt.dest_buffer_Y_addr = y_addr;
        tBlt.dest_buffer_C_addr = c_addr;
        tBlt.dest_buffer_Y_width = 0;
        tBlt.dest_buffer_Y_height = 0;
        tBlt.dest_buffer_Y_pitch = u4FbDispWidth;   
        tBlt.dest_clip_x = 0;
        tBlt.dest_clip_y = 0;
        tBlt.dest_clip_w = stDestInfo.u32Width;
        tBlt.dest_clip_h = stDestInfo.u32Height;
        
        _mt53_fb_vdp_stretchblit(&tBlt);

        u4TmpBufPitch = bRotateBuf ? u4FbDispHeight : u4FbDispWidth;
        u4TmpBufPitch = (((u4TmpBufPitch + 15) >> 4) << 4);
        u4TmpBufHeight = bRotateBuf ? u4FbDispWidth : u4FbDispHeight;

        pTmpBuffer = (UINT8*)vmalloc(2*u4TmpBufPitch*u4TmpBufHeight);
        if (pTmpBuffer == NULL)
        {
            printk("vmalloc tmp buffer fail.\n");
            return MTR_OUT_OF_MEMORY;
        }
        printk("memset it.\n");
        x_memset(pTmpBuffer, 0, 2*u4TmpBufPitch*u4TmpBufHeight);

        tRotate.u4Src = y_addr;
        tRotate.u4SrcCbCrOffset = c_addr - y_addr;
        tRotate.eSrcCm = CM_YCbYCr422_DIRECT16;
        tRotate.u4SrcPitch = u4FbDispWidth;
        tRotate.u4SrcX = 0;
        tRotate.u4SrcY = 0;
        tRotate.u4Dst = (UINT32)pTmpBuffer;
        tRotate.u4DstCbCrOffset = u4TmpBufPitch*u4TmpBufHeight;
        tRotate.eDstCm = CM_YCbYCr422_DIRECT16;
        tRotate.u4DstPitch = u4TmpBufPitch;
        tRotate.u4DstX = 0;
        tRotate.u4DstY = 0;
        tRotate.u4Width = tBlt.dest_clip_w;
        tRotate.u4Height = tBlt.dest_clip_h;
        tRotate.u4RotateOp = _get_rotate_flag(eRotate);

        _mt53fb_rotate(&tRotate);
    }

    if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_0)
    {
        tBlt.src_buffer_Y_addr = stSourceInfo.u32PhotoAddress;
        tBlt.src_buffer_C_addr = stSourceInfo.u32PhotoAddress + stSourceInfo.u32PhotoSize/2;
        tBlt.src_buffer_Y_width = 0;//stSourceInfo.u32PhotoPitch;
        tBlt.src_buffer_Y_height = 0;//stSourceInfo.u32Height;
        tBlt.src_buffer_Y_pitch = stSourceInfo.u32PhotoPitch;

        tBlt.src_clip_x = stSourceInfo.u32X;
        tBlt.src_clip_y = stSourceInfo.u32Y;
        tBlt.src_clip_w = stSourceInfo.u32Width;
        tBlt.src_clip_h = stSourceInfo.u32Height;
    }
    else
    {
        tBlt.src_buffer_Y_addr = (UINT32)pTmpBuffer;
        tBlt.src_buffer_C_addr = (UINT32)(pTmpBuffer + u4TmpBufPitch*u4TmpBufHeight);
        tBlt.src_buffer_Y_width = 0;//stSourceInfo.u32PhotoPitch;
        tBlt.src_buffer_Y_height = 0;//stSourceInfo.u32Height;
        tBlt.src_buffer_Y_pitch = u4TmpBufPitch;

        tBlt.src_clip_x = 0;
        tBlt.src_clip_y = 0;
        tBlt.src_clip_w = bRotateBuf ? stDestInfo.u32Height : stDestInfo.u32Width;
        tBlt.src_clip_h = bRotateBuf ? stDestInfo.u32Width : stDestInfo.u32Height;

        if (u4FbDispHeight > GFX_HW_MAX_HEIGHT)
        {
            GFX_Memset((UINT8*)y_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0);
            GFX_Memset((UINT8*)(y_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0);
            GFX_Memset((UINT8*)c_addr, u4FbDispWidth, GFX_HW_MAX_HEIGHT, 0x80);
            GFX_Memset((UINT8*)(c_addr+u4FbDispWidth*GFX_HW_MAX_HEIGHT), u4FbDispWidth, (u4FbDispHeight-GFX_HW_MAX_HEIGHT), 0x80);  
        }
        else
        {
            GFX_Memset((UINT8*)y_addr, u4FbDispWidth, u4FbDispHeight, 0);
            GFX_Memset((UINT8*)c_addr, u4FbDispWidth, u4FbDispHeight, 0x80);
        }
    }
        tBlt.dest_buffer_Y_addr = y_addr;
        tBlt.dest_buffer_C_addr = c_addr;
        tBlt.dest_buffer_Y_width = u4FbDispWidth;
        tBlt.dest_buffer_Y_height = u4FbDispHeight;
        tBlt.dest_buffer_Y_pitch = u4FbDispWidth;

        tBlt.dest_clip_x = stDestInfo.u32X;
        tBlt.dest_clip_y = stDestInfo.u32Y;
        tBlt.dest_clip_w = stDestInfo.u32Width;
        tBlt.dest_clip_h = stDestInfo.u32Height;
        
        _mt53_fb_vdp_stretchblit(&tBlt);
    if(pTmpBuffer)
    {
        vfree(pTmpBuffer);
        pTmpBuffer = NULL;
    }
  
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
    
    if (prFbmPicHdr == NULL)
    {
         MTPHOTO4K_PRINT("[%s] FBM_GetFrameBufferPicHdr failed.\n", __FUNCTION__);
         return MTR_FBM_ERROR;
    }
            
    prFbmPicHdr->ucPicStruct = (UCHAR)MPEG_PIC_FRAME;
    prFbmPicHdr->fgProgressiveFrm = TRUE;
    prFbmPicHdr->fgRepFirstFld = FALSE;
    prFbmPicHdr->fgTopFldFirst = FALSE;
    VDP_SetSrcRegion(u1VideoPath, 1, rRegion); 
    
    FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE); //default  
    //FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY); 
    g_fb_id[B2R_NEXT_ID] = ucFbId;

    return MTR_OK;
}

VOID _b2r_fade_callback(UCHAR videoPath, void * pv_tag)
{
    UINT8       u1Value = 0;
    MTPHOTO4K_FADE_B2R_CALLBACK_TAG_T * pt_tag;
    UINT32      u4Shift = 0, u4RefValue = 0;

    if (pv_tag == NULL)
    {
        printk("_b2r_fade_callback pv_tag is null.\n");
        return;
    }

    pt_tag = (MTPHOTO4K_FADE_B2R_CALLBACK_TAG_T *)pv_tag;

    if (videoPath != pt_tag->videoPath)
    {
        return;
    }

    if ( _u4DrvVideoGetMute(videoPath))
    {
        printk("it is mute.\n");
        return;
    }
    if (bPause)
    {
        VDP_VsyncCBRegister(videoPath, NULL, NULL);
        _MTPHOTO4K_Notify(NULL, MTPHOTO4K_CALLBACK_TYPE_FADE_PAUSED);
        printk("_b2r_fade_callback is paused.\n");
        x_sema_unlock(hSema);
        return;
    }
    
    if (pt_tag->fadeFrameIdx > pt_tag->fadeFrame)
    {
        VDP_VsyncCBRegister(videoPath, NULL, NULL);
        _MTPHOTO4K_Notify(NULL, MTPHOTO4K_CALLBACK_TYPE_FADE_COMPLETE);
        x_sema_unlock(hSema);
        printk("_b2r_fade_callback is finished.\n");
        return;
    }
    u4Shift = ((MAX_FADE_DURATION * 10 )/ pt_tag->fadeFrame + 5)/10;//si she wu ru
    if (pt_tag->fadeType == MTPHOTO4K_FADE_TYPE_OUT)
    {    
        u4RefValue = pt_tag->fadeFrameIdx * u4Shift;
        if (pt_tag->fadeFrameIdx == pt_tag->fadeFrame)
        {
            u4RefValue = MAX_FADE_DURATION - 1;
        }
    }
    else
    {
        u4RefValue = (pt_tag->fadeFrame - pt_tag->fadeFrameIdx) * u4Shift;
        if (pt_tag->fadeFrameIdx == 0)
        {
            u4RefValue = MAX_FADE_DURATION - 1;
        }
    }

    if (u4RefValue >= MAX_FADE_DURATION)
    {
        u4RefValue = MAX_FADE_DURATION - 1;
    }
    u4RefValue = au2CurveFadeReferenceTable[u4RefValue];

    u1Value = 255 * u4RefValue / 4096;

    printk("u1Value = %d.\n", u1Value);

    #if (defined(CC_MT5399) || defined(CC_MT5890))
    vDrvSetFade(pt_tag->videoPath, u1Value, pt_tag->fadeBgColor);
    #endif
    
    pt_tag->fadeFrameIdx++;
}


static INT32 _MTPHOTO4K_StartFade(UINT32 u4Arg)
{

    MTAL_IOCTL_4ARG_T                   rArg;
    UINT8                               videoPath;
    UINT32                              duration;

    USR_SPACE_ACCESS_VALIDATE_4ARG(u4Arg);
    COPY_FROM_USER_4ARG(u4Arg,rArg);

    videoPath = rArg.ai4Arg[0];
    duration  = rArg.ai4Arg[2];

    if (g_fb_id[B2R_NEXT_ID] !=  g_fb_id[B2R_CURRENT_ID])
    {
        g_fb_id[B2R_CURRENT_ID] = g_fb_id[B2R_NEXT_ID];
        
        // Display
        FBM_SetFrameBufferStatus(g_fbg_id, g_fb_id[B2R_CURRENT_ID], (UCHAR)FBM_FB_STATUS_READY); 
        FBM_SetFrameBufferStatus(g_fbg_id, g_fb_id[B2R_CURRENT_ID], (UCHAR)FBM_FB_STATUS_DISPLAYQ);
        VDP_SetMode(videoPath, VDP_MODE_NORMAL); 
    }

    t_fade_tag.videoPath = videoPath;
    t_fade_tag.fadeType = rArg.ai4Arg[1];
    t_fade_tag.fadeFrame = duration * 30 / 1000;
    t_fade_tag.fadeBgColor = rArg.ai4Arg[3];
    t_fade_tag.fadeFrameIdx = 0;

    if (t_fade_tag.fadeFrame > MAX_FADE_DURATION)
    {
        t_fade_tag.fadeFrame = MAX_FADE_DURATION;
    }

    x_sema_lock(hSema, X_SEMA_OPTION_WAIT);
    VDP_VsyncCBRegister(videoPath, _b2r_fade_callback, (VOID*)(&t_fade_tag));

    return MTR_OK;
}

static INT32 _MTPHOTO4K_PauseFade(UINT32 u4Arg)
{
    bPause = TRUE;
    x_sema_lock(hSema, X_SEMA_OPTION_WAIT);
    x_sema_unlock(hSema);
    bPause = FALSE;
    return MTR_OK;
}

static INT32 _MTPHOTO4K_RestartFade(UINT32 u4Arg)
{
    if (t_fade_tag.fadeFrameIdx >= t_fade_tag.fadeFrame)
    {
        return MTR_NOT_OK;
    }

    x_sema_lock(hSema, X_SEMA_OPTION_WAIT);
    VDP_VsyncCBRegister(t_fade_tag.videoPath, _b2r_fade_callback, (VOID*)(&t_fade_tag));

    return MTR_OK;
}
VOID _b2r_scroll_callback(UCHAR videoPath, void * pv_tag)
{
    VDP_REGION_T        tDispRegion = {0};
    INT32               i4PerHorMove = 0, i4PerVerMove = 0;
    MTPHOTO4K_SCROLL_B2R_CALLBACK_TAG_T    *pt_tag;
    MTPHOTO4K_SCROLL_DIRECTION              direction;
    UINT32              u4Width, u4Height;  
    UINT32              u4FbDispWidth, u4FbDispHeight;

    if (pv_tag == NULL)
    {
        printk("_b2r_fade_callback pv_tag is null.\n");
        return;
    }

    pt_tag = (MTPHOTO4K_SCROLL_B2R_CALLBACK_TAG_T*)pv_tag;

    if (_Panel_Is_4K2K())
    {
        u4FbDispWidth = B2R_WIDTH_4K2K;
        u4FbDispHeight = B2R_HEIGHT_4K2K;
    }
    else
    {
        u4FbDispWidth = B2R_WIDTH_FHD;
        u4FbDispHeight = B2R_HEIGHT_FHD;
    }

    if ( _u4DrvVideoGetMute(videoPath))
    {
        printk("it is mute.\n");
        if ((pt_tag->direction == MTPHOTO4K_SCROLL_B2T) || (pt_tag->direction == MTPHOTO4K_SCROLL_R2L))
        {
            tDispRegion.u4X = 0;
            tDispRegion.u4Y = 0;
            tDispRegion.u4Width = u4FbDispWidth;
            tDispRegion.u4Height = u4FbDispHeight;
            
            if (pt_tag->direction == MTPHOTO4K_SCROLL_B2T)
            {
                tDispRegion.u4Y = pt_tag->displayHeight - u4FbDispHeight;
            }
            else
            {
                tDispRegion.u4X = pt_tag->displayWidth - u4FbDispWidth;
            }
            
            VDP_SetRenderRegion(pt_tag->videoPath, &tDispRegion);
        }
        return;
    }
    
    
    direction = pt_tag->direction;
    u4Width = pt_tag->displayWidth;
    u4Height = pt_tag->displayHeight;

    if (videoPath != pt_tag->videoPath)
    {
        return;
    }

    if (bPause)
    {
        VDP_VsyncCBRegister(videoPath, NULL, NULL);
        _MTPHOTO4K_Notify(NULL, MTPHOTO4K_CALLBACK_TYPE_SCROLL_PAUSED);
        x_sema_unlock(hSema);
        printk("_b2r_scroll_callback is paused.\n");
        return;
    }

    

    //printk("_b2r_scroll_callback, pt_tag->stepIdx=%d.\n", pt_tag->stepIdx);

    tDispRegion.u4X = 0;
    tDispRegion.u4Y = 0;
    tDispRegion.u4Width = u4FbDispWidth;
    tDispRegion.u4Height = u4FbDispHeight;

    //try to calculate stepNum firstly.
    if (pt_tag->stepIdx == 0)
    {
        if ((direction == MTPHOTO4K_SCROLL_L2R) || (direction == MTPHOTO4K_SCROLL_R2L))
        {
            i4PerHorMove = (u4Width - u4FbDispWidth) / pt_tag->stepNum;
            if (i4PerHorMove == 0)
            {
                i4PerHorMove = 1;
            }
            i4PerHorMove = ((i4PerHorMove + 15) >> 4) << 4;//align 16
            pt_tag->stepNum = (u4Width - u4FbDispWidth) / i4PerHorMove;
        }
        else
        {
            i4PerVerMove = (u4Height - u4FbDispHeight + pt_tag->stepNum - 1) / pt_tag->stepNum;
            pt_tag->stepNum = (u4Height - u4FbDispHeight) / i4PerVerMove;
        }
        if (pt_tag->stepNum == 0)
        {
            VDP_VsyncCBRegister(videoPath, NULL, NULL);
            _MTPHOTO4K_Notify(NULL, MTPHOTO4K_CALLBACK_TYPE_SCROLL_COMPLETE);
            x_sema_unlock(hSema);
            printk("_b2r_scroll_callback is finished.\n");
            return;
        }
    }
    else
    {
        if ((direction == MTPHOTO4K_SCROLL_L2R) || (direction == MTPHOTO4K_SCROLL_R2L))
        {
            i4PerHorMove = (u4Width - u4FbDispWidth) / pt_tag->stepNum;
        }
        else
        {
            i4PerVerMove = (u4Height - u4FbDispHeight) / pt_tag->stepNum;
        }
    }

    if (pt_tag->stepIdx > pt_tag->stepNum)
    {
        VDP_VsyncCBRegister(videoPath, NULL, NULL);
        _MTPHOTO4K_Notify(NULL, MTPHOTO4K_CALLBACK_TYPE_SCROLL_COMPLETE);
        x_sema_unlock(hSema);
        printk("_b2r_scroll_callback is finished.\n");
        return;
    }
    
    if (direction == MTPHOTO4K_SCROLL_L2R)
    {
        tDispRegion.u4X = (pt_tag->stepIdx) * i4PerHorMove;
    }
    else if (direction == MTPHOTO4K_SCROLL_R2L)
    {
        tDispRegion.u4X = u4Width - u4FbDispWidth - (pt_tag->stepIdx) * i4PerHorMove;
    }
    else if (direction == MTPHOTO4K_SCROLL_T2B)
    {
        tDispRegion.u4Y = (pt_tag->stepIdx) * i4PerVerMove;
    }
    else if (direction == MTPHOTO4K_SCROLL_B2T)
    {
        tDispRegion.u4Y = u4Height - u4FbDispHeight - (pt_tag->stepIdx) * i4PerVerMove;
    }

    //last frame is specail
    if (pt_tag->stepIdx == pt_tag->stepNum)
    {
        if (direction == MTPHOTO4K_SCROLL_L2R)
        {
            tDispRegion.u4X = u4Width - u4FbDispWidth;
        }
        else if (direction == MTPHOTO4K_SCROLL_R2L)
        {
            tDispRegion.u4X = 0;
        }
        else if (direction == MTPHOTO4K_SCROLL_T2B)
        {
            tDispRegion.u4Y = u4Height - u4FbDispHeight;
        }
        else if (direction == MTPHOTO4K_SCROLL_B2T)
        {
            tDispRegion.u4Y = 0;
        }
    }

    printk("scroll position [left, top] = [%d, %d].\n", tDispRegion.u4X, tDispRegion.u4Y);
    VDP_SetRenderRegion(videoPath, &tDispRegion);

    pt_tag->stepIdx++;
}

INT32   _rotate_0_and_180(MTPHOTO4K_VDP_ROTATE_EX_T* pt_panorama_rotate)
{
    UINT8   u1Block, i;
    UINT32  u4DstBlockHSize = 0, u4DstBlockVSize = 0, u4SrcBlockHSize = 0, u4SrcBlockVSize = 0;
    BOOL    bHorizontal = FALSE;
    UINT32  u4BltSrcWidth = 0, u4BltSrcHeight = 0, u4BltDstWidth = 0, u4BltDstHeight = 0;
    MTPHOTO4K_STRETCH_BLIT_T    tBlt = {0};
    UINT8*                      pTmpBuffer = NULL;
    
    MTPHOTO4K_VDP_ROTATE_T      tRotate;
    UINT32                      u4FbDispWidth, u4FbDispHeight;
    UINT32                      u4RotateOp = _get_rotate_flag(pt_panorama_rotate->eRotate);

    if (_Panel_Is_4K2K())
    {
        u4FbDispWidth = B2R_WIDTH_4K2K;
        u4FbDispHeight = B2R_HEIGHT_4K2K;
    }
    else
    {
        u4FbDispWidth = B2R_WIDTH_FHD;
        u4FbDispHeight = B2R_HEIGHT_FHD;
    }
    
    if (pt_panorama_rotate->u4SrcWidth > u4FbDispWidth)
    {
        bHorizontal = TRUE;
    }
    
    pTmpBuffer = vmalloc(B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K * 2);
    if (pTmpBuffer == NULL)
    {
        printk("tmp buffer allocate fail.\n");
        return MTR_OUT_OF_MEMORY;
    }
    x_memset(pTmpBuffer, 0, B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K * 2);

    if (bHorizontal)
    {
        u1Block = (pt_panorama_rotate->u4SrcWidth + u4FbDispWidth - 1) / u4FbDispWidth;
        u4DstBlockHSize = pt_panorama_rotate->u4DstWidth / u1Block;
        u4SrcBlockHSize = pt_panorama_rotate->u4SrcWidth / u1Block;
    }
    else
    {
        u1Block = (pt_panorama_rotate->u4SrcHeight + u4FbDispHeight - 1) / u4FbDispHeight;
        u4DstBlockVSize = pt_panorama_rotate->u4DstHeight / u1Block;
        u4SrcBlockVSize = pt_panorama_rotate->u4SrcHeight / u1Block;
    }

    for (i = 0; i < u1Block; i++)
    {
        if (i == u1Block - 1)
        {
            if(bHorizontal)
            {
                u4BltSrcWidth = pt_panorama_rotate->u4SrcWidth - i * u4SrcBlockHSize;
                u4BltDstWidth = pt_panorama_rotate->u4DstWidth - i * u4DstBlockHSize;
            }
            else
            {
                u4BltSrcHeight = pt_panorama_rotate->u4SrcHeight - i * u4SrcBlockVSize;
                u4BltDstHeight = pt_panorama_rotate->u4DstHeight - i * u4DstBlockVSize;
            }
        }
        else
        {
            if(bHorizontal)
            {
                u4BltSrcWidth = u4SrcBlockHSize;
                u4BltDstWidth = u4DstBlockHSize;
            }
            else
            {
                u4BltSrcHeight = u4SrcBlockVSize;
                u4BltDstHeight = u4DstBlockVSize;
            }
        }

        tBlt.src_buffer_Y_addr = pt_panorama_rotate->u4Src;
        tBlt.src_buffer_C_addr = pt_panorama_rotate->u4SrcCbCrOffset + tBlt.src_buffer_Y_addr;
        tBlt.src_buffer_Y_width = 0;
        tBlt.src_buffer_Y_height = 0;
        tBlt.src_buffer_Y_pitch = pt_panorama_rotate->u4SrcPitch;
        tBlt.dest_buffer_Y_addr = pt_panorama_rotate->u4Dst;
        tBlt.dest_buffer_C_addr = tBlt.dest_buffer_Y_addr + pt_panorama_rotate->u4DstCbCrOffset;
        tBlt.dest_buffer_Y_width = 0;
        tBlt.dest_buffer_Y_height = 0;
        tBlt.dest_buffer_Y_pitch = pt_panorama_rotate->u4DstPitch;
        tBlt.src_clip_x = bHorizontal ? (i * u4SrcBlockHSize + pt_panorama_rotate->u4SrcX) : pt_panorama_rotate->u4SrcX;
        tBlt.src_clip_y = bHorizontal ? pt_panorama_rotate->u4SrcY : (i * u4SrcBlockVSize + pt_panorama_rotate->u4SrcY);
        tBlt.src_clip_w = bHorizontal ? u4BltSrcWidth : pt_panorama_rotate->u4SrcWidth;
        tBlt.src_clip_h = bHorizontal ? pt_panorama_rotate->u4SrcHeight : u4BltSrcHeight;
        if ((u4RotateOp & VDP_ROTATE_FLIP) && (u4RotateOp & VDP_ROTATE_180))
        {
            tBlt.dest_clip_x = bHorizontal ? (i * u4DstBlockHSize + pt_panorama_rotate->u4DstX) : 0;
        }
        else
        {
            tBlt.dest_clip_x = 0;
        }
        if (u4RotateOp & VDP_ROTATE_180)
        {
            tBlt.dest_clip_y = 0;
        }
        else
        {
            tBlt.dest_clip_y = bHorizontal ? 0 : (i * u4DstBlockVSize + pt_panorama_rotate->u4DstY);
        }
        
        tBlt.dest_clip_w = tBlt.src_clip_w;
        tBlt.dest_clip_h = tBlt.src_clip_h;
        _mt53_fb_vdp_stretchblit(&tBlt);
        
        
        tRotate.u4Src = tBlt.dest_buffer_Y_addr;
        tRotate.u4SrcCbCrOffset = pt_panorama_rotate->u4DstCbCrOffset;
        tRotate.u4SrcPitch = pt_panorama_rotate->u4DstPitch;
        tRotate.eSrcCm = pt_panorama_rotate->eSrcCm;
        tRotate.u4SrcX = tBlt.dest_clip_x;
        tRotate.u4SrcY = tBlt.dest_clip_y;
        tRotate.u4Dst = (UINT32)pTmpBuffer;
        tRotate.u4DstCbCrOffset = B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K;
        tRotate.u4DstPitch = B2R_WIDTH_4K2K;
        tRotate.eDstCm = pt_panorama_rotate->eDstCm;
        tRotate.u4DstX = 0;
        tRotate.u4DstY = 0;
        tRotate.u4Width = tBlt.dest_clip_w;
        tRotate.u4Height = tBlt.dest_clip_h;
        tRotate.u4RotateOp = u4RotateOp;
        _mt53fb_rotate(&tRotate);
        { 
            INT32 j;
            for (j = 0; j < tRotate.u4Height; j++)
            {
                x_memset((VOID*)(VIRTUAL(tRotate.u4Src)+(tRotate.u4SrcY+j)*tRotate.u4SrcPitch + tRotate.u4SrcX), 0, tRotate.u4Width);
                x_memset((VOID*)(VIRTUAL(tRotate.u4Src)+(tRotate.u4SrcY+j)*tRotate.u4SrcPitch+tRotate.u4SrcCbCrOffset + tRotate.u4SrcX), 0x80, tRotate.u4Width);
            }
            HalFlushInvalidateDCache();
        }

        tBlt.src_buffer_Y_addr = (UINT32)pTmpBuffer;
        tBlt.src_buffer_C_addr = (UINT32)pTmpBuffer + B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K;
        tBlt.src_buffer_Y_width = 0;
        tBlt.src_buffer_Y_height = 0;
        tBlt.src_buffer_Y_pitch = tRotate.u4DstPitch;
        tBlt.dest_buffer_Y_addr = pt_panorama_rotate->u4Dst;
        tBlt.dest_buffer_C_addr = tBlt.dest_buffer_Y_addr + pt_panorama_rotate->u4DstCbCrOffset;
        tBlt.dest_buffer_Y_width = 0;
        tBlt.dest_buffer_Y_height = 0;
        tBlt.dest_buffer_Y_pitch = pt_panorama_rotate->u4DstPitch;
        tBlt.src_clip_x = 0;
        tBlt.src_clip_y = 0;
        tBlt.src_clip_w = tRotate.u4Width;
        tBlt.src_clip_h = tRotate.u4Height;
        if ((u4RotateOp & VDP_ROTATE_FLIP) && (u4RotateOp & VDP_ROTATE_180))
        {
            tBlt.dest_clip_x = bHorizontal ? 
                        (pt_panorama_rotate->u4DstX + i * u4DstBlockHSize) : 
                        pt_panorama_rotate->u4DstX;
        }
        else
        {
            tBlt.dest_clip_x = bHorizontal ? 
                        (pt_panorama_rotate->u4DstWidth + pt_panorama_rotate->u4DstX - (i + 1) * u4DstBlockHSize) : 
                        pt_panorama_rotate->u4DstX;
        }
        if (u4RotateOp & VDP_ROTATE_180)
        {
            tBlt.dest_clip_y = bHorizontal ? 
                        pt_panorama_rotate->u4DstY : 
                        (pt_panorama_rotate->u4DstHeight + pt_panorama_rotate->u4DstY - (i + 1) * u4DstBlockVSize);
        }
        else
        {
            tBlt.dest_clip_y = bHorizontal ? 
                        pt_panorama_rotate->u4DstY : 
                        (pt_panorama_rotate->u4DstY + i * u4DstBlockVSize);
        }
        tBlt.dest_clip_w = bHorizontal ? u4BltDstWidth : pt_panorama_rotate->u4DstWidth;
        tBlt.dest_clip_h = bHorizontal ? pt_panorama_rotate->u4DstHeight : u4BltDstHeight;    

        if ((u4RotateOp & VDP_ROTATE_FLIP) && (u4RotateOp & VDP_ROTATE_180))
        {
            if ((i == u1Block - 1) && !bHorizontal)
            {
                tBlt.dest_clip_x = pt_panorama_rotate->u4DstX;
                tBlt.dest_clip_y = pt_panorama_rotate->u4DstY;
            }
        }
        else if (u4RotateOp & VDP_ROTATE_FLIP)
        {
            if ((i == u1Block - 1) && bHorizontal)
            {
                tBlt.dest_clip_x = pt_panorama_rotate->u4DstX;
                tBlt.dest_clip_y = pt_panorama_rotate->u4DstY;
            }
        }
        else
        {
            if (i == u1Block - 1)
            {
                tBlt.dest_clip_x = pt_panorama_rotate->u4DstX;
                tBlt.dest_clip_y = pt_panorama_rotate->u4DstY;
            }
        }
        _mt53_fb_vdp_stretchblit(&tBlt);
    }
    

    if (pTmpBuffer)
    {
        vfree(pTmpBuffer);
        pTmpBuffer = NULL;
    }

    return MTR_OK;
}

INT32   _rotate_90_and_270(MTPHOTO4K_VDP_ROTATE_EX_T* pt_panorama_rotate)
{
    UINT8   u1Block, i;
    UINT32  u4DstBlockHSize = 0, u4DstBlockVSize = 0, u4SrcBlockHSize = 0, u4SrcBlockVSize = 0;
    BOOL    bHorizontal = FALSE;
    UINT32  u4BltSrcWidth = 0, u4BltSrcHeight = 0, u4BltDstWidth =  0, u4BltDstHeight = 0;
    MTPHOTO4K_STRETCH_BLIT_T    tBlt = {0};
    UINT8*                      pTmpBuffer = NULL;
    
    MTPHOTO4K_VDP_ROTATE_T      tRotate;
    UINT32                      u4FbDispWidth, u4FbDispHeight;
    UINT32                      u4RotateOp = _get_rotate_flag(pt_panorama_rotate->eRotate);

    if (_Panel_Is_4K2K())
    {
        u4FbDispWidth = B2R_WIDTH_4K2K;
        u4FbDispHeight = B2R_HEIGHT_4K2K;
    }
    else
    {
        u4FbDispWidth = B2R_WIDTH_FHD;
        u4FbDispHeight = B2R_HEIGHT_FHD;
    }
    
    if (pt_panorama_rotate->u4SrcWidth > u4FbDispWidth)
    {
        bHorizontal = TRUE;
    }
    
    pTmpBuffer = vmalloc(B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K * 2);
    if (pTmpBuffer == NULL)
    {
        printk("tmp buffer allocate fail.\n");
        return MTR_OUT_OF_MEMORY;
    }
    x_memset(pTmpBuffer, 0, B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K * 2);

    if (bHorizontal)
    {
        u1Block = (pt_panorama_rotate->u4SrcWidth + u4FbDispHeight - 1) / u4FbDispHeight;
        u4DstBlockVSize = pt_panorama_rotate->u4DstHeight / u1Block;
        u4SrcBlockHSize = pt_panorama_rotate->u4SrcWidth / u1Block;
    }
    else
    {
        u1Block = (pt_panorama_rotate->u4SrcHeight + u4FbDispWidth - 1) / u4FbDispWidth;
        u4DstBlockHSize = pt_panorama_rotate->u4DstWidth / u1Block;
        u4SrcBlockVSize = pt_panorama_rotate->u4SrcHeight / u1Block;
    }
    for (i = 0; i < u1Block; i++)
    {
        if (i == u1Block - 1)
        {
            if(bHorizontal)
            {
                u4BltSrcWidth = pt_panorama_rotate->u4SrcWidth - i * u4SrcBlockHSize;
                u4BltDstHeight = pt_panorama_rotate->u4DstHeight - i * u4DstBlockVSize;
            }
            else
            {
                u4BltSrcHeight = pt_panorama_rotate->u4SrcHeight - i * u4SrcBlockVSize;
                u4BltDstWidth = pt_panorama_rotate->u4DstWidth - i * u4DstBlockHSize;
            }
        }
        else
        {
            if(bHorizontal)
            {
                u4BltSrcWidth = u4SrcBlockHSize;
                u4BltDstHeight = u4DstBlockVSize;
            }
            else
            {
                u4BltSrcHeight = u4SrcBlockVSize;
                u4BltDstWidth = u4DstBlockHSize;
            }
        }

        tBlt.src_buffer_Y_addr = pt_panorama_rotate->u4Src;
        tBlt.src_buffer_C_addr = pt_panorama_rotate->u4SrcCbCrOffset + tBlt.src_buffer_Y_addr;
        tBlt.src_buffer_Y_width = 0;
        tBlt.src_buffer_Y_height = 0;
        tBlt.src_buffer_Y_pitch = pt_panorama_rotate->u4SrcPitch;
        tBlt.dest_buffer_Y_addr = pt_panorama_rotate->u4Dst;
        tBlt.dest_buffer_C_addr = tBlt.dest_buffer_Y_addr + pt_panorama_rotate->u4DstCbCrOffset;
        tBlt.dest_buffer_Y_width = 0;
        tBlt.dest_buffer_Y_height = 0;
        tBlt.dest_buffer_Y_pitch = pt_panorama_rotate->u4DstPitch;
        tBlt.src_clip_x = bHorizontal ? (i * u4SrcBlockHSize + pt_panorama_rotate->u4SrcX) : pt_panorama_rotate->u4SrcX;
        tBlt.src_clip_y = bHorizontal ? pt_panorama_rotate->u4SrcY : (i * u4SrcBlockVSize + pt_panorama_rotate->u4SrcY);
        tBlt.src_clip_w = bHorizontal ? u4BltSrcWidth : pt_panorama_rotate->u4SrcWidth;
        tBlt.src_clip_h = bHorizontal ? pt_panorama_rotate->u4SrcHeight : u4BltSrcHeight;
        if (u4RotateOp & VDP_ROTATE_90)
        {
            tBlt.dest_clip_x = 0;
        }
        else
        {
            tBlt.dest_clip_x = bHorizontal ? 0 : (pt_panorama_rotate->u4DstX + i * u4DstBlockHSize);
        }
        if ((pt_panorama_rotate->eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90_WITH_FLIP) ||
            (pt_panorama_rotate->eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270))
        {
            tBlt.dest_clip_y = 0;
        }
        else
        {
            tBlt.dest_clip_y = bHorizontal ? (i * u4DstBlockVSize + pt_panorama_rotate->u4DstY): 0;
        }
        tBlt.dest_clip_w = tBlt.src_clip_w;
        tBlt.dest_clip_h = tBlt.src_clip_h;
        _mt53_fb_vdp_stretchblit(&tBlt);

        tRotate.u4Src = tBlt.dest_buffer_Y_addr;
        tRotate.u4SrcCbCrOffset = pt_panorama_rotate->u4DstCbCrOffset;
        tRotate.u4SrcPitch = pt_panorama_rotate->u4DstPitch;
        tRotate.eSrcCm = pt_panorama_rotate->eSrcCm;
        tRotate.u4SrcX = tBlt.dest_clip_x;
        tRotate.u4SrcY = tBlt.dest_clip_y;
        tRotate.u4Dst = (UINT32)pTmpBuffer;
        tRotate.u4DstCbCrOffset = B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K;
        tRotate.u4DstPitch = B2R_WIDTH_4K2K;
        tRotate.eDstCm = pt_panorama_rotate->eDstCm;
        tRotate.u4DstX = 0;
        tRotate.u4DstY = 0;
        tRotate.u4Width = tBlt.dest_clip_w;
        tRotate.u4Height = tBlt.dest_clip_h;
        tRotate.u4RotateOp = u4RotateOp;
        _mt53fb_rotate(&tRotate);

        if (bHorizontal)
        {
            GFX_Memset((UINT8*)(tRotate.u4Src+tRotate.u4SrcY*tRotate.u4SrcPitch),
                       tRotate.u4SrcPitch, tRotate.u4Height, 0);
            GFX_Memset((UINT8*)(tRotate.u4Src+tRotate.u4SrcY*tRotate.u4SrcPitch+tRotate.u4SrcCbCrOffset),
                       tRotate.u4SrcPitch, tRotate.u4Height, 0x80);
        }
        else if((u4RotateOp & VDP_ROTATE_90) && (i == u1Block - 1))
        { 
            INT32 j;
            for (j = 0; j < tRotate.u4Height; j++)
            {
                x_memset((VOID*)(VIRTUAL(tRotate.u4Src)+(tRotate.u4SrcY+j)*tRotate.u4SrcPitch), 0, tRotate.u4Width);
                x_memset((VOID*)(VIRTUAL(tRotate.u4Src)+(tRotate.u4SrcY+j)*tRotate.u4SrcPitch+tRotate.u4SrcCbCrOffset), 0x80, tRotate.u4Width);
            }
            HalFlushInvalidateDCache();
        }
        else if (u4RotateOp & VDP_ROTATE_270)
        {
            INT32 j;
            for (j = 0; j < tRotate.u4Height; j++)
            {
                x_memset((VOID*)(VIRTUAL(tRotate.u4Src)+(tRotate.u4SrcY+j)*tRotate.u4SrcPitch + tRotate.u4SrcX), 0, tRotate.u4Width);
                x_memset((VOID*)(VIRTUAL(tRotate.u4Src)+(tRotate.u4SrcY+j)*tRotate.u4SrcPitch+tRotate.u4SrcCbCrOffset + tRotate.u4SrcX), 0x80, tRotate.u4Width);
            }
            HalFlushInvalidateDCache();
        }

        tBlt.src_buffer_Y_addr = (UINT32)pTmpBuffer;
        tBlt.src_buffer_C_addr = (UINT32)pTmpBuffer + B2R_WIDTH_4K2K * B2R_HEIGHT_4K2K;
        tBlt.src_buffer_Y_width = 0;
        tBlt.src_buffer_Y_height = 0;
        tBlt.src_buffer_Y_pitch = tRotate.u4DstPitch;
        tBlt.dest_buffer_Y_addr = pt_panorama_rotate->u4Dst;
        tBlt.dest_buffer_C_addr = tBlt.dest_buffer_Y_addr + pt_panorama_rotate->u4DstCbCrOffset;
        tBlt.dest_buffer_Y_width = 0;
        tBlt.dest_buffer_Y_height = 0;
        tBlt.dest_buffer_Y_pitch = pt_panorama_rotate->u4DstPitch;
        tBlt.src_clip_x = 0;
        tBlt.src_clip_y = 0;
        tBlt.src_clip_w = tRotate.u4Height;
        tBlt.src_clip_h = tRotate.u4Width;
        if (u4RotateOp & VDP_ROTATE_90)
        {
            tBlt.dest_clip_x = bHorizontal ? 
                        pt_panorama_rotate->u4DstX : 
                        (pt_panorama_rotate->u4DstWidth + pt_panorama_rotate->u4DstX - (i+1)*u4DstBlockHSize);
        }
        else
        {
            tBlt.dest_clip_x = bHorizontal ? 
                        pt_panorama_rotate->u4DstX : 
                        (pt_panorama_rotate->u4DstX + i * u4DstBlockHSize);
        }
        if ((pt_panorama_rotate->eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90_WITH_FLIP) ||
            (pt_panorama_rotate->eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270))
        {
            tBlt.dest_clip_y = bHorizontal ? 
                        (pt_panorama_rotate->u4DstHeight + pt_panorama_rotate->u4DstY - (i+1) * u4DstBlockVSize) : 
                        pt_panorama_rotate->u4DstY;
        }
        else
        {
            tBlt.dest_clip_y = bHorizontal ? 
                        (pt_panorama_rotate->u4DstY + i * u4DstBlockVSize) : 
                        pt_panorama_rotate->u4DstY;
        }
        tBlt.dest_clip_w = bHorizontal ? pt_panorama_rotate->u4DstWidth : u4BltDstWidth;
        tBlt.dest_clip_h = bHorizontal ? u4BltDstHeight : pt_panorama_rotate->u4DstHeight;    

        if (u4RotateOp & VDP_ROTATE_90)
        {
            if ((i == u1Block - 1) && !bHorizontal)
            {
                tBlt.dest_clip_x = pt_panorama_rotate->u4DstX;
            }
        }
        if ((pt_panorama_rotate->eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90_WITH_FLIP) ||
            (pt_panorama_rotate->eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270))
        {
            if ((i == u1Block - 1) && bHorizontal)
            {
                tBlt.dest_clip_y = pt_panorama_rotate->u4DstY;
            }
        }
        _mt53_fb_vdp_stretchblit(&tBlt);
    }
    if (pTmpBuffer)
    {
        vfree(pTmpBuffer);
        pTmpBuffer = NULL;
    }

    return MTR_OK;
}

static INT32 _panorama_rotate(MTPHOTO4K_VDP_ROTATE_EX_T* pt_panorama_rotate)
{
    MTPHOTO4K_ROTATION_T        eRotate = pt_panorama_rotate->eRotate;
    MTPHOTO4K_STRETCH_BLIT_T    tBlt = {0};

    if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_0)
    {
        tBlt.src_buffer_Y_addr = pt_panorama_rotate->u4Src;
        tBlt.src_buffer_C_addr = pt_panorama_rotate->u4Src + pt_panorama_rotate->u4SrcCbCrOffset;
        tBlt.src_buffer_Y_width = 0;
        tBlt.src_buffer_Y_height = 0;
        tBlt.src_buffer_Y_pitch = pt_panorama_rotate->u4SrcPitch;

        tBlt.dest_buffer_Y_addr = pt_panorama_rotate->u4Dst;
        tBlt.dest_buffer_C_addr = pt_panorama_rotate->u4Dst + pt_panorama_rotate->u4DstCbCrOffset;
        tBlt.dest_buffer_Y_width = 0;
        tBlt.dest_buffer_Y_height = 0;
        tBlt.dest_buffer_Y_pitch = pt_panorama_rotate->u4DstPitch;

        tBlt.src_clip_x = pt_panorama_rotate->u4SrcX;
        tBlt.src_clip_y = pt_panorama_rotate->u4SrcY;
        tBlt.src_clip_w = pt_panorama_rotate->u4SrcWidth;
        tBlt.src_clip_h = pt_panorama_rotate->u4SrcHeight;

        tBlt.dest_clip_x = pt_panorama_rotate->u4DstX;
        tBlt.dest_clip_y = pt_panorama_rotate->u4DstY;
        tBlt.dest_clip_w = pt_panorama_rotate->u4DstWidth;
        tBlt.dest_clip_h = pt_panorama_rotate->u4DstHeight;
        
        return _mt53_fb_vdp_stretchblit(&tBlt);
    }
    else if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_180 ||
            eRotate == MTPHOTO4K_ROTATION_CLOCKWS_0_WITH_FLIP ||
            eRotate == MTPHOTO4K_ROTATION_CLOCKWS_180_WITH_FLIP)
    {
        return _rotate_0_and_180(pt_panorama_rotate);
    }
    
    else if (eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90 ||
            eRotate == MTPHOTO4K_ROTATION_CLOCKWS_90_WITH_FLIP ||
            eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270 ||
            eRotate == MTPHOTO4K_ROTATION_CLOCKWS_270_WITH_FLIP)
    {
        return _rotate_90_and_270(pt_panorama_rotate);
    }

    return MTR_OK;
}
static INT32 _MTPHOTO4K_StartScroll(UINT32 u4Arg)
{

    MTAL_IOCTL_9ARG_T               rArg;
    MTPHOTO4K_DISPLAY_REGION_T      stDispRegion, stSrcRegion;
    MTPHOTO4K_DISPLAY_REGION_T      *pDispRegion, *pSrcRegion;
    UINT8*                          pBufferAddr;
    UINT32                          u4BufferPitch, u4BufferSize;
    UINT8                           u1ByteCount = 2, u1VideoPath;
    UINT32                          y_addr, c_addr;
    UCHAR                           ucFbgId, ucFbId;
    FBM_PIC_HDR_T*                  prFbmPicHdr; 
    MTPHOTO4K_SCROLL_DIRECTION      eDirection;
    UINT32                          u4Duration;
    UINT32                          u4FbWidth, u4FbHeight;
    MT_RESULT_T                     ret;
    VDP_REGION_T                    rRegion = {0, 0, 10000, 10000};
    VDP_OVERSCAN_REGION_T           rORegion = {0, 0, 0, 0};
    UINT32                          u4FbDispWidth, u4FbDispHeight;
    
    USR_SPACE_ACCESS_VALIDATE_9ARG(u4Arg);
    COPY_FROM_USER_9ARG(u4Arg,rArg);

    u1VideoPath = rArg.ai4Arg[0];
    pBufferAddr = (UINT8*)(rArg.ai4Arg[1]);
    u4BufferSize = rArg.ai4Arg[2];
    u4BufferPitch = rArg.ai4Arg[3];
    
    pDispRegion = (MTPHOTO4K_DISPLAY_REGION_T*)rArg.ai4Arg[5];
    USR_SPACE_ACCESS_VALIDATE_ARG(pDispRegion, MTPHOTO4K_DISPLAY_REGION_T);
    COPY_FROM_USER_ARG(pDispRegion, stDispRegion, MTPHOTO4K_DISPLAY_REGION_T);

    pSrcRegion = (MTPHOTO4K_DISPLAY_REGION_T*)rArg.ai4Arg[4];
    USR_SPACE_ACCESS_VALIDATE_ARG(pSrcRegion, MTPHOTO4K_DISPLAY_REGION_T);
    COPY_FROM_USER_ARG(pSrcRegion, stSrcRegion, MTPHOTO4K_DISPLAY_REGION_T);

    u4Duration = rArg.ai4Arg[6];
    eDirection = rArg.ai4Arg[7];

    if (_Panel_Is_4K2K())
    {
        u4FbDispWidth = B2R_WIDTH_4K2K;
        u4FbDispHeight = B2R_HEIGHT_4K2K;
    }
    else
    {
        u4FbDispWidth = B2R_WIDTH_FHD;
        u4FbDispHeight = B2R_HEIGHT_FHD;
    }
    
    if(u1ByteCount*stDispRegion.u4Width*stDispRegion.u4Height > (2*3840*16380))
    {
        MTPHOTO4K_PRINT("[%s] Buffer size over frame buffer  !!\n", __FUNCTION__);
        return MTR_ERR_INV;
    }

    if (stDispRegion.u4Width <= u4FbDispWidth && 
        ((eDirection == MTPHOTO4K_SCROLL_L2R) || (eDirection == MTPHOTO4K_SCROLL_R2L)))
    {
        MTPHOTO4K_PRINT("The width is small, cannot do L2R or R2L scroll.\n");
        return MTR_ERR_INV;
    }

    if (stDispRegion.u4Height <= u4FbDispHeight && 
        ((eDirection == MTPHOTO4K_SCROLL_T2B) || (eDirection == MTPHOTO4K_SCROLL_B2T)))
    {
        MTPHOTO4K_PRINT("The height is small, cannot do T2B or B2T scroll.\n");
        return MTR_ERR_INV;
    }

    //X offset should be even for 422
    stDestInfo.u32X= stDispRegion.u4x & (~1U);
    stDestInfo.u32Y= stDispRegion.u4y;
    stDestInfo.u32Width= stDispRegion.u4Width;
    stDestInfo.u32Height= stDispRegion.u4Height;

    stSourceInfo.u32X = stSrcRegion.u4x & (~1U);
    stSourceInfo.u32Y = stSrcRegion.u4y;
    stSourceInfo.u32Width = stSrcRegion.u4Width;
    stSourceInfo.u32Height = stSrcRegion.u4Height;
    stSourceInfo.u32PhotoAddress = (UINT32)pBufferAddr;
    stSourceInfo.u32PhotoSize = u4BufferSize;
    stSourceInfo.u32PhotoPitch = u4BufferPitch;
    stSourceInfo.u8VideoPath = u1VideoPath;

    if (stDestInfo.u32Width>u4FbDispWidth)
    {
        u4FbWidth = ((stDestInfo.u32Width + 15) >> 4) << 4; //align 16
    }
    else
    {
        u4FbWidth = u4FbDispWidth;
    }
    if (stDestInfo.u32Height>u4FbDispHeight)
    {
        u4FbHeight = ((stDestInfo.u32Height + 3) >> 2) << 2;//align 4
    }
    else
    {
        u4FbHeight = u4FbDispHeight;
    }

    if (g_fbg_id != FBM_FBG_ID_UNKNOWN)
    {
        FBM_ReleaseGroup(g_fbg_id);
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, 0);
        g_fbg_id = FBM_FBG_ID_UNKNOWN;
    }

    ret = _mt53_fb_b2r_install(FBM_VDEC_JPEG, u4FbDispWidth, u4FbDispHeight, u4FbWidth);
    if (ret != MTR_OK)
    {
        MTPHOTO4K_PRINT("[%s] _mt53_fb_b2r_install fail !!\n", __FUNCTION__);
        return ret;
    }

    if (B2R_OneFbChgTrigger(u1VideoPath) == FALSE)
    {
        MTPHOTO4K_PRINT("[%s] B2R_OneFbChgTrigger fail !!\n", __FUNCTION__);
        return MTR_FBM_ERROR;
    }

    ucFbgId = g_fbg_scroll_id;
    MTPHOTO4K_PRINT("try FBM_GetEmptyFrameBuffer.\n");
    do
    {
        ucFbId = FBM_GetEmptyFrameBuffer(ucFbgId, (UINT32)1500);   
    }while(ucFbId == FBM_FB_ID_UNKNOWN);

    x_thread_delay(35);
    _mt53_fb_b2r_setPitch(u4FbDispWidth, u4FbDispHeight, u4FbWidth);

    VDP_SetSrcRegion(u1VideoPath, 1, rRegion); 
    VDP_SetOutRegion(u1VideoPath, 1, rRegion); 
    VDP_SetOverScan(u1VideoPath,  rORegion);
    
    MTPHOTO4K_PRINT("FBM_GetEmptyFrameBuffer ok.\n");
             
    FBM_GetFrameBufferAddr(ucFbgId, ucFbId, &y_addr, &c_addr);
    stDestInfo.u32DestAddrY = (UINT32)VIRTUAL(y_addr);
    stDestInfo.u32DestAddrU = (UINT32)VIRTUAL(c_addr); 

    MTPHOTO4K_PRINT("clean buffer.\n");
    {
        INT32 i = 0, step = 0;
        UINT32  height = GFX_HW_MAX_HEIGHT;

        step = (u4FbHeight + GFX_HW_MAX_HEIGHT -1) / GFX_HW_MAX_HEIGHT;

        for (i = 0; i < step; i++)
        {
            if (i == step - 1)
            {
                height = u4FbHeight - i * GFX_HW_MAX_HEIGHT;
            }
            GFX_Memset((UINT8*)y_addr+i*u4FbWidth*GFX_HW_MAX_HEIGHT, u4FbWidth, height, 0);
            GFX_Memset((UINT8*)c_addr+i*u4FbWidth*GFX_HW_MAX_HEIGHT, u4FbWidth, height, 0x80);   
        }
    }

    {
        MTPHOTO4K_VDP_ROTATE_EX_T tRotate = {0};

        tRotate.u4Src = (unsigned int)pBufferAddr;
        tRotate.u4SrcCbCrOffset = u4BufferSize/2;
        tRotate.eSrcCm = CM_YCbYCr422_DIRECT16;
        tRotate.u4SrcPitch = u4BufferPitch;
        tRotate.u4SrcX = stSourceInfo.u32X;
        tRotate.u4SrcY = stSourceInfo.u32Y;
        tRotate.u4SrcWidth = stSourceInfo.u32Width;
        tRotate.u4SrcHeight = stSourceInfo.u32Height;

        tRotate.u4Dst = y_addr;
        tRotate.u4DstCbCrOffset = c_addr - y_addr;
        tRotate.eDstCm = CM_YCbYCr422_DIRECT16;
        tRotate.u4DstPitch = u4FbWidth;
        tRotate.u4DstX = stDestInfo.u32X;
        tRotate.u4DstY = stDestInfo.u32Y;
        tRotate.u4DstWidth = stDestInfo.u32Width;
        tRotate.u4DstHeight = stDestInfo.u32Height;

        tRotate.eRotate = rArg.ai4Arg[8];
        
        _panorama_rotate(&tRotate);
    } 
  
    prFbmPicHdr = FBM_GetFrameBufferPicHdr(ucFbgId, ucFbId);
    
    if (prFbmPicHdr == NULL)
    {
         MTPHOTO4K_PRINT("[%s] FBM_GetFrameBufferPicHdr failed.\n", __FUNCTION__);
         return MTR_FBM_ERROR;
    }
            
    prFbmPicHdr->ucPicStruct = (UCHAR)MPEG_PIC_FRAME;
    prFbmPicHdr->fgProgressiveFrm = TRUE;
    prFbmPicHdr->fgRepFirstFld = FALSE;
    prFbmPicHdr->fgTopFldFirst = FALSE;
    
    FBM_UpdateReferenceList(ucFbgId, ucFbId, FALSE); //default  
    FBM_SetFrameBufferStatus(ucFbgId, ucFbId, (UCHAR)FBM_FB_STATUS_READY); 

    g_fb_id[B2R_CURRENT_ID] = ucFbId;

    // Display
    FBM_SetFrameBufferStatus(g_fbg_scroll_id, g_fb_id[B2R_CURRENT_ID], (UCHAR)FBM_FB_STATUS_DISPLAYQ);
    VDP_SetMode(u1VideoPath, VDP_MODE_NORMAL);

    t_scroll_tag.videoPath = u1VideoPath;
    t_scroll_tag.stepNum = (u4Duration*30/1000); //30hz
    t_scroll_tag.direction = eDirection;
    t_scroll_tag.displayWidth = stDestInfo.u32Width;
    t_scroll_tag.displayHeight = stDestInfo.u32Height;
    t_scroll_tag.stepIdx  = 0;

    x_sema_lock(hSema, X_SEMA_OPTION_WAIT);
    VDP_VsyncCBRegister(t_scroll_tag.videoPath, _b2r_scroll_callback, (VOID*)(&t_scroll_tag));

    return MTR_OK;
}


static INT32 _MTPHOTO4K_PauseScroll(UINT32 u4Arg)
{
    bPause = TRUE;
    x_sema_lock(hSema, X_SEMA_OPTION_WAIT);
    x_sema_unlock(hSema);
    bPause = FALSE;
    return MTR_OK;
}

static INT32 _MTPHOTO4K_RestartScroll(UINT32 u4Arg)
{
    if (t_scroll_tag.stepIdx > t_scroll_tag.stepNum)
    {
        return MTR_NOT_OK;
    }

    x_sema_lock(hSema, X_SEMA_OPTION_WAIT);
    VDP_VsyncCBRegister(t_scroll_tag.videoPath, _b2r_scroll_callback, (VOID*)(&t_scroll_tag));
    return MTR_OK;
}

static INT32 _MTPHOTO4K_VdpAllocate(UINT32 u4Arg)
{
    MTAL_IOCTL_4ARG_T       rArg;
    UINT32                  ui4_width, ui4_height;
    BOOL                    bDoubleBuffer;
    FBM_CREATE_FBG_PAR_T    rPar;
    UCHAR                   fbg_type, fbg_id, fb_id;
    UINT32                  y_addr, c_addr;
    
    USR_SPACE_ACCESS_VALIDATE_4ARG(u4Arg);
    COPY_FROM_USER_4ARG(u4Arg,rArg);

    ui4_width       = rArg.ai4Arg[0];
    ui4_height      = rArg.ai4Arg[1];
    bDoubleBuffer   = rArg.ai4Arg[2];

    if (g_VdpBuffer[0].fbg_id == 0xFF)
    {
        x_memset(&rPar, 0, sizeof(FBM_CREATE_FBG_PAR_T));
        rPar.u1VdecId       = (UINT8)0;
        rPar.u1AppMode      = FBM_FBG_APP_MTIMAGE;
        rPar.fgExternalAddr = FALSE;
        rPar.u1RequestFbNs  = VDP_FB_NUM;

        fbg_type = FBM_SelectGroupType(B2R_WIDTH_FHD, B2R_HEIGHT_FHD);   
          
        FBM_SetColorMode(FBM_CM_422);
        fbg_id = FBM_CreateGroupExt(fbg_type, FBM_VDEC_JPEG, ui4_width, ui4_height, &rPar);
        
        if(fbg_id == FBM_FBG_ID_UNKNOWN)
        {
            MTPHOTO4K_PRINT("[%s] FBM_Create_Group %d x %d Fail \n", __FUNCTION__, ui4_width, ui4_height);
            return MTR_FBM_ERROR;
        }
        g_VdpBuffer[0].fbg_id = fbg_id;
    }

    fb_id = FBM_GetEmptyFrameBuffer(g_VdpBuffer[0].fbg_id, (UINT32)1500);
    if (fb_id == FBM_FB_ID_UNKNOWN) 
    {
        return MTR_FBM_ERROR;
    }
    if (fb_id >= VDP_FB_NUM)
    {
        MTPHOTO4K_PRINT("[%s] fb_id=%d is overflow.\n", __FUNCTION__, fb_id);
        return MTR_FBM_ERROR;
    }
               
    FBM_GetFrameBufferAddr(g_VdpBuffer[0].fbg_id, fb_id, &y_addr, &c_addr);
    FBM_SetFrameBufferStatus(g_VdpBuffer[0].fbg_id, fb_id, (UCHAR)FBM_FB_STATUS_READY); 

    g_VdpBuffer[fb_id].fbg_id   = g_VdpBuffer[0].fbg_id;
    g_VdpBuffer[fb_id].fb_id    = fb_id;
    g_VdpBuffer[fb_id].width    = ui4_width;
    g_VdpBuffer[fb_id].height   = ui4_height;
    g_VdpBuffer[fb_id].y_addr   = y_addr;
    g_VdpBuffer[fb_id].c_addr   = c_addr;

    rArg.ai4Arg[3] = fb_id + VDP_ID_OFFSET;

    printk("allocate buffer id : %d.\n", rArg.ai4Arg[3]);

    USR_SPACE_ACCESS_VALIDATE_ARG(u4Arg, MTAL_IOCTL_4ARG_T);
    COPY_TO_USER_ARG(u4Arg, rArg, MTAL_IOCTL_4ARG_T); 

    return MTR_OK;
}

static INT32 _MTPHOTO4K_VdpDeAllocate(UINT32 u4Arg)
{
    MTAL_IOCTL_2ARG_T       rArg;
    UINT32                  ui4_buf_id;
    UCHAR                   fb_id, i;
    
    USR_SPACE_ACCESS_VALIDATE_2ARG(u4Arg);
    COPY_FROM_USER_2ARG(u4Arg,rArg);

    ui4_buf_id = rArg.ai4Arg[0];
    fb_id = ui4_buf_id - VDP_ID_OFFSET;

    if (fb_id >= VDP_FB_NUM)
    {
        MTPHOTO4K_PRINT("[%s] fb_id=%d is overflow.\n", __FUNCTION__, fb_id);
        return MTR_ERR_INV;
    }

    if (g_VdpBuffer[fb_id].fbg_id == 0xFF || g_VdpBuffer[fb_id].fb_id == 0xFF)
    {
        return MTR_OK;
    }

    g_VdpBuffer[fb_id].fb_id = 0xFF;

    for (i = 0; i < VDP_FB_NUM; i++)
    {
        if (g_VdpBuffer[i].fb_id != 0xFF)
        {
            break;
        }
    }

    if (i == VDP_FB_NUM)
    {
        FBM_ReleaseGroup(g_VdpBuffer[fb_id].fbg_id);
        FBM_FbgChgNotify(FBM_FBG_ID_UNKNOWN, 0);
        for (i = 0; i < VDP_FB_NUM; i++)
        {
            g_VdpBuffer[i].fbg_id = 0xFF;
        }
    }

    return MTR_OK;
}

static INT32 _MTPHOTO4K_VdpGetBufInfo(UINT32 u4Arg)
{
    MTAL_IOCTL_7ARG_T       rArg;
    UINT32                  ui4_buf_id;
    UCHAR                   fb_id;
    
    USR_SPACE_ACCESS_VALIDATE_7ARG(u4Arg);
    COPY_FROM_USER_7ARG(u4Arg,rArg);

    ui4_buf_id = rArg.ai4Arg[0];
    fb_id = ui4_buf_id - VDP_ID_OFFSET;

    if (fb_id >= VDP_FB_NUM)
    {
        MTPHOTO4K_PRINT("[%s] fb_id=%d is overflow.\n", __FUNCTION__, fb_id);
        return MTR_ERR_INV;
    }

    if (g_VdpBuffer[fb_id].fbg_id == 0xFF ||
        g_VdpBuffer[fb_id].fb_id != fb_id)
    {
        return MTR_NOT_OK;
    }

    rArg.ai4Arg[1] = g_VdpBuffer[fb_id].width;
    rArg.ai4Arg[2] = g_VdpBuffer[fb_id].height;
    rArg.ai4Arg[3] = g_VdpBuffer[fb_id].y_virt_addr;
    rArg.ai4Arg[4] = g_VdpBuffer[fb_id].c_virt_addr;
    rArg.ai4Arg[5] = g_VdpBuffer[fb_id].y_addr;
    rArg.ai4Arg[6] = g_VdpBuffer[fb_id].c_addr;

    USR_SPACE_ACCESS_VALIDATE_ARG(u4Arg, MTAL_IOCTL_7ARG_T);
    COPY_TO_USER_ARG(u4Arg, rArg, MTAL_IOCTL_7ARG_T); 

    return MTR_OK;
}

static INT32 _MTPHOTO4K_VdpSetBufInfo(UINT32 u4Arg)
{
    MTAL_IOCTL_3ARG_T       rArg;
    UINT32                  ui4_buf_id;
    UCHAR                   fb_id;
    
    USR_SPACE_ACCESS_VALIDATE_3ARG(u4Arg);
    COPY_FROM_USER_3ARG(u4Arg,rArg);

    ui4_buf_id = rArg.ai4Arg[0];
    fb_id = ui4_buf_id - VDP_ID_OFFSET;

    if (fb_id >= VDP_FB_NUM)
    {
        MTPHOTO4K_PRINT("[%s] fb_id=%d is overflow.\n", __FUNCTION__, fb_id);
        return MTR_ERR_INV;
    }

    g_VdpBuffer[fb_id].y_virt_addr = rArg.ai4Arg[1];
    g_VdpBuffer[fb_id].c_virt_addr = rArg.ai4Arg[2];

    return MTR_OK;
}

static INT32 _MTPHOTO4K_Is4KPanel(UINT32 u4Arg)
{
    MTAL_IOCTL_2ARG_T   rArg;
    BOOL                b4k2k = FALSE;

    USR_SPACE_ACCESS_VALIDATE_2ARG(u4Arg);
    COPY_FROM_USER_2ARG(u4Arg,rArg);
    
    b4k2k = _Panel_Is_4K2K();

    rArg.ai4Arg[0] = b4k2k;

    USR_SPACE_ACCESS_VALIDATE_ARG(u4Arg, MTAL_IOCTL_2ARG_T);
    COPY_TO_USER_ARG(u4Arg, rArg, MTAL_IOCTL_2ARG_T); 

    return MTR_OK;
}


int mtal_ioctl_mtphoto4k(struct inode *inode, struct file *file,
    unsigned int cmd, unsigned long arg)
{
    int result = MTR_OK;
    
    if ( hMtPhoto4kSemaHandle == (HANDLE_T)NULL )
    {
        if ( x_sema_create(&hMtPhoto4kSemaHandle, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) != 0 )
        {
            return MTR_NOT_OK;
        }
    }

    x_sema_lock(hMtPhoto4kSemaHandle, X_SEMA_OPTION_WAIT);

    switch (cmd) 
    {
        case MTAL_IO_PHOTO4K_INIT:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_INIT begin\n");
            result = _MTPHOTO4K_Init(arg);
            break;
        case MTAL_IO_PHOTO4K_SET_DISPLAY_INFO:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_SET_DISPLAY_INFO begin\n");
            result = _MTPHOTO4K_SetDisplayInfo(arg);
            break;
        case MTAL_IO_PHOTO4K_SHOW:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_SHOW begin\n");
            result = _MTPHOTO4K_Show(arg);
            break;
        case MTAL_IO_PHOTO4K_DEINIT:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_DEINIT begin\n");
            result = _MTPHOTO4K_DeInit();
            break;      
        case MTAL_IO_PHOTO4K_START_FADE_EFFECT:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_START_FADE_EFFECT begin\n");
            result = _MTPHOTO4K_StartFade(arg);
            break;
        case MTAL_IO_PHOTO4K_PREPARE:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_PREPARE begin\n");
            result = _MTPHOTO4K_Prepare(arg);
            break;
        case MTAL_IO_PHOTO4K_PAUSE_FADE_EFFECT:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_PAUSE_FADE_EFFECT begin\n");
            result = _MTPHOTO4K_PauseFade(arg);
            break;
        case MTAL_IO_PHOTO4K_RESTART_FADE_EFFECT:
            MTPHOTO4K_PRINT("invoke MTAL_IO_MTPHOTO4K_RESTART_FADE_EFFECT begin\n");
            result = _MTPHOTO4K_RestartFade(arg);
            break;
        case MTAL_IO_PHOTO4K_START_PANORAMA_SCROLL:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_START_PANORAMA_SCROLL begin\n");
            result = _MTPHOTO4K_StartScroll(arg);
            break;
        case MTAL_IO_PHOTO4K_PAUSE_PANORAMA_SCROLL:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_PAUSE_PANORAMA_SCROLL begin\n");
            result = _MTPHOTO4K_PauseScroll(arg);
            break;
        case MTAL_IO_PHOTO4K_RESTART_PANORAMA_SCROLL:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_RESTART_PANORAMA_SCROLL begin\n");
            result = _MTPHOTO4K_RestartScroll(arg);
            break;
        case MTAL_IO_PHOTO4K_VDP_ALLOCATE:
            result = _MTPHOTO4K_VdpAllocate(arg);
            break;
        case MTAL_IO_PHOTO4K_VDP_DEALLOCATE:
            result = _MTPHOTO4K_VdpDeAllocate(arg);
            break;
        case MTAL_IO_PHOTO4K_VDP_GETINFO:
            result = _MTPHOTO4K_VdpGetBufInfo(arg);
            break;
        case MTAL_IO_PHOTO4K_VDP_SETINFO:
            result = _MTPHOTO4K_VdpSetBufInfo(arg);
            break;
        case MTAL_IO_PHOTO4K_IS_4K_PANEL:
            MTPHOTO4K_PRINT("invoke MTAL_IO_PHOTO4K_IS_4K_PANEL begin\n");
            result = _MTPHOTO4K_Is4KPanel(arg);
            break;
        default:
            result = MTR_NOT_OK;
            break;
    }

    x_sema_unlock(hMtPhoto4kSemaHandle);
    
    return result;
}


