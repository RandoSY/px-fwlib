/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "app_pins.h"
#include "px_adc.h"
#include "px_board.h"
#include "px_sysclk.h"
#include "px_uart.h"
#include "px_uart_stdio.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#define APP_ADC_CHANNEL              PX_BOARD_ADC0
#define APP_ADC_MAX                  4095u
#define APP_VREF_MV_DEFAULT          3300ul

#define APP_DEBOUNCE_MS              35u
#define APP_POST_SAMPLE_DELAY_MS     100u
#define APP_LED_BIT_DWELL_MS         200u
#define APP_BEEP_FREQUENCY_HZ        2000u
#define APP_SINGLE_BEEP_MS           75u
#define APP_DOUBLE_BEEP_MS           60u
#define APP_DOUBLE_BEEP_GAP_MS       80u
#define APP_CONTINUOUS_PERIOD_MS     1000u
#define APP_BEEP_HALF_PERIOD_US      250u

#define APP_TERMINAL_LINE_SIZE       64u

/* _____TYPE DEFINITIONS_____________________________________________________ */
typedef enum
{
    APP_TRIGGER_BUTTON,
    APP_TRIGGER_SERIAL,
    APP_TRIGGER_CONTINUOUS,
} app_trigger_t;

/* _____GLOBAL VARIABLES_____________________________________________________ */
static px_adc_handle_t  app_adc_handle;
static px_uart_handle_t app_uart_handle;

/* _____LOCAL VARIABLES______________________________________________________ */
static char     app_terminal_line[APP_TERMINAL_LINE_SIZE];
static uint8_t  app_terminal_line_len;
static bool     app_measurement_requested;
static bool     app_continuous_mode;
static bool     app_verbose = true;
static bool     app_button_was_pressed;
static bool     app_skip_line_feed;
static uint32_t app_button_ignore_until;
static uint32_t app_next_continuous_tick;
static uint32_t app_vref_mv = APP_VREF_MV_DEFAULT;
static app_trigger_t app_trigger = APP_TRIGGER_BUTTON;

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */
static void app_print_help(void);
static void app_print_status(void);
static void app_prompt(void);
static void app_request_measurement(app_trigger_t trigger);
static void app_run_measurement(void);

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * app_trigger_name(app_trigger_t trigger)
{
    switch(trigger)
    {
    case APP_TRIGGER_BUTTON:     return "button";
    case APP_TRIGGER_SERIAL:     return "serial";
    case APP_TRIGGER_CONTINUOUS: return "continuous";
    default:                     return "unknown";
    }
}

static bool app_time_reached(uint32_t now, uint32_t target)
{
    return ((int32_t)(now - target) >= 0);
}

static void app_set_indicator(bool red, bool green)
{
    if(red)
    {
        px_gpio_out_set_hi(&px_gpio_meter_red);
    }
    else
    {
        px_gpio_out_set_lo(&px_gpio_meter_red);
    }

    if(green)
    {
        px_gpio_out_set_hi(&px_gpio_meter_green);
    }
    else
    {
        px_gpio_out_set_lo(&px_gpio_meter_green);
    }
}

static void app_set_binary_indicator(bool bit_value)
{
    /* The LED color is the bit value: GREEN=1 and RED=0. */
    if(bit_value)
    {
        app_set_indicator(false, true);
    }
    else
    {
        app_set_indicator(true, false);
    }
}

static void app_display_binary(uint16_t raw)
{
    int8_t bit;

    for(bit = 11; bit >= 0; bit--)
    {
        bool bit_value = (raw & ((uint16_t)1u << bit)) != 0;

        app_set_binary_indicator(bit_value);
        if(app_verbose)
        {
            printf("LED_BIT index=%u value=%u green=%u red=%u dwell_ms=%u\n",
                   (unsigned int)bit,
                   bit_value ? 1u : 0u,
                   bit_value ? 1u : 0u,
                   bit_value ? 0u : 1u,
                   APP_LED_BIT_DWELL_MS);
        }
        px_board_delay_ms(APP_LED_BIT_DWELL_MS);
    }

    app_set_indicator(false, false);
}

static void app_beep(uint16_t duration_ms)
{
    uint32_t cycles;
    uint32_t i;

    cycles = ((uint32_t)duration_ms * 1000ul)
           / (2ul * APP_BEEP_HALF_PERIOD_US);

    for(i = 0; i < cycles; i++)
    {
        px_gpio_out_set_hi(&px_gpio_meter_buzzer);
        px_board_delay_us(APP_BEEP_HALF_PERIOD_US);
        px_gpio_out_set_lo(&px_gpio_meter_buzzer);
        px_board_delay_us(APP_BEEP_HALF_PERIOD_US);
    }

    px_gpio_out_set_lo(&px_gpio_meter_buzzer);
}

static uint16_t app_raw_to_mv(uint16_t raw)
{
    return (uint16_t)((((uint32_t)raw * app_vref_mv) + (APP_ADC_MAX / 2u))
                      / APP_ADC_MAX);
}

static uint16_t app_raw_to_percent_tenths(uint16_t raw)
{
    return (uint16_t)((((uint32_t)raw * 1000ul) + (APP_ADC_MAX / 2u))
                      / APP_ADC_MAX);
}

static void app_print_binary(uint16_t raw)
{
    int8_t bit;

    printf("ADC_BINARY=0b");
    for(bit = 11; bit >= 0; bit--)
    {
        putchar((raw & ((uint16_t)1u << bit)) ? '1' : '0');
    }
    printf("\n");
}

static void app_print_status(void)
{
    printf("\n[STATUS] VREF=%lu mV, ADC=12-bit, channel=%u\n",
           (unsigned long)app_vref_mv,
           (unsigned int)APP_ADC_CHANNEL);
    printf("[STATUS] timing: post-sample=%u ms, LED-bit=%u ms, beep=%u Hz/%u ms, double-gap=%u ms\n",
           APP_POST_SAMPLE_DELAY_MS,
           APP_LED_BIT_DWELL_MS,
           APP_BEEP_FREQUENCY_HZ,
           APP_SINGLE_BEEP_MS,
           APP_DOUBLE_BEEP_GAP_MS);
    printf("[STATUS] continuous=%s, verbose=%s, input=PA0, key=PA14, red=PA1, green=PA2, buzzer=PA3\n",
           app_continuous_mode ? "on" : "off",
           app_verbose ? "on" : "off");
    printf("[STATUS] LED encoding: GREEN=1, RED=0, serial order=MSB to LSB\n");
}

static void app_print_help(void)
{
    printf("\nADC meter commands:\n");
    printf("  m or blank  take one measurement\n");
    printf("  h/help/?    show this help\n");
    printf("  c           toggle continuous measurements\n");
    printf("  q           stop continuous measurements\n");
    printf("  s           show status and timing settings\n");
    printf("  d           toggle verbose debug messages\n");
    printf("  v <mV>      set assumed VREF, for example: v 3300\n");
    printf("\nHardware: ADC PA0, key PA14, USART1 PA11/PA12, red PA1, green PA2, buzzer PA3.\n");
    printf("ADC input range is 0..VDD. Use current-limiting resistors for LEDs.\n");
    printf("LEDs serially show all 12 bits, MSB first: GREEN=1, RED=0.\n");
}

static void app_prompt(void)
{
    printf("\nadc> ");
}

static char * app_skip_spaces(char * text)
{
    while((*text == ' ') || (*text == '\t'))
    {
        text++;
    }
    return text;
}

static void app_command(char * line)
{
    char * end;
    char * argument;
    unsigned long value;

    line = app_skip_spaces(line);

    if((*line == '\0') || (strcmp(line, "m") == 0) || (strcmp(line, "measure") == 0))
    {
        app_request_measurement(APP_TRIGGER_SERIAL);
        return;
    }

    if((strcmp(line, "h") == 0) || (strcmp(line, "help") == 0) || (strcmp(line, "?") == 0))
    {
        app_print_help();
        return;
    }

    if(strcmp(line, "c") == 0)
    {
        app_continuous_mode = !app_continuous_mode;
        if(app_continuous_mode)
        {
            app_next_continuous_tick = px_sysclk_get_tick_count();
            printf("\n[MODE] continuous measurements enabled\n");
        }
        else
        {
            printf("\n[MODE] continuous measurements disabled\n");
        }
        return;
    }

    if(strcmp(line, "q") == 0)
    {
        app_continuous_mode = false;
        printf("\n[MODE] continuous measurements stopped\n");
        return;
    }

    if(strcmp(line, "s") == 0)
    {
        app_print_status();
        return;
    }

    if(strcmp(line, "d") == 0)
    {
        app_verbose = !app_verbose;
        printf("\n[MODE] verbose debug %s\n", app_verbose ? "enabled" : "disabled");
        return;
    }

    if((strncmp(line, "v ", 2) == 0) || (strncmp(line, "v=", 2) == 0))
    {
        argument = app_skip_spaces(line + 2);
        value = strtoul(argument, &end, 10);
        end = app_skip_spaces(end);
        if((*argument == '\0') || (*end != '\0') || (value < 1000ul) || (value > 3600ul))
        {
            printf("\n[ERROR] VREF must be an integer from 1000 to 3600 mV\n");
            return;
        }
        app_vref_mv = (uint32_t)value;
        printf("\n[CONFIG] VREF set to %lu mV\n", app_vref_mv);
        return;
    }

    printf("\n[ERROR] Unknown command '%s'. Type h for help.\n", line);
}

static void app_terminal_service(void)
{
    uint8_t data;

    while(px_uart_rd_u8(&app_uart_handle, &data))
    {
        if(data == '\r')
        {
            app_terminal_line[app_terminal_line_len] = '\0';
            app_command(app_terminal_line);
            app_terminal_line_len = 0;
            app_skip_line_feed = true;
            if(!app_measurement_requested)
            {
                app_prompt();
            }
        }
        else if(data == '\n')
        {
            /* Most terminals send CR/LF; do not process that as two commands. */
            if(app_skip_line_feed)
            {
                app_skip_line_feed = false;
                continue;
            }

            app_terminal_line[app_terminal_line_len] = '\0';
            app_command(app_terminal_line);
            app_terminal_line_len = 0;
            if(!app_measurement_requested)
            {
                app_prompt();
            }
        }
        else if((data == '\b') || (data == 127u))
        {
            app_skip_line_feed = false;
            if(app_terminal_line_len != 0)
            {
                app_terminal_line_len--;
                printf("\b \b");
            }
        }
        else if((data >= 32u) && (data <= 126u))
        {
            app_skip_line_feed = false;
            if(app_terminal_line_len < (APP_TERMINAL_LINE_SIZE - 1u))
            {
                app_terminal_line[app_terminal_line_len++] = (char)data;
                px_uart_wr_u8(&app_uart_handle, data);
            }
        }
    }
}

static void app_poll_button(void)
{
    uint32_t now = px_sysclk_get_tick_count();
    bool button_pressed = PX_USR_BTN_IS_PRESSED();

    if(button_pressed && !app_button_was_pressed
       && !app_measurement_requested
       && app_time_reached(now, app_button_ignore_until))
    {
        app_button_ignore_until = now + APP_DEBOUNCE_MS;
        app_request_measurement(APP_TRIGGER_BUTTON);
    }
    app_button_was_pressed = button_pressed;
}

static void app_request_measurement(app_trigger_t trigger)
{
    if(!app_measurement_requested)
    {
        app_trigger = trigger;
        app_measurement_requested = true;
    }
}

static void app_run_measurement(void)
{
    uint16_t raw;
    uint16_t millivolts;
    uint16_t percent_tenths;
    uint32_t tick_start;
    uint32_t tick_adc_done;
    uint32_t tick_delay_done;
    uint32_t tick_beep_done;
    uint32_t tick_report_start;
    uint32_t tick_led_done;
    uint32_t tick_done;
    app_trigger_t trigger = app_trigger;

    app_measurement_requested = false;
    PX_USR_LED_ON();
    app_set_indicator(true, false);

    tick_start = px_sysclk_get_tick_count();
    printf("\n[MEASURE] trigger=%s, starting 12-bit conversion\n",
           app_trigger_name(trigger));
    raw = px_adc_sample(&app_adc_handle, APP_ADC_CHANNEL);
    tick_adc_done = px_sysclk_get_tick_count();

    if(app_verbose)
    {
        printf("[DEBUG] ADC conversion complete after %lu ms\n",
               (unsigned long)(tick_adc_done - tick_start));
        printf("[DEBUG] waiting %u ms before audible acknowledgement\n",
               APP_POST_SAMPLE_DELAY_MS);
    }
    px_board_delay_ms(APP_POST_SAMPLE_DELAY_MS);
    tick_delay_done = px_sysclk_get_tick_count();

    if(app_verbose)
    {
        printf("[DEBUG] single beep: %u Hz for %u ms\n",
               APP_BEEP_FREQUENCY_HZ,
               APP_SINGLE_BEEP_MS);
    }
    app_beep(APP_SINGLE_BEEP_MS);
    tick_beep_done = px_sysclk_get_tick_count();

    millivolts = app_raw_to_mv(raw);
    percent_tenths = app_raw_to_percent_tenths(raw);
    tick_report_start = px_sysclk_get_tick_count();

    printf("ADC_RESULT raw=%u hex=0x%03X voltage=%u mV level=%u.%u%%\n",
           (unsigned int)raw,
           (unsigned int)raw,
           (unsigned int)millivolts,
           (unsigned int)(percent_tenths / 10u),
           (unsigned int)(percent_tenths % 10u));
    app_print_binary(raw);
    printf("LED_SERIAL encoding=GREEN=1 RED=0 order=MSB-to-LSB bit_ms=%u\n",
           APP_LED_BIT_DWELL_MS);
    app_display_binary(raw);
    tick_led_done = px_sysclk_get_tick_count();
    printf("TIMING adc_ms=%lu delay_ms=%lu beep_ms=%lu led_ms=%lu\n",
           (unsigned long)(tick_adc_done - tick_start),
           (unsigned long)(tick_delay_done - tick_adc_done),
           (unsigned long)(tick_beep_done - tick_delay_done),
           (unsigned long)(tick_led_done - tick_beep_done));

    if(app_verbose)
    {
        printf("[DEBUG] result report started at +%lu ms; issuing completion double beep\n",
               (unsigned long)(tick_report_start - tick_start));
    }
    app_beep(APP_DOUBLE_BEEP_MS);
    px_board_delay_ms(APP_DOUBLE_BEEP_GAP_MS);
    app_beep(APP_DOUBLE_BEEP_MS);
    tick_done = px_sysclk_get_tick_count();

    PX_USR_LED_OFF();
    printf("MEASURE_DONE total_ms=%lu; LED serial output used GREEN=1 and RED=0\n",
           (unsigned long)(tick_done - tick_start));
    app_prompt();
}

/* _____PUBLIC FUNCTIONS_____________________________________________________ */
int main(void)
{
    px_board_init();
    px_sysclk_init();

    /* PA0 is the ADC input; PA1..PA3 are application outputs. */
    px_gpio_init(&px_gpio_adc0);
    px_gpio_init(&px_gpio_meter_red);
    px_gpio_init(&px_gpio_meter_green);
    px_gpio_init(&px_gpio_meter_buzzer);

    px_adc_init();
    if(!px_adc_open(&app_adc_handle, PX_ADC_NR_1))
    {
        app_set_indicator(true, false);
        printf("ADC open failed. Halting.\n");
        while(true) {;}
    }

    px_uart_init();
    px_uart_open2(&app_uart_handle,
                  PX_UART_NR_1,
                  115200,
                  PX_UART_DATA_BITS_8,
                  PX_UART_PARITY_NONE,
                  PX_UART_STOP_BITS_1);
    px_uart_stdio_init(&app_uart_handle);

    PX_USR_LED_OFF();
    app_set_indicator(false, false);
    px_gpio_out_set_lo(&px_gpio_meter_buzzer);

    printf("\nWeAct STM32G030 12-bit ADC meter\n");
    printf("VREF=%lu mV, ADC input=PA0, serial=USART1 115200 8N1\n",
           (unsigned long)app_vref_mv);
    printf("Press the PA14 button or type m and Enter. Type h for help.\n");
    app_print_status();
    app_prompt();

    while(true)
    {
        uint32_t now;

        app_terminal_service();
        app_poll_button();
        now = px_sysclk_get_tick_count();

        if(app_continuous_mode && !app_measurement_requested
           && app_time_reached(now, app_next_continuous_tick))
        {
            app_next_continuous_tick = now + APP_CONTINUOUS_PERIOD_MS;
            app_request_measurement(APP_TRIGGER_CONTINUOUS);
        }

        if(app_measurement_requested)
        {
            app_run_measurement();
            if(app_continuous_mode)
            {
                app_next_continuous_tick = px_sysclk_get_tick_count()
                                          + APP_CONTINUOUS_PERIOD_MS;
            }
        }
        else
        {
            __WFI();
        }
    }
}
