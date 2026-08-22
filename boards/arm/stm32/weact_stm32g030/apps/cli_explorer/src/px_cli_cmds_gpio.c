/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_cli.h"
#include "px_cli_cmds_gpio.h"
#include "px_gpio.h"
#include "px_log.h"
#include "px_stm32cube.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_gpio");

/* _____LOCAL VARIABLES______________________________________________________ */
static px_gpio_handle_t gpio_handle;

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
const char * px_cli_cmd_gpio_str_to_handle(px_gpio_handle_t * handle,
                                           char *             pin_str)
{
    unsigned long pin;
    char * end;

    if(strcmp(pin_str, "led") == 0)
    {
        *handle = px_gpio_usr_led;
        return NULL;
    }
    if(strcmp(pin_str, "button") == 0)
    {
        *handle = px_gpio_usr_btn;
        return NULL;
    }
    if(strcmp(pin_str, "spi_cs") == 0)
    {
        *handle = px_gpio_spi1_cs;
        return NULL;
    }
    if(strcmp(pin_str, "spi_sck") == 0)
    {
        *handle = px_gpio_spi1_sck;
        return NULL;
    }
    if(strcmp(pin_str, "spi_miso") == 0)
    {
        *handle = px_gpio_spi1_miso;
        return NULL;
    }
    if(strcmp(pin_str, "spi_mosi") == 0)
    {
        *handle = px_gpio_spi1_mosi;
        return NULL;
    }
    if(strcmp(pin_str, "adc0") == 0)
    {
        *handle = px_gpio_adc0;
        return NULL;
    }
    if(strcmp(pin_str, "adc1") == 0)
    {
        *handle = px_gpio_adc1;
        return NULL;
    }
    if(strcmp(pin_str, "adc2") == 0)
    {
        *handle = px_gpio_adc2;
        return NULL;
    }
    if(strcmp(pin_str, "adc3") == 0)
    {
        *handle = px_gpio_adc3;
        return NULL;
    }

    if((pin_str[0] != 'a') && (pin_str[0] != 'b') && (pin_str[0] != 'c'))
    {
        return "Error: pin must be a0..a15, b0..b1, c14..c15, or a board alias";
    }
    pin = strtoul(pin_str + 1, &end, 0);
    if((*end != '\0') || (pin > 15))
    {
        return "Error: pin number must be 0..15";
    }
    if((pin_str[0] == 'b') && (pin > 1))
    {
        return "Error: STM32G030F6P6 exposes GPIOB pins b0..b1 here";
    }
    if((pin_str[0] == 'c') && (pin < 14))
    {
        return "Error: GPIOC pins c14..c15 are the board LSE pins";
    }

    switch(pin_str[0])
    {
    case 'a': px_gpio_open2(handle, GPIOA, PX_GPIO_PORT_A, (uint8_t)pin); break;
    case 'b': px_gpio_open2(handle, GPIOB, PX_GPIO_PORT_B, (uint8_t)pin); break;
    default:  px_gpio_open2(handle, GPIOC, PX_GPIO_PORT_C, (uint8_t)pin); break;
    }
    return NULL;
}

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * cli_gpio_get_handle(char * pin_str)
{
    return px_cli_cmd_gpio_str_to_handle(&gpio_handle, pin_str);
}

static void cli_gpio_report_pin(px_gpio_handle_t * handle)
{
    uint32_t mask = (uint32_t)1u << handle->pin;

    switch(handle->mode)
    {
    case PX_GPIO_MODE_IN:  printf("Input "); break;
    case PX_GPIO_MODE_OUT: printf("Output "); break;
    case PX_GPIO_MODE_AF:  printf("AF%u ", (unsigned int)handle->af); break;
    case PX_GPIO_MODE_ANA: printf("Analog\n"); return;
    default:               printf("Unknown mode\n"); return;
    }

    switch(handle->pull)
    {
    case PX_GPIO_PULL_UP: printf("Pull-Up "); break;
    case PX_GPIO_PULL_DN: printf("Pull-Down "); break;
    default: break;
    }

    if(handle->mode == PX_GPIO_MODE_IN)
    {
        printf("value=%u\n",
               LL_GPIO_IsInputPinSet(handle->gpio_base_reg, mask) ? 1u : 0u);
        return;
    }

    printf("otype=%s speed=%u",
           (handle->otype == PX_GPIO_OTYPE_OD) ? "OD" : "PP",
           (unsigned int)handle->ospeed);
    if(handle->mode == PX_GPIO_MODE_OUT)
    {
        printf(" value=%u",
               LL_GPIO_IsOutputPinSet(handle->gpio_base_reg, mask) ? 1u : 0u);
    }
    putchar('\n');
}

static void cli_gpio_report_port(char name, GPIO_TypeDef * port, px_gpio_port_t port_id,
                                 uint8_t first, uint8_t last)
{
    uint8_t pin;

    for(pin = first; pin <= last; pin++)
    {
        printf("%c%u: ", name, (unsigned int)pin);
        px_gpio_open2(&gpio_handle, port, port_id, pin);
        cli_gpio_report_pin(&gpio_handle);
    }
}

static void cli_gpio_report_regs(char name, GPIO_TypeDef * port)
{
    printf("GPIO%c MODER=%08lx OTYPER=%08lx OSPEEDR=%08lx PUPDR=%08lx\n",
           name,
           (unsigned long)port->MODER,
           (unsigned long)port->OTYPER,
           (unsigned long)port->OSPEEDR,
           (unsigned long)port->PUPDR);
    printf("GPIO%c IDR=%08lx ODR=%08lx AFRL=%08lx AFRH=%08lx\n",
           name,
           (unsigned long)port->IDR,
           (unsigned long)port->ODR,
           (unsigned long)port->AFR[0],
           (unsigned long)port->AFR[1]);
}

static const char * px_cli_cmd_fn_gpio_info(uint8_t argc, char * argv[])
{
    const char * result;

    if(strcmp(argv[0], "all") == 0)
    {
        cli_gpio_report_port('a', GPIOA, PX_GPIO_PORT_A, 0, 15);
        cli_gpio_report_port('b', GPIOB, PX_GPIO_PORT_B, 0, 1);
        cli_gpio_report_port('c', GPIOC, PX_GPIO_PORT_C, 14, 15);
    }
    else if(strcmp(argv[0], "regs") == 0)
    {
        cli_gpio_report_regs('A', GPIOA);
        cli_gpio_report_regs('B', GPIOB);
        cli_gpio_report_regs('C', GPIOC);
    }
    else
    {
        result = cli_gpio_get_handle(argv[0]);
        if(result != NULL) return result;
        cli_gpio_report_pin(&gpio_handle);
    }
    return NULL;
}

static const char * px_cli_cmd_fn_gpio_cfg_in(uint8_t argc, char * argv[])
{
    const char * result = cli_gpio_get_handle(argv[0]);
    if(result != NULL) return result;

    if(argc > 1)
    {
        switch(px_cli_util_argv_to_option(1, "pu\0pd\0"))
        {
        case 0: gpio_handle.pull = PX_GPIO_PULL_UP; break;
        case 1: gpio_handle.pull = PX_GPIO_PULL_DN; break;
        default: return "Error: pull must be pu or pd";
        }
    }
    else
    {
        gpio_handle.pull = PX_GPIO_PULL_NO;
    }
    gpio_handle.mode = PX_GPIO_MODE_IN;
    gpio_handle.otype = PX_GPIO_OTYPE_NA;
    gpio_handle.ospeed = PX_GPIO_OSPEED_NA;
    gpio_handle.out_init = PX_GPIO_OUT_INIT_NA;
    gpio_handle.af = PX_GPIO_AF_NA;
    px_gpio_init(&gpio_handle);
    return NULL;
}

static const char * px_cli_cmd_fn_gpio_cfg_out(uint8_t argc, char * argv[])
{
    const char * result = cli_gpio_get_handle(argv[0]);
    if(result != NULL) return result;

    if(argv[1][0] == '0') gpio_handle.out_init = PX_GPIO_OUT_INIT_LO;
    else if(argv[1][0] == '1') gpio_handle.out_init = PX_GPIO_OUT_INIT_HI;
    else return "Error: output value must be 0 or 1";

    gpio_handle.otype = PX_GPIO_OTYPE_PP;
    gpio_handle.pull = PX_GPIO_PULL_NO;
    if(argc > 2)
    {
        if(strcmp(argv[2], "od") == 0) gpio_handle.otype = PX_GPIO_OTYPE_OD;
        else return "Error: optional output mode must be od";
    }
    if(argc > 3)
    {
        if(strcmp(argv[3], "pu") == 0) gpio_handle.pull = PX_GPIO_PULL_UP;
        else return "Error: optional pull mode must be pu";
    }
    gpio_handle.mode = PX_GPIO_MODE_OUT;
    gpio_handle.ospeed = PX_GPIO_OSPEED_VH;
    gpio_handle.af = PX_GPIO_AF_NA;
    px_gpio_init(&gpio_handle);
    return NULL;
}

static const char * px_cli_cmd_fn_gpio_cfg_af(uint8_t argc, char * argv[])
{
    const char * result = cli_gpio_get_handle(argv[0]);
    if(result != NULL) return result;
    if(!px_cli_util_argv_to_u8(1, 0, 7))
    {
        return "Error: AF must be 0..7";
    }
    gpio_handle.mode = PX_GPIO_MODE_AF;
    gpio_handle.otype = PX_GPIO_OTYPE_PP;
    gpio_handle.ospeed = PX_GPIO_OSPEED_HI;
    gpio_handle.pull = PX_GPIO_PULL_NO;
    gpio_handle.out_init = PX_GPIO_OUT_INIT_NA;
    gpio_handle.af = (px_gpio_af_t)px_cli_argv_val.u8;
    px_gpio_init(&gpio_handle);
    return NULL;
}

static const char * px_cli_cmd_fn_gpio_cfg_ana(uint8_t argc, char * argv[])
{
    const char * result = cli_gpio_get_handle(argv[0]);
    if(result != NULL) return result;
    gpio_handle.mode = PX_GPIO_MODE_ANA;
    gpio_handle.otype = PX_GPIO_OTYPE_NA;
    gpio_handle.ospeed = PX_GPIO_OSPEED_NA;
    gpio_handle.pull = PX_GPIO_PULL_NO;
    gpio_handle.out_init = PX_GPIO_OUT_INIT_NA;
    gpio_handle.af = PX_GPIO_AF_NA;
    px_gpio_init(&gpio_handle);
    return NULL;
}

static const char * px_cli_cmd_fn_gpio_in(uint8_t argc, char * argv[])
{
    const char * result = cli_gpio_get_handle(argv[0]);
    if(result != NULL) return result;
    printf("%u\n", px_gpio_in_is_hi(&gpio_handle) ? 1u : 0u);
    return NULL;
}

static const char * px_cli_cmd_fn_gpio_out(uint8_t argc, char * argv[])
{
    const char * result = cli_gpio_get_handle(argv[0]);
    if(result != NULL) return result;
    if(argv[1][0] == '0') px_gpio_out_set_lo(&gpio_handle);
    else if(argv[1][0] == '1') px_gpio_out_set_hi(&gpio_handle);
    else return "Error: output value must be 0 or 1";
    return NULL;
}

/* _____COMMAND TREE________________________________________________________ */
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_info,    "info", 1, 1, "<pin|all|regs>",       "Report pin or GPIO registers")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_cfg_in,  "ci",   1, 2, "<pin> [pu|pd]",        "Configure a pin as input")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_cfg_out, "co",   2, 4, "<pin> <0|1> [od] [pu]", "Configure a pin as output")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_cfg_af,  "af",   2, 2, "<pin> <0..7>",        "Configure a pin for an alternate function")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_cfg_ana, "ana",  1, 1, "<pin>",                "Configure a pin as analog")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_in,      "i",    1, 1, "<pin>",                "Read a pin input")
PX_CLI_CMD_CREATE(px_cli_cmd_gpio_out,     "o",    2, 2, "<pin> <0|1>",          "Write a pin output")

PX_CLI_GROUP_CREATE(px_cli_group_gpio, "gpio")
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_info,    px_cli_cmd_fn_gpio_info)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_cfg_in,  px_cli_cmd_fn_gpio_cfg_in)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_cfg_out, px_cli_cmd_fn_gpio_cfg_out)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_cfg_af,  px_cli_cmd_fn_gpio_cfg_af)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_cfg_ana, px_cli_cmd_fn_gpio_cfg_ana)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_in,      px_cli_cmd_fn_gpio_in)
    PX_CLI_CMD_ADD(px_cli_cmd_gpio_out,     px_cli_cmd_fn_gpio_out)
PX_CLI_GROUP_END()
