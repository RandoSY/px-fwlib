#ifndef __PX_LCD_ST7565P_TSE2G0330E_CFG_H__
#define __PX_LCD_ST7565P_TSE2G0330E_CFG_H__
/* =============================================================================
     _____   _____   _____   ____    _   _    ____    __  __   _____  __   __
    |  __ \ |_   _| / ____| / __ \  | \ | |  / __ \  |  \/  | |_   _| \ \ / /
    | |__) |  | |  | |     | |  | | |  \| | | |  | | | \  / |   | |    \ V /
    |  ___/   | |  | |     | |  | | | . ` | | |  | | | |\/| |   | |     > < 
    | |      _| |_ | |____ | |__| | | |\  | | |__| | | |  | |  _| |_   / . \
    |_|     |_____| \_____| \____/  |_| \_|  \____/  |_|  |_| |_____| /_/ \_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_lcd_st7565p_tse2g0330e.h : Truly TSE2G0330-E 128x64 monochrome LCD wtih Sitronix ST7565P driver configuration
    Author(s):      Pieter Conradie
    Creation Date:  2018-09-10

============================================================================= */

/** 
 *  @addtogroup PX_LCD_ST7565P_TSE2G0330E
 *
 *  @{
 */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defs.h"
#include "px_board.h"

/* _____DEFINITIONS__________________________________________________________ */
/// Macro to set LCD reset pin low
#define PX_LCD_CFG_RST_LO()     px_gpio_out_set_lo(&px_gpio_lcd_rst)

/// Macro to set LCD reset pin high
#define PX_LCD_CFG_RST_HI()     px_gpio_out_set_hi(&px_gpio_lcd_rst)

/// Macro to set RS pin low
#define PX_LCD_CFG_RS_LO()      px_gpio_out_set_lo(&px_gpio_lcd_rs)

/// Macro to set RS pin high
#define PX_LCD_CFG_RS_HI()      px_gpio_out_set_hi(&px_gpio_lcd_rs)

/// Configuration option to rotate mapping of LCD by 180 degrees
#define PX_LCD_CFG_ROT_180_DEG  1

/// @}
#endif
