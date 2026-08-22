/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_board.h"
#include "px_cli.h"
#include "px_cli_cmds_uart.h"
#include "px_log.h"
#include "px_systmr.h"
#include "px_uart.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_uart");

/* _____LOCAL VARIABLES______________________________________________________ */
static uint32_t uart_baud = 115200;
static px_uart_data_bits_t uart_data_bits = PX_UART_DATA_BITS_8;
static px_uart_parity_t uart_parity = PX_UART_PARITY_NONE;
static px_uart_stop_bits_t uart_stop_bits = PX_UART_STOP_BITS_1;

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * cli_uart_get_byte(uint8_t index, uint8_t * value)
{
    if(!px_cli_util_argv_to_u8(index, 0, 255)) return "Error: byte must be 0..255";
    *value = px_cli_argv_val.u8;
    return NULL;
}

static const char * cli_uart_receive(uint8_t count, uint16_t timeout_ms)
{
    uint8_t data[16];
    uint8_t received = 0;
    px_systmr_t timer;

    px_systmr_start(&timer, PX_SYSTMR_MS_TO_TICKS(timeout_ms));
    while(!px_systmr_has_expired(&timer) && (received < count))
    {
        if(px_uart_rd_u8(&px_uart1_handle, &data[received])) received++;
    }
    printf("received=%u/%u: ", (unsigned int)received, (unsigned int)count);
    for(uint8_t i = 0; i < received; i++) printf("0x%02x ", data[i]);
    putchar('\n');
    return NULL;
}

static const char * px_cli_cmd_fn_uart_cfg(uint8_t argc, char * argv[])
{
    px_uart_data_bits_t data_bits = PX_UART_DATA_BITS_8;
    px_uart_parity_t parity = PX_UART_PARITY_NONE;
    px_uart_stop_bits_t stop_bits = PX_UART_STOP_BITS_1;

    if(argv[0][0] != '1') return "Error: this board exposes USART1 only";
    if(!px_cli_util_argv_to_u32(1, 300, PX_BOARD_PER_CLK_HZ / 16ul))
    {
        return "Error: baud must be 300 to 4000000";
    }
    uart_baud = px_cli_argv_val.u32;
    if(argc > 2)
    {
        if(argv[2][0] == '7') data_bits = PX_UART_DATA_BITS_7;
        else if(argv[2][0] == '8') data_bits = PX_UART_DATA_BITS_8;
        else if(argv[2][0] == '9') data_bits = PX_UART_DATA_BITS_9;
        else return "Error: data bits must be 7, 8 or 9";
    }
    if(argc > 3)
    {
        if(argv[3][0] == 'n') parity = PX_UART_PARITY_NONE;
        else if(argv[3][0] == 'o') parity = PX_UART_PARITY_ODD;
        else if(argv[3][0] == 'e') parity = PX_UART_PARITY_EVEN;
        else return "Error: parity must be n, o or e";
    }
    if(argc > 4)
    {
        if(argv[4][0] == '1') stop_bits = PX_UART_STOP_BITS_1;
        else if(argv[4][0] == '2') stop_bits = PX_UART_STOP_BITS_2;
        else return "Error: stop bits must be 1 or 2";
    }
    if(!px_uart_change_baud(&px_uart1_handle, uart_baud)
       || !px_uart_change_data_format(&px_uart1_handle, data_bits, parity, stop_bits))
    {
        return "Error: UART configuration rejected";
    }
    uart_data_bits = data_bits;
    uart_parity = parity;
    uart_stop_bits = stop_bits;
    printf("USART1 configured: %lu baud, %u data, parity=%c, %u stop\n",
           (unsigned long)uart_baud,
           (unsigned int)uart_data_bits,
           (uart_parity == PX_UART_PARITY_NONE) ? 'N' :
               ((uart_parity == PX_UART_PARITY_ODD) ? 'O' : 'E'),
           (unsigned int)uart_stop_bits);
    return NULL;
}

static const char * px_cli_cmd_fn_uart_info(uint8_t argc, char * argv[])
{
    printf("USART1 console: PA11 TX / PA12 RX, %lu baud, %u%c%u\n",
           (unsigned long)uart_baud,
           (unsigned int)uart_data_bits,
           (uart_parity == PX_UART_PARITY_NONE) ? 'N' :
               ((uart_parity == PX_UART_PARITY_ODD) ? 'O' : 'E'),
           (unsigned int)uart_stop_bits);
    printf("UART commands use this same port; use an external loopback for r/x tests.\n");
    return NULL;
}

static const char * px_cli_cmd_fn_uart_write(uint8_t argc, char * argv[])
{
    uint8_t i;
    uint8_t value;
    for(i = 0; i < argc; i++)
    {
        if(cli_uart_get_byte(i, &value) != NULL) return "Error: invalid UART byte";
        px_uart_putchar(&px_uart1_handle, (char)value);
    }
    return NULL;
}

static const char * px_cli_cmd_fn_uart_receive(uint8_t argc, char * argv[])
{
    uint8_t count;
    if(!px_cli_util_argv_to_u8(0, 1, 16)) return "Error: byte count must be 1..16";
    count = px_cli_argv_val.u8;
    if(!px_cli_util_argv_to_u16(1, 0, 65535)) return "Error: timeout must be 0..65535 ms";
    return cli_uart_receive(count, px_cli_argv_val.u16);
}

static const char * px_cli_cmd_fn_uart_exchange(uint8_t argc, char * argv[])
{
    uint8_t count;
    uint8_t i;
    uint8_t value;
    uint16_t timeout_ms;

    if(!px_cli_util_argv_to_u8(0, 1, 16)) return "Error: receive count must be 1..16";
    count = px_cli_argv_val.u8;
    if(!px_cli_util_argv_to_u16(1, 0, 65535)) return "Error: timeout must be 0..65535 ms";
    timeout_ms = px_cli_argv_val.u16;
    for(i = 2; i < argc; i++)
    {
        if(cli_uart_get_byte(i, &value) != NULL) return "Error: invalid UART byte";
        px_uart_putchar(&px_uart1_handle, (char)value);
    }
    return cli_uart_receive(count, timeout_ms);
}

static const char * px_cli_cmd_fn_uart_string(uint8_t argc, char * argv[])
{
    uint16_t timeout_ms;
    char * text = argv[1];
    px_systmr_t timer;
    char rx;

    if(!px_cli_util_argv_to_u16(0, 0, 65535)) return "Error: timeout must be 0..65535 ms";
    timeout_ms = px_cli_argv_val.u16;
    while(*text != '\0') px_uart_putchar(&px_uart1_handle, *text++);
    if(argc > 2)
    {
        if(strcmp(argv[2], "cr") == 0) px_uart_putchar(&px_uart1_handle, '\r');
        else if(strcmp(argv[2], "lf") == 0) px_uart_putchar(&px_uart1_handle, '\n');
        else if(strcmp(argv[2], "crlf") == 0)
        {
            px_uart_putchar(&px_uart1_handle, '\r');
            px_uart_putchar(&px_uart1_handle, '\n');
        }
        else return "Error: terminator must be cr, lf or crlf";
    }

    px_systmr_start(&timer, PX_SYSTMR_MS_TO_TICKS(timeout_ms));
    while(!px_systmr_has_expired(&timer))
    {
        if(px_uart_rd_u8(&px_uart1_handle, (uint8_t *)&rx))
        {
            if((rx == '\r') || (rx == '\n') || (rx == '\0')) break;
            putchar(rx);
        }
    }
    putchar('\n');
    return NULL;
}

static const char * px_cli_cmd_fn_uart_errors(uint8_t argc, char * argv[])
{
#if PX_UART_CFG_ERR_STAT_EN
    printf("overrun=%u framing=%u parity=%u\n",
           (unsigned int)px_uart_rx_err_overrun_get_and_rst(&px_uart1_handle),
           (unsigned int)px_uart_rx_err_framing_get_and_rst(&px_uart1_handle),
           (unsigned int)px_uart_rx_err_parity_get_and_rst(&px_uart1_handle));
#else
    printf("UART error counters are disabled\n");
#endif
    return NULL;
}

/* _____COMMAND TREE________________________________________________________ */
PX_CLI_CMD_CREATE(px_cli_cmd_uart_cfg,      "cfg",  2, 5, "<1> <baud> [data] [parity] [stop]", "Configure USART1")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_info,     "info", 0, 0, "",                                      "Report USART1 configuration")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_write,    "w",    1, 16, "<d0> .. [d15]",                         "Write raw bytes")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_receive,  "r",    2, 2, "<nr bytes> <timeout ms>",                "Receive raw bytes")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_exchange, "x",    3, 18, "<nr bytes> <timeout ms> <d0> .. [d15]", "Write and receive raw bytes")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_string,   "s",    2, 3, "<timeout ms> <string> [cr|lf|crlf]",     "Write and receive a line")
PX_CLI_CMD_CREATE(px_cli_cmd_uart_errors,   "err",  0, 0, "",                                      "Read and clear UART error counters")

PX_CLI_GROUP_CREATE(px_cli_group_uart, "uart")
    PX_CLI_CMD_ADD(px_cli_cmd_uart_cfg,      px_cli_cmd_fn_uart_cfg)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_info,     px_cli_cmd_fn_uart_info)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_write,    px_cli_cmd_fn_uart_write)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_receive,  px_cli_cmd_fn_uart_receive)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_exchange, px_cli_cmd_fn_uart_exchange)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_string,   px_cli_cmd_fn_uart_string)
    PX_CLI_CMD_ADD(px_cli_cmd_uart_errors,   px_cli_cmd_fn_uart_errors)
PX_CLI_GROUP_END()
