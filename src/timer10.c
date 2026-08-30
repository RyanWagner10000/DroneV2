/*
 * file: timer10.c
 * description: file that contains the helper functions for timer 10
 * author: Ryan Wagner
 * date: August 28, 2026
 * notes:
 */

#include "timer10.h"

/**
 * @brief Initialize Timer10 peripheral
 *
 * @param None
 *
 * @return None
 */
void init_timer10(void)
{
    // Enable clock access to General Purpose Timer 10
    RCC->APB2ENR |= (1U << 17);

    // clock/((PSC+1)*(ARR+1)) = Frequency
    // 96,000,000/((95+1)*(999+1)) = 1000Hz = 1ms

    // Set Prescaler
    TIM10->PSC = 95U;

    // Set Auto-reload Register
    TIM10->ARR = 999U;

    // Set counter to 0
    TIM10->CNT = 0;

    // Enable
    TIM10->CR1 |= (1U << 0);

    return;
}

/**
 * @brief Basic delay function in 1ms increments
 *
 * @param num_milliseconds Number of milliseconds to delay
 *
 * @return None
 */
void delay_millisecond(uint32_t num_milliseconds)
{
    for (uint32_t i = 0; i < num_milliseconds; i++)
    {
        TIM10->CNT = 0;
        while (TIM10->CNT < 999U)
            ;
    }

    return;
}