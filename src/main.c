/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: August 8, 2026
 * notes:
 */

#include "main.h"

int16_t accel_xyz[3] = {0, 0, 0};
int16_t gyro_xyz[3] = {0, 0, 0};
// int16_t mag_xyz[3] = {0, 0, 0};

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

    init_imu_exti();

    init_usart();
    usart_write_string("USART2 Working!\n");

    init_imu_exti();

    init_timer2();
    init_timer10();

    init_spi1();

    // Upon success/fail, play noise
    // Implement logic for pass/fail
    success_noise();
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
    // Init IMU module
    init_lsm9ds1();

    return;
}

static void print_xyz(int16_t *xyz)
{
    usart_write_string("[");
    usart_write_number((int32_t)xyz[0]);
    usart_write_char(' ');
    usart_write_number((int32_t)xyz[1]);
    usart_write_char(' ');
    usart_write_number((int32_t)xyz[2]);
    usart_write_char(']');
    usart_write_char('\n');

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

    on_led(GREEN_LED);

    while (1)
    {
        if (getImuXLFlag() == 1)
        {
            setImuXLFlag(0);
            get_accel_data(accel_xyz);

            // print_xyz(accel_xyz);
        }

        if (getImuGYFlag() == 1)
        {
            setImuGYFlag(0);
            get_gyro_data(gyro_xyz);

            // print_xyz(gyro_xyz);
        }
    }

    return 0;
}