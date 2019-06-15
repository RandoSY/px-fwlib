/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2014 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/piconomix-fwlib/blob/master/LICENSE.md
    
    Title:          px_cli_cmds_log_fs.h : CLI commands for log_fs
    Author(s):      Pieter Conradie
    Creation Date:  2014-07-08

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */
#include <avr/io.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_pgm_P.h"
#include "px_log_fs.h"
#include "px_vt100.h"
#include "px_dbg.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_DBG_DECL_NAME("cli");

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */
 
/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char* px_cli_cmd_fn_log_fs_info(uint8_t argc, char* argv[])
{
    px_log_fs_info();
    return NULL;
}

static const char* px_cli_cmd_fn_log_fs_list(uint8_t argc, char* argv[])
{
    px_log_fs_err_t  px_log_fs_err;
    px_log_fs_file_t file;
    uint16_t         file_nr = 1;

    px_log_fs_err = px_log_fs_file_find_first(&file);
    while(px_log_fs_err == PX_LOG_FS_ERR_NONE)
    {
        PX_PRINTF_P("File %d: start page = %d, timestamp %02hu-%02hu-%02hu %02hu:%02hu:%02hu\n",
                 file_nr++,
                 file.start_page,
                 (unsigned short)file.time_stamp.year,
                 (unsigned short)file.time_stamp.month,
                 (unsigned short)file.time_stamp.day,
                 (unsigned short)file.time_stamp.hour,
                 (unsigned short)file.time_stamp.min,
                 (unsigned short)file.time_stamp.sec);
        px_log_fs_err = px_log_fs_file_find_next(&file);
    }

    return NULL;
}

// Create CLI command structures
PX_CLI_CMD_CREATE(px_cli_cmd_log_fs_info,     "info",     0, 0,   "",   "Report file system info")
PX_CLI_CMD_CREATE(px_cli_cmd_log_fs_list,     "ls",       0, 0,   "",   "List files")

PX_CLI_GROUP_CREATE(px_cli_group_log_fs, "fs")
    PX_CLI_CMD_ADD(px_cli_cmd_log_fs_info,     px_cli_cmd_fn_log_fs_info)
    PX_CLI_CMD_ADD(px_cli_cmd_log_fs_list,     px_cli_cmd_fn_log_fs_list)
PX_CLI_GROUP_END()
