/*
 * file: lsm9ds1.c
 * description: file that sets up the accelerometer, gyro, and magnetometer
 * author: Ryan Wagner
 * date: August 29, 2026
 * notes:
 */

#include "lsm9ds1.h"

// static uint8_t MAX_BUFFER_SIZE = 8;

/**
 * @brief Internal function to write byte to specific register on the IMU module
 *
 * @param sensor Sensor enum type for which IMU sensor
 * @param address Register address on IMU module
 * @param value Value to set register on the module
 *
 * @return None
 */
static void write_register_single(Sensor sensor, uint8_t address, uint8_t value)
{
    uint8_t tx_buffer[2] = {address, value};
    uint8_t rx_buffer[2] = {0, 0};

    if (sensor == AG)
    {
        // Set CS pin LOW
        enable_csag_spi1();

        // Send write operation
        transfer_spi1(tx_buffer, rx_buffer, 2);

        // Set CE pin HIGH
        disable_csag_spi1();
    }
    else if (sensor == M)
    {
        // Set CS pin LOW
        enable_csm_spi1();

        // Send write operation
        transfer_spi1(tx_buffer, rx_buffer, 2);

        // Set CE pin HIGH
        disable_csm_spi1();
    }

    return;
}

/**
 * @brief Internal function to read byte of specific register on the IMU module
 *
 * @param sensor Sensor enum type for which IMU sensor
 * @param address Register address on IMU module
 *
 * @return Single uint8_t byte of data
 */
static uint8_t read_register_single(Sensor sensor, uint8_t address)
{
    // Format message
    uint8_t tx_buffer[2] = {address | READ_OPERATION, 0x00};
    uint8_t rx_buffer[2] = {0, 0};

    if (sensor == AG)
    {
        // Set CS pin LOW
        enable_csag_spi1();

        // Send read operation
        transfer_spi1(tx_buffer, rx_buffer, 2);

        // Set CE pin HIGH
        disable_csag_spi1();
    }
    else if (sensor == M)
    {
        // Set CS pin LOW
        enable_csm_spi1();

        // Send read operation
        transfer_spi1(tx_buffer, rx_buffer, 2);

        // Set CE pin HIGH
        disable_csm_spi1();
    }

    return rx_buffer[1];
}

/**
 * @brief Internal function to read N-many bytes from specific register on the IMU module
 *
 * @param sensor Sensor enum type for which IMU sensor
 * @param address Register address on IMU module
 * @param values Values array to get from registers on the module
 * @param length Length of value array
 *
 * @return None
 */
// static void read_register_multi(Sensor sensor, uint8_t address, uint8_t *values, uint8_t length)
// {
//     uint8_t tx_buffer[MAX_BUFFER_SIZE];
//     uint8_t rx_buffer[MAX_BUFFER_SIZE];

//     length = length <= MAX_BUFFER_SIZE ? length : MAX_BUFFER_SIZE;

//     // Copy data into Tx buffer
//     tx_buffer[0] = address | READ_OPERATION;
//     if (sensor == M)
//     {
//         tx_buffer[0] = address | READ_OPERATION | (1U << 6);
//     }
//     for (uint8_t i = 1; i < length + 1; i++)
//     {
//         tx_buffer[i] = 0x00;
//     }

//     if (sensor == AG)
//     {
//         // Set CS pin LOW
//         enable_csag_spi1();

//         // Send write operation
//         transfer_spi1(tx_buffer, rx_buffer, length + 1);

//         // Set CE pin HIGH
//         disable_csag_spi1();
//     }
//     else if (sensor == M)
//     {
//         // Set CS pin LOW
//         disable_csag_spi1();

//         // Send write operation
//         transfer_spi1(tx_buffer, rx_buffer, length + 1);

//         // Set CE pin HIGH
//         disable_csm_spi1();
//     }

//     // Transfer data into values array
//     for (int8_t i = 0; i < length; i++)
//     {
//         values[i] = rx_buffer[i + 1];
//     }

//     return;
// }

/**
 * @brief Returns the ID of the Accelerometer and Gyroscope
 *
 * @param None
 *
 * @return None
 */
uint8_t get_who_am_i_ag(void)
{
    // Read WHO_AM_I (should be 0x68)
    return read_register_single(AG, WHO_AM_I);
}

/**
 * @brief Returns the ID of the Magnetometer
 *
 * @param None
 *
 * @return None
 */
uint8_t get_who_am_i_m(void)
{
    // Read WHO_AM_I (should be 0x3D)
    return read_register_single(M, WHO_AM_I_M);
}

void init_lsm9ds1(void)
{
    uint8_t value = 0;

    // Gyroscope
    value |= (6U << 5); // ODR_G
    value |= (1U << 3); // FS_G
    value |= (3U << 0); // BW_G
    write_register_single(AG, CTRL_REG1_G, value);
    value = 0;

    // Accelerometer
    value |= (6U << 5); // ODR_XL
    value |= (2U << 3); // FS_XL
    write_register_single(AG, CTRL_REG6_XL, value);
    value = 0;

    // Magnetometer
    value |= (2U << 5); // OM [1:0]
    value |= (4U << 2); // DO [1:0]
    write_register_single(M, CTRL_REG1_M, value);
    value = 0;

    value |= (2U << 2); // OMZ [1:0]
    write_register_single(M, CTRL_REG4_M, value);
    value = 0;

    value |= (1U << 7); // I2C_DISABLE
    value &= ~(3U << 0); // MD[1:0]
    write_register_single(M, CTRL_REG3_M, value);
    value = 0;

    uint8_t whoami_ag = get_who_am_i_ag();
    if (whoami_ag == 0x68)
    {
        ; // Do something to confirm it's reading
    }

    uint8_t whoami_m = get_who_am_i_m();
    if (whoami_m == 0x3D)
    {
        ; // Do something to confirm it's reading
    }

    return;
}