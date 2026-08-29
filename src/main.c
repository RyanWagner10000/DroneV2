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

    initGreenLED();
    initRedLED();

    initTimer2();
    initTimer10();

    // Upon success/fail, play noise
    // Implement logic for pass/fail
    successNoise();
    // failNoise();
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

    while (1)
    {
        delayMillisecond(100U);
        onLED(GREEN_LED);

        delayMillisecond(100U);
        offLED(GREEN_LED);
    }

    return 0;
}