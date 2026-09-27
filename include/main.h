/*
 * file: main.h
 * description: header file for main
 * author: Ryan Wagner
 * date: November 10, 2025
 * notes:
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>
#include "fpu.h"
#include "rcc.h"
#include "gpio.h"
#include "timer2.h"
#include "timer10.h"
#include "lsm9ds1.h"
#include "usart.h"
#include "exti.h"
#include "nrf24.h"

enum CONTROLLER_BUTTON
{
    BUTTON_A = 0,
    BUTTON_B = 1,
    BUTTON_X = 2,
    BUTTON_Y = 3,
    BUTTON_LB = 4,
    BUTTON_RB = 5,
    BUTTON_L3 = 9,
    BUTTON_R3 = 10
};

#endif // MAIN_H