#ifndef __APP_PINS_H__
#define __APP_PINS_H__

/*
 * External interface pins for the ADC meter.
 *
 * PA0 remains the analog input. PA1, PA2, and PA3 are repurposed by this
 * application as digital outputs for the red LED, green LED, and buzzer.
 * LEDs are assumed active-high with their resistors connected to ground.
 */

#include "px_gpio.h"

#define PX_GPIO_METER_RED \
    PX_GPIO(A, 1, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, \
            PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)

#define PX_GPIO_METER_GREEN \
    PX_GPIO(A, 2, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, \
            PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)

#define PX_GPIO_METER_BUZZER \
    PX_GPIO(A, 3, PX_GPIO_MODE_OUT, PX_GPIO_OTYPE_PP, PX_GPIO_OSPEED_LO, \
            PX_GPIO_PULL_NO, PX_GPIO_OUT_INIT_LO, PX_GPIO_AF_NA)

static const px_gpio_handle_t px_gpio_meter_red = {PX_GPIO_METER_RED};
static const px_gpio_handle_t px_gpio_meter_green = {PX_GPIO_METER_GREEN};
static const px_gpio_handle_t px_gpio_meter_buzzer = {PX_GPIO_METER_BUZZER};

#endif
