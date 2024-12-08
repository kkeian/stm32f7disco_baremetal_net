/* GNU ld binutils documentation for more info on the meaning of this file. */
/* "entry" point of the application */
ENTRY(_reset);

/* define memory regions based on addresses and lengths in data sheet */
MEMORY {
    flash(rx) : ORIGIN = 0x8000000, LENGTH = 256k
    sram(rwx) : ORIGIN = 0x20000000, LENGTH = 40k
}

/* Initialize Main Stack Pointer (MSP) to highest physical address of RAM (end of valid RAM addresses) */
/* the first vector in vector table must be MSP followed immediately by a pointer to the reset handler */
/* for more information on the vector table required (expected by the MCU) layout, see the */
/* System Level Programmers' Model - Exception Model section of the ARM Architecture Reference Manual (ARM) */
/* for the MCU of your board */
_estack = ORIGIN(sram) + LENGTH(sram);

SECTIONS {
    /* required sections: .bss, .data, .text */
    /* should make a section for vector table entries */
    /* order of output sections specified = order they're stacked in memory */

    /* first we want MSP. Then all 15 other required HW interrupt handlers. Then EXTI handlers if we use them */
    .vectors : {
        KEEP(*(.vectors)) /* make sure link-time optimization / gc doesn't eliminate vector table */
    } > flash

    /* next code */
    .text : {
        *(.text)
    } > flash

    /* next our data which will have to copy to RAM at init of runtime env */
    .data : {
        _sdata = .;
        *(.data);
        _edata = .;
    } > sram AT > flash
    /* sram is where program should expect .data to reside at runtime */
    /* flash is where it is put AT initially when we flash the MCU */
    /* _sidata is the start address in RAM where we want to copy .data contents to */
    /* _sdata and _edata are the start and end addresses where the .data contents are initially located in flash */
    _sidata = LOADADDR(.data);

    .bss : {
        _sbss = .;
        *(.bss COMMON); /* COMMON are common symbols in ELF files per the definition in ld docs */
        _ebss = .;
    } > sram
    /* put uninit'd data directly into RAM at flash */

    /* make sure to align along byte boundaries because ARM32 is a byte addressed Architecture */
    . = ALIGN(8); 
}