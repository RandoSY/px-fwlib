#ifndef __PX_CLI_CMDS_GPIO_H__
#define __PX_CLI_CMDS_GPIO_H__

#include "px_cli.h"
#include "px_gpio.h"

const char * px_cli_cmd_gpio_str_to_handle(px_gpio_handle_t * handle,
                                           char *             pin_str);
extern const px_cli_group_t px_cli_group_gpio;

#endif
