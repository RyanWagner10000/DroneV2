/*
 * file: timer2.c
 * description: file that contains the helper functions for timer 2
 * author: Ryan Wagner
 * date: August 18, 2026
 * notes:
 * Link to site used for timer and frequencies for tones
 * https://docs.madmachine.io/learn/peripherals/buzzer#new-component
 */

#include "timer2.h"

/**
 * @brief Turn Buzzer on
 *
 * @param None
 *
 * @return None
 */
void on_buzzer(void)
{
    // Reset counter with new values
    TIM2->EGR |= (1U << 0);
    // Clear interrupt flag
    TIM2->SR &= ~(1U << 0);

    TIM2->CR1 |= (1U << 0);

    return;
}

/**
 * @brief Turn Buzzer off
 *
 * @param None
 *
 * @return None
 */
void off_buzzer(void)
{
    TIM2->CR1 &= ~(1U << 0);

    return;
}

/**
 * @brief Set frequency of TIM2 CH1
 *
 * @param frequency Frequency value in Hz
 *
 * @return None
 */
void set_frequency(uint32_t frequency)
{
    TIM2->ARR = frequency;
    TIM2->CCR1 = frequency / 2U;

    return;
}

/**
 * @brief Preset noise sequence for success status
 *
 * @param None
 *
 * @return None
 */
void success_noise(void)
{
    volatile uint32_t counter = 0;
    uint32_t max = 550000;

    set_frequency(1911U);
    on_buzzer();
    for (counter = 0; counter < max; ++counter)
    {
        ;
    }
    off_buzzer();

    set_frequency(1516U);
    on_buzzer();
    for (counter = 0; counter < max; ++counter)
    {
        ;
    }
    off_buzzer();

    set_frequency(1275U);
    on_buzzer();
    for (counter = 0; counter < max * 2; ++counter)
    {
        ;
    }
    off_buzzer();

    return;
}

/**
 * @brief Preset noise sequence for failure status
 *
 * @param beeps Number of beeps for warning. -1 means inf.
 * @param fast Whether to beep fast(1) or slow(0)
 *
 * @return None
 */
void fail_noise(int8_t beeps, uint8_t fast)
{
    volatile uint32_t counter = 0;
    uint8_t inf = 0;
    if (beeps < 0)
    {
        beeps = 1;
    }
    
    while (beeps > 0)
    {
        for (counter = 0; counter < 2600000; ++counter)
        {
            ;
        }

        set_frequency(2272U);
        on_buzzer();
        for (counter = 0; counter < 700000; ++counter)
        {
            ;
        }
        off_buzzer();

        if (!fast)
        {
            for (counter = 0; counter < 500000; ++counter)
            {
                ;
            }
        }
        for (counter = 0; counter < 300000; ++counter)
        {
            ;
        }

        set_frequency(2272U);
        on_buzzer();
        for (counter = 0; counter < 700000; ++counter)
        {
            ;
        }
        off_buzzer();

        --beeps;
        if (inf)
        {
            ++beeps;
        }
    }

    return;
}

/**
 * @brief Initialize Timer2 CH1 peripheral
 *
 * @param None
 *
 * @return None
 */
void init_timer2(void)
{
    // Enable clock access to General Purpose Timer 2
    RCC->APB1ENR |= (1U << 0);
    // Enable clock access to GPIOA
    RCC->AHB1ENR |= (1U << 0);

    // Configure GPIOA PA0 to output PWM
    GPIOA->MODER |= (2U << 0);
    // Set alternate function to Timer
    GPIOA->AFRL &= ~(15U << 0); // reset
    GPIOA->AFRL |= (1U << 0);   // set

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
    TIM2->PSC = 47U;
    // Set Auto-reload Register
    TIM2->ARR = 954U;

    // Set Capture Compare for channels 1
    TIM2->CCR1 = 477;

    // Reset counter with new values
    TIM2->EGR |= (1U << 0);
    // Clear interrupt flag
    TIM2->SR &= ~(1U << 0);

    // Don't enable, just init

    return;
}
