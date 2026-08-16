/*
 * file: gpio.c
 * description: file that contains helper functions for various things on GPIO pins
 * author: Ryan Wagner
 * date: August 9, 2026
 * notes:
 */

#include "gpio.h"

/**
 * @brief Initiate the Blue LED on pin on PD13
 *
 * @param None
 *
 * @return None
 */
void initBlueLED(void)
{
    // Enable clock access to GPIOD
    RCC->AHB1ENR |= GPIOCEN;

    // Set mode to output
    GPIOC->MODER |= (1U << 26);
    GPIOC->MODER &= ~(1U << 27);

    return;
}

/**
 * @brief Initiate the Red LED on pin on PD14
 *
 * @param None
 *
 * @return None
 */
void initRedLED(void)
{
    // Enable clock access to GPIOD
    RCC->AHB1ENR |= GPIOCEN;

    // Set PD14 mode to output
    GPIOC->MODER |= (1U << 28);
    GPIOC->MODER &= ~(1U << 29);

    return;
}

/**
 * @brief Initiate the Green LED on pin on PC15
 *
 * @param None
 *
 * @return None
 */
void initGreenLED(void)
{
    // Enable clock access to GPIOD
    RCC->AHB1ENR |= GPIOCEN;

    // Set PD12 mode to output
    GPIOC->MODER |= (1U << 30);
    GPIOC->MODER &= ~(1U << 31);

    return;
}

/**
 * @brief Turn on LED
 *
 * @param led RED/BLUE/GREEN_LED variable
 *
 * @return None
 */
void onLED(uint32_t led)
{
    // Set pin High
    GPIOC->BSRR |= led;

    return;
}

/**
 * @brief Turn off LED
 *
 * @param led RED/BLUE/GREEN_LED variable
 *
 * @return None
 */
void offLED(uint32_t led)
{
    // Set pin High
    GPIOC->BSRR |= (led << 16);

    return;
}

/**
 * @brief Toggle LED to opposite state
 *
 * @param led RED/BLUE/GREEN_LED variable
 *
 * @return None
 */
void toggleLED(uint32_t led)
{
    // Set output of pin High/Low
    GPIOC->ODR ^= led;

    return;
}
