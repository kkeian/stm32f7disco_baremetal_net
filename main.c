#include "lib/board_config.h"

int main(void)
{
    return 0;
}

__attribute__((naked, noreturn)) void _reset(void)
{
    // init MCU and board (setup runtime env + clock setup and system configuration)
    init();

    main();
    // in case main exits:
    while (1) ;
}