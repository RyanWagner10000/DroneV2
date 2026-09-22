/*
 * file: uasrt.c
 * description: file that contains the helper functions for USART communicaiton
 * author: Ryan Wagner
 * date: August 30, 2026
 * notes:
 */

#include "usart.h"

/**
 * @brief Initialize USART2 peripheral
 *
 * @param None
 *
 * @return None
 */
void init_usart(void)
{
    // Allow clock access to USART2 on bus APB1
    RCC->APB1ENR |= (1U << 17);
    // Allow clock access to GPIOA on bus AHB1
    RCC->AHB1ENR |= (1U << 0);

    // Set USART2 PA2 & PA3 Pin Mode
    GPIOA->MODER |= (1U << 7);
    GPIOA->MODER &= ~(1U << 6);
    GPIOA->MODER |= (1U << 5);
    GPIOA->MODER &= ~(1U << 4);

    // Set USART2 alternate function mode
    // AF7 for PA3
    GPIOA->AFRL &= ~(15U << 12);
    GPIOA->AFRL |= (7U << 12);
    // AF7 for PA2
    GPIOA->AFRL &= ~(1U << 8);
    GPIOA->AFRL |= (7U << 8);

    // Configure baud: 26.04167
    USART2->BRR = 0x1A1;

    // Enable Tx and Rx on CR1
    USART2->CR1 |= (1U << 3); // Tx
    USART2->CR1 |= (1U << 2); // Rx

    // Enable USART2
    USART2->CR1 |= (1U << 13);
}

/**
 * @brief Calculate the length of a character array
 *
 * @param s Character array
 *
 * @return Size of input character array
 */
uint32_t get_string_length(const char *s)
{
    uint32_t length = 0;

    while (s[length] != '\0')
    {
        length++;
    }

    return length;
}

/**
 * @brief Write a "string" to the USART2 peripheral
 *
 * @param string Character array to "print" to the terminal
 *
 * @return None
 */
void usart_write_string(const char *string)
{
    int32_t str_len = get_string_length(string);
    char char_i = 0;

    for (int32_t i = 0; i < str_len; i++)
    {
        // Make sure the transmit data register is NOT empty
        while (!(USART2->SR & (1U << 7)))
            ;

        // Get first byte of of input
        char_i = string[i];
        USART2->DR = (char_i & 0xFF);

        // Wait until done transmitting
        while (!(USART2->SR & (1U << 6)))
            ;
    }

    return;
}

/**
 * @brief Write a single character to the USART2 peripheral
 *
 * @param character ASCII style character to "print" to the terminal
 *
 * @return None
 */
void usart_write_char(uint8_t character)
{
    // Make sure the transmit data register is NOT empty
    while (!(USART2->SR & (1U << 7)))
        ;

    // Get first byte of of input
    USART2->DR = (character & 0xFF);

    // Wait until done transmitting
    while (!(USART2->SR & (1U << 6)))
        ;

    return;
}

/**
 * @brief Write an integer to the USART2 peripheral
 *
 * @param value Unsigned 32-bit integer to "print" to the terminal
 *
 * @return None
 */
void usart_write_number(int32_t value)
{
    // Max 10 digits for int32_t + null terminator + 1 extra for sign
    char buffer[12]; 
    int i = 0;

    // Handle zero case
    if (value == 0)
    {
        usart_write_string("0");
        return;
    }

    uint8_t is_neg = 0;
    if (value < 0)
    {
        is_neg = 1;
        value *= -1; // Make positive
    }

    // Convert number to string (reversed)
    while (value > 0)
    {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    // Add the negative sign
    if (is_neg)
    {
        buffer[i++] = '-';
    }

    // Print in correct order (reverse through the buffer)
    while (i > 0)
    {
        usart_write_char(buffer[--i]);
    }

    // usart_write_char('\n');
}

/**
 * @brief Concatenate two character arrays into one
 *
 * @param str1 First character array
 * @param str2 Second character array
 * @param concat Resulting joining character array
 *
 * @return None
 *
 * @note Size of concat vairable must be of sufficient size to hold both strings or extra characters will be lost
 */
void str_concat(char *str1, char *str2, char *concat)
{
    uint32_t i = 0;
    uint32_t j = 0;

    while (str1[i] != '\0' && i < MAX_STRING_CONCAT)
    {
        concat[i] = str1[i];
        i++;
    }

    if (i == MAX_STRING_CONCAT - 1)
    {
        concat[i] = '\0';
        return;
    }

    /* Copy characters from the source string to the end of the destination string */
    while (str2[j] != '\0' && i < MAX_STRING_CONCAT)
    {
        concat[i] = str2[j];
        i++;
        j++;
    }

    /* Null-terminate the final concatenated string */
    concat[i] = '\0';

    return;
}

/**
 * @brief Reverse a string
 *
 * @param str Given charcter array to reverse
 * @param len Length of charcter array
 *
 * @return None
 */
void reverse(char *str, uint32_t len)
{
    uint32_t i = 0, j = len - 1;
    while (i < j)
    {
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

/**
 * @brief Convert an integer to a character array
 *
 * @param n Integer to convert to string
 * @param buffer Buffer to store digits as characters
 *
 * @return Length of the converted string
 */
uint32_t int_to_str(int32_t n, char *buffer)
{
    uint32_t length = 0;
    uint32_t is_neg = 0;
    uint32_t num; // Use unsigned for the actual conversion

    if (n == 0)
    {
        buffer[length++] = '0';
        buffer[length] = '\0';
        return length;
    }

    if (n < 0)
    {
        is_neg = 1;
        num = (uint32_t)(-n); // Convert to unsigned after negating
    }
    else
    {
        num = (uint32_t)n;
    }

    while (num != 0)
    {
        uint32_t rem = num % 10;
        buffer[length++] = rem + '0';
        num = num / 10;
    }

    if (is_neg)
    {
        buffer[length++] = '-';
    }

    buffer[length] = '\0';
    reverse(buffer, length);

    return length;
}

/**
 * @brief Convert a float to a character array
 *
 * @param n Float to convert to string
 * @param buffer Buffer to store digits as characters
 * @param afterpoint Decimal point resolution of the float conversion
 *
 * @return None
 */
void float_to_str(float n, char *buffer, uint32_t afterpoint)
{
    afterpoint++;

    // Handle negative numbers
    uint32_t index = 0;
    if (n < 0.0f)
    {
        buffer[index++] = '-';
        n = -n;
    }

    // Extract integer part as an actual integer
    uint32_t ipart = (uint32_t)n;

    // Extract fractional part
    float fpart = n - (float)ipart;

    // Convert integer part to string
    int32_t i = int_to_str(ipart, buffer + index);
    i += index;

    // Check for decimal part
    if (afterpoint != 0)
    {
        buffer[i++] = '.';

        // Get fractional digits
        for (uint32_t k = 0; k < afterpoint; k++)
        {
            fpart *= 10.0f;
            uint32_t digit = (uint32_t)fpart;
            buffer[i++] = '0' + digit;
            fpart -= (float)digit;
        }
        buffer[i] = '\0';
    }

    return;
}

/**
 * @brief Output contents of register to temrinal via USART2
 *
 * @param address Address of register read from
 * @param data Data read from address
 * @param format Format to print the data (Decimal, Hex, Binary)
 *
 * @return None
 */
void print_register(uint8_t address, uint32_t data, uint8_t format)
{
    switch (format)
    {
    case DECIMAL:
    {
        // Char arrays to hold number
        char data_str[MAX_INT_STRING];
        char addr_str[MAX_INT_STRING];
        char concat[MAX_STRING_CONCAT];

        // Convert ints to strings
        int_to_str((int32_t)data, data_str);
        int_to_str((int32_t)address, addr_str);

        // Concatenate the strings together
        str_concat("Register ", addr_str, concat);
        str_concat(concat, " = ", concat);
        str_concat(concat, data_str, concat);
        str_concat(concat, "\n", concat);

        // Print to terminal
        usart_write_string(concat);
        break;
    }
    case HEX:
    {
        break;
    }
    case BINARY:
    {
        // Char arrays to hold number
        char addr_str[MAX_INT_STRING];
        char concat[MAX_STRING_CONCAT];

        // Concatenate the strings together
        str_concat("Register ", addr_str, concat);
        str_concat(concat, " = ", concat);

        // Print individual bits
        int32_t bit = 0;
        char buffer[2];
        for (int8_t i = 31; i >= 0; i--) {
            bit = (data >> i) & 1 ? '1' : '0';
            int_to_str(bit, buffer);
            str_concat(concat, buffer, concat);
        }
        str_concat(concat, "\n", concat);

        // Print to terminal
        usart_write_string(concat);

        break;
    }
    default:
    {
        usart_write_string("Format to print not known.\n");
        return;
    }
    }

    return;
}