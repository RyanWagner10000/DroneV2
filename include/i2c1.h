/*
 * file: i2c1.h
 * description: header file for i2c1 interface
 * author: Ryan Wagner
 * date: October 20, 2026
 * notes:
 */

#ifndef I2C1_H
#define I2C1_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"

#define I2C1_BASE 0x40005400

#define I2C1 ((I2C1_TypeDef *)I2C1_BASE)

typedef struct
{
    volatile uint32_t CR1;   // offset: 0x00
    volatile uint32_t CR2;   // offset: 0x04
    volatile uint32_t OAR1;  // offset: 0x08
    volatile uint32_t OAR2;  // offset: 0x0C
    volatile uint32_t DR;    // offset: 0x10
    volatile uint32_t SR1;   // offset: 0x14
    volatile uint32_t SR2;   // offset: 0x18
    volatile uint32_t CCR;   // offset: 0x1C
    volatile uint32_t TRISE; // offset: 0x20
    volatile uint32_t FLTR;  // offset: 0x24
} I2C1_TypeDef;

void init_i2c1(void);

#endif // I2C1_H