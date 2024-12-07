

SOURCES = "main.c"

firmware.elf: $(SOURCES)
	arm-none-eabi-gcc -c -o main.o main.c

firmware.bin: firmware.elf
	arm-none-eabi-objdump -O binary $< $@

flash: firmware.bin
	st-flash --reset write $<