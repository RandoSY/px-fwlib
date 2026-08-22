#ifndef __PX_BOARD_GPIO_H__
#define __PX_BOARD_GPIO_H__
/* =============================================================================
    WeAct STM32G030 Core Board GPIO definitions

    The board uses the STM32G030F6P6 TSSOP20 pinout. PA11 and PA12 are
    remapped to PA9 and PA10 when USART1 is used; the corresponding board
    header nets are labelled PA11 and PA12.
============================================================================= */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* _____DEFINITIONS__________________________________________________________ */
#define PX_GPIO_UART1_TX        PX_GPIO(A, 11, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_4)
#define PX_GPIO_UART1_RX        PX_GPIO(A, 12, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_4)

#define PX_GPIO_SPI1_CS         PX_GPIO(A,  8, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_NA)
#define PX_GPIO_SPI1_SCK        PX_GPIO(A,  5, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_0)
#define PX_GPIO_SPI1_MISO       PX_GPIO(A,  6, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)
#define PX_GPIO_SPI1_MOSI       PX_GPIO(A,  7, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_0)

#define PX_GPIO_ADC0            PX_GPIO(A,  0, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_ADC1            PX_GPIO(A,  1, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_ADC2            PX_GPIO(A,  2, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)
#define PX_GPIO_ADC3            PX_GPIO(A,  3, PX_GPIO_MODE_ANA, PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)

#define PX_GPIO_USR_LED         PX_GPIO(A,  4, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_HI, PX_GPIO_AF_NA)
#define PX_GPIO_USR_BTN         PX_GPIO(A, 14, PX_GPIO_MODE_IN,  PX_GPIO_OTYPE_NA, PX_GPIO_OSPEED_NA, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_NA)

#define PX_GPIO_SWDIO           PX_GPIO(A, 13, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_UP, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)
#define PX_GPIO_SWDCK           PX_GPIO(A, 14, PX_GPIO_MODE_AF,  PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_HI, PX_GPIO_PULL_DN, PX_GPIO_OUT_INIT_NA, PX_GPIO_AF_0)

/* _____GLOBAL VARIABLES_____________________________________________________ */
static const px_gpio_handle_t px_gpio_uart1_tx = {PX_GPIO_UART1_TX};
static const px_gpio_handle_t px_gpio_uart1_rx = {PX_GPIO_UART1_RX};

static const px_gpio_handle_t px_gpio_spi1_cs  = {PX_GPIO_SPI1_CS};
static const px_gpio_handle_t px_gpio_spi1_sck = {PX_GPIO_SPI1_SCK};
static const px_gpio_handle_t px_gpio_spi1_miso = {PX_GPIO_SPI1_MISO};
static const px_gpio_handle_t px_gpio_spi1_mosi = {PX_GPIO_SPI1_MOSI};

static const px_gpio_handle_t px_gpio_adc0 = {PX_GPIO_ADC0};
static const px_gpio_handle_t px_gpio_adc1 = {PX_GPIO_ADC1};
static const px_gpio_handle_t px_gpio_adc2 = {PX_GPIO_ADC2};
static const px_gpio_handle_t px_gpio_adc3 = {PX_GPIO_ADC3};

static const px_gpio_handle_t px_gpio_usr_led = {PX_GPIO_USR_LED};
static const px_gpio_handle_t px_gpio_usr_btn = {PX_GPIO_USR_BTN};
static const px_gpio_handle_t px_gpio_swdio = {PX_GPIO_SWDIO};
static const px_gpio_handle_t px_gpio_swdck = {PX_GPIO_SWDCK};

#ifdef __cplusplus
}
#endif

#endif
