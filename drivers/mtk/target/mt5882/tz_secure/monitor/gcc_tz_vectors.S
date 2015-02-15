
    .section	.tzvect.text

/*
; -----------------------------------------------------------------------------
; Monitor exception vector table
;      Most of these exceptions does not do anything other than branching to
;      itself except SMC vector. The SMC vector jumps to SMC handler which
;      takes care of proper switching from NWd to SWd and from SWd to NWd.
; -----------------------------------------------------------------------------
*/
TZ_VectorTable:
    b       .                   @ INT_Reset
    b       .                   @ INT_Undef_Inst
    b       software_interrupt  @ INT_Software
    b       .                   @ INT_Prefetch_Abort
    b       .                   @ INT_Data_Abort
    b       not_used            @ INT_Reserved
    b       irq                 @ INT_IRQ
    b       .                   @ INT_FIQ
    