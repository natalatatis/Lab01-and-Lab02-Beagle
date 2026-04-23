ENTRY(_start)

SECTIONS
{
    . = 0x82000000;

    .text : {
        KEEP(*(.text))
        *(.text*)
    }

    .rodata : {
        . = ALIGN(4);
        *(.rodata*)
    }

    .data : {
        . = ALIGN(4);
        *(.data*)
    }

    .bss : {
        . = ALIGN(8);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        __bss_end__ = .;
    }

    /* RESERVED STACK AREA 
       We move the location counter far ahead, then define the stack.
       This creates a "hole" in memory that nothing else can use.
    */
    . = 0x84000000; 
    
    /* Reserve 0x2000 (8KB) for the system stack */
    . = . + 0x2000;
    . = ALIGN(8);
    _stack_top = .;  /* SP starts here and grows DOWN towards 0x84000000 */

    /* Reserve 0x1000 (4KB) for IRQ stack if needed later */
    . = . + 0x1000;
    . = ALIGN(8);
    __irq_stack_top = .;
}