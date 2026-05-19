/* Linker script for STM32F446RETx Microcontroller */
ENTRY(Reset_Handler)

_estack = 0x20020000;    /* End of 128KB RAM */

MEMORY
{
  RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
}

SECTIONS
{
  /* The startup code goes first into FLASH */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector)) /* Startup vector table */
    . = ALIGN(4);
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections */
    . = ALIGN(4);
  } >FLASH

  /* Used by the startup to initialize data */
  _sidata = LOADADDR(.data);

  /* Initialized data sections go into RAM, loaded from FLASH */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* Create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */
    . = ALIGN(4);
    _edata = .;        /* Define a global symbol at data end */
  } >RAM AT > FLASH

  /* Uninitialized data section */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;         /* Define a global symbol at bss start */
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;         /* Define a global symbol at bss end */
  } >RAM
}