/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_cli.h"
#include "px_cli_cmds.h"
#include "px_cli_cmds_adc.h"
#include "px_cli_cmds_gpio.h"
#include "px_cli_cmds_mem.h"
#include "px_cli_cmds_rtc.h"
#include "px_cli_cmds_spi.h"
#include "px_cli_cmds_uart.h"
#include "px_gpio.h"
#include "px_log.h"
#include "px_stm32cube.h"
#include "px_sysclk.h"
#include "stm32g030xx.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli");

static const px_gpio_handle_t cli_buzzer =
{
    PX_GPIO(A, 3, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO,
            PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)
};

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * px_cli_cmd_reset_fn(uint8_t argc, char * argv[]) PX_ATTR_NORETURN;

static const char * px_cli_cmd_reset_fn(uint8_t argc, char * argv[])
{
    NVIC_SystemReset();
    while(true) {;}
}

static const char * px_cli_cmd_led_fn(uint8_t argc, char * argv[])
{
    switch(px_cli_util_argv_to_option(0, "on\0off\0toggle\0"))
    {
    case 0: PX_USR_LED_ON();     break;
    case 1: PX_USR_LED_OFF();    break;
    case 2: PX_USR_LED_TOGGLE(); break;
    default: return "Error: use on, off or toggle";
    }
    return NULL;
}

static const char * px_cli_cmd_beep_fn(uint8_t argc, char * argv[])
{
    uint16_t duration_ms;
    uint16_t frequency_hz = 2000;
    uint32_t half_period_us;
    uint32_t cycles;
    uint32_t i;

    if(!px_cli_util_argv_to_u16(0, 0, 10000))
    {
        return "Error: duration must be 0 to 10000 ms";
    }
    duration_ms = px_cli_argv_val.u16;

    if(argc > 1)
    {
        if(!px_cli_util_argv_to_u16(1, 100, 10000))
        {
            return "Error: frequency must be 100 to 10000 Hz";
        }
        frequency_hz = px_cli_argv_val.u16;
    }

    half_period_us = 1000000ul / (2ul * frequency_hz);
    if(half_period_us == 0)
    {
        half_period_us = 1;
    }
    cycles = ((uint32_t)duration_ms * 1000ul) / (2ul * half_period_us);

    px_gpio_init(&cli_buzzer);
    for(i = 0; i < cycles; i++)
    {
        px_gpio_out_set_hi(&cli_buzzer);
        px_board_delay_us((uint16_t)half_period_us);
        px_gpio_out_set_lo(&cli_buzzer);
        px_board_delay_us((uint16_t)half_period_us);
    }
    px_gpio_out_set_lo(&cli_buzzer);
    printf("Beep %u ms at %u Hz on PA3\n", duration_ms, frequency_hz);
    return NULL;
}

static const char * px_cli_cmd_delay_fn(uint8_t argc, char * argv[])
{
    uint16_t delay;

    if(!px_cli_util_argv_to_u16(0, 0, 65535))
    {
        return "Error: delay must be 0 to 65535";
    }
    delay = px_cli_argv_val.u16;

    if(argc > 1)
    {
        switch(px_cli_util_argv_to_option(1, "ms\0s\0"))
        {
        case 0: px_board_delay_ms(delay); break;
        case 1: while(delay-- != 0) px_board_delay_ms(1000); break;
        default: return "Error: unit must be ms or s";
        }
    }
    else
    {
        px_board_delay_ms(delay);
    }
    return NULL;
}

static const char * px_cli_cmd_info_fn(uint8_t argc, char * argv[])
{
    printf("Board: WeAct STM32G030 core board\n");
    printf("Clock: %lu Hz, SysTick=%lu ticks\n",
           (unsigned long)PX_BOARD_SYS_CLK_HZ,
           (unsigned long)px_sysclk_get_tick_count());
    printf("DBG_IDCODE:    0x%08lx\n", (unsigned long)DBG->IDCODE);
    printf("SCB_CPUID:     0x%08lx\n", (unsigned long)SCB->CPUID);
    printf("RCC_CR:        0x%08lx\n", (unsigned long)RCC->CR);
    printf("RCC_CFGR:      0x%08lx\n", (unsigned long)RCC->CFGR);
    printf("GPIO pins: ADC PA0..PA3, LED PA4, SPI1 PA5..PA8, USART1 PA11/PA12, button PA14\n");
    printf("Peripherals: ADC1, SPI1, RTC, USART1; internal 12-bit ADC reference assumption=3300 mV\n");
    return NULL;
}

/* _____COMMAND TREE________________________________________________________ */
PX_CLI_CMD_CREATE(px_cli_cmd_reset, "rst",   0, 0, "",                       "Reset microcontroller")
PX_CLI_CMD_CREATE(px_cli_cmd_led,   "led",   1, 1, "<on|off|toggle>",        "Control the active-low onboard LED")
PX_CLI_CMD_CREATE(px_cli_cmd_beep,  "beep",  1, 2, "<ms> [freq Hz]",          "Generate a GPIO buzzer tone on PA3")
PX_CLI_CMD_CREATE(px_cli_cmd_delay, "delay", 1, 2, "<delay> [ms|s]",          "Delay for a measured interval")
PX_CLI_CMD_CREATE(px_cli_cmd_info,  "info",  0, 0, "",                       "Report board, clock, ID and pin information")
PX_CLI_CMD_CREATE(px_cli_cmd_help,  "help",  0, 1, "[cmd prefix]",            "Display command help")

PX_CLI_CMD_LIST_CREATE(px_cli_cmd_list)
    PX_CLI_CMD_ADD  (px_cli_cmd_info,  px_cli_cmd_info_fn)
    PX_CLI_CMD_ADD  (px_cli_cmd_led,   px_cli_cmd_led_fn)
    PX_CLI_CMD_ADD  (px_cli_cmd_beep,  px_cli_cmd_beep_fn)
    PX_CLI_GROUP_ADD(px_cli_group_adc)
    PX_CLI_GROUP_ADD(px_cli_group_gpio)
    PX_CLI_GROUP_ADD(px_cli_group_uart)
    PX_CLI_GROUP_ADD(px_cli_group_spi)
    PX_CLI_GROUP_ADD(px_cli_group_rtc)
    PX_CLI_GROUP_ADD(px_cli_group_mem)
    PX_CLI_CMD_ADD  (px_cli_cmd_delay, px_cli_cmd_delay_fn)
    PX_CLI_CMD_ADD  (px_cli_cmd_reset, px_cli_cmd_reset_fn)
    PX_CLI_CMD_ADD  (px_cli_cmd_help,  px_cli_cmd_help_fn)
PX_CLI_CMD_LIST_END()
