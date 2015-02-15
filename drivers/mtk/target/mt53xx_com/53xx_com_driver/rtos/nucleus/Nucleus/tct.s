;************************************************************************
;*
;*               Copyright Mentor Graphics Corporation 2002
;*                         All Rights Reserved.
;*
;* THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION WHICH IS
;* THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS LICENSORS AND IS
;* SUBJECT TO LICENSE TERMS.
;*
;************************************************************************
;************************************************************************
;*
;* FILE NAME                                  VERSION
;*
;*      tct.s                         Nucleus PLUS\Integrator\ADS 1.14.8
;*
;* COMPONENT
;*
;*      TC - Thread Control
;*
;* DESCRIPTION
;*
;*      This file contains the target processor dependent routines for
;*      performing target-dependent scheduling functions.
;*
;* FUNCTIONS
;*
;*      TCT_Control_Interrupts              Enable / disable interrupts
;*                                          by changing
;*                                          TCD_Interrupt_Level
;*      TCT_Local_Control_Interrupts        Enable/disable interrupts
;*                                          by not changing
;*                                          TCD_Interrupt_Level
;*      TCT_Restore_Interrupts              Restores interrupts to the
;*                                          level in TCD_Interrupt_Level
;*      TCT_Build_Task_Stack                Build initial task stack
;*      TCT_Build_HISR_Stack                Build initial HISR stack
;*      TCT_Build_Signal_Frame              Build signal handler frame
;*      TCT_Check_Stack                     Check current stack
;*      TCT_Schedule                        Schedule the next thread
;*      TCT_Control_To_Thread               Transfer control to a thread
;*      TCT_Control_To_System               Transfer control from thread
;*      TCT_Signal_Exit                     Exit from signal handler
;*      TCT_Current_Thread                  Returns a pointer to current
;*                                            thread
;*      TCT_Set_Execute_Task                Sets TCD_Execute_Task under
;*                                            protection from interrupts
;*      TCT_Protect                         Protect critical section
;*      TCT_Unprotect                       Unprotect critical section
;*      TCT_Unprotect_Specific              Release specific protection
;*      TCT_Set_Current_Protect             Set the thread's current
;*                                            protection field
;*      TCT_Protect_Switch                  Switch to protected thread
;*      TCT_Schedule_Protected              Schedule the protected thread
;*      TCT_Interrupt_Context_Save          Save interrupted context
;*      TCT_Interrupt_Context_Restore       Restore interrupted context
;*      TCT_Activate_HISR                   Activate a HISR
;*      TCT_HISR_Shell                      HISR execution shell
;*
;************************************************************************

;**************************************
;*   INCLUDE FILE                     *
;**************************************

    INCLUDE asm_defs.inc             ; Assembly definition file

;**************************************
;*   COMPILE-TIME SWITCHES            *
;**************************************

; NOTE: Define "NU_SUPERV_USER_MODE" to include hooks for
;       mode switching.  To do this use the -predefine option
;       on the assembler line:
;           -predefine "NU_SUPERV_USER_MODE {TRUE}"

    IF  (:LNOT: :DEF: NU_SUPERV_USER_MODE)
        GBLL       NU_SUPERV_USER_MODE
NU_SUPERV_USER_MODE   SETL    {FALSE}
    ENDIF

; NOTE: Define "NU_MMU_MODE" to include hooks for MMU
;       support.  To do this use the -predefine option
;       on the assembler line:
;           -predefine "NU_MMU_MODE {TRUE}"

    IF  (:LNOT: :DEF: NU_MMU_MODE)
        GBLL       NU_MMU_MODE
NU_MMU_MODE   SETL    {FALSE}
    ENDIF

; NOTE: Define "INCLUDE_PROVIEW" to include hooks for Nucleus ProView
;       support.  To do this use the -pd option on the assembler line:
;           -pd "INCLUDE_PROVIEW SETL {TRUE}"

    IF  (:LNOT: :DEF: INCLUDE_PROVIEW)
        GBLL       INCLUDE_PROVIEW
INCLUDE_PROVIEW   SETL    {FALSE}
    ENDIF

; NOTE: Define "NUCLEUS_THREAD_PROFILING" to include hooks for Thread
;       profiling support. To do this use the -pd option on the assembler
;       line:
;           -pd "NUCLEUS_THREAD_PROFILING {TRUE}"

    IF  (:LNOT: :DEF: NUCLEUS_THREAD_PROFILING)
        GBLL       NUCLEUS_THREAD_PROFILING
NUCLEUS_THREAD_PROFILING   SETL    {FALSE}
    ENDIF



;******************************************************
;*   INCLUDE files dependent on compile time switches *
;******************************************************

    IF NU_MMU_MODE
        INCLUDE ../MMU/inc/mm_extr.s        ; MMU macros
    ENDIF

        PRESERVE8

        AREA |C$$data|,DATA
|x$dataseg|

;*********************************************
;*   EXTERNAL VARIABLE/FUNCTION DECLARATIONS *
;*********************************************

; Define a variable for saving the system stack limit in.  This value is
; intialized in ASM_DEFS.INC and is in effect whenever the system stack is in
; use.

    EXPORT  TCT_System_Limit
TCT_System_Limit
    DCD     0x00000000

; Define a variable for saving a flag which indicates whether their is
; a special nested case in our interrupt routine.  A special nested
; interrupt is a nested interrupt that go off while the core is still
; in IRQ mode.

TCT_Special_Nested
    DCD     0x00000000

; If assembled with TASM the variable {CONFIG} will be set to 16
; If assembled with ARMASM the variable {CONFIG} will be set to 32
; Set the variable THUMB to TRUE or false depending on whether the
; file is being assembled with TASM or ARMASM.

        GBLL    THUMB
        GBLL    ARM
    [ {CONFIG} = 16
THUMB   SETL    {TRUE}
ARM     SETL    {FALSE}
; If assembling with TASM go into 32 bit mode as the Armulator will
; start up the program in ARM state.
        CODE32
    |
THUMB   SETL    {FALSE}
ARM     SETL    {TRUE}
    ]

        AREA |C$$code|, CODE, READONLY
|x$codeseg|

    ; Define external inner-component global data references.

    IMPORT  TCD_Execute_Task
    IMPORT  TCD_Execute_HISR
    IMPORT  TCD_Current_Thread
    IMPORT  TCD_System_Stack
    IMPORT  TCD_Interrupt_Count
    IMPORT  TCD_Active_HISR_Heads
    IMPORT  TCD_Active_HISR_Tails
    IMPORT  TCD_Interrupt_Level
    IMPORT  TMD_Time_Slice
    IMPORT  TMD_Time_Slice_State


    ; Define external function references.

    IMPORT  TCC_Task_Shell
    IMPORT  TCC_Signal_Shell
    IMPORT  ERC_System_Error

    IF NU_MMU_MODE
        IMPORT  MMT_Invalidate_TLB
        IMPORT  MMT_Set_Translation_Table
        IMPORT  CCT_Invalidate_Cache
        IMPORT  msd_current_module
        IMPORT  msd_kernel_module
    ENDIF

    IF INCLUDE_PROVIEW
        IMPORT  _NU_Idle_Hook
        IMPORT  _NU_Schedule_Task_Hook
    ENDIF

    IF NUCLEUS_THREAD_PROFILING
    	IMPORT  TP_ThreadEnter
    	IMPORT  TP_ThreadExit
    ENDIF

    ; Define internal function references.

    EXPORT  TCT_Schedule_Protected

    ; Define pointers to system variables so their addresses may be obtained in a
    ; pc-relative manner.

Special_Nested
    DCD     TCT_Special_Nested

System_Limit
    DCD     TCT_System_Limit

Int_Level
    DCD     TCD_Interrupt_Level

Task_Shell
    DCD     TCC_Task_Shell

HISR_Shell
    DCD     TCT_HISR_Shell

Signal_Shell
    DCD     TCC_Signal_Shell

Current_Thread
    DCD     TCD_Current_Thread

Execute_HISR
    DCD     TCD_Execute_HISR

Execute_Task
    DCD     TCD_Execute_Task

Time_Slice
    DCD     TMD_Time_Slice

Slice_State
    DCD     TMD_Time_Slice_State

System_Stack
    DCD     TCD_System_Stack

Int_Count
    DCD     TCD_Interrupt_Count

HISR_Tails
    DCD     TCD_Active_HISR_Tails

HISR_Heads
    DCD     TCD_Active_HISR_Heads

    IF NU_MMU_MODE
Current_Module
    DCD     msd_current_module

Kernel_Module
    DCD     msd_kernel_module
    ENDIF

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Control_Interrupts
;*
;* DESCRIPTION
;*
;*      This function enables and disables interrupts as specified by
;*      the caller.  Interrupts disabled by this call are left disabled
;*      until the another call is made to enable them.
;*
;* CALLED BY
;*
;*      Application
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      new_level                           New interrupt enable level
;*
;* OUTPUTS
;*
;*      old_level                           Previous interrupt enable
;*                                            level
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*      C. Meredith     03-01-1994      Lockout interrupts while setting
;*                                        up the new level, resutling in
;*                                        version 1.1
;*      D. Lamie        03-18-1994      Verified version 1.1
;*      M. Trippi       02-03-1997      Masked the return value to only
;*                                        return the interrupt bits.
;*                                        (SPR0252)
;*
;************************************************************************

    EXPORT  TCT_Control_Interrupts
TCT_Control_Interrupts

    ; Lock out all interrupts before any checking or changing

    MRS     r2,CPSR                         ; Pickup current CPSR
    ORR     r2,r2,#LOCKOUT                  ; Build lockout CPSR
    MSR     CPSR_cxsf,r2                    ; Lockout interrupts temporarily

    ; Obtain the current interrupt lockout posture.

    LDR     r1,Int_Level                    ; Pickup interrupt level
    LDR     r3,[r1, #0]                     ; Pickup current interrupt lockout

    ; Setup new interrupt lockout posture.

    BIC     r2,r2,#LOCK_MSK                 ; Clear lockout mask
    ORR     r2,r2,r0                        ; Build new CPSR with appropriate
                                            ;   interrupts locked out
    STR     r0,[r1,#0]                      ; Save current lockout

    ; renable interrupts for the specified lockout

    MSR     CPSR_cxsf,r2                    ; Setup new CPSR lockout bits

    ; Return old interrupt lockout level.

    AND     r0,r3,#LOCK_MSK                 ; Return previous lockout
    BX      lr                              ; Return to caller

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Local_Control_Interrupts
;*
;* DESCRIPTION
;*
;*      This function enables and disables interrupts as specified by
;*      the caller.
;*
;* CALLED BY
;*
;*      Application
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      new_level                           New interrupt enable level
;*
;* OUTPUTS
;*
;*      old_level                           Previous interrupt enable
;*                                            level
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      C. Meredith     03-01-1994      Created initial version 1.1
;*      D. Lamie        03-18-1994      Verified version 1.1
;*      M. Trippi       02-03-1997      Masked the return value to only
;*                                        return the interrupt bits.
;*                                        (SPR0252)
;*
;************************************************************************

    EXPORT  TCT_Local_Control_Interrupts
TCT_Local_Control_Interrupts

    ; read in and save the old level

    MRS     r2,CPSR                         ; Pickup current CPSR
    MOV     r3,r2                           ; save the old level

    ; clear out the old level and set the new level

    BIC     r2,r2,#LOCK_MSK                 ; Clear all current interrupts
    ORR     r2,r2,r0                        ; Build new CPSR with new
                                            ;   interrupt level

    ; renable interrupts for the specified lockout

    MSR     CPSR_cxsf,r2                    ; Setup new CPSR interrupt bits

    ; Return old interrupt lockout level.

    AND     r0,r3,#LOCK_MSK                 ; Return previous lockout
    BX      lr                              ; Return to caller

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Restore_Interrupts
;*
;* DESCRIPTION
;*
;*      This function restores interrupts to that specified in the global
;*      TCD_Interrupt_Level variable.
;*
;* CALLED BY
;*
;*      Application
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      None.
;*
;* OUTPUTS
;*
;*      None.
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      C. Meredith     03-01-1994      Created initial version 1.1
;*      D. Lamie        03-18-1994      Verified version 1.1
;*
;************************************************************************

    EXPORT  TCT_Restore_Interrupts
TCT_Restore_Interrupts

    ; lock out all interrupts before any checking or changing

    MRS     r1,CPSR                     ; Pickup current CPSR
    MOV     r2,r1                       ; save the CPSR value
    ORR     r1,r1,#LOCKOUT              ; Build lockout CPSR
    MSR     CPSR_cxsf,r1                ; Lockout interrupts temporarily

    ; Obtain the current interrupt lockout posture.

    BIC     r2,r2,#LOCK_MSK             ; Clear current interrupt levels
    LDR     r1,Int_Level                ; Load address of TCD_Interrupt_Level
    LDR     r0,[r1, #0]                 ; Pickup current interrupt lockout
    ORR     r2,r2,r0                    ; Build new CPSR with appropriate
                                        ;   interrupts locked out

    ; Setup new CPSR lockout bits

    MSR     CPSR_cxsf,r2

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Build_Task_Stack
;*
;* DESCRIPTION
;*
;*      This function builds an initial stack frame for a task.  The
;*      initial stack contains information concerning initial values of
;*      registers and the task's point of entry.  Furthermore, the
;*      initial stack frame is in the same form as an interrupt stack
;*      frame.
;*
;* CALLED BY
;*
;*      TCC_Create_Task                     Create a new task
;*      TCC_Reset_Task                      Reset the specified task
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      task                                Task control block pointer
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Build_Task_Stack
TCT_Build_Task_Stack

    ; Pickup the stack starting address.

    LDR     r2,[r0,#TC_STACK_START]

    ; Pickup the stack size in bytes.

    LDR     r1,[r0,#TC_STACK_SIZE]

    ; Calculate the stack ending address.

    ADD     r3,r1,r2                        ; Compute the beginning of stack
    BIC     r3,r3,#7                        ; Insure word alignment
    SUB     r3,r3,#8                        ; Reserve a word (doubleword by xavier)

    ; Save the stack ending address.

    STR     r3,[r0,#TC_STACK_END]

; Build an initial stack.  This initial stack frame facilitates an
; interrupt return to the TCC_Task_Shell function, which in turn
; invokes the application task.  The initial stack frame has the
; following format:

;     (Lower Address) Stack Top ->    1       (Interrupt stack type)
;                                     CPSR    Saved CPSR
;                                     r0      Saved r0
;                                     r1      Saved r1
;                                     r2      Saved r2
;                                     r3      Saved r3
;                                     r4      Saved r4
;                                     r5      Saved r5
;                                     r6      Saved r6
;                                     r7      Saved r7
;                                     r8      Saved r8
;                                     r9/sb   Saved r9
;                                     r10/sl  Saved r10
;                                     r11/fp  Saved r11
;                                     r12/ip  Saved r12
;                                     sp      Saved sp
;                                     lr      Saved lr
;     (Higher Address) Stack Bottom-> pc      Saved pc

    ; Reference the task shell.

    LDR     r2,Task_Shell                   ; Pickup address of shell entry

    IF THUMB
        BIC     r2,r2,#1                    ; Clear low bit
    ENDIF

    STR     r2,[r3], #-4                    ; Store entry address on stack
    MOV     r2,#0                           ; Clear value for initial registers
    STR     r2,[r3], #-4                    ; Store initial lr
    ADD     r2,r3,#&8                       ; Compute initial sp
    STR     r2,[r3], #-4                    ; Store initial sp (Stack Bottom)
    STR     r2,[r3], #-4                    ; Store initial ip
    STR     r2,[r3], #-4                    ; Store initial fp
    LDR     r2,[r0,#TC_STACK_START]         ; Pickup the stack starting address
    STR     r2,[r3], #-4                    ; Store initial r10/sl
    MOV     r2,#0                           ; Clear value for initial registers
    STR     r9,[r3], #-4                    ; Store initial r9/sb
    STR     r2,[r3], #-4                    ; Store initial r8
    STR     r2,[r3], #-4                    ; Store initial r7
    STR     r2,[r3], #-4                    ; Store initial r6
    STR     r2,[r3], #-4                    ; Store initial r5
    STR     r2,[r3], #-4                    ; Store initial r4
    STR     r2,[r3], #-4                    ; Store initial r3
    STR     r2,[r3], #-4                    ; Store initial r2
    STR     r2,[r3], #-4                    ; Store initial r1
    STR     r2,[r3], #-4                    ; Store initial r0
    MSR     CPSR_f,r2                       ; Clear the flags
    MRS     r2,CPSR                         ; Pickup the CPSR
    BIC     r2,r2,#LOCK_MSK                 ; Clear initial interrupt lockout

    ; If assembled for THUMB, Set to THUMB state

    IF THUMB
        ORR     r2,r2,#&20
    ENDIF

    IF NU_SUPERV_USER_MODE                  ; All tasks should run in User
        BIC     r2,r2,#MODE_MASK            ; Clear mode bits of CPSR
        ORR     r2,r2,#USR_MODE             ; Set USER mode
    ENDIF

    STR     r2,[r3], #-4                    ; Store CPSR on the initial stack
    MOV     r2,#1                           ; Build interrupt stack type (1)
    STR     r2,[r3, #0]                     ; Store stack type on the top

    ; Save the minimum amount of remaining stack memory.

    MOV     r2,#72                          ; Size of interrupt stack frame
    SUB     r1,r1,r2                        ; Compute minimum available bytes
    STR     r1,[r0, #TC_STACK_MINIMUM]      ; Save in minimum stack area

    ; Save the new stack pointer into the task's control block.

    STR     r3,[r0, #TC_STACK_POINTER]

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Build_HISR_Stack
;*
;* DESCRIPTION
;*
;*      This function builds an HISR stack frame that allows quick
;*      scheduling of the HISR.
;*
;* CALLED BY
;*
;*      TCC_Create_HISR                     Create HISR function
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      hisr                                HISR control block pointer
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Build_HISR_Stack
TCT_Build_HISR_Stack

    ; Pickup the stack starting address

    LDR     r2,[r0,#TC_STACK_START]

    ; Pickup the stack size in bytes

    LDR     r1,[r0,#TC_STACK_SIZE]

    ; Calculate the stack ending address.

    ADD     r3,r1,r2                        ; Compute the beginning of stack
    BIC     r3,r3,#3                        ; Insure word alignment
    SUB     r3,r3,#4                        ; Reserve a word

    ; Save the stack ending address.

    STR     r3,[r0,#TC_STACK_END]

; Build an initial stack.  This initial stack frame facilitates an
; solicited return to the TCT_HISR_Shell function, which in turn
; invokes the appropriate HISR.  The initial HISR stack frame has the
; following format:

;     (Lower Address) Stack Top ->    0       (Solicited stack type)
;      !!FOR THUMB ONLY!!             0/0x20  Saved state mask
;                                     r4      Saved r4
;                                     r5      Saved r5
;                                     r6      Saved r6
;                                     r7      Saved r7
;                                     r8      Saved r8
;                                     r9/sb   Saved r9
;                                     r10/sl  Saved r10
;                                     r11/fp  Saved r11
;                                     r12/ip  Saved r12
;     (Higher Address) Stack Bottom-> pc      Saved pc

    ; Reference the HISR shell.

    LDR     r2,HISR_Shell                   ; Pickup address of shell entry
    STR     r2,[r3], #-4                    ; Store entry address on stack
    ADD     r2,r3,#&4                       ; Compute initial sp
    STR     r2,[r3], #-4                    ; Store initial ip
    STR     r2,[r3], #-4                    ; Store initial fp
    LDR     r2,[r0,#TC_STACK_START]         ; Pickup the stack starting address
    STR     r2,[r3], #-4                    ; Store initial r10/sl
    MOV     r2,#0                           ; Clear value for initial registers
    STR     r9,[r3], #-4                    ; Store initial r9/sb
    STR     r2,[r3], #-4                    ; Store initial r8
    STR     r2,[r3], #-4                    ; Store initial r7
    STR     r2,[r3], #-4                    ; Store initial r6
    STR     r2,[r3], #-4                    ; Store initial r5
    STR     r2,[r3], #-4                    ; Store initial r4

    ; If in THUMB Mode, Store initial state mask

    IF THUMB
        STR     r2,[r3], #-4
    ENDIF

    ; Store solicited stack type on the top of the stack

    STR     r2,[r3, #0]

    ; Save the minimum amount of remaining stack memory.

    IF THUMB
        MOV     r2,#48                      ; Size of solicited stack frame
    ELSE
        MOV     r2,#44                      ; Size of solicited stack frame
    ENDIF

    ; Compute minimum available bytes and save in minimum stack area.

    SUB     r1,r1,r2
    STR     r1,[r0, #TC_STACK_MINIMUM]

    ; Save the new stack pointer into the task's control block.

    STR     r3,[r0, #TC_STACK_POINTER]

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Build_Signal_Frame
;*
;* DESCRIPTION
;*
;*      This function builds a frame on top of the task's stack to
;*      cause the task's signal handler to execute the next time
;*      the task is executed.
;*
;* CALLED BY
;*
;*      TCC_Send_Signals                    Send signals to a task
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      task                                Task control block pointer
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Build_Signal_Frame
TCT_Build_Signal_Frame

    ; Pickup the stack pointer.

    LDR     r3,[r0,#TC_STACK_POINTER]       ; Pickup the current stack pointer

; Build a signal stack.  This signal stack frame facilitates an
; solicited return to the TCC_Signal_Shell function, which in turn
; invokes the appropriate signal handler.  The initial HISR stack frame
; has the following format:

;     (Lower Address) Stack Top ->    0       (Solicited stack type)
;      !!FOR THUMB ONLY!!             0/0x20  Saved state mask
;                                     r4      Saved r4
;                                     r5      Saved r5
;                                     r6      Saved r6
;                                     r7      Saved r7
;                                     r8      Saved r8
;                                     r9/sb   Saved r9
;                                     r10/sl  Saved r10
;                                     r11/fp  Saved r11
;                                     r12/ip  Saved r12
;     (Higher Address) Stack Bottom-> pc      Saved pc

    ; Reference the Signal shell.

    LDR     r2,Signal_Shell                 ; Pickup address of shell entry

    ; If in THUMB state, clear low bit.

    IF THUMB
        BIC     r2,r2,#1
    ENDIF

    SUB     r3,r3,#4                        ; Reserve a word
    STR     r2,[r3], #-4                    ; Store entry address on stack
    ADD     r2,r3,#&4                       ; Compute initial sp
    STR     r2,[r3], #-4                    ; Store initial ip
    STR     r2,[r3], #-4                    ; Store initial fp
    LDR     r2,[r0,#TC_STACK_START]         ; Pickup the stack starting address
    STR     r2,[r3], #-4                    ; Store initial r10/sl
    MOV     r2,#0                           ; Clear value for initial registers
    STR     sb,[r3], #-4                    ; Store initial v6/sb
    STR     r2,[r3], #-4                    ; Store initial r8
    STR     r2,[r3], #-4                    ; Store initial r7
    STR     r2,[r3], #-4                    ; Store initial r6
    STR     r2,[r3], #-4                    ; Store initial r5
    STR     r2,[r3], #-4                    ; Store initial r4

    IF THUMB
        MOV     r1,#&20                     ; Get initial state mask
        STR     r1,[r3], #-4                ; Store initial state mask
    ENDIF

    ; Store solicited stack type on the top of the stack

    STR     r2,[r3, #0]

    ; Save the new stack pointer into the task's control block.

    STR     r3,[r0, #TC_STACK_POINTER]

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Check_Stack
;*
;* DESCRIPTION
;*
;*      This function checks the current stack for overflow conditions.
;*      Additionally, this function keeps track of the minimum amount
;*      of stack space for the calling thread and returns the current
;*      available stack space.
;*
;* CALLED BY
;*
;*      TCC_Send_Signals                    Send signals to a task
;*
;* CALLS
;*
;*      ERC_System_Error                    System error handler
;*
;* INPUTS
;*
;*      None
;*
;* OUTPUTS
;*
;*      available bytes in stack
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Check_Stack
TCT_Check_Stack

    ; Pickup the current thread pointer.

    LDR     r0,Current_Thread
    LDR     r0,[r0,#0]

    ; Determine if there is a current thread.

    CMP     r0,#0                           ; Determine if a thread is active
    MOV     r3,#0                           ; Default remaining value
    BEQ     TCT_Skip_Stack_Check            ; If NU_NULL, skip stack checking

    ; Determine if the stack pointers are out of range.

    LDR     r2,[r0,#TC_STACK_START]         ; Pickup start of stack area
    CMP     sp,r2                           ; Compare with current stack ptr
    BLT     TCT_Stack_Range_Error           ; If less, stack is out of range
    LDR     r1,[r0,#TC_STACK_END]           ; Pickup end of stack area
    CMP     sp,r1                           ; Compare with current stack ptr
    BLE     TCT_Stack_Range_Okay            ; If less, stack range is okay

    ; Stack overflow condition exits.

TCT_Stack_Range_Error

    STR     lr,[sp, #4]!                    ; Store lr on the stack
    MOV     r0,#3                           ; Build NU_STACK_OVERFLOW code

    LDR     r3,=ERC_System_Error            ; Call system error handler.  Note:
    BX      r3                              ;   control is not returned!
                                            ;   Examine stack to find return
                                            ;   address of this routine.
    ; Stack range is okay.

TCT_Stack_Range_Okay

    ; Calculate the amount of available space on the stack.

    SUB     r3,sp,r2

    ; Determine if there is enough memory on the stack to save
    ; all of the registers.

    CMP     r3,#80

    ; If there is enough memory, there is no stack overflow yet
    ; branch to TCT_No_Stack_Error.

    BCS     TCT_No_Stack_Error

    ; Stack overflow condition is about to happen.

    STR     lr,[sp, #4]!                    ; Store lr on the stack
    MOV     r0,#3                           ; Build NU_STACK_OVERFLOW code

    LDR     r3,=ERC_System_Error            ; Call system error handler.  Note:
    BX      r3                              ;   control is not returned!
                                            ;   Examine stack to find return
                                            ;   address of this routine.

TCT_No_Stack_Error

    ; Determine if this is a new minimum amount of stack space.
    ; If so, save the new stack minimum.

    LDR     r2,[r0,#TC_STACK_MINIMUM]
    CMP     r3,r2
    STRCC   r3,[r0,#TC_STACK_MINIMUM]

TCT_Skip_Stack_Check

    ; Return the remaining number of bytes on the stack.

    MOV     r0,r3

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Schedule
;*
;* DESCRIPTION
;*
;*      This function waits for a thread to become ready.  Once a thread
;*      is ready, this function initiates a transfer of control to that
;*      thread.
;*
;* CALLED BY
;*
;*      INC_Initialize                      Main initialization routine
;*
;* CALLS
;*
;*      TCT_Control_To_Thread               Transfer control to a thread
;*
;* INPUTS
;*
;*      TCD_Execute_Task                    Pointer to task to execute
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Schedule
TCT_Schedule

;	IF NUCLEUS_THREAD_PROFILING
;		BL      TP_ThreadExit               ; call thread exit
;	ENDIF

    ; Restore interrupts according to the value contained in
    ; TCD_Interrupt_Level.

    LDR     r1,Int_Level                    ; Get address of interrupt level
    MRS     r0,CPSR                         ; Pickup current CPSR
    LDR     r2,[r1, #0]                     ; Pickup current interrupt lockout
    BIC     r0,r0,#LOCK_MSK                 ; Clear the interrupt lockout bits
    ORR     r0,r0,r2                        ; Build new interrupt lockout CPSR
    MSR     CPSR_cxsf,r0                    ; Setup new CPSR

    ; Get the addresses of the HISR and Task

    LDR     r2,Execute_HISR                 ; Pickup TCD_Execute_HISR address
    LDR     r3,Execute_Task                 ; Pickup TCD_Execute_Task address

    ; If Nucleus Proview is defined.

    IF INCLUDE_PROVIEW

    ; Check to see if there is a task to execute upon entering TCT_Schedule.
    ; If not, we start IDLE.

        LDR     r0,[r2, #0]                 ; Pickup highest priority HISR ptr
        CMP     r0,#0                       ; Is there a HISR active?
        BNE     TCT_Schedule_Thread         ; Found an HISR
        LDR     r0,[r3, #0]                 ; Pickup highest priority Task ptr
        CMP     r0,#0                       ; Is there a task active?
        BNE     TCT_Schedule_Thread         ; If not, start IDLE.
        STR     r2,[sp, #-4]!               ; Save r2 on the stack
        STR     r3,[sp, #-4]!               ; Save r3 on the stack
        BL      _NU_Idle_Hook
        LDR     r3,[sp], #4                 ; Recover r2
        LDR     r2,[sp], #4                 ; Recover r3

    ENDIF



TCT_Schedule_Loop

    ; Wait until a thread (task or HISR) is available to execute.
    ; When a thread is available, branch to TCT_Schedule_Thread.

    LDR     r0,[r2, #0]                     ; Pickup highest priority HISR ptr
    CMP     r0,#0                           ; Is there a HISR active?
    BNE     TCT_Schedule_Thread             ; Found a HISR
    LDR     r0,[r3, #0]                     ; Pickup highest priority Task ptr
    CMP     r0,#0                           ; Is there a task active?
    BEQ     TCT_Schedule_Loop               ; If not, continue the search

TCT_Schedule_Thread

    ; Either a task or HISR is ready to execute.  Lockout interrupts while
    ; the thread is transferred (by falling through to the following routine).

    MRS     r1,CPSR                         ; Pickup CPSR again
    ORR     r1,r1,#LOCKOUT                  ; Build interrupt lockout value
    MSR     CPSR_cxsf,r1                    ; Lockout interrupts

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Control_To_Thread
;*
;* DESCRIPTION
;*
;*      This function transfers control to the specified thread.  Each
;*      time control is transferred to a thread, its scheduled counter
;*      is incremented.  Additionally, time-slicing for task threads is
;*      enabled in this routine.  The TCD_Current_Thread pointer is
;*      setup by this function.
;*
;* CALLED BY
;*
;*      TCT_Schedule                        Indirectly called
;*      TCT_Protect                         Protection task switch
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      thread                              Thread control block pointer
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*      R. Baum         04-17-2002      Added code to update page tables
;*                                      for the MMU/Modules product.
;*
;************************************************************************

    EXPORT  TCT_Control_To_Thread
TCT_Control_To_Thread

    ; Setup the current thread pointer.

    IF NU_MMU_MODE
        MMU_DISABLE
    ENDIF

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r2,[r0, #TC_SCHEDULED]          ; Pickup scheduled count
    STR     r0,[r1, #0]                     ; Setup current thread pointer

    ; Increment the thread scheduled counter.

    LDR     r3,[r0, #TC_CUR_TIME_SLICE]     ; Pickup time slice value
    ADD     r2,r2,#1                        ; Increment the scheduled count
    STR     r2,[r0, #TC_SCHEDULED]          ; Store new scheduled count

    ; Check for time slice option.

    CMP     r3,#0                           ; Is there a time slice?
    BEQ     TCT_No_Start_TS_1               ; If 0, there is no time slice

    ; Start a time slice.

    LDR     r2,Time_Slice                   ; Pickup address of TMD_Time_Slice
    LDR     r1,Slice_State                  ; Pickup address of
                                            ; TMD_Time_Slice_State
    STR     r3,[r2, #0]                     ; Setup the time slice
    MOV     r2,#0                           ; Build active state flag
    STR     r2,[r1,#0]                      ; Set the active flag

TCT_No_Start_TS_1

    ; Nucleus MMU
    ; Get module pointer, switch Translation Table, flush TLBs, and
    ; flush cache for the new thread

    IF NU_MMU_MODE

        LDR     r1,Current_Module           ; load current module pointer
        LDR     r2,[r1, #0]                 ; setup module pointer

        LDR     r3,[r0, #TC_ID]             ; Pickup id for current thread
        LDR     r4,=TC_TASK_ID
        CMP     r3,r4                       ; determine if thread is task or HISR
        BNE     TCT_HISR_Module             ; if not equal this is a HISR

        LDR     r3,[r0, #TC_MODULE]         ; Pickup module pointer of task thread
        B       TCT_Check_Same_Module       ; skip the HISR code

TCT_HISR_Module

        LDR     r3,[r0, #TC_HISR_MODULE]    ; Pickup module pointer of HISR thread

TCT_Check_Same_Module

        CMP     r2,r3                       ; is new thread part of current module?
        BEQ     TCT_Same_Module             ; if so, do nothing

        STR     r3,[r1, #0]                 ; update the current module pointer

        STMDB   sp!,{r0}                    ; Save r0 so we can pass TTBR to
                                            ; MMT_Set_Translation_Table
        LDR     r0,[r3, #MS_TARGET]         ; Get Level 1 base from module
        BL      MMT_Set_Translation_Table   ; and set it up

        LDMIA   sp!,{r0}                    ; restore r0 from stack

        BL      MMT_Invalidate_TLB          ; invalidate all TLB entries

        BL      CCT_Invalidate_Cache        ; invalidate all cache

TCT_Same_Module

        LDR     r2,Kernel_Module            ; Get the kernel module
        CMP     r3,r2                       ; compare to the current module
        MOV     r2,#0                       ; initialize the register
        BNE     TCT_Skip_Kernel_Check       ; leave zero for possible MMU Enable
        MOV     r2,#1                       ; change to 1 so we don't enable MMU

TCT_Skip_Kernel_Check

    ENDIF

    ; If Nucleus Proview is defined.

    IF INCLUDE_PROVIEW

        STR     r0,[sp, #-4]!               ; Save r0 on the stack
        BL      _NU_Schedule_Task_Hook      ; Branch to RTView
        LDR     r0,[sp], #4                 ; Recover return address

    ENDIF

	IF NUCLEUS_THREAD_PROFILING
		STR     r0,[sp, #-4]!               ; save r0 (C return value destroys r0)
		BL      TP_ThreadEnter              ; call thread enter
		LDR     r0,[sp], #4                 ; recover thread pointer
	ENDIF

    ; Pickup the thread's stack pointer and resume the thread.

    LDR     sp,[r0, #TC_STACK_POINTER]

    ; find out if we need to turn on MMU

    IF NU_MMU_MODE
        LDR     r3,[r0, #TC_SU_MODE]
        ORR     r3,r3,r2
    ENDIF

    ; Pop off the saved information associated with the thread. After we
    ; determine which type of stack is present.  A 1 on the top of the
    ; stack indicates an interrupt stack, while a 0 on the top of the
    ; stack indicates a solicited type of stack.

    ; Remember that the interrupt level that is restored must represent
    ; the interrupt level in TCD_Interrupt_Level.

    LDR     r1,[sp], #4                     ; Pop off the stack type
    CMP     r1,#1                           ; See if it is an interrupt stack
    BEQ     TCT_Interrupt_Resume            ; If so, an interrupt resume of
                                            ; thread is required
    LDR     r1,Int_Level                    ; Pickup address of interrupt
                                            ; lockout
    MRS     r0,CPSR                         ; Pickup current CPSR
    BIC     r0,r0,#LOCK_MSK                 ; Clear lockout mask
    LDR     r2,[r1, #0]                     ; Pickup interrupt lockout mask
    ORR     r0,r0,r2                        ; Build new interrupt lockout mask

    IF THUMB
        LDR     r2,[sp], #4                 ; Pop off the state mask
        ORR     r0,r0,r2                    ; Set appropriate state
    ENDIF

    IF NU_SUPERV_USER_MODE
        MOV     r2,sp                       ; save current stack pointer
        ADD     sp,sp,#40
        MRS     r1,CPSR                     ; Pickup current CPSR
        BIC     r1,r1,#MODE_MASK            ; Clear mode
        ORR     r1,r1,#SUP_MODE             ; Set to supervisor
        MSR     CPSR_cxsf,r1                ; change to supervisor
        MOV     sp,r2                       ; restore stack pointer
    ENDIF

    MSR     SPSR_cxsf,r0                    ; Place it into the SPSR

    IF NU_MMU_MODE
        CMP     r3,#0
        BNE     TCT_Skip_MMU_Enable
        MMU_ENABLE

TCT_Skip_MMU_Enable
    ENDIF

    LDMIA   sp!,{r4-r12,pc}^                ; A solicited return is required.
                                            ; This type of return only
                                            ; recovers r4-r12 & pc
TCT_Interrupt_Resume

    LDR     r0,[sp], #4                     ; Pop off the CPSR
    LDR     r1,Int_Level                    ; Pickup address of interrupt
                                            ; lockout
    BIC     r0,r0,#LOCK_MSK                 ; Clear lockout mask
    LDR     r2,[r1, #0]                     ; Pickup interrupt lockout mask
    ORR     r0,r0,r2                        ; Build new interrupt lockout mask

    IF NU_SUPERV_USER_MODE
        ADD     r2,sp,#56                   ; calculate lr start
        LDMIA   r2,{lr}                     ; restore lr in system mode
        MOV     r2,sp                       ; save current stack pointer
        ADD     sp,sp,#64                   ; calculate new stack pointer
        MRS     r1,CPSR                     ; Pickup current CPSR
        BIC     r1,r1,#MODE_MASK            ; Clear mode
        ORR     r1,r1,#SUP_MODE             ; Set to supervisor
        MSR     CPSR_cxsf,r1                ; change to supervisor
        MOV     sp,r2                       ; restore old stack pointer for return
    ENDIF

    MSR     SPSR_cxsf,r0                    ; Place it into the SPSR

    IF NU_MMU_MODE
        CMP     r3,#0
        BNE     TCT_Skip_Interrupt_MMU_Enable
        MMU_ENABLE
TCT_Skip_Interrupt_MMU_Enable
    ENDIF

    ; Recover all registers and resume at point of interrupt

    LDMIA   sp,{r0-pc}^

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Control_To_System
;*
;* DESCRIPTION
;*
;*      This function returns control from a thread to the system.  Note
;*      that this service is called in a solicited manner, i.e. it is
;*      not called from an interrupt thread.  Registers required by the
;*      compiler to be preserved across function boundaries are saved by
;*      this routine.  Note that this is usually a sub-set of the total
;*      number of available registers.
;*
;* CALLED BY
;*
;*      Other Components
;*
;* CALLS
;*
;*      TCT_Schedule                        Schedule the next thread
;*
;* INPUTS
;*
;*      None
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*      C. Meredith     03-01-1994      Corrected problem in time-slice
;*                                        reset logic, resulting in
;*                                        version 1.1
;*      D. Lamie        03-18-1994      Verified version 1.1
;*
;************************************************************************

    EXPORT  TCT_Control_To_System
TCT_Control_To_System

    ; Lockout interrupts.

    MRS     r0,CPSR                         ; Pickup current CPSR
    ORR     r0,r0,#LOCKOUT                  ; Build interrupt lockout value
    MSR     CPSR_cxsf,r0                    ; Lockout interrupts

    ; Save a minimal context of the thread on the current stack.

    STMDB   sp!,{r4-r12,lr}

    IF THUMB
        MOV     r2,lr                       ; Determine what state the caller
        MOV     r2,r2,LSL #31               ; was in and build an
        MOV     r2,r2,LSR #26               ; appropriate state mask
        STR     r2,[sp, #-4]!               ; Place it on the stack
    ENDIF

    ; Build solicited stack type value and NU_NULL value and place it on the
    ; top of the stack.

    MOV     r2,#0
    STR     r2,[sp, #-4]!

    ; Setup a pointer to the thread control block and set the current
    ; thread pointer to NU_NULL.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r0,[r1, #0]                     ; Pickup current thread pointer
    STR     r2,[r1, #0]                     ; Set current thread pointer to
                                            ; NU_NULL

    ; Check to see if a time slice is active.

    LDR     r3,Slice_State                  ; Pickup time slice state address
    LDR     r1,[r3, #0]                     ; Pickup time slice state flag
    CMP     r1,#0                           ; Compare with active value
    BNE     TCT_No_Stop_TS_1                ; If non-active, don't disable

    ; Insure that the next time the task runs it gets a fresh time slice.
    ; Clear any active time slice by setting the state to NOT_ACTIVE.

    LDR     r1,[r0, #TC_TIME_SLICE]         ; Pickup original time slice
    MOV     r2,#1                           ; Build disable value
    STR     r2,[r3, #0]                     ; Disable time slice
    STR     r1,[r0, #TC_CUR_TIME_SLICE]     ; Reset current time slice

TCT_No_Stop_TS_1

    ; Save off the current stack pointer in the control block.

    STR     sp,[r0, #TC_STACK_POINTER]      ; Save the thread's stack pointer

    ; Clear the task's current protection.

    LDR     r1,[r0, #TC_CURRENT_PROTECT]    ; Pickup current thread pointer
    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r0, #TC_CURRENT_PROTECT]    ; Clear the protect pointer field
    STR     r2,[r1, #0]                     ; Release the actual protection

    ; Switch to the system stack.

    LDR     r1,System_Stack                 ; Pickup address of stack pointer
    LDR     r2,System_Limit                 ; Pickup address of stack limit ptr
    LDR     sp,[r1, #0]                     ; Switch to system stack
    LDR     r10,[r2, #0]                    ; Setup system stack limit

    ; Finished, return to the scheduling loop.

	IF NUCLEUS_THREAD_PROFILING
		BL      TP_ThreadExit               ; call thread exit
	ENDIF

    B       TCT_Schedule                    ; Return to scheduling loop

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Signal_Exit
;*
;* DESCRIPTION
;*
;*      This function exits from a signal handler.  The primary purpose
;*      of this function is to clear the scheduler protection and switch
;*      the stack pointer back to the normal task's stack pointer.
;*
;* CALLED BY
;*
;*      TCC_Signal_Shell                    Signal handling shell func
;*
;* CALLS
;*
;*      TCT_Schedule                        Scheduler
;*
;* INPUTS
;*
;*      None
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*      C. Meredith     03-01-1994      Corrected problem in time-slice
;*                                        reset logic, resulting in
;*                                        version 1.1
;*      D. Lamie        03-18-1994      Verified version 1.1
;*
;************************************************************************

    EXPORT  TCT_Signal_Exit
TCT_Signal_Exit

    ; Lockout interrupts.

    MRS     r3,CPSR                         ; Pickup current CPSR
    ORR     r3,r3,#LOCKOUT                  ; Build lockout value
    MSR     CPSR_cxsf,r3                    ; Lockout interrupts

    ; Setup a pointer to the thread control block.

    LDR     r1,Current_Thread               ; Pickup address of thread pointer
    LDR     r0,[r1,#0]                      ; Pickup current thread pointer

    ; Clear the current thread control block.

    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r1, #0]                     ; Clear current thread pointer

    ; Check to see if a time slice is active.

    LDR     r3,Slice_State                  ; Pickup time slice state address
    LDR     r1,[r3, #0]                     ; Pickup time slice state flag
    CMP     r1,#0                           ; Compare with active value
    BNE     TCT_No_Stop_TS_2                ; If non-active, don't disable

    ; Insure that the next time the task runs it gets a fresh time slice.
    ; Clear any active time slice by setting the state to NOT_ACTIVE.

    LDR     r1,[r0, #TC_TIME_SLICE]         ; Pickup original time slice
    MOV     r2,#1                           ; Build disable value
    STR     r2,[r3, #0]                     ; Disable time slice
    STR     r1,[r0, #TC_CUR_TIME_SLICE]     ; Reset current time slice

TCT_No_Stop_TS_2

    ; Switch back to the saved stack.  The saved stack pointer was saved
    ; before the signal frame was built.

    LDR     r1,[r0, #TC_SAVED_STACK_PTR]    ; Pickup saved stack pointer
    STR     r1,[r0, #TC_STACK_POINTER]      ; Place in current stack pointer

    ; Clear the task's current protection.

    LDR     r1,[r0, #TC_CURRENT_PROTECT]    ; Pickup current thread pointer
    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r0, #TC_CURRENT_PROTECT]    ; Clear the protect pointer field
    STR     r2,[r1, #0]                     ; Release the actual protection

    ; Switch to the system stack.

    LDR     r1,System_Stack                 ; Pickup address of stack pointer
    LDR     r2,System_Limit                 ; Pickup address of stack limit ptr
    LDR     sp,[r1, #0]                     ; Switch to system stack
    LDR     r10,[r2, #0]                    ; Setup system stack limit

    ; Finished, return to the scheduling loop.

	IF NUCLEUS_THREAD_PROFILING
		BL      TP_ThreadExit               ; call thread exit
	ENDIF

    B       TCT_Schedule                    ; Return to scheduling loop

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Current_Thread
;*
;* DESCRIPTION
;*
;*      This function returns the current thread pointer.
;*
;* CALLED BY
;*
;*      Application
;*      System Components
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      None
;*
;* OUTPUTS
;*
;*      Pointer to current thread
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Current_Thread
TCT_Current_Thread

    ; Return the current thread pointer.

    LDR     r0,Current_Thread               ; Pickup address of thread pointer
    LDR     r0,[r0, #0]                     ; Pickup current thread pointer

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Set_Execute_Task
;*
;* DESCRIPTION
;*
;*      This function sets the current task to execute variable under
;*      protection against interrupts.
;*
;* CALLED BY
;*
;*      TCC Scheduling Routines
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      task                                Pointer to task control block
;*
;* OUTPUTS
;*
;*      TCD_Execute_Task                    Modified variable
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Set_Execute_Task
TCT_Set_Execute_Task

    ; Setup the TCD_Execute_Task pointer.

    LDR     r1,Execute_Task                 ; Pickup execute task ptr address
    STR     r0,[r1,#0]                      ; Setup new task to execute

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Protect
;*
;* DESCRIPTION
;*
;*      This function protects against multiple thread access.
;*
;* CALLED BY
;*
;*      Application
;*      System Components
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      protect                            Pointer to protection block
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Protect
TCT_Protect

    ; Determine if the caller is in a task or HISR thread.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r3,[r1, #0]                     ; Pickup current thread pointer
    CMP     r3,#0                           ; Check to see if it is non-NULL
    BEQ     TCT_Skip_Protect                ; If NULL, skip protection

    ; Lockout interrupts.

    MRS     r1,CPSR                         ; Pickup current CPSR
    ORR     r1,r1,#LOCKOUT                  ; Place lockout value in
    MSR     CPSR_cxsf,r1                    ; Lockout interrupts

TCT_Protect_Loop

    ; Wait until the protect structure is available.

    LDR     r1,[r0, #0]                     ; Pickup protection owner field
    CMP     r1,#0                           ; Is there any protection?
    BEQ     TCT_Protect_Available           ; If NU_NULL, no current protection

    ; Protection structure is not available.
    ; Indicate that another thread is waiting.

    MOV     r2,#1                           ; Build thread waiting flag
    STR     r2,[r0, #4]                     ; Set waiting field

    ; Directly schedule the thread waiting.

    STR     r0,[sp, #-4]!                   ; Save r0 on the stack
    STR     lr,[sp, #-4]!                   ; Save lr on the stack
    MOV     r0,r3                           ; Place current thread into r0
    BL      TCT_Schedule_Protected          ; Call routine to schedule the
                                            ; owner of the thread
    LDR     lr,[sp], #4                     ; Recover saved lr
    LDR     r0,[sp], #4                     ; Recover saved r0

    ; Get the current thread pointer

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r3,[r1, #0]                     ; Pickup current thread pointer

    ; Lockout interrupts.

    MRS     r1,CPSR                         ; Pickup current CPSR
    ORR     r1,r1,#LOCKOUT                  ; Place lockout value in
    MSR     CPSR_cxsf,r1                    ; Lockout interrupts

    ; Examine protect flags again

    B       TCT_Protect_Loop

TCT_Protect_Available

    ; Protection structure is available.
    ; Indicate that this thread owns the protection.

    STR     r3,[r0, #0]                     ; Indicate calling thread owns this
                                            ; protection

    ; Clear the thread waiting flag.

    MOV     r2,#0                           ; Clear value
    STR     r2,[r0, #4]                     ; Clear the thread waiting flag

    ; Save the protection pointer in the thread's control block.  Note
    ; that both task and HISR threads share the same control block format.

    STR     r0,[r3, #TC_CURRENT_PROTECT]    ; Setup current protection

    ; Restore interrupts.

    LDR     r2,Int_Level                    ; Pickup address of interrupt level
    MRS     r1,CPSR                         ; Pickup current CPSR
    LDR     r3,[r2, #0]                     ; Pickup interrupt lockout level
    BIC     r1,r1,#LOCK_MSK                 ; Clear lockout bits
    ORR     r1,r1,r3                        ; Build new interrupt lockout
    MSR     CPSR_cxsf,r1                    ; Setup CPSR appropriately

TCT_Skip_Protect

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Unprotect
;*
;* DESCRIPTION
;*
;*      This function releases protection of the currently active
;*      thread.  If the caller is not an active thread, then this
;*      request is ignored.
;*
;* CALLED BY
;*
;*      Application
;*      System Components
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      None
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Unprotect
TCT_Unprotect

    ; Determine if the caller is in a task or HISR thread.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r3,[r1, #0]                     ; Pickup current thread pointer
    CMP     r3,#0                           ; Check to see if it is non-NULL
    BEQ     TCT_Skip_Unprotect              ; If NULL, skip unprotection

    ; Determine if there is a currently active protection.

    LDR     r0,[r3, #TC_CURRENT_PROTECT]    ; Pickup current protect field
    CMP     r0,#0                           ; Is there a protection in force?
    BEQ     TCT_Skip_Unprotect              ; If not, nothing is protected

    ; Lockout interrupts.

    MRS     r1,CPSR                         ; Pickup current CPSR
    ORR     r1,r1,#LOCKOUT                  ; Place lockout value in
    MSR     CPSR_cxsf,r1                    ; Lockout interrupts

    ; Check for a higher priority thread waiting for the protection structure.

    LDR     r2,[r0, #4]                     ; Pickup thread waiting flag
    CMP     r2,#0                           ; Are there any threads waiting?
    BEQ     TCT_Not_Waiting_Unpr            ; If not, just release protection

    ; Transfer control to the system.  Note that this
    ; automatically clears the current protection and it returns
    ; to the caller of this routine instead of this routine.

    B       TCT_Control_To_System           ; Return control to the system

TCT_Not_Waiting_Unpr

    ; Clear the protection.

    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r0, #0]                     ; Release the protection
    STR     r2,[r3, #TC_CURRENT_PROTECT]    ; Clear protection pointer in the
                                            ; control block

TCT_Not_Protected

    ; Restore interrupts again.

    LDR     r2,Int_Level                    ; Pickup address of interrupt level
    MRS     r1,CPSR                         ; Pickup current CPSR
    LDR     r3,[r2, #0]                     ; Pickup interrupt lockout level
    BIC     r1,r1,#LOCK_MSK                 ; Clear lockout bits
    ORR     r1,r1,r3                        ; Build new interrupt lockout
    MSR     CPSR_cxsf,r1                    ; Setup CPSR appropriately

TCT_Skip_Unprotect

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Unprotect_Specific
;*
;* DESCRIPTION
;*
;*      This function releases a specific protection structure.
;*
;* CALLED BY
;*
;*      Application
;*      System Components
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      protect                            Pointer to protection block
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*      C. Meredith     03-01-1994      Corrected problem in time-slice
;*                                        reset logic, corrected bug
;*                                        using protect ptr, resulting
;*                                        in version 1.1
;*      D. Lamie        03-18-1994      Verified version 1.1
;*
;************************************************************************

    EXPORT  TCT_Unprotect_Specific
TCT_Unprotect_Specific

    ; Determine if the caller is in a task or HISR thread.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r3,[r1, #0]                     ; Pickup current thread pointer
    CMP     r3,#0                           ; Check to see if it is non-NULL
    BEQ     TCT_Skip_Unprot_Spec            ; If NULL, skip unprotect specific

    ; Lockout interrupts.

    MRS     r1,CPSR                         ; Pickup current CPSR
    ORR     r1,r1,#LOCKOUT                  ; Place lockout value in
    MSR     CPSR_cxsf,r1                    ; Lockout interrupts

    ; Clear the protection pointer.

    MOV     r2,#0                           ; Build NU_NULL value
    STR     r2,[r0, #0]                     ; Clear protection ownership

    ; Determine if a thread is waiting.

    LDR     r1,[r0, #4]                     ; Pickup the waiting field
    CMP     r1,#0                           ; Is there another thread waiting?
    BEQ     TCT_Not_Waiting_Unspec          ; No, restore interrupts and return

    ; Save a minimal context of the thread on the current stack.

    STMDB   sp!,{r4-r12,lr}

    ; Determine the current state.

    IF THUMB
        MOV     r2,lr                       ; Determine what state the caller
        MOV     r2,r2,LSL #31               ; was in and build an
        MOV     r2,r2,LSR #26               ; appropriate state mask
        STR     r2,[sp, #-4]!               ; Place it on the stack
    ENDIF

    ; Place the solicited stack type value and NU_NULL value on the
    ; top of the stack

    MOV     r2,#0                           ; Build solicited stack type value
                                            ; and NU_NULL value
    STR     r2,[sp, #-4]!                   ; Place it on the top of the stack

    ; Setup a pointer to the thread control block and clear the current
    ; thread control block pointer.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r0,[r1, #0]                     ; Pickup current thread pointer
    STR     r2,[r1, #0]                     ; Set current thread pointer to
                                            ; NU_NULL

    ; Check to see if a time slice is active.

    LDR     r3,Slice_State                  ; Pickup time slice state address
    LDR     r1,[r3, #0]                     ; Pickup time slice state flag
    CMP     r1,#0                           ; Compare with active value
    BNE     TCT_No_Stop_TS_3                ; If non-active, don't disable

    ; Insure that the next time the task runs it gets a fresh time slice
    ; by setting the state to NOT_ACTIVE.

    LDR     r1,[r0, #TC_TIME_SLICE]         ; Pickup original time slice
    MOV     r2,#1                           ; Build disable value
    STR     r2,[r3, #0]                     ; Disable time slice
    STR     r1,[r0, #TC_CUR_TIME_SLICE]     ; Reset current time slice

TCT_No_Stop_TS_3

    ; Save off the current stack pointer in the control block.

    STR     sp,[r0, #TC_STACK_POINTER]

    ; Switch to the system stack.

    LDR     r1,System_Stack                 ; Pickup address of stack pointer
    LDR     r2,System_Limit                 ; Pickup address of stack limit ptr
    LDR     sp,[r1, #0]                     ; Switch to system stack
    LDR     r10,[r2, #0]                    ; Setup system stack limit

    ; Finished, return to the scheduling loop.

	IF NUCLEUS_THREAD_PROFILING
		BL      TP_ThreadExit               ; call thread exit
	ENDIF

    B       TCT_Schedule

TCT_Not_Waiting_Unspec

    ; Restore interrupts.

    LDR     r2,Int_Level                    ; Pickup address of interrupt level
    MRS     r1,CPSR                         ; Pickup current CPSR
    LDR     r3,[r2, #0]                     ; Pickup interrupt lockout level
    BIC     r1,r1,#LOCK_MSK                 ; Clear lockout bits
    ORR     r1,r1,r3                        ; Build new interrupt lockout
    MSR     CPSR_cxsf,r1                    ; Setup CPSR appropriately

TCT_Skip_Unprot_Spec

    ; Return to caller.

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Set_Current_Protect
;*
;* DESCRIPTION
;*
;*      This function sets the current protection field of the current
;*      thread's control block to the specified protection pointer.
;*
;* CALLED BY
;*
;*      TCC_Resume_Task                    Resume task function
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      protect                            Pointer to protection block
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Set_Current_Protect
TCT_Set_Current_Protect

    ; Determine if the caller is in a task or HISR thread.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r3,[r1, #0]                     ; Pickup current thread pointer
    CMP     r3,#0                           ; Check to see if a thread is
                                            ; active

    ; If the caller is in a task or HISR, modify the current protection.

    STRNE   r0,[r3, #TC_CURRENT_PROTECT]

    ; Return to caller.

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Protect_Switch
;*
;* DESCRIPTION
;*
;*      This function waits until a specific task no longer has any
;*      protection associated with it.  This is necessary since task's
;*      cannot be suspended or terminated unless they have released all
;*      of their protection.
;*
;* CALLED BY
;*
;*      System Components
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      thread                             Pointer to thread control blk
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Protect_Switch
TCT_Protect_Switch

    ; Lockout interrupts.

    MRS     r1,CPSR                         ; Pickup current CPSR
    ORR     r1,r1,#LOCKOUT                  ; Place lockout value in
    MSR     CPSR_cxsf,r1                    ; Lockout interrupts

    ; Wait until the specified task has no protection associated with it.

    LDR     r1,[r0, #TC_CURRENT_PROTECT]    ; Pickup protection of specified
                                            ; thread
    CMP     r1,#0                           ; Does the specified thread have
                                            ; an active protection?
    BEQ     TCT_Switch_Done                 ; If not, protect switch is done

    ; Indicate that a higher priority thread is waiting.

    MOV     r2,#1                           ; Build waiting flag value
    STR     r2,[r1, #4]                     ; Set waiting flag of the
                                            ; protection owned by the other
                                            ; thread

    ; Directly schedule the thread waiting.

    LDR     r2,Current_Thread               ; Pickup current thread ptr address
    STR     r0,[sp, #-4]!                   ; Save r0 on the stack
    STR     lr,[sp, #-4]!                   ; Save lr on the stack
    MOV     r1,r0                           ; Move new thread into r1
    LDR     r0,[r2, #0]                     ; Pickup current thread pointer
    BL      TCT_Schedule_Protected          ; Call routine to schedule the
                                            ; owner of the thread
    LDR     lr,[sp], #4                     ; Recover saved lr
    LDR     r0,[sp], #4                     ; Recover saved r0

    B       TCT_Protect_Switch              ; Branch to top of routine and
                                            ; start over

TCT_Switch_Done

    ; Restore interrupts.

    LDR     r2,Int_Level                    ; Pickup address of interrupt level
    MRS     r1,CPSR                         ; Pickup current CPSR
    LDR     r3,[r2, #0]                     ; Pickup interrupt lockout level
    BIC     r1,r1,#LOCK_MSK                 ; Clear lockout bits
    ORR     r1,r1,r3                        ; Build new interrupt lockout
    MSR     CPSR_cxsf,r1                    ; Setup CPSR appropriately

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Schedule_Protected
;*
;* DESCRIPTION
;*
;*      This function saves the minimal context of the thread and then
;*      directly schedules the thread that has protection over the
;*      the thread that called this routine.
;*
;* CALLED BY
;*
;*      TCT_Protect
;*      TCT_Protect_Switch
;*
;* CALLS
;*
;*      TCT_Control_To_Thread               Transfer control to protected
;*                                              thread
;*
;* INPUTS
;*
;*      None
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*      C. Meredith     03-01-1994      Corrected problem in time-slice
;*                                        reset logic, resulting in
;*                                        version 1.1
;*      D. Lamie        03-18-1994      Verified version 1.1
;*
;************************************************************************

TCT_Schedule_Protected

    ; Interrupts are already locked out by the caller.
    ; Save minimal context required by the system.

    STMDB   sp!,{r4-r12,lr}                 ; Save minimal context of thread on
                                            ; the current stack
    IF THUMB
        MOV     r2,lr                       ; Determine what state the caller
        MOV     r2,r2,LSL #31               ; was in and build an
        MOV     r2,r2,LSR #26               ; appropriate state mask
        STR     r2,[sp, #-4]!               ; Place it on the stack
    ENDIF

    ; Put solicited stack type value and NU_NULL value on top of the stack

    MOV     r2,#0                           ; Build solicited stack type value
                                            ; and NU_NULL value
    STR     r2,[sp, #-4]!                   ; Place it on the top of the stack
    MOV     r4,r1                           ; Save thread to schedule

    ; Setup a pointer to the thread control block and clear it.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    STR     r2,[r1, #0]                     ; Set current thread pointer to
                                            ; NU_NULL

    ; Check to see if a time slice is active.

    LDR     r3,Slice_State                  ; Pickup time slice state address
    LDR     r1,[r3, #0]                     ; Pickup time slice state flag
    CMP     r1,#0                           ; Compare with active value
    BNE     TCT_No_Stop_TS_4                ; If non-active, don't disable

    ; Insure that the next time the task runs it gets a fresh time slice
    ; by setting the state to NOT_ACTIVE.

    LDR     r1,[r0, #TC_TIME_SLICE]         ; Pickup original time slice
    MOV     r2,#1                           ; Build disable value
    STR     r2,[r3, #0]                     ; Disable time slice
    STR     r1,[r0, #TC_CUR_TIME_SLICE]     ; Reset current time slice

TCT_No_Stop_TS_4

    ; Save off the current stack pointer in the control block.

    STR     sp,[r0, #TC_STACK_POINTER]

    ; Switch to the system stack.

    LDR     r1,System_Stack                 ; Pickup address of stack pointer
    LDR     r2,System_Limit                 ; Pickup address of stack limit ptr
    LDR     sp,[r1, #0]                     ; Switch to system stack
    LDR     r10,[r2, #0]                    ; Setup system stack limit

    ; Transfer control to the specified thread directly.

    LDR     r2,Int_Level                    ; Pickup address of interrupt level
    MRS     r1,CPSR                         ; Pickup current CPSR
    LDR     r3,[r2, #0]                     ; Pickup interrupt lockout level
    BIC     r1,r1,#LOCK_MSK                 ; Clear lockout bits
    ORR     r1,r1,r3                        ; Build new interrupt lockout
    MOV     r0,r4                           ; Indicate thread to schedule
    MSR     CPSR_cxsf,r1                    ; Setup CPSR appropriately
    ORR     r1,r1,#LOCKOUT                  ; Build lockout value again
    MSR     CPSR_cxsf,r1                    ; Lockout interrupts again
    B       TCT_Control_To_Thread           ; Schedule the thread indirectly

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Interrupt_Context_Save
;*
;* DESCRIPTION
;*
;*      This function saves the interrupted thread's context.  Nested
;*      interrupts are also supported.  If a task or HISR thread was
;*      interrupted, the stack pointer is switched to the system stack
;*      after the context is saved.
;*
;* CALLED BY
;*
;*      Application ISRs                    Assembly language ISRs
;*      INT_Interrupt_Shell                 Interrupt handler shell
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      vector                              Interrupt's vector number
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*      D. Driscoll     01-04-2002      Released version 1.13.3.
;*                                      Updated to handle nested /
;*                                      prioritized IRQs
;************************************************************************

    EXPORT  TCT_Interrupt_Context_Save
TCT_Interrupt_Context_Save

    ; Determine if this is a nested interrupt.

    LDR     r1,Int_Count                    ; Pickup address of interrupt count
    LDR     r2,[r1, #0]                     ; Pickup interrupt counter
    ADD     r2,r2,#1                        ; Add 1 to interrupt counter
    STR     r2,[r1, #0]                     ; Store new interrupt counter value
    CMP     r2,#1                           ; Is it nested?
    BNE     TCT_Nested_Save                 ; No

    ; Check for a special nested case.  A special nested case occurs when
    ; a second interrupt goes off before Int_Count is incremented.

    MRS     r1,SPSR                         ; Pickup value of SPSR
    AND     r1,r1,#MODE_MASK                ; Clear all but mode from SPSR
    TEQ     r1,#IRQ_MODE                    ; Check to see if in IRQ Mode
    BNE     TCT_Normal_Save                 ; If not equal, a normal save is done

    ; Set special nested flag

    MOV     r1,#1
    LDR     r2,Special_Nested
    STR     r1,[r2]

    ; Check if a thread is executing

    LDR     r1,Current_Thread
    LDR     r1,[r1, #0]
    CMP     r1,#0
    BNE     TCT_Thread_Save

TCT_Nested_Save

    ; 1.  Save the necessary exception registers into r1-r3

    MOV     r1,sp                           ; Put the exception sp into r1
    MOV     r2,lr                           ; Move the return address for the caller
                                            ;  of this function into r2
    MRS     r3,spsr                         ; Put the exception spsr into r3

    ; 2.  Adjust the exception stack pointer for future exceptions

    ADD     sp,sp,#24                       ; sp will point to enable reg value when done

    ; 3.  Switch CPU modes to save context on system stack

    MRS     r5,CPSR                         ; Pickup the current CPSR
    BIC     r5,r5,#MODE_MASK                ; Clear the mode bits

    IF NU_SUPERV_USER_MODE
        ORR     r5,r5,#SYS_MODE             ; Change to system mode (SYS)
    ELSE
        ORR     r5,r5,#SUP_MODE             ; Change to supervisor mode (SVD)
    ENDIF

    MSR     CPSR_cxsf,r5                    ; Switch modes (IRQ->SVC)

    ; 4.  Store the SVC sp into r5 so the sp can be saved as is.

    MOV     r5,sp

    ; 5.  Save the exception return address on the stack (PC).

    STMDB   r5!,{r4}

    ; 6.  Save r6-r14 on stack

    STMDB   r5!,{r6-r14}

    ; 7.  Switch back to using sp now that the original sp has been saved.

    MOV     sp,r5

    ; 8.  Get r0-r5 off exception stack and save on system stack

    LDMIA   r1!,{r5-r9,r11}
    STMDB   sp!,{r5-r9,r11}

    ; 9. Save the SPSR on the system stack (CPSR)

    STMDB   sp!,{r3}

    ; Return to calling ISR

    BX      r2

TCT_Normal_Save

    ; Determine if a thread was interrupted.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r1,[r1, #0]                     ; Pickup the current thread pointer
    CMP     r1,#0                           ; Is it NU_NULL?
    BEQ     TCT_Idle_Context_Save           ; If no, no real save is necessary

TCT_Thread_Save

    ; Yes, a thread was interrupted.  Save complete context on the
    ; thread's stack.

    ; 1.  Save the necessary exception registers into r1-r3

    MOV     r1,sp                           ; Put the exception sp into r1
    MOV     r2,lr                           ; Move the return address for the caller
                                            ;  of this function into r2
    MRS     r3,spsr                         ; Put the exception spsr into r3

    ; 2.  Adjust the exception stack pointer for future exceptions

    ADD     sp,sp,#24                       ; sp will point to enable reg value when done

    ; 3.  Switch CPU modes to save context on system stack

    MRS     r5,CPSR                         ; Pickup the current CPSR
    BIC     r5,r5,#MODE_MASK                ; Clear the mode bits

    IF NU_SUPERV_USER_MODE
        ORR     r5,r5,#SYS_MODE             ; Change to system mode (SYS)
    ELSE
        ORR     r5,r5,#SUP_MODE             ; Change to supervisor mode (SVC)
    ENDIF

    MSR     CPSR_cxsf,r5                    ; Switch modes

    ; 4.  Store the SVC sp into r5 so the sp can be saved as is.

    MOV     r5,sp

    ; 5.  Save the exception return address on the stack (PC).

    STMDB   r5!,{r4}

    ; 6.  Save r6-r14 on stack

    STMDB   r5!,{r6-r14}

    ; 7.  Switch back to using sp now that the original sp has been saved.

    MOV     sp,r5

    ; 8.  Get r0-r5 off exception stack and save on system stack

    LDMIA   r1!,{r5-r9,r11}
    STMDB   sp!,{r5-r9,r11}

    ; 9. Save the SPSR on the system stack (CPSR)

    STMDB   sp!,{r3}

    ; 10. Save stack type to the task stack (1=interrupt stack)

    MOV     r1,#1                           ; Interrupt stack type
    STMDB   sp!,{r1}

    ; Save the thread's stack pointer in the control block.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r3,[r1, #0]                     ; Pickup current thread pointer
    STR     sp,[r3, #TC_STACK_POINTER]      ; Save stack pointer

    ; Determine if a time slice is active.  If so, the remaining time left on
    ; the time slice must be saved in the task's control block.

    LDR     r4,Slice_State                  ; Pickup time slice state address
    LDR     r1,[r4, #0]                     ; Pickup time slice state
    CMP     r1,#0                           ; Determine if time slice active
    BNE     TCT_Save_Exit                   ; If not, skip time slice reset

    ; Pickup the remaining portion of the time slice and save it
    ; in the task's control block.  */

    LDR     r5,Time_Slice                   ; Pickup address of time slice left
    LDR     r5,[r5, #0]                     ; Pickup remaining time slice
    STR     r5,[r3, #TC_CUR_TIME_SLICE]     ; Store remaining time slice
    MOV     r1,#1                           ; Build disable time slice value
    STR     r1,[r4, #0]                     ; Disable time slice

    ; Switch to the system stack.

    LDR     r1,System_Stack                 ; Pickup address of stack pointer
    LDR     r3,System_Limit                 ; Pickup address of stack limit ptr
    LDR     sp,[r1, #0]                     ; Switch to system stack
    LDR     r10,[r3, #0]                    ; Setup system stack limit

TCT_Save_Exit

    ; Return to caller ISR.

    BX      r2

TCT_Idle_Context_Save

    MOV     r2,lr                           ; Save lr in r2
    ADD     sp,sp,#24                       ; Adjust exception sp for future interrupts

    ; Switch CPU mode.

    MRS     r1,CPSR                         ; Pickup current CPSR
    BIC     r1,r1,#MODE_MASK                ; Clear the current mode
    IF NU_SUPERV_USER_MODE
        ORR     r1,r1,#SYS_MODE             ; Prepare to switch to system
                                            ;   mode (SYS)
    ELSE
        ORR     r1,r1,#SUP_MODE             ; Prepare to switch to supervisor
                                            ;   mode (SVC)
    ENDIF
    MSR     CPSR_cxsf,r1                    ; Switch mode

    ; Return to caller ISR

    BX      r2

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Interrupt_Context_Restore
;*
;* DESCRIPTION
;*
;*      This function restores the interrupt context if a nested
;*      interrupt condition is present.  Otherwise, this routine
;*      transfers control to the scheduling function.
;*
;* CALLED BY
;*
;*      Application ISRs                    Assembly language ISRs
;*      INT_Interrupt_Shell                 Interrupt handler shell
;*
;* CALLS
;*
;*      TCT_Schedule                        Thread scheduling function
;*
;* INPUTS
;*
;*      None
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*      D. Driscoll     01-04-2002      Released version 1.13.3.
;*                                      Updated to handle nested /
;*                                      prioritized IRQs
;*
;************************************************************************

    EXPORT  TCT_Interrupt_Context_Restore
TCT_Interrupt_Context_Restore

    ; Decrement and check for nested interrupt conditions.

    LDR     r1,Int_Count                    ; Pickup address of interrupt count
    LDR     r2,[r1, #0]                     ; Pickup interrupt counter
    SUB     r2,r2,#1                        ; Decrement interrupt counter
    STR     r2,[r1, #0]                     ; Store interrupt counter
    CMP     r2,#0
    BNE     TCT_Nested_Restore

    ; Check for special nested case

    LDR     r1,Special_Nested
    LDR     r2,[r1]
    CMP     r2,#1
    BNE     TCT_Not_Nested_Restore

    ; Clear Special nested flag

    MOV     r2,#0
    STR     r2,[r1]

    ; See if a thread was active during special nested situation

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    LDR     r0,[r1, #0]                     ; Pickup current thread pointer
    CMP     r0,#0                           ; Determine if a thread is active
    BEQ     TCT_Nested_Restore              ; If not, just do a nested restore

    ; A thread was active during this special situation
    ; Switch to this thread's stack and adjust the SP below the stack type

    LDR     sp,[r0,#TC_STACK_POINTER]
    ADD     sp,sp,#4

TCT_Nested_Restore

    LDR     r1,[sp], #4                     ; Pickup the saved CPSR

    IF NU_SUPERV_USER_MODE

        ADD     r3,sp,#56                   ; calculate lr start
        LDMIA   r3,{lr}                     ; restore lr in system mode
        MOV     r3,sp                       ; save current stack pointer
        ADD     sp,sp,#64                   ; calculate new stack pointer
        MRS     r2,CPSR                     ; Pickup current CPSR
        BIC     r2,r2,#MODE_MASK            ; Clear mode
        ORR     r2,r2,#SUP_MODE             ; Set to supervisor
        MSR     CPSR_cxsf,r2                ; change to supervisor
        MOV     sp,r3                       ; restore stack pointer

    ENDIF

    MSR     SPSR_cxsf,r1                    ; Place into saved SPSR

    ; Return to the point of interrupt.

    LDMIA   sp,{r0-r15}^

TCT_Not_Nested_Restore

    ; Clear TCD_Current_Thread

    MOV     r2,#0
    LDR     r1,Current_Thread
    STR     r2,[r1]

    ; Reset the system stack pointer.

    LDR     r1,System_Stack                 ; Pickup address of stack pointer
    LDR     r2,System_Limit                 ; Pickup address of stack limit ptr
    LDR     sp,[r1, #0]                     ; Switch to system stack
    LDR     r10,[r2, #0]                    ; Setup system stack limit

    ; Return to scheduling loop

    B       TCT_Schedule

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_Activate_HISR
;*
;* DESCRIPTION
;*
;*      This function activates the specified HISR.  If the HISR is
;*      already activated, the HISR's activation count is simply
;*      incremented.  Otherwise, the HISR is placed on the appropriate
;*      HISR priority list in preparation for execution.
;*
;* CALLED BY
;*
;*      Application LISRs
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      hisr                                Pointer to HISR to activate
;*
;* OUTPUTS
;*
;*      NU_SUCCESS                          Successful completion
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_Activate_HISR
TCT_Activate_HISR

    ; Lockout interrupts.

    STR     r4,[sp, #-4]!                   ; Save r4
    MRS     r4,CPSR                         ; Pickup current CPSR
    ORR     r1,r4,#LOCKOUT                  ; Build interrupt lockout value
    MSR     CPSR_cxsf,r1                    ; Lockout interrupts

    ; Determine if the HISR is already active.

    LDR     r1,[r0,#TC_ACTIVATION_COUNT]    ; Pickup current activation count
    CMP     r1,#0                           ; Is it the first activation?
    BEQ     TCT_First_Activate              ; Yes, place it on the correct list

    ; Increment the activation count.  Make sure that it does not go
    ; to zero.

    ADDS    r1,r1,#1                        ; Increment the activation count
    STR     r1,[r0,#TC_ACTIVATION_COUNT]    ; Store new activation count

    MVNEQ   r1,#0                           ; If counter rolled-over reset
    STREQ   r1,[r0,#TC_ACTIVATION_COUNT]    ; Store all ones count
    B       TCT_Activate_Done               ; Finished with activation

TCT_First_Activate

    ; Set the activation count to 1.

    MOV     r1,#1                           ; Initial activation count
    STR     r1,[r0,#TC_ACTIVATION_COUNT]    ; Store initial activation count

    ; Pickup the HISR's priority.

    ; Determine if there is something in the given priority list.

    LDRB    r1,[r0,#TC_PRIORITY]            ; Pickup priority of HISR
    LDR     r2,HISR_Tails                   ; Pickup tail pointer base
    LDR     r3,[r2,r1,LSL #2]               ; Pickup tail pointer for priority
    CMP     r3,#0                           ; Is this first HISR at priority?
    BEQ     TCT_First_HISR                  ; No, append to end of HISR list

    ; Something is already on this list.  Add after the tail.

    STR     r0,[r3,#TC_ACTIVE_NEXT]         ; Setup the active next pointer
    STR     r0,[r2,r1,LSL #2]               ; Setup the tail pointer
    B       TCT_Activate_Done               ; Finished with activate processing

TCT_First_HISR

    ; Nothing is on this list.

    LDR     r3,HISR_Heads                   ; Pickup address of head pointers
    STR     r0,[r2,r1,LSL #2]               ; Set tail pointer to this HISR
    STR     r0,[r3,r1,LSL #2]               ; Set head pointer to this HISR

    ; Determine the highest priority HISR.

    LDR     r1,[r3,#0]                      ; Pickup priority 0 head pointer
    LDR     r0,Execute_HISR                 ; Build address to execute HISR ptr
    CMP     r1,#0                           ; Is priority 0 active?
    LDREQ   r1,[r3,#4]                      ; If not, pickup priority 1 head
    CMPEQ   r1,#0                           ; Is priority 1 active?
    LDREQ   r1,[r3,#8]                      ; Else, must be priority 2 active
    STR     r1,[r0,#0]                      ; Store which ever priority is the
                                            ;   active one

TCT_Activate_Done

    MSR     CPSR_cxsf,r4                    ; Restore interrupt lockout
    LDR     r4,[sp], #4                     ; Restore corrupted r4
    MOV     r0,#0                           ; Always return NU_SUCCESS

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TCT_HISR_Shell
;*
;* DESCRIPTION
;*
;*      This function is the execution shell of each and every HISR.  If
;*      the HISR has completed its processing, this shell routine exits
;*      back to the system.  Otherwise, it sequentially calls the HISR
;*      routine until the activation count goes to zero.
;*
;* CALLED BY
;*
;*      HISR Scheduling
;*
;* CALLS
;*
;*      hisr -> tc_entry                    Actual entry function of HISR
;*
;* INPUTS
;*
;*      None
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TCT_HISR_Shell
TCT_HISR_Shell

    ; Point at the HISR.

    LDR     r0,Current_Thread               ; Build address of thread pointer
    LDR     r5,[r0, #0]                     ; Pickup control block pointer

TCT_HISR_Loop

    IF NU_SUPERV_USER_MODE

        ; We need to modify the current supervisor user
        ; mode counter to properly keep up with what mode
        ; we are in.  Since this is the entry point
        ; superv_user_count should always be at 0.

        ; get a local copy of the mode counter

        LDR     r1,[r5, #TC_HISR_SU_MODE]   ; read tc_su_mode from current thread

        ; important not to make superv_user_count less than zero

        CMP     r1,#0                       ; check to see if tc_su_mode is 0
                                            ; if it is, something has gone amiss
        BNE     TCT_HISR_Decrement_SU

        ; change to user mode

        STR     lr,[sp, #4]!                ; Store lr on the stack
        MOV     a1,#5                       ; Build NU_NOT_IN_SUPERVISOR_MODE
                                            ; code
        LDR     a4,=ERC_System_Error        ; Call system error handler.  Note:
        BX      a4                          ;   control is not returned!
                                            ;   Examine stack to find return

TCT_HISR_Decrement_SU

        ; decrement tc_su_mode

        SUB     r1,r1,#1

        ; Check if superv_user_count is 0

        CMP     r1,#0
        BNE     TCT_HISR_User_Switch_Exit

        ; change to user mode

        MRS     r2,CPSR                     ; get CPSR
        BIC     r2,r2,#MODE_MASK            ; clear mode bits
        ORR     r2,r2,#USR_MODE             ; set USER mode

        IF NU_MMU_MODE                      ; Do this before the mode switch,
                                            ; user mode is not allowed
                                            ; to use the co-processor
            MMU_ENABLE                      ; Turn MMU on for protection
        ENDIF                               ; End of NU_MMU_MODE

        MSR     CPSR_csxf,r2                ; write new CPSR, setting USER mode

TCT_HISR_User_Switch_Exit

        ; Write the value back to the current thread.

        STR     r1,[r5,#TC_HISR_SU_MODE]    ; write back the mode switch counter.

    ENDIF                                   ; End of NU_SUPERV_USER_MODE

    ; Call the HISR's entry routine.

    IF ARM

        MOV     lr,pc                       ; Setup return value
        LDR     pc,[r5,#TC_HISR_ENTRY]      ; Call HISR entry function
    ELSE
        LDR     r4,[r5,#TC_HISR_ENTRY]      ; Get HISR entry function
        TST     r4,#1                       ; See if calling Thumb or ARM
        BNE     Thumb
        MOV     lr,pc                       ; Setup return value
        BX      r4
        B       ARMCODE
Thumb
        LDR     lr, =ThumbAfterHisr+1
        BX      r4
        CODE16
ThumbAfterHisr
        LDR     r1,=ARMCODE
        BX      r1
        CODE32
    ENDIF

ARMCODE

    IF NU_SUPERV_USER_MODE

        ; When we return from the HISR we need to put ourselves
        ; back in supervisor mode.

        ; get a local copy of the mode counter

        LDR     r1,[r5, #TC_HISR_SU_MODE]   ; read tc_su_mode from current thread

        ; For consistency increment the supervisor user mode counter.

        ADD     r1,r1,#1                    ; increment tc_su_mode

        ; check to see if the switch is necessary, if superv_user_count is
        ; is greater than 1 we are already in supervisor mode so only switch
        ; when it is equal to 1

        CMP     r1,#1
        BNE     TCT_HISR_System_Switch_Exit

        ; switch to supervisor mode

        SWI     0

TCT_HISR_System_Switch_Exit

        ; Write the value back to the current thread

        STR     r1,[r5,#TC_HISR_SU_MODE]    ; write back the mode switch counter.

    ENDIF

    ; Lockout interrupts.

    MRS     r1,CPSR                         ; Pickup current CPSR
    ORR     r1,r1,#LOCKOUT                  ; Build interrupt lockout
    MSR     CPSR_cxsf,r1                    ; Lockout interrupts

    ; On return, decrement the activation count and check to see if
    ; it is 0.  Once it reaches 0, the HISR should be made inactive.

    LDR     r0,[r5, #TC_ACTIVATION_COUNT]   ; Pickup current activation count
    SUBS    r0,r0,#1                        ; Subtract and set condition codes
    STR     r0,[r5, #TC_ACTIVATION_COUNT]   ; Store new activation count
    BEQ     TCT_HISR_Finished               ; Finished processing HISR

    ; Restore interrupts.

    LDR     r2,Int_Level                    ; Pickup address of interrupt level
    MRS     r1,CPSR                         ; Pickup current CPSR
    LDR     r3,[r2, #0]                     ; Pickup interrupt lockout level
    BIC     r1,r1,#LOCK_MSK                 ; Clear lockout bits
    ORR     r1,r1,r3                        ; Build new interrupt lockout
    MSR     CPSR_cxsf,r1                    ; Setup CPSR appropriately
    B       TCT_HISR_Loop                   ; Return to HISR loop

TCT_HISR_Finished

    ; At this point, the HISR needs to be made inactive.

    ; Determine if this is the only HISR on the given priority list.

    LDR     lr,HISR_Tails                   ; Pickup tail pointers address
    LDRB    r3,[r5,#TC_PRIORITY]            ; Pickup priority
    LDR     r6,[lr,r3,LSL #2]               ; Pickup this priority tail pointer
    LDR     r2,Execute_HISR                 ; Build address to execute HISR ptr
    MOV     r12,#0                          ; Clear r12
    LDR     r1,HISR_Heads                   ; Pickup head pointers address
    CMP     r6,r5                           ; Is this priority tail the same as
                                            ; the current HISR?
    BNE     TCT_More_HISRs                  ; If not, more HISRs at this
                                            ; priority

    ; The only HISR on the list.  Clean up the list and check for the
    ; highest priority HISR.

    STR     r12,[r1,r3,LSL #2]              ; Set head pointer to NU_NULL
    STR     r12,[lr,r3,LSL #2]              ; Set tail pointer to NU_NULL

    ; Determine the highest priority HISR.

    LDR     r3,[r1,#0]                      ; Pickup priority 0 head pointer
    CMP     r3,#0                           ; Is there an HISR active?
    LDREQ   r3,[r1,#4]                      ; If not, pickup priority 1 pointer
    CMPEQ   r3,#0                           ; Is there an HISR active?
    LDREQ   r3,[r1,#8]                      ; If not, pickup priority 2 pointer
    STR     r3,[r2,#0]                      ; Setup execute HISR pointer
    B       TCT_HISR_Exit                   ; Exit HISR processing

TCT_More_HISRs

    ; Move the head pointer to the next HISR in the list.
    ; Also set the TCD_Execute_HISR pointer.

    LDR     lr,[r5,#TC_ACTIVE_NEXT]         ; Pickup next HISR to activate

    STR     lr,[r1,r3,LSL #2]               ; Setup new head pointer
    STR     lr,[r2, #0]                     ; Setup execute HISR pointer

TCT_HISR_Exit

    ; Build fake return to the top of this loop.  The next time the HISR
    ; is activated, it will return to the top of this function.

    LDR     lr,HISR_Shell                   ; Pickup address of shell entry
    STMDB   sp!,{r4-r12,lr}                 ; Save minimal context of thread on
                                            ; the current stack
    MOV     r2,#0                           ; Build solicited stack type value
                                            ; and NU_NULL value
    IF THUMB
        STR     r2,[sp, #-4]!               ; Save state mask
    ENDIF

    STR     r2,[sp, #-4]!                   ; Place it on the top of the stack

    ; Clear the current thread control block.

    LDR     r1,Current_Thread               ; Pickup current thread ptr address
    STR     r2,[r1, #0]                     ; Set current thread pointer to
                                            ;   NU_NULL

    ; Save off the current stack pointer in the control block.

    STR     sp,[r5, #TC_STACK_POINTER]      ; Save the thread's stack pointer

    ; Switch to the system stack.

    LDR     r1,System_Stack                 ; Pickup address of stack pointer
    LDR     r2,System_Limit                 ; Pickup address of stack limit ptr
    LDR     sp,[r1, #0]                     ; Switch to system stack
    LDR     r10,[r2, #0]                    ; Setup system stack limit

    ; Transfer control to the main scheduling loop.

    B       TCT_Schedule                    ; Return to main scheduling loop

    END








