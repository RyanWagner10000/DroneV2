/*
 * file: fpu.c
 * description: file that sets up the SPI1 communication
 * author: Ryan Wagner
 * date: August 29, 2026
 * notes:
 */

#include "spi1.h"

/**
 * @brief Initalize SPI1 for IMU module
 *
 * @param None
 *
 * @return None
 *
 * @note Uses bus clock frequency / 16 for timing
 */
void init_spi1(void)
{

    // Enable clock to GPIOB
    RCC->AHB1ENR |= (1U << 1);

    // Enable clock to SPI1
    RCC->APB2ENR |= (1U << 12);

    // Set pin mode for PA5, PA6, PA7 to alternate function
    // Set pin mode for PB0, and PB1 to output for chip select
    // Clear first
    GPIOA->MODER &= ~(3U << 10); // Clear 0b00
    GPIOA->MODER &= ~(3U << 12); // Clear 0b00
    GPIOA->MODER &= ~(3U << 14); // Clear 0b00
    GPIOB->MODER &= ~(3U << 0);  // Clear 0b00
    GPIOB->MODER &= ~(3U << 2);  // Clear 0b00
    // Then set
    GPIOA->MODER |= (2U << 10);  // AF mode 0b10
    GPIOA->MODER |= (2U << 12);  // AF mode 0b10
    GPIOA->MODER |= (2U << 14); // AF mode 0b10
    GPIOB->MODER |= (1U << 0); // Output mode 0b01
    GPIOB->MODER |= (1U << 2); // Output mode 0b01

    // Set alternate function mode for PA5, PA6, PA7
    // Clear first
    GPIOA->AFRL &= ~(15U << 20); // Clear 0b0000
    GPIOA->AFRL &= ~(15U << 24); // Clear 0b0000
    GPIOA->AFRL &= ~(15U << 28); // Clear 0b0000
    // Then set
    GPIOA->AFRL |= (5U << 20); // AFRL5 -> 0b0101 = AF5
    GPIOA->AFRL |= (5U << 24); // AFRL6 -> 0b0101 = AF5
    GPIOA->AFRL |= (5U << 28); // AFRL7 -> 0b0101 = AF5

    // Initialize CS Pin to high
    GPIOB->ODR |= (1U << 0);
    GPIOB->ODR |= (1U << 1);

    // Set output type to push-pull
    GPIOA->OTYPER &= ~(1U << 3);
    GPIOA->OTYPER &= ~(1U << 4);
    GPIOA->OTYPER &= ~(1U << 5);

    // Set high speed for SPI pins
    GPIOA->OSPEEDR |= (3U << 6);
    GPIOA->OSPEEDR |= (3U << 8);
    GPIOA->OSPEEDR |= (3U << 10);

    // No pull-up / pull-down
    GPIOA->PUPDR &= ~(3U << 6);
    GPIOA->PUPDR &= ~(3U << 8);
    GPIOA->PUPDR &= ~(3U << 10);

    // Clear config to init
    SPI1->CR1 = 0x0000;

    // Set clock to fPCLK/16 = 96MHz/16 = 6MHz
    SPI1->CR1 |= (3U << 3);

    // Set CPHA and CPOL to 1 (Mode 3) to determine behavior
    SPI1->CR1 |= (3U << 0);
    // SPI1->CR1 &= ~(3U << 0);

    // Set MSB first
    SPI1->CR1 &= ~(1U << 7);

    // Set mode to Master
    SPI1->CR1 |= (1U << 2);

    // Select software slave management SSM = 1 & SSI = 1
    SPI1->CR1 |= (1U << 9); // SSM
    SPI1->CR1 |= (1U << 8); // SSI

    // Turn on SPI1
    SPI1->CR1 |= (1U << 6);

    return;
}

void transfer_spi1(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length)
{
    uint8_t i = 0;
    while (i < length)
    {
        // Wait until TXE is set
        while (!(SPI1->SR & (1U << 1)))
            ;

        // Write data to register
        SPI1->DR = tx_buffer[i];

        // Wait for RXNE to be set
        while (!(SPI1->SR & (1U << 0)))
            ;

        // Read data from register
        rx_buffer[i] = (SPI1->DR) & 0xFF;

        ++i;
    }

    // Wait for BUSY flag to reset
    while ((SPI1->SR & (1U << 7)))
        ;

    // Drain the RX buffer of the junk byte clocked in during TX
    while (SPI1->SR & (1U << 0))
    {
        (void)SPI1->DR;
    }
    // Clear OVR flag
    (void)SPI1->SR; // Clear OVR

    return;
}

/**
 * @brief Enable the Chip Select pin on GPIO PB0
 *
 * @param None
 *
 * @return None
 */
void enable_csag_spi1(void)
{
    // Turn on SPI to device
    GPIOB->ODR &= ~(1U << 0);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Enable the Chip Select pin on GPIO PB1
 *
 * @param None
 *
 * @return None
 */
void enable_csm_spi1(void)
{
    // Turn on SPI to device
    GPIOB->ODR &= ~(1U << 1);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Select pin on GPIO PB0
 *
 * @param None
 *
 * @return None
 */
void disable_csag_spi1(void)
{

    // Turn off SPI to device
    GPIOB->ODR |= (1U << 0);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Select pin on GPIO PB1
 *
 * @param None
 *
 * @return None
 */
void disable_csm_spi1(void)
{

    // Turn off SPI to device
    GPIOB->ODR |= (1U << 1);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}