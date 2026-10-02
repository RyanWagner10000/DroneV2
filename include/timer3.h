/*
 * file: Timer3.h
 * description: header file for timer peripheral structure
 * author: Ryan Wagner
 * date: October 1, 2026
 * notes:
 */

#ifndef TIMER3_H
#define TIMER3_H

#include <stdint.h>
#include "gpio.h"
#include "nvic.h"

#define TIM3_BASE (0x40000400)

#define TIM3 ((timer3_TypeDef *)TIM3_BASE)

typedef struct
{
    volatile uint32_t CR1;       // offset: 0x00
    volatile uint32_t CR2;       // offset: 0x04
    volatile uint32_t SMCR;      // offset: 0x08
    volatile uint32_t DIER;      // offset: 0x0C
    volatile uint32_t SR;        // offset: 0x10
    volatile uint32_t EGR;       // offset: 0x14
    volatile uint32_t CCMR1;     // offset: 0x18
    volatile uint32_t CCMR2;     // offset: 0x1C
    volatile uint32_t CCER;      // offset: 0x20
    volatile uint32_t CNT;       // offset: 0x24
    volatile uint32_t PSC;       // offset: 0x28
    volatile uint32_t ARR;       // offset: 0x2C
    volatile uint32_t RESERVED1; // offset: 0x30
    volatile uint32_t CCR1;      // offset: 0x34
    volatile uint32_t CCR2;      // offset: 0x38
    volatile uint32_t CCR3;      // offset: 0x3C
    volatile uint32_t CCR4;      // offset: 0x40
    volatile uint32_t RESERVED2; // offset: 0x44
    volatile uint32_t DCR;       // offset: 0x48
    volatile uint32_t DMAR;      // offset: 0x4C
    volatile uint32_t OR;        // offset: 0x50
} timer3_TypeDef;

void restart_one_sec_timer(void);
void reset_one_sec_flag(void);
void set_one_sec_flag(void);
uint8_t get_one_sec_flag(void);
void init_timer3(void);

#endif // TIMER3_H