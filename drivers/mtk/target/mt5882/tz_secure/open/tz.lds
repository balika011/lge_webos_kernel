
#include "tzst.h"

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(INT_Reset)

SECTIONS
{
    __text_start = .;
    .text :
    {
        *(.tzinit.*)
        *(.tzmon.*)
        
        . = ALIGN(0x20);
        __tzvect_start = .;
        *(.tzvect.*)
        
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.data .data.*)
        *(.bss .bss.*)
        *(COMMON)
        *(.got.plt)
        *(.got)
    }
    
    .rel.dyn :
    {
        __rel_dyn_start = .;
        *(.rel.init)
        *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*)
        *(.rel.fini)
        *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*)
        *(.rel.data.rel.ro* .rel.gnu.linkonce.d.rel.ro.*)
        *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*)
        *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*)
        *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*)
        *(.rel.ctors)
        *(.rel.dtors)
        *(.rel.got)
        *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*)
        __rel_dyn_end = .;
    }
    
    . = ALIGN(0x20);
    .stack :
    {
        __stack_end = .;
        . += PA_STACK_LEN;
        __stack_start = .;
    }
    
    . = ALIGN(0x40);
    .vfifo :
    {
        __vfifo_start = .;
        . += PA_VFIFO_LEN;
        __vfifo_end = .;
    }
    
    . = ALIGN(0x20);
    __heap_start = .;

#ifdef CC_SVP_SUPPORT
    ASSERT((TZ_DRAM_SC_SIZE - (__heap_start - __text_start) > 0x120000), "project_x/target/mt53xx_com/tz_secure/open/tz.ldx:trustzone heap size is not enough")
#endif

    /DISCARD/ : { *(.eh_frame) *(.dynamic) *(.hash) *(.dynsym) *(.dynstr) *(.plt) *(.rel.plt) }
}
