/* _____PROJECT INCLUDES_____________________________________________________ */
#include "px_board.h"
#include "px_stm32cube.h"

/* _____LOCAL DEFINITIONS____________________________________________________ */
#ifndef PX_BOARD_WEACT_STM32G030
#warning "Wrong 'px_board.h' has been included. Check that include path is correct."
#endif

#ifndef STM32G030xx
#warning "This BSP is intended for STM32G030xx devices."
#endif

/* _____LOCAL FUNCTIONS______________________________________________________ */
static void px_board_clocks_init(void)
{
    /* Enable the clocks needed by the clock and pin remap configuration. */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* 64 MHz operation requires two Flash wait states on STM32G030. */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    /* HSI16 / 1 * 8 / 2 = 64 MHz. */
    LL_RCC_HSI_Enable();
    while(!LL_RCC_HSI_IsReady()) {;}
    LL_RCC_HSI_SetCalibTrimming(64);
    LL_RCC_SetHSIDiv(LL_RCC_HSI_DIV_1);
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI,
                                LL_RCC_PLLM_DIV_1,
                                8,
                                LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_EnableDomain_SYS();
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady()) {;}

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_PLL) {;}

    /* The board routes the USART1 signals to the PA11/PA12 header nets. */
    LL_SYSCFG_EnablePinRemap(LL_SYSCFG_PIN_RMP_PA11 | LL_SYSCFG_PIN_RMP_PA12);

    /* The board includes a 32.768 kHz crystal on PC14/PC15. */
    LL_RCC_LSE_SetDriveCapability(LL_RCC_LSEDRIVE_LOW);
    LL_RCC_LSE_Enable();
    while(!LL_RCC_LSE_IsReady()) {;}
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTC);
    LL_RCC_EnableRTC();

    LL_SetSystemCoreClock(PX_BOARD_SYS_CLK_HZ);
}

static void px_board_gpio_init(void)
{
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    px_gpio_init(&px_gpio_uart1_tx);
    px_gpio_init(&px_gpio_uart1_rx);
    px_gpio_init(&px_gpio_spi1_cs);
    px_gpio_init(&px_gpio_spi1_sck);
    px_gpio_init(&px_gpio_spi1_miso);
    px_gpio_init(&px_gpio_spi1_mosi);
    px_gpio_init(&px_gpio_usr_led);

    /* PA14 is shared with SWCLK on the board and is the active-high button. */
    px_gpio_init(&px_gpio_usr_btn);
}

/* _____GLOBAL FUNCTIONS_____________________________________________________ */
void px_board_init(void)
{
    px_board_clocks_init();
    px_board_gpio_init();
    px_board_delay_init();
}

void px_board_spi_cs_lo(uint8_t cs_id)
{
    if(cs_id == PX_BOARD_SPI1_CS)
    {
        px_gpio_out_set_lo(&px_gpio_spi1_cs);
    }
}

void px_board_spi_cs_hi(uint8_t cs_id)
{
    if(cs_id == PX_BOARD_SPI1_CS)
    {
        px_gpio_out_set_hi(&px_gpio_spi1_cs);
    }
}

void px_board_delay_init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);
    TIM14->PSC = PX_UDIV_ROUND(PX_BOARD_PER_CLK_HZ, 1000000ul) - 1;
    TIM14->ARR = 0xffff;
    TIM14->CR1 |= TIM_CR1_OPM;
}

void px_board_delay_deinit(void)
{
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM14);
}

void px_board_delay_us(uint16_t delay_us)
{
    TIM14->ARR = delay_us;
    TIM14->EGR = TIM_EGR_UG;
    TIM14->CR1 |= TIM_CR1_CEN;
    while(TIM14->CR1 & TIM_CR1_CEN) {;}
}

void px_board_delay_ms(uint16_t delay_ms)
{
    while(delay_ms != 0)
    {
        px_board_delay_us(1000);
        delay_ms--;
    }
}
