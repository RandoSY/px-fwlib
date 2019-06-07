/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2019 Pieter Conradie <https://piconomix.com>
 
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
    
    Title:          px_log_fs_glue.h : Physical device glue layer for Log FS
    Author(s):      Pieter Conradie
    Creation Date:  2019-01-11

============================================================================= */

/* _____STANDARD INCLUDES____________________________________________________ */

/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_log_fs_glue.h"
#include "px_log_fs_cfg.h"
#include "px_at25s.h"

#include "px_dbg.h"
PX_DBG_DECL_NAME("px_log_fs_glue")

/* _____LOCAL DEFINITIONS____________________________________________________ */

/* _____MACROS_______________________________________________________________ */

/* _____GLOBAL VARIABLES_____________________________________________________ */

/* _____LOCAL VARIABLES______________________________________________________ */

/* _____LOCAL FUNCTION DECLARATIONS__________________________________________ */

/* _____LOCAL FUNCTIONS______________________________________________________ */

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_log_fs_glue_rd(void *   buffer, 
                       uint16_t page,
                       uint16_t start_byte_in_page,
                       uint16_t nr_of_bytes)
{
    // Sanity checks
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);    
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);
    PX_DBG_ASSERT(start_byte_in_page < PX_LOG_FS_CFG_PAGE_SIZE);
    PX_DBG_ASSERT(nr_of_bytes <= (PX_LOG_FS_CFG_PAGE_SIZE - start_byte_in_page));    

    // Read data from Serial Flash
    px_at25s_rd_page_offset(buffer, page, start_byte_in_page, nr_of_bytes);
}

void px_log_fs_glue_wr(const void * buffer,
                       uint16_t     page,
                       uint16_t     start_byte_in_page,
                       uint16_t     nr_of_bytes)
{
    // Sanity checks
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);    
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);
    PX_DBG_ASSERT(start_byte_in_page < PX_LOG_FS_CFG_PAGE_SIZE);
    PX_DBG_ASSERT(nr_of_bytes <= (PX_LOG_FS_CFG_PAGE_SIZE - start_byte_in_page));

    // Write data to Serial Flash
    px_at25s_wr_page_offset(buffer, page, start_byte_in_page, nr_of_bytes);
}

void px_log_fs_glue_erase_block(uint16_t page)
{
    // Sanity checks
    PX_DBG_ASSERT(page >= PX_LOG_FS_CFG_PAGE_START);    
    PX_DBG_ASSERT(page <= PX_LOG_FS_CFG_PAGE_END);
    PX_DBG_ASSERT(page & (PX_LOG_FS_CFG_ERASE_BLOCK_SIZE - 1) == 0);
    
#if   (PX_LOG_FS_CFG_ERASE_BLOCK_SIZE == PX_AT25S_PAGES_PER_BLOCK_4KB)

    // Erase 4 KB block
    px_at25s_erase(PX_AT25S_BLOCK_4KB, page);

#elif (PX_LOG_FS_CFG_ERASE_BLOCK_SIZE == PX_AT25S_PAGES_PER_BLOCK_32KB)

    // Erase 32 KB block
    px_at25s_erase(PX_AT25S_BLOCK_32KB, page);

#elif (PX_LOG_FS_CFG_ERASE_BLOCK_SIZE == PX_AT25S_PAGES_PER_BLOCK_64KB)

    // Erase 64 KB block
    px_at25s_erase(PX_AT25S_BLOCK_64KB, page);

#else
    #error "Invalid block size specified");
#endif
}
