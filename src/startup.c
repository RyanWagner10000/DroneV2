#include <stdint.h>
#include "gpio.h"

extern uint32_t _estack;
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void Default_Handler(void);
void Reset_Handler(void);
void HardFault_Handler(void);
void UsageFault_Handler(void);
void BusFault_Handler(void);
int main();

void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));

uint32_t vector_tbl[] __attribute__((section(".isr_vector_tbl"))) = {
    (uint32_t)&_estack,            // - : Initial Stack Pointer
    (uint32_t)&Reset_Handler,      // - : Reset
    (uint32_t)&NMI_Handler,        // - : NMI
    (uint32_t)&HardFault_Handler,  // - : HardFault
    (uint32_t)&MemManage_Handler,  // - : MemManage
    (uint32_t)&BusFault_Handler,   // - : BusFault
    (uint32_t)&UsageFault_Handler, // - : UsageFault
    0,                             // - : Reserved
    0,                             // - : Reserved
    0,                             // - : Reserved
    0,                             // - : Reserved
    (uint32_t)&Default_Handler,    // - : SVCall
    (uint32_t)&Default_Handler,    // - : Debug Monitor
    0,                             // - : Reserved
    (uint32_t)&Default_Handler,    // - : PendSV
    (uint32_t)&Default_Handler,    // - : SysTick
    (uint32_t)&HardFault_Handler,  // 0 : Window Watchdog interrupt
    (uint32_t)&HardFault_Handler,  // 1 : PVD through EXTI line detectioninterrupt
    (uint32_t)&HardFault_Handler,  // 2 : Tamper and TimeStamp interrupts through the EXTI line
    (uint32_t)&HardFault_Handler,  // 3 : RTC Wake-up interrupt through the EXTI line
    (uint32_t)&HardFault_Handler,  // 4 : Flash global interrupt
    (uint32_t)&HardFault_Handler,  // 5 : RCC global interrupt
    (uint32_t)&HardFault_Handler,  // 6 : EXTI Line0 interrupt
    (uint32_t)&HardFault_Handler,  // 7 : EXTI Line1 interrupt
    (uint32_t)&HardFault_Handler,  // 8 : EXTI Line2 interrupt
    (uint32_t)&HardFault_Handler,  // 9 : EXTI Line3 interrupt
    (uint32_t)&HardFault_Handler,  // 10: EXTI Line4 interrupt
    (uint32_t)&HardFault_Handler,  // 11: DMA1 Stream0 global interrupt
    (uint32_t)&HardFault_Handler,  // 12: DMA1 Stream1 global interrupt
    (uint32_t)&HardFault_Handler,  // 13: DMA1 Stream2 global interrupt
    (uint32_t)&HardFault_Handler,  // 14: DMA1 Stream3 global interrupt
    (uint32_t)&HardFault_Handler,  // 15: DMA1 Stream4 global interrupt
    (uint32_t)&HardFault_Handler,  // 16: DMA1 Stream5 global interrupt
    (uint32_t)&HardFault_Handler,  // 17: DMA1 Stream6 global interrupt
    (uint32_t)&HardFault_Handler,  // 18: ADC1 global interrupts
    (uint32_t)&HardFault_Handler,  // 23: EXTI Line[9:5] interrupts
    (uint32_t)&HardFault_Handler,  // 24: TIM1 Break interrupt and TIM9 global interrupt
    (uint32_t)&HardFault_Handler,  // 25: TIM1 Update interrupt and TIM10 global interrupt
    (uint32_t)&HardFault_Handler,  // 26: TIM1 Trigger and Commutation interrupts and TIM11 global interrupt
    (uint32_t)&HardFault_Handler,  // 27: TIM1 Capture Compare interrupt
    (uint32_t)&HardFault_Handler,  // 28: TIM2 global interrupt
    (uint32_t)&HardFault_Handler,  // 29: TIM3 global interrupt
    (uint32_t)&HardFault_Handler,  // 30: TIM4 global interrupt
    (uint32_t)&HardFault_Handler,  // 31: I2C1 event interrupt
    (uint32_t)&HardFault_Handler,  // 32: I2C1 error interrupt
    (uint32_t)&HardFault_Handler,  // 33: I2C2 event interrupt
    (uint32_t)&HardFault_Handler,  // 34: I2C2 error interrupt
    (uint32_t)&HardFault_Handler,  // 35: SPI1 global interrupt
    (uint32_t)&HardFault_Handler,  // 36: SPI2 global interrupt
    (uint32_t)&HardFault_Handler,  // 37: USART1 global interrupt
    (uint32_t)&HardFault_Handler,  // 38: USART2 global interrupt
    (uint32_t)&HardFault_Handler,  // 40: EXTI Line[15:10] interrupts
    (uint32_t)&HardFault_Handler,  // 41: RTC Alarms (A and B) through EXTI line interrupt
    (uint32_t)&HardFault_Handler,  // 42: USB On-The-Go FS Wake-up through EXTI line interrupt
    (uint32_t)&HardFault_Handler,  // 47: DMA1 Stream7 global interrupt
    (uint32_t)&HardFault_Handler,  // 49: SDIO global interrupt
    (uint32_t)&HardFault_Handler,  // 50: TIM5 global interrupt
    (uint32_t)&HardFault_Handler,  // 51: SPI3 global interrupt
    (uint32_t)&HardFault_Handler,  // 56: DMA2 Stream0 global interrupt
    (uint32_t)&HardFault_Handler,  // 57: DMA2 Stream1 global interrupt
    (uint32_t)&HardFault_Handler,  // 58: DMA2 Stream2 global interrupt
    (uint32_t)&HardFault_Handler,  // 59: DMA2 Stream3 global interrupt
    (uint32_t)&HardFault_Handler,  // 60: DMA2 Stream4 global interrupt
    (uint32_t)&HardFault_Handler,  // 67: USB On The Go FS global interrupt
    (uint32_t)&HardFault_Handler,  // 68: DMA2 Stream5 global interrupt
    (uint32_t)&HardFault_Handler,  // 69: DMA2 Stream6 global interrupt
    (uint32_t)&HardFault_Handler,  // 70: DMA2 Stream7 global interrupt
    (uint32_t)&HardFault_Handler,  // 71: USART6 global interrupt
    (uint32_t)&HardFault_Handler,  // 72: I2C3 event interrupt
    (uint32_t)&HardFault_Handler,  // 73: I2C3 error interrupt
    (uint32_t)&HardFault_Handler,  // 81: FPU global interrupt
    (uint32_t)&HardFault_Handler,  // 84: SPI4 global interrupt
    (uint32_t)&HardFault_Handler,  // 85: SPI5 global interrupt
};

void Default_Handler(void)
{
    while (1)
    {
    }
}

void HardFault_Handler(void)
{
    initBlueLED();
    while (1)
    {
        toggleLED(BLUE_LED);
        for (uint32_t i = 0; i < 500000; i++)
            ;
    }
}

void BusFault_Handler(void)
{
    initBlueLED();
    while (1)
    {
        toggleLED(BLUE_LED);
        for (uint32_t i = 0; i < 500000; i++)
            ;
    }
}

void UsageFault_Handler(void)
{
    initBlueLED();
    while (1)
    {
        toggleLED(BLUE_LED);
        for (uint32_t i = 0; i < 500000; i++)
            ;
    }
}

void Reset_Handler(void)
{
    // Calculate the sizes of the .data and .bss sections
    uint32_t data_mem_size = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint32_t bss_mem_size = (uint32_t)&_ebss - (uint32_t)&_sbss;

    /* Convert byte sizes to 32-bit word counts to match uint32_t* copies (4 == sizeof(uint32_t)) */
    data_mem_size /= 4;
    bss_mem_size /= 4;

    // Initialize pointers to the source and destination of the .data section
    uint32_t *p_src_mem = (uint32_t *)(&_etext);
    uint32_t *p_dest_mem = (uint32_t *)(&_sdata);

    // Copy .data section from FLASH to SRAM
    for (uint32_t i = 0; i < data_mem_size; i++)
    {
        *p_dest_mem++ = *p_src_mem++;
    }
    // Initialize the .bss section to zero in SRAM
    p_dest_mem = (uint32_t *)(&_sbss);

    for (uint32_t i = 0; i < bss_mem_size; i++)
    {
        // Set bss section to zero
        *p_dest_mem++ = 0;
    }

    // Call the application's main function
    main();

    while(1);
}