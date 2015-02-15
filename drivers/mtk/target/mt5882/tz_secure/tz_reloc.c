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

#include <linux/elf.h>
#include "x_typedef.h"
#include "tzst.h"

//----------------------------------------------------------------------

#define R_ARM_RELATIVE	23

UINT32 PA_LOAD_BASE = 0;
UINT32 PA_VFIFO_START = 0, PA_VFIFO_END = 0;
UINT32 PA_HEAP_START = 0, PA_HEAP_END = 0;

EXTERN UINT32 __rel_dyn_start, __rel_dyn_end;
EXTERN UINT32 __vfifo_start, __vfifo_end;
EXTERN UINT32 __heap_start;


void Tz_Reloc(void)
{
    UINT32 base;
    Elf32_Rel *start, *end, *p;

    __asm__ __volatile__
    (
        "ldr	%0, =(. + 12)\n"
        "sub	%0, pc, %0"
        : "=r" (base)
    );

    start = (Elf32_Rel *)(base + (UINT32)&__rel_dyn_start);
    end = (Elf32_Rel *)(base + (UINT32)&__rel_dyn_end);

    for (p = start; p < end; p++)
    {
        switch (p->r_info)
        {
        case R_ARM_RELATIVE:
            *((unsigned int *)(base + p->r_offset)) += base;
            break;

        default:
            // unhandled relocation type, add to fix it.
            while (1);
        }
    }
    
    // Fix base address for PIC trustzone binary
    PA_LOAD_BASE = base - PA_CODE_BASE;
    
    PA_VFIFO_START = (UINT32)&__vfifo_start;
    PA_VFIFO_END = (UINT32)&__vfifo_end;
    
    PA_HEAP_START = (UINT32)&__heap_start;
    PA_HEAP_END = PA_LOAD_BASE + TZ_DRAM_SC_SIZE;
}

