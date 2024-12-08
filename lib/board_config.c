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
// bits to set for enabling the clock for each part of the MAC in the RCC register
enum { MAC_CLK_EN = 2, MAC_TX_EN = 4, MAC_RX_EN = 8 };
// for setting RMII or MII on Ethernet peripheral
struct syscfg {
    volatile uint32_t MEMRMP, PMC, EXTICR1, EXTICR2, EXTICR3, EXTICR4, CMPCR;
};

struct eth {
    volatile uint32_t MACCR, MACFFR, MACHTHR, MACHTLR, MACMIIAR,
                      MACMIIDR, MACFCR, MACVLANTR, MACRWUFFR,
                      MACPMTCSR, MACDBGR, MACSR, MACIMR, MACA0HR,
                      MACA0LR, MACA1HR, MACA1LR, MACA2HR, MACA2LR,
                      MACA3HR, MACA3LR, MMCCR, MMCRIR, MMCTIR,
                      MMCRIMR, MMCTIMR, MMCTGFSCCR, MMCTGFMSCCR,
                      MMCTGFCR, MMCRFCECR, MMCRFAECR, MMCRGUFCR,
                      PTPTSCR, PTPSSIR, PTPTSHR, PTPTSLR, PTPTSHUR,
                      PTPTSLUR, PTPTSAR, PTPTTHR, PTPTTLR, PTPTSSR,
                      DMABMR, DMATPDR, DMARPDR, DMARDLAR, DMATDLAR,
                      DMASR, DMAOMR, DMAIER, DMAMFBOCR, DMARSWTR,
                      DMACHTDR, DMACHRDR, DMACHTBAR, DMACHRBAR;
};

// starting memory-mapped addrs from ref manual
#define SYSCFG ((struct syscfg *) 0x40013800)
#define RCC ((struct rcc *) 0x40023800)
#define ETH ((struct eth *) 0x40028000)

void init(void)
{
    // Enable SYSCFG clock before we set IF to PHY
    RCC->AHB2ENR &= 0x10000000; // clear SYSCFG clk en bit
    RCC->AHB2ENR ^= 0x10000000; // set SYSCFG clk en bit

    // Select Eth PHY IF
    // clear RMII/MII bit
    SYSCFG->PMC &= 0xFF7FFFFF;
    // Reset value is 0 = MII IF selected:
    // SYSCFG->PMC |= 0x00000000;

    // configure MAC clocks - via AHB1 RCC register:
    // enable the 3 MAC clocks
    // RMII/MII clock
    // TX clock
    // RX clock
    RCC->AHB1ENR &= 0xF1FFFFFF; // clear MAC TX, RX, and MAC enable bits
    RCC->AHB1ENR ^= 0x02000000; // set MAC clock enable bit
    RCC->AHB1ENR ^= (0x04000000) ^ (0x08000000); // set TX and RX EN bits

    // wait 2 clock cycles before peripheral (Eth PHY)
    // peripheral registers can be accessed
}