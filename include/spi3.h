/*
 * file: spi3.h
 * description: header file for spi3 communication
 * author: Ryan Wagner
 * date: September 22, 2026
 * notes:
 */

#ifndef SPI3_H
#define SPI3_H

#include <stdint.h>
#include "gpio.h"

#define SPI3_BASE 0x40003C00

#define SPI3 ((SPI3_TypeDef *)SPI3_BASE)

typedef struct
{
    volatile uint32_t CR1;     // offset: 0x00
    volatile uint32_t CR2;     // offset: 0x04
    volatile uint32_t SR;      // offset: 0x08
    volatile uint32_t DR;      // offset: 0x0C
    volatile uint32_t CPCPR;   // offset: 0x10
    volatile uint32_t RXCRCR;  // offset: 0x14
    volatile uint32_t TXCRCR;  // offset: 0x18
    volatile uint32_t I2SCFGR; // offset: 0x1C
    volatile uint32_t I2SPR;   // offset: 0x20
} SPI3_TypeDef;

void init_spi3(void);
void transfer_spi3(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length);
void enable_ce(void);
void enable_csn(void);
void disable_ce(void);
void disable_csn(void);

#endif // SPI3_H