#ifndef __PX_ADC_CFG_H__
#define __PX_ADC_CFG_H__

#include "px_defs.h"

/* One calibrated, single-conversion ADC peripheral. */
#define PX_ADC_CFG_ADC1_EN 1

/* 64 MHz PCLK / 4 = 16 MHz ADC clock. */
#define PX_ADC_CFG_CLK PX_ADC_CFG_CLK_PCLK_DIV4

/* Long acquisition time is friendly to high-impedance voltage sources. */
#define PX_ADC_CFG_SAMPLE_TIME PX_ADC_CFG_SAMPLE_TIME_160_5

#define PX_ADC_CFG_RES PX_ADC_CFG_RES_12_BITS
#define PX_ADC_CFG_OVERSAMPLING PX_ADC_CFG_OVERSAMPLING_NONE
#define PX_ADC_CFG_OVERSAMPLING_SHIFT PX_ADC_CFG_OVERSAMPLING_SHIFT_NONE

#endif
