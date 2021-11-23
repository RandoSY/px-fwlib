/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
    
    Title:          Piconomix STM32 Hero Board Low Power example
    Author(s):      Pieter Conradie
    Creation Date:  2019-03-08
 
============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_board.h"
#include "px_exti.h"
#include "px_spi.h"
#include "px_uart.h"
#include "px_at25s.h"
#include "px_sd.h"
#include "px_lcd_st7567_jhd12864.h"
#include "px_gfx.h"
#include "px_gfx_res.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart1_handle;
px_spi_handle_t  px_spi_lcd_handle;
px_spi_handle_t  px_spi_sf_handle;
px_spi_handle_t  px_spi_sd_handle;

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void main_exti9_handler(void)
{    
}

static void main_init(void)
{
    // Initialize board
    px_board_init();
    px_exti_init();
    px_spi_init();
    px_uart_init();

    // Open UART1
    px_uart_open2(&px_uart1_handle,
                  PX_UART_NR_1,
                  115200, 
                  PX_UART_DATA_BITS_8, 
                  PX_UART_PARITY_NONE, 
                  PX_UART_STOP_BITS_1);

    // Initialise LCD driver
    px_spi_open2(&px_spi_lcd_handle,
                 PX_SPI_NR_2,
                 PX_BOARD_SPI2_CS_LCD,
                 px_spi_util_baud_hz_to_clk_div(PX_LCD_MAX_SPI_CLOCK_HZ),
                 PX_LCD_SPI_MODE, 
                 PX_LCD_SPI_DATA_ORDER,
                 0x00);
    px_lcd_init(&px_spi_lcd_handle);

    // Initialise AT25S Serial Flash driver
    px_spi_open2(&px_spi_sf_handle,
                 PX_SPI_NR_2,
                 PX_BOARD_SPI2_CS_SF,
                 px_spi_util_baud_hz_to_clk_div(PX_AT25S_MAX_SPI_CLOCK_HZ), 
                 PX_AT25S_SPI_MODE, 
                 PX_AT25S_SPI_DATA_ORDER,
                 0x00);
    px_at25s_init(&px_spi_sf_handle);
    // Resume Serial Flash in case it was left in a power down state and the 
    // processor reset
    px_at25s_resume_from_deep_power_down();

    // Initialise SD Card driver
    px_spi_open2(&px_spi_sd_handle,
                 PX_SPI_NR_1,
                 PX_BOARD_SPI1_CS_SD,
                 px_spi_util_baud_hz_to_clk_div(PX_SD_MAX_SPI_CLOCK_HZ),
                 PX_SD_SPI_MODE, 
                 PX_SD_SPI_DATA_ORDER,
                 PX_SD_SPI_MO_DUMMY_BYTE);
    px_sd_init(&px_spi_sd_handle);   
}

static void main_gpio_disable(void)
{
    // Disable uninitialised UART2 peripheral pins
    px_gpio_mode_set(&px_gpio_uart2_tx, PX_GPIO_MODE_ANA); //PA2
    px_gpio_mode_set(&px_gpio_uart2_rx, PX_GPIO_MODE_ANA); //PA3
    // Disable SD Card detect pin
    px_gpio_mode_set(&px_gpio_7_sd_cd,  PX_GPIO_MODE_ANA); //PB7
    // Disable uninitialised I2C peripheral pins
    px_gpio_mode_set(&px_gpio_i2c1_scl, PX_GPIO_MODE_ANA); //PB8
    px_gpio_mode_set(&px_gpio_i2c1_sda, PX_GPIO_MODE_ANA); //PB9
    // Disable uninitialised UART4 peripheral pins
    px_gpio_mode_set(&px_gpio_uart4_tx, PX_GPIO_MODE_ANA); //PC10
    px_gpio_mode_set(&px_gpio_uart4_rx, PX_GPIO_MODE_ANA); //PC11
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint16_t counter = 0;
    char     str[16];

    // Initialize board
    main_init();

    // Initialise graphics
    px_gfx_init();    
    
    // SD Card inserted?
    if(PX_SD_CARD_DETECTED())
    {
        // Initialise SD card
        px_sd_init(&px_spi_sd_handle);
    }

    // Power down Serial Flash to minimise power consumption
    px_at25s_deep_power_down();

    // Enable external falling edge interrupt on Port C pin 9 (6/NO button)
    px_exti_open(PX_EXTI_PORT_C,
                 PX_EXTI_LINE_9,
                 &main_exti9_handler);
    px_exti_type_set(PX_EXTI_LINE_9, PX_EXTI_TYPE_FALLING_EDGE);
    px_exti_enable(PX_EXTI_LINE_9);

    // Disable debug interface
    px_board_dbg_disable();

    // Set mode of uninitialized peripheral GPIO pins to Analog as well as
    // SD Card Detect pin that will draw ~73uA while SD card is inserted 
    // (Pull-up resistor = ~45 kOhm).
    main_gpio_disable();
    
    // Loop forever
    while(true)
    {
#if (CFG_LCD_STAY_ON == 0)
        // Turn LCD power save off
        px_lcd_power_save_off();
#endif
        // Enable LCD backlight
        PX_LCD_BACKLIGHT_ON();
        // LED on
        PX_USR_LED_ON();
        // Draw new counter value on LCD
        sprintf(str, "%04u", ++counter);
        px_gfx_buf_clear();
        px_gfx_draw_str(0, 0, str);
        px_gfx_draw_update();
        // Wait until button is released
        do
        {
            px_board_delay_ms(1000);
        }
        while(PX_USR_PB_IS_PRESSED());

        // LED off
        PX_USR_LED_OFF();
        // Disable LCD backlight
        PX_LCD_BACKLIGHT_OFF();
#if (CFG_LCD_STAY_ON == 0)
        // Turn LCD power save on
        px_lcd_power_save_on();
#endif

        // Put core into STOP mode until an interrupt occurs
        px_board_stop_mode();
    }	
}
