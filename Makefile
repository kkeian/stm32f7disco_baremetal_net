CFLAGS ?= -Werror -Wformat -Wextra -Wmissing-include-dirs -Wuninitialized -ffreestanding -nostdlib -nolibc -v 
#LDFLAGS ?= 

SOURCES = main.c
FLASHADR ?= 0x8000000

firmware.elf: $(SOURCES)
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@ 

.PHONY: compile
compile: firmware.elf

.PHONY: asm
asm: $(SOURCES)
	arm-none-eabi-gcc $(CFLAGS) -S $< -o firmware.s

.PHONY: debug
debug: $(SOURCES)
	arm-none-eabi-gcc $(CFLAGS) -g firmware.elf

# convert .elf to binary format executable
firmware.bin: firmware.elf
	arm-none-eabi-objdump -O binary $< $@

# using automatic variable $< for matching 1st pre-req name
flash: firmware.bin
	st-flash --reset write $< $(FLASHADR)

.PHONY: clean
clean:
	-rm main.s main.o firmware.*