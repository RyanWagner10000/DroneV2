/*
 * file: lsm9ds1.h
 * description: headefile foLSM9DS1 constants
 * author: Ryan Wagner
 * date: August 29, 2026
 * notes:
 */

#ifndef LSM9DS1_H
#define LSM9DS1_H

#include <stdint.h>
#include "spi1.h"
#include "timer2.h"

#define READ_OPERATION_LSM9DS1 (1U << 7)

#define ACT_THS_LSM9DS1 (0x04)
#define ACT_DUR_LSM9DS1 (0x05)
#define INT_GEN_CFG_XL_LSM9DS1 (0x06)
#define INT_GEN_THS_X_XL_LSM9DS1 (0x07)
#define INT_GEN_THS_Y_XL_LSM9DS1 (0x08)
#define INT_GEN_THS_Z_XL_LSM9DS1 (0x09)
#define INT_GEN_DUR_XL_LSM9DS1 (0x0A)
#define REFERENCE_G_LSM9DS1 (0x0B)
#define INT1_CTRL_LSM9DS1 (0x0C)
#define INT2_CTRL_LSM9DS1 (0x0D)
#define WHO_AM_I_LSM9DS1 (0x0F)
#define CTRL_REG1_G_LSM9DS1 (0x10)
#define CTRL_REG2_G_LSM9DS1 (0x11)
#define CTRL_REG3_G_LSM9DS1 (0x12)
#define ORIENT_CFG_G_LSM9DS1 (0x13)
#define INT_GEN_SRC_G_LSM9DS1 (0x14)
#define OUT_TEMP_L_LSM9DS1 (0x15)
#define OUT_TEMP_H_LSM9DS1 (0x16)
#define STATUS_REG1_LSM9DS1 (0x17)
#define OUT_X_L_G_LSM9DS1 (0x18)
#define OUT_X_H_G_LSM9DS1 (0x19)
#define OUT_Y_L_G_LSM9DS1 (0x1A)
#define OUT_Y_H_G_LSM9DS1 (0x1B)
#define OUT_Z_L_G_LSM9DS1 (0x1C)
#define OUT_Z_H_G_LSM9DS1 (0x1D)
#define CTRL_REG4_LSM9DS1 (0x1E)
#define CTRL_REG5_XL_LSM9DS1 (0x1F)
#define CTRL_REG6_XL_LSM9DS1 (0x20)
#define CTRL_REG7_XL_LSM9DS1 (0x21)
#define CTRL_REG8_LSM9DS1 (0x22)
#define CTRL_REG9_LSM9DS1 (0x23)
#define CTRL_REG10_LSM9DS1 (0x24)
#define INT_GEN_SRC_XL_LSM9DS1 (0x26)
#define STATUS_REG2_LSM9DS1 (0x27)
#define OUT_X_L_XL_LSM9DS1 (0x28)
#define OUT_X_H_XL_LSM9DS1 (0x29)
#define OUT_Y_L_XL_LSM9DS1 (0x2A)
#define OUT_Y_H_XL_LSM9DS1 (0x2B)
#define OUT_Z_L_XL_LSM9DS1 (0x2C)
#define OUT_Z_H_XL_LSM9DS1 (0x2D)
#define FIFO_CTRL_LSM9DS1 (0x2E)
#define FIFO_SRC_LSM9DS1 (0x2F)
#define INT_GEN_CFG_G_LSM9DS1 (0x30)
#define INT_GEN_THS_XH_G_LSM9DS1 (0x31)
#define INT_GEN_THS_XL_G_LSM9DS1 (0x32)
#define INT_GEN_THS_YH_G_LSM9DS1 (0x33)
#define INT_GEN_THS_YL_G_LSM9DS1 (0x34)
#define INT_GEN_THS_ZH_G_LSM9DS1 (0x35)
#define INT_GEN_THS_ZL_G_LSM9DS1 (0x36)
#define INT_GEN_DUR_G_LSM9DS1 (0x37)
#define OFFSET_X_REG_L_M_LSM9DS1 (0x05)
#define OFFSET_X_REG_H_M_LSM9DS1 (0x06)
#define OFFSET_Y_REG_L_M_LSM9DS1 (0x07)
#define OFFSET_Y_REG_H_M_LSM9DS1 (0x08)
#define OFFSET_Z_REG_L_M_LSM9DS1 (0x09)
#define OFFSET_Z_REG_H_M_LSM9DS1 (0x0A)
#define WHO_AM_I_M_LSM9DS1 (0x0F)
#define CTRL_REG1_M_LSM9DS1 (0x20)
#define CTRL_REG2_M_LSM9DS1 (0x21)
#define CTRL_REG3_M_LSM9DS1 (0x22)
#define CTRL_REG4_M_LSM9DS1 (0x23)
#define CTRL_REG5_M_LSM9DS1 (0x24)
#define STATUS_REG_M_LSM9DS1 (0x27)
#define OUT_X_L_M_LSM9DS1 (0x28)
#define OUT_X_H_M_LSM9DS1 (0x29)
#define OUT_Y_L_M_LSM9DS1 (0x2A)
#define OUT_Y_H_M_LSM9DS1 (0x2B)
#define OUT_Z_L_M_LSM9DS1 (0x2C)
#define OUT_Z_H_M_LSM9DS1 (0x2D)
#define INT_CFG_M_LSM9DS1 (0x30)
#define INT_SRC_M_LSM9DS1 (0x31)
#define INT_THS_L_M_LSM9DS1 (0x32)
#define INT_THS_H_M_LSM9DS1 (0x33)

// Sensor type enum
typedef enum
{
    AG,
    M,
} Sensor;

void init_lsm9ds1(void);
void get_accel_data(int16_t *xyz);
void get_gyro_data(int16_t *xyz);
void get_mag_data(int16_t *xyz);
uint8_t get_imu_XL_flag(void);
void set_imu_XL_flag(void);
void reset_imu_XL_flag(void);
uint8_t get_imu_GY_flag(void);
void set_imu_GY_flag(void);
void reset_imu_GY_flag(void);

#endif // LSM9DS1_H