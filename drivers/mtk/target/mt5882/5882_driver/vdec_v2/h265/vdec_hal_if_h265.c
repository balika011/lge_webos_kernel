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
#include "../hw_common/vdec_hw_common.h"
#include "../hw_common/vdec_hal_if_common.h"
#include "../hw_common/vdec_hal_errcode.h"
#include "vdec_hal_if_h265.h"
#include "vdec_hw_h265.h"
#include "x_debug.h"
//#include <string.h>


const CHAR DIAG_SCAN[16]  =
{  0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

const CHAR DIAG_SCAN8[64] =
{  0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
   12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
   35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
   58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

const UINT32 PIP_PP_ComSetting[11] = 
{
    0, 480, 960, 1232, 1368, 1436, 1556, 960, 60, 128, 162
};

#define MAX_LCU_BOUNDARY    17
const UINT32 LCU_BOUNDARY[17] = 
{
    0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,512
};

#ifdef MPV_DUMP_H265_DEC_REG
void VDec_DumpH265Reg(UCHAR ucMpvId);
#endif


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H265_InitVDecHW(UINT32 u4Handle, VDEC_INFO_H265_INIT_PRM_T *prH265VDecInitPrm);
// Description :Initialize video decoder hardware only for H265
// Parameter : u4VDecID : video decoder hardware ID
//                  prH265VDecInitPrm : pointer to VFIFO info struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H265_InitVDecHW(UINT32 u4VDecID)
{
#ifdef VDEC_SIM_DUMP
     LOG(0,"// i4VDEC_HAL_H265_InitVDecHW() start!!\n");
#endif

    vVDecResetHW(u4VDecID, VDEC_FMT_H265);

#ifdef VDEC_SIM_DUMP
    LOG(0,"// i4VDEC_HAL_H265_InitVDecHW() Done!!\n");
#endif
   
    return  VDEC_HAL_HANDLE_OK;
}


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H265_LAE_InitVDecHW(UINT32 u4Handle, VDEC_INFO_H265_INIT_PRM_T *prH265VDecInitPrm);
// Description :Initialize video decoder hardware only for H265
// Parameter : u4VDecID : video decoder hardware ID
//                  prH265VDecInitPrm : pointer to VFIFO info struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H265_LAE_InitVDecHW(UINT32 u4VDecID)
{

#ifdef VDEC_SIM_DUMP
     LOG(0,"// i4VDEC_HAL_H265_LAE_InitVDecHW() start!!\n");
#endif
    //lae0 ==> RW_MCORE_LAE_CORE bit24; lae1 ==> RW_MCORE_LAE_CORE bit25;
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_LAE_CORE,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_LAE_CORE) | (1 << (22 + u4VDecID)));
    vVDecResetHW(u4VDecID, VDEC_FMT_H265);
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_LAE_CORE,u4VDecReadMCORETOP(u4VDecID,RW_MCORE_LAE_CORE) & (~(1 << (22 + u4VDecID))));
#ifdef VDEC_SIM_DUMP
    LOG(0,"// i4VDEC_HAL_H265_LAE_InitVDecHW() Done!!\n");
#endif
   
    return  VDEC_HAL_HANDLE_OK;
}


// **************************************************************************
// Function : INT32 vVDEC_HAL_H265_SetLAE0DecAddr(UINT32 u4VDecID,UINT32 u4CABACAddr,UINT32 u4ECAddr);
// Description :Config LAE working buffer for H265
// Parameter : u4VDecID : video decoder hardware ID
//             u4CABACAddr : bitstream info and cabac info buffer
//             u4ECAddr    : error concealment info buffer
// Return      : void
// **************************************************************************
void vVDEC_HAL_H265_SetLAEDecAddr(UINT32 u4VDecID,UINT32 u4CABACAddr,UINT32 u4ECAddr)
{
    if(u4VDecID == VDEC_LAE0)
    {
        vVDecWriteMCORETOP(u4VDecID,RW_MCORE_CABAC0,u4CABACAddr);
        vVDecWriteMCORETOP(u4VDecID,RW_MCORE_EC0,u4ECAddr);
    }
    else if(u4VDecID == VDEC_LAE1)
    {
        vVDecWriteMCORETOP(u4VDecID,RW_MCORE_CABAC1,u4CABACAddr);
        vVDecWriteMCORETOP(u4VDecID,RW_MCORE_EC1,u4ECAddr);
    }
    else
    {
        ASSERT(0);
    }
}

// **************************************************************************
// Function : INT32 vVDEC_HAL_H265_SetLAECoreNumber(UINT32 u4VDecID,UINT32 u4LAECore)
// Description :Config LAE Active Core Number (For Oryx LAE + Single Core flow only)
// Parameter : u4VDecID : video decoder hardware ID            
//             u4LAECore    : Core Number
// Return      : void
// **************************************************************************
void vVDEC_HAL_H265_SetLAEDecCore(UINT32 u4VDecID,UINT32 u4LAECore)
{
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_LAE_CORE,u4LAECore);      
}

// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H265_ShiftGetBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits);
// Description :Read barrel shifter after shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4ShiftBits : shift bits number
// Return      : Value of barrel shifter input window after shifting
// **************************************************************************
UINT32 u4VDEC_HAL_H265_ShiftGetBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits)
{
    UINT32 u4RegVal;
  
    u4RegVal = u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, u4ShiftBits);
  
    return (u4RegVal);
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H265_GetBitStreamShift(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits);
// Description :Read Barrel Shifter before shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4ShiftBits : shift bits number
// Return      : Value of barrel shifter input window before shifting
// **************************************************************************
UINT32 u4VDEC_HAL_H265_GetBitStreamShift(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits)
{
    UINT32 u4RegVal0;
  
    u4RegVal0 = u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, 0);
    u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, u4ShiftBits);
    
    return (u4RegVal0);
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H265_GetRealBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits);
// Description :Read Barrel Shifter before shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4ShiftBits : shift bits number
// Return      : Most significant (32 - u4ShiftBits) bits of barrel shifter input window before shifting
// **************************************************************************
UINT32 u4VDEC_HAL_H265_GetRealBitStream(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4ShiftBits)
{
    UINT32 u4RegVal0;
  
    u4RegVal0 = u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, 0);
    u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, u4ShiftBits);
    
    return (u4RegVal0 >> (32-u4ShiftBits));
}


// **************************************************************************
// Function : UINT32 bVDEC_HAL_H265_GetBitStreamFlg(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Read Barrel Shifter before shifting
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : MSB of barrel shifter input window before shifting
// **************************************************************************
BOOL bVDEC_HAL_H265_GetBitStreamFlg(UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4RegVal;
  
    u4RegVal = u4VDEC_HAL_H265_GetBitStreamShift(u4BSID, u4VDecID, 1);
    return ((u4RegVal >> 31));
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H265_UeCodeNum(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Do UE variable length decoding
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : Input window after UE variable length decoding
// **************************************************************************
UINT32 u4VDEC_HAL_H265_UeCodeNum(UINT32 u4BSID, UINT32 u4VDecID)
{
        return (u4VDecReadHEVCVLD(u4VDecID,RO_HEVLD_UE));
}


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H265_SeCodeNum(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Do SE variable length decoding
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : Input window after SE variable length decoding
// **************************************************************************
INT32 i4VDEC_HAL_H265_SeCodeNum(UINT32 u4BSID, UINT32 u4VDecID)
{

        return ((INT32)u4VDecReadHEVCVLD(u4VDecID, RO_HEVLD_SE));
}

// *********************************************************************
// Function    : UINT32 u4VDEC_HAL_H265_GetStartCode_PicStart(UINT32 u4BSID, UINT32 u4VDecID)
// Description : Get next start code
// Parameter   : u4BSID : Barrel shifter ID
//                   u4VDecID : VLD ID
// Return      : None
// *********************************************************************
UINT32 u4VDEC_HAL_H265_GetStartCode_PicStart (UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4Temp = 0;
    UINT32 u4Temp2 =0;
    UINT32 u4NalType = 0;
    UINT32 u4RetryNum = 0x100000;
    UINT32 i;
    
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] u4VDEC_HAL_H265_GetStartCode_PicStart() start!!\n");
#endif

    do{
        //u4Temp = u4VDEC_HAL_H265_ShiftGetBitStream(u4BSID, u4VDecID, 0);
        if ((u4Temp >> 8& 0x00ffffff) != START_CODE){
           vVDecWriteHEVCVLD(u4VDecID,  RW_HEVLD_CTRL, u4VDecReadHEVCVLD(u4VDecID,RW_HEVLD_CTRL) | (HEVC_FIND_SC_CFG) );
           u4VDecReadHEVCVLD (u4VDecID, RW_HEVLD_CTRL );

           vVDecWriteHEVCVLD(u4VDecID,  HEVC_FC_TRG_REG,  HEVC_SC_START );
#ifdef VDEC_SIM_DUMP
           LOG(0,"wait(`HEVC_SC_START == 0);\n");
#endif
           for (i = 0; i < u4RetryNum; i++ ){
                if ((u4VDecReadHEVCVLD(u4VDecID,  HEVC_FC_TRG_REG) & HEVC_SC_START) == 0)
                    break;
           }
        }  
        u4Temp = u4VDEC_HAL_H265_ShiftGetBitStream(u4BSID, u4VDecID, 8);
        u4Temp = u4VDEC_HAL_H265_GetBitStreamShift(u4BSID, u4VDecID, 32);
        u4NalType = (((u4Temp & 0xffff)>>9) & 0x3f);
        u4Temp2 =  u4VDEC_HAL_H265_ShiftGetBitStream(u4BSID, u4VDecID, 0);
    } while ( (((u4Temp2>>31) & 0x01) == 0) && (u4NalType <= 21)  );   // until get "first_slice_segment_in_pic_flag" == 1

#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] Intput Window GetStartCode  = 0x%08x\n",u4Temp );
    LOG(0,"// [INFO] u4VDEC_HAL_H265_GetStartCode_PicStart() Done!!\n");
#endif


    return u4Temp;
}


// *********************************************************************
// Function    : UINT32 u4VDEC_HAL_H265_LAE_GetStartCode_PicStart(UINT32 u4BSID, UINT32 u4VDecID)
// Description : Get next start code
// Parameter   : u4BSID : Barrel shifter ID
//                   u4VDecID : VLD ID
// Return      : None
// *********************************************************************
UINT32 u4VDEC_HAL_H265_LAE_GetStartCode_PicStart (UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4Temp = 0;
    UINT32 u4Temp2 =0;
    UINT32 u4NalType = 0;
    UINT32 u4RetryNum = 0x100000;
    UINT32 i;
    UINT32 u4LAEID;
    
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] u4VDEC_HAL_H265_GetStartCode_PicStart() start!!\n");
#endif

    //u4LAEID = (u4VDecID == 0) ? VDEC_LAE0 : VDEC_LAE1;
    u4LAEID = u4VDecID;
    do{
        //u4Temp = u4VDEC_HAL_H265_ShiftGetBitStream(u4BSID, u4LAEID, 0);

        if ((u4Temp >> 8& 0x00ffffff) != START_CODE){
           vVDecWriteHEVCVLD(u4LAEID,  RW_HEVLD_CTRL, u4VDecReadHEVCVLD(u4LAEID,RW_HEVLD_CTRL) | (HEVC_FIND_SC_CFG) );
           u4VDecReadHEVCVLD (u4LAEID, RW_HEVLD_CTRL );

           vVDecWriteHEVCVLD(u4LAEID,  HEVC_FC_TRG_REG,  HEVC_SC_START );
#ifdef VDEC_SIM_DUMP
           LOG(0,"wait(`HEVC_SC_START == 0);\n");
#endif
           for (i = 0; i < u4RetryNum; i++ ){
                if ((u4VDecReadHEVCVLD(u4LAEID,  HEVC_FC_TRG_REG) & HEVC_SC_START) == 0)
                    break;
           }
        }  
        u4Temp = u4VDEC_HAL_H265_ShiftGetBitStream(u4BSID, u4LAEID, 8);
        u4Temp = u4VDEC_HAL_H265_GetBitStreamShift(u4BSID, u4LAEID, 32);
        u4NalType = (((u4Temp & 0xffff)>>9) & 0x3f);
        u4Temp2 =  u4VDEC_HAL_H265_ShiftGetBitStream(u4BSID, u4LAEID, 0);
    } while ( (((u4Temp2>>31) & 0x01) == 0) && (u4NalType <= 21)  );   // until get "first_slice_segment_in_pic_flag" == 1

#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] Intput Window GetStartCode  = 0x%08x\n",u4Temp );
    LOG(0,"// [INFO] u4VDEC_HAL_H265_GetStartCode_PicStart() Done!!\n");
#endif

    return u4Temp;
}


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H265_InitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, VDEC_INFO_H265_BS_INIT_PRM_T *prH265BSInitPrm);
// Description :Initialize barrel shifter with byte alignment
// Parameter :u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 prH265BSInitPrm : pointer to h265 initialize barrel shifter information struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H265_InitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, VDEC_INFO_H265_BS_INIT_PRM_T *prH265BSInitPrm)
{
    BOOL fgInitBSResult = FALSE,fgInitVLD = FALSE;
    INT32 i4Try = 0;
    
    #ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] i4VDEC_HAL_H265_InitBarrelShifter() start!!\n");
    #endif

    while(((fgInitBSResult = fgInitH265BarrelShift(u4VDecID, prH265BSInitPrm)) == FALSE) && (i4Try < 500))
    {
        fgInitVLD = TRUE;
        i4VDEC_HAL_H265_InitVDecHW(u4VDecID);
        i4Try ++;
    }

    if(fgInitVLD)
    {
        LOG(1,"==============Init VLD Fail ================\n");
    }
        
    if (fgInitBSResult)
    {
#ifdef VDEC_SIM_DUMP
        LOG(0,"// [INFO] i4VDEC_HAL_H265_InitBarrelShifter() Done!!\n");
#endif
        return VDEC_HAL_HANDLE_OK;
    }
    else
    {
        LOG(0,"\n// [ERROR] i4VDEC_HAL_H265_InitBarrelShifter() Fail!!!!!!!!!!!!!!\n\n");
        return INIT_BARRELSHIFTER_FAIL;
    }
}


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H265_LAE_InitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, VDEC_INFO_H265_BS_INIT_PRM_T *prH265BSInitPrm);
// Description :Initialize barrel shifter with byte alignment
// Parameter :u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 prH265BSInitPrm : pointer to h265 initialize barrel shifter information struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H265_LAE_InitBarrelShifter(UINT32 u4BSID, UINT32 u4VDecID, VDEC_INFO_H265_BS_INIT_PRM_T *prH265BSInitPrm)
{
    BOOL fgInitBSResult;
    UINT32 u4LAEID;
    //u4LAEID = (u4VDecID == 0) ? VDEC_LAE0 : VDEC_LAE1;
    u4LAEID = u4VDecID;
    
    #ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] i4VDEC_HAL_H265_InitBarrelShifter() start!!\n");
    #endif

    fgInitBSResult = fgInitH265BarrelShift(u4LAEID, prH265BSInitPrm);

    if (fgInitBSResult)
    {
#ifdef VDEC_SIM_DUMP
        LOG(0,"// [INFO] i4VDEC_HAL_H265_InitBarrelShifter() Done!!\n");
#endif
        return VDEC_HAL_HANDLE_OK;
    }
    else
    {
        LOG(0,"\n// [ERROR] i4VDEC_HAL_H265_InitBarrelShifter() Fail!!!!!!!!!!!!!!\n\n");
        return INIT_BARRELSHIFTER_FAIL;
    }
}

// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H265_ReadRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 *pu4Bits);
// Description :Read current read pointer
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 pu4Bits : read pointer value with remained bits
// Return      : Read pointer value with byte alignment
// **************************************************************************
UINT32 u4VDEC_HAL_H265_ReadRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4VFIFOSa, UINT32 *pu4Bits)
{
    UINT32 retval;
    #ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] u4VDEC_HAL_H265_ReadRdPtr() start!!\n");
    #endif
    retval = u4VDecReadH265VldRPtr(u4BSID, u4VDecID, pu4Bits, PHYSICAL(u4VFIFOSa));
    #ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] u4VDEC_HAL_H265_ReadRdPtr() done!!\n");
    #endif

    return retval;
}


// **************************************************************************
// Function : void v4VDEC_HAL_H265_AlignRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4AlignType);
// Description :Align read pointer to byte,word or double word
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
//                 u4AlignType : read pointer align type
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_AlignRdPtr(UINT32 u4BSID, UINT32 u4VDecID, UINT32 u4AlignType)
{
    return;
}


// **************************************************************************
// Function : UINT32 u4VDEC_HAL_H265_GetBitcount(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Read barrel shifter bitcount after initializing 
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : Current bit count
// **************************************************************************
UINT32 u4VDEC_HAL_H265_GetBitcount(UINT32 u4BSID, UINT32 u4VDecID)
{
    return VDEC_HAL_HANDLE_OK;
}


// **************************************************************************
// Function : void vVDEC_HAL_H265_Modification(UINT32 u4VDecID);
// Description :Reference list reordering
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_RPL_Modification(UINT32 u4VDecID)
{
    UINT32 u4Cnt,u4LoopCnt;
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] vVDEC_HAL_H265_RPL_Modification()\n");
#endif
    
    vVDecWriteHEVCVLD(u4VDecID, HEVC_FC_TRG_REG, HEVC_RPL_MOD);
    u4Cnt = 0;
    u4LoopCnt = 0;
    while(u4LoopCnt < 1000)
    {
        if (u4Cnt == 100)
        {
            if (0==(u4VDecReadHEVCVLD(u4VDecID, HEVC_FC_TRG_REG) & HEVC_RPL_MOD))
            {
                break;
            }
            else
            {
                u4Cnt = 0;   
                u4LoopCnt ++;
            }
        }
        else
        {
            u4Cnt ++;
        }
    }
}

// **************************************************************************
// Function : void vVDEC_HAL_H265_PredWeightTable(UINT32 u4VDecID);
// Description :Decode prediction weighting table
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_PredWeightTable(UINT32 u4VDecID)
{
  
    UINT32 u4Cnt,u4LoopCnt;
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] vVDEC_HAL_H265_PredWeightTable()\n");
#endif
  
    vVDecWriteHEVCVLD(u4VDecID, HEVC_FC_TRG_REG, HEVC_WEIGHT_PRED_TBL);
    u4Cnt = 0;
    u4LoopCnt = 0;
    while(u4LoopCnt < 1000)
    {
        if (u4Cnt == 100)
        {
            if (0==(u4VDecReadHEVCVLD(u4VDecID, HEVC_FC_TRG_REG) & HEVC_WEIGHT_PRED_TBL))
            {
                break;
            }
            else
            {
                u4Cnt = 0;      
                u4LoopCnt ++;
            }
        }
        else
        {
            u4Cnt ++;
        }
    }
  
}


// **************************************************************************
// Function : void vVDEC_HAL_H265_TrailingBits(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Remove traling bits to byte align
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_TrailingBits(UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4Temp;
  
    u4Temp = 8 - (u4VDecHEVCVLDShiftBits(u4BSID, u4VDecID) % 8);
    // at list trailing bit

    if (u4Temp == 8 ){
        u4Temp = u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, 8);
    } else {
        u4Temp = u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, u4Temp);
    }
    
}


// **************************************************************************
// Function : BOOL bVDEC_HAL_H265_IsMoreRbspData(UINT32 u4BSID, UINT32 u4VDecID);
// Description :Check whether there is more rbsp data
// Parameter : u4BSID  : barrelshifter ID
//                 u4VDecID : video decoder hardware ID
// Return      : Is morw Rbsp data or not
// **************************************************************************
BOOL bVDEC_HAL_H265_IsMoreRbspData(UINT32 u4BSID, UINT32 u4VDecID)
{
    UINT32 u4RemainedBits;
    UINT32 u4Temp;
    INT32 i;
    
    u4RemainedBits = (u4VDecHEVCVLDShiftBits(u4BSID, u4VDecID) % 8); //0~7
    //u4RemainedBits = (8 - (((u4VDecReadHEVCVLD(RW_HEVLD_CTRL) >> 16) & 0x3F) % 8));  
    u4Temp = 0xffffffff;
    for (i=0; i<=u4RemainedBits; i++)
    {
        u4Temp &= (~(1<<i));
    }
    
    if ((u4VDecHEVCVLDGetBitS(u4BSID, u4VDecID, 0) & u4Temp) == (0x80000000))
    {
        // no more
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


// **************************************************************************
// Function : void vVDEC_HAL_H265_InitPicInfo(UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm);
// Description :Set HW registers to initialize picture info
// Parameter : u4VDecID : video decoder hardware ID
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_InitPicInfo(UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UINT32 u4Dram_pitch_width, u4Log2_max_cu_size, u4Max_cu_size;
    UINT32 u4Pic_width, u4Pic_height;
    INT32 i4Pic_mb_x,i4Pic_mb_y;
    H265_SPS_Data* prSPS;

    prSPS = prH265DecPrm->prSPS;
    u4Pic_width = prSPS->u4PicWidthInLumaSamples;
    u4Pic_height = prSPS->u4PicHeightInLumaSamples;

    u4Log2_max_cu_size =  prSPS->u4Log2DiffMaxMinCodingBlockSize + prSPS->u4Log2MinCodingBlockSizeMinus3+3;
    u4Max_cu_size = 1<<u4Log2_max_cu_size;

    //UFO mode need take care this register **********************TBD
    //dram_pitch_width = (((pic_width+(max_cu_size-1))>>log2_max_cu_size )<<log2_max_cu_size)/16;
    u4Dram_pitch_width = (((u4Pic_width+(64-1))>>6 )<<6)/16;

    // MC part
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] MC settings\n");
#endif
    vVDecWriteMC(u4VDecID, HEVC_PIC_WIDTH, u4Pic_width);
    vVDecWriteMC(u4VDecID, HEVC_PIC_HEIGHT, u4Pic_height);
    vVDecWriteMC(u4VDecID, HEVC_DRAM_PITCH, u4Dram_pitch_width );
    
    // 10 bit setting
    vVDecWriteMC(u4VDecID, HEVC_MC_CMPTMODE, (prH265DecPrm->ucMain10Mode == HEVC_COMPACT_MODE)? 0x1:0x0);
    if((prH265DecPrm->ucDualCoreMode == DUAL_CORE_MODE) || (prH265DecPrm->ucDualCoreMode == M10TILE_DUAL_CORE_MODE))
    {
        vVDecWriteHEVCPP(u4VDecID, HEVC_PP_PARA, 0x1);
    }
    
    i4Pic_mb_x = (((u4Pic_width  + u4Max_cu_size - 1)/u4Max_cu_size)*u4Max_cu_size)/16;
    i4Pic_mb_y = (((u4Pic_height + u4Max_cu_size - 1)/u4Max_cu_size)*u4Max_cu_size)/16;
    //PP part

    vVDecWriteMC(u4VDecID, HEVC_DBK_ON, 0x1);
    vVDecWriteMC(u4VDecID, HEVC_Y_OUTPUT, (PHYSICAL(prH265DecPrm->prCurrFBufInfo->u4YStartAddr)>>9) & 0x7fffff );    
    vVDecWriteMC(u4VDecID, HEVC_C_OUTPUT, (PHYSICAL(prH265DecPrm->prCurrFBufInfo->u4CStartAddr)>>8) & 0xffffff );
    vVDecWriteMC(u4VDecID, HEVC_OUTPUT_WIDTH, i4Pic_mb_x);
    
    vVDecWriteMC(u4VDecID, HEVC_DBK_ON2, ((prSPS->u4ChromaFormatIdc==0)? 2:3));
    vVDecWriteMC(u4VDecID, HEVC_ENABLE_WR_REC, 0x1);
    vVDecWriteMC(u4VDecID, HEVC_PIC_WITDTH_MB, i4Pic_mb_x-1);
    vVDecWriteMC(u4VDecID, HEVC_PIC_HEIGHT_MB, i4Pic_mb_y-1);

    //set mv addr
    vVDecWriteHEVCMV(u4VDecID, HEVC_MV_WR_SA ,(PHYSICAL(prH265DecPrm->prCurrFBufInfo->u4MvStartAddr)>>4) & 0xfffffff );
    
    // leon58
    if(prH265DecPrm->ucMain10Mode == HEVC_16BIT_MODE)
    {
        vVDecWriteHEVCPP(u4VDecID, HEVC_EXT_BIT_DRAM_MODE, 0x0);
    }
    else if(prH265DecPrm->ucMain10Mode == HEVC_REORDER_MODE)
    {        
        vVDecWriteHEVCPP(u4VDecID, HEVC_EXT_BIT_DRAM_MODE, 0x2);
    }
    else if(prH265DecPrm->ucMain10Mode == HEVC_COMPACT_MODE)
    {
        vVDecWriteHEVCPP(u4VDecID, HEVC_EXT_BIT_DRAM_MODE, 0x1);
    }

    // CRC
    #if defined(CC_MT5890)
    if(IS_IC_5861())
    {
        vVDecWriteHEVCMISC(u4VDecID,(1 << 2),0x100);
    }
    else
    {
        vVDecWriteMCORETOP(u4VDecID,(192 << 2),0x1);
    }
    #else
        vVDecWriteHEVCMISC(u4VDecID,(1 << 2),0x100);
    #endif
    
    if(prH265DecPrm->fgIsStillPicture)
    {        
        #ifdef VDEC_SIM_DUMP
        LOG(0,"// [INFO] PP Setting For Still Picture Profile!\n");
        #endif
        vVDEC_HAL_H265_SetStillImg(u4VDecID,prH265DecPrm);
    }
}


// **************************************************************************
// Function : void vVDEC_HAL_H265_SetRefPicListReg(UINT32 u4VDecID,VDEC_INFO_H265_PIC_INFO_T *prH265PicInfo,VDEC_INFO_H265_FBUF_INFO_T *prH265FBBufInfo);
// Description :Set HW registers related with P reference list
// Parameter : u4VDecID : video decoder hardware ID
//                 prPRefPicListInfo : pointer to information of p reference list
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_SetRefPicListReg(UINT32 u4VDecID,VDEC_INFO_H265_PIC_INFO_T *prH265PicInfo,VDEC_INFO_H265_FBUF_INFO_T *prH265FBBufInfo)
{
    UINT32 i;
    UINT32 u4Addr,u4Value;

#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] MV settings\n");
#endif

    u4Value   =  prH265PicInfo->bLowDelayFlag;
    vVDecWriteHEVCMV(u4VDecID, HEVC_MV_CTRL,u4Value);

    u4Value   = 0;
    if(prH265PicInfo->i4RefListNum > 0)
    {
        for(i = 0; i < 16; i++) 
        {
            INT32 idx;
            idx = (i%prH265PicInfo->i4RefListNum);
            u4Value = u4Value | (prH265PicInfo->i4LongTermList0[idx] << i) | (prH265PicInfo->i4LongTermList1[idx] << (i+16));
        }
        vVDecWriteHEVCMV(u4VDecID, HEVC_LONG_TERM,u4Value);
    }

    u4Value = 0;
    for(i = 0; i < prH265PicInfo->i4DpbLTBuffCnt; i++)
    {
        if(i%4 == 0) 
        {
            u4Value = 0;
        }
        u4Value |= ((prH265PicInfo->i4DpbLTBuffId[i] + 1) & 0x1f) << (i*8);
        if(i%4 == 3)
        {
            vVDecWriteHEVCMV(u4VDecID, HEVC_DPB_LT_BUF_ID_0_3+ ((i>>2)<<2) ,u4Value);
        }
    }

    if(i%4 != 0) 
    {
        vVDecWriteHEVCMV(u4VDecID, HEVC_DPB_LT_BUF_ID_0_3+ ((i>>2)<<2),u4Value);
    }

    if(prH265PicInfo->i4RefListNum > 0)
    {
        for(i = 0; i < 16; i++)
        { 
            INT32 idx;
            idx = (i%prH265PicInfo->i4RefListNum);
            u4Addr = HEVC_L0_INFO_0 + i*4;
            u4Value = ((prH265PicInfo->i4PocDiffList0[idx] & 0xff) << 0) |
              (((prH265PicInfo->i4BuffIdList0[idx] + 1) & 0x1f) << 8) ;
            vVDecWriteHEVCMV(u4VDecID, u4Addr ,u4Value);
        }

        for(i = 0; i < 16; i++)
        {
            INT32 idx;
            idx = (i%prH265PicInfo->i4RefListNum);
            u4Addr = HEVC_L1_INFO_0 + i*4;
            u4Value = ((prH265PicInfo->i4PocDiffList1[idx] & 0xff) << 0) |
              (((prH265PicInfo->i4BuffIdList1[idx] + 1) & 0x1f) << 8) ;
            vVDecWriteHEVCMV(u4VDecID, u4Addr ,u4Value);
        }
    }

    if(prH265PicInfo->i4RefListNum > 0)
    {
        INT32 idx;
        INT32 i4FbufId;
        for(i = 0; i < 16; i++)
        {
            idx = (i%prH265PicInfo->i4RefListNum);
            i4FbufId = prH265PicInfo->i4BuffIdList0[idx];
            vVDecWriteHEVCMV(u4VDecID, i << 2 , (PHYSICAL(prH265FBBufInfo[i4FbufId].u4MvStartAddr) >> 4) & 0xfffffff );
            vVDecWriteMC(u4VDecID, (i + 279) << 2 ,PHYSICAL(prH265FBBufInfo[i4FbufId].u4YStartAddr));            
            vVDecWriteMC(u4VDecID, (i + 760) << 2 ,PHYSICAL(prH265FBBufInfo[i4FbufId].u4CStartAddr));
        }
        
        for(i = 0; i < 16; i++)
        {
            idx = (i%prH265PicInfo->i4RefListNum);
            i4FbufId = prH265PicInfo->i4BuffIdList1[idx];
            vVDecWriteHEVCMV(u4VDecID, (i + 16) << 2 ,(PHYSICAL(prH265FBBufInfo[i4FbufId].u4MvStartAddr) >> 4) & 0xfffffff );
            vVDecWriteMC(u4VDecID, (i + 311) << 2 ,PHYSICAL(prH265FBBufInfo[i4FbufId].u4YStartAddr) );            
            vVDecWriteMC(u4VDecID, (i + 792) << 2 ,PHYSICAL(prH265FBBufInfo[i4FbufId].u4CStartAddr));
        }
   }
}


void u4VDEC_HAL_H265_SetUFO(UINT32 u4VDecID,VDEC_INFO_H265_FBUF_INFO_T *prCurrentFBuf)
{
    BOOL bIsUFO;
    bIsUFO = prCurrentFBuf->bIsUFOEncoded;
    // UFO mode settings
    if (bIsUFO)
    { 
        vVDecWriteMC(u4VDecID, HEVC_UFO_Y_LEN_ADDR, PHYSICAL(prCurrentFBuf->u4YLenStartAddr));
        vVDecWriteMC(u4VDecID, HEVC_UFO_C_LEN_ADDR, PHYSICAL(prCurrentFBuf->u4CLenStartAddr));
        // leon
        vVDecWriteMC(u4VDecID, HEVC_Y_PIC_SIZE, prCurrentFBuf->u4DramYSize);        
        vVDecWriteMC(u4VDecID, HEVC_C_PIC_SIZE, prCurrentFBuf->u4DramCSize);
        vVDecWriteHEVCPP(u4VDecID,HEVC_PP_CYCLE_SPEED,0x3);
        //vVDecWriteHEVCPP(u4VDecID,HEVC_PP_UFO_SWITCH,(u4VDecReadHEVCPP(u4VDecID,HEVC_PP_UFO_SWITCH) | 0x3));

        //PP #806 bit[7:0] must be set 0x96 @ Oryx ES2
        //To do ....
        #if defined(CC_MT5890)
        if(IS_IC_5890_ES1() || IS_IC_5890_MPS1())
        {
            vVDecWriteHEVCPP(u4VDecID,HEVC_PP_UFO_MODE,0x96);
        }
        #endif

        // enable UFO mode
        vVDecWriteMC(u4VDecID, HEVC_UFO_CTR, 0x11);
    } 
    else 
    {
        vVDecWriteMC(u4VDecID, HEVC_UFO_CTR, 0x0);
    }
}


// **************************************************************************
// Function : u4VDEC_HAL_H265_SetECInfo(UINT32 u4VDecID,VDEC_INFO_H265_FBUF_INFO_T *prECRefFBuf,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
// Description :Set error concealment ref buffer
// Parameter : u4VDecID : video decoder hardware ID
//                 prECRefFBuf
//                 prECRefFBuf : pointer to reference buffer struct
// Return      : None
// **************************************************************************
void u4VDEC_HAL_H265_SetECInfo(UINT32 u4VDecID,VDEC_INFO_H265_FBUF_INFO_T *prECRefFBuf,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    BOOL fgTile = FALSE;
    UCHAR i;
    #ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] EC settings\n");
    #endif
    // yc seperate enable
    vVDecWriteMC(u4VDecID, HEVC_MC_Y_C_SEP_SET,  0x1 );
    
//    vVDecWriteMC(u4VDecID, HEVC_MC_REF_P_LIST0_Y,  PHYSICAL(prECRefFBuf->u4YStartAddr));
//    vVDecWriteMC(u4VDecID, HEVC_MC_REF_B_LIST0_Y,  PHYSICAL(prECRefFBuf->u4YStartAddr));
//    vVDecWriteMC(u4VDecID, HEVC_MC_REF_B_LIST1_Y,  PHYSICAL(prECRefFBuf->u4YStartAddr));

//    vVDecWriteMC(u4VDecID, HEVC_MC_REF_P_LIST0_C,  PHYSICAL(prECRefFBuf->u4CStartAddr));
//    vVDecWriteMC(u4VDecID, HEVC_MC_REF_B_LIST0_C,  PHYSICAL(prECRefFBuf->u4CStartAddr));
//    vVDecWriteMC(u4VDecID, HEVC_MC_REF_B_LIST1_C,  PHYSICAL(prECRefFBuf->u4CStartAddr));

//    vVDecWriteHEVCMV(u4VDecID, 0, PHYSICAL(prECRefFBuf->u4MvStartAddr)>>4 );
//    vVDecWriteHEVCMV(u4VDecID, 16*4, PHYSICAL(prECRefFBuf->u4MvStartAddr)>>4 );

    for(i=0; i<16; i++)
    {
        vVDecWriteMC(u4VDecID, (279+i)*4,  PHYSICAL(prECRefFBuf->u4YStartAddr) );
        vVDecWriteMC(u4VDecID, (311+i)*4,  PHYSICAL(prECRefFBuf->u4YStartAddr) );
        vVDecWriteMC(u4VDecID, (760+i)*4,  PHYSICAL(prECRefFBuf->u4CStartAddr) );
        vVDecWriteMC(u4VDecID, (792+i)*4,  PHYSICAL(prECRefFBuf->u4CStartAddr) );
        vVDecWriteHEVCMV(u4VDecID, i*4, PHYSICAL(prECRefFBuf->u4MvStartAddr)>>4 );
        vVDecWriteHEVCMV(u4VDecID, (16+i)*4, PHYSICAL(prECRefFBuf->u4MvStartAddr)>>4 );
    }
    
    vVDecWriteHEVCVLD(u4VDecID, HEVC_VLD_ERROR_TYPE_ENABLE, 0xfffefffb);
    //vVDecWriteHEVCVLD(u4VDecID, HEVC_VLD_PICTURE_BYTES, total_bytes_in_curr_pic);
    vVDecWriteHEVCVLD(u4VDecID, HEVC_VLD_ERROR_HANDLING, 0x04011d01);   // 06172013, turn on slice_reconceal_sel
    
    fgTile = prH265DecPrm->prPPS->bTilesEnabledFlag;
    if(fgTile)
    {
        #if HEVC_CRC_COMPARE
        if(prH265DecPrm->prCurrFBufInfo->bIsUFOEncoded)
        {
            vVDecWriteHEVCPP(u4VDecID, HEVC_PP_ERROR_HANDLE_MODE, 0x00007011 );
        }
        else
        {
            vVDecWriteHEVCPP(u4VDecID, HEVC_PP_ERROR_HANDLE_MODE, 0x80004011 );
        }
        #else
        vVDecWriteHEVCPP(u4VDecID, HEVC_PP_ERROR_HANDLE_MODE, 0x01004011 );
        #endif
    }
    else
    {
        vVDecWriteHEVCPP(u4VDecID, HEVC_PP_ERROR_HANDLE_MODE, 0x00007011 );
    }
    
    #if defined(CC_MT5890)//Need improve, search neighbor buffer idx for error concealment. fix buffer idx = 0 temp.
    if(IS_IC_5861())
    {
        vVDecWriteHEVCMV(u4VDecID, HEVC_MV_EC_FBID, u4VDecReadHEVCMV(u4VDecID, HEVC_MV_EC_FBID) & (0xFFFFFF0F) );
    }
    #else
        vVDecWriteHEVCMV(u4VDecID, HEVC_MV_EC_FBID, u4VDecReadHEVCMV(u4VDecID, HEVC_MV_EC_FBID) & (0xFFFFFF0F) );   
    #endif
}


// **************************************************************************
// Function : void vVDEC_HAL_H265_SetSPSHEVLD(UINT32 u4VDecID, H265_SPS_Data *prSPS);
// Description :Set Slice data to HW
// Parameter : u4VDecID : video decoder hardware ID
//                 prPPS
//                 prSPS : pointer to sequence parameter set struct
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_SetSPSHEVLD(UINT32 u4VDecID, H265_SPS_Data *prSPS, H265_PPS_Data *prPPS)
{
    UINT32 u4Max_cu_width, u4Max_cu_heigtht;
    UINT32 u4Pic_width, u4Pic_height;
    UINT32 u4Lcu_pic_width, u4Lcu_pic_height;
    UINT32 u4Log2_lt_ref_pic_sps = 1;
    UINT32 u4Log2_short_ref_pic_idx = 1;
    UINT32 u4Log2_max_cu_size;
    UINT32 u4Log2_max_tu_size;

    UINT32 u4SPSInfo;
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] vVDEC_HAL_H265_SetSPSHEVLD() start!!\n");
#endif
    
    u4Log2_max_cu_size =  prSPS->u4Log2DiffMaxMinCodingBlockSize + prSPS->u4Log2MinCodingBlockSizeMinus3+3;
    u4Log2_max_tu_size = prSPS->u4Log2DiffMaxMinTtransformBlockSize + prSPS->u4Log2MinTransformBlockSizeMinus2+2;

    u4Max_cu_width = 1 << u4Log2_max_cu_size;
    u4Max_cu_heigtht = 1 << u4Log2_max_cu_size;

    u4Pic_width = prSPS->u4PicWidthInLumaSamples;
    u4Pic_height = prSPS->u4PicHeightInLumaSamples;
    u4Pic_width = ((u4Pic_width + u4Max_cu_width -1) >> u4Log2_max_cu_size ) << u4Log2_max_cu_size;
    u4Pic_height = ((u4Pic_height + u4Max_cu_width -1) >> u4Log2_max_cu_size ) << u4Log2_max_cu_size;

    u4Lcu_pic_width    = ( u4Pic_width%u4Max_cu_width ) ? u4Pic_width /u4Max_cu_width  + 1 : u4Pic_width /u4Max_cu_width;
    u4Lcu_pic_height   = ( u4Pic_height%u4Max_cu_heigtht ) ? u4Pic_height/u4Max_cu_heigtht + 1 : u4Pic_height/u4Max_cu_heigtht;

    vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_PIC_MB_SIZE_M1, (((u4Lcu_pic_width  - 1) & 0xfff) << 0 ) |(((u4Lcu_pic_height - 1) & 0xfff) << 16 ));  
    vVDecWriteVLDTOP(u4VDecID, HEVC_VLD_TOP_PIC_PIX_SIZE, (((prSPS->u4PicWidthInLumaSamples) & 0xffff) << 0 ) | (((prSPS->u4PicHeightInLumaSamples) & 0xffff) << 16 ));  
    vVDecWriteVLDTOP(u4VDecID, HEVC_VLD_TOP_PIC_BLK_SIZE, (((u4Lcu_pic_width  - 0) & 0xfff) << 0 ) |(((u4Lcu_pic_height - 0) & 0xfff) << 16 ));  

   
    // for 10 bit or 12 bit
    if(prSPS->u4BitDepthLumaMinus8 != 0 && prSPS->u4BitDepthChromaMinus8!= 0)
    {
       if(prSPS->u4BitDepthLumaMinus8 == 0x2 && prSPS->u4BitDepthChromaMinus8 == 0x2) 
       {
            vVDecWriteVLDTOP(u4VDecID, HEVC_VLD_TOP_BIT_DEPTH_SETTING, 0x22);  
            #if defined(CC_MT5890)//gazelle need this setting
            if(IS_IC_5861())
            {
                vVDecWriteHEVCPP(u4VDecID, HEVC_PP_2BIT_REARRANGE, 0x1);
            }
            #else
                vVDecWriteHEVCPP(u4VDecID, HEVC_PP_2BIT_REARRANGE, 0x1);
            #endif
       }
       else if (prSPS->u4BitDepthLumaMinus8 == 0x4 && prSPS->u4BitDepthChromaMinus8 == 0x4) 
       {
            // leon, need confirm
            vVDecWriteVLDTOP(u4VDecID, HEVC_VLD_TOP_BIT_DEPTH_SETTING, 0x22);  
       }
       else
       {
            // error
       }
    }     
    
    u4SPSInfo = 0;
    u4SPSInfo = (prSPS->u4ChromaFormatIdc & 0x3);
    u4SPSInfo |= ((prSPS->bUsePCM& 0x1)<< 2);
    u4SPSInfo |= ((prPPS->bListsModificationPresentFlag& 0x1) << 3); 
    u4SPSInfo |= ((prSPS->bUseAMP & 0x1) << 4);
    u4SPSInfo |= ((prSPS->bUseSAO & 0x1) << 5);
    u4SPSInfo |= ((prSPS->bPCMFilterDisableFlag & 0x1) << 6);
    u4SPSInfo |= ((prSPS->bTMVPFlagsPresent & 0x1) << 7);
    u4SPSInfo |= ((prSPS->u4PCMBitDepthLumaMinus1 & 0xf) << 8);
    u4SPSInfo |= ((prSPS->u4PCMBitDepthChromaMinus1& 0xf) << 12);
    u4SPSInfo |= ((prSPS->u4NumLongTermRefPicSPS & 0x3f) << 16);
    u4SPSInfo |= ((prSPS->bLongTermRefsPresent & 0x1) << 22);
    u4SPSInfo |= ((prSPS->u4NumShortTermRefPicSets & 0x7f) << 24);
    
    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_SPS_0, u4SPSInfo);

    //printk("1-------prSPS->u4NumLongTermRefPicSPS %d\n",prSPS->u4NumLongTermRefPicSPS);
    while ( (prSPS->u4NumLongTermRefPicSPS & 0x3f) > (1 << u4Log2_lt_ref_pic_sps)){
        //printk("looping-------log2_lt_ref_pic_sps %d\n",log2_lt_ref_pic_sps);
        u4Log2_lt_ref_pic_sps++;
    }
    //printk("2-------prSPS->u4NumShortTermRefPicSets %d\n",prSPS->u4NumShortTermRefPicSets);
    while ( (prSPS->u4NumShortTermRefPicSets & 0x7f) > (1 << u4Log2_short_ref_pic_idx)){
        //printk("looping-------log2_short_ref_pic_idx %d\n",log2_short_ref_pic_idx);
        u4Log2_short_ref_pic_idx++;
    }
    //printk("3-------log2_lt_ref_pic_sps %d; log2_short_ref_pic_idx %d\n", log2_lt_ref_pic_sps, log2_short_ref_pic_idx);
    
    u4SPSInfo = 0;
    u4SPSInfo = ((prSPS->u4Log2MaxPicOrderCntLsbMinus4+4) & 0x1f);
    u4SPSInfo |= ((u4Log2_short_ref_pic_idx & 0x7)<< 8);
    u4SPSInfo |= ((u4Log2_lt_ref_pic_sps & 0x7)<< 12);
    u4SPSInfo |= ((prSPS->bUseStrongIntraSmoothing & 0x1)<< 16);
    
    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_SPS_1, u4SPSInfo);

    u4SPSInfo = 0;
    u4SPSInfo = ((prSPS->u4Log2MinCodingBlockSizeMinus3+3) & 0x7);
    u4SPSInfo |= ((u4Log2_max_cu_size & 0x7)<< 4);
    u4SPSInfo |= (((prSPS->u4Log2MinTransformBlockSizeMinus2+2)& 0x7)<< 8);
    u4SPSInfo |= ((u4Log2_max_tu_size & 0x7)<< 12);
    u4SPSInfo |= (((prSPS->u4PCMLog2LumaMinSizeMinus3+3) & 0x7)<< 16);
    u4SPSInfo |= (((prSPS->u4PCMLog2LumaMaxSize)& 0x7)<< 20);
    u4SPSInfo |= (((prSPS->u4QuadtreeTUMaxDepthInter -1)& 0x7)<< 24);
    u4SPSInfo |= (((prSPS->u4QuadtreeTUMaxDepthIntra -1)& 0x7)<< 28);

    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_SPS_SIZE, u4SPSInfo);
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] vVDEC_HAL_H265_SetSPSHEVLD() done!!\n");
#endif

}


// **************************************************************************
// Function : void vVDEC_HAL_H265_SetPPSHEVLD(UINT32 u4VDecID, H265_SPS_Data *prSPS, H265_PPS_Data *prPPS)
// Description :Set PPS data to HW
// Parameter : u4VDecID : video decoder hardware ID
//                   prSPS
//                   prPPS : pointer to picture parameter set struct
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_SetPPSHEVLD(UINT32 u4VDecID, H265_SPS_Data *prSPS, H265_PPS_Data *prPPS)
{
    UINT32 u4PPSInfo;
    INT32 u4Log2_min_cu_qp_delta_size;
    INT32 u4Log2_max_cu_size; 
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] vVDEC_HAL_H265_SetPPSHEVLD() start!!\n");
#endif
   
    u4Log2_max_cu_size =  prSPS->u4Log2DiffMaxMinCodingBlockSize + prSPS->u4Log2MinCodingBlockSizeMinus3+3;

    u4PPSInfo = 0;
    u4PPSInfo = (prPPS->bSignHideFlag & 0x1);
    u4PPSInfo |= ((prPPS->bCabacInitPresentFlag & 0x1)<< 1);
    u4PPSInfo |= ((prPPS->bConstrainedIntraPredFlag & 0x1)<< 2);
    u4PPSInfo |= ((prPPS->bTransformSkipEnabledFlag & 0x1)<< 3);
    u4PPSInfo |= ((prPPS->bWPPredFlag& 0x1)<< 4);
    u4PPSInfo |= ((prPPS->bWPBiPredFlag& 0x1)<< 5);
    u4PPSInfo |= ((prPPS->bOutputFlagPresentFlag & 0x1)<< 6);
    u4PPSInfo |= ((prPPS->bTransquantBypassEnableFlag & 0x1)<< 7);
    u4PPSInfo |= ((prPPS->bDependentSliceSegmentsEnabledFlag & 0x1)<< 8);
    u4PPSInfo |= ((prPPS->bEntropyCodingSyncEnabledFlag & 0x1)<< 9);
    u4PPSInfo |= ((prPPS->bSliceHeaderExtensionPresentFlag & 0x1)<< 11);
    u4PPSInfo |= ((prPPS->u4NumRefIdxL0DefaultActiveMinus1& 0xf)<< 16);
    u4PPSInfo |= ((prPPS->u4NumRefIdxL1DefaultActiveMinus1& 0xf)<< 20);
    u4PPSInfo |= (((prPPS->u4Log2ParallelMergeLevelMinus2+2)& 0x7)<< 24);
    u4PPSInfo |= ((prPPS->u4NumExtraSliceHeaderBits & 0x7)<< 28);

    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_PPS, u4PPSInfo);

    u4Log2_min_cu_qp_delta_size = u4Log2_max_cu_size - (prPPS->u4DiffCuQPDeltaDepth);

    u4PPSInfo = 0;
    u4PPSInfo = (prPPS->bCuQPDeltaEnabledFlag & 0x1);
    u4PPSInfo |= ((prPPS->bPPSSliceChromaQpFlag & 0x1)<< 1);
    u4PPSInfo |= ((u4Log2_min_cu_qp_delta_size & 0x7)<< 4);

    // 8 bit
    if(prSPS->u4BitDepthLumaMinus8 == 0 && prSPS->u4BitDepthChromaMinus8 == 0)
    {
    u4PPSInfo |= (((prPPS->i4PicInitQPMinus26+26) & 0x3f)<< 8);
    }
    else // 10 bit
    {
        u4PPSInfo |= (((prPPS->i4PicInitQPMinus26+26) & 0x7f)<< 8);
    }
    
    u4PPSInfo |= ((prPPS->i4PPSCbQPOffset & 0x1f)<< 16);
    u4PPSInfo |= ((prPPS->i4PPSCrQPOffset & 0x1f)<< 24);

    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_PPS_QP, u4PPSInfo);

    u4PPSInfo = 0;
    u4PPSInfo = (prPPS->bTilesEnabledFlag & 0x1);
    u4PPSInfo |= ((prPPS->u4NumColumnsMinus1 & 0x1f)<< 8);
    u4PPSInfo |= ((prPPS->u4NumRowsMinus1 & 0x1f)<< 16);
    u4PPSInfo |= ((prPPS->bLoopFilterAcrossTilesEnabledFlag & 0x1)<< 24);
    
    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_PPS_TILE , u4PPSInfo);

    u4PPSInfo = 0;
    u4PPSInfo = (prPPS->bLoopFilterAcrossSlicesEnabledFlag & 0x1);
    u4PPSInfo |= ((prPPS->bDeblockingFilterControlPresentFlag & 0x1)<< 1);
    u4PPSInfo |= ((prPPS->bDeblockingFilterOverrideEnabledFlag & 0x1)<< 2);
    u4PPSInfo |= ((prPPS->bPicDisableDeblockingFilterFlag & 0x1)<< 3);
    u4PPSInfo |= ((prPPS->i4DeblockingFilterBetaOffsetDiv2 & 0x1f)<< 4);     //[notice] initial value need to be check
    u4PPSInfo |= ((prPPS->i4DeblockingFilterTcOffsetDiv2 & 0x1f)<< 9);      //

    vVDecWriteHEVCVLD(u4VDecID, RW_HEVLD_PPS_DBK , u4PPSInfo);
    
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] vVDEC_HAL_H265_SetPPSHEVLD() done!!\n");
#endif

}


// **************************************************************************
// Function :void vVDEC_HAL_H265_SetSHDRHEVLD(UINT32 u4VDecID, VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
// Description :Set part of slice header data to HW
// Parameter : u4VDecID : video decoder hardware ID
//                 prSliceHdr : pointer to slice parameter set struct
//                 bUseSAO, prPPS
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_SetSHDRHEVLD(UINT32 u4VDecID, VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    UINT32 u4SHDRInfo,u4Length = 0;
    INT32 i4Indexer,i4NumRpsCurrTempList = 0;
    INT32 i4NumRpsCurrTempList0 = 0;
    UCHAR ucNalType = 0xFF;
    BOOL bUseSAO;

    H265_SPS_Data *prSPS;
    H265_PPS_Data *prPPS;
    H265_Slice_Hdr_Data *prSliceHdr;
    VDEC_INFO_H265_PIC_INFO_T *prH265PicInfo;
    
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] vVDEC_HAL_H265_SetSHDRHEVLD() start!!\n");
#endif

    prSPS = prH265DecPrm->prSPS;
    prPPS = prH265DecPrm->prPPS;
    prSliceHdr = prH265DecPrm->prSliceHdr;
    prH265PicInfo = prH265DecPrm->prPicInfo;
    
    bUseSAO = prSPS->bUseSAO;
    u4SHDRInfo = 0;
    u4SHDRInfo = ((bUseSAO & prSliceHdr->bSaoEnabledFlag & 0x1) << 5);
    u4SHDRInfo |= ((bUseSAO & prSliceHdr->bSaoEnabledFlagChroma & 0x1) << 6);
    u4SHDRInfo |= ((prSliceHdr->bCabacInitFlag & 0x1) << 7);

    // 8 bit
    if(prSPS->u4BitDepthLumaMinus8 == 0 && prSPS->u4BitDepthChromaMinus8 == 0)
    {
        u4SHDRInfo |= ((prSliceHdr->i4SliceQp & 0x3f) << 8);
    }
    else // 10 bit    
    {
        u4SHDRInfo |= ((prSliceHdr->i4SliceQp & 0x7f) << 8);
    }
    u4SHDRInfo |= (((prPPS->i4PPSCbQPOffset + prSliceHdr->i4SliceQpDeltaCb) & 0x1f) << 16);
    u4SHDRInfo |= (((prPPS->i4PPSCrQPOffset + prSliceHdr->i4SliceQpDeltaCr) & 0x1f) << 24);
    
    vVDecWriteHEVCVLD(u4VDecID, RW_HEVC_SLICE_1 , u4SHDRInfo);

    u4SHDRInfo = 0;
    u4SHDRInfo = (((prSliceHdr->i4NumRefIdx[0]-1) & 0xf) << 0) ;
    u4SHDRInfo |= (((prSliceHdr->i4NumRefIdx[1]-1) & 0xf) << 4);
    u4SHDRInfo |= ((prSliceHdr->u4ColRefIdx & 0xf) << 8) ;
    u4SHDRInfo |= (((5-prSliceHdr->u4FiveMinusMaxNumMergeCand) & 0x7) << 12);
    u4SHDRInfo |= ((prSliceHdr->i4DeblockingFilterBetaOffsetDiv2 & 0x1f) << 16);        //[notice] initial value need to be check
    u4SHDRInfo |= ((prSliceHdr->i4DeblockingFilterTcOffsetDiv2 & 0x1f) << 21);          //
    u4SHDRInfo |= ((prSliceHdr->bTMVPFlagsPresent & 0x1) << 26);
    u4SHDRInfo |= ((prSliceHdr->bColFromL0Flag & 0x1) << 27);
    u4SHDRInfo |= ((prSliceHdr->bMvdL1ZeroFlag & 0x1) << 28);
    u4SHDRInfo |= ((prSliceHdr->bLoopFilterAcrossSlicesEnabledFlag & 0x1) << 29);
    u4SHDRInfo |= ((prSliceHdr->bDeblockingFilterDisableFlag & 0x1) << 30);

    vVDecWriteHEVCVLD(u4VDecID, RW_HEVC_SLICE_2, u4SHDRInfo);

    u4SHDRInfo = 0;
    u4SHDRInfo = (( prH265PicInfo->i4StrNumDeltaPocs & 0x1f) << 0);
    u4SHDRInfo |= ((( prH265PicInfo->i4StrNumNegPosPics) & 0x3f) << 8);
    u4SHDRInfo |= (( prH265PicInfo->i4NumLongTerm & 0x1f) << 16);
    u4SHDRInfo |= (( prH265PicInfo->i4NumLongTermSps & 0x1f) << 24);

    vVDecWriteHEVCVLD(u4VDecID, RW_HEVC_SLICE_STR_LT, u4SHDRInfo);

    // calculate i4NumRpsCurrTempList
    for( i4Indexer=0; i4Indexer < prSliceHdr->pShortTermRefPicSets->u4NumberOfNegativePictures 
            +prSliceHdr->pShortTermRefPicSets->u4NumberOfPositivePictures
            +prSliceHdr->pShortTermRefPicSets->u4NumberOfLongtermPictures; i4Indexer++) 
    {
        if(prSliceHdr->pShortTermRefPicSets->bUsed[i4Indexer]) 
        {
            i4NumRpsCurrTempList++;
        }
    }

    i4NumRpsCurrTempList0 = i4NumRpsCurrTempList;
    if ( i4NumRpsCurrTempList0 > 1 )
    {
        u4Length = 1;
        i4NumRpsCurrTempList0 --;
    }
    
    while ( i4NumRpsCurrTempList0 >>= 1) 
    {
        u4Length ++;
    }

    u4SHDRInfo = 0;
    u4SHDRInfo = (UINT32)(((i4NumRpsCurrTempList & 0xf) << 0) | ((u4Length & 0x7) << 4));

    vVDecWriteHEVCVLD(u4VDecID, RW_HEVC_REF_PIC_LIST_MOD, u4SHDRInfo);

    u4SHDRInfo = 0;
    ucNalType = (UCHAR)(prSliceHdr->u4NalType);
    u4SHDRInfo  = (UINT32)((0xffff << 16) | (ucNalType << 8) | (21));
    
    vVDecWriteHEVCVLD(u4VDecID, RW_HEVC_ERR_DET_CTRL, u4SHDRInfo);
#ifdef VDEC_SIM_DUMP
    LOG(0,"// [INFO] vVDEC_HAL_H265_SetSHDRHEVLD() done!!\n");
#endif

}

// **************************************************************************
// Function :void vVDEC_HAL_H265_SetSLPP(UINT32 u4VDecID, pH265_SL_Data ScallingList)
// Description :Set scaling list data to HW
// Parameter : u4VDecID : video decoder hardware ID
//                 ScallingList : scaling list data
/// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_SetSLPP(UINT32 u4VDecID, pH265_SL_Data ScallingList)
{
    UINT32  u4SLInfo;

    //if(_rH265PicInfo[u4VDecID].u4SliceCnt==0)
    {
        u4SLInfo = 0;
        u4SLInfo = ((ScallingList->i4ScalingListDC[2][0] & 0xff )<< 0);
        u4SLInfo |= ((ScallingList->i4ScalingListDC[2][1] & 0xff )<< 8);
        u4SLInfo |= ((ScallingList->i4ScalingListDC[2][2] & 0xff )<<16);
        u4SLInfo |= ((ScallingList->i4ScalingListDC[2][3] & 0xff )<<24);
        
        vVDecWriteHEVCPP(u4VDecID, HEVC_IQ_SACLING_FACTOR_DC_0, u4SLInfo );

        u4SLInfo = 0;
        u4SLInfo = ((ScallingList->i4ScalingListDC[2][4] & 0xff )<< 0);
        u4SLInfo |= ((ScallingList->i4ScalingListDC[2][5] & 0xff )<< 8);
        u4SLInfo |= ((ScallingList->i4ScalingListDC[3][0] & 0xff )<<16);
        u4SLInfo |= ((ScallingList->i4ScalingListDC[3][1] & 0xff )<<24);

        vVDecWriteHEVCPP(u4VDecID, HEVC_IQ_SACLING_FACTOR_DC_1, u4SLInfo );
    }

}


// **************************************************************************
// Function :void vVDEC_HAL_H265_SetSLVLD(UINT32 u4VDecID, INT32 *coeff, INT32 width, INT32 height, INT32 invQuantScales,UINT32 *u4SramAddr)
// Description :Set scaling list data to HW
// Parameter : u4VDecID : video decoder hardware ID
//                 coeff : scaling list coefficient
//                 width, height, invQuantScales
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_SetSLVLD(UINT32 u4VDecID, INT32 *coeff, INT32 i4Width, INT32 i4Height, INT32 invQuantScales,UINT32 *u4SramAddr)
{
    INT32 i4Idx;
    UINT32  u4SLInfo;

    if((invQuantScales == 40) && (i4Width == i4Height))
    { 
        if(i4Width==4) //4x4
        {
            for( i4Idx = 0; i4Idx < 4; i4Idx++)
            {           
                  //if(_rH265PicInfo[u4VDecID].u4SliceCnt==0)
                  {
                      vVDecWriteVLD(u4VDecID, HEVC_IQ_SRAM_ADDR, *u4SramAddr );
                      u4SLInfo = 0;
                      u4SLInfo =  (coeff[0*4+i4Idx] & 0xff )<< 0;
                      u4SLInfo |= (coeff[1*4+i4Idx] & 0xff )<< 8;
                      u4SLInfo |= (coeff[2*4+i4Idx] & 0xff )<< 16;
                      u4SLInfo |= (coeff[3*4+i4Idx] & 0xff )<< 24;
                     
                      vVDecWriteVLD(u4VDecID, HEVC_IQ_SRAM_DATA, u4SLInfo );
                  }
                  (*u4SramAddr)++;
            }
    
        }
        else //8x8, 16x16, 32x32
        {
              for( i4Idx = 0; i4Idx < 8; i4Idx++)
              {
                    //if(_rH265PicInfo[u4VDecID].u4SliceCnt==0)
                    {
                        vVDecWriteVLD(u4VDecID, HEVC_IQ_SRAM_ADDR, *u4SramAddr);
                        u4SLInfo = 0;
                        u4SLInfo =  (coeff[0*8+i4Idx] & 0xff )<< 0;
                        u4SLInfo |= (coeff[1*8+i4Idx] & 0xff )<< 8;
                        u4SLInfo |= (coeff[2*8+i4Idx] & 0xff )<< 16;
                        u4SLInfo |= (coeff[3*8+i4Idx] & 0xff )<< 24;
                       
                        vVDecWriteVLD(u4VDecID, HEVC_IQ_SRAM_DATA, u4SLInfo );
                    }
                    (*u4SramAddr)++;
      
                    //if(_rH265PicInfo[u4VDecID].u4SliceCnt==0)
                    {
                        vVDecWriteVLD(u4VDecID, HEVC_IQ_SRAM_ADDR, *u4SramAddr);
                        u4SLInfo = 0;
                        u4SLInfo =  (coeff[4*8+i4Idx] & 0xff )<< 0;
                        u4SLInfo |= (coeff[5*8+i4Idx] & 0xff )<< 8;
                        u4SLInfo |= (coeff[6*8+i4Idx] & 0xff )<< 16;
                        u4SLInfo |= (coeff[7*8+i4Idx] & 0xff )<< 24;
  
                        vVDecWriteVLD(u4VDecID, HEVC_IQ_SRAM_DATA, u4SLInfo );
                    }
                    (*u4SramAddr)++;
              }
          }   
    }
}



// **************************************************************************
// Function : void vVDEC_HAL_H265_SetTilesInfo(UINT32 u4VDecID, H265_SPS_Data *prSPS, H265_PPS_Data *prPPS,VDEC_INFO_H265_PIC_INFO_T *prH265PicInfo)
//// Description :Set Tiles Info to HW
// Parameter : u4VDecID : video decoder hardware ID
//                 prSPS
//                 prPPS : pointer to picture parameter set struct
//              
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_SetTilesInfo(UINT32 u4VDecID, H265_SPS_Data *prSPS, H265_PPS_Data *prPPS,VDEC_INFO_H265_PIC_INFO_T *prH265PicInfo)
{
    UINT32  u4ColumnIdx = 0,u4RowIdx = 0;
    UINT32  u4BoundaryIdx = 0;
    UINT32  u4RightEdgePosInCU,u4BottomEdgePosInCU;
    UINT32  u4Last_LCU_X_Tile[17];
    UINT32  u4Last_LCU_Y_Tile[17];
    
    x_memset(u4Last_LCU_X_Tile,0,sizeof(u4Last_LCU_X_Tile));
    x_memset(u4Last_LCU_Y_Tile,0,sizeof(u4Last_LCU_X_Tile));
    
    for( u4ColumnIdx = 0; u4ColumnIdx < prPPS->u4NumColumnsMinus1+1; u4ColumnIdx++ )
    {
        u4RightEdgePosInCU = prH265PicInfo->rTileInfo[u4ColumnIdx].u4RightEdgePosInCU;
        u4BoundaryIdx = ((u4RightEdgePosInCU >> 5) > (MAX_LCU_BOUNDARY - 1)) ? (MAX_LCU_BOUNDARY - 1) : (u4RightEdgePosInCU >> 5);
        u4Last_LCU_X_Tile[u4BoundaryIdx] |= ((1<<(u4RightEdgePosInCU - LCU_BOUNDARY[u4BoundaryIdx])));
        //printk("uiRightEdgePosInCU = %d reg[%d] 0x%x\n",uiRightEdgePosInCU,u4BoundaryIdx,u4Last_LCU_X_Tile[u4BoundaryIdx]);
    }
    
    for( u4RowIdx = 0; u4RowIdx < prPPS->u4NumRowsMinus1+1; u4RowIdx++ )
    {
        u4BottomEdgePosInCU = prH265PicInfo->rTileInfo[u4RowIdx*(prPPS->u4NumColumnsMinus1+1)].u4BottomEdgePosInCU;
        u4BoundaryIdx = ((u4BottomEdgePosInCU >> 5) > (MAX_LCU_BOUNDARY - 1)) ? (MAX_LCU_BOUNDARY - 1) : (u4BottomEdgePosInCU >> 5);
        u4Last_LCU_Y_Tile[u4BoundaryIdx] |= ((1<<(u4BottomEdgePosInCU - LCU_BOUNDARY[u4BoundaryIdx])));
        //printk("uiBottomEdgePosInCU = %d reg[%d] 0x%x\n",uiBottomEdgePosInCU,u4BoundaryIdx,u4Last_LCU_Y_Tile[u4BoundaryIdx]);
    }

    for(u4BoundaryIdx = 0; u4BoundaryIdx < 17; u4BoundaryIdx++)
    {
        if(u4BoundaryIdx < 16)
        {
            vVDecWriteHEVCVLD(u4VDecID,RW_HEVC_TILE_X_0 + (u4BoundaryIdx << 2),u4Last_LCU_X_Tile[u4BoundaryIdx]);
            vVDecWriteHEVCVLD(u4VDecID,RW_HEVC_TILE_Y_0 + (u4BoundaryIdx << 2),u4Last_LCU_Y_Tile[u4BoundaryIdx]);
        }
        else
        {
            vVDecWriteHEVCVLD(u4VDecID,RW_HEVC_TILE_X_16,u4Last_LCU_X_Tile[u4BoundaryIdx]);
            vVDecWriteHEVCVLD(u4VDecID,RW_HEVC_TILE_Y_16,u4Last_LCU_Y_Tile[u4BoundaryIdx]);
        }
    }
        
}
    


// **************************************************************************
// Function : INT32 i4VDEC_HAL_H265_DecStart(UINT32 u4VDecID, VDEC_INFO_H265_DEC_PRM_T *prDecPrm);
// Description :Set video decoder hardware registers to decode for H265
// Parameter : ptHalDecH265Info : pointer to H265 decode info struct
// Return      : =0: success.
//                  <0: fail.
// **************************************************************************
INT32 i4VDEC_HAL_H265_DecStart(UINT32 u4VDecID, VDEC_INFO_H265_DEC_PRM_T *prDecPrm)
{
    if(prDecPrm->ucVDecStatus == REAL_DECODE)
    {
        //Check performance
        #ifdef VDEC_H265_Performance
        vVDec_HAL_Enable_CalcCLK(u4VDecID);
        HAL_GetTime(&prDecPrm->rTimeS);
        #endif
        
        if(prDecPrm->ucDualCoreMode == DUAL_CORE_MODE)
        {
            vVDecConfigMultiCore(u4VDecID,0);
            //dual core decode
            vVDecTrigMultiCore(u4VDecID,0,0,0);
        }
        else if(prDecPrm->ucDualCoreMode == DUAL_CORE_MODE)
        {
            //TBD not support
            ASSERT(0);
        }
        else
        {
            //single core decode
            vVDecWriteHEVCVLD( u4VDecID, HEVLD_PIC_TRG_REG, 0x1);
        }
    }
    else if(prDecPrm->ucVDecStatus == LAE_DECODE)
    {
        vVDecWriteHEVCVLD( u4VDecID, HEVLD_PIC_TRG_REG, 0x1);
    }
    return VDEC_HAL_HANDLE_OK;
}


// **************************************************************************
// Function : void v4VDEC_HAL_H265_GetMbxMby(UINT32 u4VDecID, UINT32 *pu4Mbx, UINT32 *pu4Mby);
// Description :Read current decoded mbx and mby
// Parameter : u4VDecID : video decoder hardware ID
//                 u4Mbx : macroblock x value
//                 u4Mby : macroblock y value
// Return      : None
// **************************************************************************
void vVDEC_HAL_H265_GetMbxMby(UINT32 u4VDecID, UINT32 *pu4Mbx, UINT32 *pu4Mby)
{
    *pu4Mbx = u4VDecReadMC(u4VDecID, RO_MC_MBX);
    *pu4Mby = u4VDecReadMC(u4VDecID, RO_MC_MBY);
}


// **************************************************************************
// Function : BOOL fgVDEC_HAL_H265_DecPicComplete(UINT32 u4VDecID);
// Description :Check if all video decoder modules are complete
// Parameter : u4VDecID : video decoder hardware ID
// Return      : TRUE: Decode complete, FALSE: Not yet
// **************************************************************************
BOOL fgVDEC_HAL_H265_DecPicComplete(UINT32 u4VDecID)
{
    if(u4VDecReadHEVCVLD(u4VDecID, RO_HEVLD_STATE_INFO) & HEVLD_PIC_FINISH)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// **************************************************************************
// Function : void u4VDEC_HAL_H265_GetErrMsg(UINT32 u4VDecID);
// Description :Read h265 error message after decoding end
// Parameter : u4VDecID : video decoder hardware ID
// Return      : H265 decode error message
// **************************************************************************
UINT32 u4VDEC_HAL_H265_GetErrMsg(UINT32 u4VDecID)
{
    return u4VDecReadHEVCVLD(u4VDecID, RO_HEVC_ERR_TYPE);
}


// **************************************************************************
// Function : void vVDEC_HAL_H265_VDec_SetPipComReg(UINT32 u4VDecID,H265_SPS_Data *prSPS)
// Description : Set common register for PIP decode
// Parameter : u4VDecID : video decoder hardware ID
// Return      : NONe
// **************************************************************************
void vVDEC_HAL_H265_VDec_SetPipComReg(UINT32 u4VDecID,H265_SPS_Data *prSPS)
{
    UINT32 u4PPStartReg,u4Index;
    u4Index = 0;

    //Common part 
    if((prSPS->u4PicWidthInLumaSamples <= 1920) && (prSPS->u4PicHeightInLumaSamples <= 1088))
    {
        for(u4PPStartReg = 45; u4PPStartReg < 53; u4PPStartReg++)
        {
            vVDecWriteHEVCPP(u4VDecID,(u4PPStartReg << 2),(u4VDecReadHEVCPP(u4VDecID,(u4PPStartReg << 2)) & 0xFFFF0000) | PIP_PP_ComSetting[u4Index]);
            u4Index ++;
        }

        for(u4PPStartReg = 705; u4PPStartReg < 708; u4PPStartReg++)
        {
            vVDecWriteHEVCPP(u4VDecID,(u4PPStartReg << 2),(u4VDecReadHEVCPP(u4VDecID,(u4PPStartReg << 2)) & 0xFFFF0000) | PIP_PP_ComSetting[u4Index]);
            u4Index ++;
        }

        vVDecWriteVLDTOP(u4VDecID,RW_VLD_TOP_PRED_SRAM_BASE,0x4000200);
        vVDecWriteHEVCPP(u4VDecID,HEVC_PP_SAO_DBK_SRAM_BASE,0x12C0000);
        //for test clear core0/core1 cpu auto clear
        //vVDecWriteHEVCMISC(u4VDecID,(41<<2),u4VDecReadHEVCMISC(u4VDecID,(41<<2)) | (1 << 28));
        if(u4VDecID == VDEC_VLD1)
        {
            vVDEC_HAL_H265_VDec_SetPipCore1Reg(u4VDecID);
        }
    }

}

void vVDEC_HAL_H265_VDec_SetPipCore1Reg(UINT32 u4VDecID)
{
    vVDecWriteVLDTOP(u4VDecID,RW_VLD_TOP_CORE1_SRAM_OFFSET,0x2080800);
    vVDecWriteHEVCPP(u4VDecID,HEVC_PP_DBK_SRAM_OFFSET,0x800);
}


void vVDEC_HAL_H265_VDec_DisableMCore(UINT32 u4VDecID)
{
    vVDecWriteMCORETOP(u4VDecID,RW_MCORE_EnableDecode,(u4VDecReadMCORETOP(u4VDecID, RW_MCORE_EnableDecode) & (~(1 << 0))));
}


void vVDEC_HAL_H265_SetStillImg(UINT32 u4VDecID,VDEC_INFO_H265_DEC_PRM_T *prH265DecPrm)
{
    vVDecWriteHEVCPP(u4VDecID, HEVC_SWITCH_DRAM_MODE, 0x1);
    vVDecWriteHEVCPP(u4VDecID, HEVC_SAO_WRAPPER_ADDR, PHYSICAL(prH265DecPrm->rStillImgInfo.u4StillImgSaoAddr));
    vVDecWriteHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_ADDR, PHYSICAL(prH265DecPrm->rStillImgInfo.u4StillImgDbkAddr));
    vVDecWriteHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_START_OFFSET, 0);
    vVDecWriteHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_HY_OFFSET, u4VDecReadHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_START_OFFSET) + 4224);
    vVDecWriteHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_HC_OFFSET, u4VDecReadHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_HY_OFFSET) + 4224);
    vVDecWriteHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_TILE_VY_OFFSET, u4VDecReadHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_HC_OFFSET) + 4224);
    vVDecWriteHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_TILE_VC_OFFSET, u4VDecReadHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_TILE_VY_OFFSET) + 2112);
    vVDecWriteHEVCPP(u4VDecID, HEVC_DBK_Y_FLAG, u4VDecReadHEVCPP(u4VDecID, HEVC_DBK_WRAPPER_TILE_VC_OFFSET) + 1056);
    vVDecWriteHEVCPP(u4VDecID, HEVC_DBK_TILE_HY_OFFSET, u4VDecReadHEVCPP(u4VDecID, HEVC_DBK_Y_FLAG) + 1056);
    vVDecWriteHEVCPP(u4VDecID, HEVC_DBK_PIC_WIDTH_ALIGN_HALF, 8448);

    vVDecWriteHEVCPP(u4VDecID, HEVC_SAO_HORIZONTAL_SIGN_OFFSET, 528);
    vVDecWriteHEVCPP(u4VDecID, HEVC_SAO_VERTICAL_SIGN_Y_OFFSET, 1584);
    vVDecWriteHEVCPP(u4VDecID, HEVC_SAO_VERTICAL_SIGN_C_OFFSET, 2112);
    vVDecWriteVLDTOP(u4VDecID, HEVC_SWITCH_VLD_WRAPPER_MODE, 0xc0000000);  
    vVDecWriteVLDTOP(u4VDecID, HEVC_NO_DRAM_BURST_MODE, 0x3);
    vVDecWriteVLDTOP(u4VDecID, HEVC_VLD_PRED_ADDR, PHYSICAL(prH265DecPrm->rStillImgInfo.u4StillImgPredAddr));
    vVDecWriteVLDTOP(u4VDecID, HEVC_VLD_IP_ADDR, PHYSICAL(prH265DecPrm->rStillImgInfo.u4StillImgIPAddr));
    vVDecWriteVLDTOP(u4VDecID, HEVC_VLD_VBUF_ADDR, PHYSICAL(prH265DecPrm->rStillImgInfo.u4StillImgVBAddr));
}

UINT32 u4VDEC_HAL_H265_VDec_ReadFinishFlag(UINT32 u4VDecID)
{
    //return ((u4VDecReadHEVCMISC(u4VDecID, RW_HEVC_DEC_COMPLETE)>>16) & 0x1);
    if(u4VDecID == 0)
    {
        return ((u4VDecReadMCORETOP(u4VDecID, 0xC) >> 16) & 0x1);    
    }
    else if(u4VDecID == 1)
    {
        return ((u4VDecReadMCORETOP(u4VDecID, 0xC) >> 17) & 0x1);
    }
    else if(u4VDecID == 2)
    {
        return ((u4VDecReadMCORETOP(u4VDecID, 0xC) >> 24) & 0x1);
    }
    else
    {
        return 0;
    }
}

void vVDEC_HAL_Set_ErrConceal(UINT32 u4InstID,UINT32 u4VDecID)
{
    UNUSED(u4InstID);
    vVDecWriteHEVCVLD(u4VDecID, HEVC_VLD_ERROR_TYPE_ENABLE, 0xfffefffb);
    vVDecWriteHEVCVLD(u4VDecID, HEVC_VLD_ERROR_HANDLING, 0x04011d01);   // 06172013, turn on slice_reconceal_sel
}

UINT32 u4VDEC_HAL_LAE_VDec_ReadFinishFlag(UINT32 u4VDecID)
{
    return ((u4VDecReadHEVCMISC(u4VDecID, RW_HEVC_DEC_COMPLETE)>>24) & 0x1);
}

UINT32 u4VDEC_HAL_MCORE_VDec_ReadFinishFlag(UINT32 u4VDecID)
{
    return ((u4VDecReadMCORETOP(u4VDecID, 0xC) >> 16) & 0x1);
}

UINT32 u4VDEC_HAL_H265_VDec_ClearInt(UINT32 u4VDecID)
{
    #ifdef VDEC_SIM_DUMP
    LOG(0,"[INFO] u4VDEC_HAL_H265_VDec_ClearInt() start!!\n");
    #endif
     //printk("Need check interrupt clean function %s\n",__FUNCTION__);
     //core0/core1 share the same clear int reg
     if(u4VDecID == 0)
     {
        vVDecWriteMCORETOP(u4VDecID,RO_LAE_STATUS,((u4VDecReadMCORETOP(u4VDecID, RO_LAE_STATUS) | ((1 << 16)))) & (~(1 << 17)) & (~(1 << 24)) & (~(1 << 25)));      
        vVDecWriteMCORETOP(u4VDecID,RO_LAE_STATUS,((u4VDecReadMCORETOP(u4VDecID, RO_LAE_STATUS) & (~(1 << 16))))& (~(1 << 17)) & (~(1 << 24)) & (~(1 << 25)));
     }
     else if(u4VDecID == 1)
     {
        vVDecWriteMCORETOP(u4VDecID,RO_LAE_STATUS,((u4VDecReadMCORETOP(u4VDecID, RO_LAE_STATUS) | ((1 << 17)))) & (~(1 << 16)) & (~(1 << 24)) & (~(1 << 25)));      
        vVDecWriteMCORETOP(u4VDecID,RO_LAE_STATUS,((u4VDecReadMCORETOP(u4VDecID, RO_LAE_STATUS) & (~(1 << 17))))& (~(1 << 16)) & (~(1 << 24)) & (~(1 << 25)));
     }
     #ifdef VDEC_SIM_DUMP
     LOG(0,"// [INFO] u4VDEC_HAL_H265_VDec_ClearInt() done!!\n");
     #endif
     return 1;
}


void vVDEC_HAL_H265_VDec_DumpReg(UINT32 u4VDecID,UINT32 u4VdecBaseAddr,UINT32 u4Start,UINT32 u4End)
{
    INT32 i,index = 0;
    UINT32 u4Value,u4Gap = 100;
    CHAR buffer[200];
    CHAR temp[200];
    u4VdecBaseAddr = u4VdecBaseAddr + (u4VDecID << 16);
    for(i = u4Start; i <= u4End; i++)
    {
        u4Value = u4ReadReg( u4VdecBaseAddr + i*4 );
        if ( u4VdecBaseAddr == VLD_REG_OFFSET0 )
            sprintf( buffer,"VLD[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == MC_REG_OFFSET0 )
            sprintf( buffer,"MC[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == HEVC_VLD_REG_OFFSET0 )
            sprintf( buffer,"HEVC_VLD[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == HEVC_PP_REG_OFFSET0 )
            sprintf( buffer,"PP[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == HEVC_MV_REG_OFFSET0 )
            sprintf( buffer,"MV[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == HEVC_MISC_REG_OFFSET0 )
            sprintf( buffer,"MISC[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == VLD_TOP_REG_OFFSET0 )
            sprintf( buffer,"VLD_TOP[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == VLD_REG_OFFSET1 )
            sprintf( buffer,"VLD1[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == MC_REG_OFFSET1 )
            sprintf( buffer,"MC1[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == HEVC_VLD_REG_OFFSET1 )
            sprintf( buffer,"HEVC_VLD1[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == HEVC_PP_REG_OFFSET1 )
            sprintf( buffer,"PP1[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == HEVC_MV_REG_OFFSET1 )
            sprintf( buffer,"MV1[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == HEVC_MISC_REG_OFFSET1 )
            sprintf( buffer,"MISC1[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == VLD_TOP_REG_OFFSET1 )
            sprintf( buffer,"VLD_TOP1[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);
        if ( u4VdecBaseAddr == MVDEC_TOP_OFFSET0 )
            sprintf( buffer,"VLD_MCore[%d] = 0x%08.0X    ",i,u4Value, u4VdecBaseAddr + 4*i);

        if(index || (u4End == i))
        {
            x_strcat(temp,buffer);
            LOG(0,"%s     \n", temp);
            index = 0;
        }
        else
        {
            x_strcpy(temp,buffer);
            index ++;
        }

        if(i - u4Start > u4Gap)
        {
            u4Gap = u4Gap + 100;
            x_thread_delay(500);
        }
    }
}

#if 0
extern INT32  Wait_decode_finished( unsigned long  start_time );
UINT32 vVDEC_HAL_H265_VDec_VPmode(UINT32 u4VDecID)
{
    UINT32 risc_val1, pic_width, pic_height;
    BOOL bIsUFO;  
    H265_SPS_Data* prSPS;

    bIsUFO = _tVerMpvDecPrm[u4VDecID].SpecDecPrm.rVDecH265DecPrm.fgIsUFOMode;
    prSPS = _tVerMpvDecPrm[u4VDecID].SpecDecPrm.rVDecH265DecPrm.prSPS;

    //VP mode for end of bitstream error
    risc_val1= u4VDecReadHEVCVLD(u4VDecID, 57*4);
    //if (_u4PicCnt[u4VDecID] >= 1){  risc_val1 = 1; }      // force frame num turn on VP mode for debug
    if ( risc_val1 & 0x1 ){
    
        UINT32 SliceStartLCURow, u4LCUsize, u4RealWidth, u4W_Dram;
        UINT32 pic_real_wifth, pic_real_height,i, minDPOC;
        UINT32 MC_130, MC_131, MC_608, VLD_TOP_26, VLD_TOP_28;
        VDEC_INFO_H265_INIT_PRM_T rH265VDecInitPrm;
        UINT8 ucRefFBIndex;

        //search min delta POC pic in DPB
        minDPOC = 9999999;
        for (i=0; i<_tVerMpvDecPrm[u4VDecID].SpecDecPrm.rVDecH265DecPrm.ucMaxFBufNum; i++ ){
            if (i ==_tVerMpvDecPrm[u4VDecID].ucDecFBufIdx){
                continue;
            }
            if (abs_hevc(_ptH265CurrFBufInfo[u4VDecID]->i4POC -_ptH265FBufInfo[u4VDecID][i].i4POC) < minDPOC){
                minDPOC = abs_hevc(_ptH265CurrFBufInfo[u4VDecID]->i4POC -_ptH265FBufInfo[u4VDecID][i].i4POC);
                ucRefFBIndex = i;
            } 
        }
        if (minDPOC==9999999){
            ucRefFBIndex= _tVerMpvDecPrm[u4VDecID].SpecDecPrm.rVDecH265DecPrm.ucPreFBIndex;
        }
               
        rH265VDecInitPrm.u4FGDatabase = (UINT32)_pucFGDatabase[u4VDecID];
        rH265VDecInitPrm.u4FGSeedbase = (UINT32)_pucFGSeedbase[u4VDecID];


        risc_val1= u4VDecReadHEVCVLD(u4VDecID, RW_HEVLD_SPS_SIZE);
        u4LCUsize = 1<<((risc_val1>>4) & 0x7);
        risc_val1= u4VDecReadHEVCVLD(u4VDecID, RO_VLD_VWPTR);
        SliceStartLCURow = (risc_val1>>16)  & 0x3ff;

        risc_val1= u4VDecReadVLDTOP(u4VDecID, HEVC_VLD_TOP_PIC_PIX_SIZE);  
        u4RealWidth = risc_val1 & 0xFFFF; 
        u4W_Dram = ((u4RealWidth + 63)/64)*64;
    
        if ((SliceStartLCURow%2)==1 && u4LCUsize==16 ){
            SliceStartLCURow--;
        }

        if (bIsUFO){    //UFO HW constrain
            while(SliceStartLCURow*u4LCUsize*u4W_Dram % (8*4096) !=0 ||((SliceStartLCURow%2)==1 && u4LCUsize==16 )){
                SliceStartLCURow--;
            }
        }       

        //SliceStartLCURow=0; //full frame copy test
        
        printk("// [INFO] VP mode!!  SliceStartLCURow %d; u4LCUsisze %d; refBufferIndex %d(pic #%d)\n", SliceStartLCURow, u4LCUsize, ucRefFBIndex,_ptH265FBufInfo[u4VDecID][ucRefFBIndex].u4PicCnt);

        pic_real_wifth = u4VDecReadMC(u4VDecID, HEVC_PIC_WIDTH);
        MC_130 =  ((pic_real_wifth+15)>>4)<<4;
        pic_real_height = u4VDecReadMC(u4VDecID, HEVC_PIC_HEIGHT);
        pic_real_height -= SliceStartLCURow*u4LCUsize;
        MC_131 = ((pic_real_height+15)>>4)<<4;
        MC_608 = u4VDecReadMC(u4VDecID, HEVC_DRAM_PITCH);
        VLD_TOP_26 = ((((pic_real_height+15)/16-1)& 0x7ff)<<16) |(((pic_real_wifth+15)/16-1)& 0x7ff);
        VLD_TOP_28 =  (((pic_real_height+15)>>4)<<20) | (((pic_real_wifth+15)>>4)<<4);
    
        i4VDEC_HAL_H265_InitVDecHW(u4VDecID);
    
#ifdef VDEC_SIM_DUMP
        printk ("// [INFO] VP UFO settings\n");
#endif
        // UFO mode settings
        if (bIsUFO){
            pic_width = ((pic_real_wifth + 63)>>6)<<6;
            pic_height = ((pic_real_height + 31)>>5)<<5;   
            vVDecWriteMC(u4VDecID, 700*4, ((pic_width/16-1)<<16) |(pic_height/16-1));      
            vVDecWriteMC(u4VDecID, 664*4, 0x11);
            vVDecWriteMC(u4VDecID, 698*4, PHYSICAL(_ptH265CurrFBufInfo[u4VDecID]->u4YLenStartAddr+(SliceStartLCURow*u4LCUsize*u4W_Dram/256)));
            vVDecWriteMC(u4VDecID, 699*4, PHYSICAL(_ptH265CurrFBufInfo[u4VDecID]->u4CLenStartAddr+(SliceStartLCURow*u4LCUsize*u4W_Dram/512)));   
            vVDecWriteMC(u4VDecID, 663*4, _ptH265CurrFBufInfo[u4VDecID]->u4PicSizeBS+(SliceStartLCURow*u4LCUsize*u4W_Dram/256)-SliceStartLCURow*u4LCUsize*u4W_Dram);
            vVDecWriteMC(u4VDecID, 701*4, _ptH265CurrFBufInfo[u4VDecID]->u4UFOLenYsize-(SliceStartLCURow*u4LCUsize*u4W_Dram/256)+(SliceStartLCURow*u4LCUsize*u4W_Dram/512));
            vVDecWriteMC(u4VDecID, 343*4, _ptH265CurrFBufInfo[u4VDecID]->u4PicSizeYBS-(SliceStartLCURow*u4LCUsize*u4W_Dram)/2);
            vVDecWriteHEVCPP(u4VDecID, 706*4, 0x1 );   // UFO garbage remove

            // bypass PP_out setting
            vVDecWriteMC(u4VDecID,139*4, ((pic_real_wifth+15)>>4));
            vVDecWriteMC(u4VDecID,152*4, ((pic_real_wifth+15)>>4)-1);
            vVDecWriteMC(u4VDecID,153*4, ((pic_real_height+15)>>4)-1);
            
            vVDecWriteMC(u4VDecID, 136*4, 0x1);
            risc_val1 = u4VDecReadMC(u4VDecID, 142*4);
            vVDecWriteMC(u4VDecID, 142*4, risc_val1 & (~0x3));
            vVDecWriteMC(u4VDecID, 148*4, 0x1);
            risc_val1 = u4VDecReadMC(u4VDecID, 525*4);
            vVDecWriteMC(u4VDecID, 525*4, risc_val1 & (~0x1));
            vVDecWriteMC(u4VDecID, 137*4, (PHYSICAL(_ptH265CurrFBufInfo[u4VDecID]->u4YStartAddr)+ u4W_Dram*SliceStartLCURow*u4LCUsize)>>9);
            vVDecWriteMC(u4VDecID, 138*4, (PHYSICAL(_ptH265CurrFBufInfo[u4VDecID]->u4YStartAddr + _ptH265CurrFBufInfo[u4VDecID]->u4CAddrOffset)+ u4W_Dram*SliceStartLCURow*u4LCUsize/2)>>8);

        }

#ifdef VDEC_SIM_DUMP
        printk ("// [INFO] VP settings\n");
#endif
        risc_val1= u4VDecReadVLDTOP(u4VDecID, 36*4);  
        vVDecWriteVLDTOP(u4VDecID, 36*4, risc_val1 | (0x1<<1) );  //Turn on VP mode flag

        vVDecWriteMC(u4VDecID, HEVC_PIC_WIDTH, MC_130);
        vVDecWriteMC(u4VDecID, HEVC_PIC_HEIGHT, MC_131);
        vVDecWriteMC(u4VDecID, HEVC_DRAM_PITCH, MC_608 );
        
        vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_PIC_MB_SIZE_M1, VLD_TOP_26);  
        vVDecWriteVLDTOP(u4VDecID, HEVC_VLD_TOP_PIC_PIX_SIZE, VLD_TOP_28);  

        vVDecWriteMC(u4VDecID, 9*4, 0x1);

        vVDecWriteMC(u4VDecID, 0*4, (PHYSICAL(_ptH265FBufInfo[u4VDecID][ucRefFBIndex].u4YStartAddr) + u4W_Dram*SliceStartLCURow*u4LCUsize)>>9);
        vVDecWriteMC(u4VDecID, 1*4, (PHYSICAL(_ptH265FBufInfo[u4VDecID][ucRefFBIndex].u4YStartAddr + _ptH265FBufInfo[u4VDecID][ucRefFBIndex].u4CAddrOffset)+ u4W_Dram*SliceStartLCURow*u4LCUsize/2)>>8);
        vVDecWriteMC(u4VDecID, 2*4, (PHYSICAL(_ptH265CurrFBufInfo[u4VDecID]->u4YStartAddr)+ u4W_Dram*SliceStartLCURow*u4LCUsize)>>9);
        vVDecWriteMC(u4VDecID, 3*4, (PHYSICAL(_ptH265CurrFBufInfo[u4VDecID]->u4YStartAddr + _ptH265CurrFBufInfo[u4VDecID]->u4CAddrOffset)+ u4W_Dram*SliceStartLCURow*u4LCUsize/2)>>8);

        risc_val1= u4VDecReadVLDTOP(u4VDecID, 36*4);  
        vVDecWriteVLDTOP(u4VDecID, 36*4,  risc_val1 |0x1 );  // Trigger VP mode 

  
        risc_val1 = u4VDecReadHEVCMISC(u4VDecID, RW_HEVC_DEC_COMPLETE);
        vVDecWriteHEVCMISC (u4VDecID, RW_HEVC_DEC_COMPLETE, risc_val1 & (~(0x1<<12)) ) ;
    
        return Wait_decode_finished( jiffies );
    }
    
    _u4CurrPicStartAddr[1]  = 0;
    return 0;
}


extern int Dump_reg( UINT32 base_r, UINT32 start_r, UINT32 end_r , char* pBitstream_name , UINT32 frame_number, BOOL bDecodeDone);
UINT32 u4BaseReg[2][8] = 
{
    HEVC_VLD_REG_OFFSET0,
    VLD_REG_OFFSET0,
    AVC_MV_REG_OFFSET0,
    MC_REG_OFFSET0,
    HEVC_PP_REG_OFFSET0,
    VLD_TOP_REG_OFFSET0,
    VDEC_MISC_BASE0,
    MVDEC_TOP_OFFSET0,
//Inst1 base addr
    HEVC_VLD_REG_OFFSET1,
    VLD_REG_OFFSET1,
    AVC_MV_REG_OFFSET1,
    MC_REG_OFFSET1,
    HEVC_PP_REG_OFFSET1,
    VLD_TOP_REG_OFFSET1,
    VDEC_MISC_BASE1,
    MVDEC_TOP_OFFSET0
};
void vVDEC_HAL_H265_VDec_DumpReg(UINT32 u4VDecID, BOOL bDecodeDone)
{
    char pBitstream_name[200] = {0};

    memcpy (pBitstream_name , _bFileStr1[u4VDecID][1]+12 , (strlen(_bFileStr1[u4VDecID][1]) -38) );
    pBitstream_name[(strlen(_bFileStr1[u4VDecID][1]) -38)] = '\0';

    printk("[INFO] Dump register for %s #%d\n", pBitstream_name,_u4PicCnt[u4VDecID] );
    //because read HEVC_VLD register will make system exception,print some of those status register.
    if(bDecodeDone)
    {
        printk("// decode end! \n");
        #if 0
        Dump_reg (HEVC_VLD_REG_OFFSET0, 0, 0, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //HEVC
        Dump_reg (HEVC_VLD_REG_OFFSET0, 33, 37, pBitstream_name, _u4PicCnt[u4VDecID], 1);  //HEVC
        Dump_reg (HEVC_VLD_REG_OFFSET0, 40, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);     //HEVC
        Dump_reg (VLD_REG_OFFSET0, 33, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //VLD
        Dump_reg (AVC_MV_REG_OFFSET0, 0, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MV
        Dump_reg (MC_REG_OFFSET0, 0, 827, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MC
        Dump_reg (HEVC_PP_REG_OFFSET0, 0, 1023, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //PP
        Dump_reg (VLD_TOP_REG_OFFSET0, 0, 128, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //VLD_TOP
        #else
        Dump_reg (u4BaseReg[u4VDecID][0], 0, 0, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //HEVC
        Dump_reg (u4BaseReg[u4VDecID][0], 33, 37, pBitstream_name, _u4PicCnt[u4VDecID], 1);  //HEVC
        Dump_reg (u4BaseReg[u4VDecID][0], 40, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);     //HEVC
        Dump_reg (u4BaseReg[u4VDecID][1], 33, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //VLD
        Dump_reg (u4BaseReg[u4VDecID][2], 0, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MV
        Dump_reg (u4BaseReg[u4VDecID][3], 0, 827, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MC
        Dump_reg (u4BaseReg[u4VDecID][4], 0, 1023, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //PP
        Dump_reg (u4BaseReg[u4VDecID][5], 0, 128, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //VLD_TOP
        Dump_reg (u4BaseReg[u4VDecID][6], 0, 128, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //MISC
        Dump_reg (u4BaseReg[u4VDecID][7], 0, 127, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //PP
        #endif
        
        if(_u4DualCoreEnable[u4VDecID] == 1)
        {
            printk("// Dump core1 register now \n");
            Dump_reg (HEVC_VLD_REG_OFFSET1, 0, 0, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //HEVC
            Dump_reg (HEVC_VLD_REG_OFFSET1, 33, 37, pBitstream_name, _u4PicCnt[u4VDecID], 1);  //HEVC
            Dump_reg (HEVC_VLD_REG_OFFSET1, 40, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);     //HEVC
            Dump_reg (VLD_REG_OFFSET1, 33, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //VLD
            Dump_reg (AVC_MV_REG_OFFSET1, 0, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MV
            Dump_reg (MC_REG_OFFSET1, 0, 827, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MC
            Dump_reg (HEVC_PP_REG_OFFSET1, 0, 1023, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //PP
        }
    }
    else
    {
        printk("// decode start!\\n");
        #if 0
        Dump_reg (HEVC_VLD_REG_OFFSET0, 0, 0, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //HEVC
        Dump_reg (HEVC_VLD_REG_OFFSET0, 33, 37, pBitstream_name, _u4PicCnt[u4VDecID], 1);  //HEVC
        Dump_reg (HEVC_VLD_REG_OFFSET0, 40, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);     //HEVC
        Dump_reg (VLD_REG_OFFSET0, 33, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //VLD
        Dump_reg (AVC_MV_REG_OFFSET0, 0, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MV
        Dump_reg (MC_REG_OFFSET0, 0, 827, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MC
        Dump_reg (HEVC_PP_REG_OFFSET0, 0, 1023, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //PP
        Dump_reg (VLD_TOP_REG_OFFSET0, 0, 128, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //VLD_TOP
        #else
        Dump_reg (u4BaseReg[u4VDecID][0], 0, 0, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //HEVC
        Dump_reg (u4BaseReg[u4VDecID][0], 33, 37, pBitstream_name, _u4PicCnt[u4VDecID], 1);  //HEVC
        Dump_reg (u4BaseReg[u4VDecID][0], 40, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);     //HEVC
        Dump_reg (u4BaseReg[u4VDecID][1], 33, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //VLD
        Dump_reg (u4BaseReg[u4VDecID][2], 0, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MV
        Dump_reg (u4BaseReg[u4VDecID][3], 0, 827, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MC
        Dump_reg (u4BaseReg[u4VDecID][4], 0, 1023, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //PP
        Dump_reg (u4BaseReg[u4VDecID][5], 0, 128, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //VLD_TOP
        Dump_reg (u4BaseReg[u4VDecID][6], 0, 128, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //MISC
        Dump_reg (u4BaseReg[u4VDecID][7], 0, 127, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //PP
        #endif

        if(_u4DualCoreEnable[u4VDecID] == 1)
        {
            printk("// Dump core1 register now \n");
            Dump_reg (HEVC_VLD_REG_OFFSET1, 0, 0, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //HEVC
            Dump_reg (HEVC_VLD_REG_OFFSET1, 33, 37, pBitstream_name, _u4PicCnt[u4VDecID], 1);  //HEVC
            Dump_reg (HEVC_VLD_REG_OFFSET1, 40, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);     //HEVC
            Dump_reg (VLD_REG_OFFSET1, 33, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);      //VLD
            Dump_reg (AVC_MV_REG_OFFSET1, 0, 255, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MV
            Dump_reg (MC_REG_OFFSET1, 0, 827, pBitstream_name, _u4PicCnt[u4VDecID], 1);        //MC
            Dump_reg (HEVC_PP_REG_OFFSET1, 0, 1023, pBitstream_name, _u4PicCnt[u4VDecID], 1);       //PP
        }
    }
    
}
#endif
