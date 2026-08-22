#ifndef __PX_BOARD_H__
#define __PX_BOARD_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___  __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \ |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | |  |  \| | | | | || |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| |  | |\  | | |_| || |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/ |_|  |_| |___| /_/\_\

    Copyright (c) 2026

    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md

    Title:          board.h : WeAct STM32G030 Core Board

============================================================================= */

/**
 *  @ingroup BOARDS_STM32
 *  @defgroup BOARDS_STM32_WEACT_STM32G030 px_board.h : WeAct STM32G030 Core Board
 *
 *  File(s):
 *  - boards/arm/stm32/weact_stm32g030/inc/px_board.h
 *  - boards/arm/stm32/weact_stm32g030/inc/px_board_gpio.h
 *  - boards/arm/stm32/weact_stm32g030/src/px_board.c
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_board_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* _____DEFINITIONS__________________________________________________________ */
/// Board name
#define PX_BOARD_WEACT_STM32G030

/// System clock frequency in Hz
#define PX_BOARD_SYS_CLK_HZ     64000000ul

/// Peripheral clock frequency in Hz
#define PX_BOARD_PER_CLK_HZ     PX_BOARD_SYS_CLK_HZ

/// @name Mapping of the board's analog header pins to ADC channels
/// @{
#define PX_BOARD_ADC0          PX_ADC_CH0
#define PX_BOARD_ADC1          PX_ADC_CH1
#define PX_BOARD_ADC2          PX_ADC_CH2
#define PX_BOARD_ADC3          PX_ADC_CH3
/// @}

/* _____TYPE DEFINITIONS_____________________________________________________ */
/// Identify which GPIO pin is used for a manual SPI chip select.
typedef enum
{
    PX_BOARD_SPI_CS_NONE,
    PX_BOARD_SPI1_CS,
} px_board_spi_cs_t;

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
/// Initialise the board clock and GPIO hardware.
void px_board_init(void);

/// Set the specified SPI chip select low.
void px_board_spi_cs_lo(uint8_t cs_id);

/// Set the specified SPI chip select high.
void px_board_spi_cs_hi(uint8_t cs_id);

/// Initialise TIM14 with 1 us clock ticks for delay use.
void px_board_delay_init(void);

/// Disable TIM14.
void px_board_delay_deinit(void);

/// Blocking delay for the specified number of microseconds.
void px_board_delay_us(uint16_t delay_us);

/// Blocking delay for the specified number of milliseconds.
void px_board_delay_ms(uint16_t delay_ms);

/* _____MACROS_______________________________________________________________ */
/// User button is active high on PA14.
#define PX_USR_BTN_IS_PRESSED()     px_gpio_in_is_hi(&px_gpio_usr_btn)

/// The user LED is wired from 3V3 through a resistor to PA4, so it is active low.
#define PX_USR_LED_ON()              px_gpio_out_set_lo(&px_gpio_usr_led)
#define PX_USR_LED_OFF()             px_gpio_out_set_hi(&px_gpio_usr_led)
#define PX_USR_LED_TOGGLE()          px_gpio_out_toggle(&px_gpio_usr_led)

#ifdef __cplusplus
}
#endif

/// @}
#endif
