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
  .word _estack                  /* 0x00: Top of Stack */
  .word Reset_Handler            /* 0x04: Reset Handler */
  .word NMI_Handler              /* 0x08: NMI Handler */
  .word HardFault_Handler        /* 0x0C: Hard Fault Handler */
  .word MemManage_Handler        /* 0x10: MPU Fault Handler */
  .word BusFault_Handler         /* 0x14: Bus Fault Handler */
  .word UsageFault_Handler       /* 0x18: Usage Fault Handler */
  .word 0                        /* 0x1C: Reserved */
  .word 0                        /* 0x20: Reserved */
  .word 0                        /* 0x24: Reserved */
  .word 0                        /* 0x28: Reserved */
  .word SVC_Handler              /* 0x2C: SVCall Handler */
  .word DebugMon_Handler         /* 0x30: Debug Monitor Handler */
  .word 0                        /* 0x34: Reserved */
  .word PendSV_Handler           /* 0x38: PendSV Handler */
  .word SysTick_Handler          /* 0x3C: SysTick Handler (Slot 15 / 16th word) */

  /* Standard peripheral interrupts would append right here down the line */

.size g_pfnVectors, .-g_pfnVectors

/* Define default weak stubs for core exceptions. 
   If your C code provides a real function matching these names,
   the linker will automatically route the vector to your C code instead! */
.section .text.Default_Handler,"ax",%progbits
Default_Handler:
  b Default_Handler
  .size Default_Handler, .-Default_Handler

  .weak NMI_Handler
  .thumb_set NMI_Handler, Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler, Default_Handler

  .weak MemManage_Handler
  .thumb_set MemManage_Handler, Default_Handler

  .weak BusFault_Handler
  .thumb_set BusFault_Handler, Default_Handler

  .weak UsageFault_Handler
  .thumb_set UsageFault_Handler, Default_Handler

  .weak SVC_Handler
  .thumb_set SVC_Handler, Default_Handler

  .weak DebugMon_Handler
  .thumb_set DebugMon_Handler, Default_Handler

  .weak PendSV_Handler
  .thumb_set PendSV_Handler, Default_Handler

  .weak SysTick_Handler
  .thumb_set SysTick_Handler, Default_Handler