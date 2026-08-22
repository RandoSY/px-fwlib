/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_adc.h"
#include "px_board.h"
#include "px_cli.h"
#include "px_gpio.h"
#include "px_log.h"
#include "px_systmr.h"
#include "px_sysclk.h"
#include "px_uart.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_adc");

#define CLI_ADC_MAX 4095ul

/* _____LOCAL VARIABLES______________________________________________________ */
static uint32_t cli_adc_vref_mv = 3300;

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * cli_adc_parse_channel(char * text, px_adc_ch_t * channel)
{
    unsigned long value;
    char * end;
    char * channel_text;

    if((text[0] == 'c') || (text[0] == 'C'))
    {
        channel_text = text + 1;
        if((channel_text[0] == 'h') || (channel_text[0] == 'H'))
        {
            channel_text++;
        }
        value = strtoul(channel_text, &end, 0);
        if((*end != '\0') || (value > 18))
        {
            return "Error: channel must be 0..3 or ch0..ch18";
        }
    }
    else
    {
        value = strtoul(text, &end, 0);
        if((*end != '\0') || (value > 3))
        {
            return "Error: external channel must be 0..3 or ch0..ch18";
        }
    }

    *channel = (px_adc_ch_t)value;
    return NULL;
}

static void cli_adc_prepare_pin(px_adc_ch_t channel)
{
    switch(channel)
    {
    case PX_ADC_CH0: px_gpio_init(&px_gpio_adc0); break;
    case PX_ADC_CH1: px_gpio_init(&px_gpio_adc1); break;
    case PX_ADC_CH2: px_gpio_init(&px_gpio_adc2); break;
    case PX_ADC_CH3: px_gpio_init(&px_gpio_adc3); break;
    default: break;
    }
}

static uint16_t cli_adc_to_mv(uint16_t raw)
{
    return (uint16_t)((((uint32_t)raw * cli_adc_vref_mv) + (CLI_ADC_MAX / 2ul))
                      / CLI_ADC_MAX);
}

static void cli_adc_print_binary(uint16_t raw)
{
    int8_t bit;

    printf("0b");
    for(bit = 11; bit >= 0; bit--)
    {
        putchar((raw & ((uint16_t)1u << bit)) ? '1' : '0');
    }
}

static void cli_adc_report(px_adc_ch_t channel, uint32_t sequence)
{
    uint16_t raw;

    cli_adc_prepare_pin(channel);
    raw = px_adc_sample(&px_adc1_handle, channel);
    printf("%lu ch%u raw=%04u hex=0x%03X voltage=%u mV binary=",
           (unsigned long)sequence,
           (unsigned int)channel,
           (unsigned int)raw,
           (unsigned int)raw,
           (unsigned int)cli_adc_to_mv(raw));
    cli_adc_print_binary(raw);
    printf("\n");
}

static const char * px_cli_cmd_fn_adc_sample(uint8_t argc, char * argv[])
{
    px_adc_ch_t channel;
    uint16_t interval_ms = 0;
    uint32_t sequence = 0;
    px_systmr_t timer;
    const char * result;
    uint8_t stop_byte;

    result = cli_adc_parse_channel(argv[0], &channel);
    if(result != NULL)
    {
        return result;
    }

    if(argc > 1)
    {
        if(!px_cli_util_argv_to_u16(1, 1, 60000))
        {
            return "Error: interval must be 1 to 60000 ms";
        }
        interval_ms = px_cli_argv_val.u16;
    }

    while(true)
    {
        cli_adc_report(channel, sequence++);
        if(interval_ms == 0)
        {
            break;
        }

        px_systmr_start(&timer, PX_SYSTMR_MS_TO_TICKS(interval_ms));
        while(!px_systmr_has_expired(&timer))
        {
            if(PX_USR_BTN_IS_PRESSED())
            {
                while(PX_USR_BTN_IS_PRESSED()) {;}
                printf("ADC watch stopped by PA14 button\n");
                return NULL;
            }
            if(px_uart_rd_u8(&px_uart1_handle, &stop_byte))
            {
                printf("ADC watch stopped by received byte 0x%02X\n", stop_byte);
                return NULL;
            }
        }
    }

    return NULL;
}

static const char * px_cli_cmd_fn_adc_binary(uint8_t argc, char * argv[])
{
    px_adc_ch_t channel;
    const char * result = cli_adc_parse_channel(argv[0], &channel);

    if(result != NULL)
    {
        return result;
    }
    cli_adc_prepare_pin(channel);
    printf("ADC_BINARY ch%u=", (unsigned int)channel);
    cli_adc_print_binary(px_adc_sample(&px_adc1_handle, channel));
    putchar('\n');
    return NULL;
}

static const char * px_cli_cmd_fn_adc_all(uint8_t argc, char * argv[])
{
    uint8_t channel;

    for(channel = 0; channel < 4; channel++)
    {
        cli_adc_report((px_adc_ch_t)channel, channel);
    }
    return NULL;
}

static const char * px_cli_cmd_fn_adc_info(uint8_t argc, char * argv[])
{
    printf("ADC1: 12-bit single conversion, VREF assumption=%lu mV\n",
           (unsigned long)cli_adc_vref_mv);
    printf("External channels: ch0=PA0 ch1=PA1 ch2=PA2 ch3=PA3\n");
    printf("Internal channels accepted by driver: ch16, ch17=VREFINT, ch18=temperature\n");
    printf("Long sample time is configured for high-impedance sources.\n");
    return NULL;
}

static const char * px_cli_cmd_fn_adc_vref(uint8_t argc, char * argv[])
{
    if(!px_cli_util_argv_to_u32(0, 1000, 3600))
    {
        return "Error: VREF must be 1000 to 3600 mV";
    }
    cli_adc_vref_mv = px_cli_argv_val.u32;
    printf("ADC VREF assumption set to %lu mV\n", (unsigned long)cli_adc_vref_mv);
    return NULL;
}

/* _____COMMAND TREE________________________________________________________ */
PX_CLI_CMD_CREATE(px_cli_cmd_adc_sample,  "s",    1, 2, "<ch> [interval ms]", "Sample ADC channel once or continuously")
PX_CLI_CMD_CREATE(px_cli_cmd_adc_binary,  "bin",  1, 1, "<ch>",               "Print one ADC sample as 12-bit binary")
PX_CLI_CMD_CREATE(px_cli_cmd_adc_all,     "all",  0, 0, "",                   "Sample external channels PA0 through PA3")
PX_CLI_CMD_CREATE(px_cli_cmd_adc_info,    "info", 0, 0, "",                   "Report ADC configuration and channel map")
PX_CLI_CMD_CREATE(px_cli_cmd_adc_vref,    "v",    1, 1, "<mV>",               "Set the voltage scaling reference assumption")

PX_CLI_GROUP_CREATE(px_cli_group_adc, "adc")
    PX_CLI_CMD_ADD(px_cli_cmd_adc_sample, px_cli_cmd_fn_adc_sample)
    PX_CLI_CMD_ADD(px_cli_cmd_adc_binary, px_cli_cmd_fn_adc_binary)
    PX_CLI_CMD_ADD(px_cli_cmd_adc_all,    px_cli_cmd_fn_adc_all)
    PX_CLI_CMD_ADD(px_cli_cmd_adc_info,   px_cli_cmd_fn_adc_info)
    PX_CLI_CMD_ADD(px_cli_cmd_adc_vref,   px_cli_cmd_fn_adc_vref)
PX_CLI_GROUP_END()
