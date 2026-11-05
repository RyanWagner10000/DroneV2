/*
 * file: i2c1.c
 * description: file that contains the functions to setup the i2c communication bus
 * author: Ryan Wagner
 * date: October 20, 2026
 * notes:
 */

#include "i2c1.h"
#include "usart.h"

/**
 * @brief Initalize I2C1 for Altimeter module
 *
 * @param None
 *
 * @return None
 *
 * @note Uses fast mode at 400kHz
 */
void init_i2c1(void)
{
    // Enable clock access to GPIOB
    RCC->AHB1ENR |= (1U << 1);

    // Enable clock access to I2C1
    RCC->APB1ENR |= (1U << 21);

    // Set PB6 and PB7 to alternate function mode
    GPIOB->MODER &= ~(3U << 12); // Clear 0b00
    GPIOB->MODER &= ~(3U << 14); // Clear 0b00
    GPIOB->MODER |= (2U << 12);  // Set 0b10
    GPIOB->MODER |= (2U << 14);  // Set 0b10

    // Set PB6 and PB7 output type to open drain
    GPIOB->OTYPER |= (1U << 6);
    GPIOB->OTYPER |= (1U << 7);

    // Set PB6 and PB7 output speed to fast
    GPIOB->OSPEEDR &= ~(3U << 12); // Clear 0b00
    GPIOB->OSPEEDR &= ~(3U << 14); // Clear 0b00
    GPIOB->OSPEEDR |= (2U << 12);  // Set 0b10
    GPIOB->OSPEEDR |= (2U << 14);  // Set 0b10

    // Set PB6 and PB7 to pull-up
    GPIOB->PUPDR &= ~(3U << 12); // Clear 0b00
    GPIOB->PUPDR &= ~(3U << 14); // Clear 0b00
    GPIOB->PUPDR |= (1U << 12);  // Set 0b01
    GPIOB->PUPDR |= (1U << 14);  // Set 0b01

    // Set PB6 and PB7 alternate function
    GPIOB->AFRL &= ~(15U << 24); // Clear 0b0000
    GPIOB->AFRL &= ~(15U << 28); // Clear 0b0000
    GPIOB->AFRL |= (4U << 24);   // Clear 0b0100
    GPIOB->AFRL |= (4U << 28);   // Clear 0b0100

    // Enter software reset I2C
    I2C1->CR1 |= (1U << 15);

    // Exit software reset I2C
    I2C1->CR1 &= ~(1U << 15);

    // Set I2C clock frequency
    I2C1->CR2 &= ~(63U << 0); // Clear 0b000000
    I2C1->CR2 |= (48U << 0);  // Set 0b110000

    // Set I2C mode
    // CCR = (f_PCLK1 / (3 * f_SCL)) = 48000000 / (3 * 400000) = 40
    // I2C1->CCR |= (1U << 15);
    // I2C1->CCR |= (40U << 0);
    // CCR = (f_PCLK1 / (2 * f_SCL)) = 48000000 / (2 * 100000) = 240
    I2C1->CCR |= (240U << 0);

    // Set rise time
    I2C1->TRISE = (49U << 0);

    // Enable I2C
    I2C1->CR1 |= (1U << 0);

    return;
}

/**
 * @brief Receive N-many bytes via I2C1
 *
 * @param slave_address Address of the slave device to communicate with
 * @param mem_address emory address to get data from
 * @param rx_buffer Buffer for received information
 * @param length Length of buffer
 *
 * @return 1 if successful, 0 otherwise
 */
uint8_t read_i2c1(uint8_t slave_address, uint8_t mem_address, uint8_t *rx_buffer, uint8_t length)
{
    uint32_t reset_count = 1000;
    uint32_t count_out = reset_count;

    // Wait until bus not busy
    while (I2C1->SR2 & (1U << 1) && --count_out)
    {
        ;
    }
    if (count_out == 0)
    {
        return 0;
    }
    else
    {
        count_out = reset_count;
    }
    // Generate start
    I2C1->CR1 |= (1U << 8);

    // Wait until start flag is set
    while (!(I2C1->SR1 & (1U << 0)) && --count_out)
    {
        ;
    }
    if (count_out == 0)
    {
        return 0;
    }
    else
    {
        count_out = reset_count;
    }
    // Transmit slave address + write
    I2C1->DR = slave_address << 1;

    // Wait until addr flag is set
    while (!(I2C1->SR1 & (1U << 1)) && --count_out)
    {
        // Check for AF
        if (I2C1->SR1 & (1U << 10))
        {
            // Generate stop
            I2C1->CR1 |= (1U << 9);
            // Reset AF
            I2C1->SR1 &= ~(1U << 10);
            return 0;
        }
    }
    if (count_out == 0)
    {
        return 0;
    }
    else
    {
        count_out = reset_count;
    }
    // Clear addr flag by reading SR2 register after SR1
    (void)I2C1->SR2;

    // Wait until transmitter empty
    while (!(I2C1->SR1 & (1U << 7)) && --count_out)
    {
        ;
    }
    if (count_out == 0)
    {
        return 0;
    }
    else
    {
        count_out = reset_count;
    }
    // Send memory address
    I2C1->DR = mem_address;

    // Wait until transmitter empty
    while (!(I2C1->SR1 & (1U << 7)) && --count_out)
    {
        ;
    }
    if (count_out == 0)
    {
        return 0;
    }
    else
    {
        count_out = reset_count;
    }
    // Generate restart
    I2C1->CR1 |= (1U << 8);

    // Wait until start flag is set
    while (!(I2C1->SR1 & (1U << 0)) && --count_out)
    {
        ;
    }
    if (count_out == 0)
    {
        return 0;
    }
    else
    {
        count_out = reset_count;
    }
    // Transmit slave address + read
    I2C1->DR = slave_address << 1 | 1U;

    // Wait until addr flag is set
    while (!(I2C1->SR1 & (1U << 1)) && --count_out)
    {
        ;
    }
    if (count_out == 0)
    {
        return 0;
    }
    else
    {
        count_out = reset_count;
    }
    // Clear addr flag by reading SR2 register after SR1
    (void)I2C1->SR2;

    // Enable Acknowledge
    I2C1->CR1 |= (1U << 10);

    uint8_t i = 0;
    while (i < length - 1)
    {
        // Wait for RXNE flag to be set
        while (!(I2C1->SR1 & (1U << 6)) && --count_out)
        {
            ;
        }
        if (count_out == 0)
        {
            return 0;
        }
        else
        {
            count_out = reset_count;
        }

        // Read data from register
        rx_buffer[i] = I2C1->DR;

        ++i;
    }
    // Disable Acknowledge
    I2C1->CR1 &= ~(1U << 10);

    // Generate stop
    I2C1->CR1 |= (1U << 9);

    // Wait for RXNE flag to be set
    while (!(I2C1->SR1 & (1U << 6)) && --count_out)
    {
        ;
    }
    if (count_out == 0)
    {
        return 0;
    }
    else
    {
        count_out = reset_count;
    }
    // Read data from register
    rx_buffer[i] = I2C1->DR;

    return 1;
}

/**
 * @brief Transmit N-many bytes via I2C1
 *
 * @param slave_address Address of the slave device to communicate with
 * @param mem_address emory address to get data from
 * @param tx_buffer Buffer of information to send
 * @param length Length of buffer
 *
 * @return None
 */
void write_i2c1(uint8_t slave_address, uint8_t mem_address, uint8_t *tx_buffer, uint8_t length)
{
    // Wait until bus not busy
    while (I2C1->SR2 & (1U << 1))
    {
        ;
    }

    // Generate start
    I2C1->CR1 |= (1U << 8);

    // Wait until start flag is set
    while (!(I2C1->SR1 & (1U << 0)))
    {
        ;
    }

    // Transmit slave address + write
    I2C1->DR = slave_address << 1;

    // Wait until addr flag is set
    while (!(I2C1->SR1 & (1U << 1)))
    {
        ;
    }

    // Clear addr flag by reading SR2 register after SR1
    uint32_t tmp = I2C1->SR2;
    tmp++;

    // Wait until transmitter empty
    while (!(I2C1->SR1 & (1U << 7)))
    {
        ;
    }

    // Send memory address
    I2C1->DR = mem_address;

    // TODO HERE IS THE BREAKPOINT FOR FOR-LOOP

    // Wait until transmitter empty
    while (!(I2C1->SR1 & (1U << 7)))
    {
        ;
    }

    uint8_t i = 0;
    while (i < length)
    {
        // Wait for TXNE flag to NOT be set
        while (!(I2C1->SR1 & (1U << 7)))
        {
            ;
        }

        // Read data from register
        I2C1->DR = tx_buffer[i];

        ++i;
    }

    // Wait for transfer to finish
    while (!(I2C1->SR1 & (1U << 2)))
    {
        ;
    }

    // Generate stop
    I2C1->CR1 |= (1U << 9);

    return;
}
