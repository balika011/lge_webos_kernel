#ifndef _VDEC_INFO_VERIFY_H_
#define _VDEC_INFO_VERIFY_H_

#include "drv_config.h"


#if (!CONFIG_DRV_LINUX)
#include <stdio.h>
#else
#include "x_os.h"

#ifndef VDEC_EMUVER_FILEIO
#include "stdio.h"
#else //VDEC_EMUVER_FILEIO
#define  FILE       UINT32
#endif //VDEC_EMUVER_FILEIO
#define strcpy     x_strcpy
#define sprintf     x_sprintf
#define strlen      x_strlen
#endif

#include "x_typedef.h"

typedef struct _VDEC_INFO_VERIFY_FILE_INFO_T_
{
  BOOL     fgGetFileInfo;
  UINT32  u4FileLength;
  UINT32  u4FileOffset;
  UCHAR  *pucTargetAddr;
  UINT32  u4TargetSz;
  UINT32  u4RealGetBytes;
  CHAR     bFileName[300];
  FILE *pFile;
  INT32 i4FileId;

  //xiaolei.li's
  #ifdef SEMI_RING_FIFO
  UINT32 u4BSLenConsumed; // bitstream length consumed for IDE seek 
  UINT32 u4BSLenOffset;
  CHAR   ucVFIFOEnough; //if bitstream length is less than VFIFO, it will be setted to 1. 1 as default
  #endif
  
}VDEC_INFO_VERIFY_FILE_INFO_T;

typedef struct _VDEC_INFO_VERIFY_MAP_UNMAP_INFO_T_
{
  UINT32  u4PhyVififoSa;
  UINT32  u4PhyRMFrmInfoSa;
  UINT32  u4PhyRMCRCResultBuf;
  UINT32  u4PhyRMGoldenDataBuf;
  UINT32  u4PhyRMOutBufY;
  UINT32  u4PhyRMOutBufC;
  UINT32 u4PhyPucDumpYBuf;
  UINT32 u4PhyPucDumpCBuf;
  UINT32 u4PhyPucPic0Y;
  UINT32 u4PhyPucPic0C;
  UINT32 u4PhyPucPic1Y;
  UINT32 u4PhyPucPic1C;  
  UINT32 u4PhyPucPic2Y;
  UINT32 u4PhyPucPic2C; 
  UINT32 u4PhypucDecWorkBuf;
  UINT32 u4PhypucDecWorkCBuf;
  
}VDEC_INFO_VERIFY_MAP_UNMAP_INFO_T;


typedef struct _VDEC_INFO_VERIFY_DEC_T_
{
  UCHAR   ucState;
  UINT32  u4DecFlag;
}VDEC_INFO_VERIFY_DEC_T;


typedef struct _VDEC_INFO_VERIFY_PIC_T_
{
  UCHAR ucMpegVer;
  UINT32 u4W;
  UINT32 u4H;
}VDEC_INFO_VERIFY_PIC_T;


#endif

