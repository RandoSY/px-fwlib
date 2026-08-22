/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_adc.h"
#include "px_board.h"
#include "px_cli.h"
#include "px_cli_cmds.h"
#include "px_log.h"
#include "px_rtc.h"
#include "px_spi.h"
#include "px_sysclk.h"
#include "px_uart.h"
#include "px_uart_stdio.h"
#include "px_stm32cube.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("main");

/* _____GLOBAL VARIABLES_____________________________________________________ */
px_uart_handle_t px_uart1_handle;
px_adc_handle_t  px_adc1_handle;
uint8_t          main_buf[MAIN_BUF_SIZE];

/* _____LOCAL VARIABLES______________________________________________________ */
static const char main_cli_init_str[] =
    "\nWeAct Studio STM32G030 CLI Explorer\n"
    "Piconomix px-fwlib processor laboratory\n"
    "64 MHz / 12-bit ADC / SPI1 / RTC / USART1\n"
    "Type help for commands.\n\n";

/* _____LOCAL FUNCTIONS______________________________________________________ */
static bool main_init(void)
{
    px_board_init();

    /* The CLI can inspect the auxiliary GPIO ports used by the G030 package. */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

    px_sysclk_init();
    px_rtc_init();
    px_adc_init();
    px_spi_init();
    px_uart_init();

    if(!px_uart_open(&px_uart1_handle, PX_UART_NR_1))
    {
        return false;
    }
    px_uart_stdio_init(&px_uart1_handle);

    /* Prepare the four exposed analog inputs for ADC use. */
    px_gpio_init(&px_gpio_adc0);
    px_gpio_init(&px_gpio_adc1);
    px_gpio_init(&px_gpio_adc2);
    px_gpio_init(&px_gpio_adc3);

    if(!px_adc_open(&px_adc1_handle, PX_ADC_NR_1))
    {
        printf("ADC initialization failed.\n");
        return false;
    }

    return true;
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    uint8_t data;

    if(!main_init())
    {
        while(true) {;}
    }

    PX_USR_LED_ON();
    PX_LOG_I("CLI explorer ready\n");
    px_cli_init(px_cli_cmd_list, main_cli_init_str);

    while(true)
    {
        if(px_uart_rd_u8(&px_uart1_handle, &data))
        {
            px_cli_on_rx_char((char)data);
        }
        __WFI();
    }
}
