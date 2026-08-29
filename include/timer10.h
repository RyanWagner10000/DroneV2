/*
 * file: Timer10.h
 * description: header file for timer peripheral structure
 * author: Ryan Wagner
 * date: August 28, 2026
 * notes:
 */

#ifndef TIMER10_H
#define TIMER10_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"

#define TIM10_BASE (0x40014400)

#define TIM10 ((timer10_TypeDef *)TIM10_BASE)

typedef struct
{
    volatile uint32_t CR1;       // offset: 0x00
    volatile uint32_t RESERVED0; // offset: 0x04
    volatile uint32_t SMCR;      // offset: 0x08
    volatile uint32_t DIER;      // offset: 0x0C
    volatile uint32_t SR;        // offset: 0x10
    volatile uint32_t EGR;       // offset: 0x14
    volatile uint32_t CCMR1;     // offset: 0x18
    volatile uint32_t RESERVED1; // offset: 0x1C
    volatile uint32_t CCER;      // offset: 0x20
    volatile uint32_t CNT;       // offset: 0x24
    volatile uint32_t PSC;       // offset: 0x28
    volatile uint32_t ARR;       // offset: 0x2C
    volatile uint32_t RESERVED2; // offset: 0x30
    volatile uint32_t CCR1;      // offset: 0x34
    volatile uint32_t CCR2;      // offset: 0x38
    volatile uint32_t RESERVED3; // offset: 0x3C
    volatile uint32_t RESERVED4; // offset: 0x40
    volatile uint32_t RESERVED5; // offset: 0x44
    volatile uint32_t RESERVED6; // offset: 0x48
    volatile uint32_t RESERVED7; // offset: 0x4C
} timer10_TypeDef;

void delayMillisecond(uint32_t num_milliseconds);
void initTimer10(void);

#endif // TIMER10_H