/*
 * file: fpu.c
 * description: file that sets up the SPI3 communication
 * author: Ryan Wagner
 * date: August 29, 2026
 * notes:
 */

#include "spi3.h"

/**
 * @brief Initalize SPI3 for IMU module
 *
 * @param None
 *
 * @return None
 *
 * @note Uses bus clock frequency / 16 for timing
 */
void init_spi3(void)
{

    // Enable clock to GPIOA & GPIOB
    RCC->AHB1ENR |= (1U << 0);
    RCC->AHB1ENR |= (1U << 1);

    // Enable clock to SPI3
    RCC->APB1ENR |= (1U << 15);

    // Set pin mode for PB3, PB4, PB5 to alternate function
    // Set pin mode for PA12 and PA15 to output for chip select
    // Clear first
    GPIOB->MODER &= ~(3U << 6); // Clear 0b00
    GPIOB->MODER &= ~(3U << 8); // Clear 0b00
    GPIOB->MODER &= ~(3U << 10); // Clear 0b00
    GPIOA->MODER &= ~(3U << 24);  // Clear 0b00
    GPIOA->MODER &= ~(3U << 30);  // Clear 0b00
    // Then set
    GPIOB->MODER |= (2U << 10); // AF mode 0b10
    GPIOB->MODER |= (2U << 12); // AF mode 0b10
    GPIOB->MODER |= (2U << 14); // AF mode 0b10
    GPIOA->MODER |= (1U << 24);  // Output mode 0b01
    GPIOA->MODER |= (1U << 30);  // Output mode 0b01

    // Set alternate function mode for PB3, PB4, PB5
    // Clear first
    GPIOB->AFRL &= ~(15U << 12); // Clear 0b0000
    GPIOB->AFRL &= ~(15U << 16); // Clear 0b0000
    GPIOB->AFRL &= ~(15U << 20); // Clear 0b0000
    // Then set
    GPIOB->AFRL |= (6U << 12); // AFRL3 -> 0b0110 = AF6
    GPIOB->AFRL |= (6U << 16); // AFRL4 -> 0b0110 = AF6
    GPIOB->AFRL |= (6U << 20); // AFRL5 -> 0b0110 = AF6

    // Initialize CS Pin to high
    GPIOA->ODR |= (1U << 12);
    GPIOA->ODR |= (1U << 15);

    // Set output type to push-pull
    GPIOB->OTYPER &= ~(1U << 3);
    GPIOB->OTYPER &= ~(1U << 4);
    GPIOB->OTYPER &= ~(1U << 5);

    // Set high speed for SPI pins
    GPIOB->OSPEEDR |= (2U << 6);
    GPIOB->OSPEEDR |= (2U << 8);
    GPIOB->OSPEEDR |= (2U << 10);

    // No pull-up / pull-down
    GPIOB->PUPDR &= ~(3U << 6);
    GPIOB->PUPDR &= ~(3U << 8);
    GPIOB->PUPDR &= ~(3U << 10);

    // Clear config to init
    SPI3->CR1 = 0x0000;

    // Set clock to fPCLK/16 = 48MHz/8 = 6MHz
    SPI3->CR1 |= (2U << 3);

    // Set CPHA and CPOL to 1 (Mode 3) to determine behavior
    SPI3->CR1 |= (3U << 0);
    // SPI3->CR1 &= ~(3U << 0);

    // Set MSB first
    SPI3->CR1 &= ~(1U << 7);

    // Set mode to Master
    SPI3->CR1 |= (1U << 2);

    // Select software slave management SSM = 1 & SSI = 1
    SPI3->CR1 |= (1U << 9); // SSM
    SPI3->CR1 |= (1U << 8); // SSI

    // Turn on SPI3
    SPI3->CR1 |= (1U << 6);

    return;
}

void transfer_spi3(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length)
{
    uint8_t i = 0;
    while (i < length)
    {
        // Wait until TXE is set
        while (!(SPI3->SR & (1U << 1)))
            ;

        // Write data to register
        SPI3->DR = tx_buffer[i];

        // Wait for RXNE to be set
        while (!(SPI3->SR & (1U << 0)))
            ;

        // Read data from register
        rx_buffer[i] = (SPI3->DR) & 0xFF;

        ++i;
    }

    // Wait for BUSY flag to reset
    while ((SPI3->SR & (1U << 7)))
        ;

    // Drain the RX buffer of the junk byte clocked in during TX
    while (SPI3->SR & (1U << 0))
    {
        (void)SPI3->DR;
    }
    // Clear OVR flag
    (void)SPI3->SR; // Clear OVR

    return;
}

/**
 * @brief Enable the Chip Select pin on GPIO PA12
 *
 * @param None
 *
 * @return None
 */
void enable_ce(void)
{
    // Turn on SPI to device
    GPIOA->ODR &= ~(1U << 12);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Enable the Chip Select pin on GPIO PA15
 *
 * @param None
 *
 * @return None
 */
void enable_csn(void)
{
    // Turn on SPI to device
    GPIOA->ODR &= ~(1U << 15);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Select pin on GPIO PA12
 *
 * @param None
 *
 * @return None
 */
void disable_ce(void)
{

    // Turn off SPI to device
    GPIOA->ODR |= (1U << 12);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Select pin on GPIO PA15
 *
 * @param None
 *
 * @return None
 */
void disable_csn(void)
{

    // Turn off SPI to device
    GPIOA->ODR |= (1U << 15);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}