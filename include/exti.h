/*
 * file: exti.h
 * description: header file for the external interrupt
 * author: Ryan Wagner
 * date: September 20, 2026
 * notes:
 */

#ifndef EXTI_H
#define EXTI_H

#include <stdint.h>
#include "rcc.h"
#include "gpio.h"
#include "syscfg.h"
#include "nvic.h"

#define EXTI_BASE 0x40013C00
#define EXTI ((EXTI_TypeDef *)EXTI_BASE)

typedef struct
{
    volatile uint32_t IMR;   // offset: 0x00
    volatile uint32_t EMR;   // offset: 0x04
    volatile uint32_t RTSR;  // offset: 0x08
    volatile uint32_t FTSR;  // offset: 0x0C
    volatile uint32_t SWIER; // offset: 0x10
    volatile uint32_t PR;    // offset: 0x14
} EXTI_TypeDef;

void init_imu_exti();

#endif // EXTI_H