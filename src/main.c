/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: August 8, 2026
 * notes:
 */

#include "main.h"

/**
 * @brief Main forever while-loop
 *
 * @param None
 *
 * @return None
 */
int main(void)
{
    // initBlueLED();
    initGreenLED();

    volatile uint32_t counter;

    while (1)
    {
        for (counter = 0; counter < 1000000; counter++)
        {
            ;
        }
        onLED(GREEN_LED);
        
        for (counter = 0; counter < 1000000; counter++)
        {
            ;
        }
        offLED(GREEN_LED);
    }

    return 0;
}