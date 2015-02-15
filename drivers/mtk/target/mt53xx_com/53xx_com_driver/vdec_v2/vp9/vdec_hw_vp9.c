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
#include "x_printf.h"
#include "../hw_common/vdec_hw_common.h"
#include "vdec_hw_vp9.h"
#include "vdec_info_vp9.h"

/* RISC Pattern Common Part Settings...*/
void vVP9RISCWrite_MC(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId )
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
    DRV_WriteReg( MC_BASE , u4Addr*4 , u4Value);
    VP9_PRINT_RISC("        RISCWrite_MC(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void vVP9RISCRead_MC(UINT32 u4Addr, UINT32* pu4Value , UINT32 u4CoreId)
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
        

    (*pu4Value) = DRV_ReadReg( MC_BASE, u4Addr*4  );
    VP9_PRINT_RISC("        RISCRead_MC(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void vVP9RISCWrite_MV(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 MV_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        MV_BASE = HEVC_MV_REG_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        MV_BASE = HEVC_MV_REG_OFFSET1;
    }
    else
    {
        MV_BASE = LAE_MV_OFFSET0;
    }

    DRV_WriteReg( MV_BASE , u4Addr*4 , u4Value);
    VP9_PRINT_RISC("        RISCWrite_MV(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void vVP9RISCRead_MV(UINT32 u4Addr, UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 MV_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        MV_BASE = HEVC_MV_REG_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        MV_BASE = HEVC_MV_REG_OFFSET1;
    }
    else
    {
        MV_BASE = LAE_MV_OFFSET0;
    }
    
    (*pu4Value) = DRV_ReadReg( MV_BASE, u4Addr*4  );
    VP9_PRINT_RISC("        RISCRead_MV( %u, %u); /* return 0x%08x */\n", u4Addr, u4CoreId, (*pu4Value));
}

void vVP9RISCWrite_PP(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 PP_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        PP_BASE = HEVC_PP_REG_OFFSET0;
    }
    else
    {
        PP_BASE = HEVC_PP_REG_OFFSET1;
    }
    
    DRV_WriteReg( PP_BASE , u4Addr*4 , u4Value);
    VP9_PRINT_RISC("        RISCWrite_PP(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void vVP9RISCRead_PP(UINT32 u4Addr, UINT32* pu4Value, UINT32 u4CoreId )
{
    UINT32 PP_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        PP_BASE = HEVC_PP_REG_OFFSET0;
    }
    else
    {
        PP_BASE = HEVC_PP_REG_OFFSET1;
    }
    
    (*pu4Value) = DRV_ReadReg( PP_BASE, u4Addr*4  );
    VP9_PRINT_RISC("        RISCRead_PP(%u, %u); // 0x%08x", u4Addr, u4CoreId, (*pu4Value));
}

void vVP9RISCWrite_VLD_TOP(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 VLD_TOP_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        VLD_TOP_BASE = VLD_TOP_REG_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        VLD_TOP_BASE = VLD_TOP_REG_OFFSET1;
    }
    else
    {
        VLD_TOP_BASE = LAE_VLDTOP_OFFSET0;
    }
    
    DRV_WriteReg( VLD_TOP_BASE , u4Addr*4 , u4Value);
    VP9_PRINT_RISC("        RISCWrite_VLD_TOP(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void vVP9RISCRead_VLD_TOP(UINT32 u4Addr, UINT32* pu4Value, UINT32 u4CoreId )
{
    UINT32 VLD_TOP_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        VLD_TOP_BASE = VLD_TOP_REG_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        VLD_TOP_BASE = VLD_TOP_REG_OFFSET1;
    }
    else
    {        
        VLD_TOP_BASE = LAE_VLDTOP_OFFSET0;
    }
    

    (*pu4Value) = DRV_ReadReg( VLD_TOP_BASE, u4Addr*4  );

    VP9_PRINT_RISC("        RISCRead_VLD_TOP(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void vVP9RISCWrite_VLD(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 VLD_BASE = VLD_REG_OFFSET0;

    if( u4CoreId == 0)
    {
        VLD_BASE = VLD_REG_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        VLD_BASE = VLD_REG_OFFSET1;
    }
    else
    {
        VLD_BASE = LAE_VLD_OFFSET0;
    }

    DRV_WriteReg( VLD_BASE , u4Addr*4 , u4Value);

    VP9_PRINT_RISC("        RISCWrite_VLD(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void vVP9RISCRead_VLD(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 VLD_BASE = 0;  

    if(u4CoreId == CORE_0_ID)
    {
        VLD_BASE = VLD_REG_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        VLD_BASE = VLD_REG_OFFSET1;
    }
    else
    {
        VLD_BASE = LAE_VLD_OFFSET0;
    }
    
    (*pu4Value) = DRV_ReadReg( VLD_BASE, u4Addr*4 );
    VP9_PRINT_RISC("        RISCRead_VLD(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void vVP9RISCRead_MISC(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 MISC_BASE = 0;  

    if(u4CoreId == CORE_0_ID)
    {
        MISC_BASE = HEVC_MISC_REG_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        MISC_BASE = HEVC_MISC_REG_OFFSET1;
    }
    else
    {        
        MISC_BASE = LAE_MISC_OFFSET0;
    }

    (*pu4Value) = DRV_ReadReg( MISC_BASE, u4Addr*4 );
    VP9_PRINT_RISC("        RISCRead_MISC(%u, %u); // 0x%08x\n", u4Addr, u4CoreId,(*pu4Value));
}

void vVP9RISCWrite_MISC(UINT32 u4Addr, UINT32 u4Value , UINT32 u4CoreId)
{
    UINT32 MISC_BASE = 0;  

    if(u4CoreId == CORE_0_ID)
    {
        MISC_BASE = HEVC_MISC_REG_OFFSET0;  
    }
    else if(u4CoreId == CORE_1_ID)
    {
        MISC_BASE = HEVC_MISC_REG_OFFSET1;  
    }    
    else
    {        
        MISC_BASE = LAE_MISC_OFFSET0;
    }

    DRV_WriteReg( MISC_BASE , u4Addr*4 , u4Value);
    VP9_PRINT_RISC("        RISCWrite_MISC(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void vVP9RISCRead_VDEC_TOP(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 MISC_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        MISC_BASE = 0xF6020000;
    }
    else // currently do not know core 1 settings
    {
        MISC_BASE = 0xF6020000;
    }
        
    (*pu4Value) = DRV_ReadReg( MISC_BASE, u4Addr*4 );
    VP9_PRINT_RISC("        RISCRead_VDEC_TOP(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void vVP9RISCWrite_VDEC_TOP( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 MISC_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        MISC_BASE = 0xF6020000;
    }
    else // currently do not know core 1 settings
    {
        MISC_BASE = 0xF6020000;
    }
        
    DRV_WriteReg( MISC_BASE , u4Addr*4 , u4Value);
    VP9_PRINT_RISC ("          RISCWrite_VDEC_TOP(%u , %-10u, %u); // 0x%08x\n",u4Addr, u4Value, u4CoreId, u4Value);
}

void vVP9RISCWrite_VP9_VLD(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 VLD_BASE = 0;

    if( u4CoreId == CORE_0_ID)
    {
        VLD_BASE = VP9_VLD_REG_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        VLD_BASE = VP9_VLD_REG_OFFSET1;
    }
    else
    {
        VLD_BASE = LAE_VP9_VLD_OFFSET0;
    }
    
    DRV_WriteReg( VLD_BASE , u4Addr*4 , u4Value);
    VP9_PRINT_RISC("        RISCWrite_VP9_VLD(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);
}

void vVP9RISCRead_VP9_VLD(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 VLD_BASE = 0;  

    if(u4CoreId == CORE_0_ID)
    {
        VLD_BASE = VP9_VLD_REG_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        VLD_BASE = VP9_VLD_REG_OFFSET1;
    }
    else
    {
        VLD_BASE = LAE_VP9_VLD_OFFSET0;
    }
    
    (*pu4Value) = DRV_ReadReg( VLD_BASE, u4Addr*4 );
    VP9_PRINT_RISC("        RISCRead_VP9_VLD(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void vVP9RISCRead_BS2(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 BS2_BASE = 0;  

    if(u4CoreId == CORE_0_ID)
    {
        BS2_BASE = VDEC_BS2_OFFSET0;
    }
    else if(u4CoreId == CORE_1_ID)
    {
        BS2_BASE = VDEC_BS2_OFFSET1;
    }
    else
    {
        BS2_BASE = LAE_BS2_OFFSET0;        
    }
 
    (*pu4Value) = DRV_ReadReg( BS2_BASE, u4Addr*4 );
    VP9_PRINT_RISC("        RISCRead_BS2(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

void vVP9RISCWrite_BS2(UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 BS2_BASE = 0;

    if( u4CoreId == CORE_0_ID)
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
    
    DRV_WriteReg( BS2_BASE , u4Addr*4 , u4Value);
    VP9_PRINT_RISC("        RISCWrite_BS2(%u, %-10u, %u); // 0x%08x\n", u4Addr, u4Value, u4CoreId, u4Value);    
}

void vVP9RISCWrite_MCore_TOP( UINT32 u4Addr, UINT32 u4Value, UINT32 u4CoreId)
{
    UINT32 MCORE_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        MCORE_BASE = MVDEC_TOP_OFFSET0;
    }
    else
    {
        MCORE_BASE = MVDEC_TOP_OFFSET0;
    }

    DRV_WriteReg( MCORE_BASE , u4Addr*4 , u4Value);
    VP9_PRINT_RISC("        RISCWrite_MCORE_TOP(%u, %-10u, %u); // 0x%08x\n",u4Addr, u4Value, u4CoreId, u4Value);
}

void vVP9RISCRead_MCore_TOP(UINT32 u4Addr , UINT32* pu4Value, UINT32 u4CoreId)
{
    UINT32 MCORE_BASE = 0;

    if(u4CoreId == CORE_0_ID)
    {
        MCORE_BASE = MVDEC_TOP_OFFSET0;
    }
    else
    {
        MCORE_BASE = MVDEC_TOP_OFFSET0;
    }
    
    (*pu4Value) = DRV_ReadReg( MCORE_BASE, u4Addr*4 );
    VP9_PRINT_RISC("        RISCRead_MCORE_TOP(%u, %u); // 0x%08x\n", u4Addr, u4CoreId, (*pu4Value));
}

