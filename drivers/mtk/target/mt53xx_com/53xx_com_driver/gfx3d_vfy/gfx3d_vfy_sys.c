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

#ifndef __DRV_GFX3D_VFY_SYS_C
#define __DRV_GFX3D_VFY_SYS_C

/*-----------------------------------------------------------------------------
                    include files
-----------------------------------------------------------------------------*/
#include "gfx3d_vfy_def.h"
#include <linux/string.h>
#include <linux/file.h>

#include "x_hal_5381.h"

#include "x_drv_cli.h"
#include "x_serial.h"
#include "x_printf.h"
#include "x_os.h"

#include "x_assert.h"
#include "x_util.h"
#include "x_stl_lib.h"
#include "x_common.h"
#include "x_bim.h"
#include "x_rand.h"
#include "drvcust_if.h"

#include "gfx3d_vfy_def.h"
#include "gfx3d_vfy_if.h"
#include "gfx3d_vfy_sys.h"
//#include "x_drv_map.h"

#include "x_hal_926.h"
#include "x_typedef.h"
#include "x_printf.h"
#include "x_debug.h"

#include "x_kmem.h"

#include <linux/fs.h>
#include "x_fm.h"
#include <asm/unistd.h>


#include <asm/uaccess.h>
#include "linux/syscalls.h"
#include "linux/module.h"


/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

UINT32 u4ReadBimReg(UINT32 u4VDecID, UINT32 u4Addr)
{
    return (u4ReadReg(BIM_REG_OFFSET + u4Addr));
}

void vWriteBimReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    vWriteReg(BIM_REG_OFFSET + u4Addr, u4Val);
}

void vMaliWriteReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    vWriteReg(MALI_REG_OFFSET + u4Addr, u4Val);
}

UINT32 u4MaliReadReg(UINT32 u4VDecID, UINT32 u4Addr)
{
    return (u4ReadReg(MALI_REG_OFFSET + u4Addr));
}


void vMaliGPWriteReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    vMaliWriteReg(u4VDecID, MALI_REG_GP_OFFSET + u4Addr, u4Val);
}

UINT32 u4MaliGPReadReg(UINT32 u4VDecID, UINT32 u4Addr)
{
    return (u4MaliReadReg(u4VDecID, MALI_REG_GP_OFFSET + u4Addr));
}

void vMaliPPWriteReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    vMaliWriteReg(u4VDecID, MALI_REG_PP_OFFSET + u4Addr, u4Val);
}

UINT32 u4MaliPPReadReg(UINT32 u4VDecID, UINT32 u4Addr)
{
    return (u4MaliReadReg(u4VDecID, MALI_REG_PP_OFFSET + u4Addr));
}

void vMaliAXIWriteReg(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    vMaliWriteReg(u4VDecID, MALI_REG_AXI_OFFSET + u4Addr, u4Val);
}

UINT32 u4MaliAXIReadReg(UINT32 u4VDecID, UINT32 u4Addr)
{
    return (u4MaliReadReg(u4VDecID, MALI_REG_AXI_OFFSET + u4Addr));
}

void vMaliWriteMem(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val)
{
    vWriteMem(u4Addr, u4Val);
}

UINT32 u4MaliReadMem(UINT32 u4VDecID, UINT32 u4Addr)
{
    return (u4ReadMem(u4Addr));
}


#if 0
//for loading data from USB
INT32 i4Gfx3DVfyReadFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
    INT32 fp;
    INT32 i4RealReadSize;

    if((DrvFSG3DUSBOpenFile(pcFileName,0, &fp)) == 0)
    {
        i4RealReadSize = DrvFSG3DUSBReadFile(fp,(void *)u4FileBufSa,u4BufSize);

        DrvFSG3DUSBCloseFile(fp);
    } 
	else
    {
        i4RealReadSize = -1;
    }

    return i4RealReadSize;
}

INT32 i4Gfx3DVfyWriteFile(char *pcFileName,UINT32 u4FileBufSa,UINT32 u4BufSize)
{
    INT32 fp;
    INT32 i4RealReadSize;

    if((DrvFSG3DUSBOpenFile(pcFileName,1, &fp)) == 0)
    {
        i4RealReadSize = DrvFSG3DUSBWriteFile(fp, (void *)u4FileBufSa,u4BufSize);

        DrvFSG3DUSBCloseFile(fp);
    } 
	else
    {
        i4RealReadSize = -1;
    }

    return i4RealReadSize;
}


INT32 i4Gfx3DVfyFopen(char *pcFileName,char *pcMode)
{
    INT32 fp;
    INT32 u4OpenMode;
    if(*pcMode == 'w')
    {
       u4OpenMode = 1;
    }
	else
    {
       u4OpenMode = 0;
    }
    DrvFSG3DUSBOpenFile(pcFileName,u4OpenMode, &fp);

    return fp;
}


char _abGfx3DPrintMsg[256];
INT32 i4Gfx3DVfyFprintf(INT32 i4FileHandle,const char *format, ...)
{
	sprintf(_abGfx3DPrintMsg,format);
	DrvFSG3DUSBWriteFile(i4FileHandle, (void *)_abGfx3DPrintMsg,strlen(_abGfx3DPrintMsg));
	return 0;
}


INT32 i4Gfx3DVfyFclose(INT32 i4FileHandle)
{
    return DrvFSG3DUSBCloseFile(i4FileHandle);
}
#endif


#if USB_G3D_VFY

#if 0
#define PATH_LEN 20
static char _strUSBPath[6][PATH_LEN] =
{  
	{"/mnt/sda1/"},  
	{"/mnt/sda/"},   
	{"/mnt/sdb1/"},	 
	{"/mnt/sdb/"},	 
	{"/mnt/sdc1/"},	 
	{"/mnt/sdc/"}	 
}
#endif

#if 1
#define DRV_FSR_SUCCESS                0
#define DRV_FSR_FAIL                  -1
#define DRV_FSR_NULL_POINT            -2
#define DRV_FSR_BUF_ADDR_ALIGN_ERR    -3
#define DRV_FSR_ONLY_SUPPORT_RDONLY   -4
#define DRV_FSR_PARAMETER_ERR         -5

#define PSEEK_SET   0   /* offset from begining of file*/
#define PSEEK_CUR   1   /* offset from current file pointer*/
#define PSEEK_END   2   /* offset from end of file*/


INT32 DrvFSG3DUSBOpenFile(char* pcDirFileName, UINT32 dwFlags, INT32* piFd)
{
    struct file *filp;

    if ((NULL == pcDirFileName) || (NULL == piFd))
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }
    
//   Printf("[Drv_FS] %s: #%d: open file: %s\n", __FUNCTION__, __LINE__, pcDirFileName);

    if(dwFlags == DRV_FS_W_C)
    {
        filp = filp_open(pcDirFileName, O_CREAT|O_WRONLY, dwFlags);
    }
    else if(dwFlags == DRV_FS_RW_C)
    {
        filp = filp_open(pcDirFileName, O_CREAT|O_RDWR, dwFlags);
    }
    else
    {
        filp = filp_open(pcDirFileName, O_RDONLY, dwFlags);
    }

    if (IS_ERR(filp))
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_FAIL);
        return (DRV_FSR_FAIL);
    }

    *piFd = (INT32)filp;

    //Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}


INT32 DrvFSG3DUSBGetFileSize(INT32 iFd, UINT32 *pu4FileSize)
{
    struct file *filp = (struct file *)iFd;
    INT32 iRet;
    INT32 iCur;
       
    if (NULL == pu4FileSize)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }

    if (NULL == filp)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    if (NULL == filp->f_op)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }
    
    /* get current offset */
    iCur = filp->f_op->llseek(filp, 0, SEEK_CUR);
    
    /* move to end to get end offset, that is the file size */
    *pu4FileSize = filp->f_op->llseek(filp, 0, SEEK_END);

    /* move back to current offset */
    iRet = filp->f_op->llseek(filp, iCur, SEEK_SET);

    //Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}


INT32 DrvFSG3DUSBSeekFile(INT32 iFd, INT64 iOffset, INT32 iOrigin)
{
    struct file *filp = (struct file *)iFd;
    INT32 iPos;
    
    if ((iOrigin < 0) || (iOrigin > 2)) /*PSEEK_SET: 0, PSEEK_CUR: 1, PSEEK_END 2*/
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_PARAMETER_ERR);
        return (DRV_FSR_PARAMETER_ERR);
    }

    if (NULL == filp)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

   // Printf("[Drv_FS] %s: #%d: old Offset=%d\n", __FUNCTION__, __LINE__, (INT32)filp->f_pos);

    iPos = filp->f_pos;

    if (iOrigin == PSEEK_SET)
    {
        iPos = iOffset;
        filp->f_pos = iPos;
    }
    else if (iOrigin == PSEEK_CUR)
    {
        iPos += iOffset;
        filp->f_pos = iPos;
    }
    else if (iOrigin == PSEEK_END)
    {
        if (NULL == filp->f_op)
        {
            Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
            return DRV_FSR_NULL_POINT;
        }
        iPos = filp->f_op->llseek(filp, iOffset, SEEK_END);
    }    

    Printf("[Drv_FS] %s: #%d: new Offset=%d\n", __FUNCTION__, __LINE__, (INT32)filp->f_pos);
    return (iPos);
}


INT32 DrvFSG3DUSBReadFile(INT32 iFd, void* pbBuf, UINT32 u4Count)
{
    struct file *filp = (struct file *)iFd;
    mm_segment_t oldfs;
    INT32 iRet;

    if (NULL == pbBuf)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }

    if (NULL == filp)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    if (NULL == filp->f_op)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    oldfs = get_fs();
    set_fs(KERNEL_DS);

    iRet = filp->f_op->read(filp, pbBuf, u4Count, &filp->f_pos);

    set_fs(oldfs);

    //Printf("[Drv_FS] %s: #%d: read %d bytes Offset=%d\n", __FUNCTION__, __LINE__, iRet, (unsigned)filp->f_pos);
    return (iRet);
}


INT32 DrvFSG3DUSBWriteFile(INT32 iFd, const void *pbBuf, DWRD dwSize)
{
    struct file *filp = (struct file *)iFd;
    mm_segment_t oldfs;
    INT32 iRet;

    if (NULL == pbBuf)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return (DRV_FSR_NULL_POINT);
    }

    if (0 == dwSize)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_PARAMETER_ERR);
        return (DRV_FSR_PARAMETER_ERR);
    }

    if (NULL == filp)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    if (NULL == filp->f_op)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    oldfs = get_fs();
    set_fs(KERNEL_DS);
    
    iRet = filp->f_op->write(filp, pbBuf, dwSize, &filp->f_pos);

    set_fs(oldfs);

    Printf("[Drv_FS] %s: #%d: write (%d, %d) bytes Offset=%d from 0x%8x\n", __FUNCTION__, __LINE__, iRet, dwSize, (unsigned)filp->f_pos, (unsigned)pbBuf);
    return (iRet);
}


INT32 DrvFSG3DUSBCloseFile(INT32 iFd)
{
    struct file *filp = (struct file *)iFd;

    if (NULL == filp)
    {
        Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_NULL_POINT);
        return DRV_FSR_NULL_POINT;
    }

    filp_close(filp, NULL);

    Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}
#endif


#if G3D_SUPPORT_MAPUNMAP
extern void BSP_dma_map_vaddr(UINT32 u4Start, UINT32 u4Len, enum data_direction dir);
extern void BSP_dma_unmap_vaddr(UINT32 u4Start, UINT32 u4Len, enum data_direction dir);
#endif

#if 1
BOOL fgReadFile(void* pvAddr, UINT32 u4Size, CHAR *strFileName, UINT32 u4OffSet, INT32 *piFd, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength)
{
	INT32 i4Fd = 0;
	INT32 i4_ret = 0;
	UINT32 u4FileSize = 0;
	UINT32 u4ReadSize = u4Size;

	if ((NULL == pvAddr) || (0 == u4Size) || (NULL == strFileName) || (NULL == pu4RealReadSize))
	   return FALSE;

	*pu4RealReadSize = 0;

    x_thread_delay(100);

    #if G3D_SUPPORT_MAPUNMAP
    #else
	HalFlushInvalidateDCache();
	#endif	

	i4_ret = DrvFSG3DUSBOpenFile((char*)(strFileName), DRV_FS_RDONLY, &i4Fd);
	if (i4_ret < 0)
	{
	  Printf("Fs open file fail %d\n", i4_ret);
	  //ASSERT(0);
	   return FALSE;
	}
	        
	i4_ret = DrvFSG3DUSBGetFileSize(i4Fd,(UINT32*) (&u4FileSize));
	if (i4_ret < 0)
	{
	  Printf("Fs get file size fail %d\n", i4_ret);
	  ASSERT(0);
	  return FALSE;
	}
	
	//Printf("DrvFSG3DUSBGetFileSize=%d\n",u4FileSize);

	*pu4FileLength = u4FileSize;   

	if (u4OffSet >= u4FileSize)
	{
	    DrvFSG3DUSBCloseFile(i4Fd);
	    Printf("[WARNING] FILE OFFSET > SIZE ERROR!!!");
	    ASSERT(0);
	    return FALSE;
	}

	//DrvFSG3DUSBSeekFile(i4Fd, u4OffSet, 0);

	if (u4Size > (u4FileSize - u4OffSet))
	    u4ReadSize = u4FileSize - u4OffSet;
	else
	{
	    Printf("[WARNING] READ FILE SIZE ERROR!!!");
	}
	 
	i4_ret = DrvFSG3DUSBReadFile(i4Fd, (void*) pvAddr, u4ReadSize);
	if(i4_ret < 0)
	{
		Printf("read file Fail!\n");
		return FALSE;
	}
	DrvFSG3DUSBCloseFile(i4Fd);

	*pu4RealReadSize = u4ReadSize;

    #if G3D_SUPPORT_MAPUNMAP
	BSP_dma_map_vaddr((UINT32)pvAddr,u4FileSize , TO_DEVICE);
	#else
	HalFlushInvalidateDCache();
	#endif

	return TRUE;
}

#else
BOOL fgReadFile(void* pvAddr, UINT32 u4BufSize, CHAR *strFileName, UINT32 u4OffSet, INT32 *piFd, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength)
{
    INT32 i4FileSize = 0;
    //CHAR    strFullFileName [256] = {0};
	//INT32 idx = 0;
    if ((NULL == pvAddr) || (0 == u4BufSize) || (NULL == strFileName) || (NULL == pu4RealReadSize) || (u4OffSet!=0))
      return FALSE;

    *pu4RealReadSize = 0;
	*pu4FileLength = 0;

	Printf("[G3D] fgReadFile strFileName is %s !!!\r\n",strFileName);

	//for(idx =0; idx<6; idx++)
	i4FileSize = i4Gfx3DVfyReadFile(strFileName,(UINT32)pvAddr,u4BufSize);

    if(i4FileSize < 0)
    {
        Printf("[G3D][WARNING] OPEN FILE ERROR!!!\r\n");
        return FALSE;
    }

    if (u4OffSet >= i4FileSize)
    {
        Printf("[G3D][WARNING] FILE OFFSET > SIZE ERROR!!!\r\n");
        return FALSE;
    }
	
    if (u4BufSize < i4FileSize)
	{
        Printf("[G3D][WARNING] READ FILE SIZE ERROR!!!\r\n");
        return FALSE;
    }

	
    //Not implemented seek case (offset) current
    *pu4FileLength = i4FileSize;
	*pu4RealReadSize = i4FileSize;

    HalFlushInvalidateDCache();
	
    return TRUE;
	
}
#endif



//ICE I/F to load data
#else
BOOL fgReadFile(void* pvAddr, UINT32 u4Size, CHAR *strFileName, UINT32 u4OffSet, INT32 *piFd, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength)
{
    FILE* pFile = NULL;
    UINT32 u4FileSize = 0;
    UINT32 u4ReadSize = u4Size;
    
    if ((NULL == pvAddr) || (0 == u4Size) || (NULL == strFileName) || (NULL == pu4RealReadSize))
      return FALSE;

    *pu4RealReadSize = 0;

    if((pFile = fopen( (UINT8*)strFileName, "rb")) == NULL)
    {
        Printf("[WARNING] OPEN FILE ERROR!!!");
        return FALSE;
    }
    
    u4FileSize = pFile->filesize;//u4GetFileSize(pFile);
    *pu4FileLength = u4FileSize;

    if (u4OffSet >= u4FileSize)
    {
        fclose(pFile);
        Printf("[WARNING] FILE OFFSET > SIZE ERROR!!!");
        return FALSE;
    }

    if(fseek (pFile, u4OffSet, SEEK_SET))
    {
        fclose(pFile);
        Printf("[WARNING] SEEK FILE ERROR!!!");
        return FALSE;
    }

    if (u4Size > (u4FileSize - u4OffSet))
        u4ReadSize = u4FileSize - u4OffSet;
    else
    {
        Printf("[WARNING] READ FILE SIZE ERROR!!!");
    }

    *pu4RealReadSize = fread((void *)pvAddr, 1, u4ReadSize, pFile);
    fclose(pFile);

#if (CONFIG_DRV_VERIFY_SUPPORT) && (CONFIG_DRV_LINUX)    
    HalFlushInvalidateDCache();
#endif


    if (u4ReadSize != *pu4RealReadSize)
    {
        Printf("[WARNING] READ FILE ERROR!!!");
        return FALSE;
    }

    return TRUE;
}

BOOL fgWriteFile(void* pvAddr, UINT32 u4Size, CHAR *strFileName, UINT32 u4OffSet, INT32 *piFd, UINT32 *pu4RealWriteSize, UINT32 *pu4FileLength)
{
    FILE* pFile = NULL;
    //UINT32 u4FileSize = 0;
    //UINT32 u4ReadSize = u4Size;
    
    if ((NULL == pvAddr) || (0 == u4Size) || (NULL == strFileName) || (NULL == pu4RealWriteSize))
      return FALSE;

    *pu4RealWriteSize = 0;

    if((pFile = fopen(strFileName, "wb")) == NULL)
    {
        Printf("[WARNING] OPEN WRITE FILE ERROR!!!");
        return FALSE;
    }

#if (CONFIG_DRV_VERIFY_SUPPORT) && (CONFIG_DRV_LINUX)    
    HalFlushInvalidateDCache();
#endif    
    
    *pu4RealWriteSize = fwrite((void *)pvAddr, 1, u4Size, pFile);
    fclose(pFile);

    if (u4Size != *pu4RealWriteSize)
    {
        Printf("[WARNING] READ FILE ERROR!!!");
        return FALSE;
    }

    return TRUE;
}

//temp for build pass
INT32 T32_HostExec(INT32 i4TargetFlag, const CHAR *szFormat, ...)
{
	return 0;
}

#endif



#endif // __DRV_GFX3D_VFY_SYS_C
