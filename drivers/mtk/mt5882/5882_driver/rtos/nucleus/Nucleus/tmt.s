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
;*      tmt.s                         Nucleus PLUS\Integrator\ADS 1.14.8
;*
;* COMPONENT
;*
;*      TM - Timer Management
;*
;* DESCRIPTION
;*
;*      This file contains the target dependent routines of the timer
;*      management component.
;*
;* FUNCTIONS
;*
;*      TMT_Set_Clock                       Set system clock
;*      TMT_Retrieve_Clock                  Retrieve system clock
;*      TMT_Read_Timer                      Read count-down timer
;*      TMT_Enable_Timer                    Enable count-down timer
;*      TMT_Adjust_Timer                    Adjust count-down timer
;*      TMT_Disable_Timer                   Disable count-down timer
;*      TMT_Retrieve_TS_Task                Retrieve time-sliced task ptr
;*      TMT_Timer_Interrupt                 Process timer interrupt
;*
;************************************************************************
;**************************************
;*   INCLUDE FILE                     *
;**************************************

    INCLUDE asm_defs.inc             ; Assembly definition file

;**************************************
;*   COMPILE-TIME SWITCHES            *
;**************************************

    PRESERVE8
    CODE32

    AREA |C$$code|, CODE, READONLY
|x$codeseg|

;*********************************************
;*   EXTERNAL VARIABLE/FUNCTION DECLARATIONS *
;*********************************************

    ; Define external inner-component global data references.

    IMPORT  TCD_Current_Thread
    IMPORT  TMD_System_Clock
    IMPORT  TMD_Timer
    IMPORT  TMD_Timer_State
    IMPORT  TMD_Time_Slice
    IMPORT  TMD_Time_Slice_Task
    IMPORT  TMD_Time_Slice_State
    IMPORT  TMD_HISR
    IMPORT  TCD_Interrupt_Level
    IMPORT  TCD_Interrupt_Count

    ; Define activate HISR function.

    IMPORT  TCT_Activate_HISR
    IMPORT  TCT_Interrupt_Context_Save
    IMPORT  TCT_Interrupt_Context_Restore
    IMPORT  INT_Unnest_Interrupt

    ; Define various data structure pointers so their addresses can be obtained
    ; in a PC-relative manner.

System_Clock
    DCD     TMD_System_Clock

Timer
    DCD     TMD_Timer

Timer_State
    DCD     TMD_Timer_State

Slice_State
    DCD     TMD_Time_Slice_State

Time_Slice
    DCD     TMD_Time_Slice

Current_Thread
    DCD     TCD_Current_Thread

Slice_Task
    DCD     TMD_Time_Slice_Task

HISR
    DCD     TMD_HISR

Int_Level
    DCD     TCD_Interrupt_Level

;************************************************************************
;*
;* FUNCTION
;*
;*      TMT_Set_Clock
;*
;* DESCRIPTION
;*
;*      This function sets the system clock to the specified value.
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
;*      new_value                           New value for the clock
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

    EXPORT  TMT_Set_Clock
TMT_Set_Clock

    ; Set the system clock to the specified value.

    LDR     r1,System_Clock                 ; Build address of system clock
    STR     r0,[r1,#0]                      ; Store new system clock value

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TMT_Retrieve_Clock
;*
;* DESCRIPTION
;*
;*      This function returns the current value of the system clock.
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
;*      None
;*
;* OUTPUTS
;*
;*      TMD_System_Clock                    Value of system clock
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TMT_Retrieve_Clock
TMT_Retrieve_Clock

    ; Return the current value of the system clock.

    LDR     r0,System_Clock                 ; Build address to system clock
    LDR     r0,[r0,#0]                      ; Pickup system clock contents

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TMT_Read_Timer
;*
;* DESCRIPTION
;*
;*      This function returns the current value of the count-down timer.
;*
;* CALLED BY
;*
;*      TMC_Start_Timer                     Start timer function
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
;*      TMD_Timer                           Value of count-down timer
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      W. Lamie        02-15-1994      Created initial version 1.0
;*      D. Lamie        02-15-1994      Verified version 1.0
;*
;************************************************************************

    EXPORT  TMT_Read_Timer
TMT_Read_Timer

    ; Return the current value of the count-down timer.

    LDR     r0,Timer                        ; Build address to timer
    LDR     r0,[r0,#0]                      ; Pickup timer contents

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TMT_Enable_Timer
;*
;* DESCRIPTION
;*
;*      This function enables the count-down timer with the specified
;*      value.
;*
;* CALLED BY
;*
;*      TMC_Start_Timer                     Start timer function
;*      TMC_Timer_Task                      Timer expiration task
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      time                                New count-down time
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

    EXPORT  TMT_Enable_Timer
TMT_Enable_Timer

    ; Place the new time value into the count-down timer.

    LDR     r1,Timer                        ; Build address of timer
    STR     r0,[r1,#0]                      ; Store new timer value

    ; Indicate that the timer is active.

    MOV     r0,#0                           ; Build TM_ACTIVE value
    LDR     r1,Timer_State                  ; Build address of timer state var
    STR     r0,[r1,#0]                      ; Change the state to active

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TMT_Adjust_Timer
;*
;* DESCRIPTION
;*
;*      This function adjusts the count-down timer with the specified
;*      value, if the new value is less than the current.
;*
;* CALLED BY
;*
;*      None
;*
;* CALLS
;*
;*      None
;*
;* INPUTS
;*
;*      time                                New count-down time.
;*
;* OUTPUTS
;*
;*      None
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      C. Meredith     03-01-1994      Created initial version 1.1
;*      D. Lamie        03-18-1994      Verified version 1.1
;*      C. Meredith     08-27-1994      Corrected bug in new timer
;*                                        adjust routine, resulting in
;*                                        version 1.1a
;*      W. Lamie        08-27-1994      Verified version 1.1a
;*
;************************************************************************

    EXPORT  TMT_Adjust_Timer
TMT_Adjust_Timer

    ; Lockout all interrupts

    MRS     r3,CPSR                         ; Pickup current CPSR
    ORR     r2,r3,#LOCKOUT                  ; Build lockout CPSR
    MSR     CPSR_cxsf,r2                    ; Setup new CPSR interrupt bits

    ; Check for the new value is less than the current time value

    LDR     r1,Timer                        ; Build address to timer var
    LDR     r2,[r1,#0]                      ; read value of the timer
    CMP     r2,r0                           ; Do Timer - time > 0, means
    BLT     TMT_No_Adjust                   ; time < Timer.

    ; adjust the time

    STR     r0,[r1,#0]                      ; load passed in timer value

    ; Restore interrupts

TMT_No_Adjust

    MSR     CPSR_cxsf,r3                    ; Setup new CPSR enable bits

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TMT_Disable_Timer
;*
;* DESCRIPTION
;*
;*      This function disables the count-down timer.
;*
;* CALLED BY
;*
;*      TMC_Start_Timer                     Start timer function
;*      TMC_Timer_Task                      Timer expiration task
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

    EXPORT  TMT_Disable_Timer
TMT_Disable_Timer

    ; Disable the count-down timer.

    MOV     r1,#1                           ; Build TM_NOT_ACTIVE value
    LDR     r0,Timer_State                  ; Build address to timer state var
    STR     r1,[r0,#0]                      ; Change timer state to not active

    ; Return to caller

    BX      lr

;************************************************************************
;*
;* FUNCTION
;*
;*      TMT_Retreive_TS_Timer
;*
;* DESCRIPTION
;*
;*      This function returns the time-sliced task pointer.
;*
;* CALLED BY
;*
;*      TMC_Timer_HISR                      Timer HISR
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
;*      TMD_Time_Slice_Task                 Time sliced task pointer
;*
;* HISTORY
;*
;*         NAME            DATE                    REMARKS
;*
;*      C. Meredith     03-01-1994      Created initial version 1.1
;*      D. Lamie        03-18-1994      Verified version 1.1
;*
;************************************************************************

    EXPORT  TMT_Retrieve_TS_Task
TMT_Retrieve_TS_Task

    ; Read the current TMD_Time_Slice_Task variable and load for
    ; return to caller.

    LDR     r1,Slice_Task                   ; Build address to timer slice var
    LDR     r0,[r1,#0]                      ; Get task pointer to be returned

    ; Return to caller

    BX      lr                              ; Return to caller

;************************************************************************
;*
;* FUNCTION
;*
;*      TMT_Timer_Interrupt
;*
;* DESCRIPTION
;*
;*      This function processes the actual hardware interrupt.
;*      Processing includes updating the system clock and the count-
;*      down timer and the time-slice timer.  If one or both of the
;*      timers expire, the timer HISR is activated.
;*
;* CALLED BY
;*
;*      Interrupt Vector
;*
;* CALLS
;*
;*      TCT_Activate_HISR                   Activate timer HISR
;*      TCT_Interrupt_Context_Save          Save interrupted context
;*      TCT_Interrupt_Context_Restore       Restore interrupted context
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

    EXPORT  TMT_Timer_Interrupt
TMT_Timer_Interrupt

    ; Increment the system clock.

    LDR     r0,System_Clock                 ; Pickup system clock address
    LDR     r1,[r0,#0]                      ; Pickup system clock contents
    ADD     r1,r1,#1                        ; Increment system clock
    STR     r1,[r0,#0]                      ; Store new system clock value

    ; Determine if the count-down timer is active.

    LDR     r1,Timer_State                  ; Build address to timer state flag
    LDR     r0,[r1,#0]                      ; Pickup timer state
    CMP     r0,#0                           ; Is there a timer active?
    BNE     TMT_No_Timer_Active             ; No, skip timer processing

    ; Pickup the value of the timer.

    LDR     r0,Timer                        ; Build timer address
    LDR     r2,[r0,#0]                      ; Pickup the current timer value

    ; Test if the Timer is at 0 and if so skip the decrement

    CMP     r2,#0
    BEQ     TMT_Expired

    ; Decrement the count-down timer.

    SUBS    r2,r2,#1                        ; Decrement the timer value
    STR     r2,[r0,#0]                      ; Store the new timer value

    BNE     TMT_No_Timer_Active             ; Skip over the Set Timer State

    ; If the timer has expired, modify the state to indicate that
    ; it has expired.

TMT_Expired

    MOV     r3,#2                           ; Build expired value
    STR     r3,[r1,#0]                      ; Change the timer state to
                                            ; expired

TMT_No_Timer_Active

    ; Determine if the time-slice timer is active.  Note that the parameters
    ; for the time-slice are controlled by the Thread Control (TC)
    ; component.

    LDR     r0,Slice_State                  ; Build time slice state address
    LDR     r2,[r0,#0]                      ; Pickup time slice state
    CMP     r2,#0                           ; Is there a time slice active?
    BNE     TMT_No_Time_Slice_Active        ; No, skip time slice processing

    ; Decrement the time slice counter.

    LDR     r2,Time_Slice                   ; Build time slice address
    LDR     r3,[r2,#0]                      ; Pickup the time slice value
    SUBS    r3,r3,#1                        ; Decrement the time slice
    STR     r3,[r2,#0]                      ; Store the new time slice value

    ; Has time slice expired?

    BNE     TMT_No_Time_Slice_Active

    ; If the time-slice timer has expired modify the
    ; time-slice state to indicate that it has.

    MOV     r3,#2                           ; Build TM_EXPIRED value
    STR     r3,[r0,#0]                      ; Indicate time slice is expired

    ; Copy the current thread into the time-slice task pointer.

    LDR     r2,Current_Thread               ; Pickup current thread pointer adr
    LDR     r2,[r2,#0]                      ; Pickup current thread pointer
    LDR     r3,Slice_Task                   ; Pickup time slice task pointer ad
    STR     r2,[r3,#0]                      ; Store current thread pointer

    ; Place a minimal time slice into the task's control block.

    MOV     r3,#1                           ; For safety, place a minimal time-
    STR     r3,[r2,#TC_CUR_TIME_SLICE]!     ; slice into the task's control
                                            ; block

TMT_No_Time_Slice_Active

    ; Determine if either of the basic timers have expired.  If so,
    ; activate the timer HISR.

    LDR     r1,[r1,#0]                      ; Pickup timer state
    CMP     r1,#2                           ; Does it indicate expiration?
    LDRNE   r0,[r0,#0]                      ; Pickup time slice state
    CMPNE   r0,#2                           ; Does it indicate expiration?
    BNE     TMT_Interrupt_Exit              ; If not expired, return to point of interrupt

    ; Put interrupt return address in r4

    MOV     r4,lr

    ; Do a complete context save.

    BL      TCT_Interrupt_Context_Save

    ; Activate the HISR timer function.

    LDR     r0,HISR                         ; Build address of timer HISR
    BL      TCT_Activate_HISR               ; Activate timer HISR

    ; Restore interrupt nesting

    MOV     r0,#TIMER_INT_MODE
    BL      INT_Unnest_Interrupt

    ; Restore context and return to scheduler

    B       TCT_Interrupt_Context_Restore

TMT_Interrupt_Exit

    ; Get interrupt enable register off stack frame

    LDR     r1,[sp,#24]

    ; Put interrupt return address in enable register's place

    STR     lr,[sp,#24]

    ; Store enable register on top of stack

    STMDB   sp!,{r1}

    ; Restore interrupt nesting

    MOV     r0,#TIMER_INT_MODE
    BL      INT_Unnest_Interrupt

    ; Return to point of interrupt

    LDMIA   sp!,{r0-r5,pc}^

    END








