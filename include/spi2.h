/*
 * file: spi2.h
 * description: header file for spi2 communication
 * author: Ryan Wagner
 * date: October 31, 2026
 * notes:
 */

#ifndef SPI2_H
#define SPI2_H

#include <stdint.h>
#include "gpio.h"

#define SPI2_BASE 0x40003800

#define SPI2 ((SPI2_TypeDef *)SPI2_BASE)

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
} SPI2_TypeDef;

void init_spi2(void);
void transfer_spi2(uint8_t *tx_buffer, uint8_t *rx_buffer, uint8_t length);
void enable_cs_flash(void);
void enable_cs_bmp(void);
void disable_cs_flash(void);
void disable_cs_bmp(void);

#endif // SPI2_H