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

/** @file vdec_ide.h
 *  Header of IDE test port interfaces
 */


#ifndef VDEC_IDE_H
#define VDEC_IDE_H

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------

#include "x_typedef.h"
#include "chip_ver.h"
#include "vdec_verify_general.h"


//-----------------------------------------------------------------------------
// Configurations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Constant definitions
//-----------------------------------------------------------------------------

// IO addresses
//
//5391
//#define IDETPIN             (IO_BASE + 0x1F800)
//#define IDETPOUT            (IO_BASE + 0x1F400)
//8530/8555
#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8580)
#define IDETPIN             (IO_BASE + 0x1A800)
#define IDETPOUT            (IO_BASE + 0x1A400)
#elif (CONFIG_CHIP_VER_CURR == CONFIG_CHIP_VER_MT8561 || CONFIG_CHIP_VER_CURR != CONFIG_CHIP_VER_MT5890) //added by xiaolei.li for 5881 emulation
#define IDETPIN             (IO_BASE + 0x3000) //for 5399 mustang
#define IDETPOUT            (IO_BASE + 0x3400)
#elif (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8560)
#define IDETPIN             (IO_BASE + 0x3E800)
#define IDETPOUT            (IO_BASE + 0x3E400)
#elif (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
#define IDETPIN             (IO_BASE + 0x35800)
#define IDETPOUT            (IO_BASE + 0x35400)
#else //#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)
#define IDETPIN             (IO_BASE + 0x31800)
#define IDETPOUT            (IO_BASE + 0x31400)
#endif //#if (CONFIG_CHIP_VER_CURR >= CONFIG_CHIP_VER_MT8555)

// Register maps
//
#define IIMTR               (IDETPIN)               // IDEW -> RISC
#define IIMRR               (IDETPIN + 4)           // RISC -> IDEW
#define IDSAR               (IDETPOUT)
#define IDLR                (IDETPOUT + 4)
#define IDCR                (IDETPOUT + 8)
#define IOMTR               (IDETPOUT + 0xc)        // IDER -> RISC
#define IOMTR2              (IDETPOUT + 0x18)       // IDER -> RISC
#define IOMRR               (IDETPOUT + 0x10)       // RISC->IDER
#define IOCTR               (IDETPOUT + 0x14)

// Register aliasing
//
#define IDE_DMA_SOURCE      (*((volatile UINT32*)(IDSAR)))
#define IDE_DMA_LEN         (*((volatile UINT32*)(IDLR)))
#define IDE_DMA_CONTROL     (*((volatile UINT8*)(IDCR)))
#define IDE_OUT_CTRL        (*((volatile UINT8*)(IOCTR)))
#define IDE_OUT_MSG1        (*((volatile UINT8*)(IOMTR)))
#define IDE_OUT_MSG2        (*((volatile UINT8*)(IOMTR2)))
#define IDE_IN_MSG          (*((volatile UINT8*)(IIMTR)))
#define IDE_RX_MSG          (*((volatile UINT8*)(IOMRR)))

// IDE buffer parameters
//
#define IDE_BUFFER_SIZE     0x10000         // 64KB


//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
// IDE command macros
//
#define IDE_IN_ENABLE       {IDE_OUT_CTRL &= 0xef;}
#define IDE_IN_DISABLE      {IDE_OUT_CTRL |= 0x10;}

#define IDE_OUT_ENABLE      {IDE_OUT_CTRL |= 1;}
#define IDE_OUT_DISABLE     {IDE_OUT_CTRL &= 0xfe;}

#define IDE_SELECT_IN       {IDE_OUT_CTRL |= 0x20;}
#define IDE_SELECT_OUT      {IDE_OUT_CTRL &= 0xdf;}
#define IDE_SELECT_ENABLE   {IDE_OUT_CTRL |= (1 << 6);}
#define IDE_SELECT_DISABLE  {IDE_OUT_CTRL &= ~(1 << 6);}

//-----------------------------------------------------------------------------
// Prototype  of inter-file functions
//-----------------------------------------------------------------------------

extern void _DmxIdeReset(void);

extern BOOL _DmxIdeGetFile(UINT32 u4Addr, UINT32 u4Type, UINT32 *u4FileLength, UINT32 u4TargSize);
extern BOOL _DmxIdeOpenFile(void);
extern BOOL _DmxIdeSendName(char *pcItem);
#ifdef SEMI_RING_FIFO
extern void _DmxIdeSeek(UINT32 u4SeekOffset,UINT8 u1SeekPoint);
extern void _DmxIdeSendReadLen(UINT32 u4ReadLength);
#endif
extern void _DmxIdeSendData(UINT32 u4Addr, UINT32 u4Mode, UINT32 u4Len);

#endif	// DMX_IDE_H

