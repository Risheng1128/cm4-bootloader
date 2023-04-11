/**
 * @file   : link.ld
 * @brief  : Linker script for NUCLEO-F303ZE Board embedding STM32F303ZETx Device
 *           - 512Kbytes FLASH
 *           - 16Kbytes CCMRAM (No used)
 *           - 64Kbytes RAM
 */

/* entry point */
ENTRY(Reset_Handler)

/* highest address of the user mode stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

_Min_Heap_Size = 0x200; /* required amount of heap */
_Min_Stack_Size = 0x400; /* required amount of stack */

/* memory definition */
MEMORY
{
    RAM (xrw): ORIGIN = 0x20000000, LENGTH = 64K
    FLASH (rx): ORIGIN = 0x8004000, LENGTH = 512K
}

/* sections */
SECTIONS
{
    /* The startup code into "FLASH" Rom type memory */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Startup code */
        . = ALIGN(4);
    } >FLASH

    /* The program code and other data into "FLASH" Rom type memory */
    .text :
    {
        . = ALIGN(4);
        *(.text)      /* .text sections (code) */
        . = ALIGN(4);
        _etext = .;   /* define a global symbols at end of code */
    } >FLASH

    /* Used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    /* Initialized data sections into "RAM" Ram type memory */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;   /* create a global symbol at data start */
        *(.data)      /* .data sections */
        . = ALIGN(4);
        _edata = .;   /* define a global symbol at data end */
    } >RAM AT> FLASH

    /* Uninitialized data section into "RAM" Ram type memory */
    . = ALIGN(4);
    .bss :
    {
        _sbss = .; /* define a global symbol at bss start */
        __bss_start__ = _sbss;
        *(.bss)
        . = ALIGN(4);
        _ebss = .; /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } >RAM

    /* User_heap_stack section, used to check that there is enough "RAM" */
    ._user_heap_stack :
    {
        . = ALIGN(8);
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        . = ALIGN(8);
    } >RAM
}
