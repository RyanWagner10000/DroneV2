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
    init_radio_exti();

    init_usart();
    usart_write_string("USART2 Working!\n");

    init_timer2();
    init_timer3();
    init_timer10();
    usart_write_string("Timers Setup!\n");

    init_spi1();
    init_spi3();
    usart_write_string("SPI's Setup!\n");

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
    init_radio(0);

    // print_radio_settings();

    return;
}

// static void print_xyz(int16_t *xyz)
// {
//     usart_write_string("[");
//     usart_write_number((int32_t)xyz[0]);
//     usart_write_char(' ');
//     usart_write_number((int32_t)xyz[1]);
//     usart_write_char(' ');
//     usart_write_number((int32_t)xyz[2]);
//     usart_write_char(']');
//     usart_write_char('\n');

//     return;
// }

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

    // Struct to hold TxRx data from ground-station
    // RadioPacket packet = {0, 0, 0, 0, 0, 0, 255, 0};
    // set_rx_mode();

    restart_one_sec_timer();
    uint32_t counter = 0;

    while (1)
    {
        // if (data_available() == 1)
        // {
        //     // usart_write_string("Data Available!\n");
        // }
        // if (get_radio_ready() == 1)
        // {
        //     usart_write_string("Radio Data Ready\n");
        //     reset_radio_ready();

        //     // // Read Rx data, print if available
        //     // read_radio(&packet, P0_PACKET_SIZE);

        //     // // Print packet for confirmation
        //     // print_packet(packet);
        // }

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
            counter++;

            // print_xyz(gyro_xyz);
        }

        if (get_one_sec_flag() == 1U)
        {
            if (counter > 952)
            {
                usart_write_number((int32_t)counter);
                usart_write_char('\n');
            }
            counter = 0;

            reset_one_sec_flag();

            toggle_led(GREEN_LED);
        }
    }

    return 0;
}