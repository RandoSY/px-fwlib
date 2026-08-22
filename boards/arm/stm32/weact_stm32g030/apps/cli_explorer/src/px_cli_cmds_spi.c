/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_cli.h"
#include "px_cli_cmds_gpio.h"
#include "px_cli_cmds_spi.h"
#include "px_gpio.h"
#include "px_log.h"
#include "px_spi.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_spi");

/* _____LOCAL VARIABLES______________________________________________________ */
static px_gpio_handle_t spi_cs_gpio;
static px_spi_handle_t spi_handle;
static bool spi_handle_open;

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * px_cli_cmd_fn_spi_cfg(uint8_t argc, char * argv[])
{
    uint32_t baud_hz = 1000000ul;
    px_spi_baud_t baud;
    px_spi_mode_t mode = PX_SPI_MODE0;
    px_spi_dord_t order = PX_SPI_DATA_ORDER_MSB;
    const char * result;

    if(argc > 0 && argv[0][0] != '1')
    {
        return "Error: this board exposes SPI1 only";
    }
    if(argc > 1)
    {
        result = px_cli_cmd_gpio_str_to_handle(&spi_cs_gpio, argv[1]);
        if(result != NULL) return result;
    }
    else
    {
        spi_cs_gpio = px_gpio_spi1_cs;
    }
    if(argc > 2)
    {
        if(!px_cli_util_argv_to_u32(2, 1, PX_BOARD_PER_CLK_HZ / 2ul))
        {
            return "Error: baud must be 1..32000000 Hz";
        }
        baud_hz = px_cli_argv_val.u32;
    }
    if(argc > 3)
    {
        if(argv[3][0] < '0' || argv[3][0] > '3') return "Error: mode must be 0..3";
        mode = (px_spi_mode_t)(argv[3][0] - '0');
    }
    if(argc > 4)
    {
        if(strcmp(argv[4], "lsb") == 0) order = PX_SPI_DATA_ORDER_LSB;
        else if(strcmp(argv[4], "msb") != 0) return "Error: order must be lsb or msb";
    }

    baud = px_spi_util_baud_hz_to_clk_div(baud_hz);
    px_gpio_out_set_hi(&spi_cs_gpio);
    px_gpio_dir_set_out(&spi_cs_gpio);
    if(spi_handle_open) px_spi_close(&spi_handle);
    if(!px_spi_open2(&spi_handle, PX_SPI_NR_1, PX_BOARD_SPI_CS_NONE,
                     baud, mode, order, 0x00))
    {
        return "Error: SPI1 could not be opened";
    }
    spi_handle_open = true;
    printf("SPI1 configured: actual baud=%lu Hz mode=%u order=%s CS=%c%u\n",
           (unsigned long)px_spi_util_clk_div_to_baud_hz(baud),
           (unsigned int)mode,
           (order == PX_SPI_DATA_ORDER_LSB) ? "lsb" : "msb",
           (spi_cs_gpio.gpio_base_reg == GPIOA) ? 'a' : 'b',
           (unsigned int)spi_cs_gpio.pin);
    return NULL;
}

static const char * cli_spi_require_open(void)
{
    return spi_handle_open ? NULL : "Error: run spi cfg first";
}

static const char * px_cli_cmd_fn_spi_cs(uint8_t argc, char * argv[])
{
    const char * result = cli_spi_require_open();
    if(result != NULL) return result;

    if(strcmp(argv[0], "lo") == 0)
    {
        px_gpio_out_set_lo(&spi_cs_gpio);
        px_spi_wr(&spi_handle, NULL, 0, PX_SPI_FLAG_START);
    }
    else if(strcmp(argv[0], "hi") == 0)
    {
        px_spi_wr(&spi_handle, NULL, 0, PX_SPI_FLAG_STOP);
        px_gpio_out_set_hi(&spi_cs_gpio);
    }
    else return "Error: use lo or hi";
    return NULL;
}

static const char * px_cli_cmd_fn_spi_write(uint8_t argc, char * argv[])
{
    uint8_t data[8];
    uint8_t i;
    const char * result = cli_spi_require_open();
    if(result != NULL) return result;
    for(i = 0; i < argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255)) return "Error: SPI byte must be 0..255";
        data[i] = px_cli_argv_val.u8;
    }
    px_spi_wr(&spi_handle, data, argc, PX_SPI_FLAG_START_AND_STOP);
    return NULL;
}

static const char * px_cli_cmd_fn_spi_read(uint8_t argc, char * argv[])
{
    uint8_t data[16];
    uint8_t count;
    uint8_t i;
    const char * result = cli_spi_require_open();
    if(result != NULL) return result;
    if(!px_cli_util_argv_to_u8(0, 1, 16)) return "Error: count must be 1..16";
    count = px_cli_argv_val.u8;
    px_spi_rd(&spi_handle, data, count, PX_SPI_FLAG_START_AND_STOP);
    for(i = 0; i < count; i++) printf("0x%02x ", data[i]);
    putchar('\n');
    return NULL;
}

static const char * px_cli_cmd_fn_spi_exchange(uint8_t argc, char * argv[])
{
    uint8_t data[8];
    uint8_t i;
    const char * result = cli_spi_require_open();
    if(result != NULL) return result;
    for(i = 0; i < argc; i++)
    {
        if(!px_cli_util_argv_to_u8(i, 0, 255)) return "Error: SPI byte must be 0..255";
        data[i] = px_cli_argv_val.u8;
    }
    px_spi_xc(&spi_handle, data, data, argc, PX_SPI_FLAG_START_AND_STOP);
    for(i = 0; i < argc; i++) printf("0x%02x ", data[i]);
    putchar('\n');
    return NULL;
}

/* _____COMMAND TREE________________________________________________________ */
PX_CLI_CMD_CREATE(px_cli_cmd_spi_cfg,      "cfg", 0, 5, "[1] [cs] [baud] [mode] [order]", "Configure SPI1")
PX_CLI_CMD_CREATE(px_cli_cmd_spi_cs,       "cs",  1, 1, "<lo|hi>",                    "Control the configured chip select")
PX_CLI_CMD_CREATE(px_cli_cmd_spi_write,    "w",   1, 8, "<d0> .. [d7]",               "Write bytes to an SPI slave")
PX_CLI_CMD_CREATE(px_cli_cmd_spi_read,     "r",   1, 1, "<nr bytes>",                  "Read bytes from an SPI slave")
PX_CLI_CMD_CREATE(px_cli_cmd_spi_exchange, "x",   1, 8, "<d0> .. [d7]",               "Exchange bytes with an SPI slave")

PX_CLI_GROUP_CREATE(px_cli_group_spi, "spi")
    PX_CLI_CMD_ADD(px_cli_cmd_spi_cfg,      px_cli_cmd_fn_spi_cfg)
    PX_CLI_CMD_ADD(px_cli_cmd_spi_cs,       px_cli_cmd_fn_spi_cs)
    PX_CLI_CMD_ADD(px_cli_cmd_spi_write,    px_cli_cmd_fn_spi_write)
    PX_CLI_CMD_ADD(px_cli_cmd_spi_read,     px_cli_cmd_fn_spi_read)
    PX_CLI_CMD_ADD(px_cli_cmd_spi_exchange, px_cli_cmd_fn_spi_exchange)
PX_CLI_GROUP_END()
