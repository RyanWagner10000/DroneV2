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
#include "spi2.h"

#define CMD_BMP388 (0x7E)
#define CONFIG_BMP388 (0x1F)
#define ODR_BMP388 (0x1D)
#define OSR_BMP388 (0x1C)
#define PWR_CTRL_BMP388 (0x1B)
#define IF_CONF_BMP388 (0x1A)
#define INT_CTRL_BMP388 (0x19)
#define FIFO_CONFIG_2_BMP388 (0x18)
#define FIFO_CONFIG_1_BMP388 (0x17)
#define FIFO_WTM_1_BMP388 (0x16)
#define FIFO_WTM_0_BMP388 (0x15)
#define FIFO_DATA_BMP388 (0x14)
#define FIFO_LENGTH_1_BMP388 (0x13)
#define FIFO_LENGTH_0_BMP388 (0x12)
#define INT_STATUS_BMP388 (0x11)
#define EVENT_BMP388 (0x10)
#define SENSORTIME_2_BMP388 (0x0E)
#define SENSORTIME_1_BMP388 (0x0D)
#define SENSORTIME_0_BMP388 (0x0C)
#define DATA_5_BMP388 (0x09)
#define DATA_4_BMP388 (0x08)
#define DATA_3_BMP388 (0x07)
#define DATA_2_BMP388 (0x06)
#define DATA_1_BMP388 (0x05)
#define DATA_0_BMP388 (0x04)
#define STATUS_BMP388 (0x03)
#define ERR_ERG_BMP388 (0x02)
#define CHIP_ID_BMP388 (0x00)

#define READ_OPERATION_BMP388 (1U << 7)

#pragma pack(push, 1)
typedef struct
{
    uint16_t NVM_PAR_T1; // offset: 0x31-0x32
    uint16_t NVM_PAR_T2; // offset: 0x33-0x34
    int8_t NVM_PAR_T3;   // offset: 0x35
    int16_t NVM_PAR_P1;  // offset: 0x36-0x37
    int16_t NVM_PAR_P2;  // offset: 0x38-0x39
    int8_t NVM_PAR_P3;   // offset: 0x3A
    int8_t NVM_PAR_P4;   // offset: 0x3B
    uint16_t NVM_PAR_P5; // offset: 0x3C-0x3D
    uint16_t NVM_PAR_P6; // offset: 0x3E-0x3F
    int8_t NVM_PAR_P7;   // offset: 0x40
    int8_t NVM_PAR_P8;   // offset: 0x41
    int16_t NVM_PAR_P9;  // offset: 0x42-0x43
    int8_t NVM_PAR_P10;  // offset: 0x44
    int8_t NVM_PAR_P11;  // offset: 0x45
} RawCalibrationData_TypeDef;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
    float PAR_T1;
    float PAR_T2;
    float PAR_T3;
    float PAR_P1;
    float PAR_P2;
    float PAR_P3;
    float PAR_P4;
    float PAR_P5;
    float PAR_P6;
    float PAR_P7;
    float PAR_P8;
    float PAR_P9;
    float PAR_P10;
    float PAR_P11;
    float TEMP;
} CalibrationData_TypeDef;
#pragma pack(pop)

uint32_t get_pressure_data(void);
float get_temperature_data(void);
uint8_t init_bmp388(void);
void set_bmp_ready(void);
uint8_t get_bmp_ready(void);
void reset_bmp_ready(void);

#endif // BME680_H