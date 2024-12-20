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
// for setting RMII or MII on Ethernet peripheral
struct syscfg {
    volatile uint32_t MEMRMP, PMC, EXTICR1, EXTICR2, EXTICR3, EXTICR4, CMPCR;
};

struct gpio {
    volatile uint32_t MODER, OTYPER, OSPEEDR,
                      PUPDR, IDR, ODR, BSRR,
                      LCKR, AFRL, AFRH;
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
// start of the GPIO addresses
#define AHB1 ((uint32_t) 0x4002000)

void init(void)
{
    // Must enable clock before accessing registers of a peripheral
    //
    // Enable SYSCFG clock before we set IF to PHY
    // reset value of register = 0x0
    RCC->APB2ENR ^= 0x4000; // set SYSCFG clk en bit

    // Select Eth PHY IF
    // clear RMII/MII bit
    // SYSCFG->PMC is used to configure PHY IF "mode":
    // Reset value is 0 = MII IF selected.
    // So this is set correctly already.

    // Enable CRC clock
    // reset value of reg: 0x0010 0000
    RCC->AHB1ENR ^= 0x1000;

    // Configure CRC for network message integrity
    // CRC polynomial at reset is CRC-32 Ethernet 0x04C1 1DB7
    // so we leave that setting.

    // configure MAC clocks - via AHB1 RCC register:
    // enable the 3 MAC clocks
    // RMII/MII clock
    // TX clock
    // RX clock
    RCC->AHB1ENR &= 0xF1FFFFFF; // clear MAC TX, RX, and MAC enable bits
    RCC->AHB1ENR ^= 0x02000000; // set MAC clock enable bit
    RCC->AHB1ENR ^= (0x04000000) ^ (0x08000000); // set ETH TX and RX EN bits

    // enable GPIO A, B, and C to allow access to GPIO registers AF for ETH functionality
    RCC->AHB1ENR ^= (0x7); // set lower 3 bits to target A, B, and C

    // set GPIO A pins to ETH AF
    struct gpio *GPIOA = gpio_port_start('A');
    // turn on (enable) pins in AF mode
    uint32_t AF_MODE = 0x2; // = 0b10
    uint32_t CFGPINS = 0x0;
    // set pin 0 - MII_CRS
    // set pin 1 - MII_RX_CLK
    // set pin 2 - MDIO for SMI
    // set pin 3 - MII_COL
    // set pin 7 - MII_RX_DV
    // shift amt = 2 (num of bits in AF_MODE) * pin num
    CFGPINS ^= ((AF_MODE) | (AF_MODE << 2) | (AF_MODE << 4) |
               (AF_MODE << 6) | (AF_MODE << 14));
    clear_and_set(&(GPIOA->MODER), CFGPINS);
    // configure pins to use AF11 which is the alternate function corresponding to
    // that pin's ETH AF
    uint32_t AF11 = 0xB; // = 0b1011
    CFGPINS = 0x0; // clear bits of accumulator mask
    // shift amt = 4 (num of bits in AF11) * pin num
    // targeting pins 0, 1, 2, 3, and 7:
    CFGPINS ^= ((AF11) | (AF11 << 4) | (AF11 << 8) | (AF11 << 12) |
               (AF11 << 28));
    // AFRL used because they correspond to pins 0-7
    // of the GPIO port's selected AF
    clear_and_set(&(GPIOA->AFRL), CFGPINS);

    // set GPIO B pins to AF mode
    struct gpio *GPIOB = gpio_port_start('B');
    // turn on pin's modes
    CFGPINS = 0x0; // clear accumulator
    // PB0 - MII_RXD2
    // PB1 - MII_RXD3
    // PB5 - PPS_OUT
    // PB8 - MII_TXD3
    // PB10 - MII_RX_ER
    // PB11 - MII_TX_EN
    // PB12 - MII_TXD0
    // PB13 - MII_TXD1
    // shift amt = 2 (num of bits in AF_MODE) * pin num
    CFGPINS ^= ((AF_MODE) | (AF_MODE << 2) | (AF_MODE << 10) |
                (AF_MODE << 16) | (AF_MODE << 20) | (AF_MODE << 22) |
                (AF_MODE << 24) | (AF_MODE << 26));
    clear_and_set(&(GPIOB->MODER), CFGPINS);
    // Select the AF for each pin whose mode was set
    // Set configured AF Low 0-7 pins above to AF11
    CFGPINS = 0x0; // clear set pins
    // shift amt = 4 (num of bits in AF11) * pin num
    // targeting pins 0, 1, and 5:
    CFGPINS ^= ((AF11) | (AF11 << 4) | (AF11 << 20));
    clear_and_set(&(GPIOB->AFRL), CFGPINS);
    // Set configured AF High 8-15 pins above to AF11
    // Note: in AFRH pins the 3 lsb (bits 0 - 3) target pin 8
    // targeting pins 8, 10, 11, 12, and 13:
    CFGPINS = 0x0; // clear set pins
    CFGPINS ^= ((AF11) | (AF11 << 8) | (AF11 << 12) | (AF11 << 16) | 
               (AF11 << 20));
    clear_and_set(&(GPIOB->AFRH), CFGPINS);

    // set GPIO C pins to AF mode
    struct gpio *GPIOC = gpio_port_start('C');
    // Enable Port C pins by setting them to use AF mode
    CFGPINS = 0x0;
    // PC1 - ETH_MDC for SMI
    // PC2 - ETH_MII_TXD2
    // PC3 - ETH_MII_TX_CLK
    // PC4 - ETH_MII_RXD0
    // PC5 - ETH_MII_RXD1
    // shift amt = 2 (num of bits in AF_MODE) * pin num
    CFGPINS ^= ((AF_MODE << 2) | (AF_MODE << 4) | (AF_MODE << 6) | (AF_MODE << 8) | (AF_MODE << 10));
    clear_and_set(&(GPIOC->MODER), CFGPINS);

    // Configure the AF for each pin to use ETH related AF which is
    // AF11 for all ETH related functions per the Ref Manual
    CFGPINS = 0x0;
    // pins we need to configure are all < 7 so we target AFRL
    // which configures the AF used on pins 0 - 7 of the port
    // shift amt = 4 (num of bits in AF11) * pin num
    // targeting pins 1, 2, 3, 4, and 5:
    CFGPINS ^= ((AF11 << 4) | (AF11 << 8) | (AF11 << 12) | (AF11 << 16) | (AF11 << 20));
    clear_and_set(&(GPIOC->AFRL), CFGPINS);

    // wait 2 clock cycles before peripheral (Eth PHY)
    // peripheral registers can be accessed
}

// helper for computing starting address of gpio
static struct gpio *gpio_port_start(char letter)
{
    return (struct gpio *) (((letter - 'A') * (0x400)) ^ AHB1);
}

// clear and set bit
static inline void clear_and_set(uint32_t *reg, uint32_t bit)
{
    *reg &= ~(bit); // clear
    *reg ^= bit; // set
}