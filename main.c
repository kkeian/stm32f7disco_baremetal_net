#include "lib/board_config.h"

int main(void)
{
    return 0;
}

__attribute__((naked, noreturn)) void reset_handl(void)
{
    // Initialize memory regions
    extern uint32_t *_sbss, *_ebss, *_sdata, *_edata, *_sidata, *_estack;
    while (_sbss < _ebss) *_sbss++ = 0; /* zero init .bss section */
    while (_sdata < _edata) *_sidata++ = *_sdata++; /* copy data from flash to RAM before program init */

    // init MCU and board (setup runtime env + clock setup and system configuration)
    init();

    main();
    // in case main exits:
    while (1) ;
}