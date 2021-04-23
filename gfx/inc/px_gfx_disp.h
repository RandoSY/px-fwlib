#ifndef __PX_GFX_DISPLAY_H__
#define __PX_GFX_DISPLAY_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
 
    Title:          px_gfx_disp.h : Glue layer to physical display
    Author(s):      Pieter Conradie
    Creation Date:  2019-05-28

============================================================================= */

/** 
 *  @addtogroup GFX
 */
/// @{

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_defines.h"
#include "px_gfx.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* _____DEFINITIONS__________________________________________________________ */

/* _____TYPE DEFINITIONS_____________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____GLOBAL FUNCTION DECLARATIONS_________________________________________ */
void px_gfx_disp_buf_clear     (void);
void px_gfx_disp_buf_pixel     (px_gfx_xy_t    x,
                                   px_gfx_xy_t    y,
                                   px_gfx_color_t color);
void px_gfx_disp_update        (const px_gfx_area_t * area);
void px_gfx_disp_dbg_report_buf(void);

/* _____MACROS_______________________________________________________________ */

/// @}
#ifdef __cplusplus
}
#endif

#endif
