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

#include "x_typedef.h"
#include "x_os.h"
#include "drv_config.h"
#include "vdec_verify_mpv_prov.h"
#include "RvdUtil.h"
#include "vdec_ide.h"
#include "vdec_type.h"
#include "x_printf.h"
#include "x_debug.h"
#include "vdec_verify_file_common.h"
#include "vdec_verify_filesetting.h"
#include "vdec_verify_common.h"
#include "risc_pattern.h"
#include "vdec_hw_common.h"

extern int debug_mode;
extern UINT32 u4GetVFIFOAddr(UINT32 u4InstID);

UCHAR *g2YGolden[VDEC_INST_MAX];
UCHAR *g2CGolden[VDEC_INST_MAX];
UCHAR *g2DPB[VDEC_INST_MAX];
UCHAR *g2VFIFO[VDEC_INST_MAX];
UCHAR *g2MVBuff[VDEC_INST_MAX];
CHAR *g2Temp[VDEC_INST_MAX];
CHAR *g2RiscBuffer[VDEC_INST_MAX];
CHAR *g2SegIdWrapper[VDEC_INST_MAX][2]; //for core0, 1
CHAR *g2LAEBuffer[VDEC_INST_MAX][2];    //for core0, 1
CHAR *g2ErrBuffer[VDEC_INST_MAX][2];    //for core0, 1
CHAR *g2TileBuffer[VDEC_INST_MAX];
CHAR *g2CountTblBuffer[VDEC_INST_MAX];

UCHAR *_pucVP9CRCYBuf0[VDEC_INST_MAX]; // multi-core 0
UCHAR *_pucVP9CRCCBuf0[VDEC_INST_MAX]; // multi-core 0
UCHAR *_pucVP9CRCYBuf1[VDEC_INST_MAX]; // multi-core 1
UCHAR *_pucVP9CRCCBuf1[VDEC_INST_MAX]; // multi-core 1
UCHAR *_pucVP9CRCYBuf2[VDEC_INST_MAX]; // single-core
UCHAR *_pucVP9CRCCBuf2[VDEC_INST_MAX]; // single-core


/* Memory Operations */
void VDEC_Pattern_MemoryAllocate(int inst_id,int vdec_type)
{
    switch (vdec_type)
    {
        case VDEC_H265:
            HEVC_Pattern_MemoryAllocate(inst_id);
            break;
        case VDEC_VP9:
            VP9_Pattern_MemoryAllocate(inst_id);
            break;
        default:
            UTIL_Printf("Please check codec type\n");
            break;
    }
}

void HEVC_Pattern_MemoryAllocate(int inst_id)
{
    UTIL_Printf("==========HEVC memory allocate ==============\n");
    //allocate VFIFO buffer
    g2VFIFO[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_VFIFO_SZ,1024, 1);
    UTIL_Printf("VFIFO buffer[%d] 0x%08x size 0x%x\n",inst_id,g2VFIFO[inst_id],PATTERN_VFIFO_SZ);
    //allocate DPB buffer
    g2DPB[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(DPB_SZ,4096, 1);
    UTIL_Printf("DPB buffer[%d] 0x%08x size 0x%x\n",inst_id,g2DPB[inst_id],DPB_SZ);
    //allocate Y golden buffer 
    g2YGolden[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_Y_GOLDEN_SZ,1024, 1);
    UTIL_Printf("g2YGolden buffer[%d] 0x%08x size 0x%x\n",inst_id,g2YGolden[inst_id],PATTERN_Y_GOLDEN_SZ);
    //allocate C golden buffer 
    g2CGolden[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_C_GOLDEN_SZ,1024, 1);
    UTIL_Printf("g2CGolden buffer[%d] 0x%08x size 0x%x\n",inst_id,g2CGolden[inst_id],PATTERN_C_GOLDEN_SZ);
    //allocate Temp buffer 
    g2Temp[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(PATTERN_Y_GOLDEN_SZ,1024, 1);
    UTIL_Printf("g2Temp buffer[%d] 0x%08x size 0x%x\n",inst_id,g2Temp[inst_id],PATTERN_Y_GOLDEN_SZ);
    //allocate Temp buffer 
    g2MVBuff[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(MV_SZ,1024, 1);
    UTIL_Printf("g2MVBuff buffer[%d] 0x%08x size 0x%x\n",inst_id,g2MVBuff[inst_id],MV_SZ);

    //allocate risc buffer
    g2RiscBuffer[inst_id] = (UCHAR *)x_alloc_aligned_verify_mem(RISC_BUFFER_SIZE,1024, 1);
    UTIL_Printf("g2RiscBuffer buffer[%d] 0x%08x size 0x%x\n",inst_id,g2RiscBuffer[inst_id],RISC_BUFFER_SIZE);     
}


void VP9_Pattern_MemoryAllocate(int u4InstID)
{  
    UINT32 u4Temp;
    UINT32 u4Align;
    VDEC_INFO_VP9_STATUS_INFO_T* prStatus;

    prStatus = _VP9_GetStatusInfo(u4InstID);
    
    UTIL_Printf("========== VP9 memory allocate ==============\n");

#ifdef CONFIG_DRV_MEM
  	u4Temp = u4GetVFIFOAddr(u4InstID);
    g2VFIFO[u4InstID] = (UCHAR *)(VIRTUAL(u4Temp));
   	UTIL_Printf("[VFIFO%d] Get reserved mem 0x%x\n",u4InstID,u4Temp);
    
    memset(g2VFIFO[u4InstID],0,PATTERN_VFIFO_SZ);
	UTIL_Printf("_pucVFifo[u4InstID] = 0x%x, pa = 0x%x\n", g2VFIFO[u4InstID], PHYSICAL(g2VFIFO[u4InstID]));

    #if VP9_CRC_ENABLE
    _pucVP9CRCYBuf0[u4InstID] =   g2VFIFO[u4InstID] + PATTERN_VFIFO_SZ;
    _pucVP9CRCCBuf0[u4InstID] =   _pucVP9CRCYBuf0[u4InstID] + VP9_CRC_BUFFER_SZ;  // 512k 
    _pucVP9CRCYBuf1[u4InstID] =   _pucVP9CRCCBuf0[u4InstID] + VP9_CRC_BUFFER_SZ;  // 512k
    _pucVP9CRCCBuf1[u4InstID] =   _pucVP9CRCYBuf1[u4InstID] + VP9_CRC_BUFFER_SZ;  // 512k
    
    _pucVP9CRCYBuf2[u4InstID] =   _pucVP9CRCCBuf1[u4InstID] + VP9_CRC_BUFFER_SZ;  // 512k
    _pucVP9CRCCBuf2[u4InstID] =   _pucVP9CRCYBuf2[u4InstID] + VP9_CRC_BUFFER_SZ;  // 512k 
    
    printk("_pucVP9CRCYBuf0[u4InstID] = 0x%x\n", _pucVP9CRCYBuf0[u4InstID]);
    printk("_pucVP9CRCCBuf0[u4InstID] = 0x%x\n", _pucVP9CRCCBuf0[u4InstID]);   
    printk("_pucVP9CRCYBuf1[u4InstID] = 0x%x\n", _pucVP9CRCYBuf1[u4InstID]);
    printk("_pucVP9CRCCBuf1[u4InstID] = 0x%x\n", _pucVP9CRCCBuf1[u4InstID]);
    printk("_pucVP9CRCYBuf2[u4InstID] = 0x%x\n", _pucVP9CRCYBuf2[u4InstID]);
    printk("_pucVP9CRCCBuf2[u4InstID] = 0x%x\n", _pucVP9CRCCBuf2[u4InstID]);
    
    //allocate Seg buffer, for core 0
    g2SegIdWrapper[u4InstID][0] = _pucVP9CRCCBuf2[u4InstID] + VP9_CRC_BUFFER_SZ;
    UTIL_Printf("g2SegIdWrapper[u4InstID][0] = 0x%x, size is 0x%x\n", g2SegIdWrapper[u4InstID][0], PATTERN_SEG_ID_SZ);    
    
    //allocate Seg buffer, for core 1
    g2SegIdWrapper[u4InstID][1] = g2SegIdWrapper[u4InstID][0] + PATTERN_SEG_ID_SZ;
    UTIL_Printf("g2SegIdWrapper[u4InstID][1] = 0x%x, size is 0x%x\n", g2SegIdWrapper[u4InstID][1], PATTERN_SEG_ID_SZ);
    
    #else
    //allocate Y golden buffer 
    g2YGolden[u4InstID] = g2VFIFO[u4InstID] + PATTERN_VFIFO_SZ;
    UTIL_Printf("g2YGolden buffer[%d] 0x%08x(0x%08x) size 0x%x\n",u4InstID,g2YGolden[u4InstID], PHYSICAL(g2YGolden[u4InstID]),PATTERN_Y_GOLDEN_SZ);
    
    //allocate C golden buffer 
    g2CGolden[u4InstID] = g2YGolden[u4InstID] + PATTERN_Y_GOLDEN_SZ;
    UTIL_Printf("g2CGolden buffer[%d] 0x%08x(0x%08x) size 0x%x\n",u4InstID,g2CGolden[u4InstID], PHYSICAL(g2CGolden[u4InstID]),PATTERN_C_GOLDEN_SZ );
    
    //allocate Seg buffer for core 0
    g2SegIdWrapper[u4InstID][0] = g2CGolden[u4InstID] + (PATTERN_C_GOLDEN_SZ);
    UTIL_Printf("g2SegIdWrapper[u4InstID][0] = 0x%x, size is 0x%x\n", g2SegIdWrapper[u4InstID][0], PATTERN_SEG_ID_SZ);

     //allocate Seg buffer for core 1
    g2SegIdWrapper[u4InstID][1] = g2SegIdWrapper[u4InstID][0] + (PATTERN_SEG_ID_SZ);
    UTIL_Printf("g2SegIdWrapper[u4InstID][1] = 0x%x, size is 0x%x\n", g2SegIdWrapper[u4InstID][1], PATTERN_SEG_ID_SZ);
    #endif   
     
    //allocate LAE buffer
//  if(prStatus->u4DualCore)    
    if(1)
    {
        g2LAEBuffer[u4InstID][0] =  g2SegIdWrapper[u4InstID][1] +  PATTERN_SEG_ID_SZ;
        UTIL_Printf("g2LAEBuffer[u4InstID][0] = 0x%08x(0x%08x), size is 0x%x\n", g2LAEBuffer[u4InstID][0],
                    PHYSICAL(g2LAEBuffer[u4InstID][0]), VP9_LAE_BUFFER_SZ);
            
        g2ErrBuffer[u4InstID][0] =  g2LAEBuffer[u4InstID][0] +  VP9_LAE_BUFFER_SZ;
        UTIL_Printf("g2ErrBuffer[u4InstID][0] = 0x%08x(0x%08x), size is 0x%x\n", g2ErrBuffer[u4InstID][0],
                    PHYSICAL(g2ErrBuffer[u4InstID][0]), VP9_ERR_BUFFER_SZ);
        
        //allocate DPB buffer
        g2DPB[u4InstID] = g2ErrBuffer[u4InstID][0] + VP9_ERR_BUFFER_SZ;
        u4Align = (((UINT32)g2DPB[u4InstID] + 4095) >> 12 ) << 12;
        g2DPB[u4InstID] = (UCHAR *)u4Align;
        UTIL_Printf("VP9 buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2DPB[u4InstID],PATTERN_VP9_FBM_SZ);
        
    }
    #if 0
    else
    {
        //allocate DPB buffer
        g2DPB[u4InstID] = g2SegIdWrapper[u4InstID][1] + PATTERN_SEG_ID_SZ;
        UTIL_Printf("VP9 buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2DPB[u4InstID],PATTERN_VP9_FBM_SZ);                  
    }
    #endif
    
    //allocate risc buffer
    //[vpx], move to vp9 working buffer allocation
    #if 0
    g2RiscBuffer[u4InstID] = (UCHAR *)x_alloc_aligned_verify_mem(VP9_RISC_BUFFER_SIZE,1024, 1);
    UTIL_Printf("g2RiscBuffer buffer[%d] 0x%08x size 0x%x\n",u4InstID,g2RiscBuffer[u4InstID],VP9_RISC_BUFFER_SIZE);
    #endif
#else    
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
}

/* RISC Pattern Common Part Settings...*/
void RISCWrite_MC(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId )
{
    UINT32 MC_BASE = 0;

    if(u4CoreId == 0)
    {
        MC_BASE = MC_REG_OFFSET0;
    }
    else
    {
        MC_BASE = MC_REG_OFFSET1;
    }
    
    #if SIM_LOG
    #else
    DRV_WriteReg( MC_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCWrite_MC(%u, %-10u, %u);  // 0x%08X\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void RISCRead_MC(UINT32 u4Addr, UINT32* pu4Value , UINT32 u4CoreId)
{
    UINT32 MC_BASE = 0;
    
    if(u4CoreId == 0)
    {
        MC_BASE = MC_REG_OFFSET0;
    }
    else
    {
        MC_BASE = MC_REG_OFFSET1;
    }
        
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( MC_BASE, u4Addr*4  );
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCRead_MC(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void RISCWrite_MV(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 MV_BASE = 0;

    if(u4CoreId == 0)
    {
        MV_BASE = HEVC_MV_REG_OFFSET0;
    }
    else if(u4CoreId == 1)
    {
        MV_BASE = HEVC_MV_REG_OFFSET1;
    }
    else
    {
        MV_BASE = LAE_MV_OFFSET0;
    }

    #if SIM_LOG
    #else
    DRV_WriteReg( MV_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCWrite_MV(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void RISCWrite_PP(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 PP_BASE = 0;

    if(u4CoreId == 0)
    {
        PP_BASE = HEVC_PP_REG_OFFSET0;
    }
    else
    {
        PP_BASE = HEVC_PP_REG_OFFSET1;
    }
    
    #if SIM_LOG
    #else
    DRV_WriteReg( PP_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCWrite_PP(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void RISCRead_PP(UINT32 u4Addr, UINT32* pu4Value, UINT32 u4CoreId )
{
    UINT32 PP_BASE = 0;

    if(u4CoreId == 0)
    {
        PP_BASE = HEVC_PP_REG_OFFSET0;
    }
    else
    {
        PP_BASE = HEVC_PP_REG_OFFSET1;
    }
    
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( PP_BASE, u4Addr*4  );
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCRead_PP(%u, %u); // 0x%08x", u4Addr, u4CoreId, (*pu4Value));
}

void RISCWrite_VLD_TOP(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 VLD_TOP_BASE = 0;

    if(u4CoreId == 0)
    {
        VLD_TOP_BASE = VLD_TOP_REG_OFFSET0;
    }
    else if(u4CoreId == 1)
    {
        VLD_TOP_BASE = VLD_TOP_REG_OFFSET1;
    }
    else
    {
        VLD_TOP_BASE = LAE_VLDTOP_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    DRV_WriteReg( VLD_TOP_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCWrite_VLD_TOP(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void RISCRead_VLD_TOP(UINT32 u4Addr, UINT32* pu4Value, UINT32 u4CoreId )
{
    UINT32 VLD_TOP_BASE = 0;

    if(u4CoreId == 0)
    {
        VLD_TOP_BASE = VLD_TOP_REG_OFFSET0;
    }
    else if(u4CoreId == 1)
    {
        VLD_TOP_BASE = VLD_TOP_REG_OFFSET1;
    }
    else
    {        
        VLD_TOP_BASE = LAE_VLDTOP_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( VLD_TOP_BASE, u4Addr*4  );
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCRead_VLD_TOP(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void RISCWrite_VLD(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 VLD_BASE = VLD_REG_OFFSET0;

    if( u4CoreId == 0)
    {
        VLD_BASE = VLD_REG_OFFSET0;
    }
    else if( u4CoreId == 1)
    {
        VLD_BASE = VLD_REG_OFFSET1;
    }
    else
    {
        VLD_BASE = LAE_VLD_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    DRV_WriteReg( VLD_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCWrite_VLD(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void RISCRead_VLD(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 VLD_BASE = 0;  

    if(u4CoreId == 0)
    {
        VLD_BASE = VLD_REG_OFFSET0;
    }
    else if(u4CoreId == 1)
    {
        VLD_BASE = VLD_REG_OFFSET1;
    }
    else
    {
        VLD_BASE = LAE_VLD_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( VLD_BASE, u4Addr*4 );
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCRead_VLD(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void RISCRead_MISC(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 MISC_BASE = 0;  

    if(u4CoreId == 0)
    {
        MISC_BASE = HEVC_MISC_REG_OFFSET0;
    }
    else if(u4CoreId == 1)
    {
        MISC_BASE = HEVC_MISC_REG_OFFSET1;
    }
    else
    {        
        MISC_BASE = LAE_MISC_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( MISC_BASE, u4Addr*4);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCRead_MISC(%u, %u); // 0x%08x\n", u4Addr, u4CoreId,(*pu4Value));
}

void RISCWrite_MISC(UINT32 u4Addr, UINT32 u4Value , UINT32 u4CoreId)
{
    UINT32 MISC_BASE = 0;  

    if(u4CoreId == 0)
    {
        MISC_BASE = HEVC_MISC_REG_OFFSET0;  
    }
    else if(u4CoreId == 1)
    {
        MISC_BASE = HEVC_MISC_REG_OFFSET1;  
    }
    else
    {
        MISC_BASE = LAE_MISC_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    DRV_WriteReg( MISC_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCWrite_MISC(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void RISCRead_VDEC_TOP(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 MISC_BASE = 0;

    if(u4CoreId == 0)
    {
        MISC_BASE = 0xF6020000;
    }
    else // currently do not know core 1 settings
    {
        MISC_BASE = 0xF6020000;
    }
        
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( MISC_BASE, u4Addr*4 );
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCRead_VDEC_TOP(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void RISCWrite_VDEC_TOP( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 MISC_BASE = 0;

    if(u4CoreId == 0)
    {
        MISC_BASE = 0xF6020000;
    }
    else if(u4CoreId == 1)
    {
        MISC_BASE = 0xF6020000;
    }
        
    #if SIM_LOG
    #else
    DRV_WriteReg( MISC_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk ("          RISCWrite_VDEC_TOP(%u , %-10u, %u); // 0x%08x\n",u4Addr, u4Value, u4CoreId, u4Value);
}

void RISCWrite_VP9_VLD(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 VLD_BASE = 0;

    if( u4CoreId == 0)
    {
        VLD_BASE = VP9_VLD_REG_OFFSET0;
    }
    else if(u4CoreId == 1)
    {
        VLD_BASE = VP9_VLD_REG_OFFSET1;
    }
    else
    {
        VLD_BASE = LAE_VP9_VLD_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    DRV_WriteReg( VLD_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCWrite_VP9_VLD(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void RISCRead_VP9_VLD(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 VLD_BASE = 0;  

    if(u4CoreId == 0)
    {
        VLD_BASE = VP9_VLD_REG_OFFSET0;
    }
    else if(u4CoreId == 1)
    {
        VLD_BASE = VP9_VLD_REG_OFFSET1;
    }
    else
    {
        VLD_BASE = LAE_VP9_VLD_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( VLD_BASE, u4Addr*4 );
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCRead_VP9_VLD(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void RISCRead_BS2(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 BS2_BASE = 0;  

    if(u4CoreId == 0)
    {
        BS2_BASE = VDEC_BS2_OFFSET0;
    }
    else if( u4CoreId == 1)
    {
        BS2_BASE = VDEC_BS2_OFFSET1;
    }
    else
    {
        BS2_BASE = LAE_BS2_OFFSET0;        
    }
    
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( BS2_BASE, u4Addr*4 );
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCRead_BS2(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void RISCWrite_BS2(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 BS2_BASE = 0;

    if( u4CoreId == 0)
    {
        BS2_BASE = VDEC_BS2_OFFSET0;
    }
    else if( u4CoreId == 1)
    {
        BS2_BASE = VDEC_BS2_OFFSET1;
    }
    else
    {
        BS2_BASE = LAE_BS2_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    DRV_WriteReg( BS2_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCWrite_BS2(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);    
}

void RISCWrite_MCore_TOP( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 MCORE_BASE = 0;

    if(u4CoreId == 0)
    {
        MCORE_BASE = MVDEC_TOP_OFFSET0;
    }
    else
    {
        MCORE_BASE = MVDEC_TOP_OFFSET0;
    }
        
    #if SIM_LOG
    #else
    DRV_WriteReg( MCORE_BASE , u4Addr*4 , u4Value);
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk ("          RISCWrite_MCORE_TOP(%u, %-10u, %u); // 0x%08x\n",u4Addr, u4Value, u4CoreId, u4Value);
}

void RISCRead_MCore_TOP(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 MCORE_BASE = 0;

    if(u4CoreId == 0)
    {
        MCORE_BASE = MVDEC_TOP_OFFSET0;
    }
    else
    {
        MCORE_BASE = MVDEC_TOP_OFFSET0;
    }
    
    #if SIM_LOG
    #else
    (*pu4Value) = DRV_ReadReg( MCORE_BASE, u4Addr*4 );
    #endif
    if (debug_mode > 0 || SIM_LOG)
        printk("        RISCRead_MCORE_TOP(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

