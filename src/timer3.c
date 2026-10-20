/*
 * file: timer11.c
 * description: file that contains the helper functions for timer 11
 * author: Ryan Wagner
 * date: October 1, 2026
 * notes:
 */

#include "timer3.h"

volatile uint8_t ONE_SEC_FLAG = 0;

void restart_one_sec_timer(void)
{
    TIM3->CNT = 0;
    ONE_SEC_FLAG = 0;
    return;
}

void reset_one_sec_flag(void)
{
    ONE_SEC_FLAG = 0;
}

void set_one_sec_flag(void)
{
    ONE_SEC_FLAG = 1;
}

uint8_t get_one_sec_flag(void)
{
    return ONE_SEC_FLAG;
}

/**
 * @brief Initialize Timer3 peripheral
 *
 * @param None
 *
 * @return None
 */
void init_timer3(void)
{
    // Enable clock access to General Purpose Timer 3
    RCC->APB1ENR |= (1U << 1);

    // clock/((PSC+1)*(ARR+1)) = Frequency
    // 96,000,000/((9599+1)*(9999+1)) = 1Hz = 1s

    // Set Prescaler
    TIM3->PSC = 9599U;

    // Set Auto-reload Register
    TIM3->ARR = 9999U;

    // Set counter to 0
    TIM3->CNT = 0;

    TIM3->SR = 0;

    // Enable update interrupt
    TIM3->DIER |= (1U << 0);

    // Enable NVIC Interrupt
    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);

    // Enable
    TIM3->CR1 |= (1U << 0);

    return;
}
