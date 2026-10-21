/*
 * file: bme680.h
 * description: header file for altimeter sensor BME680
 * author: Ryan Wagner
 * date: October 20, 2025
 * notes:
 */

#ifndef BME680_H
#define BME680_H

#include <stdint.h>
#include "i2c1.h"

void init_bme680(void);

#endif // BME680_H