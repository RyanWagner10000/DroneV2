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

#define BARO_CAP (30)

// Global IMU variables
int16_t accel_xyz[3] = {0, 0, 0};
int16_t gyro_xyz[3] = {0, 0, 0};
// int16_t mag_xyz[3] = {0, 0, 0};

// Global baro variables
float pressure = 0.0;
float pressure_ground = 0.0;
float height = 0.0;
RollingAverage relative_height;
float ra_buffer[BARO_CAP] = {0};

// Global SD Card File variables
static FATFS fs;
static FIL logfile;

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
    // Init Micro SD Card reader
    // 1 = mount now: runs disk_initialize, reads boot sector
    FRESULT fr = f_mount(&fs, "", 1);
    if (fr == FR_OK)
    {
        usart_write_string("SD Card Reader Initialized!\n");

        fr = f_open(&logfile, "LOG.BIN", FA_WRITE | FA_OPEN_APPEND);

        if (fr == FR_OK)
        {
            usart_write_string("  SD Card File Init Successful!\n");

            // Write test
            UINT bw;
            uint8_t test_write_buffer[] = "This is a test write to the SD card.\n";
            FRESULT fr = f_write(&logfile, test_write_buffer, sizeof test_write_buffer, &bw);

            if (fr != FR_OK || bw != sizeof test_write_buffer)
            {
                usart_write_string("Failure to write test sentence to SD card file\n");
            }
            else
            {
                usart_write_string("  SD Card File Write Successful!\n");
            }

            f_close(&logfile);
        }
        else
        {
            usart_write_string("  SD Card File Initialization was NOT successful\n");
        }
    }
    else
    {
        usart_write_string("!!!!! !!!!! !!!!!  ");
        usart_write_string("SD Card NOT Setup");
        usart_write_string("  !!!!! !!!!! !!!!!\n");
    }

    // Init IMU module
    init_lsm9ds1();
    init_imu_exti();

    uint8_t pass = init_radio(0);
    if (pass)
    {
        usart_write_string("Radio Setup!\n");
        // print_radio_settings();
        init_radio_exti();
    }
    else
    {
        usart_write_string("!!!!! !!!!! !!!!!  ");
        usart_write_string("Radio NOT Setup");
        usart_write_string("  !!!!! !!!!! !!!!!\n");
    }

    // Init Barometer module
    pass = init_bmp388();
    if (pass)
    {
        // Setup rolling average
        relative_height.buffer = ra_buffer;
        relative_height.capacity = BARO_CAP;
        relative_height.count = 0;
        relative_height.head = 0;
        relative_height.sum = 0;

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

/**
 * @brief Function to calculate and update the relative height rolling average
 *
 * @param None
 *
 * @return None
 */
static float calculate_relative_height(void)
{
    // Poor linear approx.
    // return (pressure_ground - pressure) * 0.083; // m/Pa

    // Better but slower approx.
    float new_value = 44307.7f * (1.0f - powf(pressure / pressure_ground, 0.190284f)); // meters

    // Update checks
    if (relative_height.count < relative_height.capacity)
    {
        // Window isn't full yet
        relative_height.sum += new_value;
        relative_height.buffer[relative_height.head] = new_value;
        relative_height.head = (relative_height.head + 1) % relative_height.capacity;
        relative_height.count++;
    }
    else
    {
        // Reached the limit to the window, subtract old value
        float old_value = relative_height.buffer[relative_height.head];
        relative_height.sum -= old_value;
        relative_height.sum += new_value;
        relative_height.buffer[relative_height.head] = new_value;
        relative_height.head = (relative_height.head + 1) % relative_height.capacity;
    }

    return (float)relative_height.sum / (float)relative_height.count;
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

    on_led(BLUE_LED);
    restart_one_sec_timer();
    float avg_num = 0.0;
    uint16_t avg_den = 0;

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

            // Contribute to base pressure average
            avg_num += get_pressure_data();
            avg_den++;
        }

        if (get_one_sec_flag())
        {
            reset_one_sec_flag();

            break_out++;
        }
    }

    off_led(BLUE_LED);

    // Set base height/pressure
    pressure_ground = avg_num / (float)avg_den;

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
        }

        if (get_imu_GY_flag() == 1U)
        {
            reset_imu_GY_flag();
            get_gyro_data(gyro_xyz);
        }

        if (get_bmp_ready() == 1U)
        {
            reset_bmp_ready();

            pressure = get_pressure_data();
            height = calculate_relative_height();
        }

        if (get_one_sec_flag())
        {
            reset_one_sec_flag();

            toggle_led(GREEN_LED);

            __disable_irq();

            // Perform SD Card Write

            __enable_irq();
        }
    }

    return 0;
}