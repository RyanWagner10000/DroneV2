/*
 * file: usart.h
 * description: header file for USART peripheral structure
 * author: Ryan Wagner
 * date: August 30, 2026
 * notes:
 */

#ifndef USART_H
#define USART_H

#include <stdint.h>
#include "gpio.h"

#define MAX_STRING_CONCAT (512)
#define MAX_FLOAT_STRING (64)
#define MAX_INT_STRING (16)
#define MAX_BIN_STRING (64)
#define MAX_HEX_STRING (8)

enum printFormat {
    DECIMAL,
    HEX,
    BINARY
};

#define USART2_BASE 0x40004400

#define USART2 ((USART_TypeDef *)USART2_BASE)

typedef struct
{
    volatile uint32_t SR;   // offset: 0x00
    volatile uint32_t DR;   // offset: 0x04
    volatile uint32_t BRR;  // offset: 0x08
    volatile uint32_t CR1;  // offset: 0x0C
    volatile uint32_t CR2;  // offset: 0x10
    volatile uint32_t CR3;  // offset: 0x14
    volatile uint32_t GTPR; // offset: 0x18
} USART_TypeDef;

void init_usart(void);
void usart_write_char(uint8_t character);
void usart_write_string(const char *string);
void usart_write_number(int32_t value);

#endif // USART_H