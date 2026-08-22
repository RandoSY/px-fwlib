#ifndef __PX_SPI_CFG_H__
#define __PX_SPI_CFG_H__

#include "px_defs.h"
#include "px_board.h"

#define PX_SPI_CFG_SPI1_EN 1
#define PX_SPI_CFG_SPI2_EN 0
#define PX_SPI_CFG_DEFAULT_BAUD PX_SPI_BAUD_CLK_DIV_32
#define PX_SPI_CFG_DEFAULT_MODE PX_SPI_MODE0
#define PX_SPI_CFG_DEFAULT_DATA_ORDER PX_SPI_DATA_ORDER_MSB
#define PX_SPI_CFG_CS_LO(cs_id) px_board_spi_cs_lo(cs_id)
#define PX_SPI_CFG_CS_HI(cs_id) px_board_spi_cs_hi(cs_id)

#endif
