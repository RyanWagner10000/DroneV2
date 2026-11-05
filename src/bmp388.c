/*
 * file: main.c
 * description: file that contains the main loop for the whole device
 * author: Ryan Wagner
 * date: August 8, 2026
 * notes:
 */

#include "bmp388.h"
#include "usart.h"

#define DEVICE_ID (0x50)

static uint8_t MAX_BUFFER_SIZE = 32;
volatile uint8_t BMP_FLAG = 0;

// Make sure the array is aligned in memory
// So that when it gets cast to the struct it lines up
CalibrationData_TypeDef CALIB_DATA;
RawCalibrationData_TypeDef RAW_CALIB_DATA;

/**
 * @brief Internal function to write byte to specific register
 *
 * @param address Register address
 * @param value Value to set register on the module
 *
 * @return None
 */
static void write_register_single(uint8_t address, uint8_t value)
{
    uint8_t tx_buffer[2] = {address, value};
    uint8_t rx_buffer[2] = {0, 0};

    // Set CS pin LOW
    enable_cs_bmp();

    // Send write operation
    transfer_spi2(tx_buffer, rx_buffer, 2);

    // Set CE pin HIGH
    disable_cs_bmp();

    return;
}

/**
 * @brief Internal function to read byte of specific register on the IMU module
 *
 * @param address Register address on IMU module
 *
 * @return Single uint8_t byte of data
 */
static uint8_t read_register_single(uint8_t address)
{
    // Format message
    uint8_t tx_buffer[3] = {address | READ_OPERATION_BMP388, 0x00, 0x00};
    uint8_t rx_buffer[3] = {0, 0, 0};

    // Set CS pin LOW
    enable_cs_bmp();

    // Send read operation
    transfer_spi2(tx_buffer, rx_buffer, 3);

    // Set CE pin HIGH
    disable_cs_bmp();

    return rx_buffer[2];
}

/**
 * @brief Internal function to read N-many bytes from specific register on the IMU module
 *
 * @param address Register address on IMU module
 * @param values Values array to get from registers on the module
 * @param length Length of value array
 *
 * @return None
 */
static void read_register_multi(uint8_t address, uint8_t *values, uint8_t length)
{
    uint8_t tx_buffer[MAX_BUFFER_SIZE - 2];
    uint8_t rx_buffer[MAX_BUFFER_SIZE - 2];

    length = length <= (MAX_BUFFER_SIZE - 2) ? length : (MAX_BUFFER_SIZE - 2);

    // Copy data into Tx buffer
    tx_buffer[0] = address | READ_OPERATION_BMP388;

    for (uint8_t i = 2; i < length + 2; i++)
    {
        tx_buffer[i] = 0x00;
    }

    // Set CS pin LOW
    enable_cs_bmp();

    // Send write operation
    transfer_spi2(tx_buffer, rx_buffer, length + 2);

    // Set CE pin HIGH
    disable_cs_bmp();

    // Transfer data into values array
    for (int8_t i = 0; i < length; i++)
    {
        values[i] = rx_buffer[i + 2];
    }

    return;
}

/**
 * @brief Calculates the compensated temperature value
 *
 * @param None
 *
 * @return Compensated temperature value
 */
static void BMP388_compensate_temperature(uint32_t uncomp_temp)
{
    float partial_data1;
    float partial_data2;

    partial_data1 = (float)(uncomp_temp - CALIB_DATA.PAR_T1);
    partial_data2 = (float)(partial_data1 * CALIB_DATA.PAR_T2);

    // Set TEMP for use in pressure calculation
    CALIB_DATA.TEMP = partial_data2 + (partial_data1 * partial_data1) * CALIB_DATA.PAR_T3;

    return;
}

/**
 * @brief Calculates the compensated pressure value
 *
 * @param None
 *
 * @return Compensated pressure value
 *
 * @note Taken from the Bosch BMP388 documentation Appendix 9
 */
static float BMP388_compensate_pressure(uint32_t uncomp_press)
{
    // Variable to store the compensated pressure
    float comp_press;

    // Temporary variables used for compensation
    float partial_data1;
    float partial_data2;
    float partial_data3;
    float partial_data4;
    float partial_out1;
    float partial_out2;

    // Calibration data
    partial_data1 = CALIB_DATA.PAR_P6 * CALIB_DATA.TEMP;
    partial_data2 = CALIB_DATA.PAR_P7 * (CALIB_DATA.TEMP * CALIB_DATA.TEMP);
    partial_data3 = CALIB_DATA.PAR_P8 * (CALIB_DATA.TEMP * CALIB_DATA.TEMP * CALIB_DATA.TEMP);
    partial_out1 = CALIB_DATA.PAR_P5 + partial_data1 + partial_data2 + partial_data3;

    partial_data1 = CALIB_DATA.PAR_P2 * CALIB_DATA.TEMP;
    partial_data2 = CALIB_DATA.PAR_P3 * (CALIB_DATA.TEMP * CALIB_DATA.TEMP);
    partial_data3 = CALIB_DATA.PAR_P4 * (CALIB_DATA.TEMP * CALIB_DATA.TEMP * CALIB_DATA.TEMP);
    partial_out2 = (float)uncomp_press * (CALIB_DATA.PAR_P1 + partial_data1 + partial_data2 + partial_data3);

    partial_data1 = (float)uncomp_press * (float)uncomp_press;
    partial_data2 = CALIB_DATA.PAR_P9 + CALIB_DATA.PAR_P10 * CALIB_DATA.TEMP;
    partial_data3 = partial_data1 * partial_data2;
    partial_data4 = partial_data3 + ((float)uncomp_press * (float)uncomp_press * (float)uncomp_press) * CALIB_DATA.PAR_P11;

    comp_press = partial_out1 + partial_out2 + partial_data4;

    return comp_press;
}

/**
 * @brief Get the compensated pressure data from the sensor
 *
 * @param None
 *
 * @return Pressure value
 *
 * @note Taken from the Bosch BMP388 documentation Appendix 9
 */
float get_pressure_data(void)
{
    // Get temperature value first
    uint8_t rx_buffer[3] = {0x00, 0x00, 0x00};

    read_register_multi(DATA_3_BMP388, rx_buffer, 3);
    uint32_t raw_value = (rx_buffer[2] << 16) | (rx_buffer[1] << 8) | (rx_buffer[0]);
    BMP388_compensate_temperature(raw_value);

    // Get pressure value
    read_register_multi(DATA_0_BMP388, rx_buffer, 3);
    raw_value = (rx_buffer[2] << 16) | (rx_buffer[1] << 8) | (rx_buffer[0]);

    return BMP388_compensate_pressure(raw_value);
}

/**
 * @brief Get the ID of the device
 *
 * @param None
 *
 * @return ID value from device
 */
static uint8_t get_device_id(void)
{
    // Read CHIP ID (should be 0x50)
    return read_register_single(CHIP_ID_BMP388);
}

/**
 * @brief Get and convert the manufacturer calibration constant from memory
 *
 * @param None
 *
 * @return None
 */
static void get_calibration_data(void)
{
    uint8_t rx_buffer[21];
    read_register_multi(0x31, rx_buffer, 21);

    uint16_t t1 = ((uint16_t)rx_buffer[1] << 8) | rx_buffer[0];
    uint16_t t2 = ((uint16_t)rx_buffer[3] << 8) | rx_buffer[2];
    uint8_t t3 = rx_buffer[4];

    uint16_t p1 = ((uint16_t)rx_buffer[6] << 8) | rx_buffer[5];
    uint16_t p2 = ((uint16_t)rx_buffer[8] << 8) | rx_buffer[7];
    uint8_t p3 = rx_buffer[9];
    uint8_t p4 = rx_buffer[10];
    uint16_t p5 = ((uint16_t)rx_buffer[12] << 8) | rx_buffer[11];
    uint16_t p6 = ((uint16_t)rx_buffer[14] << 8) | rx_buffer[13];
    uint8_t p7 = rx_buffer[15];
    uint8_t p8 = rx_buffer[16];
    uint16_t p9 = ((uint16_t)rx_buffer[18] << 8) | rx_buffer[17];
    uint8_t p10 = rx_buffer[19];
    uint8_t p11 = rx_buffer[20];

    // Put the raw NVM values into the struct
    RAW_CALIB_DATA.NVM_PAR_T1 = t1;
    RAW_CALIB_DATA.NVM_PAR_T2 = t2;
    RAW_CALIB_DATA.NVM_PAR_T3 = (int8_t)t3;

    RAW_CALIB_DATA.NVM_PAR_P1 = (int16_t)p1;
    RAW_CALIB_DATA.NVM_PAR_P2 = (int16_t)p2;
    RAW_CALIB_DATA.NVM_PAR_P3 = (int8_t)p3;
    RAW_CALIB_DATA.NVM_PAR_P4 = (int8_t)p4;
    RAW_CALIB_DATA.NVM_PAR_P5 = p5;
    RAW_CALIB_DATA.NVM_PAR_P6 = p6;
    RAW_CALIB_DATA.NVM_PAR_P7 = (int8_t)p7;
    RAW_CALIB_DATA.NVM_PAR_P8 = (int8_t)p8;
    RAW_CALIB_DATA.NVM_PAR_P9 = (int16_t)p9;
    RAW_CALIB_DATA.NVM_PAR_P10 = (int8_t)p10;
    RAW_CALIB_DATA.NVM_PAR_P11 = (int8_t)p11;

    // Convert to floating point
    CALIB_DATA.PAR_T1 = (float)RAW_CALIB_DATA.NVM_PAR_T1 * 256.0f;
    CALIB_DATA.PAR_T2 = (float)RAW_CALIB_DATA.NVM_PAR_T2 / 1073741824.0f;
    CALIB_DATA.PAR_T3 = (float)RAW_CALIB_DATA.NVM_PAR_T3 / 281474976710656.0f;

    CALIB_DATA.PAR_P1 = (float)RAW_CALIB_DATA.NVM_PAR_P1 - 16384.0f;
    CALIB_DATA.PAR_P1 /= 1048576.0f;
    CALIB_DATA.PAR_P2 = (float)RAW_CALIB_DATA.NVM_PAR_P2 - 16384.0f;
    CALIB_DATA.PAR_P2 /= 536870912.0f;
    CALIB_DATA.PAR_P3 = (float)RAW_CALIB_DATA.NVM_PAR_P3 / 4294967296.0f;
    CALIB_DATA.PAR_P4 = (float)RAW_CALIB_DATA.NVM_PAR_P4 / 137438953472.0f;
    CALIB_DATA.PAR_P5 = (float)RAW_CALIB_DATA.NVM_PAR_P5 * 8.0f;
    CALIB_DATA.PAR_P6 = (float)RAW_CALIB_DATA.NVM_PAR_P6 / 64.0f;
    CALIB_DATA.PAR_P7 = (float)RAW_CALIB_DATA.NVM_PAR_P7 / 256.0f;
    CALIB_DATA.PAR_P8 = (float)RAW_CALIB_DATA.NVM_PAR_P8 / 32768.0f;
    CALIB_DATA.PAR_P9 = (float)RAW_CALIB_DATA.NVM_PAR_P9 / 281474976710656.0f;
    CALIB_DATA.PAR_P10 = (float)RAW_CALIB_DATA.NVM_PAR_P10 / 281474976710656.0f;
    CALIB_DATA.PAR_P11 = (float)RAW_CALIB_DATA.NVM_PAR_P11 / 36893488147419100000.0f;

    return;
}

/**
 * @brief Initiate and configure the registers for the BMP388 sensor
 *
 * @param None
 *
 * @return 1 for success and 0 for failure
 */
uint8_t init_bmp388(void)
{
    uint8_t success = 1;

    // Start by getting device ID to prove wakeup worked
    uint8_t check_value = get_device_id();
    if (check_value != DEVICE_ID)
    {
        success = 0;
    }

    get_calibration_data();

    // Set interrupt info: push-pull, active high, latching, data ready
    write_register_single(INT_CTRL_BMP388, 0x46);
    check_value = read_register_single(INT_CTRL_BMP388);
    if (check_value != 0x46)
    {
        success = 0;
    }

    // Set oversampling: 8x Pressure, 1x Temperature
    write_register_single(OSR_BMP388, 0x03);
    check_value = read_register_single(OSR_BMP388);
    if (check_value != 0x03)
    {
        success = 0;
    }

    // Set output datarate: 20ms/50Hz
    write_register_single(ODR_BMP388, 0x02);
    check_value = read_register_single(ODR_BMP388);
    if (check_value != 0x02)
    {
        success = 0;
    }

    // Set IIR Filter: 3
    write_register_single(CONFIG_BMP388, 0x04);
    check_value = read_register_single(CONFIG_BMP388);
    if (check_value != 0x04)
    {
        success = 0;
    }

    // Enable sensors
    write_register_single(PWR_CTRL_BMP388, 0x33);
    check_value = read_register_single(PWR_CTRL_BMP388);
    if (check_value != 0x33)
    {
        success = 0;
    }

    check_value = read_register_single(ERR_ERG_BMP388);
    if (check_value & (1U << 2))
    {
        usart_write_string("Sensor configuration failure!\n");
        success = 0;
    }

    return success;
}

/**
 * @brief Sets the Pressure sensor update flag
 *
 * @param None
 *
 * @return None
 */
void set_bmp_ready(void)
{
    BMP_FLAG = 1;
    return;
}

/**
 * @brief Gets the Pressure sensor update flag
 *
 * @param None
 *
 * @return None
 */
uint8_t get_bmp_ready(void)
{
    return BMP_FLAG;
}

/**
 * @brief Resets the Presseure sensor update flag
 *
 * @param None
 *
 * @return None
 */
void reset_bmp_ready(void)
{
    read_register_single(INT_STATUS_BMP388);
    BMP_FLAG = 0;
    return;
}
