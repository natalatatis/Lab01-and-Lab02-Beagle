.syntax unified
.cpu cortex-a8
.fpu vfpv3
.code 32

.global _start
.global PUT32
.global GET32

.extern main
.extern __bss_start__
.extern __bss_end__
.extern _stack_top

.section .text
.align 5

_start:
    ldr pc, reset_addr
    ldr pc, undef_addr
    ldr pc, swi_addr
    ldr pc, pre_addr
    ldr pc, data_addr
    ldr pc, unused_addr
    ldr pc, irq_addr
    ldr pc, fiq_addr

reset_addr:  .word reset_handler
undef_addr:  .word undef_handler
swi_addr:    .word hang
pre_addr:    .word hang
data_addr:   .word data_handler
unused_addr: .word hang
irq_addr:    .word hang
fiq_addr:    .word hang

reset_handler:
    /* 1. Disable Interrupts */
    cpsid i
    
    /* 2. Set Vector Base Address Register (VBAR) */
    /* This tells the CPU: "My exception table is at _start" */
    /* This prevents the silent reboot you are seeing */
    ldr r0, =_start
    mcr p15, 0, r0, c12, c0, 0
    isb

    /* 3. Set and Align stack pointer (8-byte alignment for AAPCS) */
    ldr sp, =_stack_top
    bic sp, sp, #0x7

    /* 4. Enable FPU (VFP) Hardware */
    mrc p15, 0, r0, c1, c0, 2
    orr r0, r0, #0x00F00000      @ Access to CP10 & CP11
    mcr p15, 0, r0, c1, c0, 2
    isb
    
    mov r0, #0x40000000          @ Set EN bit (30)
    vmsr fpexc, r0

    /* 5. Clear .bss section safely */
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    mov r2, #0
    b bss_check

bss_loop:
    str r2, [r0], #4

bss_check:
    cmp r0, r1
    blt bss_loop

    /* 6. Jump to C main */
    bl main
    
hang: b hang

/* --- Panic Debug Handlers --- */
/* If the system crashes, these will print 'D' or 'U' and stop */
data_handler:
    mov r0, #'D'   @ Data Abort (Bad memory access)
    b panic_print

undef_handler:
    mov r0, #'U'   @ Undefined Instruction (Stack corruption)
    b panic_print

panic_print:
    ldr r1, =0x44E09000  @ UART0 Base
1:  ldr r2, [r1, #0x14]  @ UART_LSR
    tst r2, #0x20        @ Check THRE (bit 5)
    beq 1b
    str r0, [r1]         @ Write char to THR
    b hang

/* Helpers */
PUT32:
    str r1, [r0]
    bx lr

GET32:
    ldr r0, [r0]
    bx lr