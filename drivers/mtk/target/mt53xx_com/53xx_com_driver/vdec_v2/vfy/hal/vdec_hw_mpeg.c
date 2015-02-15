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
//#include "vdec_hw_common.h"
//#include "vdec_hw_mpeg.h"
//#include "x_typedef.h"

#include "vdec_hw_common.h"

#include "vdec_info_mpeg.h"

#include "vdec_hw_mpeg.h"

#include "vdec_info_h264.h"

#include "vdec_hw_h264.h"


#ifdef CONFIG_TV_DRV_VFY 
#include "x_hal_5381.h"
#include "x_hal_926.h"
#else
#include "x_hal_ic.h"
#include "x_hal_1176.h"
#endif // CONFIG_TV_DRV_VFY

#include "x_debug.h"

#if CONFIG_DRV_VERIFY_SUPPORT
#include "vdec_verify_general.h"
#include "vdec_verify_mpv_prov.h"
#include "vdec_hal_if_mpeg.h"

#if (!CONFIG_DRV_LINUX)
#include <string.h>
#include <stdio.h>
#endif

extern void vVDecOutputDebugString(const CHAR * format, ...);
extern BOOL fgWrMsg2PC(void* pvAddr, UINT32 u4Size, UINT32 u4Mode, VDEC_INFO_VERIFY_FILE_INFO_T *pFILE_INFO);
extern void vVDecOutputDebugString(const CHAR * format, ...);
#else

#include "vdec_drv_mpeg4_info.h"

#ifdef VDEC_MPEG4_SUPPORT_RESYNC_MARKER

extern UINT32 _u4ResyncMarkMbx[2][2];
extern UINT32 _u4ResyncMarkMby[2][2];
extern UINT32 _u4ResyncMarkerCnt[2];

#endif

#endif

#ifdef CONFIG_TV_DRV_VFY 
#ifdef MPEG4_NEW_MODE
extern void vVDecWriteAVCMV(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val);
extern UINT32 u4VDecReadAVCMV(UINT32 u4VDecID, UINT32 u4Addr);
extern void vVDecWriteAVSPP(UINT32 u4VDecID, UINT32 u4Addr, UINT32 u4Val);
extern UINT32 u4VDecReadAVSPP(UINT32 u4VDecID,UINT32 u4Addr);
#endif
#endif

void vVLDDec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm);
void vVLDDoDec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgBPic,
                      BOOL fgB21FrmPic);
void vVLDDecPrmProc(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm);
void vVLDSetPicType(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm);
void vVLDSetPicSz(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgB21FrmPic);

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
void vVLDMp4SetPicSz(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgB21FrmPic);//mtk40343 9/7/2010
#endif

UINT32 u4VLDPicH(UCHAR ucMpegVer, UINT32 u4PicH);
void vVLDSetMBRowPrm(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgBPic, BOOL fgFrmPic);
void vVLDM4vDec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgBVop);
void vVLDM4vDecPrmProc(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm);
void vVLDMp4TriggerDec(UINT32 u4VDecID);
void vVLDMp4DecPrmProc(UINT32 u4VDecID, VDEC_INFO_MPEG4_DEC_PRM_T *prMp4Prm,
                                UINT32 u4MBx,
                                UCHAR ucIntraDcVlcThr,
                                UCHAR ucQuarterSample,
                                UCHAR ucDataPartitioned);
void vVLDBarl2Vdec(UINT32 u4VDecID);
void vVLDSetDcacAddr(UINT32 u4VDecID, UINT32 dAddr);
void vVLDDx3Dec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm);
void vVLDDx3DecPrmProc(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm);
void vSetDecFld(UINT32 u4VDecID,VDEC_INFO_DEC_PRM_T *prDecPrm);
void vMCSetDecFld(UINT32 u4VDecID, UINT32 u4DecFld, UINT32 u42ndFldSw);
void vSetMcDecBuf(UINT32 u4VDecID,VDEC_INFO_DEC_PRM_T *prDecPrm);
void vMCWriteToDigBuf(UINT32 u4VDecID, BOOL fgSwitch);
void vVLDSetBRef(UINT32 u4VDecID, BOOL fgPVop);
void vMCSetBRef(UINT32 u4VDecID,BOOL fgIVop);
void VDecDumpMpegRegister(UINT32 u4VDecID);

UINT32 u4VDecReadMP4PP(UINT32 u4VDecID, UINT32 u4Addr);

#ifdef DUMP_REG
void VDec_DumpReg(UCHAR ucMpvId);
#endif

// *********************************************************************
// Function :void vVLDDec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
// Description : Decode Picture
// Parameter : prDecPrm: Decode Parameters Set
// Return    : None
// *********************************************************************
void vVLDDec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
    VDEC_INFO_MPEG2_PIC_PRM_T *prMp2;
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif
    
    
    vVLDDecPrmProc(u4VDecID, prDecPrm);
    vVLDSetPicType(u4VDecID, prDecPrm);
  
    prMp2 = &(prMpegDecPrm->rPicLayer.rMp2PicPrm);
  
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8561 )
  
    if(prMp2->ucPicStruct != FRM_PIC)
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_SCALE, 0);
    }
#endif
     #ifdef MPEG2_THRESHOLD_TEST
     //vVDecWriteDV(u4VDecID, (41 << 2), (u4VDecReadDV(u4VDecID,(41 << 2)) |0x1)); 
     UTIL_Printf("Beforer %d frame,the rptr is  0x%x!!! \n", _u4FileCnt[u4VDecID], (u4VDecReadVLD(u4VDecID, 0xFC) - u4VDecReadVLD(u4VDecID, 0xB0))); //sun for temp
     UTIL_Printf("DV(0xA4)  = 0x%4x\n",u4VDecReadDV(u4VDecID, 0xA4));
     UTIL_Printf("VLD(0x14C)  = 0x%4x\n",u4VDecReadVLD(u4VDecID, 0x14C));    
     UTIL_Printf("VLD(0x150)  = 0x%4x\n",u4VDecReadVLD(u4VDecID, 0x150));   
     UTIL_Printf("VLD(0xFC)  = 0x%4x\n",u4VDecReadVLD(u4VDecID, 0xFC));
     UTIL_Printf("VLD(0xB0)  = 0x%4x\n",u4VDecReadVLD(u4VDecID, 0xB0));
     #endif
  
    vVLDDoDec(u4VDecID, prDecPrm, (prMp2->ucPicCdTp == B_TYPE),
                (prMpegDecPrm->fgB21Mode && (prMp2->ucPicStruct == FRM_PIC)));
}

void VDec_DumpMpeg4Reg(UINT32 u4VDecID)
{
    UINT32 u4Val;
    int reg;

    UTIL_Printf("VLD Settings\n");

    for (reg = 33; reg <= 39; reg ++)
    {
        u4Val = u4VDecReadVLD(u4VDecID, (reg << 2));
        UTIL_Printf("%d (0x%x) = 0x%4x\n", reg, (reg<<2), u4Val);
    }

    for (reg = 42; reg <= 70; reg ++)
    {
        u4Val = u4VDecReadVLD(u4VDecID, (reg << 2));
        UTIL_Printf("%d (0x%x) = 0x%4x\n", reg, (reg<<2), u4Val);
    }

    for (reg = 112; reg <= 130; reg ++)
    {
        u4Val = u4VDecReadVLD(u4VDecID, (reg << 2));
        UTIL_Printf("%d (0x%x) = 0x%4x\n", reg, (reg<<2), u4Val);
    }

    for (reg = 189; reg <= 255; reg ++)
    {
        u4Val = u4VDecReadVLD(u4VDecID, (reg << 2));
        UTIL_Printf("%d (0x%x) = 0x%4x\n", reg, (reg<<2), u4Val);
    }
    
    UTIL_Printf("MC Settings\n");
    
    for(reg = 0; reg <= 511; reg++)
    {          
        u4Val = u4VDecReadMC(u4VDecID, (reg << 2));
        UTIL_Printf("%d (0x%x) = 0x%4x\n", reg, (reg<<2), u4Val);
    }
}

extern BOOL _VDecNeedDumpRegister(UINT32 u4VDecID);

// *********************************************************************
// Function :void vVLDDoDec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgBPic, BOOL fgB21FrmPic)
// Description : Decode Picture
//               For Mpeg1/2, this is the last function to call to start decoding
//               For DivX3/Mpeg4, we have to call vVLDMp4TriggerDec() after this.
// Parameter : prDecPrm: Decode Parameters Set
//             fgBPic: This picture is a B-Pic
//             fgB21FrmPic: B_1/2 mode and frame pic
// Return    : None
// *********************************************************************
void vVLDDoDec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgBPic, BOOL fgB21FrmPic)
{
    UINT32 u4Tmp;
    UINT32 u4Proc = 0,u4MBend = 0;
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
    UINT32 u4PicW = 0,u4PicH = 0;
    #endif

	
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif
    
    vVDecWriteVLD(u4VDecID, RW_VLD_VDOUFM, VLD_VDOUFM + VLD_ERR_MONITOR);

#if (!CONFIG_DRV_FPGA_BOARD)
    vVDecWriteMC(u4VDecID, RW_MC_MODE_CTL, MC_QIU_BANK4|MC_QIU_BANK8|MC_DRAM_REQ_DELAY_1T|MC_DRAM_REQ_MERGE_OFF|MC_MV_MERGE_OFF);
  #endif  
    u4Proc |= VLD_PDHW;
    if (fgBPic)
    {
        if (prMpegDecPrm->fgIgnoreVdo)
        {
            u4Proc|= VLD_IGBCL;
        }
    #ifdef HW_BUG_VLDPROC
        u4Proc|= VLD_IGBCL;
    #endif
        
        if (fgB21FrmPic)
        {
            u4Proc |= VLD_B21EN;
            if (prMpegDecPrm->ucDecFld == MC_TOP_FLD)
            {
                u4Proc |= VLD_DECTOP;
            }
            else
            {
                u4Proc |= VLD_DECBTM;
            }
        }
        // check Partial Decode
        if (prMpegDecPrm->u4DecXOff != 0)
        {
            u4Tmp = ((prMpegDecPrm->u4DecXOff >> 4) << 8) +
                    ((prMpegDecPrm->u4DecXOff + prMpegDecPrm->u4DecW) >> 4);
            if (((prMpegDecPrm->u4DecXOff + prMpegDecPrm->u4DecW) & 0xF) != 0)
            {
                u4Tmp += 1;
            }
	     u4MBend = u4Tmp;
        }
        else
        {
            u4Tmp = (prMpegDecPrm->u4DecW >> 4);
            if ((prMpegDecPrm->u4DecW & 0xF) != 0)
            {
                u4Tmp += 1;
            }
	     u4MBend = u4Tmp;
        }
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
	#else
        u4Proc |= u4Tmp;
	#endif
    }
    else
    {
        // I, P shall decode whole picture
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
	u4MBend = prDecPrm->u4PicW >> 4;
	#else
        u4Proc |= (prDecPrm->u4PicW >> 4);
	#endif
    }
  
    if (prMpegDecPrm->fgRetErr)
    {
        u4Proc |= VLD_RTERR;
    }

  #ifdef RET_IF_ERR
    u4Proc |= VLD_RTERR;
  #endif
  
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) 
	u4PicW = ((prDecPrm->u4PicW +15)>>4)<<4;
	if(prDecPrm->ucPicStruct == FRM_PIC)
	{
		u4PicH = ((prDecPrm->u4PicH + 15)>>4)<<4;
	}
	else
	{
		u4PicH = (((prDecPrm->u4PicH + 15)>>4)<<4)>>1;
	}

//	vVDecWriteVLD(u4VDecID, RW_VLD_PIC_SIZE, u4PicH<<16|u4PicW);
	
       vVDecWriteVLDTOP(u4VDecID,RW_TOPVLD_WMV_PICSIZE,u4PicH<<16|u4PicW);
	vVDecWriteVLD(u4VDecID, RW_VLD_DIGMBSA, u4MBend);
	vVDecWriteMC(u4VDecID,RW_MC_MVDCAC_SEL,u4VDecReadMC(u4VDecID,RW_MC_MVDCAC_SEL) | (1<<20));
#endif

    // prevent hw gen interrupt before last frame write in dram mtk40343
    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8560)
    vVDecWriteMC(u4VDecID, 0x834, u4VDecReadMC(u4VDecID, 0x834)&(~(0x1<<4)));
    #endif



#if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8520)
    if ((prDecPrm->rDownScalerPrm.fgDSCLEn) || (prMpegDecPrm->rMpegPpInfo.fgPpEnable))
    {
        // turn off error concealment because VDSCL and PP Wrapper can not work with it
        u4Proc |= VLD_PERRCON;
    }
#else
    if (prMpegDecPrm->rMpegPpInfo.fgPpEnable)
    {
        // turn off error concealment because PP Wrapper can not work with it        
        //u4Proc |= VLD_PERRCON;
        //UTIL_Printf("MPV keep error concealment on when PP enable\n");   
        //after mt8530, if  enable PP, must enable error concealment to decode an error bitstream 
        //fix CQ BDP00124291
    }
#if !CONFIG_DRV_VERIFY_SUPPORT /*&& !CONFIG_FPGA_BOARD*/
    {
        VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
        
        if (VDEC_MPEG1 == prMpegDecPrm->ucMpegVer)
        {
            //mpeg1 does not support error concealment
            UINT32 u4Reg = u4VDecReadVLD(u4VDecID, 156 * 4);
            u4Reg |= 0x1400D2;
            vVDecWriteVLD(u4VDecID, 156 * 4, u4Reg);
        }
    }
#endif
#endif

#if !CONFIG_DRV_VERIFY_SUPPORT /*&& !CONFIG_FPGA_BOARD*/
    if (_VDecNeedDumpRegister(u4VDecID))
    {
        UTIL_Printf("MPV Dump register before decode start\n");
        VDecDumpMpegRegister(u4VDecID);
        UTIL_Printf("\n MPV Dump register end \n");
    }
#endif

#ifdef DUMP_REG
    VDec_DumpReg(u4VDecID);
#endif
    #if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560) && (CONFIG_CHIP_VER_CURR <  CONFIG_CHIP_VER_MT8580 && CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT8561))
    vVDecWriteMC(u4VDecID,RW_MC_DDR3_EN,(u4VDecReadMC(u4VDecID, RW_MC_DDR3_EN) & (~(1<<4))));
    #endif
    

    //mtk40110 Qing.Li 2010/11/25, to fix mpeg4 DCAC Pred bug
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
    if (2 == prDecPrm->ucMpegSpecType)
    {
        #if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
        vVDecWriteVLD(u4VDecID, RW_VLD_DCACWK, 0x21);
        #else
        vVDecWriteVLD(u4VDecID, RW_VLD_DCACWK, 0x20);
        #endif
        vVDecWriteVLD(u4VDecID, RW_VLD_DCMVSEL, 0xb03008f6);
    }
    else if (3 == prDecPrm->ucMpegSpecType)
    {
        #if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 ||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890))
        vVDecWriteVLD(u4VDecID, RW_VLD_DCACWK, 0x21);
        #else
        vVDecWriteVLD(u4VDecID, RW_VLD_DCACWK, 0x20);
        #endif
        vVDecWriteVLD(u4VDecID, RW_VLD_DCMVSEL, 0x903008f2);
    }
    prDecPrm->ucMpegSpecType = 0;
#endif

#if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)  //xiaolei.li  for 8561eco emulation
//UTIL_Printf("ucMpegVer is %d\n",prMpegDecPrm->ucMpegVer);
if(VDEC_MPEG4 == prMpegDecPrm->ucMpegVer)  //only for MPEG4, except DIVX3 & Short header & Sorenson h263 xiaolei.li
{
	vVDecWriteVLD(u4VDecID,RW_VLD_DCACWK,(u4VDecReadVLD(u4VDecID,RW_VLD_DCACWK)|(1<<4)));
	//UTIL_Printf("VLD_127 = 0x%x\n",u4VDecReadVLD(u4VDecID,RW_VLD_DCACWK));
}
else if(VDEC_MPEG2 == prMpegDecPrm->ucMpegVer)
{
	//vWriteReg((0x2c000 + 0x200), (u4ReadReg(0x2c000 + 0x200)|(15 << 14))); // write PP_128[17:14] =1 to roll back mpeg2 error concealment 
	//UTIL_Printf("PP_128 = 0x%x\n",u4ReadReg(0x2c000 + 0x200));
}
#endif

#if PARA8_BEHAVIOR_TEST
vVDecWriteMC(u4VDecID,0x998,u4VDecReadMC(u4VDecID,0x998)|(1<<4)); // trigger MC_para8 mode. remy emulation xiaolei.li
#endif
/*enable crc check by sun*/
#ifdef MPEG4_CRCCHECK_ENABLE
    vWriteReg((VDEC_CRC_REG_OFFSET0 + 0x4), 0x01);
#if (VDEC_PP_ENABLE)  //switch to caculate PP out CRC by lu.sun
   // vVDecWriteDV(u4VDecID, 0x4, (u4VDecReadDV(u4VDecID, 0x4)|0x2));//DV is replaced by MISC_BASE, and for mpeg4 PP if use pixel by pixel, this is not needed.
                                                                                                                                                            //vVDecProc():vVDecEnableCRC(u4InstID, 1, 1) choose not use PP crc
#endif
#endif
    //ADD L2NBM error recovery type = restart by next frame by lu.sun
    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890  || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) 
    if((VDEC_MPEG1 == prMpegDecPrm->ucMpegVer)||(VDEC_MPEG2 == prMpegDecPrm->ucMpegVer))
    {
        vVDecWriteMC(u4VDecID,RW_MC_L2NBM_ER_TYPE,(u4VDecReadMC(u4VDecID, RW_MC_L2NBM_ER_TYPE) | (0x3<<4))); 
    } 
    #endif
  
  #if VDEC_UFO_ENABLE   //here, bypass pp out(UFO need to turn on pp out but bypass it)
    UINT32 u4Reg;
    u4Reg = u4VDecReadMC(u4VDecID, RW_MC_PP_ENABLE); //MC#136 [0]  ' 1  turn on PP out 
    printk("MC#136 %x\n",u4Reg);
    u4Reg = u4VDecReadMC(u4VDecID, RW_MC_PP_WB_BY_POST); //MC#148 [0]  ' 1  turn off MC out
    printk("MC#148 %x\n",u4Reg);    
    u4Reg = u4VDecReadMC(u4VDecID, 0x834);// pp out buffer enable
    printk("MC#525 %x\n",u4Reg);
    u4Reg = u4VDecReadMC(u4VDecID, RW_MC_PP_DBLK_MODE); //MC#136 [0]  ' 1  turn on PP out 
    printk("MC#238 %x\n",u4Reg);
    u4Reg = u4VDecReadMC(u4VDecID, 664*4);
    printk("MC#664 %x\n",u4Reg);
#endif    

   #ifdef VDEC_SIM_DUMP
   if(VDEC_MPEG1 == prMpegDecPrm->ucMpegVer || VDEC_MPEG2 == prMpegDecPrm->ucMpegVer)
    {
        UTIL_Printf("===>input window: 0x %08x\n", u4VDecReadVLD(u4VDecID,0xf0));  
    }
    UTIL_Printf("===>Start trigger HW decode !!!!!!!\n");  
   #endif

#if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
           {
               HAL_GetTime(&rTimeStartDec[u4VDecID]);
               UTIL_Printf("Start_%d:second:ms %08d:%08d\n",u4VDecID,rTimeStartDec[u4VDecID].u4Seconds,rTimeStartDec[u4VDecID].u4Micros);
           }
#endif

    // Process Decode
    #if ((CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890)  || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) 
    if((VDEC_MPEG1 == prMpegDecPrm->ucMpegVer)||(VDEC_MPEG2 == prMpegDecPrm->ucMpegVer))
    #endif
        vVDecWriteVLD(u4VDecID, RW_VLD_PROC, u4Proc);
    

}


// *********************************************************************
// Function :void vVLDDecPrmProc(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
// Description : Process on decode related parameters
// Parameter : prDecPrm: decode related parameters
// Return    : None
// *********************************************************************
void vVLDDecPrmProc(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
    UINT32 u4RegVal1;
    VDEC_INFO_MPEG2_PIC_PRM_T *prMp2Prm;
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif

#if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8555)
	 if (BSP_GetIcVersion() == IC_8555)
        {
     if(u4VDecID == 1)
     {
         vWriteReg(0x32000,(u4ReadReg(0x32000) | 0x1));
     }
     else
     {
        vWriteReg(0x2B000,(u4ReadReg(0x2B000) | 0x1));
     }
        }
#endif
    vVDecWriteVLD(u4VDecID, RW_VLD_MP4_FLG, 0);
#if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8555)
	if (BSP_GetIcVersion() == IC_8555)
	{
     if(u4VDecID == 1)
     {
        vWriteReg(0x32000,(u4ReadReg(0x32000) & (~0x1)));
     }
     else
     {
        vWriteReg(0x2B000,(u4ReadReg(0x2B000) & (~0x1)));
     }
       }
#endif
     //vVDecWriteVLD(u4VDecID, RW_VLD_MP4_FLG, 0);
  
    prMp2Prm = &(prMpegDecPrm->rPicLayer.rMp2PicPrm);
    u4RegVal1 = (prMp2Prm->ucPicStruct << 30) +
               (prMp2Prm->ucFrmPredFrmDct << 29) +
               (prMp2Prm->ucConcMotVec << 28) +
               (prMp2Prm->ucQScaleType << 27) +
               (prMp2Prm->ucTopFldFirst << 26) +
               (prMp2Prm->ucFullPelFordVec << 25) +
               (prMp2Prm->ucFullPelBackVec << 24) +
               (prMp2Prm->ucPicCdTp << 21) +
               (prMp2Prm->ucIntraVlcFmt << 20) +
               (prMp2Prm->ucIntraDcPre << 18) +
               (prMp2Prm->ucAltScan << 17);
  
    if (prMpegDecPrm->ucMpegVer == 1)
    {
        u4RegVal1 = u4RegVal1 +
                 (prMp2Prm->ucFordFCode << 12) +
                 (prMp2Prm->ucFordFCode << 8) +
                 (prMp2Prm->ucBackFCode << 4) +
                 (prMp2Prm->ucBackFCode);
    }
    else
    {
        u4RegVal1 = u4RegVal1 +
                   (prMp2Prm->pucfcode[0][0] << 12) +
                   (prMp2Prm->pucfcode[0][1] << 8) +
                   (prMp2Prm->pucfcode[1][0] << 4) +
                   (prMp2Prm->pucfcode[1][1]);
    }
    vVDecWriteVLD(u4VDecID, RW_VLD_PARA, u4RegVal1);
  
    //setting of the dwMaxMblMod
    vVLDSetPicSz(u4VDecID, prDecPrm, ((prMp2Prm->ucPicCdTp == B_TYPE) &&
                            (prMp2Prm->ucPicStruct == FRM_PIC) &&
                             prMpegDecPrm->fgB21Mode));
    vVLDSetMBRowPrm(u4VDecID, prDecPrm, (prMp2Prm->ucPicCdTp == B_TYPE),
                              (prMp2Prm->ucPicStruct == FRM_PIC));

	//For fix letterbox detect
    #if ((CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8580) || (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) ||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890))
		vVDecWriteVLDTOP(u4VDecID,RW_TOPVLD_WMV_PICSIZE_MB,((((prDecPrm->u4PicH+ 15) >> 4) - 1) << 16)|(((prDecPrm->u4PicW + 15) >> 4) - 1));
    #endif  
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_WIDTH, ((prDecPrm->u4PicW + 15) >> 4) << 4);
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_HEIGHT, ((prDecPrm->u4PicH + 15) >> 4) << 4);                         
}


void vVLDSetPicType(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
    UINT32 u4Value = 0;
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif

    switch (prMpegDecPrm->ucMpegVer)
    {
        case VDEC_MPEG1:
        case VDEC_MPEG2:
            u4Value = 1 << 4;
            break;
        case VDEC_DIVX3:
            u4Value = 3 << 5;
            break;
        case VDEC_MPEG4:
            u4Value = 1 << 6;
            break;
        default:
            return;
    }
    if ((VDEC_MPEG1 == prMpegDecPrm->ucMpegVer) || (VDEC_MPEG2 == prMpegDecPrm->ucMpegVer))
    {
        VDEC_INFO_MPEG2_PIC_PRM_T *prMp2Prm = &(prMpegDecPrm->rPicLayer.rMp2PicPrm);
        if (I_TYPE == prMp2Prm->ucPicCdTp)
            u4Value |= (1 << 0);
        else if (P_TYPE == prMp2Prm->ucPicCdTp)
            u4Value |= (1 << 1);
        else if (B_TYPE == prMp2Prm->ucPicCdTp)
            u4Value |= (1 << 2);
    }
    else if ((VDEC_DIVX3 == prMpegDecPrm->ucMpegVer) || (VDEC_MPEG4 == prMpegDecPrm->ucMpegVer))
    {
        VDEC_INFO_MPEG4_DEC_PRM_T *prMp4Prm = &(prMpegDecPrm->rPicLayer.rMp4DecPrm);
    
        if (0 == prMp4Prm->ucVopCdTp)
            u4Value |= (1 << 0);
        else if (1 == prMp4Prm->ucVopCdTp)
            u4Value |= (1 << 1);
        else if (2 == prMp4Prm->ucVopCdTp)
            u4Value |= (1 << 2);
        else if (3 == prMp4Prm->ucVopCdTp)
            u4Value |= (1 << 3);
    
        if (VDEC_MPEG4 == prMpegDecPrm->ucMpegVer)
        {
            VDEC_INFO_MPEG4_VOL_PRM_T *prVolPrm = prMpegDecPrm->rPicLayer.rMp4DecPrm.rDep.rM4vDecPrm.prVol;
            if (prVolPrm->ucShortVideoHeader)
                u4Value |= (1 << 7);
        }
    }
	#ifdef MPEG4_NEW_MODE
	if((VDEC_MPEG1 == prMpegDecPrm->ucMpegVer) || (VDEC_MPEG2 == prMpegDecPrm->ucMpegVer))
    	    vVDecWriteVLD(u4VDecID, RW_VLD_VOP_TYPE, u4Value);
	#else
	    vVDecWriteVLD(u4VDecID, RW_VLD_VOP_TYPE, u4Value);
	#endif
}


// *********************************************************************
// Function :void vVLDSetPicSz(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgB21FrmPic)
// Description : Set RW_VLD_PICSZ register
// Parameter : prDecPrm: pointer to decoder relate variables
//             fgB21FrmPic: B_1/2 mode and frame pic
// Return    : None
// *********************************************************************
void vVLDSetPicSz(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgB21FrmPic)
{
#if (CONFIG_CHIP_VER_CURR <= CONFIG_CHIP_VER_MT8560) 
    UINT32 u4MaxMblMod;
    UINT32 u4WidthMb;
#endif
    UINT32 u4RegVal2;
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif

   #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) //fixed by lu.sun
     u4RegVal2 = ((prMpegDecPrm->ucMpegVer == VDEC_MPEG1) << 31) + ((prMpegDecPrm->ucMpegVer == VDEC_MPEG2) << 30) + ((prDecPrm->u4PicH) << 16) +  0x1FF;            
    #else
	
    if (fgB21FrmPic)
    {
        u4MaxMblMod = prMpegDecPrm->u4MaxMbl * 2 + 1;
    }
    else
    {
        u4MaxMblMod = prMpegDecPrm->u4MaxMbl;
    }
    u4WidthMb = (prDecPrm->u4PicW + 15) >> 4;
	
    u4RegVal2 = ((prMpegDecPrm->ucMpegVer == VDEC_MPEG1) << 31) + ((prDecPrm->u4PicH + 15) << 16) +
                ((u4MaxMblMod & 0xFF) << 8) + u4WidthMb;
    #endif
	
    vVDecWriteVLD(u4VDecID, RW_VLD_PICSZ, u4RegVal2);
}


// *********************************************************************
// Function :void vVLDSetPicSz(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgB21FrmPic)
// Description : Set RW_VLD_PICSZ register
// Parameter : prDecPrm: pointer to decoder relate variables
//             fgB21FrmPic: B_1/2 mode and frame pic
// Return    : None
// *********************************************************************
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
void vVLDMp4SetPicSz(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgB21FrmPic)
{
	UINT32 u4MaxMblMod;
	UINT32 u4WidthMbM1,u4HeightMbM1,u4Width,u4Height;
	UINT32 u4RegVal2;
		//VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
		VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
		 VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif
	
		if (fgB21FrmPic)
		{
			u4MaxMblMod = prMpegDecPrm->u4MaxMbl * 2 + 1;
		}
		else
		{
			u4MaxMblMod = prMpegDecPrm->u4MaxMbl;
		}
		
		u4WidthMbM1 = ((prDecPrm->u4PicW + 15) >> 4) - 1;
		u4HeightMbM1 = ((prDecPrm->u4PicH + 15) >> 4) - 1;
		u4Width = prDecPrm->u4PicW + 15;
		u4Height = prDecPrm->u4PicH + 15;

		u4RegVal2 = ((prMpegDecPrm->ucMpegVer == VDEC_MPEG1) << 31) + (u4MaxMblMod & 0x1FF);
		
    vVDecWriteVLD(u4VDecID, RW_VLD_PICSZ, u4RegVal2);
//		vVDecWriteVLD(u4VDecID, RW_VLD_MB_SIZE, u4HeightMbM1<<16 | u4WidthMbM1);
//		vVDecWriteVLD(u4VDecID, RW_VLD_PIC_SIZE, u4Height<<16 | u4Width);
		vVDecWriteVLDTOP(u4VDecID,RW_TOPVLD_WMV_PICSIZE_MB,u4HeightMbM1<<16 | u4WidthMbM1);
		vVDecWriteVLDTOP(u4VDecID,RW_TOPVLD_WMV_PICSIZE,u4Height<<16 | u4Width);

}
#endif


// *********************************************************************
// Function : UINT32 u4VLDPicH(UCHAR ucMpegVer, UINT32 u4PicH)
// Description : Add 15 to picture height or not according to mpeg version
// Parameter : prDecPrm: pointer to decoder relate variables
//             u4PicH: original height
// Return    : new height
// *********************************************************************
UINT32 u4VLDPicH(UCHAR ucMpegVer, UINT32 u4PicH)
{
    if ((ucMpegVer == 1) || (ucMpegVer == 2))
    {
        return (u4PicH);
    }
    else
    {
        return (u4PicH + 15);
    }
}


// *********************************************************************
// Function :void vVLDSetMBRowPrm(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgBPic, BOOL fgFrmPic)
// Description : Set RW_VLD_MBROWPRM register
// Parameter : prDecPrm: pointer to decoder relate variables
//             fgBPic: B-picture
//             fgFrmPic: frame picture
// Return    : None
// *********************************************************************
void vVLDSetMBRowPrm(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgBPic, BOOL fgFrmPic)
{
    UINT32 u4RegVal3;
    UINT32 u4StartX, u4StartY, u4EndX, u4EndY;
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif

    //Partial decode parameters calculation
    u4StartX = prMpegDecPrm->u4DecXOff >> 4;
    if ((prMpegDecPrm->u4DecXOff & 0xf) != 0)
    {
        u4StartX += 1;
    }
  
    u4EndX = (prMpegDecPrm->u4DecXOff + prMpegDecPrm->u4DecW - 1) >> 4;
    UNUSED(u4EndX);
    u4StartY = prMpegDecPrm->u4DecYOff >> 4;
    if (prMpegDecPrm->u4DecYOff == 0)
    {
        u4StartY = 0;
    }
    else if ((u4StartY & 0x1) != 0)
    {
        u4StartY += 1;
    }
    u4EndY = (prMpegDecPrm->u4DecYOff + u4VLDPicH(prMpegDecPrm->ucMpegVer, prMpegDecPrm->u4DecH) - 1) >> 4;
  
    if ((prMpegDecPrm->u4DecH == prDecPrm->u4PicH) &&
       (prMpegDecPrm->u4DecYOff == 0))
    {
    #if 1 //CONFIG_MT8520_VDEC_EMU
     #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
	 u4EndY = 0x1ff;
	 #else
        u4EndY = 127;
	 #endif
    #else
        u4EndY = 63;
    #endif    
    }
    else if ((u4EndY & 0x1)==0)
    {
        if(!(prMpegDecPrm->ucMpegVer == 1 || prMpegDecPrm->ucMpegVer == 2))
        {
           u4EndY -= 1;
        }
    }
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
    if (fgBPic)
    {
       if (fgFrmPic)
       {
           u4RegVal3 = (u4StartY << 16) + u4EndY;
       }
       else
       {
           u4RegVal3 = ((u4StartY / 2)  << 16) +
                      ((u4EndY - 1) / 2) ;
       }
    }
    else
    {
        u4RegVal3 = 0x1ff;
    }
    #else
    if (fgBPic)
    {
       if (fgFrmPic)
       {
           u4RegVal3 = (u4StartY << 24) + (u4EndY << 16) +
                      (prMpegDecPrm->u4BBufStart << 8);
       }
       else
       {
           u4RegVal3 = ((u4StartY / 2)  << 24) +
                      (((u4EndY - 1) / 2) << 16) +
                      (prMpegDecPrm->u4BBufStart << 8);
       }
    }
    else
    {
        u4RegVal3 = (127 << 16) + (prMpegDecPrm->u4BBufStart << 8);  
    }
    #endif
	
    vVDecWriteVLD(u4VDecID, RW_VLD_MBROWPRM, u4RegVal3);
}


// *********************************************************************
// Function :void vVLDM4vDec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgBVop)
// Description : Decode M4v Picture
// Parameter : prDecPrm: Decode Parameters Set
//             fgBVop: B-VOP
// Return    : None
// *********************************************************************
void vVLDM4vDec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm, BOOL fgBVop)
{
    vVLDM4vDecPrmProc(u4VDecID, prDecPrm);
    vVLDSetPicType(u4VDecID, prDecPrm);
    vVLDDoDec(u4VDecID, prDecPrm, fgBVop, FALSE);
    vVLDMp4TriggerDec(u4VDecID);
}


// *********************************************************************
// Function :void vVLDM4vDecPrmProc(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
// Description : Process M4v decode related parameters
// Parameter : prDecPrm: decode related parameters
// Return    : None
// *********************************************************************
void vVLDM4vDecPrmProc(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
    UINT32 u4MBx;
    UINT32 u4RegVal1;
    UINT32 u4Mp4Hdr;
	UINT32 i;
	CHAR *ucdata;
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif
    VDEC_INFO_MPEG4_DEC_PRM_T *prMp4Prm = &(prMpegDecPrm->rPicLayer.rMp4DecPrm);
    VDEC_INFO_MPEG4_VOL_PRM_T *prVolPrm = prMp4Prm->rDep.rM4vDecPrm.prVol;
    VDEC_INFO_MPEG4_VOP_PRM_T *prVopPrm = prMp4Prm->rDep.rM4vDecPrm.prVop;
    VDEC_INFO_MPEG_DIR_MODE_T *prDirMdPrm = prVopPrm->prDirMd;
    VDEC_INFO_MPEG_GMC_PRM_T *prGmcPrm = prVopPrm->prGmcPrm;

 #if 1 //mtk40088 add for MPEG4 quant_scale
    UINT32 u4RegValue = 0;
    u4RegValue = u4VDecReadVLD(u4VDecID, RW_VLD_BREF);
    vVDecWriteVLD(u4VDecID, RW_VLD_BREF, u4RegValue | 0x100);
 #endif
 #if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8555)
	 if (BSP_GetIcVersion() == IC_8555)
        {
	    if(u4VDecID == 1)
    {
        vWriteReg(0x32000,(u4ReadReg(0x32000) | 0x1));
    }
    else
    {
        vWriteReg(0x2B000,(u4ReadReg(0x2B000) | 0x1));
    }
        }
#endif
    vVDecWriteVLD(u4VDecID, RW_VLD_MP4_FLG, 1);
	#ifdef CONFIG_TV_DRV_VFY 
	#ifdef MPEG4_NEW_MODE
	if(prDecPrm->ucMpegSpecType == 3) // indicate codeversion is DIVX4/5/6 xiaolei.li for python verification
        vVDecWriteVLD(u4VDecID, RW_VLD_MP4_FLG, u4VDecReadVLD(u4VDecID, RW_VLD_MP4_FLG)|(1<<9)); //divx456_flag
	vVDecWriteVLD(u4VDecID, RW_VLD_MP4_FLG, u4VDecReadVLD(u4VDecID, RW_VLD_MP4_FLG)|(1<<16));
	vVDecWriteVLD(u4VDecID, 0x2E0, u4VDecReadVLD(u4VDecID, 0x2E0)|(0xFF<<16)|(1<<4));
	vVDecWriteVLD(u4VDecID, 0x2F4,0xFFFFFFFF);
	vVDecWriteVLD(u4VDecID, 0x2F8,0xFFFFFFFF);
	#endif
	#endif
#if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8555)
	if (BSP_GetIcVersion() == IC_8555)
	{
    if(u4VDecID == 1)
    {
        vWriteReg(0x32000,(u4ReadReg(0x32000) & (~0x1)));
    }
    else
    {
        vWriteReg(0x2B000,(u4ReadReg(0x2B000) & (~0x1)));
    }
       }
#endif
  
    u4MBx = (prDecPrm->u4PicW + 15) / 16;
  
    u4RegVal1 = (prVopPrm->fgTopFldFirst << 26) +
                (prMp4Prm->ucVopCdTp << 21) +
                (prVopPrm->ucFordFCode << 12) +
                (prVopPrm->ucBackFCode << 4) + 
                (prVopPrm->fgAlternateVerticalScanFlag << 17);
    vVDecWriteVLD(u4VDecID, RW_VLD_PARA, u4RegVal1);
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
    vVLDMp4SetPicSz(u4VDecID, prDecPrm, FALSE);
    #else
    vVLDSetPicSz(u4VDecID, prDecPrm, FALSE);
    #endif
    vVLDSetMBRowPrm(u4VDecID, prDecPrm, prMp4Prm->ucVopCdTp == VCT_B, TRUE);
    vVLDMp4DecPrmProc(u4VDecID, prMp4Prm, u4MBx, prVopPrm->ucIntraDcVlcThr,
                                      prVolPrm->ucQuarterSample,
                                      prVolPrm->ucDataPartitioned);
	#ifdef CHIP_MT5881 // just for remy, 5399 no need.
	if(2 == prDecPrm->ucMpegSpecType) //mpeg4 mode
	{
		vVDecWriteMC(u4VDecID, 661*4, u4VDecReadMC(u4VDecID, 661*4) | (1<<0)); //mpeg4 mode chroma mv modification
	}
	#endif
	#ifdef CHIP_MT5399 //xiaolei.li follow vVLDMp4DecPrmProc
	if((2 == prDecPrm->ucMpegSpecType) && prVolPrm->ucQuarterSample) //mpeg4 mode & qpel
	{
		vVDecWriteMC(u4VDecID, RW_MC_CBCR_MV_TYPE, u4VDecReadMC(u4VDecID, RW_MC_CBCR_MV_TYPE) & (~(1<<8)));
	}
	//else
	//{
	//	vVDecWriteMC(u4VDecID, RW_MC_CBCR_MV_TYPE, u4VDecReadMC(u4VDecID, RW_MC_CBCR_MV_TYPE) | (1<<8));
	//}
	#endif
	#ifdef MPEG4_NEW_MODE
	//UTIL_Printf("i4GmcYMvX = %d, i4GmcYMvY = %d\n",prMp4Prm->rDep.rM4vDecPrm.prVop->prGmcPrm->i4GmcYMvX,prMp4Prm->rDep.rM4vDecPrm.prVop->prGmcPrm->i4GmcYMvY);
	_SetM4VAVCMV(u4VDecID,prMp4Prm);
	#endif
  
    // GSLin's
    u4Mp4Hdr = (prVolPrm->ucResyncMarkerDisable << 26) +
               (prVolPrm->ucQuarterSample << 25) +               
               (prVolPrm->ucSorenson << 23 ) +  //PANDA H263
               (prVolPrm->ucQuantType << 24) +
               (prVolPrm->ucInterlaced << 22) +
               (prVolPrm->ucDataPartitioned << 21) +
               (prVolPrm->ucReversibleVlc << 20) +
               (prVolPrm->ucShortVideoHeader << 19) +
               (prVolPrm->u2VopTimeIncrementResolution);
	if(VDEC_H263 == prMpegDecPrm->ucMpegVer)
	{
		u4Mp4Hdr = (u4Mp4Hdr | (1<<31)); // set gob align chk disable = 1  xiaolei.li
		vVDecWriteVLD(u4VDecID, 124*4, (u4VDecReadVLD(u4VDecID, 124*4)|(1<<20))); //just for viper&python set gob align chk in skip mb disable = 1 xiaolei.li	
	}
    vVDecWriteVLD(u4VDecID, RW_VLD_MP4_HDR, u4Mp4Hdr);
    vVDecWriteVLD(u4VDecID, RW_VLD_M4SH, prVolPrm->ucSourceFormat);

	#if BANDWIDTH_MEASURE || PARA8_BEHAVIOR_TEST || PERFORMANCE_TEST
	switch(prMp4Prm->ucVopCdTp)
	{
		case 0:
			UTIL_Printf("[MPEG4]: vop type = I\n");
		break;
		case 1:
			UTIL_Printf("[MPEG4]: vop type = P\n");
		break;
		case 2:
			UTIL_Printf("[MPEG4]: vop type = B\n");
		break;
		case 3:
			UTIL_Printf("[MPEG4]: vop type = S\n");
		break;
		default:
			UTIL_Printf("[MPEG4]: vop type error\n");
		break;	
	}	
	#endif
    if (prMp4Prm->ucVopCdTp == VCT_B)
    {
    	vVDecWriteVLD(u4VDecID, RW_VLD_MP4_HDR, (u4VDecReadVLD(u4VDecID, RW_VLD_MP4_HDR) & (~(1<<20)))); // for 8561 eco emulation xiaolei.li
        vVLDSetBRef(u4VDecID, prVopPrm->ucBRefCdTp == VCT_P);
		
		if(prVopPrm->ucBRefCdTp == VCT_I) // I0->B1 clear MV_DIRECT_DRAM  xiaolei.li  for 8561eco emulation
		{
			#ifndef MPEG4_NEW_MODE // this workaround is for mpeg4 old mode xiaolei.li for python verification		
			#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD))
			for(i=prMp4Prm->rMpeg4WorkBufSa.u4MvecSa;i<(prMp4Prm->rMpeg4WorkBufSa.u4MvecSa+VER_MVEC_SZ);i++)
			#else
			for(i=prMp4Prm->rMpeg4WorkBufSa.u4MvecSa;i<(prMp4Prm->rMpeg4WorkBufSa.u4MvecSa+(MVEC_SZ_W * MVEC_SZ_H));i++)
			#endif
			{
				ucdata = (CHAR*)i;
				*ucdata = 0;
			}
			UTIL_Printf("Clear MV_DIRECT_DRAM here(1)\n");
			#endif
			#ifdef CONFIG_TV_DRV_VFY 
			#ifdef MPEG4_NEW_MODE
			vVDecWriteVLD(u4VDecID, RW_VLD_BREF, u4VDecReadVLD(u4VDecID, RW_VLD_BREF)|(1<<1)); //co_located_i_flag
			#endif
			#endif
		}			
		
        vMCSetBRef(u4VDecID, prVopPrm->ucBRefCdTp == VCT_I);
    }

	#ifndef MPEG4_NEW_MODE
	// HHKuo's
    vVDecWriteVLD(u4VDecID, RW_VLD_DIRE_MD, prDirMdPrm->u4Trb | (prDirMdPrm->u4Trd << 16));

    if (prVolPrm->ucInterlaced)
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_DIRE_MD_IL, prDirMdPrm->u4Trbi | (prDirMdPrm->u4Trdi << 16));
    }
	#endif
  
    // CCJu's
    vVDecWriteMC(u4VDecID, RW_MC_WARP_POINT_NUM, prGmcPrm->ucEffectiveWarpingPoints);
    vVDecWriteMC(u4VDecID, RW_MC_GMC_Y_MV, (prGmcPrm->i4GmcYMvX << 16) | (prGmcPrm->i4GmcYMvY & 0xffff));
	#ifdef CHIP_MT5399 //xiaolei.li
	if(2 == prDecPrm->ucMpegSpecType) //mpeg4 mode
	{
		vVDecWriteMC(u4VDecID, RW_MC_GMC_C_MV, (prGmcPrm->i4GmcCMvX << 16) | (prGmcPrm->i4GmcCMvY & 0xffff));
	}
	else if(3 == prDecPrm->ucMpegSpecType) //divx mode
	{
		vVDecWriteMC(u4VDecID, RW_MC_GMC_C_MV, (prGmcPrm->i4GmcYMvX << 15) | ((prGmcPrm->i4GmcYMvY >> 1) & 0xffff));
	}
	#else
	vVDecWriteMC(u4VDecID, RW_MC_GMC_C_MV, (prGmcPrm->i4GmcCMvX << 16) | (prGmcPrm->i4GmcCMvY & 0xffff));
	#endif
    // DivX3 only registers...
    // Maybe it's not necessary but clear them anyway
    vVDecWriteVLD(u4VDecID, RW_VLD_DX3, 0);

	//UTIL_Printf("[MPEG4]: VLD_122 = %d, MV_134 = %d\n",u4VDecReadVLD(u4VDecID, RW_VLD_BREF),u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_COL_PIC));
	#ifdef MPEG4_NEW_MODE
	vVDecWriteVLD(u4VDecID, 184*4, u4VDecReadVLD(u4VDecID, 184*4)|(1<<4));  //for python verification xiaolei.li
	#endif

	#ifdef MPEG4_NEW_MODE
	//set 189,190,184 to close error handling after viper, e.p.viper,python,remy added by xiaolei.li 2012.05.31
	vVDecWriteVLD(u4VDecID, 184*4, u4VDecReadVLD(u4VDecID, 184*4)|(0xff<<16));
	vVDecWriteVLD(u4VDecID, 189*4, 0xffffffff);
	vVDecWriteVLD(u4VDecID, 190*4, 0xffffffff);
	#endif
	#ifdef CHIP_MT5881  //for 5881 emulation xiaolei.li
	#endif
	#if 0//for 5399 high resolution: xvid_3840*1088.m4v test xiaolei.li
	vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_PRED_SRAM_CFG, (u4VDecReadVLDTOP(u4VDecID,RW_VLD_TOP_PRED_SRAM_CFG) | (0x1<< 30)));
	//UTIL_Printf("[MPEG4]: Bypass VLD Wrapper\n");
	#endif
}


// *********************************************************************
// Function :void vVLDMp4TriggerDec(UINT32 u4VDecID)
// Description : Start decode for Mpeg4 (only used in Mpeg4 or DivX3)
// Parameter : None
// Return    : None
// *********************************************************************
void vVLDMp4TriggerDec(UINT32 u4VDecID)
{    
    UTIL_Printf("[Input Win]: 0x%x\n",u4VDecReadVLD(u4VDecID, 60*4));
    //vVDecWriteVLD(u4VDecID, RW_VLD_DCMVSEL, 0x903000f2);
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    UINT32 vld138;
    vld138 = u4VDecReadVLD(u4VDecID, RW_VLD_MODE);
    vld138 |= (1<<28);
    vVDecWriteVLD(u4VDecID, RW_VLD_MODE, vld138);
#endif

    //mtk40110 Qing.Li 2010/11/25, to fix mpeg4 DCAC Pred bug
#if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8560)
    vVDecWriteVLD(u4VDecID, RW_VLD_DCMVSEL, 0x903008f2);
#endif

    //200808 YN
    //To fix error bistream hang issue.
    //vVDecWriteVLD(u4VDecID, RW_VLD_DRAMMBSEL, 0x910002);
    vVDecWriteVLD(u4VDecID, RW_VLD_DRAMMBSEL, 0x1910002);
	//enable crc
	vVDecWriteCRC(u4VDecID, 0x4, 1);
#if CONFIG_DRV_VERIFY_SUPPORT 
    if (_u4FileCnt[u4VDecID] == _u4DumpRegPicNum[u4VDecID])
    {
       //vVDEC_HAL_MPEG_VDec_DumpReg(u4VDecID, TRUE);
		UTIL_Printf("Before Decode\n");
	   VDecDumpMpegRegister(u4VDecID);
    }
#endif

#if 0
    UINT32 u4Val;
    BOOL fgLogReg = FALSE;
    int reg;
    int vldstart = 42;
    int vldsize = 400;
    int mcstart = 0;
    int mcsize = 400;

    if (fgLogReg == TRUE)
    {
	 UTIL_Printf("VLD Before Settings\n");
        for(reg= vldstart; reg < (vldstart+vldsize); reg++)
        {          
            u4Val = u4VDecReadVLD(u4VDecID, (reg << 2));
            UTIL_Printf("%d (0x%x) = 0x%4x\n", reg, (reg<<2), u4Val);
        }
        
	 UTIL_Printf("MC Before Settings\n");
        for(reg= mcstart; reg < (mcstart+mcsize); reg++)
        {          
            u4Val = u4VDecReadMC(u4VDecID, (reg << 2));
            UTIL_Printf("%d (0x%x) = 0x%4x\n", reg, (reg<<2), u4Val);
        }
    }
#endif

    
#if (PROFILE_TIME_TEST && CONFIG_DRV_VERIFY_SUPPORT)
               {
                   HAL_GetTime(&rTimeStartDec[u4VDecID]);
                   UTIL_Printf("Start_%d:second:ms %08d:%08d\n",u4VDecID,rTimeStartDec[u4VDecID].u4Seconds,rTimeStartDec[u4VDecID].u4Micros);
               }
#endif


    // For Mpeg4, to start decode we have to write this register as follows
    mb();
    vVDecWriteVLD(u4VDecID, RW_VLD_DECSTART, 1);
    mb();
    vVDecWriteVLD(u4VDecID, RW_VLD_DECSTART, 0);
}


// *********************************************************************
//void vVLDMp4DecPrmProc(UINT32 u4VDecID, VDEC_INFO_MPEG4_DEC_PRM_T *prMp4Prm,
//                       UINT32 u4MBx,
//                       UCHAR ucIntraDcVlcThr,
//                       UCHAR ucQuarterSample,
//                       UCHAR ucDataPartitioned)
// Description : write Mpeg4/DivX3 common registers
// Parameter : prMp4Prm: mp4 decode parameters
//             u4MBx: width in MB
//             ucIntraDcVlcThr: intra_dc_vlc_thr
//             ucQuarterSample: quarter_sample
//             ucDataPartitioned: data_partitioned
// Return    : None
// *********************************************************************
void vVLDMp4DecPrmProc(UINT32 u4VDecID, VDEC_INFO_MPEG4_DEC_PRM_T *prMp4Prm,
                                UINT32 u4MBx,
                                UCHAR ucIntraDcVlcThr,
                                UCHAR ucQuarterSample,
                                UCHAR ucDataPartitioned)
{
    UINT32 u4DcacPitch;
    UINT32 u4NonPBit;
    UINT32 u4Temp = 0;
    UINT32 u4Mbstart_Dcac_Switch = 0;
  #ifdef VDEC_MPEG4_SUPPORT_RESYNC_MARKER 
    UINT32 u4ResyncMarkMB = 0;
  #endif    
  #if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 ||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890))
    UINT32 i;
    CHAR *ucdata;
  #endif
	
  //data partition S/P VOP clear MV DIRECT DRAM xiaolei.li for 8561eco emulation
    if((ucDataPartitioned == 1) &&((prMp4Prm->ucVopCdTp == VCT_P)||(prMp4Prm->ucVopCdTp == VCT_S)))  
    {
    	#ifndef MPEG4_NEW_MODE // this workaround is for mpeg4 old mode xiaolei.li for python verification
    	#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD))
		for(i=prMp4Prm->rMpeg4WorkBufSa.u4MvecSa;i<(prMp4Prm->rMpeg4WorkBufSa.u4MvecSa+VER_MVEC_SZ);i++)
		#else
		for(i=prMp4Prm->rMpeg4WorkBufSa.u4MvecSa;i<(prMp4Prm->rMpeg4WorkBufSa.u4MvecSa+(MVEC_SZ_W * MVEC_SZ_H));i++)
		#endif
		{
			ucdata = (CHAR*)i;
			*ucdata = 0;
		}
		UTIL_Printf("Clear MV_DIRECT_DRAM here(2)\n");
		#endif
    }    

    // GSLin's
    vVLDBarl2Vdec(u4VDecID);
    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890 ||CONFIG_CHIP_VER_CURR== CONFIG_CHIP_VER_MT8561)
    vVDecWriteVLD(u4VDecID,RW_VLD_LDSH,u4VDecReadVLD(u4VDecID,RW_VLD_LDSH) & 0xFFFFFEFF);
    #endif
    vVDecWriteVLD(u4VDecID, RW_VLD_QUANT, (ucIntraDcVlcThr << 8) + prMp4Prm->ucVopQuant);

    // HHKuo's
    vVLDSetDcacAddr(u4VDecID, u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4DcacSa));

	#ifdef CONFIG_TV_DRV_VFY 
	#ifdef MPEG4_NEW_MODE
	vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_PRED_MODE, u4VDecReadVLDTOP(u4VDecID,RW_VLD_TOP_PRED_MODE) & 0xFFFFFFDF);  //Set DRAM burst mode(DC & DTV)
	vVDecWriteVLDTOP(u4VDecID, RW_VLD_TOP_PRED_ADDR, u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4DcacSa));
	vVDecWriteVLD(u4VDecID,RW_VLD_BMB_SA, u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4Bmb1Sa));
	if(prMp4Prm->ucVopCdTp == VCT_I)
	{
		vVDecWriteVLD(u4VDecID,RW_VLD_BMB_SIZE,(u4VDecReadVLD(u4VDecID,RW_VLD_BMB_SIZE)&0xFFF00000)|(prMp4Prm->rMpeg4WorkBufSize.u4Bmb2Size >> 4));
	}
	else
	{
		vVDecWriteVLD(u4VDecID,RW_VLD_BMB_SIZE,(u4VDecReadVLD(u4VDecID,RW_VLD_BMB_SIZE)&0xFFF00000)|((2 * prMp4Prm->rMpeg4WorkBufSize.u4Bmb1Size) >> 4 ));
	}
	vVDecWriteVLD(u4VDecID,RW_VLD_BCODE_SA_NEWMODE,u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4BcodeSa));
	vVDecWriteVLD(u4VDecID,RW_VLD_BCODE_SIZE_NEWMODE,((prMp4Prm->rMpeg4WorkBufSize.u4DcacSize>>4)<<16) | (prMp4Prm->rMpeg4WorkBufSize.u4BcodeSize>>4));
	//_SetMpeg4AVCMV(u4VDecID,prMp4Prm);
	#endif
	#endif
  #ifndef MPEG4_NEW_MODE
  #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561) 
    u4DcacPitch =       (prMp4Prm->rMpeg4WorkBufSize.u4DcacSize >> 4) |
				    (8 << 24) |  // bit per pixel
                                (0 << 28) |  // only write 1 in DivX 3.11 1st picture
                                (ucQuarterSample << 29) |
  #ifdef WRITE_FULL_DCAC_DATA
                                (1 << 30) |   // write full dcac data to DRAM
  #else
                                (0 << 30) |   // only write 1 line data to DRAM
  #endif
                                (ucDataPartitioned << 31);
  
   vVDecWriteVLD(u4VDecID, RW_VLD_DCACPITCH, u4DcacPitch);
   vVDecWriteVLD(u4VDecID, RW_VLD_MVDRAM, (u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4MvecSa) >> 2));
   vVDecWriteVLD(u4VDecID, RW_VLD_MB1SA, (u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4Bmb1Sa) >> 2));	
   vVDecWriteVLD(u4VDecID, RW_VLD_MB2SA, (u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4Bmb2Sa) >> 2));	
   vVDecWriteVLD(u4VDecID, RW_VLD_COSA, (u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4BcodeSa) >> 2));
   u4Temp = (u4MBx * 4) |((u4MBx * 4*3) << 16);
   vVDecWriteVLD(u4VDecID, RW_VLD_ADDREXTEND, u4Temp);
   vVDecWriteVLD(u4VDecID, RW_VLD_MVSA, (u4MBx * 4));
   vVDecWriteVLD(u4VDecID, RW_VLD_BMB1, (u4MBx * 4));
   vVDecWriteVLD(u4VDecID, RW_VLD_BMB2, (u4MBx * 4));
   vVDecWriteVLD(u4VDecID, RW_VLD_BCODE_SA, ((prMp4Prm->rMpeg4WorkBufSize.u4BcodeSize >> 4)<<16) | 0 << 1);//????
//   vVDecWriteVLD(u4VDecID, RW_VLD_BCODE_SA,  0 << 1);//????

   
  #else
    u4DcacPitch = (u4MBx * 4) |  // MBx * 4
                                ((u4MBx * 4 * 3) << 12) |  // MBx * 4 * 3
                                (8 << 24) |  // bit per pixel
                                (0 << 28) |  // only write 1 in DivX 3.11 1st picture
                                (ucQuarterSample << 29) |
  #ifdef WRITE_FULL_DCAC_DATA
                                (1 << 30) |   // write full dcac data to DRAM
  #else
                                (0 << 30) |   // only write 1 line data to DRAM
  #endif
                 (ucDataPartitioned << 31);
    vVDecWriteVLD(u4VDecID, RW_VLD_DCACPITCH, u4DcacPitch);
    //vVDecWriteVLD(u4VDecID, RW_VLD_MVSA, (PHYSICAL(prMp4Prm->rMpeg4WorkBufSa.u4MvecSa) >> 2) | ((u4MBx * 4) << 22));
    vVDecWriteVLD(u4VDecID, RW_VLD_MVSA, ((u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4MvecSa) >> 2) & 0x3FFFFF) | ((u4MBx * 4) << 22)); 
    u4Temp = (((u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4MvecSa) >> 2) >> 22) & 0x3F);
    //vVDecWriteVLD(u4VDecID, RW_VLD_ADDREXTEND, (u4VDecReadVLD(u4VDecID, RW_VLD_ADDREXTEND) & 0xFFFFFFC0) |
    //(((PHYSICAL(prMp4Prm->rMpeg4WorkBufSa.u4MvecSa) >> 2) >> 22) & 0x3F)); 
    //vVDecWriteVLD(u4VDecID, RW_VLD_BMB1, (PHYSICAL(prMp4Prm->rMpeg4WorkBufSa.u4Bmb1Sa) >> 2) | ((u4MBx * 4) << 22));
    vVDecWriteVLD(u4VDecID, RW_VLD_BMB1, ((u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4Bmb1Sa) >> 2) & 0x3FFFFF) | ((u4MBx * 4) << 22)); 
    u4Temp |= ((((u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4Bmb1Sa) >> 2) >> 22) & 0x3F) << 8);
    //vVDecWriteVLD(u4VDecID, RW_VLD_ADDREXTEND, (u4VDecReadVLD(u4VDecID, RW_VLD_ADDREXTEND) & 0xFFFFC0FF) |
    //((((PHYSICAL(prMp4Prm->rMpeg4WorkBufSa.u4Bmb1Sa) >> 2) >> 22) & 0x3F) << 8)); 
    //vVDecWriteVLD(u4VDecID, RW_VLD_BMB2, (PHYSICAL(prMp4Prm->rMpeg4WorkBufSa.u4Bmb2Sa) >> 2) | ((u4MBx * 4) << 22));
    vVDecWriteVLD(u4VDecID, RW_VLD_BMB2, ((u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4Bmb2Sa) >> 2) & 0x3FFFFF) | ((u4MBx * 4) << 22));
    u4Temp |= ((((u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4Bmb2Sa) >> 2) >> 22) & 0x3F) << 16);
    vVDecWriteVLD(u4VDecID, RW_VLD_ADDREXTEND, u4Temp);
    //vVDecWriteVLD(u4VDecID, RW_VLD_ADDREXTEND, (u4VDecReadVLD(u4VDecID, RW_VLD_ADDREXTEND) & 0xFFC0FFFF) |
    //((((PHYSICAL(prMp4Prm->rMpeg4WorkBufSa.u4Bmb2Sa) >> 2) >> 22) & 0x3F) << 16)); 
  
    // Modified by C.K. Hu 20040617. Porting from 1389_5039.
  
    //if (dwSetDDR == 0) // for SDR
    //{
      // bit 25: wait for wdle, required for 8105 DDR, and OK for 1389
      vVDecWriteVLD(u4VDecID, RW_VLD_BCODE_SA, ((u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4BcodeSa) >> 2) & 0xFFFFFF) | (0 << 25)
      | (((u4AbsDramANc(prMp4Prm->rMpeg4WorkBufSa.u4BcodeSa) >> 2) & 0xF000000) << 4));
#endif

      
      if (0 == prMp4Prm->ucVopCdTp)// for 8520
          u4NonPBit = 1 << 18;
      else if ((1 == prMp4Prm->ucVopCdTp) || (2 == prMp4Prm->ucVopCdTp))
          u4NonPBit = 0 << 18;
      else
          u4NonPBit = u4VDecReadVLD(u4VDecID, RW_VLD_DCMVSEL) & (1 << 18);
        
      u4DcacPitch = (0 << 0)  |  //turn on reset direction_busy
             (1 << 1)  |  //turn on idctbusy + VWRDRAMDCACST[0]
             (0 << 6)  |  //dcacbusy wait for wdle (required for 8105 DDR)
             (0 << 7)  |  //dcacbusy4is wait for wdle (required for 8105 DDR)
             (1<<11)   |
             (0 << 16) |  //mv_dram reset by 2D reset
             (0 << 17) |  //turn on mv_modb1
             u4NonPBit  |  //none_P_bitstream mode
             (0 << 19) |  //turn on sram
             (0 << 20) |  //mvbusy wait for wdle (required for 8105 DDR)
             (0 << 21);   //turn 3t period without mem_pause;
      if (1) // for data-partitioned, but can turn on in general case
      {
          u4DcacPitch |= (1 << 4) |  //turn on dcacbusy + VWRDRAMDCACST[2]
                  (1 << 5);   //turn on dcacbusy4is + VWRDRAMDCACST[2]
      }
      u4DcacPitch |= (((UINT32)1) << 31); // for 8520
      vVDecWriteVLD(u4VDecID, RW_VLD_DCMVSEL, u4DcacPitch);
      u4DcacPitch = (0 << 0) | //vmbbusy is busy until data write2DRAM, not Write2WRFIFO.
             (0 << 1) | //vmbbusy wait for wdle (required for 8105 DDR)
             (0 << 16)| //stop vld_state by DCACFULL
             (0 << 17)| //trun off reset dramctrl when mc_abort
             (1 << 18)| //stop vld_state by VDramWrVFull
             (0 << 20); //check wr3fifo full when Wdle (required for 8105 DDR)
      u4DcacPitch |= (1 << 23); // for 8520
      vVDecWriteVLD(u4VDecID, RW_VLD_DRAMMBSEL, u4DcacPitch);
#endif

      u4Mbstart_Dcac_Switch = u4VDecReadVLD(u4VDecID, RW_MBSTART_DCAC_SWITCH);
      //u4Mbstart_Dcac_Switch |= (1<<30);   //mark this operation after MT8561 ECO . xiaolei.li 20120323

#ifdef VDEC_MPEG4_SUPPORT_RESYNC_MARKER 
      //Enable resync mark ECO.
      if(_u4ResyncMarkerCnt[u4VDecID] != 0)
      {
          u4Mbstart_Dcac_Switch |= (VLD_RESYNC_MARK_ECO);
      }
      

      //Set resync mark Mbx, Mby.
      if (_u4ResyncMarkerCnt[u4VDecID] == 1)
      {
         u4ResyncMarkMB |= ((_u4ResyncMarkMbx[u4VDecID][0]) << 16);
         u4ResyncMarkMB |= ((_u4ResyncMarkMby[u4VDecID][0]) );

         vVDecWriteVLD(u4VDecID, RW_VLD_RESYNC_MARK, u4ResyncMarkMB);
      }
      else
      if (_u4ResyncMarkerCnt[u4VDecID] == 2)      
      {
         u4ResyncMarkMB |= ((_u4ResyncMarkMbx[u4VDecID][0]) << 16);
         u4ResyncMarkMB |= ((_u4ResyncMarkMby[u4VDecID][0]));
         
         u4ResyncMarkMB |= ((_u4ResyncMarkMbx[u4VDecID][1]) << 24);
         u4ResyncMarkMB |= ((_u4ResyncMarkMby[u4VDecID][1]) << 8);      
         
         vVDecWriteVLD(u4VDecID, RW_VLD_RESYNC_MARK, u4ResyncMarkMB);
      }
      
#endif
      
      vVDecWriteVLD(u4VDecID, RW_MBSTART_DCAC_SWITCH, u4Mbstart_Dcac_Switch);
    //} // Mark DDR setting by C.K. Hu. 20050509
        // Because HW will decode error for DDR setting even DRAM is DDR, so use SDR setting for DDR DRAM
    //else // for DDR
    //{
      // bit 25: wait for wdle, required for 8105 DDR, and OK for 1389
    //  vWriteVLD(RW_VLD_BCODE_SA, (dwAbsDramANc(BCODE_SA) >> 2) | (1 << 25));
    //  u4NonPBit = dReadVLD(RW_VLD_DCMVSEL) & (1 << 18);
    //  u4DcacPitch = (0 << 0)  |  //turn on reset direction_busy
    //         (1 << 1)  |  //turn on idctbusy + VWRDRAMDCACST[0]
    //         (1 << 6)  |  //dcacbusy wait for wdle (required for 8105 DDR)
    //         (1 << 7)  |  //dcacbusy4is wait for wdle (required for 8105 DDR)
    //         (0 << 16) |  //mv_dram reset by 2D reset
    //         (0 << 17) |  //turn on mv_modb1
    //         u4NonPBit  |  //none_P_bitstream mode
    //         (0 << 19) |  //turn on sram
    //         (1 << 20) |  //mvbusy wait for wdle (required for 8105 DDR)
    //         (0 << 21);   //turn 3t period without mem_pause;
    //  if(1) // for data-partitioned, but can turn on in general case
    //  {
    //    u4DcacPitch |= (1 << 4) |  //turn on dcacbusy + VWRDRAMDCACST[2]
    //            (1 << 5);   //turn on dcacbusy4is + VWRDRAMDCACST[2]
    //  }
    //  vWriteVLD(RW_VLD_DCMVSEL, u4DcacPitch);
    //  u4DcacPitch = (0 << 0) | //vmbbusy is busy until data write2DRAM, not Write2WRFIFO.
    //         (1 << 1) | //vmbbusy wait for wdle (required for 8105 DDR)
    //         (0 << 16)| //stop vld_state by DCACFULL
    //         (0 << 17)| //trun off reset dramctrl when mc_abort
    //         (1 << 18)| //stop vld_state by VDramWrVFull
    //         (1 << 20); //check wr3fifo full when Wdle (required for 8105 DDR)
    //  vWriteVLD(RW_VLD_DRAMMBSEL, u4DcacPitch);
    //}
  
    // CCJu's
  //  vWriteMC(RW_MC_QPEL_TYPE, TYPE_14496);
    vVDecWriteMC(u4VDecID, RW_MC_QPEL_TYPE, prMp4Prm->u4QPelType);
    vVDecWriteMC(u4VDecID, RW_MC_QUARTER_SAMPLE, ucQuarterSample);
    vVDecWriteMC(u4VDecID, RW_MC_ROUNDING_CTRL, prMp4Prm->ucVopRoundingType);
  
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_WIDTH, prMp4Prm->u4UmvPicW);
    vVDecWriteMC(u4VDecID, RW_MC_UMV_PIC_HEIGHT, prMp4Prm->u4UmvPicH);
  
    vVDecWriteMC(u4VDecID, RW_MC_CBCR_MV_TYPE, prMp4Prm->u4CMvType);
  
    // Modified by C.K. Hu 20040617. Porting from 1389_5039
    u4DcacPitch = u4VDecReadMC(u4VDecID, RW_MC_BREF);
    u4DcacPitch |= 0x10;
    vVDecWriteMC(u4VDecID, RW_MC_BREF, u4DcacPitch);    

	#ifdef MPEG4_NEW_MODE  //for cobra/viper/python xiaolei.li
	#if (!defined (CHIP_MT5881)) && (!defined (CHIP_MT5399))
	vVDecWriteAVSPP(u4VDecID,(513*4),(u4VDecReadAVSPP(u4VDecID,(513*4)) & 0xFFFFFFEF)); //set pred_wr last mb row nop enable = 0
	vVDecWriteAVSPP(u4VDecID,(469*4),(u4VDecReadAVSPP(u4VDecID,(469*4))|1)); //set dcac last mb_row nop disable
	#endif
	#endif
}


// *********************************************************************
// Function : void vVLDBarl2Vdec(UINT32 u4VDecID)
// Description : Load sum from barrel shifter to VDec HW
// Parameter : None
// Return    : None
// *********************************************************************
void vVLDBarl2Vdec(UINT32 u4VDecID)
{
    // hw workaround
    // wait hw stable before load sum to mpeg4 mode
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
	u4VDecReadVLD(u4VDecID,RO_VLD_BARL);
    //wait input window stable
	while(!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL)&0x1));
	while((u4VDecReadVLD(u4VDecID, RO_VLD_SUM + (0 << 10)) & 0x20));
#else
    u4VDecReadVLD(u4VDecID,RO_VLD_BARL);
    while (!(u4VDecReadVLD(u4VDecID, RO_VLD_SRAMCTRL + (0 << 10))&0x10000));
    while(!(u4VDecReadVLD(u4VDecID, RO_VLD_FETCHOK + (0 << 10)) & VLD_DRAM_REQ_FIN));
    while((u4VDecReadVLD(u4VDecID, RO_VLD_SUM + (0 << 10)) & 0x20));
#endif    
    vVDecWriteVLD(u4VDecID, RW_VLD_LDSR, 1);
    vVDecWriteVLD(u4VDecID, RW_VLD_LDSR, 0);
}


// *********************************************************************
// Function :void vVLDSetDcacAddr(UINT32 u4VDecID, UINT32 dAddr)
// Description : Set DCAC area start address
// Parameter : None
// Return    : None
// *********************************************************************
void vVLDSetDcacAddr(UINT32 u4VDecID, UINT32 dAddr)
{
    vVDecWriteVLD(u4VDecID, RW_VLD_DCACSA, (dAddr >> 2));
}


// *********************************************************************
// Function :void vVLDDx3Dec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
// Description : Decode DivX3 Picture
// Parameter : prDecPrm: Decode Parameters Set
// Return    : None
// *********************************************************************
void vVLDDx3Dec(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
    vVLDDx3DecPrmProc(u4VDecID, prDecPrm);
    vVLDSetPicType(u4VDecID, prDecPrm);
    vVLDDoDec(u4VDecID, prDecPrm, FALSE, FALSE);
    vVLDMp4TriggerDec(u4VDecID);
}


// *********************************************************************
// Function :void vVLDDx3DecPrmProc(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDx3Prm)
// Description : process DivX3 decode parameters
// Parameter : prDx3Prm: DivX3 decode related parameters
// Return    : None
// *********************************************************************
void vVLDDx3DecPrmProc(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif
    VDEC_INFO_MPEG4_DEC_PRM_T *prMp4Prm  = &(prMpegDecPrm->rPicLayer.rMp4DecPrm);
    VDEC_INFO_DIVX3_PIC_PRM_T *prDx3Prm = &(prMp4Prm->rDep.rDx3DecPrm);
    UINT32 u4Dx3Hdr;
    #if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580) ||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
    UINT32 u4DX3SliceBound;
    #endif
    #if 1 //mtk40088 add for MPEG4 quant_scale
    UINT32 u4RegValue = 0;
    u4RegValue = u4VDecReadVLD(u4VDecID, RW_VLD_BREF);
    vVDecWriteVLD(u4VDecID, RW_VLD_BREF, u4RegValue | 0x100);
    #endif

#if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8555)  
	 if (BSP_GetIcVersion() == IC_8555)
        {
	    if(u4VDecID == 1)
    {
        vWriteReg(0x32000,(u4ReadReg(0x32000) | 0x1));
    }
    else
    {
        vWriteReg(0x2B000,(u4ReadReg(0x2B000) | 0x1));
    }
        }
#endif
    vVDecWriteVLD(u4VDecID, RW_VLD_MP4_FLG, 0x101);
	#ifdef CONFIG_TV_DRV_VFY 
	#ifdef MPEG4_NEW_MODE
	vVDecWriteVLD(u4VDecID, RW_VLD_MP4_FLG, u4VDecReadVLD(u4VDecID, RW_VLD_MP4_FLG)|(1<<16));	
	//vVDecWriteVLD(u4VDecID, 0x2E0, u4VDecReadVLD(u4VDecID, 0x2E0)|(0xFF<<16)|(1<<4));
	//vVDecWriteVLD(u4VDecID, 0x2F4,0xFFFFFFFF);
	//vVDecWriteVLD(u4VDecID, 0x2F8,0xFFFFFFFF);
	{
		UINT32 u4LumaDCDefPred, u4ChromaDCDefPred, u4Temp;
		//UINT64 u8Remainder;

		u4Temp = _DCscaler_311(prMp4Prm->ucVopQuant,0);
		u4LumaDCDefPred = 1024 + (u4Temp>>1);
		u4LumaDCDefPred = u4LumaDCDefPred / u4Temp; //u8Div6432(u4LumaDCDefPred, u4Temp, &u8Remainder);

		u4Temp = _DCscaler_311(prMp4Prm->ucVopQuant,4);
		u4ChromaDCDefPred = 1024 + (u4Temp>>1);
		u4ChromaDCDefPred = u4ChromaDCDefPred / u4Temp; //u8Div6432(u4ChromaDCDefPred, u4Temp, &u8Remainder);

		vVDecWriteAVSPP(u4VDecID, RW_PP_DIVX311_PRED, u4LumaDCDefPred | (u4ChromaDCDefPred<<8));
	}	
	#endif
	#endif
#if(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8555)
	if (BSP_GetIcVersion() == IC_8555)
	{
    if(u4VDecID == 1)
    {
        vWriteReg(0x32000,(u4ReadReg(0x32000) & (~0x1)));
    }
    else
    {
        vWriteReg(0x2B000,(u4ReadReg(0x2B000) & (~0x1)));
    }
       }
#endif
    vVDecWriteVLD(u4VDecID, RW_VLD_PARA, (prMp4Prm->ucVopCdTp << 21));
    #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
    vVLDMp4SetPicSz(u4VDecID, prDecPrm, FALSE);
    #else
    vVLDSetPicSz(u4VDecID, prDecPrm, FALSE);
    #endif
    vVLDSetMBRowPrm(u4VDecID, prDecPrm, FALSE, TRUE);
    vVLDMp4DecPrmProc(u4VDecID, prMp4Prm, (prDecPrm->u4PicW + 15) / 16, 0, 0, 0);
	#ifdef CHIP_MT5399 //xiaolei.li follow vVLDMp4DecPrmProc
	{
		vVDecWriteMC(u4VDecID, RW_MC_CBCR_MV_TYPE, u4VDecReadMC(u4VDecID, RW_MC_CBCR_MV_TYPE) | (1<<8));
	}
	#endif
	#ifdef MPEG4_NEW_MODE
	_SetDIVX3AVCMV(u4VDecID,prMp4Prm);
    #endif
    u4Dx3Hdr = (prDx3Prm->ucAltIAcChromDctIdx) |
              (prDx3Prm->ucAltIAcChromDct << 1) |
              (prDx3Prm->ucAltIAcLumDctIdx << 2) |
              (prDx3Prm->ucAltIAcLumDct << 3) |
              (prDx3Prm->ucAltIDcDct << 4) |
              (prDx3Prm->ucAltPAcDctIdx << 8) |
              (prDx3Prm->ucAltPAcDct << 9) |
              (prDx3Prm->ucAltPDcDct << 10) |
              (prDx3Prm->ucAltMv << 11) |
              (prDx3Prm->ucHasSkip << 16) |
              (prDx3Prm->ucFrameMode << 20);
    vVDecWriteVLD(u4VDecID, RW_VLD_DX3, u4Dx3Hdr);
    #if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580))
    if(fgRunBelowSegmentAfter(IC_8580_VER_B))
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_DVX_BOUND_MODE, 0);
        u4DX3SliceBound = prDx3Prm->ucSliceBoundary[0] | (prDx3Prm->ucSliceBoundary[1] << 16);
        vVDecWriteVLD(u4VDecID, RW_VLD_DVX_BOUND_R1, u4DX3SliceBound);
        u4DX3SliceBound = prDx3Prm->ucSliceBoundary[2] | (prDx3Prm->ucSliceBoundary[3] << 16);
        vVDecWriteVLD(u4VDecID, RW_VLD_DVX_BOUND_R2, u4DX3SliceBound);
        vVDecWriteVLD(u4VDecID, RW_VLD_DVX_BOUND_R3, prDx3Prm->ucSliceBoundary[4]);
    }
    else
    #endif
    {
    #if (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 ||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890))
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_DVX_BOUND_MODE, 0);
        u4DX3SliceBound = prDx3Prm->ucSliceBoundary[0] | (prDx3Prm->ucSliceBoundary[1] << 16);
        vVDecWriteVLD(u4VDecID, RW_VLD_DVX_BOUND_R1, u4DX3SliceBound);
        u4DX3SliceBound = prDx3Prm->ucSliceBoundary[2] | (prDx3Prm->ucSliceBoundary[3] << 16);
        vVDecWriteVLD(u4VDecID, RW_VLD_DVX_BOUND_R2, u4DX3SliceBound);
        vVDecWriteVLD(u4VDecID, RW_VLD_DVX_BOUND_R3, prDx3Prm->ucSliceBoundary[4]);
    }
    #endif
    }
    // Non-DivX3 (MPEG4-only) registers...
    // Maybe it's not necessary but clear them anyway
    // GSLin's
    vVDecWriteVLD(u4VDecID, RW_VLD_MP4_HDR, 0);
    vVDecWriteVLD(u4VDecID, RW_VLD_M4SH, 0);

	#ifndef MPEG4_NEW_MODE
    // HHKuo's
    vVDecWriteVLD(u4VDecID, RW_VLD_DIRE_MD, 0);
  //  vWriteVLD(RW_VLD_BMB1, 0);
  //  vWriteVLD(RW_VLD_BMB2, 0);
  //  vWriteVLD(RW_VLD_BCODE_SA, 0);
    vVDecWriteVLD(u4VDecID, RW_VLD_DIRE_MD_IL, 0);
	#endif
	// CCJu's
    vVDecWriteMC(u4VDecID, RW_MC_WARP_POINT_NUM, 0);
    vVDecWriteMC(u4VDecID, RW_MC_GMC_Y_MV, 0);
    vVDecWriteMC(u4VDecID, RW_MC_GMC_C_MV, 0);

	vVDecWriteVLD(u4VDecID, 184*4, u4VDecReadVLD(u4VDecID, 184*4)|(1<<4));  //for python verification xiaolei.li
}


void vVDecMpegCommSetting(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
//VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;

#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif
     
    vVDecWriteVLD(u4VDecID, RW_VLD_PWRSAVE, 0);  
    //-------------------------------------------------
    // global setting un@}l]@Yi
    //-------------------------------------------------
    vVDecWriteMC(u4VDecID, RW_MC_R1Y, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0YSa)) >> 9); // div 512
    vVDecWriteMC(u4VDecID, RW_MC_R1C, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0CSa)) >> 8); // div 256
    vVDecWriteMC(u4VDecID, RW_MC_R2Y, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1YSa)) >> 9); // div 512
    vVDecWriteMC(u4VDecID, RW_MC_R2C, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1CSa)) >> 8); // div 256
    vVDecWriteMC(u4VDecID, RW_MC_BY,  (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa)) >> 8); // div 256
    vVDecWriteMC(u4VDecID, RW_MC_BC,  (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2CSa)) >> 7); // div 128
    vVDecWriteMC(u4VDecID, RW_MC_BY1,  (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa)) >> 9); // div 256
    vVDecWriteMC(u4VDecID, RW_MC_BC1,  (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2CSa)) >> 8); // div 128


    
#ifdef VDEC_SIM_DUMP00
         if(_u4PicCdTp[u4VDecID] == I_TYPE)
         {
            UTIL_Printf("//Dec0_Y_0x%x.bin_Size_0x%x\n",PHYSICAL((UINT32)(_u4WorkBuffer[_u4DecBufIdx[u4VDecID]*2])),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<8);
        UTIL_Printf("//Dec0_C_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4DecBufIdx[u4VDecID]*2 + 1]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<7);
         }
         else if(_u4PicCdTp[u4VDecID] == P_TYPE)
         {
            UTIL_Printf("//Dec0_Y_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4DecBufIdx[u4VDecID]*2]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<8);
        UTIL_Printf("//Dec0_C_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4DecBufIdx[u4VDecID]*2+ 1]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<7);
        UTIL_Printf("//Ref0_Y_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4FRefBufIdx[u4VDecID]*2]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<8);
        UTIL_Printf("//Ref0_C_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4FRefBufIdx[u4VDecID]*2 + 1]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<7);
         }
         else
         {
            UTIL_Printf("//Dec0_Y_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4DecBufIdx[u4VDecID]*2]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<8);
        UTIL_Printf("//Dec0_C_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4DecBufIdx[u4VDecID] *2+ 1]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<7);
        UTIL_Printf("//Ref0_Y_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4FRefBufIdx[u4VDecID]*2]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<8);
        UTIL_Printf("//Ref0_C_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4FRefBufIdx[u4VDecID]*2 + 1]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<7);    
        UTIL_Printf("//Ref1_Y_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4BRefBufIdx[u4VDecID]*2]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<8);
        UTIL_Printf("//Ref1_C_0x%x.bin_Size_0x%x\n",PHYSICAL(_u4WorkBuffer[_u4BRefBufIdx[u4VDecID] *2+ 1]),((prDecPrm->u4PicH + 15) >> 4)*((prDecPrm->u4PicW+ 15) >> 4)<<7);
         }
#endif


    vSetMcDecBuf(u4VDecID, prDecPrm);
    vSetDecFld(u4VDecID, prDecPrm);
    // addr swap mode
    vVDecWriteMC(u4VDecID, RW_MC_ADDRSWAP, prDecPrm->ucAddrSwapMode);

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8550)
    #if MPEG2_TEST_TV_BITSTREAM
    //Set NBM address swap mode
    vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL, 
                 ((u4VDecReadMC(u4VDecID, RW_MC_NBM_CTRL)  & 0xFFFFFFFC) |prDecPrm->ucAddrSwapMode));
    #else
    vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL, 
                 ((u4VDecReadMC(u4VDecID, RW_MC_NBM_CTRL)  & 0xFFFFFFF8) |prDecPrm->ucAddrSwapMode));
    #endif
     
#if VDEC_MC_NBM_OFF
     //Turn off NBM address swap mode
     vVDecWriteMC(u4VDecID, RW_MC_NBM_CTRL, 
               (u4VDecReadMC(u4VDecID, RW_MC_NBM_CTRL) | (RW_MC_NBM_OFF)  ));
#endif

#if VDEC_DDR3_SUPPORT //|| MPEG2_TEST_TV_BITSTREAM
     //Turn on DDR3 mode
     vVDecWriteMC(u4VDecID, RW_MC_DDR_CTRL0, 
               ((u4VDecReadMC(u4VDecID, RW_MC_DDR_CTRL0)  & 0xFFFFFFFE) |0x1));

     vVDecWriteMC(u4VDecID, RW_MC_DDR_CTRL1, 
               ((u4VDecReadMC(u4VDecID, RW_MC_DDR_CTRL1)  & 0xFFFFFFFE) |0x1));
#endif

#endif

#if (!CONFIG_DRV_FPGA_BOARD)
    vVDecWriteMC(u4VDecID, RW_MC_MODE_CTL, MC_QIU_BANK4|MC_QIU_BANK8|MC_DRAM_REQ_DELAY_1T|MC_DRAM_REQ_MERGE_OFF|MC_MV_MERGE_OFF);
#endif

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    #if (CONFIG_CHIP_VER_CURR < CONFIG_CHIP_VER_MT8550)
       vVDecWriteVLD(u4VDecID, RW_VLD_PIC_W_MB, ((prDecPrm->u4PicBW + 15)>> 4));  
    #else
      #if VDEC_DDR3_SUPPORT   
       #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561)
	   vVDecWriteMC(u4VDecID, RW_MC_PIC_W_MB,((prMpegDecPrm->u4DecW + 63)>>6)<<2);
	   #else
          vVDecWriteVLD(u4VDecID, RW_VLD_PIC_W_MB, ((prMpegDecPrm->u4DecW + 63)>>6)<<2);  
	#endif
      #else
       #if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580 || CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 ||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT5890))
	   vVDecWriteMC(u4VDecID, RW_MC_DDR3_EN, (u4VDecReadMC(u4VDecID, RW_MC_DDR3_EN)  & 0xFFFFFFFE));
	   vVDecWriteMC(u4VDecID, RW_MC_PIC_W_MB,((prMpegDecPrm->u4DecW + 15)>> 4));
      #else
          vVDecWriteVLD(u4VDecID, RW_VLD_PIC_W_MB, ((prMpegDecPrm->u4DecW + 15)>> 4));   
      #endif
    #endif
#endif
#endif
  //Mars,[2004/06/08],turn on for P-scan 32-pulldown detection
  #ifdef PSCAN_SUPPORT
  //max set Motion Plane address
  //  if(fgIsPScanTv())
  
    // Don't turn ON Motion Plane HW in MPEG4, HW not support in DIVX
    if (_fgPScanOn && fgIsMpeg2())
    {
        vSetPSMP();//]wmotion plane Ū@ӡAg@
        vSetPSCtrl();//]wPSacn MC control register
    }
    //max
  #endif
    if (prMpegDecPrm->rMpegPpInfo.fgPpEnable)
    {   // Qing Li fix here for 8550 pp reg set
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
        vVDecWriteMC(u4VDecID, RW_MC_PP_X_RANGE, ((prDecPrm->u4PicW + 15) >> 4) - 1);
        vVDecWriteMC(u4VDecID, RW_MC_PP_Y_RANGE, (((prDecPrm->u4PicH + 15) >> 4) >> (prDecPrm->ucPicStruct != 3)) - 1);
        //vVDecWriteAVCVLD(u4VDecID, RW_AVLD_SHDR_2, 0x6E00);
        //vVDecWriteMC(u4VDecID, RW_MC_PP_MODE, H264_MODE);
    }
    else
    {
        #if (VDEC_DDR3_SUPPORT && (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)) // DDR3+PPOUT setting mtk40343
		if((prDecPrm->u4PicW > 16) && (prDecPrm->u4PicH > 16))//when width or height is less than 16,should be set MC out @mtk40343
		{
		vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, 1);
		vVDecWriteMC(u4VDecID, RW_MC_DDR3_EN, u4VDecReadMC(u4VDecID, RW_MC_DDR3_EN)&(~(0x1)));// pp out buffer enable
		vVDecWriteMC(u4VDecID, RW_MC_PP_DBLK_MODE, 0);//turn off de-blocking
		vVDecWriteMC(u4VDecID, RW_MC_PP_WB_BY_POST, 1); // wirte PP out only
	 		if(prDecPrm->ucDecFBufIdx == 0)
			{
			   vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0YSa))>>9);
			   vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0CSa)) >> 8);
			}
			else if(prDecPrm->ucDecFBufIdx == 1)
			{
			   vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1YSa))>>9);
			   vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1CSa)) >> 8);
			}
			else if(prDecPrm->ucDecFBufIdx == 2)
			{
				vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa))>>9);
				vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2CSa)) >> 8);
			}
			vVDecWriteMC(u4VDecID, RW_MC_PP_MB_WIDTH, ((prDecPrm->u4PicW + 63) >> 6)<<2);
			vVDecWriteMC(u4VDecID, RW_MC_PP_X_RANGE, ((prDecPrm->u4PicW + 15) >> 4) - 1);
		       vVDecWriteMC(u4VDecID, RW_MC_PP_Y_RANGE, (((prDecPrm->u4PicH + 15) >> 4) >> (prDecPrm->ucPicStruct != 3)) - 1);
		}
		else
		{
        vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, 0);
    }
   #elif VDEC_UFO_ENABLE
         if((prDecPrm->u4PicW > 16) && (prDecPrm->u4PicH > 16))//when width or height is less than 16,should be set MC out @mtk40343
        {
    	  vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, 1);
    	  vVDecWriteMC(u4VDecID, 0x834, u4VDecReadMC(u4VDecID, 0x834)&(~(0x1)));// pp out buffer enable
    	  //vVDecWriteMC(u4VDecID, RW_MC_PP_DBLK_MODE, 0);//turn off de-blocking
    	  vVDecWriteMC(u4VDecID, RW_MC_PP_DBLK_MODE, 0);//turn off de-blocking
    	  vVDecWriteMC(u4VDecID, RW_MC_PP_WB_BY_POST, 1); // wirte PP out only

            //UINT32 temp;//test only
            UINT32 u4PIC_W_H;
            u4PIC_W_H = ((((prDecPrm->u4PicW+15)>>4)-1)<<16)|(((prDecPrm->u4PicH+15)>>4)-1);
            vVDecWriteMC(u4VDecID, RW_MC_PP_MB_WIDTH, (prDecPrm->u4PicW + 15) >> 4); ///????????????????

            //temp = ((prDecPrm->u4PicW + 15) >> 4);
            //printk("[Louis] u4PicW_0 %d\n",temp);

            vVDecWriteMC(u4VDecID, 700*4, u4PIC_W_H);//700*4 = 0xAF0
            vVDecWriteMC(u4VDecID, 664*4, 0x11);//664*4 = 0xA60 //set ufo_en_flag =1
            //for PP out

            if(prDecPrm->ucDecFBufIdx == 0)
            {
                vVDecWriteMC(u4VDecID, 698*4, u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0YSa+prDecPrm->u4PIC_SIZE_BS));//698*4 = 0xAE8
                vVDecWriteMC(u4VDecID, 699*4, u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0YSa+prDecPrm->u4PIC_SIZE_BS+prDecPrm->u4UFO_LEN_SIZE_Y));//699*4 = 0xAEC
                vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0YSa))>>9);
                vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic0CSa)) >> 8);
                memset(prMpegDecPrm->rMpegFrameBufSa.u4Pic0YSa, 0 ,prMpegDecPrm->u4DramPicArea);            
            }
            else if(prDecPrm->ucDecFBufIdx == 1)
            {
                vVDecWriteMC(u4VDecID, 698*4, u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1YSa+prDecPrm->u4PIC_SIZE_BS));//698*4 = 0xAE8
                vVDecWriteMC(u4VDecID, 699*4, u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1YSa+prDecPrm->u4PIC_SIZE_BS+prDecPrm->u4UFO_LEN_SIZE_Y));//699*4 = 0xAEC
                vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1YSa))>>9);
                vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic1CSa)) >> 8);
                memset(prMpegDecPrm->rMpegFrameBufSa.u4Pic1YSa, 0 ,prMpegDecPrm->u4DramPicArea);            
            }
            else if(prDecPrm->ucDecFBufIdx == 2)
            {
                vVDecWriteMC(u4VDecID, 698*4, u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa+prDecPrm->u4PIC_SIZE_BS));//698*4 = 0xAE8
                vVDecWriteMC(u4VDecID, 699*4, u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa+prDecPrm->u4PIC_SIZE_BS+prDecPrm->u4UFO_LEN_SIZE_Y));//699*4 = 0xAEC
                vVDecWriteMC(u4VDecID, RW_MC_PP_Y_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa))>>9);
                vVDecWriteMC(u4VDecID, RW_MC_PP_C_ADDR, (u4AbsDramANc(prMpegDecPrm->rMpegFrameBufSa.u4Pic2CSa)) >> 8);
                memset(prMpegDecPrm->rMpegFrameBufSa.u4Pic2YSa, 0 ,prMpegDecPrm->u4DramPicArea);            
            }
            
            //for MC(UFO DEC)
            vVDecWriteMC(u4VDecID, 663*4, prDecPrm->u4PIC_SIZE_BS);//663*4 = 0xA5C
            vVDecWriteMC(u4VDecID, 701*4, prDecPrm->u4UFO_LEN_SIZE_Y);//701*4 = 0xAF4 
            vVDecWriteMC(u4VDecID, 343*4, prDecPrm->u4PIC_SIZE_Y_BS);//343*4 = 0x55C
            vVDecWriteMC(u4VDecID, RW_MC_PP_X_RANGE, ((prDecPrm->u4PicW + 15) >> 4) - 1);
            vVDecWriteMC(u4VDecID, RW_MC_PP_Y_RANGE, (((prDecPrm->u4PicH + 15) >> 4) >> (prDecPrm->ucPicStruct != 3)) - 1);

            vVDecWriteAVSPP(u4VDecID, 16*4, 1<<24|(u4VDecReadMP4PP(u4VDecID, 16*4)));//change to dram 16*32
        }
        #else
        vVDecWriteMC(u4VDecID, RW_MC_PP_ENABLE, 0);        
	#endif
    }

#if VDEC_SARNOFF_ON//Qing Li add here for testing sarnoff
    vVDecWriteVLD(u4VDecID, RO_AVLD_ERR_MESSAGE, (u4VDecReadVLD(u4VDecID, RO_AVLD_ERR_MESSAGE) | (0x1 << 18) | (0x1 << 20)));
#endif

    if((u4VDecReadVLD(u4VDecID, RW_VLD_PICSZ)>> 31)) //if mpeg1,turn on some error concealment bits
         vVDecWriteVLD(u4VDecID, RO_AVLD_ERR_MESSAGE, (u4VDecReadVLD(u4VDecID, RO_AVLD_ERR_MESSAGE) | (0x1 << 4) | (0x1 << 6)));

     

#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8530)
    vVDecWriteMC(u4VDecID, 0x5E4, (u4VDecReadMC(0, 0x5E4) |(0x1 <<12)) );
    //vVDecWriteMC(0, 0x660, (u4VDecReadMC(0, 0x660) |(0x80000000)) );     //SJ: mark this action. MC/Prediction Dram Agent Switch. xiaolei.li mustang emu
/*
   #ifndef VDEC_PIP_WITH_ONE_HW
    vVDecWriteMC(1, 0x5E4, (u4VDecReadMC(1, 0x5E4) |(0x1 <<12)) );
    //vVDecWriteMC(1, 0x660, (u4VDecReadMC(1, 0x660) |(0x80000000)) );    //SJ: mark this action. MC/Prediction Dram Agent Switch. xiaolei.li mustang emu
   #endif
   */
#endif

#ifdef DEC_END_POLLING //clear vdec interrupt
vVDecWriteMISC(u4VDecID,41*4,u4VDecReadMISC(u4VDecID,41*4)|(1<<4));
vVDecWriteMISC(u4VDecID,41*4,u4VDecReadMISC(u4VDecID,41*4)&(~(1<<4)));
#endif
}


// *********************************************************************
// Function : void vSetDecFld(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
// Description : Set Decoding Field to Register
// Parameter : None
// Return    : None
// *********************************************************************
void vSetDecFld(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
    UINT32 u4DecFld;
    UINT32 u42ndFld;
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif

    if (prDecPrm->ucPicStruct == FRM_PIC) //(fgIsFrmPic(dwIdx))
    {
        u4DecFld = MC_TOP_FLD;
        u42ndFld = MC_2ND_FLD_OFF;
    }
    else
    {
        if (prDecPrm->ucPicType != B_TYPE) //(fgIsRefPic(dwIdx))
        {
            if (prMpegDecPrm->fgDec2ndFld) //(fgIsDecFlagSet(DEC_FLG_2ND_FLD_PIC))
            {
                u4DecFld = (prDecPrm->ucPicStruct == TWO_FLDPIC_TOPFIRST) ?
                           MC_BOTTOM_FLD : MC_TOP_FLD;
                u42ndFld = MC_2ND_FLD_ON;
            }
            else
            {
                u4DecFld = (prDecPrm->ucPicStruct == TWO_FLDPIC_TOPFIRST) ?
                           MC_TOP_FLD : MC_BOTTOM_FLD;
                u42ndFld = MC_2ND_FLD_OFF;
            }
        }
        else  // B Picture
        {
            u4DecFld = (prDecPrm->ucPicStruct == TOP_FLD_PIC) ?
                       MC_TOP_FLD : MC_BOTTOM_FLD;
            u42ndFld = MC_2ND_FLD_OFF;
        }
    }
  
    prMpegDecPrm->ucDecFld = BYTE0(u4DecFld);
    vMCSetDecFld(u4VDecID, u4DecFld, u42ndFld);
}


// *********************************************************************
// Function : void vMCSetDecFld(UINT32 u4VDecID, UINT32 u4DecFld, UINT32 u42ndFldSw)
// Description : MC Set Decode Field
// Parameter : dDecFld: MC_TOP_FLD/MC_BOTTOM_FLD
//             d2ndFldSw: MC_2ND_FLD_ON/MC_2ND_FLD_OFF
// Return    : None
// *********************************************************************
void vMCSetDecFld(UINT32 u4VDecID, UINT32 u4DecFld, UINT32 u42ndFldSw)
{
    if (u4DecFld == MC_TOP_FLD)
    {
        vVDecWriteMC(u4VDecID, RW_MC_DECFLD, 0);
    }
    else
    {
        vVDecWriteMC(u4VDecID, RW_MC_DECFLD, 1);
    }
    if (u42ndFldSw == MC_2ND_FLD_ON)
    {
        vVDecWriteMC(u4VDecID, RW_MC_2FLDMD, 1);
    }
    else
    {
        vVDecWriteMC(u4VDecID, RW_MC_2FLDMD, 0);
    }
}


// *********************************************************************
// Function : void vSetMcDecBuf(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
// Description : Write Decoding Buffer into HW Register
// Parameter : None
// Return    : None
// *********************************************************************
void vSetMcDecBuf(UINT32 u4VDecID, VDEC_INFO_DEC_PRM_T *prDecPrm)
{
    //VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#if ((CONFIG_DRV_VERIFY_SUPPORT) ||(CONFIG_DRV_FPGA_BOARD) && (!VDEC_DRV_PARSER))
    VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *) &(prDecPrm->SpecDecPrm.rVDecMPEGDecPrm);
#else
     VDEC_INFO_MPEG_DEC_PRM_T *prMpegDecPrm = (VDEC_INFO_MPEG_DEC_PRM_T *)prDecPrm->prVDecCodecHalPrm;
#endif

    switch (prDecPrm->ucPicType)//(prGetFrmInfo(_dwDecBufIdx)->bPicCdTp)
    {
        case I_TYPE:
        case P_TYPE:
            vMCSetOutputBuf(u4VDecID, prDecPrm->ucDecFBufIdx, prMpegDecPrm->u4FRefBufIdx);
            vMCWriteToDigBuf(u4VDecID, OFF);
            break;
        case B_TYPE:
            /*if(fgIsDecCfgSet(DEC_CFG_DIGEST))
            {
              if(fgVDecDigNeed2StepVP()) {
                vMCSetOutputBuf(MC_DIG_BUF, _dwFRefBufIdx);
                vMCWriteToDigBuf(ON);
              } else
              {
                vMCSetOutputBuf(MC_DIG_BUF, _dwFRefBufIdx);
                vMCWriteToDigBuf(ON);
              }
            }
            else */
            {
              vMCSetOutputBuf(u4VDecID, prDecPrm->ucDecFBufIdx + 2, prMpegDecPrm->u4FRefBufIdx);
              vMCWriteToDigBuf(u4VDecID, OFF);
            }
          break;
    }
}


// *********************************************************************
// Function : void vMCWriteToDigBuf(UINT32 u4VDecID, BOOL fgSwitch)
// Description : MC Write to DIG buffer switch
//               write to decoded buffer and digest buffer at the same time
// Parameter : fgSwitch: ON/OFF
// Return    : None
// *********************************************************************
void vMCWriteToDigBuf(UINT32 u4VDecID, BOOL fgSwitch)
{
    if (fgSwitch)
    {
        vVDecWriteMC(u4VDecID, RW_MC_WMBDIG, 1);
    }
    else
    {
        vVDecWriteMC(u4VDecID, RW_MC_WMBDIG, 0);
    }
}


// *********************************************************************
// Function :void vVLDSetBRef(UINT32 u4VDecID, BOOL fgPVop)
// Description : Set RW_VLD_BREF
// Parameter : fgPVop: indicate the backward reference picture is an P-VOP
// Return    : None
// *********************************************************************
void vVLDSetBRef(UINT32 u4VDecID, BOOL fgPVop)
{
    if (fgPVop)
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_BREF, (u4VDecReadVLD(u4VDecID, RW_VLD_BREF) & 0xFFFFFFFE)); //should keep 122[8]=1. xiaolei.li 2012.4.4
		//vVDecWriteVLD(u4VDecID, RW_VLD_BREF, 0);
	}
    else
    {
        vVDecWriteVLD(u4VDecID, RW_VLD_BREF, (u4VDecReadVLD(u4VDecID, RW_VLD_BREF) | 1)); //should keep 122[8]=1. xiaolei.li 2012.4.4
		//vVDecWriteVLD(u4VDecID, RW_VLD_BREF, 1);
	}
}


// *********************************************************************
// Function :void vMCSetBRef(UINT32 u4VDecID, BOOL fgIVop)
// Description : Set RW_MC_BREF
// Parameter : fgIVop: indicate the backward reference picture is an I-VOP
// Return    : None
// *********************************************************************
void vMCSetBRef(UINT32 u4VDecID, BOOL fgIVop)
{
    UINT32 u4VldDcMvSel = u4VDecReadVLD(u4VDecID, RW_VLD_DCMVSEL);
    UINT32 u4McBRef = u4VDecReadMC(u4VDecID, RW_MC_BREF);
    // Note for 1389Lite:
    // Before v5024, this function is not 1389Lite compatible.
    // If an earlier code needs to run in 1389Lite,
    // its vMCSetBRef() should be replaced by this newer one in v5024.
    if (fgIVop)
    {
        vVDecWriteMC(u4VDecID, RW_MC_BREF, u4McBRef | NML_BREF_I_VOP); // MT1389 normal
        vVDecWriteVLD(u4VDecID, RW_VLD_DCMVSEL, u4VldDcMvSel | LITE_BREF_I_VOP); // MT1389 lite
    }
    else
    {
        vVDecWriteMC(u4VDecID, RW_MC_BREF, u4McBRef & (~NML_BREF_I_VOP));  // MT1389 normal
        vVDecWriteVLD(u4VDecID, RW_VLD_DCMVSEL, u4VldDcMvSel & (~LITE_BREF_I_VOP)); // MT1389 lite
    }
}

#define DUMP_ERROR 0
void VDecDumpMpegRegister(UINT32 u4VDecID)
{
                
     UINT32 u4Val;
     int reg;
     int vldstart;
     //    int vldstart = 42;
     //    int vldsize = 200;
     int mcstart = 0;
     int mcsize = 700;
	
	 UTIL_Printf("AVC MV Top Settings\n");
	 for(vldstart = 0; vldstart<200; vldstart++)
	 {
        u4Val = u4VDecReadAVCMV(u4VDecID, (vldstart << 2));
	#if DUMP_ERROR 
        ((UINT32 *)(_pucRegister[u4VDecID]))[vldstart] = u4Val;
	#else
         //UTIL_Printf("%d (0x%x)  = 0x%4x\n",vldstart, (vldstart<<2),u4Val);
         UTIL_Printf("AVC_MV[%d]  = 0x%8x\n",vldstart, u4Val);
	#endif
     }
	 
     UTIL_Printf("VLD base Settings\n");
     for(vldstart = 33; vldstart < 40; vldstart++)
     {
        u4Val = u4VDecReadVLD(u4VDecID, (vldstart << 2));
	#if DUMP_ERROR 
        ((UINT32 *)(_pucRegister[u4VDecID]))[vldstart] = u4Val;
	#else
         UTIL_Printf("VLD[%d] = 0x%8x\n",vldstart, u4Val);
	#endif
     }
     for(vldstart = 42; vldstart < 71; vldstart++)
     {
         u4Val = u4VDecReadVLD(u4VDecID, (vldstart << 2));
	 #if DUMP_ERROR 
         ((UINT32 *)(_pucRegister[u4VDecID]))[vldstart] = u4Val;
	 #else
         UTIL_Printf("VLD[%d] = 0x%8x\n",vldstart, u4Val);
	 #endif
     }
     for(vldstart = 72; vldstart < 131; vldstart++)
     {
         u4Val = u4VDecReadVLD(u4VDecID, (vldstart << 2));
	 #if DUMP_ERROR 
         ((UINT32 *)(_pucRegister[u4VDecID]))[vldstart] = u4Val;
	 #else
         UTIL_Printf("VLD[%d] = 0x%8x\n",vldstart, u4Val);
	 #endif
     }
     for(vldstart = 132; vldstart < 256; vldstart++)
     {
         u4Val = u4VDecReadVLD(u4VDecID, (vldstart << 2));
	#if DUMP_ERROR 
         ((UINT32 *)(_pucRegister[u4VDecID]))[vldstart] = u4Val;
	#else
        UTIL_Printf("VLD[%d] = 0x%8x\n",vldstart, u4Val);
	#endif
     }
	#if ((CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)||(CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561))
	 UTIL_Printf("VLD_TOP Settings \n");
	 for(vldstart=0;vldstart < 64;vldstart++)
	 {
		  u4Val = u4VDecReadVLDTOP(u4VDecID, (vldstart << 2));
	#if DUMP_ERROR 
		  ((UINT32 *)(_pucRegister[u4VDecID]))[vldstart] = u4Val;
	#else
		  UTIL_Printf("VLD_TOP[%d]  = 0x%8x\n",vldstart, u4Val);
	#endif
	 
	 }
	#endif
	 UTIL_Printf("PP Settings \n");
	 for(vldstart=0;vldstart < 601;vldstart++)
	 {
		  u4Val = u4VDecReadMP4PP(u4VDecID, (vldstart << 2));
	#if DUMP_ERROR 
		  ((UINT32 *)(_pucRegister[u4VDecID]))[vldstart] = u4Val;
	#else
		  UTIL_Printf("PP[%d] = 0x%8x\n",vldstart, u4Val);
	#endif
	 
	 }	 
     UTIL_Printf("MC Settings\n");
     for(reg= mcstart; reg < (mcstart+mcsize); reg++)
     {          
         u4Val = u4VDecReadMC(u4VDecID, (reg << 2));
       #if DUMP_ERROR 
         ((UINT32 *)(_pucRegister[u4VDecID]))[reg + 0x100] = u4Val;
	   #else
         UTIL_Printf("MC[%d] = 0x%8x\n", reg, u4Val);
	   #endif
     }
     UTIL_Printf("IS Settings\n");
     for(reg = 128; reg < 192; reg++)
     {
         u4Val = u4VDecReadMP4PP(u4VDecID, (reg << 2));
         UTIL_Printf("PP[%d] = 0x%8x\n", reg, u4Val);
     }
     UTIL_Printf("IQ Settings\n");
     for(reg = 320; reg < 384; reg++)
     {
         u4Val = u4VDecReadMP4PP(u4VDecID, (reg << 2));
         UTIL_Printf("PP[%d] = 0x%8x\n", reg, u4Val);
     }
     UTIL_Printf("IT Settings\n");
     for(reg = 576; reg < 640; reg++)
     {
        u4Val = u4VDecReadMP4PP(u4VDecID, (reg << 2));
        UTIL_Printf("PP[%d] = 0x%8x\n", reg, u4Val);
     }
                
     u4Val = u4VDecReadVLD(u4VDecID, 0x00);
     UTIL_Printf("VLD[%d] = 0x%8x\n", (0x00>>2), u4Val);
                  
     u4Val = u4VDecReadMC(u4VDecID, (608 << 2));
     UTIL_Printf("MC[%d] = 0x%8x\n", 608, u4Val);
     UTIL_Printf("CRC register! \n");
     vVDecWriteCRC(u4VDecID,VDEC_CRC_REG_EN,0x01);//crc enable ; mc agent
     for(reg = 0; reg < 65; reg++)
     {
        u4Val = u4VDecReadCRC(u4VDecID, (reg << 2));
        UTIL_Printf("CRC[%d] = 0x%8x\n", reg, u4Val);
    }
}
UINT32 u4VDecReadMP4PP(UINT32 u4VDecID, UINT32 u4Addr)
{
#if (!CONFIG_DRV_FPGA_BOARD)
     u4VDecID = 0;
#endif

    if (u4VDecID == 0)
    {
        return (u4ReadReg(AVS_PP_REG_OFFSET0 + u4Addr));
    }
    else
    {
        return (u4ReadReg(AVS_PP_REG_OFFSET1 + u4Addr));
    }
}
#ifdef DUMP_REG
UINT32 _aru4DumpData[0x200];
// *********************************************************************
// Function :void VDec_DumpReg(UCHAR ucMpvId)
// Description : Set RW_MC_BREF
// Parameter : fgIVop: indicate the backward reference picture is an I-VOP
// Return    : None
// *********************************************************************
void VDec_DumpReg(UCHAR ucMpvId)
{
    UINT8  ucIdx;
    UINT32 u4DumpIdx;

    UINT32 *pu4DumpArea;

    //pu4DumpArea = (UINT32*)x_mem_alloc(0x200);
    pu4DumpArea = &_aru4DumpData;
    u4DumpIdx = 0;
    
    for (ucIdx = 34; ucIdx <= 39; ucIdx++)
    {
        pu4DumpArea[u4DumpIdx] = u4VDecReadVLD(ucMpvId, 4*ucIdx);
        u4DumpIdx ++;
    }

    for (ucIdx = 42; ucIdx <= 112; ucIdx++)
    {
        pu4DumpArea[u4DumpIdx] = u4VDecReadVLD(ucMpvId, 4*ucIdx);
        u4DumpIdx ++;
    }


    for (ucIdx = 0; ucIdx <= 43; ucIdx++)
    {
        pu4DumpArea[u4DumpIdx] = u4VDecReadMC(ucMpvId, 4*ucIdx);
        u4DumpIdx ++;
    }

    pu4DumpArea[u4DumpIdx] = u4VDecReadMC(ucMpvId, 4*114);
    u4DumpIdx ++;
    
    //x_mem_free(pu4DumpArea);

}
  #endif

#ifdef CONFIG_TV_DRV_VFY 
#ifdef MPEG4_NEW_MODE
static UINT32 _DCscaler_311(UINT32 quant, UINT32 block_num)
  {
	  UINT32 type = (block_num < 4) ? 0 : 1;
	  UINT32 scale;
	  if (quant <= 4) 
		  return 8;
	  else if (quant <= 8) 
	  {
		  scale = quant<<1;
		  if(type)
			  return (quant + 13)>>1;
		  else
			  return scale;
	  }
	  else if (quant <= 24)
	  {
		  scale = quant + 8;
		  if(type)
			  return (quant + 13)>>1;
		  else
			  return scale;
	  }
	  else
	  {
		  scale = (quant<<1) - 16;
		  if(type)
			  return quant - 6;
		  else
			  return scale;
	  }
  }
  
static void _SetMpeg4AVCMV(UINT32 u4VDecID,VDEC_INFO_MPEG4_DEC_PRM_T* prDecPrm)
  {
	  UINT32 u4Temp = 0;
	  INT32 u4Trb, u4Trd, u4Trbi, u4Trdi; //Use int32 instead of uint32.
	  
	  u4Temp = (UINT32)(prDecPrm->rDep.rM4vDecPrm.prVol->ucQuarterSample | (prDecPrm->rDep.rM4vDecPrm.prVop->fgTopFldFirst<<1));
	  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_FIELD, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_FIELD) | u4Temp);
	  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_MVSA, (u4AbsDramANc(prDecPrm->rMpeg4WorkBufSa.u4MvecSa)>>4));
	  
	  // direct mode, check co-located picture type.
	  if (prDecPrm->ucVopCdTp == VCT_B && prDecPrm->rDep.rM4vDecPrm.prVop->ucBRefCdTp == VCT_I)
	  {
		  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_COL_PIC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_COL_PIC) | 0x1);
	  }
	  else
	  {
		  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_COL_PIC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_COL_PIC) & 0xFFFFFFFE);
	  }
	   
	  //if (prDecPrm->u1VideoType == VIDEO_TYPE_DX311)
	  //{
	  //	  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_GMC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_GMC) & 0xC000C000);
	  //	  return;
	  //}
	  //else
	  {
		  VDEC_INFO_MPEG_GMC_PRM_T *prGmcPrm = prDecPrm->rDep.rM4vDecPrm.prVop->prGmcPrm;
		  if(prDecPrm->rDep.rM4vDecPrm.prVol->ucQuarterSample)
		  {
			  u4Temp = (((prGmcPrm->i4GmcYMvX) << 16) & 0x3FFFFFFF) | ((prGmcPrm->i4GmcYMvY) & 0x3FFF);
		  }
		  else
		  {
			  u4Temp = (((prGmcPrm->i4GmcYMvX>>1) << 16) & 0x3FFFFFFF) | ((prGmcPrm->i4GmcYMvY>>1) & 0x3FFF);
		  }
		  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_GMC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_GMC) | u4Temp);
	  }
  
	  
	  u4Trb = prDecPrm->rDep.rM4vDecPrm.prVop->prDirMd->u4Trb;
	  u4Trd = prDecPrm->rDep.rM4vDecPrm.prVop->prDirMd->u4Trd;
	  u4Trbi = prDecPrm->rDep.rM4vDecPrm.prVop->prDirMd->u4Trbi;
	  u4Trdi = prDecPrm->rDep.rM4vDecPrm.prVop->prDirMd->u4Trdi;
  
	  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TRFRAME, (u4Trb << 16) | u4Trd);
  
	  if(u4Trd != 0)
	  {
		  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_DIV, (((u4Trb << 14) / u4Trd) << 16) | (((u4Trd - u4Trb) << 14) / u4Trd));
	  }
	  
	  if (prDecPrm->rDep.rM4vDecPrm.prVol->ucInterlaced)
	  {
		  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TRFRAME_IL, (u4Trbi << 16) | u4Trdi);
		  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_P1, ((u4Trbi+1) << 16) | (u4Trdi+1));
		  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_M1, ((u4Trbi-1) << 16) | (u4Trdi-1));
  
		  if(u4Trdi != 0)
		  {
			  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_DIV, (((u4Trbi<< 14) / u4Trdi) << 16) | (((u4Trdi - u4Trbi) << 14) / u4Trdi));
		  }
		  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_DIV_P1, ((((u4Trbi+1) << 14) / (u4Trdi+1)) << 16) | (((u4Trdi - u4Trbi) << 14) / (u4Trdi+1)));
		  
		  if(u4Trdi == 1)
		  {
			  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_DIV_M1, 0);
		  }
		  else
		  {
			  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_DIV_M1, ((((u4Trbi-1) << 14) / (u4Trdi-1)) << 16) | (((u4Trdi - u4Trbi) << 14) / (u4Trdi-1)));
		  }
	  }

	  vVDecWriteAVCMV(u4VDecID,183*4, 4);
	  vVDecWriteAVCMV(u4VDecID,185*4, 0);
	  vVDecWriteAVCMV(u4VDecID,186*4, 0x11);
	  vVDecWriteAVCMV(u4VDecID,187*4, 0xffffffff);
	  vVDecWriteAVCMV(u4VDecID,189*4, 0);
	  vVDecWriteAVCMV(u4VDecID,190*4, 0x40000000);
	  vVDecWriteAVCMV(u4VDecID,191*4, 0x40000000);
	  
 }

static void _SetDIVX3AVCMV(UINT32 u4VDecID,VDEC_INFO_MPEG4_DEC_PRM_T* prDecPrm)
{
	UINT32 u4Temp = 0;
	INT32 u4Trb, u4Trd, u4Trbi, u4Trdi; //Use int32 instead of uint32.

	// mark this two equation because DIVX3 doesnot have ucQuarterSample & fgTopFldFirst.
	// otherwise, there happens virtual add error. xiaolei.li
	//u4Temp = (UINT32)(prDecPrm->rDep.rM4vDecPrm.prVol->ucQuarterSample | (prDecPrm->rDep.rM4vDecPrm.prVop->fgTopFldFirst<<1));
	//vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_FIELD, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_FIELD) | u4Temp);

	vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_MVSA, (u4AbsDramANc(prDecPrm->rMpeg4WorkBufSa.u4MvecSa)>>4));
	// direct mode, check co-located picture type.
	if (prDecPrm->ucVopCdTp == VCT_B && prDecPrm->rDep.rM4vDecPrm.prVop->ucBRefCdTp == VCT_I)
	{
		vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_COL_PIC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_COL_PIC) | 0x1);
	}
	else
	{
		vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_COL_PIC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_COL_PIC) & 0xFFFFFFFE);
	}
	//if (prDecPrm->u1VideoType == VIDEO_TYPE_DX311)
	//{
			vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_GMC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_GMC) & 0xC000C000);
	//		return;
	//}

}
static void _SetM4VAVCMV(UINT32 u4VDecID,VDEC_INFO_MPEG4_DEC_PRM_T* prDecPrm)
{
	UINT32 u4Temp = 0;
	INT32 u4Trb, u4Trd, u4Trbi, u4Trdi; //Use int32 instead of uint32.
	//UTIL_Printf("i4GmcYMvX = %d, i4GmcYMvY = %d\n",prDecPrm->rDep.rM4vDecPrm.prVop->prGmcPrm->i4GmcYMvX,prDecPrm->rDep.rM4vDecPrm.prVop->prGmcPrm->i4GmcYMvY);

	u4Temp = (UINT32)(prDecPrm->rDep.rM4vDecPrm.prVol->ucQuarterSample | (prDecPrm->rDep.rM4vDecPrm.prVop->fgTopFldFirst<<1));
	vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_FIELD, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_FIELD) | u4Temp);
	vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_MVSA, (u4AbsDramANc(prDecPrm->rMpeg4WorkBufSa.u4MvecSa) >> 4));

	//UTIL_Printf("[MPEG4 debug]: MVSA= 0x%x, 0x%x\n",prDecPrm->rMpeg4WorkBufSa.u4MvecSa,u4AbsDramANc(prDecPrm->rMpeg4WorkBufSa.u4MvecSa));
	//UTIL_Printf("[MPEG4 debug]: Reg= 0x%x\n",u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_MVSA));
	
	// direct mode, check co-located picture type.
	if (prDecPrm->ucVopCdTp == VCT_B && prDecPrm->rDep.rM4vDecPrm.prVop->ucBRefCdTp == VCT_I)
	{
		vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_COL_PIC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_COL_PIC) | 0x1);
	}
	else
	{
		vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_COL_PIC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_COL_PIC) & 0xFFFFFFFE);
	}

	  {
		  VDEC_INFO_MPEG_GMC_PRM_T *prGmcPrm = prDecPrm->rDep.rM4vDecPrm.prVop->prGmcPrm;
		  if(prDecPrm->rDep.rM4vDecPrm.prVol->ucQuarterSample)
		  {
			  u4Temp = (((prGmcPrm->i4GmcYMvX) << 16) & 0x3FFF0000) | ((prGmcPrm->i4GmcYMvY) & 0x3FFF);
		  }
		  else
		  {
			  u4Temp = (((prGmcPrm->i4GmcYMvX>>1) << 16) & 0x3FFF0000) | ((prGmcPrm->i4GmcYMvY>>1) & 0x3FFF);
		  }
		  //UTIL_Printf("[MPEG4]: GMCMVX = %d, GMCMVY = %d\n",prGmcPrm->i4GmcYMvX,prGmcPrm->i4GmcYMvY);
		  vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_GMC, u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_GMC) | u4Temp);
		  //UTIL_Printf("[MPEG4]: AVC_MV_183 = 0x%x\n",u4VDecReadAVCMV(u4VDecID,RW_AVC_MC_GMC));
	  }	
	
	u4Trb = prDecPrm->rDep.rM4vDecPrm.prVop->prDirMd->u4Trb;
	u4Trd = prDecPrm->rDep.rM4vDecPrm.prVop->prDirMd->u4Trd;
	u4Trbi = prDecPrm->rDep.rM4vDecPrm.prVop->prDirMd->u4Trbi;
	u4Trdi = prDecPrm->rDep.rM4vDecPrm.prVop->prDirMd->u4Trdi;
	
	vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TRFRAME, (u4Trb << 16) | u4Trd);
	
	if(u4Trd != 0)
	{
		vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_DIV, (((u4Trb << 14) / u4Trd) << 16) | (((u4Trd - u4Trb) << 14) / u4Trd));
	}
	
	if (prDecPrm->rDep.rM4vDecPrm.prVol->ucInterlaced)
	{
		vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TRFRAME_IL, (u4Trbi << 16) | u4Trdi);
		vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_P1, ((u4Trbi+1) << 16) | (u4Trdi+1));
		vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_M1, ((u4Trbi-1) << 16) | (u4Trdi-1));
	
		if(u4Trdi != 0)
		{
			vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_DIV, (((u4Trbi<< 14) / u4Trdi) << 16) | (((u4Trdi - u4Trbi) << 14) / u4Trdi));
		}
		vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_DIV_P1, ((((u4Trbi+1) << 14) / (u4Trdi+1)) << 16) | (((u4Trdi - u4Trbi) << 14) / (u4Trdi+1)));
		
		if(u4Trdi == 1)
		{
			vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_DIV_M1, 0);
		}
		else
		{
			vVDecWriteAVCMV(u4VDecID,RW_AVC_MC_TR_IL_DIV_M1, ((((u4Trbi-1) << 14) / (u4Trdi-1)) << 16) | (((u4Trdi - u4Trbi) << 14) / (u4Trdi-1)));
		}
	}
/*
	  vVDecWriteAVCMV(u4VDecID,183*4, 4);
	  vVDecWriteAVCMV(u4VDecID,185*4, 0);
	  vVDecWriteAVCMV(u4VDecID,186*4, 0x11);
	  vVDecWriteAVCMV(u4VDecID,187*4, 0xffffffff);
	  vVDecWriteAVCMV(u4VDecID,189*4, 0);
	  vVDecWriteAVCMV(u4VDecID,190*4, 0x40000000);
	  vVDecWriteAVCMV(u4VDecID,191*4, 0x40000000);
*/
}

#endif
#endif
