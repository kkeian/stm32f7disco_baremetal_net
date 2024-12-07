#include "board_config.h"

struct rcc {
    volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR,
                      AHB3RSTR, _RES1, APB1RSTR, APB2RSTR, _RES2, _RES3,
                      AHB1ENR, AHB2ENR, AHB3ENR, _RES4, APB1ENR, APB2ENR,
                      _RES5, _RES6, AHB1LPENR, AHB2LPENR, AHB3LPENR,
                      _RES7, APB1LPENR, APB2LPENR, _RES8, _RES9, BDCR,
                      CSR, _RES10, _RES11, SSCGR, PLLI2SCFGR, PLLSAICFGR,
                      DCKCFGR1, DCKCFGR2;
};

enum { MAC_CLK_EN = 2, MAC_TX_EN = 4, MAC_RX_EN = 8 };

struct syscfg {
    volatile uint32_t MEMRMP, PMC, EXTICR1, EXTICR2, EXTICR3, EXTICR4, CMPCR;
};

// starting memory-mapped addrs from ref manual
#define SYSCFG ((struct syscfg *) 0x40013800)
#define RCC ((struct rcc *) 0x40023800)

void init(void)
{
    // Select Eth PHY IF
    // clear RMII/MII bit
    SYSCFG->PMC &= 0xFF7FFFFF;
    // Reset value = 0 = MII IF selected:
    // SYSCFG->PMC |= 0x00000000;

    // configure source of MAC clocks: AHB clock

    // enable the 3 MAC clocks
    // RMII/MII clock
    // TX clock
    // RX clock
    RCC->AHB1ENR &= 0xF1FFFFFF; // clear MAC TX, RX, and MAC enable bits
    RCC->AHB1ENR ^= 0x02000000; // set MAC clock enable bit

    // wait 2 clock cycles before peripheral (Eth PHY)
    // peripheral registers can be accessed
}