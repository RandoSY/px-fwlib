#ifndef __PX_STM32CUBE_H__
#define __PX_STM32CUBE_H__
/* =============================================================================
     ____    ___    ____    ___    _   _    ___    __  __   ___  __  __ TM
    |  _ \  |_ _|  / ___|  / _ \  | \ | |  / _ \  |  \/  | |_ _| \ \/ /
    | |_) |  | |  | |     | | | | |  \| | | | | | | |\/| |  | |   \  /
    |  __/   | |  | |___  | |_| | | |\  | | |_| | | |  | |  | |   /  \
    |_|     |___|  \____|  \___/  |_| \_|  \___/  |_|  |_| |___| /_/\_\

    Copyright (c) 2018 Pieter Conradie <https://piconomix.com>
 
    License: MIT
    https://github.com/piconomix/px-fwlib/blob/master/LICENSE.md
 
    Title:          px_stm32cube.h : Include file for STM32Cube library
    Author(s):      Pieter Conradie
    Creation Date:  2018-04-11

============================================================================= */

/** 
 *  @ingroup STM32
 *  @defgroup STM32_STM32CUBE px_stm32cube.h : Include file for STM32Cube library
 *  
 *  This file includes all the required files from the STM32Cube library
 *  
 *  File(s):
 *  - arm/stm32/inc/px_stm32cube.h
 *
 *  @{
 */

/* _____PROJECT INCLUDES_____________________________________________________ */
#if STM32L0

    #include "stm32l0xx_ll_adc.h"
    #include "stm32l0xx_ll_bus.h"
    #include "stm32l0xx_ll_comp.h"
    #include "stm32l0xx_ll_cortex.h"
    #include "stm32l0xx_ll_crc.h"
    #include "stm32l0xx_ll_crs.h"
    #include "stm32l0xx_ll_dac.h"
    #include "stm32l0xx_ll_dma.h"
    #include "stm32l0xx_ll_exti.h"
    #include "stm32l0xx_ll_gpio.h"
    #include "stm32l0xx_ll_i2c.h"
    #include "stm32l0xx_ll_iwdg.h"
    #include "stm32l0xx_ll_lptim.h"
    #include "stm32l0xx_ll_lpuart.h"
    #include "stm32l0xx_ll_pwr.h"
    #include "stm32l0xx_ll_rcc.h"
    #include "stm32l0xx_ll_rng.h"
    #include "stm32l0xx_ll_rtc.h"
    #include "stm32l0xx_ll_spi.h"
    #include "stm32l0xx_ll_system.h"
    #include "stm32l0xx_ll_tim.h"
    #include "stm32l0xx_ll_usart.h"
    #include "stm32l0xx_ll_utils.h"
    #include "stm32l0xx_ll_wwdg.h"

#elif STM32L1

    #include "stm32l1xx_ll_adc.h"
    #include "stm32l1xx_ll_bus.h"
    #include "stm32l1xx_ll_comp.h"
    #include "stm32l1xx_ll_cortex.h"
    #include "stm32l1xx_ll_crc.h"
    #include "stm32l1xx_ll_dac.h"
    #include "stm32l1xx_ll_dma.h"
    #include "stm32l1xx_ll_exti.h"
    #include "stm32l1xx_ll_gpio.h"
    #include "stm32l1xx_ll_i2c.h"
    #include "stm32l1xx_ll_iwdg.h"
    #include "stm32l1xx_ll_pwr.h"
    #include "stm32l1xx_ll_rcc.h"
    #include "stm32l1xx_ll_rtc.h"
    #include "stm32l1xx_ll_spi.h"
    #include "stm32l1xx_ll_system.h"
    #include "stm32l1xx_ll_tim.h"
    #include "stm32l1xx_ll_usart.h"
    #include "stm32l1xx_ll_utils.h"
    #include "stm32l1xx_ll_wwdg.h"

#elif STM32G0

    #include "stm32g0xx_ll_adc.h"
    #include "stm32g0xx_ll_bus.h"
    #include "stm32g0xx_ll_comp.h"
    #include "stm32g0xx_ll_cortex.h"
    #include "stm32g0xx_ll_crc.h"
    #include "stm32g0xx_ll_dac.h"
    #include "stm32g0xx_ll_dma.h"
    #include "stm32g0xx_ll_dmamux.h"
    #include "stm32g0xx_ll_exti.h"
    #include "stm32g0xx_ll_gpio.h"
    #include "stm32g0xx_ll_i2c.h"
    #include "stm32g0xx_ll_iwdg.h"
    #include "stm32g0xx_ll_lptim.h"
    #include "stm32g0xx_ll_lpuart.h"
    #include "stm32g0xx_ll_pwr.h"
    #include "stm32g0xx_ll_rcc.h"
    #include "stm32g0xx_ll_rng.h"
    #include "stm32g0xx_ll_rtc.h"
    #include "stm32g0xx_ll_spi.h"
    #include "stm32g0xx_ll_system.h"
    #include "stm32g0xx_ll_tim.h"
    #include "stm32g0xx_ll_ucpd.h"
    #include "stm32g0xx_ll_usart.h"
    #include "stm32g0xx_ll_utils.h"
    #include "stm32g0xx_ll_wwdg.h"

#else
    #error "Unsupported STM32 series"
#endif

/// @}
#endif
