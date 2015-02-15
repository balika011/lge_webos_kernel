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




// 2013/03/28 Chia-Mao Hung For hevc early emulation

#ifndef _VDEC_RISC_PATTERN_
#define _VDEC_RISC_PATTERN_

#define SIM_LOG 0  //use for simulate on mt5399

// VP9 Define Settings here
#define VP9_RING_VFIFO_SUPPORT 1
#define VP9_PRE_REGISTER_DUMP 0
#define VP9_CRC_ENABLE 1
#define VP9_HD 1    // used for 4k 
// ~

// HAL
#define DRV_WriteReg(IO_BASE, dAddr, dVal)  *(volatile UINT32 *)(IO_BASE + 0xF0000000 + dAddr) = dVal
#define DRV_ReadReg(IO_BASE, dAddr)        *(volatile UINT32 *)(IO_BASE + 0xF0000000 + dAddr)

//HEVC HAL
#define HEVC_DRV_WriteReg(IO_BASE, dAddr, dVal)  *(volatile UINT32 *)(IO_BASE + 0xF0000000 + dAddr) = dVal
#define HEVC_DRV_ReadReg(IO_BASE, dAddr)        *(volatile UINT32 *)(IO_BASE + 0xF0000000 + dAddr)

///////////
#define MAX_DPB_NUM  17
#define MAX_VP9_BUF (8 + 0)
#define MAX_VP9_MV_BUF 2 // for core 0, core 1
//#define MAX_VP9_TILE_BUF ((4096 >> 6) * 4) // max # of col tiles * max # of row tiles 

//#define EC_SETTINGS
#define VDEC_INST_MAX   2
#define VP9_PERFORMANCE_LOG 1
#define LAE_ID 8

#define PATTERN_Y_GOLDEN_SZ 0x1FE000//(1920*1088)
#define PATTERN_C_GOLDEN_SZ (PATTERN_Y_GOLDEN_SZ >> 1)//(1920*1088/2)
#define PATTERN_VFIFO_SZ    0xA00000//(0xA00000) //10MB size
#define PATTERN_SEG_ID_SZ   (0x12000)  //48KB, (actual calculation is ((4096*2304) >> 12)) << 5
                                      //here for alignment
#define PATTERN_VP9_LAE_SZ  (0x2000)  //128KB Actually 76K for HD is fine
#define PATTERN_VP9_ERR_SZ  (0x0)     //reserved for future usage..
#define DPB_SZ              (0x100000) //16MB size
#if VP9_HD
#define PATTERN_VP9_FBM_SZ  (0x1800000) //24 MB (HD 1920x1088 * 1.5 * 8)
#else // 4k
#define PATTERN_VP9_FBM_SZ  (0x6200000) //98 MB (4k2k 4096x2160 * 1.5 * 8)
#endif
#define MV_SZ               (0x7f000) //4k2k need re-config
#define BITSTREAM_BUFF_SIZE PATTERN_VFIFO_SZ
#define RISC_BUFFER_SIZE    16384
#define VP9_RISC_BUFFER_SIZE 0x40000  //256k
#define VP9_TILE_BUFFER_SIZE 0x10000  //64k, actually (64 x 16 x 4) is enough
#define VP9_CRC_BUFFER_SZ    0x80000   // 512k
#define VP9_COUNT_TBL_SZ     0x10000   //(256*16*4), actuall 4k is ok
#define VP9_LAE_BUFFER_SZ    ((64 * 4 * 16)* 8 * 64)
#define VP9_ERR_BUFFER_SZ    (288 << 4)

typedef struct _VDEC_INFO_VP9_FB_INFO_T_
{
    UINT32 u4UFO_LEN_SIZE_Y;
    UINT32 u4UFO_LEN_SIZE_C;
    UINT32 u4DramPicY_Y_LENSize;
    UINT32 u4DramPicC_C_LENSize;
    UINT32 PIC_SIZE_Y;
    UINT32 PIC_SIZE_C;
    
    UINT32 VP9_TILE_Addr;
    UINT32 VP9_COUNT_TBL_Addr;
    UINT32 VP9_FBM_YAddr[MAX_VP9_BUF];
    UINT32 VP9_FBM_CAddr[MAX_VP9_BUF];
    UINT32 VP9_MV_Addr[MAX_VP9_MV_BUF];
    UINT32 VP9_FBM_YLEN_Addr[MAX_VP9_BUF];
    UINT32 VP9_FBM_CLEN_Addr[MAX_VP9_BUF];
    
    UINT32 u4Width;
    UINT32 u4Height;
    UINT32 u4CurrentFBId;
}VDEC_INFO_VP9_FB_INFO_T;

typedef struct _VDEC_INFO_VP9_STATUS_INFO_T_
{    
    BOOL   fgVP9CRCOpen;
    UINT32 u4DecodeTimeOut;
    UINT32 u4UFOMode;
    UINT32 u4DualCore;
    UINT32 u4VP9FileScanned;
    UINT32 u4BistreamResult;

}VDEC_INFO_VP9_STATUS_INFO_T;

#if 1
extern UCHAR *g2YGolden[VDEC_INST_MAX];
extern UCHAR *g2CGolden[VDEC_INST_MAX];
extern UCHAR *g2DPB[VDEC_INST_MAX];
extern UCHAR *g2VFIFO[VDEC_INST_MAX];
extern UCHAR *g2MVBuff[VDEC_INST_MAX];
extern CHAR *g2Temp[VDEC_INST_MAX];
extern CHAR *g2RiscBuffer[VDEC_INST_MAX];      //pattern tmp buffer
extern CHAR *g2SegIdWrapper[VDEC_INST_MAX][2];    //vp9 only
extern CHAR *g2LAEBuffer[VDEC_INST_MAX][2];     //lae used buffer
extern CHAR *g2ErrBuffer[VDEC_INST_MAX][2];    //for core0, 1
extern CHAR *g2TileBuffer[VDEC_INST_MAX];
extern CHAR *g2CountTblBuffer[VDEC_INST_MAX];

extern UCHAR *_pucVP9CRCYBuf0[VDEC_INST_MAX]; // multi-core 0
extern UCHAR *_pucVP9CRCCBuf0[VDEC_INST_MAX]; // multi-core 0
extern UCHAR *_pucVP9CRCYBuf1[VDEC_INST_MAX]; // multi-core 1
extern UCHAR *_pucVP9CRCCBuf1[VDEC_INST_MAX]; // multi-core 1
extern UCHAR *_pucVP9CRCYBuf2[VDEC_INST_MAX]; // single-core
extern UCHAR *_pucVP9CRCCBuf2[VDEC_INST_MAX]; // single-core

#endif

void RISCWrite_MC( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCRead_MC( UINT32 u4Addr, UINT32* pu4Value, UINT32 u4CoreId);
void RISCWrite_MV( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCWrite_PP( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCRead_PP( UINT32 u4Addr, UINT32* pu4Value, UINT32 u4CoreId);
void RISCWrite_VLD_TOP( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCRead_VLD_TOP( UINT32 u4Addr, UINT32* pu4Value, UINT32 u4CoreId);
void RISCRead_MISC ( UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId);
void RISCWrite_MISC( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCWrite_VLD( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCRead_VLD ( UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId);
void RISCRead_VDEC_TOP ( UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId);
void RISCWrite_VDEC_TOP( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCRead_VP9_VLD(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId);
void RISCWrite_VP9_VLD(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCRead_BS2(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId);
void RISCWrite_BS2(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCWrite_MCore_TOP( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId);
void RISCRead_MCore_TOP(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId);

void RISCRead_GCON ( UINT32 u4Addr , UINT32* pu4Value);
void RISCWrite_GCON( UINT32 u4Addr, UINT32 u4Value );
int Dump_reg( UINT32 base_r, UINT32 start_r, UINT32 end_r , char* pBitstream_name , UINT32 frame_number, BOOL bDecodeDone );
int Dump_mem( unsigned char* buf, unsigned int size , int  frame_num , unsigned int type , bool isTimeout);
void Dump_ESA_NBM_performane_log( char* pBName );
void RISC_instructions();
void hevc_test( int inst_id,int frame_start , int frame_end );
void vp9_test(int u4InstID, bool fgDualCore, bool fgUFOMode, int frame_start , int frame_end );
void VDEC_Pattern_MemoryAllocate(int inst_id,int vdec_type);
void HEVC_Pattern_MemoryAllocate(int inst_id);
void VP9_Pattern_MemoryAllocate(int inst_id); 
void HEVC_DPB_Partition(int inst_id);
void VP9_FB_Config(UINT32 u4InstId);
int VP9_RegDump(UINT32 u4Base, UINT32 u4Start, UINT32 u4End , UINT32 frame_number, BOOL bDecodeDone);
int VP9DumpMem( UINT32 u4InstID, unsigned char* buf, unsigned int size , int  frame_num , unsigned int type , bool isTimeout);
int VP9DumpTileMem( UINT32 u4InstID, unsigned char* buf, unsigned int size , int  frame_num);
void VP9_UFO_CONFIG(UINT32 u4InstID, UINT32 u4CoreId);
VDEC_INFO_VP9_FB_INFO_T* _VP9_GetFBInfo(UINT32 u4InstID);
VDEC_INFO_VP9_STATUS_INFO_T* _VP9_GetStatusInfo(UINT32 u4InstID);

#endif
