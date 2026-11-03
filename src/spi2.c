/*
 * file: spi2.c
 * description: file that sets up the SPI2 communication
 * author: Ryan Wagner
 * date: October 31, 2026
 * notes:
 */

#include "spi2.h"
#include "usart.h"

/**
 * @brief Initalize SPI2 for IMU module
 *
 * @param None
 *
 * @return None
 *
 * @note Uses bus clock frequency / 16 for timing
 */
void init_spi2(void)
{

    // Enable clock to GPIOB
    RCC->AHB1ENR |= (1U << 1);

    // Enable clock to SPI2
    RCC->APB1ENR |= (1U << 14);

    // Set pin mode for PB13, PB14, PB15 to alternate function
    // Set pin mode for PB10 and PB12 to output for chip select
    // Clear first
    GPIOB->MODER &= ~(3U << 26); // Clear 0b00
    GPIOB->MODER &= ~(3U << 28); // Clear 0b00
    GPIOB->MODER &= ~(3U << 30); // Clear 0b00
    GPIOB->MODER &= ~(3U << 20); // Clear 0b00
    GPIOB->MODER &= ~(3U << 24); // Clear 0b00
    // Then set
    GPIOB->MODER |= (2U << 26); // AF mode 0b10
    GPIOB->MODER |= (2U << 28); // AF mode 0b10
    GPIOB->MODER |= (2U << 30); // AF mode 0b10
    GPIOB->MODER |= (1U << 20); // Output mode 0b01
    GPIOB->MODER |= (1U << 24); // Output mode 0b01

    // Set alternate function mode for PB13, PB14, PB15
    // Clear first
    GPIOB->AFRH &= ~(15U << 20); // Clear 0b0000
    GPIOB->AFRH &= ~(15U << 24); // Clear 0b0000
    GPIOB->AFRH &= ~(15U << 28); // Clear 0b0000
    // Then set
    GPIOB->AFRH |= (5U << 20); // AFRH13 -> 0b0101 = AF5
    GPIOB->AFRH |= (5U << 24); // AFRH14 -> 0b0101 = AF5
    GPIOB->AFRH |= (5U << 28); // AFRH15 -> 0b0101 = AF5

    // Initialize CS Pin to high
    GPIOB->ODR |= (1U << 10);
    GPIOB->ODR |= (1U << 12);

    // Set output type to push-pull
    GPIOB->OTYPER &= ~(1U << 13);
    GPIOB->OTYPER &= ~(1U << 14);
    GPIOB->OTYPER &= ~(1U << 15);

    // Set high speed for SPI pins
    GPIOB->OSPEEDR |= (3U << 26);
    GPIOB->OSPEEDR |= (3U << 28);
    GPIOB->OSPEEDR |= (3U << 30);

    // No pull-up / pull-down
    GPIOB->PUPDR &= ~(3U << 26);
    GPIOB->PUPDR &= ~(3U << 28);
    GPIOB->PUPDR &= ~(3U << 30);

    // Clear config to init
    SPI2->CR1 = 0x0000;

    // Set clock to fPCLK/8 = 48MHz/8 = 6MHz
    SPI2->CR1 |= (2U << 3);

    // Set CPHA and CPOL to 1 (Mode 3) to determine behavior
    SPI2->CR1 |= (3U << 0);
    // SPI2->CR1 &= ~(3U << 0);

    // Set MSB first
    SPI2->CR1 &= ~(1U << 7);

    // Set mode to Master
    SPI2->CR1 |= (1U << 2);

    // Select software slave management SSM = 1 & SSI = 1
    SPI2->CR1 |= (1U << 9); // SSM
    SPI2->CR1 |= (1U << 8); // SSI

    // Turn on SPI2
    SPI2->CR1 |= (1U << 6);

    return;
}

/**
 * @brief Transmit and/or receive N-many bytes via SPI2
 *
 * @param tx_buffer Buffer of transmit information
 * @param rx_buffer Buffer for received information
 * @param length Length of buffers
 *
 * @return None
 */
void transfer_spi2(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length)
{
    uint8_t i = 0;
    while (i < length)
    {
        // Wait until TXE is set
        while (!(SPI2->SR & (1U << 1)))
            ;

        // Write data to register
        SPI2->DR = tx_buffer[i];

        // Wait for RXNE to be set
        while (!(SPI2->SR & (1U << 0)))
            ;

        // Read data from register
        rx_buffer[i] = (SPI2->DR) & 0xFF;

        ++i;
    }

    // Wait for BUSY flag to reset
    while ((SPI2->SR & (1U << 7)))
        ;

    // Drain the RX buffer of the junk byte clocked in during TX
    while (SPI2->SR & (1U << 0))
    {
        (void)SPI2->DR;
    }
    // Clear OVR flag
    (void)SPI2->SR; // Clear OVR

    return;
}

/**
 * @brief Enable the Chip Select pin on GPIO PB10
 *
 * @param None
 *
 * @return None
 */
void enable_cs_flash(void)
{
    // Turn on SPI to device
    GPIOB->ODR &= ~(1U << 10);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Enable the Chip Select pin on GPIO PB12
 *
 * @param None
 *
 * @return None
 */
void enable_cs_bmp(void)
{
    // Turn on SPI to device
    GPIOB->ODR &= ~(1U << 12);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Select pin on GPIO PB10
 *
 * @param None
 *
 * @return None
 */
void disable_cs_flash(void)
{

    // Turn off SPI to device
    GPIOB->ODR |= (1U << 10);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}

/**
 * @brief Disable the Chip Select pin on GPIO PB12
 *
 * @param None
 *
 * @return None
 */
void disable_cs_bmp(void)
{

    // Turn off SPI to device
    GPIOB->ODR |= (1U << 12);

    // Small delay
    for (volatile uint32_t i = 0; i < 10; i++)
        ;

    return;
}