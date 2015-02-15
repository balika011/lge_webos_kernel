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

#ifndef __KERNEL__
#include "x_bim.h"
#endif

#include "x_rand.h"
#include "drvcust_if.h"

#include "gfx3d_vfy_def.h"
#include "gfx3d_vfy_if.h"
#include "gfx3d_vfy_sys.h"

#include "../UDVT/UDVT.h"
#include "../UDVT/UDVT_IF.h"
#include "../UDVT/x_drv_map.h"

#include "x_hal_926.h"
#include "x_typedef.h"
#include "x_printf.h"
#include "x_debug.h"

#include "x_kmem.h"
//#include "drv_fs.h"
#include <linux/fs.h>
#include "x_fm.h"
#include <asm/unistd.h>


#include <asm/uaccess.h>
#include "linux/syscalls.h"
#include "linux/module.h"

/*-----------------------------------------------------------------------------
                    macros, defines, typedefs, enums
 ----------------------------------------------------------------------------*/

UINT32 u4ReadBimReg(UINT32 u4Addr)
{
    return (u4ReadReg(BIM_REG_OFFSET + u4Addr));
}

void vWriteBimReg(UINT32 u4Addr, UINT32 u4Val)
{
    vWriteReg(BIM_REG_OFFSET + u4Addr, u4Val);
}

void vMaliWriteReg(UINT32 u4Addr, UINT32 u4Val)
{
    vWriteReg(MALI_REG_OFFSET + u4Addr, u4Val);
}

UINT32 u4MaliReadReg(UINT32 u4Addr)
{
    return (u4ReadReg(MALI_REG_OFFSET + u4Addr));
}

void vMaliGPWriteReg(UINT32 u4Addr, UINT32 u4Val)
{
    vMaliWriteReg(MALI_REG_GP_OFFSET + u4Addr, u4Val);
}

UINT32 u4MaliGPReadReg(UINT32 u4Addr)
{
    return (u4MaliReadReg(MALI_REG_GP_OFFSET + u4Addr));
}

void vMaliPPWriteReg(UINT32 u4Addr, UINT32 u4Val)
{
    vMaliWriteReg(MALI_REG_PP_OFFSET + u4Addr, u4Val);
}

UINT32 u4MaliPPReadReg(UINT32 u4Addr)
{
    return (u4MaliReadReg(MALI_REG_PP_OFFSET + u4Addr));
}

void vMaliAXIWriteReg(UINT32 u4Addr, UINT32 u4Val)
{
    vMaliWriteReg(MALI_REG_AXI_OFFSET + u4Addr, u4Val);
}

UINT32 u4MaliAXIReadReg(UINT32 u4Addr)
{
    return (u4MaliReadReg(MALI_REG_AXI_OFFSET + u4Addr));
}

void vMaliWriteMem(UINT32 u4Addr, UINT32 u4Val)
{
    vWriteMem(u4Addr, u4Val);
}

UINT32 u4MaliReadMem(UINT32 u4Addr)
{
    return (u4ReadMem(u4Addr));
}


#if USB_G3D_VFY

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

    //Printf("[Drv_FS] %s: #%d: return %d\n", __FUNCTION__, __LINE__, DRV_FSR_SUCCESS);
    return (DRV_FSR_SUCCESS);
}

BOOL fgReadFile(void* pvAddr, UINT32 u4Size, CHAR *strFileName, UINT32 u4OffSet, INT32 *piFd, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength)
{
	INT32 i4Fd = 0;
	INT32 i4_ret = 0;
	UINT32 u4FileSize = 0;
	UINT32 u4ReadSize = u4Size;

	if ((NULL == pvAddr) || (0 == u4Size) || (NULL == strFileName) || (NULL == pu4RealReadSize))
	   return FALSE;

	*pu4RealReadSize = 0;
	//HalFlushInvalidateDCache();
	
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

	HalFlushInvalidateDCache();

	return TRUE;
}

//ICE I/F to load data
#elif T32_HOST 

static UINT32 u4fsize(FILE* pFile)
{
    UINT32 u4Result = 0;
    UINT32 u4Offset = 0;

    if(pFile == NULL)
        return u4Result;

    u4Offset = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    u4Result = ftell(pFile);
    fseek(pFile, u4Offset, SEEK_SET);
    return u4Result;
}


// *********************************************************************
// Function    : BOOL fgGPUReadPCFile(void* pvAddr, UINT32 u4Size, char *strFileName, UINT32 u4OffSet, UINT32 *pu4RealReadSize)
// Description : read bitstream from pc to memory
// Parameter   : None
// Return      : TRUE/FALSE
// *********************************************************************
BOOL fgGPUReadPCFile(void* pvAddr, UINT32 u4Size, char *strFileName, UINT32 u4OffSet, UINT32 *pu4RealReadSize, UINT32 *pu4FileLength)
{
    FILE* pFile = NULL;
    UINT32 u4FileSize = 0;
    UINT32 u4ReadSize = u4Size;
    if ((NULL == pvAddr) || (0 == u4Size) || (NULL == strFileName) || (NULL == pu4RealReadSize))
    {
        Printf("%s @ line %d some param error %s\n",__FUNCTION__,__LINE__,strFileName);
        return FALSE;
    }
    else
    {
        Printf("%s @ line %d read file %s go\n",__FUNCTION__,__LINE__,strFileName);
    }

    *pu4RealReadSize = 0;

    if((pFile = fopen(strFileName, "rb")) == NULL)
        return FALSE;
    
    u4FileSize = u4fsize(pFile);
    *pu4FileLength = u4FileSize;

    if (u4OffSet >= u4FileSize)
    {
        fclose(pFile);
        Printf("\n read offset(%d) > filesize(%d)", u4OffSet, u4FileSize);
        return FALSE;
    }

    if(fseek (pFile, u4OffSet, SEEK_SET))
    {
        fclose(pFile);
        Printf("\n seek fail: %s", strFileName);
        return FALSE;
    }

    if (u4Size > (u4FileSize - u4OffSet))
        u4ReadSize = u4FileSize - u4OffSet;

    *pu4RealReadSize = fread((void *)pvAddr, 1, u4ReadSize, pFile);
    fclose(pFile);

    if (u4ReadSize != *pu4RealReadSize)
    {
        Printf("\n read fail: %s", strFileName);
        //return FALSE;
    }

    return TRUE;
}

/* FATS Path */
#else
BOOL fgG3DFATSFsReadFile(
    CHAR *strFileName,
    void* pvAddr,
    UINT32 u4Offset,
    UINT32 u4Length,
    UINT32 *pu4RealReadSize,
    UINT32 *pu4TotalFileLength,
    INT32 *pi4FileId)
{

#ifdef CONFIG_TV_DRV_VFY
	INT32 i4_ret;
	INT32 i4Fd;
	UINT32 u4FileSize = 0;
	UINT32 u4ReadSize = 0;

	i4_ret = DrvFSOpenFile(strFileName, DRV_FS_RDONLY, &i4Fd);	 
	//Printf("Fs open file ret: %d\n", i4_ret); 
	if (i4_ret < 0)
	{
		Printf("Fs open file fail %d\n", i4_ret);
		//ASSERT(0);
		return 0;
	}
	*pi4FileId = i4Fd;
	
	 i4_ret = DrvFSGetFileSize(i4Fd, &u4FileSize);
	 if (i4_ret < 0)
	 {
		 Printf("Fs get file size fail %d\n", i4_ret);
		 //ASSERT(0);
		 return 0;
	 }
	  
	 *pu4TotalFileLength = u4FileSize; 			

	 if (u4Offset >= u4FileSize)
	 {
		  DrvFSCloseFile(i4Fd);
		  Printf(" read offset(%d) > filesize(%d)\n", u4Offset, u4FileSize);
		  return FALSE;
	 }
	 if(DrvFSSeekFile(i4Fd, (INT64) u4Offset, 0))
	 {
		 DrvFSCloseFile(i4Fd);
		 Printf(" seek fail: %s\n", strFileName);
		 return FALSE;
	 }

     Printf("u4Length(%d), read offset(%d), filesize(%d)\n ", u4Length, u4Offset, u4FileSize);
	if (u4Length >= (u4FileSize - u4Offset))
	{
	    u4ReadSize = u4FileSize - u4Offset;
	}
	else
	{       
	    printk("Warning =====>File size is larger than VFIFO size! 0x%x Byte will be read\n",u4Length);
	    u4ReadSize = u4Length;
	    *pu4TotalFileLength = u4Length;
	        
	}
    
    Printf(" u4ReadSize(%d), filesize(%d)\n", u4ReadSize, u4FileSize);
    
    i4_ret = DrvFSReadFile(i4Fd, pvAddr, u4ReadSize);
    if(i4_ret < 0)
    {
          Printf("read file Fail!\n");
          return FALSE;
    }
    DrvFSCloseFile(i4Fd);
    
   *pu4RealReadSize = i4_ret;
	if (u4ReadSize != *pu4RealReadSize)
	{
		Printf("\n read fail: %s", strFileName);
		return FALSE;
	}
#endif

	return TRUE;	
}


#endif

#endif // __DRV_GFX3D_VFY_SYS_C


