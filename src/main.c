#include <stdint.h>

/* --- REGISTERS: SYSTEM AND CONTROL ENGINE --- */
#define RCC_BASE      0x40023800U
#define RCC_AHB1ENR   (*(volatile uint32_t *)(RCC_BASE + 0x30U))
#define RCC_APB1ENR   (*(volatile uint32_t *)(RCC_BASE + 0x40U))
#define RCC_APB2ENR   (*(volatile uint32_t *)(RCC_BASE + 0x44U))

#define GPIOAEN       (1U << 0)
#define USART2EN      (1U << 17)
#define SPI1EN        (1U << 12)

/* --- REGISTERS: CORTEX-M4 INTERNAL SYSTICK MAPPING --- */
#define STK_CTRL      (*(volatile uint32_t *)0xE000E010U)
#define STK_LOAD      (*(volatile uint32_t *)0xE000E014U)
#define STK_VAL       (*(volatile uint32_t *)0xE000E018U)

/* --- REGISTERS: GPIO PORT A ENGINE --- */
#define GPIOA_BASE    0x40020000U
#define GPIOA_MODER   (*(volatile uint32_t *)(GPIOA_BASE + 0x00U))
#define GPIOA_AFRL    (*(volatile uint32_t *)(GPIOA_BASE + 0x20U))
#define GPIOA_ODR     (*(volatile uint32_t *)(GPIOA_BASE + 0x14U))

#define CS_PIN        (1U << 4)    // PA4: Software Chip Select
#define CS_LOW()      (GPIOA_ODR &= ~CS_PIN)
#define CS_HIGH()     (GPIOA_ODR |= CS_PIN)

/* --- REGISTERS: UNIVERSAL SERIAL PERIPHERAL (USART2) --- */
#define USART2_BASE   0x40004400U
#define USART2_SR     (*(volatile uint32_t *)(USART2_BASE + 0x00U))
#define USART2_DR     (*(volatile uint32_t *)(USART2_BASE + 0x04U))
#define USART2_BRR    (*(volatile uint32_t *)(USART2_BASE + 0x08U))
#define USART2_CR1    (*(volatile uint32_t *)(USART2_BASE + 0x0CU))

/* --- REGISTERS: SERIAL PERIPHERAL INTERFACE (SPI1) --- */
#define SPI1_BASE     0x40013000U
#define SPI1_CR1      (*(volatile uint32_t *)(SPI1_BASE + 0x00U))
#define SPI1_CR2      (*(volatile uint32_t *)(SPI1_BASE + 0x04U))
#define SPI1_SR       (*(volatile uint32_t *)(SPI1_BASE + 0x08U))
#define SPI1_DR       (*(volatile uint32_t *)(SPI1_BASE + 0x0CU))

/* --- SD STORAGE PROTOCOL COMMANDS --- */
#define CMD0          0x40U   // GO_IDLE_STATE (Resets the SD card)

/* --- SYSTEM GLOBAL COUNTERS --- */
static volatile uint32_t system_ms = 0;

/* --- FUNCTION DECLARATIONS --- */
void systick_init(void);
void uart2_init(void);
void uart2_write(char c);
void uart2_print(const char *str);
void uart2_print_uint(uint32_t val);
void spi1_init(void);
uint8_t spi1_transfer(uint8_t data);
void sd_send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc);
uint8_t sd_init(void);

/* --- APPLICATION ENTRY POINT --- */
int main(void) {
    // Initialize low-level bare-metal hardware subsystems
    uart2_init();
    systick_init();
    spi1_init();

    uart2_print("\r\n=== STM32-BlackBox-Telemetry: Milestone 2 Phase ===\r\n");
    uart2_print("System Engine: SysTick Core Active. Event Scheduling Enabled.\r\n");
    uart2_print("Storage Engine: SPI1 Master Interface Initialized (250kHz Mode).\r\n");

    // Execute the physical hardware SD card wakeup sequence
    uint8_t sd_state = sd_init();
    if (sd_state == 0x01U) {
        uart2_print("[SUCCESS] SD Storage Engine accepted SPI layout. State: IDLE.\r\n");
    } else {
        uart2_print("[ERROR] SD Card handshake failed to respond or rejected SPI mode.\r\n");
    }

    uint32_t last_sync_time = 0;

    /* --- COOPERATIVE MAIN SCHEDULER LOOP --- */
    while (1) {
        // Asynchronous structural task executed strictly every 2000ms
        if ((system_ms - last_sync_time) >= 2000U) {
            last_sync_time = system_ms;

            uart2_print("[METRIC] Flight Frame Sync Ok. Uptime: ");
            uart2_print_uint(last_sync_time / 1000U);
            uart2_print("s\r\n");
        }
    }

    return 0;
}

/* --- HARDWARE INTERRUPT HANDLERS --- */

/**
 * SysTick Exception Handler. Triggers precisely every 1.00 milliseconds.
 * Routed dynamically by the vector table in startup_stm32f446re.s
 */
void SysTick_Handler(void) {
    system_ms++;
}

/* --- DRIVER SUBSYSTEM IMPLEMENTATIONS --- */

/**
 * Configure SysTick Timer to fire interrupts at a stable 1 kHz baseline
 */
void systick_init(void) {
    // 16,000,000 Hz / 1,000 = 16,000 clock cycles per millisecond interval
    STK_LOAD = 16000U - 1U;
    STK_VAL  = 0U;
    
    // Bits: TICKINT=1 (Enable Interrupt), ENABLE=1 (Turn on Counter), CLKSOURCE=1 (Processor Clock)
    STK_CTRL = (1U << 2) | (1U << 1) | (1U << 0);
}

/**
 * Configure USART2 for Standard Telemetry Output (4800 baud, 8N1)
 */
void uart2_init(void) {
    RCC_AHB1ENR |= GPIOAEN;
    RCC_APB1ENR |= USART2EN;

    // Configure PA2 and PA3 to Alternative Function Mode (10)
    GPIOA_MODER &= ~((3U << 4) | (3U << 6));
    GPIOA_MODER |=  ((2U << 4) | (2U << 6));

    // Map PA2 (TX) and PA3 (RX) explicitly to Alternate Function 7 (USART2)
    GPIOA_AFRL &= ~((0xFU << 8) | (0xFU << 12));
    GPIOA_AFRL |=  ((7U << 8) | (7U << 12));

    // Baud rate settings for 4800 Baud @ 16 MHz Clock Engine
    // BRR = 16,000,000 / 4,800 = 3333.33 -> Mantissa = 208 (0xD0), Fraction = 5 (0x5) -> 0x0D05
    USART2_BRR = 0x0D05U; 

    // Enable Transmit (TE), Receive (RE), and the peripheral itself (UE)
    USART2_CR1 = (1U << 3) | (1U << 2) | (1U << 13);
}

void uart2_write(char c) {
    while (!(USART2_SR & (1U << 7))); // Wait until TXE (Transmit Data Register Empty) is set
    USART2_DR = (uint32_t)c;
}

void uart2_print(const char *str) {
    while (*str) {
        uart2_write(*str++);
    }
}

void uart2_print_uint(uint32_t val) {
    char buf[11];
    int i = 10;
    buf[i] = '\0';

    if (val == 0) {
        uart2_write('0');
        return;
    }

    while (val > 0) {
        i--;
        buf[i] = (char)('0' + (val % 10));
        val /= 10;
    }
    uart2_print(&buf[i]);
}

/**
 * Configure SPI1 in Master Mode for low-level storage handshakes
 * Clock Speed: ~250 kHz baseline for safe SD initialization (16MHz clock / 64 prescaler)
 * Protocol Layout: 8-bit Data Length, MSB Transmitted First, CPOL=0, CPHA=0
 */
void spi1_init(void) {
    // 1. Route operational clock lines to both GPIO Port A and the SPI1 engine block
    RCC_AHB1ENR |= GPIOAEN;
    RCC_APB2ENR |= SPI1EN;

    // 2. Configure PA5 (SCLK), PA6 (MISO), and PA7 (MOSI) to Alternate Function Mode (10)
    GPIOA_MODER &= ~((3U << 10) | (3U << 12) | (3U << 14));
    GPIOA_MODER |=  ((2U << 10) | (2U << 12) | (2U << 14));

    // 3. Bind pins 5, 6, and 7 directly to Alternate Function 5 (SPI1 routing matrix)
    GPIOA_AFRL &= ~((0xFU << 20) | (0xFU << 24) | (0xFU << 28));
    GPIOA_AFRL |=  ((5U << 20) | (5U << 24) | (5U << 28));

    // 4. Set up PA4 as a standard manual output line to function as Software Slave Select
    GPIOA_MODER &= ~(3U << 8);
    GPIOA_MODER |=  (1U << 8);
    CS_HIGH(); // De-assert CS immediately by pulling it high

    // 5. Build Control Register 1 parameters
    // BR[2:0] (Bits 5:3) = 101 -> Clock divided by 64 (~250 kHz safe baseline)
    // MSTR (Bit 2) = 1       -> Master Mode node configuration
    // SSM (Bit 9) = 1        -> Manage Slave Select behavior entirely through software
    // SSI (Bit 8) = 1        -> Internally assert master state assignment stability
    SPI1_CR1 = (5U << 3) | (1U << 2) | (1U << 9) | (1U << 8);

    // 6. Awaken SPI1 execution hardware
    SPI1_CR1 |= (1U << 6); // SPE (SPI Enable Bit)
}

/**
 * Atomic 8-bit Full-Duplex Bus Transaction.
 * Sends a byte across MOSI while simultaneously grabbing a byte from MISO.
 */
uint8_t spi1_transfer(uint8_t data) {
    // Load data byte straight into the physical transmission queue register
    SPI1_DR = data;

    // Halt until the internal hardware flags state that the full transaction loop is complete
    // RXNE (Receive Buffer Not Empty, Bit 0) confirms an incoming byte has shifted back in
    while (!(SPI1_SR & (1U << 0)));

    // Return the byte shifted out of the slave device
    return (uint8_t)SPI1_DR;
}

/**
 * Sends a standard 6-byte SD command frame over the SPI bus
 */
void sd_send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc) {
    spi1_transfer(cmd);                 // Transmission Start + Command index 
    spi1_transfer((arg >> 24) & 0xFFU); // Argument Bits [31:24]
    spi1_transfer((arg >> 16) & 0xFFU); // Argument Bits [23:16]
    spi1_transfer((arg >> 8)  & 0xFFU); // Argument Bits [15:8]
    spi1_transfer(arg         & 0xFFU); // Argument Bits [7:0]
    spi1_transfer(crc);                 // Calculated CRC Checksum + Stop Bit
}

/**
 * Executes the physical hardware wakeup sequence for the SD Storage Medium
 * Returns: 0x01 on successful IDLE state shift, 0xFF on Timeout/Failure
 */
uint8_t sd_init(void) {
    uint8_t response = 0xFF;
    uint32_t timeout = 0;

    // Phase 1: Provide the Power-On Synchronization Window
    // Keep CS high and pump 10 dummy bytes (80 clock cycles) into the card
    CS_HIGH();
    for (int i = 0; i < 10; i++) {
        spi1_transfer(0xFFU);
    }

    // Phase 2: Assert Chip Select and issue CMD0 (Go Idle State)
    uart2_print("[STORAGE] Asserting CS. Sending CMD0 to transition state...\r\n");
    CS_LOW();
    
    // CMD0 argument is 0x00000000. Hardcoded CRC for CMD0 with zero argument is 0x95.
    sd_send_cmd(CMD0, 0x00000000U, 0x95U);

    // Phase 3: Wait for the R1 Response frame (looking for 0x01 - Idle State)
    // The card can take up to 8 frames to clear its busy state shift
    do {
        response = spi1_transfer(0xFFU); // Pump clock, read MISO line
        timeout++;
    } while ((response == 0xFFU) && (timeout < 100U));

    // Release the SPI bus cleanly by pulling CS high and sending one final dummy byte
    CS_HIGH();
    spi1_transfer(0xFFU);

    // Report back raw bus state metrics
    uart2_print("[STORAGE] CMD0 R1 Response Received: 0x");
    uart2_print_uint(response);
    uart2_print("\r\n");

    return response;
}