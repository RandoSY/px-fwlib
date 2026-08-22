# ------------------------------------------------------------------------------
# WeAct STM32G030 processor CLI explorer
# ------------------------------------------------------------------------------

PROJECT = cli_explorer

MCU        = -mthumb -mcpu=cortex-m0plus
FLASH_SIZE ?= 32k
SRAM_SIZE  = 8k
STACK_SIZE = 512
HEAP_SIZE  = 512

PX_FWLIB = ../../../../../../
ARCH = arch/arm/stm32
BSP = $(PX_FWLIB)/boards/arm/stm32/weact_stm32g030
STMCUBE = $(PX_FWLIB)/libs/STM32Cube/G0

SRC += src/main.c
SRC += src/px_cli_cmds.c
SRC += src/px_cli_cmds_adc.c
SRC += src/px_cli_cmds_gpio.c
SRC += src/px_cli_cmds_mem.c
SRC += src/px_cli_cmds_rtc.c
SRC += src/px_cli_cmds_spi.c
SRC += src/px_cli_cmds_uart.c
SRC += $(BSP)/src/px_board.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_adc.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_gpio.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_rtc.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_spi.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_sysclk.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_uart.c
SRC += $(PX_FWLIB)/$(ARCH)/src/px_uart_stdio.c
SRC += $(PX_FWLIB)/comms/src/px_cli.c
SRC += $(PX_FWLIB)/comms/src/px_vt100.c
SRC += $(PX_FWLIB)/utils/src/px_log.c
SRC += $(PX_FWLIB)/utils/src/px_ring_buf.c
SRC += $(PX_FWLIB)/utils/src/px_rtc_util.c
SRC += $(PX_FWLIB)/utils/src/px_systmr.c
SRC += $(STMCUBE)/Drivers/CMSIS/Device/ST/STM32G0xx/Source/Templates/system_stm32g0xx.c

ASRC += $(STMCUBE)/Drivers/CMSIS/Device/ST/STM32G0xx/Source/Templates/gcc/startup_stm32g030xx.s

INCDIRS += cfg
INCDIRS += inc
INCDIRS += src
INCDIRS += $(BSP)/inc
INCDIRS += $(PX_FWLIB)
INCDIRS += $(PX_FWLIB)/$(ARCH)/inc
INCDIRS += $(PX_FWLIB)/common/inc
INCDIRS += $(PX_FWLIB)/comms/inc
INCDIRS += $(PX_FWLIB)/utils/inc
INCDIRS += $(STMCUBE)/Drivers/CMSIS/Device/ST/STM32G0xx/Include
INCDIRS += $(STMCUBE)/Drivers/CMSIS/Include
INCDIRS += $(STMCUBE)/Drivers/STM32G0xx_HAL_Driver/Inc

CDEFS += PX_LOG=1
CDEFS += STM32G0
CDEFS += STM32G030xx
CDEFS += USE_FULL_LL_DRIVER

CFLAGS += -gdwarf-2
CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fshort-enums
CFLAGS += -Wall
CFLAGS += -Wstrict-prototypes
CFLAGS += -std=gnu99
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += --specs=nano.specs

CFLAGS_DEBUG   += -Og
CFLAGS_RELEASE += -Os

LDFLAGS += -Wl,-gc-sections,--relax
LDFLAGS += -Wl,-print-memory-usage
LDFLAGS += --specs=nosys.specs
LDFLAGS += --specs=nano.specs
LDFLAGS += -T $(PX_FWLIB)/$(ARCH)/px_stm32_flash.ld

include $(PX_FWLIB)/$(ARCH)/MakeRules.mk
