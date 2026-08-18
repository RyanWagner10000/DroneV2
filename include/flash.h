/*
 * file: flash.h
 * description: header file for flash peripheral structure
 * author: Ryan Wagner
 * date: August 17, 2026
 * notes:
 */

#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

#define FLASH_BASE (0x40023C00)

#define FLASH ((FLASH_TypeDef *)FLASH_BASE)

typedef struct
{
    volatile uint32_t ACR;     // offset: 0x00
    volatile uint32_t KEYR;    // offset: 0x04
    volatile uint32_t OPTKEYR; // offset: 0x08
    volatile uint32_t SR;      // offset: 0x0C
    volatile uint32_t CR;      // offset: 0x10
    volatile uint32_t OPTCR;   // offset: 0x14
} FLASH_TypeDef;

void init_flash_96MHz(void);

#endif // FLASH_H