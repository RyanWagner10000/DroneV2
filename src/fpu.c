/*
 * file: fpu.c
 * description: file that sets up the FPU
 * author: Ryan Wagner
 * date: August 25, 2026
 * notes:
 */

#include "fpu.h"

void init_fpu(void)
{
    FPU->CPACR |= (3U << 20);
    FPU->CPACR |= (3U << 22);

    return;
}