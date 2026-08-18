/*
 * file: flash.c
 * description: file that contains the helper functions for Flash configuration
 * author: Ryan Wagner
 * date: August 17, 2026
 * notes:
 */

#include "flash.h"

/**
 * @brief Initialize FLASH peripheral for 96MHz
 *
 * @param None
 *
 * @return None
 */
void init_flash_96MHz(void)
{
    // Set latency to 3 wait states
    FLASH->ACR |= (3U << 0);

    // Set Data cache
    FLASH->ACR |= (1U << 10);

    // Set Instruction cache
    FLASH->ACR |= (1U << 9);

    // Set Prefetch
    FLASH->ACR |= (1U << 8);

    return;
}