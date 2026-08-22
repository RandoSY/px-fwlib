/* _____STANDARD INCLUDES____________________________________________________ */
#include <stdio.h>
#include <string.h>

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "main.h"
#include "px_cli.h"
#include "px_cli_cmds_mem.h"
#include "px_log.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
PX_LOG_NAME("cli_cmds_mem");

/* _____LOCAL FUNCTIONS______________________________________________________ */
static const char * cli_mem_get_address(uint8_t index, uint32_t * address)
{
    if(!px_cli_util_argv_to_u32(index, 0, PX_U32_MAX))
    {
        return "Error: address must be a 32-bit hexadecimal or decimal value";
    }
    *address = px_cli_argv_val.u32;
    return NULL;
}

static const char * px_cli_cmd_fn_mem_read(uint8_t argc, char * argv[])
{
    uint32_t address;
    uint8_t count = 1;

    if(cli_mem_get_address(1, &address) != NULL)
    {
        return "Error: invalid address";
    }
    if(argc > 2)
    {
        if(!px_cli_util_argv_to_u8(2, 1, 16))
        {
            return "Error: count must be 1..16";
        }
        count = px_cli_argv_val.u8;
    }

    switch(argv[0][0])
    {
    case 'b':
    {
        uint8_t * data = (uint8_t *)address;
        while(count-- != 0)
        {
            uint8_t value = *data++;
            printf("0x%02x %u\n", value, value);
        }
        break;
    }
    case 'w':
    {
        uint16_t * data;
        if((address & 1u) != 0) return "Error: word address must be aligned";
        data = (uint16_t *)address;
        while(count-- != 0)
        {
            uint16_t value = *data++;
            printf("0x%04x %u\n", value, value);
        }
        break;
    }
    case 'l':
    {
        uint32_t * data;
        if((address & 3u) != 0) return "Error: long address must be aligned";
        data = (uint32_t *)address;
        while(count-- != 0)
        {
            uint32_t value = *data++;
            printf("0x%08lx %lu\n", (unsigned long)value, (unsigned long)value);
        }
        break;
    }
    default:
        return "Error: access size must be b, w or l";
    }
    return NULL;
}

static const char * px_cli_cmd_fn_mem_write(uint8_t argc, char * argv[])
{
    uint32_t address;
    uint8_t i;

    if(cli_mem_get_address(1, &address) != NULL)
    {
        return "Error: invalid address";
    }
    if((argv[0][0] == 'w') && ((address & 1u) != 0))
    {
        return "Error: word address must be aligned";
    }
    if((argv[0][0] == 'l') && ((address & 3u) != 0))
    {
        return "Error: long address must be aligned";
    }

    switch(argv[0][0])
    {
    case 'b':
    {
        uint8_t * data = (uint8_t *)address;
        for(i = 2; i < argc; i++)
        {
            if(!px_cli_util_argv_to_u8(i, 0, PX_U8_MAX)) return "Error: invalid byte";
            *data++ = px_cli_argv_val.u8;
        }
        break;
    }
    case 'w':
    {
        uint16_t * data = (uint16_t *)address;
        for(i = 2; i < argc; i++)
        {
            if(!px_cli_util_argv_to_u16(i, 0, PX_U16_MAX)) return "Error: invalid word";
            *data++ = px_cli_argv_val.u16;
        }
        break;
    }
    case 'l':
    {
        uint32_t * data = (uint32_t *)address;
        for(i = 2; i < argc; i++)
        {
            if(!px_cli_util_argv_to_u32(i, 0, PX_U32_MAX)) return "Error: invalid long";
            *data++ = px_cli_argv_val.u32;
        }
        break;
    }
    default:
        return "Error: access size must be b, w or l";
    }
    return NULL;
}

static const char * px_cli_cmd_fn_mem_dump(uint8_t argc, char * argv[])
{
    uint32_t address;
    uint16_t count;
    uint16_t offset;

    if(cli_mem_get_address(0, &address) != NULL)
    {
        return "Error: invalid address";
    }
    if(!px_cli_util_argv_to_u16(1, 1, MAIN_BUF_SIZE))
    {
        return "Error: byte count must be 1..256";
    }
    count = px_cli_argv_val.u16;
    memcpy(main_buf, (const void *)address, count);

    for(offset = 0; offset < count; offset += 16)
    {
        uint8_t i;
        printf("%08lx: ", (unsigned long)(address + offset));
        for(i = 0; i < 16; i++)
        {
            if((offset + i) < count) printf("%02x ", main_buf[offset + i]);
            else printf("   ");
        }
        printf("|");
        for(i = 0; i < 16 && (offset + i) < count; i++)
        {
            uint8_t c = main_buf[offset + i];
            putchar((c >= 32u) && (c < 127u) ? (char)c : '.');
        }
        printf("|\n");
    }
    return NULL;
}

/* _____COMMAND TREE________________________________________________________ */
PX_CLI_CMD_CREATE(px_cli_cmd_mem_read,  "r", 2, 3, "<b|w|l> <adr> [count]", "Read memory")
PX_CLI_CMD_CREATE(px_cli_cmd_mem_write, "w", 3, 10, "<b|w|l> <adr> <d0> .. [d7]", "Write memory")
PX_CLI_CMD_CREATE(px_cli_cmd_mem_dump,  "d", 2, 2, "<adr> <nr bytes>", "Hex dump memory")

PX_CLI_GROUP_CREATE(px_cli_group_mem, "mem")
    PX_CLI_CMD_ADD(px_cli_cmd_mem_read,  px_cli_cmd_fn_mem_read)
    PX_CLI_CMD_ADD(px_cli_cmd_mem_write, px_cli_cmd_fn_mem_write)
    PX_CLI_CMD_ADD(px_cli_cmd_mem_dump,  px_cli_cmd_fn_mem_dump)
PX_CLI_GROUP_END()
