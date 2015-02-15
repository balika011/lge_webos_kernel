/**********************************************************************/
/***************           MTK CONFIDENTIAL            ****************/
/***************                                       ****************/
/***************   Description : MT8118 MTKPrintf      ****************/
/***************                 Procedure             ****************/
/***************                                       ****************/
/***************       Company : MediaTek Inc.         ****************/
/***************    Programmer : Ted Hu                ****************/
/**********************************************************************/
#define _VDEC_VERIFY_MAIN_C_
#include "drv_config.h"
#include "x_ckgen.h"
#include "x_debug.h"

#if (!CONFIG_DRV_LINUX)
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef CONFIG_TV_DRV_VFY 
#include "vdec_vfy_debug.h"
#endif // CONFIG_TV_DRV_VFY

#include "vdec_verify_file_common.h"
#include "vdec_verify_filesetting.h"
#include "vdec_ide.h"
#include "vdec_verify_mm_map.h"
#include "vdec_verify_mpv_prov.h"
#include "vdec_verify_vdec.h"
#include "vdec_verify_vdecode.h"
#include "vdec_hal_if_common.h"
#include "vdec_verify_vparser_rm.h"
#include "vdec_verify_vparser_vp6.h"

extern int rand(void);

#ifndef CONFIG_TV_DRV_VFY 
#include <mach/cache_operation.h> 
#endif // CONFIG_TV_DRV_VFY

#ifdef SATA_HDD_FS_SUPPORT
#include "sata_fs_io.h"
#endif       

#ifdef CONFIG_TV_DRV_VFY
#include "x_hal_5381.h"
#include "x_hal_926.h"
#else
#include "x_hal_ic.h"
#include "x_hal_1176.h"
#endif //CONFIG_TV_DRV_VFY
extern INT32 CLI_Parser(const CHAR *szCmdBuf);
void vVdecUDTVInit(UINT32 u4InstID);
void vVDecClockSelect(void);
void vVDecTouch(UINT32  u4InstID,UCHAR ucVldID);
void vDrmaBusySet (UINT32  u4InstID);
BOOL _fgSemaCreated[2]={FALSE,FALSE};
extern UCHAR u4TempSrc[20480];

// *********************************************************************
// Function    : void vVDecReDecSetting(UINT32 u4InstID)
// Description : System entry point
// Parameter   : None
// Return      : None
// *********************************************************************
void vVDecReDecSetting(UINT32  u4InstID, UINT32 u4ReDecPicNum, UINT32 u4ReDecTimes)
{
#ifdef REDEC 
    _u4ReDecPicNum[u4InstID] = u4ReDecPicNum;
    _u4ReDecNum[u4InstID] = u4ReDecTimes;
#endif    
}

void Rm_mem_set(char *ptr, char c_value, UINT32 size)
{
	UINT32 i;
	
	if (ptr == NULL)
	{
		return;
	}

	
	for(i=0; i < size; i++)
	{
		*ptr = c_value;
		 ptr++;
	}

	return;
}

BOOL fgVDecVerify_PrepareInfo_RM(UINT32 u4InstID)
{
  BOOL fgOpen;
  char strMessage[512];

  {
  	 
	 Rm_mem_set(strMessage, 0, 512);
	 sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][0]);
	 UTIL_Printf("[VDEC]Test0 %s\n", strMessage);
     vVDecOutputDebugString(strMessage);

	 Rm_mem_set(strMessage, 0, 512);
	 sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][1]);
	 UTIL_Printf("[VDEC]Test1 %s\n", strMessage);
     vVDecOutputDebugString(strMessage);

	 Rm_mem_set(strMessage, 0, 512);
	 sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][2]);
	 UTIL_Printf("[VDEC]Test2 %s\n", strMessage);
     vVDecOutputDebugString(strMessage);

	 Rm_mem_set(strMessage, 0, 512);
	 sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][3]);
	 UTIL_Printf("[VDEC]Test3 %s\n", strMessage);
     vVDecOutputDebugString(strMessage);

	 Rm_mem_set(strMessage, 0, 512);
	 sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][4]);
	 UTIL_Printf("[VDEC]Test4 %s\n", strMessage);
     vVDecOutputDebugString(strMessage);

	 Rm_mem_set(strMessage, 0, 512);
	 sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][5]);
	 UTIL_Printf("[VDEC]Test5 %s\n", strMessage);
     vVDecOutputDebugString(strMessage);

	 Rm_mem_set(strMessage, 0, 512);
	 sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][6]);
	 UTIL_Printf("[VDEC]Test6 %s\n", strMessage);
     vVDecOutputDebugString(strMessage);

	 Rm_mem_set(strMessage, 0, 512);
	 sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][7]);
	 UTIL_Printf("[VDEC]Test7 %s\n", strMessage);
     vVDecOutputDebugString(strMessage);

	 Rm_mem_set(strMessage, 0, 512);
	 sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][11]);
	 UTIL_Printf("[VDEC]Test11 %s\n", strMessage);
     vVDecOutputDebugString(strMessage);

  }
  
  #ifdef RM_ATSPEED_TEST_ENABLE
  //AU Info 
  _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
  _tInFileInfo[u4InstID].pucTargetAddr = _pucRMAULikeBuf[u4InstID];
  _tInFileInfo[u4InstID].u4TargetSz = RM_AULIKEBUF_SZ;    
  _tInFileInfo[u4InstID].u4FileLength = 0;    
  _tInFileInfo[u4InstID].u4FileOffset = 0;

  sprintf(strMessage, "%s: ", _bFileStr1[u4InstID][RM_AUFIFO_INDEX]);
  vVDecOutputDebugString(strMessage);

  fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][RM_AUFIFO_INDEX],"r+b", &_tInFileInfo[u4InstID]);
  if(fgOpen == FALSE)
  {
    vVDecOutputDebugString("Open bit-stream fail\n");
    strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
    sprintf(strMessage, "%s", "Open bit-stream fail\n");
    fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
    return FALSE;
  }

  //Checksum Info
  #if 1
  _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
  _tInFileInfo[u4InstID].pucTargetAddr = _pucRMChecksumBuf[u4InstID];
  _tInFileInfo[u4InstID].u4TargetSz = RM_CHECKSUM_BUFFER_SZ;    
  _tInFileInfo[u4InstID].u4FileLength = 0;    
  _tInFileInfo[u4InstID].u4FileOffset = 0;

  sprintf(strMessage, "%s: ", _bFileStr1[u4InstID][RM_SUMINFO_INDEX]);
  vVDecOutputDebugString(strMessage);

  fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][RM_SUMINFO_INDEX],"r+b", &_tInFileInfo[u4InstID]);
  if(fgOpen == FALSE)
  {
    vVDecOutputDebugString("Open bit-stream fail\n");
    strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
    sprintf(strMessage, "%s", "Open bit-stream fail\n");
    fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
    return FALSE;
  }
  #endif //0
  #endif //RM_ATSPEED_TEST_ENABLE

  //Read RM Frame Info from PC  
  _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
  _tInFileInfo[u4InstID].pucTargetAddr = _pucRMFrmInfoBuf[u4InstID];
  _tInFileInfo[u4InstID].u4TargetSz = RM_FRMINFO_SZ;    
  _tInFileInfo[u4InstID].u4FileLength = 0;    
  _tInFileInfo[u4InstID].u4FileOffset = 0;
  
  sprintf(strMessage, "Test %s: ", _bFileStr1[u4InstID][RM_FRMINFO_INDEX]);
  vVDecOutputDebugString(strMessage);

  fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][RM_FRMINFO_INDEX],"r+b", &_tInFileInfo[u4InstID]);
  if(fgOpen == FALSE)
  {
    vVDecOutputDebugString("Open bit-stream fail\n");
    strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
    sprintf(strMessage, "%s", "Open bit-stream fail\n");
    fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
    return FALSE;
  }

#ifndef RM_CRCCHECKFLOW_SUPPORT
#ifndef RM_ATSPEED_TEST_ENABLE
  //Read RM Goldem from PC
  _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
  _tInFileInfo[u4InstID].pucTargetAddr = _pucRMGoldenDataBuf[u4InstID];
  _tInFileInfo[u4InstID].u4TargetSz = RM_GOLDENDATA_SZ;    
  _tInFileInfo[u4InstID].u4FileLength = 0;    
  _tInFileInfo[u4InstID].u4FileOffset = 0;

  sprintf(strMessage, "%s: ", _bFileStr1[u4InstID][5]);
  vVDecOutputDebugString(strMessage);

  fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][5],"r+b", &_tInFileInfo[u4InstID]);
  if(fgOpen == FALSE)
  {
    vVDecOutputDebugString("Open bit-stream fail\n");
    strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
    sprintf(strMessage, "%s", "Open bit-stream fail\n");
    fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
    return FALSE;
  }
#endif //#ifndef RM_ATSPEED_TEST_ENABLE        
#endif //RM_CRCCHECKFLOW_SUPPORT

  #ifdef RM_CRCCHECKFLOW_SUPPORT
  #ifdef RM_CRCCHECK_ENABLE
  //Only for CRC Check
  if (fgRMCheckCRCResult)
  {
    _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
    _tInFileInfo[u4InstID].pucTargetAddr = _pucRMCRCResultBuf[u4InstID];
    _tInFileInfo[u4InstID].u4TargetSz = RM_CRCRESULT_SZ;    
    _tInFileInfo[u4InstID].u4FileLength = 0;    
    _tInFileInfo[u4InstID].u4FileOffset = 0;

    sprintf(strMessage, "%s: ", _bFileStr1[u4InstID][RM_CRCINFO_INDEX]);
    vVDecOutputDebugString(strMessage);

    fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][RM_CRCINFO_INDEX],"r+b", &_tInFileInfo[u4InstID]);
    if(fgOpen == FALSE)
    {
      vVDecOutputDebugString("Open bit-stream fail\n");
      strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
      sprintf(strMessage, "%s", "Open bit-stream fail\n");
      fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
      return FALSE;
    }
  }
  #endif //RM_CRCCHECK_ENABLE
  #endif //RM_CRCCHECKFLOW_SUPPORT

  #ifdef RM_SUPPORT_NEW_TABLE_MODE
  //Read rom table(inter and intra) from PC
  	{
  		//Open intra
		_tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
	  	_tInFileInfo[u4InstID].pucTargetAddr = _pucRMIntraTableBuf[u4InstID];
	  	_tInFileInfo[u4InstID].u4TargetSz = RM_INTRA_TABLE_DATA_SZ;    
	  	_tInFileInfo[u4InstID].u4FileLength = 0;    
	  	_tInFileInfo[u4InstID].u4FileOffset = 0;

	  	sprintf(strMessage, "%s: ", _Rm_intra_rom_table_list[u4InstID]);
	  	vVDecOutputDebugString(strMessage);

	  	fgOpen = fgOpenFile(u4InstID, _Rm_intra_rom_table_list[u4InstID],"r+b", &_tInFileInfo[u4InstID]);
	  	if(fgOpen == FALSE)
	  	{
	    	vVDecOutputDebugString("Open bit-stream fail\n");
	    	strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
	    	sprintf(strMessage, "%s", "Open bit-stream fail\n");
	    	fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
	    	return FALSE;
	  	}
		else
		{

			UTIL_Printf("Succeed in opening intra table\n");
		}

        //Open inter
        _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
	  	_tInFileInfo[u4InstID].pucTargetAddr = _pucRMInterTableBuf[u4InstID];
	  	_tInFileInfo[u4InstID].u4TargetSz = RM_INTER_TABLE_DATA_SZ;    
	  	_tInFileInfo[u4InstID].u4FileLength = 0;    
	  	_tInFileInfo[u4InstID].u4FileOffset = 0;

	  	sprintf(strMessage, "%s: ", _Rm_inter_rom_table_list[u4InstID]);
	  	vVDecOutputDebugString(strMessage);

	  	fgOpen = fgOpenFile(u4InstID, _Rm_inter_rom_table_list[u4InstID],"r+b", &_tInFileInfo[u4InstID]);
	  	if(fgOpen == FALSE)
	  	{
	    	vVDecOutputDebugString("Open bit-stream fail\n");
	    	strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
	    	sprintf(strMessage, "%s", "Open bit-stream fail\n");
	    	fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
	    	return FALSE;
	  	}
		else
		{

			UTIL_Printf("Succeed in opening inter talbe\n");
		}
  	}


  #endif

  return TRUE;
}

UINT32 u4Get10Value(UINT8 u1Num)
{
	if (u1Num > 1)
	{
		UINT8 i=0;
		UINT32 u4Value=10;
		
		for(i=1; i < u1Num; i++)
		{
			u4Value = u4Value * 10;
		}
		return u4Value;
	}
	else if(u1Num == 1)
	{
		return 10;
	}
	else
	{
		return 0;
	}
}

// *********************************************************************
// Function    : void vVDecVerifyThread(UINT32 u4InstID)
// Description : System entry point
// Parameter   : None
// Return      : None
// *********************************************************************
#if DTV_COMMON_CONFIG
extern void vVDecTestAVCBarl(void);
extern void vVDecTestVP6Barl(void);
extern void vVDecTestVP8Barl(void);

extern UCHAR *ucVFifo;
extern UCHAR *pvaVfifo;
extern BOOL _fgVDecWorkBufAllocated[2];

#define x_alloc_aligned_verify_mem(u4Size, u4Align, fgChannelA) VIRTUAL(BSP_AllocAlignedDmaMemory(u4Size,u4Align))
#endif
void vVDecVerifyThread(void *param_array)
{
    void** ppv_param = (void**)param_array ;
    UINT32 u4InstID;
    UCHAR ucVldID = 0xff;
    UINT32 u4WmvMode;
    BOOL fgOpen,fgExistVerifyLoop;
    char strMessage[512];
    CHAR strVP6SizeFile[ 512];
#if DTV_COMMON_CONFIG
    CHAR *pTest = NULL;
    CHAR *pvTest = NULL;
#endif
    u4InstID =  (UINT32) ppv_param[0];
    u4WmvMode  =  (UINT32) ppv_param[1];   
    _u4K2KMode[u4InstID] = (UINT32) ppv_param[1];
    _u4LaeMode[u4InstID] = (UINT32) ppv_param[2];
    _u4DualCoreEnable[u4InstID] = (UINT32) ppv_param[3];
    _u4Main10Mode = (UINT32) ppv_param[4];
    _u4VP9LogOption = (UINT32) ppv_param[4];
    _u4UFOMode = (UINT32) ppv_param[5];
    _u4CRCMode = (UINT32) ppv_param[6];
    _fgVDecWorkBufAllocated[u4InstID] = FALSE;
    fgExistVerifyLoop = FALSE;
    UTIL_Printf("u4InstID:%d, u4WmvMode:%d\n", u4InstID, u4WmvMode);
    UTIL_Printf("_u4UFOMode:%d, _u4CRCMode:%d\n", _u4UFOMode, _u4CRCMode);
    if (u4WmvMode > 0)
    {
        _u4WmvMode[u4InstID] = 1;
        _u4AdobeMode[u4InstID] = 1;
    }
    else
    {
        _u4WmvMode[u4InstID] = 0;
        _u4AdobeMode[u4InstID] = 0;
    }
	vVDecInit(u4InstID);
    if(_u4DualCoreEnable[u4InstID])
    {
        UINT32 u4LAEId;
        u4LAEId = (u4InstID == 0) ? VDEC_LAE0 : VDEC_LAE1;
        //vVDecInit(VDEC_LAE0);
        vVDecInit(u4LAEId);
        if(_u4DualCoreEnable[u4InstID] == 1) //MultiCore mode
        {
            _u4DualCoreEnable[VDEC_VLD1] = 1;
        }
    }
    if(CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550) //xiaolei's  wmv new mode after chip8550
    {
    	_u4WmvMode[u4InstID] = 1;
        _u4AdobeMode[u4InstID] = 1;
    }
  
    vMemoryAllocate(u4InstID);
    UTIL_Printf("[VDEC] Allocate memory OK\n");
#if VDEC_DRAM_BUSY_TEST
    vDrmaBusySet (u4InstID);
#endif

#ifdef SATA_HDD_READ_SUPPORT
    #ifndef  SATA_HDD_FS_SUPPORT
    fgInitHDDFileAccess(u4InstID);
    #else
      //FS mount
    fgHDDFsMount(0);
   #endif
#endif

#ifdef IDE_READ_SUPPORT
    _DmxIdeReset();
#endif

#ifdef BARREL2_THREAD_SUPPORT
  if(!_fgSemaCreated[u4InstID])
  {
    VERIFY (x_sema_create(&_ahVDecEndSema[u4InstID], X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
    _fgSemaCreated[u4InstID] = TRUE;
  }
#endif
    _u4StartCompPicNum[u4InstID] = 0;
    _u4EndCompPicNum[u4InstID] = 0xffffffff;
    _u4DumpRegPicNum[u4InstID] = 0xffffffff;
    _u4MaxReDecBistreamCnt[u4InstID] = 0;
    _u4LaeDecCnt[u4InstID] = 0;
    _u4Bitstream_Num[u4InstID] = 0;

#ifdef REDEC 
  //_u4ReDecNum[u4InstID] = 0;
  //_u4ReDecPicNum[u4InstID] = 0;
  _u4ReDecCnt[u4InstID] = 0;
#endif
  _u4RedecBistreamCnt[u4InstID] = 0xffffffff;
  _u4FileCnt[u4InstID] = 0;
  _u4FileListIdx[u4InstID] = 0;
  _u4FilePos[u4InstID] = 0;
  #ifdef WMV_CRC_COMPOSITE_CHECK_ENABLE
  _u4CRCCnt[u4InstID] = 0;
  #endif
#ifdef VERIFICATION_DOWN_SCALE
  _fgVDSCLEnableRandomTest[u4InstID] = TRUE;
   _fgVDSCLEnableLumaKeyTest[u4InstID] = FALSE;
#endif
#ifdef BARREL2_SUPPORT
  _u4BSID[u4InstID] = 1;
#else
  _u4BSID[u4InstID] = 0;
#endif
    u4BitstreamLoaded[u4InstID] = 0;
    u4OpenSimDump[u4InstID] = 0;
#if DTV_COMMON_CONFIG
    #if MPEG2_TEST_TV_BITSTREAM
    _u2AddressSwapMode[u4InstID] = 2;
    #else
    _u2AddressSwapMode[u4InstID] = ADDRSWAP_OFF;
    #endif
#else
#if VDEC_DDR3_SUPPORT    
    _u2AddressSwapMode[u4InstID] = ADDRSWAP_DDR3;
#else
    _u2AddressSwapMode[u4InstID] = ADDRSWAP_OFF;
#endif
#endif

  _tFileListInfo[u4InstID].i4FileId = 0xFFFFFFFF;
  UTIL_Printf("[VDEC%d] test begin 4K2K MODE %d _u4LaeMode %d McoreMode %d\n",u4InstID,_u4K2KMode[u4InstID],_u4LaeMode[u4InstID],_u4DualCoreEnable[u4InstID]);
  while(fgVdecReadFileName(u4InstID, &_tFileListInfo[u4InstID], &_tFileListRecInfo[u4InstID], &_u4StartCompPicNum[u4InstID], &_u4EndCompPicNum[u4InstID], &_u4DumpRegPicNum[u4InstID]))
  {
     _fgDec2ndFldPic[u4InstID] = 0;
     _u4SkipFrameCnt[u4InstID] = 0;
     _u4PicCnt[u4InstID] = 0;
     if(fgExistVerifyLoop)
     {
       break;
     }

    #if VDEC_TEST_ADDSWAP    
    if(_u4CodecVer[u4InstID] != VDEC_MPEG2)
    {
        _u2AddressSwapMode[u4InstID] = (UINT16) (((UINT32) rand())%8);
        while(_u2AddressSwapMode[u4InstID] == 3 || _u2AddressSwapMode[u4InstID] == 7)
        {
            _u2AddressSwapMode[u4InstID] = (UINT16) (((UINT32) rand())%8);
        }
    }
    else
    {
        _u2AddressSwapMode[u4InstID] = 2;
    }
    UTIL_Printf("[VDEC] Address Swap Mode = %d\n", _u2AddressSwapMode[u4InstID]);//MPEG2 use fix swap mode:2
    #endif

	
     if (!fgVDecAllocWorkBuffer(u4InstID))
     {
         UTIL_Printf("[VDEC]Memory alloc failed\n");
         break;
     }

     #if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) && (!CONFIG_DRV_FPGA_BOARD))
 	    //vVDecSetVldMcClk(u4InstID,_u4CodecVer[u4InstID]);
     #endif

     #ifdef VDEC_VIDEOCODEC_RM
     fgOpen = fgVDecVerify_PrepareInfo_RM(u4InstID);

     if(fgOpen == FALSE)
     {
       continue;
     }
     #endif //VDEC_VIDEOCODEC_RM
   
#ifdef LETTERBOX_SUPPORT
     if(_u4CodecVer[u4InstID] == VDEC_H264)
     {
     VDEC_INFO_VERIFY_FILE_INFO_T tInFileInfo;
     
     tInFileInfo.fgGetFileInfo = TRUE;
     tInFileInfo.pucTargetAddr = (UCHAR*) _pucSettingFileSa[u4InstID];
     tInFileInfo.u4TargetSz = (UINT32) FILE_LIST_SZ;    
     tInFileInfo.u4FileLength = 0;    
     tInFileInfo.u4FileOffset = 0;
	 #ifdef SEMI_RING_FIFO
	 tInFileInfo.u4BSLenOffset = 0;
	 tInFileInfo.ucVFIFOEnough = 1;
	 #endif
      sprintf(strMessage, "%s: ", _bFileStr1[u4InstID][12]);
      vVDecOutputDebugString(strMessage);
      UTIL_Printf("=====> Setting File Name = %s < ===== \n", strMessage);
      fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][12],"r+b", &tInFileInfo);
      if(fgOpen == FALSE)
      {
       vVDecOutputDebugString("Open setting file fail\n");
       strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
       sprintf(strMessage, "%s", "Open setting file fail\n");
       fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
       //vVDecFreeWorkBuffer(u4InstID);
       continue;
      }
      _pcLBDSettingFile[u4InstID] = (char *)_pucSettingFileSa[u4InstID];

     tInFileInfo.fgGetFileInfo = TRUE;
     tInFileInfo.pucTargetAddr = (UCHAR*) _pucGoldenFileSa[u4InstID];
     tInFileInfo.u4TargetSz = (UINT32) FILE_LIST_SZ;    
     tInFileInfo.u4FileLength = 0;    
     tInFileInfo.u4FileOffset = 0;
	 #ifdef SEMI_RING_FIFO
	 tInFileInfo.u4BSLenOffset = 0;
	 tInFileInfo.ucVFIFOEnough = 1;
     #endif
      sprintf(strMessage, "%s: ", _bFileStr1[u4InstID][13]);
      vVDecOutputDebugString(strMessage);
      UTIL_Printf("=====> Golden File Name = %s < ===== \n", strMessage);
      fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][13],"r+b", &tInFileInfo);
      if(fgOpen == FALSE)
      {
       vVDecOutputDebugString("Open golden file fail\n");
       strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
       sprintf(strMessage, "%s", "Open golden file fail\n");
       fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
       //vVDecFreeWorkBuffer(u4InstID);
       continue;
      }
      _pcLBDGoldenFile[u4InstID] = (char *)_pucGoldenFileSa[u4InstID];
      }
#endif
     
   
   #ifdef  RING_VFIFO_SUPPORT
      _u4LoadBitstreamCnt[u4InstID] = 0;
    #endif
     _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
     #ifdef RM_RINGVIFO_FLOW
     _tInFileInfo[u4InstID].pucTargetAddr = (UCHAR*) _pucRMRingWorkBuf[u4InstID];
     _tInFileInfo[u4InstID].u4TargetSz = (UINT32) RM_RINGFLOW_TEMPFIFO_SZ;    
     #else //RM_RINGVIFO_FLOW
     _tInFileInfo[u4InstID].pucTargetAddr = _pucVFifo[u4InstID];
     _tInFileInfo[u4InstID].u4TargetSz = V_FIFO_SZ;    
     #endif //RM_RINGVIFO_FLOW
     _tInFileInfo[u4InstID].u4FileLength = 0;    
     _tInFileInfo[u4InstID].u4FileOffset = 0;

	 #ifdef SEMI_RING_FIFO
	 _tInFileInfo->u4BSLenConsumed = 0; //xiaolei.li's
	 _tInFileInfo->u4BSLenOffset = 0;
	 _tInFileInfo->ucVFIFOEnough = 1;
     #endif
      sprintf(strMessage, "%s: ", _bFileStr1[u4InstID][1]);
      vVDecOutputDebugString(strMessage);
      
      UTIL_Printf("=====> Test File Name = %s < ===== \n", strMessage);
      
      fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);//Load bitstream

	  #ifndef SEMI_RING_FIFO
      if(_tInFileInfo[u4InstID].u4FileLength > V_FIFO_SZ)
       {
         UTIL_Printf("=====>Error:The Vfifo size is not enough!. \n");
         UTIL_Printf("=====>The file's size is 0x%x, video fifo size:0x%x\n", _tInFileInfo[u4InstID].u4FileLength, V_FIFO_SZ);
         //vVDecFreeWorkBuffer(u4InstID);
         //continue;
       }
      #endif
     
   #ifdef  RING_VFIFO_SUPPORT
      _u4LoadBitstreamCnt[u4InstID]++;
    #endif

     if(fgOpen == FALSE)
     {
       vVDecOutputDebugString("Open bit-stream fail\n");
       strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
       sprintf(strMessage, "%s", "Open bit-stream fail\n");
       fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
       //vVDecFreeWorkBuffer(u4InstID);
       continue;
     }
     else
     	{
     	  if((u4InstID == 1) && _ucMVCType[1]) 
     	  {    
     	    _fgMVCReady[0] = TRUE;    
     	    _fgMVCReady[1] = FALSE;	  
     	  }

        u4BitstreamLoaded[u4InstID] ++;
     	}

        //Copy for Temp Buf to VFIFO
        #ifdef RM_RINGVIFO_FLOW
        {
          UINT32 u4VFifoWPtr = (UINT32) _pucVFifo[u4InstID];
          UINT32 u4VFIFOSa = (UINT32) _pucVFifo[u4InstID];
          UINT32 u4VFIFOSz = (UINT32) V_FIFO_SZ;
          UINT32 u4TempVFIFOSa = (UINT32) _pucRMRingWorkBuf[u4InstID];
          UINT32 u4CopySize = _tInFileInfo[u4InstID].u4FileLength;
          UINT32 u4RemSz = 0;

          #ifdef RM_ATSPEED_TEST_ENABLE
          _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecRMDecPrm.u4RMFIFORPtr = (UINT32) _pucRMAULikeBuf[u4InstID];
          #else //RM_ATSPEED_TEST_ENABLE
          _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecRMDecPrm.u4RMFIFORPtr = (UINT32) _pucVFifo[u4InstID];
          #endif //RM_ATSPEED_TEST_ENABLE

          if ((u4VFifoWPtr+_tInFileInfo[u4InstID].u4FileLength) < (UINT32) (u4VFIFOSa+u4VFIFOSz))
          {
            x_memcpy((void*)(u4VFifoWPtr), (void*)u4TempVFIFOSa, u4CopySize);
            _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecRMDecPrm.u4RMVFIFOWPtr = u4VFIFOSa + u4CopySize;
          }
          else
          {
            u4CopySize = u4VFIFOSa+u4VFIFOSz - u4VFifoWPtr;
            u4RemSz = _tInFileInfo[u4InstID].u4FileLength - u4CopySize;
            x_memcpy((void*)(u4VFifoWPtr), (void*)u4TempVFIFOSa, u4CopySize);

            x_memcpy((void*)(u4VFIFOSa), (void*)(u4TempVFIFOSa+u4CopySize), u4RemSz);

            _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecRMDecPrm.u4RMVFIFOWPtr = u4VFIFOSa + u4RemSz;
          }
        }
        #endif //RM_RINGVIFO_FLOW

        
     if(_u4CodecVer[u4InstID] == VDEC_H264)
     {
       if(_tInFileInfo[u4InstID].u4FileLength > V_FIFO_SZ)
       {
        vAddStartCode2Dram(_pucVFifo[u4InstID] + V_FIFO_SZ - 4);
       }
       else
       {
        vAddStartCode2Dram(_pucVFifo[u4InstID]+_tInFileInfo[u4InstID].u4FileLength);
       }
     }

     strcpy(_tFileListRecInfo[u4InstID].bFileName,_FileList_Rec[u4InstID]);
     sprintf(strMessage,"The bitstream file length / real returns is %d/%d (0x%.8x/0x%.8x)\n",
      _tInFileInfo[u4InstID].u4FileLength, _tInFileInfo[u4InstID].u4RealGetBytes,_tInFileInfo[u4InstID].u4FileLength, _tInFileInfo[u4InstID].u4RealGetBytes);
     fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
        
     if (_u4CodecVer[u4InstID] == VDEC_VP6)
     {
         _rSizeFileInfo.fgGetFileInfo = TRUE;  
         _rSizeFileInfo.pucTargetAddr = _pucSizeFileBuf[u4InstID];
         _rSizeFileInfo.u4TargetSz = 1024*400;;  
         _rSizeFileInfo.u4FileLength = 0;
         sprintf(strVP6SizeFile, "%s.size", _bFileStr1[u4InstID][1]);
         fgOpenFile(u4InstID, strVP6SizeFile,"r+b", &_rSizeFileInfo);

         if(fgOpen == FALSE)
         {
             sprintf(strMessage, "%s", "Open VP6 size fail\n");
             fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
             continue;
        }

         _fgVP6SmallFlolder[u4InstID] = fgVP6SmallFolder(u4InstID);

     }   

    UTIL_Printf("_u4CodecVer is %d\n", _u4CodecVer[u4InstID]);
	

	if(_u4CodecVer[u4InstID] == VDEC_VP6)
	{
		CHAR _bFileNameCRC[20] = {"\\MergeCRC.out\0"};
		CHAR _bFileCnt[20] = {"\\pic_cnt.txt\0"};
	    //CHAR _bGoldFileName[256] = {"\\crc.bin\0"};
	    CHAR _bPatternPath [256];
	    CHAR _bPathAddStr[256] = {"emu_data\\pattern\\\0"};
	    CHAR _bFileName [256];
	    UINT32 u4Temp;

	    UINT32 path_byte_addr = 0;
	    UINT32 filename_byte_addr = 0;
	    INT32 i, j;
	    UCHAR strMessage [ 256];

		
		for (i=0; ; i++)
	    {
	          if (_bFileStr1[u4InstID][1][i] == '\0')
	            break;

	          if (_bFileStr1[u4InstID][1][i] == 'b' || _bFileStr1[u4InstID][1][i] == 'B')
	          {
	              if( (_bFileStr1[u4InstID][1][i+1] == 'i' || _bFileStr1[u4InstID][1][i+1] == 'I')
	              && (_bFileStr1[u4InstID][1][i+2] == 't' || _bFileStr1[u4InstID][1][i+2] == 'T')
	              && (_bFileStr1[u4InstID][1][i+3] == 's' || _bFileStr1[u4InstID][1][i+3] == 'S')
	              && (_bFileStr1[u4InstID][1][i+4] == 't' || _bFileStr1[u4InstID][1][i+4] == 'T')
	              && (_bFileStr1[u4InstID][1][i+5] == 'r' || _bFileStr1[u4InstID][1][i+5] == 'R')
	              && (_bFileStr1[u4InstID][1][i+6] == 'e' || _bFileStr1[u4InstID][1][i+6] == 'E')
	              && (_bFileStr1[u4InstID][1][i+7] == 'a' || _bFileStr1[u4InstID][1][i+7] == 'A')
	              && (_bFileStr1[u4InstID][1][i+8] == 'm' || _bFileStr1[u4InstID][1][i+8] == 'M') )
	              {
	                  path_byte_addr = i;
	              }
	          }
	          else
	          if (_bFileStr1[u4InstID][1][i] == '.')
	          {
	              if( (_bFileStr1[u4InstID][1][i+1] == 'v' || _bFileStr1[u4InstID][1][i+1] == 'V')
	              && (_bFileStr1[u4InstID][1][i+2] == 'd' || _bFileStr1[u4InstID][1][i+2] == 'D')
	              && (_bFileStr1[u4InstID][1][i+3] == 'o' || _bFileStr1[u4InstID][1][i+3] == 'O'))
	              {
	                  filename_byte_addr = i;
	              }
	          }
	    }

	    j = 0;
	    for (i=path_byte_addr+10; i < filename_byte_addr; i++)
	    {
	        _bFileName[j] = _bFileStr1[u4InstID][1][i];
	        j++;
	    }
	    _bFileName[j] = '\0';

	    for (j=0; j < path_byte_addr; j++)
	    {
	        _bPatternPath[j] = _bFileStr1[u4InstID][1][j];
	    }
	    
		u4Temp = sprintf(_bPatternPath+path_byte_addr, "%s", _bPathAddStr);  
		u4Temp += sprintf(_bPatternPath+path_byte_addr+u4Temp, "%s", _bFileName);
		//u4Temp += sprintf(_bPatternPath+path_byte_addr+u4Temp, "%s", _bGoldFileName);	 
		_bPatternPath[path_byte_addr+u4Temp] = '\0';

		UTIL_Printf("pattern  path  %s: \n", _bPatternPath);

        //open pic_cnt.dat
        _pucPicTolCnt[u4InstID][0] = 0;
		_pucPicTolCnt[u4InstID][1] = 0;
		_pucPicTolCnt[u4InstID][2] = 0;
		_pucPicTolCnt[u4InstID][3] = 0;
		_pucPicTolCnt[u4InstID][4] = 0;
		_pucPicTolCnt[u4InstID][5] = 0;
		_tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;	
		_tFBufFileInfo[u4InstID].pucTargetAddr = _pucPicTolCnt[u4InstID];
		_tFBufFileInfo[u4InstID].u4TargetSz = 16;  
		_tFBufFileInfo[u4InstID].u4FileLength = 0;

		u4Temp = sprintf((char*)_bFileStr1[u4InstID][3], "%s", (char*)_bPatternPath);
	    u4Temp += sprintf((char*)_bFileStr1[u4InstID][3] + u4Temp, "%s", _bFileCnt);

		UTIL_Printf("Filecnt path  %s \n", _bFileStr1[u4InstID][3]);
		#ifdef SEMI_RING_FIFO
	    _tFBufFileInfo[u4InstID].u4BSLenOffset = 0;
	    #endif

	#if VDEC_DRAM_BUSY_TEST
	    i4VDEC_HAL_Dram_Busy_Off(u4InstID);
	    fgOpenFile(u4InstID, (char*)_bFileStr1[u4InstID][3],(char*)"r+b", &_tFBufFileInfo[u4InstID]);
	    vDrmaBusySet(u4InstID);
	#else
	    fgOpenFile(u4InstID, (char*)_bFileStr1[u4InstID][3],(char*)"r+b", &_tFBufFileInfo[u4InstID]);
	#endif
	    {
			UCHAR ucNum = 0;
			
			i = 0;
			_u4PicTotalCnt[u4InstID] = 0;
			ucNum = _pucPicTolCnt[u4InstID][4];
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				i = 1;
				_u4PicTotalCnt[u4InstID] = ucNum - 0x30;
			}
			ucNum = _pucPicTolCnt[u4InstID][3];
			
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				if (i==0)
				{
					i += 1;
					_u4PicTotalCnt[u4InstID] = ucNum - 0x30;
				}
				else
				{
					
					_u4PicTotalCnt[u4InstID] += ((ucNum - 0x30) * u4Get10Value(i));
					i += 1;
				}
				
			}
			ucNum = _pucPicTolCnt[u4InstID][2];
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				if (i==0)
				{
					i += 1;
					_u4PicTotalCnt[u4InstID] = (ucNum - 0x30);
				}
				else
				{
					_u4PicTotalCnt[u4InstID] += ((ucNum - 0x30) * u4Get10Value(i));
					i += 1;
				}
			}
			ucNum = _pucPicTolCnt[u4InstID][1];
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				if (i==0)
				{
					_u4PicTotalCnt[u4InstID] = (ucNum - 0x30);
					i += 1;
				}
				else
				{
					_u4PicTotalCnt[u4InstID] += ((ucNum - 0x30) * u4Get10Value(i));
					i += 1;
				}
			}
			ucNum = _pucPicTolCnt[u4InstID][0];
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				if (i==0)
				{
					_u4PicTotalCnt[u4InstID] = (ucNum - 0x30);
					i += 1;
				}
				else
				{
					_u4PicTotalCnt[u4InstID] += ((ucNum - 0x30) * u4Get10Value(i));
					i += 1;
				}	
			}
			UTIL_Printf("_u4PicTotalCnt  %d, i is %d \n", _u4PicTotalCnt[u4InstID], i);
	    }

		#if VDEC_SUPPORT_MERGE_CRC
	    //open crc.bin
		_tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;	
		_tFBufFileInfo[u4InstID].pucTargetAddr = _pucMergeCRCBuf[u4InstID];
		_tFBufFileInfo[u4InstID].u4TargetSz = MERGE_CRC_BUF_SZ;  
		_tFBufFileInfo[u4InstID].u4FileLength = 0;

		#ifdef SEMI_RING_FIFO
	    _tFBufFileInfo[u4InstID].u4BSLenOffset = 0;
	    #endif
		 u4Temp = sprintf((char*)_bFileStr1[u4InstID][3], "%s", (char*)_bPatternPath);
         u4Temp += sprintf((char*)_bFileStr1[u4InstID][3] + u4Temp, "%s", _bFileNameCRC);
	
		//vConcateStr((char*)_bFileStr1[u4InstID][3], (char*)_bPatternPath, (char*)_bFileNameCRC, (UINT32)_u4FileCnt[u4InstID]);

        UTIL_Printf("Merge CRC file path  %s \n", _bFileStr1[u4InstID][3]);
	#if VDEC_DRAM_BUSY_TEST
	    i4VDEC_HAL_Dram_Busy_Off(u4InstID);
	    fgOpenFile(u4InstID, (char*)_bFileStr1[u4InstID][3],(char*)"r+b", &_tFBufFileInfo[u4InstID]);
	    vDrmaBusySet(u4InstID);
	#else
	    fgOpenFile(u4InstID, (char*)_bFileStr1[u4InstID][3],(char*)"r+b", &_tFBufFileInfo[u4InstID]);
	#endif

	    #endif
	}

	if(_u4CodecVer[u4InstID] == VDEC_VP8)
	{
		CHAR _bFileNameCRC[20] = {"\\crc.bin\0"};
		CHAR _bFileCnt[20] = {"\\pic_cnt.dat\0"};
	    //CHAR _bGoldFileName[256] = {"\\crc.bin\0"};
	    CHAR _bPatternPath [256];
	    CHAR _bPathAddStr[256] = {"pattern\\golden\\"};
	    CHAR _bFileName [256];
	    UINT32 u4Temp;

	    UINT32 path_byte_addr = 0;
	    UINT32 filename_byte_addr = 0;
	    INT32 i, j;
	    UCHAR strMessage [ 256];

		
		for (i=0; ; i++)
	    {
	          if (_bFileStr1[u4InstID][1][i] == '\0')
	          	break;

	          if (_bFileStr1[u4InstID][1][i] == 'b' || _bFileStr1[u4InstID][1][i] == 'B')
	          {
	              if( (_bFileStr1[u4InstID][1][i+1] == 'i' || _bFileStr1[u4InstID][1][i+1] == 'I')
	              && (_bFileStr1[u4InstID][1][i+2] == 't' || _bFileStr1[u4InstID][1][i+2] == 'T')
	              && (_bFileStr1[u4InstID][1][i+3] == 's' || _bFileStr1[u4InstID][1][i+2] == 'S')
	              && (_bFileStr1[u4InstID][1][i+4] == 't' || _bFileStr1[u4InstID][1][i+4] == 'T')
	              && (_bFileStr1[u4InstID][1][i+5] == 'r' || _bFileStr1[u4InstID][1][i+5] == 'R')
	              && (_bFileStr1[u4InstID][1][i+6] == 'e' || _bFileStr1[u4InstID][1][i+6] == 'E')
	              && (_bFileStr1[u4InstID][1][i+7] == 'a' || _bFileStr1[u4InstID][1][i+7] == 'A')
	              && (_bFileStr1[u4InstID][1][i+8] == 'm' || _bFileStr1[u4InstID][1][i+8] == 'M') )
	              {
	                  path_byte_addr = i;
	              }
	          }
	          else
	          if (_bFileStr1[u4InstID][1][i] == '.')
	          {
	              if( (_bFileStr1[u4InstID][1][i+1] == 'i' || _bFileStr1[u4InstID][1][i+1] == 'I')
	              && (_bFileStr1[u4InstID][1][i+2] == 'v' || _bFileStr1[u4InstID][1][i+2] == 'V')
	              && (_bFileStr1[u4InstID][1][i+3] == 'f' || _bFileStr1[u4InstID][1][i+3] == 'F'))
	              {
	                  filename_byte_addr = i;
	              }
	          }
	    }
	
	    j = 0;
	    for (i=path_byte_addr+10; i < filename_byte_addr; i++)
	    {
	        _bFileName[j] = _bFileStr1[u4InstID][1][i];
	        j++;
	    }
	    _bFileName[j] = '\0';

	    for (j=0; j < path_byte_addr; j++)
	    {
	        _bPatternPath[j] = _bFileStr1[u4InstID][1][j];
	    }
	    
		u4Temp = sprintf(_bPatternPath+path_byte_addr, "%s", _bPathAddStr);  
		u4Temp += sprintf(_bPatternPath+path_byte_addr+u4Temp, "%s", _bFileName);
		//u4Temp += sprintf(_bPatternPath+path_byte_addr+u4Temp, "%s", _bGoldFileName);	 
		_bPatternPath[path_byte_addr+u4Temp] = '\0';

		UTIL_Printf("pattern  path  %s: \n", _bPatternPath);

        //open pic_cnt.dat
        _pucPicTolCnt[u4InstID][0] = 0;
		_pucPicTolCnt[u4InstID][1] = 0;
		_pucPicTolCnt[u4InstID][2] = 0;
		_pucPicTolCnt[u4InstID][3] = 0;
		_pucPicTolCnt[u4InstID][4] = 0;
		_pucPicTolCnt[u4InstID][5] = 0;
		_tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;	
		_tFBufFileInfo[u4InstID].pucTargetAddr = _pucPicTolCnt[u4InstID];
		_tFBufFileInfo[u4InstID].u4TargetSz = 16;  
		_tFBufFileInfo[u4InstID].u4FileLength = 0;

		u4Temp = sprintf((char*)_bFileStr1[u4InstID][3], "%s", (char*)_bPatternPath);
	    u4Temp += sprintf((char*)_bFileStr1[u4InstID][3] + u4Temp, "%s", _bFileCnt);

		UTIL_Printf("Filecnt path  %s \n", _bFileStr1[u4InstID][3]);

		#ifdef SEMI_RING_FIFO
	    _tFBufFileInfo[u4InstID].u4BSLenOffset = 0;
	    #endif
        #if VDEC_DRAM_BUSY_TEST
	    i4VDEC_HAL_Dram_Busy_Off(u4InstID);
	    fgOpenFile(u4InstID, (char*)_bFileStr1[u4InstID][3],(char*)"r+b", &_tFBufFileInfo[u4InstID]);
	    vDrmaBusySet(u4InstID);
	    #else
	    fgOpenFile(u4InstID, (char*)_bFileStr1[u4InstID][3],(char*)"r+b", &_tFBufFileInfo[u4InstID]);
	    #endif
	    {
			UCHAR ucNum = 0;
			
			i = 0;
			_u4PicTotalCnt[u4InstID] = 0;
			ucNum = _pucPicTolCnt[u4InstID][4];
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				i = 1;
				_u4PicTotalCnt[u4InstID] = ucNum - 0x30;
			}
			ucNum = _pucPicTolCnt[u4InstID][3];
			
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				if (i==0)
				{
					i += 1;
					_u4PicTotalCnt[u4InstID] = ucNum - 0x30;
				}
				else
				{
					
					_u4PicTotalCnt[u4InstID] += ((ucNum - 0x30) * u4Get10Value(i));
					i += 1;
				}
				
			}
			ucNum = _pucPicTolCnt[u4InstID][2];
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				if (i==0)
				{
					i += 1;
					_u4PicTotalCnt[u4InstID] = (ucNum - 0x30);
				}
				else
				{
					_u4PicTotalCnt[u4InstID] += ((ucNum - 0x30) * u4Get10Value(i));
					i += 1;
				}
			}
			ucNum = _pucPicTolCnt[u4InstID][1];
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				if (i==0)
				{
					_u4PicTotalCnt[u4InstID] = (ucNum - 0x30);
					i += 1;
				}
				else
				{
					_u4PicTotalCnt[u4InstID] += ((ucNum - 0x30) * u4Get10Value(i));
					i += 1;
				}
			}
			ucNum = _pucPicTolCnt[u4InstID][0];
			if (ucNum >= 0x30 && ucNum <=0x39)
			{
				if (i==0)
				{
					_u4PicTotalCnt[u4InstID] = (ucNum - 0x30);
					i += 1;
				}
				else
				{
					_u4PicTotalCnt[u4InstID] += ((ucNum - 0x30) * u4Get10Value(i));
					i += 1;
				}	
			}
			UTIL_Printf("_u4PicTotalCnt  %d, i is %d \n", _u4PicTotalCnt[u4InstID], i);
	    }
		#if VDEC_SUPPORT_MERGE_CRC
	    //open crc.bin
		_tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;	
		_tFBufFileInfo[u4InstID].pucTargetAddr = _pucMergeCRCBuf[u4InstID];
		_tFBufFileInfo[u4InstID].u4TargetSz = MERGE_CRC_BUF_SZ;  
		_tFBufFileInfo[u4InstID].u4FileLength = 0;

		 u4Temp = sprintf((char*)_bFileStr1[u4InstID][3], "%s", (char*)_bPatternPath);
         u4Temp += sprintf((char*)_bFileStr1[u4InstID][3] + u4Temp, "%s", _bFileNameCRC);
	
		//vConcateStr((char*)_bFileStr1[u4InstID][3], (char*)_bPatternPath, (char*)_bFileNameCRC, (UINT32)_u4FileCnt[u4InstID]);

        UTIL_Printf("Merge CRC file path  %s \n", _bFileStr1[u4InstID][3]);
		#ifdef SEMI_RING_FIFO
	    _tFBufFileInfo[u4InstID].u4BSLenOffset = 0;
	    #endif
		#if VDEC_DRAM_BUSY_TEST
	    i4VDEC_HAL_Dram_Busy_Off(u4InstID);
	    fgOpenFile(u4InstID, (char*)_bFileStr1[u4InstID][3],(char*)"r+b", &_tFBufFileInfo[u4InstID]);
	    vDrmaBusySet(u4InstID);
	    #else
	    fgOpenFile(u4InstID, (char*)_bFileStr1[u4InstID][3],(char*)"r+b", &_tFBufFileInfo[u4InstID]);
	    #endif
		#endif
		
	}
    
	  

    vVDec_FlushDCacheRange((UINT32) _pucVFifo[u4InstID],V_FIFO_SZ);
    //HalFlushInvalidateDCache();
    //_tBufPVAddrInfo[u4InstID].u4PhyVififoSa = BSP_dma_map_single((UINT32) _pucVFifo[u4InstID], V_FIFO_SZ, TO_DEVICE); 
    
     // main decoding loop
     UTIL_Printf("vMpvPlay start\n");
    vMpvPlay(u4InstID,0);
    
    _u4LaeDecCnt[u4InstID] = 0;
    _fgFirstInit = TRUE;
    _u4FileScanned++;
    _u4Main10Mode = 0;
    _u4Bitstream_Num[u4InstID]++;
    vVDec_InvDCacheRange((UINT32) _pucVFifo[u4InstID],V_FIFO_SZ);
    
     #ifndef VDEC_VIDEOCODEC_RM	
      //BSP_dma_unmap_single(_tBufPVAddrInfo[u4InstID].u4PhyVififoSa, V_FIFO_SZ, TO_DEVICE);  	 
     #endif
     
     //vVDecFreeWorkBuffer(u4InstID);
  }

  vVDecDeInit(u4InstID);
  
  vVDecFreeWorkBuffer(u4InstID);

  #ifdef PCFILE_WRITE  
  if(_tInFileInfo[u4InstID].pFile)
  {
    fclose(_tInFileInfo[u4InstID].pFile);
  }
  #endif
  vVerifyVDecIsrStop(u4InstID);
  vMemoryFree(u4InstID);

#ifdef SATA_HDD_READ_SUPPORT
   #ifdef  SATA_HDD_FS_SUPPORT
      if (_tFileListInfo[u4InstID].i4FileId != 0xFFFFFFFF)
      {
         fgHDDFsCloseFile(u4InstID);
         _tFileListInfo[u4InstID].i4FileId = 0xFFFFFFFF;
      }
      //FS mount
    fgHDDFsUnMount(0);
   #endif
#endif

}


void vSecBSVerifyThread(void *param_array)
{
#ifdef BARREL2_THREAD_SUPPORT
  void** ppv_param = (void**)param_array ;
  UINT32 u4InstID =  (UINT32) ppv_param[0];
  UINT32 u4SecBSSpec =  (UINT32) ppv_param[1];

  if(u4SecBSSpec == 0)
  {
    VDEC_INFO_WMV_BS_INIT_PRM_T rWmvBSInitPrm;
  
    _pucSecVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_nc_mem(1024*1024,1024);
    x_memset(_pucSecVFifo[u4InstID], 0x73, 1024*1024);
    rWmvBSInitPrm.u4VFifoSa = (UINT32)_pucSecVFifo[u4InstID];
    rWmvBSInitPrm.u4VFifoEa = (UINT32)_pucSecVFifo[u4InstID] + 1024*1024;
    rWmvBSInitPrm.u4ReadPointer= (UINT32)_pucSecVFifo[u4InstID];
    rWmvBSInitPrm.u4WritePointer= 0xFFFFFFFF;//(UINT32)_pucSecVFifo[u4InstID] + 1024*1024;
    if (_i4CodecVersion[u4InstID] == VC1)
    {
        i4VDEC_HAL_WMV_InitBarrelShifter(1 ,u4InstID, &rWmvBSInitPrm, TRUE);
    }
    else
    {
        i4VDEC_HAL_WMV_InitBarrelShifter(1 ,u4InstID, &rWmvBSInitPrm, FALSE);
    }
    while(1)
    {
      u4VDEC_HAL_WMV_ShiftGetBitStream(1, u4InstID, random(32));
      u4VDEC_HAL_WMV_ShiftGetBitStream(1, u4InstID, random(32));
      u4VDEC_HAL_WMV_ShiftGetBitStream(1, u4InstID, random(32));
      u4VDEC_HAL_WMV_ShiftGetBitStream(1, u4InstID, random(32));
      x_thread_delay(1);
    }
  }
  else if(u4SecBSSpec == 1)
  {
    VDEC_INFO_H264_BS_INIT_PRM_T rH264BSInitPrm;

    if(!_fgSemaCreated[u4InstID])
    {
      VERIFY (x_sema_create(&_ahVDecEndSema[u4InstID], X_SEMA_TYPE_BINARY, X_SEMA_STATE_UNLOCK) == OSR_OK);
      _fgSemaCreated[u4InstID] = TRUE;
    }
    _pucSecVFifo[u4InstID] = (UCHAR *)x_alloc_aligned_nc_mem(1024*1024,1024);
    x_memset(_pucSecVFifo[u4InstID], 0x73, 1024*1024);
    rH264BSInitPrm.u4VFifoSa = (UINT32)_pucSecVFifo[u4InstID];
    rH264BSInitPrm.u4VFifoEa = (UINT32)_pucSecVFifo[u4InstID] + 1024*1024;
    rH264BSInitPrm.u4VLDRdPtr = (UINT32)_pucSecVFifo[u4InstID];
    rH264BSInitPrm.u4VLDWrPtr = 0xFFFFFFFF;//(UINT32)_pucSecVFifo[u4InstID] + 1024*1024;
    i4VDEC_HAL_H264_InitBarrelShifter(1, u4InstID, &rH264BSInitPrm);
    while(1)
    {
      VERIFY (x_sema_lock(_ahVDecEndSema[u4InstID], X_SEMA_OPTION_WAIT) == OSR_OK);
      u4VDEC_HAL_H264_ShiftGetBitStream(1, u4InstID, random(32));
      u4VDEC_HAL_H264_ShiftGetBitStream(1, u4InstID, random(32));
      u4VDEC_HAL_H264_ShiftGetBitStream(1, u4InstID, random(32));
      u4VDEC_HAL_H264_ShiftGetBitStream(1, u4InstID, random(32));
      VERIFY (x_sema_unlock(_ahVDecEndSema[u4InstID]) == OSR_OK);
      x_thread_delay(1);
    }
  }
#endif
}

void vVDecClockSelect(void)
{
   i4VDEC_HAL_Common_Init(0);
}

void vDrmaBusySet (UINT32  u4InstID)
{
    i4VDEC_HAL_Dram_Busy(0, PHYSICAL( (UINT32)(_pucDramBusy[u4InstID]) ), 0x10000);
}

void vDrmaBusyOff (UINT32  u4InstID)
{
    i4VDEC_HAL_Dram_Busy_Off(0, PHYSICAL( (UINT32)(_pucDramBusy[u4InstID]) ), 0x10000);
}

void vVDecTouch(UINT32  u4InstID,UCHAR ucVldID)
{
    //UINT32 u4InstID;
    UTIL_Printf("%s @ %d u4InstId %d rg7018 0x%x\n",__FUNCTION__,__LINE__,u4InstID,*(volatile UINT32 *)(0xF0000000 + 0x7018));
    _u4K2KMode[u4InstID] = 0;
    _u4CodecVer[u4InstID] = VDEC_H265;
    _u4StartCompPicNum[u4InstID] = 0;
    _u4EndCompPicNum[u4InstID] = 0xffffffff;
    _u4DumpRegPicNum[u4InstID] = 0xffffffff;
    _u4DualCoreEnable[u4InstID] = 0;
    _u4CRCMode = 1;
    _tInFileInfo[u4InstID].u4FileLength = 0x1000000;
    //allocate memory part
    vMemoryAllocate(u4InstID);
    if (!fgVDecAllocWorkBuffer(u4InstID))
    {
        UTIL_Printf("[VDEC]Memory alloc failed\n");
    }
    //memcpy array to VFIFO
    memset(_pucVFifo[u4InstID],0,V_FIFO_SZ);
    memcpy(_pucVFifo[u4InstID],u4TempSrc,10240);

    //Decode flow
    //vVDec_FlushDCacheRange((UINT32) _pucVFifo[u4InstID],V_FIFO_SZ);
    //HalFlushInvalidateDCache();
    // main decoding loop
    vMpvPlay(u4InstID,ucVldID);
}

void vVdecUDTVInit(UINT32 u4InstID)
{
    CLI_Parser("sif.swi 0 1");
    x_thread_delay(10);
    CLI_Parser("sif.swxw 0 0x100 0xc0 1 0xff 1 0x0");
    x_thread_delay(10);
    CLI_Parser("sif.swxw 0 0x100 0xc0 1 0x61 1 0x4");
    x_thread_delay(10);
    CLI_Parser("sif.swxw 0 0x100 0xc0 1 0x68 1 0x0");
    x_thread_delay(10);
    CLI_Parser("sif.swxw 0 0x100 0xc0 1 0x6a 1 0x0");
    x_thread_delay(10);
    CLI_Parser("sif.swxw 0 0x100 0xc0 1 0x0 1 0x6e");
    x_thread_delay(10);
    CLI_Parser("sif.swxw 0 0x100 0xc0 1 0x1b 1 0xc");
    x_thread_delay(10);
    CLI_Parser("sif.swxw 0 0x100 0xc0 1 0x8 1 0x44");
    x_thread_delay(10);
    CLI_Parser("sif.swxw 0 0x100 0xc0 1 0x11 1 0x55");
    x_thread_delay(10);
    CLI_Parser("sif.swxw 0 0x100 0xc0 1 0x1a 1 0x68");
    x_thread_delay(10);

    CLI_Parser("udvt.i");
    x_thread_delay(100);
    
    UTIL_Printf("UDVT init done\n");
}


