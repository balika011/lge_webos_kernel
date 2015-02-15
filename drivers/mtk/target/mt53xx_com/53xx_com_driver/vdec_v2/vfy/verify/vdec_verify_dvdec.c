#include "vdec_verify_mpv_prov.h"
#include "vdec_verify_dvdec.h"
#include "vdec_hal_if_common.h"
#include "vdec_hal_if_wmv.h"
#include "vdec_hal_if_mpeg.h"
#include "vdec_hal_if_h264.h"
#include "vdec_info_dv.h"
#include "vdec_hw_common.h"
#include "vdec_hw_h264.h"
//#include "vdec_hw_dvdec.h"
//#include "Nucleus.h"
#include <string.h>
#include <stdio.h>

//SofeWare DeMuxer
//#include "SWDMX.h"

//2005.08.02 Ted Hu, For MT8108 DV decode Test
//           Remerber to disable all in normal operation
//#define MT8108_PASS_TEST
//#define MT8108_VLCMD_TEST
//#define MT8108_FORCE_TO_422
#define MT8108_FORCE_TO_420

//#ifdef MT8108_PASS_TEST
  #define PASS_1    0x1
  #define PASS_2    0x2
  #define PASS_3    0x3
  #define PASS_AUTO 0x4

  UINT32 _dwPASS = PASS_AUTO;
//#endif

//#ifdef MT8108_VLCMD_TEST
  #define VLCMD_OFF  0x0
  #define VLCMD_ON   0x1
  #define VLCMD_AUTO 0x2

  UINT32 _dwVLCMD = VLCMD_AUTO;
//#endif

#if defined(ENABLE_SOFTWARE_DV_DEMUXER)
  extern UINT32 dwNtscPalDetect;
#endif


//For Clean Video
UINT32 dwIdleNotify_8520  = 0;

TDvInfo          _tDvInfo_8520;
UINT32			_dwVLDID_8520;
UINT32			_dwDvFormat;
UINT32			_dwDvFrmSize;
UINT32			_dwDvCbCrFormat;
extern BYTE bDvCmdConnect_8520(UINT32 dArrayIdx);

/////////////////// Global variables in this file /////////////////////
BOOL  fgDecodeNormalPlayRecord_8520 = TRUE;
INT32 i4DecodeDelayFrameRecord_8520 = 0;
INT32 i4DecodeDelayFrameCount_8520  = 0;
BOOL fgIdleEnable_8520              = FALSE;
///////////////////////////////////////////////////////////////////////

//*********************************************************************
//Function    : void vDVDecodeShowFirstBlankVideo(UINT32 dwFormat, UINT32 dwColor)
//Description : Set First Blank Video to Main Video
//Parameter   : dwFormat : DMX_NTSC61834/DMX_PAL61834/DMX_NTSC314M/DMX_PAL314M
//              dwColor  : DMX_BLANK_BLACK
//Return      : N/A
//*********************************************************************
/*
void vDVDecodeShowFirstBlankVideo(UINT32 dwFormat, UINT32 dwColor)
{
  UINT32 dY, dC;
  
  //Set Spec, cause we need a P/N spec
  //If the it has already set spec, use old spec, otherwise use our spec
  if(_tDvDmx.dwDVSpec == DV_UNKNOW)
  {
    #if DV_DECODE_SHOE_BLANK_DEBUG	
      PRINTF("[DV DEC] Set Spec to %x", dwFormat);
    #endif
    
    fgDmxSetDVSpec(dwFormat);
    _tDvInfo_8520.wFrm_Buf_Ns = dwVsvFrmBufNs(_tDvInfo_8520.dwArrayIdx);
    _tDvInfo_8520.pVFBufArray = prVsvGetFBufAddrArray(_tDvInfo_8520.dwArrayIdx);    
  }
  else
  {
    #if DV_DECODE_SHOE_BLANK_DEBUG	
      PRINT("[DV DEC] Use old Spec");
    #endif
  }	

  //Set a Blank Video top Main Video
  
  //Step.1 Get a Buffer from VSV
  _dwDvCurDecIdx = dAcquireBuf();
  
  #if DV_DECODE_SHOE_BLANK_DEBUG	
    PRINTF("[DV DEC] Get DEC Idx = %d", _dwDvCurDecIdx);
  #endif
      
  //Step.2 Fill Blank Y&C
  vVsvGetFBufAddr(_dwDvCurDecIdx, &dY, &dC, _tDvInfo_8520.dwArrayIdx);
  
  if( dY != 0 && dC != 0)
  {
    #ifdef MT8108_FORCE_TO_422
    fgUtMemSet((BYTE *)dY, VSV_FBUF_422Y_SZ, 0x10101010);
    fgUtMemSet((BYTE *)dC, VSV_FBUF_422C_SZ, 0x80808080);  
    #else
      fgUtMemSet((BYTE *)dY, VSV_FBUF_Y_SZ, 0x10101010);
      fgUtMemSet((BYTE *)dC, VSV_FBUF_C_SZ, 0x80808080);  
    #endif
    
    #if DV_DECODE_SHOE_BLANK_DEBUG	
      PRINTF("[DV DEC]  Y = 0x%x, C = 0x%x", dY, dC);
    #endif
  }
  else 
  {
    #if DV_DECODE_SHOE_BLANK_DEBUG	
      PRINTF("[DV DEC]  Y = 0x%x, C = 0x%x", dY, dC);
    #endif    
  }	
    
  //Step.3 Set this Y&C address to VSV
  if(fgVsvSetNextDispBuf(_dwDvCurDecIdx, _tDvInfo_8520.dwArrayIdx) == FALSE)
  {
    #if DV_DECODE_SHOE_BLANK_DEBUG	
      PRINT("[DV DEC]  Set Next Display FAIL");
    #endif    
  }
  else
  {
    #if DV_DECODE_SHOE_BLANK_DEBUG	
      PRINT("[DV DEC]  Set Next Display SUCCESS");
    #endif     
  }	
 
}	
*/
//*********************************************************************
//Function    : BOOL fgDVDecodeGetIdleStatus_8520(void)
//Description : Get Idle status
//Parameter   : N/A
//Return      : TRUE  : Idle Enable
//              FALSE : Normal Play
//*********************************************************************
BOOL fgDVDecodeGetIdleStatus_8520(void)
{
  return fgIdleEnable_8520;
}

//*********************************************************************
//Function    : void vDVDecodeIdle_8520(BOOL fgEnable)
//Description : Stop DV Decode to Main Video
//Parameter   : fgEnable : TRUE  Idle Decode
//                         FALSE Normal Decode
//Return      : N/A
//*********************************************************************
void vDVDecodeIdle_8520(BOOL fgEnable)
{
  if(fgEnable == TRUE)
  {
    fgIdleEnable_8520 = TRUE;
  }
  else
  {
    fgIdleEnable_8520 = FALSE;	
  }		
}
	
//*********************************************************************
//Function    : void vDVDecodeSetMode_8520(BOOL fgNormalPlay, INT32 i4DelayFrame)
//Description : Set DV Decode mode
//Parameter   : fgNormalPlay : TRUE  = Normal Play, 
//                             FALSE = Decode one Pass
//              i4DelayFrame : Delay frame
//Return      : N/A
//*********************************************************************
void vDVDecodeSetMode_8520(BOOL fgNormalPlay, INT32 i4DelayFrame)
{
  if(fgNormalPlay != fgDecodeNormalPlayRecord_8520 || i4DelayFrame != i4DecodeDelayFrameRecord_8520)
  {	
    fgDecodeNormalPlayRecord_8520 = fgNormalPlay;
    i4DecodeDelayFrameRecord_8520 = i4DelayFrame;
    i4DecodeDelayFrameCount_8520  = i4DelayFrame;
  }
}	

//*********************************************************************
//Function    : BOOL fgDVDecodeGetMode(void)
//Description : Get DV Decode mode
//Parameter   : N/A
//Return      : TRUE  = Normal mode
//              FALSE = Decode one Pass
//*********************************************************************
/*
static BOOL fgDVDecodeGetMode(void)
{
  if(fgDecodeNormalPlayRecord_8520 == TRUE)
  {
    if(i4DecodeDelayFrameCount_8520 == 0)
    {
      return TRUE;
    }	
    else
    {
      i4DecodeDelayFrameCount_8520--;
      return FALSE;
    }	
  }	
  else
  {
    return FALSE;
  }	
}
*/
// *********************************************************************
// Function : void vInitDvDec_8520(UINT32 u4InstID)
// Description : Initialize Dv Dec (reset)
// Parameter : None
// Return    : None
// *********************************************************************
void vInitDvDec_8520(UINT32 u4InstID)
{
  vDVDecReset_8520();
  bDvCmdConnect_8520(0);
  //i4VDEC_HAL_DV_InitBarrelShifter(_u4BSID,_u4VDecID, &rWmvBSInitPrm);
  //i4VDEC_HAL_DV_InitBarrelShifter(0,0, &_tDvInfo_8520);
	_u4FilePos[u4InstID] = 0;
	_dwPASS = PASS_3;
	_dwVLCMD = VLCMD_ON;
	_tDvInfo_8520.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
	_tDvInfo_8520.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
	_tDvInfo_8520.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID];
	_tDvInfo_8520.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
	_tDvInfo_8520.dVBufAddr = (UINT32) _pucDPB[u4InstID];
	_tDvInfo_8520.dwDVSpec = _dwDvFormat;
	_tDvInfo_8520.dVFrmSize = _dwDvFrmSize;
	_dwVLDID_8520 = DVDEC_VLD_ID_8520;
	_pucDecWorkBuf[u4InstID] = (UCHAR *) _pbRLCB0Addr; //_pucDPB;
}


// *********************************************************************
// Function : void vDvDecAllocMem(void)
// Description : Initialize Memory used by DV Dec
// Parameter : None
// Return    : None
// *********************************************************************
void vDvDecAllocMem_8520(void)
{//u4AbsDramANc
  _tDvInfo_8520.dRLCB0Addr = dUtPhyDramAddr((UINT32)_pbRLCB0Addr);
  _tDvInfo_8520.dRLCB1Addr = dUtPhyDramAddr((UINT32)_pbRLCB1Addr);
}

/*
// *********************************************************************
// Function : void vDvDmxFreeMem(void)
// Description : Free Memory used by DV Dmx
// Parameter : None
// Return    : None
// *********************************************************************
void vDvDecFreeMem(void)
{
  BYTE * pbBase;
  if(_tDvInfo_8520.dRLCB0Addr == 0)
  {
    return;
  }

  pbBase = (BYTE *)dUtNCLogDramAddr(_tDvInfo_8520.dRLCB0Addr);

}
*/
/*
// *********************************************************************
// Function : BOOL fgDmxSetDVSpec(UINT32 dwSpec)
// Description : Set DV Information when receiving the first frame from DMX
// Parameter : dwSpec : Format
// Return    : TRUE/FALSE
// *********************************************************************
BOOL fgDmxSetDVSpec(UINT32 dwSpec)
{
  UINT32 dTmp2;

  if(_tDvInfo_8520.dwArrayIdx > VSV_MAX_ARRAY_ID)
  {
    return (FALSE);
  }
  
  if(dwSpec != DMX_UNKNOW)
  {    
    #if DV_DECODE_DEBUG
      PRINT("[DV DEC]  Set Spec by SW");
    #endif
   
    dTmp2 = dwSpec;
    
    _tDvDmx.fgSpecsetByHW = FALSE;
  }   
  else
  {
    #if DV_DECODE_DEBUG
      PRINT("[DV DEC]  Set Spec by HW");
    #endif
      	
    _tDvDmx.fgSpecsetByHW = TRUE;
      	
    #if defined(ENABLE_SOFTWARE_DV_DEMUXER)
      dTmp2 = dwNtscPalDetect;
    #else
      dTmp2 = dReadDMX(RW_DMX_STS) & 0x383;
    #endif
  }

  switch(dTmp2)
  {
    case DMX_NTSC61834:
      _tDvDmx.dwDVSpec = DV_NTSC_61834;
      _tDvDmx.dVFrmSize = DV_NTSC_VIDEO_FRM_SIZE;
      _tDvDmx.dAFrmSize = DV_NTSC_AUDIO_FRM_SIZE;
      _tDvDmx.dFifoFrmCnt = DV_NTSC_FRM_CNT;
      _tDvDmx.vSeqInfo.bFrameRate = FRAME_RATE_30;
      
      #ifdef MT8108
        #ifdef MT8108_FORCE_TO_422
          _tDvDmx.vSeqInfo.bSrcType = VSRC_422;
        #else
          #ifdef MT8108_FORCE_TO_420
            _tDvDmx.vSeqInfo.bSrcType = VSRC_420;
          #else
          _tDvDmx.vSeqInfo.bSrcType = VSRC_420_NOSWAP;
        #endif
        #endif
      #else
        _tDvDmx.vSeqInfo.bSrcType = VSRC_422;
      #endif
      
      _tDvDmx.vSeqInfo.fg16_9 = FALSE;
      _tDvDmx.vSeqInfo.fgNtsc = TRUE;
      _tDvDmx.vSeqInfo.wPicW = _tDvInfo_8520.dPicW = 720;
      _tDvDmx.vSeqInfo.wPicH = _tDvInfo_8520.dPicH = 480;

      #ifdef MT8108
        #ifdef MT8108_FORCE_TO_422
          fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_422);
        #else
          fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_420);
        #endif
      #else
        fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_422);
      #endif

      vWriteDMX(RW_DMX_NBLK, DV_NTSC_FRM_CNT); // set Fifo Max Frm Count
      _dwDvPtsStep = 3003;

      #if DV_HAS_OWN_INDIVIDUAL_DMX_FIFO  
        #if MINIMUM_DV_DEMUXER
          vHwDmxSetVFifoReg(_tDvDmx.dVBufAddr, (_tDvDmx.dVBufAddr + DV_NTSC_VIDEO_FRM_SIZE*DV_NTSC_FRM_CNT));
          vHwDmxSetAFifoReg(_tDvDmx.dABufAddr, (_tDvDmx.dABufAddr + (DV_NTSC_VIDEO_FRM_SIZE*DV_NTSC_FRM_CNT)/15));
        #endif
      #endif
      break;
      
    case DMX_NTSC314M:
      _tDvDmx.dwDVSpec = DV_NTSC_314M;
      _tDvDmx.dVFrmSize = DV_NTSC_VIDEO_FRM_SIZE;
      _tDvDmx.dAFrmSize = DV_NTSC_AUDIO_FRM_SIZE;
      _tDvDmx.dFifoFrmCnt = DV_NTSC_FRM_CNT;
      _tDvDmx.vSeqInfo.bFrameRate = FRAME_RATE_30;
      
      #ifdef MT8108
        #ifdef MT8108_FORCE_TO_422
          _tDvDmx.vSeqInfo.bSrcType = VSRC_422;
        #else
          #ifdef MT8108_FORCE_TO_420
            _tDvDmx.vSeqInfo.bSrcType = VSRC_420;
          #else
          _tDvDmx.vSeqInfo.bSrcType = VSRC_420_NOSWAP;
        #endif
        #endif
      #else
        _tDvDmx.vSeqInfo.bSrcType = VSRC_422;
      #endif
      
      _tDvDmx.vSeqInfo.fg16_9 = FALSE;
      _tDvDmx.vSeqInfo.fgNtsc = TRUE;
      _tDvDmx.vSeqInfo.wPicW = _tDvInfo_8520.dPicW = 720;
      _tDvDmx.vSeqInfo.wPicH = _tDvInfo_8520.dPicH = 480;
      
      #ifdef MT8108
        #ifdef MT8108_FORCE_TO_422
          fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_422);
        #else
          fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_420);
        #endif
      #else
        fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_422);
      #endif

      vWriteDMX(RW_DMX_NBLK, DV_NTSC_FRM_CNT); // set Fifo Max Frm Count
      _dwDvPtsStep = 3003;

      #if DV_HAS_OWN_INDIVIDUAL_DMX_FIFO  
        #if MINIMUM_DV_DEMUXER
          vHwDmxSetVFifoReg(_tDvDmx.dVBufAddr, (_tDvDmx.dVBufAddr + DV_NTSC_VIDEO_FRM_SIZE*DV_NTSC_FRM_CNT));
          vHwDmxSetAFifoReg(_tDvDmx.dABufAddr, (_tDvDmx.dABufAddr + (DV_NTSC_VIDEO_FRM_SIZE*DV_NTSC_FRM_CNT)/15));
        #endif
      #endif
      break;
  
    case DMX_PAL61834:
      _tDvDmx.dwDVSpec = DV_PAL_61834;
      _tDvDmx.dVFrmSize = DV_PAL_VIDEO_FRM_SIZE;
      _tDvDmx.dAFrmSize = DV_PAL_AUDIO_FRM_SIZE;
      _tDvDmx.dFifoFrmCnt = DV_PAL_FRM_CNT;
      _tDvDmx.vSeqInfo.bFrameRate = FRAME_RATE_25;
//      _tDvDmx.vSeqInfo.bSrcType = VSRC_420_NOSWAP;
      #ifdef MT8108_FORCE_TO_420
        _tDvDmx.vSeqInfo.bSrcType = VSRC_420;
      #else
      _tDvDmx.vSeqInfo.bSrcType = VSRC_420_NOSWAP;
      #endif
      _tDvDmx.vSeqInfo.fg16_9 = FALSE;
      _tDvDmx.vSeqInfo.fgNtsc = FALSE;
      _tDvDmx.vSeqInfo.wPicW = _tDvInfo_8520.dPicW = 720;
      _tDvDmx.vSeqInfo.wPicH = _tDvInfo_8520.dPicH = 576;
      fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_420);
      vWriteDMX(RW_DMX_NBLK, DV_PAL_FRM_CNT);  // set Fifo Max Frm Count
      _dwDvPtsStep = 3600;

      #if DV_HAS_OWN_INDIVIDUAL_DMX_FIFO  
        #if MINIMUM_DV_DEMUXER
          vHwDmxSetVFifoReg(_tDvDmx.dVBufAddr, (_tDvDmx.dVBufAddr + DV_PAL_FRM_CNT*DV_PAL_VIDEO_FRM_SIZE));
          vHwDmxSetAFifoReg(_tDvDmx.dABufAddr, (_tDvDmx.dABufAddr + (DV_PAL_FRM_CNT*DV_PAL_VIDEO_FRM_SIZE)/15));
        #endif
      #endif
      break;
      
  case DMX_PAL314M:
      _tDvDmx.dwDVSpec = DV_PAL_314M;
      _tDvDmx.dVFrmSize = DV_PAL_VIDEO_FRM_SIZE;
      _tDvDmx.dAFrmSize = DV_PAL_AUDIO_FRM_SIZE;
      _tDvDmx.dFifoFrmCnt = DV_PAL_FRM_CNT;
      _tDvDmx.vSeqInfo.bFrameRate = FRAME_RATE_30;
      
      #ifdef MT8108
        #ifdef MT8108_FORCE_TO_422
          _tDvDmx.vSeqInfo.bSrcType = VSRC_422;
        #else
          #ifdef MT8108_FORCE_TO_420
          _tDvDmx.vSeqInfo.bSrcType = VSRC_420;
          #else
            _tDvDmx.vSeqInfo.bSrcType = VSRC_420_NOSWAP;
          #endif
        #endif
      #else
        _tDvDmx.vSeqInfo.bSrcType = VSRC_422;
      #endif
      
      _tDvDmx.vSeqInfo.fg16_9 = FALSE;
      _tDvDmx.vSeqInfo.fgNtsc = FALSE;
      _tDvDmx.vSeqInfo.wPicW = _tDvInfo_8520.dPicW = 720;
      _tDvDmx.vSeqInfo.wPicH = _tDvInfo_8520.dPicH = 576;
      
      #ifdef MT8108
        #ifdef MT8108_FORCE_TO_422
          fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_422);
        #else
          fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_420);
        #endif
      #else
        fgVsvConfigDecFrmBufArray(VSV_DEC_ARRAY_CFG_422);
      #endif
    
      vWriteDMX(RW_DMX_NBLK, DV_PAL_FRM_CNT);  // set Fifo Max Frm Count
      _dwDvPtsStep = 3600;

      #if DV_HAS_OWN_INDIVIDUAL_DMX_FIFO  
        #if MINIMUM_DV_DEMUXER
          vHwDmxSetVFifoReg(_tDvDmx.dVBufAddr, (_tDvDmx.dVBufAddr + DV_PAL_FRM_CNT*DV_PAL_VIDEO_FRM_SIZE));
          vHwDmxSetAFifoReg(_tDvDmx.dABufAddr, (_tDvDmx.dABufAddr + (DV_PAL_FRM_CNT*DV_PAL_VIDEO_FRM_SIZE)/15));
        #endif
      #endif
      break;
      
    default:
      _tDvDmx.dwDVSpec = DV_UNKNOW; // invalid
      //return;
      break;
  }

  if(_tDvDmx.dwDVSpec != DV_UNKNOW)
  {
    vDmxSendCmd(DVDMX_CMD_SETPIC);

    //Ted Hu 2004/03/10 For New DV AV SYNC
    _dwDvPts = 9000;
    _tDvDmx.dRdIdx=0;
  }
  else
  {
    return (FALSE);
  }
   
  return (TRUE);
}
*/
/*
// *********************************************************************
// Function : void vCleanVideo(void)
// Description : Clean Video playing now
// Parameter : None
// Return    : None
// *********************************************************************
void vCleanVideo(void)
{   	
  #if (DV_DECODE_IDLE_DEBUG | DV_DECODE_CLEAN_DEBUG)
    //PRINT("[DV DEC] vCleanVideo()");
    PRINTF("[DV DEC] %s",(_tDvInfo_8520.dwDisYBuffer != 0)?"_tDvInfo_8520.dwDisYBuffer != 0":"_tDvInfo_8520.dwDisYBuffer == 0");
    PRINTF("[DV DEC] %s",(_tDvInfo_8520.dwDisCBuffer != 0)?"_tDvInfo_8520.dwDisCBuffer != 0":"_tDvInfo_8520.dwDisCBuffer == 0");
  #endif

  if(_tDvInfo_8520.dwDisYBuffer != 0 && _tDvInfo_8520.dwDisCBuffer != 0)	
  {
    #if (DV_DECODE_IDLE_DEBUG | DV_DECODE_CLEAN_DEBUG)
      PRINTF("[DV DEC] Clean Dec Idx = %x, Dis Idx = %x",_tDvInfo_8520.dwCurDecIdx, _tDvInfo_8520.dwCurDisIdx);
      PRINTF("[DV DEC]  Y = %x, C = %x", _tDvInfo_8520.dwDisYBuffer, _tDvInfo_8520.dwDisCBuffer);
    #endif
    
    if( VSRC_422 == _tDvDmx.vSeqInfo.bSrcType )
    {
      #if (DV_DECODE_IDLE_DEBUG | DV_DECODE_CLEAN_DEBUG)
        PRINTF("[DV DEC] VSRC_422");
      #endif
    fgUtMemSet((BYTE *)_tDvInfo_8520.dwDisYBuffer, VSV_FBUF_422Y_SZ, 0x10101010);
    fgUtMemSet((BYTE *)_tDvInfo_8520.dwDisCBuffer, VSV_FBUF_422C_SZ, 0x80808080);  
  }
    else if( VSRC_420_NOSWAP == _tDvDmx.vSeqInfo.bSrcType )
    {
      #if (DV_DECODE_IDLE_DEBUG | DV_DECODE_CLEAN_DEBUG)
        PRINTF("[DV DEC] VSRC_420_NOSWAP");
      #endif
      fgUtMemSet((BYTE *)_tDvInfo_8520.dwDisYBuffer, VSV_FBUF_Y_SZ, 0x10101010);
      fgUtMemSet((BYTE *)_tDvInfo_8520.dwDisCBuffer, VSV_FBUF_C_SZ, 0x80808080);  
    }
    else if( VSRC_420 == _tDvDmx.vSeqInfo.bSrcType )
    {
      #if (DV_DECODE_IDLE_DEBUG | DV_DECODE_CLEAN_DEBUG)
        PRINTF("[DV DEC] VSRC_420");
      #endif
      fgUtMemSet((BYTE *)_tDvInfo_8520.dwDisYBuffer, VSV_FBUF_Y_SZ, 0x10101010);
      fgUtMemSet((BYTE *)_tDvInfo_8520.dwDisCBuffer, VSV_FBUF_C_SZ, 0x80808080);  
    }
    
  }
}	
*/
// *********************************************************************
// Function : void vDvDecOneFrm_8520(UINT32 dwDecIdx)
// Description : Decode one DV frame to Buf[dDecIdx]
// Parameter : None
// Return    : None
// *********************************************************************
void vDvDecOneFrm_8520(UINT32 dDecIdx)
{
  //UINT32 dY, dC;

  #if DV_DECODE_DEBUG
    PRINT("[DV DEC] vDvDecOneFrm_8520() Enter");
  #endif
    
  if(_tDvInfo_8520.fgIsDecoding == TRUE)
  {
    #if DV_DECODE_DEBUG
      PRINT("[DV DEC]  _tDvInfo_8520.fgIsDecoding = TRUE and vDvDecOneFrm_8520() Leave");
    #endif
    	
    return;
  }
  //_tDvInfo_8520.dSrcIdx = _tDvDmx.dRdIdx;

  //Power on VDec
  //vWriteVLD(RW_VLD_PWON, 0); 
	//vVDecWriteVLD(0, addr, val);
  
  //2004.06.23 Ted Hu, For fix DV decode don't return ISR after switch Input DV when DVD playing
  vDVDecReset_8520();
  
/*
  // 2006.08.12 Ben Lok, for DV digest trick mode //
  // add by victor lin, for test address swap in DV 
  if ((VSRC_420 == _tDvDmx.vSeqInfo.bSrcType) || (VSRC_422 == _tDvDmx.vSeqInfo.bSrcType)) 
  	vMCAddrSwapSw(ON); 
  else 
  	vMCAddrSwapSw(OFF); 
*/

  //Change Current Process to Non_Preempt mode
  //NU_Change_Preemption(NU_NO_PREEMPT);

  vWriteDV_8520(RW_DV_ACT, 0);
  vVDecWriteMC(DVDEC_VLD_ID_8520, RW_MC_OPBUF, 0);
  
/*
  if( fgDVDecodeGetIdleStatus_8520() 
      && _tDvInfo_8520.dwDecYBufferLast != 0 
      && _tDvInfo_8520.dwDecCBufferLast != 0       
      && _tDvInfo_8520.dwCurDecIdx != _tDvInfo_8520.dwCurDisIdx)
  {
    dY = _tDvInfo_8520.dwDecYBufferLast;
    dC = _tDvInfo_8520.dwDecCBufferLast;
    dwIdleNotify_8520++;	    
  }
  else
  {	
    dwIdleNotify_8520 = 0;	
    _tDvInfo_8520.dwCurDecIdx = _dwDvCurDecIdx;
    vVsvGetFBufAddr(_dwDvCurDecIdx, &dY, &dC, _tDvInfo_8520.dwArrayIdx);
  }

  if(dwIdleNotify_8520 == 1)
  {	
    vCleanVideo();	
    
    #if DV_DECODE_IDLE_DEBUG
      PRINTF("[DV DEC] Decode Dec Idx = %x, Dis Idx = %x",_tDvInfo_8520.dwCurDecIdx, _tDvInfo_8520.dwCurDisIdx);
    #endif
  }
  
  #if DV_DECODE_CLEAN_DEBUG
    PRINTF("[DV DEC]  Decode Dec Idx = %x", _dwDvCurDecIdx);
  #endif
   */
  vVDecWriteMC(DVDEC_VLD_ID_8520, RW_MC_R1Y, (u4AbsDramANc(_pucPic0Y[0]) >> 9)); //512 byte
  vVDecWriteMC(DVDEC_VLD_ID_8520, RW_MC_R1C, (u4AbsDramANc(_pucPic0C[0]) >> 8)); //256 byte

/*
  _tDvInfo_8520.dwDecYBuffer = dY;
  _tDvInfo_8520.dwDecCBuffer = dC;

  if(_tDvInfo_8520.dwDecYBuffer != 0 && _tDvInfo_8520.dwDecCBuffer != 0)
  {
    _tDvInfo_8520.dwDecYBufferLast = _tDvInfo_8520.dwDecYBuffer;
    _tDvInfo_8520.dwDecCBufferLast = _tDvInfo_8520.dwDecCBuffer;
  }
  */
 // if(!fgDVDecodeGetMode())
  {
    //Set DV decoder to Only Pass 1
    //vWriteDV_8520(RW_DV_MODE, dReadDV_8520(RW_DV_MODE) & 0xfffff0ff | 0x500);
  }	

  //#ifdef MT8108
  //  #ifdef MT8108_PASS_TEST
      if(_dwPASS == PASS_3)
      {
        UINT32 dwMode = dReadDV_8520(RW_DV_MODE) & 0xfffff0ff;
        vWriteDV_8520(RW_DV_MODE, dwMode);
      }
      else if(_dwPASS == PASS_2)
      {
        UINT32 dwMode =dReadDV_8520(RW_DV_MODE) & 0xfffff0ff | 0x600;
        vWriteDV_8520(RW_DV_MODE, dwMode);
      }
      else if(_dwPASS == PASS_1)
      {
        UINT32 dwMode =dReadDV_8520(RW_DV_MODE) & 0xfffff0ff | 0x500;
        vWriteDV_8520(RW_DV_MODE, dwMode);
      }
      else if(_dwPASS == PASS_AUTO)
      {
        //Do nothing
      }
 //   #endif

 //   #ifdef MT8108_VLCMD_TEST
      if(_dwVLCMD == VLCMD_OFF)
      {
        vWriteDV_8520(RW_DV_MODE, dReadDV_8520(RW_DV_MODE) & 0xffffff0f);
      }
      else if(_dwVLCMD == VLCMD_ON)
      {
        vWriteDV_8520(RW_DV_MODE, dReadDV_8520(RW_DV_MODE) & 0xffffff0f | 0x10);
      }
      else if(_dwVLCMD == VLCMD_AUTO)
      {
        //Do nothing
      }
 //   #endif
//  #endif

//need enable interrupt?
//vWriteDV_8520(RW_DV_MODE, dReadDV_8520(RW_DV_MODE)  | 0xF);

#ifdef MT8108
  #ifdef MT8108_FORCE_TO_422
    //vWriteMC(RW_MC_422TO420, 1);
  #else
    //vWriteMC(RW_MC_422TO420, 1);
  #endif
#endif

//convert to 420 output or not.
if(420 == _dwDvCbCrFormat)
{
	vVDecWriteMC(DVDEC_VLD_ID_8520, RW_MC_422TO420, 1); //on
}
else
{
	vVDecWriteMC(DVDEC_VLD_ID_8520, RW_MC_422TO420, 0); //off
}

//set MC DRAM access mode to block/raster.
vVDecWriteMC(DVDEC_VLD_ID_8520, RW_MC_PS_DRAM_MODE, 0); //blk
//vVDecWriteMC(DVDEC_VLD_ID_8520, RW_MC_PS_DRAM_MODE, 3);  //raster

  vWriteDV_8520(RW_DV_CFG, _tDvInfo_8520.dwDVSpec);
  //vWriteDV_8520(RW_DV_ACT, DV_TRIG_MC);

  //vWriteDV_8520(RW_DV_RLCB0, (u4AbsDramANc(_tDvInfo_8520.dRLCB0Addr) >> 2));
  //vWriteDV_8520(RW_DV_RLCB1, (u4AbsDramANc(_tDvInfo_8520.dRLCB1Addr) >> 2));
  vWriteDV_8520(RW_DV_RLCB0, (u4AbsDramANc(0x300000) >> 2));  //FIXME: workaround for DSP...
  vWriteDV_8520(RW_DV_RLCB1, (u4AbsDramANc(0x400000) >> 2));  //FIXME: workaround for DSP...

  //OSS_ASSERT(0 != _tDvDmx.dVBufAddr && UT_INVALID_ADDR != _tDvDmx.dVBufAddr);
  //vWriteDV_8520(RW_DV_SRCB, ((_tDvDmx.dVBufAddr + _tDvDmx.dRdIdx*_tDvDmx.dVFrmSize)>>2));
  vWriteDV_8520(RW_DV_SRCB, (u4AbsDramANc(_tDvInfo_8520.u4VFifoSa)>>2));
  
  _tDvInfo_8520.fgIsDecoding = TRUE;


  //Change back to Preemptable mode
  //NU_Change_Preemption(NU_PREEMPT);

  //iDvClrEvent(DVEG_DEC_DONE);

  //Trigger Decode
  vWriteDV_8520(RW_DV_ACT, DV_ACTIVATE);
  vWriteDV_8520(RW_DV_ACT, 0);

  #if DV_DECODE_DEBUG
    PRINT("[DV DEC] vDvDecOneFrm_8520() Leave");
  #endif
  
}

// *********************************************************************
// Function : BOOL fgIsDvDecoding_8520(void)
// Description : Check if DV is decoding?
// Parameter : None
// Return    : None
// *********************************************************************
BOOL fgIsDvDecoding_8520(void)
{
  return (_tDvInfo_8520.fgIsDecoding);
}



// *********************************************************************
// Function : void vDvDecDone_8520(void)
// Description : Things to do while one dv frame is decoded done!
//               Can also be used for dummy decode
// Parameter : None
// Return    : None
// *********************************************************************
volatile UINT32 PollCnt_8520 = 0;
void vDvDecDone_8520(void)
{
  // Check Decoding End or Not
  PollCnt_8520 = 0;
  while(1)
  {
    if(PollCnt_8520 < 1000)
    {
      PollCnt_8520++;
    } 
    else
    {
      break;
    }
  }

  vDVDecReset_8520();
/*
  // increase the read pointer by one
  if(_tDvDmx.fgSpecsetByHW == TRUE)
  {
    vDvDmxIncRdPtr();
  }


  // Activate HISR to set DV Event Group
  // TODO: Should check about whether decode success or time-out or decode failed
  NU_Activate_HISR(&_rHisrDvDec);
  */
    
}

// *********************************************************************
// Function : void vDvDecIrq_8520(void)
// Description : Dmx IRQ routine
// Parameter : None
// Return    : None
// *********************************************************************
//2004.06.25 Ted Hu, Add Decode
UINT32 dwDVDecodeDone_8520;
UINT32 dwDVDecodeTimeOut_8520;
UINT32 dwDVDecodeOther_8520;

extern UINT32 dDispDec_8520[10000];
extern UINT32 dDDIdx_8520;
void vDvDecIrq_8520(void)
{
  UINT32 dwTmp;

  //Decoding done, maybe it is a TimeOut
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

  if(dDDIdx_8520 < 10000)
  {
    //dDispDec_8520[dDDIdx_8520] = (_dwDvCurDecIdx| 0x70000000);
    dDDIdx_8520++;
  }
  //vDvDecDone_8520();
}




// *********************************************************************
// Function : void vDvDecInitIrq_8520(void)
// Description : Dv Int Init
// Parameter : None
// Return    : None
// *********************************************************************
void vDvDecInitIrq_8520(void)
{
  //vDvDecInstallIrq_8520(vDvDecIrq_8520);
  //BIM_DisableIrq(0xffffffff); //ginny mark it 071015
  BIM_DisableIrq(VECTOR_VDFUL);
  BIM_ClearIrq(VECTOR_VDFUL);
  BIM_EnableIrq(VECTOR_VDFUL);
}

BYTE bDvCmdPlay_8520()
{
/*
  if(_tDvInfo_8520.dwDVSpec == DV_UNKNOW) // Picture size is set
  {
    return (DV_CMD_RESULT_DONEXT);
  }

  if(_tDvInfo_8520.fgConnect ==  FALSE)
  {
    return (DV_CMD_RESULT_DONEXT);
  }
*/
  _tDvInfo_8520.fgStartDec = TRUE;
  return 0;//(DV_CMD_RESULT_DONEXT);
}
/*
BYTE bDvCmdDisConnect(void)
{
  _tDvInfo_8520.fgConnect = FALSE;

  do
  {
    if(_tDvInfo_8520.fgIsDecoding == TRUE)
    {
      BIM_DisableIrq(VECTOR_VDFUL);
      iDvClrEvent(DVEG_DEC_DONE);
      BIM_EnableIrq(VECTOR_VDFUL);
      
      //2006.01.12 Ted Hu, For switch input too fast cause NO DVEG_DEC_DONE get,
      //Set a TimeOut
      dDvGetEvent(DVEG_DEC_DONE, NU_OR, NU_NO_SUSPEND);
      
      //2006.01.12 Ted Hu, must find the root cause
      _tDvInfo_8520.fgIsDecoding = FALSE;
    } 
    else
    {
      break;
    }
  }while (1);
  
  _tDvDmx.dwDVSpec = DV_UNKNOW;
  _tDvInfo_8520.fgConnect = FALSE;
  _tDvInfo_8520.fgStartDec = FALSE;

  iDvSetEvent(DVEG_DEC_HALT, NU_OR);

  return (DV_CMD_RESULT_DONEXT);

}
*/
BYTE bDvCmdConnect_8520(UINT32 dArrayIdx)
{
  BYTE bResult;

  if(_tDvInfo_8520.fgConnect == TRUE)
  {
    bResult = DV_CMD_RESULT_DONEXT_BREAK;
  } 
  else
  {
    vRstDvInfo_8520();
    _tDvInfo_8520.dwArrayIdx = dArrayIdx;
    _tDvInfo_8520.fgConnect = TRUE;
	vDvDecAllocMem_8520();
    vDvDecInitIrq_8520();
    bResult = DV_CMD_RESULT_DONEXT;
  }

  return (bResult);
}

void vDvCmdProc_8520()
{
/*
  UINT32 dCmd[4];
  
  STATUS status;
*/
BOOL bResult;
    //bResult = bDvCmdConnect_8520(dCmd[1]); // Array Idx is put in dCmd[1]
	bResult = bDvCmdPlay_8520();
	//bResult = bDvCmdDisConnect();

}

// *********************************************************************
// Function : void vDvPlayLoop(void)
// Description : Dv Play, and handle command send to dv decode kernel
// Parameter : None
// Return    : None
// *********************************************************************
UINT32 DV_PLAY_STATE_8520 = 0;
UINT32 DV_CNT_8520 = 0;
void vDvPlayLoop_8520()
{
 // while(1)
  {	
    vDvCmdProc_8520();

    if(//(_tDvDmx.dwDVSpec != DV_UNKNOW) && 
    	(_tDvInfo_8520.fgConnect == TRUE))
    {	
      bDvNormDecProc_8520(TRUE);
    } 
    else
    {	
    /*
      if(_tDvInfo_8520.fgOn != TRUE)
      {
        //NU_Sleep(10); // 100ms, assume 1 os tick is 10ms
      }
      else if(_tDvInfo_8520.fgConnect == FALSE)
      {
        //dDvGetEvent(DVEG_CMD_IN, NU_OR, 1);
      }
      else if(_tDvInfo_8520.dwDVSpec == DV_UNKNOW)
      {
        //2004/03/27 Ted Hu. Let vDvPlayLoop not lock all process
        //NU_Sleep(10); // 100ms, assume 1 os tick is 10ms
         
        //dDvGetEvent(DVEG_DMX_DATAIN+DVEG_CMD_IN, NU_OR, NU_SUSPEND);
      }
      */
    }
  }
}
/*
// *********************************************************************
// Function : 
// Description : 
// Parameter : 
// Return    : 
// *********************************************************************
BOOL fgDVDMXRegCallBackForVAUX(void (*pvFunc)(UCHAR ucPC1, UCHAR ucPC2, UCHAR ucPC3, UCHAR ucPC4))
{
  _dwVAUXCallBack = 0;		
  _dwVAUXCallBack = (CALLBACKWITH4PARM)pvFunc;
  return TRUE;
}	

// 2006.07.24 Ben Lok, for DV Digest trick mode //
UINT32 vDVGetEmptyFBuf(void)
{
  return dwVDecGetEmptyFBufEx(_tDvInfo_8520.dwArrayIdx, NULL, NULL);
}
*/
// *********************************************************************
// Function : BYTE bDvNormDecProc_8520(void)
// Description : Normal Decoding function
// Parameter : fgRealAVIn : TRUE/FALSE
// Return    : STATUS
// *********************************************************************
UINT32 dDispDec_8520[10000];
UINT32 dDDIdx_8520 = 0;

//Ted Hu 2004/03/10 For New DV AV SYNC
UINT32 _dwDvCurDmxRIdx_8520;

//2006.01.20 Ted Hu, Timer
UINT32 _dwDecodeTimer_8520;

BYTE bDvNormDecProc_8520(BOOL fgRealAVIn)
{
  //UNSIGNED     dReceivedEvent;
  //RVFrmBufInfo *prVFrmBuf; 
  //UINT32         dwVAUX;
  //UINT32         dY, dC;
  //UINT32         status;

  // 2006.08.12 Ben Lok, for DV digest trick mode //
  //if(fgVDecCanDecode())
  {
	  // decode one frame //
  
  #if DV_DECODE_DEBUG
    PRINT("[DV DEC] bDvNormDecProc_8520() Enter");
  #endif
  /*
  if(fgIsDvDecoding_8520() == TRUE)
  {
    #if DV_DECODE_DEBUG
      PRINT("[DV DEC]  fgIsDvDecoding_8520() == TRUE");
    #endif	
    
    //Use Timeout
    //dReceivedEvent = dDvGetEvent(DVEG_DEC_DONE+DVEG_DEC_TIMEOUT+DVEG_CMD_IN, NU_OR, NU_NO_SUSPEND);
    
    if(_dwDecodeTimer_8520 > 10)
    {
      //TimeOut	
      
      _dwDecodeTimer_8520 = 0;
      
      vDvDecDone_8520();
      return 0; 
    }	
    else
    {
      return (0);
    }
  }
  else
  {
    _dwDecodeTimer_8520 = 0;
  }	
  */
/*
  //Put the previous frame into Display Queue
  if(_dwDvCurDecIdx != 0xff)
  {
    #if DV_DECODE_DEBUG
      PRINT("[DV DEC]  _dwDvCurDecIdx != 0xff");
    #endif
      	
    prVFrmBuf = prVsvGetFBufInfo(_dwDvCurDecIdx, _tDvInfo_8520.dwArrayIdx);
    prVFrmBuf->bPicCdTp = I_TYPE;
    prVFrmBuf->bPicStruct = FRM_PIC;
    prVFrmBuf->fgAntiFlicker  = 0;

    //FBUF_FLG_TOP_FLD_FIRST;
    prVFrmBuf->dwFlag = 0;

    prVFrmBuf->dwPts = _pdDvPts[_dwDvCurDmxRIdx_8520];

    if(!fgDVDecodeGetIdleStatus_8520())
    {	
      //Only for get display buffer address
      vVsvGetFBufAddr(_dwDvCurDecIdx, &dY, &dC, _tDvInfo_8520.dwArrayIdx);
  
      #if DV_DECODE_DEBUG
        PRINT("[DV DEC]  Get Nex Display Buffer");
      #endif
      
      //Set Next Disp Buf
	      //status = fgVsvSetNextDispBuf(_dwDvCurDecIdx, _tDvInfo_8520.dwArrayIdx); 
	      // 2006.08.12 Ben Lok, for DV digest trick mode //
		  status = fgVDecSetNextDispBufEx(_dwDvCurDecIdx, _tDvInfo_8520.dwArrayIdx, _tDvInfo_8520.dPicW, _tDvInfo_8520.dPicH , &vDVGetEmptyFBuf);
				  
		      if( FALSE == status)      
      {
        _tDvInfo_8520.dwCurDisIdx = _dwDvCurDecIdx;	
        _tDvInfo_8520.dwDisYBuffer = dY;
        _tDvInfo_8520.dwDisCBuffer = dC;
        
        #if DV_DECODE_CLEAN_DEBUG
          PRINTF("[DV DEC]  Set Nex Display Buffer FAIL, Dec Idx = %x", _dwDvCurDecIdx);
        #endif
        
        #if DV_DECODE_DEBUG
          PRINT("[DV DEC]  Set Nex Display Buffer FAIL");
        #endif
            	
        //Error case	
//        iDvClrEvent(DVEG_QUEUE_AVLB);
        dReceivedEvent = dDvGetEvent(DVEG_QUEUE_AVLB+DVEG_CMD_IN, NU_OR_CONSUME, NU_SUSPEND);
        return (0);
      }
	        else // Set next display buffer success //
      {
        _tDvInfo_8520.dwCurDisIdx = _dwDvCurDecIdx;	
        _tDvInfo_8520.dwDisYBuffer = dY;
        _tDvInfo_8520.dwDisCBuffer = dC;
            	
        #if DV_DECODE_CLEAN_DEBUG
          PRINTF("[DV DEC]  Set Nex Display Buffer SUCCESS, Dec Idx = %x", _dwDvCurDecIdx);
        #endif
                    	
        #if DV_DECODE_DEBUG
          PRINT("[DV DEC]  Set Nex Display Buffer SUCCESS");
        #endif
              	
        _dwDvCurDecIdx = 0xff;
      }
    }
    else
    {
      #if DV_DECODE_DEBUG
        PRINT("[DV DEC]  Idle Status");
      #endif    	
      
      _dwDvCurDecIdx = 0xff;
    }	
  }
  else
  {
    #if DV_DECODE_CLEAN_DEBUG
      PRINTF("[DV DEC]  No Set Next Display Dec Idx = %x", _dwDvCurDecIdx);
    #endif
          	
    #if DV_DECODE_DEBUG
      PRINT("[DV DEC]  _dwDvCurDecIdx = 0xff");
    #endif  
  }	
*/

  //Prepare to Decode Next Picture
  //Check Fifo Empty or not
  BIM_DisableIrq(VECTOR_DV);
/*
  if(fgHwDmxIsFifoEmpty() == TRUE  && fgRealAVIn == TRUE)
  {
    #if DV_DECODE_DEBUG
      PRINT(" fgHwDmxIsFifoEmpty() = TRUE and bDvNormDecProc_8520() Leave");
    #endif
          	
    iDvClrEvent(DVEG_DMX_DATAIN);
    BIM_EnableIrq(VECTOR_DV);
    dDvGetEvent(DVEG_DMX_DATAIN+DVEG_CMD_IN, NU_OR, NU_SUSPEND);
    return (0);
  }
*/
  BIM_EnableIrq(VECTOR_DV);
  /*
  //Get Next Empty Buffer
		  //_dwDvCurDecIdx = dAcquireBuf();
		  // 2006.08.12 Ben Lok, for DV digest trick mode //
		  //_dwDvCurDecIdx = dwVDecGetEmptyFBufEx(_tDvInfo_8520.dwArrayIdx, NULL, NULL);

  #if DV_DECODE_DEBUG
    PRINTF("[DV DEC]  Get Empty Buffer = %d", _dwDvCurDecIdx);
  #endif
  
  //Ted Hu 2004/03/10 For New DV AV SYNC
  _dwDvCurDmxRIdx_8520 = _tDvDmx.dRdIdx;

  if(_dwDvCurDecIdx == FBUF_INVALID_IDX)
  {
    #if DV_DECODE_DEBUG
      PRINT("[DV DEC]  _dwDvCurDecIdx = FBUF_INVALID_IDX and bDvNormDecProc_8520() Leave");
    #endif
    	
    iDvClrEvent(DVEG_FRMBUF_AVLB);
    dReceivedEvent = dDvGetEvent(DVEG_FRMBUF_AVLB+DVEG_CMD_IN, NU_OR, 1);
    return (0);
  }

  //Before Decode a new Frame
  if(dDDIdx_8520 < 10000)
  {
    dDispDec_8520[dDDIdx_8520] = (_dwDvCurDecIdx| 0x50000000);
    dDDIdx_8520++;
  }
  
  if(_dwVAUXCallBack != 0)
  {
    dwVAUX = dReadDMX(RO_DMX_VSC);
    _dwVAUXCallBack(dwVAUX & 0x000000FF, (dwVAUX & 0x0000FF00)>>8, (dwVAUX & 0x00FF0000)>>16, (dwVAUX & 0xFF000000)>>24);
  }	
       */
  //vDvDecOneFrm_8520(_dwDvCurDecIdx);
    vDvDecOneFrm_8520(0);

  #if DV_DECODE_DEBUG
    PRINT("[DV DEC] bDvNormDecProc_8520() Leave");
  #endif
  
  }

  // 2006.08.12 Ben Lok, for DV digest trick mode //
  //if(!fgVDecPIPOutput())
  {
    //return FALSE;
  }

  return (0);

}



// *********************************************************************
// Function : void vTaskDv_8520(UNSIGNED argc, void *argv)
// Description : DV Main Routine
// Parameter : None
// Return    : argc, argv[]
// *********************************************************************
extern BOOL fgVsvPlayReconstructFrm(BOOL fgReconstFrm);
void vTaskDv_8520(void) //(UNSIGNED argc, void *argv)
{
  vDvDecAllocMem_8520();
  vDvPlayLoop_8520();
}

// *********************************************************************
// Function : OSS_STATUS iDvInitTaskvoid)
// Description : Initialize DV OS related code
// Parameter : None
// Return    : None
// *********************************************************************
/*
UINT32 iDvInitTask(void)
{
	vTaskDv_8520();
  return 0;//(NU_SUCCESS); 

}
*/
/////////////////// Global variables in this file /////////////////////
BOOL  fgDemuxerNormalPlayRecord_8520 = TRUE;
INT32 i4DemuxerDelayFrameRecord_8520 = 0;
INT32 i4DemuxerDelayFrameCount_8520  = 0;
BOOL  fgDVAVSyncInit_8520 = TRUE;
///////////////////////////////////////////////////////////////////////

//*********************************************************************
//Function    : void vDVGetDmxInfo(UINT32 *u4TotlaISR, UINT32 *u4TotalFrame,
//                                 UINT32 *u4TotalNOEQERROR, UINT32 *u4TotalSEQERROR,
//                                 UINT32 *u4TotalVIDEOFULL)
//Description : Get Dmx Info
//Parameter   : 
//Return      : N/A
//*********************************************************************
/*
void vDVGetDmxInfo(UINT32 *u4TotlaISR, UINT32 *u4TotalFrame,
                   UINT32 *u4TotalNOEQERROR, UINT32 *u4TotalSEQERROR,
                   UINT32 *u4TotalVIDEOFULL)
{
  *u4TotlaISR       = _tDvDmx.dwTotalISR;
  *u4TotalFrame     = _tDvDmx.dwTotalFrame;  
  *u4TotalNOEQERROR = _tDvDmx.dwTotalNOEQERROR;    
  *u4TotalSEQERROR  = _tDvDmx.dwTotalSEQERROR;  
  *u4TotalVIDEOFULL = _tDvDmx.dwTotalVIDEOFULL;   
}
*/
//*********************************************************************
//Function    : void vDVDemuxerResetAVSync(void)
//Description : Reset AVSync
//Parameter   : N/A
//Return      : N/A
//*********************************************************************
/*
void vDVDemuxerResetAVSync(void)
{
  fgDVAVSyncInit_8520 = TRUE;
}
*/
//*********************************************************************
//Function    : void vDVDemuxerSetMode(BOOL fgNormalPlay, INT32 i4DelayFrame)
//Description : Set DV demuxer mode
//Parameter   : fgNormalPlay : TRUE  = Normal Play, 
//                             FALSE = No AV SYNC
//              i4DelayFrame : Delay frame
//Return      : N/A
//*********************************************************************
/*
void vDVDemuxerSetMode(BOOL fgNormalPlay, INT32 i4DelayFrame)
{
  if(fgNormalPlay != fgDemuxerNormalPlayRecord_8520 || i4DelayFrame != i4DemuxerDelayFrameRecord_8520)
  {	
    fgDemuxerNormalPlayRecord_8520 = fgNormalPlay;
    i4DemuxerDelayFrameRecord_8520 = i4DelayFrame;
    i4DemuxerDelayFrameCount_8520  = i4DelayFrame;
  }
}	
*/
//*********************************************************************
//Function    : BOOL fgDVDemuxerGetMode(void)
//Description : Get DV demuxer mode
//Parameter   : N/A
//Return      : TRUE  = Normal mode
//              FALSE = No AV SYNC
//*********************************************************************
/*
static BOOL fgDVDemuxerGetMode(void)
{
  if(fgDemuxerNormalPlayRecord_8520 == TRUE)
  {
    if(i4DemuxerDelayFrameCount_8520 == 0)
    {
      return TRUE;
    }	
    else
    {
      i4DemuxerDelayFrameCount_8520--;
      return FALSE;
    }	
  }	
  else
  {
    return FALSE;
  }	
}
*/
// *********************************************************************
// Function : void vRstDvInfo_8520(void)
// Description : Initialize tDVInfo value
// Parameter : None
// Return    : None
// *********************************************************************
void vRstDvInfo_8520(void)
{
  //Default value
  _tDvInfo_8520.dSrcIdx          = 0;
  _tDvInfo_8520.dwDecYBuffer     = 0;
  _tDvInfo_8520.dwDecCBuffer     = 0;  
  _tDvInfo_8520.dwDisYBuffer     = 0;
  _tDvInfo_8520.dwDisCBuffer     = 0;    
  _tDvInfo_8520.dwDecYBufferLast = 0;
  _tDvInfo_8520.dwDecCBufferLast = 0;
  _tDvInfo_8520.dwCurDecIdx      = 0;
  _tDvInfo_8520.dwCurDisIdx      = 0;
  //_dwDvCurDecIdx            = 0xff;

}


// *********************************************************************
// Function : BOOL fgIsDvSpecSet_8520(void)
// Description : Initialize Memory used by DV (not including display/
//               decode buffers)
// Parameter : None
// Return    : None
// *********************************************************************
BOOL fgIsDvSpecSet_8520(void)
{
  if(_tDvInfo_8520.dwDVSpec == DV_UNKNOW)
  {
    return (FALSE);
  }
  else
  {
    return (TRUE);
  }
}

// *********************************************************************
// Function : void vDmxReceiveOneFrm(void)
// Description : Things to do when receive one frame from DMX
//               Increase Write pointer by one!
// Parameter : None
// Return    : None
// *********************************************************************
/*
extern UINT32 dTotalWrCnt;

void vDmxReceiveOneFrm(void)
{
  dTotalWrCnt++;
  _pdDvPts[_tDvDmx.dWrIdx] = _dwDvPts;

  if(fgDVDemuxerGetMode())
  {
    //2004/03/09 Ted Hu For NEW AV SYNC
    if(fgDVAVSyncInit_8520 == TRUE)
    {
      vPrDvSet1stAPts(_dwDvPts);
      fgDVAVSyncInit_8520 = FALSE;
    }
    //vDspSendPts(_pdDvPts[_tDvDmx.dWrIdx],  (_tDvDmx.dABufAddr + (_tDvDmx.dWrIdx * _tDvDmx.dAFrmSize)));
    //2004.04.23 Ted Hu, For change to DMODE
    vDspSendPts(_pdDvPts[_tDvDmx.dWrIdx],0);
    
    _dwDvPts += _dwDvPtsStep;
    _tDvDmx.dWrIdx++;
  }
  
  if(_tDvDmx.dWrIdx >= _tDvDmx.dFifoFrmCnt)
  {
    _tDvDmx.dWrIdx = 0; // turn back the write pointer
  }

}
*/

static void (*_pvDvIrqFunc)(void) = vNullIrq_8520;

// *********************************************************************
// Function : void vDvDecInstallIrq_8520(void)
// Description : Install DV DMX IRQ routine
// Parameter : None
// Return    : None
// *********************************************************************
void vDvDecInstallIrq_8520(void (*pvIrqEntry)(void))
{
  BIM_DisableIrq(VECTOR_VDFUL);
  BIM_ClearIrq(VECTOR_VDFUL);
  _pvDvIrqFunc = pvIrqEntry;
}


// *********************************************************************
// Function : void vDvDecReleaseIrq_8520(void)
// Description : Release DV DMX IRQ routine
// Parameter : None
// Return    : None
// *********************************************************************
void vDvDecReleaseIrq_8520(void)
{
  BIM_DisableIrq(VECTOR_VDFUL);
  BIM_ClearIrq(VECTOR_VDFUL);
  _pvDvIrqFunc = vNullIrq_8520;
}

// *********************************************************************
// Function : void vDvDecIrq_8520(void)
// Description : Dmx IRQ routine
// Parameter : None
// Return    : None
// *********************************************************************
void vDvDecodeIrq_8520(INT32 i)
{
  _pvDvIrqFunc();
  BIM_ClearIrq(VECTOR_VDFUL);
}

//static void (*_pvDmxIrqFunc)(void) = vNullIrq_8520;

// *********************************************************************
// Function : void vNullIrq_8520(void)
// Description : NULL IRQ routine
// Parameter : None
// Return    : None
// *********************************************************************
void vNullIrq_8520(void)
{
  return;
}



