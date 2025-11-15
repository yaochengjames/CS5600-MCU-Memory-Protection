.syntax unified
    .cpu cortex-m4
    .fpu fpv4-sp-d16
    .thumb

    .extern main
    .extern _estack
    .extern _sidata
    .extern _sdata
    .extern _edata
    .extern _sbss
    .extern _ebss

    .section .isr_vector,"a",%progbits
    .align  2
    .globl  g_pfnVectors
g_pfnVectors:
    .word   _estack
    .word   Reset_Handler
    .word   NMI_Handler
    .word   HardFault_Handler
    .word   MemManage_Handler
    .word   BusFault_Handler
    .word   UsageFault_Handler
    .word   0
    .word   0
    .word   0
    .word   0
    .word   vPortSVCHandler
    .word   DebugMon_Handler
    .word   0
    .word   xPortPendSVHandler
    .word   xPortSysTickHandler

    .thumb_func
Default_Handler:
1:  b 1b

    .weak NMI_Handler
    .thumb_set NMI_Handler,Default_Handler
    
    .weak HardFault_Handler
    .thumb_set HardFault_Handler,Default_Handler
    
    .weak MemManage_Handler
    .thumb_set MemManage_Handler,Default_Handler
    
    .weak BusFault_Handler
    .thumb_set BusFault_Handler,Default_Handler
    
    .weak UsageFault_Handler
    .thumb_set UsageFault_Handler,Default_Handler

    .weak DebugMon_Handler
    .thumb_set DebugMon_Handler,Default_Handler

    .text
    .thumb
    .align 2
    .globl  Reset_Handler
    .type   Reset_Handler,%function
Reset_Handler:
    ldr   r0, =_estack
    msr   msp, r0

    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
CopyData:
    cmp r1, r2
    ittt lt
    ldrlt r3, [r0], #4
    strlt r3, [r1], #4
    blt CopyData

    ldr r1, =_sbss
    ldr r2, =_ebss
    movs r3, #0
ZeroBss:
    cmp r1, r2
    itt lt
    strlt r3, [r1], #4
    blt ZeroBss

    bl main
    b .
