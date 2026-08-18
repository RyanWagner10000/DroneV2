/*
 * file: rcc.c
 * description: file that contains the helper functions for RCC or Timers
 * author: Ryan Wagner
 * date: August 9, 2026
 * notes:
 */

#include "rcc.h"

/**
 * @brief Initialize RCC peripheral
 *
 * @param None
 *
 * @return None
 */
void init_rcc(void)
{
    // Enable HSE source to feed PLL
    RCC->CR |= (1U << 16);
    // Wait until it's ready by polling HSERDY
    while (!(RCC->CR & 0x20000))
    {
        ;
    }

    // Set FLASH to have 3 wait states for memory timing stability
    init_flash_96MHz();

    // Make sure PLL is off before making changes
    RCC->CR &= ~(1U << 24);
    // Wait till PLLRDY is 0
    while (RCC->CR & 0x2000000)
    {
        ;
    }

    // Configure PLL register
    uint32_t pllcfgr = RCC->PLLCFGR;
    // Clear, then wrtie M section
    pllcfgr &= ~(63U << 0);
    pllcfgr |= (25U << 0); // Set to / 25
    // Clear, then wrtie N section
    pllcfgr &= ~(511U << 6);
    pllcfgr |= (0xC0 << 6); // Set to * 192
    // Clear, then wrtie P section
    pllcfgr &= ~(3U << 16); // Just leave 0 for /2
    // Clear, then wrtie Q section
    pllcfgr &= ~(15U << 24);
    pllcfgr |= (4U << 24); // Set to * 4
    // Write final value to register
    RCC->PLLCFGR = pllcfgr;

    // Enable PLL
    RCC->CR |= (1U << 24);
    // Wait till ready
    while (!(RCC->CR & 0x2000000))
    {
        ;
    }

    // Set Bus Prescalars
    // APB2 High-speed max is 100MHz, so no division
    RCC->CFGR &= ~(7U << 13);
    // APB1 Low-speed max is 50MHz, so /2 division
    RCC->CFGR &= ~(7U << 10);
    RCC->CFGR |= (4U << 10);
    // AHB High-speed max is 100MHz, so no division
    RCC->CFGR &= ~(15U << 4);
    

    // Change the System Clock Switch to PLL instead of HSI
    RCC->CFGR |= (2U << 0);
    // Wait until system clock switch status is set
    while ((RCC->CFGR & 0x0C) != 8U)
    {
        ;
    }

    return;
}