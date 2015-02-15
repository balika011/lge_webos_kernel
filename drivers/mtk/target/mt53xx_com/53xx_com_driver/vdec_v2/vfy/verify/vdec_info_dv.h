#ifndef _VDEC_INFO_DV_H_
#define _VDEC_INFO_DV_H_

#include "x_typedef.h"

//#include "dvdec_priv.h"
//#include "dvdmx_priv.h"
//#include "vfbuf_pub.h"

//#include "dv_decode.h"
  enum
  {
    DV_PAL_61834   = 0x0B,	//420
    DV_NTSC_61834  = 0x0C, //411
    DV_PAL_314M_25MB    = 0x1D,  //411
    DV_NTSC_314M_25MB   = 0x1C, //411
    DV_PAL_314M_50MB    = 0x2F,  //422
    DV_NTSC_314M_50MB   = 0x2E, //422
    DV_UNKNOW      = 0xFF
  };

//Define DV Video Bitstream size
#define DV_NTSC_VIDEO_FRM_SIZE    0x1A5E0         //(135 DIF)*(80 Bytes DI FSize)*(10 Sequence) = 0x1A5E0
#define DV_PAL_VIDEO_FRM_SIZE     0x1FA40         //(135 DIF)*(80 Bytes DI FSize)*(12 Sequence) = 0x1FA40

/////////////////// Define ////////////////////////////////////////////
//For DeMuxer Detail Audio Debug
#define DV_DEMUXER_AUDIO_FRAME_DEBUG                      0   

//For DeMuxer Detail Video Debug
#define DV_DEMUXER_VIDEO_FRAME_DEBUG                      0

//For DMODE=0 and if NOEQ and SEQER happen.
//Must handle it as a Frmae come, but only a write point update
#define DV_DEMUXER_PROCESS_SEQER_NOEQ_UPDATE_WRITE_POINT  0

//For SEQERR happen, still process this frame
#define DV_DEMUXER_POCRESS_SEQER                          0

//For NOEQ happen, still process this frame
#define DV_DVMUXER_PROCESS_NOEQ                           0

//For Minimum and not suit both PAL/NTSC buffer
#define MINIMUM_DV_DEMUXER                                0
  #define MINIMUM_DV_FRAME                                3

//For Pre-Process DV Demuxer
#define DV_DEMUXER_PRE_PROCESS                            0

//For DV Decode Idle and Clean Debug
#define DV_DECODE_IDLE_DEBUG                              0

//For DV Decode Debug
#define DV_DECODE_DEBUG                                   0

//For DV Decode Clean Debug
#define DV_DECODE_CLEAN_DEBUG                             0

//For Show Blank Video Debug
#define DV_DECODE_SHOE_BLANK_DEBUG                        0

//Define DV Spec 314M Header size
#define DV_NTSC_HEADER_SIZE       0x12C0
#define DV_PAL_HEADER_SIZE        0x1680


//Buffer used by DV Decode
#define DVDEC_RLCB0_SIZE           0x2000
#define DVDEC_RLCB1_SIZE           0x2000


//PTS STEP, used by fake DV PTS
#define NTSC_PTS_STEP             90000/30
#define PAL_PTS_STEP              90000/25

///////////////////////////////////////////////////////////////////////

/////////////////// Strecture /////////////////////////////////////////////
/*
typedef struct tagTDvDmxInfo
{
  RVSeqInfo vSeqInfo;
  
  BOOL fgInit;
  BOOL fgOn;
  
  enum
  {
    DVDMX_STATE_OFF,	
    DVDMX_STATE_ON
  }dState;

  enum
  {
    DV_PAL_61834   = 0x0B,	
    DV_NTSC_61834  = 0x0C,
    DV_PAL_314M    = 0x0D,
    DV_NTSC_314M   = 0x1C,
    DV_UNKNOW      = 0xFF
  }dwDVSpec;
  
  //Indicate who set the spec
  BOOL fgSpecsetByHW;
  
  //Total Frame count in Ring FIFO
  UINT32 dFifoFrmCnt;   
  
  //Video Buffer Address
  UINT32 dVBufAddr;
  
  //Single Video Frame Size
  UINT32 dVFrmSize;     
  
  //Audio Buffer Address
  UINT32 dABufAddr;
  
  //Single Audio Frame Size
  UINT32 dAFrmSize;     

  //Header Buffer
  UINT32 dHBufAddr;
  UINT32 dHFrmSize;
  
  //R/W pointer
  UINT32 dRdIdx;
  UINT32 dWrIdx;
  
  //Status Record
  UINT32 dwTotalISR;
  UINT32 dwTotalFrame;
  UINT32 dwTotalNOEQERROR;
  UINT32 dwTotalSEQERROR;
  UINT32 dwTotalVIDEOFULL;
  
  #if DV_DEMUXER_PRE_PROCESS
    UINT32 dwDVDmxLastPN;
    UINT32 dwDVDmxPNSameCount;
    UINT32 dwDVDmxIdleCount;
  #endif
  
}TDvDmxInfo;
*/
typedef struct tagTDvInfo
{
  BOOL fgConnect;
  BOOL fgStartDec;
  BOOL fgSetPic;
  UINT16 wFrm_Buf_Ns;
  UINT32 dwArrayIdx;

  //RVFrmBufAddr * pVFBufArray;
 
  BOOL fgIsDecoding;

  //Current Source Idx in Fifo which is being decoded
  UINT32 dSrcIdx;
  
  //Memory used by DVDec
  UINT32 dRLCB0Addr;
  UINT32 dRLCB1Addr;
  

  //Current Decode Frame Buffer Index
  UINT32 dwCurDecIdx;
  
  //Current Decode Buffer Address
  UINT32 dwDecYBuffer;
  UINT32 dwDecCBuffer;
  
  //Last Decode Buffer Address
  UINT32 dwDecYBufferLast;
  UINT32 dwDecCBufferLast;
 
  //Current Display Frame Buffer Index
  UINT32 dwCurDisIdx;
  
  //Current Display Buffer Address
  UINT32 dwDisYBuffer;
  UINT32 dwDisCBuffer;
  
  UINT32 dPicW;
  UINT32 dPicH;

	//Video Buffer Address
	UINT32 dVBufAddr;
 
	UINT32  u4VLDRdPtr;
	UINT32  u4VLDWrPtr;
	UINT32  u4VFifoSa;                 ///< Video Fifo memory start address    
	UINT32  u4VFifoEa;                 ///< Video Fifo memory end address

	UINT32  dwDVSpec;
	
	//Single Video Frame Size
	UINT32 dVFrmSize;   

} TDvInfo;

/*
typedef struct _VDEC_INFO_H264_BS_INIT_PRM_T_
{
    UINT32  u4VLDRdPtr;
    UINT32  u4VLDWrPtr;
    UINT32  u4VFifoSa;                 ///< Video Fifo memory start address    
    UINT32  u4VFifoEa;                 ///< Video Fifo memory end address
}VDEC_INFO_H264_BS_INIT_PRM_T;


typedef struct _VDEC_INFO_H264_INIT_PRM_T_
{
    UINT32  u4FGSeedbase;
    UINT32  u4CompModelValue; 
    UINT32  u4FGDatabase;
    VDEC_INFO_H264_BS_INIT_PRM_T rH264BSInitPrm;
}VDEC_INFO_H264_INIT_PRM_T;
*/
///////////////////////////////////////////////////////////////////////

/////////////////// Extern Functions and Variables ////////////////////

//Status
extern TDvInfo    _tDvInfo_8520;
//extern TDvDmxInfo _tDvDmx;
extern UINT32		_dwVLDID_8520;

//Memory Buffer

#if 0 //DV_HAS_OWN_INDIVIDUAL_DMX_FIFO  /* Allocate FIFO Buffer in DV module */
  extern BYTE *const _pbVBufAddr;
  extern BYTE *const _pbABufAddr;
#endif
//extern BYTE *const _pbRLCB0Addr;
//extern BYTE *const _pbRLCB1Addr;

//PTS
/*
extern UINT32 _dwDvCurDecIdx;
extern UINT32 _pdDvPts[DV_NTSC_FRM_CNT];
extern UINT32 _dwDvPtsStep;
extern UINT32 _dwDvPts;
*/

//For dv_play.c
extern void vDvDecInitIrq_8520(void);
extern void vDvPlayLoop_8520(void);
extern BYTE bDvNormDecProc_8520(BOOL fgRealAVIn);

//For dv_util.c
extern void vRstDvInfo_8520(void);
extern BOOL fgIsDvSpecSet_8520(void);
extern void vDmxReceiveOneFrm(void);

//For dv_irq.c
extern void vDvDecInstallIrq_8520(void (*pvIrqEntry)(void));
extern void vDvDecReleaseIrq_8520(void);
extern void vNullIrq_8520(void);
extern void vDvDmxInstallIrq(void (*pvIrqEntry)(void));
extern void vDvDmxReleaseIrq(void);
extern void vDvDemuxerIrq(INT32 iTmp);

//For dvdec_util.c
extern void vInitDvDec_8520(UINT32 u4InstID);
extern void vDvDecAllocMem_8520(void);
extern void vDvDecFreeMem(void);
extern BOOL fgDmxSetDVSpec(UINT32 dwSpec);
extern void vDvDecOneFrm_8520(UINT32 dDecIdx);
extern BOOL fgIsDvDecoding_8520(void);
extern void vDvDecDone_8520(void);
extern void vDvDecIrq_8520(void);



#endif //#ifndef _HAL_VDEC_H264_IF_H_

