/*
 * file: syscfg.h
 * description: header file for the external interrupt
 * author: Ryan Wagner
 * date: September 20, 2026
 * notes:
 */

#ifndef SYSCFG_H
#define SYSCFG_H

#include <stdint.h>
#include "rcc.h"

#define SYSCFG_BASE 0x40013800
#define SYSCFG ((SYSCFG_TypeDef *)SYSCFG_BASE)

typedef struct
{
    volatile uint32_t MEMRMP;  // offset: 0x00
    volatile uint32_t PMC;     // offset: 0x04
    volatile uint32_t EXTICR1; // offset: 0x08
    volatile uint32_t EXTICR2; // offset: 0x0C
    volatile uint32_t EXTICR3; // offset: 0x10
    volatile uint32_t EXTICR4; // offset: 0x14
    volatile uint32_t CMPCR;   // offset: 0x20
} SYSCFG_TypeDef;

#endif // SYSCFG_H