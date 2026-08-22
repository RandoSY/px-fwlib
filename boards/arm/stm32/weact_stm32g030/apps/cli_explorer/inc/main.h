#ifndef __CLI_EXPLORER_MAIN_H__
#define __CLI_EXPLORER_MAIN_H__

#include "px_adc.h"
#include "px_uart.h"

#define MAIN_BUF_SIZE 256

extern px_uart_handle_t px_uart1_handle;
extern px_adc_handle_t  px_adc1_handle;
extern uint8_t          main_buf[MAIN_BUF_SIZE];

#endif
