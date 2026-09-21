CROSS_COMPILE = aarch64-linux-gnu-

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

CFLAGS = -Wall -Wextra -O2 \
	-ffreestanding \
	-fno-pie \
	-nostdlib \
	-nostartfiles \
	-Iinclude

LDFLAGS = -T linker.ld

BUILD = build

OBJS = \
	$(BUILD)/boot.o \
	$(BUILD)/main.o \
	$(BUILD)/uart.o

all: $(BUILD)/kernel.elf

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: arch/arm64/boot.S | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/main.o: kernel/main.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/uart.o: drivers/uart.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

run: all
	qemu-system-aarch64 \
		-M virt \
		-cpu cortex-a72 \
		-m 512M \
		-nographic \
		-kernel $(BUILD)/kernel.elf

clean:
	rm -rf $(BUILD)

.PHONY: all run clean
