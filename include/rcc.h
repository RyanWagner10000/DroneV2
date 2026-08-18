/*
 * file: rcc.h
 * description: header file for RCC peripheral structure
 * author: Ryan Wagner
 * date: August 9, 2026
 * notes:
 */

#ifndef RCC_H
#define RCC_H

#include <stdint.h>
#include "flash.h"

#define SYS_CLOCK (96000000U);

#define RCC_BASE (0x40023800)

#define RCC ((RCC_TypeDef *)RCC_BASE)

typedef struct
{
    volatile uint32_t CR;         // offset: 0x00
    volatile uint32_t PLLCFGR;    // offset: 0x04
    volatile uint32_t CFGR;       // offset: 0x08
    volatile uint32_t CIR;        // offset: 0x0C
    volatile uint32_t AHB1RSTR;   // offset: 0x10
    volatile uint32_t AHB2RSTR;   // offset: 0x14
    volatile uint32_t RESERVED0;  // offset: 0x18
    volatile uint32_t RESERVED1;  // offset: 0x1C
    volatile uint32_t APB1RSTR;   // offset: 0x20
    volatile uint32_t APB2RSTR;   // offset: 0x24
    volatile uint32_t RESERVED2;  // offset: 0x28
    volatile uint32_t RESERVED3;  // offset: 0x2C
    volatile uint32_t AHB1ENR;    // offset: 0x30
    volatile uint32_t AHB2ENR;    // offset: 0x34
    volatile uint32_t RESERVED4;  // offset: 0x38
    volatile uint32_t RESERVED5;  // offset: 0x3C
    volatile uint32_t APB1ENR;    // offset: 0x40
    volatile uint32_t APB2ENR;    // offset: 0x44
    volatile uint32_t RESERVED6;  // offset: 0x48
    volatile uint32_t RESERVED7;  // offset: 0x4C
    volatile uint32_t AHB1LPENR;  // offset: 0x50
    volatile uint32_t AHB2LPENR;  // offset: 0x54
    volatile uint32_t RESERVED8;  // offset: 0x58
    volatile uint32_t RESERVED9;  // offset: 0x5C
    volatile uint32_t APB1LPENR;  // offset: 0x60
    volatile uint32_t APB2LPENR;  // offset: 0x64
    volatile uint32_t RESERVED10; // offset: 0x68
    volatile uint32_t RESERVED11; // offset: 0x6C
    volatile uint32_t BDCR;       // offset: 0x70
    volatile uint32_t CSR;        // offset: 0x74
    volatile uint32_t RESERVED12; // offset: 0x78
    volatile uint32_t RESERVED13; // offset: 0x7C
    volatile uint32_t SSCGR;      // offset: 0x80
    volatile uint32_t PLLI2SCFGR; // offset: 0x84
    volatile uint32_t DCKCFGR;    // offset: 0x8C
} RCC_TypeDef;

void init_rcc(void);

#endif // RCC_H