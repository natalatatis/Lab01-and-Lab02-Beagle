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
    b reset_handler
    b . @ Undefined Instruction
    b . @ Software Interrupt
    b . @ Prefetch Abort
    b . @ Data Abort
    b . @ Reserved
    b . @ IRQ
    b . @ FIQ

reset_handler:
    /* 1. Disable Interrupts */
    cpsid i

    /* 2. Set stack pointer */
    ldr sp, =_stack_top

    /* 3. Enable FPU (VFP) Hardware */
    /* This is required for __aeabi_ floating point functions to work */
    mrc p15, 0, r0, c1, c0, 2    @ Read CP Access Control Register (CPACR)
    orr r0, r0, #0x00F00000      @ Enable full access to CP10 and CP11
    mcr p15, 0, r0, c1, c0, 2    @ Write CPACR
    isb                          @ Instruction Synchronization Barrier
    
    mov r0, #0x40000000          @ Set EN bit (bit 30)
    vmsr fpexc, r0               @ Write to FP Exception Control register

    /* 4. Clear .bss section */
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    mov r2, #0

clear_bss:
    cmp r0, r1
    strlt r2, [r0], #4
    blt clear_bss

    /* 5. Jump to C main */
    bl main

hang:
    b hang

/* --- Memory access helpers --- */
PUT32:
    str r1, [r0]
    bx lr

GET32:
    ldr r0, [r0]
    bx lr