#include "vdec_verify_mpv_prov.h"
#include "vdec_verify_vdecode.h"
#include "vdec_hal_if_common.h"
#include "vdec_hal_if_wmv.h"
#include "vdec_hal_if_mpeg.h"
#include "vdec_hal_if_h264.h"
#include "vdec_hal_if_h265.h"
#include "vdec_info_common.h"
#include "vdec_verify_dvdec.h"
#include "vdec_info_dv.h"
#include "vdec_hal_if_rm.h"
#include "vdec_hal_if_vp6.h"
#include "vdec_hal_if_vp8.h"
#include "vdec_verify_vparser_h265.h"

#include <linux/vmalloc.h>

#ifndef CONFIG_TV_DRV_VFY
#include <mach/cache_operation.h>
#endif //CONFIG_TV_DRV_VFY

//#include "vdec_hw_dvdec.h"
#include "vdec_hw_common.h"
#include "vdec_hw_h264.h"
#include "vdec_hw_vp6.h"
#include "vdec_hw_vp8.h"
#include "vdec_hw_h265.h"

#include "x_bim.h"
#include "x_os.h"
#include "x_assert.h"
#include "x_debug.h"
#if (!CONFIG_DRV_LINUX)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#endif


//#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
//#include "x_hal_8520.h"
//#else
//#include "x_hal_8530.h"
//#endif

#ifdef CONFIG_TV_DRV_VFY
#include "x_hal_5381.h"
#else
#include "x_hal_ic.h"
#endif //CONFIG_TV_DRV_VFY


#ifdef VERIFICATION_FGT
#include "vdec_verify_fgt.h"
#endif

#include "vdec_verify_file_common.h"
#include "vdec_verify_filesetting.h"
#include "vdec_verify_irq_fiq_proc.h"
#include "vdec_verify_common.h"

#include "vdec_verify_vparser_mpeg.h"
#include "vdec_verify_vparser_mpeg4.h"
#include "vdec_verify_vparser_wmv.h"
#include "vdec_verify_vparser_h264.h"
#include "vdec_verify_vparser_rm.h"
#include "vdec_verify_vparser_vp6.h"
#include "vdec_verify_vparser_avs.h"
#include "vdec_verify_vparser_vp8.h"
#include "vdec_verify_vparser_vp9.h"


extern int rand(void);

extern void reset_pic_hdr_bits(UINT32 u4InstID);
extern UINT32 pic_hdr_bitcount(UINT32 u4InstID) ;
extern void x_free_aligned_verify_mem(void *pvAddr, BOOL fgChannelA);
//#define HEVC_UFO_MODE
extern int vH265DumpMem( UINT32 u4InstID, unsigned char* buf, unsigned int size , int  frame_num , unsigned int type , bool isTimeout);
//	extern void vH265DumpInfo(UINT32 u4VDecID);

#if 0
extern FILE *fopen(const UINT8 *filename, const UINT8 *mode);
extern INT32 fread(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern INT32 fwrite(void *ptr, UINT32 size, UINT32 n, FILE *stream);
extern INT32 fseek(FILE *stream, INT32 offset, INT32 whence);
extern INT32 fclose(FILE *stream);
extern INT64 ftell(FILE *stream);
#endif

void vNormDecProc(UINT32 u4InstID,UCHAR ucVldID);
void vVerifyWMVInitProc(UINT32 u4InstID);
void vInitVParserMPEG(UINT32 u4InstID);
void ComputeDQuantDecParam(UINT32 u4InstID);
void vVerInitVDec(UINT32 u4InstID,UCHAR ucVldID);
void vVDecProc(UINT32 u4InstID,UCHAR ucVldID);
void vSetDownScaleParam(UINT32 u4InstID, BOOL fgEnable, VDEC_INFO_VDSCL_PRM_T *prDownScalerPrm);
void vCodecVersion(UINT32 u4InstID, UINT32 u4CodecFOURCC);
void vChkVDec(UINT32 u4InstID,UCHAR ucVldID);
void vH264VDecEnd(UINT32 u4InstID);
void vH265VDecEnd(UINT32 u4InstID);
void vVerifyFlushBufInfo(UINT32 u4InstID);
BOOL fgIsH264VDecComplete(UINT32 u4InstID);
void vVerifyAdapRefPicmarkingProce(UINT32 u4InstID);
void vVerifySetPicRefType(UINT32 u4InstID, UCHAR ucPicStruct, UCHAR ucRefType);
UCHAR bGetPicRefType(UINT32 u4InstID, UCHAR ucPicStruct);
void vChkOutputFBuf(UINT32 u4InstID);
void vAdd2RefPicList(UINT32 u4InstID);
void vVerifyClrPicRefInfo(UINT32 u4InstID, UCHAR ucPicType, UCHAR ucFBufIdx);

void vVerifyFlushAllSetData(UINT32 u4InstID);
void vH264DecEndProc(UINT32 u4InstID);
void vWMVDecEndProc(UINT32 u4InstID);
void vMPEGDecEndProc(UINT32 u4InstID);
void vVerifyDx3SufxChk(UINT32 u4InstID);
void vMp4FixBCode(UINT32 u4InstID);
void PostAdjustReconRange(UINT32 u4InstID);
void vWMVVDecEnd(UINT32 u4InstID);
BOOL fgIsWMVVDecComplete(UINT32 u4InstID);
void vVerifySetVSyncPrmBufPtr(UINT32 u4InstID, UINT32 u4BufIdx);
void vReadWMVChkSumGolden(UINT32 u4InstID);
void vReadH264ChkSumGolden(UINT32 u4InstID);
void vReadMPEGChkSumGolden(UINT32 u4InstID);
void vVerifySetUpParm(UINT32 u4InstID, UINT32 dwPicW, UINT32 dwPicH, UINT32 dwFrmRatCod, BOOL fgDivXM4v, BOOL fgDx4M4v);
void vDvCompare(UINT32 u4InstID);
void vReadDvChkSumGolden(UINT32 u4InstID);
void vPrintMpegChksum(UINT32 u4InstID);


void vVParserProc(UINT32 u4InstID,UCHAR ucVldID);
void vVerifyInitVParserWMV(UINT32 u4InstID);
void vVPrsMPEGIPProc(UINT32 u4InstID);
void vVPrsMPEGBProc(UINT32 u4InstID);
void vVDecLaeInit(UINT32 u4InstID);
void vVDecLaeProc(UINT32 u4InstID,UCHAR ucVldID);
void vVDecDumpLaeBuffer(UINT32 u4InstID);

#ifdef VPMODE
INT32 i4VPModeDecStart(UINT32 u4VDecID,VDEC_INFO_DEC_PRM_T *prDecPrm);
#endif

void vAVCDumpChkSum(void);
void vPrintDumpReg(void);

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
void vDumpSram(UINT32 u4InstID);
void vWriteSram(UINT32 u4InstID,UINT32 u4SramAddr,UINT32 u4SramValue);
UINT32 u4ReadSram(UINT32 u4InstID,UINT32 u4SramAddr);
#endif


#ifdef MPEG_CRC_ENABLE
void vMPEG_CRC_Chk(UINT32 u4InstID);
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
	void vVDecBufferFlush(UINT32 u4InstID,UINT32 u4BufIdx);
#endif

// Notice VDEC_VFY_TEST_POWER_DOWN_CTRL in all place
// If open this define for test H264, need mark vVDecResetHW in i4VDEC_HAL_H264_InitBarrelShifter
#define VDEC_VFY_TEST_POWER_DOWN_CTRL (0)
#if VDEC_VFY_TEST_POWER_DOWN_CTRL
BOOL fgTestPowerDownCtrl(UINT32 u4InstID);
#endif

#if DTV_COMMON_CONFIG
void vVDecTestAVCBarl(void);
void vVDecTestMemorySet(void);
void vVDecTestBar(void);
void vVDecTestRegRW(void);

void vVDecTestRMBarl(void);
void vVDecTestVP6Barl(void);
void vVDecTestVP8Barl(void);

UCHAR *ucVFifo;
UCHAR *pvaVfifo;
#define TEST_VFIFO_SIZE     0x400

void vVDecTestBar(void)
{
    UINT32 i,val;
    for(i = 0 ; i < 256; i++)
    {
        val = u4VDecAVCVLDGetBitS(0, 0, 0);
        val = (val >> 24);
        UTIL_Printf("BS(%d) = (0x%x) golden (0x%x)\n",i,val,pvaVfifo[i]);
        u4VDecAVCVLDGetBitS(0, 0, 8);
    }
}


void vVDecTestVp6Bar(void)
{
    UINT32 i,val;
    for(i = 0 ; i < 256; i++)
    {
        val = u4VDecVP6VLDGetBits(0, 0, 0);
		
        val = (val >> 24);
        UTIL_Printf("BS(%d) = (0x%x) golden (0x%x)\n",i,val,pvaVfifo[i]);
        u4VDecVP6VLDGetBits(0, 0, 8);
    }
}


void vVDecTestVp8Bar(void)
{
    UINT32 i,val;
    for(i = 0 ; i < 256; i++)
    {
        val = u4VDecVP8VLDGetBits(0, 0, 0);
		
        val = (val >> 24);
        UTIL_Printf("BS(%d) = (0x%x) golden (0x%x)\n",i,val,pvaVfifo[i]);
        u4VDecVP8VLDGetBits(0, 0, 8);
    }
}



void vVDecTestRegRW(void)
{
    UINT32 i,val;
    for(i = 0 ; i < 10; i++)
    {
        vVDecWriteMC(0,i << 2,i+0x1000);
        UTIL_Printf("MC_WRITE(%d) = (0x%x)\n",i,i+0x1000);
    }
    
    for(i = 0 ; i < 10; i++)
    {
        val = u4VDecReadMC(0,i << 2);
        UTIL_Printf("MC_READ(%d) = (0x%x)\n",i,val);
    }
}
extern void HalFlushInvalidateDCache(void);
void vVDecTestMemorySet(void)
{
    UINT32 i;
    for(i = 0; i < TEST_VFIFO_SIZE; i++)
    {
        pvaVfifo[i] = i;
    }
    //vVDec_FlushDCacheRange((UINT32)pvaVfifo,(UINT32)(pvaVfifo + TEST_VFIFO_SIZE) );
    HalFlushInvalidateDCache();
    
}
void vVDecTestAVCBarl(void)
{
    VDEC_INFO_H264_INIT_PRM_T rH264VDecInitPrm;
    VDEC_INFO_H264_BS_INIT_PRM_T rH264BSInitPrm;
    _tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = 0xff;
    _tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicW = 0;
    _tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicH = 0;
    _tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.prSEI = &_rSEI[0];
    _tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.prFGTPrm= &_rFGTPrm[0];
    _u4TotalDecFrms[0] = 0;
    vVDecTestMemorySet();
    vOutputPOCData(0xFFFFFFFF);
    vVerifyFlushBufInfo(0);
    vVerifyFlushAllSetData(0);
    vSetDecFlag(0, DEC_FLAG_CHG_FBUF);
    i4VDEC_HAL_H264_InitVDecHW(0);
    i4VDEC_HAL_Common_Init(0);
    rH264BSInitPrm.u4VFifoSa = (UINT32)ucVFifo;
    rH264BSInitPrm.u4VFifoEa = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    rH264BSInitPrm.u4VLDRdPtr = (UINT32)ucVFifo;
    rH264BSInitPrm.u4VLDWrPtr = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    //vVDecTestRegRW();
    i4VDEC_HAL_H264_InitBarrelShifter(0, 0, &rH264BSInitPrm);
    while(0)
    {
        x_thread_delay(100);
    }
    vVDecTestBar();
    
  }


void vVDecTestRMBarl(void)
{
    VDEC_INFO_RM_BS_INIT_PRM_T rRMBSInitPrm;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = 0xff;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicW = 0;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicH = 0;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.prSEI = &_rSEI[0];
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.prFGTPrm= &_rFGTPrm[0];
    //_u4TotalDecFrms[0] = 0;
    vVDecTestMemorySet();
    vOutputPOCData(0xFFFFFFFF);
    vVerifyFlushBufInfo(0);
    vVerifyFlushAllSetData(0);
    vSetDecFlag(0, DEC_FLAG_CHG_FBUF);
    {
		vVDecResetHW(0, VDEC_FMT_RV);

        //Init RM VDec HW
		vRM_Hal_VldSoftReset(0);

		vRM_Hal_MvInit(0, _tVerMpvDecPrm[0].SpecDecPrm.rVDecRMDecPrm.u4MvHwWorkBuf);

		vRM_Hal_McInit(0);

		vRM_Hal_PpInit(0);

		//Init RM BS HW
		vRM_Hal_VldInit(0, _tVerMpvDecPrm[0].SpecDecPrm.rVDecRMDecPrm.u4VldPredWorkBuf);
    }
    i4VDEC_HAL_Common_Init(0);
    rRMBSInitPrm.u4VFifoSa = (UINT32)ucVFifo;
    rRMBSInitPrm.u4VFifoEa = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    rRMBSInitPrm.u4ReadPointer = (UINT32)ucVFifo;
    rRMBSInitPrm.u4WritePointer = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    //vVDecTestRegRW();
    i4RM_HAL_InitBarrelShifter(0, 0, &rRMBSInitPrm);
    while(0)
    {
        x_thread_delay(100);
    }
    vVDecTestBar();
}


void vVDecTestVP6Barl(void)
{
	VDEC_INFO_VP6_VFIFO_PRM_T     rVp6VDecInitPrm;
    VDEC_INFO_VP6_BS_INIT_PRM_T  rVp6BSInitPrm;
	
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = 0xff;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicW = 0;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicH = 0;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.prSEI = &_rSEI[0];
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.prFGTPrm= &_rFGTPrm[0];
    //_u4TotalDecFrms[0] = 0;
    vVDecTestMemorySet();
    vOutputPOCData(0xFFFFFFFF);
    vVerifyFlushBufInfo(0);
    vVerifyFlushAllSetData(0);
    vSetDecFlag(0, DEC_FLAG_CHG_FBUF);

	rVp6VDecInitPrm.u4VFifoSa = (UINT32)ucVFifo;
    rVp6VDecInitPrm.u4VFifoEa = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    i4VDEC_HAL_VP6_InitVDecHW(0, &rVp6VDecInitPrm);

    i4VDEC_HAL_Common_Init(0);
    rVp6BSInitPrm.u4VFifoSa = (UINT32)ucVFifo;
    rVp6BSInitPrm.u4VFifoEa = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    rVp6BSInitPrm.u4ReadPointer = (UINT32)ucVFifo;
    rVp6BSInitPrm.u4WritePointer = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    //vVDecTestRegRW();
    i4VDEC_HAL_VP6_InitBarrelShifter(0, 0, &rVp6BSInitPrm);
    while(0)
    {
        x_thread_delay(100);
    }
    vVDecTestVp6Bar();

}


void vVDecTestVP8Barl(void)
{
	VDEC_INFO_VP8_VFIFO_PRM_T     rVp8VDecInitPrm;
    VDEC_INFO_VP8_FRM_HDR_T       rVDecVp8FrmHdr;
	
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = 0xff;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicW = 0;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicH = 0;
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.prSEI = &_rSEI[0];
    //_tVerMpvDecPrm[0].SpecDecPrm.rVDecH264DecPrm.prFGTPrm= &_rFGTPrm[0];
    //_u4TotalDecFrms[0] = 0;
    vVDecTestMemorySet();
    vOutputPOCData(0xFFFFFFFF);
    vVerifyFlushBufInfo(0);
    vVerifyFlushAllSetData(0);
    vSetDecFlag(0, DEC_FLAG_CHG_FBUF);

	rVp8VDecInitPrm.u4VFifoSa = (UINT32)ucVFifo;
    rVp8VDecInitPrm.u4VFifoEa = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    i4VDEC_HAL_VP8_InitVDecHW(0, &rVp8VDecInitPrm);

    i4VDEC_HAL_Common_Init(0);
    rVDecVp8FrmHdr.u4FifoStart = (UINT32)ucVFifo;
    rVDecVp8FrmHdr.u4FifoEnd = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    rVDecVp8FrmHdr.u4ReadPointer = (UINT32)ucVFifo;
    rVDecVp8FrmHdr.u4WritePos = (UINT32)ucVFifo + TEST_VFIFO_SIZE;
    //vVDecTestRegRW();
    i4VDEC_HAL_VP8_InitBarrelShifter(0, 0, &rVDecVp8FrmHdr);
    while(0)
    {
        x_thread_delay(100);
    }
    vVDecTestVp8Bar();

}

#endif
// *********************************************************************
// Function    : void vNormDecProc(UINT32 u4InstID,UCHAR ucVldID)
// Description : normal decode procedure
// Parameter   : None
// Return      : None
// *********************************************************************
void vMpvPlay(UINT32 u4InstID,UCHAR ucVldID)
{
  #if VDEC_VFY_TEST_POWER_DOWN_CTRL
  fgTestPowerDownCtrl(u4InstID);
  #endif
  if(_u4CodecVer[u4InstID] == VDEC_WMV)
  {
    vVerifyWMVInitProc(u4InstID);
  }
  while(_u4VerBitCount[u4InstID] < (_tInFileInfo[u4InstID].u4FileLength << 3))
  {
    vNormDecProc(u4InstID,ucVldID);
  }
}

// *********************************************************************
// Function    : void vVerifyWMVInitProc(UINT32 u4InstID)
// Description : WMV initialize process
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyWMVInitProc(UINT32 u4InstID)
{
  VDEC_INFO_WMV_VFIFO_PRM_T rWmvVFifoInitPrm;
  VDEC_INFO_WMV_BS_INIT_PRM_T rWmvBSInitPrm;
  //UINT32 *pu4VFIFOSa;
  char fiInName[256];
  char FileExt[4];
  INT32 iLen,i4RCVNumFrames,i4CodecVersion,u4CodecFOURCC;

  strcpy(fiInName, _bFileStr1[u4InstID][1]);
  iLen = strlen(fiInName);
  if( (fiInName[iLen-4] == '.') && (fiInName[iLen-3] == 'v') &&
      (fiInName[iLen-2] == '9') && (fiInName[iLen-1] == 'e') )
  {
    strcpy(FileExt, "v9e");
    u4CodecFOURCC = FOURCC_WMVA_WMV;
  }
  if( (fiInName[iLen-4] == '.') && (fiInName[iLen-3] == 'v') &&
      (fiInName[iLen-2] == 'c') && (fiInName[iLen-1] == '1') )
  {
    strcpy(FileExt, "vc1");
    u4CodecFOURCC = FOURCC_WVC1_WMV;
  }
  if( (fiInName[iLen-4] == '.') && (fiInName[iLen-3] == 'r') &&
      (fiInName[iLen-2] == 'c') && (fiInName[iLen-1] == 'v') )
  {
    strcpy(FileExt, "rcv");
    i4RCVNumFrames = (*_pucVFifo[u4InstID])+((*(_pucVFifo[u4InstID]+1))<<8)+((*(_pucVFifo[u4InstID]+2))<<16)+((*(_pucVFifo[u4InstID]+3))<<24);//(*pu4VFIFOSa);
    _i4RcvVersion[u4InstID] = (i4RCVNumFrames >> 30) & 0x1;
    i4CodecVersion = i4RCVNumFrames >> 24;
    if (_i4RcvVersion[u4InstID] == 0)
    {
      i4CodecVersion &= 0x7f;
    }
    else
    {
      i4CodecVersion &= 0x3f;
    }

    if(i4CodecVersion == 0)      /* WMV7 */
      u4CodecFOURCC = FOURCC_WMV1_WMV;
    else if(i4CodecVersion == 1)              /* MP43, not supported */
      u4CodecFOURCC = FOURCC_MP43_WMV;
    else if(i4CodecVersion == 2) /* WMV8 */
      u4CodecFOURCC = FOURCC_WMV2_WMV;
    else if(i4CodecVersion== 3)               /* MP42, not supported */
      u4CodecFOURCC = FOURCC_MP42_WMV;
    else if(i4CodecVersion == 4)              /* MP4S, not supported */
      u4CodecFOURCC = FOURCC_MP4S_WMV;
    else if(i4CodecVersion == 5) /* Simple & Main Profile */
      u4CodecFOURCC = FOURCC_WMV3_WMV;
    else if(i4CodecVersion == 6) /* Advanced Profile */
      u4CodecFOURCC = FOURCC_WMVA_WMV;
    else if(i4CodecVersion == 8) /* Advanced Profile */
      u4CodecFOURCC = FOURCC_WVC1_WMV;
  }

  vCodecVersion(u4InstID, u4CodecFOURCC);
  vSetVerFRefBuf(u4InstID, 0);
  vSetVerBRefBuf(u4InstID, 1);
  rWmvVFifoInitPrm.u4CodeType = _i4CodecVersion[u4InstID];
  rWmvVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
  rWmvVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  i4VDEC_HAL_WMV_InitVDecHW(u4InstID,&rWmvVFifoInitPrm);
  rWmvBSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
  rWmvBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  rWmvBSInitPrm.u4ReadPointer= (UINT32)_pucVFifo[u4InstID];
  rWmvBSInitPrm.u4WritePointer= (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;


  if (_i4CodecVersion[u4InstID] == VDEC_VC1)
  {
      i4VDEC_HAL_WMV_InitBarrelShifter(_u4BSID[u4InstID],u4InstID, &rWmvBSInitPrm, TRUE);
  }
  else
  {
     i4VDEC_HAL_WMV_InitBarrelShifter(_u4BSID[u4InstID],u4InstID, &rWmvBSInitPrm, FALSE);
  }
}

// *********************************************************************
// Function    : void vNormDecProc(UINT32 u4InstID)
// Description : normal decode procedure
// Parameter   : None
// Return      : None
// *********************************************************************
void vNormDecProc(UINT32 u4InstID,UCHAR ucVldID)
{
  switch(_tVerDec[u4InstID].ucState)
  {
    case DEC_NORM_INIT_PRM:
      vVerInitVDec(u4InstID,ucVldID);
      //_tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;
      //UTIL_Printf("state[%d] Init Dec\n",u4InstID);
      break;
    case DEC_NORM_VPARSER:
      vVParserProc(u4InstID,ucVldID);
      //UTIL_Printf("state[%d] PARSE->Trig Dec\n",u4InstID);
      break;
    case DEC_NORM_WAIT_TO_DEC:
      vVDecProc(u4InstID,ucVldID);
      //UTIL_Printf("state[%d] Trig Dec->Dec Done\n",u4InstID);
      break;
    case DEC_NORM_WAIT_DECODE:
      vChkVDec(u4InstID,ucVldID);
      //UTIL_Printf("state[%d] Dec Done->Parse\n",u4InstID);
      break;
    case DEC_NORM_LAE_DEC:
      vVDecLaeProc(u4InstID,ucVldID);
      //UTIL_Printf("state[%d] Lae Dec Done->Mcore\n",u4InstID);
      break;
  }
}

// *********************************************************************
// Function    : void vVerInitVDec(UINT32 u4InstID,UCHAR ucVldID)
// Description : Dec procedure initilize
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerInitVDec(UINT32 u4InstID,UCHAR ucVldID)
{
    UINT32 u4HwID;
    #if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
    if(_ucMVCType[u4InstID] == 0)//only base view
    {
        u8VdecFrameTypeDuration[0] = 0;//for acc I/IDR frame dec time
        u8VdecFrameTypeDuration[1] = 0;//for acc P frame dec time
        u8VdecFrameTypeDuration[2] = 0;//for acc B frame dec time
        u8VdecFrameTypeCount[0] = 0;//for I frame count
        u8VdecFrameTypeCount[1] = 0;//for P frame count
        u8VdecFrameTypeCount[2] = 0;//for B frame count
    }
    #endif
    
    if (_u4CodecVer[u4InstID] == VDEC_RM)
    {
      	vRM_VerInitDec(u4InstID);
    }
    else if (_u4CodecVer[u4InstID] == VDEC_H265){   //2013/04/03 CM Hung HEVC verification
     int i,j;
     int InstID;
     VDEC_INFO_H265_INIT_PRM_T rH265VDecInitPrm;
     VDEC_INFO_H265_BS_INIT_PRM_T rH265BSInitPrm;
     x_memset(&rH265VDecInitPrm, 0, sizeof(VDEC_INFO_H265_INIT_PRM_T));     
     x_memset(&rH265BSInitPrm, 0, sizeof(VDEC_INFO_H265_BS_INIT_PRM_T));
     vHEVCInitROM(u4InstID);     
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bIsUFOMode = 0;
#ifdef HEVC_UFO_MODE
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bIsUFOMode = 1;
#else
    if(_u4UFOMode == 1)
    {
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bIsUFOMode = 1;
        _tVerMpvDecPrm[u4InstID].fgIsUFOMode = 1;
    }
#endif
     if(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bIsUFOMode)
        printk("UFO Mode Enable!\n");
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.ucMaxFBufNum = 0xff;
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.rLastInfo.u4LastPicW = 0;
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.rLastInfo.u4LastPicH = 0;  
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.i4PrePOC = 0;
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bFirstSliceInSequence = 1;
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.i4RAPOC = MAX_INT;
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.ucVDecStatus = VDEC_NORMAL_ST;
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4LaeCabacAddr = _pucBSCABACBuff[u4InstID];
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4LaeEcAddr= _pucECInfoBuff[u4InstID];
    
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.prSEI = &_rH265SEI[u4InstID];     
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.fgIsStillPicture = 0;
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4ReadPtrOffset = 0;
     _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4PreReadPtr = 0;
     _u4TotalDecFrms[u4InstID] = 0;
     _u4PrevPtr[u4InstID] = 0;
     _u4PicCnt[u4InstID] = 0;
     _u4VerBitCount[u4InstID]  = 0;
     _u4SkipPicNum[u4InstID] = 0;
     _u4CurrPicStartAddr[1] = 0;    //VP mode
     #if 0
     _u4LAEParseStartAddr[0] = 0;
     _u4LAEParseStartAddr[1] = 0;
     #else
     _u4LAEParseStartAddr[u4InstID] = 0;
     #endif
     _u4AUParseStartAddr[u4InstID] = 0;

    #if 1
    vAllocateRPS(u4InstID);
    #else
    vAllocateRPS(0);
    vAllocateRPS(1);
    #endif
    
    if(_u4DualCoreEnable[u4InstID])
    {
        if(u4InstID == 0)
        {
            vAllocateRPS(VDEC_LAE0);
        }
        else
        {
            vAllocateRPS(VDEC_LAE1);
        }
    }
    for (i = 0; i<256; i++){
        _rH265PPS[u4InstID][i].bSL_Init = 0;
        _rH265PPS[u4InstID][i].bPPSValid = 0;
        for (j = 0; j<MAX_TILES_WITTH_HEIGHT; j++){
            _rH265PPS[u4InstID][i].u4ColumnWidthMinus1[j] = 0;
            _rH265PPS[u4InstID][i].u4RowHeightMinus1[j] = 0;
        }     
    }
    
    //allocate scaling list array
    #if 1
    vAllocateScaleList(u4InstID);
    #else
    vAllocateScaleList(0);
    vAllocateScaleList(1);
    #endif
    //
    if(_u4DualCoreEnable[u4InstID])
    {
        if(u4InstID == 0)
        {
            vAllocateScaleList(VDEC_LAE0);
        }
        else
        {
            vAllocateScaleList(VDEC_LAE1);
        }
    }

    
    x_memset(_pucDPB[u4InstID],0,DPB_SZ);
    vVDec_FlushDCacheRange((UINT32) _pucDPB[u4InstID],DPB_SZ);
    
    vVerifyFlushBufInfo(u4InstID);
    vVerifyFlushAllSetData(u4InstID);
    vSetDecFlag(u4InstID, DEC_FLAG_CHG_FBUF);

    rH265VDecInitPrm.u4FGDatabase = (UINT32)_pucFGDatabase[u4InstID];
    rH265VDecInitPrm.u4FGSeedbase = (UINT32)_pucFGSeedbase[u4InstID];
    if(1)//(_u4DualCoreEnable[u4InstID] == 0)
    {
        i4VDEC_HAL_H265_InitVDecHW(u4InstID);
        rH265BSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
        rH265BSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
        rH265BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID];

    #ifndef  RING_VFIFO_SUPPORT
        rH265BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    #else
        rH265BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
    #endif
        rH265BSInitPrm.u4PredSa = /*PHYSICAL*/((UINT32)_pucPredSa[u4InstID]);
        if(_u4DualCoreEnable[u4InstID] == 1)
        {
            rH265BSInitPrm.u4DualCoreEnable = 1;
        }
        else
        {
            rH265BSInitPrm.u4DualCoreEnable = 0;
        }
        i4VDEC_HAL_H265_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rH265BSInitPrm);
    }
  }
	else if (_u4CodecVer[u4InstID] == VDEC_H264)
    //if(_u4CodecVer[u4InstID] == VDEC_H264)
  	{
	    VDEC_INFO_H264_INIT_PRM_T rH264VDecInitPrm;
	    VDEC_INFO_H264_BS_INIT_PRM_T rH264BSInitPrm;
	    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = 0xff;
	    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicW = 0;
	    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicH = 0;
	    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSEI = &_rSEI[u4InstID];
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prFGTPrm= &_rFGTPrm[u4InstID];
		ucCurrStatus = VDEC_NORMAL_ST;
	    _u4TotalDecFrms[u4InstID] = 0;
		//_u4CurrPicStartAddr[0] = 0;
		_u4CurrPicStartAddr[1] = 0;
		_u4LAEParseStartAddr[0] = 0;
		_u4LAEParseStartAddr[1] = 0;
		//memset(_pucDPB[u4InstID],0,DPB_SZ);
	    vOutputPOCData(0xFFFFFFFF);
	    vVerifyFlushBufInfo(u4InstID);
	    vVerifyFlushAllSetData(u4InstID);
	    vSetDecFlag(u4InstID, DEC_FLAG_CHG_FBUF);
	  #ifdef VERIFICATION_FGT
	    vAllocFGTTable(u4InstID);
	  #endif
	  #ifdef BARREL2_THREAD_SUPPORT
	    VERIFY (x_sema_lock(_ahVDecEndSema[u4InstID], X_SEMA_OPTION_WAIT) == OSR_OK);
	  #endif
	    rH264VDecInitPrm.u4FGDatabase = (UINT32)_pucFGDatabase[u4InstID];
	    rH264VDecInitPrm.u4CompModelValue = (UINT32)(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue);
	    rH264VDecInitPrm.u4FGSeedbase = (UINT32)_pucFGSeedbase[u4InstID];
		#ifdef AVC_UFO_MODE
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsUFOMode = 1;
		_tVerMpvDecPrm[VDEC_LAE0].SpecDecPrm.rVDecH264DecPrm.fgIsUFOMode = 1;
		_tVerMpvDecPrm[u4InstID].fgIsUFOMode = 1;
		#else
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsUFOMode = 0;
		#endif
		#ifdef AVC_16Bit_MODE
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIs16BitMode = 1;
		_tVerMpvDecPrm[VDEC_LAE0].SpecDecPrm.rVDecH264DecPrm.fgIs16BitMode = 1;
		UTIL_Printf("16bit mode\n");
		#else
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIs16BitMode = 0;
		#endif	
		#ifdef AVC_COMPACT_MODE
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsCompactMode = 1;
		_tVerMpvDecPrm[VDEC_LAE0].SpecDecPrm.rVDecH264DecPrm.fgIsCompactMode = 1;
		UTIL_Printf("compact mode\n");
		#else
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsCompactMode = 0;
		#endif
		#ifdef AVC_REORDER_MODE
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsReorderMode = 1;
		UTIL_Printf("reorder mode\n");
		#else
		_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsReorderMode = 0;
		#endif
	
		i4VDEC_HAL_H264_InitVDecHW(u4InstID);
		rH264BSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
		rH264BSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
		rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID];
		rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
		#ifndef  RING_VFIFO_SUPPORT
		rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
		#else
		//  rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
		rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
		#endif
		rH264BSInitPrm.u4PredSa = /*PHYSICAL*/((UINT32)_pucPredSa[u4InstID]);
		i4VDEC_HAL_H264_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rH264BSInitPrm);
	
	  #ifdef BARREL2_THREAD_SUPPORT
	    VERIFY (x_sema_unlock(_ahVDecEndSema[u4InstID]) == OSR_OK);
	  #endif
	}
  else if(_u4CodecVer[u4InstID] == VDEC_WMV)
  {
    vVerifyInitVParserWMV(u4InstID);
    vDEC_HAL_COMMON_SetVLDPower(u4InstID,ON);
    if(_i4CodecVersion[u4InstID] != VDEC_VC1)
    {
      vRCVFileHeader(u4InstID);
      if(_i4CodecVersion[u4InstID] == VDEC_WMV3)
      {
        _u4VprErr[u4InstID] = u4DecodeVOLHead_WMV3(u4InstID);
      }
      else if((_i4CodecVersion[u4InstID] == VDEC_WMV1) || (_i4CodecVersion[u4InstID] == VDEC_WMV2))
      {
        _u4VprErr[u4InstID] = u4DecodeVOLHead_WMV12(u4InstID);
      }
      _u4WMVBitCount[u4InstID] = pic_hdr_bitcount(u4InstID);
      _i4HeaderLen[u4InstID] = _u4WMVBitCount[u4InstID] / 8;
      _iSetPos[u4InstID] = _i4HeaderLen[u4InstID];
    }
    _u4PicHdrBits[u4InstID] = 0;
    _fgCounting[u4InstID] = FALSE;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_VP6)
  {
      vVerInitVP6(u4InstID);
  }
//#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
  else if(_u4CodecVer[u4InstID] == VDEC_VP8)
  {
      vVerInitVP8(u4InstID);
  }
  else if(_u4CodecVer[u4InstID] == VDEC_VP9)
  {
      vVerInitVP9(u4InstID);
  }
//#endif
  else if(_u4CodecVer[u4InstID] == VDEC_AVS)
  {
      vVerInitAVS(u4InstID);
  }
  else
  {
    VDEC_INFO_MPEG_VFIFO_PRM_T rMPEGVDecInitPrm;
    VDEC_INFO_MPEG_BS_INIT_PRM_T rMPEGBSInitPrm;

    vInitVParserMPEG(u4InstID);
    rMPEGVDecInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rMPEGVDecInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rMPEGBSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rMPEGBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rMPEGBSInitPrm.u4ReadPointer= (UINT32)_pucVFifo[u4InstID];
    rMPEGVDecInitPrm.u4CodeType = _u4CodecVer[u4InstID];
  #ifndef  RING_VFIFO_SUPPORT
    #ifdef MPEG2_THRESHOLD_TEST
     rMPEGBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + 0x10000 + 16;
    #else
    rMPEGBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    #endif
  #else
//    rMPEGBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
	rMPEGBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));//mtk40343
  #endif
    i4VDEC_HAL_MPEG_InitVDecHW(u4InstID, &rMPEGVDecInitPrm);
    #ifdef MPEG2_THRESHOLD_TEST
     vVDecWriteVLD(u4InstID, RW_VLD_BS_THLD_DE, (0x1000|u4VDecReadVLD(u4InstID, RW_VLD_BS_THLD_DE)));
    #endif
    if(rMPEGVDecInitPrm.u4CodeType == VDEC_MPEG2)
    {
        i4VDEC_HAL_MPEG_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rMPEGBSInitPrm);
    }
    else
    {
        i4VDEC_HAL_MPEG4_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rMPEGBSInitPrm);
    }
    // Restore Quantization Matrix
    if(_fgVerLoadIntraMatrix[u4InstID])
    {
      vVDEC_HAL_MPEG_ReLoadQuantMatrix(u4InstID, TRUE);
    }
    if(_fgVerLoadNonIntraMatrix[u4InstID])
    {
      vVDEC_HAL_MPEG_ReLoadQuantMatrix(u4InstID, FALSE);
    }
    if(_u4CodecVer[u4InstID] == VDEC_MPEG2)
    {
      vDEC_HAL_COMMON_SetVLDPower(u4InstID,ON);
      vVDec_HAL_CRC_Enable(u4InstID,1);//mc crc
      u4VParserMPEG12(u4InstID, TRUE);
      vDEC_HAL_COMMON_SetVLDPower(u4InstID,OFF);
    }
    else if((_u4CodecVer[u4InstID] == VDEC_MPEG4) || (_u4CodecVer[u4InstID] == VDEC_H263))
    {
      if(!_fgShortHeader[u4InstID])
      {
        vDEC_HAL_COMMON_SetVLDPower(u4InstID,ON);
        vVDec_HAL_CRC_Enable(u4InstID,TRUE);
        u4VParserMPEG4(u4InstID, TRUE);
        vDEC_HAL_COMMON_SetVLDPower(u4InstID,OFF);
      }
    }
    else if(_u4CodecVer[u4InstID] == VDEC_DIVX3)
    {
      UINT32 temp;
      _u4Divx3SetPos[u4InstID] += 8;
      // skip Compression and "SizeImage"
      temp = u4VDEC_HAL_MPEG_GetBitStreamShift(_u4BSID[u4InstID], u4InstID, 32);
      _u4DIVX3Width[u4InstID] = (((temp&0x00ff0000)>>16)<<8) + ((temp & 0xff000000)>>24);
      temp = u4VDEC_HAL_MPEG_GetBitStreamShift(_u4BSID[u4InstID], u4InstID, 32);
      _u4DIVX3Height[u4InstID] = (((temp&0x00ff0000)>>16)<<8) + ((temp & 0xff000000)>>24);
      vVerifySetUpParm(u4InstID, _u4DIVX3Width[u4InstID], _u4DIVX3Height[u4InstID], 4 /*FRC_29_97*/, FALSE, FALSE);
    }
    
    _tVerPic[u4InstID].u4W = _u4HSize[u4InstID];
    _tVerPic[u4InstID].u4H = _u4VSize[u4InstID];
    _tVerPic[u4InstID].ucMpegVer = _ucMpegVer[u4InstID];
        UTIL_Printf("[MPEG] Inst%d, SIZE:[H:%d, V:%d], VerPic Size[H:%d, V:%d]\n", 
        u4InstID, _u4HSize[u4InstID], _u4VSize[u4InstID], _tVerPic[u4InstID].u4W, _tVerPic[u4InstID].u4H); 
  }
  if(_u4K2KMode[u4InstID])
  {
    _tVerDec[u4InstID].ucState = DEC_NORM_LAE_DEC;
    if (_u4CodecVer[u4InstID] == VDEC_H265)
    {
        u4HwID = (u4InstID == 0) ? VDEC_LAE0 : VDEC_LAE1;
        _tVerMpvDecPrm[u4HwID].SpecDecPrm.rVDecH265DecPrm.ucVDecStatus = VDEC_LAE_ST;
        printk("set %d state VDEC_LAE_ST\n",u4HwID);
    }
  }
  else
  {
    _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;
  }


  
  vVerifyVDecIsrInit(u4InstID);
}

// for AVI or QT, we know frame rate from system layer
// for M4V, maybe set it to 4 (29.976Hz)
void vVerifySetUpParm(UINT32 u4InstID, UINT32 dwPicW, UINT32 dwPicH, UINT32 dwFrmRatCod, BOOL fgDivXM4v, BOOL fgDx4M4v)
{
  _u4RealHSize[u4InstID] = dwPicW;
  _u4RealVSize[u4InstID] = dwPicH;
  _u4UPicW[u4InstID] = dwPicW;
  _u4UPicH[u4InstID] = dwPicH;
  _u4UFrmRatCod[u4InstID] = dwFrmRatCod;
  _fgVerUDivXM4v[u4InstID] = fgDivXM4v;
  _fgVerUDx4M4v[u4InstID] = fgDx4M4v;
}

BOOL fgLaeIsrInit = FALSE;

void vVerifyVDecIsrInit(UINT32 u4InstID)
{
  //BIM_DisableIrq(0xffffffff); //ginny mark it 071015
#ifndef IRQ_DISABLE
  if (u4InstID == 0)
  {
    x_os_isr_fct pfnOldIsr;

#ifdef  MPEG2_THRESHOLD_TEST
    // reg threshold ISR
    if (x_reg_isr(VECTOR_VDLIT, vVDec1IrqProc, &pfnOldIsr) != OSR_OK)
    {
	ASSERT(0);
    }
#endif
    // reg ISR
    if (x_reg_isr(VECTOR_VDFUL, vVDec0IrqProc, &pfnOldIsr) != OSR_OK)
    {
	ASSERT(0);
    }
  }
  else if (u4InstID == 1)
  {
    x_os_isr_fct pfnOldIsr;

    // reg ISR
    if (x_reg_isr(VECTOR_VDLIT, vVDec1IrqProc, &pfnOldIsr) != OSR_OK)
    {
	ASSERT(0);
    }
  }
    #if VDEC_SUPPORT_IOMMU
    {
      x_os_isr_fct pfnOldIsr;
      if (x_reg_isr(VECTOR_MMU_VDEC, vVDecIOMMUIrqProc, &pfnOldIsr) != OSR_OK)
      {
      ASSERT(0);
      }
    }
    #endif

    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
    if(fgLaeIsrInit == FALSE)
    {
        UTIL_Printf("LAE irq register\n");
        x_os_isr_fct pfnLaeIsr;
        if (x_reg_isr(VECTOR_LAE0, vVDecLae0IrqProc, &pfnLaeIsr) != OSR_OK)
        {
            ASSERT(0);
        }
        fgLaeIsrInit = TRUE;
    }
    #endif

#endif
}

void vVerifyVDecIsrStop(UINT32 u4InstID)
{
#ifndef IRQ_DISABLE
  x_os_isr_fct pfnOldIsr;

  if(u4InstID == VDEC_VLD0)
  {
    // dereg ISR
    if (x_reg_isr(VECTOR_VDFUL, NULL, &pfnOldIsr) != OSR_OK)
    {
      ASSERT(0);
    }
  }
  else if(u4InstID == VDEC_VLD1)
  {
    // dereg ISR
    if (x_reg_isr(VECTOR_VDLIT, NULL, &pfnOldIsr) != OSR_OK)
    {
      ASSERT(0);
    }
  }
#if VDEC_SUPPORT_IOMMU
      {
          if (x_reg_isr(VECTOR_MMU_VDEC, NULL, &pfnOldIsr) != OSR_OK)
          {
            ASSERT(0);
          }
      }
#endif
#endif
}

void vVerifyInitVParserWMV(UINT32 u4InstID)
{
  VDEC_INFO_WMV_SEQ_PRM_T *prWMVSPS = &_rWMVSPS[u4InstID];
  VDEC_INFO_WMV_ETRY_PRM_T *prWMVEPS = &_rWMVEPS[u4InstID];
  VDEC_INFO_WMV_PIC_PRM_T *prWMVPPS = &_rWMVPPS[u4InstID];
  VDEC_INFO_WMV_ICOMP_PRM_T *prWMVICOMPPS = &_rWMVICOMPPS[u4InstID];

  _u4WMVBitCount[u4InstID] = 0;
  _u4WMVByteCount[u4InstID] = 0;
  reset_pic_hdr_bits(u4InstID);

  _u4DispBufIdx[u4InstID] = 1;
  if(_u4DispBufIdx[u4InstID] == 1)
  {
    vSetVerFRefBuf(u4InstID, 0);
    vSetVerBRefBuf(u4InstID, 1);
  }
  else
  {
    vSetVerFRefBuf(u4InstID, 1);
    vSetVerBRefBuf(u4InstID, 0);
  }
  _u4WMVDecPicNo[u4InstID] = 0;

  //Sequence Header variables initialization
  //Advanced
  prWMVSPS->fgBroadcastFlags = FALSE;
  prWMVSPS->fgInterlacedSource = FALSE;
  prWMVSPS->fgTemporalFrmCntr = FALSE;

  prWMVSPS->fgSeqFrameInterpolation = FALSE;

  prWMVSPS->fgHRDPrmFlag = FALSE;
  prWMVSPS->i4HRDNumLeakyBuckets = 0;
  prWMVPPS->fgTopFieldFirst = TRUE; //ming
  //Simple & Main
  prWMVSPS->fgXintra8Switch = FALSE;
  prWMVSPS->fgMultiresEnabled = FALSE;
  prWMVSPS->i4ResIndex = 0;
  prWMVSPS->fgDCTTableMBEnabled = FALSE;
  prWMVSPS->fgPreProcRange = FALSE;
  prWMVSPS->i4NumBFrames = 1;
  prWMVSPS->fgRotatedIdct = FALSE;
  prWMVPPS->ucFrameCodingMode = PROGRESSIVE; //ming
   // WMV7 & WMV8
  prWMVSPS->fgMixedPel = FALSE;
  prWMVSPS->fgFrmHybridMVOn = FALSE;
  prWMVSPS->fgXintra8 = FALSE;

  //End of Sequence

  prWMVPPS->fgPostRC1 = TRUE;

  //EntryPoint Header variables initialization
  prWMVEPS->fgBrokenLink = FALSE;
  prWMVEPS->fgClosedEntryPoint = FALSE;
  prWMVEPS->fgPanScanPresent = FALSE;
  prWMVEPS->fgRefDistPresent = FALSE;
  prWMVEPS->fgLoopFilter = FALSE;
  prWMVEPS->fgUVHpelBilinear = FALSE;
  prWMVEPS->i4RangeState = 0;
  prWMVEPS->i4ReconRangeState = 0;
  prWMVEPS->fgExtendedMvMode = FALSE;
  prWMVEPS->i4MVRangeIndex = 0;
  prWMVEPS->i4DQuantCodingOn = 0;
  prWMVEPS->fgXformSwitch = FALSE;
  prWMVEPS->fgSequenceOverlap = FALSE;
  //Quant related
  prWMVEPS->fgExplicitSeqQuantizer = FALSE;
  prWMVEPS->fgExplicitFrameQuantizer = FALSE;
  prWMVEPS->fgExplicitQuantizer = FALSE;
  prWMVPPS->fgUse3QPDZQuantizer = FALSE;
  prWMVPPS->fgHalfStep = FALSE;

  prWMVEPS->fgExtendedDeltaMvMode = FALSE;
  prWMVEPS->i4DeltaMVRangeIndex = 0;
  prWMVEPS->i4ExtendedDMVX = 0;
  prWMVEPS->i4ExtendedDMVY = 0;
  prWMVEPS->i4RefFrameDistance = 0;

  prWMVPPS->i4BNumerator = 0;
  prWMVSPS->i4NumBFrames = 1;
  prWMVPPS->i4DCStepSize = 0;
  prWMVPPS->i4X9MVMode = 0;

  prWMVPPS->fgMBXformSwitching = FALSE;
  prWMVPPS->i4FrameXformMode = 0;


  prWMVEPS->fgRangeRedYFlag = FALSE;
  prWMVEPS->fgRangeRedUVFlag = FALSE;
//End of EntryPoint


  prWMVPPS->u4ForwardRefPicType = 0;
  prWMVPPS->u4BackwardRefPicType = 0;

  // Picture Header
  //WMV7 & WMV8
  prWMVPPS->fgDCPredIMBInPFrame = FALSE;
  //for field pictures

  prWMVPPS->fgTopFieldFirst = TRUE;
  prWMVPPS->fgRepeatFirstField = FALSE;
  //FALSE for PROGRESSIVE.
  prWMVPPS->fgInterlaceV2 = FALSE;
  prWMVPPS->fgFieldMode = FALSE;
  prWMVPPS->i4CurrentField = 0; // 0:TOP, 1:BOTTOM field
  prWMVPPS->i4CurrentTemporalField = 0; // 0:1st field or frame picture, 1: 2nd field

  prWMVPPS->i4MaxZone1ScaledFarMVX = 0;
  prWMVPPS->i4MaxZone1ScaledFarMVY = 0;
  prWMVPPS->i4Zone1OffsetScaledFarMVX = 0;
  prWMVPPS->i4Zone1OffsetScaledFarMVY = 0;
  prWMVPPS->i4FarFieldScale1 = 0;
  prWMVPPS->i4FarFieldScale2 = 0;
  prWMVPPS->i4NearFieldScale = 0;
  prWMVPPS->i4MaxZone1ScaledFarBackMVX = 0;
  prWMVPPS->i4MaxZone1ScaledFarBackMVY = 0;
  prWMVPPS->i4Zone1OffsetScaledFarBackMVX = 0;
  prWMVPPS->i4Zone1OffsetScaledFarBackMVY = 0;
  prWMVPPS->i4FarFieldScaleBack1 = 0;
  prWMVPPS->i4FarFieldScaleBack2 = 0;
  prWMVPPS->i4NearFieldScaleBack = 0;

  prWMVPPS->fgTwoRefPictures = TRUE;
  prWMVPPS->fgUseOppFieldForRef = TRUE;
  prWMVPPS->fgUseSameFieldForRef = TRUE;
  //Robert TODO: 0511
  prWMVPPS->fgBackRefUsedHalfPel = FALSE;
  prWMVPPS->fgBackRefTopFieldHalfPelMode = FALSE;
  prWMVPPS->fgBackRefBottomFieldHalfPelMode = FALSE;

  prWMVPPS->fgMvResolution = FALSE;

  prWMVPPS->i4Overlap = 0;
  prWMVPPS->i4MvTable = 0;
  prWMVPPS->i4CBPTable = 0;
  prWMVPPS->i4MBModeTable = 0;
  prWMVPPS->i42MVBPTable = 0;
  prWMVPPS->i44MVBPTable = 0;

  //!WMVA profile
  prWMVSPS->fgPreProcRange = FALSE;


  prWMVEPS->fgNewDCQuant = FALSE;

  prWMVPPS->fgDCTTableMB = FALSE;

 // WMV7 & WMV8
 //Robert TODO:
  if(_i4CodecVersion[u4InstID] == VDEC_WMV2)
  {
    prWMVSPS->fgSkipBitCoding = TRUE;
    prWMVSPS->fgNewPcbPcyTable = TRUE;
  }
  else
  {
    prWMVSPS->fgSkipBitCoding = FALSE;
    prWMVSPS->fgNewPcbPcyTable = FALSE;
  }

  prWMVSPS->fgCODFlagOn = TRUE;


  if(_i4CodecVersion[u4InstID] >= VDEC_WMV3)
  {
    prWMVEPS->fgNewDCQuant = TRUE;
  }

  ComputeDQuantDecParam(u4InstID);

  prWMVPPS->ucDiffQtProfile = 0;

  _iSeqHdrData1[u4InstID] = 0;
  _iSeqHdrData2[u4InstID] = 0;
  prWMVSPS->i4SkipBitModeV87 = 0;
  prWMVSPS->i4Wmv8BpMode = 0;

  //NEEDS to initialize
  _new_entry_point[u4InstID] = 0;
  prWMVSPS->fgPostProcInfoPresent = FALSE;
  prWMVSPS->fgYUV411 = FALSE;
  prWMVSPS->fgSpriteMode = FALSE;
  prWMVEPS->i4RangeRedY = 0;
  prWMVEPS->i4RangeMapUV = 0;
  prWMVPPS->ucRepeatFrameCount = 0;
  prWMVPPS->ucDQuantBiLevelStepSize = 0;
  prWMVPPS->fgDQuantOn = FALSE;
  prWMVPPS->i4Panning = 0;
  prWMVPPS->fgDQuantBiLevel = FALSE;

  prWMVICOMPPS->i4ResetMvDram = 0;
  prWMVICOMPPS->i4SecondFieldParity = 0;
  prWMVICOMPPS->i4BoundaryUMVIcomp = 0;

  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecWMVDecPrm.prSPS = &_rWMVSPS[u4InstID];
  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecWMVDecPrm.prEPS = &_rWMVEPS[u4InstID];
  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecWMVDecPrm.prPPS = &_rWMVPPS[u4InstID];
  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecWMVDecPrm.prICOMPS = &_rWMVICOMPPS[u4InstID];
  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecWMVDecPrm.fgWmvMode = _u4WmvMode[u4InstID];
}

void vInitVParserMPEG(UINT32 u4InstID)
{
  _u4Divx3SetPos[u4InstID] = 0;
  _u4BrokenLink[u4InstID] = 2;
  _u4DispBufIdx[u4InstID] = 1;
  if(_u4DispBufIdx[u4InstID] == 1)
  {
    vSetVerFRefBuf(u4InstID, 0);
    vSetVerBRefBuf(u4InstID, 1);
  }
  else
  {
    vSetVerFRefBuf(u4InstID, 1);
    vSetVerBRefBuf(u4InstID, 0);
  }
  _u4Datain[u4InstID] = 0;          // for dwGetBitStream() return value
  _u4BitCount[u4InstID] = 0;        // for dwGetBitStream() byte aligned

  _fgVerSeqHdr[u4InstID]=0;            // sequence header process
  _ucMpegVer[u4InstID] = VDEC_UNKNOWN;

  _u4HSize[u4InstID]=0;             // horizontal size = horizontal size value +
                          // horizontal size extension << 12
  _u4VSize[u4InstID]=0;             // vertical size = vertical size value +
                          //                 vertical size extension << 12
  _ucParW[u4InstID] = 1;
  _ucParH[u4InstID] = 1;
  _u4HSizeVal[u4InstID]=0;          // horizontal size value
  _u4VSizeVal[u4InstID]=0;          // vertical size value

  _fgVerProgressiveSeq[u4InstID]=0;    //progressive_sequence
  _u4PicWidthMB[u4InstID]=0;
  _u4BPicIniFlag[u4InstID]=0;
  _u4BPicIniFlag0[u4InstID]=0;

  //_ucHSizeExt=0;           // horizontal size extension
  //_ucVSizeExt=0;           // vertical size extension
  _ucCrmaFmt[u4InstID]=0;            //chroma_format;
  _ucFullPelFordVec[u4InstID]=0;     // full_pel_forward_vector;
  _ucFordFCode[u4InstID]=0;          // forward_f_code;
  _ucFullPelBackVec[u4InstID]=0;     // full_pel_backward_vector;
  _ucBackFCode[u4InstID]=0;          // backward_f_code;
  _ucIntraDcPre[u4InstID]=0;         // intra_dc_precision;
  _fgVerAltScan[u4InstID]=0;
  _fgVerQScaleType[u4InstID]=0;        // q_scale_type;
  _fgVerFrmPredFrmDct[u4InstID]=0;     // frame_pred_frame_dct;
  _fgVerIntraVlcFmt[u4InstID]=0;
  _fgVerConcealMotionVec[u4InstID]=0;  // concealment_motion_vectors;
  _pucfcode[u4InstID][0][0]=0;
  _pucfcode[u4InstID][0][1]=0;
  _pucfcode[u4InstID][1][0]=0;
  _pucfcode[u4InstID][1][1]=0;
  _u4PicPSXOff[u4InstID] = 0xFFFFFFFF;
  //_dwOldPicPSXSkip = 0xFFFFFFFF;

  _fgVerLoadIntraMatrix[u4InstID] = 0;
  _fgVerLoadNonIntraMatrix[u4InstID] = 0;

  //_bLastPicBBufMd=0;      // 1 for 8-line mode, 0 for 16-line mode

  _fgVerBrokenLink[u4InstID] = FALSE;
  _fgVerClosedGop[u4InstID] = FALSE;

  _u4UserDataCodecVersion[u4InstID] = 0;
  _u4UserDataBuildNumber[u4InstID] = 0;
  _u4TimeBase[u4InstID] = 0;
  _fgVerShortVideoHeader[u4InstID] = FALSE;
  _fgSorenson[u4InstID] = FALSE;
  _ucSourceFormat[u4InstID] = 0;
  _ucVisualObjectVerid[u4InstID] = 1;
  _fgVerQuarterSample[u4InstID] = FALSE;
  _fgVerReversibleVlc[u4InstID] = FALSE;
  _fgVerReducedResolutionVopEnable[u4InstID] = FALSE;
  _rDirMode[u4InstID].u4TFrm = 0xffffffff;
  _fgVerTopFldFirst[u4InstID] = FALSE;

  _rMPEG4VopPrm[u4InstID].prDirMd = &_rDirMode[u4InstID];
  _rMPEG4VopPrm[u4InstID].prGmcPrm = &_rMPEG4GmcPrm[u4InstID];
  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rDep.rM4vDecPrm.prVol = &_rMPEG4VolPrm[u4InstID];
  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rDep.rM4vDecPrm.prVop = &_rMPEG4VopPrm[u4InstID];
}

void vSetWMVDecParam( UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm)
{

  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.i4CodecVersion = _i4CodecVersion[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.i4MemBase = 0;

if (!tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.fgWmvMode)
{
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Bp1Sa = (UINT32)_pucBp_1[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Bp2Sa = (UINT32)_pucBp_2[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Bp3Sa = (UINT32)_pucBp_3[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Bp4Sa = (UINT32)_pucBp_4[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Dcac2Sa = (UINT32)_pucDcac_2[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4DcacSa = (UINT32)_pucDcac[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Mv12Sa = (UINT32)_pucMv_1_2[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Mv1Sa = (UINT32)_pucMv_1[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Mv2Sa = (UINT32)_pucMv_2[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Mv3Sa = (UINT32)_pucMv_3[u4InstID];
}
else
{
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4DcacNewSa = (UINT32)_pucDcacNew[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4MvNewSa = (UINT32)_pucMvNew[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Bp0NewSa = (UINT32)_pucBp0New[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Bp1NewSa = (UINT32)_pucBp1New[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Bp2NewSa = (UINT32)_pucBp2New[u4InstID];
}
//#endif

  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Pic0CSa = (UINT32)_pucPic0C[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Pic0YSa = (UINT32)_pucPic0Y[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Pic1CSa = (UINT32)_pucPic1C[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Pic1YSa = (UINT32)_pucPic1Y[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Pic2CSa = (UINT32)_pucPic2C[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Pic2YSa = (UINT32)_pucPic2Y[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Pp1Sa = (UINT32)_pucPp_1[u4InstID];
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.rWmvWorkBufSa.u4Pp2Sa = (UINT32)_pucPp_2[u4InstID];
  tVerMpvDecPrm->ucDecFBufIdx = BYTE0(_u4DecBufIdx[u4InstID]);
  tVerMpvDecPrm->SpecDecPrm.rVDecWMVDecPrm.u4FRefBufIdx = _u4FRefBufIdx[u4InstID];
}

BOOL fgVDecProcWMV(UINT32 u4InstID)
{
  UINT32 u4Bytes,u4Bits;
  VDEC_INFO_WMV_BS_INIT_PRM_T rWmvBSInitPrm;
  VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm;

  tVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];
  vSetWMVDecParam(u4InstID, tVerMpvDecPrm);
#if 0
  vVerifySetVSyncPrmBufPtr(u4InstID, _u4DecBufIdx[u4InstID]);
  _tBufPVAddrInfo[u4InstID].u4PhypucDecWorkBuf= BSP_dma_map_single((UINT32) _pucPic0Y[0], PIC_Y_SZ, FROM_DEVICE);
  _tBufPVAddrInfo[u4InstID].u4PhypucDecWorkCBuf= BSP_dma_map_single((UINT32) _pucPic0Y[0], PIC_Y_SZ, FROM_DEVICE);
#else
  _tBufPVAddrInfo[u4InstID].u4PhyPucPic0Y= BSP_dma_map_single((UINT32) _pucPic0Y[0], PIC_Y_SZ, FROM_DEVICE);
  _tBufPVAddrInfo[u4InstID].u4PhyPucPic0C= BSP_dma_map_single((UINT32) _pucPic0C[0], PIC_C_SZ, FROM_DEVICE);
  _tBufPVAddrInfo[u4InstID].u4PhyPucPic1Y= BSP_dma_map_single((UINT32) _pucPic1Y[0], PIC_Y_SZ, FROM_DEVICE);
  _tBufPVAddrInfo[u4InstID].u4PhyPucPic1C= BSP_dma_map_single((UINT32) _pucPic1C[0], PIC_C_SZ, FROM_DEVICE);
  _tBufPVAddrInfo[u4InstID].u4PhyPucPic2Y= BSP_dma_map_single((UINT32) _pucPic2Y[0], PIC_Y_SZ, FROM_DEVICE);
  _tBufPVAddrInfo[u4InstID].u4PhyPucPic2C= BSP_dma_map_single((UINT32) _pucPic2C[0], PIC_C_SZ, FROM_DEVICE);
#endif
  //Log Input Window before Trigger VDec
{
  UINT32 u4InputWindow = 0;

  u4InputWindow = u4VDecReadVLD(u4InstID, 0xf0);
  //UTIL_Printf(" Input Window %x \n", u4InputWindow);
}

#ifdef VERIFICATION_DOWN_SCALE
  #ifdef DOWN_SCALE_SUPPORT
    //vSetDownScaleParam(u4InstID, TRUE, &_tDownScalerPrm[u4InstID]);
    vSetDownScaleParam(u4InstID, TRUE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
  #else
    //vSetDownScaleParam(u4InstID, FALSE, &_tDownScalerPrm[u4InstID]);
    vSetDownScaleParam(u4InstID, FALSE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
  #endif
  //vDEC_HAL_COMMON_SetDownScaler(u4InstID, &_tDownScalerPrm[u4InstID]);
  //vVDECSetDownScalerPrm(u4InstID, &_tDownScalerPrm[u4InstID]);
  vVDECSetDownScalerPrm(u4InstID, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
#endif
  if(_u4BSID[u4InstID] == 1)
  {
    u4Bytes = u4VDEC_HAL_WMV_ReadRdPtr(1, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
    rWmvBSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rWmvBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rWmvBSInitPrm.u4ReadPointer= (UINT32)_pucVFifo[u4InstID] + u4Bytes;
  #ifndef  RING_VFIFO_SUPPORT
    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  #else
//    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
  #endif
    if (_i4CodecVersion[u4InstID] == VDEC_VC1)
    {
       i4VDEC_HAL_WMV_InitBarrelShifter(0, u4InstID, &rWmvBSInitPrm, TRUE);
    }
    else
    {
       i4VDEC_HAL_WMV_InitBarrelShifter(0, u4InstID, &rWmvBSInitPrm, FALSE);
    }
    u4VDEC_HAL_WMV_ShiftGetBitStream(0, u4InstID, u4Bits);
  }
vVDecWriteCRC(0, VDEC_CRC_REG_EN, 1);
  if(_i4CodecVersion[u4InstID] == VDEC_VC1)
  {
    i4VDEC_HAL_WMV_DecStart(u4InstID, tVerMpvDecPrm); //umv_from_mb = 0 for WMVA
    if((_rWMVPPS[u4InstID].ucFrameCodingMode != INTERLACEFIELD) || (_rWMVPPS[u4InstID].i4CurrentTemporalField == 1))
    {
      _u4WMVDecPicNo[u4InstID]++;
	  UTIL_Printf("WMVA\n");
    }
  }
  else
  {
    i4VDEC_HAL_WMV_DecStart(u4InstID, tVerMpvDecPrm); //umv_from_mb = 1 for !WMVA
    _u4WMVDecPicNo[u4InstID]++;
	UTIL_Printf("!WMVA\n");
  }

  return (TRUE);
}

void vCodecVersion(UINT32 u4InstID, UINT32 u4CodecFOURCC)
{
  VDEC_INFO_WMV_SEQ_PRM_T *prWMVSPS = &_rWMVSPS[u4InstID];

  prWMVSPS->fgVC1 = FALSE;
  if(u4CodecFOURCC == FOURCC_WVC1_WMV)
  {
    u4CodecFOURCC = FOURCC_WMVA_WMV;
    prWMVSPS->fgVC1 = TRUE;
  }

  if((u4CodecFOURCC == FOURCC_WMV1_WMV) || (u4CodecFOURCC == FOURCC_wmv1_WMV))
    _i4CodecVersion[u4InstID] = VDEC_WMV1;
  else if((u4CodecFOURCC == FOURCC_WMV2_WMV) || (u4CodecFOURCC == FOURCC_wmv2_WMV))
    _i4CodecVersion[u4InstID] = VDEC_WMV2;
  else if((u4CodecFOURCC == FOURCC_WMV3_WMV) || (u4CodecFOURCC == FOURCC_wmv3_WMV))
    _i4CodecVersion[u4InstID] = VDEC_WMV3;
  else if((u4CodecFOURCC == FOURCC_WMVA_WMV) || (u4CodecFOURCC == FOURCC_wmva_WMV))
    _i4CodecVersion[u4InstID] = VDEC_VC1;
  else
  {
    vVDecOutputDebugString("WMV Codec Error\n");
  }
}


void vSetFGTParam(UINT32 u4InstID, VDEC_INFO_H264_FGT_PRM_T *prFGTPrm)
{
  //VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm;
  //ptVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];

#ifdef FGT_SUPPORT
    VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm;
    ptVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];

    prFGTPrm->ucDataScr = FGT_EN | FGT_SCR_PP; // If Cancel flag is TRUE, it will be bypass mode
#ifdef DOWN_SCALE_SUPPORT
    prFGTPrm->ucDataScr |= FGT_VDSCL_BUSY_EN;
#endif
    prFGTPrm->pucFGTScrYAddr = _pucDecWorkBuf[u4InstID];
    prFGTPrm->pucFGTScrCAddr = _pucDecWorkBuf[u4InstID] + _ptCurrFBufInfo[u4InstID]->u4DramPicSize;
    prFGTPrm->pucFGTTrgYAddr = _pucFGTBuf[u4InstID];
    prFGTPrm->pucFGTTrgCAddr = _pucFGTBuf[u4InstID] + _ptCurrFBufInfo[u4InstID]->u4DramPicSize;
    prFGTPrm->ucMBXSize =  ((_ptCurrFBufInfo[u4InstID]->u4W + 15)>> 4);
    prFGTPrm->ucMBYSize =  (((_ptCurrFBufInfo[u4InstID]->u4H >> (1-(fgIsFrmPic(u4InstID)))) + 15)>> 4);
    prFGTPrm->u4Ctrl =  ((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumModelValuesMinus1[0] & 0x3) << 0) |
                                                             ((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumModelValuesMinus1[1] & 0x3) << 2) |
                                                             ((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumModelValuesMinus1[2] & 0x3) << 4) |
                                                             ((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4NumModelValuesMinus1[2] & 0x3) << 4) |
                                                             (ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgFilmGrainCharacteristicsCancelFlag << 8) |
                                                             (ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgCompModelPresentFlag[0] << 9) |
                                                             (ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgCompModelPresentFlag[1] << 10) |
                                                             (ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->fgCompModelPresentFlag[2] << 11) |
                                                             ((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSEI->u4Log2ScaleFactor&0xfff) << 12) |
                                                             ((_ptCurrFBufInfo[u4InstID]->i4POC & 0xff) << 16) |
                                                             (((ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4IdrPicId) & 0x7) << 24);
#else
    prFGTPrm->ucDataScr = 0;
#endif
}


void vSaveDownScaleParam(UINT32 u4InstID, VDEC_INFO_VDSCL_PRM_T *prDownScalerPrm)
{
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.ucPicStruct = prDownScalerPrm->ucPicStruct;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.ucScanType = prDownScalerPrm->ucScanType;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.ucScrAgent = prDownScalerPrm->ucScrAgent;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.ucSpectType = prDownScalerPrm->ucSpectType;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.ucVdoFmt = prDownScalerPrm->ucVdoFmt;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4DispW = prDownScalerPrm->u4DispW;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4SrcHeight = prDownScalerPrm->u4SrcHeight;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4SrcWidth = prDownScalerPrm->u4SrcWidth;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4TrgHeight = prDownScalerPrm->u4TrgHeight;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4TrgWidth = prDownScalerPrm->u4TrgWidth;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4TrgOffH = prDownScalerPrm->u4TrgOffH;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4TrgOffV = prDownScalerPrm->u4TrgOffV;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4TrgYAddr = prDownScalerPrm->u4TrgYAddr;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4TrgCAddr = prDownScalerPrm->u4TrgCAddr;
  _tVerMpvDecPrm[u4InstID].rDownScalerPrm.u4WorkAddr = prDownScalerPrm->u4WorkAddr;
}

void vSetDownScaleParam(UINT32 u4InstID, BOOL fgEnable, VDEC_INFO_VDSCL_PRM_T *prDownScalerPrm)
{
#ifdef VERIFICATION_DOWN_SCALE
  UINT32 dwPicWidthDec,dwPicHeightDec,u4DramPicSize;
  VDEC_INFO_DEC_PRM_T *ptVerMpvDecPrm;
  ptVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];

  dwPicWidthDec = _tVerMpvDecPrm[u4InstID].u4PicW;
  dwPicHeightDec = _tVerMpvDecPrm[u4InstID].u4PicH;
  if(fgEnable)
  {
    prDownScalerPrm->fgMbaff  = FALSE;
    prDownScalerPrm->fgDSCLEn = TRUE;
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    prDownScalerPrm->ucAddrSwapMode = _tVerMpvDecPrm[u4InstID].ucAddrSwapMode ^ 0x4;
#else
    prDownScalerPrm->ucAddrSwapMode = ADDRSWAP_OFF;
#endif

    prDownScalerPrm->fgLumaKeyEn = _fgVDSCLEnableLumaKeyTest[u4InstID];
    prDownScalerPrm->u2LumaKeyValue= (UINT16) (((UINT32) rand())%256);

    if(_u4CodecVer[u4InstID] == VDEC_WMV)
    {
      if(_rWMVPPS[u4InstID].ucFrameCodingMode == INTERLACEFIELD)
      {
        if(_rWMVPPS[u4InstID].i4CurrentField == 1)
        {
          prDownScalerPrm->ucPicStruct = BOTTOM_FIELD;
        }
        else
        {
          prDownScalerPrm->ucPicStruct = TOP_FIELD;
        }
      }
      else if(_rWMVPPS[u4InstID].ucFrameCodingMode == INTERLACEFRAME)
      {
        prDownScalerPrm->ucPicStruct = TOP_BOTTOM_FIELD;
      }
      else
      {
        prDownScalerPrm->ucPicStruct = FRAME;
      }
      if ((_rWMVEPS[u4InstID].fgLoopFilter == 1) || (_rWMVPPS[u4InstID].i4Overlap & 1))
      {

         if (prDownScalerPrm->fgLumaKeyEn)
            prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_MC >> 2; //WMV+Luma_Key Only support MC out
         else
            prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_PP >> 2;

        prDownScalerPrm->fgEnColorCvt = FALSE;
      }
      else
      {
        prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_MC >> 2;
        prDownScalerPrm->fgEnColorCvt = (BOOL) (((UINT32) rand())&0x1);//random(2);
      }
    }
    else if(_u4CodecVer[u4InstID] == VDEC_H264)
    {
      prDownScalerPrm->ucPicStruct = ptVerMpvDecPrm->ucPicStruct;
      prDownScalerPrm->fgMbaff = ptVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSPS->fgMbAdaptiveFrameFieldFlag;
    }
    else
    {
      if(_fgVerProgressiveFrm[u4InstID] || _fgVerProgressiveSeq[u4InstID])
      {
        prDownScalerPrm->ucPicStruct = 3;
      }
      else
      {
        prDownScalerPrm->ucPicStruct = (ptVerMpvDecPrm->ucPicStruct > 3) ?
          (ptVerMpvDecPrm->ucPicStruct - 3) : ptVerMpvDecPrm->ucPicStruct;
      }
    }

    if((prDownScalerPrm->ucPicStruct == TOP_FIELD) || (prDownScalerPrm->ucPicStruct == BOTTOM_FIELD))
    {
      dwPicHeightDec = (dwPicHeightDec >> 1);
    }
    if(((_u4CodecVer[u4InstID] == VDEC_WMV)&&(_rWMVPPS[u4InstID].i4CurrentTemporalField==0))||
      ((_u4CodecVer[u4InstID] == VDEC_H264)&&(fgIsDecFlagSet(u4InstID, DEC_FLAG_CHG_FBUF)))||
      ((!_fgDec2ndFldPic[u4InstID])&&(_u4CodecVer[u4InstID] != VDEC_WMV)&&(_u4CodecVer[u4InstID] != VDEC_H264)))
    {
      if(_u4CodecVer[u4InstID] == VDEC_WMV)
      {
        prDownScalerPrm->ucSpectType = RW_VDSCL_SPEC_WMV >> 5;
        prDownScalerPrm->fgYOnly = 0;
        if ((_rWMVEPS[u4InstID].fgLoopFilter == 1) || (_rWMVPPS[u4InstID].i4Overlap & 1))
        {
             if (prDownScalerPrm->fgLumaKeyEn)
                prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_MC >> 2; //WMV+Luma_Key Only support MC out
             else
                prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_PP >> 2;

          prDownScalerPrm->fgEnColorCvt = FALSE;
        }
        else
        {
          prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_MC >> 2;
          prDownScalerPrm->fgEnColorCvt = (BOOL) (((UINT32) rand())&0x1);//random(2);
        }
      }
      else if(_u4CodecVer[u4InstID] == VDEC_H264)
      {
        prDownScalerPrm->ucSpectType = RW_VDSCL_SPEC_264 >> 5;
        prDownScalerPrm->fgYOnly = (fgIsMonoPic(u4InstID)? (RW_VDSCL_Y_ONLY>>7): 0);
      #ifdef FGT_SUPPORT
        prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_FG >> 2;
      #else
        prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_PP >> 2;
      #endif
        prDownScalerPrm->fgEnColorCvt = FALSE;
      }
      else
      {
        prDownScalerPrm->ucSpectType = RW_VDSCL_SPEC_MPEG >> 5;
        prDownScalerPrm->fgYOnly = 0;
          if((_u4CodecVer[u4InstID] == VDEC_MPEG2) && _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.fgPpEnable)
          {
              prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_PP >> 2;
              prDownScalerPrm->fgEnColorCvt = FALSE;
          }
          else
          {
        prDownScalerPrm->ucScrAgent = RW_VDSCL_SRC_MC >> 2;
        prDownScalerPrm->fgEnColorCvt = (BOOL) (((UINT32) rand())&0x1);//random(2);
      }
      }
      prDownScalerPrm->u4SrcHeight = dwPicHeightDec;
      prDownScalerPrm->u4SrcWidth = dwPicWidthDec;

      prDownScalerPrm->u4SrcYOffH = 0;
      prDownScalerPrm->u4SrcYOffV = 0;
      prDownScalerPrm->u4SrcCOffH = 0;
      prDownScalerPrm->u4SrcCOffV = 0;
      prDownScalerPrm->u4SclYOffH = 0;
      prDownScalerPrm->u4SclYOffV = 0;
      prDownScalerPrm->u4SclCOffH = 0;
      prDownScalerPrm->u4SclCOffV = 0;


      if(_fgVDSCLEnableRandomTest[u4InstID])
      {
        prDownScalerPrm->ucScanType = (UCHAR) (((UINT32) rand())&0x1);//random(2);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
        prDownScalerPrm->ucVdoFmt = 0;
#else
        prDownScalerPrm->ucVdoFmt = (UCHAR) (((UINT32) rand())&0x1);//random(2);
#endif

        while(TRUE)
        {
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
          if (dwPicWidthDec > 960)
          {
             prDownScalerPrm->u4TrgWidth = (UINT32) (((UINT32) rand())%960)+0x10;

             if (prDownScalerPrm->u4TrgWidth > 960)
                prDownScalerPrm->u4TrgWidth = 960;

          }
          else
#endif
          {
             prDownScalerPrm->u4TrgWidth = (UINT32) (((UINT32) rand())%dwPicWidthDec)+0x10;
          }

          prDownScalerPrm->u4TrgHeight = (UINT32) (((UINT32) rand())%dwPicHeightDec)+0x10;
          if(((prDownScalerPrm->u4TrgWidth%2)==0)
            &&((prDownScalerPrm->u4TrgHeight%4)==0)
            &&(prDownScalerPrm->u4TrgWidth <= dwPicWidthDec)
            &&(prDownScalerPrm->u4TrgHeight <= (dwPicHeightDec>>(prDownScalerPrm->ucVdoFmt)))
          )
          {
            if(prDownScalerPrm->u4TrgWidth == dwPicWidthDec)
            {
              prDownScalerPrm->u4TrgOffH = 0;
            }
            else
            {
              prDownScalerPrm->u4TrgOffH = (((((UINT32) rand())%(dwPicWidthDec-prDownScalerPrm->u4TrgWidth))>>1)<<1);
            }
            if(prDownScalerPrm->u4TrgHeight == (dwPicHeightDec>>(prDownScalerPrm->ucVdoFmt)))
            {
              prDownScalerPrm->u4TrgOffV = 0;
            }
            else
            {
              prDownScalerPrm->u4TrgOffV = (((((UINT32) rand())%((dwPicHeightDec-prDownScalerPrm->u4TrgHeight)))>>2)<<2);
            }
            break;
          }
        }
        if(prDownScalerPrm->ucScrAgent == (RW_VDSCL_SRC_PP >> 2))
        {
          if(prDownScalerPrm->ucSpectType == (RW_VDSCL_SPEC_WMV >> 5))
          {
            #if 0
            if((prDownScalerPrm->u4SrcWidth>>1) >= prDownScalerPrm->u4TrgWidth)
            {
              prDownScalerPrm->u4SrcYOffH = (UINT32)(rand()%7);
              prDownScalerPrm->u4SrcCOffH = (UINT32)(rand()%4);
            }
            #endif
            if(prDownScalerPrm->ucPicStruct == TOP_BOTTOM_FIELD)
            {
              prDownScalerPrm->u4SrcYOffV = (UINT32)((((UINT32) rand())%3)*2);
              prDownScalerPrm->u4SrcCOffV = (UINT32)((((UINT32) rand())%3)*2);
            }
            else
            {
              prDownScalerPrm->u4SrcYOffV = (UINT32)(((UINT32) rand())%5);
              prDownScalerPrm->u4SrcCOffV = (UINT32)(((UINT32) rand())%5);
            }
          }
          else//h264
          {
            #if 0
            if((prDownScalerPrm->u4SrcWidth>>1) >= prDownScalerPrm->u4TrgWidth)
            {
              prDownScalerPrm->u4SrcYOffH = (UINT32)(rand()%9);
              prDownScalerPrm->u4SrcCOffH = (UINT32)(rand()%5);
            }
            #endif
            if(prDownScalerPrm->ucPicStruct == TOP_BOTTOM_FIELD)
            {
              prDownScalerPrm->u4SrcYOffV = (UINT32)((((UINT32) rand())%4)*2);
              prDownScalerPrm->u4SrcCOffV = (UINT32)((((UINT32) rand())%2)*2);
            }
            else
            {
              prDownScalerPrm->u4SrcYOffV = (UINT32)(((UINT32) rand())%7);
              prDownScalerPrm->u4SrcCOffV = (UINT32)(((UINT32) rand())%4);
            }
          }
        }
        else
        {
          #if 0
          if((prDownScalerPrm->u4SrcWidth>>1) >= prDownScalerPrm->u4TrgWidth)
          {
            prDownScalerPrm->u4SrcYOffH = (UINT32)(rand()%7);
            prDownScalerPrm->u4SrcCOffH = (UINT32)(rand()%4);
          }
          #endif
          if(prDownScalerPrm->ucPicStruct == TOP_BOTTOM_FIELD)
          {
            prDownScalerPrm->u4SrcYOffV = (UINT32)((((UINT32) rand())%5)*2);
            prDownScalerPrm->u4SrcCOffV = (UINT32)((((UINT32) rand())%3)*2);
          }
          else
          {
            prDownScalerPrm->u4SrcYOffV = (UINT32)(((UINT32) rand())%9);
            prDownScalerPrm->u4SrcCOffV = (UINT32)(((UINT32) rand())%5);
          }
        }
        if(prDownScalerPrm->fgEnColorCvt)
        {
          prDownScalerPrm->u4SrcYOffH = prDownScalerPrm->u4SrcYOffH - (prDownScalerPrm->u4SrcYOffH%2);
          prDownScalerPrm->u4SrcYOffV = prDownScalerPrm->u4SrcYOffV - (prDownScalerPrm->u4SrcYOffV%2);
          prDownScalerPrm->u4SrcCOffH = prDownScalerPrm->u4SrcCOffH - (prDownScalerPrm->u4SrcCOffH%2);
          prDownScalerPrm->u4SrcCOffV = prDownScalerPrm->u4SrcCOffV - (prDownScalerPrm->u4SrcCOffV%2);
        }
        if(prDownScalerPrm->fgMbaff)
        {
          prDownScalerPrm->u4SrcYOffH = 0;
          prDownScalerPrm->u4SrcYOffV = 0;
          prDownScalerPrm->u4SrcCOffH = 0;
          prDownScalerPrm->u4SrcCOffV = 0;
        }
        if((prDownScalerPrm->u4TrgWidth + prDownScalerPrm->u4TrgOffH) < 1920 )
        {
          prDownScalerPrm->u4DispW = ((((prDownScalerPrm->u4TrgWidth + prDownScalerPrm->u4TrgOffH  + (((UINT32) rand())%(1920 - prDownScalerPrm->u4TrgWidth - prDownScalerPrm->u4TrgOffH  )))+15)>>4)<<4);
        }
        else
        {
          prDownScalerPrm->u4DispW = 1920;
        }
        prDownScalerPrm->u4TrgBufLen = prDownScalerPrm->u4DispW;
      }
      else
      {
        //srand(IO_READ32(PARSER_BASE,0x4C)&0xFFFF);
        prDownScalerPrm->ucScanType = 0;//random(2);
        prDownScalerPrm->ucVdoFmt = 0;//random(2);
        //prDownScalerPrm->u4DispW = (((dwPicWidthDec + 15) >> 4) << 4);
        //prDownScalerPrm->u4TrgBufLen = prDownScalerPrm->u4DispW;
        while(TRUE)
        {
          prDownScalerPrm->u4TrgWidth = dwPicWidthDec; //random(dwPicWidthDec) + 0x40;
          prDownScalerPrm->u4TrgHeight = dwPicHeightDec;//random(dwPicHeightDec) + 0x30;
          if((prDownScalerPrm->u4TrgHeight>=8)&&(prDownScalerPrm->u4TrgWidth>=8)&&((prDownScalerPrm->u4TrgWidth%2)==0)&&((prDownScalerPrm->u4TrgHeight%4)==0)
          &&(prDownScalerPrm->u4TrgWidth <= dwPicWidthDec)&&(prDownScalerPrm->u4TrgHeight <= (dwPicHeightDec>>(prDownScalerPrm->ucVdoFmt))))
          {
            if(prDownScalerPrm->u4TrgWidth == dwPicWidthDec)
            {
              prDownScalerPrm->u4TrgOffH = 0;
            }
            else
            {
              prDownScalerPrm->u4TrgOffH = (((((UINT32) rand())%(dwPicWidthDec-prDownScalerPrm->u4TrgWidth))>>1)<<1);
            }
            if(prDownScalerPrm->u4TrgHeight == (dwPicHeightDec>>(prDownScalerPrm->ucVdoFmt)))
            {
              prDownScalerPrm->u4TrgOffV = 0;
            }
            else
            {
              prDownScalerPrm->u4TrgOffV = (((((UINT32) rand())%((dwPicHeightDec-prDownScalerPrm->u4TrgHeight)))>>2)<<2);
            }
            break;
          }
        }
        if((prDownScalerPrm->u4TrgWidth + prDownScalerPrm->u4TrgOffH) < 1920 )
        {
          prDownScalerPrm->u4DispW = ((((prDownScalerPrm->u4TrgWidth + prDownScalerPrm->u4TrgOffH  + (((UINT32) rand())%(1920 - prDownScalerPrm->u4TrgWidth - prDownScalerPrm->u4TrgOffH  )))+15)>>4)<<4);
        }
        else
        {
          prDownScalerPrm->u4DispW = 1920;
        }

        prDownScalerPrm->u4DispW = (((dwPicWidthDec + 15) >> 4) << 4);
        prDownScalerPrm->u4TrgBufLen = prDownScalerPrm->u4DispW;
      #if 0
        prDownScalerPrm->u4TrgHeight = dwPicHeightDec;
        prDownScalerPrm->u4TrgWidth = dwPicWidthDec;
        prDownScalerPrm->u4TrgOffH = 0;
        prDownScalerPrm->u4TrgOffV = 0;
      #endif
      }
      //prDownScalerPrm->pucTrgYAddr = _pucVDSCLBuf[u4InstID];
      prDownScalerPrm->u4TrgYAddr = (UINT32)(_pucVDSCLBuf[u4InstID]);
      u4DramPicSize = 0x1FE000;//1920*1088//((((_tVerMpvDecPrm.u4PicW + 15) >> 4) * ((_tVerMpvDecPrm.u4PicH + 31) >> 5)) << 9);
      //prDownScalerPrm->u4TrgCAddr = *(UINT32*)(_pucVDSCLBuf[u4InstID] + u4DramPicSize);
      prDownScalerPrm->u4TrgCAddr = prDownScalerPrm->u4TrgYAddr + u4DramPicSize;
      //prDownScalerPrm->u4WorkAddr = *(UINT32*)_pucVDSCLWorkBuf[u4InstID];
      prDownScalerPrm->u4WorkAddr = (UINT32)(_pucVDSCLWorkBuf[u4InstID]);
      vFilledFBuf(u4InstID, _pucVDSCLBuf[u4InstID], _ptCurrFBufInfo[u4InstID]->u4DramPicSize);
    }
    //vVDecOutputDebugString("Vdo=%d PicHeight= %d TargHeight=%d DispW=%d\n",prDownScalerPrm->ucVdoFmt,
    //dwPicHeightDec, prDownScalerPrm->u4TrgHeight,prDownScalerPrm->u4DispW);
  }
  else
  {
    prDownScalerPrm->fgDSCLEn = FALSE;
  }

#if 0
  if((_u4CodecVer == WMV)&&(_rWMVPPS.i4CurrentTemporalField==0)&&(prDownScalerPrm->fgDSCLEn==TRUE))
  {
    vFilledFBuf(_pucVDSCLBuf, _ptCurrFBufInfo->u4DramPicSize);
  }
  else if((_u4CodecVer != H264)&&(!_fgDec2ndFldPic)&&(prDownScalerPrm->fgDSCLEn==TRUE))
  {
    vFilledFBuf(_pucVDSCLBuf, _ptCurrFBufInfo->u4DramPicSize);
  }
#endif
#endif
}

void ComputeDQuantDecParam(UINT32 u4InstID)
{
    INT32 i4StepSize;
    INT32 i4DCStepSize ;
    VDEC_INFO_WMV_ETRY_PRM_T *prWMVEPS = &_rWMVEPS[u4InstID];
    VDEC_INFO_WMV_PIC_PRM_T *prWMVPPS = &_rWMVPPS[u4InstID];

    for (i4StepSize = 1; i4StepSize < 63; i4StepSize++) {
        VDEC_INFO_WMV_DQUANT_PRM_T *pDQ = &prWMVPPS->rDQuantParam3QPDeadzone[i4StepSize];

        INT32 i4DoubleStepSize = (i4StepSize + 1);

        pDQ->i4DoubleStepSize = i4DoubleStepSize;
        pDQ->i4StepMinusStepIsEven = 0;
        pDQ->i4DoublePlusStepSize = i4DoubleStepSize;
        pDQ->i4DoublePlusStepSizeNeg = -1 * pDQ->i4DoublePlusStepSize;

        i4DCStepSize = (i4StepSize + 1) >> 1;
        if (i4DCStepSize <= 4) {
            pDQ->i4DCStepSize = 8;
            if(prWMVEPS->fgNewDCQuant && i4DCStepSize <= 2) {
                pDQ->i4DCStepSize = 2 * i4DCStepSize;
            }
        } else {
            pDQ->i4DCStepSize = i4DCStepSize / 2 + 6;
        }
    }

    for (i4StepSize = 1; i4StepSize < 63; i4StepSize++) {
        VDEC_INFO_WMV_DQUANT_PRM_T *pDQ = &prWMVPPS->rDQuantParam5QPDeadzone [i4StepSize];
        INT32 i4DoubleStepSize;

        i4DoubleStepSize = (i4StepSize + 1);

        pDQ->i4DoubleStepSize = i4DoubleStepSize;

        if (_i4CodecVersion[u4InstID] >= VDEC_WMV3) {
            pDQ->i4StepMinusStepIsEven = (i4StepSize + 1) >> 1;

            pDQ->i4DoublePlusStepSize = i4DoubleStepSize + pDQ->i4StepMinusStepIsEven;
        } else {
            INT32 iStepSize2 = (i4StepSize + 1) >> 1;
            pDQ->i4StepMinusStepIsEven = iStepSize2 - ((iStepSize2 & 1) == 0);
            pDQ->i4DoublePlusStepSize = i4DoubleStepSize + pDQ->i4StepMinusStepIsEven;
        }

	    pDQ->i4DoublePlusStepSizeNeg = -1 * pDQ->i4DoublePlusStepSize;

        i4DCStepSize = (i4StepSize + 1) >> 1;
        if (i4DCStepSize <= 4) {
            pDQ->i4DCStepSize = 8;
            if(prWMVEPS->fgNewDCQuant && i4DCStepSize <= 2)
                pDQ->i4DCStepSize = 2 * i4DCStepSize;

        } else {
            pDQ->i4DCStepSize = i4DCStepSize / 2 + 6;
        }
    }
}

// *********************************************************************
// Function    : void vVParserProc(UINT32 u4InstID,UCHAR ucVldID);
// Description : Video parser procedure
// Parameter   : None
// Return      : None
// *********************************************************************
void vVParserProc(UINT32 u4InstID,UCHAR ucVldID)
{
  UINT32 u4VldByte,u4VldBit,u4RetVal;
  char strMessage[512];

  if (_u4CodecVer[u4InstID] == VDEC_RM)
  {
    #ifdef RM_ATSPEED_TEST_ENABLE
    vRM_VParserEx(u4InstID);
    #else //RM_ATSPEED_TEST_ENABLE
    vRM_VParser(u4InstID);
    #endif //RM_ATSPEED_TEST_ENABLE
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_H265)
  {
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bNoDecode = 0;
        u4RetVal = vHEVCSearchRealPic(u4InstID,ucVldID);

        if (NOT_SUPPORT == u4RetVal)
        {
            _u4VerBitCount[u4InstID] = 0xFFFFFFFF;
            _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bNoDecode = 1;
            _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;  //skip decode
        }
        else if ( PARSE_OK != u4RetVal)
        {
            if (SLICE_SYNTAX_ERROR==u4RetVal)
            {
                _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bNoDecode = 1;
                _u4PicCnt[u4InstID]++;  //skip decode
            }
            _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;  //keep header parsing
        } 
        else 
        {
            _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
        }
    
  }
  else if(_u4CodecVer[u4InstID] == VDEC_H264)
  //if(_u4CodecVer[u4InstID] == VDEC_H264)
  {
#if defined(MVC_VFY_SUPPORT)
    while(_ucMVCType[u4InstID] && (_fgMVCReady[u4InstID] == FALSE))
    {
        x_thread_delay(5);
    }
#endif
    #ifdef MVC_DEBUG
    UTIL_Printf("InstID (%d),_ucMVCType (%d)\n",u4InstID,_ucMVCType[u4InstID]);
    #endif
    vSearchRealPic(u4InstID);
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_WMV)
  {

    VDEC_INFO_WMV_SEQ_PRM_T *prWMVSPS = &_rWMVSPS[u4InstID];
    VDEC_INFO_WMV_PIC_PRM_T *prWMVPPS = &_rWMVPPS[u4InstID];

    _u4CurrPicStartAddr[u4InstID] = u4VDEC_HAL_WMV_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
   #if WMV_EC_IMPROVE_SUPPORT
    //Search Slice Start Code
    if(_i4CodecVersion[u4InstID] == VDEC_VC1)
    {
        vWMVSearchSliceStartCode(u4InstID);
    }
    #endif
#endif

    if(fgVParserProcWMV(u4InstID))
    {
      if(prWMVSPS->fgXintra8)
      {
        _u4WMVDecPicNo[u4InstID]++;
        _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
        return;
      }
      UpdateVopheaderParam(u4InstID);
      if(prWMVPPS->ucPicType == SKIPFRAME)
      {
        VDEC_INFO_WMV_VFIFO_PRM_T rWmvVFifoInitPrm;
        VDEC_INFO_WMV_BS_INIT_PRM_T rWmvBSInitPrm;

        _u4SkipFrameCnt[u4InstID]++;
        _u4WMVDecPicNo[u4InstID]++;
        _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
        if(_i4CodecVersion[u4InstID] != VDEC_VC1)
        {
          rWmvVFifoInitPrm.u4CodeType = _i4CodecVersion[u4InstID];
          rWmvVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
          rWmvVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
          i4VDEC_HAL_WMV_InitVDecHW(u4InstID,&rWmvVFifoInitPrm);
          if(_iSetPos[u4InstID] >= V_FIFO_SZ)
          {
            _iSetPos[u4InstID] = _iSetPos[u4InstID] - V_FIFO_SZ;
          }
          rWmvBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
          rWmvBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
          rWmvBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID] + _iSetPos[u4InstID];
        #ifndef  RING_VFIFO_SUPPORT
          rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
        #else
 //         rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
          rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
        #endif
          i4VDEC_HAL_WMV_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rWmvBSInitPrm, FALSE);
        }
        else
        {
          vVDEC_HAL_WMV_AlignRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], BYTE_ALIGN);  //in order to use fgNextStartCode().
          u4VldByte = u4VDEC_HAL_WMV_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
          rWmvVFifoInitPrm.u4CodeType = _i4CodecVersion[u4InstID];
          rWmvVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
          rWmvVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
          i4VDEC_HAL_WMV_InitVDecHW(u4InstID,&rWmvVFifoInitPrm);
          rWmvBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
          rWmvBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
          rWmvBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID] + u4VldByte;
        #ifndef  RING_VFIFO_SUPPORT
          rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
        #else
 //         rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
          rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
        #endif
          i4VDEC_HAL_WMV_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rWmvBSInitPrm, TRUE);
        }
      }
      else
      {
        u4VldByte = u4VDEC_HAL_WMV_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
        //vVDecOutputDebugString("BYTE = %d and Bit = %d after parsing\n",u4VldByte,u4VldBit);
        _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
      }
    }
    else
    {
      if(_u4VprErr[u4InstID] == END_OF_FILE)
      {
        _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
      }
      else
      {
        strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
        sprintf(strMessage,"Parsing header error : 0x%.8x\n",_u4VprErr[u4InstID]);
        fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
      }
    }

#if VDEC_DDR3_SUPPORT
  /* if (_rWMVSPS[u4InstID].u4PicWidthSrc > 720|| _rWMVSPS[u4InstID].u4PicHeightSrc> 576)
   {
        UTIL_Printf("DDR3 Not Support Size over HD\n");
        strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
        sprintf(strMessage,"DDR3 Size Over HD : Not Support in FPGA\n");
        fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);

        _u4VerBitCount[u4InstID] = 0xFFFFFFFF;
   }*/
#endif


  }
  else if(_u4CodecVer[u4InstID] == VDEC_MPEG2)
  {
    vDEC_HAL_COMMON_SetVLDPower(u4InstID,ON);
    _u4CurrPicStartAddr[u4InstID] = u4VDEC_HAL_MPEG_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
    u4VParserMPEG12(u4InstID, FALSE);

    #if DUAL_VDEC_TEST
    //dual-format common setting
    UTIL_Printf("u4InstID:%d,_u4RealHSize:%d, _u4RealVSize:%d\n",u4InstID, _u4RealHSize[u4InstID],_u4RealVSize[u4InstID]);
    vVDEC_HAL_MPEG_SetDualFormatComReg(u4InstID, _u4RealHSize[u4InstID] , _u4RealVSize[u4InstID] );
    #endif
    
    vDEC_HAL_COMMON_SetVLDPower(u4InstID,OFF);
    switch(_u4PicCdTp[u4InstID])
    {
      case I_TYPE:
      case P_TYPE:
        vVPrsMPEGIPProc(u4InstID);
        break;
      case B_TYPE:
        vVPrsMPEGBProc(u4InstID);
        break;
      default:
        break;
    }
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
  }
  else if((_u4CodecVer[u4InstID] == VDEC_DIVX3) || (_u4CodecVer[u4InstID] == VDEC_MPEG4) || (_u4CodecVer[u4InstID] == VDEC_H263))
  {
    vDEC_HAL_COMMON_SetVLDPower(u4InstID,ON);
    _u4CurrPicStartAddr[u4InstID] = u4VDEC_HAL_MPEG_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
    u4VParserMPEG4(u4InstID, FALSE);
    vDEC_HAL_COMMON_SetVLDPower(u4InstID,OFF);
    if(_fgVerVopCoded0[u4InstID])
    {
    #ifndef	VPMODE
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
    #else
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;//qiguo 8/6
    #endif
    }
    else
    {
      switch(_u4PicCdTp[u4InstID])
      {
        case I_TYPE:
        case P_TYPE:
          vVPrsMPEGIPProc(u4InstID);
          break;
        case B_TYPE:
          vVPrsMPEGBProc(u4InstID);
          break;
        default:
          break;
      }
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
    }
  }
  else if(_u4CodecVer[u4InstID] == VDEC_VP6)
  {
      u4VerVParserVP6(u4InstID, FALSE);
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_VP8)
  {
      u4VerVParserVP8(u4InstID, FALSE);
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_VP9)
  {
      vVerVParserVP9(u4InstID, FALSE);
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_AVS)
  {
      u4VerVParserAVS(u4InstID, FALSE);
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
  }
  #ifdef VERIFY_DV_SUPPORT
  else if(_u4CodecVer[u4InstID] == DV)
  {
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
  }
  #endif
}


void vAVCLAEDecStart(UINT32 u4InstID)
{
	UINT32 u4LAEID;
	u4LAEID = (u4InstID == 0) ? VDEC_LAE0 : VDEC_LAE1;
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vAVCLAEDecStart start\n");
#endif
	i4VDEC_HAL_H264_LAEDecStart(u4LAEID, &_tVerMpvDecPrm[u4LAEID]);
#ifdef VDEC_SIM_DUMP
	UTIL_Printf("vAVCLAEDecStart end\n");
#endif

}


void vVDecLaeInit(UINT32 u4InstID)
{
    UINT32 u4LaeID = 0;
    u4LaeID = (u4InstID == 0) ? VDEC_LAE0 : VDEC_LAE1;
    VDEC_INFO_DEC_PRM_T  *ptVerMpvDecPrm;
    ptVerMpvDecPrm = &_tVerMpvDecPrm[u4LaeID];

    ptVerMpvDecPrm->SpecDecPrm.rVDecH265DecPrm.rLaeInfo.u4LaeCabacAddr = _pucBSCABACBuff[u4InstID];
    ptVerMpvDecPrm->SpecDecPrm.rVDecH265DecPrm.rLaeInfo.u4LaeEcAddr = _pucECInfoBuff[u4InstID];
    ptVerMpvDecPrm->SpecDecPrm.rVDecH265DecPrm.prPicInfo = &_rH265PicInfo[u4LaeID];
}
// *********************************************************************
// Function    : void vVDecLAEProc(UINT32 u4InstID,UCHAR ucVldID)
// Description : Set VDec LAE related parameters
// Parameter   : None
// Return      : None
// *********************************************************************
void vVDecLaeProc(UINT32 u4InstID,UCHAR ucVldID)
{
    UINT32 u4RetVal = PARSE_OK;
    BOOL fgLaeDecDone = FALSE;
    UINT32 u4LaeID = 0;
    UINT32 u4IrqId = 0,u4TimeCount = 0;

    if(u4InstID == 0)
    {
        u4LaeID = VDEC_LAE0;
        u4IrqId = 0;
    }
    else
    {
        u4LaeID = VDEC_LAE1;
        u4IrqId = 1;
    }
    
    _fgLaeComplete[u4IrqId] = FALSE;  
    if(_u4CodecVer[u4InstID] == VDEC_H265)
    {
        ucVldID = VDEC_LAE0;
        vVDecLaeInit(u4InstID);
        u4RetVal = vHEVCLAEParseSliceNal(u4InstID,ucVldID);

        if (SLICE_SYNTAX_ERROR == u4RetVal)
        {
            //_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bNoDecode = 1;
            _u4LaeDecCnt[u4InstID]++;  //skip decode
        }
        else
        {
            vHEVCLAETrigDec(u4LaeID);

            #ifndef IRQ_DISABLE
            while((_fgLaeComplete[u4IrqId] == FALSE) && (u4TimeCount < 10))
            {
                x_thread_delay(10);
                u4TimeCount ++;
            }
            
            if((_fgLaeComplete[u4IrqId] == FALSE) || (u4TimeCount >= 10))
            {
                UTIL_Printf("[H265-LAE] LAE decode timeout ###########\n");
            }
            #else
            fgLaeDecDone = fgHEVCDecIsFinish(u4LaeID,jiffies);
            if(fgLaeDecDone)
            {
                //Dump LAE buffer for check LAE decode 
                // vVDecDumpLaeBuffer(u4InstID); leon
    //            UTIL_Printf("[H265-LAE] LAE decode finish!!!!!!!!!\n");
            }
            else
            {
                vVDecDumpLaeBuffer(u4InstID);
                UTIL_Printf("[H265-LAE] LAE decode timeout  !!!!!!!!!\n");
            }
            #endif
            
            //clear LAE interrupt bit
            if(u4LaeID == VDEC_LAE0)
            {
                //vVDecWriteMCORETOP(u4InstID,RO_LAE_STATUS,(u4VDecReadMCORETOP(u4InstID, RO_LAE_STATUS) & (~(1 << 24))));
                vVDecWriteMCORETOP(u4InstID,RO_LAE_STATUS,((u4VDecReadMCORETOP(u4InstID, RO_LAE_STATUS) | ((1 << 24)))) & (~(1 << 16)) & (~(1 << 17)) & (~(1 << 25)));      
                vVDecWriteMCORETOP(u4InstID,RO_LAE_STATUS,((u4VDecReadMCORETOP(u4InstID, RO_LAE_STATUS) & (~(1 << 24))))& (~(1 << 16)) & (~(1 << 17)) & (~(1 << 25)));
            }
            else
            {
                //vVDecWriteMCORETOP(u4InstID,RO_LAE_STATUS,(u4VDecReadMCORETOP(u4InstID, RO_LAE_STATUS) & (~(1 << 24))));   
                vVDecWriteMCORETOP(u4InstID,RO_LAE_STATUS,((u4VDecReadMCORETOP(u4InstID, RO_LAE_STATUS) | ((1 << 25)))) & (~(1 << 16)) & (~(1 << 17)) & (~(1 << 24)));      
                vVDecWriteMCORETOP(u4InstID,RO_LAE_STATUS,((u4VDecReadMCORETOP(u4InstID, RO_LAE_STATUS) & (~(1 << 25))))& (~(1 << 16)) & (~(1 << 17)) & (~(1 << 24)));
            }
            _u4LaeDecCnt[u4InstID] ++;
        }
    }
    if (_u4CodecVer[u4InstID] == VDEC_H264)
	{
		ucCurrStatus = VDEC_LAE_ST;
		u4RetVal = vAVCLAEParseSliceHeader(u4InstID);
        vAVCLAEDecStart(u4InstID);
		#ifndef IRQ_DISABLE
		#else
        fgLaeDecDone = fgAVCLAEDecIsFinish(u4LaeID, jiffies);
        if(fgLaeDecDone)
        {
        	if (14 == _u4FileCnt[0])
        	{
        	//vVDecDumpLaeBuffer(u4InstID);
        	}
            UTIL_Printf("[H264-LAE] LAE decode finish!!!!!!!!!\n");
        }
        else
        {      	
            UTIL_Printf("[H264-LAE] LAE decode timeout!!!!!!!!!\n");
			vVDEC_HAL_H264_VDec_DumpReg(VDEC_LAE0);
			//vVDecDumpLaeBuffer(u4InstID);			
        }		
        vVDecWriteMCORETOP(u4LaeID, RO_LAE_STATUS,(u4VDecReadMCORETOP(u4LaeID, RO_LAE_STATUS) & (~(1 << 24))));	
		#endif
        _u4LaeDecCnt[u4InstID] ++;
	}
	
    if((_u4LaeMode[0] == 1) || ( SLICE_SYNTAX_ERROR == u4RetVal))
    {
        //if test LAE decode only
        printk("Lae decode continue ........\n");
        _tVerDec[u4InstID].ucState = DEC_NORM_LAE_DEC;
    }
    else
    {
        //else test dual core decode
        //go to core0  parser header
        //core1 parser again ? if no, what about hw acc ref registers
        _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;
    }
}


void vVPrsMPEGIPProc(UINT32 u4InstID)
{
  if(_u4PicStruct[u4InstID] == FRM_PIC)
  {
    if(_u4BrokenLink[u4InstID] > 0)
    {
      _u4BrokenLink[u4InstID] --;
    }
    _fgDec2ndFldPic[u4InstID] = 0;
    vSetVerFRefBuf(u4InstID, _u4BRefBufIdx[u4InstID]);
    vSetVerBRefBuf(u4InstID, 1 - _u4FRefBufIdx[u4InstID]);
    vSetVerDecBuf(u4InstID, _u4BRefBufIdx[u4InstID]);

  }
  else  // Field Picture
  {
    if(_fgVerPrevBPic[u4InstID])
    {
      _fgDec2ndFldPic[u4InstID] = 0;
    }
    // 1st Field Picture
    if(!_fgDec2ndFldPic[u4InstID])
    {
      if(_u4BrokenLink[u4InstID] > 0)
      {
        _u4BrokenLink[u4InstID] --;
      }
      // Set Reference Buffer
      vSetVerFRefBuf(u4InstID, _u4BRefBufIdx[u4InstID]);
      vSetVerBRefBuf(u4InstID, 1 - _u4FRefBufIdx[u4InstID]);
      vSetVerDecBuf(u4InstID, _u4BRefBufIdx[u4InstID]);
      _u4PicStruct[u4InstID] = (_u4PicStruct[u4InstID] == TOP_FLD_PIC) ? TWO_FLDPIC_TOPFIRST : TWO_FLDPIC_BTMFIRST;
    }
    // 2nd Field Picture
    else
    {
      vSetVerDecBuf(u4InstID, _u4BRefBufIdx[u4InstID]);
    }
  }
}

void vVPrsMPEGBProc(UINT32 u4InstID)
{
  if(_u4PicStruct[u4InstID] == FRM_PIC || !_fgDec2ndFldPic[u4InstID])
  {
    // Set Decoding Buffer
    vSetVerDecBuf(u4InstID, 2);
  }
  else
  {
    // Field picture and 2ND_FLD_PIC
    vSetVerDecBuf(u4InstID, 2);
  }

  if(_u4PicStruct[u4InstID] == FRM_PIC)
  {
    _fgDec2ndFldPic[u4InstID] = FALSE;
  }
  else
  {
    if(!_fgVerPrevBPic[u4InstID])
    {
      _fgDec2ndFldPic[u4InstID] = FALSE;
    }
  }
}


// *********************************************************************
// Function    : void vVDecProc(UINT32 u4InstID,UCHAR ucVldID)
// Description : Set VDec related parameters
// Parameter   : None
// Return      : None
// *********************************************************************
BOOL _fgReInitBS=TRUE;
void vVDecProc(UINT32 u4InstID,UCHAR ucVldID)
{
  UINT32 u4Bits;
  _fgVDecComplete[u4InstID] = FALSE;
  #ifdef MPEG2_THRESHOLD_TEST
  _fgVDecComplete[1] = FALSE;
  #endif

#if VDEC_DRAM_BUSY_TEST
     vDrmaBusySet (u4InstID);
#endif
#if VDEC_DDR3_SUPPORT
    _tVerMpvDecPrm[u4InstID].ucAddrSwapMode = ADDRSWAP_DDR3;
#else
    _tVerMpvDecPrm[u4InstID].ucAddrSwapMode = _u2AddressSwapMode[u4InstID];
#endif

  if (_u4CodecVer[u4InstID] == VDEC_RM)
  {
    vRM_TriggerDecode(u4InstID, &_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecRMDecPrm.rRMParsPicInfo);
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_H265)
  {
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4NuhTemporalId = _u4NuhTemporalId[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.prCurrFBufInfo = _ptH265CurrFBufInfo[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.prCurrFBufInfo->u4YStartAddr = /*PHYSICAL*/((UINT32)_pucDecWorkBuf[u4InstID]); 

    #ifdef VDEC_SIM_DUMP
    printk("[INFO] PP_OUT  u4YStartAddr: 0x%08X,  u4CStartAddr: 0x%08X\n", PHYSICAL(_ptH265CurrFBufInfo[u4InstID]->u4YStartAddr),
                                PHYSICAL(_ptH265CurrFBufInfo[u4InstID]->u4CStartAddr));
    #endif
    if (_u4PicCnt[u4InstID] == _u4DumpRegPicNum[u4InstID]){
        vVDEC_HAL_H265_VDec_DumpReg(u4InstID, 0);
    }
#ifdef HEVC_DUMP_BITSTREAM_INFO
        UINT32 u4Bits, u4FrameSPtr;
        UCHAR pcFilename_info[200] = {0};
        UCHAR pcInfo_data[200] = {0};
    
        u4FrameSPtr = u4VDEC_HAL_H265_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
        if (u4FrameSPtr<_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4PreReadPtr){
            _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4ReadPtrOffset++;
        }
        strncpy (pcFilename_info , _bFileStr1[u4InstID][1], (strlen(_bFileStr1[u4InstID][1]) -26));
        strcat (pcFilename_info, "_bitstream.info");
        sprintf(pcInfo_data, "frame %d 0x%x ", _u4PicCnt[u4InstID], _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4ReadPtrOffset*V_FIFO_SZ+u4FrameSPtr);
        fgWrData2PC(pcInfo_data, strlen(pcInfo_data), 7, pcFilename_info);       
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4PreReadPtr = u4FrameSPtr;
#endif

    if(_u4DualCoreEnable[u4InstID] == 1)
    {
        vVDecTrigMultiCore(u4InstID,PHYSICAL(_pucBSCABACBuff[u4InstID]),PHYSICAL(_pucECInfoBuff[u4InstID]), 0);
    }
    else if(_u4DualCoreEnable[u4InstID] == 2)
    {
        vVDecTrigMultiCore(u4InstID,PHYSICAL(_pucBSCABACBuff[u4InstID]),PHYSICAL(_pucECInfoBuff[u4InstID]), 1);
    }
    else
    {
        #if DEBUG_LEVEL >= DBG_LEVEL_INFO
        printk("[INFO] Rptr single decode slice %d start 0x%x - ",_u4PicCnt[u4InstID],u4VDEC_HAL_H265_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits));
        #endif
        i4VDEC_HAL_H265_DecStart(u4InstID, &_tVerMpvDecPrm[u4InstID]);
    }
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;

  }
  else if(_u4CodecVer[u4InstID] == VDEC_H264)
  //if(_u4CodecVer[u4InstID] == VDEC_H264)
  {
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.u4OffsetBeforeDec = u4VDEC_HAL_H264_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &_u4BitCount[u4InstID]);
	if (1 == _u4DualCoreEnable[u4InstID])
	{
		_tVerMpvDecPrm[1].SpecDecPrm.rVDecH264DecPrm.u4OffsetBeforeDec = u4VDEC_HAL_H264_ReadRdPtr(0, VDEC_VLD1, (UINT32)_pucVFifo[u4InstID], &_u4BitCount[u4InstID]);
	}
  #ifdef REDEC
    _u4VLDPosByte[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &_u4VLDPosBit[u4InstID]);
    if( _u4ReDecCnt[u4InstID] > 0)
    {
     vVerifyVDecSetPicInfo(u4InstID, &_tVerMpvDecPrm[u4InstID]);
    }
  #endif
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucNalRefIdc = _u4NalRefIdc[u4InstID];
  	if (1 == _u4DualCoreEnable[u4InstID])
  	{
  		_tVerMpvDecPrm[1].SpecDecPrm.rVDecH264DecPrm.ucNalRefIdc = _u4NalRefIdc[1];
  	}

    if(_ucMVCType[u4InstID] == 2)
    {
       //Dep View
       _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsAllegMvcCfg = (_u4NalRefIdc[0] > 0)? 1: 0;
    }
    else
    {
       _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsAllegMvcCfg = 0;
    }

    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsFrmPic = fgIsFrmPic(u4InstID);
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsIDRPic = fgIsIDRPic(u4InstID);
    //_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.u4DecWorkBuf = (UINT32)_pucDecWorkBuf[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prCurrFBufInfo = _ptCurrFBufInfo[u4InstID];
	#ifdef Y_C_SEPERATE
	#else
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prCurrFBufInfo->u4YStartAddr = /*PHYSICAL*/((UINT32)_pucDecWorkBuf[u4InstID]);
	#endif
	if (1 == _u4DualCoreEnable[u4InstID])
	{
		_tVerMpvDecPrm[1].SpecDecPrm.rVDecH264DecPrm.fgIsFrmPic = fgIsFrmPic(VDEC_VLD1);
	    _tVerMpvDecPrm[1].SpecDecPrm.rVDecH264DecPrm.fgIsIDRPic = fgIsIDRPic(VDEC_VLD1);
	    //_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.u4DecWorkBuf = (UINT32)_pucDecWorkBuf[u4InstID];
	    _tVerMpvDecPrm[1].SpecDecPrm.rVDecH264DecPrm.prCurrFBufInfo = _ptCurrFBufInfo[1];
	    //_tVerMpvDecPrm[1].SpecDecPrm.rVDecH264DecPrm.prCurrFBufInfo->u4YStartAddr = /*PHYSICAL*/((UINT32)_pucDecWorkBuf[1]);
	}
  #ifdef VERIFICATION_DOWN_SCALE
    #ifdef DOWN_SCALE_SUPPORT
        //vSetDownScaleParam(u4InstID, TRUE, &_tDownScalerPrm[u4InstID]);
        vSetDownScaleParam(u4InstID, TRUE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
    #else
        //vSetDownScaleParam(u4InstID, FALSE, &_tDownScalerPrm[u4InstID]);
        vSetDownScaleParam(u4InstID, FALSE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
    #endif
    //vDEC_HAL_COMMON_SetDownScaler(u4InstID, &_tDownScalerPrm[u4InstID]);
        //vVDECSetDownScalerPrm(u4InstID, &_tDownScalerPrm[u4InstID]);
        vVDECSetDownScalerPrm(u4InstID, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
  #endif
  #ifdef FGT_SUPPORT
    vSetFGTParam(&_rFGTPrm[u4InstID]);
    i4VDEC_HAL_H264_FGTSetting(u4InstID, &_rFGTPrm[u4InstID]);
  #endif

    if(_ucMVCType[u4InstID] == 2)
  	 _fgVDecComplete[0] = FALSE;

  #ifdef LETTERBOX_SUPPORT
    vLBDParaParsing(u4InstID);
    vVDECSetLetetrBoxDetPrm(u4InstID, &_rLBDPrm[u4InstID]);
  #endif

    //[20110826]temp fixed for 8580 build failed.
    //BSP_InvDCacheRange((UINT32)_pucDPB[u4InstID],DPB_SZ);
    vVDec_InvDCacheRange((UINT32)_pucDPB[u4InstID], DPB_SZ);

    #if 0
    {
    UINT32 _u4VLDPosBit = 0;

    UTIL_Printf("[H264] Inst%d, DecStart, ofst0:0x%x, ofst1:0x%x, rd:0x%x, input window: 0x%08x\n",
      u4InstID, _u4FileOffset[0], _u4FileOffset[1], u4VDEC_HAL_H264_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &_u4VLDPosBit), u4VDecReadAVCVLD(u4InstID, RO_AVLD_BARL));
    }
    #endif
	if (1 == _u4DualCoreEnable[u4InstID])
	{
		i4VDEC_HAL_H264_DecStart(VDEC_VLD0, &_tVerMpvDecPrm[0]);
		i4VDEC_HAL_H264_DecStart(VDEC_VLD1, &_tVerMpvDecPrm[1]);
		vVDEC_HAL_H264_TriggerDualCore(u4InstID);
	}
	else
	{
    	i4VDEC_HAL_H264_DecStart(u4InstID, &_tVerMpvDecPrm[u4InstID]);
		vVDEC_HAL_H264_TriggerDecode(u4InstID);
	}
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_WMV)
  {
      vVDecEnableCRC(u4InstID, 1, 0);
  #ifdef REDEC
    _u4VLDPosByte[u4InstID] = u4VDEC_HAL_WMV_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &_u4VLDPosBit[u4InstID]);
  #endif
    if(fgVDecProcWMV(u4InstID))
    {
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
    }
  }
  else if(_u4CodecVer[u4InstID] == VDEC_VP6)
  {
    vVerifyVDecSetVP6Info(u4InstID);
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_VP8)
  {
    vVerifyVDecSetVP8Info(u4InstID);
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_VP9)
  {
    vVerDecodeVP9(u4InstID);
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
  }
  else if(_u4CodecVer[u4InstID] == VDEC_AVS)
  {
    vVerifyVDecSetAVSInfo(u4InstID);
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
  }

  else
  {
     vVDecEnableCRC(u4InstID, 1, 1); // MPEG2 or MPEG4
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.fgDec2ndFld = _fgDec2ndFldPic[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegFrameBufSa.u4Pic0CSa = (UINT32)_pucPic0C[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegFrameBufSa.u4Pic0YSa = (UINT32)_pucPic0Y[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegFrameBufSa.u4Pic1CSa = (UINT32)_pucPic1C[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegFrameBufSa.u4Pic1YSa = (UINT32)_pucPic1Y[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegFrameBufSa.u4Pic2CSa = (UINT32)_pucPic2C[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegFrameBufSa.u4Pic2YSa = (UINT32)_pucPic2Y[u4InstID];
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.fgPpEnable = VDEC_PP_ENABLE;
  //  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.u4PpYBufSa = (UINT32)_pucPpYSa[u4InstID];
//    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.u4PpCBufSa = (UINT32)_pucPpCSa[u4InstID];    
#if VDEC_UFO_ENABLE
        UINT32 u4DramPicSize;
        UINT32 u4DramPicArea;
    
        u4PIC_WIDTH[u4InstID] = ((_tVerMpvDecPrm[u4InstID].u4PicW + 15)>>4)<<4;
        u4PIC_HEIGHT[u4InstID] = ((_tVerMpvDecPrm[u4InstID].u4PicH + 31)>>5)<<5;
        u4PIC_SIZE_Y[u4InstID] = ((u4PIC_WIDTH[u4InstID] * u4PIC_HEIGHT[u4InstID] + 511)>>9)<<9;
        u4PIC_SIZE[u4InstID] = (((u4PIC_SIZE_Y[u4InstID] + u4PIC_SIZE_Y[u4InstID]>>1) + 511)>>9)<<9;
        
        u4UFO_LEN_SIZE_Y[u4InstID] = ((((u4PIC_SIZE_Y[u4InstID] +255)>>8)+63+(16*8))>>6)<<6; 
        u4UFO_LEN_SIZE_C[u4InstID] = (((u4UFO_LEN_SIZE_Y[u4InstID]>>1)+15+(16*8))>>4)<<4;
    
        UTIL_Printf("u4PIC_SIZE_Y[0]:0x%x,u4PIC_SIZE[0]:0x%x,u4UFO_LEN_SIZE_Y[u4InstID]:0x%x,u4UFO_LEN_SIZE_C[0]:0x%x\n ",u4PIC_SIZE_Y[u4InstID],u4PIC_SIZE[u4InstID],u4UFO_LEN_SIZE_Y[0] ,u4UFO_LEN_SIZE_C[0]);
        u4PIC_SIZE_Y_BS[u4InstID] = ((u4PIC_SIZE_Y[u4InstID] + 4095)>>12)<<12;
        u4PIC_SIZE_BS[u4InstID] = ((u4PIC_SIZE_Y_BS[u4InstID] + (u4PIC_SIZE_Y[u4InstID]>>1)+511)>>9)<<9;
        u4PIC_SIZE_REF[u4InstID] = ((u4PIC_SIZE_BS[u4InstID] + (u4UFO_LEN_SIZE_Y[u4InstID]<<1) + 4095)>>12)<<12;
    
        _pucPic0C[u4InstID] = _pucPic0Y[u4InstID]+u4PIC_SIZE_Y_BS[u4InstID];
        _pucPic1C[u4InstID] = _pucPic1Y[u4InstID]+u4PIC_SIZE_Y_BS[u4InstID];
        _pucPic2C[u4InstID] = _pucPic2Y[u4InstID]+u4PIC_SIZE_Y_BS[u4InstID];
        UTIL_Printf("vVDecProc  u4PIC_SIZE_Y_BS[0]:0x%x,_pucPic0C[0]:0x%x,_pucPic1C[u4InstID]:0x%x,_pucPic2C[u4InstID]:0x%x\n",u4PIC_SIZE_Y_BS[u4InstID],_pucPic0C[u4InstID],_pucPic1C[u4InstID],_pucPic2C[u4InstID]);
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.fgDec2ndFld = _fgDec2ndFldPic[u4InstID];
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegFrameBufSa.u4Pic0CSa = (UINT32)_pucPic0C[u4InstID];
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegFrameBufSa.u4Pic1CSa = (UINT32)_pucPic1C[u4InstID];
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegFrameBufSa.u4Pic2CSa = (UINT32)_pucPic2C[u4InstID];
    
        _tVerMpvDecPrm[u4InstID].u4PIC_SIZE_BS = u4PIC_SIZE_BS[u4InstID];
        _tVerMpvDecPrm[u4InstID].u4UFO_LEN_SIZE_Y = u4UFO_LEN_SIZE_Y[u4InstID]; 
        _tVerMpvDecPrm[u4InstID].u4PIC_SIZE_Y_BS = u4PIC_SIZE_Y_BS[u4InstID];
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DramPicSize = u4PIC_SIZE_Y_BS[u4InstID];
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DramPicArea = ((u4PIC_SIZE_REF[u4InstID] + (u4PIC_SIZE_Y[u4InstID]>>4)+4095)>>12)<<12;
    
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.u4PpYBufSa = (UINT32)_pucPpYSa[u4InstID];
        _pucPpCSa[u4InstID] = _pucPpYSa[u4InstID]+u4PIC_SIZE_Y_BS[u4InstID];
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.u4PpCBufSa = (UINT32)_pucPpCSa[u4InstID]; 
#else
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.u4PpYBufSa = (UINT32)_pucPpYSa[u4InstID];
        _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.u4PpCBufSa = (UINT32)_pucPpCSa[u4InstID];
#endif    
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecH = ((_tVerMpvDecPrm[u4InstID].u4PicH + 15) >> 4 ) << 4;
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecW = ((_tVerMpvDecPrm[u4InstID].u4PicW + 15) >> 4 ) << 4;
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecXOff = 0;
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecYOff = 0;
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4FRefBufIdx = _u4FRefBufIdx[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4MaxMbl = ((_tVerMpvDecPrm[u4InstID].u4PicH + 15) >> 4 ) - 1;
    _tVerMpvDecPrm[u4InstID].ucDecFBufIdx = BYTE0(_u4DecBufIdx[u4InstID]);
    _tVerMpvDecPrm[u4InstID].ucPicStruct = BYTE0(_u4PicStruct[u4InstID]);;
    _tVerMpvDecPrm[u4InstID].ucPicType = BYTE0(_u4PicCdTp[u4InstID]);
//    _tVerMpvDecPrm[u4InstID].ucMpegSpecType = 3;//DIVX path
    _tVerMpvDecPrm[u4InstID].ucMpegSpecType = 2;//MPEG4 path
    if(_u4CodecVer[u4InstID] == VDEC_MPEG2)
    {
        _tVerMpvDecPrm[u4InstID].ucMpegSpecType = 4;//non
    }

    if(_u4CodecVer[u4InstID] == VDEC_MPEG2)
    {
    #ifdef REDEC
      _u4VLDPosByte[u4InstID] = u4VDEC_HAL_MPEG_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &_u4VLDPosBit[u4InstID]);
    #endif
    #ifdef VERIFICATION_DOWN_SCALE
      #ifdef DOWN_SCALE_SUPPORT
            //vSetDownScaleParam(u4InstID, TRUE, &_tDownScalerPrm[u4InstID]);
            vSetDownScaleParam(u4InstID, TRUE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
      #else
            //vSetDownScaleParam(u4InstID, FALSE, &_tDownScalerPrm[u4InstID]);
            vSetDownScaleParam(u4InstID, FALSE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
      #endif
      //vDEC_HAL_COMMON_SetDownScaler(u4InstID, &_tDownScalerPrm[u4InstID]);
            //vVDECSetDownScalerPrm(u4InstID, &_tDownScalerPrm[u4InstID]);
            //vVDECSetDownScalerPrm(u4InstID, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
    #endif
      if(_u4BSID[u4InstID] == 1)
      {
        UINT32 u4Bytes,u4Bits;
        VDEC_INFO_MPEG_BS_INIT_PRM_T rMpegBSInitPrm;

        vDEC_HAL_COMMON_SetVLDPower(u4InstID,ON);
        u4Bytes = u4VDEC_HAL_MPEG_ReadRdPtr(1, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
        rMpegBSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
        rMpegBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
        rMpegBSInitPrm.u4ReadPointer= (UINT32)_pucVFifo[u4InstID] + u4Bytes;
      #ifndef  RING_VFIFO_SUPPORT
        rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
      #else
//        rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
	rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));//mtk40343
      #endif
        i4VDEC_HAL_MPEG_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
        u4VDEC_HAL_MPEG_ShiftGetBitStream(0, u4InstID, u4Bits);
      }

      #if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580) || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
        vVDEC_HAL_MPEG2_DisableMVOverflowDetection(u4InstID);//add by mtk40343
        //UTIL_Printf("disable mv overflow\n");
      #endif
      i4VDEC_HAL_MPEG12_DecStart(u4InstID, &_tVerMpvDecPrm[u4InstID]);
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
    }
    else if(_u4CodecVer[u4InstID] == VDEC_DIVX3)
    {
    #ifdef REDEC
      _u4VLDPosByte[u4InstID] = u4VDEC_HAL_MPEG_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &_u4VLDPosBit[u4InstID]);
    #endif
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4BcodeSa = (UINT32)_pucMp4Bcode[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4Bmb1Sa = (UINT32)_pucMp4Bmb1[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4Bmb2Sa = (UINT32)_pucMp4Bmb2[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4DcacSa = (UINT32)_pucMp4Dcac[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4MvecSa = (UINT32)_pucMp4Mvec[u4InstID];

     #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSize.u4BcodeSize = BCODE_SZ;//count in 16 byte
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSize.u4DcacSize = DCAC_SZ;
     #endif

    #ifdef VERIFICATION_DOWN_SCALE
      #ifdef DOWN_SCALE_SUPPORT
            //vSetDownScaleParam(u4InstID, TRUE, &_tDownScalerPrm[u4InstID]);
            vSetDownScaleParam(u4InstID, TRUE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
      #else
            //vSetDownScaleParam(u4InstID, FALSE, &_tDownScalerPrm[u4InstID]);
            vSetDownScaleParam(u4InstID, FALSE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
      #endif
      //vDEC_HAL_COMMON_SetDownScaler(u4InstID, &_tDownScalerPrm[u4InstID]);
            //vVDECSetDownScalerPrm(u4InstID, &_tDownScalerPrm[u4InstID]);
            vVDECSetDownScalerPrm(u4InstID, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
    #endif
      if(_u4BSID[u4InstID] == 1)
      {
        UINT32 u4Bytes,u4Bits;
        VDEC_INFO_MPEG_BS_INIT_PRM_T rMpegBSInitPrm;

        vDEC_HAL_COMMON_SetVLDPower(u4InstID,ON);
        u4Bytes = u4VDEC_HAL_MPEG_ReadRdPtr(1, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
        rMpegBSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
        rMpegBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
        rMpegBSInitPrm.u4ReadPointer= (UINT32)_pucVFifo[u4InstID] + u4Bytes;
      #ifndef  RING_VFIFO_SUPPORT
        rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
      #else
	  //		rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
		  rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));//mtk40343
      #endif
        i4VDEC_HAL_MPEG4_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
        u4VDEC_HAL_MPEG_ShiftGetBitStream(0, u4InstID, u4Bits);
      }
      i4VDEC_HAL_DIVX3_DecStart(u4InstID, &_tVerMpvDecPrm[u4InstID]);
      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
    }
        else if(_u4CodecVer[u4InstID] == VDEC_MPEG4 || _u4CodecVer[u4InstID] == VDEC_H263)
    {
    #ifdef REDEC
      _u4VLDPosByte[u4InstID] = u4VDEC_HAL_MPEG_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &_u4VLDPosBit[u4InstID]);
    #endif
            //PANDA H263 Deblocking test
            _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.fgPpEnable = VDEC_PP_ENABLE;
            _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.u4PpYBufSa = (UINT32)_pucPpYSa[u4InstID];
            _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rMpegPpInfo.u4PpCBufSa = (UINT32)_pucPpCSa[u4InstID];
            //~PANDA
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rDep.rM4vDecPrm.prVol = &_rMPEG4VolPrm[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rDep.rM4vDecPrm.prVop = &_rMPEG4VopPrm[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rDep.rM4vDecPrm.prVop->prDirMd = &_rDirMode[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rDep.rM4vDecPrm.prVop->prGmcPrm = &_rMPEG4GmcPrm[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4BcodeSa = (UINT32)_pucMp4Bcode[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4Bmb1Sa = (UINT32)_pucMp4Bmb1[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4Bmb2Sa = (UINT32)_pucMp4Bmb2[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4DcacSa = (UINT32)_pucMp4Dcac[u4InstID];
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSa.u4MvecSa = (UINT32)_pucMp4Mvec[u4InstID];

      #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSize.u4BcodeSize = BCODE_SZ;//count in 16 byte
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSize.u4DcacSize = DCAC_SZ;
	  #ifdef MPEG4_NEW_MODE
	  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSize.u4MvecSize = VER_MVEC_SZ;
	  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSize.u4Bmb1Size = VER_BMB1_SZ;
	  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.rPicLayer.rMp4DecPrm.rMpeg4WorkBufSize.u4Bmb2Size = VER_BMB2_SZ;
	  #endif
      #endif

    #ifdef VERIFICATION_DOWN_SCALE
      #ifdef DOWN_SCALE_SUPPORT
            //vSetDownScaleParam(u4InstID, TRUE, &_tDownScalerPrm[u4InstID]);
            vSetDownScaleParam(u4InstID, TRUE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
      #else
            //vSetDownScaleParam(u4InstID, FALSE, &_tDownScalerPrm[u4InstID]);
            vSetDownScaleParam(u4InstID, FALSE, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
      #endif
      //vDEC_HAL_COMMON_SetDownScaler(u4InstID, &_tDownScalerPrm[u4InstID]);
            //vVDECSetDownScalerPrm(u4InstID, &_tDownScalerPrm[u4InstID]);
            vVDECSetDownScalerPrm(u4InstID, &(_tVerMpvDecPrm[u4InstID].rDownScalerPrm));
    #endif
      if(_u4BSID[u4InstID] == 1)
      {
        UINT32 u4Bytes,u4Bits;
        VDEC_INFO_MPEG_BS_INIT_PRM_T rMpegBSInitPrm;

        vDEC_HAL_COMMON_SetVLDPower(u4InstID,ON);
        u4Bytes = u4VDEC_HAL_MPEG_ReadRdPtr(1, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
        rMpegBSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
        rMpegBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
        rMpegBSInitPrm.u4ReadPointer= (UINT32)_pucVFifo[u4InstID] + u4Bytes;
      #ifndef  RING_VFIFO_SUPPORT
        rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
      #else
	  //		rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
		  rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));//mtk40343
      #endif
        i4VDEC_HAL_MPEG4_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
        u4VDEC_HAL_MPEG_ShiftGetBitStream(0, u4InstID, u4Bits);
      }
      if(0)//(_fgReInitBS)
      {
        UINT32 u4Bytes,u4Bits;
        VDEC_INFO_MPEG_BS_INIT_PRM_T rMpegBSInitPrm;

        vDEC_HAL_COMMON_SetVLDPower(u4InstID,ON);
        u4Bytes = u4VDEC_HAL_MPEG_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
        rMpegBSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
        rMpegBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
        rMpegBSInitPrm.u4ReadPointer= (UINT32)_pucVFifo[u4InstID] + u4Bytes;
      #ifndef  RING_VFIFO_SUPPORT
        rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
      #else
	  //		rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
		  rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));//mtk40343
      #endif
        i4VDEC_HAL_MPEG4_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
        u4VDEC_HAL_MPEG_ShiftGetBitStream(0, u4InstID, u4Bits);
      }
#ifdef VPMODE
      if(_fgVerVopCoded0[u4InstID])//qiguo
      {
	vVDEC_HAL_MPEG_SetMPEG4Flag(u4InstID, FALSE);
	_u4FileCnt[u4InstID] = _u4FileCnt[u4InstID]  -1;
      	i4VPModeDecStart(u4InstID, &_tVerMpvDecPrm[u4InstID]);
      }
      else
#endif
      {
      i4VDEC_HAL_MPEG4_DecStart(u4InstID, &_tVerMpvDecPrm[u4InstID]);
      }

      _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_DECODE;
    }
  }
}


void vVDecDumpLaeBuffer(UINT32 u4InstID)
{
    UINT32 u4temp,u4WriteSize;
    FILE *pFile;
    //Dump buffer
    UTIL_Printf("Dump LAE buffer for check!\n");
    UCHAR fpDumpCabacFile[200] = "d:\\ChkFolder\\LAE_";
    UCHAR fpDumpEcFile[200] = "d:\\ChkFolder\\EC_";
    //char pBitstream_name[200] = {0};
    
    //memcpy (pBitstream_name , _bFileStr1[u4InstID][1]+17 , (strlen(_bFileStr1[u4InstID][1]) - 47) );
    //pBitstream_name[(strlen(_bFileStr1[u4InstID][1]) - 47)] = '\0';
    
    printk("vVDecDumpLaeBuffer %s\n",_bFileStr1[u4InstID][1]);
    u4temp = strlen(fpDumpCabacFile);
    //u4temp += sprintf(fpDumpCabacFile+u4temp,"%s_",pBitstream_name);
	u4temp += sprintf(fpDumpCabacFile+u4temp,"%d",_u4LaeDecCnt[u4InstID]);
	u4temp += sprintf(fpDumpCabacFile+u4temp,"%s",".lae");

	u4temp = strlen(fpDumpEcFile);
	//u4temp += sprintf(fpDumpEcFile+u4temp,"%s_",pBitstream_name);
	u4temp += sprintf(fpDumpEcFile+u4temp,"%d",_u4LaeDecCnt[u4InstID]);
	u4temp += sprintf(fpDumpEcFile+u4temp,"%s",".ec");
	
	UTIL_Printf("Dump lae cabac file %s\n",fpDumpCabacFile);
	UTIL_Printf("Dump lae ec file %s\n",fpDumpEcFile);

	pFile = fopen(fpDumpCabacFile,"wb");
	if(pFile == NULL)
	{
		UTIL_Printf("Create file error !\n");
	}
	u4WriteSize = fwrite ((char* )(_pucBSCABACBuff[u4InstID]), 1, CABAC_SZ, pFile);
    fclose(pFile);   

    UTIL_Printf("Dump LAE CABAC buffer done!\n");
    
	pFile = fopen(fpDumpEcFile,"wb");
	if(pFile == NULL)
	{
		UTIL_Printf("Create file error !\n");
	}
	
	u4WriteSize = fwrite ((char* )(_pucECInfoBuff[u4InstID]), 1, EC_SZ, pFile);
    fclose(pFile); 
    UTIL_Printf("Dump LAE ErrorInfo buffer done!\n");
}


void PostAdjustReconRange(UINT32 u4InstID)
{
  VDEC_INFO_WMV_SEQ_PRM_T *prWMVSPS = &_rWMVSPS[u4InstID];
  VDEC_INFO_WMV_ETRY_PRM_T *prWMVEPS = &_rWMVEPS[u4InstID];
  VDEC_INFO_WMV_PIC_PRM_T *prWMVPPS = &_rWMVPPS[u4InstID];

  if(prWMVSPS->fgPreProcRange)
  {
    if(prWMVPPS->ucPicType == IVOP || prWMVPPS->ucPicType == BIVOP)
    {
      if(prWMVSPS->i4NumBFrames == 0)
      {
        prWMVEPS->i4ReconRangeState = prWMVEPS->i4RangeState;
      }
      else if(prWMVPPS->ucPicType != BVOP)
      {
        AdjustReconRange(u4InstID);
      }
    }
  }
}

void vVerifySetVSyncPrmBufPtr(UINT32 u4InstID, UINT32 u4BufIdx)
{
  switch(u4BufIdx)
  {
    case 0:
      _pucDecWorkBuf[u4InstID] = (UCHAR *) _pucPic0Y[_u1AlphaDecPrmIdx[u4InstID]];
      _pucDecCWorkBuf[u4InstID] = (UCHAR *) _pucPic0C[_u1AlphaDecPrmIdx[u4InstID]];
      break;
    case 1:
      _pucDecWorkBuf[u4InstID] = (UCHAR *) _pucPic1Y[_u1AlphaDecPrmIdx[u4InstID]];
      _pucDecCWorkBuf[u4InstID] = (UCHAR *) _pucPic1C[_u1AlphaDecPrmIdx[u4InstID]];
      break;
    case 2:
      _pucDecWorkBuf[u4InstID] = (UCHAR *) _pucPic2Y[_u1AlphaDecPrmIdx[u4InstID]];
      _pucDecCWorkBuf[u4InstID] = (UCHAR *) _pucPic2C[_u1AlphaDecPrmIdx[u4InstID]];
      break;
  }

  UTIL_Printf("[MPEG] Inst%d, vVerifySetVSyncPrmBufPtr, DecPrmIdx:%d, YWorkBuf:0x%x, CWorkBuf:0x%x\n", 
      u4InstID, _u1AlphaDecPrmIdx[u4InstID], _pucDecWorkBuf[u4InstID], _pucDecCWorkBuf[u4InstID]); 
}

void vWMVVDecEnd(UINT32 u4InstID)
{
  //VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm;
  //VDEC_INFO_H264_FBUF_INFO_T *tFBufInfo;
  VDEC_INFO_WMV_VFIFO_PRM_T rWmvVFifoInitPrm;
  VDEC_INFO_WMV_BS_INIT_PRM_T rWmvBSInitPrm;
  UINT32 u4VldByte,u4VldBit;
  VDEC_INFO_WMV_PIC_PRM_T *prWMVPPS = &_rWMVPPS[u4InstID];

  //tFBufInfo = _ptCurrFBufInfo[u4InstID];
  //tVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];

  u4VldByte = u4VDEC_HAL_WMV_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
  _u4WMVByteCount[u4InstID] = u4VldByte;

  #ifdef SEMI_RING_FIFO
  if(_i4CodecVersion[u4InstID] != VDEC_VC1)
      _tInFileInfo[u4InstID].u4BSLenConsumed = _iSetPos[u4InstID];
  else
      _tInFileInfo[u4InstID].u4BSLenConsumed = _u4WMVByteCount[u4InstID];//xiaolei.li's  
  #endif
#ifdef LETTERBOX_DETECTION_ONLY
  vCheckLBDResult(u4InstID);
#else
  vWMVWrData2PC(u4InstID, _pucDumpYBuf[u4InstID], ((((_tVerPic[u4InstID].u4W + 15) >> 4) * ((_tVerPic[u4InstID].u4H + 31) >> 5)) << 9));
#endif

  // reset HW
#ifdef REDEC
  if(_u4ReDecCnt[u4InstID] > 0)
  {
    _u4WMVDecPicNo[u4InstID]--;
    rWmvVFifoInitPrm.u4CodeType = _i4CodecVersion[u4InstID];
    rWmvVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rWmvVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    i4VDEC_HAL_WMV_InitVDecHW(u4InstID,&rWmvVFifoInitPrm);
    rWmvBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    rWmvBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rWmvBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID] + _u4VLDPosByte[u4InstID];
  #ifndef  RING_VFIFO_SUPPORT
    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  #else
//    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
  #endif
    if (_i4CodecVersion[u4InstID] == VDEC_VC1)
    {
        i4VDEC_HAL_WMV_InitBarrelShifter(0, u4InstID, &rWmvBSInitPrm, TRUE);
    }
    else
    {
       i4VDEC_HAL_WMV_InitBarrelShifter(0, u4InstID, &rWmvBSInitPrm, FALSE);
    }
    u4VDEC_HAL_WMV_ShiftGetBitStream(0, u4InstID, _u4VLDPosBit[u4InstID]);
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
    return;
  }
#endif

  //ming modify@2006/4/12
  if(prWMVPPS->ucFrameCodingMode == INTERLACEFIELD)
  {
    prWMVPPS->i4CurrentTemporalField ^= 1; //toggle field
     prWMVPPS->i4CurrentField ^= 1;
  }

  if(_i4CodecVersion[u4InstID] != VDEC_VC1)
  {
    rWmvVFifoInitPrm.u4CodeType = _i4CodecVersion[u4InstID];
    rWmvVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rWmvVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    i4VDEC_HAL_WMV_InitVDecHW(u4InstID,&rWmvVFifoInitPrm);
    if(_iSetPos[u4InstID] >= V_FIFO_SZ)
    {
      _iSetPos[u4InstID] = _iSetPos[u4InstID] - V_FIFO_SZ;
    }
    rWmvBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    rWmvBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rWmvBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID] + _iSetPos[u4InstID];
  #ifndef  RING_VFIFO_SUPPORT
    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  #else
//    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
  #endif
    i4VDEC_HAL_WMV_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rWmvBSInitPrm, FALSE);
  }
  else // WMVA
  {
    rWmvVFifoInitPrm.u4CodeType = _i4CodecVersion[u4InstID];
    rWmvVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rWmvVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    i4VDEC_HAL_WMV_InitVDecHW(u4InstID,&rWmvVFifoInitPrm);
    rWmvBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    rWmvBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rWmvBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID] + u4VldByte;
  #ifndef  RING_VFIFO_SUPPORT
    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  #else
//    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
  #endif
    i4VDEC_HAL_WMV_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rWmvBSInitPrm, TRUE);
  }
  if(_rWMVPPS[u4InstID].ucPicType != SKIPFRAME)
  {
    //update _iReconRangeState
    PostAdjustReconRange(u4InstID);
  }
#ifndef INTERGRATION_WITH_DEMUX
#ifdef  RING_VFIFO_SUPPORT
  if((_u4LoadBitstreamCnt[u4InstID]&0x1) && (rWmvBSInitPrm.u4ReadPointer >
  ((UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ/2))))
  {
    _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
    _tInFileInfo[u4InstID].pucTargetAddr = _pucVFifo[u4InstID];
    _tInFileInfo[u4InstID].u4FileOffset = (V_FIFO_SZ * ((_u4LoadBitstreamCnt[u4InstID]+ 1)/2));
    _tInFileInfo[u4InstID].u4TargetSz = (V_FIFO_SZ/2);
    _tInFileInfo[u4InstID].u4FileLength = 0;
	#ifdef SEMI_RING_FIFO
	_tInFileInfo[u4InstID].u4BSLenOffset = 0;
	_tInFileInfo[u4InstID].ucVFIFOEnough = 1;
	#endif
	UTIL_Printf("--u4FileOffset:%d, _u4LoadBitstreamCnt[u4InstID]:%d, ReadPointer:0x%x\n",	_tInFileInfo[u4InstID].u4FileOffset, _u4LoadBitstreamCnt[u4InstID], rWmvBSInitPrm.u4ReadPointer);
  #ifdef  SATA_HDD_READ_SUPPORT
    if(!fgOpenHDDFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]))
    {
      fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
    }
  #else
    fgOpenFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
  #endif
    _u4LoadBitstreamCnt[u4InstID]++;
  }
  else if((!(_u4LoadBitstreamCnt[u4InstID]&0x1)) && (rWmvBSInitPrm.u4ReadPointer <
  ((UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ/2))))
  {
    _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
    _tInFileInfo[u4InstID].pucTargetAddr = _pucVFifo[u4InstID] + (V_FIFO_SZ/2);
    _tInFileInfo[u4InstID].u4FileOffset =  ((V_FIFO_SZ * (_u4LoadBitstreamCnt[u4InstID]+ 1)) /2);
    _tInFileInfo[u4InstID].u4TargetSz = (V_FIFO_SZ/2);
    _tInFileInfo[u4InstID].u4FileLength = 0;
	UTIL_Printf("u4FileOffset:%d, _u4LoadBitstreamCnt[u4InstID]:%d, ReadPointer:0x%x\n",  _tInFileInfo[u4InstID].u4FileOffset, _u4LoadBitstreamCnt[u4InstID], rWmvBSInitPrm.u4ReadPointer);
	#ifdef SEMI_RING_FIFO
	_tInFileInfo[u4InstID].u4BSLenOffset = 0;
	_tInFileInfo[u4InstID].ucVFIFOEnough = 1;
	#endif
  #ifdef  SATA_HDD_READ_SUPPORT
    if(!fgOpenHDDFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]))
    {
      fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
    }
  #else
    fgOpenFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
  #endif
    _u4LoadBitstreamCnt[u4InstID]++;
  }
  #endif
  #endif
  _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;
}

// *********************************************************************
// Function    : BOOL fgIsWMVVDecComplete(UINT32 u4InstID)
// Description : Check if VDec complete with interrupt
// Parameter   : None
// Return      : None
// *********************************************************************
BOOL fgIsWMVVDecComplete(UINT32 u4InstID)
{
  UINT32 u4MbX;
  UINT32 u4MbY;
  VDEC_INFO_WMV_SEQ_PRM_T *prWMVSPS = &_rWMVSPS[u4InstID];
  VDEC_INFO_WMV_PIC_PRM_T *prWMVPPS = &_rWMVPPS[u4InstID];


  if(_fgVDecComplete[u4InstID])
  {
#if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
    HAL_GetDeltaTime(&rTimeDecDuration[u4InstID],&rTimeStartDec[u4InstID],&rTimeEndDec[u4InstID]);
    UTIL_Printf("Delta_%d:second:ms %08d:%08d\n",u4InstID,rTimeDecDuration[u4InstID].u4Seconds,rTimeDecDuration[u4InstID].u4Micros);
#endif    
#if PERFORMANCE_TEST
extern void v4VDEC_Dump_Performance_Reg();
    v4VDEC_Dump_Performance_Reg();
#endif

    vVDEC_HAL_WMV_GetMbxMby(u4InstID, &u4MbX, &u4MbY);
    if(prWMVPPS->ucFrameCodingMode != INTERLACEFIELD)
    {
      if(u4MbX < ((prWMVSPS->u4PicWidthDec >> 4) -1) || (u4MbY < ((prWMVSPS->u4PicHeightDec >> 4) -1)))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    }
    else
    {
      if(u4MbX < ((prWMVSPS->u4PicWidthDec >> 4) -1) || u4MbY < ((prWMVSPS->u4PicHeightDec >> 5) -1))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

void vWMVDecEndProc(UINT32 u4InstID)
{
  UINT32 u4Cnt;
  UINT32 u4CntTimeChk;
  UINT32 u4MbX;
  UINT32 u4MbY;
  char strMessage[256];
  UINT32 u4MbX_last;
  UINT32 u4MbY_last;
  UINT32 u4mvErrType;
  VDEC_INFO_WMV_ERR_INFO_T prWmvErrInfo;

  u4Cnt=0;
  u4CntTimeChk = 0;
  _fgVDecErr[u4InstID] = FALSE;
  if(_rWMVPPS[u4InstID].ucPicType != SKIPFRAME)
  {
    while(u4CntTimeChk < DEC_RETRY_NUM)
    {
      u4Cnt ++;
      if((u4Cnt & 0x3f)== 0x3f)
      {
  #ifdef DEC_END_POLLING
  if(u4VDEC_HAL_WMV_VDec_ReadFinishFlag(u4InstID) & 0x1)
  	  _fgVDecComplete[u4InstID] = TRUE;
  #else
  #ifndef IRQ_DISABLE
  #else
        if(u4VDEC_HAL_WMV_VDec_ReadFinishFlag(u4InstID) & 0x1)
        {
          _fgVDecComplete[u4InstID] = TRUE;
          if(u4InstID == 0)
          {
            BIM_ClearIrq(VECTOR_VDFUL);
          }
          else
          {
            BIM_ClearIrq(VECTOR_VDLIT);
          }
        }
  #endif
  #endif
        if(fgIsWMVVDecComplete(u4InstID))
        {
          u4CntTimeChk = 0;
		  UTIL_Printf("Dram Clock:0x%x, Vdec Clock:0x%x\n", u4VDecReadMC(u4InstID, 0x778), u4VDecReadMC(u4InstID, 0x9e0));
          break;
        }
        else
        {
          u4MbX_last = u4MbX;
          u4MbY_last = u4MbY;
          vVDEC_HAL_WMV_GetMbxMby(u4InstID, &u4MbX, &u4MbY);
          if((u4MbX == u4MbX_last) && (u4MbY == u4MbY_last))
          {
            u4CntTimeChk ++;
          }
          else
          {
            u4CntTimeChk =0;
          }
        }
        u4Cnt = 0;
      }
    }

	UTIL_Printf("\n Decode Finish, prepare to compare \n");
    u4mvErrType = u4VDEC_HAL_WMV_GetErrType(u4InstID);
    vVDEC_HAL_WMV_GetErrInfo(u4InstID, &prWmvErrInfo);
    if((u4CntTimeChk == DEC_RETRY_NUM) ||
      //(u4mvErrType!= 0) || (prWmvErrInfo.u4WmvErrCnt != 0))
      (prWmvErrInfo.u4WmvErrType != 0) || (prWmvErrInfo.u4WmvErrCnt != 0)) // vld_162 is wmv error type reg. xiaolei.li for mustang emulation
    {
    #ifndef INTERGRATION_WITH_DEMUX
    #ifdef EXT_COMPARE
      _fgVDecErr[u4InstID] = TRUE;
    #endif
      if(u4CntTimeChk == DEC_RETRY_NUM)
      {
        vVDecOutputDebugString("\n!!!!!!!!! Decoding Timeout !!!!!!!\n");
        sprintf(strMessage, "%s", "\n!!!!!!!!! Decoding Timeout !!!!!!!");
		UTIL_Printf("!!!!!!!!!!!Decode Timeout!!!!!!!!!!!!!\n");
        //fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
        //vVDEC_HAL_WMV_VDec_DumpReg(u4InstID, FALSE); //sun for temp
      }
      vVDEC_HAL_WMV_GetMbxMby(u4InstID, &u4MbX, &u4MbY);
      vVDecOutputDebugString("\n!!!!!!!!! Decoding Error 0x%.8x!!!!!!!\n", prWmvErrInfo.u4WmvErrType);
      sprintf(strMessage,"\n!!!!!!!!! Decoding Error 0x%.8x at MC (x,y)=(%d/%d, %d/%d)  !!!!!!!\n", prWmvErrInfo.u4WmvErrType /*u4mvErrType*/,
                u4MbX, ((_tVerPic[u4InstID].u4W + 15)>> 4) - 1, u4MbY, (((_tVerPic[u4InstID].u4H >> (1-(fgWMVIsFrmPic(u4InstID)))) + 15)>> 4) - 1);
	  UTIL_Printf("!!!!!!!!! Decoding Error 0x%.8x at MC (x,y)=(%d/%d, %d/%d)  !!!!!!!\n",prWmvErrInfo.u4WmvErrType /*u4mvErrType*/,
                u4MbX, ((_tVerPic[u4InstID].u4W + 15)>> 4) - 1, u4MbY, (((_tVerPic[u4InstID].u4H >> (1-(fgWMVIsFrmPic(u4InstID)))) + 15)>> 4) - 1);
	  vVDEC_HAL_WMV_GetRealMby(u4InstID,&u4MbY);	
	  UTIL_Printf("Real MBY = %d\n",u4MbY);
	  //fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
      sprintf(strMessage,"the length is %d (0x%.8x)\n", _tInFileInfo[u4InstID].u4FileLength, _tInFileInfo[u4InstID].u4FileLength);
      //fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
      vReadWMVChkSumGolden(u4InstID);
      //vWrite2PC(u4InstID, 1, _pucVFifo[u4InstID]);
      vVDEC_HAL_WMV_VDec_DumpReg(u4InstID, FALSE); //sun for temp
    #endif
    }
#if BANDWIDTH_MEASURE
		vVDEC_HAL_BANDWID_MEAS_DumpReg(u4InstID);
#endif
#if PARA8_BEHAVIOR_TEST
	vVDEC_HAL_PARA8_BEHAVIOR_DumpReg(u4InstID);
#endif
    vVDEC_HAL_WMV_DecEndProcess(u4InstID);
    vVDEC_HAL_WMV_AlignRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], BYTE_ALIGN);
    vVerifySetVSyncPrmBufPtr(u4InstID, _u4DecBufIdx[u4InstID]);
    vReadWMVChkSumGolden(u4InstID);
	//vVDecBufferFlush(u4InstID,_u4DecBufIdx[u4InstID]);
	HalFlushInvalidateDCache();
  }


  #if VDEC_DRAM_BUSY_TEST
       vDrmaBusyOff (u4InstID);
  #endif

#if 0
   BSP_dma_unmap_single(_tBufPVAddrInfo[u4InstID].u4PhypucDecWorkBuf, PIC_Y_SZ, FROM_DEVICE);
   BSP_dma_unmap_single(_tBufPVAddrInfo[u4InstID].u4PhypucDecWorkCBuf, PIC_C_SZ, FROM_DEVICE);
#else
  BSP_dma_unmap_single(_tBufPVAddrInfo[u4InstID].u4PhyPucPic0Y, PIC_Y_SZ, FROM_DEVICE);
  BSP_dma_unmap_single(_tBufPVAddrInfo[u4InstID].u4PhyPucPic0C, PIC_C_SZ, FROM_DEVICE);
  BSP_dma_unmap_single(_tBufPVAddrInfo[u4InstID].u4PhyPucPic1Y, PIC_Y_SZ, FROM_DEVICE);
  BSP_dma_unmap_single( _tBufPVAddrInfo[u4InstID].u4PhyPucPic1C, PIC_C_SZ, FROM_DEVICE);
  BSP_dma_unmap_single(_tBufPVAddrInfo[u4InstID].u4PhyPucPic2Y, PIC_Y_SZ, FROM_DEVICE);
  BSP_dma_unmap_single(_tBufPVAddrInfo[u4InstID].u4PhyPucPic2C, PIC_C_SZ, FROM_DEVICE);
#endif

 if(_u4FileCnt[u4InstID] == _u4DumpRegPicNum[u4InstID])
   {
      vVDEC_HAL_WMV_VDec_DumpReg(u4InstID, FALSE);
    }

#if 0//(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
  UINT32 u4MbnReg474, u4MbnReg476, u4MbnReg477;
  u4MbnReg474 = u4VDecReadMC(u4InstID, (474<<2));
  u4MbnReg476 = u4VDecReadMC(u4InstID, (476<<2));
  u4MbnReg477 = u4VDecReadMC(u4InstID, (477<<2));

  sprintf(strMessage, "\nMBN LOG_474 = 0x%.8x!!!!!!!\n", u4MbnReg474);
  fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
  sprintf(strMessage, "\nMBN LOG_476 = 0x%.8x!!!!!!!\n", u4MbnReg476);
  fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
  sprintf(strMessage, "\nMBN LOG_477 = 0x%.8x!!!!!!!\n", u4MbnReg477);
  fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
#endif

  vWMVVDecEnd(u4InstID);
}

UINT32 gu4MCCycle[2] = {0,0};
UINT32 gu4VLDCycle[2] = {0,0};
extern UINT32 _u4H265Cnt[2];
void vH265DecEndProc(UINT32 u4InstID, unsigned long  start_time)
{
    UINT32 u4Cnt = 0;
    UINT32 u4MCcnt = 0,u4Clk;
    UINT32 u4CntTimeChk,u4ErrorType;
    UINT32 u4Bits;
    UINT32 u4RetryDelay = 10;
    UINT32 u4RetRegValue = 0;
    BOOL bIsTimeOut = 0,fgDecDone = 0;
    char bitstream_name[200] = {0};

    memcpy (bitstream_name , _bFileStr1[u4InstID][1]+12 , (strlen(_bFileStr1[u4InstID][1]) -38) );
    bitstream_name[(strlen(_bFileStr1[u4InstID][1]) -38)] = '\0';

    u4Cnt=0;
    u4CntTimeChk = 0;

    if ( _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bNoDecode )
    {
      printk("[INFO] Syntax Error No Decode!!\n");
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bNoDecode = 0;
    }
    else
    {

#ifndef IRQ_DISABLE
        while((u4RetryDelay > 0) && (_fgVDecComplete[u4InstID] == FALSE))
        {
            x_thread_delay(40);
            u4RetryDelay --;
        }
        u4ErrorType = u4VDEC_HAL_H265_GetErrMsg(u4InstID);

        if((u4ErrorType != 0) && (_u4DualCoreEnable[u4InstID] == 0))
        {
            UTIL_Printf("HEVC decode error 0x%08x !!!!!!!!!!!!!!!!!!!!!!!!!\n",u4ErrorType);
        }

#if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
        HAL_GetDeltaTime(&rTimeDecDuration[u4InstID],&rTimeStartDec[u4InstID],&rTimeEndDec[u4InstID]);
        //      UTIL_Printf("Start[%d.%06d] -- End[%d.%06d]\n",rTimeStartDec[u4InstID].u4Seconds,rTimeStartDec[u4InstID].u4Micros,
        //                                                    rTimeEndDec[u4InstID].u4Seconds,rTimeEndDec[u4InstID].u4Micros);
        u4Cnt = u4VDecReadVLDTOP(u4InstID,(40 << 2));
        u4MCcnt = u4VDecReadMC(u4InstID,(478 << 2));
        //fast clk = 504M; mid clk = 432M
        u4Clk = (u4ReadReg(CKGEN_VDEC_SYS_CFG) == 0xE) ? 504:432;
        UTIL_Printf("decode time %08d VS hw %08d VLD40, %d VLD40_IRQ %d MC_478, %d MC_478_IRQ, %d @ %d\n",(((rTimeDecDuration[u4InstID].u4Seconds) * 1000) + rTimeDecDuration[u4InstID].u4Micros),
                                                     (u4Cnt/u4Clk),u4Cnt,gu4VLDCycle[u4InstID],u4MCcnt,gu4MCCycle[u4InstID],_u4H265Cnt[u4InstID]);
#endif
#else
        if(_u4DualCoreEnable[u4InstID])
        {
            fgDecDone = u4VDEC_HAL_MCORE_VDec_ReadFinishFlag(u4InstID);
        }
        else
        {
            fgDecDone = u4VDEC_HAL_H265_VDec_ReadFinishFlag(u4InstID);
        }

        if(fgDecDone)
        {
          _fgVDecComplete[u4InstID] = TRUE;
        }
            
        while((fgDecDone != 1) && (u4RetryDelay > 0))
        {
            if(_u4DualCoreEnable[u4InstID])
            {
                fgDecDone = u4VDEC_HAL_MCORE_VDec_ReadFinishFlag(u4InstID);
            }
            else
            {
                fgDecDone = u4VDEC_HAL_H265_VDec_ReadFinishFlag(u4InstID);
                if(fgDecDone)
                {
                    _fgVDecComplete[u4InstID] = TRUE;
                    break;
                }
            }
            u4RetryDelay--;
            x_thread_delay(500);
        }
#endif
      
      if(u4RetryDelay == 0 || (_fgVDecComplete[u4InstID] == FALSE))
      {
        printk("\n!!!!!!Decode Polling int timeout 0x%08x!!!!!!\n\n",u4VDecReadMCORETOP(u4InstID, 0xC) );
        vVDEC_HAL_H265_VDec_DumpReg(u4InstID, 1);
      }
      #ifdef VDEC_SIM_DUMP
      printk("[INFO] Decode done!!\n");
      #endif

      if (!bIsTimeOut) 
	  {
      	#ifdef HEVC_ESA_NBM_LOG
          vH265Dump_ESA_NBM_performane_log( u4InstID, bitstream_name, _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.bIsUFOMode);
      	#endif
      }

        #if DEBUG_LEVEL >= DBG_LEVEL_INFO
        printk("decode end @ 0x%08x\n",u4VDEC_HAL_H265_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits));
        #endif
      
      u4VDEC_HAL_H265_VDec_ClearInt(u4InstID);

      _rH265PicInfo[u4InstID].u4SliceCnt = 0;
      #ifdef VDEC_SIM_DUMP
          printk("[INFO] Update Current (pic_cnt=%d POC=%d) FB index %d\n", _u4PicCnt[u4InstID],_rH265SliceHdr[u4InstID].i4POC, _tVerMpvDecPrm[u4InstID].ucDecFBufIdx );
      #endif

      _ptH265CurrFBufInfo[u4InstID]->u4PicCnt = _u4PicCnt[u4InstID];
      _ptH265CurrFBufInfo[u4InstID]->i4POC = _rH265SliceHdr[u4InstID].i4POC;

      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.rLastInfo.u4LastPicW = _ptH265CurrFBufInfo[u4InstID]->u4W;
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.rLastInfo.u4LastPicH =  _ptH265CurrFBufInfo[u4InstID]->u4H;    
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.rLastInfo.u4LastPicWidthInLumaSamples = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.prSPS->u4PicWidthInLumaSamples;
      _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.rLastInfo.u4LastPicHeightInLumaSamples = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.prSPS->u4PicHeightInLumaSamples;

      _ptH265CurrFBufInfo[u4InstID]->ucFBufStatus = FRAME;
      _ptH265CurrFBufInfo[u4InstID]->bFirstSliceReferenced = 1;
      
        if ( bIsTimeOut )
        {
            vVDEC_HAL_H265_VDec_DumpReg(u4InstID, 1);
            _u4VerBitCount[u4InstID] = 0xFFFFFFFF;
        } 

    }

    vH265VDecEnd(u4InstID);  
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.ucPreFBIndex = _tVerMpvDecPrm[u4InstID].ucDecFBufIdx;
}


#define ChunChia_LOG 0
extern void vVDEC_HAL_PARA8_BEHAVIOR_DumpReg(UINT32 u4VDecID);
void vH264DecEndProc(UINT32 u4InstID)
{
	UINT32 u4Cnt;
  	UINT32 u4CntTimeChk;
  	//BOOL fgWaitChk;
	UINT32 u4Temp = 0;
  	UINT32 u4MbX;
  	UINT32 u4MbY; 

  	UINT32 u4MbX_last;
  	UINT32 u4MbY_last;
	
  	char strMessage[256];

#if ChunChia_LOG
  	UINT32 u4Mc770, u4Mc774, u4Mc778, u4Mc8B8;
#endif
	u4Temp = 1 + _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgFieldPicFlag;
	if (1 == _u4DualCoreEnable[0])
	{
		if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->fgMbAdaptiveFrameFieldFlag
			&& (!_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgFieldPicFlag))
		{
			if (0 == ((_tVerMpvDecPrm[u4InstID].u4PicH/32) % 2))
			{
				vVDEC_HAL_H264_GetMbxMby(VDEC_VLD1, &u4MbX, &u4MbY);
			}
			else
			{
				vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
			}
		}
		else
		{
			if (0 == ((_tVerMpvDecPrm[u4InstID].u4PicH/u4Temp/16) % 2))
			{
				vVDEC_HAL_H264_GetMbxMby(VDEC_VLD1, &u4MbX, &u4MbY);
			}
			else
			{
				vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
			}
		}
	}
	else
	{
  		vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
	}
  	_fgVDecErr[u4InstID] = FALSE;

  	u4Cnt=0;
  	u4CntTimeChk = 0;
  	while(u4CntTimeChk < DEC_RETRY_NUM)
  	{
    	u4Cnt ++;
    	if((u4Cnt & 0x3f)== 0x3f)
    	{
      	#ifndef IRQ_DISABLE
      	#else
	  		if (1 == _u4DualCoreEnable[u4InstID])
	  		{
			  	if (u4VDEC_HAL_MCORE_VDec_ReadFinishFlag(u4InstID))
			  	{
			  		_fgVDecComplete[u4InstID] = TRUE;
			        vVDecWriteMCORETOP(u4InstID, RO_LAE_STATUS,(u4VDecReadMCORETOP(u4InstID, RO_LAE_STATUS) & (~(1 << 16))));
			  	}
	  		}
      		else 
      		{
	  			if(u4VDEC_HAL_H264_VDec_ReadFinishFlag(u4InstID))
      			{
        			_fgVDecComplete[u4InstID] = TRUE;
        			if(u4InstID == 0)
        			{
          				BIM_ClearIrq(VECTOR_VDFUL);
        			}
        			else
        			{
          				BIM_ClearIrq(VECTOR_VDLIT);
        			}
      			}
      		}
      	#endif

      		if(fgIsH264VDecComplete(u4InstID))
      		{
        		u4CntTimeChk = 0;
        		break;
      		}
	      	else
	      	{
	        	u4MbX_last = u4MbX;
	        	u4MbY_last = u4MbY;
	        	if (1 == _u4DualCoreEnable[0])
				{
					if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->fgMbAdaptiveFrameFieldFlag
					&& (!_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgFieldPicFlag))
					{
						if (0 == ((_tVerMpvDecPrm[u4InstID].u4PicH/32) % 2))
						{
							vVDEC_HAL_H264_GetMbxMby(VDEC_VLD1, &u4MbX, &u4MbY);
						}
						else
						{
							vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
						}
					}
					else
					{
						if (0 == ((_tVerMpvDecPrm[u4InstID].u4PicH/u4Temp/16) % 2))
						{
							vVDEC_HAL_H264_GetMbxMby(VDEC_VLD1, &u4MbX, &u4MbY);
						}
						else
						{
							vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
						}
					}
				}
				else
				{
			  		vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
				}
	        		//vVDecOutputDebugString("\nMbX = %d, MbY = %d\n", u4MbX,u4MbY);
	        	if((u4MbX == u4MbX_last) && (u4MbY == u4MbY_last))
	        	{
	          		u4CntTimeChk ++;
	        	}
	        	else
	        	{
	          		u4CntTimeChk =0;
	        	}
			}
      		u4Cnt = 0;
    	}
  	}
  
    #if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
    {
        //Get end timer should be in IRQ,or residual offset
        //HAL_GetTime(&rTimeEndDec[u4InstID]);
        //UTIL_Printf("second:ms %08d:%08d\n",rTimeEndDec[u4InstID].u4Seconds,rTimeEndDec[u4InstID].u4Micros);
        if(_ucMVCType[u4InstID] == 0)//only base view
        {
            HAL_GetDeltaTime(&rTimeDecDuration[u4InstID],&rTimeStartDec[u4InstID],&rTimeEndDec[u4InstID]);
            switch(_rH264SliceHdr[u4InstID].u4SliceType)
            {
                case 2:
                case 7:
                case 4:
                case 9:
                    u8VdecFrameTypeDuration[0] += (((rTimeDecDuration[u4InstID].u4Seconds) * 1000) + rTimeDecDuration[u4InstID].u4Micros)/100;
                    u8VdecFrameTypeCount[0] ++;
                break;
                case 0:
                case 3:
                case 5:
                case 8:
                    u8VdecFrameTypeDuration[1] += (((rTimeDecDuration[u4InstID].u4Seconds) * 1000) + rTimeDecDuration[u4InstID].u4Micros)/100;
                    u8VdecFrameTypeCount[1] ++;
                break;
                case 1:
                case 6:
                    u8VdecFrameTypeDuration[2] += (((rTimeDecDuration[u4InstID].u4Seconds) * 1000) + rTimeDecDuration[u4InstID].u4Micros)/100;
                    u8VdecFrameTypeCount[2] ++;
                break;
            }
        }
        //UTIL_Printf("delta time = 0x%d(unit : 0.1ms)\n",(((rTimeDecDuration[u4InstID].u4Seconds) * 1000) + rTimeDecDuration[u4InstID].u4Micros)/100);
    }
    #endif

  	if (DEC_RETRY_NUM <= u4CntTimeChk)
  	{
    	UTIL_Printf("[H264] vH264DecEndProc, time out!!");
		//vVDecDumpLaeBuffer(u4InstID);
  	}
  	//vVDecOutputDebugString("\n{H264 DEC} u4InstID = 0x%d, end decode\n", u4InstID);

	  #if 0 // close for test, tmp
	  vVDEC_HAL_H264_VDec_PowerDown(u4InstID);
	  #endif

  if((u4CntTimeChk == DEC_RETRY_NUM) ||
    ((u4VDEC_HAL_H264_GetErrMsg(u4InstID) != 0)
      && (!(((u4VDEC_HAL_H264_GetErrMsg(u4InstID) == 8) || (u4VDEC_HAL_H264_GetErrMsg(u4InstID) == 0x40)) && (fgVDEC_HAL_H264_DecPicComplete(u4InstID))))))
  {
#ifndef INTERGRATION_WITH_DEMUX
#ifdef EXT_COMPARE
    _fgVDecErr[u4InstID] = TRUE;
#endif
    if(u4CntTimeChk == DEC_RETRY_NUM)
    {
      vVDecOutputDebugString("\n!!!!!!!!! Decoding Timeout !!!!!!!\n");
      sprintf(strMessage, "%s", "\n!!!!!!!!! Decoding Timeout !!!!!!!");
      fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
      //vVDEC_HAL_H264_VDec_DumpReg(u4InstID);
    }
    vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
    vVDecOutputDebugString("\n!!!!!!!!! Decoding Error 0x%.8x in pic %d (frm %d) !!!!!!!\n", u4VDEC_HAL_H264_GetErrMsg(u4InstID), _u4PicCnt[u4InstID], _u4FileCnt[u4InstID]);
    sprintf(strMessage,"\n!!!!!!!!! Decoding Error 0x%.8x at MC (x,y)=(%d/%d, %d/%d) in pic %d (frm %d) !!!!!!!\n", u4VDEC_HAL_H264_GetErrMsg(u4InstID),
              u4MbX, ((_ptCurrFBufInfo[u4InstID]->u4W + 15)>> 4) - 1, u4MbY, (((_ptCurrFBufInfo[u4InstID]->u4H >> (1-(fgIsFrmPic(u4InstID)))) + 15)>> 4) - 1, _u4PicCnt[u4InstID], _u4FileCnt[u4InstID]);
    fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
    //sprintf(strMessage,"the length is %d (0x%.8x)\n", _tInFileInfo[u4InstID].u4FileLength, _tInFileInfo[u4InstID].u4FileLength);
    //fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
    #if 1
    {
        UINT32 u4FileOffset,u4Bits;
        u4FileOffset = u4VDEC_HAL_H264_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
        UTIL_Printf("HW ReadPointer before trigger @ 0x%x, Dec End @ 0x%x\n",_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.u4OffsetBeforeDec,u4FileOffset);
    }
    #endif
    //vWrite2PC(u4InstID, 1, _pucVFifo[u4InstID]);
    //vVDEC_HAL_H264_VDec_DumpReg(u4InstID);
  #endif
  }
    #ifdef MVC_DEBUG
    {
        UINT32 u4FileOffset,u4Bits;
        u4FileOffset = u4VDEC_HAL_H264_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
        UTIL_Printf("HW(%d) ReadPointer before trigger @ 0x%x, Dec End @ 0x%x\n",u4InstID,_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.u4OffsetBeforeDec,u4FileOffset);
    }
    #endif
  //vVDEC_HAL_H264_VDec_DumpReg(u4InstID);
  vReadH264ChkSumGolden(u4InstID);
  vVDec_InvDCacheRange((UINT32)_pucDPB[u4InstID], DPB_SZ);

  #if 0
  {
    UINT32 i,u4HW_Y_Result[4],u4HW_CbCr_Result[4];
    
    for (i = 0; i < 4; i++)
    {
        u4HW_Y_Result[i] = u4VDecReadCRC(u4InstID, (i+2)<<2);
        UTIL_Printf("Y[%d] 0x%08x\n",i,u4HW_Y_Result[i]);
    }
    for (i = 0; i < 4; i++)
    {
        u4HW_CbCr_Result[i] = u4VDecReadCRC(u4InstID, (i+6)<<2);
        UTIL_Printf("C[%d] 0x%08x\n",i,u4HW_CbCr_Result[i]);
    }
  }
  #endif
//Print LOG

#if PARA8_BEHAVIOR_TEST
    vVDEC_HAL_PARA8_BEHAVIOR_DumpReg(u4InstID);
#endif


#if ChunChia_LOG
  u4Mc770 = u4VDecReadMC(u4InstID, 0x770);
  u4Mc774 = u4VDecReadMC(u4InstID, 0x774);
  u4Mc778 = u4VDecReadMC(u4InstID, 0x778);
  u4Mc8B8 = u4VDecReadMC(u4InstID, 0x8B8);

  sprintf(strMessage,"======\n");
  UTIL_Printf("%s", strMessage);

  sprintf(strMessage,"(dram_dle_cnt: 0x%x, mc_dle_cnt: 0x%x, cycle_cnt: 0x%x, dram_dle_by_preq: 0x%x)\n", u4Mc770, u4Mc774, u4Mc778, u4Mc8B8);
  UTIL_Printf("%s", strMessage);

#endif
	
	if (7 == _u4FileCnt[u4InstID])
	{
		//vVDecDumpBuf(u4InstID, _pucVFifo[u4InstID], 136314880, _u4FileCnt[u4InstID], 4, 0);
	}

  vH264VDecEnd(u4InstID);
}

// *********************************************************************
// Function    : BOOL fgIsMPEGVDecComplete(UINT32 u4InstID)
// Description : Check if VDec complete with interrupt
// Parameter   : None
// Return      : None
// *********************************************************************
BOOL fgIsMPEGVDecComplete(UINT32 u4InstID)
{
  UINT32 u4MbX;
  UINT32 u4MbY;

  #ifdef MPEG2_THRESHOLD_TEST
   if(_fgVDecComplete[1])
   {
     //VLD: 0xFC, 0xB0, 0x14C, 0x150,   DV: 0xA4
    // u4VDecReadDV(u4InstID, (41 << 2));
    // vVDecWriteDV(u4InstID, (41 << 2), (u4VDecReadDV(u4InstID,(41 << 2)) |(0x1<<5)));
     //vVDecWriteDV(u4InstID, (41 << 2), (u4VDecReadDV(u4InstID,(41 << 2)) &(~(0x1<<5))));
     UTIL_Printf("After %d frame,the rptr is  0x%x!!! \n", _u4FileCnt[u4InstID], (u4VDecReadVLD(u4InstID, 0xFC) - u4VDecReadVLD(u4InstID, 0xB0)));
     UTIL_Printf("%d (0x%x)  = 0x%4x\n",41, (41<<2),u4VDecReadDV(u4InstID, (41 << 2)));
     UTIL_Printf("%d (0x%x)  = 0x%4x\n",53, (53<<2),u4VDecReadDV(u4InstID, (53 << 2)));
     UTIL_Printf("%d (0x%x)  = 0x%4x\n",83, (83<<2),u4VDecReadVLD(u4InstID, (83 << 2)));
     UTIL_Printf("%d (0x%x)  = 0x%4x\n",84, (84<<2),u4VDecReadVLD(u4InstID, (84 << 2)));
     UTIL_Printf("%d (0x%x)  = 0x%4x\n",86, (86<<2),u4VDecReadVLD(u4InstID, (86 << 2)));
     //UTIL_Printf("VLD(0xFC)  = 0x%4x\n",u4VDecReadVLD(u4InstID, 0xFC));
     //UTIL_Printf("VLD(0xB0)  = 0x%4x\n",u4VDecReadVLD(u4InstID, 0xB0));
     _u4VerBitCount[u4InstID] = 0xffffffff;
     return TRUE;
   }
   else
   {
    // return FALSE;
   }
  #endif
  if(_fgVDecComplete[u4InstID])
  {
#if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
    HAL_GetDeltaTime(&rTimeDecDuration[u4InstID],&rTimeStartDec[u4InstID],&rTimeEndDec[u4InstID]);
    UTIL_Printf("Delta_%d:second:ms %08d:%08d\n",u4InstID,rTimeDecDuration[u4InstID].u4Seconds,rTimeDecDuration[u4InstID].u4Micros);
#endif    
#if PERFORMANCE_TEST
extern void v4VDEC_Dump_Performance_Reg();

    v4VDEC_Dump_Performance_Reg();
#endif

    vVDEC_HAL_MPEG_GetMbxMby(u4InstID, &u4MbX, &u4MbY);
    if(_u4PicStruct[u4InstID] == FRM_PIC)
    {
      if((u4MbX < (((_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecW + _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecXOff + 15) >> 4) -1))
        || (u4MbY < (((_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecH + _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecYOff + 15) >> 4) -1)))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    }
    else
    {
      if((u4MbX < (((_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecW + _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecXOff + 15) >> 4) -1))
        || u4MbY < (((_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecH + _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecYOff + 15) >> 5) -1))
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}
int vVP8DumpMem( UINT32 u4InstID, unsigned char* buf, unsigned int size , int  frame_num , unsigned int type , bool isTimeout)
{
    UCHAR fpDumpFile[100] = "d:\\ChkFolder\\vDecY_";
    UCHAR fpDumpFileC[100] = "d:\\ChkFolder\\vDecC_";
    UCHAR *fpDump;
    UINT32 u4ReadSize;
    UINT32 u4Temp;
    FILE *pFile = NULL;
    char pBitstream_name[200] = {0};

    //memcpy (pBitstream_name , _bFileStr1[u4InstID][1]+12 , (strlen(_bFileStr1[u4InstID][1]) -38) );
    //pBitstream_name[(strlen(_bFileStr1[u4InstID][1]) -38)] = '\0';

    if(type == 1)
    {
        fpDump = fpDumpFile;
    }
    else
    {
        fpDump = fpDumpFileC;
    }

    u4Temp = strlen(fpDump);
    //u4Temp += sprintf(fpDump + u4Temp,"%s_",pBitstream_name);
    u4Temp += sprintf(fpDump + u4Temp,"%d",frame_num);
    u4Temp += sprintf(fpDump + u4Temp,"%s",".bin");

    pFile = fopen(fpDump,"wb");
    if(pFile == NULL)
    {
        UTIL_Printf("Create file error !\n");
    }

    u4ReadSize = fwrite ((char* )(buf), 1, size, pFile);
    UTIL_Printf("read file len = %d @ 0x%x\n",u4ReadSize,(UINT32)buf);
    fclose(pFile);
    return 0;
}

//Qing Li add here for dump reg and pic raw data
extern void VDecDumpMpegRegister(UINT32 u4VDecID);

BOOL   _fgDumpDeblocky = FALSE;
UINT32 _u4MpvEmuDumpCount = 0;
UINT32 _u4MpvEmuDumpStartPicCount = (UINT32)(0);
UINT32 _u4MpvEmuDumpEndPicCount = (UINT32)(2000);
FILE*  _pFileHandleY = NULL;
FILE*  _pFileHandleCbcr = NULL;
FILE*  _pFileHandleDeblockyY = NULL;
FILE*  _pFileHandleDeblockyCbcr = NULL;

#define	COMPOSITE	1
CHAR  pFileName[200]= {"\0"};

void vMPEGVDecDumpPic(UINT32 u4InstID)
{
    #if COMPOSITE
    CHAR  pFileNameY[200] = "N:\\MPEG2_New_Confidential-B\\01Test-Data\\sarnoff-test\\pattern\\";
    CHAR  pFileNameCbcr[200] = "N:\\MPEG2_New_Confidential-B\\01Test-Data\\sarnoff-test\\pattern\\";
    #else
    //CHAR  pFileNameY[200] = "d:\\ChkFolder\\";
    //CHAR  pFileNameCbcr[200] = "d:\\ChkFolder\\";
    #if VDEC_SARNOFF_ON
    CHAR  pFileNameY[200] = "N:\\MPEG2_New_Confidential-B\\01Test-Data\\sarnoff-test\\pattern\\";
    CHAR  pFileNameCbcr[200] = "N:\\MPEG2_New_Confidential-B\\01Test-Data\\sarnoff-test\\pattern\\";
    #else
    CHAR  pFileNameY[200] = "N:\\MPEG2_New_Confidential-B\\01Test-Data\\mpeg2-error-test-data\\pattern\\";
    CHAR  pFileNameCbcr[200] = "N:\\MPEG2_New_Confidential-B\\01Test-Data\\mpeg2-error-test-data\\pattern\\";
    #endif
    #endif
    #if COMPOSITE
    CHAR  pFileNameYDeblocky[200] = "d:\\ChkFolder\\YGroupPic";
    CHAR  pFileNameCbcrDeblocky[200] = "d:\\ChkFolder\\CGroupPic";
    CHAR *pFileType = ".raw";
    #else
//    CHAR  pFileNameYDeblocky[200] = "d:\\ChkFolder\\YGroupPic_Deblocky_";
//    CHAR  pFileNameCbcrDeblocky[200] = "d:\\ChkFolder\\CGroupPic_Deblocky_";

    //CHAR  pFileNameYDeblocky[200] = "d:\\ChkFolder\\";
    //CHAR  pFileNameCbcrDeblocky[200] = "d:\\ChkFolder\\";
    CHAR  pFileNameYDeblocky[200] = "N:\\MPEG_Old_Confidential-B\\De-blocking\\pattern";
    CHAR  pFileNameCbcrDeblocky[200] = "N:\\MPEG2_Confidential-B\\De-blocking\\pattern";
    #endif


    FILE*  pFileTemp = NULL;
    UINT32 u4DataSizeY = 0;
    UINT32 u4DataSizeCbcr = 0;
    UINT32 u4SizeV = 0;
    UINT32 u4SizeH = 0;
    UINT32 u4Temp = 0;

    if ((_u4FileCnt[u4InstID] < _u4MpvEmuDumpStartPicCount) ||
        (_u4FileCnt[u4InstID] >= _u4MpvEmuDumpEndPicCount))
    {
        return;
    }
    // vVDecBufferFlush(u4InstID,_u4DecBufIdx[u4InstID]);
    #if (!COMPOSITE)
	#if 1
     vConcateStr(pFileNameYDeblocky, _bFileStr1[u4InstID][0], "_Y.bok\0", _u4FileCnt[u4InstID]);
     vConcateStr(pFileNameCbcrDeblocky, _bFileStr1[u4InstID][0], "_CbCr.bok\0", _u4FileCnt[u4InstID]);

     vConcateStr(pFileNameY, _bFileStr1[u4InstID][0], "_y_gold.raw\0", _u4FileCnt[u4InstID]);
     vConcateStr(pFileNameCbcr, _bFileStr1[u4InstID][0], "_c_gold.raw\0", _u4FileCnt[u4InstID]);

    u4Temp = strlen(pFileNameYDeblocky);

	#else
    u4Temp = strlen(pFileNameYDeblocky);
    u4Temp += sprintf(pFileNameYDeblocky+u4Temp,"%s",_bFileStr1[u4InstID][8]);
    u4Temp += sprintf(pFileNameYDeblocky+u4Temp,"_%d_",_u4FileCnt[u4InstID]);
    u4Temp += sprintf(pFileNameYDeblocky+u4Temp,"%s","Y.out");
    u4Temp = 0;
    u4Temp = strlen(pFileNameCbcrDeblocky);
    u4Temp += sprintf(pFileNameCbcrDeblocky+u4Temp,"%s",_bFileStr1[u4InstID][8]);
    u4Temp += sprintf(pFileNameCbcrDeblocky+u4Temp,"_%d_",_u4FileCnt[u4InstID]);
    u4Temp += sprintf(pFileNameCbcrDeblocky+u4Temp,"%s","CbCr.out");

    u4Temp = 0;
     u4Temp = strlen(pFileNameY);
    u4Temp += sprintf(pFileNameY+u4Temp,"%s",_bFileStr1[u4InstID][8]);
    u4Temp += sprintf(pFileNameY+u4Temp,"_%d_",_u4FileCnt[u4InstID]);
    u4Temp += sprintf(pFileNameY+u4Temp,"%s","Y.out");
    u4Temp = 0;
    u4Temp = strlen(pFileNameCbcr);
    u4Temp += sprintf(pFileNameCbcr+u4Temp,"%s",_bFileStr1[u4InstID][8]);
    u4Temp += sprintf(pFileNameCbcr+u4Temp,"_%d_",_u4FileCnt[u4InstID]);
    u4Temp += sprintf(pFileNameCbcr+u4Temp,"%s","CbCr.out");
     #endif


    #else
    u4Temp = strlen(pFileNameYDeblocky);
    u4Temp += sprintf(pFileNameYDeblocky+u4Temp,"%s",_bFileStr1[u4InstID][8]);
    u4Temp += sprintf(pFileNameYDeblocky+u4Temp,"%s",pFileType);
    u4Temp = 0;
    u4Temp = strlen(pFileNameCbcrDeblocky);
    u4Temp += sprintf(pFileNameCbcrDeblocky+u4Temp,"%s",_bFileStr1[u4InstID][8]);
    u4Temp += sprintf(pFileNameCbcrDeblocky+u4Temp,"%s",pFileType);
    u4Temp = 0;
    u4Temp = strlen(pFileNameY);
    u4Temp += sprintf(pFileNameY+u4Temp,"%s",(_bFileStr1[u4InstID][8]+1));
    u4Temp += sprintf(pFileNameY+u4Temp,"%s","YGroupPic");
    u4Temp += sprintf(pFileNameY+u4Temp,"%s",pFileType);
    pFileNameY[u4Temp] = '\0';
    u4Temp = 0;
    u4Temp = strlen(pFileNameCbcr);
    u4Temp += sprintf(pFileNameCbcr+u4Temp,"%s",(_bFileStr1[u4InstID][8]+1));
    u4Temp += sprintf(pFileNameCbcr+u4Temp,"%s","CGroupPic");
    u4Temp += sprintf(pFileNameCbcr+u4Temp,"%s",pFileType);
    pFileNameCbcr[u4Temp] = '\0';
    #endif



    UTIL_Printf("MPV Emu start to dump No.%d pic to PC\n", _u4FileCnt[u4InstID]);

    if (0 != (_u4RealHSize[u4InstID] * _u4RealVSize[u4InstID]))
    {
        u4SizeH = _u4RealHSize[u4InstID];
        u4SizeV = _u4RealVSize[u4InstID];
    }
    else
    {
        u4SizeH = _u4HSizeVal[u4InstID];
        u4SizeV = _u4VSizeVal[u4InstID];
    }

    u4DataSizeY = (((u4SizeH + 15)>>4)<<4) * (((u4SizeV + 31)>>5)<<5);
    u4DataSizeCbcr = u4DataSizeY / 2;
    UTIL_Printf("MPV Emu H size %d\n", (((u4SizeH + 15)>>4)<<4));
    UTIL_Printf("MPV Emu V size %d\n", (((u4SizeV + 31)>>5)<<5));

    if (!_fgDumpDeblocky)
    {
    //BSP_InvDCacheRange((UINT32)_pucDecWorkBuf[u4InstID],GOLD_Y_SZ);
    //BSP_InvDCacheRange((UINT32)_pucDecCWorkBuf[u4InstID],GOLD_C_SZ);
  }
  else
  {
     //BSP_InvDCacheRange((UINT32)_pucPpYSa[u4InstID],GOLD_Y_SZ);
     //BSP_InvDCacheRange((UINT32)_pucPpCSa[u4InstID],GOLD_C_SZ);
  }

    // close all files after completing writing raw data to these files
#if COMPOSITE
    if (((_u4FileCnt[u4InstID] + 1) == _u4MpvEmuDumpEndPicCount) || (strcmp(_bFileStr1[u4InstID][8],pFileName)))
#endif
    {
        if (_pFileHandleY)
        {
            fclose(_pFileHandleY);
            _pFileHandleY = NULL;
            UTIL_Printf("MPV Emu close file %s\n", pFileNameY);
        }

        if (_pFileHandleCbcr)
        {
            fclose(_pFileHandleCbcr);
            _pFileHandleCbcr = NULL;
            UTIL_Printf("MPV Emu close file %s\n", pFileNameCbcr);
        }

        if (_pFileHandleDeblockyY)
        {
            fclose(_pFileHandleDeblockyY);
            _pFileHandleDeblockyY = NULL;
            UTIL_Printf("MPV Emu close file %s\n", pFileNameYDeblocky);
        }

        if (_pFileHandleDeblockyCbcr)
        {
            fclose(_pFileHandleDeblockyCbcr);
            _pFileHandleDeblockyCbcr = NULL;
            UTIL_Printf("MPV Emu close file %s\n", pFileNameCbcrDeblocky);
        }
	#if COMPOSITE
	 strcpy(pFileName,_bFileStr1[u4InstID][8]);
        _u4MpvEmuDumpCount = 0;
	#else
	 if(strcmp(_bFileStr1[u4InstID][0],pFileName))
	  _u4MpvEmuDumpCount = 0;
	 strcpy(pFileName,_bFileStr1[u4InstID][0]);
	#endif
    }





    if (!_fgDumpDeblocky)
    {
        /* Dump Y */
        do
        {
            if (NULL == _pFileHandleY)
            {
                pFileTemp = fopen(pFileNameY, "wb");
                if (NULL == pFileTemp)
                {
                    UTIL_Printf("MPV Emu Create %s fail\n", pFileNameY);
                    break;
                }

                _pFileHandleY = pFileTemp;
                UTIL_Printf("MPV Emu create %s success\n", pFileNameY);
            }

            if (u4DataSizeY != fwrite ((char* )(_pucDecWorkBuf[u4InstID]), 1, u4DataSizeY, _pFileHandleY))
            {
                UTIL_Printf("MPV Emu Write to %s fail\n", pFileNameY);
                UTIL_Printf("MPV Emu need to Write data count %d\n", u4DataSizeY);
                fclose(_pFileHandleY);
                _pFileHandleY = NULL;
                UTIL_Printf("MPV Emu close file %s\n", pFileNameY);
            }

            UTIL_Printf("MPV Emu Write to %s success\n", pFileNameY);
        } while(0);

        /* Dump Cbcr */
        do
        {
            if (NULL == _pFileHandleCbcr)
            {
                pFileTemp = fopen(pFileNameCbcr, "wb");
                if (NULL == pFileTemp)
                {
                    UTIL_Printf("MPV Emu Create %s fail\n", pFileNameCbcr);
                    break;
                }

                _pFileHandleCbcr = pFileTemp;
                UTIL_Printf("MPV Emu create %s success\n", pFileNameCbcr);
            }

            if (u4DataSizeCbcr != fwrite ((char* )(_pucDecCWorkBuf[u4InstID]), 1, u4DataSizeCbcr, _pFileHandleCbcr))
            {
                UTIL_Printf("MPV Emu Write to %s fail\n", pFileNameCbcr);
                UTIL_Printf("MPV Emu need to Write data count %d\n", u4DataSizeCbcr);
                fclose(_pFileHandleCbcr);
                _pFileHandleCbcr = NULL;
                UTIL_Printf("MPV Emu close file %s\n", pFileNameCbcr);
            }

            UTIL_Printf("MPV Emu Write to %s success\n", pFileNameCbcr);
        } while(0);
    }
    else
    {
        /* Dump Deblocking Y */
        do
        {
            if (NULL == _pFileHandleDeblockyY)
            {
                pFileTemp = fopen(pFileNameYDeblocky, "wb");
                if (NULL == pFileTemp)
                {
                    UTIL_Printf("MPV Emu Create %s fail\n", pFileNameYDeblocky);
                    break;
                }

                _pFileHandleDeblockyY = pFileTemp;
                UTIL_Printf("MPV Emu create %s success\n", pFileNameYDeblocky);
            }

            if (u4DataSizeY != fwrite ((char* )(_pucPpYSa[u4InstID]), 1, u4DataSizeY, _pFileHandleDeblockyY))
            {
                UTIL_Printf("MPV Emu Write to %s fail\n", pFileNameYDeblocky);
                UTIL_Printf("MPV Emu need to Write data count %d\n", u4DataSizeY);
                fclose(_pFileHandleDeblockyY);
                _pFileHandleDeblockyY = NULL;
                UTIL_Printf("MPV Emu close file %s\n", pFileNameYDeblocky);
            }

            UTIL_Printf("MPV Emu Write to %s success\n", pFileNameYDeblocky);
        } while(0);

        /* Dump Deblocking Cbcr */
        do
        {
            if (NULL == _pFileHandleDeblockyCbcr)
            {
                pFileTemp = fopen(pFileNameCbcrDeblocky, "wb");
                if (NULL == pFileTemp)
                {
                    UTIL_Printf("MPV Emu Create %s fail\n", pFileNameCbcrDeblocky);
                    break;
                }

                _pFileHandleDeblockyCbcr = pFileTemp;
                UTIL_Printf("MPV Emu create %s success\n", pFileNameCbcrDeblocky);
            }

            if (u4DataSizeCbcr != fwrite ((char* )(_pucPpCSa[u4InstID]), 1, u4DataSizeCbcr, _pFileHandleDeblockyCbcr))
            {
                UTIL_Printf("MPV Emu Write to %s fail\n", pFileNameCbcrDeblocky);
                UTIL_Printf("MPV Emu need to Write data count %d\n", u4DataSizeCbcr);
                fclose(_pFileHandleDeblockyCbcr);
                _pFileHandleDeblockyCbcr = NULL;
                UTIL_Printf("MPV Emu close file %s\n", pFileNameCbcrDeblocky);
            }

            UTIL_Printf("MPV Emu Write to %s success\n", pFileNameCbcrDeblocky);
        } while(0);

    }

    _u4MpvEmuDumpCount ++;
    UTIL_Printf("MPV Emu Has written %d pic to PC\n", _u4MpvEmuDumpCount);

}

BOOL _fgDumpReg = FALSE;
void vMPEGVDecEnd(UINT32 u4InstID)
{
  //VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm;
  //VDEC_INFO_H264_FBUF_INFO_T *tFBufInfo;
  VDEC_INFO_MPEG_VFIFO_PRM_T rMpegVFifoInitPrm;
  VDEC_INFO_MPEG_BS_INIT_PRM_T rMpegBSInitPrm;
  UINT32 u4VldByte,u4VldBit;


  //tFBufInfo = _ptCurrFBufInfo[u4InstID];
  //tVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];

  if(_fgVerVopCoded0[u4InstID])
  {
    u4VldByte = u4VDEC_HAL_MPEG_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit) + 4;
  }
  else
  {
    u4VldByte = u4VDEC_HAL_MPEG_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit) - 4;
  }
  _u4WMVByteCount[u4InstID] = u4VldByte;

  #ifdef SEMI_RING_FIFO
  if(_u4CodecVer[u4InstID] == VDEC_DIVX3)
      _tInFileInfo[u4InstID].u4BSLenConsumed = _u4Divx3SetPos[u4InstID];
  else
      _tInFileInfo[u4InstID].u4BSLenConsumed = _u4WMVByteCount[u4InstID];//xiaolei.li's
  #endif
#ifdef LETTERBOX_DETECTION_ONLY
  vCheckLBDResult(u4InstID);
#else
    /*
    UTIL_Printf("Print CHKSUM:\n");
    vPrintMpegChksum(u4InstID);//dhmodify
    */
  vMPEGWrData2PC(u4InstID, _pucDumpYBuf[u4InstID], ((((_u4RealHSize[u4InstID] + 15) >> 4) * ((_u4RealVSize[u4InstID] + 31) >> 5)) << 9));
#endif

  if (_fgDumpReg)
  {
      UTIL_Printf("MPV Dump register after decode\n");
      VDecDumpMpegRegister(u4InstID);
      UTIL_Printf("\n MPV Dump register end \n");
  }

  // reset HW
#ifdef REDEC
  if(_u4ReDecCnt[u4InstID] > 0)
  {
    rMpegVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rMpegVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rMpegVFifoInitPrm.u4CodeType = _u4CodecVer[u4InstID];
    i4VDEC_HAL_MPEG_InitVDecHW(u4InstID,&rMpegVFifoInitPrm);
    rMpegBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    rMpegBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rMpegBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID] + _u4VLDPosByte[u4InstID];
  #ifndef  RING_VFIFO_SUPPORT
    rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  #else
  //		rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
	  rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));//mtk40343
  #endif
    if(_u4CodecVer[u4InstID] == VDEC_MPEG2)
    {
        i4VDEC_HAL_MPEG_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
    }
    else
    {
        i4VDEC_HAL_MPEG4_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
    }
    u4VDEC_HAL_MPEG_ShiftGetBitStream(0, u4InstID, _u4VLDPosBit[u4InstID]);
    // Restore Quantization Matrix
    if(_fgVerLoadIntraMatrix[u4InstID])
    {
      vVDEC_HAL_MPEG_ReLoadQuantMatrix(u4InstID, TRUE);
    }
    if(_fgVerLoadNonIntraMatrix[u4InstID])
    {
      vVDEC_HAL_MPEG_ReLoadQuantMatrix(u4InstID, FALSE);
    }
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
    return;
  }
#endif

  if(!fgMPEGIsFrmPic(u4InstID))
  {
    _fgDec2ndFldPic[u4InstID] = 1 - _fgDec2ndFldPic[u4InstID];//vToggleDecFlag(DEC_FLG_2ND_FLD_PIC);
  }
  _u4MpegDecPicNo[u4InstID]++;

  if(_u4CodecVer[u4InstID] == VDEC_DIVX3)
  {
    rMpegVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rMpegVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rMpegVFifoInitPrm.u4CodeType = _u4CodecVer[u4InstID];
    i4VDEC_HAL_MPEG_InitVDecHW(u4InstID,&rMpegVFifoInitPrm);
    if(_u4Divx3SetPos[u4InstID] >= V_FIFO_SZ)
    {
      _u4Divx3SetPos[u4InstID] = _u4Divx3SetPos[u4InstID] - V_FIFO_SZ;
    }
    rMpegBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    rMpegBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rMpegBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID] + _u4Divx3SetPos[u4InstID];
  #ifndef  RING_VFIFO_SUPPORT
    rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  #else
  //		rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
	  rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));//mtk40343
  #endif
    i4VDEC_HAL_MPEG4_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rMpegBSInitPrm);
    // Restore Quantization Matrix
    if(_fgVerLoadIntraMatrix[u4InstID])
    {
      vVDEC_HAL_MPEG_ReLoadQuantMatrix(u4InstID, TRUE);
    }
    if(_fgVerLoadNonIntraMatrix[u4InstID])
    {
      vVDEC_HAL_MPEG_ReLoadQuantMatrix(u4InstID, FALSE);
    }
  }
  else // MPEG
  {
    rMpegVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rMpegVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rMpegVFifoInitPrm.u4CodeType = _u4CodecVer[u4InstID];
    i4VDEC_HAL_MPEG_InitVDecHW(u4InstID,&rMpegVFifoInitPrm);
    rMpegBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    rMpegBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rMpegBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID] + u4VldByte;
  #ifndef  RING_VFIFO_SUPPORT
  #ifdef MPEG2_THRESHOLD_TEST
    rMpegBSInitPrm.u4WritePointer =  (UINT32)_pucVFifo[u4InstID] + u4VldByte + 0x10000 + 16;
  #else
    rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  #endif
  #else
  //		rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
	  rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));//mtk40343
  #endif
  #ifdef MPEG2_THRESHOLD_TEST
     vVDecWriteVLD(u4InstID, RW_VLD_BS_THLD_DE, (0x1000|u4VDecReadVLD(u4InstID, RW_VLD_BS_THLD_DE)));
   #endif
    if(_u4CodecVer[u4InstID] == VDEC_MPEG2)
    {
        i4VDEC_HAL_MPEG_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
    }
    else
    {
        i4VDEC_HAL_MPEG4_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
    }
    // Restore Quantization Matrix
    if(_fgVerLoadIntraMatrix[u4InstID])
    {
      vVDEC_HAL_MPEG_ReLoadQuantMatrix(u4InstID, TRUE);
    }
    if(_fgVerLoadNonIntraMatrix[u4InstID])
    {
      vVDEC_HAL_MPEG_ReLoadQuantMatrix(u4InstID, FALSE);
    }
  }

#ifndef INTERGRATION_WITH_DEMUX
#ifdef  RING_VFIFO_SUPPORT
  if((_u4LoadBitstreamCnt[u4InstID]&0x1) && (rMpegBSInitPrm.u4ReadPointer  >
  ((UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ/2))))
  {
    _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
    _tInFileInfo[u4InstID].pucTargetAddr = _pucVFifo[u4InstID];
    _tInFileInfo[u4InstID].u4FileOffset = (V_FIFO_SZ * ((_u4LoadBitstreamCnt[u4InstID]+ 1)/2));
    _tInFileInfo[u4InstID].u4TargetSz = (V_FIFO_SZ/2);
    _tInFileInfo[u4InstID].u4FileLength = 0;
  #ifdef  SATA_HDD_READ_SUPPORT
    if(!fgOpenHDDFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]))
    {
      fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
    }
  #else
    fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
  #endif
    _u4LoadBitstreamCnt[u4InstID]++;
  }
  else if((!(_u4LoadBitstreamCnt[u4InstID]&0x1)) && (rMpegBSInitPrm.u4ReadPointer  <
  ((UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ/2))))
  {
    _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
    _tInFileInfo[u4InstID].pucTargetAddr = _pucVFifo[u4InstID] + (V_FIFO_SZ/2);
    _tInFileInfo[u4InstID].u4FileOffset =  ((V_FIFO_SZ * (_u4LoadBitstreamCnt[u4InstID]+ 1)) /2);
    _tInFileInfo[u4InstID].u4TargetSz = (V_FIFO_SZ/2);
    _tInFileInfo[u4InstID].u4FileLength = 0;
  #ifdef  SATA_HDD_READ_SUPPORT
    if(!fgOpenHDDFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]))
    {
      fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
    }
  #else
    fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
  #endif
    _u4LoadBitstreamCnt[u4InstID]++;
  }
#endif
#endif
  _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;
}

void vMPEGDecEndProc(UINT32 u4InstID)
{
  #ifdef IRQ_DISABLE
  BOOL fgMpeg4;
  #endif
  UINT32 u4Cnt;
  UINT32 u4CntTimeChk;
  UINT32 u4MbX;
  UINT32 u4MbY;
  char strMessage[256];
  UINT32 u4MbX_last;
  UINT32 u4MbY_last;
  UINT32 u4MpegErrType = 0;
   UINT32 length;
  UCHAR *pbVFifo;
  VDEC_INFO_MPEG_ERR_INFO_T prMpegErrInfo;

  u4Cnt=0;
  u4CntTimeChk = 0;
  _fgVDecErr[u4InstID] = FALSE;


  #ifndef VPMODE
  if(!_fgVerVopCoded0[u4InstID])
  #endif
  {
    #ifdef IRQ_DISABLE
    fgMpeg4 = (_u4CodecVer[u4InstID] != VDEC_MPEG2)? TRUE : FALSE;
    #endif

    while(u4CntTimeChk < DEC_RETRY_NUM)
    {
      u4Cnt ++;
      if((u4Cnt & 0x3f)== 0x3f)
      {
        #ifndef IRQ_DISABLE
        #else
        if(u4VDEC_HAL_MPEG_VDec_ReadFinishFlag(u4InstID, fgMpeg4) & 0x1)
        {
          _fgVDecComplete[u4InstID] = TRUE;
          if(u4InstID == 0)
          {
            BIM_ClearIrq(VECTOR_VDFUL);
          }
          else
          {
            BIM_ClearIrq(VECTOR_VDLIT);
          }
        }
        #endif

        if(fgIsMPEGVDecComplete(u4InstID))
        {
          u4CntTimeChk = 0;
          break;
        }
        else
        {
          u4MbX_last = u4MbX;
          u4MbY_last = u4MbY;
          vVDEC_HAL_MPEG_GetMbxMby(u4InstID, &u4MbX, &u4MbY);
          if((u4MbX == u4MbX_last) && (u4MbY == u4MbY_last))
          {
            u4CntTimeChk ++;
          }
          else
          {
            u4CntTimeChk =0;
          }
        }
        u4Cnt = 0;
      }
    }
/*
    UTIL_Printf("Power SPEC = 0x%x\n",u4VDecReadMISC(0,RW_VDEC_MISC_PDN_CON_SPEC));
    UTIL_Printf("Power MODULE = 0x%x\n",u4VDecReadMISC(0,RW_VDEC_MISC_PDN_CON_MODULE));
    UTIL_Printf("Clk_sel = 0x%x\n",u4VDecReadMISC(0,VDEC_SYS_CLK_SEL));
    */
    #if VDEC_DRAM_BUSY_TEST
       vDrmaBusyOff (u4InstID);
    #endif

    if(_ucMpegVer[u4InstID] != VDEC_MPEG2)
    {
      u4MpegErrType = u4VDEC_HAL_MPEG4_GetErrType(u4InstID);
    }
	else
	{
		vVDEC_HAL_MPEG_GetErrInfo(u4InstID, &prMpegErrInfo);
	}
	if((u4CntTimeChk == DEC_RETRY_NUM) ||
      (u4MpegErrType!= 0) || (prMpegErrInfo.u4MpegErrCnt != 0))
    {
    #ifndef INTERGRATION_WITH_DEMUX
    //#ifdef EXT_COMPARE
    if(((_ucMpegVer[u4InstID] != VDEC_MPEG2) && u4MpegErrType!= 0)
	  	|| ((_ucMpegVer[u4InstID] == VDEC_MPEG2) && prMpegErrInfo.u4MpegErrType != 0)
	  	|| (u4CntTimeChk == DEC_RETRY_NUM))
      _fgVDecErr[u4InstID] = TRUE;
    //#endif
      if(u4CntTimeChk == DEC_RETRY_NUM)
      {
        vVDecOutputDebugString("\n!!!!!!!!! Decoding Timeout !!!!!!!\n");
        sprintf(strMessage, "\n!!!!!!!!!Frame %d Decoding Timeout !!!!!!!\n",_u4FileCnt[u4InstID]);
		UTIL_Printf("\n!!!!!!!!!Frame %d Decoding Timeout !!!!!!!\n",_u4FileCnt[u4InstID]);
        fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
        //vDumpReg();
       // VDecDumpMpegRegister(u4InstID);
       //ASSERT(0);
      }
	  if(_fgVDecErr[u4InstID] == TRUE)
	  {
      vVDEC_HAL_MPEG_GetMbxMby(u4InstID, &u4MbX, &u4MbY);
      vVDecOutputDebugString("\n!!!!!!!!! Decoding Error 0x%.8x!!!!!!!\n", prMpegErrInfo.u4MpegErrType);
      sprintf(strMessage,"\n!!!!!!!!! Decoding Error 0x%.8x 0x%.8x 0x%.8xat MC (x,y)=(%d/%d, %d/%d)  !!!!!!!\n", u4MpegErrType,
                prMpegErrInfo.u4MpegErrType,prMpegErrInfo.u4MpegErrRow,u4MbX, ((_tVerPic[u4InstID].u4W + 15)>> 4) - 1, u4MbY,
                (((_tVerPic[u4InstID].u4H >> (1-(fgMPEGIsFrmPic(u4InstID)))) + 15)>> 4) - 1);
	  UTIL_Printf("\n!!!!!!!!! Decoding Error 0x%.8x 0x%.8x 0x%.8xat MC (x,y)=(%d/%d, %d/%d)  !!!!!!!\n", u4MpegErrType,
                prMpegErrInfo.u4MpegErrType,prMpegErrInfo.u4MpegErrRow,u4MbX, ((_tVerPic[u4InstID].u4W + 15)>> 4) - 1, u4MbY,
                (((_tVerPic[u4InstID].u4H >> (1-(fgMPEGIsFrmPic(u4InstID)))) + 15)>> 4) - 1);
      fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
      sprintf(strMessage,"the length is %d (0x%.8x)\n", _tInFileInfo[u4InstID].u4FileLength, _tInFileInfo[u4InstID].u4FileLength);
      fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
      vReadMPEGChkSumGolden(u4InstID);
     // vWrite2PC(u4InstID, 1, _pucVFifo[u4InstID]);
      //vWrite2PC(u4InstID, 12, (UCHAR *)(&_u4DumpChksum[u4InstID][0]));
      //vDumpReg();
      UTIL_Printf("After Decode\n");
	  //VDecDumpMpegRegister(u4InstID);
	  }
	#endif
    }

	
#if BANDWIDTH_MEASURE
	//Printf("[MPEG4] Band Width Measurement\n");
	vVDEC_HAL_BANDWID_MEAS_DumpReg(u4InstID);
#endif
#if PARA8_BEHAVIOR_TEST
	vVDEC_HAL_PARA8_BEHAVIOR_DumpReg(u4InstID);
#endif
    UTIL_Printf("===>Frame %d  Decoding Done !!!!!!!\n",_u4FileCnt[u4InstID]);    

#ifdef VDEC_SIM_DUMP
   UTIL_Printf("===>Frame %d  Decoding Done !!!!!!!\n",_u4FileCnt[u4InstID]);
#endif
    if(_ucMpegVer[u4InstID] != VDEC_MPEG2)
    {
      vVDEC_HAL_MPEG_VLDVdec2Barl(u4InstID);
    }
    if(_ucMpegVer[u4InstID] == VDEC_DIVX3)
    {
      vVerifyDx3SufxChk(u4InstID);
    }
    if(_u4CodecVer[u4InstID] == VDEC_MPEG4)
    {
      // Mpeg4 workaround 
      #ifndef MPEG4_NEW_MODE  
      vMp4FixBCode(u4InstID); // mark this workaround on ICs after viper xiaolei.li
	  #endif
    }
    if(_u4VerBitCount[u4InstID] != 0xffffffff)
    {
    vVDEC_HAL_MPEG_AlignRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], BYTE_ALIGN);
    vVerifySetVSyncPrmBufPtr(u4InstID, _u4DecBufIdx[u4InstID]);
    vReadMPEGChkSumGolden(u4InstID);
  }
	//vVDecBufferFlush(u4InstID,_u4DecBufIdx[u4InstID]);
	HalFlushInvalidateDCache();
  }

#if 0// Qing Li add here for test speed log
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
	UINT32 u4MbnReg474, u4MbnReg476, u4MbnReg477;
	u4MbnReg474 = u4VDecReadMC(u4InstID, (474<<2));
	u4MbnReg476 = u4VDecReadMC(u4InstID, (476<<2));
	u4MbnReg477 = u4VDecReadMC(u4InstID, (477<<2));

	sprintf(strMessage, "\nMBN LOG_474 = 0x%.8x!!!!!!!\n", u4MbnReg474);
	fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
	sprintf(strMessage, "\nMBN LOG_476 = 0x%.8x!!!!!!!\n", u4MbnReg476);
	fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
	sprintf(strMessage, "\nMBN LOG_477 = 0x%.8x!!!!!!!\n", u4MbnReg477);
	fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
#endif
#endif

    if(_u4FileCnt[u4InstID] == _u4DumpRegPicNum[u4InstID])
   {
		UTIL_Printf("After Decode\n");
      VDecDumpMpegRegister(u4InstID);
    }
  // HalFlushInvalidateDCache();
   //vMPEGVDecDumpPic(u4InstID);

#if 0 //mc performance
  {
  UINT32 u4CodeType,u4CycleDram;
  UCHAR u4TypeCode[2];
  u4CodeType = u4VDecReadVLD(u4InstID,RW_VLD_VOP_TYPE)&0x0f;
  switch(u4CodeType)
  {
	  case 0:
		  u4TypeCode[0] = 'I';
		  break;
	  case 2:
		  u4TypeCode[0] = 'P';
		  break;
	  case 4:
		  u4TypeCode[0] = 'B';
		  break;
	  case 8:
		  u4TypeCode[0] = 'S';
		  break;
  }
  u4TypeCode[1] = '\0';
  u4CycleDram = u4VDecReadMC(u4InstID,RO_MC_DRAM_CYCLE);
  UTIL_Printf("VDEC_perf_measure: seq_name=%s pic_idx=%d pic_width=%d pic_height=%d pic_type=%s CYCLE_DRAM=%d\n",_bFileStr1[u4InstID][8],
			   _u4FileCnt[u4InstID],_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecW,_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecMPEGDecPrm.u4DecH,
			   u4TypeCode,u4CycleDram);
  UTIL_Printf("DRAM BEHAVIOR SETTING 0x%x\n",u4VDecReadMC(u4InstID,RW_MC_PARA_BEHAVIOR));
  }
#endif
    #if 0
    if(_u4FileCnt[u4InstID] == 5)
    {
    	UTIL_Printf("WorkAround before 4139 = 0x%x,4140 = 0x%x\n",u4ReadSram(u4InstID,4139),u4ReadSram(u4InstID,4140));
    	vWriteSram(u4InstID,4139,u4ReadSram(u4InstID,4140));
	UTIL_Printf("WorkAround after 4139 = 0x%x,4140 = 0x%x\n",u4ReadSram(u4InstID,4139),u4ReadSram(u4InstID,4140));
    }
    #endif

    //dump sram ==>dcac and bcode data
    #if 0
	vDumpSram(u4InstID);
    #endif

 #ifdef MPEG2_THRESHOLD_TEST
    if(_u4VerBitCount[u4InstID] != 0xffffffff)
     vMPEGVDecEnd(u4InstID);
 #else
  vMPEGVDecEnd(u4InstID);
 #endif
}

#define VDEC_BCODE_SRAM_ADDR      4096
#define WRAPPER
void vMp4FixBCode(UINT32 u4InstID)
{
  UINT32 dMbx, dMby;
  UINT32 dPrd;
  UINT32 dIdx;
  #ifdef WRAPPER
  UINT32 dTemp;
  #else
  UINT32 *pdPtr;
  #endif

  if(_u4PicCdTp[u4InstID] == P_TYPE)
  {
    dMbx = (_u4RealHSize[u4InstID] + 15) / 16;
    dMby = (_u4RealVSize[u4InstID] + 15) / 16;
    dPrd = dMbx * dMby;
    if((dPrd % 32) == 0)
    {
      dIdx = dPrd / 32;
      #ifdef WRAPPER
      dTemp = VDEC_BCODE_SRAM_ADDR + dIdx;
      vWriteSram(u4InstID,(dTemp-1),u4ReadSram(u4InstID,dTemp));
      #else
      pdPtr = (UINT32 *)_pucMp4Bcode[u4InstID];
      pdPtr[dIdx - 1] = pdPtr[dIdx];
      #endif

    }
  }
}

BOOL fgIsDvDecComplete(UINT32 u4InstID)
{
/*
  //UINT32 u4MbX;
  //UINT32 u4MbY;
  UINT32 dwTmp;

  dwTmp = dReadDV_8520(RO_DV_ST);
  //if(dwTmp && 0x1)
  if(dwTmp & 0x1)
  {
    dwDVDecodeDone_8520++;
  }
  //else if(dwTmp==0x2)
  else if(dwTmp & 0x2)
  {
    dwDVDecodeTimeOut_8520++;
  }
  else
  {
    dwDVDecodeOther_8520++;
  }

  if(_fgVDecComplete[u4InstID])
  {
    _fgVDecComplete[u4InstID] = FALSE;
    return TRUE;
  }
  return FALSE;
*/
    return TRUE;
}


// *********************************************************************
// Function    : void vChkVDec(UINT32 u4InstID,UCHAR ucVldID)
// Description : Check if decoded complete & related settings
// Parameter   : None
// Return      : None
// *********************************************************************
void vChkVDec(UINT32 u4InstID,UCHAR ucVldID)
{
  char strMessage[512];
  BOOL fgOpen;
  VDEC_INFO_WMV_BS_INIT_PRM_T rWmvBSInitPrm;
  VDEC_INFO_MPEG_BS_INIT_PRM_T rMpegBSInitPrm;

  if (_u4CodecVer[u4InstID] == VDEC_RM)
  {
    vRM_VDecDecEndProc(u4InstID);
  }  else if(_u4CodecVer[u4InstID] == VDEC_H265)
  {
    vH265DecEndProc(u4InstID, jiffies);
  }
  else if(_u4CodecVer[u4InstID] == VDEC_H264)
  //if(_u4CodecVer[u4InstID] == VDEC_H264)
  {
    vH264DecEndProc(u4InstID);
  }
  else if(_u4CodecVer[u4InstID] == VDEC_WMV)
  {
    if(_u4VprErr[u4InstID] == END_OF_FILE)
    {UTIL_Printf("=====>end of file. \n");
      sprintf(strMessage," Compare Finish==> Pic count to [%d] \n", _u4FileCnt[u4InstID] - 1);
      strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
      fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
      _u4VerBitCount[u4InstID] = 0xffffffff;
    }
    else if(_rWMVSPS[u4InstID].fgXintra8)
    {UTIL_Printf("=====>fgxintra8. \n");
      sprintf(strMessage," Compare Finish==> Pic count to [%d] \n", _u4FileCnt[u4InstID] - 1);
      strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
      fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
      _u4VerBitCount[u4InstID] = 0xffffffff;
    }
    else
    {
      vWMVDecEndProc(u4InstID);
    }
  }
 else if(_u4CodecVer[u4InstID] == VDEC_VP6)
 {
     vVerVP6DecEndProc(u4InstID);
    _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;//DEC_NORM_DEC_END;
 }
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
 else if(_u4CodecVer[u4InstID] == VDEC_VP8)
 {
     vVerVP8DecEndProc(u4InstID);
    _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;//DEC_NORM_DEC_END;
 }
#endif
 else if(_u4CodecVer[u4InstID] == VDEC_VP9)
 {
     vVerVP9DecEnd(u4InstID);
    _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;//DEC_NORM_DEC_END;
 }
 else if(_u4CodecVer[u4InstID] == VDEC_AVS)
 {
     vVerAVSDecEndProc(u4InstID);
     _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;//DEC_NORM_DEC_END;
  }
  else
  {
    vMPEGDecEndProc(u4InstID);
  }
#ifdef REDEC
    if(_u4ReDecCnt[u4InstID] == 0)
#endif
    {
        _u4PicCnt[u4InstID] ++;

    }
#ifdef REDEC
    else
    {
      sprintf(strMessage,"[%d], ", _u4PicCnt[u4InstID]);
      strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
      //fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
    }
#endif
	#ifdef SEMI_RING_FIFO
	if((_tInFileInfo[u4InstID].u4BSLenConsumed > ((V_FIFO_SZ * 4)/5)))
	{
		UTIL_Printf("[ReLoad Bitstream] \n");
		_tInFileInfo[u4InstID].u4BSLenOffset = _tInFileInfo[u4InstID].u4BSLenOffset + _tInFileInfo[u4InstID].u4BSLenConsumed;
		_tInFileInfo[u4InstID].pucTargetAddr = _pucVFifo[u4InstID];
		if((_tInFileInfo[u4InstID].u4FileLength - _tInFileInfo[u4InstID].u4BSLenOffset) <= ((V_FIFO_SZ * 4)/5))
			_tInFileInfo[u4InstID].u4TargetSz = _tInFileInfo[u4InstID].u4FileLength - _tInFileInfo[u4InstID].u4BSLenOffset + 4;
		else
			_tInFileInfo[u4InstID].u4TargetSz = V_FIFO_SZ;
	 	fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);//Load bitstream
		_tInFileInfo[u4InstID].u4BSLenConsumed = 0;
		HalFlushInvalidateDCache(); // cache flush
		if(_u4CodecVer[u4InstID] == VDEC_WMV)
		{
			rWmvBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    		rWmvBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    		rWmvBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID];
		    rWmvBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
			if(_i4CodecVersion[u4InstID] != VDEC_VC1)
			{
				i4VDEC_HAL_WMV_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rWmvBSInitPrm, FALSE);
				_iSetPos[u4InstID] = 0;//_i4HeaderLen[u4InstID];
			}
			else
			{
				i4VDEC_HAL_WMV_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rWmvBSInitPrm, TRUE);
			}
		}
		else if(_u4CodecVer[u4InstID] == VDEC_H264)
		{}
		else if(_u4CodecVer[u4InstID] == VDEC_WMV)
		{}
		else if(_u4CodecVer[u4InstID] == VDEC_VP6)
		{}
		#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
		else if(_u4CodecVer[u4InstID] == VDEC_VP8)
		{
		   VDEC_INFO_VP8_FRM_HDR_T rVp8BSInitPrm = _rVDecVp8FrmHdr[u4InstID];
           UINT32 u4Size0, u4Size1, u4Size2, u4Size3;
		   
		   _u4VP8ByteCount[u4InstID] = 0;
 
		   u4Size0 = *(_pucVFifo[u4InstID] +_u4VP8ByteCount[u4InstID]);
		   u4Size1 = *(_pucVFifo[u4InstID] +_u4VP8ByteCount[u4InstID] + 1);
		   u4Size2 = *(_pucVFifo[u4InstID] +_u4VP8ByteCount[u4InstID] + 2);
		   u4Size3 = *(_pucVFifo[u4InstID] +_u4VP8ByteCount[u4InstID] + 3);

		   _u4VP8FrmSZ[u4InstID] = ( ((u4Size0 & 0xFF)) | ((u4Size1 & 0xFF) << 8) | ((u4Size2 & 0xFF) << 16) | (u4Size3 & 0xFF)<<24);
		   _u4VP8ByteCount[u4InstID] +=VP8_IVF_FRAME_HEADER_SZ;

		    rVp8BSInitPrm.u4FifoStart =  (UINT32)_pucVFifo[u4InstID];
		    rVp8BSInitPrm.u4FifoEnd = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
		    rVp8BSInitPrm.u4VldStartPos =  (UINT32)_pucVFifo[u4InstID] + _u4VP8ByteCount[u4InstID];
		  #ifndef  RING_VFIFO_SUPPORT
		    rVp8BSInitPrm.u4WritePos = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
		  #else

		    rVp8BSInitPrm.u4WritePos = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
		  #endif

		    rVp8BSInitPrm.u4FrameSize= _u4VP8FrmSZ[u4InstID] ;
		    _rVDecVp8FrmHdr[u4InstID] = rVp8BSInitPrm;
			
		}
		#endif
		else if(_u4CodecVer[u4InstID] == VDEC_AVS)
		{}
		else // MPEG
		{
			rMpegBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    		rMpegBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    		rMpegBSInitPrm.u4ReadPointer =  (UINT32)_pucVFifo[u4InstID];
    		rMpegBSInitPrm.u4WritePointer = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
			if(_u4CodecVer[u4InstID] == VDEC_MPEG2)
    		{
        		i4VDEC_HAL_MPEG_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
    		}
    		else if(_u4CodecVer[u4InstID] == VDEC_DIVX3)
    		{
				_u4Divx3SetPos[u4InstID] = 0;
				i4VDEC_HAL_MPEG4_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);	
			}
			else
    		{
        		i4VDEC_HAL_MPEG4_InitBarrelShifter(0, u4InstID, &rMpegBSInitPrm);
			}
		}
    }
#endif
}


void vVerifyDx3SufxChk(UINT32 u4InstID)
{
  UINT32 dwByte, dwBit, dwShift, dwNextPicAddr;

  if(_ucVopCdTp[u4InstID] == VCT_I)
  {
    dwByte = u4VDEC_HAL_MPEG_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &dwBit);
    dwBit += (dwByte * 8);
    if(_u4Divx3SetPos[u4InstID] >= V_FIFO_SZ)
    {
      _u4Divx3SetPos[u4InstID] = _u4Divx3SetPos[u4InstID] - V_FIFO_SZ;
    }
    dwNextPicAddr = _u4Divx3SetPos[u4InstID] * 8;

    if(dwNextPicAddr >= dwBit)
    {
      dwShift = dwNextPicAddr - dwBit;
    }
    else
    {
      dwShift = (dwNextPicAddr + (V_FIFO_SZ * 8)) - dwBit;
    }

    _fgVerSwitchRounding[u4InstID] = FALSE;
    if(dwShift >= 17)
    {
      _fgVerSwitchRounding[u4InstID] = (u4VDEC_HAL_MPEG_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0) >> 15) & 0x1;
    }
  }
}


// *********************************************************************
// Function    : void vVerifyFlushAllSetData(UINT32 u4InstID)
// Description : flush DPB info
// Parameter   None
// Return      : None
// *********************************************************************
void vVerifyFlushAllSetData(UINT32 u4InstID)
{
  UINT32 i;

  for(i=0; i<32; i++)
  {
    _rH264SPS[u4InstID][i].fgSPSValid = FALSE;
  }
  for(i=0; i<256; i++)
  {
    _rH264PPS[u4InstID][i].fgPPSValid = FALSE;
  }
}


// *********************************************************************
// Function    : void vVerifyFlushBufInfo(UINT32 u4InstID)
// Description : flush DPB info
// Parameter   None
// Return      : None
// *********************************************************************
void vVerifyFlushBufInfo(UINT32 u4InstID)
{
  UINT32 i;

  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.u4MaxLongTermFrameIdx = 0xffffffff;
  
    if(_u4CodecVer[u4InstID] == VDEC_H265)
    {
        for(i = 0; i < 25; i++)
        {
            _ptH265FBufInfo[u4InstID][i].u4DecOrder = 0;
            vVerifyClrFBufInfo(u4InstID, i);    
        }
        _ptH265RefPicList[u4InstID].u4RefPicCnt = 0;
    }
    else if(_u4CodecVer[u4InstID] == VDEC_H264)
    {
        for(i=0; i < 17; i++)
        {
            _ptFBufInfo[u4InstID][i].u4DecOrder = 0;
            vVerifyClrFBufInfo(u4InstID, i);    
        }

        for(i=0; i<6; i++)
        {
            _ptRefPicList[u4InstID][i].u4RefPicCnt = 0;
        }
    }
  
}

extern void Margin_padding(UINT32 Ptr_output_Y, UINT32 Ptr_output_C, UINT32 PIC_SIZE_Y );
extern int vH265DumpMem( UINT32 u4InstID, unsigned char* buf, unsigned int size , int  frame_num , unsigned int type , bool isTimeout);

extern BOOL _fgH265YCrcOpen[2];
extern UINT32 _u4H265Cnt[2];

// *********************************************************************
// Function    : void vH265VDecEnd(UINT32 u4InstID)
// Description : VDec complete related setting
// Parameter   : None
// Return      : None
// *********************************************************************
void vH265VDecEnd(UINT32 u4InstID)
{     
    UINT32 u4ErrType = 0;
    UINT32 u4Bits;
    int isFail = 0;
    
    #ifdef HEVC_VP_MODE
    u4ErrType = 1;
    #endif
   //u4ErrType = u4VDEC_HAL_H265_GetErrMsg(u4InstID);     
   //u4ErrType = 1;       //test for error bitstream no golden compare

   //if ( _u4PicCnt[u4InstID] >= _u4StartCompPicNum[u4InstID] ) // test compare range
   //if ( _u4PicCnt[u4InstID] >= 244 ) // debug test compare range
    {  
        if ( u4ErrType== 0 )
        {
#ifndef  HEVC_VP_MODE
            #if 0
            Margin_padding(_ptH265CurrFBufInfo[u4InstID]->u4YStartAddr, 
                                    _ptH265CurrFBufInfo[u4InstID]->u4CStartAddr ,
                                    _ptH265CurrFBufInfo[u4InstID]->u4DramYSize );
            #endif
//	            #if HEVC_CRC_COMPARE
            if(_u4CRCMode == 1)
            {
                if(1)//(!_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.prPPS->bTilesEnabledFlag)
                {
                    isFail = vH265_CheckCRCResult(u4InstID);
                }
                else
                {
                    isFail = vH265GoldenComparison( u4InstID, _u4PicCnt[u4InstID], 
                _ptH265CurrFBufInfo[u4InstID]->u4DramYSize, 
                _ptH265CurrFBufInfo[u4InstID]->u4YStartAddr, 
                _ptH265CurrFBufInfo[u4InstID]->u4CStartAddr,
                _ptH265CurrFBufInfo[u4InstID]->u4DramMvSize, 1 ,
                _ptH265CurrFBufInfo[u4InstID]->u4YLenStartAddr,
                _ptH265CurrFBufInfo[u4InstID]->u4CLenStartAddr,
                _ptH265CurrFBufInfo[u4InstID]->u4UFOLenYsize,
                _ptH265CurrFBufInfo[u4InstID]->u4UFOLenCsize);
                }
            }
            else
            {
             isFail = vH265GoldenComparison( u4InstID, _u4PicCnt[u4InstID], 
                _ptH265CurrFBufInfo[u4InstID]->u4DramYSize, 
                _ptH265CurrFBufInfo[u4InstID]->u4YStartAddr, 
                _ptH265CurrFBufInfo[u4InstID]->u4CStartAddr,
                _ptH265CurrFBufInfo[u4InstID]->u4DramMvSize, 1 ,
                _ptH265CurrFBufInfo[u4InstID]->u4YLenStartAddr,
                _ptH265CurrFBufInfo[u4InstID]->u4CLenStartAddr,
                _ptH265CurrFBufInfo[u4InstID]->u4UFOLenYsize,
                _ptH265CurrFBufInfo[u4InstID]->u4UFOLenCsize);
            }
#endif
        } 
        else
        {
            printk("[H265] Error:0x%x\n",u4ErrType);
        }
        
        if (isFail) {
            printk("[Warning!!] hardware error type:[57]0x%x [58]0x%x\n", u4VDecReadHEVCVLD(u4InstID, 0xE4), u4VDecReadHEVCVLD(u4InstID, 0xE8));
        }

        if (isFail) {
            printk("Input_window 0x%08x\n",u4VDEC_HAL_H265_GetBitStreamShift(_u4BSID[u4InstID],u4InstID,0));
            vVDEC_HAL_H265_VDec_DumpReg(u4InstID, 1);
            _u4VerBitCount[u4InstID] = 0xFFFFFFFF;
        } else if (_u4PicCnt[u4InstID] == _u4DumpRegPicNum[u4InstID]){
            vVDEC_HAL_H265_VDec_DumpReg(u4InstID, 1);
        }
        
        if(isFail || _u4PicCnt[u4InstID] == _u4DumpRegPicNum[u4InstID])
        {
            int j;
            for(j=0; j<_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.ucMaxFBufNum; j++){
                if((_ptH265FBufInfo[u4InstID][j].ucFBufStatus == NO_PIC && _ptH265FBufInfo[u4InstID][j].ucFBufRefType == NREF_PIC) || _tVerMpvDecPrm[u4InstID].ucDecFBufIdx == j)
                    continue;
                printk("dump reference buffer, index:%d, VA:0x%x\n", j, _ptH265FBufInfo[u4InstID][j].u4YStartAddr);
//	#ifndef SATA_HDD_READ_SUPPORT
//	                vH265DumpMem( u4InstID, _ptH265FBufInfo[u4InstID][j].u4YStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4DramYSize*3/2, j , 3, 0);
                vH265DumpMem(u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4YStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4DramYSize, j , 6, 0);         
                vH265DumpMem(u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4CStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4DramCSize, j , 7, 0);
                if(_u4UFOMode)
                {
                    vH265DumpMem( u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4YLenStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4UFOLenYsize, j , 8, 0); 
                    vH265DumpMem( u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4CLenStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4UFOLenCsize, j , 9, 0); 
                }
//	#endif
            }            
            vH265DumpMem(u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4YStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4DramYSize, _u4PicCnt[u4InstID] , 1, 0);         
            vH265DumpMem(u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4CStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4DramCSize, _u4PicCnt[u4InstID] , 2, 0);
            if(_u4UFOMode)
            {
                vH265DumpMem( u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4YLenStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4UFOLenYsize, _u4PicCnt[u4InstID] , 4, 0); 
                vH265DumpMem( u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4CLenStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4UFOLenCsize, _u4PicCnt[u4InstID] , 5, 0); 
            }
        }
    }

    vH265RingFIFO_read(u4InstID, 1);
    
    // update next picture start addr
    _u4CurrPicStartAddr[1] = u4VDEC_HAL_H265_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
    
#ifdef HEVC_DUMP_PIC_BITSTREAM
           UCHAR pcFilename_bitstream[200] = {0};
       
           strncpy (pcFilename_bitstream , _bFileStr1[u4InstID][1], (strlen(_bFileStr1[u4InstID][1]) -4));
           sprintf(pcFilename_bitstream, "%s_bitstream_f%d.dat", pcFilename_bitstream,  _u4PicCnt[u4InstID]);
           if (_testCnt>_u4CurrPicStartAddr[1]){   //ring FIFO
               fgWrData2PC(_pucVFifo[u4InstID]+_testCnt, V_FIFO_SZ -_testCnt+1, 7, pcFilename_bitstream);
               fgWrData2PC(_pucVFifo[u4InstID], _u4CurrPicStartAddr[1], 7, pcFilename_bitstream);
           }else{
           fgWrData2PC(_pucVFifo[u4InstID]+_testCnt,  _u4CurrPicStartAddr[1]-_testCnt+1, 7, pcFilename_bitstream);
           }
#endif
    
#ifdef HEVC_DUMP_BITSTREAM_INFO
           UCHAR pcFilename_info[200] = {0};
           UCHAR pcInfo_data[200] = {0};
       
           if (_u4CurrPicStartAddr[1]<_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4PreReadPtr){
               _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4ReadPtrOffset++;
           }
           strncpy (pcFilename_info , _bFileStr1[u4InstID][1], (strlen(_bFileStr1[u4InstID][1]) -26));
           strcat (pcFilename_info, "_bitstream.info");
           sprintf(pcInfo_data, "0x%x\n", _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4ReadPtrOffset*V_FIFO_SZ+_u4CurrPicStartAddr[1] );
           fgWrData2PC(pcInfo_data, strlen(pcInfo_data), 7, pcFilename_info);
           _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH265DecPrm.u4PreReadPtr = _u4CurrPicStartAddr[1];
       
#endif

    //record LAE parsre start address,flow last slice nal.
    _u4LAEParseStartAddr[u4InstID] = u4VDEC_HAL_H265_ReadRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
    
   // printk("[INFO] Register Dump Before VP mode\n");   
   // vVDEC_HAL_H265_VDec_DumpReg(u4InstID, 1);
    
#ifdef HEVC_VP_MODE
    isFail = vVDEC_HAL_H265_VDec_VPmode(u4InstID);
#else
    if(_u4DualCoreEnable[u4InstID] == 0)
    {
        _u4CurrPicStartAddr[1]  = 0;
    }
#endif
    printk("[After DEC_END] _u4CurrPicStartAddr[1] = 0x%x\n", _u4CurrPicStartAddr[1]);

#ifndef  HEVC_VP_MODE
    // Errot bitstream
   if ( u4ErrType!= 0 ){
        // for YUV dump (Error concealment)

        #if 0
        if ( isFail ) {
            vVDEC_HAL_H265_VDec_DumpReg(u4InstID, 1);
            _u4VerBitCount[u4InstID] = 0xFFFFFFFF;
        } else if (_u4PicCnt[u4InstID] == _u4DumpRegPicNum[u4InstID]){
            vVDEC_HAL_H265_VDec_DumpReg(u4InstID, 1);
        }
        #endif
         printk("Dump YUV");
        
         vH265DumpMem(u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4YStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4DramYSize, _u4PicCnt[u4InstID] , 1, 0);         
         vH265DumpMem(u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4CStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4DramCSize, _u4PicCnt[u4InstID] , 2, 0);
         if(_u4UFOMode)
         {
             vH265DumpMem( u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4YLenStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4UFOLenYsize, _u4PicCnt[u4InstID] , 4, 0); 
             vH265DumpMem( u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4CLenStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4UFOLenCsize, _u4PicCnt[u4InstID] , 5, 0); 
         }
    }
#else
     //HalFlushInvalidateDCache();

     printk("[INFO] VP Mode dump YUV; Pic# %d\n", _u4PicCnt[u4InstID]);
     if (_u4Main10Mode == 0 && !_u4UFOMode)
     {
         vH265DumpYUV(u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4YStartAddr,
                         _ptH265CurrFBufInfo[u4InstID]->u4CStartAddr,
                         _ptH265CurrFBufInfo[u4InstID]->u4DramPicSize);
     }
     else
     {
         vH265DumpMem(u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4YStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4DramYSize, _u4PicCnt[u4InstID] , 1, 0);         
         vH265DumpMem(u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4CStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4DramCSize, _u4PicCnt[u4InstID] , 2, 0);
     }
     
     if(_u4UFOMode)
     { 
         vH265DumpMem( u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4YLenStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4UFOLenYsize, _u4PicCnt[u4InstID] , 4, 0); 
         vH265DumpMem( u4InstID, _ptH265CurrFBufInfo[u4InstID]->u4CLenStartAddr, _ptH265CurrFBufInfo[u4InstID]->u4UFOLenCsize, _u4PicCnt[u4InstID] , 5, 0); 
     }

    // printk("[INFO] Register Dump After VP mode\n");
    // vVDEC_HAL_H265_VDec_DumpReg(u4InstID, 1);
#endif

    if ( _u4PicCnt[u4InstID] >= _u4EndCompPicNum[u4InstID] ) { // stop verification
        _u4VerBitCount[u4InstID] = 0xFFFFFFFF;
    }

    if(_u4K2KMode[u4InstID])
    {
        _tVerDec[u4InstID].ucState = DEC_NORM_LAE_DEC;
    }
    else
    {
        _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;
    }
    
}


// *********************************************************************
// Function    : void vH264VDecEnd(UINT32 u4InstID)
// Description : VDec complete related setting
// Parameter   : None
// Return      : None
// *********************************************************************
void vH264VDecEnd(UINT32 u4InstID)
{
	VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm, *tLAEVerMpvDecPrm;
  	VDEC_INFO_H264_FBUF_INFO_T *tFBufInfo, *tLAEFBufInfo;
#if defined(SW_RESET) || defined(REDEC)
  	VDEC_INFO_H264_INIT_PRM_T rH264VDecInitPrm;
  	VDEC_INFO_H264_BS_INIT_PRM_T rH264BSInitPrm;
#endif
#ifdef SW_RESET
  	UINT32 u4Bits;
#endif

#if VDEC_VER_COMPARE_CRC
#ifndef LETTERBOX_DETECTION_ONLY
  	BOOL fgCRCPass = FALSE;
#endif
#endif
	if (1 == _u4DualCoreEnable[0])
	{
		tLAEFBufInfo = _ptCurrFBufInfo[VDEC_LAE0];
  		tLAEVerMpvDecPrm = &_tVerMpvDecPrm[VDEC_LAE0];
	}
  	tFBufInfo = _ptCurrFBufInfo[u4InstID];
  	tVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];


#ifdef LETTERBOX_DETECTION_ONLY
  vCheckLBDResult(u4InstID);
#else
#if VDEC_VER_COMPARE_CRC
	if (1 == _u4DualCoreEnable[0])
	{
		fgCRCPass = vH264_MCoreCheckCRCResult(u4InstID);
	}
	else
	{
        #ifdef VDEC_DUMP_CRC
        fgCRCPass = FALSE;
        #else
        fgCRCPass = vH264_CheckCRCResult(u4InstID);
        #endif
  		
	}
   	if (fgCRCPass == FALSE)
#endif
  	{
  		if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.fgIsCompactMode 
			&& (_tVerMpvDecPrm[u4InstID].ucPicStruct == TOP_FIELD))
  		{
  			//10 bit must be compared in frame, top field will do nothing
  		}
		else
		{
    		vH264WrData2PC(u4InstID, _pucDumpYBuf[u4InstID], tFBufInfo->u4DramPicSize);
		}
  	}
#endif

#ifdef REDEC
	if(_u4ReDecCnt[u4InstID] > 0)
	{
    #ifdef BARREL2_THREAD_SUPPORT
		VERIFY (x_sema_lock(_ahVDecEndSema[u4InstID], X_SEMA_OPTION_WAIT) == OSR_OK);
    #endif
		_u4FileOffset[u4InstID] =  _u4VLDPosByte[u4InstID];
	  	rH264VDecInitPrm.u4FGDatabase = (UINT32)_pucFGDatabase[u4InstID];
	  	rH264VDecInitPrm.u4CompModelValue = (UINT32)(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue);
	  	rH264VDecInitPrm.u4FGSeedbase = (UINT32)_pucFGSeedbase[u4InstID];
	  	i4VDEC_HAL_H264_InitVDecHW(u4InstID,&rH264VDecInitPrm);
	  	rH264BSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
	  	rH264BSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
	  	rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID] + _u4FileOffset[u4InstID];
	#ifndef  RING_VFIFO_SUPPORT
	  	rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
	#else
	//	rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
		rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
    #endif
		rH264BSInitPrm.u4PredSa = /*PHYSICAL*/((UINT32)_pucPredSa[u4InstID]);
		i4VDEC_HAL_H264_InitBarrelShifter(0, u4InstID, &rH264BSInitPrm);
		u4VDEC_HAL_H264_ShiftGetBitStream(0, u4InstID, _u4VLDPosBit[u4InstID]);
    #ifdef BARREL2_THREAD_SUPPORT
		VERIFY (x_sema_unlock(_ahVDecEndSema[u4InstID]) == OSR_OK);
    #endif
		_u4VLDPosByte[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &_u4VLDPosBit[u4InstID]);
		_tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
		return;
	}
#endif

  	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicW = tFBufInfo->u4W;
  	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicH = tFBufInfo->u4H;
	if (1 == _u4DualCoreEnable[0])
	{
		tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicW = tLAEFBufInfo->u4W;
	  	tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicH = tLAEFBufInfo->u4H;
	}
  //Marking procedure
  	if (1 == _u4DualCoreEnable[0])
  	{
  		if(fgIsRefPic(VDEC_LAE0))
	  	{
	    	if(fgIsIDRPic(VDEC_LAE0)) // IDR pic
	    	{
	      		if(tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgLongTermReferenceFlag)
	      		{
	        		vVerifySetPicRefType(VDEC_LAE0, tLAEVerMpvDecPrm->ucPicStruct, LREF_PIC);
	        		tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.u4MaxLongTermFrameIdx = 0;
	        		tLAEFBufInfo->u4LongTermFrameIdx = 0;
	        		tLAEFBufInfo->u4TFldLongTermFrameIdx = 0;
	        		tLAEFBufInfo->u4BFldLongTermFrameIdx = 0;
	      		}
		      	else
		      	{
		        	tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.u4MaxLongTermFrameIdx = 0xffffffff;				
		        	vVerifySetPicRefType(VDEC_LAE0, tLAEVerMpvDecPrm->ucPicStruct, SREF_PIC);
		      	}
	    	}
	    	else // !IDR pic
		    {
		      	if(tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgAdaptiveRefPicMarkingModeFlag)
		      	{      		
		        	vVerifyAdapRefPicmarkingProce(VDEC_LAE0);
		      	}
		      	else
		      	{
		        	vVerifySlidingWindowProce(VDEC_LAE0);
		      	}			
			    if(bGetPicRefType(VDEC_LAE0, tLAEVerMpvDecPrm->ucPicStruct) != LREF_PIC)
			    {
			        vVerifySetPicRefType(VDEC_LAE0, tLAEVerMpvDecPrm->ucPicStruct, SREF_PIC);
			    }
		    }
	  	}
  	}
  	if(fgIsRefPic(u4InstID))
  	{
    	if(fgIsIDRPic(u4InstID)) // IDR pic
    	{
      		if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgLongTermReferenceFlag)
      		{
      			if (1 == _u4DualCoreEnable[u4InstID])
      			{
      				vVerifySetPicRefType(VDEC_VLD0, tVerMpvDecPrm->ucPicStruct, LREF_PIC);
					vVerifySetPicRefType(VDEC_VLD1, tVerMpvDecPrm->ucPicStruct, LREF_PIC);
      			}
				else
				{
        			vVerifySetPicRefType(u4InstID, tVerMpvDecPrm->ucPicStruct, LREF_PIC);
				}
        		tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.u4MaxLongTermFrameIdx = 0;
        		tFBufInfo->u4LongTermFrameIdx = 0;
        		tFBufInfo->u4TFldLongTermFrameIdx = 0;
        		tFBufInfo->u4BFldLongTermFrameIdx = 0;
      		}
	      	else
	      	{
	        	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.u4MaxLongTermFrameIdx = 0xffffffff;
				if (1 == _u4DualCoreEnable[u4InstID])
				{
					vVerifySetPicRefType(VDEC_VLD0, tVerMpvDecPrm->ucPicStruct, SREF_PIC);
					vVerifySetPicRefType(VDEC_VLD1, tVerMpvDecPrm->ucPicStruct, SREF_PIC);
				}
				else
				{
	        		vVerifySetPicRefType(u4InstID, tVerMpvDecPrm->ucPicStruct, SREF_PIC);
				}
	      	}
    	}
    	else // !IDR pic
	    {
	      	if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgAdaptiveRefPicMarkingModeFlag)
	      	{
	      		if (1 == _u4DualCoreEnable[u4InstID])
	      		{
	      			vVerifyAdapRefPicmarkingProce(VDEC_VLD0);
					vVerifyAdapRefPicmarkingProce(VDEC_VLD1);
	      		}
				else
				{
	        		vVerifyAdapRefPicmarkingProce(u4InstID);
				}
	      	}
	      	else
	      	{
	      		if (1 == _u4DualCoreEnable[u4InstID])
	      		{
	      			vVerifySlidingWindowProce(VDEC_VLD0);
					vVerifySlidingWindowProce(VDEC_VLD1);
	      		}
				else
				{
	        		vVerifySlidingWindowProce(u4InstID);
				}
	      	}
			if (1 == _u4DualCoreEnable[u4InstID])
			{
				if((bGetPicRefType(VDEC_VLD0, tVerMpvDecPrm->ucPicStruct) != LREF_PIC) 
					&& (bGetPicRefType(VDEC_VLD1, tVerMpvDecPrm->ucPicStruct) != LREF_PIC))
	      		{
	        		vVerifySetPicRefType(VDEC_VLD0, tVerMpvDecPrm->ucPicStruct, SREF_PIC);
					vVerifySetPicRefType(VDEC_VLD1, tVerMpvDecPrm->ucPicStruct, SREF_PIC);
	      		}
			}
			else
			{
		      	if(bGetPicRefType(u4InstID, tVerMpvDecPrm->ucPicStruct) != LREF_PIC)
		      	{
		        	vVerifySetPicRefType(u4InstID, tVerMpvDecPrm->ucPicStruct, SREF_PIC);
		      	}
			}
	    }
  	}

  	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.fgLastMmco5 = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgMmco5;
  	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.ucLastPicStruct = tVerMpvDecPrm->ucPicStruct;
	if (1 == _u4DualCoreEnable[0])
	{
		tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.fgLastMmco5 = tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgMmco5;
  		tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.ucLastPicStruct = tLAEVerMpvDecPrm->ucPicStruct;
	}
  	if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.fgLastMmco5)
  	{
    	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum = 0;
    	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastFrameNumOffset = 0;
  	}
  	else
  	{
    	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum;
    	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastFrameNumOffset = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset;
  	}
	if (1 == _u4DualCoreEnable[0])
	{
		if(tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.fgLastMmco5)
	  	{
	    	tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum = 0;
	    	tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastFrameNumOffset = 0;
	  	}
  		else
	  	{
	    	tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastFrameNum = tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4FrameNum;
	    	tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastFrameNumOffset = tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.i4FrmNumOffset;
	  	}
	}
  	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastPOC =  tFBufInfo->i4POC;
  	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastTFldPOC = tFBufInfo->i4TFldPOC;
  	tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastBFldPOC = tFBufInfo->i4BFldPOC;
	if (1 == _u4DualCoreEnable[0])
	{
		tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastPOC =  tLAEFBufInfo->i4POC;
  		tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastTFldPOC = tLAEFBufInfo->i4TFldPOC;
  		tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastBFldPOC = tLAEFBufInfo->i4BFldPOC;
	}
  	if(fgIsRefPic(u4InstID))
  	{
	    tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastRefPOC = tFBufInfo->i4POC;
	//    tVerMpvDecPrm->rLastInfo.iLastRefPOCCntLsb = tVerMpvDecPrm->prSliceHdr->i4PicOrderCntLsb;
	//    tVerMpvDecPrm->rLastInfo.iLastRefPOCCntMsb = tVerMpvDecPrm->prSliceHdr->i4PicOrderCntMsb;
	    tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastRefPOCLsb =  tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntLsb;
	    tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastRefPOCMsb = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntMsb;
  	}
	if (1 == _u4DualCoreEnable[0])
	{
		if(fgIsRefPic(VDEC_LAE0))
	  	{
		    tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastRefPOC = tLAEFBufInfo->i4POC;
		//    tVerMpvDecPrm->rLastInfo.iLastRefPOCCntLsb = tVerMpvDecPrm->prSliceHdr->i4PicOrderCntLsb;
		//    tVerMpvDecPrm->rLastInfo.iLastRefPOCCntMsb = tVerMpvDecPrm->prSliceHdr->i4PicOrderCntMsb;
		    tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastRefPOCLsb =  tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntLsb;
		    tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.i4LastRefPOCMsb = tLAEVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->i4PicOrderCntMsb;
	  	}
	}
#if defined(MVC_VFY_SUPPORT)
  	if(_ucMVCType[u4InstID] != 0)
  	{
       tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.ucLastDpbId = tVerMpvDecPrm->ucDecFBufIdx;
       tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastViewId =  tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.rMvcExtInfo.u4ViewId;
       x_memcpy(&_rH264PrevFbInfo[u4InstID], _ptCurrFBufInfo[u4InstID], sizeof(VDEC_INFO_H264_FBUF_INFO_T));
    }
#endif
	
  	if(fgIsDecFlagSet(u4InstID, DEC_FLAG_CHG_FBUF))
  	{
    	_ptCurrFBufInfo[u4InstID]->eH264DpbStatus = H264_DPB_STATUS_DECODED;
    	_ptCurrFBufInfo[u4InstID]->u4DecOrder = _u4TotalDecFrms[u4InstID];
    	vChkOutputFBuf(u4InstID);
    	_u4TotalDecFrms[u4InstID] ++;
  	}
  	else
  	{
    	_ptCurrFBufInfo[u4InstID]->eH264DpbStatus = H264_DPB_STATUS_FLD_DECODED;
  	}
	if (1 == _u4DualCoreEnable[0])
	{
		if(fgIsDecFlagSet(VDEC_LAE0, DEC_FLAG_CHG_FBUF))
	  	{
	    	_ptCurrFBufInfo[VDEC_LAE0]->eH264DpbStatus = H264_DPB_STATUS_DECODED;
	    	_ptCurrFBufInfo[VDEC_LAE0]->u4DecOrder = _u4TotalDecFrms[u4InstID];
	    	vChkOutputFBuf(VDEC_LAE0);
	    	_u4TotalDecFrms[VDEC_LAE0] ++;
	  	}
	  	else
	  	{
	    	_ptCurrFBufInfo[VDEC_LAE0]->eH264DpbStatus = H264_DPB_STATUS_FLD_DECODED;
	  	}
	}
#ifdef SW_RESET
		if (1 == _u4DualCoreEnable[0])
		{			
			_u4FileOffset[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(0, VDEC_LAE0, (UINT32)_pucVFifo[u4InstID], &u4Bits);   
		}
		else
		{
	 	 	_u4FileOffset[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4Bits);
		}
	  rH264BSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
	  rH264BSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
	  rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID] + _u4FileOffset[u4InstID];
	   if (_ucMVCType[u4InstID] > 0)
	   {
		  if (u4InstID == 0)
		  {
			  rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID] + _u4FileOffset[1];
		  }
		  else if (u4InstID == 1)
		  {
			 rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID] + _u4FileOffset[0];
		  }
	   }	
#ifndef  RING_VFIFO_SUPPORT
	  rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
#else
	  //  rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
	  rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
#endif
	  rH264BSInitPrm.u4PredSa = /*PHYSICAL*/((UINT32)_pucPredSa[u4InstID]);
#endif

#ifndef INTERGRATION_WITH_DEMUX
#ifdef  RING_VFIFO_SUPPORT
  	if((_u4LoadBitstreamCnt[u4InstID]&0x1) && (rH264BSInitPrm.u4VLDRdPtr >
  	((UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ/2))))
  	{
    	_tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
    	_tInFileInfo[u4InstID].pucTargetAddr = _pucVFifo[u4InstID];
    	_tInFileInfo[u4InstID].u4FileOffset = ((V_FIFO_SZ * (_u4LoadBitstreamCnt[u4InstID]+ 1))/2);
		//UTIL_Printf("InstID %d, LoadBitstreamCnt %d, fileoffset 0x%x\n", u4InstID, _u4LoadBitstreamCnt[u4InstID], _tInFileInfo[u4InstID].u4FileOffset);
    	_tInFileInfo[u4InstID].u4TargetSz = (V_FIFO_SZ/2);
    	_tInFileInfo[u4InstID].u4FileLength = 0;
  	#ifdef  SATA_HDD_READ_SUPPORT
    	if(!fgOpenHDDFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]))
    	{
      		//fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
    	}
  	#elif defined(IDE_READ_SUPPORT)
       	fgOpenIdeFile(_bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
  	#else
    	fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
  	#endif
		_tInFileInfo[u4InstID].u4FileLength = _tInFileInfo[u4InstID].u4FileOffset + _tInFileInfo[u4InstID].u4RealGetBytes;
    	_u4LoadBitstreamCnt[u4InstID]++;	
		printk("[INFO] u4FileOffset =  0x%08X; Read size = 0x%08X bytes\n",_tInFileInfo[u4InstID].u4FileOffset, _tInFileInfo[u4InstID].u4RealGetBytes);
  	}
  	else if((!(_u4LoadBitstreamCnt[u4InstID]&0x1)) && (rH264BSInitPrm.u4VLDRdPtr <
  	((UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ/2))))
  	{
	    _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
	    _tInFileInfo[u4InstID].pucTargetAddr = _pucVFifo[u4InstID] + (V_FIFO_SZ/2);
	    _tInFileInfo[u4InstID].u4FileOffset = ((V_FIFO_SZ * (_u4LoadBitstreamCnt[u4InstID]+ 1))/2);
		//UTIL_Printf("2~~InstID %d, LoadBitstreamCnt %d, fileoffset 0x%x\n", u4InstID, _u4LoadBitstreamCnt[u4InstID], _tInFileInfo[u4InstID].u4FileOffset);
	    _tInFileInfo[u4InstID].u4TargetSz = (V_FIFO_SZ/2);
	    _tInFileInfo[u4InstID].u4FileLength = 0;
  	#ifdef  SATA_HDD_READ_SUPPORT
	    if(!fgOpenHDDFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]))
	    {
	      	//fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
	    }
  	#elif defined(IDE_READ_SUPPORT)
       	fgOpenIdeFile(_bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
  	#else
    	fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
  	#endif		
		_tInFileInfo[u4InstID].u4FileLength = _tInFileInfo[u4InstID].u4FileOffset + _tInFileInfo[u4InstID].u4RealGetBytes;
   		_u4LoadBitstreamCnt[u4InstID]++;
  	}
  	if((0 ==_tInFileInfo[u4InstID].u4RealGetBytes) ||
  	(V_FIFO_SZ/2 != _tInFileInfo[u4InstID].u4RealGetBytes) )
	{
		//vAddStartCode2Dram(_pucVFifo+_tInFileInfo.u4FileLength);
		UCHAR *pbDramAddr = _tInFileInfo[u4InstID].pucTargetAddr+_tInFileInfo[u4InstID].u4RealGetBytes;

		pbDramAddr[0] = 0x00; pbDramAddr++;
		if((UINT32)(_pucVFifo[u4InstID] + V_FIFO_SZ) <= (UINT32)pbDramAddr)
		{
			pbDramAddr = _pucVFifo[u4InstID];
		}
		pbDramAddr[0] = 0x00; pbDramAddr++;
		if((UINT32)(_pucVFifo[u4InstID] + V_FIFO_SZ) <= (UINT32)pbDramAddr)
		{
			pbDramAddr = _pucVFifo[u4InstID];
		}
		pbDramAddr[0] = 0x01;
  	}
#endif
#endif

	if (1 == _u4K2KMode[u4InstID])
	{
		// update next picture start addr
		//record LAE parsre start address,flow last slice nal.
		_u4LAEParseStartAddr[u4InstID] = u4VDEC_HAL_H264_ReadRdPtr(_u4BSID[u4InstID], VDEC_LAE0, (UINT32)_pucVFifo[u4InstID], &u4Bits);
		_u4CurrPicStartAddr[1] = _u4LAEParseStartAddr[u4InstID];
		_tVerDec[u4InstID].ucState = DEC_NORM_LAE_DEC;
	}
	else
	{
  		_tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;
	}
  	if((u4InstID == 0) && _ucMVCType[0])
  	{
      	_fgMVCReady[0] = FALSE;
      	_fgMVCReady[1] = TRUE;
      	if(_u4VerBitCount[u4InstID] == 0xffffffff)
      	{
          	while((_fgMVCReady[0] == FALSE) || (_fgMVCReady[1] == TRUE))
          	{
             	x_thread_delay(5);
          	}
          	x_thread_delay(10);
      	}
  	}

  	if((u4InstID == 1) && _ucMVCType[1] && (_u4VerBitCount[1] != 0xffffffff))
  //if((u4InstID == 1) && _ucMVCType[1])
  	{
      	_fgMVCReady[0] = TRUE;
      	_fgMVCReady[1] = FALSE;
  	}
    vVDec_FlushDCacheRange((UINT32) _pucVFifo[u4InstID],V_FIFO_SZ);
}

// *********************************************************************
// Function    : void vVerifySetPicRefType(UINT32 u4InstID, UCHAR ucPicStruct, ucPicStruct ucRefType)
// Description : set pic ref type
// Parameter   : UCHAR ucPicType: pic struct : FRAME, TOP_FIELD, BOTTOM_FIELD
//                     UCHAR ucRefType: pic ref type: NREF_PIC, SREF_PIC, LREF_PIC
// Return      : None
// *********************************************************************
void vVerifySetPicRefType(UINT32 u4InstID, UCHAR ucPicStruct, UCHAR ucRefType)
{
  if(ucPicStruct & TOP_FIELD)
  {
    _ptCurrFBufInfo[u4InstID]->ucTFldRefType = ucRefType;
  }
  if(ucPicStruct & BOTTOM_FIELD)
  {
    _ptCurrFBufInfo[u4InstID]->ucBFldRefType = ucRefType;
  }
  //if(ucPicStruct == FRAME)
  {
    _ptCurrFBufInfo[u4InstID]->ucFBufRefType = ucRefType;
  }
  //else
  {
    //_ptCurrFBufInfo->ucFBufRefType = NREF_PIC;
  }
}

// *********************************************************************
// Function    : void vVerifyAdapRefPicmarkingProce(UINT32 u4InstID)
// Description : marking the decoded ref pic with adaptive method
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyAdapRefPicmarkingProce(UINT32 u4InstID)
{
  VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm;
  UINT32 u4PicNumX;
  UINT32 u4Cnt;
  INT32 i;

  tVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];
  u4Cnt = 0;
  while(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] != 0)
  {
    switch(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MemoryManagementControlOperation[u4Cnt]&0xff)
    {
      case 0:
        break;
      case 1:
        // picNumX
        tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4DifferencOfPicNumsMinus1 = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] >> 8;
        //if(fgIsFrmPic(_u4VDecID))
        {
          u4PicNumX = _ptCurrFBufInfo[u4InstID]->i4PicNum - tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4DifferencOfPicNumsMinus1 - 1;
        }
#if 0
        else if(tVerMpvDecPrm->ucPicStruct & TOP_FIELD)
        {
          u4PicNumX = _ptCurrFBufInfo[u4InstID]->i4TFldPicNum - tVerMpvDecPrm->prSliceHdr->u4DifferencOfPicNumsMinus1 - 1;
        }
        else if(tVerMpvDecPrm->ucPicStruct & BOTTOM_FIELD)
        {
          u4PicNumX = _ptCurrFBufInfo[u4InstID]->i4BFldPicNum - tVerMpvDecPrm->prSliceHdr->u4DifferencOfPicNumsMinus1 - 1;
        }
#endif
        for(i=0; i < tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
        {
          if(fgIsFrmPic(u4InstID) && (_ptFBufInfo[u4InstID][i].i4PicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucFBufRefType == SREF_PIC))
          {
            vVerifyClrPicRefInfo(u4InstID, FRAME, i);
            i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
          }
          else if((!fgIsFrmPic(u4InstID)) &&
                     (((_ptFBufInfo[u4InstID][i].i4TFldPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == SREF_PIC))
                     || ((_ptFBufInfo[u4InstID][i].i4BFldPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == SREF_PIC))))
          {
            if((_ptFBufInfo[u4InstID][i].i4TFldPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == SREF_PIC))
            {
              vVerifyClrPicRefInfo(u4InstID, TOP_FIELD, i);
            }
            if((_ptFBufInfo[u4InstID][i].i4BFldPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == SREF_PIC))
            {
              vVerifyClrPicRefInfo(u4InstID, BOTTOM_FIELD, i);
            }
            i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
          }
        }
        break;
      case 2:
        tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermPicNum = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] >> 8;
        u4PicNumX = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermPicNum;
        for(i=0; i < tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
        {
          if(fgIsFrmPic(u4InstID) && (_ptFBufInfo[u4InstID][i].i4LongTermPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucFBufRefType == LREF_PIC))
          {
            vVerifyClrPicRefInfo(u4InstID, FRAME, i);
            i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
          }
          else if((!fgIsFrmPic(u4InstID)) &&
                     (((_ptFBufInfo[u4InstID][i].i4TFldLongTermPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC))
                     || ((_ptFBufInfo[u4InstID][i].i4BFldLongTermPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC))))
          {
            if((_ptFBufInfo[u4InstID][i].i4TFldLongTermPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC))
            {
              vVerifyClrPicRefInfo(u4InstID, TOP_FIELD, i);
            }
            if((_ptFBufInfo[u4InstID][i].i4BFldLongTermPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC))
            {
              vVerifyClrPicRefInfo(u4InstID, BOTTOM_FIELD, i);
            }
            i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
          }
        }
        break;
      case 3:
        tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4DifferencOfPicNumsMinus1 = (tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] >> 8) & 0xff;
        tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] >> 16;
        u4PicNumX = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
        for(i=0; i < tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
        {
          if(i != tVerMpvDecPrm->ucDecFBufIdx)
          {
            if((_ptFBufInfo[u4InstID][i].ucFBufStatus == FRAME) && (_ptFBufInfo[u4InstID][i].u4LongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucFBufRefType == LREF_PIC))
            {
              vVerifyClrPicRefInfo(u4InstID, FRAME, i);
              i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
            }
            else if((_ptFBufInfo[u4InstID][i].ucFBufStatus != FRAME) &&
                       (((_ptFBufInfo[u4InstID][i].u4TFldLongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC))
                       || ((_ptFBufInfo[u4InstID][i].u4BFldLongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC))))
            {
              if((_ptFBufInfo[u4InstID][i].u4TFldLongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC))
              {
                vVerifyClrPicRefInfo(u4InstID, TOP_FIELD, i);
              }
              if((_ptFBufInfo[u4InstID][i].u4BFldLongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC))
              {
                vVerifyClrPicRefInfo(u4InstID, BOTTOM_FIELD, i);
              }
              i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
            }
          }
        }

        // picNumX
        //if(fgIsFrmPic(_u4VDecID))
        {
          u4PicNumX = _ptCurrFBufInfo[u4InstID]->i4PicNum - tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4DifferencOfPicNumsMinus1 - 1;
        }
#if 0
        else if(tVerMpvDecPrm->ucPicStruct & TOP_FIELD)
        {
          u4PicNumX = _ptCurrFBufInfo[u4InstID]->i4TFldPicNum - tVerMpvDecPrm->prSliceHdr->u4DifferencOfPicNumsMinus1 - 1;
        }
        else if(tVerMpvDecPrm->ucPicStruct & BOTTOM_FIELD)
        {
          u4PicNumX = _ptCurrFBufInfo[u4InstID]->i4BFldPicNum - tVerMpvDecPrm->prSliceHdr->u4DifferencOfPicNumsMinus1 - 1;
        }
#endif
        for(i=0; i < tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
        {
          if(fgIsFrmPic(u4InstID) && (_ptFBufInfo[u4InstID][i].i4PicNum == u4PicNumX)
              && (_ptFBufInfo[u4InstID][i].ucFBufRefType == SREF_PIC) && (!_ptFBufInfo[u4InstID][i].fgNonExisting))
          {
            _ptFBufInfo[u4InstID][i].ucFBufRefType = LREF_PIC;
            _ptFBufInfo[u4InstID][i].ucTFldRefType = LREF_PIC;
            _ptFBufInfo[u4InstID][i].ucBFldRefType = LREF_PIC;
            _ptFBufInfo[u4InstID][i].u4LongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
            _ptFBufInfo[u4InstID][i].u4TFldLongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
            _ptFBufInfo[u4InstID][i].u4BFldLongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
            _ptFBufInfo[u4InstID][i].i4LongTermPicNum = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
            _ptFBufInfo[u4InstID][i].i4TFldLongTermPicNum = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
            _ptFBufInfo[u4InstID][i].i4BFldLongTermPicNum = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
            i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
          }
          else if((!fgIsFrmPic(u4InstID)) &&
                     (((_ptFBufInfo[u4InstID][i].i4TFldPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == SREF_PIC)  && (!_ptFBufInfo[u4InstID][i].fgNonExisting))
                     || ((_ptFBufInfo[u4InstID][i].i4BFldPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == SREF_PIC)  && (!_ptFBufInfo[u4InstID][i].fgNonExisting))))
          {
            if((_ptFBufInfo[u4InstID][i].i4TFldPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == SREF_PIC)  && (!_ptFBufInfo[u4InstID][i].fgNonExisting))
            {
              _ptFBufInfo[u4InstID][i].ucTFldRefType = LREF_PIC;
              _ptFBufInfo[u4InstID][i].u4TFldLongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
              _ptFBufInfo[u4InstID][i].i4TFldLongTermPicNum = (tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx<<1) + ((_tVerMpvDecPrm[u4InstID].ucPicStruct == TOP_FIELD)? 1: 0);
              if(_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC)
              {
                _ptFBufInfo[u4InstID][i].ucFBufRefType = LREF_PIC;
                _ptFBufInfo[u4InstID][i].u4LongTermFrameIdx  = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
                _ptFBufInfo[u4InstID][i].i4LongTermPicNum = _ptFBufInfo[u4InstID][i].u4LongTermFrameIdx;
              }
            }
            if((_ptFBufInfo[u4InstID][i].i4BFldPicNum == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == SREF_PIC)  && (!_ptFBufInfo[u4InstID][i].fgNonExisting))
            {
              _ptFBufInfo[u4InstID][i].ucBFldRefType = LREF_PIC;
              _ptFBufInfo[u4InstID][i].u4BFldLongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
              _ptFBufInfo[u4InstID][i].i4BFldLongTermPicNum = (tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx<<1) + ((_tVerMpvDecPrm[u4InstID].ucPicStruct == BOTTOM_FIELD)? 1: 0);
              if(_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC)
              {
                _ptFBufInfo[u4InstID][i].ucFBufRefType = LREF_PIC;
                _ptFBufInfo[u4InstID][i].u4LongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
                _ptFBufInfo[u4InstID][i].i4LongTermPicNum = _ptFBufInfo[u4InstID][i].u4LongTermFrameIdx;
              }
            }

            i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
          }
        }
        break;
      case 4:
        tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MaxLongTermFrameIdxPlus1 = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] >> 8;
        if(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MaxLongTermFrameIdxPlus1 == 0)
        {
           _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.u4MaxLongTermFrameIdx = 0xffffffff;
           u4PicNumX = 0;
        }
        else
        {
          u4PicNumX =(tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MaxLongTermFrameIdxPlus1);
        }
        for(i=0; i < tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
        {
          if((_ptFBufInfo[u4InstID][i].u4LongTermFrameIdx  >= u4PicNumX) &&
              ((_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC) || (_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC)))
          {
            _ptFBufInfo[u4InstID][i].ucTFldRefType = NREF_PIC;
            _ptFBufInfo[u4InstID][i].ucBFldRefType = NREF_PIC;
            _ptFBufInfo[u4InstID][i].ucFBufRefType = NREF_PIC;
          }
        }
        break;
      case 5:
        _ptCurrFBufInfo[u4InstID]->u4FrameNum = 0;
        _ptCurrFBufInfo[u4InstID]->i4PicNum = 0;
        _ptCurrFBufInfo[u4InstID]->i4TFldPicNum = 0;
        _ptCurrFBufInfo[u4InstID]->i4BFldPicNum = 0;
        if(tVerMpvDecPrm->ucPicStruct == TOP_FIELD)
        {
          _ptCurrFBufInfo[u4InstID]->i4TFldPOC = 0;
        }
        else if(tVerMpvDecPrm->ucPicStruct == BOTTOM_FIELD)
        {
          _ptCurrFBufInfo[u4InstID]->i4BFldPOC = 0;
        }
        else if(tVerMpvDecPrm->ucPicStruct == FRAME)
        {
          _ptCurrFBufInfo[u4InstID]->i4TFldPOC -= _ptCurrFBufInfo[u4InstID]->i4POC;
          _ptCurrFBufInfo[u4InstID]->i4BFldPOC -= _ptCurrFBufInfo[u4InstID]->i4POC;
          _ptCurrFBufInfo[u4InstID]->i4POC = (_ptCurrFBufInfo[u4InstID]->i4TFldPOC < _ptCurrFBufInfo[u4InstID]->i4BFldPOC)?
                                                       _ptCurrFBufInfo[u4InstID]->i4TFldPOC: _ptCurrFBufInfo[u4InstID]->i4BFldPOC;
        }

        vVerifyFlushBufRefInfo(u4InstID);
        break;
      case 6:
        tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4MemoryManagementControlOperation[u4Cnt] >> 8;
        u4PicNumX = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
        for(i=0; i < tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
        {
          if(i != tVerMpvDecPrm->ucDecFBufIdx)
          {
            if((_ptFBufInfo[u4InstID][i].ucFBufStatus == FRAME) && (_ptFBufInfo[u4InstID][i].u4LongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucFBufRefType == LREF_PIC))
            {
              vVerifyClrPicRefInfo(u4InstID, FRAME, i);
              i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
            }
            else if((_ptFBufInfo[u4InstID][i].ucFBufStatus != FRAME) &&
                       (((_ptFBufInfo[u4InstID][i].u4TFldLongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC))
                       || ((_ptFBufInfo[u4InstID][i].u4BFldLongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC))))
            {
              if((_ptFBufInfo[u4InstID][i].u4TFldLongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucTFldRefType == LREF_PIC))
              {
                vVerifyClrPicRefInfo(u4InstID, TOP_FIELD, i);
              }
              if((_ptFBufInfo[u4InstID][i].u4BFldLongTermFrameIdx == u4PicNumX) && (_ptFBufInfo[u4InstID][i].ucBFldRefType == LREF_PIC))
              {
                vVerifyClrPicRefInfo(u4InstID, BOTTOM_FIELD, i);
              }
              i = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; // break
            }
          }
        }

        if(fgIsFrmPic(u4InstID)) // 2 flds decoded
        {
          _ptCurrFBufInfo[u4InstID]->ucFBufRefType = LREF_PIC;
          _ptCurrFBufInfo[u4InstID]->ucTFldRefType = LREF_PIC;
          _ptCurrFBufInfo[u4InstID]->ucBFldRefType = LREF_PIC;
          _ptCurrFBufInfo[u4InstID]->u4LongTermFrameIdx = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
          _ptCurrFBufInfo[u4InstID]->u4TFldLongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
          _ptCurrFBufInfo[u4InstID]->u4BFldLongTermFrameIdx = tVerMpvDecPrm->SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
        }
        else if(tVerMpvDecPrm->ucPicStruct & TOP_FIELD) // 1 fld decoded
        {
          _ptCurrFBufInfo[u4InstID]->ucTFldRefType = LREF_PIC;
          _ptCurrFBufInfo[u4InstID]->u4LongTermFrameIdx = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
          _ptCurrFBufInfo[u4InstID]->u4TFldLongTermFrameIdx = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
          if(_ptCurrFBufInfo[u4InstID]->ucBFldRefType == LREF_PIC)
          {
            _ptCurrFBufInfo[u4InstID]->ucFBufRefType = LREF_PIC;
          }
        }
        else if(tVerMpvDecPrm->ucPicStruct & BOTTOM_FIELD) // 1 fld decoded
        {
          _ptCurrFBufInfo[u4InstID]->ucBFldRefType = LREF_PIC;
          _ptCurrFBufInfo[u4InstID]->u4LongTermFrameIdx = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
          _ptCurrFBufInfo[u4InstID]->u4BFldLongTermFrameIdx = _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->u4LongTermFrameIdx;
          if(_ptCurrFBufInfo[u4InstID]->ucTFldRefType == LREF_PIC)
          {
            _ptCurrFBufInfo[u4InstID]->ucFBufRefType = LREF_PIC;
          }
        }
        break;
    }
    u4Cnt ++;
  }
}

// *********************************************************************
// Function    : UCHAR bGetPicRefType(UINT32 u4InstID, UCHAR ucPicStruct)
// Description : get pic ref type
// Parameter   : UCHAR ucPicType: pic struct : FRAME, TOP_FIELD, BOTTOM_FIELD
// Return      : UCHAR ucRefType: pic ref type: NREF_PIC, SREF_PIC, LREF_PIC
// *********************************************************************
UCHAR bGetPicRefType(UINT32 u4InstID, UCHAR ucPicStruct)
{
  if(ucPicStruct == TOP_FIELD)
  {
    return _ptCurrFBufInfo[u4InstID]->ucTFldRefType;
  }
  else if(ucPicStruct == BOTTOM_FIELD)
  {
    return _ptCurrFBufInfo[u4InstID]->ucBFldRefType;
  }
  else//if(ucPicStruct == FRAME)
  {
    return _ptCurrFBufInfo[u4InstID]->ucFBufRefType;
  }
}





// *********************************************************************
// Function    : void vChkOutputFBuf(UINT32 u4InstID)
// Description : Output 1 frm buff in DPB when DPB full
// Parameter   :
// Return      : None
// *********************************************************************
void vChkOutputFBuf(UINT32 u4InstID)
{
  UINT32 u4MinPOCFBufIdx;
  VDEC_INFO_DEC_PRM_T *tVerMpvDecPrm;

  tVerMpvDecPrm = &_tVerMpvDecPrm[u4InstID];

    // needs to output
    do
    {
        u4MinPOCFBufIdx = ucVDecGetMinPOCFBuf(u4InstID, tVerMpvDecPrm, TRUE);

        if((u4MinPOCFBufIdx != 0xFF)
            && (_ptFBufInfo[u4InstID][u4MinPOCFBufIdx].eH264DpbStatus != H264_DPB_STATUS_EMPTY))
        {
            _ptFBufInfo[u4InstID][u4MinPOCFBufIdx].eH264DpbStatus = H264_DPB_STATUS_OUTPUTTED;
            vOutputPOCData(_ptFBufInfo[u4InstID][u4MinPOCFBufIdx].u4DecOrder);
#if 0
            u4MinPOCFBufIdx = ucVDecGetMinPOCFBuf(u4InstID, tMpvDecPrm, TRUE);
            if((u4MinPOCFBufIdx != 0xff)
                && (_ptFBufInfo[u4MinPOCFBufIdx].eH264DpbStatus != H264_DPB_STATUS_EMPTY))
            {
                //prH264DrvInfo->ucH264DpbOutputFbId = u4MinPOCFBufIdx;
            }
            else
            {
                u4MinPOCFBufIdx = 0xFF;
            }
#endif
            // check in next entry
        }
        else if((u4MinPOCFBufIdx != 0xFF)
            && (_ptFBufInfo[u4InstID][u4MinPOCFBufIdx].eH264DpbStatus == H264_DPB_STATUS_EMPTY))
        {
            u4MinPOCFBufIdx = 0xff;
        }

    }while(u4MinPOCFBufIdx != 0xff);


#if 0
  // Check if DPB full
  iMinPOC = 0x7fffffff;
  for(i=0; i<_tVerMpvDecPrm.SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum; i++)
  {
    if(_ptFBufInfo[i].ucFBufStatus == NO_PIC)
    {
      iMinPOC = 0x7fffffff;
      u4MinPOCFBufIdx = i;
      break;
    }
    // miew: need to take care of field empty
    else if((iMinPOC > _ptFBufInfo[i].i4POC) && fgIsNonRefFBuf(i))
    {
      iMinPOC = _ptFBufInfo[i].i4POC;
      u4MinPOCFBufIdx = i;
    }
  }

  // No free FBuf, output 1 fbuf is needed
  if(_ptFBufInfo[u4MinPOCFBufIdx].ucFBufStatus != NO_PIC)
  {
    vVerifyClrFBufInfo(u4MinPOCFBufIdx);
  }
#endif
}


// *********************************************************************
// Function    : void vVerifyClrPicRefInfo(UINT32 u4InstID, UCHAR ucPicType, UCHAR ucFBufIdx)
// Description : Clear picture info in frame buffer
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerifyClrPicRefInfo(UINT32 u4InstID, UCHAR ucPicType, UCHAR ucFBufIdx)
{
  if(ucPicType & TOP_FIELD)
  {
    _ptFBufInfo[u4InstID][ucFBufIdx].ucTFldRefType = NREF_PIC;
  }
  if(ucPicType & BOTTOM_FIELD)
  {
    _ptFBufInfo[u4InstID][ucFBufIdx].ucBFldRefType = NREF_PIC;
  }
  _ptFBufInfo[u4InstID][ucFBufIdx].ucFBufRefType = NREF_PIC;
}

// *********************************************************************
// Function    : BOOL fgIsH264VDecComplete(UINT32 u4InstID)
// Description : Check if VDec complete with interrupt
// Parameter   : None
// Return      : None
// *********************************************************************
BOOL fgIsH264VDecComplete(UINT32 u4InstID)
{
 	UINT32 u4MbX;
  	UINT32 u4MbY;
	UINT32 u4Temp = 0;
  	if(_fgVDecComplete[u4InstID]  || (_ucMVCType[u4InstID] == 2 && _fgVDecComplete[0]))
  	{
    	if (1 == _u4DualCoreEnable[0])
		{
			u4Temp = 1 + _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgFieldPicFlag;
			if (_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSPS->fgMbAdaptiveFrameFieldFlag
			&& (!_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSliceHdr->fgFieldPicFlag))
			{
				if (0 == ((_tVerMpvDecPrm[u4InstID].u4PicH/32) % 2))
				{
					vVDEC_HAL_H264_GetMbxMby(VDEC_VLD1, &u4MbX, &u4MbY);
				}
				else
				{
					vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
				}
			}
			else
			{
				if (0 == ((_tVerMpvDecPrm[u4InstID].u4PicH/u4Temp/16) % 2))
				{
					vVDEC_HAL_H264_GetMbxMby(VDEC_VLD1, &u4MbX, &u4MbY);
				}
				else
				{
					vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
				}
			}
		}
		else
		{
	  		vVDEC_HAL_H264_GetMbxMby(u4InstID,&u4MbX, &u4MbY);
		}

    	if(fgIsFrmPic(u4InstID))
    	{
      		if(u4MbX < ((_ptCurrFBufInfo[u4InstID]->u4W >> 4) -1) || (u4MbY < ((_ptCurrFBufInfo[u4InstID]->u4H >> 4) -1)))
      		{
        		return FALSE;
      		}
      		else
      		{
        		return TRUE;
      		}
    	}
    	else
    	{
      		if(u4MbX < ((_ptCurrFBufInfo[u4InstID]->u4W >> 4) -1) || u4MbY < ((_ptCurrFBufInfo[u4InstID]->u4H >> 5) -1))
      		{
        		return FALSE;
      		}
      		else
      		{
        		return TRUE;
      		}
    	}
  	}
  	return FALSE;
}

// *********************************************************************
// Function    : void vReadH264ChkSumGolden(UINT32 u4InstID)
// Description : write check sum in rec file
// Parameter   : None
// Return      : None
// *********************************************************************
void vReadH264ChkSumGolden(UINT32 u4InstID)
{
  vVDEC_HAL_H264_VDec_ReadCheckSum(u4InstID, &_u4DumpChksum[u4InstID][0]);
}

// *********************************************************************
// Function    : void vReadWMVChkSumGolden(UINT32 u4InstID)
// Description : write check sum in rec file
// Parameter   : None
// Return      : None
// *********************************************************************
void vReadWMVChkSumGolden(UINT32 u4InstID)
{
  vVDEC_HAL_WMV_VDec_ReadCheckSum(u4InstID, &_u4DumpChksum[u4InstID][0]);
}

// *********************************************************************
// Function    : void vReadMPEGChkSumGolden(UINT32 u4InstID)
// Description : write check sum in rec file
// Parameter   : None
// Return      : None
// *********************************************************************
void vReadMPEGChkSumGolden(UINT32 u4InstID)
{
  vVDEC_HAL_MPEG_VDec_ReadCheckSum(u4InstID, &_u4DumpChksum[u4InstID][0]);
}

void vPrintMpegChksum(UINT32 u4InstID)
{
    UINT32 i , u4Val;

    for(i = 72; i < 76 ; i ++)
    {
        u4Val = u4VDecReadVLD(u4InstID, i *4);
        UTIL_Printf("[R][VDEC]VLD(%d, 32'h%x)\n", i ,u4Val);
    }
    u4Val = u4VDecReadVLD(u4InstID, 251*4);
    UTIL_Printf("[R][VDEC]VLD(%d, 32'h%x)\n", i ,u4Val);

    for(i = 378;i <= 383; i ++)
    {
        u4Val = u4VDecReadMC(u4InstID, i * 4);
        UTIL_Printf("[R][VDEC]MC(%d, 32'h%x)\n", i ,u4Val);        
    }
    for(i = 148; i <= 149; i++)
    {
        u4Val = u4ReadReg(AVC_MV_REG_OFFSET0 + 4 * i);
        UTIL_Printf("[R][VDEC]AVC_MV(%d, 32'h%x\n)", i ,u4Val);
    }
}
void vReadDvChkSumGolden(UINT32 u4InstID)
{
  UINT32  u4Temp,u4Cnt;
  UINT32 u4VDecID;
  UINT32 *pu4CheckSum;

  u4VDecID = u4InstID;
  pu4CheckSum = &_u4DumpChksum[u4InstID][0];

  u4Temp = 0;
  *pu4CheckSum = u4VDecReadMC(u4VDecID, 0x5f4);
  pu4CheckSum ++;
  u4Temp ++;
  *pu4CheckSum = u4VDecReadMC(u4VDecID, 0x5f8);
  pu4CheckSum ++;
  u4Temp ++;
  *pu4CheckSum = u4VDecReadMC(u4VDecID, 0x608);
  pu4CheckSum ++;
  u4Temp ++;
  *pu4CheckSum = u4VDecReadMC(u4VDecID, 0x60c);
  pu4CheckSum ++;
  u4Temp ++;

  //MC  378~397
  for(u4Cnt=378; u4Cnt<=397; u4Cnt++)
  {
    *pu4CheckSum = u4VDecReadMC(u4VDecID, (u4Cnt<<2));
    pu4CheckSum ++;
    u4Temp ++;
  }

  //AVC VLD  165~179
  for(u4Cnt=165; u4Cnt<=179; u4Cnt++)
  {
    *pu4CheckSum = u4VDecReadAVCVLD(u4VDecID, (u4Cnt<<2));
      pu4CheckSum ++;
      u4Temp ++;
  }

  //MV  147~151
  for(u4Cnt=147; u4Cnt<=151; u4Cnt++)
  {
    *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));
    pu4CheckSum ++;
    u4Temp ++;
  }

  //IP  212
  *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (212 << 2));
  pu4CheckSum ++;
  u4Temp ++;

  //IQ  235~239
  for(u4Cnt=241; u4Cnt<=245; u4Cnt++)
  {
    *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));
    pu4CheckSum ++;
    u4Temp ++;
  }

  //IS  241~245
  for(u4Cnt=241; u4Cnt<=245; u4Cnt++)
  {
    *pu4CheckSum = u4VDecReadAVCMV(u4VDecID, (u4Cnt<<2));
    pu4CheckSum ++;
    u4Temp ++;
  }

  while(u4Temp < MAX_CHKSUM_NUM)
  {
    *pu4CheckSum = 0;
    pu4CheckSum ++;
    u4Temp ++;
  }
}

void vDvCompare(UINT32 u4InstID)
{
   vDvWrData2PC(u4InstID, _pucDumpYBuf[u4InstID]);
}

BOOL fgInitSem = TRUE;
void vVDecInit(UINT32 u4InstID)
{
    UINT32 u4InstId,i,j,u4NeedInstId;
    UINT32 u4PpsID;
    
    UTIL_Printf("VDEC[%d] Init\n",u4InstID);
    _fgWaitSub = FALSE;
    //for(u4InstId = 0; u4InstId < 3; u4InstId ++)
    {
        for(u4PpsID = 0; u4PpsID < MAX_H265_PPS_THD; u4PpsID ++)
        {
            for( i = 0; i < SCALING_LIST_SIZE_NUM; i++)
            {
                for( j = 0; j < SCALING_LIST_NUM; j++)
                {
                    if(u4PpsID < MAX_H265_SPS_THD)
                    {
                        _rSPSScalingListDeltaCoef[u4InstID][u4PpsID][i][j] = NULL;
                    }
                    _rPPSScalingListDeltaCoef[u4InstID][u4PpsID][i][j] = NULL;
                }
            }
        }
    }
    
    //for(u4InstId = 0; u4InstId < 4; u4InstId ++)
    {
        for (i = 0; i< 32; i++)
        {
            for (j = 0; j< 64; j++)
            {
                _rH265SPS[u4InstID][i].pShortTermRefPicSets[j] = NULL;                
            }
        }
    }

    if(fgInitSem)
    {
        VERIFY (x_sema_create(&hVdecOpenFileSem, X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
        VERIFY (x_sema_create(&hVdecDecDone[0], X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
        VERIFY (x_sema_create(&hVdecDecDone[1], X_SEMA_TYPE_BINARY, X_SEMA_STATE_LOCK) == OSR_OK);
        fgInitSem = FALSE;
        UTIL_Printf("VDEC create sem 0x%x vdec 0x%x 0x%x \n",hVdecOpenFileSem,hVdecDecDone[0],hVdecDecDone[1]);
    }
}

void vVDecDeInit(UINT32 u4InstID)
{
    UINT32 u4InstId,i,j;
    UINT32 u4PpsID;
    
    UTIL_Printf("VDEC[%d] Deinit\n",u4InstID);
    #if 0
    for(u4InstId = 0; u4InstId < 3; u4InstId += 2)
    {
        for(u4PpsID = 0; u4PpsID < MAX_H265_PPS_THD; u4PpsID ++)
        {
            for( i = 0; i < SCALING_LIST_SIZE_NUM; i++)
            {
                for( j = 0; j < SCALING_LIST_NUM; j++)
                {
                    if((u4PpsID < MAX_H265_SPS_THD) && (_rSPSScalingListDeltaCoef[u4InstID][u4PpsID][i][j] != NULL))
                    {
                        #if 1
                        x_free_aligned_verify_mem(_rSPSScalingListDeltaCoef[u4InstID][u4PpsID][i][j],0);
                        #else
                        vfree(_rSPSScalingListDeltaCoef[u4InstId][u4PpsID][i][j]);
                        #endif
                        _rSPSScalingListDeltaCoef[u4InstID][u4PpsID][i][j] = NULL;
                    }
                    
                    if(_rPPSScalingListDeltaCoef[u4InstID][u4PpsID][i][j] != NULL)
                    {
                        #if 1
                        x_free_aligned_verify_mem(_rPPSScalingListDeltaCoef[u4InstID][u4PpsID][i][j],0);
                        #else
                        vfree(_rPPSScalingListDeltaCoef[u4InstId][u4PpsID][i][j]);
                        #endif
                        _rPPSScalingListDeltaCoef[u4InstID][u4PpsID][i][j] = NULL;
                    }
                }
            }
        }
    }
    #else
	if(_u4CodecVer[u4InstID] == VDEC_H265)
        vFreeScaleList(u4InstID);
    if(_u4DualCoreEnable[u4InstID])
    {
        if(u4InstID == 0)
        {
            vFreeScaleList(VDEC_LAE0);
        }
        else if(u4InstID == 1)
        {
            vFreeScaleList(VDEC_LAE1);
        }
    }
    #endif
    
    UTIL_Printf("Release SPS\n");
    #if 0
    for(u4InstId = 0; u4InstId < 3; u4InstId += 2)
    {
        for (i = 0; i < 32; i++)
        {
            for (j = 0; j < 64; j++)
            {
                if(_rH265SPS[u4InstID][i].pShortTermRefPicSets[j] != NULL)
                {
                    #if 1
                    x_free_aligned_verify_mem(_rH265SPS[u4InstID][i].pShortTermRefPicSets[j],0);
                    #else
                    vfree(_rH265SPS[u4InstId][i].pShortTermRefPicSets[j]);
                    #endif
                    _rH265SPS[u4InstID][i].pShortTermRefPicSets[j] = NULL;                
                }
            }
        }
    }
    #else    
	if(_u4CodecVer[u4InstID] == VDEC_H265)
        vFreeRPS(u4InstID);
    if(_u4DualCoreEnable[u4InstID])
    {
        if(u4InstID == 0)
        {
            vFreeRPS(VDEC_LAE0);
        }
        else if(u4InstID == 1)
        {
            vFreeRPS(VDEC_LAE1);
        }
    }
    #endif
//    _fgWaitSub = FALSE;
}

UCHAR ucVDecAllocateVld(UINT32 u4InstID)
{
    if(_u4CodecVer[u4InstID] == VDEC_H265)
    {
        if(u4InstID == 0 || (_u4DualCoreEnable[u4InstID] == 1))
        {
            return VDEC_VLD0;
        }
        else if(u4InstID == 1)
        {
            return VDEC_VLD1;
        }
    }
    else
    {
        return VDEC_VLD0;
    }
}


#ifdef VPMODE
INT32 i4VPModeDecStart(UINT32 u4VDecID,VDEC_INFO_DEC_PRM_T *prDecPrm)
{

#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif

//VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if VDEC_DDR3_SUPPORT
    UINT32 u4DDR3_PicWdith = 0;
#endif
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
	UINT32 u4PicW,u4PicH,u4WithMB;
#endif

#if (CONFIG_DRV_LINUX_DATA_CONSISTENCY)
//    HalFlushInvalidateDCache();
#endif

	if (prDecPrm->ucDecFBufIdx == 2)
	{
		if(prMpegDecPrm->u4FRefBufIdx == 0)
       	{
           	prMpegDecPrm->rMpegFrameBufSa.u4Pic1YSa = prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa;
           	prMpegDecPrm->rMpegFrameBufSa.u4Pic1CSa = prMpegDecPrm->rMpegFrameBufSa.u4Pic2CSa;
		prMpegDecPrm->u4FRefBufIdx = 1;
		prDecPrm->ucDecFBufIdx = 0;
		_u4DecBufIdx[u4VDecID] = 0;
		_u4FRefBufIdx[u4VDecID] = 1;
       	}
       	else
       	{
          	prMpegDecPrm->rMpegFrameBufSa.u4Pic0YSa = prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa;
          	prMpegDecPrm->rMpegFrameBufSa.u4Pic0CSa = prMpegDecPrm->rMpegFrameBufSa.u4Pic2CSa;
          	prMpegDecPrm->u4FRefBufIdx = 0;
		prDecPrm->ucDecFBufIdx = 1;
		_u4DecBufIdx[u4VDecID] = 1;
		_u4FRefBufIdx[u4VDecID] = 0;
       	}

	}

//    vVDECSetDownScalerPrm(u4VDecID, &prDecPrm->rDownScalerPrm);

    vVDecWriteMC(u4VDecID, RW_MC_R1Y, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0YSa)) >> 9); // div 512
    vVDecWriteMC(u4VDecID, RW_MC_R1C, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0CSa)) >> 8); // div 256
    vVDecWriteMC(u4VDecID, RW_MC_R2Y, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1YSa)) >> 9); // div 512
    vVDecWriteMC(u4VDecID, RW_MC_R2C, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1CSa)) >> 8); // div 256
    vVDecWriteMC(u4VDecID, RW_MC_BY,  (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa)) >> 8); // div 256
    vVDecWriteMC(u4VDecID, RW_MC_BC,  (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2CSa)) >> 7); // div 128
    vVDecWriteMC(u4VDecID, RW_MC_BY1,  (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa)) >> 9); // div 256
    vVDecWriteMC(u4VDecID, RW_MC_BC1,  (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2CSa)) >> 8); // div 128
    vVDecWriteMC(u4VDecID, RW_MC_DIGY, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0YSa)) >> 9); // div 512
    vVDecWriteMC(u4VDecID, RW_MC_DIGC, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0CSa)) >> 8); // div 256
    vMCSetOutputBuf(u4VDecID, (UINT32)prDecPrm->ucDecFBufIdx, prMpegDecPrm->u4FRefBufIdx);

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
      #if VDEC_DDR3_SUPPORT
          u4DDR3_PicWdith = (((prDecPrm->u4PicBW + 63) >> 6) << 2);
          #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
          vVDecWriteMC(u4VDecID, RW_MC_PIC_W_MB, u4DDR3_PicWdith);
          #else
          vVDecWriteVLD(u4VDecID, RW_VLD_PIC_W_MB, u4DDR3_PicWdith);
          #endif
      #else
          #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
          vVDecWriteMC(u4VDecID, RW_MC_PIC_W_MB, ((prDecPrm->u4PicBW + 15)>> 4));
          vVDecWriteMC(u4VDecID, RW_MC_DDR3_EN, (u4VDecReadMC(u4VDecID, RW_MC_DDR3_EN)  & 0xFFFFFFFE));
      #else
          vVDecWriteVLD(u4VDecID, RW_VLD_PIC_W_MB, ((prDecPrm->u4PicBW + 15)>> 4));
      #endif
#endif
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    vVDecWriteMC(0, 0x5E4, (u4VDecReadMC(0, 0x5E4) |(0x1 <<12)) );
    vVDecWriteMC(0, 0x660, (u4VDecReadMC(0, 0x660) |(0x80000000)) );
    #ifndef VDEC_PIP_WITH_ONE_HW
    vVDecWriteMC(1, 0x5E4, (u4VDecReadMC(1, 0x5E4) |(0x1 <<12)) );
    vVDecWriteMC(1, 0x660, (u4VDecReadMC(1, 0x660) |(0x80000000)) );
    #endif
#endif

    if (prMpegDecPrm->rMpegPpInfo.fgPpEnable)
    {
        UINT32 u4MBqp = 0;

        vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, 1);
        vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, u4AbsDramANc(prMpegDecPrm->rMpegPpInfo.u4PpYBufSa) >> 9);
        vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, u4AbsDramANc(prMpegDecPrm->rMpegPpInfo.u4PpCBufSa) >> 8);
        vVDecWriteMC(u4VDecID, RW_MC_PP_MB_WIDTH, (prDecPrm->u4PicW + 15) >> 4);

        u4MBqp = (prMpegDecPrm->rMpegPpInfo.au1MBqp[0] & 0x1F) | ((UINT32)(prMpegDecPrm->rMpegPpInfo.au1MBqp[1] & 0x1F) << 8) \
                       | ((UINT32)(prMpegDecPrm->rMpegPpInfo.au1MBqp[2] & 0x1F) << 16) | ((UINT32)(prMpegDecPrm->rMpegPpInfo.au1MBqp[3] & 0x1F) << 24);
        vVDecWriteMC(u4VDecID, RW_MC_PP_QP_TYPE, u4MBqp);

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
        vVDecWriteMC(u4VDecID, RW_MC_PP_DBLK_MODE, DBLK_Y+DBLK_C);
        vVDecWriteMC(u4VDecID, RW_MC_PP_WB_BY_POST, 0); // wirte MC out and PP out
        if (prMpegDecPrm->rMpegPpInfo.fgPpDemoEn)
        {
            vVDecWriteMC(u4VDecID, 0x658, ((u4VDecReadMC(u4VDecID, 0x658)&0xFFFFFFFE)|0x1)); // partial deblocking
            vVDecWriteMC(u4VDecID, 0x65C, ((((prDecPrm->u4PicH + 15) >> 4) - 1) << 24) | ((((prDecPrm->u4PicW + 15) >> 5) - 1) << 8)); // XY end MB
        }
        else
        {
            vVDecWriteMC(u4VDecID, 0x658, (u4VDecReadMC(u4VDecID, 0x658)&0xFFFFFFFE));
        }
#else
        vVDecWriteMC(u4VDecID, RW_MC_PP_DBLK_MODE, DBLK_Y+DBLK_C);
        //vVDecWriteMC(u4VDecID, RW_MC_PP_WB_BY_POST, 0); // wirte MC out and PP out
#endif
        //vVDecWriteMC(u4VDecID, RW_MC_PP_QP_TYPE, 0x00000114);
        //vVDecWriteMC(u4VDecID, RW_MC_PP_WB_BY_POST, 0); // wirte MC out and PP out
        vVDecWriteMC(u4VDecID, RW_MC_PP_X_RANGE, ((prDecPrm->u4PicW + 15) >> 4) - 1);
        vVDecWriteMC(u4VDecID, RW_MC_PP_Y_RANGE, (((prDecPrm->u4PicH + 15) >> 4) >> (prDecPrm->ucPicStruct != 3)) - 1);
        //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_2, 0x6E00);
        //vVDecWriteMC(u4VDecID, RW_MC_PP_MODE, H264_MODE);
    }
    else
    {
        vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, 0);
    }

	#ifndef VPMODE_FOR_5881
    vVDecWriteVLD(u4VDecID, RW_VLD_PSUPCTR, ((prDecPrm->u4PicW * prDecPrm->u4PicH) >> 8) + 1);
    vVDecWriteVLD(u4VDecID, RW_VLD_PARA, 0xC0500000); //Frame Picture + VP ???
	#endif

    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)
//    vVDecWriteVLD(u4VDecID, RW_VLD_PICSZ, ((prDecPrm->u4PicH) << 16) + (prDecPrm->u4PicW >> 4));

	u4PicW = ((prDecPrm->u4PicW +15)>>4)<<4;
	u4PicH = ((prDecPrm->u4PicH + 15)>>4)<<4;
	u4WithMB = ((prDecPrm->u4PicW +15)>>4);
	vVDecWriteVLDTOP(u4VDecID,RW_TOPVLD_WMV_PICSIZE,u4PicH<<16|u4PicW);
	#ifndef VPMODE_FOR_5881
	vVDecWriteVLD(u4VDecID, RW_VLD_PICSZ, (prDecPrm->u4PicH)<<16);
	vVDecWriteVLD(u4VDecID, RW_VLD_DIGMBSA, u4WithMB);
//	vVDecWriteTopVLD(u4VDecID,RW_TOPVLD_WMV_PICSIZE_MB,(((prDecPrm->u4PicW+ 15)>>4) -1) | ((((prDecPrm->u4PicH + 15)>>4) - 1)<<16));
	vVDecWriteVLD(u4VDecID, RW_VLD_MBROWPRM,  0x1ff );
	#endif
    #else
	#ifndef VPMODE_FOR_5881
    vVDecWriteVLD(u4VDecID, RW_VLD_PICSZ, ((prDecPrm->u4PicH + 15) << 16) + (prDecPrm->u4PicW >> 4));
    vVDecWriteVLD(u4VDecID, RW_VLD_MBROWPRM,  ( ((prDecPrm->u4PicH + 15) >> 4 ) - 1) << 16);
	#endif
    #endif

    // addr swap mode
    vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP, prDecPrm->ucAddrSwapMode);

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
     vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL,
                 ((u4VDecReadMC(u4VDecID, RW_MC_NBM_CTRL)  & 0xFFFFFFF8) |prDecPrm->ucAddrSwapMode));
#endif

/*
    vVDecWriteMC(u4VDecID, RW_MC_HREFP, 0);
    vVDecWriteMC(u4VDecID, RW_MC_DIGWD, ((prDecPrm->u4PicW + 15) >> 4));
    vVDecWriteVLD(u4VDecID, RW_VLD_DIGMBSA, 0);
    vVDecWriteVLD(u4VDecID, RW_VLD_SCALE, 0);//(random(3)<<24) |(random(3)<<16));
    vVDecWriteVLD(u4VDecID, RW_VLD_DIGMBYOFF, 0);
*/
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_WIDTH, ((prDecPrm->u4PicW + 15) >> 4) << 4);
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_HEIGHT,  ((prDecPrm->u4PicH + 31) >> 5) << 5);

	#ifndef VPMODE_FOR_5881
    vVDecWriteVLD(u4VDecID, RW_VLD_PROC, VLD_RTERR + VLD_PDHW + VLD_PSUP +
              (prDecPrm->u4PicW >> 4));
	#endif
#ifdef VPMODE_FOR_5881
	vVDecWriteVLDTOP(u4VDecID,26*4,(((prDecPrm->u4PicW+ 15)>>4) -1) | ((((prDecPrm->u4PicH + 15)>>4) - 1)<<16));
	vVDecWriteVLDTOP(u4VDecID,36*4,u4VDecReadVLDTOP(u4VDecID,36*4)|(1<<1)); //turn on vp mode xiaolei.li
	vVDecWriteVLDTOP(u4VDecID,36*4,u4VDecReadVLDTOP(u4VDecID,36*4)|(1<<0)); //trigger vp mode xiaolei.li
#endif

    return HAL_HANDLE_OK;
}
#endif

void vAVCDumpChkSum(void)
{
	UINT32 i,u4Val,u4VDecID = 0;
	UTIL_Printf("read AVCVLD \n");
	for(i = 165; i<180; i++)
	{
		u4Val = u4VDecReadAVCVLD(u4VDecID, i<<2);
		UTIL_Printf("%d (0x%x)  = 0x%4x\n",i, (i<<2),u4Val);
	}
	UTIL_Printf("read AVC MC \n");
	for(i = 147; i<152; i++)
	{
		u4Val = u4VDecReadAVCMV(u4VDecID, i<<2);
		UTIL_Printf("%d (0x%x)  = 0x%4x\n",i, (i<<2),u4Val);
	}

	UTIL_Printf("read AVC MC \n");
	for(i = 378; i<397; i++)
	{
		u4Val = u4VDecReadMC(u4VDecID, i<<2);
		UTIL_Printf("%d (0x%x)  = 0x%4x\n",i, (i<<2),u4Val);
	}



}

extern UCHAR *_pucRegister[2];
void vPrintDumpReg(void)
{

    UINT32 u4Val,u4Cnt;
    UINT32 u4InstID = 0;
//    UTIL_Printf("Before Decode!\n");
    for(u4Cnt = 33; u4Cnt < 40; u4Cnt++)
    {
    	u4Val = ((UINT32 *)(_pucRegister[u4InstID]))[u4Cnt] ;
       UTIL_Printf("%d (0x%x)  = 0x%4x\n",u4Cnt, (u4Cnt<<2),u4Val);
    }
    for(u4Cnt = 42; u4Cnt < 71; u4Cnt++)
    {
    	u4Val  = ((UINT32 *)(_pucRegister[u4InstID]))[u4Cnt] ;
       UTIL_Printf("%d (0x%x)  = 0x%4x\n",u4Cnt, (u4Cnt<<2),u4Val);
    }
    for(u4Cnt = 112; u4Cnt < 131; u4Cnt++)
    {
    	u4Val  = ((UINT32 *)(_pucRegister[u4InstID]))[u4Cnt] ;
       UTIL_Printf("%d (0x%x)  = 0x%4x\n",u4Cnt, (u4Cnt<<2),u4Val);
    }
    for(u4Cnt = 192; u4Cnt < 256; u4Cnt++)
    {
    	u4Val = ((UINT32 *)(_pucRegister[u4InstID]))[u4Cnt] ;
       UTIL_Printf("%d (0x%x)  = 0x%4x\n",u4Cnt, (u4Cnt<<2),u4Val);
    }

    UTIL_Printf("MC register data \n");
    for(u4Cnt = 0; u4Cnt < 500; u4Cnt++)
    {
    	u4Val = ((UINT32 *)(_pucRegister[u4InstID]))[u4Cnt + 0x100] ;
       UTIL_Printf("%d (0x%x)  = 0x%4x\n",u4Cnt, (u4Cnt<<2),u4Val);
    }
    UTIL_Printf("Dump end!\n");

}

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
#define SRAMSZ	52*1024
#define SRAMWRTCMD  (1 << 16)
#define SRAMREDCMD  (0 << 16)

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8561))
void vWriteSram(UINT32 u4InstID,UINT32 u4SramAddr,UINT32 u4SramValue)
{
    UINT32 u4Temp;
    u4Temp = (u4VDecReadMC(u4InstID,0x93C) & (0xffffc000)) |SRAMWRTCMD;
    u4Temp |= u4SramAddr;
//    UTIL_Printf("Write SRAM 0x93c para = 0x%x,wrdata = 0x%x\n",u4Temp,u4SramValue);
    vVDecWriteMC(u4InstID,0x93C,u4Temp);
    vVDecWriteMC(u4InstID,0x940,u4SramValue);//set sram data
}
#else
void vWriteSram(UINT32 u4InstID,UINT32 u4SramAddr,UINT32 u4SramValue)
{
	UINT32 u4Temp;
	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)|(1<<0));//enable of sram and cs of sram
	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)&(~(0x3fff<<12)));//set sram addr
	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C) | (u4SramAddr<<12));//set sram addr
	vVDecWriteMC(u4InstID,0x940,u4SramValue);//set sram data
	u4Temp = (1<<4)|(1<<8);
	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C) | u4Temp);//enable write
	vVDecWriteMC(u4InstID,0x93C,0);//clear all
}
#endif

#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8561))
UINT32 u4ReadSram(UINT32 u4InstID,UINT32 u4SramAddr)
{
    UINT32 u4RegVal,u4Temp = 0;
    u4Temp = u4VDecReadMC(u4InstID,0x93C) & (~(SRAMWRTCMD)) & (0xffffc000);
    u4Temp |= u4SramAddr;
//    UTIL_Printf("Read SRAM 0x93c para = 0x%x\n",u4Temp);
    vVDecWriteMC(u4InstID,0x93C,u4Temp);
    u4RegVal = u4VDecReadMC(u4InstID,0x940);
//    UTIL_Printf("0X940(2) = 0x%x\n",u4RegVal);
    return u4RegVal;
}
#else
UINT32 u4ReadSram(UINT32 u4InstID,UINT32 u4SramAddr)
{
	UINT32 u4RegVal;
	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)|(1<<0)|(1<<4));//enable of sram and cs of sram
       vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)&(~(0x3fff<<12)));//set sram addr
	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C) | u4SramAddr<<12);//set sram addr
	u4RegVal = u4VDecReadMC(u4InstID,0x944);
//	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)&(~(1<<0))&(~(1<<4)));//disable sram read
	vVDecWriteMC(u4InstID,0x93C,0);
	return u4RegVal;
}
#endif

void vDumpSram(UINT32 u4InstID)
{
	UINT32 u4Mcstart,u4SramAddr,u4RegVal,u4ReadSize,u4temp;
	UCHAR fpDumpFile[100] = "d:\\ChkFolder\\sram";
//	UCHAR ucTempBuff[30];
	UCHAR *fpRear = ".bin";
	FILE *pFile = NULL;

	u4temp = strlen(fpDumpFile);
	u4temp += sprintf(fpDumpFile+u4temp,"%d",_u4FileCnt[u4InstID]);
	u4temp += sprintf(fpDumpFile+u4temp,"%s",fpRear);
//	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)|(1<<0)|(1<<4));//enable of sram and cs of sram
       vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)|(1<<0));//enable of sram and cs of sram
       vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)|(1<<4));//enable of sram and cs of sram
       UTIL_Printf("Before read SRAM MC 0x93C = 0x%x \n",u4VDecReadMC(u4InstID,0x93C));
	for(u4Mcstart = 0;  u4Mcstart <13312; u4Mcstart++)
	{
		u4SramAddr = (u4Mcstart) << 12;
		vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)&(~(0x3fff<<12)));//set sram addr
		vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C) | u4SramAddr);//set sram addr
//		x_thread_delay(2);
		u4RegVal = u4VDecReadMC(u4InstID,0x944);
		((UINT32*)(_pucDumpSRAMBuf[u4InstID]))[u4Mcstart] = u4RegVal;
	}

//	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)&(~(1<<0))&(~(1<<4)));//disable sram read
	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)&(~(1<<4)));//disable sram read
	vVDecWriteMC(u4InstID,0x93C,u4VDecReadMC(u4InstID,0x93C)&(~(1<<0)));//disable sram read
	UTIL_Printf("After read SRAM MC 0x93C = 0x%x \n",u4VDecReadMC(u4InstID,0x93C));

	pFile = fopen(fpDumpFile,"w+");
	if(pFile == NULL)
	{
		UTIL_Printf("Create file error !\n");
	}
	#if 0
	for(u4Mcstart = 0;  u4Mcstart <13312; u4Mcstart++)
	{
		ucLen = 0;
		ucLen = sprintf(ucTempBuff,"%x ",(_pucDumpSRAMBuf[u4InstID])[u4Mcstart*4 + 3]);
		ucLen += sprintf(ucTempBuff+ucLen,"%x ",(_pucDumpSRAMBuf[u4InstID])[u4Mcstart*4 + 2]);
		ucLen += sprintf(ucTempBuff+ucLen,"%x ",(_pucDumpSRAMBuf[u4InstID])[u4Mcstart*4 + 1]);
		ucLen += sprintf(ucTempBuff+ucLen,"%x",(_pucDumpSRAMBuf[u4InstID])[u4Mcstart*4 ]);
		fseek(pFile,SEEK_CUR,0);
		fwrite((char*)(_pucDumpSRAMBuf[u4InstID] + u4Mcstart*4),1,strlen(ucTempBuff),pFile);
	}
	#endif

	u4ReadSize = fwrite ((char* )(_pucDumpSRAMBuf[u4InstID]), 1, SRAMSZ, pFile);

	UTIL_Printf("read file len = %d \n",u4ReadSize);
	fclose(pFile);
}

void vVDecBufferFlush(UINT32 u4InstID,UINT32 u4BufIdx)
{
    switch(u4BufIdx)
    {
        case 0:
            BSP_InvDCacheRange((UINT32)(_pucPic0Y[u4InstID]),GOLD_Y_SZ);
            BSP_InvDCacheRange((UINT32)(_pucPic0C[u4InstID]),GOLD_C_SZ);
			vVDec_FlushDCacheRange((UINT32)(_pucPic0Y[u4InstID]),GOLD_Y_SZ);
		 	vVDec_FlushDCacheRange((UINT32)(_pucPic0C[u4InstID]),GOLD_C_SZ);
			break;
        case 1:
            BSP_InvDCacheRange((UINT32)(_pucPic1Y[u4InstID]),GOLD_Y_SZ);
            BSP_InvDCacheRange((UINT32)(_pucPic1C[u4InstID]),GOLD_C_SZ);
			vVDec_FlushDCacheRange((UINT32)(_pucPic1Y[u4InstID]),GOLD_Y_SZ);
		 	vVDec_FlushDCacheRange((UINT32)(_pucPic1C[u4InstID]),GOLD_C_SZ);
            break;
        case 2:
            BSP_InvDCacheRange((UINT32)(_pucPic2Y[u4InstID]),GOLD_Y_SZ);
            BSP_InvDCacheRange((UINT32)(_pucPic2C[u4InstID]),GOLD_C_SZ);
			vVDec_FlushDCacheRange((UINT32)(_pucPic2Y[u4InstID]),GOLD_Y_SZ);
		 	vVDec_FlushDCacheRange((UINT32)(_pucPic2C[u4InstID]),GOLD_C_SZ);
            break;
        default:
            break;
    }
}

#endif

#ifdef MPEG_CRC_ENABLE
BOOL fgYcrc = FALSE;
BOOL fgCcrc = FALSE;

void vMPEG_CRC_Chk(UINT32 u4InstID)
{

    UINT32 u4HWresult[8],reg;
    UCHAR *pGoldenY,*pGoldenC;
    BOOL fgResult = TRUE;
    if(fgYcrc == FALSE)
    {
        _tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;
        _tFBufFileInfo[u4InstID].pucTargetAddr = u4YCrcGolden[u4InstID];
        _tFBufFileInfo[u4InstID].u4TargetSz = 0x40000;
        _tFBufFileInfo[u4InstID].u4FileLength = 0;
		#ifdef SEMI_RING_FIFO
		_tFBufFileInfo[u4InstID].u4BSLenOffset = 0;
		_tFBufFileInfo[u4InstID].ucVFIFOEnough = 1;
		#endif
        #ifdef IDE_READ_SUPPORT
        fgYcrc = fgPureOpenIdeFile( _bFileStr1[u4InstID][14],"r+b", &_tFBufFileInfo[u4InstID]);
        #else
        fgYcrc = fgOpenFile(u4InstID, _bFileStr1[u4InstID][14],"r+b", &_tFBufFileInfo[u4InstID]);
        #endif
   }

   if(fgCcrc == FALSE)
   {
        _tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;
        _tFBufFileInfo[u4InstID].pucTargetAddr = u4CCrcGolden[u4InstID];
        _tFBufFileInfo[u4InstID].u4TargetSz = 0x40000;
        _tFBufFileInfo[u4InstID].u4FileLength = 0;
		#ifdef SEMI_RING_FIFO
		_tFBufFileInfo[u4InstID].u4BSLenOffset = 0;
		_tFBufFileInfo[u4InstID].ucVFIFOEnough = 1;
		#endif
        #ifdef IDE_READ_SUPPORT
        fgCcrc = fgPureOpenIdeFile( _bFileStr1[u4InstID][15],"r+b", &_tFBufFileInfo[u4InstID]);
        #else
        fgCcrc = fgOpenFile(u4InstID, _bFileStr1[u4InstID][15],"r+b", &_tFBufFileInfo[u4InstID]);
        #endif
   }
   #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
   BSP_CleanDCacheRange((UINT32)(u4CCrcGolden[u4InstID]),0X40000);
   BSP_CleanDCacheRange((UINT32)(u4YCrcGolden[u4InstID]),0X40000);
   BSP_InvDCacheRange((UINT32)(u4CCrcGolden[u4InstID]),0X40000);
   BSP_InvDCacheRange((UINT32)(u4YCrcGolden[u4InstID]),0X40000);
   #endif
   //Get HW CRC
   for(reg = 2; reg < 10; reg++)
   {
        u4HWresult[reg - 2] = u4VDecReadCRC(u4InstID, (reg << 2));
//      UTIL_Printf("%d (0x%x) = 0x%4x\n", reg, (reg<<2), u4HWresult[reg - 2]);
   }

   //compare crc
   pGoldenY = u4YCrcGolden[u4InstID] + 16*_u4FileCnt[u4InstID];
   pGoldenC = u4CCrcGolden[u4InstID] + 16*_u4FileCnt[u4InstID];

   for(reg = 0; reg < 4; reg++)
   {
      if(*((UINT32*)pGoldenY) != (u4HWresult[reg]))
      {
        UTIL_Printf("Y CRC CMP ERR! 0x%x and 0x%x\n",*((UINT32*)pGoldenY),(u4HWresult[reg]));
        fgResult = FALSE;
      }
      pGoldenY +=4;
      if(*((UINT32*)pGoldenC) != (u4HWresult[reg + 4]))
      {
        UTIL_Printf("CbCr CRC CMP ERR!0x%x and 0x%x\n",*((UINT32*)pGoldenC),(u4HWresult[reg + 4]));
        fgResult = FALSE;
      }
      pGoldenC += 4;
   }

   if(fgResult)
    UTIL_Printf("Compare CRC ok ===> frame %d\n",_u4FileCnt[u4InstID]);
   else
   {
    UTIL_Printf("Compare CRC error ===> frame %d\n",_u4FileCnt[u4InstID]);
    _u4VerBitCount[u4InstID] = 0xffffffff;
   }

   if(_tFBufFileInfo[u4InstID].u4FileLength <= (UINT32)pGoldenC - (UINT32)(u4CCrcGolden[u4InstID]))
   {
     if(fgResult)
     {
        UTIL_Printf("Compare finish ===> frame %d\n",_u4FileCnt[u4InstID] - 1);
     }
     _u4VerBitCount[u4InstID] = 0xffffffff;
   }
   _u4FileCnt[u4InstID]++;
}
#endif

#if VDEC_VFY_TEST_POWER_DOWN_CTRL
BOOL _fgTestPowerDownCtrlDone = FALSE;
BOOL fgTestPowerDownCtrl(UINT32 u4InstID)
{
  if (TRUE == _fgTestPowerDownCtrlDone)
  {
    return TRUE;
  }
  else
  {
    _fgTestPowerDownCtrlDone = TRUE;
  }

  if (VDEC_H264 == _u4CodecVer[u4InstID])
  {
    VDEC_INFO_H264_INIT_PRM_T rH264VDecInitPrm;
    VDEC_INFO_H264_BS_INIT_PRM_T rH264BSInitPrm;

    UTIL_Printf("[VDEC%d] fgTestPowerDownCtrl, H264, RW_PDN_CRTL2 register value:0x%x, 0\n", u4VDecReadDV(u4InstID, RW_PDN_CRTL2));

    vVDEC_HAL_H264_VDec_PowerDown(u4InstID);

    rH264BSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rH264BSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID];
    rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    #ifndef  RING_VFIFO_SUPPORT
    rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    #else
    //  rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
    rH264BSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
    #endif
    rH264BSInitPrm.u4PredSa = /*PHYSICAL*/((UINT32)_pucPredSa[u4InstID]);

    UTIL_Printf("[VDEC%d] fgTestPowerDownCtrl, H264, RW_PDN_CRTL2 register value:0x%x, 1\n", u4InstID, u4VDecReadDV(u4InstID, RW_PDN_CRTL2));

    if (HAL_HANDLE_OK != i4VDEC_HAL_H264_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rH264BSInitPrm))
    {
      UTIL_Printf("[VDEC%d] fgTestPowerDownCtrl, H264, cannot init BS after power down, result OK!!\n", u4InstID);
    }
    else
    {
      UTIL_Printf("[VDEC%d] fgTestPowerDownCtrl, H264, can init BS after power down, result NG!!\n", u4InstID);
    }

    UTIL_Printf("[VDEC%d] fgTestPowerDownCtrl, H264, RW_PDN_CRTL2 register value:0x%x, 2\n", u4InstID, u4VDecReadDV(u4InstID, RW_PDN_CRTL2));

    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.ucMaxFBufNum = 0xff;
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicW = 0;
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.rLastInfo.u4LastPicH = 0;
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSEI = &_rSEI[u4InstID];
    _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prFGTPrm= &_rFGTPrm[u4InstID];
    _u4TotalDecFrms[u4InstID] = 0;
    rH264VDecInitPrm.u4FGDatabase = (UINT32)_pucFGDatabase[u4InstID];
    rH264VDecInitPrm.u4CompModelValue = (UINT32)(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecH264DecPrm.prSEI->pucCompModelValue);
    rH264VDecInitPrm.u4FGSeedbase = (UINT32)_pucFGSeedbase[u4InstID];
    i4VDEC_HAL_H264_InitVDecHW(u4InstID);

    UTIL_Printf("[VDEC%d] fgTestPowerDownCtrl, H264, RW_PDN_CRTL2 register value:0x%x, 3\n", u4InstID, u4VDecReadDV(u4InstID, RW_PDN_CRTL2));

    if (HAL_HANDLE_OK == i4VDEC_HAL_H264_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rH264BSInitPrm))
    {
      UTIL_Printf("[VDEC%d] fgTestPowerDownCtrl, H264, can init BS after power on, result OK!!\n", u4InstID);
    }
    else
    {
      UTIL_Printf("[VDEC%d] fgTestPowerDownCtrl, H264, cannot init BS after power on, result NG!!\n", u4InstID);
    }

    UTIL_Printf("[VDEC%d] fgTestPowerDownCtrl, H264, RW_PDN_CRTL2 register value:0x%x, 4\n", u4InstID, u4VDecReadDV(u4InstID, RW_PDN_CRTL2));
  }

  return TRUE;
}
#endif
