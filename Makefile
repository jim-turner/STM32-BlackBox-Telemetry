# Target Microcontroller Configuration
MCU_FLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft

# Toolchain Definitions
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Build Configurations & Directories
CFLAGS = $(MCU_FLAGS) -O0 -g3 -Wall -Iinclude
LDFLAGS = $(MCU_FLAGS) -TStartup/stm32f446retx_flash.ld -nostdlib

# Explicit list of the object files we expect to build
OBJS = build/main.o build/startup_stm32f446xx.o
TARGET = build/blackbox_telemetry

# Build Rules
all: $(TARGET).elf $(TARGET).bin

# Rule to compile the C source file
build/main.o: src/main.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to compile the assembly startup file
build/startup_stm32f446xx.o: Startup/startup_stm32f446xx.s
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# The linker now explicitly waits until both object files are built
$(TARGET).elf: $(OBJS)
	@mkdir -p build
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Convert the linked ELF binary to a raw .bin file for flashing
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf build