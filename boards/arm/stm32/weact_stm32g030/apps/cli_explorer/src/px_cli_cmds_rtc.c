/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_cli.h"
#include "px_cli_cmds_rtc.h"
#include "px_log.h"
#include "px_rtc.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_rtc");

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * px_cli_cmd_fn_rtc_read(uint8_t argc, char * argv[])
{
    px_rtc_date_time_t date_time;

    px_rtc_date_time_rd(&date_time);
    px_rtc_util_printf_date_time(&date_time);
    putchar('\n');
    return NULL;
}

static const char * px_cli_cmd_fn_rtc_write(uint8_t argc, char * argv[])
{
    px_rtc_date_time_t date_time;
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int min;
    unsigned int sec;

    if((sscanf(argv[0], "%02u-%02u-%02u", &year, &month, &day) != 3)
       || (sscanf(argv[1], "%02u:%02u:%02u", &hour, &min, &sec) != 3))
    {
        return "Error: use yy-mm-dd hh:mm:ss";
    }
    date_time.year = (uint16_t)year;
    date_time.month = (uint8_t)month;
    date_time.day = (uint8_t)day;
    date_time.hour = (uint8_t)hour;
    date_time.min = (uint8_t)min;
    date_time.sec = (uint8_t)sec;
    if(!px_rtc_util_date_time_fields_are_valid(&date_time))
    {
        return "Error: date or time field is invalid";
    }
    px_rtc_date_time_wr(&date_time);
    return NULL;
}

/* _____COMMAND TREE________________________________________________________ */
PX_CLI_CMD_CREATE(px_cli_cmd_rtc_read,  "r", 0, 0, "",                    "Read RTC time")
PX_CLI_CMD_CREATE(px_cli_cmd_rtc_write, "w", 2, 2, "<yy-mm-dd> <hh:mm:ss>", "Write RTC time")

PX_CLI_GROUP_CREATE(px_cli_group_rtc, "rtc")
    PX_CLI_CMD_ADD(px_cli_cmd_rtc_read,  px_cli_cmd_fn_rtc_read)
    PX_CLI_CMD_ADD(px_cli_cmd_rtc_write, px_cli_cmd_fn_rtc_write)
PX_CLI_GROUP_END()
