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

#ifndef __VDEC_DEBUG_H__
#define __VDEC_DEBUG_H__

#define DEFINE_IS_LOG	MPV_IsLog
#include "x_debug.h"

#ifdef CONFIG_TV_DRV_VFY

#include "x_drv_map.h"

#define UTIL_Printf printf
#define CKGEN_AgtSelClk
#define CKGEN_AgtGetClk

#define VECTOR_VDFUL          24        //VDFUL
#define VECTOR_VDLIT          89        //VDLIT
#define VECTOR_LAE0           0x55
#define VECTOR_LAE1           0x56

#define CONFIG_DRV_FPGA_BOARD 0

#define CLK_CFG1_MC_SEL_SYSPLL2_1_2               (5)
#define CLK_CFG1_VDEC_SEL_SYSPLL1_1_2             (1)
#define CLK_CFG1_VDEC_SEL_27M                     (0)
#define REG_RW_CLK_CFG8                     0x009C
#define CLK_CFG1_PNG_SEL_SYSPLL2_1_2              (5)

#define BSP_InvDCacheRange

#define fgRunBelowSegmentAfter(X) (X)
#define IC_8580_VER_A 0
#define IC_8580_VER_B 1

typedef unsigned long     DWRD;

typedef enum
{
    e_CLK_RISC,             //0   0x70.3
    e_CLK_ADSP,             //1   0x70.7
    e_CLK_ADSP2,            //2   0x70.11
    e_CLK_NFLASH,           //3   0x70.19
    e_CLK_SACD,             //4   0x70.23
    e_CLK_SLOW_RISC,
    e_CLK_VFD,              //5   0x70.31
    e_CLK_VDEC,             //6   0x74.3
    e_CLK_MC,               //7   0x74.7
    e_CLK_OSD,              //8   0x74.11
    e_CLK_PNG,              //9   0x74.15
    e_CLK_RESZ,             //10  0x74.19
    e_CLK_GRAPH,            //11  0x74.23
    e_CLK_NR,               //12  0x74.27
    e_CLK_DEMUX,            //13  0x74.31
    e_CLK_IR_DIV,           //14  0x78.4
    e_CLK_SD,               //15  0x78.12
    e_CLK_SD_HCLK,          //16  0x78.15
    e_CLK_ABIST2 ,          //17  0x78.19
    e_CLK_ABIST,            //18  0x78.31
    e_CLK_SVO_STDBY,        //19  0x7C.3
    e_CLK_GCPU,             //20  0x7C.7
    e_CLK_MVDO2,            //21  0x7C.11
    e_CLK_JPEG,             //22  0x7C.15
    e_CLK_SYS_SUB,          //23  0x7C.19
    e_CLK_SIF,              //24  0x7C.23
    e_CLK_VENC_2F,          //25  0x7C.27
    e_CLK_BCLK,             //26  0x7C.31
    e_CLK_MAX               //27
} e_CLK_T;

#define PRIORITY_CLASS_REALTIME         50    ///< for hard real-time
#define PRIORITY_CLASS_HIGH             100   ///< for streaming
#define PRIORITY_CLASS_NORMAL           150   ///< for normal application
#define PRIORITY_CLASS_IDLE             200   ///< for background

#define PRIORITY_LAYER_TIME_CRITICAL    0
#define PRIORITY_LAYER_DRIVER           10
#define PRIORITY_LAYER_MIDDLEWARE       20
#define PRIORITY_LAYER_UI               30

#define PRIORITY(CLASS, LAYER, OFFSET)  ((CLASS) + (LAYER) + (OFFSET))

#define MPV_THREAD_NAME			"MPV"
#define MPV_STACK_SIZE			16384
#define MPV_THREAD_PRIORITY		PRIORITY(PRIORITY_CLASS_HIGH, PRIORITY_LAYER_TIME_CRITICAL, 0)

#define MPV_VPRS_THREAD_NAME		"VPrs"
#define MPV_VPRS_STACK_SIZE			2048
#define MPV_VPRS_THREAD_PRIORITY	PRIORITY(PRIORITY_CLASS_HIGH, PRIORITY_LAYER_TIME_CRITICAL, 0)

//xiaolei.li's ==>for 8561 ckgen
#define CLK_CFG1_VDEC_SEL_HA_DDS			(7)
#define REG_RW_PLL_CFG0                     0x0038          //PLL Group Configuration 0
#define CLK_CLK_VDEC_SEL_27M				(0)
#define CLK_CLK_PNG_SEL_SYSPLL2_1_2 		(5)
#define MPEG4_NEW_MODE

//for 5881 remy emu
//#define MT5881_WMV_NO_WRAPPER //for 5881 wmv emulation
#define VPMODE_FOR_5881 //vp mode has a change on chip 5881 xiaolei.li for 5881 emulation
#define DEC_END_POLLING //decode end through polling
#define CHIP_MT5881 //for 5881 emulation xiaolei.li

//for 5399 mustang emu
#define CHIP_MT5399 //for new code added xiaolei.li

#endif // CONFIG_TV_DRV_VFY

#endif /* __VDEC_DEBUG_H__ */

