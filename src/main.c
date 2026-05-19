/**
 * STM32-BlackBox-Telemetry: Milestone 1.4 (Bare-Metal UART Output)
 * Target Hardware: Nucleo-F446RE (ARM Cortex-M4)
 * USART2 Pins: PA2 (TX), PA3 (RX) - Hardwired to ST-LINK Virtual COM Port
 */

#include <stdint.h>

// Peripheral Base Addresses
#define RCC_BASE      0x40023800U
#define GPIOA_BASE    0x40020000U
#define USART2_BASE   0x40004400U

// RCC Register Offsets
#define RCC_AHB1ENR   (*(volatile uint32_t *)(RCC_BASE   + 0x30U))
#define RCC_APB1ENR   (*(volatile uint32_t *)(RCC_BASE   + 0x40U))

// GPIOA Register Offsets
#define GPIOA_MODER   (*(volatile uint32_t *)(GPIOA_BASE + 0x00U))
#define GPIOA_AFRL    (*(volatile uint32_t *)(GPIOA_BASE + 0x20U))
#define GPIOA_ODR     (*(volatile uint32_t *)(GPIOA_BASE + 0x14U))

// USART2 Register Offsets
#define USART2_SR     (*(volatile uint32_t *)(USART2_BASE + 0x00U))
#define USART2_DR     (*(volatile uint32_t *)(USART2_BASE + 0x04U))
#define USART2_BRR    (*(volatile uint32_t *)(USART2_BASE + 0x08U))
#define USART2_CR1    (*(volatile uint32_t *)(USART2_BASE + 0x0CU))

// Bit Definitions
#define GPIOAEN       (1U << 0)    // Enable GPIOA Clock
#define USART2EN      (1U << 17)   // Enable USART2 Clock (on APB1)
#define LED_PIN       (1U << 5)    // PA5

void uart_init(void) {
    // 1. Enable Clocks to GPIOA and USART2
    RCC_AHB1ENR |= GPIOAEN;       
    RCC_APB1ENR |= USART2EN;      

    // 2. Configure PA2 and PA3 for Alternate Function Mode
    GPIOA_MODER &= ~((3U << 4) | (3U << 6)); 
    GPIOA_MODER |=  ((2U << 4) | (2U << 6)); 

    // 3. Map PA2 and PA3 specifically to Alternate Function 7 (AF7 = USART2)
    GPIOA_AFRL &= ~((0xFU << 8) | (0xFU << 12)); 
    GPIOA_AFRL |=  ((7U << 8) | (7U << 12));    

    // 4. Configure Baud Rate to an ultra-stable 4800 bps baseline
    // This wider timing window absorbs the factory internal clock drift.
    // 16,000,000 / 4,800 = 3333.33 => 3333 = 0x0D05
    USART2_BRR = 0x0D05;

    // 5. Enable USART2, Transmitter, and Receiver
    USART2_CR1 = (1U << 13) | (1U << 3) | (1U << 2);
}

// Push a single byte out the physical transmission wire
void uart_write(int ch) {
    // Wait until Transmit Data Register is empty (Bit 7 of SR = TXE)
    while (!(USART2_SR & (1U << 7)));
    // Write character byte to Data Register
    USART2_DR = (ch & 0xFF);
}

// Helper to stream whole text phrases
void uart_print(const char *str) {
    while (*str) {
        if (*str == '\n') {
            uart_write('\r');
        }
        uart_write(*str++);
    }
}

int main(void) {
    // Initialize serial output
    uart_init();

    // Set up the LED pin output mode
    GPIOA_MODER &= ~(3U << 10); 
    GPIOA_MODER |=  (1U << 10); 

    uart_print("\n=== STM32-BlackBox-Telemetry Booting ===\n");
    uart_print("System status: Core Awake, LED Initialized.\n");

    while (1) {
        // Toggle the physical LED indicator
        GPIOA_ODR ^= LED_PIN;

        // Print telemetry heartbeat every cycle
        uart_print("[TELEMETRY] System executing... Heartbeat stable.\n");

        // Loop delay
        for (volatile int i = 0; i < 1000000; i++) {
            __asm("nop");
        }
    }

    return 0;
}

// ============================================================================
// SYSTEM EXCEPTION HANDLERS
// ============================================================================

void NMI_Handler(void) {
    while (1);
}

void HardFault_Handler(void) {
    while (1);
}