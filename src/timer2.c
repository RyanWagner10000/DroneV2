/*
 * file: timer2.c
 * description: file that contains the helper functions for timer 2
 * author: Ryan Wagner
 * date: August 18, 2026
 * notes:
 */

#include "timer2.h"

/**
 * @brief Turn Buzzer on
 *
 * @param None
 *
 * @return None
 */
void onBuzzer(void)
{
    // Reset counter with new values
    TIM2->EGR |= (1U << 0);
    // Clear interrupt flag
    TIM2->SR &= ~(1U << 0);

    TIM2->CR1 |= (1U << 0);
}

/**
 * @brief Turn Buzzer off
 *
 * @param None
 *
 * @return None
 */
void offBuzzer(void)
{
    TIM2->CR1 &= ~(1U << 0);
}

/**
 * @brief Initialize Timer2 peripheral
 *
 * @param None
 *
 * @return None
 */
void initTimer2(void)
{
    // 1047 Hz timer for Octave 6 Middle C
    // UEF = 48,000,000 / ((479 + 1) * (95 + 1)) = 1047 Hz

    // Enable clock access to General Purpose Timer 2
    RCC->APB1ENR |= (1U << 0);
    // Enable clock access to GPIOA
    RCC->AHB1ENR |= (1U <<0);

    // Configure GPIOA PA0 to output PWM
    GPIOA->MODER |= (2U << 0);
    // Set alternate function to PWM
    GPIOA->AFRL &= ~(15U << 0); // reset
    GPIOA->AFRL |= (1U << 0); // set

    // Set auto reload
    TIM2->CR1 |= (1U << 7);
    // Set Capture Compare for channel 1
    TIM2->CCMR1 &= ~(7U << 4);
    TIM2->CCMR1 |= (6U << 4);
    TIM2->CCMR1 |= (1U << 3);
    // Enable
    TIM2->CCER |= (1U << 0);
    // Set counter to 0
    TIM2->CNT = 0;
    // Set Prescaler
    TIM2->PSC = 479U;
    // Set Auto-reload Register
    TIM2->ARR = 95U;
    
    // Set Capture Compare for channels 1
    // Set to about Middle-C Octave 6
    TIM2->CCR1 = 47;

    // Reset counter with new values
    TIM2->EGR |= (1U << 0);
    // Clear interrupt flag
    TIM2->SR &= ~(1U << 0);

    // TIM2->CR1 |= (1U << 0);

    return;
}