#include <stdint.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include "delay.h"

void delay_setup(void) {
    rcc_periph_clock_enable(RCC_TIM2);
    timer_set_prescaler(TIM2, rcc_apb1_frequency / 1000000 - 1);
    timer_set_period(TIM2, 0xffff);
    timer_one_shot_mode(TIM2);
}

void delay_us(uint16_t us) {
    TIM_ARR(TIM2) = us;
    TIM_EGR(TIM2) = TIM_EGR_UG;
    TIM_CR1(TIM2) |= TIM_CR1_CEN;
    while (TIM_CR1(TIM2) & TIM_CR1_CEN);
}
