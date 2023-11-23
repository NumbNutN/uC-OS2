.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb
.section .text.os_cpu_a,"x"

.equ CONTROL_SPSEL_SET ,0x02
.equ ICSR_PENDSVSET ,0x10000000
.equ SCB_ICSR ,0xE000ED04

.global CPU_SR_STORE

//从R0加载新的BASEPRI并把原本的BASEPRI返回
CPU_SR_STORE:
    MRS R1,BASEPRI
    MSR BASEPRI,R0
    MOV R0,R1
    BX LR


    //AS treat all undefined symbol as external
.extern OSTaskSwHook
.extern OSRunning
.global OSStartHighRdy

OSStartHighRdy:
    LDR R0,=OSTaskSwHook
    BLX R0
    //Set OSRunning
    LDR R0,=OSRunning
    MOV R1,#1
    STR R1,[R0]
    //Get High Ready Task's StackPointer
    LDR R0,=OSTCBHighRdy
    LDR R0,[R0]
    //Get the stack top pointer
    LDR R1,[R0]
    /* first load register s16-s31 */
    VLDMIA R1!,{S16-S31}
    /* second load register r4-r11 */
    LDMFD R1!,{R4-R11}
    //Skip the EXC_RETURN
    LDMFD R1!,{R0}
    
    /* R4 for SP and R5 for FPSCR */
    STMFD SP!,{R4-R5}
    MOV R4,R1
    LDMFD R4!,{R0-R3,R12,LR}
    //PC in R2 && xPSR in R3
    LDMFD R4!,{R2,R3}
    /* finally load register s0-s15 and fpscr */
    VLDMIA R4!,{S0-S15}
    LDR R5,[R4],#4
    VMSR FPSCR,R5
    /* skip odd address */
    SUB R4,R4,#4

    MOV R1,R4
    LDMFD SP!,{R4-R5}
    //MSR CPSR,R3       Auctually We could not do this and it makes no sense
    //set the new PSP
    MSR PSP,R1
    //Now the Stack Pointer should be switch to the PSP cause we are going to the first task in thread mode
    LDR R3,=CONTROL_SPSEL_SET
    MRS R1,CONTROL
    ORR R1,R1,R3
    MSR CONTROL,R1
    //When changing the stack pointer, software must use an ISB instruction immediately after 
    //    the MSR instruction. This ensures that instructions after the ISB execute using the new 
    //    stack pointer. See ISB on page 173
    ISB
    BX R2

.global OSIntCtxSw
.global OSCtxSw
OSCtxSw:
OSIntCtxSw:
    LDR R1,=SCB_ICSR
    LDR R0,[R1]
    ORR R0,R0,ICSR_PENDSVSET
    STR R0,[R1]
    BX LR

.extern CPUSR_CTX

.global PendSV_Handler
.type PendSV_Handler, %function
PendSV_Handler:
    STMFD SP!,{LR}
    //The OSctxSw function is register as the pendSV exception handler
    //Notice:Interrupts are disabled during OSCtxSw()
    MOV R0,#0x10
    BL CPU_SR_STORE
    LDR R1,=CPUSR_CTX
    STR R0,[R1]
    //As the handler use MSP in default,we should find the psp to save the rest of registers in proper stack
    MRS R0,PSP
    //As the hardware doesn't want to point the PSP to proper address,we should fix it manually
    //SUB R0,R0,#0x20
    //Save the rest registers ,also included EXC_RETURN
    STMFD R0!,{R4-R11,LR}
    /* at last save registers S16-S31 */
    VSTMDB R0!,{S16-S31}
    //Save the new tasks stack pointer to current task TCB
    LDR R1,=OSTCBCur
    LDR R1,[R1]
    STR R0,[R1]
    //Cll user definable OSTaskSwHook()
    LDR R0,=OSTaskSwHook
    @ ORR R0,R0,#1
    BLX R0
    //Make the pointer to the current OS_TCB point to the OS_TCB of the task being resumed
    LDR R0,=OSTCBCur
    LDR R1,=OSTCBHighRdy
    LDR R1,[R1]
    STR R1,[R0]

    LDR R0,=OSPrioCur
    LDR R1,=OSPrioHighRdy
    LDRB R1,[R1]
    STRB R1,[R0]

    //Get the stack pointer of the task to resume
    LDR R0,=OSTCBCur
    LDR R0,[R0]
    LDR R0,[R0]
    /* Restore registers S16-S31 */
    VLDMIA R0!,{S16-S31}
    //Restore the registers R4-R11
    LDMFD R0!,{R4-R11,LR}
    MSR PSP,R0

    //@updare:2023-5-9 Restore BASEPRI
    LDR R0,=CPUSR_CTX
    LDR R0,[R0]
    BL CPU_SR_STORE
    LDMFD SP!,{PC}




    

