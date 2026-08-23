/*
 * file: gpio.h
 * description: header file for GPIO peripheral structure
 * author: Ryan Wagner
 * date: November 22, 2025
 * notes:
 */

#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "rcc.h"

#define GPIOA_BASE 0x40020000
#define GPIOB_BASE 0x40020400
#define GPIOC_BASE 0x40020800

#define BLUE_LED (1U << 13)
#define RED_LED (1U << 14)
#define GREEN_LED (1U << 15)

#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)

typedef struct
{
    volatile uint32_t MODER;   // offset: 0x00
    volatile uint32_t OTYPER;  // offset: 0x04
    volatile uint32_t OSPEEDR; // offset: 0x08
    volatile uint32_t PUPDR;   // offset: 0x0C
    volatile uint32_t IDR;     // offset: 0x10
    volatile uint32_t ODR;     // offset: 0x14
    volatile uint32_t BSRR;    // offset: 0x18
    volatile uint32_t LCKR;    // offset: 0x1C
    volatile uint32_t AFRL;    // offset: 0x20
    volatile uint32_t AFRH;    // offset: 0x24
} GPIO_TypeDef;

void initGreenLED(void);
void initRedLED(void);
void initBlueLED(void);
void onLED(uint32_t led);
void offLED(uint32_t led);
void toggleLED(uint32_t led);

#endif // GPIO_H