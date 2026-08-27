/*
 * file: FPU.h
 * description: header file for FPU peripheral structure
 * author: Ryan Wagner
 * date: August 25, 2026
 * notes:
 */

#ifndef FPU_H
#define FPU_H

#include <stdint.h>

#define FPU_BASE 0xE000ED88

#define FPU ((FPU_TypeDef *)FPU_BASE)

typedef struct
{
    volatile uint32_t CPACR;
} FPU_TypeDef;

void init_fpu(void);

#endif // FPU_H