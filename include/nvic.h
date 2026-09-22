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

#define WWDG_IRQn (0)
#define PVD_IRQn (1)
#define TAMP_STAMP_IRQn (2)
#define RTC_WKUP_IRQn (3)
#define FLASH_IRQn (4)
#define RCC_IRQn (5)
#define EXTI0_IRQn (6)
#define EXTI1_IRQn (7)
#define EXTI2_IRQn (8)
#define EXTI3_IRQn (9)
#define EXTI4_IRQn (10)
#define DMA1_Stream0_IRQn (11)
#define DMA1_Stream1_IRQn (12)
#define DMA1_Stream2_IRQn (13)
#define DMA1_Stream3_IRQn (14)
#define DMA1_Stream4_IRQn (15)
#define DMA1_Stream5_IRQn (16)
#define DMA1_Stream6_IRQn (17)
#define ADC_IRQn (18)
#define CAN1_TX_IRQn (19)
#define CAN1_RX0_IRQn (20)
#define CAN1_RX1_IRQn (21)
#define CAN1_SCE_IRQn (22)
#define EXTI9_5_IRQn (23)
#define TIM1_BRK_TIM9_IRQn (24)
#define TIM1_UP_TIM10_IRQn (25)
#define TIM1_TRG_COM_TIM11_IRQn (26)
#define TIM1_CC_IRQn (27)
#define TIM2_IRQn (28)
#define TIM3_IRQn (29)
#define TIM4_IRQn (30)
#define I2C1_EV_IRQn (31)
#define I2C1_ER_IRQn (32)
#define I2C2_EV_IRQn (33)
#define I2C2_ER_IRQn (34)
#define SPI1_IRQn (35)
#define SPI2_IRQn (36)
#define USART1_IRQn (37)
#define USART2_IRQn (38)
#define USART3_IRQn (39)
#define EXTI15_10_IRQn (40)
#define RTC_Alarm_IRQn (41)
#define OTG_FS_WKUP_IRQn (42)
#define TIM8_BRK_TIM12_IRQn (43)
#define TIM8_UP_TIM13_IRQn (44)
#define TIM8_TRG_COM_TIM14_IRQn (45)
#define TIM8_CC_IRQn (46)
#define DMA1_Stream7_IRQn (47)
#define FSMC_IRQn (48)
#define SDIO_IRQn (49)
#define TIM5_IRQn (50)
#define SPI3_IRQn (51)
#define UART4_IRQn (52)
#define UART5_IRQn (53)
#define TIM6_DAC_IRQn (54)
#define TIM7_IRQn (55)
#define DMA2_Stream0_IRQn (56)
#define DMA2_Stream1_IRQn (57)
#define DMA2_Stream2_IRQn (58)
#define DMA2_Stream3_IRQn (59)
#define DMA2_Stream4_IRQn (60)
#define ETH_IRQn (61)
#define ETH_WKUP_IRQn (62)
#define CAN2_TX_IRQn (63)
#define CAN2_RX0_IRQn (64)
#define CAN2_RX1_IRQn (65)
#define CAN2_SCE_IRQn (66)
#define OTG_FS_IRQn (67)
#define DMA2_Stream5_IRQn (68)
#define DMA2_Stream6_IRQn (69)
#define DMA2_Stream7_IRQn (70)
#define USART6_IRQn (71)
#define I2C3_EV_IRQn (72)
#define I2C3_ER_IRQn (73)
#define OTG_HS_EP1_OUT_IRQn (74)
#define OTG_HS_EP1_IN_IRQn (75)
#define OTG_HS_WKUP_IRQn (76)
#define OTG_HS_IRQn (77)
#define DCMI_IRQn (78)
#define CRYP_IRQn (79)
#define HASH_RNG_IRQn (80)
#define FPU_IRQn (81)

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