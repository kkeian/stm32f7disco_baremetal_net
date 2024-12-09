CFLAGS ?= -Werror -Wformat -Wextra -Wmissing-include-dirs -Wuninitialized \
		  -mthumb -mcpu=cortex-m4 -ffreestanding -nostdlib -nolibc -v \
		  -ffunction-sections -fdata-sections
LDFLAGS ?= -nolibc -nostdlib -belf32-littlearm -Tlink.ld

SOURCES = main.c
FLASHADR ?= 0x8000000

# relocatable object file (no abs addrs for machine code and symbols)
firmware.o: $(SOURCES)
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@ 

# use linker script to fill in missing addrs for machine code and symbols
firmware.elf: firmware.O
	arm-none-eabi-gcc $(LDFLAGS) $< -o $@

# to view contents/analyze output: arm-none-eabi-objdump
# useful options:
# -h view section headers of ELF file
# -d view executable disassembly
# -s view all of sections specified with -j SECTIONNAME
# -r view relocatio nentries
# -b BFD target obj format e.g. arm32-little

# user-facing phony tgts
.PHONY: compile
compile: firmware.o

.PHONY: link
link: firmware.elf

.PHONY: asm
asm: $(SOURCES)
	arm-none-eabi-gcc $(CFLAGS) -S $< -o firmware.s

.PHONY: debug
debug: $(SOURCES)
	arm-none-eabi-gcc $(CFLAGS) -g -c firmware.elf

# convert .elf to binary format executable
firmware.bin: firmware.elf
	arm-none-eabi-objdump -O binary $< $@

# using automatic variable $< for matching 1st pre-req name
flash: firmware.bin
	st-flash --reset write $< $(FLASHADR)

.PHONY: clean
clean:
	-rm main.s main.o firmware.*