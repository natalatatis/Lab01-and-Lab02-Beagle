.syntax unified
.cpu cortex-a8
.code 32

.global _start
.global PUT32
.global GET32

.extern main
.extern _stack_top

.section .text
_start:
    ldr sp, =_stack_top
    bl main

hang:
    b hang

PUT32:
    str r1, [r0]
    bx lr

GET32:
    ldr r0, [r0]
    bx lr