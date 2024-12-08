/* GNU ld binutils documentation for more info on the meaning of this file. */
/* "entry" point of the application */
ENTRY(_reset);

/* define memory regions based on addresses and lengths in data sheet */
MEMORY {
    flash(rx) : ORIGIN = 0x8000000, LENGTH = 256k
    sram(rwx) : ORIGIN = 0x20000000, LENGTH = 40k
}

/* Initialize stack to top address of RAM */
_estack = ORIGIN(sram) + LENGTH(sram);

SECTIONS {
    
}