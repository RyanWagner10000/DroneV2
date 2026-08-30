/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: August 8, 2026
 * notes:
 */

#include "main.h"

/**
 * @brief Function to init all the standard peripherals and report success/fail
 *
 * @param None
 *
 * @return None
 */
void initPeripherals(void)
{
    init_rcc();
    init_fpu();

    init_green_led();
    init_red_led();
    init_blue_led();
    off_led(GREEN_LED);
    off_led(RED_LED);
    off_led(BLUE_LED);

    init_timer2();
    init_timer10();

    init_spi1();

    // Upon success/fail, play noise
    // Implement logic for pass/fail
    success_noise();
    // fail_noise();
}

/**
 * @brief Function to init all the modules/sensor boards and report success/fail
 *
 * @param None
 *
 * @return None
 */
void initModules(void)
{
    init_lsm9ds1();

    return;
}

/**
 * @brief Main forever while-loop
 *
 * @param None
 *
 * @return None
 */
int main(void)
{
    initPeripherals();
    
    delay_millisecond(100);

    initModules();

    delay_millisecond(100);

    while (1)
    {
        delay_millisecond(100U);
        on_led(GREEN_LED);

        delay_millisecond(100U);
        off_led(GREEN_LED);
    }

    return 0;
}