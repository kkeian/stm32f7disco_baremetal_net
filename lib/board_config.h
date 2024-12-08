#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <inttypes.h>

void init(void);
static uint32_t gpio_port_start(char port_letter);
static void clear_and_set(uint32_t *reg, uint32_t bit);

#endif