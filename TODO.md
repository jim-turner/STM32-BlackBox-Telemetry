# Project Progress Tracking

## Milestone 1: Hardware Baseline
- [X] 1.1 Write bare-metal main.c to target Nucleo-F446RE LD2 LED
- [X] 1.2 Add linker script and startup assembly to compile
- [X] 1.3 Flash the board and confirm physical blink
- [X] 1.4 Establish UART text output to Ubuntu terminal

## Milestone 2: Asynchronous Timing & Storage Architecture
- [ ] 2.1 Initialize the SysTick Timer for precise 1ms clock ticks
- [ ] 2.2 Rebuild the main loop around non-blocking event scheduling
- [ ] 2.3 Configure the SPI peripheral registers for external communication
- [ ] 2.4 Integrate raw block-layer SD card read/write capabilities