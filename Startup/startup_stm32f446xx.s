.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global g_pfnVectors
.global Reset_Handler

/* Loop until the end of BSS section */
.section .text.Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
  /* Set stack pointer */
  ldr   sp, =_estack    

  /* Copy the data segment initializers from flash to SRAM */
  movs  r1, #0
  bl    LoopCopyDataInit

CopyDataInit:
  ldr   r3, =_sidata
  ldr   r3, [r3, r1]
  str   r3, [r0, r1]
  adds  r1, r1, #4

LoopCopyDataInit:
  ldr   r0, =_sdata
  ldr   r2, =_edata
  adds  r3, r0, r1
  cmp   r3, r2
  bcc   CopyDataInit

  /* Zero fill the bss segment */
  ldr   r2, =_sbss
  ldr   r4, =_ebss
  movs  r3, #0
  b     LoopFillZerobss

FillZerobss:
  str  r3, [r2]
  adds r2, r2, #4

LoopFillZerobss:
  cmp  r2, r4
  bcc  FillZerobss

  /* Call the application's entry point */
  bl    main
  bx    lr
.size Reset_Handler, .-Reset_Handler

/* Vector Table Initialization */
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object

g_pfnVectors:
  .word _estack
  .word Reset_Handler
  /* Minimal stub table: Add peripheral handlers here as your architecture expands */
  .word NMI_Handler
  .word HardFault_Handler

  