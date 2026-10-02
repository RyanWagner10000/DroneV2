/*
 * file: nvic.h
 * description: header file for the NVIC vector table
 * author: Ryan Wagner
 * date: September 19, 2026
 * notes:
 *   Reference STM file PM0214-rev6.pdf
 */

#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>

#define WWDG_IRQn (0)                // offset: 0x40
#define PVD_IRQn (1)                 // offset: 0x44
#define TAMP_STAMP_IRQn (2)          // offset: 0x48
#define RTC_WKUP_IRQn (3)            // offset: 0x4C
#define FLASH_IRQn (4)               // offset: 0x50
#define RCC_IRQn (5)                 // offset: 0x54
#define EXTI0_IRQn (6)               // offset: 0x58
#define EXTI1_IRQn (7)               // offset: 0x5C
#define EXTI2_IRQn (8)               // offset: 0x60
#define EXTI3_IRQn (9)               // offset: 0x64
#define EXTI4_IRQn (10)              // offset: 0x68
#define DMA1_Stream0_IRQn (11)       // offset: 0x6C
#define DMA1_Stream1_IRQn (12)       // offset: 0x70
#define DMA1_Stream2_IRQn (13)       // offset: 0x74
#define DMA1_Stream3_IRQn (14)       // offset: 0x78
#define DMA1_Stream4_IRQn (15)       // offset: 0x7C
#define DMA1_Stream5_IRQn (16)       // offset: 0x80
#define DMA1_Stream6_IRQn (17)       // offset: 0x84
#define ADC_IRQn (18)                // offset: 0x88
#define EXTI9_5_IRQn (23)            // offset: 0x9C
#define TIM1_BRK_TIM9_IRQn (24)      // offset: 0xA0
#define TIM1_UP_TIM10_IRQn (25)      // offset: 0xA4
#define TIM1_TRG_COM_TIM11_IRQn (26) // offset: 0xA8
#define TIM1_CC_IRQn (27)            // offset: 0xAC
#define TIM2_IRQn (28)               // offset: 0xB0
#define TIM3_IRQn (29)               // offset: 0xB4
#define TIM4_IRQn (30)               // offset: 0xB8
#define I2C1_EV_IRQn (31)            // offset: 0xBC
#define I2C1_ER_IRQn (32)            // offset: 0xC0
#define I2C2_EV_IRQn (33)            // offset: 0xC4
#define I2C2_ER_IRQn (34)            // offset: 0xC8
#define SPI1_IRQn (35)               // offset: 0xCC
#define SPI2_IRQn (36)               // offset: 0xD0
#define USART1_IRQn (37)             // offset: 0xD4
#define USART2_IRQn (38)             // offset: 0xD8
#define EXTI15_10_IRQn (40)          // offset: 0xE0
#define RTC_Alarm_IRQn (41)          // offset: 0xE4
#define OTG_FS_WKUP_IRQn (42)        // offset: 0xE8
#define DMA1_Stream7_IRQn (47)       // offset: 0xFC
#define SDIO_IRQn (49)               // offset: 0x104
#define TIM5_IRQn (50)               // offset: 0x108
#define SPI3_IRQn (51)               // offset: 0x10C
#define DMA2_Stream0_IRQn (56)       // offset: 0x120
#define DMA2_Stream1_IRQn (57)       // offset: 0x124
#define DMA2_Stream2_IRQn (58)       // offset: 0x128
#define DMA2_Stream3_IRQn (59)       // offset: 0x12C
#define DMA2_Stream4_IRQn (60)       // offset: 0x130
#define OTG_FS_IRQn (67)             // offset: 0x14C
#define DMA2_Stream5_IRQn (68)       // offset: 0x150
#define DMA2_Stream6_IRQn (69)       // offset: 0x154
#define DMA2_Stream7_IRQn (70)       // offset: 0x158
#define USART6_IRQn (71)             // offset: 0x15C
#define I2C3_EV_IRQn (72)            // offset: 0x160
#define I2C3_ER_IRQn (73)            // offset: 0x164
#define FPU_IRQn (81)                // offset: 0x184
#define SPI4_IRQn (84)               // offset: 0x190
#define SPI5_IRQn (85)               // offset: 0x194

// Define the pointer to NVIC at its base address
#define NVIC_BASE (0xE000E100)
#define NVIC ((NVIC_TypeDef *)NVIC_BASE)

#define NVIC_EnableIRQ(IRQn) (NVIC->ISER[(IRQn) >> 5] = (1 << ((IRQn) & 0x1F)))
#define NVIC_DisableIRQ(IRQn) (NVIC->ICER[(IRQn) >> 5] = (1 << ((IRQn) & 0x1F)))
#define NVIC_SetPriority(IRQn, priority) (NVIC->IP[(IRQn)] = ((priority) << 4))

typedef struct
{
    volatile uint32_t ISER[8];
    uint32_t RESERVED0[24];
    volatile uint32_t ICER[8];
    uint32_t RESERVED1[24];
    volatile uint32_t ISPR[8];
    uint32_t RESERVED2[24];
    volatile uint32_t ICPR[8];
    uint32_t RESERVED3[24];
    volatile uint32_t IABR[8];
    uint32_t RESERVED4[56];
    volatile uint8_t IP[240];
    uint32_t RESERVED5[644];
    volatile uint32_t STIR;
} NVIC_TypeDef;

#endif // NVIC_H