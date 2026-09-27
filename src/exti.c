/*
 * file: exti.c
 * description: file that sets up any external interrupts from sensors or whatnot
 * author: Ryan Wagner
 * date: September 20, 2026
 * notes:
 */

#include "exti.h"

static inline void __disable_irq(void)
{
    __asm__ volatile("cpsid i" : : : "memory");
}

static inline void __enable_irq(void)
{
    __asm__ volatile("cpsie i" : : : "memory");
}

/**
 * @brief Function to init the pins used for IMU external interrupts
 *
 * @param None
 *
 * @return None
 */
void init_imu_exti(void)
{

    // Disable interrupts
    __disable_irq();

    // Enable clock access to GPIOA
    RCC->AHB1ENR |= (1U << 0);

    // Set PA1 and PA4 as input
    GPIOA->MODER &= ~(3U << 2);
    GPIOA->MODER &= ~(3U << 8);

    // Enable clock access to SYSCFG
    RCC->APB2ENR |= (1U << 14);

    // Select Port A for pins 1 and 4 on EXTI1 and EXTI4, respectively
    SYSCFG->EXTICR1 &= ~(15U << 4);
    SYSCFG->EXTICR2 &= ~(15U << 0);

    // Unmask EXTI1 and EXTI4
    EXTI->IMR |= (1U << 1);
    EXTI->IMR |= (1U << 4);

    // Select rising edge trigger
    EXTI->RTSR |= (1U << 1);
    EXTI->RTSR |= (1U << 4);

    // Enable NVIC bits for EXTI1 and EXTI4
    NVIC_SetPriority(EXTI1_IRQn, 1);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_SetPriority(EXTI4_IRQn, 1);
    NVIC_EnableIRQ(EXTI4_IRQn);

    // Enable Interrupts
    __enable_irq();

    return;
}

/**
 * @brief Function to init the pins used for radio external interrupts
 *
 * @param None
 *
 * @return None
 */
void init_radio_exti(void)
{

    // Disable interrupts
    __disable_irq();

    // Enable clock access to GPIOB
    RCC->AHB1ENR |= (1U << 1);

    // Set PB9 input and Pull-Up resistor
    GPIOB->MODER &= ~(3U << 18);
    GPIOB->PUPDR |= (1U << 18);
    GPIOB->PUPDR &= ~(1U << 19);

    // Enable clock access to SYSCFG
    RCC->APB2ENR |= (1U << 14);

    // Select Port B for pin 9 on EXTI3
    SYSCFG->EXTICR3 |= (1U << 4);

    // Unmask EXTI9
    EXTI->IMR |= (1U << 9);

    // Select falling edge trigger
    EXTI->FTSR |= (1U << 9);

    // Enable NVIC bits for EXTI1 and EXTI4
    NVIC_SetPriority(EXTI9_5_IRQn, 0);
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    // Enable Interrupts
    __enable_irq();

    return;
}