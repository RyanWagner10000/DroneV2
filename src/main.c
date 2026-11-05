/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: August 8, 2026
 * notes:
 */

#include "main.h"
#include <math.h> // Required for powf()
#include <errno.h>

int16_t accel_xyz[3] = {0, 0, 0};
int16_t gyro_xyz[3] = {0, 0, 0};
// int16_t mag_xyz[3] = {0, 0, 0};
float pressure = 0.0;
float pressure_ground = 0.0;
float relative_height = 0.0;

/**
 * @brief Function to return error number for math.h library
 *
 * @param None
 *
 * @return Error number
 */
int *__errno(void)
{
    return &errno;
}

/**
 * @brief Function to init all the standard peripherals and report success/fail
 *
 * @param None
 *
 * @return None
 */
static void init_peripherals(void)
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
    init_radio_exti();

    init_usart();
    usart_write_string("\n\n--------------------------------------------------\n");
    usart_write_string("USART2 Working!\n");

    init_timer2();
    init_timer3();
    init_timer10();
    usart_write_string("Timers Setup!\n");

    init_spi1();
    init_spi2();
    init_spi3();
    usart_write_string("SPI's Setup!\n");

    // init_i2c1();
    // usart_write_string("I2C Setup!\n");

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
static void init_modules(void)
{
    // Init IMU module
    init_lsm9ds1();

    uint8_t pass = init_radio(0);
    if (pass)
    {
        usart_write_string("Radio Setup!\n");
        print_radio_settings();
    }
    else
    {
        usart_write_string("!!!!! !!!!! !!!!!  ");
        usart_write_string("Radio NOT Setup");
        usart_write_string("  !!!!! !!!!! !!!!!\n");
    }

    pass = init_bmp388();
    if (pass)
    {
        init_bmp_exti();
        reset_bmp_ready();
        usart_write_string("Altimeter Setup!\n");
    }
    else
    {
        usart_write_string("!!!!! !!!!! !!!!!  ");
        usart_write_string("Altimeter NOT Setup");
        usart_write_string("  !!!!! !!!!! !!!!!\n");
    }

    return;
}

static float calculate_relative_height(void)
{
    // Poor linear approx.
    // return (pressure_ground - pressure) * 0.083; // m/Pa

    // Better but slower approx.
    float ratio = pressure / pressure_ground;
    return 44307.7f * (1.0f - powf(ratio, 0.190284f)); // meters
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
    init_peripherals();

    delay_millisecond(100);

    init_modules();

    delay_millisecond(100);

    // Struct to hold TxRx data from ground-station
    RadioPacket packet = {0, 0, 0, 0, 0, 0, 255, 0};
    set_rx_mode();
    read_radio(&packet, P0_PACKET_SIZE);

    // Sensor start up, getting the averages going before takeoff
    uint8_t break_out = 0;
    float pressure_avg_num = 0.0;
    uint16_t pressure_avg_den = 0;

    on_led(BLUE_LED);
    restart_one_sec_timer();

    while (break_out < 5)
    {
        if (get_radio_ready() == 1U)
        {
            reset_radio_ready();
        }

        if (get_imu_XL_flag() == 1U)
        {
            reset_imu_XL_flag();
        }

        if (get_imu_GY_flag() == 1U)
        {
            reset_imu_GY_flag();
        }

        if (get_bmp_ready() == 1U)
        {
            reset_bmp_ready();

            // Contribute to pressure average
            pressure_avg_num += get_pressure_data();
            pressure_avg_den++;
        }

        if (get_one_sec_flag())
        {
            reset_one_sec_flag();

            break_out++;
        }
    }

    off_led(BLUE_LED);

    // Set relative height to 0
    pressure_ground = pressure_avg_num / pressure_avg_den;

    while (1)
    {
        if (get_radio_ready() == 1U)
        {
            // usart_write_string("Radio Data Ready\n");
            reset_radio_ready();

            // Read Rx data, print if available
            read_radio(&packet, P0_PACKET_SIZE);

            // Print packet for confirmation
            // print_packet(packet);
        }

        if (get_imu_XL_flag() == 1U)
        {
            reset_imu_XL_flag();
            get_accel_data(accel_xyz);

            // print_xyz(accel_xyz);
        }

        if (get_imu_GY_flag() == 1U)
        {
            reset_imu_GY_flag();
            get_gyro_data(gyro_xyz);

            // print_xyz(gyro_xyz);
        }

        if (get_bmp_ready() == 1U)
        {
            reset_bmp_ready();

            pressure = get_pressure_data();
            relative_height = calculate_relative_height();
            char f_buf[MAX_FLOAT_STRING];
            char s_buf[MAX_STRING_CONCAT];
            float_to_str(relative_height, f_buf, 3);
            str_concat("Height: ", f_buf, s_buf);
            usart_write_string(s_buf);
            usart_write_char('\n');
        }

        if (get_one_sec_flag())
        {
            reset_one_sec_flag();

            toggle_led(GREEN_LED);
        }
    }

    return 0;
}