/*
 * file: spi1.h
 * description: header file for spi1 communication
 * author: Ryan Wagner
 * date: August 29, 2026
 * notes:
 */

#ifndef SPI1_H
#define SPI1_H

#include <stdint.h>
#include "gpio.h"

#define SPI1_BASE 0x40013000

#define SPI1 ((SPI1_TypeDef *)SPI1_BASE)

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
} SPI1_TypeDef;

void init_spi1(void);
void transfer_spi1(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length);
void enable_csag_spi1(void);
void enable_csm_spi1(void);
void disable_csag_spi1(void);
void disable_csm_spi1(void);

#endif // SPI1_H