#include "vdec_verify_mpv_prov.h"
#include "vdec_verify_mm_map.h"
#include "x_os.h"

#ifndef CONFIG_TV_DRV_VFY
#include "x_rtos.h"
#endif // CONFIG_TV_DRV_VFY

#include "drv_config.h"
#include "x_printf.h"
#include "x_debug.h"
#include "vdec_hw_common.h"
#include "../../UDVT/x_drv_map.h"
#include "x_typedef.h"

#ifndef CONFIG_TV_DRV_VFY
#include <mach/cache_operation.h>
#endif // CONFIG_TV_DRV_VFY

#ifndef CONFIG_TV_DRV_VFY
#include "sys_config.h"
#if CONFIG_SYS_MEM_PHASE2
#include "x_mem_phase2.h"
#elif CONFIG_SYS_MEM_PHASE3
#include "x_kmem.h"
#endif
#endif //CONFIG_TV_DRV_VFY

#if GET_RESERVERD_MEM
extern unsigned long get_drvmem_mem(void);
static UINT32 _u4MemReserved = 0;
UINT32 u4GetVFIFOAddr(UINT32 u4InstID);
#define ALIGN(value, align) ((value + (align - 1)) & (~(align - 1))) //(((value / align) + ((0 == (value % align)) ? 0 : 1)) * align)
#endif

#if VDEC_SUPPORT_IOMMU
extern void* BSP_VAllocFragmentMem(UINT32 u4Size);
extern void* BSP_VAllocFragmentMem2(UINT32 u4Size);
#endif


//#define x_alloc_aligned_verify_mem(u4Size, u4Align, fgChannelA) x_alloc_aligned_verify_mem_ex(u4Size, u4Align, fgChannelA, __FUNCTION__, __LINE__)

#define x_alloc_aligned_verify_mem(u4Size, u4Align, fgChannelA) VIRTUAL(BSP_AllocAlignedDmaMemory(u4Size,u4Align))
//#define x_alloc_aligned_verify_mem(u4Size, u4Align, fgChannelA) BSP_AllocVfyReserveMemory(MEM_CHANNEL_B, u4Size,u4Align)

void * x_alloc_aligned_verify_mem_ex(UINT32 u4Size, UINT32 u4Align, BOOL fgChannelA, const char *szFunction, INT32 i4Line)
{
	UINT32 u4Mem;
	UCHAR *p = NULL;
	UINT32 u4MB = u4Size / (1024 * 1024);
	UINT32 u4KB = (u4Size - u4MB * 1024 * 1024) / 1024;
	UINT32 u4B  = u4Size - u4MB * 1024 * 1024 - u4KB * 1024;

	if(fgChannelA)
	{
#if (!CONFIG_DRV_LINUX)
		return x_alloc_aligned_nc_mem(u4Size, u4Align);
#else
		if (fgChannelA)
		{
	   		p = x_alloc_aligned_mem(u4Size, u4Align);
		}
	//else
	//   p =x_alloc_aligned_mem(u4Size, u4Align);
	//return x_alloc_aligned_dma_mem(u4Size, u4Align);
		if (p)
		{
		    x_memset(p, 0, u4Size);
		    vVDec_FlushDCacheRange((UINT32)p, u4Size);
		}
		UTIL_Printf("<vdec> alloc(A) %uM %uK %uB return 0x%08X (%s, %d)\n", u4MB, u4KB, u4B, p, szFunction, i4Line);
		return (p);
#endif
	}
	else
	{
#if (!CONFIG_DRV_LINUX)
		u4Mem =  NONCACHE((UINT32)(x_alloc_aligned_ch2_mem(u4Size, u4Align)));
#else
		u4Mem =  ((UINT32)(x_alloc_aligned_ch2_mem(u4Size, u4Align)));
#endif

        if (u4Mem)
        {
            x_memset((void *)u4Mem, 0, u4Size);
            //vVDec_FlushDCacheRange(u4Mem, u4Size);
            HalFlushInvalidateDCache();
        }

        UTIL_Printf("<vdec> alloc(B) %uM %uK %uB return 0x%08X (%s, %d)\n", u4MB, u4KB, u4B, u4Mem, szFunction, i4Line);

        return (void *)(u4Mem);
    }
}

void x_free_aligned_verify_mem(void *pvAddr, BOOL fgChannelA)
{
	if (pvAddr)
	{
#if (!CONFIG_DRV_LINUX)
		x_free_aligned_nc_mem(pvAddr);
#else

        if (fgChannelA)
        {
          x_free_aligned_mem(pvAddr);
//	          UTIL_Printf("<vdec> free(A) 0x%08X (%s, %d)\n", pvAddr, __FUNCTION__, __LINE__);
        }
        else
        {
          x_free_aligned_mem(pvAddr);
//	          UTIL_Printf("<vdec> free(B) 0x%08X (%s, %d)\n", pvAddr, __FUNCTION__, __LINE__);
        }
	//return x_free_aligned_dma_mem(pvAddr);
#endif
	}
}


void vMemoryAllocate_RM(UINT32 u4InstID)
{
#ifdef DYNAMIC_MEMORY_ALLOCATE
#ifdef DOWN_SCALE_SUPPORT
	_pucVDSCLBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_BUF_SZ,1024, VDSCL_CHANEL_A);
	_pucVDSCLWorkBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_WORK_SZ,1024, VDSCL_CHANEL_A);
	_pucVDSCLWork1Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK1_SZ,1024, VDSCL_CHANEL_A);
	_pucVDSCLWork2Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK2_SZ,1024, VDSCL_CHANEL_A);
	_pucVDSCLWork3Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK3_SZ,1024, VDSCL_CHANEL_A);
	_pucVDSCLWork4Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK4_SZ,1024, VDSCL_CHANEL_A);
#endif

#ifdef  SATA_HDD_READ_SUPPORT
#ifndef  SATA_HDD_FS_SUPPORT
	_pucGoldenFileInfoSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLDEN_FILE_INFO_SZ,1024, 1);
#endif
#endif
#if 1
   UINT32 u4VfifoAddr = (UINT32)get_drvmem_mem();
    UTIL_Printf("Test u4VfifoAddr = 0x%x \n", u4VfifoAddr);

   _pucVFifo[u4InstID] = (UCHAR *)(VIRTUAL(u4VfifoAddr));

   UTIL_Printf("Test _pucVFifo[u4InstID] = 0x%x, size is 0x%lx, Physical address= 0x%x\n", _pucVFifo[u4InstID], V_FIFO_SZ, PHYSICAL(_pucVFifo[u4InstID]));

   _pucFileListSa[u4InstID] = _pucVFifo[u4InstID] + V_FIFO_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
  UTIL_Printf("_pucFileListSa[u4InstID] Address :%x!, Size is 0x%x\n",_pucFileListSa[u4InstID], FILE_LIST_SZ);


  #ifdef RM_CRCCHECKFLOW_SUPPORT
	_pucRMCRCResultBuf[u4InstID] =	_pucFileListSa[u4InstID] + FILE_LIST_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(RM_CRCRESULT_SZ, 1024, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucRMCRCResultBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMCRCResultBuf[u4InstID], RM_CRCRESULT_SZ);
  #endif //RM_CRCCHECKFLOW_SUPPORT

  _pucDumpYBuf[u4InstID] = _pucRMCRCResultBuf[u4InstID] + RM_CRCRESULT_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(GOLD_Y_SZ,1024, GOLD_CHANEL_A);
  UTIL_Printf("_pucDumpYBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucDumpYBuf[u4InstID], GOLD_Y_SZ);
  _pucDumpCBuf[u4InstID] = _pucDumpYBuf[u4InstID] + GOLD_Y_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(GOLD_C_SZ,1024, GOLD_CHANEL_A);
  UTIL_Printf("_pucDumpCBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucDumpCBuf[u4InstID], GOLD_C_SZ);
  _pucPic0Y[u4InstID] = _pucDumpCBuf[u4InstID] + GOLD_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ, 2048, WORKING_AREA_CHANEL_A); //UCHAR address
  UTIL_Printf("_pucPic0Y[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic0Y[u4InstID], PIC_Y_SZ);
  _pucPic0C[u4InstID] = _pucPic0Y[u4InstID] + PIC_Y_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic0C[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic0C[u4InstID],PIC_C_SZ);

  _pucPic1Y[u4InstID] = _pucPic0C[u4InstID] + PIC_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ, 2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic1Y[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic1Y[u4InstID], PIC_Y_SZ);
  _pucPic1C[u4InstID] = _pucPic1Y[u4InstID] + PIC_Y_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic1C[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic1C[u4InstID], PIC_C_SZ);

  _pucPic2Y[u4InstID] = _pucPic1C[u4InstID] + PIC_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ, 2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic2Y[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic2Y[u4InstID], PIC_Y_SZ);
  _pucPic2C[u4InstID] = _pucPic2Y[u4InstID] + PIC_Y_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic2C[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic2C[u4InstID], PIC_C_SZ);

  _pucPic3Y[u4InstID] = _pucPic2C[u4InstID] + PIC_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ, 2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic3Y[u4InstID] Address :%x!!!\n",_pucPic3Y[u4InstID]);
  _pucPic3C[u4InstID] = _pucPic3Y[u4InstID] + PIC_Y_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ, 2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic3C[u4InstID] Address :%x!!!\n",_pucPic3C[u4InstID]);

  _pucRMFrmInfoBuf[u4InstID] = _pucPic3C[u4InstID] + PIC_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(RM_FRMINFO_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMFrmInfoBuf[u4InstID] Address :%x!!!\n",_pucRMFrmInfoBuf[u4InstID]);

  _pucRMMvHwWorkBuf[u4InstID] = _pucRMFrmInfoBuf[u4InstID] + RM_FRMINFO_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(RM_MVHWBUF_SZ, 4096, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMMvHwWorkBuf[u4InstID] Address :%x!!!\n",_pucRMMvHwWorkBuf[u4InstID]);
  _pucRmVldPredWorkBuf[u4InstID] = _pucRMMvHwWorkBuf[u4InstID] + RM_MVHWBUF_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(RM_VLDPRED_SZ, 4096, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRmVldPredWorkBuf[u4InstID] Address :%x!!!\n",_pucRmVldPredWorkBuf[u4InstID]);

  x_memset(_pucRMMvHwWorkBuf[u4InstID], 0, RM_MVHWBUF_SZ);
  x_memset(_pucRmVldPredWorkBuf[u4InstID], 0, RM_VLDPRED_SZ);

  _pucRMGoldenDataBuf[u4InstID] = _pucRmVldPredWorkBuf[u4InstID] + RM_VLDPRED_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(RM_GOLDENDATA_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMGoldenDataBuf[u4InstID] Address :%x!!!\n",_pucRMGoldenDataBuf[u4InstID]);

  _pucDumpYBuf_1[u4InstID] = _pucRMGoldenDataBuf[u4InstID] + RM_GOLDENDATA_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(GOLD_Y_SZ_1, 2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucDumpYBuf_1[u4InstID] Address :%x!!!\n",_pucDumpYBuf_1[u4InstID]);
  _pucDumpCBuf_1[u4InstID] = _pucDumpYBuf_1[u4InstID] + GOLD_Y_SZ_1;//(UCHAR *)x_alloc_aligned_verify_mem(GOLD_C_SZ_1, 2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucDumpCBuf_1[u4InstID] Address :%x!!!\n",_pucDumpCBuf_1[u4InstID]);



  #ifdef DOWN_SCALE_SUPPORT
  _pucVDSCLBuf_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_BUF_SZ_1, 1024, WORKING_AREA_CHANEL_A);

  //Only for RV8 Debug
  //_pucRMMCOutputBufY[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_MCOUT_Y_SZ, 1024, WORKING_AREA_CHANEL_A);
  //_pucRMMCOutputBufC[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_MCOUT_C_SZ, 1024, WORKING_AREA_CHANEL_A);
  #endif //DOWN_SCALE_SUPPORT

  _pucRMReszWorkBuf[u4InstID] = _pucDumpCBuf_1[u4InstID] + GOLD_C_SZ_1;//(UCHAR *)x_alloc_aligned_verify_mem(RM_RSZWORKBUF_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMReszWorkBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMReszWorkBuf[u4InstID], RM_RSZWORKBUF_SZ);
  _pucRMRingWorkBuf[u4InstID] = _pucRMReszWorkBuf[u4InstID] + RM_RSZWORKBUF_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(RM_RINGFLOW_TEMPFIFO_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMRingWorkBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMRingWorkBuf[u4InstID], RM_RINGFLOW_TEMPFIFO_SZ);
  #ifdef RM_ATSPEED_TEST_ENABLE
  _pucRMAULikeBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_AULIKEBUF_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMAULikeBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMAULikeBuf[u4InstID], RM_AULIKEBUF_SZ);
  #endif
  _pucRMChecksumBuf[u4InstID] = _pucRMRingWorkBuf[u4InstID] + RM_RINGFLOW_TEMPFIFO_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(RM_CHECKSUM_BUFFER_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMChecksumBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMChecksumBuf[u4InstID], RM_CHECKSUM_BUFFER_SZ);

  #ifdef RM_SUPPORT_NEW_TABLE_MODE
  _pucRMIntraTableBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_INTRA_TABLE_DATA_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMIntraTableBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMIntraTableBuf[u4InstID], RM_INTRA_TABLE_DATA_SZ);
  _pucRMInterTableBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_INTER_TABLE_DATA_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMInterTableBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMInterTableBuf[u4InstID], RM_INTER_TABLE_DATA_SZ);
  #endif

#else
  _pucFileListSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
  UTIL_Printf("_pucFileListSa[u4InstID] Address :%x!, Size is 0x%x\n",_pucFileListSa[u4InstID], FILE_LIST_SZ);
  UTIL_Printf("8888 Size   is %x\n", FILE_LIST_SZ);

  _pucVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(V_FIFO_SZ,4096, 1);
  //_pucVFifo[u4InstID] = (UCHAR *)get_drvmem_mem();

  #ifdef RM_CRCCHECKFLOW_SUPPORT
	_pucRMCRCResultBuf[u4InstID] =	(UCHAR *)x_alloc_aligned_verify_mem(RM_CRCRESULT_SZ, 1024, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucRMCRCResultBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMCRCResultBuf[u4InstID], RM_CRCRESULT_SZ);
  #endif //RM_CRCCHECKFLOW_SUPPORT
  UTIL_Printf("_pucVFifo[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucVFifo[u4InstID], V_FIFO_SZ);
  _pucDumpYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLD_Y_SZ,1024, GOLD_CHANEL_A);
  UTIL_Printf("_pucDumpYBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucDumpYBuf[u4InstID], GOLD_Y_SZ);
  _pucDumpCBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLD_C_SZ,1024, GOLD_CHANEL_A);
  UTIL_Printf("_pucDumpCBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucDumpCBuf[u4InstID], GOLD_C_SZ);
  _pucPic0Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ, 2048, WORKING_AREA_CHANEL_A); //UCHAR address
  UTIL_Printf("_pucPic0Y[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic0Y[u4InstID], PIC_Y_SZ);
  _pucPic0C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic0C[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic0C[u4InstID],PIC_C_SZ);

  _pucPic1Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ, 2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic1Y[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic1Y[u4InstID], PIC_Y_SZ);
  _pucPic1C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic1C[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic1C[u4InstID], PIC_C_SZ);

  _pucPic2Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ, 2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic2Y[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic2Y[u4InstID], PIC_Y_SZ);
  _pucPic2C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucPic2C[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucPic2C[u4InstID], PIC_C_SZ);

	_pucPic3Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ, 2048, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucPic3Y[u4InstID] Address :%x!!!\n",_pucPic3Y[u4InstID]);
	_pucPic3C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ, 2048, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucPic3C[u4InstID] Address :%x!!!\n",_pucPic3C[u4InstID]);

	_pucRMFrmInfoBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_FRMINFO_SZ, 1024, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucRMFrmInfoBuf[u4InstID] Address :%x!!!\n",_pucRMFrmInfoBuf[u4InstID]);

	_pucRMMvHwWorkBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_MVHWBUF_SZ, 4096, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucRMMvHwWorkBuf[u4InstID] Address :%x!!!\n",_pucRMMvHwWorkBuf[u4InstID]);
	_pucRmVldPredWorkBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_VLDPRED_SZ, 4096, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucRmVldPredWorkBuf[u4InstID] Address :%x!!!\n",_pucRmVldPredWorkBuf[u4InstID]);

	x_memset(_pucRMMvHwWorkBuf[u4InstID], 0, RM_MVHWBUF_SZ);
	x_memset(_pucRmVldPredWorkBuf[u4InstID], 0, RM_VLDPRED_SZ);

	_pucRMGoldenDataBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_GOLDENDATA_SZ, 1024, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucRMGoldenDataBuf[u4InstID] Address :%x!!!\n",_pucRMGoldenDataBuf[u4InstID]);

	_pucDumpYBuf_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLD_Y_SZ_1, 2048, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucDumpYBuf_1[u4InstID] Address :%x!!!\n",_pucDumpYBuf_1[u4InstID]);
	_pucDumpCBuf_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLD_C_SZ_1, 2048, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucDumpCBuf_1[u4InstID] Address :%x!!!\n",_pucDumpCBuf_1[u4InstID]);

  #ifdef RM_SUPPORT_NEW_TABLE_MODE
  _pucRMIntraTableBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_INTRA_TABLE_DATA_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMIntraTableBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMIntraTableBuf[u4InstID], RM_INTRA_TABLE_DATA_SZ);
  _pucRMInterTableBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_INTER_TABLE_DATA_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMInterTableBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMInterTableBuf[u4InstID], RM_INTER_TABLE_DATA_SZ);
  #endif

#ifdef DOWN_SCALE_SUPPORT
	_pucVDSCLBuf_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_BUF_SZ_1, 1024, WORKING_AREA_CHANEL_A);

	//Only for RV8 Debug
	//_pucRMMCOutputBufY[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_MCOUT_Y_SZ, 1024, WORKING_AREA_CHANEL_A);
	//_pucRMMCOutputBufC[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_MCOUT_C_SZ, 1024, WORKING_AREA_CHANEL_A);
#endif //DOWN_SCALE_SUPPORT

  _pucRMReszWorkBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_RSZWORKBUF_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMReszWorkBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMReszWorkBuf[u4InstID], RM_RSZWORKBUF_SZ);
  _pucRMRingWorkBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_RINGFLOW_TEMPFIFO_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMRingWorkBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMRingWorkBuf[u4InstID], RM_RINGFLOW_TEMPFIFO_SZ);
  #ifdef RM_ATSPEED_TEST_ENABLE
  _pucRMAULikeBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_AULIKEBUF_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMAULikeBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMAULikeBuf[u4InstID], RM_AULIKEBUF_SZ);
  #endif
  _pucRMChecksumBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RM_CHECKSUM_BUFFER_SZ, 1024, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucRMChecksumBuf[u4InstID] Address :%x!!!, Size is 0x%x\n",_pucRMChecksumBuf[u4InstID], RM_CHECKSUM_BUFFER_SZ);

#endif //RM_CRCCHECKFLOW_SUPPORT

#else //DYNAMIC_MEMORY_ALLOCATE
	//Don't Support in RM Decoder Emulation/Verification Flow
	ASSERT(0);
#endif //DYNAMIC_MEMORY_ALLOCATE
}

#ifdef VDEC_MPEG_EMU
void vMemoryAllocate_MPEG(UINT32 u4InstID)
{
     #if (!VDEC_SUPPORT_IOMMU)
#if DUAL_VDEC_TEST
    if(u4InstID == 0)
    {
        _pucVFifo[u4InstID] = (UCHAR *)VIRTUAL(BSP_AllocVfyReserveMemory(MEM_CHANNEL_B,MEM_256M,1024));
        x_memset(_pucVFifo[u4InstID],0,V_FIFO_SZ);
        UTIL_Printf("_pucVFifo[u4InstID] = 0x%x, size = 0x%x, PHY:0x%x\n", _pucVFifo[u4InstID],V_FIFO_SZ, (UINT32)get_drvmem_mem());
    }
    else
    {
        _pucVFifo[u4InstID] = (UCHAR *)VIRTUAL(BSP_AllocVfyReserveMemory(MEM_CHANNEL_B,MEM_256M,1024)) + MEM_128M;
        x_memset(_pucVFifo[u4InstID],0,V_FIFO_SZ);
        UTIL_Printf("_pucVFifo[u4InstID] = 0x%x, size = 0x%x, PHY:0x%x\n", _pucVFifo[u4InstID],V_FIFO_SZ, (UINT32)get_drvmem_mem());
    }

#else
    _pucVFifo[u4InstID] = (UCHAR *)VIRTUAL(BSP_AllocVfyReserveMemory(MEM_CHANNEL_B,MEM_256M,1024));
    x_memset(_pucVFifo[u4InstID],0,V_FIFO_SZ);
    UTIL_Printf("_pucVFifo[u4InstID] = 0x%x, size = 0x%x, PHY:0x%x\n", _pucVFifo[u4InstID],V_FIFO_SZ, (UINT32)get_drvmem_mem());
#endif
      #else

    //_pucVFifo[u4InstID] = (UCHAR *)((BSP_VAllocFragmentMem(V_FIFO_SZ)));
   // _pucVFifo[u4InstID] = (UCHAR *)((BSP_VAllocFragmentMem2(V_FIFO_SZ)));
    //x_memset(_pucVFifo[u4InstID],0,V_FIFO_SZ);
    //UTIL_Printf("IOMMU Test _pucVFifo[u4InstID] = 0x%x, size is 0x%lx\n", _pucVFifo[u4InstID], V_FIFO_SZ);
    #endif
        #if VDEC_SUPPORT_IOMMU
       UINT32 u4VfifoAddr = (UINT32)get_drvmem_mem();
       UTIL_Printf("Test u4VfifoAddr = 0x%x \n", u4VfifoAddr);

       _pucFileListSa[u4InstID] = (UCHAR *)(VIRTUAL(u4VfifoAddr));
       UTIL_Printf("_pucFileListSa[u4InstID] = 0x%x, size is 0x%lx\n", _pucFileListSa[u4InstID], FILE_LIST_SZ);
        #else
        _pucFileListSa[u4InstID] = _pucVFifo[u4InstID] + V_FIFO_SZ;
        UTIL_Printf("_pucFileListSa[u4InstID] = 0x%x, size =0x%x\n", _pucFileListSa[u4InstID],FILE_LIST_SZ);
        #endif
    _pucDumpYBuf[u4InstID] = _pucFileListSa[u4InstID] + FILE_LIST_SZ;
    UTIL_Printf("_pucDumpYBuf[u4InstID] = 0x%x, size = 0x%x\n", _pucDumpYBuf[u4InstID],GOLD_Y_SZ);
    _pucDumpCBuf[u4InstID] = _pucDumpYBuf[u4InstID] + GOLD_Y_SZ;
    UTIL_Printf("_pucDumpCBuf[u4InstID] = 0x%x, size = 0x%x\n", _pucDumpCBuf[u4InstID],GOLD_C_SZ);
     _pucCRCBuf[u4InstID]  =  _pucDumpCBuf[u4InstID] + GOLD_C_SZ;
     UTIL_Printf("_pucCRCBuf[u4InstID] = 0x%x, size:0x%x\n", _pucCRCBuf[u4InstID],CRC_BUF_SZ);
     _pucVDSCLBuf[u4InstID] = _pucCRCBuf[u4InstID] + CRC_BUF_SZ;
     UTIL_Printf("_pucVDSCLBuf[u4InstID] = 0x%x, size:0x%x\n", _pucVDSCLBuf[u4InstID], VDSCL_BUF_SZ);

      _pucPic0Y[u4InstID] = _pucVDSCLBuf[u4InstID] + VDSCL_BUF_SZ;
      _pucPic0C[u4InstID] = _pucPic0Y[u4InstID] + PIC_Y_SZ;
      _pucPic1Y[u4InstID] = _pucPic0C[u4InstID] + PIC_C_SZ;
      _pucPic1C[u4InstID] = _pucPic1Y[u4InstID] + PIC_Y_SZ;
      _pucPic2Y[u4InstID] = _pucPic1C[u4InstID] + PIC_C_SZ;
      _pucPic2C[u4InstID] = _pucPic2Y[u4InstID] + PIC_Y_SZ;
      UTIL_Printf("PIC_Y_SZ:0x%x, PIC_C_SZ:0x%x\n", PIC_Y_SZ, PIC_C_SZ);
      UTIL_Printf("_pucPic0Y[u4InstID] = 0x%x, phy:0x%x\n", _pucPic0Y[u4InstID], PHYSICAL((UINT32)_pucPic0Y[u4InstID]));
      UTIL_Printf("_pucPic0C[u4InstID] = 0x%x, phy:0x%x\n", _pucPic0C[u4InstID], PHYSICAL((UINT32)_pucPic0C[u4InstID]));
      UTIL_Printf("_pucPic1Y[u4InstID] = 0x%x, phy:0x%x\n", _pucPic1Y[u4InstID], PHYSICAL((UINT32)_pucPic1Y[u4InstID]));
      UTIL_Printf("_pucPic1C[u4InstID] = 0x%x, phy:0x%x\n", _pucPic1C[u4InstID], PHYSICAL((UINT32)_pucPic1C[u4InstID]));
      UTIL_Printf("_pucPic2Y[u4InstID] = 0x%x, phy:0x%x\n",_pucPic2Y [u4InstID], PHYSICAL((UINT32)_pucPic2Y[u4InstID]));
      UTIL_Printf("_pucPic2C[u4InstID] = 0x%x, phy:0x%x\n", _pucPic2C[u4InstID], PHYSICAL((UINT32)_pucPic2C[u4InstID]));

     _pucPpYSa[u4InstID] = _pucPic2C[u4InstID] + PIC_C_SZ;
      _pucPpCSa[u4InstID] = _pucPpYSa[u4InstID] + DEC_PP_Y_SZ;
      _pucMp4Dcac[u4InstID] = _pucPpCSa[u4InstID] + DEC_PP_C_SZ;
      #if !DRAM_256M_ALLOC
        _pucMp4Mvec[u4InstID] = _pucMp4Dcac[u4InstID] + DCAC_SZ;
      #endif
      _pucMp4Bmb1[u4InstID] = _pucMp4Dcac[u4InstID] + DCAC_SZ +VER_MVEC_SZ;
      _pucMp4Bmb2[u4InstID] = _pucMp4Bmb1[u4InstID] + VER_BMB1_SZ;
      _pucMp4Bcode[u4InstID] = _pucMp4Bmb2[u4InstID] + VER_BMB2_SZ;

      if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
      {
        //  _pucAddressSwapBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(ADDSWAP_BUF_SZ, 2048, WORKING_AREA_CHANEL_A);
          _pucAddressSwapBuf[u4InstID] = _pucMp4Bcode[u4InstID]  + PIC_C_SZ;
          UTIL_Printf("_pucAddressSwapBuf[u4InstID] = 0x%x, size:0x%x\n", _pucAddressSwapBuf[u4InstID], ADDSWAP_BUF_SZ);
      }
      _pucDumpSRAMBuf[u4InstID] =_pucMp4Bcode[u4InstID]  + PIC_C_SZ + BCODE_SZ;


      UTIL_Printf("_pucPpYSa[u4InstID] = 0x%x, size:0x%x\n", _pucPpYSa[u4InstID], DEC_PP_Y_SZ);
      UTIL_Printf("_pucPpCSa[u4InstID] = 0x%x, size:0x%x\n", _pucPpCSa[u4InstID],DEC_PP_C_SZ);
      UTIL_Printf("_pucMp4Dcac[u4InstID] = 0x%x, size:0x%x,phy:0x%x\n", _pucMp4Dcac[u4InstID], DCAC_SZ,PHYSICAL((UINT32) _pucMp4Dcac[u4InstID]));
      UTIL_Printf("_pucMp4Mvec[u4InstID] = 0x%x, size:0x%x\n", _pucMp4Mvec[u4InstID],VER_MVEC_SZ);
      UTIL_Printf("_pucMp4Bmb1[u4InstID] = 0x%x, size:0x%x\n", _pucMp4Bmb1[u4InstID], VER_BMB1_SZ);
      UTIL_Printf("_pucMp4Bmb2[u4InstID] = 0x%x, size:0x%x\n", _pucMp4Bmb2[u4InstID], VER_BMB2_SZ);
      UTIL_Printf("_pucMp4Bcode[u4InstID] = 0x%x, size:0x%x\n", _pucMp4Bcode[u4InstID], BCODE_SZ,PHYSICAL((UINT32)_pucMp4Bcode[u4InstID]));
      UTIL_Printf("_pucDumpSRAMBuf[u4InstID]  = 0x%x\n", _pucDumpSRAMBuf[u4InstID]);
}
#endif
#ifdef VDEC_AVS_EMU
void vMemoryAllocate_AVS(UINT32 u4InstID)
{
    UCHAR * ucMemAddr;

#ifdef DYNAMIC_MEMORY_ALLOCATE
    ucMemAddr = (UCHAR *)VIRTUAL(BSP_AllocVfyReserveMemory(MEM_CHANNEL_B,MEM_256M, 1024));
    UTIL_Printf("//Acquire Memory here, BaseAddr is 0x%8x\n", ucMemAddr);

#ifdef	SATA_HDD_READ_SUPPORT
  #ifndef  SATA_HDD_FS_SUPPORT
    _pucGoldenFileInfoSa[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + GOLDEN_FILE_INFO_SZ;
    UTIL_Printf("//_pucGoldenFileInfoSa[%d] = 0x%x, size is 0x%x\n", u4InstID, _pucGoldenFileInfoSa[u4InstID], GOLDEN_FILE_INFO_SZ);
  #endif
#endif

    _pucVFifo[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + V_FIFO_SZ;
    UTIL_Printf("//_pucVFifo[%d] = 0x%x, size is 0x%x(%d), Physical address= 0x%x\n",
        u4InstID, _pucVFifo[u4InstID], V_FIFO_SZ, V_FIFO_SZ, PHYSICAL(_pucVFifo[u4InstID]));

    _pucFileListSa[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr +FILE_LIST_SZ;
    UTIL_Printf("//_pucFileListSa[%d] = 0x%x, size is 0x%lx\n", u4InstID, _pucFileListSa[u4InstID], FILE_LIST_SZ);

    _pucDumpYBuf[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr +GOLD_Y_SZ;
    UTIL_Printf("//_pucDumpYBuf[%d] = 0x%x, size is 0x%lx\n", u4InstID, _pucDumpYBuf[u4InstID], GOLD_Y_SZ);

    _pucDumpCBuf[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr +GOLD_C_SZ;
    UTIL_Printf("//_pucDumpCBuf[%d] = 0x%x, size is 0x%lx\n", u4InstID, _pucDumpCBuf[u4InstID], GOLD_C_SZ);

    _pucCRCBuf[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr +0x800; //32;
    UTIL_Printf("//_pucCRCBuf[%d] = 0x%x\n", u4InstID, _pucCRCBuf[u4InstID]);

  #if AVS_CRC_COMPARE_SUPORT
    _pucMergeCRCBuf[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + MERGE_CRC_BUF_SZ;
    UTIL_Printf("//_pucMergeCRCBuf[%d] = 0x%x, size is 0x%x\n", u4InstID, _pucMergeCRCBuf[u4InstID], MERGE_CRC_BUF_SZ);
  #endif

    _pucPicTolCnt[u4InstID]= ucMemAddr;
    ucMemAddr = ucMemAddr + 0x800; //16;
    UTIL_Printf("//_pucPicTolCnt[%d] = 0x%x\n", u4InstID, _pucPicTolCnt[u4InstID]);

#if AVS_SUPPORT_YC_SEPERATE
#if AVS_UFO_MODE_SUPPORT
    ucMemAddr = (UCHAR*)((((UINT32)ucMemAddr + 8191)>>13)<<13);
#endif
    _pucPic0Y[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_Y_SZ;
    _pucPic1Y[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_Y_SZ;
    _pucPic2Y[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_Y_SZ;
    _pucPic3Y[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_Y_SZ;

    _pucPic0C[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_C_SZ;
    _pucPic1C[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_C_SZ;
    _pucPic2C[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_C_SZ;
    _pucPic3C[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_C_SZ;
#else
    _pucPic0Y[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_Y_SZ;
    _pucPic0C[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_C_SZ;

    _pucPic1Y[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_Y_SZ;
    _pucPic1C[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_C_SZ;

    _pucPic2Y[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_Y_SZ;
    _pucPic2C[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_C_SZ;

    _pucPic3Y[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_Y_SZ;
    _pucPic3C[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + PIC_C_SZ;
#endif

    UTIL_Printf("//PIC_Y_SZ : 0x%x, PIC_C_SZ : 0x%x\n", PIC_Y_SZ, PIC_C_SZ);
    UTIL_Printf("//_pucPic0Y[%d] = 0x%x\n", u4InstID, _pucPic0Y[u4InstID]);
    UTIL_Printf("//_pucPic0C[%d] = 0x%x\n", u4InstID, _pucPic0C[u4InstID]);
    UTIL_Printf("//_pucPic1Y[%d] = 0x%x\n", u4InstID, _pucPic1Y[u4InstID]);
    UTIL_Printf("//_pucPic1C[%d] = 0x%x\n", u4InstID, _pucPic1C[u4InstID]);
    UTIL_Printf("//_pucPic2Y[%d] = 0x%x\n", u4InstID, _pucPic2Y [u4InstID]);
    UTIL_Printf("//_pucPic2C[%d] = 0x%x\n", u4InstID, _pucPic2C[u4InstID]);
    UTIL_Printf("//_pucPic3Y[%d] = 0x%x\n", u4InstID, _pucPic3Y [u4InstID]);
    UTIL_Printf("//_pucPic3C[%d] = 0x%x\n", u4InstID, _pucPic3C[u4InstID]);

    {
    UINT32 u4VLDPredSZ = AVS_VLD_PRED_SZ;
    UINT32 u4AVSVLDMvSZ = AVS_VLD_MV_SZ;

    u4VLDPredSZ = ALIGN(u4VLDPredSZ,1024);
    u4AVSVLDMvSZ = ALIGN(u4AVSVLDMvSZ,1024);

    _pucAvsPred[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + u4VLDPredSZ;

    _pucAvsMv1[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + u4AVSVLDMvSZ;

    _pucAvsMv2[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + u4AVSVLDMvSZ;
    UTIL_Printf("//_pucAvsPred[%d] = 0x%x\n", u4InstID, _pucAvsPred[u4InstID]);
    UTIL_Printf("//_pucMv_1[%d] = 0x%x\n", u4InstID, _pucAvsMv1[u4InstID]);
    UTIL_Printf("//_pucMv_2[%d] = 0x%x\n", u4InstID, _pucAvsMv2[u4InstID]);

    if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
    {
        _pucAddressSwapBuf[u4InstID] = ucMemAddr;
        ucMemAddr = ucMemAddr + ADDSWAP_BUF_SZ;
        UTIL_Printf("//_pucAddressSwapBuf[%d] = 0x%x, size:0x%x\n", u4InstID, _pucAddressSwapBuf[u4InstID], ADDSWAP_BUF_SZ);
    }

  #if VDEC_DRAM_BUSY_TEST
    _pucDramBusy[u4InstID] = ucMemAddr;
    ucMemAddr = ucMemAddr + 0x10000;
    UTIL_Printf("//_pucDramBusy[%d] = 0x%x\n", u4InstID, _pucDramBusy[u4InstID]);
  #endif
    }
#else
    ASSERT(0);
#endif
}
#endif


#if VDEC_VP8_EMU

void vMemoryAllocate_VP8(UINT32 u4InstID)
{
#ifdef DYNAMIC_MEMORY_ALLOCATE

#ifdef  SATA_HDD_READ_SUPPORT
   #ifndef  SATA_HDD_FS_SUPPORT
  //if(_pucGoldenFileInfoSa[u4InstID] == NULL)
  {
    _pucGoldenFileInfoSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLDEN_FILE_INFO_SZ,1024, 1);
  }
#endif
#endif

#ifdef LETTERBOX_SUPPORT
  {
    _pucSettingFileSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
    UTIL_Printf("_pucSettingFileSa[u4InstID] = 0x%x\n", _pucSettingFileSa[u4InstID]);
    _pucGoldenFileSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
    UTIL_Printf("_pucGoldenFileSa[u4InstID] = 0x%x\n", _pucGoldenFileSa[u4InstID]);
  }
#endif

 // if(_pucVFifo[u4InstID] == NULL)
  if(u4InstID == 0 && _ucMVCType[1] && _pucVFifo[1])
  {
      _pucVFifo[0] = _pucVFifo[1];
  }
  else
  {
    #if VDEC_SUPPORT_IOMMU
    UTIL_Printf("Test 1\n");
    //_pucVFifo[u4InstID] = (UCHAR *)((BSP_VAllocFragmentMem2(V_FIFO_SZ)));

	//UTIL_Printf("Test _pucVFifo[u4InstID] = 0x%x, size is 0x%lx\n", _pucVFifo[u4InstID], V_FIFO_SZ);
	#else
    //_pucVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(V_FIFO_SZ,1024, 1);
   //_pucVFifo[u4InstID] = (UCHAR *)(VIRTUAL((UINT32)get_drvmem_mem()));
	_pucVFifo[u4InstID] = (UCHAR *)VIRTUAL(BSP_AllocVfyReserveMemory(MEM_CHANNEL_A,MEM_256M, 1024));

#if 0
    UINT32 u4VfifoAddr = (UINT32)get_drvmem_mem();
    UTIL_Printf("Test u4VfifoAddr = 0x%x \n", u4VfifoAddr);

   _pucVFifo[u4InstID] = (UCHAR *)(VIRTUAL(u4VfifoAddr));
#endif

   UTIL_Printf("Test _pucVFifo[u4InstID] = 0x%x, size is 0x%lx, Physical address= 0x%x\n", _pucVFifo[u4InstID], V_FIFO_SZ, PHYSICAL(_pucVFifo[u4InstID]));
	#endif




  }

 //if(_pucFileListSa[u4InstID] == NULL)
 {
   //_pucFileListSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 0);
    #if VDEC_SUPPORT_IOMMU
    UINT32 u4VfifoAddr = (UINT32)get_drvmem_mem();
    UTIL_Printf("Test u4VfifoAddr = 0x%x \n", u4VfifoAddr);

    _pucFileListSa[u4InstID] = (UCHAR *)(VIRTUAL(u4VfifoAddr));
	UTIL_Printf("_pucFileListSa[u4InstID] = 0x%x, size is 0x%lx\n", _pucFileListSa[u4InstID], FILE_LIST_SZ);
	#else
   _pucFileListSa[u4InstID] = _pucVFifo[u4InstID] + V_FIFO_SZ;
   UTIL_Printf("_pucFileListSa[u4InstID] = 0x%x, size is 0x%lx\n", _pucFileListSa[u4InstID], FILE_LIST_SZ);
   #endif

   //_pucVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(V_FIFO_SZ,1024, 1);
    //UTIL_Printf("Test u4VfifoAddr = 0x%x \n", _pucVFifo[u4InstID]);
 }

 // if(_pucDumpYBuf[u4InstID] == NULL)
  {
        //_pucDumpYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLD_Y_SZ,2048, 0/*GOLD_CHANEL_A*/);
		_pucDumpYBuf[u4InstID] = _pucFileListSa[u4InstID] + FILE_LIST_SZ;
        UTIL_Printf("_pucDumpYBuf[u4InstID] = 0x%x, size is 0x%lx\n", _pucDumpYBuf[u4InstID], GOLD_Y_SZ);
  }
  //if(_pucDumpCBuf[u4InstID] == NULL)
  {
        //_pucDumpCBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLD_C_SZ,2048, 0/*GOLD_CHANEL_A*/);
        _pucDumpCBuf[u4InstID] = _pucDumpYBuf[u4InstID] + GOLD_Y_SZ;
        UTIL_Printf("_pucDumpCBuf[u4InstID] = 0x%x, size is 0x%lx\n", _pucDumpCBuf[u4InstID], GOLD_C_SZ);
  }

  //_pucCRCBuf[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(32,16, 0/*GOLD_CHANEL_A*/);
  _pucCRCBuf[u4InstID] =_pucDumpCBuf[u4InstID] + GOLD_C_SZ;
  UTIL_Printf("_pucCRCBuf[u4InstID] = 0x%x\n", _pucCRCBuf[u4InstID]);


  {
  	#if VDEC_SUPPORT_MERGE_CRC
  	//_pucMergeCRCBuf[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(MERGE_CRC_BUF_SZ,16, 0/*GOLD_CHANEL_A*/);
  	_pucMergeCRCBuf[u4InstID] = _pucCRCBuf[u4InstID] + 0x1000;
    UTIL_Printf("_pucMergeCRCBuf[u4InstID] = 0x%x, sizeof is %lx\n", _pucMergeCRCBuf[u4InstID], MERGE_CRC_BUF_SZ);
	_pucPicTolCnt[u4InstID] = _pucMergeCRCBuf[u4InstID] + MERGE_CRC_BUF_SZ;
    UTIL_Printf("_pucPicTolCnt[u4InstID] = 0x%x\n", _pucPicTolCnt[u4InstID]);
    #else
	//_pucPicTolCnt[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(16,2, 0/*GOLD_CHANEL_A*/);
	_pucPicTolCnt[u4InstID] = _pucCRCBuf[u4InstID] + 0x1000;
    UTIL_Printf("_pucPicTolCnt[u4InstID] = 0x%x\n", _pucPicTolCnt[u4InstID]);
	#endif
  }

     _pucDPB[u4InstID] = 0;
     _pucPredSa[u4InstID] = 0;
     #ifdef VERIFICATION_FGT
       _pucFGSeedbase[u4InstID] = 0;
       _pucFGDatabase[u4InstID] = 0;
       _pucFGSEISa[u4InstID] = 0;
      _pucFGTBuf[u4InstID] = 0;
     #endif

      _pucAddressSwapBuf[u4InstID] = 0;

#ifdef DOWN_SCALE_SUPPORT
       _pucVDSCLBuf[u4InstID] = 0;
      _pucVDSCLWorkBuf[u4InstID] = 0;
      _pucVDSCLWork1Sa[u4InstID] = 0;
      _pucVDSCLWork2Sa[u4InstID] = 0;
      _pucVDSCLWork3Sa[u4InstID] = 0;
      _pucVDSCLWork4Sa[u4InstID] = 0;
#endif

#if VDEC_AVS_EMU
{
      _pucPic0Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      _pucPic0C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic1Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic1C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic2Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic2C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic3Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic3C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      UTIL_Printf("_pucPic0Y[u4InstID] = 0x%x\n", _pucPic0Y[u4InstID]);
      UTIL_Printf("_pucPic0C[u4InstID] = 0x%x\n", _pucPic0C[u4InstID]);
      UTIL_Printf("_pucPic1Y[u4InstID] = 0x%x\n", _pucPic1Y[u4InstID]);
      UTIL_Printf("_pucPic1C[u4InstID] = 0x%x\n", _pucPic1C[u4InstID]);
      UTIL_Printf("_pucPic2Y[u4InstID] = 0x%x\n", _pucPic2Y [u4InstID]);
      UTIL_Printf("_pucPic2C[u4InstID] = 0x%x\n", _pucPic2C[u4InstID]);
      UTIL_Printf("_pucPic3Y[u4InstID] = 0x%x\n", _pucPic3Y [u4InstID]);
      UTIL_Printf("_pucPic3C[u4InstID] = 0x%x\n", _pucPic3C[u4InstID]);

      _pucAvsPred[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVS_VLD_PRED_SZ,1024, WORKING_AREA_CHANEL_A);
      _pucAvsMv1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVS_VLD_MV_SZ,1024, WORKING_AREA_CHANEL_A);
      _pucAvsMv2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVS_VLD_MV_SZ,1024, WORKING_AREA_CHANEL_A);
      UTIL_Printf("_pucDcac[u4InstID] = 0x%x\n", _pucAvsPred[u4InstID]);
      UTIL_Printf("_pucMv_1[u4InstID] = 0x%x\n", _pucAvsMv1[u4InstID]);
      UTIL_Printf("_pucMv_2[u4InstID] = 0x%x\n", _pucAvsMv2[u4InstID]);

      if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
      {
          _pucAddressSwapBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(ADDSWAP_BUF_SZ, 2048, WORKING_AREA_CHANEL_A);
          UTIL_Printf("_pucAddressSwapBuf[u4InstID] = 0x%x\n", _pucAddressSwapBuf[u4InstID]);
      }
  }
#endif

#if VDEC_DRAM_BUSY_TEST
     _pucDramBusy[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(0x10000,2048, WORKING_AREA_CHANEL_A);
#endif

#else

#ifdef VERIFICATION_FGT
  _pucFGSeedbase[u4InstID] = (UCHAR *)FGT_SEED_SA;
  _pucFGDatabase[u4InstID] = (UCHAR *)FGT_DATABASE_SA;
  _pucFGSEISa[u4InstID] = (UCHAR *)FGT_SEI_SA;
  _pucFGTBuf[u4InstID] = (UCHAR *)FGT_SA;
#endif

#if 1//def DOWN_SCALE_SUPPORT
  _pucVDSCLBuf[u4InstID] = (UCHAR *)VDSCL_BUF_SA;
  _pucVDSCLWorkBuf[u4InstID] = (UCHAR *)VDSCL_WORK_SA;
  _pucVDSCLWork1Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK1_SA;
  _pucVDSCLWork2Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK2_SA;
  _pucVDSCLWork3Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK3_SA;
  _pucVDSCLWork4Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK4_SA;
#endif
  _pucFileListSa[u4InstID] = (UCHAR*)FILE_LIST_SA;
  _pucVFifo[u4InstID] = (UCHAR *)V_FIFO_SA;
  _pucDPB[u4InstID] = (UCHAR *)DPB_SA;
  _pucPredSa[u4InstID] = (UCHAR *)AVC_PRED_SA;
  _pucDumpYBuf[u4InstID] = (UCHAR *)GOLD_Y_SA;
  _pucDumpCBuf[u4InstID] = (UCHAR *)GOLD_C_SA;
  // WMV Part
  _pucPic0Y[u4InstID] = (UCHAR *)PIC0Y_SA; //UCHAR address
  _pucPic0C[u4InstID] = (UCHAR *)PIC0C_SA;
  _pucPic1Y[u4InstID] = (UCHAR *)PIC1Y_SA;
  _pucPic1C[u4InstID] = (UCHAR *)PIC1C_SA;
  _pucPic2Y[u4InstID] = (UCHAR *)PIC2Y_SA;
  _pucPic2C[u4InstID] = (UCHAR *)PIC2C_SA;
  _pucDcac[u4InstID] = (UCHAR *)DCAC_SA;
  _pucMv_1[u4InstID] = (UCHAR *)Mv_1;
  _pucMv_2[u4InstID] = (UCHAR *)Mv_2;
  _pucBp_1[u4InstID] = (UCHAR *)Bp_1;
  _pucBp_2[u4InstID] = (UCHAR *)Bp_2;
  _pucBp_3[u4InstID] = (UCHAR *)Bp_3;
  _pucBp_4[u4InstID] = (UCHAR *)Bp_4;
  _pucMv_3[u4InstID] = (UCHAR *)Mv_3;
  _pucMv_1_2[u4InstID] = (UCHAR *)Mv_1_2;
  _pucDcac_2[u4InstID] = (UCHAR *)DCAC_2;
  _pucPp_1[u4InstID] = (UCHAR *)DEC_PP_1;
  _pucPp_2[u4InstID] = (UCHAR *)DEC_PP_2;
  // MPEG part
  _pucPpYSa[u4InstID] = (UCHAR *)DEC_PP_Y_SA;
  _pucPpCSa[u4InstID] = (UCHAR *)DEC_PP_C_SA;
  _pucMp4Dcac[u4InstID] = (UCHAR *)MP4_DCAC_SA;
  _pucMp4Mvec[u4InstID] = (UCHAR *)VER_MVEC_SA;
  _pucMp4Bmb1[u4InstID] = (UCHAR *)VER_BMB1_SA;
  _pucMp4Bmb2[u4InstID] = (UCHAR *)VER_BMB2_SA;
  _pucMp4Bcode[u4InstID] = (UCHAR *)BCODE_SA;

    _pucDcacNew[u4InstID] = (UCHAR *)DCAC_NEW_SA;
    _pucMvNew[u4InstID] = (UCHAR *)MV_NEW_SA;
    _pucBp0New[u4InstID] = (UCHAR *)BP_0_NEW_SA;
    _pucBp1New[u4InstID] = (UCHAR *)BP_1_NEW_SA;
    _pucBp2New[u4InstID] = (UCHAR *)BP_2_NEW_SA;
#endif

}

#endif

#if VDEC_VP9_EMU

void vMemoryAllocate_VP9(UINT32 u4InstID)
{    
    UINT32 u4Temp;
    UINT32 u4Align;
    
    UTIL_Printf("========== VP9 memory allocate ==============\n");
    
    _pucFileListSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 0);
   
    UTIL_Printf("_pucFileListSa[u4InstID] = 0x%x, size is 0x%lx\n", _pucFileListSa[u4InstID], FILE_LIST_SZ);   
    
#ifdef CONFIG_DRV_MEM
  	u4Temp = u4GetVFIFOAddr(u4InstID);
    _pucVFifo[u4InstID] = (UCHAR *)(VIRTUAL(u4Temp));
   	UTIL_Printf("[VFIFO%d] Get reserved mem 0x%x\n",u4InstID,u4Temp);
    
    memset(_pucVFifo[u4InstID], 0, V_FIFO_SZ);
	UTIL_Printf("_pucVFifo[u4InstID] = 0x%x, pa = 0x%x\n", _pucVFifo[u4InstID], PHYSICAL(_pucVFifo[u4InstID]));  

    #if VP9_CRC_ENABLE
    _pucVP9CRCYBuf0[u4InstID] =   _pucVFifo[u4InstID] + V_FIFO_SZ;
    _pucVP9CRCCBuf0[u4InstID] =   _pucVP9CRCYBuf0[u4InstID] + VP9_CRC_BUFFER_SZ;  // 512k 
    _pucVP9CRCYBuf1[u4InstID] =   _pucVP9CRCCBuf0[u4InstID] + VP9_CRC_BUFFER_SZ;  // 512k
    _pucVP9CRCCBuf1[u4InstID] =   _pucVP9CRCYBuf1[u4InstID] + VP9_CRC_BUFFER_SZ;  // 512k
    printk("_pucVP9CRCYBuf0[%d] = 0x%x\n", u4InstID, _pucVP9CRCYBuf0[u4InstID]);
    printk("_pucVP9CRCCBuf0[%d] = 0x%x\n", u4InstID, _pucVP9CRCCBuf0[u4InstID]);   
    printk("_pucVP9CRCYBuf1[%d] = 0x%x\n", u4InstID, _pucVP9CRCYBuf1[u4InstID]);
    printk("_pucVP9CRCCBuf1[%d] = 0x%x\n", u4InstID, _pucVP9CRCCBuf1[u4InstID]);
    
    //allocate Seg buffer, for core 0
    _pucSegId[u4InstID][0] = _pucVP9CRCCBuf1[u4InstID] + VP9_CRC_BUFFER_SZ;
    UTIL_Printf("_pucSegId[u4InstID][0] = 0x%x, size is 0x%x\n", _pucSegId[u4InstID][0], VP9_SEG_ID_SZ);    
    
    //allocate Seg buffer, for core 1
    _pucSegId[u4InstID][1] = _pucSegId[u4InstID][0] + VP9_SEG_ID_SZ;
    UTIL_Printf("_pucSegId[u4InstID][1] = 0x%x, size is 0x%x\n", _pucSegId[u4InstID][1], VP9_SEG_ID_SZ);
    
    #else
    //allocate Y golden buffer 
    _pucDumpYBuf[u4InstID] = _pucVFifo[u4InstID] + V_FIFO_SZ;
    UTIL_Printf("_pucDumpYBuf[%d] 0x%08x(0x%08x) size 0x%x\n",u4InstID,_pucDumpYBuf[u4InstID], PHYSICAL(_pucDumpYBuf[u4InstID]),VP9_GOLD_Y_SZ);
    
    //allocate C golden buffer 
    _pucDumpCBuf[u4InstID] = _pucDumpYBuf[u4InstID] + VP9_GOLD_Y_SZ;
    UTIL_Printf("_pucDumpCBuf[%d] 0x%08x(0x%08x) size 0x%x\n",u4InstID,_pucDumpCBuf[u4InstID], PHYSICAL(_pucDumpCBuf[u4InstID]),VP9_GOLD_C_SZ );
    
    //allocate Seg buffer for core 0
    _pucSegId[u4InstID][0] = _pucDumpCBuf[u4InstID] + (VP9_GOLD_C_SZ);
    UTIL_Printf("_pucSegId[u4InstID][0] = 0x%x, size is 0x%x\n", _pucSegId[u4InstID][0], VP9_SEG_ID_SZ);

     //allocate Seg buffer for core 1
    _pucSegId[u4InstID][1] = _pucSegId[u4InstID][0] + (VP9_SEG_ID_SZ);
    UTIL_Printf("_pucSegId[u4InstID][1] = 0x%x, size is 0x%x\n", _pucSegId[u4InstID][1], VP9_SEG_ID_SZ);
    #endif   
     
    //allocate LAE buffer, when dual_core mode assigned
    if(_u4DualCoreEnable[u4InstID])
    {
        _pucLAEBuffer[u4InstID][0] =  _pucSegId[u4InstID][1] +  VP9_SEG_ID_SZ;
        UTIL_Printf("g2LAEBuffer[u4InstID][0] = 0x%08x(0x%08x), size is 0x%x\n", _pucLAEBuffer[u4InstID][0],
                    PHYSICAL(_pucLAEBuffer[u4InstID][0]), VP9_LAE_BUFFER_SZ);
            
        _pucErrBuffer[u4InstID][0] =  _pucLAEBuffer[u4InstID][0] +  VP9_LAE_BUFFER_SZ;
        UTIL_Printf("g2ErrBuffer[u4InstID][0] = 0x%08x(0x%08x), size is 0x%x\n", _pucErrBuffer[u4InstID][0],
                    PHYSICAL(_pucErrBuffer[u4InstID][0]), VP9_ERR_BUFFER_SZ);
        
        //allocate DPB buffer
        _pucDPB[u4InstID] = _pucErrBuffer[u4InstID][0] + VP9_ERR_BUFFER_SZ;
        u4Align = (((UINT32)_pucDPB[u4InstID] + 4095) >> 12 ) << 12;
        
        _pucDPB[u4InstID] = (UCHAR *)u4Align;
        UTIL_Printf("VP9 buffer[%d] 0x%08x size 0x%x\n",u4InstID, _pucDPB[u4InstID], VP9_DPB_SZ);        
    }
    else
    {
        //allocate DPB buffer
        _pucDPB[u4InstID] = _pucSegId[u4InstID][1] + VP9_SEG_ID_SZ;
        UTIL_Printf("VP9 buffer[%d] 0x%08x size 0x%x\n",u4InstID, _pucDPB[u4InstID], VP9_DPB_SZ);                  
    }
    
    //allocate risc buffer
    //[vpx], move to vp9 working buffer allocation
    #if 0
    g2RiscBuffer[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VP9_RISC_BUFFER_SIZE,1024, 1);
    UTIL_Printf("g2RiscBuffer buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2RiscBuffer[u4InstID],VP9_RISC_BUFFER_SIZE);
    #endif
#else
    // fix me here
    #if 0
    //allocate VFIFO buffer
    g2VFIFO[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_VFIFO_SZ,1024, 1);
    UTIL_Printf("VFIFO buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2VFIFO[u4InstID],PATTERN_VFIFO_SZ);
    
    //allocate DPB buffer
    g2DPB[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DPB_SZ,4096, 1);
    UTIL_Printf("DPB buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2DPB[u4InstID],DPB_SZ);
    
    //allocate Y golden buffer 
    g2YGolden[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_Y_GOLDEN_SZ,1024, 1);
    UTIL_Printf("g2YGolden buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2YGolden[u4InstID],PATTERN_Y_GOLDEN_SZ);
    
    //allocate C golden buffer 
    g2CGolden[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_C_GOLDEN_SZ,1024, 1);
    UTIL_Printf("g2CGolden buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2CGolden[u4InstID],PATTERN_C_GOLDEN_SZ);
    
    //allocate Temp buffer 
    g2Temp[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_Y_GOLDEN_SZ,1024, 1);
    UTIL_Printf("g2Temp buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2Temp[u4InstID],PATTERN_Y_GOLDEN_SZ);
    
    //allocate Temp buffer 
    g2MVBuff[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(MV_SZ,1024, 1);
    UTIL_Printf("g2MVBuff buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2MVBuff[u4InstID],MV_SZ);

    //allocate risc buffer
    g2RiscBuffer[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(RISC_BUFFER_SIZE,1024, 1);
    UTIL_Printf("g2RiscBuffer buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2RiscBuffer[u4InstID],RISC_BUFFER_SIZE);
    #endif
#endif
}

#endif

#if VDEC_VP6_EMU
void vMemoryAllocate_VP6(UINT32 u4InstID)
{
#ifdef DYNAMIC_MEMORY_ALLOCATE

#ifdef  SATA_HDD_READ_SUPPORT
   #ifndef  SATA_HDD_FS_SUPPORT
  //if(_pucGoldenFileInfoSa[u4InstID] == NULL)
  {
    _pucGoldenFileInfoSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLDEN_FILE_INFO_SZ,1024, 1);
  }
#endif
#endif

#ifdef LETTERBOX_SUPPORT
  {
    _pucSettingFileSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
    UTIL_Printf("_pucSettingFileSa[u4InstID] = 0x%x\n", _pucSettingFileSa[u4InstID]);
    _pucGoldenFileSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
    UTIL_Printf("_pucGoldenFileSa[u4InstID] = 0x%x\n", _pucGoldenFileSa[u4InstID]);
  }
#endif


 
 // if(_pucVFifo[u4InstID] == NULL)
  if(u4InstID == 0 && _ucMVCType[1] && _pucVFifo[1])
  {
      _pucVFifo[0] = _pucVFifo[1];
  }
  else
  {
    #if VDEC_SUPPORT_IOMMU
    //UTIL_Printf("Test 1\n");
    //_pucVFifo[u4InstID] = (UCHAR *)((BSP_VAllocFragmentMem(V_FIFO_SZ)));
	#else
    //_pucVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(V_FIFO_SZ,1024, 1);
   //_pucVFifo[u4InstID] = (UCHAR *)(VIRTUAL((UINT32)get_drvmem_mem()));

	_pucVFifo[u4InstID] = (UCHAR *)VIRTUAL(BSP_AllocVfyReserveMemory(MEM_CHANNEL_A,MEM_256M, 1024));
#if 0
    UINT32 u4VfifoAddr = (UINT32)get_drvmem_mem();
    UTIL_Printf("Test u4VfifoAddr = 0x%x \n", u4VfifoAddr);

   _pucVFifo[u4InstID] = (UCHAR *)(VIRTUAL(u4VfifoAddr));
#endif
   UTIL_Printf("Test _pucVFifo[u4InstID] = 0x%x, size is 0x%lx, Physical address= 0x%x\n", _pucVFifo[u4InstID], V_FIFO_SZ, PHYSICAL(_pucVFifo[u4InstID]));
   #endif


  }

 //if(_pucFileListSa[u4InstID] == NULL)
 {
   //_pucFileListSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 0);
   #if VDEC_SUPPORT_IOMMU
    UINT32 u4VfifoAddr = (UINT32)get_drvmem_mem();
    UTIL_Printf("Test u4VfifoAddr = 0x%x \n", u4VfifoAddr);

    _pucFileListSa[u4InstID] = (UCHAR *)(VIRTUAL(u4VfifoAddr));
	UTIL_Printf("_pucFileListSa[u4InstID] = 0x%x, size is 0x%lx\n", _pucFileListSa[u4InstID], FILE_LIST_SZ);
	#else
   _pucFileListSa[u4InstID] = _pucVFifo[u4InstID] + V_FIFO_SZ;
   UTIL_Printf("_pucFileListSa[u4InstID] = 0x%x, size is 0x%lx\n", _pucFileListSa[u4InstID], FILE_LIST_SZ);
   #endif

   //_pucVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(V_FIFO_SZ,1024, 1);
   //UTIL_Printf("Test u4VfifoAddr = 0x%x \n", _pucVFifo[u4InstID]);
 }

 // if(_pucDumpYBuf[u4InstID] == NULL)
  {
        //_pucDumpYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLD_Y_SZ,2048, 0/*GOLD_CHANEL_A*/);
		_pucDumpYBuf[u4InstID] = _pucFileListSa[u4InstID] + FILE_LIST_SZ;
        UTIL_Printf("_pucDumpYBuf[u4InstID] = 0x%x, size is 0x%lx\n", _pucDumpYBuf[u4InstID], GOLD_Y_SZ);
  }
  //if(_pucDumpCBuf[u4InstID] == NULL)
  {
        //_pucDumpCBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLD_C_SZ,2048, 0/*GOLD_CHANEL_A*/);
        _pucDumpCBuf[u4InstID] = _pucDumpYBuf[u4InstID] + GOLD_Y_SZ;
        UTIL_Printf("_pucDumpCBuf[u4InstID] = 0x%x, size is 0x%lx\n", _pucDumpCBuf[u4InstID], GOLD_C_SZ);
  }

  //_pucCRCBuf[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(32,16, 0/*GOLD_CHANEL_A*/);
  _pucCRCBuf[u4InstID] =_pucDumpCBuf[u4InstID] + GOLD_C_SZ;
  UTIL_Printf("_pucCRCBuf[u4InstID] = 0x%x\n", _pucCRCBuf[u4InstID]);


  {
  	#if VDEC_SUPPORT_MERGE_CRC
  	//_pucMergeCRCBuf[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(MERGE_CRC_BUF_SZ,16, 0/*GOLD_CHANEL_A*/);
  	_pucMergeCRCBuf[u4InstID] = _pucCRCBuf[u4InstID] + 0x1000;
    UTIL_Printf("_pucMergeCRCBuf[u4InstID] = 0x%x, sizeof is %lx\n", _pucMergeCRCBuf[u4InstID], MERGE_CRC_BUF_SZ);
	_pucPicTolCnt[u4InstID] = _pucMergeCRCBuf[u4InstID] + MERGE_CRC_BUF_SZ;
    UTIL_Printf("_pucPicTolCnt[u4InstID] = 0x%x\n", _pucPicTolCnt[u4InstID]);
    #else
	//_pucPicTolCnt[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(16,2, 0/*GOLD_CHANEL_A*/);
	_pucPicTolCnt[u4InstID] = _pucCRCBuf[u4InstID] + 0x1000;
    UTIL_Printf("_pucPicTolCnt[u4InstID] = 0x%x\n", _pucPicTolCnt[u4InstID]);
	#endif
  }

     _pucDPB[u4InstID] = 0;
     _pucPredSa[u4InstID] = 0;
     #ifdef VERIFICATION_FGT
       _pucFGSeedbase[u4InstID] = 0;
       _pucFGDatabase[u4InstID] = 0;
       _pucFGSEISa[u4InstID] = 0;
      _pucFGTBuf[u4InstID] = 0;
     #endif

      _pucAddressSwapBuf[u4InstID] = 0;

#ifdef DOWN_SCALE_SUPPORT
       _pucVDSCLBuf[u4InstID] = 0;
      _pucVDSCLWorkBuf[u4InstID] = 0;
      _pucVDSCLWork1Sa[u4InstID] = 0;
      _pucVDSCLWork2Sa[u4InstID] = 0;
      _pucVDSCLWork3Sa[u4InstID] = 0;
      _pucVDSCLWork4Sa[u4InstID] = 0;
#endif

#if VDEC_AVS_EMU
{
      _pucPic0Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      _pucPic0C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic1Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic1C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic2Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic2C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic3Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic3C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      UTIL_Printf("_pucPic0Y[u4InstID] = 0x%x\n", _pucPic0Y[u4InstID]);
      UTIL_Printf("_pucPic0C[u4InstID] = 0x%x\n", _pucPic0C[u4InstID]);
      UTIL_Printf("_pucPic1Y[u4InstID] = 0x%x\n", _pucPic1Y[u4InstID]);
      UTIL_Printf("_pucPic1C[u4InstID] = 0x%x\n", _pucPic1C[u4InstID]);
      UTIL_Printf("_pucPic2Y[u4InstID] = 0x%x\n", _pucPic2Y [u4InstID]);
      UTIL_Printf("_pucPic2C[u4InstID] = 0x%x\n", _pucPic2C[u4InstID]);
      UTIL_Printf("_pucPic3Y[u4InstID] = 0x%x\n", _pucPic3Y [u4InstID]);
      UTIL_Printf("_pucPic3C[u4InstID] = 0x%x\n", _pucPic3C[u4InstID]);

      _pucAvsPred[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVS_VLD_PRED_SZ,1024, WORKING_AREA_CHANEL_A);
      _pucAvsMv1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVS_VLD_MV_SZ,1024, WORKING_AREA_CHANEL_A);
      _pucAvsMv2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVS_VLD_MV_SZ,1024, WORKING_AREA_CHANEL_A);
      UTIL_Printf("_pucDcac[u4InstID] = 0x%x\n", _pucAvsPred[u4InstID]);
      UTIL_Printf("_pucMv_1[u4InstID] = 0x%x\n", _pucAvsMv1[u4InstID]);
      UTIL_Printf("_pucMv_2[u4InstID] = 0x%x\n", _pucAvsMv2[u4InstID]);

      if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
      {
          _pucAddressSwapBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(ADDSWAP_BUF_SZ, 2048, WORKING_AREA_CHANEL_A);
          UTIL_Printf("_pucAddressSwapBuf[u4InstID] = 0x%x\n", _pucAddressSwapBuf[u4InstID]);
      }
  }
#endif

#if VDEC_DRAM_BUSY_TEST
     _pucDramBusy[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(0x10000,2048, WORKING_AREA_CHANEL_A);
#endif

#else

#ifdef VERIFICATION_FGT
  _pucFGSeedbase[u4InstID] = (UCHAR *)FGT_SEED_SA;
  _pucFGDatabase[u4InstID] = (UCHAR *)FGT_DATABASE_SA;
  _pucFGSEISa[u4InstID] = (UCHAR *)FGT_SEI_SA;
  _pucFGTBuf[u4InstID] = (UCHAR *)FGT_SA;
#endif

#if 1//def DOWN_SCALE_SUPPORT
  _pucVDSCLBuf[u4InstID] = (UCHAR *)VDSCL_BUF_SA;
  _pucVDSCLWorkBuf[u4InstID] = (UCHAR *)VDSCL_WORK_SA;
  _pucVDSCLWork1Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK1_SA;
  _pucVDSCLWork2Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK2_SA;
  _pucVDSCLWork3Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK3_SA;
  _pucVDSCLWork4Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK4_SA;
#endif
  _pucFileListSa[u4InstID] = (UCHAR*)FILE_LIST_SA;
  _pucVFifo[u4InstID] = (UCHAR *)V_FIFO_SA;
  _pucDPB[u4InstID] = (UCHAR *)DPB_SA;
  _pucPredSa[u4InstID] = (UCHAR *)AVC_PRED_SA;
  _pucDumpYBuf[u4InstID] = (UCHAR *)GOLD_Y_SA;
  _pucDumpCBuf[u4InstID] = (UCHAR *)GOLD_C_SA;
  // WMV Part
  _pucPic0Y[u4InstID] = (UCHAR *)PIC0Y_SA; //UCHAR address
  _pucPic0C[u4InstID] = (UCHAR *)PIC0C_SA;
  _pucPic1Y[u4InstID] = (UCHAR *)PIC1Y_SA;
  _pucPic1C[u4InstID] = (UCHAR *)PIC1C_SA;
  _pucPic2Y[u4InstID] = (UCHAR *)PIC2Y_SA;
  _pucPic2C[u4InstID] = (UCHAR *)PIC2C_SA;
  _pucDcac[u4InstID] = (UCHAR *)DCAC_SA;
  _pucMv_1[u4InstID] = (UCHAR *)Mv_1;
  _pucMv_2[u4InstID] = (UCHAR *)Mv_2;
  _pucBp_1[u4InstID] = (UCHAR *)Bp_1;
  _pucBp_2[u4InstID] = (UCHAR *)Bp_2;
  _pucBp_3[u4InstID] = (UCHAR *)Bp_3;
  _pucBp_4[u4InstID] = (UCHAR *)Bp_4;
  _pucMv_3[u4InstID] = (UCHAR *)Mv_3;
  _pucMv_1_2[u4InstID] = (UCHAR *)Mv_1_2;
  _pucDcac_2[u4InstID] = (UCHAR *)DCAC_2;
  _pucPp_1[u4InstID] = (UCHAR *)DEC_PP_1;
  _pucPp_2[u4InstID] = (UCHAR *)DEC_PP_2;
  // MPEG part
  _pucPpYSa[u4InstID] = (UCHAR *)DEC_PP_Y_SA;
  _pucPpCSa[u4InstID] = (UCHAR *)DEC_PP_C_SA;
  _pucMp4Dcac[u4InstID] = (UCHAR *)MP4_DCAC_SA;
  _pucMp4Mvec[u4InstID] = (UCHAR *)VER_MVEC_SA;
  _pucMp4Bmb1[u4InstID] = (UCHAR *)VER_BMB1_SA;
  _pucMp4Bmb2[u4InstID] = (UCHAR *)VER_BMB2_SA;
  _pucMp4Bcode[u4InstID] = (UCHAR *)BCODE_SA;

    _pucDcacNew[u4InstID] = (UCHAR *)DCAC_NEW_SA;
    _pucMvNew[u4InstID] = (UCHAR *)MV_NEW_SA;
    _pucBp0New[u4InstID] = (UCHAR *)BP_0_NEW_SA;
    _pucBp1New[u4InstID] = (UCHAR *)BP_1_NEW_SA;
    _pucBp2New[u4InstID] = (UCHAR *)BP_2_NEW_SA;
#endif

}


#endif

#ifdef VDEC_WMV_EMU
void vMemoryAllocate_WMV(UINT32 u4InstID)
{
#ifdef DYNAMIC_MEMORY_ALLOCATE

#ifdef  SATA_HDD_READ_SUPPORT
#ifndef  SATA_HDD_FS_SUPPORT
		_pucGoldenFileInfoSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLDEN_FILE_INFO_SZ,1024, 1);
    #endif //SATA_HDD_FS_SUPPORT
#endif //SATA_HDD_READ_SUPPORT

#ifdef LETTERBOX_SUPPORT
		_pucSettingFileSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
		UTIL_Printf("_pucSettingFileSa[u4InstID] = 0x%x\n", _pucSettingFileSa[u4InstID]);
		_pucGoldenFileSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
		UTIL_Printf("_pucGoldenFileSa[u4InstID] = 0x%x\n", _pucGoldenFileSa[u4InstID]);
#endif //LETTERBOX_SUPPORT

	// if(_pucVFifo[u4InstID] == NULL)
		if(u4InstID == 0 && _ucMVCType[1] && _pucVFifo[1])
		{
			_pucVFifo[0] = _pucVFifo[1];
		}
		else
		{
	#ifdef CONFIG_DRV_MEM
			//reserve 148MB
	#ifdef AVC_HIGH_RESOLUTION
			_pucVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(V_FIFO_SZ,1024, 1);
	#else
            _pucVFifo[u4InstID] = (UCHAR *)VIRTUAL(BSP_AllocVfyReserveMemory(MEM_CHANNEL_B,MEM_256M, 1024));
	#endif
	#else
			_pucVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(V_FIFO_SZ,1024, 1);
	#endif
			memset(_pucVFifo[u4InstID],0,V_FIFO_SZ);
			UTIL_Printf("_pucVFifo[u4InstID] = 0x%x, pa = 0x%x\n", _pucVFifo[u4InstID], PHYSICAL(_pucVFifo[u4InstID]));
		}
	 {
	   //_pucFileListSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 0);
	   _pucFileListSa[u4InstID] = _pucVFifo[u4InstID] + V_FIFO_SZ;
	   UTIL_Printf("_pucFileListSa[u4InstID] = 0x%x, size is 0x%lx, PHYSICAL:0x%x\n", _pucFileListSa[u4InstID], FILE_LIST_SZ, PHYSICAL(_pucFileListSa[u4InstID]));
	 }

#ifdef CONFIG_DRV_MEM
		if(u4InstID == 0 && _ucMVCType[1])
		{
			_pucDumpYBuf[u4InstID] =  _pucFileListSa[u4InstID] + FILE_LIST_SZ; //_pucAVCMVBuff_Sub[16] + ((1920*1088) / 4);
			UTIL_Printf("_pucDumpYBuf[%d] = 0x%x, pa = 0x%x\n", u4InstID, _pucDumpYBuf[u4InstID], PHYSICAL(_pucDumpYBuf[u4InstID]));
			_pucDumpCBuf[u4InstID] = _pucDumpYBuf[u4InstID] + GOLD_Y_SZ;
			UTIL_Printf("_pucDumpCBuf[%d] = 0x%x, pa = 0x%x\n", u4InstID, _pucDumpCBuf[u4InstID], PHYSICAL(_pucDumpCBuf[u4InstID]));
		}
		else
		{
			_pucDumpYBuf[u4InstID] =  _pucFileListSa[u4InstID] + FILE_LIST_SZ; //_pucVFifo[u4InstID] +	V_FIFO_SZ;
			UTIL_Printf("_pucDumpYBuf[%d] = 0x%x, pa = 0x%x\n", u4InstID, _pucDumpYBuf[u4InstID], PHYSICAL(_pucDumpYBuf[u4InstID]));
			_pucDumpCBuf[u4InstID] = _pucDumpYBuf[u4InstID] + GOLD_Y_SZ;
			UTIL_Printf("_pucDumpCBuf[%d] = 0x%x, pa = 0x%x\n", u4InstID, _pucDumpCBuf[u4InstID], PHYSICAL(_pucDumpCBuf[u4InstID]));
		}

	  //_pucCRCBuf[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(32,16, 0/*GOLD_CHANEL_A*/);
	  _pucCRCBuf[u4InstID] =_pucDumpCBuf[u4InstID] + GOLD_C_SZ;
	  UTIL_Printf("_pucCRCBuf[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucCRCBuf[u4InstID], PHYSICAL(_pucCRCBuf[u4InstID]));
	  x_memset((UCHAR*) ((UINT32)_pucDumpYBuf[u4InstID]), 0, GOLD_Y_SZ);
	  x_memset((UCHAR*) ((UINT32)_pucDumpYBuf[u4InstID]), 0, GOLD_C_SZ);
#endif

	  {
  	#if VDEC_SUPPORT_MERGE_CRC
		//_pucMergeCRCBuf[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(MERGE_CRC_BUF_SZ,16, 0/*GOLD_CHANEL_A*/);
		_pucMergeCRCBuf[u4InstID] = _pucCRCBuf[u4InstID] + 0x1000;
		x_memset((UCHAR*) ((UINT32)_pucMergeCRCBuf[u4InstID]), 0, MERGE_CRC_BUF_SZ);
		UTIL_Printf("_pucMergeCRCBuf[u4InstID] = 0x%x, sizeof is %lx, PHYSICAL:0x%x\n", _pucMergeCRCBuf[u4InstID], MERGE_CRC_BUF_SZ, PHYSICAL(_pucMergeCRCBuf[u4InstID]));
			//_pucPicTolCnt[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(16,2, 0/*GOLD_CHANEL_A*/);
		_pucPicTolCnt[u4InstID] = _pucMergeCRCBuf[u4InstID] + MERGE_CRC_BUF_SZ;
		UTIL_Printf("_pucPicTolCnt[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPicTolCnt[u4InstID],  PHYSICAL(_pucPicTolCnt[u4InstID]));

	#else
			//_pucPicTolCnt[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(16,2, 0/*GOLD_CHANEL_A*/);
		_pucPicTolCnt[u4InstID] = _pucCRCBuf[u4InstID] + 0x1000;
		UTIL_Printf("_pucPicTolCnt[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPicTolCnt[u4InstID],  PHYSICAL(_pucPicTolCnt[u4InstID]));
    #endif

	  }

		_pucDPB[u4InstID] = 0;
		_pucPredSa[u4InstID] = 0;
#ifdef VERIFICATION_FGT
		_pucFGSeedbase[u4InstID] = 0;
		_pucFGDatabase[u4InstID] = 0;
		_pucFGSEISa[u4InstID] = 0;
		_pucFGTBuf[u4InstID] = 0;
#endif
		_pucFGSEISa[u4InstID] = 0;
		_pucAddressSwapBuf[u4InstID] = 0;

#ifdef DOWN_SCALE_SUPPORT
		_pucVDSCLBuf[u4InstID] = 0;
		_pucVDSCLWorkBuf[u4InstID] = 0;
		_pucVDSCLWork1Sa[u4InstID] = 0;
		_pucVDSCLWork2Sa[u4InstID] = 0;
		_pucVDSCLWork3Sa[u4InstID] = 0;
		_pucVDSCLWork4Sa[u4InstID] = 0;
#endif //DOWN_SCALE_SUPPORT

#if VDEC_DRAM_BUSY_TEST
		_pucDramBusy[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(0x10000,2048, WORKING_AREA_CHANEL_A);
#endif

#else // DYNAMIC_MEMORY_ALLOCATE

#ifdef VERIFICATION_FGT
		_pucFGSeedbase[u4InstID] = (UCHAR *)FGT_SEED_SA;
		_pucFGDatabase[u4InstID] = (UCHAR *)FGT_DATABASE_SA;
		_pucFGSEISa[u4InstID] = (UCHAR *)FGT_SEI_SA;
		_pucFGTBuf[u4InstID] = (UCHAR *)FGT_SA;
#endif
	_pucFGSEISa[u4InstID] = (UCHAR *)FGT_SEI_SA;

#if 1//def DOWN_SCALE_SUPPORT
		_pucVDSCLBuf[u4InstID] = (UCHAR *)VDSCL_BUF_SA;
		_pucVDSCLWorkBuf[u4InstID] = (UCHAR *)VDSCL_WORK_SA;
		_pucVDSCLWork1Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK1_SA;
		_pucVDSCLWork2Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK2_SA;
		_pucVDSCLWork3Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK3_SA;
		_pucVDSCLWork4Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK4_SA;
#endif
		_pucFileListSa[u4InstID] = (UCHAR*)FILE_LIST_SA;
		_pucVFifo[u4InstID] = (UCHAR *)V_FIFO_SA;
		_pucDPB[u4InstID] = (UCHAR *)DPB_SA;
		_pucPredSa[u4InstID] = (UCHAR *)AVC_PRED_SA;
		_pucDumpYBuf[u4InstID] = (UCHAR *)GOLD_Y_SA;
		_pucDumpCBuf[u4InstID] = (UCHAR *)GOLD_C_SA;
		// WMV Part
		_pucPic0Y[u4InstID] = (UCHAR *)PIC0Y_SA; //UCHAR address
		_pucPic0C[u4InstID] = (UCHAR *)PIC0C_SA;
		_pucPic1Y[u4InstID] = (UCHAR *)PIC1Y_SA;
		_pucPic1C[u4InstID] = (UCHAR *)PIC1C_SA;
		_pucPic2Y[u4InstID] = (UCHAR *)PIC2Y_SA;
		_pucPic2C[u4InstID] = (UCHAR *)PIC2C_SA;
		_pucDcac[u4InstID] = (UCHAR *)DCAC_SA;
		_pucMv_1[u4InstID] = (UCHAR *)Mv_1;
		_pucMv_2[u4InstID] = (UCHAR *)Mv_2;
		_pucBp_1[u4InstID] = (UCHAR *)Bp_1;
		_pucBp_2[u4InstID] = (UCHAR *)Bp_2;
		_pucBp_3[u4InstID] = (UCHAR *)Bp_3;
		_pucBp_4[u4InstID] = (UCHAR *)Bp_4;
		_pucMv_3[u4InstID] = (UCHAR *)Mv_3;
		_pucMv_1_2[u4InstID] = (UCHAR *)Mv_1_2;
		_pucDcac_2[u4InstID] = (UCHAR *)DCAC_2;
		_pucPp_1[u4InstID] = (UCHAR *)DEC_PP_1;
		_pucPp_2[u4InstID] = (UCHAR *)DEC_PP_2;
		// MPEG part
		_pucPpYSa[u4InstID] = (UCHAR *)DEC_PP_Y_SA;
		_pucPpCSa[u4InstID] = (UCHAR *)DEC_PP_C_SA;
		_pucMp4Dcac[u4InstID] = (UCHAR *)MP4_DCAC_SA;
		_pucMp4Mvec[u4InstID] = (UCHAR *)VER_MVEC_SA;
		_pucMp4Bmb1[u4InstID] = (UCHAR *)VER_BMB1_SA;
		_pucMp4Bmb2[u4InstID] = (UCHAR *)VER_BMB2_SA;
		_pucMp4Bcode[u4InstID] = (UCHAR *)BCODE_SA;

		_pucDcacNew[u4InstID] = (UCHAR *)DCAC_NEW_SA;
		_pucMvNew[u4InstID] = (UCHAR *)MV_NEW_SA;
		_pucBp0New[u4InstID] = (UCHAR *)BP_0_NEW_SA;
		_pucBp1New[u4InstID] = (UCHAR *)BP_1_NEW_SA;
		_pucBp2New[u4InstID] = (UCHAR *)BP_2_NEW_SA;

    #endif// DYNAMIC_MEMORY_ALLOCATE
}
#endif

// *********************************************************************
// Function    : void vMemoryAllocate(UINT32 u4InstID)
// Description : allocate memory
// Parameter   : None
// Return      : None
// *********************************************************************
void vMemoryAllocate(UINT32 u4InstID)
{
    UINT32 u4Temp;
#ifdef VDEC_VIDEOCODEC_RM
  vMemoryAllocate_RM(u4InstID);    //RM Decoder Memory Allocation Path
  #if VDEC_DRAM_BUSY_TEST
  _pucDramBusy[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(0x10000,2048, WORKING_AREA_CHANEL_A);
  UTIL_Printf("_pucDramBusy[u4InstID] = 0x%x\n", _pucDramBusy[u4InstID]);
  #endif
#else //VDEC_VIDEOCODEC_RM

#if VDEC_WMV_EMU
	vMemoryAllocate_WMV(u4InstID);
#else

#if VDEC_AVS_EMU
    vMemoryAllocate_AVS(u4InstID);
#else //VDEC_AVS_EMU
#if VDEC_MPEG_EMU
    vMemoryAllocate_MPEG(u4InstID);
#else
#if VDEC_VP8_EMU
    vMemoryAllocate_VP8(u4InstID);
#else
#if VDEC_VP9_EMU
    vMemoryAllocate_VP9(u4InstID);
#else
#if VDEC_VP6_EMU
    vMemoryAllocate_VP6(u4InstID);
#else //VDEC_MPEG_VP6

#ifdef DYNAMIC_MEMORY_ALLOCATE

#ifdef  SATA_HDD_READ_SUPPORT
#ifndef  SATA_HDD_FS_SUPPORT
	_pucGoldenFileInfoSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(GOLDEN_FILE_INFO_SZ,1024, 1);
    #endif //SATA_HDD_FS_SUPPORT
#endif //SATA_HDD_READ_SUPPORT

#ifdef LETTERBOX_SUPPORT
	_pucSettingFileSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
	UTIL_Printf("_pucSettingFileSa[u4InstID] = 0x%x\n", _pucSettingFileSa[u4InstID]);
	_pucGoldenFileSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 1);
	UTIL_Printf("_pucGoldenFileSa[u4InstID] = 0x%x\n", _pucGoldenFileSa[u4InstID]);
#endif //LETTERBOX_SUPPORT

// if(_pucVFifo[u4InstID] == NULL)
	if(u4InstID == 0 && _ucMVCType[1] && _pucVFifo[1])
	{
		_pucVFifo[0] = _pucVFifo[1];
	}
	else
	{
	    #ifdef CONFIG_DRV_MEM
		//reserve 148MB
		_pucVFifo[u4InstID] = (UCHAR *)VIRTUAL(BSP_AllocVfyReserveMemory(MEM_CHANNEL_B,MEM_256M, 1024));
	    #else
		_pucVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(V_FIFO_SZ,1024, 1);
	    #endif
		memset(_pucVFifo[u4InstID],0,V_FIFO_SZ);
		UTIL_Printf("_pucVFifo[u4InstID] = 0x%x, pa = 0x%x\n", _pucVFifo[u4InstID], PHYSICAL(_pucVFifo[u4InstID]));
	}
 {
   _pucFileListSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FILE_LIST_SZ,1024, 0);
   //_pucFileListSa[u4InstID] = _pucVFifo[u4InstID] + V_FIFO_SZ;
   UTIL_Printf("_pucFileListSa[u4InstID] = 0x%x, size is 0x%lx, PHYSICAL:0x%x\n", _pucFileListSa[u4InstID], FILE_LIST_SZ, PHYSICAL(_pucFileListSa[u4InstID]));
 }

#ifdef CONFIG_DRV_MEM
    UINT32 u4DumpYSize = GOLD_Y_SZ;
    UINT32 u4DumpCSize = GOLD_C_SZ;
    if(_u4CRCMode == 1)
    {
      u4DumpYSize = GOLD_Y_SZ;
      u4DumpCSize = GOLD_C_SZ;
    }

	if(u4InstID == 0 && _ucMVCType[1])
	{
		_pucDumpYBuf[u4InstID] =  _pucAVCMVBuff_Sub[16] + ((1920*1088) / 4);
		UTIL_Printf("_pucDumpYBuf[%d] = 0x%x, pa = 0x%x\n", u4InstID, _pucDumpYBuf[u4InstID], PHYSICAL(_pucDumpYBuf[u4InstID]));
        _pucDumpCBuf[u4InstID] = _pucDumpYBuf[u4InstID] + u4DumpYSize;
		UTIL_Printf("_pucDumpCBuf[%d] = 0x%x, pa = 0x%x\n", u4InstID, _pucDumpCBuf[u4InstID], PHYSICAL(_pucDumpCBuf[u4InstID]));
	}
	else
	{
		_pucDumpYBuf[u4InstID] =  _pucVFifo[u4InstID] +  V_FIFO_SZ;
		UTIL_Printf("_pucDumpYBuf[%d] = 0x%x, pa = 0x%x\n", u4InstID, _pucDumpYBuf[u4InstID], PHYSICAL(_pucDumpYBuf[u4InstID]));
        _pucDumpCBuf[u4InstID] = _pucDumpYBuf[u4InstID] + u4DumpYSize;
		UTIL_Printf("_pucDumpCBuf[%d] = 0x%x, pa = 0x%x\n", u4InstID, _pucDumpCBuf[u4InstID], PHYSICAL(_pucDumpCBuf[u4InstID]));
	}

  //_pucCRCBuf[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(32,16, 0/*GOLD_CHANEL_A*/);
    if(_u4CRCMode == 1)
    {
        _pucCRCBuf[u4InstID] =_pucDumpCBuf[u4InstID] + u4DumpCSize;
        UTIL_Printf("_pucCRCBuf[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucCRCBuf[u4InstID], PHYSICAL(_pucCRCBuf[u4InstID]));
    }
    x_memset((UCHAR*) ((UINT32)_pucDumpYBuf[u4InstID]), 0, u4DumpYSize);
    x_memset((UCHAR*) ((UINT32)_pucDumpCBuf[u4InstID]), 0, u4DumpCSize);
#endif

  {
  	#if VDEC_SUPPORT_MERGE_CRC
  	//_pucMergeCRCBuf[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(MERGE_CRC_BUF_SZ,16, 0/*GOLD_CHANEL_A*/);
  	_pucMergeCRCBuf[u4InstID] = _pucCRCBuf[u4InstID] + 0x1000;
	x_memset((UCHAR*) ((UINT32)_pucMergeCRCBuf[u4InstID]), 0, MERGE_CRC_BUF_SZ);
    UTIL_Printf("_pucMergeCRCBuf[u4InstID] = 0x%x, sizeof is %lx, PHYSICAL:0x%x\n", _pucMergeCRCBuf[u4InstID], MERGE_CRC_BUF_SZ, PHYSICAL(_pucMergeCRCBuf[u4InstID]));
   // #endif
	//_pucPicTolCnt[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(16,2, 0/*GOLD_CHANEL_A*/);
	_pucPicTolCnt[u4InstID] = _pucMergeCRCBuf[u4InstID] + MERGE_CRC_BUF_SZ;
    UTIL_Printf("_pucPicTolCnt[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPicTolCnt[u4InstID],  PHYSICAL(_pucPicTolCnt[u4InstID]));

	#else
	//_pucPicTolCnt[u4InstID]= (UCHAR *)x_alloc_aligned_verify_mem(16,2, 0/*GOLD_CHANEL_A*/);
	_pucPicTolCnt[u4InstID] = _pucCRCBuf[u4InstID] + 0x1000;
    UTIL_Printf("_pucPicTolCnt[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPicTolCnt[u4InstID],  PHYSICAL(_pucPicTolCnt[u4InstID]));
#endif
  }

#if AVC_NEW_CRC_COMPARE
  _pucH264CRCYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(400*1024, 1024, 1);
  printk("_pucH264CRCYBuf[u4InstID] = 0x%x\n", _pucH264CRCYBuf[u4InstID]);
#endif //AVC_NEW_CRC_COMPARE


//	#if HEVC_CRC_COMPARE
    if(_u4CRCMode == 1)
    {
#ifdef CONFIG_DRV_MEM
        _pucH265CRCYBuf0[u4InstID] =   _pucDumpYBuf[u4InstID];                // 100k
        _pucH265CRCCBuf0[u4InstID] =   _pucH265CRCYBuf0[u4InstID] + 0x19000;  // 100k
        _pucH265CRCYBuf1[u4InstID] =   _pucH265CRCCBuf0[u4InstID] + 0x19000;  // 100k
        _pucH265CRCCBuf1[u4InstID] =   _pucH265CRCYBuf1[u4InstID] + 0x19000;  // 100k
        printk("_pucH265CRCYBuf0[u4InstID] = 0x%x\n", _pucH265CRCYBuf0[u4InstID]);
        printk("_pucH265CRCCBuf0[u4InstID] = 0x%x\n", _pucH265CRCCBuf0[u4InstID]);
        printk("_pucH265CRCYBuf1[u4InstID] = 0x%x\n", _pucH265CRCYBuf1[u4InstID]);
        printk("_pucH265CRCCBuf1[u4InstID] = 0x%x\n", _pucH265CRCCBuf1[u4InstID]);
#else
        _pucH265CRCYBuf0[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(100*1024, 1024, 1);
        _pucH265CRCCBuf0[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(100*1024, 1024, 1);
        _pucH265CRCYBuf1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(100*1024, 1024, 1);
        _pucH265CRCCBuf1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(100*1024, 1024, 1);
        printk("_pucH265CRCYBuf0[u4InstID] = 0x%x\n", _pucH265CRCYBuf0[u4InstID]);
        printk("_pucH265CRCCBuf0[u4InstID] = 0x%x\n", _pucH265CRCCBuf0[u4InstID]);
        printk("_pucH265CRCYBuf1[u4InstID] = 0x%x\n", _pucH265CRCYBuf1[u4InstID]);
        printk("_pucH265CRCCBuf1[u4InstID] = 0x%x\n", _pucH265CRCCBuf1[u4InstID]);
#endif
    }
//	#endif

	_pucDPB[u4InstID] = 0;
	_pucPredSa[u4InstID] = 0;
#ifdef VERIFICATION_FGT
	_pucFGSeedbase[u4InstID] = 0;
	_pucFGDatabase[u4InstID] = 0;
	_pucFGSEISa[u4InstID] = 0;
	_pucFGTBuf[u4InstID] = 0;
#endif
	_pucFGSEISa[u4InstID] = 0;
	_pucAddressSwapBuf[u4InstID] = 0;

#ifdef DOWN_SCALE_SUPPORT
	_pucVDSCLBuf[u4InstID] = 0;
	_pucVDSCLWorkBuf[u4InstID] = 0;
	_pucVDSCLWork1Sa[u4InstID] = 0;
	_pucVDSCLWork2Sa[u4InstID] = 0;
	_pucVDSCLWork3Sa[u4InstID] = 0;
	_pucVDSCLWork4Sa[u4InstID] = 0;
#endif //DOWN_SCALE_SUPPORT

#if VDEC_DRAM_BUSY_TEST
	_pucDramBusy[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(0x10000,2048, WORKING_AREA_CHANEL_A);
#endif

#else // DYNAMIC_MEMORY_ALLOCATE

#ifdef VERIFICATION_FGT
	_pucFGSeedbase[u4InstID] = (UCHAR *)FGT_SEED_SA;
	_pucFGDatabase[u4InstID] = (UCHAR *)FGT_DATABASE_SA;
	_pucFGSEISa[u4InstID] = (UCHAR *)FGT_SEI_SA;
	_pucFGTBuf[u4InstID] = (UCHAR *)FGT_SA;
#endif
_pucFGSEISa[u4InstID] = (UCHAR *)FGT_SEI_SA;

#if 1//def DOWN_SCALE_SUPPORT
	_pucVDSCLBuf[u4InstID] = (UCHAR *)VDSCL_BUF_SA;
	_pucVDSCLWorkBuf[u4InstID] = (UCHAR *)VDSCL_WORK_SA;
	_pucVDSCLWork1Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK1_SA;
	_pucVDSCLWork2Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK2_SA;
	_pucVDSCLWork3Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK3_SA;
	_pucVDSCLWork4Sa[u4InstID] = (UCHAR *)VDSCL_SW_WORK4_SA;
#endif
	_pucFileListSa[u4InstID] = (UCHAR*)FILE_LIST_SA;
	_pucVFifo[u4InstID] = (UCHAR *)V_FIFO_SA;
	_pucDPB[u4InstID] = (UCHAR *)DPB_SA;
	_pucPredSa[u4InstID] = (UCHAR *)AVC_PRED_SA;
	_pucDumpYBuf[u4InstID] = (UCHAR *)GOLD_Y_SA;
	_pucDumpCBuf[u4InstID] = (UCHAR *)GOLD_C_SA;
	// WMV Part
	_pucPic0Y[u4InstID] = (UCHAR *)PIC0Y_SA; //UCHAR address
	_pucPic0C[u4InstID] = (UCHAR *)PIC0C_SA;
	_pucPic1Y[u4InstID] = (UCHAR *)PIC1Y_SA;
	_pucPic1C[u4InstID] = (UCHAR *)PIC1C_SA;
	_pucPic2Y[u4InstID] = (UCHAR *)PIC2Y_SA;
	_pucPic2C[u4InstID] = (UCHAR *)PIC2C_SA;
	_pucDcac[u4InstID] = (UCHAR *)DCAC_SA;
	_pucMv_1[u4InstID] = (UCHAR *)Mv_1;
	_pucMv_2[u4InstID] = (UCHAR *)Mv_2;
	_pucBp_1[u4InstID] = (UCHAR *)Bp_1;
	_pucBp_2[u4InstID] = (UCHAR *)Bp_2;
	_pucBp_3[u4InstID] = (UCHAR *)Bp_3;
	_pucBp_4[u4InstID] = (UCHAR *)Bp_4;
	_pucMv_3[u4InstID] = (UCHAR *)Mv_3;
	_pucMv_1_2[u4InstID] = (UCHAR *)Mv_1_2;
	_pucDcac_2[u4InstID] = (UCHAR *)DCAC_2;
	_pucPp_1[u4InstID] = (UCHAR *)DEC_PP_1;
	_pucPp_2[u4InstID] = (UCHAR *)DEC_PP_2;
	// MPEG part
	_pucPpYSa[u4InstID] = (UCHAR *)DEC_PP_Y_SA;
	_pucPpCSa[u4InstID] = (UCHAR *)DEC_PP_C_SA;
	_pucMp4Dcac[u4InstID] = (UCHAR *)MP4_DCAC_SA;
	_pucMp4Mvec[u4InstID] = (UCHAR *)VER_MVEC_SA;
	_pucMp4Bmb1[u4InstID] = (UCHAR *)VER_BMB1_SA;
	_pucMp4Bmb2[u4InstID] = (UCHAR *)VER_BMB2_SA;
	_pucMp4Bcode[u4InstID] = (UCHAR *)BCODE_SA;

	_pucDcacNew[u4InstID] = (UCHAR *)DCAC_NEW_SA;
	_pucMvNew[u4InstID] = (UCHAR *)MV_NEW_SA;
	_pucBp0New[u4InstID] = (UCHAR *)BP_0_NEW_SA;
	_pucBp1New[u4InstID] = (UCHAR *)BP_1_NEW_SA;
	_pucBp2New[u4InstID] = (UCHAR *)BP_2_NEW_SA;

    #endif// DYNAMIC_MEMORY_ALLOCATE
  #endif //VDEC_VP8_EMU  
  #endif //VDEC_VP9_EMU
  #endif //VDEC_VP6_EMU
  #endif //VDEC_MPEG_EMU
  #endif//VDEC_AVS_EMU
  #endif //vdec-wmv-emu
#endif //VDEC_VIDEOCODEC_RM
}


// *********************************************************************
// Function    : void vMemoryFree(UINT32 u4InstID)
// Description : Free memory
// Parameter   : None
// Return      : None
// *********************************************************************
void vMemoryFree(UINT32 u4InstID)
{
#ifdef DYNAMIC_MEMORY_ALLOCATE
	x_free_aligned_verify_mem(_pucFileListSa[u4InstID], 1);
	if(_ucMVCType[u4InstID] != 1)
	{
	#ifdef CONFIG_DRV_MEM
	#else
	x_free_aligned_verify_mem(_pucVFifo[u4InstID], 1);
	x_free_aligned_verify_mem(_pucDumpYBuf[u4InstID], GOLD_CHANEL_A);
	x_free_aligned_verify_mem(_pucDumpCBuf[u4InstID], GOLD_CHANEL_A);
    #if VDEC_SUPPORT_MERGE_CRC
    x_free_aligned_verify_mem(_pucMergeCRCBuf[u4InstID], GOLD_CHANEL_A);
    x_free_aligned_verify_mem(_pucPicTolCnt[u4InstID], 16);
	#endif
	#endif
	//x_free_aligned_verify_mem(_pucCRCBuf[u4InstID], GOLD_CHANEL_A);
	}
#ifdef LETTERBOX_SUPPORT
	x_free_aligned_verify_mem(_pucSettingFileSa[u4InstID], 1);
	x_free_aligned_verify_mem(_pucGoldenFileSa[u4InstID], 1);
#endif

#if VDEC_DRAM_BUSY_TEST
	x_free_aligned_verify_mem(_pucDramBusy[u4InstID], WORKING_AREA_CHANEL_A);
#endif
#endif
}


BOOL _fgVDecWorkBufAllocated[2] = {FALSE, FALSE};

BOOL fgVDecAllocWorkBuffer(UINT32 u4InstID)
{
#ifdef DYNAMIC_MEMORY_ALLOCATE

#if VDEC_H264_REDUCE_MV_BUFF
	INT32 i;
#endif
	UTIL_Printf("fgVDecAllocWorkBuffer, u4InstID:%d\n", u4InstID);

	if (FALSE == _fgVDecWorkBufAllocated[u4InstID])
	{
		_fgVDecWorkBufAllocated[u4InstID] = TRUE;
	}
	else
	{
		UTIL_Printf("fgVDecAllocWorkBuffer, u4InstID:%d, allocated alread before!!\n", u4InstID);
		return TRUE;
	}

#if VDEC_AVS_EMU
#if DTV_COMMON_CONFIG

#else
  if(_u4CodecVer[u4InstID] != VDEC_AVS && _pucPic0Y[u4InstID] != 0)
  {
       x_free_aligned_verify_mem(_pucPic0Y[u4InstID], WORKING_AREA_CHANEL_A);
       x_free_aligned_verify_mem(_pucPic0C[u4InstID], WORKING_AREA_CHANEL_A);
       x_free_aligned_verify_mem(_pucPic1Y[u4InstID], WORKING_AREA_CHANEL_A);
       x_free_aligned_verify_mem(_pucPic1C[u4InstID], WORKING_AREA_CHANEL_A);
       x_free_aligned_verify_mem(_pucPic2Y[u4InstID], WORKING_AREA_CHANEL_A);
       x_free_aligned_verify_mem(_pucPic2C[u4InstID], WORKING_AREA_CHANEL_A);
       x_free_aligned_verify_mem(_pucPic3C[u4InstID], WORKING_AREA_CHANEL_A);
       x_free_aligned_verify_mem(_pucPic3C[u4InstID], WORKING_AREA_CHANEL_A);

       x_free_aligned_verify_mem(_pucAvsPred[u4InstID], WORKING_AREA_CHANEL_A);
       x_free_aligned_verify_mem(_pucAvsMv1[u4InstID], WORKING_AREA_CHANEL_A);
       x_free_aligned_verify_mem(_pucAvsMv2[u4InstID], WORKING_AREA_CHANEL_A);

       _pucPic0Y[u4InstID] = 0;
       _pucPic0C[u4InstID] = 0;
       _pucPic1Y[u4InstID] = 0;
       _pucPic1C[u4InstID] = 0;
       _pucPic2Y[u4InstID] = 0;
       _pucPic2C[u4InstID] = 0;
       _pucPic3C[u4InstID] = 0;
       _pucPic3C[u4InstID] = 0;

       _pucAvsPred[u4InstID] = 0;
       _pucAvsMv1[u4InstID] = 0;
       _pucAvsMv2[u4InstID] = 0;
	if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
       {
          _pucAddressSwapBuf[u4InstID] = 0;
       }
  }
#endif
#endif
	 if ( _u4CodecVer[u4InstID] == VDEC_H265)
	  {
	      //////////////////////////////////
	      //    VFIFO                     //
	      //    Y Golden                  //
	      //    C Golden                  //
	      //    LAE                       //
	      //    DPB                       //
	      /////////////////////////////////
	      UINT32 u4DumpYSize = GOLD_Y_SZ;
	      UINT32 u4DumpCSize = GOLD_C_SZ;
	      if(_u4CRCMode == 1)
	      {
            u4DumpYSize = GOLD_Y_SZ;
            u4DumpCSize = GOLD_C_SZ;
	      }

	      if(_u4K2KMode[u4InstID])
	      {
	        #ifdef CONFIG_DRV_MEM
	        _pucBSCABACBuff[u4InstID] = (UCHAR *)((UINT32)_pucDumpCBuf[u4InstID] + u4DumpCSize);
	        _pucECInfoBuff[u4InstID] = (UCHAR *)((UINT32)_pucBSCABACBuff[u4InstID] + CABAC_SZ);
	        #else
	        _pucBSCABACBuff[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(CABAC_SZ,4096, 0);
	        _pucECInfoBuff[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(EC_SZ,4096, 0);
	        #endif
	        x_memset(_pucBSCABACBuff[u4InstID],0,CABAC_SZ);
	        x_memset(_pucECInfoBuff[u4InstID],0,EC_SZ);
	        printk("Bitstream & cabac info buffer v[0x%x] p[0x%x]\n",_pucBSCABACBuff[u4InstID],PHYSICAL(_pucBSCABACBuff[u4InstID]));

	        printk("Error concealment info buffer v[0x%x] p[0x%x]\n",_pucECInfoBuff[u4InstID],PHYSICAL(_pucECInfoBuff[u4InstID]));

	        #ifdef CONFIG_DRV_MEM
	        _pucDPB[u4InstID] = (UCHAR *)((UINT32)_pucECInfoBuff[u4InstID] + EC_SZ);
	        #else
	        _pucDPB[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DPB_SZ,4096, 0);
	        #endif
	        printk("_pucDPB[u4InstID] = 0x%x pa = 0x%x size 0x%x\n", _pucDPB[u4InstID],PHYSICAL(_pucDPB[u4InstID]),DPB_SZ);
	      }
	      else
	      {
	        #ifdef CONFIG_DRV_MEM
	        _pucDPB[u4InstID] = (UCHAR *)((UINT32)_pucDumpCBuf[u4InstID] + u4DumpCSize);
	        #else
	        _pucDPB[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DPB_SZ,4096, 0);
	        #endif
	        printk("_pucDPB[u4InstID] = 0x%x pa = 0x%x size 0x%x\n", _pucDPB[u4InstID],PHYSICAL(_pucDPB[u4InstID]),DPB_SZ);
	      }

	      #ifdef CONFIG_DRV_MEM
          _pucVLDPredBuf[u4InstID] = (UCHAR *)((((UINT32)_pucDPB[u4InstID] + DPB_SZ) + 255) & (~255));
          _pucVLDIPBuf[u4InstID] = (UCHAR *)((((UINT32)_pucVLDPredBuf[u4InstID] + PRED_SZ) + 255) & (~255));
          _pucVLDVBuf[u4InstID] = (UCHAR *)((((UINT32)_pucVLDIPBuf[u4InstID] + IP_SZ) + 255) & (~255));
          _pucPPSAOBuf[u4InstID] = (UCHAR *)((((UINT32)_pucVLDVBuf[u4InstID] + VBUF_SZ) + 255) & (~255));
          _pucPPDBKBuf[u4InstID] = (UCHAR *)((((UINT32)_pucPPSAOBuf[u4InstID] + SAO_SZ) + 255) & (~255));
          _pucYConvertBuf[u4InstID] = (UCHAR *)((((UINT32)_pucPPDBKBuf[u4InstID] + DBK_SZ) + 255) & (~255));
          _pucUConvertBuf[u4InstID] = (UCHAR *)((((UINT32)_pucYConvertBuf[u4InstID] + Y_CONVERT_SZ) + 255) & (~255));
          _pucVConvertBuf[u4InstID] = (UCHAR *)((((UINT32)_pucUConvertBuf[u4InstID] + UV_CONVERT_SZ) + 255) & (~255));
          
	      UTIL_Printf("_pucVLDPredBuf :0x%X\n", _pucVLDPredBuf[u4InstID]);
	      UTIL_Printf("_pucVLDIPBuf :0x%X\n", _pucVLDIPBuf[u4InstID]);
	      UTIL_Printf("_pucVLDVBuf :0x%X\n", _pucVLDVBuf[u4InstID]);
	      UTIL_Printf("_pucPPSAOBuf :0x%X\n", _pucPPSAOBuf[u4InstID]);
	      UTIL_Printf("_pucPPDBKBuf :0x%X\n", _pucPPDBKBuf[u4InstID]);

	      UTIL_Printf("_pucYConvertBuf :0x%X,phy 0x%x\n", _pucYConvertBuf[u4InstID],PHYSICAL(_pucYConvertBuf[u4InstID]));
	      UTIL_Printf("_pucUConvertBuf :0x%X,phy 0x%x\n", _pucUConvertBuf[u4InstID],PHYSICAL(_pucUConvertBuf[u4InstID]));
	      UTIL_Printf("_pucVConvertBuf :0x%X,phy 0x%x\n", _pucVConvertBuf[u4InstID],PHYSICAL(_pucVConvertBuf[u4InstID]));
	      
	      UTIL_Printf("============================================\n");
	      UTIL_Printf("========VFIFO 0X%X==========================\n",V_FIFO_SZ);
	      UTIL_Printf("========YGOLD 0X%X==========================\n",GOLD_Y_SZ);
	      UTIL_Printf("========CGOLD 0X%X==========================\n",GOLD_C_SZ);
	      UTIL_Printf("========PRED  0X%X==========================\n",PRED_SZ);
	      UTIL_Printf("========IP    0X%X==========================\n",IP_SZ);
	      UTIL_Printf("========VBUF  0X%X==========================\n",VBUF_SZ);
	      UTIL_Printf("========SAO   0X%X==========================\n",SAO_SZ);
	      UTIL_Printf("========DBK   0X%X==========================\n",DBK_SZ);
	      if(_u4K2KMode[u4InstID])
	      {
	        UTIL_Printf("========CABAC 0X%X==========================\n",CABAC_SZ);
	        UTIL_Printf("========EC    0X%X==========================\n",EC_SZ);
	        UTIL_Printf("========DPB    0X%X==========================\n",DPB_SZ);
	        UTIL_Printf("========Total size 0x%x======================\n",(V_FIFO_SZ + GOLD_Y_SZ + GOLD_C_SZ + PRED_SZ +
                IP_SZ + VBUF_SZ + SAO_SZ + DBK_SZ + DPB_SZ + CABAC_SZ + EC_SZ));
	      }
	      else
	      {
	        UTIL_Printf("========DPB    0X%X==========================\n",DPB_SZ);
	        UTIL_Printf("========Total size 0x%x======================\n",(V_FIFO_SZ + GOLD_Y_SZ + GOLD_C_SZ + PRED_SZ +
                IP_SZ + VBUF_SZ + SAO_SZ + DBK_SZ + DPB_SZ));
	      }
	      #endif
	  }
	if(_u4CodecVer[u4InstID] == VDEC_H264)
	{
#ifdef CONFIG_DRV_MEM
#ifdef MVC_VFY_SUPPORT
	 	if(1 == u4InstID)
	 	{
	   		_pucDPB[u4InstID] = _pucDumpCBuf[u4InstID] + GOLD_C_SZ;
	 	}
	 	else if(0 == u4InstID)
	 	{
	    	_pucDPB[u4InstID] = _pucDumpCBuf[u4InstID] + GOLD_C_SZ;
	 	}
#else
	    #ifdef AVC_HIGH_RESOLUTION
	    _pucDPB[u4InstID] = (UCHAR *)0xc2000000;//(UCHAR *)VIRTUAL(get_drvmem_mem());
	    #else
		if (1 == _u4K2KMode[u4InstID])
		{
			_pucBSCABACBuff[u4InstID] = _pucDumpCBuf[u4InstID] + GOLD_C_SZ;
			_pucECInfoBuff[u4InstID] = _pucBSCABACBuff[u4InstID] + CABAC_SZ;
			memset(_pucBSCABACBuff[u4InstID],0,CABAC_SZ);
			memset(_pucECInfoBuff[u4InstID],0,EC_SZ);
			_pucDPB[u4InstID] = _pucECInfoBuff[u4InstID] + EC_SZ;
			_pucDPB[VDEC_LAE0] = _pucDPB[u4InstID];
		}
		else
		{
	    	_pucBSCABACBuff[u4InstID] = _pucDumpCBuf[u4InstID] + GOLD_C_SZ;
			_pucECInfoBuff[u4InstID] = _pucBSCABACBuff[u4InstID] + CABAC_SZ;
			memset(_pucBSCABACBuff[u4InstID],0,CABAC_SZ);
			memset(_pucECInfoBuff[u4InstID],0,EC_SZ);
			_pucDPB[u4InstID] = _pucECInfoBuff[u4InstID] + EC_SZ;
		}
	    #endif
#endif
#else
	//_pucDPB[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DPB_SZ,1024, 1);
		if (1 == _u4K2KMode[u4InstID])
		{
			_pucBSCABACBuff[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(CABAC_SZ,4096, 0);
			_pucECInfoBuff[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(EC_SZ,4096, 0);
			memset(_pucBSCABACBuff[u4InstID],0,CABAC_SZ);
			memset(_pucECInfoBuff[u4InstID],0,EC_SZ);
		}
		if (0 < u4InstID)
		{
			_pucDPB[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DPB_SZ,4096, 0);
		}
		else
		{
			_pucDPB[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DPB_SZ,4096, 0/*WORKING_AREA_CHANEL_A*/);
		}
#endif

		UTIL_Printf("_pucDPB[%d] = 0x%x, pa = 0x%x\n", u4InstID, _pucDPB[u4InstID], PHYSICAL(_pucDPB[u4InstID]));
		if (1 == _u4K2KMode[u4InstID])
		{
			UTIL_Printf("_pucBSCABACBuff[u4InstID] = 0x%x, _pucECInfoBuff[u4InstID] = 0x%x\n",
			_pucBSCABACBuff[u4InstID],
			_pucECInfoBuff[u4InstID]);
		}
		_pucPredSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVC_PRED_SZ,1024, 0/*WORKING_AREA_CHANEL_A*/);
		UTIL_Printf("_pucPredSa[u4InstID] = 0x%x, pa = 0x%x\n", _pucPredSa[u4InstID], PHYSICAL(_pucPredSa[u4InstID]));


#ifdef VERIFICATION_FGT
		_pucFGSeedbase[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FGT_SEED_SZ,1024,WORKING_AREA_CHANEL_A);
		_pucFGDatabase[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FGT_DATABASE_SZ,1024, WORKING_AREA_CHANEL_A);
		_pucFGSEISa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FGT_SEI_SZ,1024, WORKING_AREA_CHANEL_A);
		_pucFGTBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FGT_SZ,1024, WORKING_AREA_CHANEL_A);
#endif
		_pucFGSEISa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(FGT_SEI_SZ,1024, WORKING_AREA_CHANEL_A);

		if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
		{
	  		_pucAddressSwapBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(ADDSWAP_BUF_SZ, 2048,  WORKING_AREA_CHANEL_A);
	  		UTIL_Printf("_pucAddressSwapBuf[u4InstID] = 0x%x\n", _pucAddressSwapBuf[u4InstID]);
		}

#if VDEC_H264_REDUCE_MV_BUFF
		if (u4InstID == 0)
		{
	#ifdef CONFIG_DRV_MEM
			if (1 == _u4DualCoreEnable[u4InstID])
			{
				_pucAVCMVBuff_Main[0] = _pucDPB[VDEC_LAE0] + DPB_SZ;
			}
			else
			{
				_pucAVCMVBuff_Main[0] = _pucDPB[u4InstID] + DPB_SZ;
			}
			if (NULL == _pucAVCMVBuff_Main[0])
			{
	    		UTIL_Printf("Reserved MV Buffer, u4InstID:%d, allocated _pucAVCMVBuff_Main fail!!\n", u4InstID);
	    		return FALSE;
			}
			UTIL_Printf("Reserved MV Buffer, _pucAVCMVBuff_Main[0] = 0x%x !\n",_pucAVCMVBuff_Main[0]);
			for (i=1; i<17; i++)
			{
	  			_pucAVCMVBuff_Main[i] = _pucAVCMVBuff_Main[i-1] + ((4096*2034) / 4);
	    		UTIL_Printf("Reserved MV Buffer, _pucAVCMVBuff_Main[%d] = 0x%x !\n",i,_pucAVCMVBuff_Main[i]);
			}
	#else
	//15/9/2010 mtk40343
			_pucAVCMVBuff_Main[0] = (UCHAR *)x_alloc_aligned_verify_mem(MVBUF_SZ,4096, 0/*WORKING_AREA_CHANEL_A*/);

			if (NULL == _pucAVCMVBuff_Main[0])
			{
			    UTIL_Printf("fgVDecAllocWorkBuffer, u4InstID:%d, allocated _pucAVCMVBuff_Main fail!!\n", u4InstID);
			    return FALSE;
			}

			for (i=1; i<17; i++)
			{
			  	_pucAVCMVBuff_Main[i] = _pucAVCMVBuff_Main[i-1] + ((1920*1088) / 4);
			    UTIL_Printf("_pucAVCMVBuff_Main[%d] = 0x%x !\n",i,_pucAVCMVBuff_Main[i]);
			}
	#endif
		}
		else
		{
		#ifdef CONFIG_DRV_MEM
			_pucAVCMVBuff_Sub[0] = _pucDPB[u4InstID] + DPB_SZ;
			if (NULL == _pucAVCMVBuff_Sub[0])
			{
			    UTIL_Printf("Reserved fgVDecAllocWorkBuffer, u4InstID:%d, allocated _pucAVCMVBuff_Sub fail!!\n", u4InstID);
			    return FALSE;
			}
			UTIL_Printf("Reserved _pucAVCMVBuff_Sub[0] = 0x0%x !\n",_pucAVCMVBuff_Sub[0]);
	 		for (i=1; i<17; i++)
	 		{
	    		_pucAVCMVBuff_Sub[i] = _pucAVCMVBuff_Sub[i-1] + ((1920*1088) / 4);
	      		UTIL_Printf("Reserved _pucAVCMVBuff_Sub[%d] = 0x0%x !\n",i,_pucAVCMVBuff_Sub[i]);
	 		}
		#else
	 		_pucAVCMVBuff_Sub[0] = (UCHAR *)x_alloc_aligned_verify_mem(1920*1088*5,4096, 0/*WORKING_AREA_CHANEL_A*/);

			if (NULL == _pucAVCMVBuff_Sub[0])
			{
			    UTIL_Printf("Reserved fgVDecAllocWorkBuffer, u4InstID:%d, allocated _pucAVCMVBuff_Sub fail!!\n", u4InstID);
			    return FALSE;
			}

	 		for (i=1; i<17; i++)
	 		{
	    		_pucAVCMVBuff_Sub[i] = _pucAVCMVBuff_Sub[i-1] + ((1920*1088) / 4);
	      		UTIL_Printf("_pucAVCMVBuff_Sub[%d] = 0x0%x !\n",i,_pucAVCMVBuff_Sub[i]);
	 		}
		#endif
		}
#endif
#ifdef DOWN_SCALE_SUPPORT
		_pucVDSCLBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_BUF_SZ, 2048, VDSCL_CHANEL_A);
		_pucVDSCLWorkBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_WORK_SZ, 1024, VDSCL_CHANEL_A);
		_pucVDSCLWork1Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK1_SZ, 1024, VDSCL_CHANEL_A);
		_pucVDSCLWork2Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK2_SZ, 1024, VDSCL_CHANEL_A);
		_pucVDSCLWork3Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK3_SZ, 1024, VDSCL_CHANEL_A);
		_pucVDSCLWork4Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK4_SZ, 1024, VDSCL_CHANEL_A);
		UTIL_Printf("_pucVDSCLBuf[u4InstID] = 0x%x\n", _pucVDSCLBuf[u4InstID]);
		UTIL_Printf("_pucVDSCLWork1Sa[u4InstID] = 0x%x\n", _pucVDSCLWork1Sa[u4InstID]);
		UTIL_Printf("_pucVDSCLWork2Sa[u4InstID] = 0x%x\n", _pucVDSCLWork2Sa[u4InstID]);
		UTIL_Printf("_pucVDSCLWork3Sa[u4InstID] = 0x%x\n", _pucVDSCLWork3Sa[u4InstID]);
		UTIL_Printf("_pucVDSCLWork4Sa[u4InstID] = 0x%x\n",_pucVDSCLWork4Sa [u4InstID]);
#endif

		if (( _pucDPB[u4InstID] == 0)
	    || ( _pucPredSa[u4InstID] == 0)
	    || ( _u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF && _pucAddressSwapBuf[u4InstID] == 0)
#ifdef VERIFICATION_FGT
	    || ( _pucFGSeedbase[u4InstID] == 0)
	    || ( _pucFGDatabase[u4InstID] == 0)
	    || ( _pucFGSEISa[u4InstID] == 0)
	    || ( _pucFGTBuf[u4InstID] == 0)
#endif
#ifdef DOWN_SCALE_SUPPORT
	    || ( _pucVDSCLBuf[u4InstID] == 0)
	    || ( _pucVDSCLWorkBuf[u4InstID] == 0)
	    || ( _pucVDSCLWork1Sa[u4InstID] == 0)
	    || ( _pucVDSCLWork2Sa[u4InstID] == 0)
	    || ( _pucVDSCLWork3Sa[u4InstID] == 0)
	    || ( _pucVDSCLWork4Sa[u4InstID] == 0)
#endif
		)
		{
	 		return FALSE;
		}

	}

else if(_u4CodecVer[u4InstID] == VDEC_WMV)
		{
#if 1//def DOWN_SCALE_SUPPORT if turn on DDR3, VDSCLBuf used for address swap buffer
				  _pucVDSCLBuf[u4InstID] = _pucPicTolCnt[u4InstID] + 0x4000;
				UTIL_Printf("_pucVDSCLBuf[u4InstID] = 0x%x\n", _pucVDSCLBuf[u4InstID]);
#endif

#ifdef DOWN_SCALE_SUPPORT
				UTIL_Printf("DOWN_SCALE_SUPPORT\n");
				_pucVDSCLWork1Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK1_SZ,1024, VDSCL_CHANEL_A);
				_pucVDSCLWork2Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK2_SZ,1024, VDSCL_CHANEL_A);
				_pucVDSCLWork3Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK3_SZ,1024, VDSCL_CHANEL_A);
				_pucVDSCLWork4Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK4_SZ,1024, VDSCL_CHANEL_A);
				  UTIL_Printf("_pucVDSCLWork1Sa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucVDSCLWork1Sa[u4InstID], PHYSICAL(_pucVDSCLWork1Sa[u4InstID]));
				  UTIL_Printf("_pucVDSCLWork2Sa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucVDSCLWork2Sa[u4InstID], PHYSICAL(_pucVDSCLWork2Sa[u4InstID]));
				  UTIL_Printf("_pucVDSCLWork3Sa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucVDSCLWork3Sa[u4InstID], PHYSICAL(_pucVDSCLWork3Sa[u4InstID]));
				  UTIL_Printf("_pucVDSCLWork4Sa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucVDSCLWork4Sa[u4InstID], PHYSICAL(_pucVDSCLWork4Sa [u4InstID]));
#endif
		// WMV Part
#if 0
				_pucPic0Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,4096, WORKING_AREA_CHANEL_A); //UCHAR address
				_pucPic0C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
				_pucPic1Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,4096, WORKING_AREA_CHANEL_A);
				_pucPic1C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
				_pucPic2Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,4096, WORKING_AREA_CHANEL_A);
				_pucPic2C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
#endif
					UTIL_Printf("DOWN_SCALE_SUPPORT111\n");
				  _pucPic0Y[u4InstID] = _pucVDSCLBuf[u4InstID] + VDSCL_BUF_SZ;
				  _pucPic0C[u4InstID] = _pucPic0Y[u4InstID] + PIC_Y_SZ;
				  _pucPic1Y[u4InstID] = _pucPic0C[u4InstID] + PIC_C_SZ;
				  _pucPic1C[u4InstID] = _pucPic1Y[u4InstID] + PIC_Y_SZ;
				  _pucPic2Y[u4InstID] = _pucPic1C[u4InstID] + PIC_C_SZ;
				  _pucPic2C[u4InstID] = _pucPic2Y[u4InstID] + PIC_Y_SZ;
				  UTIL_Printf("DOWN_SCALE_SUPPORT---\n");

#if 1//0
				  //x_memset(_pucPic0Y[u4InstID], 0, PIC_Y_SZ);
				  UTIL_Printf("_pucPic0Y[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic0Y[u4InstID], PHYSICAL(_pucPic0Y[u4InstID]));
				  //x_memset(_pucPic0C[u4InstID], 0, PIC_C_SZ);
				   UTIL_Printf("_pucPic0C[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic0C[u4InstID], PHYSICAL(_pucPic0C[u4InstID]));
				  //x_memset(_pucPic1Y[u4InstID], 0, PIC_Y_SZ);
				  UTIL_Printf("_pucPic1Y[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic1Y[u4InstID], PHYSICAL(_pucPic1Y[u4InstID]));
				  //x_memset(_pucPic1C[u4InstID], 0, PIC_C_SZ);
				  UTIL_Printf("_pucPic1C[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic1C[u4InstID], PHYSICAL(_pucPic1C[u4InstID]));
				  //x_memset(_pucPic2Y[u4InstID], 0, PIC_Y_SZ);
				  UTIL_Printf("_pucPic2Y[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic2Y [u4InstID], PHYSICAL(_pucPic2Y [u4InstID]));
				  //x_memset(_pucPic2Y[u4InstID], 0, PIC_C_SZ);
				  UTIL_Printf("_pucPic2C[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic2C[u4InstID],  PHYSICAL(_pucPic2C[u4InstID]));
#endif
				if (_u4WmvMode[u4InstID] == 0)
				{
#if 0
				_pucDcac[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DCAC_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucMv_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Mv_1_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucMv_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Mv_2_SZ,1024, WORKING_AREA_CHANEL_A);
#endif
				  _pucDcac[u4InstID] =	_pucPic2C[u4InstID] + PIC_C_SZ;
				  _pucMv_1[u4InstID] =	_pucDcac[u4InstID] + DCAC_SZ;
				  _pucMv_2[u4InstID] =	_pucMv_1[u4InstID] + Mv_1_SZ;
				  UTIL_Printf("_pucDcac[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucDcac[u4InstID], PHYSICAL(_pucDcac[u4InstID]));
				  UTIL_Printf("_pucMv_1[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMv_1[u4InstID], PHYSICAL(_pucMv_1[u4InstID]));
				  UTIL_Printf("_pucMv_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMv_2[u4InstID], PHYSICAL(_pucMv_2[u4InstID]));
#if 0
				_pucBp_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Bp_1_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucBp_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Bp_2_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucBp_3[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Bp_3_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucBp_4[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Bp_4_SZ,1024, WORKING_AREA_CHANEL_A);
#endif
				  _pucBp_1[u4InstID] = _pucMv_2[u4InstID] + Mv_2_SZ;
				  _pucBp_2[u4InstID] =	_pucBp_1[u4InstID] + Bp_1_SZ;
				  _pucBp_3[u4InstID] = _pucBp_2[u4InstID] + Bp_2_SZ;
				  _pucBp_4[u4InstID] = _pucBp_3[u4InstID] + Bp_3_SZ;
				  UTIL_Printf("_pucBp_1[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp_1[u4InstID], PHYSICAL(_pucBp_1[u4InstID]));
				  UTIL_Printf("_pucBp_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp_2[u4InstID], PHYSICAL(_pucBp_2[u4InstID]));
				  UTIL_Printf("_pucBp_3[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp_3[u4InstID], PHYSICAL(_pucBp_3[u4InstID]));
				  UTIL_Printf("_pucBp_4[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp_4[u4InstID], PHYSICAL(_pucBp_4[u4InstID]));
				}

				if (_u4WmvMode[u4InstID] == 0)
				{
#if 0
				_pucMv_3[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Mv_3_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucMv_1_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Mv_1_2_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucDcac_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DCAC_2_SZ,1024, WORKING_AREA_CHANEL_A);
#endif
				  _pucMv_3[u4InstID] = _pucBp_4[u4InstID] + Bp_4_SZ;
				  _pucMv_1_2[u4InstID] = _pucMv_3[u4InstID] + Mv_3_SZ;
				  _pucDcac_2[u4InstID] = _pucMv_1_2[u4InstID] + Mv_1_2_SZ;
				  UTIL_Printf("_pucMv_3[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMv_3[u4InstID], PHYSICAL(_pucMv_3[u4InstID]));
				  UTIL_Printf("_pucMv_1_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMv_1_2[u4InstID], PHYSICAL( _pucMv_1_2[u4InstID]));
				  UTIL_Printf("_pucDcac_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucDcac_2[u4InstID], PHYSICAL(_pucDcac_2[u4InstID]));
				}
				else
				{
#if 0
				_pucDcacNew[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DCAC_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucMvNew[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(MV_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucBp0New[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(BP_0_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucBp1New[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(BP_1_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucBp2New[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(BP_2_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
#endif
				  _pucDcacNew[u4InstID] = _pucPic2C[u4InstID] + PIC_C_SZ;
				  _pucMvNew[u4InstID] = _pucDcacNew[u4InstID] + 0x10000; //DCAC_NEW_SZ;
				  _pucBp0New[u4InstID] = _pucMvNew[u4InstID] + 0x10000; //MV_NEW_SZ;
				  _pucBp1New[u4InstID] = _pucBp0New[u4InstID] + 0x10000;//BP_0_NEW_SZ;
				  _pucBp2New[u4InstID] =  _pucBp1New[u4InstID] + 0x10000;//BP_1_NEW_SZ;
				  UTIL_Printf("_pucBp0New[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp0New[u4InstID], PHYSICAL(_pucBp0New[u4InstID])); //sun for temp
				  UTIL_Printf("_pucBp1New[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp1New[u4InstID], PHYSICAL(_pucBp1New[u4InstID]));
				  UTIL_Printf("_pucBp2New[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp2New[u4InstID], PHYSICAL(_pucBp2New[u4InstID]));
				}
#if 0
				_pucPp_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucPp_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_SZ,1024, WORKING_AREA_CHANEL_A);
				// MPEG part
				_pucPpYSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucPpCSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_C_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucMp4Dcac[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DCAC_SZ,1024, WORKING_AREA_CHANEL_A);
			  #if DTV_COMMON_CONFIG
			  #else
				_pucMp4Mvec[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VER_MVEC_SZ,1024, WORKING_AREA_CHANEL_A);
		      #endif
				_pucMp4Bmb1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VER_BMB1_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucMp4Bmb2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VER_BMB2_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucMp4Bcode[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(BCODE_SZ,1024, WORKING_AREA_CHANEL_A);
				_pucDumpSRAMBuf[u4InstID] = (UCHAR*)x_alloc_aligned_verify_mem(54*1024,1024,1);
#endif
					 if (_u4WmvMode[u4InstID] == 0)
						{
						 _pucPp_1[u4InstID] = _pucDcac_2[u4InstID] + DCAC_2_SZ;
						 x_memset((UCHAR*) ((UINT32)_pucPp_1[u4InstID]), 0, DEC_PP_SZ);
						}
					 else{
							_pucPp_1[u4InstID] = _pucBp2New[u4InstID] + 0x10000;// BP_2_NEW_SZ;
							x_memset((UCHAR*) ((UINT32)_pucPp_1[u4InstID]), 0, DEC_PP_SZ);
						}
					  _pucPp_2[u4InstID] =	_pucPp_1[u4InstID] + DEC_PP_SZ;
					  x_memset((UCHAR*) ((UINT32)_pucPp_1[u4InstID]), 0, DEC_PP_SZ);
					  _pucPpYSa[u4InstID] = _pucPp_2[u4InstID] + DEC_PP_SZ;
					  _pucPpCSa[u4InstID] = _pucPpYSa[u4InstID] + DEC_PP_Y_SZ;
					  _pucMp4Dcac[u4InstID] = _pucPpCSa[u4InstID] + DEC_PP_C_SZ;
#if DTV_COMMON_CONFIG
#else
					  _pucMp4Mvec[u4InstID] = _pucDumpSRAMBuf[u4InstID] + 54*1024;
#endif
					  _pucMp4Bmb1[u4InstID] = _pucMp4Dcac[u4InstID] + DCAC_SZ;
					  _pucMp4Bmb2[u4InstID] = _pucMp4Bmb1[u4InstID] + VER_BMB1_SZ;
					  _pucMp4Bcode[u4InstID] =	_pucMp4Bmb2[u4InstID] + VER_BMB2_SZ;
					  _pucDumpSRAMBuf[u4InstID] =  _pucMp4Bcode[u4InstID] + BCODE_SZ;
				  UTIL_Printf("_pucPp_1[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPp_1[u4InstID], PHYSICAL(_pucPp_1[u4InstID]));
				  UTIL_Printf("_pucPp_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPp_2[u4InstID], PHYSICAL(_pucPp_2[u4InstID]));
				  UTIL_Printf("_pucPpYSa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPpYSa[u4InstID], PHYSICAL(_pucPpYSa[u4InstID]));
				  UTIL_Printf("_pucPpCSa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPpCSa[u4InstID], PHYSICAL(_pucPpCSa[u4InstID]));
				  UTIL_Printf("_pucMp4Dcac[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Dcac[u4InstID], PHYSICAL(_pucMp4Dcac[u4InstID]));
				  UTIL_Printf("_pucMp4Mvec[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Mvec[u4InstID], PHYSICAL(_pucMp4Mvec[u4InstID]));
				  UTIL_Printf("_pucMp4Bmb1[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Bmb1[u4InstID], PHYSICAL(_pucMp4Bmb1[u4InstID]));
				  UTIL_Printf("_pucMp4Bmb2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Bmb2[u4InstID], PHYSICAL(_pucMp4Bmb2[u4InstID]));
				  UTIL_Printf("_pucMp4Bcode[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Bcode[u4InstID], PHYSICAL(_pucMp4Bcode[u4InstID]));
				  UTIL_Printf("_pucDumpSRAMBuf[u4InstID]  = 0x%x, PHYSICAL:0x%x\n", _pucDumpSRAMBuf[u4InstID], PHYSICAL(_pucDumpSRAMBuf[u4InstID]));

		}
	//~AVC Working Buffer
	#ifndef VDEC_MPEG_EMU
	else
	if (_u4CodecVer[u4InstID] == VDEC_MPEG1
	|| _u4CodecVer[u4InstID] == VDEC_MPEG2
	|| _u4CodecVer[u4InstID] == VDEC_MPEG4
	|| _u4CodecVer[u4InstID] == VDEC_H263
	|| _u4CodecVer[u4InstID] == VDEC_DIVX3)
	{
#if 1//def DOWN_SCALE_SUPPORT if turn on DDR3, VDSCLBuf used for address swap buffer
      _pucVDSCLBuf[u4InstID] = _pucPicTolCnt[u4InstID] + 0x4000;
	UTIL_Printf("_pucVDSCLBuf[u4InstID] = 0x%x\n", _pucVDSCLBuf[u4InstID]);
#endif

#ifdef DOWN_SCALE_SUPPORT
	UTIL_Printf("DOWN_SCALE_SUPPORT\n");
	_pucVDSCLWork1Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK1_SZ,1024, VDSCL_CHANEL_A);
	_pucVDSCLWork2Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK2_SZ,1024, VDSCL_CHANEL_A);
	_pucVDSCLWork3Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK3_SZ,1024, VDSCL_CHANEL_A);
	_pucVDSCLWork4Sa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_SW_WORK4_SZ,1024, VDSCL_CHANEL_A);
      UTIL_Printf("_pucVDSCLWork1Sa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucVDSCLWork1Sa[u4InstID], PHYSICAL(_pucVDSCLWork1Sa[u4InstID]));
      UTIL_Printf("_pucVDSCLWork2Sa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucVDSCLWork2Sa[u4InstID], PHYSICAL(_pucVDSCLWork2Sa[u4InstID]));
      UTIL_Printf("_pucVDSCLWork3Sa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucVDSCLWork3Sa[u4InstID], PHYSICAL(_pucVDSCLWork3Sa[u4InstID]));
      UTIL_Printf("_pucVDSCLWork4Sa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucVDSCLWork4Sa[u4InstID], PHYSICAL(_pucVDSCLWork4Sa [u4InstID]));
#endif


	// WMV Part
  #if 0
	_pucPic0Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,4096, WORKING_AREA_CHANEL_A); //UCHAR address
	_pucPic0C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
	_pucPic1Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,4096, WORKING_AREA_CHANEL_A);
	_pucPic1C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
	_pucPic2Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,4096, WORKING_AREA_CHANEL_A);
	_pucPic2C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
  #endif
  		UTIL_Printf("DOWN_SCALE_SUPPORT111\n");
 	  _pucPic0Y[u4InstID] = _pucVDSCLBuf[u4InstID] + VDSCL_BUF_SZ;
      _pucPic0C[u4InstID] = _pucPic0Y[u4InstID] + PIC_Y_SZ;
      _pucPic1Y[u4InstID] = _pucPic0C[u4InstID] + PIC_C_SZ;
      _pucPic1C[u4InstID] = _pucPic1Y[u4InstID] + PIC_Y_SZ;
      _pucPic2Y[u4InstID] = _pucPic1C[u4InstID] + PIC_C_SZ;
      _pucPic2C[u4InstID] = _pucPic2Y[u4InstID] + PIC_Y_SZ;
	  UTIL_Printf("DOWN_SCALE_SUPPORT---\n");

	  #if 1//0
	  x_memset(_pucPic0Y[u4InstID], 0, PIC_Y_SZ);
	  UTIL_Printf("_pucPic0Y[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic0Y[u4InstID], PHYSICAL(_pucPic0Y[u4InstID]));
	  x_memset(_pucPic0C[u4InstID], 0, PIC_C_SZ);
	   UTIL_Printf("_pucPic0C[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic0C[u4InstID], PHYSICAL(_pucPic0C[u4InstID]));
	  x_memset(_pucPic1Y[u4InstID], 0, PIC_Y_SZ);
	  UTIL_Printf("_pucPic1Y[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic1Y[u4InstID], PHYSICAL(_pucPic1Y[u4InstID]));
	  x_memset(_pucPic1C[u4InstID], 0, PIC_C_SZ);
	  UTIL_Printf("_pucPic1C[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic1C[u4InstID], PHYSICAL(_pucPic1C[u4InstID]));
	  x_memset(_pucPic2Y[u4InstID], 0, PIC_Y_SZ);
	  UTIL_Printf("_pucPic2Y[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic2Y [u4InstID], PHYSICAL(_pucPic2Y [u4InstID]));
	  x_memset(_pucPic2Y[u4InstID], 0, PIC_C_SZ);
	  UTIL_Printf("_pucPic2C[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPic2C[u4InstID],  PHYSICAL(_pucPic2C[u4InstID]));
	  #endif
	if (_u4WmvMode[u4InstID] == 0)
	{
     #if 0
	_pucDcac[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DCAC_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucMv_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Mv_1_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucMv_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Mv_2_SZ,1024, WORKING_AREA_CHANEL_A);
	 #endif
	  _pucDcac[u4InstID] =  _pucPic2C[u4InstID] + PIC_C_SZ;
      _pucMv_1[u4InstID] =  _pucDcac[u4InstID] + DCAC_SZ;
      _pucMv_2[u4InstID] =  _pucMv_1[u4InstID] + Mv_1_SZ;
      UTIL_Printf("_pucDcac[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucDcac[u4InstID], PHYSICAL(_pucDcac[u4InstID]));
      UTIL_Printf("_pucMv_1[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMv_1[u4InstID], PHYSICAL(_pucMv_1[u4InstID]));
      UTIL_Printf("_pucMv_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMv_2[u4InstID], PHYSICAL(_pucMv_2[u4InstID]));

	#if 0
	_pucBp_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Bp_1_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucBp_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Bp_2_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucBp_3[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Bp_3_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucBp_4[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Bp_4_SZ,1024, WORKING_AREA_CHANEL_A);
	  #endif
	  _pucBp_1[u4InstID] = _pucMv_2[u4InstID] + Mv_2_SZ;
      _pucBp_2[u4InstID] =  _pucBp_1[u4InstID] + Bp_1_SZ;
      _pucBp_3[u4InstID] = _pucBp_2[u4InstID] + Bp_2_SZ;
      _pucBp_4[u4InstID] = _pucBp_3[u4InstID] + Bp_3_SZ;
      UTIL_Printf("_pucBp_1[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp_1[u4InstID], PHYSICAL(_pucBp_1[u4InstID]));
      UTIL_Printf("_pucBp_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp_2[u4InstID], PHYSICAL(_pucBp_2[u4InstID]));
      UTIL_Printf("_pucBp_3[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp_3[u4InstID], PHYSICAL(_pucBp_3[u4InstID]));
      UTIL_Printf("_pucBp_4[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp_4[u4InstID], PHYSICAL(_pucBp_4[u4InstID]));
	}

	if (_u4WmvMode[u4InstID] == 0)
	{
   #if 0
	_pucMv_3[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Mv_3_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucMv_1_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(Mv_1_2_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucDcac_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DCAC_2_SZ,1024, WORKING_AREA_CHANEL_A);
#endif
      _pucMv_3[u4InstID] = _pucBp_4[u4InstID] + Bp_4_SZ;
      _pucMv_1_2[u4InstID] = _pucMv_3[u4InstID] + Mv_3_SZ;
      _pucDcac_2[u4InstID] = _pucMv_1_2[u4InstID] + Mv_1_2_SZ;
      UTIL_Printf("_pucMv_3[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMv_3[u4InstID], PHYSICAL(_pucMv_3[u4InstID]));
      UTIL_Printf("_pucMv_1_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMv_1_2[u4InstID], PHYSICAL( _pucMv_1_2[u4InstID]));
      UTIL_Printf("_pucDcac_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucDcac_2[u4InstID], PHYSICAL(_pucDcac_2[u4InstID]));
	}
	else
	{
   #if 0
	_pucDcacNew[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DCAC_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucMvNew[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(MV_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucBp0New[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(BP_0_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucBp1New[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(BP_1_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucBp2New[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(BP_2_NEW_SZ,1024, WORKING_AREA_CHANEL_A);
  #endif
  	  _pucDcacNew[u4InstID] = _pucPic2C[u4InstID] + PIC_C_SZ;
      _pucMvNew[u4InstID] = _pucDcacNew[u4InstID] + 0x10000; //DCAC_NEW_SZ;
      _pucBp0New[u4InstID] = _pucMvNew[u4InstID] + 0x10000; //MV_NEW_SZ;
      _pucBp1New[u4InstID] = _pucBp0New[u4InstID] + 0x10000;//BP_0_NEW_SZ;
      _pucBp2New[u4InstID] =  _pucBp1New[u4InstID] + 0x10000;//BP_1_NEW_SZ;
      UTIL_Printf("_pucBp0New[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp0New[u4InstID], PHYSICAL(_pucBp0New[u4InstID])); //sun for temp
      UTIL_Printf("_pucBp1New[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp1New[u4InstID], PHYSICAL(_pucBp1New[u4InstID]));
      UTIL_Printf("_pucBp2New[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucBp2New[u4InstID], PHYSICAL(_pucBp2New[u4InstID]));
	}
#if 0
	_pucPp_1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucPp_2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_SZ,1024, WORKING_AREA_CHANEL_A);
	// MPEG part
	_pucPpYSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucPpCSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_C_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucMp4Dcac[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DCAC_SZ,1024, WORKING_AREA_CHANEL_A);
	  #if DTV_COMMON_CONFIG
	  #else
	_pucMp4Mvec[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VER_MVEC_SZ,1024, WORKING_AREA_CHANEL_A);
      #endif
	_pucMp4Bmb1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VER_BMB1_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucMp4Bmb2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VER_BMB2_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucMp4Bcode[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(BCODE_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucDumpSRAMBuf[u4InstID] = (UCHAR*)x_alloc_aligned_verify_mem(54*1024,1024,1);
#endif
		 if (_u4WmvMode[u4InstID] == 0)
		 	{
		 	 _pucPp_1[u4InstID] = _pucDcac_2[u4InstID] + DCAC_2_SZ;
			 x_memset((UCHAR*) ((UINT32)_pucPp_1[u4InstID]), 0, DEC_PP_SZ);
		 	}
		 else{
		 	 	_pucPp_1[u4InstID] = _pucBp2New[u4InstID] + 0x10000;// BP_2_NEW_SZ;
		 	 	x_memset((UCHAR*) ((UINT32)_pucPp_1[u4InstID]), 0, DEC_PP_SZ);
		 	}
		  _pucPp_2[u4InstID] =  _pucPp_1[u4InstID] + DEC_PP_SZ;
		  x_memset((UCHAR*) ((UINT32)_pucPp_1[u4InstID]), 0, DEC_PP_SZ);
		  _pucPpYSa[u4InstID] = _pucPp_2[u4InstID] + DEC_PP_SZ;
		  _pucPpCSa[u4InstID] = _pucPpYSa[u4InstID] + DEC_PP_Y_SZ;
		  _pucMp4Dcac[u4InstID] = _pucPpCSa[u4InstID] + DEC_PP_C_SZ;
	#if DTV_COMMON_CONFIG
	#else
		  _pucMp4Mvec[u4InstID] = _pucDumpSRAMBuf[u4InstID] + 54*1024;
	#endif
		  _pucMp4Bmb1[u4InstID] = _pucMp4Dcac[u4InstID] + DCAC_SZ;
		  _pucMp4Bmb2[u4InstID] = _pucMp4Bmb1[u4InstID] + VER_BMB1_SZ;
		  _pucMp4Bcode[u4InstID] =  _pucMp4Bmb2[u4InstID] + VER_BMB2_SZ;
		  _pucDumpSRAMBuf[u4InstID] =  _pucMp4Bcode[u4InstID] + BCODE_SZ;
      UTIL_Printf("_pucPp_1[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPp_1[u4InstID], PHYSICAL(_pucPp_1[u4InstID]));
      UTIL_Printf("_pucPp_2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPp_2[u4InstID], PHYSICAL(_pucPp_2[u4InstID]));
      UTIL_Printf("_pucPpYSa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPpYSa[u4InstID], PHYSICAL(_pucPpYSa[u4InstID]));
      UTIL_Printf("_pucPpCSa[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucPpCSa[u4InstID], PHYSICAL(_pucPpCSa[u4InstID]));
      UTIL_Printf("_pucMp4Dcac[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Dcac[u4InstID], PHYSICAL(_pucMp4Dcac[u4InstID]));
      UTIL_Printf("_pucMp4Mvec[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Mvec[u4InstID], PHYSICAL(_pucMp4Mvec[u4InstID]));
      UTIL_Printf("_pucMp4Bmb1[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Bmb1[u4InstID], PHYSICAL(_pucMp4Bmb1[u4InstID]));
      UTIL_Printf("_pucMp4Bmb2[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Bmb2[u4InstID], PHYSICAL(_pucMp4Bmb2[u4InstID]));
      UTIL_Printf("_pucMp4Bcode[u4InstID] = 0x%x, PHYSICAL:0x%x\n", _pucMp4Bcode[u4InstID], PHYSICAL(_pucMp4Bcode[u4InstID]));
      UTIL_Printf("_pucDumpSRAMBuf[u4InstID]  = 0x%x, PHYSICAL:0x%x\n", _pucDumpSRAMBuf[u4InstID], PHYSICAL(_pucDumpSRAMBuf[u4InstID]));
	}
        #endif
	else
	if (_u4CodecVer[u4InstID] == VDEC_VP6)
	{
     UCHAR *AlphaBakup = NULL;
   #if 0
      #if VDEC_DDR3_SUPPORT//def DOWN_SCALE_SUPPORT if turn on DDR3, VDSCLBuf used for address swap buffer
      _pucVDSCLBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VDSCL_BUF_SZ,2048, VDSCL_CHANEL_A);
      UTIL_Printf("_pucVDSCLBuf[u4InstID] = 0x%x\n", _pucVDSCLBuf[u4InstID]);
      #endif
       _pucSizeFileBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(1024*400,2048, WORKING_AREA_CHANEL_A);
      _pucPic0Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      _pucPic0C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic1Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic1C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic2Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic2C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);

      _pucPpYSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPpCSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A);
   #else
      #if VDEC_DDR3_SUPPORT//def DOWN_SCALE_SUPPORT if turn on DDR3, VDSCLBuf used for address swap buffer
      _pucVDSCLBuf[u4InstID] = _pucPicTolCnt[u4InstID] + 0x1000;//(UCHAR *)x_alloc_aligned_verify_mem(VDSCL_BUF_SZ,2048, VDSCL_CHANEL_A);
      UTIL_Printf("_pucVDSCLBuf[u4InstID] = 0x%x\n", _pucVDSCLBuf[u4InstID]);
	  _pucSizeFileBuf[u4InstID] = _pucVDSCLBuf[u4InstID] + VDSCL_BUF_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(1024*400,2048, WORKING_AREA_CHANEL_A);
      #else
       _pucSizeFileBuf[u4InstID] = _pucPicTolCnt[u4InstID] + VDSCL_BUF_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(1024*400,2048, WORKING_AREA_CHANEL_A);
      #endif
      _pucPic0Y[u4InstID] = _pucSizeFileBuf[u4InstID] + 1024 * 400;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      _pucPic0C[u4InstID] = _pucPic0Y[u4InstID]  + PIC_Y_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic1Y[u4InstID] = _pucPic0C[u4InstID]  + PIC_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic1C[u4InstID] = _pucPic1Y[u4InstID]  + PIC_Y_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic2Y[u4InstID] = _pucPic1C[u4InstID]  + PIC_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPic2C[u4InstID] = _pucPic2Y[u4InstID]  + PIC_Y_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);

      _pucPpYSa[u4InstID] = _pucPic2C[u4InstID]  + PIC_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      _pucPpCSa[u4InstID] = _pucPpYSa[u4InstID]  + DEC_PP_Y_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A);


   #endif

	UTIL_Printf("_pucSizeFileBuf[%d] = 0x%x\n", u4InstID, _pucSizeFileBuf[u4InstID]);
	UTIL_Printf("_pucPic0Y[%d] = 0x%x\n", u4InstID, _pucPic0Y[u4InstID]);
	UTIL_Printf("_pucPic0C[%d] = 0x%x\n", u4InstID, _pucPic0C[u4InstID]);
	UTIL_Printf("_pucPic1Y[%d] = 0x%x\n", u4InstID, _pucPic1Y[u4InstID]);
	UTIL_Printf("_pucPic1C[%d] = 0x%x\n", u4InstID, _pucPic1C[u4InstID]);
	UTIL_Printf("_pucPic2Y[%d] = 0x%x\n", u4InstID, _pucPic2Y [u4InstID]);
	UTIL_Printf("_pucPic2C[%d] = 0x%x\n", u4InstID, _pucPic2C[u4InstID]);
	UTIL_Printf("_pucPpYSa[%d] = 0x%x\n", u4InstID, _pucPpYSa[u4InstID]);
	UTIL_Printf("_pucPpCSa[%d] = 0x%x\n", u4InstID, _pucPpCSa[u4InstID]);

	  AlphaBakup = _pucPpCSa[u4InstID] + DEC_PP_C_SZ;
	if (_u1AlphaBitstream[u4InstID])
	{
	  u4InstID = 1;
		  UTIL_Printf("_u1AlphaBitstream\n");
		  #if 0
          _pucPic0Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
          _pucPic0C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
          _pucPic1Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
          _pucPic1C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
          _pucPic2Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
          _pucPic2C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);

          _pucPpYSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ,2048, WORKING_AREA_CHANEL_A);
          _pucPpCSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A);
		  #else

	      _pucPic0Y[u4InstID] = AlphaBakup;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
	      _pucPic0C[u4InstID] = _pucPic0Y[u4InstID]  + PIC_Y_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
	      _pucPic1Y[u4InstID] = _pucPic0C[u4InstID]  + PIC_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
	      _pucPic1C[u4InstID] = _pucPic1Y[u4InstID]  + PIC_Y_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
	      _pucPic2Y[u4InstID] = _pucPic1C[u4InstID]  + PIC_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
	      _pucPic2C[u4InstID] = _pucPic2Y[u4InstID]  + PIC_Y_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);

	      _pucPpYSa[u4InstID] = _pucPic2C[u4InstID]  + PIC_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ,2048, WORKING_AREA_CHANEL_A);
	      _pucPpCSa[u4InstID] = _pucPpYSa[u4InstID]  + DEC_PP_Y_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A);



		  #endif


	  UTIL_Printf("_pucPic0Y[%d] = 0x%x\n", u4InstID, _pucPic0Y[u4InstID]);
	  UTIL_Printf("_pucPic0C[%d] = 0x%x\n", u4InstID, _pucPic0C[u4InstID]);
	  UTIL_Printf("_pucPic1Y[%d] = 0x%x\n", u4InstID, _pucPic1Y[u4InstID]);
	  UTIL_Printf("_pucPic1C[%d] = 0x%x\n", u4InstID, _pucPic1C[u4InstID]);
	  UTIL_Printf("_pucPic2Y[%d] = 0x%x\n", u4InstID, _pucPic2Y [u4InstID]);
	  UTIL_Printf("_pucPic2C[%d] = 0x%x\n", u4InstID, _pucPic2C[u4InstID]);
	  UTIL_Printf("_pucPpYSa[%d] = 0x%x\n", u4InstID, _pucPpYSa[u4InstID]);
	  UTIL_Printf("_pucPpCSa[%d] = 0x%x\n", u4InstID, _pucPpCSa[u4InstID]);

	  u4InstID = 0;
	}


      #if 0
      if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
      {
          _pucAddressSwapBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(ADDSWAP_BUF_SZ, 2048, WORKING_AREA_CHANEL_A);
          UTIL_Printf("_pucAddressSwapBuf[u4InstID] = 0x%x\n", _pucAddressSwapBuf[u4InstID]);
      }
	  #else
      if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
      {
          _pucAddressSwapBuf[u4InstID] = _pucPpCSa[u4InstID] + DEC_PP_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(ADDSWAP_BUF_SZ, 2048, WORKING_AREA_CHANEL_A);
          UTIL_Printf("_pucAddressSwapBuf[u4InstID] = 0x%x\n", _pucAddressSwapBuf[u4InstID]);
      }

	  #endif
	}
	else
	if (_u4CodecVer[u4InstID] == VDEC_VP8)
	{
#if 1
		 _pucSizeFileBuf[u4InstID] = _pucPicTolCnt[u4InstID] + 0x2000;//(UCHAR *)x_alloc_aligned_verify_mem(1024*400,2048, WORKING_AREA_CHANEL_A);
	   UTIL_Printf("_pucSizeFileBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucSizeFileBuf[u4InstID], 1024*400);

      _pucWorkYBuf[u4InstID] = _pucSizeFileBuf[u4InstID] + 1024*400;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ+PIC_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
       UTIL_Printf("_pucWorkYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucWorkYBuf[u4InstID],PIC_Y_SZ+PIC_C_SZ);
//      _pucWorkCBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);

#if VDEC_VP8_WEBP_SUPPORT
   
      _pucDumpArfYBuf[u4InstID] = _pucWorkYBuf[u4InstID] + PIC_Y_SZ+PIC_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ+DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpArfYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpArfYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);
      _pucDumpGldYBuf[u4InstID] = _pucDumpArfYBuf[u4InstID] + DEC_PP_Y_SZ+DEC_PP_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ+DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpGldYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpGldYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);
      _pucDumpLstYBuf[u4InstID] = _pucDumpGldYBuf[u4InstID]  + DEC_PP_Y_SZ+DEC_PP_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ+DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpLstYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpLstYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);
     _pucVLDWrapper[u4InstID] = _pucDumpLstYBuf[u4InstID] + DEC_PP_Y_SZ+DEC_PP_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(VLD_PRED_SZ,1024, WORKING_AREA_CHANEL_A);
	 UTIL_Printf("_pucVLDWrapper[u4InstID] = 0x%x, size is 0x%x\n", _pucVLDWrapper[u4InstID], VLD_PRED_SZ);
     _pucPPWrapperY[u4InstID] = _pucVLDWrapper[u4InstID] + VLD_PRED_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PP_WRAPY_SZ,1024, WORKING_AREA_CHANEL_A);
	 UTIL_Printf("_pucPPWrapperY[u4InstID] = 0x%x, size is 0x%x\n", _pucPPWrapperY[u4InstID], PP_WRAPY_SZ);
     _pucPPWrapperC[u4InstID] = _pucPPWrapperY[u4InstID]  +  PP_WRAPY_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PP_WRAPC_SZ,1024, WORKING_AREA_CHANEL_A);
	 UTIL_Printf("_pucPPWrapperC[u4InstID] = 0x%x, size is 0x%x\n", _pucPPWrapperC[u4InstID], PP_WRAPC_SZ);
     _pucSegIdWrapper[u4InstID] = _pucPPWrapperC[u4InstID] + PP_WRAPC_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(SEG_ID_SZ,1024, WORKING_AREA_CHANEL_A);
	 UTIL_Printf("_pucSegIdWrapper[u4InstID] = 0x%x, size is 0x%x\n", _pucSegIdWrapper[u4InstID], SEG_ID_SZ);
#else
      #if VDEC_VP8_COPY_SEGID_SUPPORT
	  _pucSegIdWrapper[u4InstID] = _pucWorkYBuf[u4InstID] + PIC_Y_SZ+PIC_C_SZ;
	  UTIL_Printf("_pucSegIdWrapper[u4InstID] = 0x%x, size is 0x%x\n", _pucSegIdWrapper[u4InstID], 256*1024);

      _pucDumpArfYBuf[u4InstID] = _pucSegIdWrapper[u4InstID] + 256*1024; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ+PIC_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpArfYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpArfYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);

	  #else
      _pucDumpArfYBuf[u4InstID] = _pucWorkYBuf[u4InstID] + PIC_Y_SZ+PIC_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ+PIC_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpArfYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpArfYBuf[u4InstID],PIC_Y_SZ+PIC_C_SZ);
	  #endif
      _pucDumpGldYBuf[u4InstID] = _pucDumpArfYBuf[u4InstID] + PIC_Y_SZ+PIC_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ+PIC_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpGldYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpGldYBuf[u4InstID],PIC_Y_SZ+PIC_C_SZ);
      _pucDumpLstYBuf[u4InstID] = _pucDumpGldYBuf[u4InstID] + PIC_Y_SZ+PIC_C_SZ; //(UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ+PIC_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpLstYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpLstYBuf[u4InstID],PIC_Y_SZ+PIC_C_SZ);

	  UTIL_Printf("Physical LstYAddr  0x%x\n", PHYSICAL((UINT32)_pucDumpLstYBuf[u4InstID]));
      UTIL_Printf("Physical GldYAddr  0x%x\n", PHYSICAL((UINT32)_pucDumpGldYBuf[u4InstID]));
      UTIL_Printf("Physical AlfYAddr  0x%x\n", PHYSICAL((UINT32)_pucDumpArfYBuf[u4InstID]));
      UTIL_Printf("Physical CurYAddr  0x%x\n", PHYSICAL((UINT32)_pucWorkYBuf[u4InstID]));
#endif
#if VDEC_VP8_WEBP_SUPPORT
      _pucWebpReszWorkBuf[u4InstID] = _pucSegIdWrapper[u4InstID] + SEG_ID_SZ;
      _pucWebpReszFrameYAddr[u4InstID] = _pucWebpReszWorkBuf[u4InstID] + WEBP_RESZ_TMP_SIZE;
      _pucWebpReszFrameCAddr[u4InstID] = _pucWebpReszFrameYAddr[u4InstID] + PIC_Y_SZ;

	  UTIL_Printf("_pucWebpReszWorkBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucWebpReszWorkBuf[u4InstID], WEBP_RESZ_TMP_SIZE);
      UTIL_Printf("_pucWebpReszFrameYAddr[u4InstID] = 0x%x, size is 0x%x\n", _pucWebpReszFrameYAddr[u4InstID], WEBP_RESZ_TMP_SIZE);
	  UTIL_Printf("_pucWebpReszFrameCAddr[u4InstID] = 0x%x, size is 0x%x\n", _pucWebpReszFrameCAddr[u4InstID], WEBP_RESZ_TMP_SIZE);

	  
      _pucPpYSa[u4InstID] =  _pucWebpReszFrameCAddr[u4InstID] + PIC_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      UTIL_Printf("_pucPpYSa[u4InstID] = 0x%x, size is 0x%x\n", _pucPpYSa[u4InstID], DEC_PP_Y_SZ);
#else

      _pucPpYSa[u4InstID] =  _pucDumpLstYBuf[u4InstID] + PIC_Y_SZ+PIC_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      UTIL_Printf("_pucPpYSa[u4InstID] = 0x%x, size is 0x%x\n", _pucPpYSa[u4InstID], DEC_PP_Y_SZ);
#endif
      _pucPpCSa[u4InstID] = _pucPpYSa[u4InstID] + DEC_PP_Y_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A);
	  UTIL_Printf("_pucPpCSa[u4InstID] = 0x%x, size is 0x%xn", _pucPpCSa[u4InstID], DEC_PP_C_SZ);




//      UTIL_Printf("_pucWorkCBuf[u4InstID] = 0x%x\n", _pucWorkCBuf[u4InstID]);




      if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
      {
          _pucAddressSwapBuf[u4InstID] = _pucPpCSa[u4InstID]  + DEC_PP_C_SZ;//(UCHAR *)x_alloc_aligned_verify_mem(ADDSWAP_BUF_SZ, 2048, WORKING_AREA_CHANEL_A);
          UTIL_Printf("_pucAddressSwapBuf[u4InstID] = 0x%x\n", _pucAddressSwapBuf[u4InstID]);
      }


#else
	_pucSizeFileBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(1024*400,2048, WORKING_AREA_CHANEL_A);

	_pucWorkYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ+PIC_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
	//      _pucWorkCBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);

#if VDEC_VP8_WEBP_SUPPORT
      _pucDumpArfYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ+DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpArfYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpArfYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);
      _pucDumpGldYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ+DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpGldYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpGldYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);
      _pucDumpLstYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ+DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpLstYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpLstYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);
     _pucVLDWrapper[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VLD_PRED_SZ,1024, WORKING_AREA_CHANEL_A);
	 UTIL_Printf("_pucVLDWrapper[u4InstID] = 0x%x, size is 0x%x\n", _pucVLDWrapper[u4InstID], VLD_PRED_SZ);
     _pucPPWrapperY[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PP_WRAPY_SZ,1024, WORKING_AREA_CHANEL_A);
	 UTIL_Printf("_pucPPWrapperY[u4InstID] = 0x%x, size is 0x%x\n", _pucPPWrapperY[u4InstID], PP_WRAPY_SZ);
     _pucPPWrapperC[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PP_WRAPC_SZ,1024, WORKING_AREA_CHANEL_A);
	 UTIL_Printf("_pucPPWrapperC[u4InstID] = 0x%x, size is 0x%x\n", _pucPPWrapperC[u4InstID], PP_WRAPC_SZ);
     _pucSegIdWrapper[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(SEG_ID_SZ,1024, WORKING_AREA_CHANEL_A);
	 UTIL_Printf("_pucSegIdWrapper[u4InstID] = 0x%x, size is 0x%x\n", _pucSegIdWrapper[u4InstID], SEG_ID_SZ);
#else
      _pucDumpArfYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ+PIC_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpArfYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpArfYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);
      _pucDumpGldYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ+PIC_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpGldYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpGldYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);
      _pucDumpLstYBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ+PIC_C_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
      UTIL_Printf("_pucDumpLstYBuf[u4InstID] = 0x%x, size is 0x%x\n", _pucDumpLstYBuf[u4InstID],DEC_PP_Y_SZ+DEC_PP_C_SZ);
#endif
      _pucPpYSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_Y_SZ,2048, WORKING_AREA_CHANEL_A);
      UTIL_Printf("_pucPpYSa[u4InstID] = 0x%x, size is 0x%x\n", _pucPpYSa[u4InstID], DEC_PP_Y_SZ);
      _pucPpCSa[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(DEC_PP_C_SZ,2048, WORKING_AREA_CHANEL_A);
	  UTIL_Printf("_pucPpCSa[u4InstID] = 0x%x, size is 0x%xn", _pucPpCSa[u4InstID], DEC_PP_C_SZ);




//      UTIL_Printf("_pucWorkCBuf[u4InstID] = 0x%x\n", _pucWorkCBuf[u4InstID]);




      if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
      {
          _pucAddressSwapBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(ADDSWAP_BUF_SZ, 2048, WORKING_AREA_CHANEL_A);
          UTIL_Printf("_pucAddressSwapBuf[u4InstID] = 0x%x\n", _pucAddressSwapBuf[u4InstID]);
      }
#endif
	}
#if 0
	else
	if (_u4CodecVer[u4InstID] == VDEC_AVS)
	{
	_pucPic0Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A); //UCHAR address
	_pucPic0C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
	_pucPic1Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
	_pucPic1C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
	_pucPic2Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
	_pucPic2C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
	_pucPic3Y[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_Y_SZ,2048, WORKING_AREA_CHANEL_A);
	_pucPic3C[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(PIC_C_SZ,2048, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucPic0Y[u4InstID] = 0x%x\n", _pucPic0Y[u4InstID]);
	UTIL_Printf("_pucPic0C[u4InstID] = 0x%x\n", _pucPic0C[u4InstID]);
	UTIL_Printf("_pucPic1Y[u4InstID] = 0x%x\n", _pucPic1Y[u4InstID]);
	UTIL_Printf("_pucPic1C[u4InstID] = 0x%x\n", _pucPic1C[u4InstID]);
	UTIL_Printf("_pucPic2Y[u4InstID] = 0x%x\n", _pucPic2Y [u4InstID]);
	UTIL_Printf("_pucPic2C[u4InstID] = 0x%x\n", _pucPic2C[u4InstID]);
	UTIL_Printf("_pucPic3Y[u4InstID] = 0x%x\n", _pucPic3Y [u4InstID]);
	UTIL_Printf("_pucPic3C[u4InstID] = 0x%x\n", _pucPic3C[u4InstID]);

	_pucAvsPred[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVS_VLD_PRED_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucAvsMv1[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVS_VLD_MV_SZ,1024, WORKING_AREA_CHANEL_A);
	_pucAvsMv2[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(AVS_VLD_MV_SZ,1024, WORKING_AREA_CHANEL_A);
	UTIL_Printf("_pucDcac[u4InstID] = 0x%x\n", _pucAvsPred[u4InstID]);
	UTIL_Printf("_pucMv_1[u4InstID] = 0x%x\n", _pucAvsMv1[u4InstID]);
	UTIL_Printf("_pucMv_2[u4InstID] = 0x%x\n", _pucAvsMv2[u4InstID]);

	if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
	{
	  _pucAddressSwapBuf[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(ADDSWAP_BUF_SZ, 2048, WORKING_AREA_CHANEL_A);
	  UTIL_Printf("_pucAddressSwapBuf[u4InstID] = 0x%x\n", _pucAddressSwapBuf[u4InstID]);
	}
	}
#endif

   #if VDEC_SUPPORT_IOMMU

       _pucVFifo[u4InstID] = (UCHAR *)((BSP_VAllocFragmentMem2(V_FIFO_SZ)));
	   x_memset(_pucVFifo[u4InstID],0,V_FIFO_SZ);

	   UTIL_Printf("Test _pucVFifo[u4InstID] = 0x%x, size is 0x%lx\n", _pucVFifo[u4InstID], V_FIFO_SZ);
   #endif

#endif
return TRUE;
}


void vVDecFreeWorkBuffer(UINT32 u4InstID)
{
UTIL_Printf("vVDecFreeWorkBuffer, u4InstID:%d\n", u4InstID);

#ifdef DYNAMIC_MEMORY_ALLOCATE

if(_u4CodecVer[u4InstID] == VDEC_H264)
{
#ifdef CONFIG_DRV_MEM
#else
x_free_aligned_verify_mem(_pucDPB[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPredSa[u4InstID], WORKING_AREA_CHANEL_A);
#endif
#ifdef VERIFICATION_FGT
x_free_aligned_verify_mem(_pucFGSeedbase[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucFGDatabase[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucFGSEISa[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucFGTBuf[u4InstID], WORKING_AREA_CHANEL_A);
#endif

if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
{
  x_free_aligned_verify_mem(_pucAddressSwapBuf[u4InstID], WORKING_AREA_CHANEL_A);
}

#if VDEC_H264_REDUCE_MV_BUFF
#ifdef CONFIG_DRV_MEM
#else
if (u4InstID == 0)
x_free_aligned_verify_mem(_pucAVCMVBuff_Main[0], WORKING_AREA_CHANEL_A);
else
x_free_aligned_verify_mem(_pucAVCMVBuff_Sub[0], WORKING_AREA_CHANEL_A);
#endif
#endif

  #ifdef DOWN_SCALE_SUPPORT
     x_free_aligned_verify_mem(_pucVDSCLBuf[u4InstID], WORKING_AREA_CHANEL_A);
     x_free_aligned_verify_mem(_pucVDSCLWorkBuf[u4InstID], WORKING_AREA_CHANEL_A);
     x_free_aligned_verify_mem(_pucVDSCLWork1Sa[u4InstID], WORKING_AREA_CHANEL_A);
     x_free_aligned_verify_mem(_pucVDSCLWork2Sa[u4InstID], WORKING_AREA_CHANEL_A);
     x_free_aligned_verify_mem(_pucVDSCLWork3Sa[u4InstID], WORKING_AREA_CHANEL_A);
     x_free_aligned_verify_mem(_pucVDSCLWork4Sa[u4InstID], WORKING_AREA_CHANEL_A);
   #endif
  }
  else if(_u4CodecVer[u4InstID] == VDEC_H265)
  {
    _fgVDecWorkBufAllocated[u4InstID] = FALSE;
    UTIL_Printf("reallocate memory next time \n");
  }
  else
    if (     _u4CodecVer[u4InstID] == VDEC_WMV
  	|| _u4CodecVer[u4InstID] == VDEC_MPEG1
  	|| _u4CodecVer[u4InstID] == VDEC_MPEG2
  	|| _u4CodecVer[u4InstID] == VDEC_MPEG4
  	|| _u4CodecVer[u4InstID] == VDEC_H263
  	|| _u4CodecVer[u4InstID] == VDEC_DIVX3)
  {
#ifdef CONFIG_DRV_MEM
#else
x_free_aligned_verify_mem(_pucVDSCLBuf[u4InstID], WORKING_AREA_CHANEL_A);
#endif

#ifdef DOWN_SCALE_SUPPORT
x_free_aligned_verify_mem(_pucVDSCLWorkBuf[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucVDSCLWork1Sa[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucVDSCLWork2Sa[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucVDSCLWork3Sa[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucVDSCLWork4Sa[u4InstID], WORKING_AREA_CHANEL_A);
#endif


#ifdef CONFIG_DRV_MEM
#else
x_free_aligned_verify_mem(_pucPic0Y[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic0C[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic1Y[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic1C[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic2Y[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic2C[u4InstID], WORKING_AREA_CHANEL_A);
_pucPic0Y[u4InstID] = 0;
_pucPic0C[u4InstID] = 0;
_pucPic1Y[u4InstID] = 0;
_pucPic1C[u4InstID] = 0;
_pucPic2Y[u4InstID] = 0;
_pucPic2C[u4InstID] = 0;
#endif


if (_u4WmvMode[u4InstID] == 0)
{
#ifdef CONFIG_DRV_MEM
#else
x_free_aligned_verify_mem(_pucDcac[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucMv_1[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucMv_2[u4InstID], WORKING_AREA_CHANEL_A);
_pucDcac[u4InstID] = 0;
_pucMv_1[u4InstID] = 0;
_pucMv_2[u4InstID] = 0;


x_free_aligned_verify_mem(_pucBp_1[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucBp_2[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucBp_3[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucBp_4[u4InstID], WORKING_AREA_CHANEL_A);
_pucBp_1[u4InstID] = 0;
_pucBp_2[u4InstID] = 0;
_pucBp_3[u4InstID] = 0;
_pucBp_4[u4InstID] = 0;
#endif
}

if (_u4WmvMode[u4InstID] == 0)
{
#ifdef CONFIG_DRV_MEM
#else
x_free_aligned_verify_mem(_pucMv_3[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucMv_1_2[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucDcac_2[u4InstID], WORKING_AREA_CHANEL_A);
_pucMv_3[u4InstID] = 0;
_pucMv_1_2[u4InstID] = 0;
_pucDcac_2[u4InstID] = 0;
#endif
}
else
{
#ifdef CONFIG_DRV_MEM
#else
x_free_aligned_verify_mem(_pucDcacNew[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucMvNew[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucBp0New[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucBp1New[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucBp2New[u4InstID], WORKING_AREA_CHANEL_A);
_pucDcacNew[u4InstID] = 0;
_pucMvNew[u4InstID] = 0;
_pucBp0New[u4InstID] = 0;
_pucBp1New[u4InstID] = 0;
_pucBp2New[u4InstID] = 0;
#endif
}
#ifdef CONFIG_DRV_MEM
#else
x_free_aligned_verify_mem(_pucPp_1[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPp_2[u4InstID], WORKING_AREA_CHANEL_A);
_pucPp_1[u4InstID] = 0;
_pucPp_2[u4InstID] = 0;
#endif

// MPEG part
#ifdef CONFIG_DRV_MEM
#else
x_free_aligned_verify_mem(_pucPpYSa[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPpCSa[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucMp4Dcac[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucMp4Mvec[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucMp4Bmb1[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucMp4Bmb2[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucMp4Bcode[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucDumpSRAMBuf[u4InstID],WORKING_AREA_CHANEL_A);
_pucPpYSa[u4InstID] = 0;
_pucPpCSa[u4InstID] = 0;
_pucMp4Dcac[u4InstID] = 0;
_pucMp4Mvec[u4InstID] = 0;
_pucMp4Bmb1[u4InstID] = 0;
_pucMp4Bmb2[u4InstID] = 0;
_pucMp4Bcode[u4InstID] = 0;
_pucDumpSRAMBuf[u4InstID] = 0;
#endif
}
else
if (_u4CodecVer[u4InstID] == VDEC_VP6)
{
x_free_aligned_verify_mem(_pucSizeFileBuf[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic0Y[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic0C[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic1Y[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic1C[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic2Y[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic2C[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPpYSa[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPpCSa[u4InstID], WORKING_AREA_CHANEL_A);
_pucPic0Y[u4InstID] = 0;
_pucPic0C[u4InstID] = 0;
_pucPic1Y[u4InstID] = 0;
_pucPic1C[u4InstID] = 0;
_pucPic2Y[u4InstID] = 0;
_pucPic2C[u4InstID] = 0;
_pucPpYSa[u4InstID] = 0;
_pucPpCSa[u4InstID] = 0;

if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
{
  x_free_aligned_verify_mem(_pucAddressSwapBuf[u4InstID], WORKING_AREA_CHANEL_A);
  _pucAddressSwapBuf[u4InstID] = 0;
}
}
else
if (_u4CodecVer[u4InstID] == VDEC_VP8)
{
x_free_aligned_verify_mem(_pucSizeFileBuf[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucWorkYBuf[u4InstID], WORKING_AREA_CHANEL_A);
//       x_free_aligned_verify_mem(_pucWorkCBuf[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucDumpArfYBuf[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucDumpGldYBuf[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucDumpLstYBuf[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPpYSa[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPpCSa[u4InstID], WORKING_AREA_CHANEL_A);
#if VDEC_VP8_WEBP_SUPPORT
x_free_aligned_verify_mem(_pucVLDWrapper[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPPWrapperY[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPPWrapperC[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucSegIdWrapper[u4InstID], WORKING_AREA_CHANEL_A);
#endif
_pucWorkYBuf[u4InstID] = 0;
//        _pucWorkCBuf[u4InstID] = 0;
_pucDumpArfYBuf[u4InstID] = 0;
_pucDumpGldYBuf[u4InstID] = 0;
_pucDumpLstYBuf[u4InstID] = 0;
_pucPpYSa[u4InstID] = 0;
_pucPpCSa[u4InstID] = 0;
#if VDEC_VP8_WEBP_SUPPORT
_pucVLDWrapper[u4InstID] = 0;
_pucSegIdWrapper[u4InstID] = 0;
_pucPPWrapperY[u4InstID] = 0;
_pucPPWrapperC[u4InstID] = 0;
#endif
if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
{
  x_free_aligned_verify_mem(_pucAddressSwapBuf[u4InstID], WORKING_AREA_CHANEL_A);
  _pucAddressSwapBuf[u4InstID] = 0;
}
}
else if (_u4CodecVer[u4InstID] == VDEC_VP9)
{
//	    x_free_aligned_verify_mem(_pucSizeFileBuf[u4InstID], WORKING_AREA_CHANEL_A);
}
#if 0
else
if (_u4CodecVer[u4InstID] == VDEC_AVS)
{
x_free_aligned_verify_mem(_pucPic0Y[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic0C[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic1Y[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic1C[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic2Y[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic2C[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic3C[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucPic3C[u4InstID], WORKING_AREA_CHANEL_A);

x_free_aligned_verify_mem(_pucAvsPred[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucAvsMv1[u4InstID], WORKING_AREA_CHANEL_A);
x_free_aligned_verify_mem(_pucAvsMv2[u4InstID], WORKING_AREA_CHANEL_A);

if (_u2AddressSwapMode[u4InstID] != ADDRSWAP_OFF)
{
  x_free_aligned_verify_mem(_pucAddressSwapBuf[u4InstID], WORKING_AREA_CHANEL_A);
}
}
#endif
#endif
return;
}
extern void HalFlushInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
extern void HalFlushDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);
extern void HalInvalidateDCacheMultipleLine(UINT32 u4Addr, UINT32 u4Size);

void vVDec_FlushDCacheRange(UINT32 u4Start, UINT32 u4Len)
{
    HalFlushInvalidateDCacheMultipleLine(u4Start,u4Len);
}

void vVDec_CleanDCacheRange(UINT32 u4Start, UINT32 u4Len)
{
    HalFlushDCacheMultipleLine(u4Start,u4Len);
}

void vVDec_InvDCacheRange(UINT32 u4Start, UINT32 u4Len)
{
    HalInvalidateDCacheMultipleLine(u4Start,u4Len);
}

#ifdef CONFIG_DRV_MEM
UINT32 u4GetVFIFOAddr(UINT32 u4InstID)
{
    UINT32 u4VfifoAddr;
    if(u4InstID == 0)
    {
        u4VfifoAddr = (UINT32)get_drvmem_mem();
    }
    else if(u4InstID == 1)
    {
        u4VfifoAddr = (UINT32)get_drvmem_mem() + 0x4000000;
        //u4VfifoAddr = (UINT32)get_drvmem_mem();
    }

    return u4VfifoAddr;
}
#endif


