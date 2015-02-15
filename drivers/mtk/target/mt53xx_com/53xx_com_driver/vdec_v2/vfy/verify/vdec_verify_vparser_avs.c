#include "vdec_verify_mpv_prov.h"
#include "vdec_hal_if_common.h"
#include "vdec_hal_if_avs.h"
#include "vdec_verify_keydef.h"
#include "vdec_verify_vparser_avs.h"
#include "vdec_info_avs.h"
#include "vdec_drv_avs_info.h"
#include "vdec_verify_file_common.h"
#include "vdec_verify_filesetting.h"


#if 0
#define CHKPARAM(u4Param, u4HBound)\
    if(u4Param > u4HBound)\
    {\
        UTIL_Printf( "Hdr err\n");\
        VDEC_ASSERT(0);\
        return;\
    }

#define CHKPARAM_R(i4Param, i4LBound, i4HBound)\
    if((i4Param < i4LBound) || (i4Param > i4HBound))\
    {\
        UTIL_Printf("Hdr err\n");\
        VDEC_ASSERT(0);\
        return;\
    }
#endif


extern int rand(void);

INT32 i4VDECVerAvsSeqHdr(UINT32 u4BsId, UINT32 u4InstID, VDEC_INFO_AVS_SEQ_HDR_T *prVDecAvsSeqHdr);
INT32 i4VDECVerAvsPicHdr(UINT32 u4BsId, UINT32 u4InstID, VDEC_INFO_AVS_SEQ_HDR_T *prVDecAvsSeqHdr, VDEC_INFO_AVS_PIC_HDR_T *prVDecAvsPicHdr);


#if VDEC_PRINT_FRAME_CHECK_SUM
UINT32 u4DecDebugChecksum(UINT32 u4FifoSa, UINT32 u4FifoEa, UINT32 u4AuStart, UINT32 u4AuEnd)
{
    UINT32 u4CheckSum = 0;

    if(u4AuStart <= u4AuEnd)
    {
        do
        {
            u4CheckSum += *((UCHAR*)u4AuStart);
        }while(u4AuStart++ < u4AuEnd);
    }
    else
    {
        do
        {
            u4CheckSum += *((UCHAR*)u4AuStart);
        }while(u4AuStart++ < u4FifoEa);
        do
        {
            u4CheckSum += *((UCHAR*)u4FifoSa);
        }while(u4FifoSa++ < u4AuEnd);
    }

    return u4CheckSum;
}
#endif


void _VDEC_ClrAvsFBufInfo(VDEC_INFO_AVS_DEC_PRM_T *prVDecAvsDecPrm, UINT32 u4FBufIdx, BOOL fgInit)
{
    VDEC_INFO_AVS_FBUF_INFO_T* prFBufInfo;
        
    prFBufInfo = &prVDecAvsDecPrm->arFBufInfo[u4FBufIdx];        
    prFBufInfo->fgNonExisting = FALSE;    
    prFBufInfo->fgIsBufRef = FALSE;
    prFBufInfo->fgIsErr = FALSE;
    prFBufInfo->fgIsNotDisp = FALSE;    
    prFBufInfo->u1FBufStatus = NO_PIC;
    prFBufInfo->ucFbId = VDEC_FB_ID_UNKNOWN;    
    prFBufInfo->u4FrameNum = (UINT32)0xffffffff;
    prFBufInfo->i4FrameNumWrap = (INT32)0xefffffff;
}

void vVerInitAVS(UINT32 u4InstID)
{
    VDEC_INFO_AVS_VFIFO_PRM_T     rAvsVDecInitPrm;
    VDEC_INFO_AVS_BS_INIT_PRM_T  rAvsBSInitPrm;
    VDEC_INFO_AVS_SEQ_HDR_T *prVDecAvsSeqHdr = &_rVDecAvsSeqHdr[u4InstID];
    VDEC_INFO_AVS_PIC_HDR_T *prVDecAvsPicHdr = &_rVDecAvsPicHdr[u4InstID];
    VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = &(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm);
    UINT32 i;
    
    _u4TotalDecFrms[u4InstID] = 0;
  #ifdef BARREL2_THREAD_SUPPORT
    VERIFY (x_sema_lock(_ahVDecEndSema[u4InstID], X_SEMA_OPTION_WAIT) == OSR_OK);
  #endif

    _u4AvsPicCnt[u4InstID] = 0;
    _u4AvsTriggerCnt[u4InstID] =0;
    _u4AvsResetHWCnt[u4InstID] = 1;
    fgAVSCRCLoaded[u4InstID] = FALSE;
#if DEBUG_DUMP_REG_ENABLE
    UTIL_Printf("//Start to ResetHW, Frame/Field [0]\n");
#endif
    rAvsVDecInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rAvsVDecInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    i4VDEC_HAL_AVS_InitVDecHW(u4InstID, &rAvsVDecInitPrm);
    
    rAvsBSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rAvsBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rAvsBSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID];
    rAvsBSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  #ifndef  RING_VFIFO_SUPPORT
    rAvsBSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
  #else
//    rAvsBSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ*(0.5 + 0.5 *(_u4LoadBitstreamCnt[u4InstID]%2)));
    rAvsBSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
  #endif
    i4VDEC_HAL_AVS_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rAvsBSInitPrm);
  #ifdef BARREL2_THREAD_SUPPORT
    VERIFY (x_sema_unlock(_ahVDecEndSema[u4InstID]) == OSR_OK);
  #endif

  _tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm.fgEnPP = TRUE;
   _rVDecAvsPicHdr[u4InstID].fgSecField = FALSE;
   x_memset((void*)prVDecAvsSeqHdr, 0, sizeof(VDEC_INFO_AVS_SEQ_HDR_T));
   x_memset((void*)prVDecAvsPicHdr, 0, sizeof(VDEC_INFO_AVS_PIC_HDR_T));   
   
   for (i = 0; i < AVS_MAX_FRM_BUFNUM; i++)
   {
        _VDEC_ClrAvsFBufInfo(prVDecAvsDecPrm, i, TRUE);
   }

//For UFO Mode
#if AVS_UFO_MODE_SUPPORT
    prVDecAvsDecPrm->fgIsUFOMode = TRUE;
#else
    prVDecAvsDecPrm->fgIsUFOMode = FALSE;
#endif
#if AVS_SUPPORT_YC_SEPERATE
    prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4YAddr = (UINT32)_pucPic3Y[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4CAddr = (UINT32)_pucPic3C[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr = (UINT32)_pucPic2Y[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddr = (UINT32)_pucPic2C[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4YAddr = (UINT32)_pucPic1Y[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4CAddr = (UINT32)_pucPic1C[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4YAddr = (UINT32)_pucPic0Y[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4CAddr = (UINT32)_pucPic0C[u4InstID];
#else
    prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4YAddr = (UINT32)_pucPic3Y[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX].u4CAddrOffset = ((UINT32)_pucPic3C[u4InstID] - (UINT32)_pucPic3Y[u4InstID]);//PIC_Y_SZ;//((UINT32)_pucPic3C[u4InstID] - (UINT32)_pucPic3Y[u4InstID]);
    prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr = (UINT32)_pucPic2Y[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddrOffset = ((UINT32)_pucPic2C[u4InstID] - (UINT32)_pucPic2Y[u4InstID]);//PIC_Y_SZ;//((UINT32)_pucPic2C[u4InstID] - (UINT32)_pucPic2Y[u4InstID]);
    prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4YAddr = (UINT32)_pucPic1Y[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4CAddrOffset = ((UINT32)_pucPic1C[u4InstID] - (UINT32)_pucPic1Y[u4InstID]);//PIC_Y_SZ;//((UINT32)_pucPic1C[u4InstID] - (UINT32)_pucPic1Y[u4InstID]);
    prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4YAddr = (UINT32)_pucPic0Y[u4InstID];
    prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4CAddrOffset = ((UINT32)_pucPic0C[u4InstID] - (UINT32)_pucPic0Y[u4InstID]);//PIC_Y_SZ;//((UINT32)_pucPic0C[u4InstID] - (UINT32)_pucPic0Y[u4InstID]);
#endif
}

extern void vVDecWriteAVSVLD(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val);
extern UINT32 u4VDecReadAVSVLD(UINT32 u4VDecID, UINT32 u4Addr);
BOOL fgAVSNextStartCode(UINT32 u4InstID)
{
#if 0 //SW method
    UINT32 u4Retry = 0;
    UINT32 u4NextStart;
    
    //vVDEC_HAL_AVS_AlignRdPtr(_u4BSID[u4InstID], u4InstID, (UINT32)_pucVFifo[u4InstID], BYTE_ALIGN);
    u4NextStart = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
   
    //check until start code 0x000001XX
    while((u4NextStart>>8)!= 1)
    {
        u4NextStart = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 8);
		u4Retry++;  // Qing Li fix here for some special bitstream

        if(++u4Retry > MAX_RETRY_COUNT1)
        {
            UTIL_Printf("//cannot find StartCode\n");
            return FALSE;
        }
    }

    return(TRUE);
#else //HW method
    UINT32 u4RetryNum = 0;
    UINT32 u4Temp = 1;
    UINT32 u4Ret = TRUE;

    vVDecWriteAVSVLD(u4InstID, RW_AVS_FIND_STARTCODE, 1);
#if DEBUG_DUMP_REG_ENABLE
    UTIL_Printf("wait(`AVS_FW_SEARCH_START_CODE == 0);\n");
#endif

    while (1)
    {
        while(u4VDecReadAVSVLD(u4InstID, RW_AVS_FIND_STARTCODE))
        {
           if (u4RetryNum == 0x100000)
           {
              UTIL_Printf("//[AVS%d] u4VDEC_AVS_Search_SliceSC, timeout\n", u4InstID);
              u4Ret = FALSE;
              break;
           }
           u4RetryNum++;
        }
        // Shift 0x000001 for read pointer
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
        if ((u4Temp >> 8) == 0x000001)
        {
            break;
        }

        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 8);
        //Search Next Start Code.
        vVDecWriteAVSVLD(u4InstID, RW_AVS_FIND_STARTCODE, 1);
#if DEBUG_DUMP_REG_ENABLE
        UTIL_Printf("wait(`AVS_FW_SEARCH_START_CODE == 0);\n");
#endif
    }

    return(u4Ret);
#endif
}



// *********************************************************************
// Function : BOOL u4VerVParserVP6(UINT32 u4InstID)
// Description :
// Parameter :
// Return    :
// *********************************************************************
UINT32 u4VerVParserAVS(UINT32 u4InstID, BOOL fgInquiry)
{
    INT32  i4RetVal = 0;
    UINT32 u4BsId = 0;
    VDEC_INFO_AVS_SEQ_HDR_T *prVDecAvsSeqHdr = &_rVDecAvsSeqHdr[u4InstID];
    VDEC_INFO_AVS_PIC_HDR_T *prVDecAvsPicHdr = &_rVDecAvsPicHdr[u4InstID];

    #if 0//DTV_COMMON_CONFIG

    UINT32 u4VldByte = 0;    
    UINT32 u4VldBit = 0;

    LOG(0, "[AVS%d] u4VerVParserAVS, 0xF4=0x%x, Rptr 0xB0=0x%x, Wptr 0x110=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x, 0\n", 
                u4InstID, u4VDecReadVLD(u4InstID, RO_VLD_SRAMCTRL), u4VDecReadVLD(u4InstID, RW_VLD_RPTR), u4VDecReadVLD(u4InstID, WO_VLD_WPTR), u4VDecReadVLD(u4InstID, RO_VLD_VRPTR),
                u4VDecReadVLD(u4InstID, RO_VLD_VBAR), u4VDecReadVLD(u4InstID, RO_VLD_FETCHOK)); 
    
    u4VldByte = u4VDEC_HAL_AVS_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
    UTIL_Printf("[AVS%d] u4VerVParserAVS, u4VldByte:0x%x, 0\n", u4InstID, u4VldByte);
    #endif

   do 
   {
       do
       {
           if (fgAVSNextStartCode(u4InstID)==FALSE)
           {
              #if VDEC_TEST_LOG
              UTIL_Printf("//[AVS%d] u4VerVParserAVS, no next start code\n", u4InstID);
              #endif
              
              return(NO_START_C_ERR1);
           }
           _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
           if((_u4Datain[u4InstID] != AVS_SEQ_HDR_SC) && 
               (_u4Datain[u4InstID] !=AVS_I_PICTURE_SC) && (_u4Datain[u4InstID] != AVS_PB_PICTURE_SC))
           {
               //Keep search next start code
               if (prVDecAvsPicHdr->fgSecField && _u4Datain[u4InstID] >= AVS_SLICE_SC_MIN && _u4Datain[u4InstID] <= AVS_SLICE_SC_MAX)
               {
                  #if VDEC_TEST_LOG
                  UTIL_Printf("//AVS_2nd Field Find Slice Start Code\n");
                  #endif
                  return (i4RetVal);
               }
               else
               {
                 _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 8);
               }
           }
           else
           {
               if (  prVDecAvsPicHdr->fgSecField 
               	&& (_u4Datain[u4InstID] ==AVS_I_PICTURE_SC || _u4Datain[u4InstID] ==AVS_PB_PICTURE_SC) )
               {
                  UTIL_Printf("//AVS: Field Picture Not Paired!!!!!\n");
#if VDEC_AVS_ERR_TEST //Only drop when testing AVS+ ErrorConcealment
                  UTIL_Printf("//No bottom field ,Drop top_field\n");
                  prVDecAvsPicHdr->fgSecField = FALSE;
                  _u4AvsTriggerCnt[u4InstID] ++;
                  _u4AvsResetHWCnt[u4InstID] ++;
#else
                  VDEC_ASSERT(0);
#endif
               }
               break;
           }
       }while (1);

       #if 0//DTV_COMMON_CONFIG // only for test
       u4VldByte = u4VDEC_HAL_AVS_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
       UTIL_Printf("[AVS%d] u4VerVParserAVS, u4VldByte:0x%x, 1\n", u4InstID, u4VldByte);
       vVDEC_HAL_AVS_BitStuff_Check(u4InstID, 0);
       #else
       vVDEC_HAL_AVS_BitStuff_Check(u4InstID, 0);
       #endif
       
       if (AVS_SEQ_HDR_SC == _u4Datain[u4InstID])
       {
         #if VDEC_TEST_LOG
         UTIL_Printf("//[AVS%d] u4VerVParserAVS, AVS_SEQ_HDR_SC, parse sequence header\n", u4InstID);
         #endif
         
          //SEQ_HEADER
         i4VDECVerAvsSeqHdr(u4BsId, u4InstID, prVDecAvsSeqHdr);      
       }
       
        #if 0//DTV_COMMON_CONFIG // test only
        #include "vdec_hw_avs.h"
        
        UTIL_Printf("[AVS%d] u4VerVParserAVS, SYS_CKCFG value: %d\n", u4InstID, u4ReadReg(CKGEN_VDEC_SYS_CFG));      
        //vWriteReg(CKGEN_VDEC_SYS_CFG,VDEC_SYS_CK_SEL_SLOW);
        //UTIL_Printf("[AVS%d] u4VerVParserAVS, SYS_CKCFG value: %d\n", u4InstID, u4ReadReg(CKGEN_VDEC_SYS_CFG));  

        UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_AVS_VLD_BITSTUFFING_SWITCH value: %d, 1\n", u4InstID, u4VDecReadAVSVLD(u4InstID, RW_AVS_VLD_BITSTUFFING_SWITCH)); 

        UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_VDEC_MISC_PDN_CON_SPEC = 0x%x\n", u4InstID, u4VDecReadMISC(u4InstID, RW_VDEC_MISC_PDN_CON_SPEC));
        UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_VDEC_MISC_PDN_CON_MODULE = 0x%x\n", u4InstID, u4VDecReadMISC(u4InstID, RW_VDEC_MISC_PDN_CON_MODULE));
        vVDecWriteMISC(u4InstID,RW_VDEC_MISC_PDN_CON_SPEC, PDN_SPEC_AVS);
        vVDecWriteMISC(u4InstID,RW_VDEC_MISC_PDN_CON_MODULE, PDN_MODULE_AVS);
        UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_VDEC_MISC_PDN_CON_SPEC = 0x%x\n", u4InstID, u4VDecReadMISC(u4InstID, RW_VDEC_MISC_PDN_CON_SPEC));
        UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_VDEC_MISC_PDN_CON_MODULE = 0x%x\n", u4InstID, u4VDecReadMISC(u4InstID, RW_VDEC_MISC_PDN_CON_MODULE));

        UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_AVS_VLD_BITSTUFFING_SWITCH value: %d, 2\n", u4InstID, u4VDecReadAVSVLD(u4InstID, RW_AVS_VLD_BITSTUFFING_SWITCH)); 

        _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
        _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
        _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
        
        UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_AVS_VLD_BITSTUFFING_SWITCH value: %d, 3\n", u4InstID, u4VDecReadAVSVLD(u4InstID, RW_AVS_VLD_BITSTUFFING_SWITCH));

        _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);

        UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_AVS_VLD_BITSTUFFING_SWITCH value: %d, 4\n", u4InstID, u4VDecReadAVSVLD(u4InstID, RW_AVS_VLD_BITSTUFFING_SWITCH));

        u4VldByte = u4VDEC_HAL_AVS_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
        UTIL_Printf("[AVS%d] u4VerVParserAVS, u4VldByte:0x%x, 2\n", u4InstID, u4VldByte);
        
        LOG(0, "[AVS%d] u4VerVParserAVS, 0xF4=0x%x, Rptr 0xB0=0x%x, Wptr 0x110=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x\n", 
                u4InstID, u4VDecReadVLD(u4InstID, RO_VLD_SRAMCTRL), u4VDecReadVLD(u4InstID, RW_VLD_RPTR), u4VDecReadVLD(u4InstID, WO_VLD_WPTR), u4VDecReadVLD(u4InstID, RO_VLD_VRPTR),
                u4VDecReadVLD(u4InstID, RO_VLD_VBAR), u4VDecReadVLD(u4InstID, RO_VLD_FETCHOK));        
        #endif

       _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);

       //UTIL_Printf("[AVS%d] u4VerVParserAVS, u4Datain:0x%x\n", u4InstID, _u4Datain[u4InstID]);

       #if 0//VDEC_AVS_ERR_TEST
       if(prVDecAvsSeqHdr->u4IsValid)
       #endif
       {
           if (AVS_I_PICTURE_SC == _u4Datain[u4InstID])
           {
             //Parse_I_PIC                  
             prVDecAvsPicHdr->fgIsIPic = TRUE;
             i4VDECVerAvsPicHdr(u4BsId, u4InstID, prVDecAvsSeqHdr, prVDecAvsPicHdr);
             break;
       }
       else
       if (AVS_PB_PICTURE_SC == _u4Datain[u4InstID])
       {
         //Parse_PB_PIC
         prVDecAvsPicHdr->fgIsIPic = FALSE;
             i4VDECVerAvsPicHdr(u4BsId, u4InstID, prVDecAvsSeqHdr, prVDecAvsPicHdr);
             break;
           }
           else
           {
             //Do Nothing.
           }
       }
    }while(1);
    
  return(i4RetVal);

}

INT32 i4VDECVerAvsSeqHdr(UINT32 u4BsId, UINT32 u4InstID, VDEC_INFO_AVS_SEQ_HDR_T *prVDecAvsSeqHdr)
{
    INT32  i4RetVal = VDEC_HDR_SUCCESS;
    UINT32 u4Temp;
    
    x_memset((void*)prVDecAvsSeqHdr, 0, sizeof(VDEC_INFO_AVS_SEQ_HDR_T));
    
    // Shift 0x000001 for read pointer
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 32);
    prVDecAvsSeqHdr->u4ProfileID = u4Temp >> 24;
    prVDecAvsSeqHdr->u4LevelID = (u4Temp >> 16) & 0xFF;
    
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 16);    

    if(prVDecAvsSeqHdr->u4ProfileID == GUANGBO)
    {
        UTIL_Printf("//This is AVS Plus Bitstream\n");
    }

#if 0
    if((!fgChkProfile((UINT8)prVDecAvsSeqHdr->u4ProfileID)) || (!fgChkLevel((UINT8)prVDecAvsSeqHdr->u4LevelID)))
    {
        i4RetVal = (INT32)E_VDEC_FAIL;
        UTIL_Printf(2, "err at SeqHdr Profile/Level\n");
        return;
    }  
#endif    
    prVDecAvsSeqHdr->u4IsProgSeq = (u4Temp >> 31);
    prVDecAvsSeqHdr->u4HSize = ((u4Temp >> 17) & 0x3FFF);
    prVDecAvsSeqHdr->u4VSize = ((u4Temp >> 3) & 0x3FFF);
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 29);

    #if AVS_TEST_HD
    prVDecAvsSeqHdr->u4LastHSize = (((prVDecAvsSeqHdr->u4HSize + 63) >> 6) << 6);
    prVDecAvsSeqHdr->u4LastVSize = (((prVDecAvsSeqHdr->u4VSize + 31) >> 5) << 5); 
    #else
    prVDecAvsSeqHdr->u4LastHSize = (((prVDecAvsSeqHdr->u4HSize + 15) >> 4) << 4);
    prVDecAvsSeqHdr->u4LastVSize = (((prVDecAvsSeqHdr->u4VSize + 15) >> 4) << 4);  
    #endif

    #if 0//!AVS_TEST_HD
    if(((prVDecAvsSeqHdr->u4HSize == 0) || (prVDecAvsSeqHdr->u4VSize == 0)) ||
        ((prVDecAvsSeqHdr->u4HSize > 1920) || (prVDecAvsSeqHdr->u4VSize > 1088)))
    {
        
        i4RetVal = (INT32)VDEC_HDR_ERR;
        UTIL_Printf("AVS: err at SeqHdr HSize/VSize\n");
        return i4RetVal;
    }
    else
    #endif
    {
        #if (DTV_COMMON_CONFIG) && (AVS_TEST_HD)
        prVDecAvsSeqHdr->u2WidthDec = (UINT16) (64 * ((prVDecAvsSeqHdr->u4HSize+63) / 64));  
        prVDecAvsSeqHdr->u2HeightDec = (UINT16) (32 * ((prVDecAvsSeqHdr->u4VSize+31) / 32));
        #else
        prVDecAvsSeqHdr->u2WidthDec = (UINT16) (16 * ((prVDecAvsSeqHdr->u4HSize+15) / 16));
        
        if (prVDecAvsSeqHdr->u4IsProgSeq == 1)
        {
           prVDecAvsSeqHdr->u2HeightDec = (UINT16) (16 * ((prVDecAvsSeqHdr->u4VSize+15) / 16));
        }
        else
        {
           prVDecAvsSeqHdr->u2HeightDec = (UINT16) (32 * ((prVDecAvsSeqHdr->u4VSize+31) / 32));
        }
        #endif
    }
    
    #if DEBUG_PARSE_LOG_ENABLE
    UTIL_Printf("//AVS: i4VDECVerAvsSeqHdr, H:%d, V:%d, LastH:%d, LastV:%d, Dec H/W:[%d, %d]\n", 
      prVDecAvsSeqHdr->u4HSize,
      prVDecAvsSeqHdr->u4VSize,
      prVDecAvsSeqHdr->u4LastHSize,
      prVDecAvsSeqHdr->u4LastVSize,
      prVDecAvsSeqHdr->u2HeightDec,
      prVDecAvsSeqHdr->u2WidthDec);
    #endif

    prVDecAvsSeqHdr->u4ChromaFmt = (u4Temp >> 30);
    prVDecAvsSeqHdr->u4SamplePrec = (u4Temp >> 27) & 0x7;
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 5);  
    
    if ( prVDecAvsSeqHdr->u4SamplePrec != 0x1)
    {
        i4RetVal = (INT32)VDEC_HDR_ERR;
        UTIL_Printf("//AVS: err at SamplePrecision = %d\n", prVDecAvsSeqHdr->u4SamplePrec);
        return i4RetVal;
    }

    prVDecAvsSeqHdr->u4AspRatio = (u4Temp  >> 28);
    prVDecAvsSeqHdr->u4FrmRate = (u4Temp >> 24) & 0xF;
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 8);
    if ( prVDecAvsSeqHdr->u4AspRatio == 0x0 || prVDecAvsSeqHdr->u4AspRatio > 0x4)
    {
        i4RetVal = (INT32)VDEC_HDR_ERR;
        UTIL_Printf("//AVS: err at u4AspRatio = %d\n", prVDecAvsSeqHdr->u4AspRatio);
        return i4RetVal;
    }  
    
    if ( prVDecAvsSeqHdr->u4FrmRate == 0x0 || prVDecAvsSeqHdr->u4FrmRate > 0x8)
    {
        i4RetVal = (INT32)VDEC_HDR_ERR;
        UTIL_Printf("//AVS: err at u4FrmRate = %d\n", prVDecAvsSeqHdr->u4FrmRate);
        return i4RetVal;
    }  

    #if DEBUG_PARSE_LOG_ENABLE
    UTIL_Printf( "//prVDecAvsSeqHdr->u4AspRatio = %d, prVDecAvsSeqHdr->u4FrmRate = %d\n", prVDecAvsSeqHdr->u4AspRatio, prVDecAvsSeqHdr->u4FrmRate);
    #endif

    prVDecAvsSeqHdr->u4BitRateL = (u4Temp >> 14);
    prVDecAvsSeqHdr->u4MarketBitBR = (u4Temp >> 13) & 0x1;
    prVDecAvsSeqHdr->u4BitRateU = (u4Temp & 0xFFF);
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 31);
    prVDecAvsSeqHdr->u4LowDelay  = (u4Temp >> 31);
    prVDecAvsSeqHdr->u4MarkerBit = (u4Temp >> 30) & 0x1;
    prVDecAvsSeqHdr->u4BBVSize = (u4Temp >> 12) & 0x3FFFF;
    prVDecAvsSeqHdr->u4RsvBits = (u4Temp >> 9) & 0x7;
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 23);
    prVDecAvsSeqHdr->u4IsValid = 1;
    
    return i4RetVal;
}

#if DEBUG_PARSE_LOG_ENABLE
extern UINT32 u4VDecReadVLD(UINT32 u4VDecID, UINT32 u4Addr);
extern UINT32 u4VDecAVSReadVLD(UINT32 u4VDecID, UINT32 u4Addr);
#endif
INT32 i4VDECVerAvsPicHdr(UINT32 u4BsId, UINT32 u4InstID, VDEC_INFO_AVS_SEQ_HDR_T *prVDecAvsSeqHdr, VDEC_INFO_AVS_PIC_HDR_T *prVDecAvsPicHdr)
{
    INT32  i4RetVal = VDEC_HDR_SUCCESS;
    UINT32 u4Temp = 0xFFFFFFFF;
    BOOL   fgSliceStartCdoe = TRUE;    

    // Turn On Bit Stuffing Check
    #if 0//DTV_COMMON_CONFIG // temp for try run, if enable, system abort after following get bit  stream code

    #else
    vVDEC_HAL_AVS_BitStuff_Check(u4InstID, 1);
    #endif

    if(!prVDecAvsSeqHdr->u4IsValid)
    {
        i4RetVal = (INT32)VDEC_HDR_ERR;
        
        #if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf( "//SeqHdr is unvalid, no parse PicHdr\n");
        #endif
        
        return i4RetVal;
    } 
    
    #if 0//DTV_COMMON_CONFIG // temp test for try run, if enable, system abort after following get bit  stream code
    UTIL_Printf("[AVS%d] i4VDECVerAvsPicHdr, BS:%d, u4Temp:0x%x, -4\n", u4InstID, _u4BSID[u4InstID], u4Temp);
    
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
    
    UTIL_Printf("[AVS%d] i4VDECVerAvsPicHdr, BS:%d, u4Temp:0x%x, -3\n", u4InstID, _u4BSID[u4InstID], u4Temp);

    {    
    //#include "vdec_hw_avs.h"

    UINT32 u4VldByte = 0;    
    UINT32 u4VldBit = 0;
        
    UTIL_Printf("[AVS%d] u4VerVParserAVS, SYS_CKCFG value: %d\n", u4InstID, u4ReadReg(CKGEN_VDEC_SYS_CFG));      
    //vWriteReg(CKGEN_VDEC_SYS_CFG,VDEC_SYS_CK_SEL_SLOW);
    //UTIL_Printf("[AVS%d] u4VerVParserAVS, SYS_CKCFG value: %d\n", u4InstID, u4ReadReg(CKGEN_VDEC_SYS_CFG));  

    //UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_AVS_VLD_BITSTUFFING_SWITCH value: %d, 1\n", u4InstID, u4VDecReadAVSVLD(u4InstID, RW_AVS_VLD_BITSTUFFING_SWITCH)); 

    UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_VDEC_MISC_PDN_CON_SPEC = 0x%x\n", u4InstID, u4VDecReadMISC(u4InstID, RW_VDEC_MISC_PDN_CON_SPEC));
    UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_VDEC_MISC_PDN_CON_MODULE = 0x%x\n", u4InstID, u4VDecReadMISC(u4InstID, RW_VDEC_MISC_PDN_CON_MODULE));
    vVDecWriteMISC(u4InstID,RW_VDEC_MISC_PDN_CON_SPEC, PDN_SPEC_AVS);
    vVDecWriteMISC(u4InstID,RW_VDEC_MISC_PDN_CON_MODULE, PDN_MODULE_AVS);
    UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_VDEC_MISC_PDN_CON_SPEC = 0x%x\n", u4InstID, u4VDecReadMISC(u4InstID, RW_VDEC_MISC_PDN_CON_SPEC));
    UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_VDEC_MISC_PDN_CON_MODULE = 0x%x\n", u4InstID, u4VDecReadMISC(u4InstID, RW_VDEC_MISC_PDN_CON_MODULE));

    //UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_AVS_VLD_BITSTUFFING_SWITCH value: %d, 2\n", u4InstID, u4VDecReadAVSVLD(u4InstID, RW_AVS_VLD_BITSTUFFING_SWITCH)); 

    _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
    _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
    _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
    
    //UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_AVS_VLD_BITSTUFFING_SWITCH value: %d, 3\n", u4InstID, u4VDecReadAVSVLD(u4InstID, RW_AVS_VLD_BITSTUFFING_SWITCH));

    _u4Datain[u4InstID] = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);

    //UTIL_Printf("[AVS%d] u4VerVParserAVS, RW_AVS_VLD_BITSTUFFING_SWITCH value: %d, 4\n", u4InstID, u4VDecReadAVSVLD(u4InstID, RW_AVS_VLD_BITSTUFFING_SWITCH));

    u4VldByte = u4VDEC_HAL_AVS_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
    UTIL_Printf("[AVS%d] u4VerVParserAVS, u4VldByte:0x%x, 2\n", u4InstID, u4VldByte);    

    LOG(0, "[AVS%d] u4VerVParserAVS, 0xF4=0x%x, Rptr 0xB0=0x%x, Wptr 0x110=0x%x, 0xFC=0x%x, 0xEC=0x%x, 0xE8=0x%x, 0\n", 
                u4InstID, u4VDecReadVLD(u4InstID, RO_VLD_SRAMCTRL), u4VDecReadVLD(u4InstID, RW_VLD_RPTR), u4VDecReadVLD(u4InstID, WO_VLD_WPTR), u4VDecReadVLD(u4InstID, RO_VLD_VRPTR),
                u4VDecReadVLD(u4InstID, RO_VLD_VBAR), u4VDecReadVLD(u4InstID, RO_VLD_FETCHOK)); 
    
    u4VldByte = u4VDEC_HAL_AVS_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);
    UTIL_Printf("[AVS%d] u4VerVParserAVS, u4VldByte:0x%x, 0\n", u4InstID, u4VldByte);
    }
        
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 8);

    UTIL_Printf("[AVS%d] i4VDECVerAvsPicHdr, BS:%d, u4Temp:0x%x, -2\n", u4InstID, _u4BSID[u4InstID], u4Temp);    
        
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 15);

    UTIL_Printf("[AVS%d] i4VDECVerAvsPicHdr, BS:%d, u4Temp:0x%x, -1\n", u4InstID, _u4BSID[u4InstID], u4Temp);
        
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 8);    

    UTIL_Printf("[AVS%d] i4VDECVerAvsPicHdr, BS:%d, u4Temp:0x%x, 0\n", u4InstID, _u4BSID[u4InstID], u4Temp);
    #endif

    // Shift 0x000001 for read pointer
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 32);
    
    //UTIL_Printf("[AVS%d] i4VDECVerAvsPicHdr, u4Temp:0x%x, 1\n", u4InstID, u4Temp);
    
    prVDecAvsPicHdr->u4BBVDelay = (u4Temp >> 16);    
#if DEBUG_PARSE_LOG_ENABLE
    UTIL_Printf("//u4BBVDelay: %d\n", prVDecAvsPicHdr->u4BBVDelay);
#endif

    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 16);
    
    //UTIL_Printf("[AVS%d] i4VDECVerAvsPicHdr, u4Temp:0x%x, 2\n", u4InstID, u4Temp);
    if(prVDecAvsSeqHdr->u4ProfileID == GUANGBO)
    {
        prVDecAvsPicHdr->u4MarkerBit0 = (u4Temp >> 31);
#if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf("//u4MarkerBit0: %d\n", prVDecAvsPicHdr->u4MarkerBit0);
#endif
        prVDecAvsPicHdr->u4BBVDelayExternsion = (u4Temp >> 24) & 0x7F;
#if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf("//u4BBVDelayExternsion: %d\n", prVDecAvsPicHdr->u4BBVDelayExternsion);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 8);
        
        prVDecAvsPicHdr->u4BBVDelay <<= 7;
        prVDecAvsPicHdr->u4BBVDelay |= prVDecAvsPicHdr->u4BBVDelayExternsion;
    }
    
    if(prVDecAvsPicHdr->fgIsIPic)
    {
        prVDecAvsPicHdr->u4TimeCodeFg = (u4Temp >> 31);
#if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf("//u4TimeCodeFg: %d\n", prVDecAvsPicHdr->u4TimeCodeFg);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
        if(prVDecAvsPicHdr->u4TimeCodeFg)
        {
            prVDecAvsPicHdr->u4TimeCode = (u4Temp >> 8);
#if DEBUG_PARSE_LOG_ENABLE
            UTIL_Printf("//u4TimeCode: %d\n", prVDecAvsPicHdr->u4TimeCode);
#endif
            u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 24);
        }
        prVDecAvsPicHdr->u4MarkerBit = (u4Temp >> 31); 
#if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf("//u4MarkerBit: %d\n", prVDecAvsPicHdr->u4MarkerBit);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
        prVDecAvsPicHdr->u4PicCodingType = (UINT32) I_PIC;
#if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf("//u4PicCodingType: %d\n", prVDecAvsPicHdr->u4PicCodingType);
#endif
    }
    else
    {
        prVDecAvsPicHdr->u4PicCodingType = (u4Temp >> 30);
#if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf("//u4PicCodingType: %d\n", prVDecAvsPicHdr->u4PicCodingType);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 2);
        //CHKPARAM_R(prVDecAvsPicHdr->u4PicCodingType, 1, 2);
    }

    //UTIL_Printf("AVS coding type = %d\n", prVDecAvsPicHdr->u4PicCodingType);

    //if (!prDecPrm->fgFirstI)
    //{
        //if (prVDecAvsPicHdr->u4PicCodingType != I_PIC)
        //{
            //UTIL_Printf(3, "Wait for the first I frame\n");
            //i4RetVal = (INT32)E_VDEC_FAIL;
            //return;
        //}
        //else
        //{
            //prDecPrm->fgFirstI = TRUE;
        //}
    //}

    //Check brokenlink B
    //if ((prVDecAvsPicHdr->u4PicCodingType == B_PIC) &&
    //    (prDecPrm->u1LastRefIdx < 1))
    //{
    //    UTIL_Printf(3, "Not enough ref frame buf for B\n");
        //i4RetVal = (INT32)E_VDEC_SKIPFRAME;
    //    i4RetVal = (INT32)E_VDEC_FAIL;
    //    return;
    //}

    prVDecAvsPicHdr->u4PicDistance = u4Temp >> 24;
#if DEBUG_PARSE_LOG_ENABLE
    UTIL_Printf("//u4PicDistance: %d\n", prVDecAvsPicHdr->u4PicDistance);
#endif

    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 8);
    if(prVDecAvsSeqHdr->u4LowDelay)
    {
        //bbv_check_times
        prVDecAvsPicHdr->u4BBVCheckTimes = u4VDEC_AvsUeCodeNum(_u4BSID[u4InstID], u4InstID);
#if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf("//u4BBVCheckTimes: %d\n", prVDecAvsPicHdr->u4BBVCheckTimes);
#endif
    }

    u4Temp =u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
    prVDecAvsPicHdr->u4ProgFrm = (u4Temp >> 31);
#if DEBUG_PARSE_LOG_ENABLE
    UTIL_Printf("//u4ProgFrm: %d\n", prVDecAvsPicHdr->u4ProgFrm);
#endif
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
    if(!prVDecAvsPicHdr->u4ProgFrm)
    {
        prVDecAvsPicHdr->u4PicStruct = u4Temp >> 31;
#if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf("//u4PicStruct: %d\n", prVDecAvsPicHdr->u4PicStruct);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
        if(!prVDecAvsPicHdr->u4PicStruct && !prVDecAvsPicHdr->fgIsIPic)
        {
            prVDecAvsPicHdr->u4AdvPredModeDisable = u4Temp >> 31;
#if DEBUG_PARSE_LOG_ENABLE
            UTIL_Printf("//u4AdvPredModeDisable: %d\n", prVDecAvsPicHdr->u4AdvPredModeDisable);
#endif
            u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
        }
        else
        {
            prVDecAvsPicHdr->u4AdvPredModeDisable = 1;
        }

        if(prVDecAvsPicHdr->u4PicStruct)
        {
            prVDecAvsPicHdr->u4ProgFrm = 1;
            prVDecAvsPicHdr->u4PicStruct = FRM_PIC;
        }
    }
    else
    {
        prVDecAvsPicHdr->u4PicStruct = FRM_PIC;
        prVDecAvsPicHdr->u4AdvPredModeDisable = 1;
    }
    prVDecAvsPicHdr->u4TFT = u4Temp >> 31;
    prVDecAvsPicHdr->u4RFF = (u4Temp >> 30) & 0x1;
    prVDecAvsPicHdr->u4FixedPicQP = (u4Temp >> 29) & 0x1;
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 3);
    prVDecAvsPicHdr->u4PicQP = u4Temp >> 26;
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 6);

#if DEBUG_PARSE_LOG_ENABLE
    UTIL_Printf("//u4TFT: %d\n", prVDecAvsPicHdr->u4TFT);
    UTIL_Printf("//u4RFF: %d\n", prVDecAvsPicHdr->u4RFF);
    UTIL_Printf("//u4FixedPicQP: %d\n", prVDecAvsPicHdr->u4FixedPicQP);
    UTIL_Printf("//u4PicQP: %d\n", prVDecAvsPicHdr->u4PicQP);
#endif

    if(!prVDecAvsPicHdr->fgIsIPic)
    {
        if(!((prVDecAvsPicHdr->u4PicCodingType == B_PIC) && 
            (prVDecAvsPicHdr->u4PicStruct == FRM_PIC) && 
            (prVDecAvsPicHdr->u4AdvPredModeDisable)))
        {
            prVDecAvsPicHdr->u4PicRefFg = (u4Temp >> 31);
#if DEBUG_PARSE_LOG_ENABLE
            UTIL_Printf("//u4PicRefFg: %d\n", prVDecAvsPicHdr->u4PicRefFg);
#endif
            u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
        }
        prVDecAvsPicHdr->u4NoForwardRefFg = (u4Temp >> 31);
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
        prVDecAvsPicHdr->u4PBFieldEnhancedFlag = (u4Temp >> 31);
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
        prVDecAvsPicHdr->u4RsvBits = (u4Temp >> 30);
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 2);
#if DEBUG_PARSE_LOG_ENABLE
        UTIL_Printf("//u4NoForwardRefFg: %d\n", prVDecAvsPicHdr->u4NoForwardRefFg);
	UTIL_Printf("//u4PBFieldEnhancedFlag: %d\n", prVDecAvsPicHdr->u4PBFieldEnhancedFlag);
        UTIL_Printf("//u4RsvBits: %d\n", prVDecAvsPicHdr->u4RsvBits);
#endif
    }

    if((!prVDecAvsPicHdr->u4ProgFrm && !prVDecAvsPicHdr->u4PicStruct) || (!prVDecAvsPicHdr->fgIsIPic))
    {
        prVDecAvsPicHdr->u4SkipModeFg = (u4Temp >> 31);
#if DEBUG_PARSE_LOG_ENABLE
         UTIL_Printf("//u4SkipModeFg: %d\n", prVDecAvsPicHdr->u4SkipModeFg);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
#if DEBUG_PARSE_LOG_ENABLE
        //UTIL_Printf("Input Window: 0x%08x, 0x%08x\n", u4VDecReadVLD(u4InstID, 0xF0), u4VDecAVSReadVLD(u4InstID, 0x00)); 
#endif
    }
    
    if(prVDecAvsPicHdr->fgIsIPic)
    {
        prVDecAvsPicHdr->u4RsvBits = (u4Temp >> 28);
#if DEBUG_PARSE_LOG_ENABLE
         UTIL_Printf("//u4RsvBits: %d\n", prVDecAvsPicHdr->u4RsvBits);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 4);
    }
    
    // loop filter
    prVDecAvsPicHdr->u4LoopFilterDisable = (u4Temp >> 31);
#if DEBUG_PARSE_LOG_ENABLE
     UTIL_Printf("//u4LoopFilterDisable: %d\n", prVDecAvsPicHdr->u4LoopFilterDisable);
#endif
    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
#if DEBUG_PARSE_LOG_ENABLE
    //UTIL_Printf("Input Window: 0x%08x, 0x%08x\n", u4VDecReadVLD(u4InstID, 0xF0), u4VDecAVSReadVLD(u4InstID, 0x00)); 
#endif
    if(!prVDecAvsPicHdr->u4LoopFilterDisable)
    {
        prVDecAvsPicHdr->u4LoopFilterParamFg = (u4Temp >> 31);
#if DEBUG_PARSE_LOG_ENABLE
         UTIL_Printf("//u4LoopFilterParamFg: %d\n", prVDecAvsPicHdr->u4LoopFilterParamFg);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
#if DEBUG_PARSE_LOG_ENABLE
        //UTIL_Printf("Input Window: 0x%08x, 0x%08x\n", u4VDecReadVLD(u4InstID, 0xF0), u4VDecAVSReadVLD(u4InstID, 0x00)); 
#endif
        if(prVDecAvsPicHdr->u4LoopFilterParamFg)
        {
            u4Temp = u4VDEC_AvsSeCodeNum(_u4BSID[u4InstID], u4InstID);
#if VDEC_TEST_LOG
            //UTIL_Printf("Input Window: 0x%08x, 0x%08x\n", u4VDecReadVLD(u4InstID, 0xF0), u4VDecAVSReadVLD(u4InstID, 0x00)); 
#endif
            if (u4Temp & 0x10000)
            {
                prVDecAvsPicHdr->i4AlphaCOffset = (INT32)(u4Temp & 0xFFFF);
                prVDecAvsPicHdr->i4AlphaCOffset = (INT32)((((UINT32)(prVDecAvsPicHdr->i4AlphaCOffset) ^ (0xFFFFFFFF)) + 1) & 0xFFFF) * (-1); 
            }
            else
            {
                prVDecAvsPicHdr->i4AlphaCOffset = (INT32)(u4Temp & 0xFFFF);
            }

            //CHKPARAM_R(prVDecAvsPicHdr->i4AlphaCOffset, -8, 8);
            u4Temp = u4VDEC_AvsSeCodeNum(_u4BSID[u4InstID], u4InstID);
#if VDEC_TEST_LOG
            //UTIL_Printf("Input Window: 0x%08x, 0x%08x\n", u4VDecReadVLD(u4InstID, 0xF0), u4VDecAVSReadVLD(u4InstID, 0x00)); 
#endif
            if (u4Temp & 0x10000)
            {
                prVDecAvsPicHdr->i4BetaOffset = (INT32)(u4Temp & 0xFFFF);
                prVDecAvsPicHdr->i4BetaOffset = (INT32)((((UINT32)(prVDecAvsPicHdr->i4BetaOffset) ^ (0xFFFFFFFF)) + 1) & 0xFFFF) * (-1); 
            }
            else
            {
                prVDecAvsPicHdr->i4BetaOffset = (INT32)(u4Temp & 0xFFFF);
            }
           //CHKPARAM_R(prVDecAvsPicHdr->i4BetaOffset, -8, 8);
#if DEBUG_PARSE_LOG_ENABLE
             UTIL_Printf("//i4AlphaCOffset: %d\n", prVDecAvsPicHdr->i4AlphaCOffset);
             UTIL_Printf("//i4BetaOffset: %d\n", prVDecAvsPicHdr->i4BetaOffset);
#endif
        }
    }

    u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
    if(prVDecAvsSeqHdr->u4ProfileID == GUANGBO)
    {
        UINT32 i;
        prVDecAvsPicHdr->u4WeightingQuantFlag = (u4Temp >> 31);
#if DEBUG_PARSE_LOG_ENABLE
         UTIL_Printf("//u4WeightingQuantFlag: %d\n", prVDecAvsPicHdr->u4WeightingQuantFlag);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
#if VDEC_TEST_LOG
        //UTIL_Printf("Input Window: 0x%08x, 0x%08x\n", u4VDecReadVLD(u4InstID, 0xF0), u4VDecAVSReadVLD(u4InstID, 0x00)); 
#endif

        if(prVDecAvsPicHdr->u4WeightingQuantFlag == 1)
        {
            prVDecAvsPicHdr->u4MbAdaptWeightingQuantDisable = (u4Temp >> 31);
            //u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);
            prVDecAvsPicHdr->u4ChromaQuantParamDisable = (u4Temp >> 30) & 0x1;
#if DEBUG_PARSE_LOG_ENABLE
             UTIL_Printf("//u4MbAdaptWeightingQuantDisable: %d\n", prVDecAvsPicHdr->u4MbAdaptWeightingQuantDisable);
             UTIL_Printf("//u4ChromaQuantParamDisable: %d\n", prVDecAvsPicHdr->u4ChromaQuantParamDisable);
#endif
            u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 2);

            if(prVDecAvsPicHdr->u4ChromaQuantParamDisable == 0)
            {
                prVDecAvsPicHdr->u4ChromaQuantParamDeltaCb = u4VDEC_AvsSeCodeNum(_u4BSID[u4InstID], u4InstID);
                prVDecAvsPicHdr->u4ChromaQuantParamDeltaCr = u4VDEC_AvsSeCodeNum(_u4BSID[u4InstID], u4InstID);
#if DEBUG_PARSE_LOG_ENABLE
                 UTIL_Printf("//u4ChromaQuantParamDeltaCb: %d\n", prVDecAvsPicHdr->u4ChromaQuantParamDeltaCb);
		 UTIL_Printf("//u4ChromaQuantParamDeltaCr: %d\n", prVDecAvsPicHdr->u4ChromaQuantParamDeltaCr);
#endif
            }
            u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);

            prVDecAvsPicHdr->u4WeightingQuantParamIndex = (u4Temp >> 30);
#if DEBUG_PARSE_LOG_ENABLE
             UTIL_Printf("//u4WeightingQuantParamIndex: %d\n", prVDecAvsPicHdr->u4WeightingQuantParamIndex);
#endif
            //u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 2);
            prVDecAvsPicHdr->u4WeightingQuantModel = (u4Temp >> 28) & 0x3;
#if DEBUG_PARSE_LOG_ENABLE
             UTIL_Printf("//u4WeightingQuantModel: %d\n", prVDecAvsPicHdr->u4WeightingQuantModel);
#endif
            u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 4);

            if((prVDecAvsPicHdr->u4WeightingQuantParamIndex == 1) ||
                ((prVDecAvsPicHdr->u4MbAdaptWeightingQuantDisable == 0) && (prVDecAvsPicHdr->u4WeightingQuantParamIndex == 3)))
            {
                for(i=0; i<6; i++)
                {
                    prVDecAvsPicHdr->u4WeightingQuantParamDelta1[i] = u4VDEC_AvsSeCodeNum(_u4BSID[u4InstID], u4InstID);
#if DEBUG_PARSE_LOG_ENABLE
                     UTIL_Printf("//u4WeightingQuantParamDelta1[%d]: %d\n", i,prVDecAvsPicHdr->u4WeightingQuantParamDelta1[i]);
#endif
                }
            }

            if((prVDecAvsPicHdr->u4WeightingQuantParamIndex == 2) ||
                 ((prVDecAvsPicHdr->u4MbAdaptWeightingQuantDisable == 0) && (prVDecAvsPicHdr->u4WeightingQuantParamIndex == 3)))
            {
                for(i=0; i<6; i++)
                {
                    prVDecAvsPicHdr->u4WeightingQuantParamDelta2[i] = u4VDEC_AvsSeCodeNum(_u4BSID[u4InstID], u4InstID);
#if DEBUG_PARSE_LOG_ENABLE
                     UTIL_Printf("//u4WeightingQuantParamDelta2[%d]: %d\n", i,prVDecAvsPicHdr->u4WeightingQuantParamDelta2[i]);
#endif
                }
            }
            u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 0);
        }

        prVDecAvsPicHdr->u4AceEnable = (u4Temp >> 31);
#if DEBUG_PARSE_LOG_ENABLE
         UTIL_Printf("//u4AceEnable: %d\n", prVDecAvsPicHdr->u4AceEnable);
#endif
        u4Temp = u4VDEC_HAL_AVS_ShiftGetBitStream(_u4BSID[u4InstID], u4InstID, 1);

    }


    fgSliceStartCdoe = (BOOL) ( u4VDEC_AVS_Search_SliceSC(_u4BSID[u4InstID], u4InstID) );
    
    if (fgSliceStartCdoe)
        prVDecAvsPicHdr->u4IsValid = 1;
    else
    {
        UTIL_Printf("//AVS Cannot Find Slice SC!!!\n");
        VDEC_ASSERT(0);
    }
    return i4RetVal;
    
}

void vVerifyAvsPrepareFBufInfo(UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{  
   //Prepare decode picture, reference picture
   UINT32 u4FbIdx = 0;
   VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = &(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm);
   
   prVDecAvsDecPrm->rAvsWorkBufSa.u4PredSa  = (UINT32)_pucAvsPred[u4InstID];
   prVDecAvsDecPrm->rAvsWorkBufSa.u4Mv1       = (UINT32)_pucAvsMv1[u4InstID];
   prVDecAvsDecPrm->rAvsWorkBufSa.u4Mv2       = (UINT32)_pucAvsMv2[u4InstID];

   if (prVDecAvsDecPrm->prPicHdr->u4PicCodingType == I_PIC)
   {
       //Decode Buffer
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0YSa = prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0CSa = prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4CAddr;
#else
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0CSa = prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4YAddr + prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4CAddrOffset;
#endif
       //Backward Buffer
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic1YSa = prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic1CSa = prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4CAddr;
#else
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic1CSa = prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4YAddr + prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4CAddrOffset;
#endif
       //Forward Buffer
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic2YSa = prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic2CSa = prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddr;
#else
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic2CSa = prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr + prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddrOffset;
#endif
   }
   else
   if (prVDecAvsDecPrm->prPicHdr->u4PicCodingType == P_PIC)
   {
       //Decode Buffer
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0YSa = prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0CSa = prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4CAddr;
#else
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0CSa = prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4YAddr + prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4CAddrOffset;
#endif
       //Backward Buffer
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic1YSa = prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic1CSa = prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddr;
#else
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic1CSa = prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4YAddr + prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX].u4CAddrOffset;
#endif
       //Forward Buffer
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic2YSa = prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic2CSa = prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4CAddr;
#else
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic2CSa = prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4YAddr + prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX].u4CAddrOffset;
#endif
   }
   else
   if (prVDecAvsDecPrm->prPicHdr->u4PicCodingType == B_PIC)
   {
       //Decode Buffer
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0YSa = prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0CSa = prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4CAddr;
#else
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0CSa = prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4YAddr + prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4CAddrOffset;
#endif
       //Backward Buffer
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic1YSa = prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic1CSa = prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4CAddr;
#else
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic1CSa = prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4YAddr + prVDecAvsDecPrm->arFBufInfo[AVS_BW_REF_IDX].u4CAddrOffset;
#endif
       //Forward Buffer
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic2YSa = prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic2CSa = prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddr;
#else
       prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic2CSa = prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4YAddr + prVDecAvsDecPrm->arFBufInfo[AVS_FW_REF_IDX].u4CAddrOffset;
#endif
   }

   prVDecAvsDecPrm->prCurrFBufInfo = &(prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX]);
   _VDEC_ClrAvsFBufInfo(prVDecAvsDecPrm, u4FbIdx, TRUE);
}

void vVerifyAVSSWRst(UINT32 u4BSID, UINT32 u4VDecID)
{    
    //_VDEC_VLDAvsWaitForSramStable();
    vVDEC_HAL_AVS_HW_Reset(u4BSID, u4VDecID);
}

void vVerifySetAvsBsInfo(UINT32 u4BSID, UINT32 u4InstID, VDEC_INFO_DEC_PRM_T *prDecParam, VDEC_INFO_AVS_BS_INIT_PRM_T rAvsBSInitPrm)
{    
    i4VDEC_HAL_AVS_SetBSInfo(u4BSID, u4InstID, prDecParam, rAvsBSInitPrm);
}


// *********************************************************************
// Function    : void vVerAVSUpdateBufStatus(UINT32 u4InstID)
// Description : Update AVS Frame Buffer Status
// Parameter   : None
// Return      : None
// *********************************************************************
void vVerAVSUpdateBufStatus(UINT32 u4InstID)
{
   VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = &(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm);
   VDEC_INFO_AVS_PIC_HDR_T *prVDecAvsPicHdr = &_rVDecAvsPicHdr[u4InstID];
   VDEC_INFO_AVS_FBUF_INFO_T rTempFBufInfo;

   if ( prVDecAvsPicHdr->u4PicStruct == FRM_PIC || prVDecAvsDecPrm->prPicHdr->fgSecField )
   {
      if (prVDecAvsPicHdr->u4PicCodingType == I_PIC || prVDecAvsPicHdr->u4PicCodingType == P_PIC)
      {
         //Maintain DPB
        x_memcpy(&rTempFBufInfo, &prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX], sizeof(VDEC_INFO_AVS_FBUF_INFO_T));
         x_memcpy(&prVDecAvsDecPrm->arFBufInfo[AVS_PREV_FW_IDX], &prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX], sizeof(VDEC_INFO_AVS_FBUF_INFO_T));
        x_memcpy(&prVDecAvsDecPrm->arFBufInfo[AVS_PREV_P_IDX], &prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX], sizeof(VDEC_INFO_AVS_FBUF_INFO_T));
        x_memcpy(&prVDecAvsDecPrm->arFBufInfo[AVS_LAST_P_IDX], &prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX], sizeof(VDEC_INFO_AVS_FBUF_INFO_T));
        //x_memcpy(&prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX], &rTempFBufInfo, sizeof(VDEC_INFO_AVS_FBUF_INFO_T));        
        _VDEC_ClrAvsFBufInfo(prVDecAvsDecPrm, AVS_CURR_WORK_IDX, TRUE);
        prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4YAddr = rTempFBufInfo.u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
        prVDecAvsDecPrm->arFBufInfo[AVS_CURR_WORK_IDX].u4CAddr= rTempFBufInfo.u4CAddr;
#endif
      }
      else
      {
          _VDEC_ClrAvsFBufInfo(prVDecAvsDecPrm, AVS_CURR_WORK_IDX, TRUE);
      }
   }
   else
   {
         //First field Do-nothing.          
   }  
}

void vVerifyVDecSetAVSInfo(UINT32 u4InstID)
{    
    UINT32 u4Width, u4Height, u4DramYSize, u4DramCSize; //For UFO Mode

    //UINT32 u4Temp;
    VDEC_INFO_DEC_PRM_T *prDecPrm = &_tVerMpvDecPrm[u4InstID];
    VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = &(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm);
    //VDEC_INFO_AVS_BS_INIT_PRM_T  rAvsBSInitPrm;

    prVDecAvsDecPrm->prSeqHdr = &_rVDecAvsSeqHdr[u4InstID];
    prVDecAvsDecPrm->prPicHdr = &_rVDecAvsPicHdr[u4InstID];


    if ((prVDecAvsDecPrm->prSeqHdr->u4IsProgSeq) ||
        ((!prVDecAvsDecPrm->prSeqHdr->u4IsProgSeq) && (!prVDecAvsDecPrm->prPicHdr->fgSecField)))
    {
        vVerifyAvsPrepareFBufInfo(u4InstID, prDecPrm);
    }

#if 0   
    if ((!prVDecAvsDecPrm->prSeqHdr->u4IsProgSeq) && (prVDecAvsDecPrm->prPicHdr->fgSecField))
    {        
 
        vVerifyAVSSWRst(_u4BSID[u4InstID], u4InstID);
                 
       //u4ReadPtr = prVdec->rPesInfo.u4VldReadPtr + prVdec->rDecParam.u4ByteCnt;
       rAvsBSInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
       rAvsBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
       rAvsBSInitPrm.u4VLDRdPtr = (UINT32)_pucVFifo[u4InstID];
       rAvsBSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
        
        vVerifySetAvsBsInfo(_u4BSID[u4InstID], u4InstID, prDecPrm, rAvsBSInitPrm);

         // Error handling for re-trigger case
        if (prVDecAvsDecPrm->prPicHdr->fgSecField)
        {
            UTIL_Printf( "prepare for decoding Sec Field\n");       
            while (1)
            {
                u4Temp = u4VDEC_AVS_Search_SC(_u4BSID[u4InstID], u4InstID);
                if ((u4Temp  >= AVS_SLICE_SC_MIN) && (u4Temp  <= AVS_SLICE_SC_MAX))
                {
                    break;
                }
                else if ((u4Temp  == AVS_I_PICTURE_SC) && (u4Temp == AVS_PB_PICTURE_SC))
                {
                     if ( prVDecAvsDecPrm->prPicHdr->u4PicStruct == FRM_PIC || prVDecAvsDecPrm->prPicHdr->fgSecField)
                     {
                          vVerAVSUpdateBufStatus(u4InstID);
                     }
                     return;
                }
            }      
        }
    }
#endif

     prVDecAvsDecPrm->i4MemBase = 0;   
    prVDecAvsDecPrm->u4FRefBufIdx = _u4FRefBufIdx[u4InstID];
    _u4RealHSize[u4InstID] = prVDecAvsDecPrm->prSeqHdr->u2WidthDec;
    _u4RealVSize[u4InstID] = prVDecAvsDecPrm->prSeqHdr->u2HeightDec;

    prDecPrm->ucPicStruct = (UCHAR) prVDecAvsDecPrm->prPicHdr->u4PicStruct;
    prDecPrm->ucPicType = (UCHAR) prVDecAvsDecPrm->prPicHdr->u4PicCodingType;
    prDecPrm->ucAddrSwapMode = _tVerMpvDecPrm[u4InstID].ucAddrSwapMode;
    prDecPrm->u4PicBW = prVDecAvsDecPrm->prSeqHdr->u2WidthDec;
    prDecPrm->u4PicW = prVDecAvsDecPrm->prSeqHdr->u2WidthDec;
    prDecPrm->u4PicH = prVDecAvsDecPrm->prSeqHdr->u2HeightDec;

    prVDecAvsDecPrm->pucDecWorkBufY = (UCHAR*) prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0YSa;
    prVDecAvsDecPrm->pucDecWorkBufC = (UCHAR*) prVDecAvsDecPrm->rAvsFrameBufSa.u4Pic0CSa;
    _pucDecWorkBuf[u4InstID] = prVDecAvsDecPrm->pucDecWorkBufY ;
    _pucDecCWorkBuf[u4InstID] = prVDecAvsDecPrm->pucDecWorkBufC ;

    prVDecAvsDecPrm->prCurrFBufInfo->u1PicCodingType =  prVDecAvsDecPrm->prPicHdr->u4PicCodingType;
    prVDecAvsDecPrm->prCurrFBufInfo->u4PicDistance = prVDecAvsDecPrm->prPicHdr->u4PicDistance;

	//For UFO Mode
	if(prVDecAvsDecPrm->fgIsUFOMode)
	{
		u4Width = ((prVDecAvsDecPrm->prSeqHdr->u4HSize + 63) >> 6) << 6;
		u4Height = ((prVDecAvsDecPrm->prSeqHdr->u4VSize + 63) >> 6) << 6;
		u4DramYSize = u4Width * u4Height;
        u4DramCSize = u4DramYSize >> 1;

        u4DramYSize = ((u4DramYSize + 63) >> 6) << 6;
        u4DramCSize = ((u4DramCSize + 63) >> 6) << 6;

		prVDecAvsDecPrm->prCurrFBufInfo->u4YLenAddr = prVDecAvsDecPrm->prCurrFBufInfo->u4YAddr + u4DramYSize;
		prVDecAvsDecPrm->prCurrFBufInfo->u4CLenAddr = prVDecAvsDecPrm->prCurrFBufInfo->u4CAddr + u4DramCSize;
		prVDecAvsDecPrm->prCurrFBufInfo->u4YLenOffset = u4DramYSize;
		prVDecAvsDecPrm->prCurrFBufInfo->u4CLenOffset = u4DramCSize;

        memset((UINT32)prVDecAvsDecPrm->pucDecWorkBufY, 0, u4DramYSize);
        memset(prVDecAvsDecPrm->prCurrFBufInfo->u4YLenAddr, 0, u4DramYSize>>8);
        memset((UINT32)prVDecAvsDecPrm->pucDecWorkBufC, 0, u4DramCSize);
        memset(prVDecAvsDecPrm->prCurrFBufInfo->u4CLenAddr, 0, u4DramCSize>>8);
	}
	else
	{
		prVDecAvsDecPrm->prCurrFBufInfo->u4YLenAddr = prVDecAvsDecPrm->prCurrFBufInfo->u4YAddr + u4DramYSize;
		prVDecAvsDecPrm->prCurrFBufInfo->u4CLenAddr = prVDecAvsDecPrm->prCurrFBufInfo->u4CAddr + u4DramYSize>>1;
		prVDecAvsDecPrm->prCurrFBufInfo->u4YLenOffset = u4DramYSize;
		prVDecAvsDecPrm->prCurrFBufInfo->u4CLenOffset = u4DramYSize >> 1;
	}
    
    i4VDEC_HAL_AVS_DecStart(_u4BSID[u4InstID], u4InstID, prDecPrm);
}


UINT32 u4AVSInverseAddrSwap(UINT32 u4AddrSwapMode, UINT32 u4SwappedAddr, BOOL fgIsYComponent)
{
  unsigned int u4NonSwappedAddr, u4TempAddr;
  switch(u4AddrSwapMode)
  {
  case 0x1: //MT8520_SWAP_MODE_1
    u4NonSwappedAddr = ((u4SwappedAddr&0xFFFFFFC0) | ((u4SwappedAddr&0x20)>>5) | ((u4SwappedAddr&0x10)>>2) | ((u4SwappedAddr&0x8)>>2) | ((u4SwappedAddr&0x7)<<3));
    break;
  case 0x2: //MT8520_SWAP_MODE_2
    u4NonSwappedAddr = ((u4SwappedAddr&0xFFFFFFE0) | ((u4SwappedAddr&0x10)>>4) | ((u4SwappedAddr&0xF)<<1));
    break;
  case 0x4: // MT5351_SWAP_MODE_0
    if(fgIsYComponent)
    {
      u4TempAddr = ((u4SwappedAddr&0xFFFFFF80) | ((u4SwappedAddr&0x40)>>4) | ((u4SwappedAddr&0x3C)<<1) | (u4SwappedAddr&0x3));
      u4NonSwappedAddr = ((u4TempAddr&0xFFFFFF80) | ((u4TempAddr&0x7C)>>2) | ((u4TempAddr&0x3)<<5));
    }
    else
    {
      u4TempAddr = ((u4SwappedAddr&0xFFFFFFC0) | ((u4SwappedAddr&0x20)>>3) | ((u4SwappedAddr&0x1C)<<1) | (u4SwappedAddr&0x3));
      u4NonSwappedAddr = ((u4TempAddr&0xFFFFFFC0) | ((u4TempAddr&0x3C)>>2) | ((u4TempAddr&0x3)<<4));
    }
    break;
  case 0x5: // MT5351_SWAP_MODE_1
    if(fgIsYComponent)
    {
      u4TempAddr = ((u4SwappedAddr&0xFFFFFF00) | ((~u4SwappedAddr)&0x80) | (u4SwappedAddr&0x7F));
      u4NonSwappedAddr = ((u4TempAddr&0xFFFFFF80) | ((u4TempAddr&0x7C)>>2) | ((u4TempAddr&0x3)<<5));
    }
    else
    {
      u4TempAddr = ((u4SwappedAddr&0xFFFFFF80) | ((~u4SwappedAddr)&0x40) | (u4SwappedAddr&0x3F));
      u4NonSwappedAddr = ((u4TempAddr&0xFFFFFFC0) | ((u4TempAddr&0x3C)>>2) | ((u4TempAddr&0x3)<<4));
    }
    break;
  case 0x6: // MT5351_SWAP_MODE_2
    if(fgIsYComponent)
    {
      u4NonSwappedAddr = ((u4SwappedAddr&0xFFFFFF80) | ((u4SwappedAddr&0x7C)>>2) | ((u4SwappedAddr&0x3)<<5));
    }
    else
    {
      u4NonSwappedAddr = ((u4SwappedAddr&0xFFFFFFC0) | ((u4SwappedAddr&0x3C)>>2) | ((u4SwappedAddr&0x3)<<4));
    }
    break;
  default:
    u4NonSwappedAddr = u4SwappedAddr;
    break;
  }
  return u4NonSwappedAddr;
}

void vAVS_InvAddressSwap(UINT32 u4InstID, 
                                                         BYTE* pbSrcBufY, BYTE* pbSrcBufC, 
                                                         BYTE* pbOutBufY, BYTE* pbOutBufC,
                                                         UINT32 u4AlignWidth, UINT32 u4AlignHeight, UINT32 u4AlignSize,
                                                         UINT32 u4HwSwapMode)
{
  UINT32 i;
  UINT32 u4DataLength;
  UINT32 u4AlignW_Luma;
  UINT32 u4AlignH_Luma;
  UINT32 u4AlignW_Chroma;
  UINT32 u4AlignH_Chroma;
  //UINT32 u4AlignSize = 0x32000;
  UINT32 u4NonSwappedAddr;  
  UINT32 u4SwappedAddr;
  BYTE * pbTempBufAddr;
  UINT32 u4AddrressSwapSize = 16;
  UINT32 u4AddrSwapMode;

   UINT8 auAddrSwapMapTable[8] =
 {
    4, 5, 6, 7, 0, 1, 2, 3
  };

  #ifdef RM_DDR3MODE_ENABLE
  u4AddrressSwapSize = 16;
  #else //RM_DDR3MODE_ENABLE
  u4AddrressSwapSize = 16;
  #endif //RM_DDR3MODE_ENABLE

  //prParsingPic = (VDEC_INFO_RM_PICINFO_T*) &_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecRMDecPrm.rRMParsPicInfo;

  u4AlignW_Luma = u4AlignWidth;//prParsingPic->u4Width;
  u4AlignH_Luma = u4AlignHeight;//prParsingPic->u4Height;
  
  //pbSrcBufY = (BYTE*) prParsingPic->u4OutBufY;
  //pbSrcBufC = (BYTE*) prParsingPic->u4OutBufC;
  //pbOutBufY = (BYTE*) _pucDumpYBuf[u4InstID];
  //pbOutBufC = (BYTE*) _pucDumpCBuf[u4InstID];

  u4AddrSwapMode = auAddrSwapMapTable[u4HwSwapMode];
  
  //Luma 
  u4DataLength = u4AlignW_Luma * u4AlignH_Luma;
  //u4DataLength = (u4DataLength + u4AlignSize-1)/u4AlignSize;
  //u4DataLength = u4DataLength * u4AlignSize;
  u4SwappedAddr = 0;
  
  for (i=0; i<u4DataLength; i+=u4AddrressSwapSize)
  {
    u4NonSwappedAddr = u4AVSInverseAddrSwap(u4AddrSwapMode, u4SwappedAddr, TRUE);
    pbTempBufAddr = (BYTE*) (pbSrcBufY+i);
    x_memcpy(&pbOutBufY[u4NonSwappedAddr<<4], &pbTempBufAddr[0],u4AddrressSwapSize);
    u4SwappedAddr++;
  }
  

  //Chroma
  u4AlignW_Chroma = u4AlignW_Luma;
  u4AlignH_Chroma = u4AlignH_Luma / 2;
  
  u4DataLength = u4AlignW_Chroma * u4AlignH_Chroma;
  //u4DataLength = (u4DataLength + u4AlignSize-1)/u4AlignSize;
  //u4DataLength = u4DataLength * u4AlignSize;
  u4SwappedAddr = 0;

  for (i=0; i<u4DataLength; i+=u4AddrressSwapSize)
  {
    u4NonSwappedAddr = u4AVSInverseAddrSwap(u4AddrSwapMode, u4SwappedAddr, FALSE);
    pbTempBufAddr = (BYTE*) (pbSrcBufC+i);
    x_memcpy(&pbOutBufC[u4NonSwappedAddr<<4], &pbTempBufAddr[0],u4AddrressSwapSize);
    u4SwappedAddr++;
  }
}

// *********************************************************************
// Function    : void vVP6WrData2PC(UINT32 u4InstID, BYTE *ptAddr, UINT32 u4Size, BOOL *fgNextFrameExist)
// Description : Write the decoded data to PC for compare
// Parameter   : None
// Return      : None
// *********************************************************************
#if ERR_DUMP
UINT32 _u4FrameNumWatch = 0xFFFFFFFF;
BOOL _fgForceWait = TRUE;
#endif
void vAVSWrData2PC(UINT32 u4InstID, UCHAR *ptAddr, UINT32 u4Size, BOOL *fgNextFrameExist)
{
    #if ((!defined(COMP_HW_CHKSUM)) || defined(DOWN_SCALE_SUPPORT))
    UINT32 u4Cnt;
    #ifdef GOLDEN_128BIT_COMP  
    UINT32 u4XPix,u4YPix;
    #endif

    UINT32 u4Width,u4Height;
    UINT32 u4YBase,u4CBase;
    //UINT32 u4BufferWidth;
    UCHAR *pbDecBuf,*pbGoldenBuf;

    #ifndef GOLDEN_128BIT_COMP  
    UINT32 u4Pix;
    UINT32  i, j;
    #else
    UINT32 u4Ty0, u4Tx0, u4Ty1, u4Tx1;
    UINT32 u4X, u4Y;
    UINT32 mbw, mbh, i, j;
    UINT32 u4Start;  
    #endif  

    #endif  
    
    VDEC_INFO_AVS_SEQ_HDR_T *prVDecAvsSeqHdr = &_rVDecAvsSeqHdr[u4InstID];
    BOOL fgDecErr,fgOpen;
    char strMessage[256];   
    BOOL fgCompare = TRUE;

    UINT32 u4NonSwapYBase = 0;
    UINT32 u4NonSwapCBase = 0;
    UINT32 u4SwapYBase = 0;
    UINT32 u4SwapCBase = 0;
    UINT32 u4NonSwapTargY = 0;
    UINT32 u4NonSwapTargC = 0;
    #if AVS_UFO_MODE_SUPPORT
    CHAR _bFileAddStrY[20] = {"_bits_Y.out\0"};
    CHAR _bFileAddStrC[20] = {"_bits_CbCr.out\0"};
    #else
    CHAR _bFileAddStrY[20] = {"_Y.out\0"};
    CHAR _bFileAddStrC[20] = {"_CbCr.out\0"};
    #endif
    CHAR _bPatternPath [256];
    #if !USB_LOAD_BS || IDE_SUPPORT
     #if AVS_UFO_MODE_SUPPORT
    CHAR _bGoldFileName[256] = {"\\ufo_\0"};	 
    CHAR _bPathAddStr[256] = {"pattern\\\0"};
    CHAR _bPP_PathStr[256] = {"\\ufo_pat\0"};
     #else
    CHAR _bGoldFileName[256] = {"\\post_\0"};    
    CHAR _bPathAddStr[256] = {"pattern\\\0"};
    CHAR _bPP_PathStr[256] = {"\\pp_pat\\pp_pat\0"};
     #endif
    #else
    CHAR _bGoldFileName[256] = {"/post_\0"};    
    CHAR _bPathAddStr[256] = {"pattern/\0"};
    CHAR _bPP_PathStr[256] = {"/pp_pat/pp_pat\0"};
    #endif    
    CHAR _bFileName [256];
    UINT32 u4Temp;
//Searh path name
    UINT32 path_byte_addr = 0;
    UINT32 filename_byte_addr = 0;

    UINT32 u4AlignWidth, u4AlignHeight;
    UINT32 u4AlignSize = 0;

    #if VDEC_ERR_RETRY 
    UINT32 u4RetryCount = 0;
    #endif
    
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
              if( (_bFileStr1[u4InstID][1][i+1] == 'a' || _bFileStr1[u4InstID][1][i+1] == 'A')
              && (_bFileStr1[u4InstID][1][i+2] == 'v' || _bFileStr1[u4InstID][1][i+2] == 'V')
              && (_bFileStr1[u4InstID][1][i+3] == 's' || _bFileStr1[u4InstID][1][i+3] == 'S'))
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
    u4Temp += sprintf(_bPatternPath+path_byte_addr+u4Temp, "%s", _bPP_PathStr);    
    u4Temp += sprintf(_bPatternPath+path_byte_addr+u4Temp, "%s", _bGoldFileName);    
    _bPatternPath[path_byte_addr+u4Temp] = '\0';

    #if 0
    UTIL_Printf("vAVSWrData2PC, pat(%x):%s, y:%s, cnt:%d, 1\n", 
      &_bPatternPath[0],
      _bPatternPath,
      _bFileAddStrY,
      _u4FileCnt[u4InstID]);
    #endif
    
    vConcateStr(_bFileStr1[u4InstID][3], _bPatternPath, _bFileAddStrY, _u4FileCnt[u4InstID]);
    
    #if 0
    UTIL_Printf("vAVSWrData2PC, _bFileStr1(%x):%s, 1\n", 
      &_bFileStr1[u4InstID][3][0],
      _bFileStr1[u4InstID][3]); 
    #endif

    strcpy(_tFileListRecInfo[u4InstID].bFileName, _FileList_Rec[u4InstID]);

    #ifdef REDEC  
    if(_u4FileCnt[u4InstID] == _u4ReDecPicNum[u4InstID] )
    {
        if(_u4ReDecNum[u4InstID] != 0)
        {
            _u4ReDecPicNum[u4InstID] = 0xFFFFFFFF;
            _u4ReDecCnt[u4InstID] = _u4ReDecNum[u4InstID];
            vVDecOutputDebugString("/n!!!!!!!!!!!!!! Re-Decode and Wait for debug !!!!!!!!!!!!!!!!\n");
        }
    }
    if(_u4ReDecCnt[u4InstID] > 0)
    {
        _u4ReDecCnt[u4InstID]--;
    }
    #endif    

    #ifdef GEN_HW_CHKSUM
    #ifndef INTERGRATION_WITH_DEMUX
    vWrite2PC(u4InstID, 9, NULL);
    #endif
    #endif

    UTIL_Printf("//Frame [%d]\n", _u4FileCnt[u4InstID]);
    
#if VDEC_ERR_RETRY
RETRY:

    if (u4RetryCount > 0)
        UTIL_Printf("//u4RetryCount [%d]\n", u4RetryCount);

    fgDecErr = FALSE;
#else
    fgDecErr = FALSE;
#endif

    #ifndef INTERGRATION_WITH_DEMUX
    if(fgCompare)
    {
        #if (defined(COMP_HW_CHKSUM) && (!defined(DOWN_SCALE_SUPPORT)))       
        #else // compare pixel by pixel
        UTIL_Printf("//Y compare\n");
        
        // Y compare
        _tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;  
        _tFBufFileInfo[u4InstID].pucTargetAddr = _pucDumpYBuf[u4InstID];
        _tFBufFileInfo[u4InstID].u4TargetSz = GOLD_Y_SZ;  
        _tFBufFileInfo[u4InstID].u4FileLength = 0;  

        #if 0
        UTIL_Printf("vAVSWrData2PC, pat(%x):%s, y:%s, cnt:%d, 2\n", 
          &_bPatternPath[0],
          _bPatternPath,
          _bFileAddStrY,
          _u4FileCnt[u4InstID]); 
        #endif
     
        // Y decoded data Compare   
        vConcateStr(_bFileStr1[u4InstID][3], _bPatternPath, _bFileAddStrY, _u4FileCnt[u4InstID]);

        #if 0
        UTIL_Printf("vAVSWrData2PC, _bFileStr1(%x):%s, 2\n", 
          &_bFileStr1[u4InstID][3][0],
          _bFileStr1[u4InstID][3]);
        #endif

        #ifdef EXT_COMPARE 
        #if AVS_TEST_HD
        _tFBufFileInfo[u4InstID].u4FileLength = (((prVDecAvsSeqHdr->u2WidthDec + 63)>>6)<<6) *(((prVDecAvsSeqHdr->u2HeightDec + 31)>>5)<<5);
        #else
        _tFBufFileInfo[u4InstID].u4FileLength = (((prVDecAvsSeqHdr->u2WidthDec + 15)>>4)<<4) *(((prVDecAvsSeqHdr->u2HeightDec + 31)>>5)<<5);
        #endif        
        #else
        #ifdef DIRECT_DEC
        if((_u4FileCnt[u4InstID] >= _u4StartCompPicNum[u4InstID]) && ( _u4FileCnt[u4InstID] <= _u4EndCompPicNum[u4InstID]))
        #endif    
        {
            #ifdef DOWN_SCALE_SUPPORT            
            #else
            
            u4NonSwapYBase = (UINT32)_pucDecWorkBuf[u4InstID];
            u4NonSwapCBase = (UINT32)_pucDecCWorkBuf[u4InstID];

            #if 1
            if (_tVerMpvDecPrm[u4InstID].ucAddrSwapMode != ADDRSWAP_OFF)
            {
               
                u4SwapYBase = (UINT32)_pucDecWorkBuf[u4InstID];
                u4SwapCBase = (UINT32)_pucDecCWorkBuf[u4InstID];
                
                u4AlignWidth = _tVerMpvDecPrm[u4InstID].u4PicW;
                u4AlignWidth = (((u4AlignWidth +63) >>6) <<6); //Align to 4MB width                    
                u4AlignHeight = _tVerMpvDecPrm[u4InstID].u4PicH;
                u4AlignHeight =  (((u4AlignHeight +31) >>5) <<5);

                u4NonSwapTargY = (UINT32)_pucDumpYBuf[u4InstID];
                u4NonSwapTargC = (UINT32)_pucDumpCBuf[u4InstID];

                vAVS_InvAddressSwap(u4InstID, 
                                   (BYTE*) u4SwapYBase,  (BYTE*) u4SwapCBase, 
                                   (BYTE*) u4NonSwapTargY, (BYTE*) u4NonSwapTargC,
                                  u4AlignWidth,  u4AlignHeight, u4AlignSize,
                                  _tVerMpvDecPrm[u4InstID].ucAddrSwapMode);
              
                u4NonSwapYBase = (UINT32)_pucAddressSwapBuf[u4InstID];
                u4NonSwapCBase = (UINT32)_pucAddressSwapBuf[u4InstID] + PIC_Y_SZ; //0x1FE000;

                // because need load golden data to buffer _pucDumpYBuf later,
                // move data to temp buffer(u4NonSwapYBase, u4NonSwapCBase) after swap processing 
                // then will compare this temp buffer data with golden buffer data
                x_memcpy((UCHAR*)u4NonSwapYBase, _pucDumpYBuf[u4InstID],(u4AlignWidth*u4AlignHeight) +u4AlignSize);
                x_memcpy((UCHAR*)u4NonSwapCBase, _pucDumpCBuf[u4InstID],(u4AlignWidth*u4AlignHeight/2) + u4AlignSize);                 

                Printf("//AVS inverse swap mode %d, u4AlignWidth:%d, u4AlignHeight:%d\n", 
                    _tVerMpvDecPrm[u4InstID].ucAddrSwapMode,
                    u4AlignWidth,
                    u4AlignHeight);
            }
            #endif

            fgOpenFile(u4InstID, _bFileStr1[u4InstID][3],"r+b", &_tFBufFileInfo[u4InstID]);               
            HalFlushInvalidateDCache();
            _u4GoldenYSize[u4InstID] = _tFBufFileInfo[u4InstID].u4RealGetBytes;

            #endif
        }
        #endif


        u4Cnt = 0; 
        #ifdef EXT_COMPARE 
        if(_ptCurrFBufInfo[u4InstID]->ucFBufStatus == FRAME)
        {
            vWrite2PC(u4InstID, 5, _pucDecWorkBuf[u4InstID]);
        }
        #else
        
        u4YBase = u4NonSwapYBase;//(UINT32)_pucDecWorkBuf[u4InstID];        
        u4Width = prVDecAvsSeqHdr->u2WidthDec;
        u4Height = prVDecAvsSeqHdr->u2HeightDec;       

        u4Height = ( (u4Height)>>5 ) <<5;
        #ifdef DIRECT_DEC
        if((_u4FileCnt[u4InstID] >= _u4StartCompPicNum[u4InstID]) && ( _u4FileCnt[u4InstID] <= _u4EndCompPicNum[u4InstID]))
        #endif    
        {
            //zhi0925 Printf("//u4YBase:0x%x, u4RealGetBytes:0x%x(%d)\n", u4YBase, _tFBufFileInfo[u4InstID].u4RealGetBytes, _tFBufFileInfo[u4InstID].u4RealGetBytes);

            #if 1 // if in swap mode, use cache data updated before, else need sync cache with dram
            if (_tVerMpvDecPrm[u4InstID].ucAddrSwapMode == ADDRSWAP_OFF)
            #endif
            {
                //HalInvalidateDCache();
                //zhi0917 vVDec_InvDCacheRange(u4YBase, _tFBufFileInfo[u4InstID].u4RealGetBytes); // tmp: use u4RealGetBytes
            }
        
            #ifdef GOLDEN_128BIT_COMP
            #if AVS_TEST_HD
            u4Tx0 = (u4Width >> 6);   // w/ 64
            u4Ty0 = (u4Height >> 5);  // h /32
            u4X = (u4Width & 0x3F);    // w % 64
            u4Y = (u4Height & 0x1F);  // h%32
            #else
            u4Tx0 = (u4Width >> 4);   // w/ 16
            u4Ty0 = (u4Height >> 5);  // h /32
            u4X = (u4Width & 0xF);    // w % 16
            u4Y = (u4Height & 0x1F);  // h%32
            #endif
            u4Tx1 = (u4X==0)? u4Tx0 : (u4Tx0+1);
            u4Ty1 = (u4Y==0)? u4Ty0 : (u4Ty0+1);

            for (mbh=0; mbh < u4Ty1; mbh++)
            {
                for (mbw=0; mbw < u4Tx1; mbw++)
                {
                    #if AVS_TEST_HD
                    u4Start = (mbh*u4Tx1 + mbw) * (64*32);
                    #else
                    u4Start = (mbh*u4Tx1 + mbw) * (16*32);
                    #endif
                    pbGoldenBuf = (UCHAR*) (((UINT32) (_pucDumpYBuf[u4InstID])) + u4Start);
                    pbDecBuf = (UCHAR*) (u4YBase + u4Start);

                    for(j=0; j < 32; j++)
                    {    
                        #if AVS_TEST_HD
                        for(i=0; i < 64; i++)
                        #else
                        for(i=0; i < 16; i++)
                        #endif
                        {                   
                            if(1)
                            {
                                if(  (mbw == u4Tx0 && i >= u4X) || (mbh == u4Ty0 && j >= u4Y))
                                {
                                    //Do not compare
                                }
                                else
                                {
                                    if ((*(pbDecBuf)) != (*(pbGoldenBuf)))
                                    {
                                        u4Cnt ++;
                                        #if AVS_TEST_HD
                                        u4XPix = mbw * 64 + i;
                                        #else
                                        u4XPix = mbw * 16 + i;
                                        #endif
                                        u4YPix = mbh * 32 + j;
					vVDecOutputDebugString("//Pic count to [%d]\n", _u4FileCnt[u4InstID]);  
                                        vVDecOutputDebugString("//Y Data Mismatch at 0x%x[x= 0x%d, y=0x%d] = 0x%x, Golden at 0x%x = 0x%x !!! \n", pbDecBuf, u4XPix, u4YPix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                                        sprintf(strMessage,"//Y Data Mismatch at 0x%x[x= 0x%d, y=0x%d] = 0x%x, Golden at 0x%x = 0x%x !!! \n", pbDecBuf, u4XPix, u4YPix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                                        fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);

                                        UTIL_Printf("//Pic count to [%d]\n", _u4FileCnt[u4InstID]);     
                                        UTIL_Printf("//Y Data Mismatch at 0x%x[x= 0x%d, y=0x%d] = 0x%x, Golden at 0x%x = 0x%x !!! \n", pbDecBuf, u4XPix, u4YPix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));     

                                        //fprintf(_tFileListRecInfo.fpFile, "Y Data Mismatch at [0x%.8x] = 0x%.2x, Golden = 0x%.2x !!! \n", i, pbDecBuf[i], _pucDumpBuf[i]);
                                        fgDecErr = TRUE;
                                        //vDumpReg();  // mark by ginny
                                        //vVDEC_HAL_AVS_VDec_DumpReg(u4InstID, FALSE);  // mark by ginny                                          
                                        break;    
                                    }
                                }
                            }//end of if


                            pbGoldenBuf++;
                            pbDecBuf++;
                        }//End of i

                        if(fgDecErr == TRUE)
                        {
                        break;
                        }
                    }//End of j

                    if(fgDecErr == TRUE)
                    {
                        break;
                    }
                }

                if(fgDecErr == TRUE)
                {
                    break;
                }
            }
            #else              
            for (u4Pix = 0; u4Pix < u4Width*u4Height; u4Pix++)
            {
                if(1)
                {
                    //pbDecBuf = (UCHAR*)u4CalculatePixelAddress_Y(u4YBase, u4XPix, u4YPix, u4BufferWidth, 1, 4);
                    pbDecBuf = (UCHAR*)(u4YBase+u4Pix);
                    //pbGoldenBuf = (UCHAR*)u4CalculatePixelAddress_Y((UINT32)_pucDumpYBuf[u4InstID], u4XPix, u4YPix, u4BufferWidth, 1, 4);
                    pbGoldenBuf = (UCHAR*)((UINT32)_pucDumpYBuf[u4InstID]+u4Pix);
                    if ((*(pbDecBuf)) != (*(pbGoldenBuf)))
                    {
                        u4Cnt ++;
                        //vVDecOutputDebugString("Y Data Mismatch at [x= 0x%.8x, y=0x%.8x] = 0x%.2x, Golden = 0x%.2x !!! \n", u4XPix, u4YPix, (*pbDecBuf), (*pbGoldenBuf));
                        vVDecOutputDebugString("//Y Data Mismatch at 0x%x[%d] = 0x%.2x, Golden at 0x%x = 0x%.2x !!! \n", pbDecBuf, u4Pix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                        sprintf(strMessage,"//Y Data Mismatch at 0x%x[%d] = 0x%.2x, Golden at 0x%x = 0x%.2x !!! \n", pbDecBuf, u4Pix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                        fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
                        //fprintf(_tFileListRecInfo.fpFile, "Y Data Mismatch at [0x%.8x] = 0x%.2x, Golden = 0x%.2x !!! \n", i, pbDecBuf[i], _pucDumpYBuf[_u4VDecID][i]);
                        fgDecErr = TRUE;
                        //vVDEC_HAL_AVS_VDec_DumpReg(u4InstID, FALSE);  // mark by ginny
                        break;    
                    }            
                }
                if(fgDecErr == TRUE)
                {
                    break;
                }
            }
            #endif		
            //vVDecOutputDebugString("\nY Data Compare Over!!! Total bytes [0x%.8x] & error [%d]\n", _u4GoldenYSize[_u4VDecID], u4Cnt);
        }
        #endif

        UTIL_Printf("//CbCr compare\n");

        // CbCr compare
        //if(!fgIsMonoPic(_u4VDecID))
        {
            // CbCr decoded data Compare
            _tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;  
            _tFBufFileInfo[u4InstID].pucTargetAddr = _pucDumpCBuf[u4InstID];
            _tFBufFileInfo[u4InstID].u4TargetSz = GOLD_C_SZ;  
            _tFBufFileInfo[u4InstID].u4FileLength = 0;

            #if 0
            UTIL_Printf("vAVSWrData2PC, pat(%x):%s, y:%s, cnt:%d, 3\n", 
              &_bPatternPath[0],
              _bPatternPath,
              _bFileAddStrC,
              _u4FileCnt[u4InstID]);  
            #endif
         
            vConcateStr(_bFileStr1[u4InstID][4], _bPatternPath, _bFileAddStrC, _u4FileCnt[u4InstID]);

            #if 0
            UTIL_Printf("vAVSWrData2PC, _bFileStr1(%x):%s, 3\n", 
              &_bFileStr1[u4InstID][4][0],
              _bFileStr1[u4InstID][4]);
            #endif
            
            #ifdef EXT_COMPARE 
            #if AVS_TEST_HD
            _tFBufFileInfo[u4InstID].u4FileLength = (((prVDecVp6FrmHdr->u2WidthDec + 63)>>6)<<6) *(((prVDecVp6FrmHdr->u2HeightDec + 31)>>5)<<5) >>1;
            #else
            _tFBufFileInfo[u4InstID].u4FileLength = (((prVDecVp6FrmHdr->u2WidthDec + 15)>>4)<<4) *(((prVDecVp6FrmHdr->u2HeightDec + 31)>>5)<<5) >>1;
            #endif
            #else
            #ifdef DIRECT_DEC
            if((_u4FileCnt[u4InstID] >= _u4StartCompPicNum[u4InstID]) && ( _u4FileCnt[u4InstID] <= _u4EndCompPicNum[u4InstID]))
            #endif    
            {  
                #ifndef DOWN_SCALE_SUPPORT
                   fgOpenFile(u4InstID, _bFileStr1[u4InstID][4],"r+b", &_tFBufFileInfo[u4InstID]);               
                   HalFlushInvalidateDCache();
                _u4GoldenCSize[u4InstID] = _tFBufFileInfo[u4InstID].u4RealGetBytes;
                #endif
            }
            #endif       
            u4Cnt = 0; 
            #ifdef EXT_COMPARE  
            if(_ptCurrFBufInfo[u4InstID]->ucFBufStatus == FRAME)
            {
                vWrite2PC(u4InstID, 6, _pucDecCWorkBuf[u4InstID]);
            }
            #else    
            #if defined(DOWN_SCALE_SUPPORT)
            UINT32 u4DramPicSize = 0x1FE000;
            u4CBase = (UINT32)_pucVDSCLBuf[u4InstID] + u4DramPicSize;
            #else
            u4CBase = u4NonSwapCBase;//(UINT32)_pucDecCWorkBuf[u4InstID];
            #endif  

            #ifdef DIRECT_DEC
            if((_u4FileCnt[u4InstID] >= _u4StartCompPicNum[u4InstID]) && ( _u4FileCnt[u4InstID] <= _u4EndCompPicNum[u4InstID]))
            #endif      
            {               
                UINT32 u4WidthC = u4Width / 2;
                UINT32 u4HeightC = u4Height / 2;
                
                Printf("//u4CBase:0x%x, u4RealGetBytes:0x%x(%d)\n", u4CBase, _tFBufFileInfo[u4InstID].u4RealGetBytes, _tFBufFileInfo[u4InstID].u4RealGetBytes);
                
                #if 1 // if in swap mode, use cache data updated before, else need sync cache with dram
                if (_tVerMpvDecPrm[u4InstID].ucAddrSwapMode == ADDRSWAP_OFF)
                #endif
                {
                    //HalInvalidateDCache();
                    //zhi0917 vVDec_InvDCacheRange(u4CBase, _tFBufFileInfo[u4InstID].u4RealGetBytes); // tmp: use u4RealGetBytes
                }
                
                #ifdef GOLDEN_128BIT_COMP
                #if AVS_TEST_HD
                u4Tx0 = ( (u4WidthC+63) >> 6);   // w/ 64
                u4Ty0 = ( (u4HeightC+15) >> 4);  // h /16
                u4X = (u4WidthC & 0x63);    // w % 64
                u4Y = (u4HeightC & 0xF);  // h % 16
                #else
                u4Tx0 = ( (u4WidthC+7) >> 3);   // w/ 8
                u4Ty0 = ( (u4HeightC+15) >> 4);  // h /16
                u4X = (u4WidthC & 0x7);    // w % 8
                u4Y = (u4HeightC & 0xF);  // h % 16
                #endif
                u4Tx1 = (u4X==0)? u4Tx0 : (u4Tx0+1);
                u4Ty1 = (u4Y==0)? u4Ty0 : (u4Ty0+1);

                for (mbh=0; mbh < u4Ty1; mbh++)
                {
                    for (mbw=0; mbw < u4Tx1; mbw++)
                    {
                        #if AVS_TEST_HD
                        u4Start = (mbh*u4Tx1 + mbw) * (64*16);
                        #else
                        u4Start = (mbh*u4Tx1 + mbw) * (16*16);
                        #endif
                        pbGoldenBuf = (UCHAR*) (((UINT32) (_pucDumpCBuf[u4InstID])) + u4Start);
                        pbDecBuf = (UCHAR*) (u4CBase + u4Start);

                        for(j=0; j < 16; j++)
                        {
                            #if AVS_TEST_HD
                            for(i=0; i < 64; i++)
                            #else
                            for(i=0; i < 8; i++)
                            #endif
                            {                       
                                if(1)
                                {
                                    if(  (mbw == u4Tx0 && i >= u4X) || (mbh == u4Ty0 && j >= u4Y))
                                    {
                                        //Do not compare
                                        pbGoldenBuf+=2;
                                        pbDecBuf+=2;
                                    }
                                    else
                                    {
                                        //Compare Cb
                                        if ((*(pbDecBuf)) != (*(pbGoldenBuf)))
                                        {
                                            #if AVS_TEST_HD
                                            u4XPix = mbw * 64 + i;
                                            #else
                                            u4XPix = mbw * 8 + i;
                                            #endif
                                            
                                            u4YPix = mbh * 16 + j;
                                            u4Cnt ++;
                                            vVDecOutputDebugString("//Cb Data Mismatch at 0x%x[x= 0x%d, y=0x%d] = 0x%x, Golden at 0x%x= 0x%x !!! \n", pbDecBuf, u4XPix, u4YPix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                                            sprintf(strMessage,"//Cb Data Mismatch at 0x%x[x= 0x%d, y=0x%d] = 0x%x, Golden at 0x%x = 0x%x !!! \n", pbDecBuf, u4XPix, u4YPix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                                            fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
 
                                            UTIL_Printf("//Cb Data Mismatch at 0x%x[x= 0x%d, y=0x%d] = 0x%x, Golden at 0x%x= 0x%x !!! \n", pbDecBuf, u4XPix, u4YPix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));     

                                            //fprintf(_tFileListRecInfo.fpFile, "Y Data Mismatch at [0x%.8x] = 0x%.2x, Golden = 0x%.2x !!! \n", i, pbDecBuf[i], _pucDumpBuf[i]);
                                            fgDecErr = TRUE;
                                            //vVDEC_HAL_AVS_VDec_DumpReg(u4InstID, FALSE);  // mark by ginny
                                            break;    
                                        }

                                        pbGoldenBuf++;
                                        pbDecBuf++;
                                        //Compare Cr
                                        if ((*(pbDecBuf)) != (*(pbGoldenBuf)))
                                        {
                                            #if AVS_TEST_HD
                                            u4XPix = mbw * 64 + i;
                                            #else
                                            u4XPix = mbw * 8 + i;
                                            #endif
                                            
                                            u4YPix = mbh * 16 + j;
                                            u4Cnt ++;
                                            vVDecOutputDebugString("//Cr Data Mismatch at 0x%x[x= 0x%d, y=0x%d] = 0x%x, Golden at 0x%x = 0x%x !!! \n", pbDecBuf, u4XPix, u4YPix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                                            sprintf(strMessage,"//Cr Data Mismatch at 0x%x[x= 0x%d, y=0x%d] = 0x%x, Golden at 0x%x = 0x%x !!! \n", pbDecBuf, u4XPix, u4YPix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                                            fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);

                                            UTIL_Printf("//Cr Data Mismatch at 0x%x[x= 0x%d, y=0x%d] = 0x%x, Golden at 0x%x = 0x%x !!! \n", pbDecBuf, u4XPix, u4YPix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));     

                                            //fprintf(_tFileListRecInfo.fpFile, "Y Data Mismatch at [0x%.8x] = 0x%.2x, Golden = 0x%.2x !!! \n", i, pbDecBuf[i], _pucDumpBuf[i]);
                                            fgDecErr = TRUE;
                                            //vVDEC_HAL_AVS_VDec_DumpReg(u4InstID, FALSE);  // mark by ginny                                            
                                            break;    
                                        }
                                        pbGoldenBuf++;
                                        pbDecBuf++;
                                    }
                                }
                                else
                                {
                                    pbGoldenBuf+=2;
                                    pbDecBuf+=2;
                                }

                            }
                            if(fgDecErr == TRUE)
                            {
                                break;
                            }
                        }

                        if(fgDecErr == TRUE)
                        {
                            break;
                        }
                    }

                    if(fgDecErr == TRUE)
                    {
                        break;
                    }
                }
                #else                          
                for (u4Pix = 0; u4Pix < u4Width*(u4Height>>1); u4Pix++)
                {
                    if((_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecVP6DecPrm.fgDec2ndFld) || (_tVerMpvDecPrm[u4InstID].ucPicStruct == FRM_PIC))
                    {
                        pbDecBuf = (UCHAR*)(u4CBase+u4Pix);
                        pbGoldenBuf = (UCHAR*)((UINT32)_pucDumpCBuf[u4InstID]+u4Pix);
                        if ((*(pbDecBuf)) != (*(pbGoldenBuf)))
                        {
                        #if defined(DOWN_SCALE_SUPPORT)
                            if(VDEC_PP_ENABLE)
                            {
                                if ((*(pbDecBuf)) > (*(pbGoldenBuf)) && (*(pbDecBuf))<= (*(pbGoldenBuf) + 1)  || 
                                    (*(pbDecBuf)) < (*(pbGoldenBuf)) && (*(pbDecBuf))>= (*(pbGoldenBuf) - 1) )
                                    {
                                        //Pass
                                        //How the C Code round off floating number method is not the same as HW in full agreement
                                        //Therefor, difference between +-1 is tolerated
                                    }
                            }
                            else
                        #endif
                            {
                            u4Cnt ++;
                            vVDecOutputDebugString("//C Data Mismatch at 0x%x[%d] = 0x%.2x, Golden at 0x%x= 0x%.2x !!! \n", pbDecBuf, u4Pix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                            sprintf(strMessage,"//C Data Mismatch at 0x%x[%d] = 0x%.2x, Golden at 0x%x = 0x%.2x !!! \n", pbDecBuf, u4Pix, (*pbDecBuf), pbGoldenBuf, (*pbGoldenBuf));
                            fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
                            //fprintf(_tFileListRecInfo.fpFile, "Y Data Mismatch at [0x%.8x] = 0x%.2x, Golden = 0x%.2x !!! \n", i, pbDecBuf[i], _pucDumpBuf[i]);
                            fgDecErr = TRUE;
                            //vVDEC_HAL_AVS_VDec_DumpReg(u4InstID, FALSE);  // mark by ginny                            
                            break;    
                        }
                    }
                    }
                    if(fgDecErr == TRUE)
                    {
                        break;
                    }
                }
                #endif          
                //vVDecOutputDebugString("CbCr Data Compare Over!!! Total bytes [0x%.8x] & error [%d]\n", _u4GoldenCSize[_u4VDecID], u4Cnt);
            }
            #endif    
        }
        #endif
    }

#if ERR_DUMP    
    if ((TRUE == fgDecErr) || (_u4FileCnt[u4InstID] == _u4FrameNumWatch))
    {
        UTIL_Printf("//dump register, fgDecErr:[%d, %d], _u4FrameNumWatch:%d, _u4FileCnt:%d\n", fgDecErr, _fgVDecErr[u4InstID], _u4FrameNumWatch, _u4FileCnt[u4InstID]);
        
        //vVDEC_HAL_AVS_VDec_DumpReg(u4InstID, FALSE);
#if DEBUG_DUMP_REG_ENABLE
        //vVdecAvsDumpReg(u4InstID, 0);
#endif

        #if VDEC_ERR_RETRY
        u4RetryCount++;
        
        if (u4RetryCount <= VDEC_ERR_RETRY_COUNT)
        {
            #if 0 // rollback file count info
            VDEC_ASSERT(_u4FileCnt[u4InstID] > 0);
            
            #ifdef REDEC    
            if(_u4ReDecCnt[u4InstID] == 0)
            #endif    
                _u4FileCnt[u4InstID]--;

            _u4VerBitCount[u4InstID] = 0;            

            fgDecErr = FALSE;
            #endif
            
            goto RETRY;
        }
        else
        #endif
        {
            #if VDEC_ERR_STOP
            while(_fgForceWait);
            #endif
        } 
    }
#endif

    #ifndef IDE_WRITE_SUPPORT
    if((_u4FileCnt[u4InstID]%10) == 0)
    #endif
    {
        #ifndef IDE_WRITE_SUPPORT
        vVDecOutputDebugString("//Pic count to [%d]\n", _u4FileCnt[u4InstID]);  
        #endif        
    }
    #endif

    #ifdef REDEC    
       if(_u4ReDecCnt[u4InstID] == 0)
    #endif    
          _u4FileCnt[u4InstID] ++;

    #if 0
    if((fgCompare) &&
       ((fgDecErr == TRUE) || (_fgVDecErr[u4InstID] == TRUE)))

    {
        UTIL_Printf(" Error ==> Pic count to [%d] \n", _u4FileCnt[u4InstID] - 1);  
    }
    #endif

    UTIL_Printf("//compare done, file cnt:%d\n", _u4FileCnt[u4InstID]);

    #ifndef INTERGRATION_WITH_DEMUX
    // Check if still pic needed compare
    if((_u4FileCnt[u4InstID] >= _u4StartCompPicNum[u4InstID]) && 
       ( _u4FileCnt[u4InstID] <= _u4EndCompPicNum[u4InstID]) &&
       (fgCompare))
    {
        #if (defined(COMP_HW_CHKSUM) && (!defined(DOWN_SCALE_SUPPORT)))
        _tTempFileInfo[u4InstID].fgGetFileInfo = TRUE;  
        _tTempFileInfo[u4InstID].pucTargetAddr = _pucDumpYBuf[u4InstID];
        _tTempFileInfo[u4InstID].u4TargetSz = GOLD_Y_SZ;  
        _tTempFileInfo[u4InstID].u4FileLength = 0; 
        vConcateStr(_bFileStr1[u4InstID][3], _bFileStr1[u4InstID][0], "_chksum.bin\0", _u4FileCnt[u4InstID]);
        #ifdef IDE_READ_SUPPORT
        fgOpen = fgPureOpenIdeFile( _bTempStr1[u4InstID],"r+b", &_tTempFileInfo[u4InstID]);
        #else
        fgOpen = fgOpenFile(u4InstID, _bTempStr1[u4InstID],"r+b", &_tTempFileInfo[u4InstID]);
        HalFlushInvalidateDCache();
        #endif
        #else
        _tFBufFileInfo[u4InstID].fgGetFileInfo = FALSE;  
        _tFBufFileInfo[u4InstID].pucTargetAddr = _pucDumpYBuf[u4InstID];
        _tFBufFileInfo[u4InstID].u4TargetSz = GOLD_Y_SZ;  
        _tFBufFileInfo[u4InstID].u4FileLength = 4; 

        #if 0
        UTIL_Printf("vAVSWrData2PC, pat:%s, y:%s, cnt:%d, 4\n", 
                      _bPatternPath,
                      _bFileAddStrY,
                      _u4FileCnt[u4InstID]);  
        #endif
        
        vConcateStr(_bFileStr1[u4InstID][3], _bPatternPath, _bFileAddStrY, _u4FileCnt[u4InstID]);

        #if 0
        UTIL_Printf("vAVSWrData2PC, _bFileStr1:%s, 4\n", 
          _bFileStr1[u4InstID][3]);
        #endif
        
        #ifdef IDE_READ_SUPPORT
        fgOpen = fgPureOpenIdeFile( _bFileStr1[u4InstID][3],"r+b", &_tFBufFileInfo[u4InstID]);
        #else
        fgOpen = fgOpenFile(u4InstID, _bFileStr1[u4InstID][3],"r+b", &_tFBufFileInfo[u4InstID]);
        HalFlushInvalidateDCache();
        #endif
        #endif

        if ((fgOpen == FALSE) || (fgDecErr == TRUE) || (_fgVDecErr[u4InstID] == TRUE))
        {
            *fgNextFrameExist = FALSE;
            
            UTIL_Printf("\n\n");
/*            
            if(fgOpen == FALSE)
            {
                UTIL_Printf("//Finish[OK] ==> Pic count to [%d] , file ==> %s\n", _u4FileCnt[u4InstID] - 1, _bFileStr1[u4InstID][1]);
                if(_u4FileCnt[u4InstID] == 1)
                {
                    if(fgOpen == FALSE)
                    {
            else
            { 
                UTIL_Printf("//fgDecErr:%d, _fgVDecErr[u4InstID]:%d\n", fgDecErr, _fgVDecErr[u4InstID]);   
                UTIL_Printf("//Finish[NG] ==> Pic count to [%d] , file ==> %s\n", _u4FileCnt[u4InstID] - 1, _bFileStr1[u4InstID][1]);                
            }
*/
            if((fgDecErr == TRUE) ||(_fgVDecErr[u4InstID] == TRUE))
            {
                UTIL_Printf("//fgDecErr:%d, _fgVDecErr[u4InstID]:%d\n", fgDecErr, _fgVDecErr[u4InstID]);   
                UTIL_Printf("//Finish[NG] ==> Pic count to [%d] , file ==> %s\n", _u4FileCnt[u4InstID] - 1, _bFileStr1[u4InstID][1]);				 
            }
            else
            {
                UTIL_Printf("//Finish[OK] ==> Pic count to [%d] , file ==> %s\n", _u4FileCnt[u4InstID] - 1, _bFileStr1[u4InstID][1]);
                if(_u4FileCnt[u4InstID] == 1)
                {
                    if(fgOpen == FALSE)
                    {
                        vVDecOutputDebugString("real NULL\n");
                    }
                }
            }

            _u4VerBitCount[u4InstID] = 0xffffffff;            
        } 
        else
        {
            UTIL_Printf("//Pic [%d] ==> OK\n", _u4FileCnt[u4InstID] - 1);  
        }            
    }//~fgCompare   
    #endif    

    if(_u4FileCnt[u4InstID] >= _u4EndCompPicNum[u4InstID])
    {
        _u4VerBitCount[u4InstID] = 0xffffffff;
    }

}

void vVerAVSVDecEnd(UINT32 u4InstID)
{
  VDEC_INFO_AVS_VFIFO_PRM_T rAvsVFifoInitPrm;
  VDEC_INFO_AVS_BS_INIT_PRM_T rAvsBSInitPrm;
  UINT32 u4VldByte,u4VldBit;
  BOOL   fgNextFrameExist = TRUE;
  VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = &(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm);
  VDEC_INFO_AVS_SEQ_HDR_T *prVDecAvsSeqHdr = &_rVDecAvsSeqHdr[u4InstID];
  VDEC_INFO_AVS_PIC_HDR_T *prPicHdr = prVDecAvsDecPrm->prPicHdr;   
      
  u4VldByte = u4VDEC_HAL_AVS_ReadRdPtr(0, u4InstID, (UINT32)_pucVFifo[u4InstID], &u4VldBit);

  #if VDEC_TEST_LOG
  {    
  UINT32 u4CheckSum = 0 ;
  
  #if 0//VDEC_PRINT_FRAME_CHECK_SUM
  u4CheckSum = u4DecDebugChecksum((UINT32)_pucVFifo[u4InstID], 
                                  (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ, 
                                  (UINT32)_pucVFifo[u4InstID] + _u4FrameStart, 
                                  u4VldByte);
  #endif
  
  UTIL_Printf("//[AVS%d] vVerAVSVDecEnd, RdPtr:0x%x, CheckSum:0x%x\n", 
      u4InstID, 
      u4VldByte,
      u4CheckSum);
  }
  #endif

  //UTIL_Printf("[tutuyc1] frame %d 0x%x 0x%x\n", (_u4AvsResetHWCnt[u4InstID]-1), _u4AvsVldPtrBeforeTrig[u4InstID], u4VldByte);


  if (0 == (_u4FileCnt[u4InstID] & 0x3F))
  {
      UTIL_Printf("//[AVS%d] vVerAVSVDecEnd, RdPtr:0x%x, Cnt:%d\n", u4InstID, u4VldByte, _u4FileCnt[u4InstID]);
  }

#if DEBUG_DUMP_REG_ENABLE
    vVdecAvsDumpReg(u4InstID, 0);
#endif

#if AVS_CRC_GENERATE
//dump AVS CRC.bin
    {
    UINT32 u4CRC[8];

    u4CRC[0] = u4VDecReadCRC(u4InstID, VDEC_CRC_Y_CHKSUM0);
    u4CRC[1] = u4VDecReadCRC(u4InstID, VDEC_CRC_Y_CHKSUM1);
    u4CRC[2] = u4VDecReadCRC(u4InstID, VDEC_CRC_Y_CHKSUM2);
    u4CRC[3] = u4VDecReadCRC(u4InstID, VDEC_CRC_Y_CHKSUM3);
    u4CRC[4] = u4VDecReadCRC(u4InstID, VDEC_CRC_C_CHKSUM0);
    u4CRC[5] = u4VDecReadCRC(u4InstID, VDEC_CRC_C_CHKSUM1);
    u4CRC[6] = u4VDecReadCRC(u4InstID, VDEC_CRC_C_CHKSUM2);
    u4CRC[7] = u4VDecReadCRC(u4InstID, VDEC_CRC_C_CHKSUM3);
    
    //UTIL_Printf("u4CRC[6]: 0x%x, u4CRC[7]: 0x%x\n", u4CRC[6], u4CRC[7]);
    
    fgAVSPlusDumpMem(u4InstID, (UCHAR*)(&u4CRC[0]), 32, _u4Bitstream_Num[u4InstID], CRC_GEN);
    }
#endif

#if AVS_CRC_COMPARE_SUPORT
    vAVS_CRCGoldenLoad(u4InstID);
    vAVS_CheckCRCResult(u4InstID, &fgNextFrameExist);
#else

   if ( prPicHdr->u4PicStruct == FRM_PIC || prPicHdr->fgSecField)
   {
#if !VDEC_AVS_ERR_TEST 
        #if AVS_TEST_HD
        vAVSWrData2PC(u4InstID, _pucDumpYBuf[u4InstID], ((((prVDecAvsSeqHdr->u2WidthDec+ 63) >> 6) * ((prVDecAvsSeqHdr->u2HeightDec + 31) >> 5)) << 9),  &fgNextFrameExist);
        #else
        vAVSWrData2PC(u4InstID, _pucDumpYBuf[u4InstID], ((((prVDecAvsSeqHdr->u2WidthDec+ 15) >> 4) * ((prVDecAvsSeqHdr->u2HeightDec + 31) >> 5)) << 9),  &fgNextFrameExist);
        #endif
#else  
       _u4FileCnt[u4InstID]++;

       if ((_tInFileInfo[u4InstID].u4FileLength - u4VldByte <128) || //1024->128, avdoid miss sequence of small size
           ( _u4FileCnt[u4InstID] >= _u4EndCompPicNum[u4InstID])
           #if DTV_COMMON_CONFIG
           || (_tInFileInfo[u4InstID].u4FileLength <= u4VldByte)
           #endif
           )
       {
           UTIL_Printf("//AVS ERR TEST: OK: PicCnt = %d\n", _u4FileCnt[u4InstID]-1);
           _u4VerBitCount[u4InstID] = 0xffffffff;
           fgNextFrameExist = FALSE;
       }
#endif
   }
#endif

#if 0//ERR_DUMP    
  //if (_u4FileCnt[u4InstID] == _u4FrameNumWatch))
  {
      UTIL_Printf("dump register, file count:%d \n", _u4FileCnt[u4InstID]);
      
      vVDEC_HAL_AVS_VDec_DumpReg(u4InstID, FALSE);
      
      #if VDEC_ERR_STOP
      while(_fgForceWait);
      #endif
  }
#endif

#if DUMP_DRAM_BUFFER
    if( prPicHdr->u4PicStruct == FRM_PIC || prPicHdr->fgSecField)
     {
        //if(fgNextFrameExist == FALSE)
        {
            UCHAR* ucYAddr;
            UCHAR* ucCAddr;
            UINT32 u4PicSize;
            
            ucYAddr = (UCHAR *)prVDecAvsDecPrm->prCurrFBufInfo->u4YAddr;
#if AVS_SUPPORT_YC_SEPERATE
            ucCAddr = (UCHAR *)prVDecAvsDecPrm->prCurrFBufInfo->u4CAddr;
#else
            ucCAddr = ucYAddr + prVDecAvsDecPrm->prCurrFBufInfo->u4CAddrOffset;
#endif
            u4PicSize = (prVDecAvsSeqHdr->u2WidthDec) * (prVDecAvsSeqHdr->u2HeightDec );
            
            UTIL_Printf("//Start to Dump Current Frame, ucYAdd: 0x%x,  ucCAdrr: 0x%x\n", ucYAddr, ucCAddr);
            UTIL_Printf("//u4With: %d,  u4Height: %d\n", prVDecAvsSeqHdr->u2WidthDec, prVDecAvsSeqHdr->u2HeightDec );
            fgAVSPlusDumpMem(u4InstID, ucYAddr, u4PicSize, _u4FileCnt[u4InstID]-1, DP_CUR_YBUFFER);
            HalFlushInvalidateDCache();
            x_thread_delay(200);
            fgAVSPlusDumpMem(u4InstID, ucCAddr, u4PicSize>>1, _u4FileCnt[u4InstID]-1, DP_CUR_CBUFFER);
            HalFlushInvalidateDCache();
        }
    }
#endif

  // reset HW
#ifdef REDEC   
  if(_u4ReDecCnt[u4InstID] > 0)
  {
    rAvsVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rAvsVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    i4VDEC_HAL_AVS_InitVDecHW(u4InstID,&rAvsVFifoInitPrm);
    rAvsBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    rAvsBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rAvsBSInitPrm.u4VLDRdPtr =  (UINT32)_pucVFifo[u4InstID] + _u4AVSByteCount[u4InstID];
    rAvsBSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    i4VDEC_HAL_AVS_InitBarrelShifter(0, u4InstID, &rAvsBSInitPrm);  
    _tVerDec[u4InstID].ucState = DEC_NORM_WAIT_TO_DEC;
    return;
  }
#endif

  if (fgNextFrameExist)
  {
    _u4AVSByteCount[u4InstID] = u4VldByte;
#if DEBUG_DUMP_REG_ENABLE
    UTIL_Printf("//Start to ResetHW, Frame/Field [%d]\n", _u4AvsResetHWCnt[u4InstID]);
#endif
    _u4AvsResetHWCnt[u4InstID]++;
    rAvsVFifoInitPrm.u4VFifoSa = (UINT32)_pucVFifo[u4InstID];
    rAvsVFifoInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    i4VDEC_HAL_AVS_InitVDecHW(u4InstID,&rAvsVFifoInitPrm);
    rAvsBSInitPrm.u4VFifoSa =  (UINT32)_pucVFifo[u4InstID];
    rAvsBSInitPrm.u4VFifoEa = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
    rAvsBSInitPrm.u4VLDRdPtr =  (UINT32)_pucVFifo[u4InstID] + _u4AVSByteCount[u4InstID];
#ifndef RING_VFIFO_SUPPORT
    rAvsBSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + V_FIFO_SZ;
#else
    rAvsBSInitPrm.u4VLDWrPtr = (UINT32)_pucVFifo[u4InstID] + ((_u4LoadBitstreamCnt[u4InstID]%2)?(V_FIFO_SZ):(V_FIFO_SZ>>1));
#endif
    i4VDEC_HAL_AVS_InitBarrelShifter(_u4BSID[u4InstID], u4InstID, &rAvsBSInitPrm);

#ifndef INTERGRATION_WITH_DEMUX
#ifdef  RING_VFIFO_SUPPORT
    if((_u4LoadBitstreamCnt[u4InstID]&0x1) && (rAvsBSInitPrm.u4VLDRdPtr > ((UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ/2))))
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

#ifdef  SATA_HDD_READ_SUPPORT
        if(!fgOpenHDDFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]))
        {
            //fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
            UTIL_Printf("//Read File Fail\n");
        }
#else
        fgOpenFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
#endif
        HalFlushInvalidateDCache();
        _u4LoadBitstreamCnt[u4InstID]++;
    }
    else if((!(_u4LoadBitstreamCnt[u4InstID]&0x1)) && (rAvsBSInitPrm.u4VLDRdPtr <((UINT32)_pucVFifo[u4InstID] + (V_FIFO_SZ/2))))
    {
        _tInFileInfo[u4InstID].fgGetFileInfo = TRUE;
        _tInFileInfo[u4InstID].pucTargetAddr = _pucVFifo[u4InstID] + (V_FIFO_SZ/2);
        _tInFileInfo[u4InstID].u4FileOffset =  ((V_FIFO_SZ * (_u4LoadBitstreamCnt[u4InstID]+ 1)) /2);
        _tInFileInfo[u4InstID].u4TargetSz = (V_FIFO_SZ/2);
        _tInFileInfo[u4InstID].u4FileLength = 0;
        
#ifdef SEMI_RING_FIFO
        _tInFileInfo[u4InstID].u4BSLenOffset = 0;
        _tInFileInfo[u4InstID].ucVFIFOEnough = 1;
#endif

#ifdef  SATA_HDD_READ_SUPPORT
        if(!fgOpenHDDFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]))
        {
            //fgOpenPCFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
            UTIL_Printf("//Read File Fail\n");
        }
#else
        fgOpenFile(u4InstID, _bFileStr1[u4InstID][1],"r+b", &_tInFileInfo[u4InstID]);
#endif
        HalFlushInvalidateDCache();
        _u4LoadBitstreamCnt[u4InstID]++;
    }
#endif
#endif

    
  }

  _tVerDec[u4InstID].ucState = DEC_NORM_VPARSER;

}



// *********************************************************************
// Function    : BOOL fgIsVP6VDecComplete(UINT32 u4InstID)
// Description : Check if VDec complete with interrupt
// Parameter   : None
// Return      : None
// *********************************************************************
BOOL fgIsAVSVDecComplete(UINT32 u4InstID)
{
  UINT32 u4MbX;
  UINT32 u4MbY;   
  VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = &(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm);
  VDEC_INFO_AVS_SEQ_HDR_T *prSeqHdr = prVDecAvsDecPrm->prSeqHdr;
  UINT32 u4MbH;

  #if AVS_TEST_HD
      u4MbH =  (prSeqHdr->u2HeightDec / 32 -1);
  #else
  if (prSeqHdr->u4IsProgSeq)
     u4MbH =  (prSeqHdr->u2HeightDec / 16 -1);
  else
     u4MbH =  (prSeqHdr->u2HeightDec / 32 -1);
  #endif
  
  if(_fgVDecComplete[u4InstID])
  {
      vVDEC_HAL_AVS_GetMbxMby(u4InstID, &u4MbX, &u4MbY);
      
      #if AVS_TEST_HD
      if( (u4MbX < (prSeqHdr->u2WidthDec / 64 - 1) ) || (u4MbY < u4MbH))
      #else
      if( (u4MbX < (prSeqHdr->u2WidthDec / 16 - 1) ) || (u4MbY < u4MbH))
      #endif
      {
        return FALSE;
      }
      else
      {
        return TRUE;
      }    
  }
  return FALSE;
}

#if DEBUG_PARSE_LOG_ENABLE
extern UINT32 u4VDecReadAVSVLD(UINT32 u4VDecID, UINT32 u4Addr);
extern UINT32 u4VDecReadAVSMV(UINT32 u4VDecID, UINT32 u4Addr);
#endif
void vVerAVSDecEndProc(UINT32 u4InstID)
{
  UINT32 u4Cnt;
  UINT32 u4CntTimeChk;
  UINT32 u4MbX;
  UINT32 u4MbY;  
#if !VDEC_AVS_ERR_TEST     
  UCHAR strMessage[256];
#endif
  UINT32 u4MbX_last;
  UINT32 u4MbY_last;
  UINT32 u4AvsErrType = 0; 
  VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = &(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm);
  VDEC_INFO_AVS_SEQ_HDR_T *prSeqHdr = prVDecAvsDecPrm->prSeqHdr; 
  VDEC_INFO_AVS_PIC_HDR_T *prPicHdr = prVDecAvsDecPrm->prPicHdr; 
  VDEC_INFO_AVS_ERR_INFO_T rAvsErrInfo;
  
  rAvsErrInfo.u2AvsMBErrCnt = 0;
  rAvsErrInfo.u4AvsErrCnt = 0;
  rAvsErrInfo.u4AvsErrRow = 0;
  rAvsErrInfo.u4AvsErrType = 0;   
  u4Cnt=0;
  u4CntTimeChk = 0;
  _fgVDecErr[u4InstID] = FALSE;
  
   while(u4CntTimeChk < DEC_RETRY_NUM)
   {    
      u4Cnt ++;    
      if((u4Cnt & 0x3f)== 0x3f)
      {
  #ifndef IRQ_DISABLE    
  #else
        #if 0
        if( u4VDEC_HAL_AVS_VDec_ReadFinishFlag(u4InstID) & 0x1)
        #else
        //MISC_41[16] to judge decode finish
        if( u4VDEC_HAL_AVS_VDec_ReadFinishFlag(u4InstID) & 0x10000)
        #endif
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
        if(fgIsAVSVDecComplete(u4InstID))
        {
          u4CntTimeChk = 0;
          break;
        }
        else
        {
          u4MbX_last = u4MbX;
          u4MbY_last = u4MbY;
          vVDEC_HAL_AVS_GetMbxMby(u4InstID, &u4MbX, &u4MbY);
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
    HAL_GetDeltaTime(&rTimeDecDuration[u4InstID],&rTimeStartDec[u4InstID],&rTimeEndDec[u4InstID]);
    UTIL_Printf("AVS_Delta_%d:%08d s, %08d us\n",u4InstID,rTimeDecDuration[u4InstID].u4Seconds,rTimeDecDuration[u4InstID].u4Micros);
#endif    

#if AVS_ESA_MEASURE
    vVdecMeasureAVSESA(u4InstID);
#endif
#if DTV_COMMON_CONFIG
    u4AvsErrType = u4VDEC_HAL_AVS_GetErrType(u4InstID, &rAvsErrInfo);

    if (u4CntTimeChk == DEC_RETRY_NUM)
    {
        UTIL_Printf("//AVS ERR Bitstream Timeout, H:0x%x, W:0x%x\n", prSeqHdr->u2HeightDec, prSeqHdr->u2WidthDec);
    }

    if ((u4CntTimeChk == DEC_RETRY_NUM) && (u4AvsErrType>>31))
    {
        UTIL_Printf("//AVS ERR Bitstream TimeOut!!!!\n");
    }
#endif

    //Acquire Error Information
    vVDEC_HAL_AVS_GetErrInfo(u4InstID, &rAvsErrInfo);

#if !VDEC_AVS_ERR_TEST
    if((u4CntTimeChk == DEC_RETRY_NUM)  || (rAvsErrInfo.u4AvsErrCnt != 0))
    {
    #ifndef INTERGRATION_WITH_DEMUX
    //#ifdef EXT_COMPARE     
      _fgVDecErr[u4InstID] = TRUE;
    //#endif
      if(u4CntTimeChk == DEC_RETRY_NUM)
      {
        vVDecOutputDebugString("\n//!!!!!!!!! Decoding Timeout !!!!!!!\n");
        sprintf(strMessage, "%s", "\n//!!!!!!!!! Decoding Timeout !!!!!!!\n");
        fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
        //vDumpReg();
#if DEBUG_DUMP_REG_ENABLE
        //vVdecAvsDumpReg(u4InstID, 0);
#endif
      }

      if(rAvsErrInfo.u4AvsErrCnt != 0)
      {
          UTIL_Printf("//AVS Error Bitstream, u4AvsErrCnt: 0x%8x, u4TFT: %d\n", rAvsErrInfo.u4AvsErrCnt, prPicHdr->u4TFT);
      }

      vVDEC_HAL_AVS_GetMbxMby(u4InstID, &u4MbX, &u4MbY);
      vVDecOutputDebugString("\n//!!!!!!!!! Decoding Error 0x%.8x!!!!!!!\n", rAvsErrInfo.u4AvsErrType);
      sprintf(strMessage,"\n//!!!!!!!!! Decoding Error 0x%.8x 0x%.8x 0x%.8xat MC (x,y)=(%d/%d, %d/%d)  !!!!!!!\n", u4AvsErrType, 
                rAvsErrInfo.u4AvsErrType,rAvsErrInfo.u4AvsErrRow,u4MbX, prSeqHdr->u4HSize -1, u4MbY, prSeqHdr->u4VSize -1);

      UTIL_Printf("//u4CntTimeChk:%d, u4MbX:%d, u4MbY:%d, u4AvsErrType:0x%x\n", u4CntTimeChk, u4MbX, u4MbY, rAvsErrInfo.u4AvsErrType);
      
      fgWrMsg2PC((void*)strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);

      
    #endif
    }
#else
#if 0 // already log in previous code
    u4AvsErrType = u4VDEC_HAL_AVS_GetErrType(u4InstID, &rAvsErrInfo);

    if(u4CntTimeChk == DEC_RETRY_NUM)
    {
        UTIL_Printf("//AVS ERR Bitstream TimeOut!!!!\n");
    }

   if ((u4CntTimeChk == DEC_RETRY_NUM) && (u4AvsErrType>>31))
   {
       UTIL_Printf("//AVS ERR Bitstream TimeOut!!!!\n");
   }
#endif
#endif
    
    //vVerifySetVSyncPrmBufPtr(u4InstID, _u4DecBufIdx[u4InstID]);

#if PARA8_BEHAVIOR_TEST
    vVDEC_HAL_PARA8_BEHAVIOR_DumpReg(u4InstID);
#endif

    vVerAVSVDecEnd(u4InstID);
    if ( prPicHdr->u4PicStruct == FRM_PIC || prPicHdr->fgSecField)
    {
       vVerAVSUpdateBufStatus(u4InstID);
    }    

    if ((!prSeqHdr->u4IsProgSeq) && (!prPicHdr->u4ProgFrm))
    {
        prPicHdr->fgSecField = (!prPicHdr->fgSecField);
    }
}

#if AVS_CRC_COMPARE_SUPORT
//Load AVS CRC.bin
void vAVS_CRCGoldenLoad(UINT32 u4InstID)
{
    UINT32 i, j;
    CHAR _bPatternPath [256];
    CHAR _bCRCFileName[256] = {"\\CRC.bin\0"};	 
    CHAR _bPathAddStr[256] = {"pattern\\\0"};
    CHAR _bPP_PathStr[256] = {"\\pp_pat\\pp_pat\0"};
    CHAR _bFileName [256];
    UINT32 u4Temp;
    //Searh path name
    UINT32 path_byte_addr = 0;
    UINT32 filename_byte_addr = 0;
	VDEC_INFO_AVS_DEC_PRM_T* prVDecAvsDecPrm = &(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm);
	VDEC_INFO_AVS_PIC_HDR_T *prPicHdr = prVDecAvsDecPrm->prPicHdr;	 
    
    //Load CRC.bin
    if(fgAVSCRCLoaded[u4InstID] == FALSE)
    {
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
            else if (_bFileStr1[u4InstID][1][i] == '.')
            {
                if( (_bFileStr1[u4InstID][1][i+1] == 'a' || _bFileStr1[u4InstID][1][i+1] == 'A')
                    && (_bFileStr1[u4InstID][1][i+2] == 'v' || _bFileStr1[u4InstID][1][i+2] == 'V')
                    && (_bFileStr1[u4InstID][1][i+3] == 's' || _bFileStr1[u4InstID][1][i+3] == 'S'))
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
        u4Temp += sprintf(_bPatternPath+path_byte_addr+u4Temp, "%s", _bPP_PathStr);    
        u4Temp += sprintf(_bPatternPath+path_byte_addr+u4Temp, "%s", _bCRCFileName);	 
        _bPatternPath[path_byte_addr+u4Temp] = '\0';
        
        u4Temp = sprintf((char*)_bFileStr1[u4InstID][3], "%s", (char*)_bPatternPath);
        
        _tFBufFileInfo[u4InstID].fgGetFileInfo = TRUE;	
        _tFBufFileInfo[u4InstID].pucTargetAddr = _pucMergeCRCBuf[u4InstID];
        _tFBufFileInfo[u4InstID].u4TargetSz = MERGE_CRC_BUF_SZ;  
        _tFBufFileInfo[u4InstID].u4FileLength = 0;	
        		
        fgOpenFile(u4InstID, _bFileStr1[u4InstID][3],"r+b", &_tFBufFileInfo[u4InstID]); 			  
        HalFlushInvalidateDCache();

        if(prPicHdr->u4PicStruct == FRM_PIC)
        {
            _u4PicTotalCnt[u4InstID] = (_tFBufFileInfo[u4InstID].u4RealGetBytes >> 5);
        }
        else
        {
            _u4PicTotalCnt[u4InstID] = (_tFBufFileInfo[u4InstID].u4RealGetBytes >> 6);
        }
        UTIL_Printf("//[AVS]CRC file %d Bytes, _u4PicTotalCnt %d\n", _tFBufFileInfo[u4InstID].u4RealGetBytes, _u4PicTotalCnt[u4InstID]);
        fgAVSCRCLoaded[u4InstID] = TRUE;
    }
}

void vAVS_CheckCRCResult(UINT32 u4InstID, BOOL* fgNextFrameExist)
{
    //read
    //REG_2: y_crc_checksum[31:0]
    //REG_3: y_crc_checksum[63:32]
    //REG_4: y_crc_checksum[95:64]
    //REG_5: y_crc_checksum[127:96]
    
    //REG_6: c_crc_checksum[31:0]
    //REG_7: c_crc_checksum[63:32]
    //REG_8: c_crc_checksum[95:64]
    //REG_9: c_crc_checksum[127:96]
    
    UINT32 u4HWCRC_Y0, u4HWCRC_Y1, u4HWCRC_Y2, u4HWCRC_Y3;
    UINT32 u4HWCRC_C0, u4HWCRC_C1, u4HWCRC_C2, u4HWCRC_C3;
    UINT32 u4CRCValueY0, u4CRCValueY1, u4CRCValueY2, u4CRCValueY3;
    UINT32 u4CRCValueC0, u4CRCValueC1, u4CRCValueC2, u4CRCValueC3;
    UINT32 u4CRCTmp3, u4CRCTmp2, u4CRCTmp1, u4CRCTmp0;
    BOOL fgDecErr = FALSE;
    BOOL fgOpen;
    UCHAR strMessage [256];
    BOOL fgCompare = FALSE;
    UCHAR *pucMergeCRCBuf = _pucMergeCRCBuf[u4InstID];
    UCHAR *pu4CRCResultBufAddr = (UCHAR *)(pucMergeCRCBuf + 32*_u4FileCnt[u4InstID]);

    VDEC_INFO_AVS_DEC_PRM_T *prVDecAvsDecPrm = &(_tVerMpvDecPrm[u4InstID].SpecDecPrm.rVDecAVSDecPrm);
    VDEC_INFO_AVS_PIC_HDR_T *prPicHdr = prVDecAvsDecPrm->prPicHdr;	 
    
    u4CRCTmp3 = *(pu4CRCResultBufAddr + 3);
    u4CRCTmp2 = *(pu4CRCResultBufAddr + 2);
    u4CRCTmp1 = *(pu4CRCResultBufAddr + 1);
    u4CRCTmp0 = *(pu4CRCResultBufAddr + 0);
    u4CRCValueY0 = (u4CRCTmp3 << 24) | (u4CRCTmp2 << 16) |	(u4CRCTmp1 << 8) |	(u4CRCTmp0 << 0) ;
    
    u4CRCTmp3 = *(pu4CRCResultBufAddr + 7);
    u4CRCTmp2 = *(pu4CRCResultBufAddr + 6);
    u4CRCTmp1 = *(pu4CRCResultBufAddr + 5);
    u4CRCTmp0 = *(pu4CRCResultBufAddr + 4);
    u4CRCValueY1 = (u4CRCTmp3 << 24) | (u4CRCTmp2 << 16) |	(u4CRCTmp1 << 8) |	(u4CRCTmp0 << 0) ;
    
    u4CRCTmp3 = *(pu4CRCResultBufAddr + 11);
    u4CRCTmp2 = *(pu4CRCResultBufAddr + 10);
    u4CRCTmp1 = *(pu4CRCResultBufAddr + 9);
    u4CRCTmp0 = *(pu4CRCResultBufAddr + 8);
    u4CRCValueY2 = (u4CRCTmp3 << 24) | (u4CRCTmp2 << 16) |	(u4CRCTmp1 << 8) |	(u4CRCTmp0 << 0) ;
    
    u4CRCTmp3 = *(pu4CRCResultBufAddr + 15);
    u4CRCTmp2 = *(pu4CRCResultBufAddr + 14);
    u4CRCTmp1 = *(pu4CRCResultBufAddr + 13);
    u4CRCTmp0 = *(pu4CRCResultBufAddr + 12);
    u4CRCValueY3 = (u4CRCTmp3 << 24) | (u4CRCTmp2 << 16) |	(u4CRCTmp1 << 8) |	(u4CRCTmp0 << 0) ;
    
    u4CRCTmp3 = *(pu4CRCResultBufAddr + 19);
    u4CRCTmp2 = *(pu4CRCResultBufAddr + 18);
    u4CRCTmp1 = *(pu4CRCResultBufAddr + 17);
    u4CRCTmp0 = *(pu4CRCResultBufAddr + 16);
    u4CRCValueC0 = (u4CRCTmp3 << 24) | (u4CRCTmp2 << 16) |	(u4CRCTmp1 << 8) |	(u4CRCTmp0 << 0) ;
    
    u4CRCTmp3 = *(pu4CRCResultBufAddr + 23);
    u4CRCTmp2 = *(pu4CRCResultBufAddr + 22);
    u4CRCTmp1 = *(pu4CRCResultBufAddr + 21);
    u4CRCTmp0 = *(pu4CRCResultBufAddr + 20);
    u4CRCValueC1 = (u4CRCTmp3 << 24) | (u4CRCTmp2 << 16) |	(u4CRCTmp1 << 8) |	(u4CRCTmp0 << 0) ;
    
    u4CRCTmp3 = *(pu4CRCResultBufAddr + 27);
    u4CRCTmp2 = *(pu4CRCResultBufAddr + 26);
    u4CRCTmp1 = *(pu4CRCResultBufAddr + 25);
    u4CRCTmp0 = *(pu4CRCResultBufAddr + 24);
    u4CRCValueC2 = (u4CRCTmp3 << 24) | (u4CRCTmp2 << 16) |	(u4CRCTmp1 << 8) |	(u4CRCTmp0 << 0) ;
    
    u4CRCTmp3 = *(pu4CRCResultBufAddr + 31);
    u4CRCTmp2 = *(pu4CRCResultBufAddr + 30);
    u4CRCTmp1 = *(pu4CRCResultBufAddr + 29);
    u4CRCTmp0 = *(pu4CRCResultBufAddr + 28);
    u4CRCValueC3 = (u4CRCTmp3 << 24) | (u4CRCTmp2 << 16) |	(u4CRCTmp1 << 8) |	(u4CRCTmp0 << 0) ;
    
    //UTIL_Printf("u4CRCValueY0 is 0x%lx, u4CRCValueC3 is 0x%lx \n", u4CRCValueY0, u4CRCValueC3);
    	
    u4HWCRC_Y0 = u4VDecReadCRC(u4InstID, VDEC_CRC_Y_CHKSUM0);
    u4HWCRC_Y1 = u4VDecReadCRC(u4InstID, VDEC_CRC_Y_CHKSUM1);
    u4HWCRC_Y2 = u4VDecReadCRC(u4InstID, VDEC_CRC_Y_CHKSUM2);
    u4HWCRC_Y3 = u4VDecReadCRC(u4InstID, VDEC_CRC_Y_CHKSUM3);
    
    u4HWCRC_C0 = u4VDecReadCRC(u4InstID, VDEC_CRC_C_CHKSUM0);
    u4HWCRC_C1 = u4VDecReadCRC(u4InstID, VDEC_CRC_C_CHKSUM1);
    u4HWCRC_C2 = u4VDecReadCRC(u4InstID, VDEC_CRC_C_CHKSUM2);
    u4HWCRC_C3 = u4VDecReadCRC(u4InstID, VDEC_CRC_C_CHKSUM3);

    if((_u4AvsPicCnt[u4InstID] >= _u4StartCompPicNum[u4InstID]) 
       && ( _u4AvsPicCnt[u4InstID] <= _u4EndCompPicNum[u4InstID]))
    {
        fgCompare = TRUE;
    }
    
    if (TRUE == fgCompare)
    {
        if( (u4HWCRC_Y0 != u4CRCValueY0) 
         || (u4HWCRC_Y1 != u4CRCValueY1)
         || (u4HWCRC_Y2 != u4CRCValueY2)
         || (u4HWCRC_Y3 != u4CRCValueY3)
         || (u4HWCRC_C0 != u4CRCValueC0)
         || (u4HWCRC_C1 != u4CRCValueC1)
         || (u4HWCRC_C2 != u4CRCValueC2)
         || (u4HWCRC_C3 != u4CRCValueC3)
        )
        {
            fgDecErr = TRUE;
            UTIL_Printf("//VDEC_CRC_REG_EN register is 0x%lx\n",u4VDecReadCRC(u4InstID, VDEC_CRC_REG_EN));
            UTIL_Printf("//u4CRCValueY0 is 0x%lx, u4CRCValueY1 is 0x%lx, u4CRCValueY2 is 0x%lx, u4CRCValueY3 is 0x%lx, u4CRCValueC0 is 0x%lx, u4CRCValueC1 is 0x%lx, u4CRCValueC2 is 0x%lx, u4CRCValueC3 is 0x%lx \n", \
                        u4CRCValueY0, u4CRCValueY1,u4CRCValueY2, u4CRCValueY3, u4CRCValueC0, u4CRCValueC1, u4CRCValueC2, u4CRCValueC3);
            UTIL_Printf("//u4HWCRC_Y0 is 0x%lx, u4HWCRC_Y1 is 0x%lx, u4HWCRC_Y2 is 0x%lx, u4HWCRC_Y3 is 0x%lx, u4HWCRC_C0 is 0x%lx, u4HWCRC_C1 is 0x%lx, u4HWCRC_C2 is 0x%lx, u4HWCRC_C3 is 0x%lx \n", \
                        u4HWCRC_Y0, u4HWCRC_Y1, u4HWCRC_Y2, u4HWCRC_Y3, u4HWCRC_C0, u4HWCRC_C1, u4HWCRC_C2, u4HWCRC_C3);
            
            sprintf(strMessage," 3 Error ==> Pic count to [%d] \n", _u4AvsPicCnt[u4InstID]);	
            UTIL_Printf("//[VDEC]Error ==> Pic count to [%d] \n", _u4AvsPicCnt[u4InstID]);
        }
        else
        {
            if(prPicHdr->u4PicStruct == FRM_PIC || prPicHdr->fgSecField)
            {
                UTIL_Printf("//Pic[%d] CRC Compare OK\n", _u4AvsPicCnt[u4InstID]);
            }
        }
    }

    _u4FileCnt[u4InstID]++;
    if(prPicHdr->u4PicStruct == FRM_PIC || prPicHdr->fgSecField)    
    {
        _u4AvsPicCnt[u4InstID]++;
    }

    if (_u4PicTotalCnt[u4InstID] <= _u4AvsPicCnt[u4InstID])
    {
        fgOpen = FALSE;
    }
    else
    {
        fgOpen = TRUE;
    }
    if((fgOpen == FALSE) ||(fgDecErr == TRUE))
    {
        *fgNextFrameExist = FALSE;
        sprintf(strMessage, "%s", "\n");
        fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
        //fprintf(_tFileListRecInfo.fpFile, "\n");	
        // break decode
        if(fgOpen == FALSE)
        {
            sprintf(strMessage,"//Finish[OK]==> Pic count to [%d], file ==> %s\n", _u4AvsPicCnt[u4InstID] - 1, _bFileStr1[u4InstID][1]);
            fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
            //fprintf(_tFileListRecInfo.fpFile, " Compare Finish==> Pic count to [%d] \n", _u4FileCnt[_u4VDecID] - 1);	 
            if(_u4AvsPicCnt[u4InstID] == 1)
            {
                if(fgOpen == FALSE)
                {
                    vVDecOutputDebugString("real NULL\n");
                }
            }
        }
        else
        {
            sprintf(strMessage,"//Finish[NG]==> Pic count to [%d], file ==> %s\n", _u4AvsPicCnt[u4InstID] - 1, _bFileStr1[u4InstID][1]);	   
            fgWrMsg2PC(strMessage,strlen(strMessage),8,&_tFileListRecInfo[u4InstID]);
            //fprintf(_tFileListRecInfo.fpFile, " Error ==> Pic count to [%d] \n", _u4FileCnt[_u4VDecID] - 1);		   
        }
        _u4VerBitCount[u4InstID] = 0xffffffff;
    }
    
    if(_u4AvsPicCnt[u4InstID] >= _u4EndCompPicNum[u4InstID])
    {
        _u4VerBitCount[u4InstID] = 0xffffffff;
    }	 
}
#endif
