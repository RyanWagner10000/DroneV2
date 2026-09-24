/*
 * file: nrf24.c
 * description: file that contains the helper functions for the NRF24L01 radio module
 * author: Ryan Wagner
 * date: September 22, 2026
 * notes:
 */

#include "nrf24.h"

static uint8_t MAX_BUFFER_SIZE = 33;

uint8_t RX_ADDR_P0_BUFFER[ADDRESS_WIDTH] = {0x01, 0x02, 0x03, 0x04, 0x00};
uint8_t RX_ADDR_P1_BUFFER[ADDRESS_WIDTH] = {0x06, 0x07, 0x08, 0x09, 0x0A};
uint8_t RX_ADDR_P2_BUFFER[ADDRESS_WIDTH] = {0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
uint8_t RX_ADDR_P3_BUFFER[ADDRESS_WIDTH] = {0x11, 0x12, 0x13, 0x14, 0x15};
uint8_t RX_ADDR_P4_BUFFER[ADDRESS_WIDTH] = {0x16, 0x17, 0x18, 0x19, 0x1A};
uint8_t RX_ADDR_P5_BUFFER[ADDRESS_WIDTH] = {0x1B, 0x1C, 0x1D, 0x1E, 0x1F};

/**
 * @brief Internal function to write byte to specific register on the radio module
 *
 * @param address Register address on radio module
 * @param value Value to set register on the module
 *
 * @return None
 */
static void write_register_single(uint8_t address, uint8_t value)
{
    uint8_t tx_buffer[2] = {(W_REGISTER | address), value};
    uint8_t rx_buffer[2] = {0, 0};

    // Set CSN pin LOW
    enable_csn();

    // Send write operation
    transfer_spi3(tx_buffer, rx_buffer, 2);

    // Set CSN pin HIGH
    disable_csn();

    return;
}

/**
 * @brief Internal function to write N-many bytes to specific register on the radio module
 *
 * @param address Register address on radio module
 * @param value Value array to set register on the module
 * @param length Length of value array
 *
 * @return None
 */
static void write_register_multi(uint8_t address, uint8_t *value, uint8_t length)
{
    uint8_t tx_buffer[MAX_BUFFER_SIZE];
    uint8_t rx_buffer[MAX_BUFFER_SIZE];

    length = length <= MAX_BUFFER_SIZE ? length : MAX_BUFFER_SIZE;

    // Copy data into Tx buffer
    tx_buffer[0] = W_REGISTER | address;
    for (uint8_t i = 1; i < length + 1; i++)
    {
        tx_buffer[i] = value[i - 1];
    }

    // Set CSN pin LOW
    enable_csn();

    // Send write operation
    transfer_spi3(tx_buffer, rx_buffer, length + 1);

    // Set CSN pin HIGH
    disable_csn();

    return;
}

/**
 * @brief Internal function to read byte of specific register on the radio module
 *
 * @param address Register address on radio module
 *
 * @return None
 */
static uint8_t read_register_single(uint8_t address)
{
    // Format message
    uint8_t tx_buffer[2] = {address, 0x00};
    uint8_t rx_buffer[2] = {0, 0};

    // Set CSN pin LOW
    enable_csn();

    // Send read operation
    transfer_spi3(tx_buffer, rx_buffer, 2);

    // Set CSN pin HIGH
    disable_csn();

    return rx_buffer[1];
}

/**
 * @brief Internal function to read N-many bytes from specific register on the radio module
 *
 * @param address Register address on radio module
 * @param values Values array to get from registers on the module
 * @param length Length of value array
 *
 * @return None
 */
static void read_register_multi(uint8_t address, uint8_t *values, uint8_t length)
{
    uint8_t tx_buffer[MAX_BUFFER_SIZE];
    uint8_t rx_buffer[MAX_BUFFER_SIZE];

    length = length <= MAX_BUFFER_SIZE ? length : MAX_BUFFER_SIZE;

    // Copy data into Tx buffer
    tx_buffer[0] = address;
    for (uint8_t i = 1; i < length + 1; i++)
    {
        tx_buffer[i] = 0x00;
    }

    // Set CSN pin LOW
    enable_csn();

    // Send write operation
    transfer_spi3(tx_buffer, rx_buffer, length + 1);

    // Set CSN pin HIGH
    disable_csn();

    // Transfer data into values array
    for (int8_t i = 0; i < length; i++)
    {
        values[i] = rx_buffer[i + 1];
    }

    return;
}

/**
 * @brief Use the built-in command to flush the Rx buffer on the radio module
 *
 * @param None
 *
 * @return None
 */
void flush_rx(void)
{
    uint8_t tx_buffer[2] = {FLUSH_RX, 0};
    uint8_t rx_buffer[2] = {0, 0};

    // Set CSN pin LOW
    enable_csn();

    // Send write operation
    transfer_spi3(tx_buffer, rx_buffer, 2);

    // Set CSN pin HIGH
    disable_csn();

    for (uint16_t i = 0; i < 1000; ++i)
    {
        ;
    }
    return;
}

/**
 * @brief Use the built-in command to flush the Tx buffer on the radio module
 *
 * @param None
 *
 * @return None
 */
void flush_tx(void)
{
    uint8_t tx_buffer[2] = {FLUSH_TX, 0};
    uint8_t rx_buffer[2] = {0, 0};

    // Set CSN pin LOW
    enable_csn();

    // Send write operation
    transfer_spi3(tx_buffer, rx_buffer, 2);

    // Set CSN pin HIGH
    disable_csn();

    for (uint16_t i = 0; i < 1000; ++i)
    {
        ;
    }
    return;
}

/**
 * @brief Initalize the registers on the radio module
 *
 * @param channel RF Channel frequency offset
 *
 * @return 0 on failure, 1 on success
 */
uint8_t init_radio(uint8_t channel)
{
    uint8_t success = 1;
    uint8_t check_value = 0xFF;

    // Settings
    uint8_t config = 0x00;
    // uint8_t en_aa = 0x3F;
    uint8_t en_aa = 0x00;
    uint8_t en_rxaddr = 0x03;
    uint8_t setup_aw = 0x03;
    uint8_t setup_retr = 0x00;
    uint8_t rf_ch = channel;
    uint8_t rf_setup = 0x0F;

    // Disable radio TxRx
    disable_ce();

    // Set Config register
    write_register_single(CONFIG, config);
    // Check value
    check_value = read_register_single(CONFIG);
    if (check_value != config)
        success = 0;
    check_value = 0xFF;

    // Set Auto-Acknowledge register
    write_register_single(EN_AA, en_aa);
    // Check value
    check_value = read_register_single(EN_AA);
    if (check_value != en_aa)
        success = 0;
    check_value = 0xFF;

    // Set Rx Address register
    write_register_single(EN_RXADDR, en_rxaddr);
    // Check value
    check_value = read_register_single(EN_RXADDR);
    if (check_value != en_rxaddr)
        success = 0;
    check_value = 0xFF;

    // Set Address Widths register
    write_register_single(SETUP_AW, setup_aw);
    // Check value
    check_value = read_register_single(SETUP_AW);
    if (check_value != setup_aw)
        success = 0;
    check_value = 0xFF;

    // Set Auto Retransmission regis
    write_register_single(SETUP_RETR, setup_retr);
    // Check value
    check_value = read_register_single(SETUP_RETR);
    if (check_value != setup_retr)
        success = 0;
    check_value = 0xFF;

    // Set RF Channel register
    write_register_single(RF_CH, rf_ch);
    // Check value
    check_value = read_register_single(RF_CH);
    if (check_value != rf_ch)
        success = 0;
    check_value = 0xFF;

    // Set RF Setup register
    write_register_single(RF_SETUP, rf_setup);
    // Check value
    check_value = read_register_single(RF_SETUP);
    if (check_value != rf_setup)
        success = 0;
    check_value = 0xFF;

    // Set Payload size for Pipe 0
    write_register_single(RX_PW_P0, P0_PACKET_SIZE);
    // Check value
    check_value = read_register_single(RX_PW_P0);
    if (check_value != P0_PACKET_SIZE)
        success = 0;
    check_value = 0xFF;

    // Set Rx Address Pipe 0 register
    write_register_multi(RX_ADDR_P0, RX_ADDR_P0_BUFFER, ADDRESS_WIDTH);

    // Read Rx Address Pipe 0 register to confirm
    uint8_t read_buffer[ADDRESS_WIDTH];
    read_register_multi(RX_ADDR_P0, read_buffer, ADDRESS_WIDTH);

    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        if (read_buffer[i] != RX_ADDR_P0_BUFFER[i])
        {
            usart_write_string("Reading the same data from RX_ADDR_P0 was unsuccessful.\n");
            break;
        }
        // Reset for next use
        read_buffer[i] = 0;
    }

    // Set Tx Address
    write_register_multi(TX_ADDR, RX_ADDR_P0_BUFFER, ADDRESS_WIDTH);
    // Check values
    read_register_multi(TX_ADDR, read_buffer, ADDRESS_WIDTH);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        if (read_buffer[i] != RX_ADDR_P0_BUFFER[i])
        {
            success = 0;
            break;
        }
        // Reset for next use
        read_buffer[i] = 0;
    }

    // Flush Tx and Rx buffers with command
    flush_rx();
    flush_tx();

    return success;
}

/**
 * @brief Print all the radio registers
 *
 * @param None
 *
 * @return None
 */
void print_radio_settings(void)
{
    uint8_t value = 0;

    usart_write_string("\n -- Current NRF24L01 Radio Setting -- \n");

    // Config register
    value = read_register_single(CONFIG);
    char config_str[MAX_INT_STRING];
    int_to_str((int32_t)value, config_str);
    char config_msg[MAX_STRING_CONCAT];
    str_concat("CONFIG = ", config_str, config_msg);
    str_concat(config_msg, "\n", config_msg);
    usart_write_string(config_msg);

    // Enable Auto-acknowledge register
    value = read_register_single(EN_AA);
    char enaa_str[MAX_INT_STRING];
    int_to_str((int32_t)value, enaa_str);
    char enaa_msg[MAX_STRING_CONCAT];
    str_concat("EN_AA = ", enaa_str, enaa_msg);
    str_concat(enaa_msg, "\n", enaa_msg);
    usart_write_string(enaa_msg);

    // Enable Rx Address register
    value = read_register_single(EN_RXADDR);
    char enrxaddr_str[MAX_INT_STRING];
    int_to_str((int32_t)value, enrxaddr_str);
    char enrxaddr_msg[MAX_STRING_CONCAT];
    str_concat("EN_RXADDR = ", enrxaddr_str, enrxaddr_msg);
    str_concat(enrxaddr_msg, "\n", enrxaddr_msg);
    usart_write_string(enrxaddr_msg);

    // Setup Address Widths register
    value = read_register_single(SETUP_AW);
    char setupaw_str[MAX_INT_STRING];
    int_to_str((int32_t)value, setupaw_str);
    char setupaw_msg[MAX_STRING_CONCAT];
    str_concat("SETUP_AW = ", setupaw_str, setupaw_msg);
    str_concat(setupaw_msg, "\n", setupaw_msg);
    usart_write_string(setupaw_msg);

    // Setup Retransmission register
    value = read_register_single(SETUP_RETR);
    char setupretr_str[MAX_INT_STRING];
    int_to_str((int32_t)value, setupretr_str);
    char setupretr_msg[MAX_STRING_CONCAT];
    str_concat("SETUP_RETR = ", setupretr_str, setupretr_msg);
    str_concat(setupretr_msg, "\n", setupretr_msg);
    usart_write_string(setupretr_msg);

    // RF Channel register
    value = read_register_single(RF_CH);
    char rfch_str[MAX_INT_STRING];
    int_to_str((int32_t)value, rfch_str);
    char rfch_msg[MAX_STRING_CONCAT];
    str_concat("RF_CH = ", rfch_str, rfch_msg);
    str_concat(rfch_msg, "\n", rfch_msg);
    usart_write_string(rfch_msg);

    // RF Setup register
    value = read_register_single(RF_SETUP);
    char rfsetup_str[MAX_INT_STRING];
    int_to_str((int32_t)value, rfsetup_str);
    char rfsetup_msg[MAX_STRING_CONCAT];
    str_concat("RF_SETUP = ", rfsetup_str, rfsetup_msg);
    str_concat(rfsetup_msg, "\n", rfsetup_msg);
    usart_write_string(rfsetup_msg);

    // Status register
    value = read_register_single(STATUS);
    char status_str[MAX_INT_STRING];
    int_to_str((int32_t)value, status_str);
    char status_msg[MAX_STRING_CONCAT];
    str_concat("STATUS = ", status_str, status_msg);
    str_concat(status_msg, "\n", status_msg);
    usart_write_string(status_msg);

    // FIFO Status register
    value = read_register_single(FIFO_STATUS);
    char fifostatus_str[MAX_INT_STRING];
    int_to_str((int32_t)value, fifostatus_str);
    char fifostatus_msg[MAX_STRING_CONCAT];
    str_concat("FIFO_STATUS = ", fifostatus_str, fifostatus_msg);
    str_concat(fifostatus_msg, "\n", fifostatus_msg);
    usart_write_string(fifostatus_msg);

    // TX_ADDR register
    uint8_t read_buffer[ADDRESS_WIDTH];
    read_register_multi(TX_ADDR, read_buffer, ADDRESS_WIDTH);
    char tx_addr_msg[MAX_STRING_CONCAT];
    str_concat("TX_ADDR = {", "", tx_addr_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char tx_addr_str[MAX_INT_STRING];
        int_to_str((int32_t)read_buffer[i], tx_addr_str);
        // Concat
        str_concat(tx_addr_msg, tx_addr_str, tx_addr_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            str_concat(tx_addr_msg, ", ", tx_addr_msg);
    }
    str_concat(tx_addr_msg, "}\n", tx_addr_msg);
    usart_write_string(tx_addr_msg);

    // RX_ADDR_P0 register
    read_register_multi(RX_ADDR_P0, read_buffer, ADDRESS_WIDTH);
    char rx_addr0_msg[MAX_STRING_CONCAT];
    str_concat("RX_ADDR_P0 = {", "", rx_addr0_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr0_str[MAX_INT_STRING];
        int_to_str((int32_t)read_buffer[i], rx_addr0_str);
        // Concat
        str_concat(rx_addr0_msg, rx_addr0_str, rx_addr0_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            str_concat(rx_addr0_msg, ", ", rx_addr0_msg);
    }
    str_concat(rx_addr0_msg, "}\n", rx_addr0_msg);
    usart_write_string(rx_addr0_msg);

    // RX_ADDR_P1 register
    read_register_multi(RX_ADDR_P1, read_buffer, ADDRESS_WIDTH);
    char rx_addr1_msg[MAX_STRING_CONCAT];
    str_concat("RX_ADDR_P1 = {", "", rx_addr1_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr1_str[MAX_INT_STRING];
        int_to_str((int32_t)read_buffer[i], rx_addr1_str);
        // Concat
        str_concat(rx_addr1_msg, rx_addr1_str, rx_addr1_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            str_concat(rx_addr1_msg, ", ", rx_addr1_msg);
    }
    str_concat(rx_addr1_msg, "}\n", rx_addr1_msg);
    usart_write_string(rx_addr1_msg);

    // RX_ADDR_P2 register
    read_register_multi(RX_ADDR_P2, read_buffer, ADDRESS_WIDTH);
    char rx_addr2_msg[MAX_STRING_CONCAT];
    str_concat("RX_ADDR_P2 = {", "", rx_addr2_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr2_str[MAX_INT_STRING];
        int_to_str((int32_t)read_buffer[i], rx_addr2_str);
        // Concat
        str_concat(rx_addr2_msg, rx_addr2_str, rx_addr2_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            str_concat(rx_addr2_msg, ", ", rx_addr2_msg);
    }
    str_concat(rx_addr2_msg, "}\n", rx_addr2_msg);
    usart_write_string(rx_addr2_msg);

    // RX_ADDR_P3 register
    read_register_multi(RX_ADDR_P3, read_buffer, ADDRESS_WIDTH);
    char rx_addr3_msg[MAX_STRING_CONCAT];
    str_concat("RX_ADDR_P3 = {", "", rx_addr3_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr3_str[MAX_INT_STRING];
        int_to_str((int32_t)read_buffer[i], rx_addr3_str);
        // Concat
        str_concat(rx_addr3_msg, rx_addr3_str, rx_addr3_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            str_concat(rx_addr3_msg, ", ", rx_addr3_msg);
    }
    str_concat(rx_addr3_msg, "}\n", rx_addr3_msg);
    usart_write_string(rx_addr3_msg);

    // RX_ADDR_P4 register
    read_register_multi(RX_ADDR_P4, read_buffer, ADDRESS_WIDTH);
    char rx_addr4_msg[MAX_STRING_CONCAT];
    str_concat("RX_ADDR_P4 = {", "", rx_addr4_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr4_str[MAX_INT_STRING];
        int_to_str((int32_t)read_buffer[i], rx_addr4_str);
        // Concat
        str_concat(rx_addr4_msg, rx_addr4_str, rx_addr4_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            str_concat(rx_addr4_msg, ", ", rx_addr4_msg);
    }
    str_concat(rx_addr4_msg, "}\n", rx_addr4_msg);
    usart_write_string(rx_addr4_msg);

    // RX_ADDR_P5 register
    read_register_multi(RX_ADDR_P5, read_buffer, ADDRESS_WIDTH);
    char rx_addr5_msg[MAX_STRING_CONCAT];
    str_concat("RX_ADDR_P5 = {", "", rx_addr5_msg);
    for (int8_t i = 0; i < ADDRESS_WIDTH; i++)
    {
        // Convert int to string
        char rx_addr5_str[MAX_INT_STRING];
        int_to_str((int32_t)read_buffer[i], rx_addr5_str);
        // Concat
        str_concat(rx_addr5_msg, rx_addr5_str, rx_addr5_msg);
        // Add comma if necessary
        if (i < ADDRESS_WIDTH - 1)
            str_concat(rx_addr5_msg, ", ", rx_addr5_msg);
    }
    str_concat(rx_addr5_msg, "}\n", rx_addr5_msg);
    usart_write_string(rx_addr5_msg);

    usart_write_string("\n\n");

    return;
}

/**
 * @brief
 *
 * @param None
 *
 * @return None
 */
struct NRF24_STATUS_DATA StatusRadio(void)
{
    struct NRF24_STATUS_DATA status_data;
    uint8_t status = 0;

    status = read_register_single(STATUS);
    status_data.STATUS_BYTE = status;
    status_data.RX_DR = (status >> 6) & 1;
    status_data.TX_DS = (status >> 5) & 1;
    status_data.MAX_RT = (status >> 4) & 1;
    status_data.RX_P_NO = (status >> 1) & 7; // 3 bits
    status_data.TX_FULL = status & 1;

    // status = 0;
    // status = read_register_single(FIFO_STATUS);
    // status_data.FIFO_STATUS_BYTE = status;
    // status_data.TX_REUSE = (status >> 6) & 1;
    // status_data.TX_FULL_F = (status >> 5) & 1;
    // status_data.TX_EMPTY = (status >> 4) & 1;
    // status_data.RX_FULL = (status >> 1) & 1;
    // status_data.RX_EMPTY = status & 1;

    // status = 0;
    // status = read_register_single(OBSERVE_TX);
    // status_data.PLOS_CNT = status & 15;
    // status_data.ARC_CNT = (status >> 4) & 15;

    return status_data;
}

/**
 * @brief Setup the NRF24L01 radio module in Tx mode
 *
 * @param None
 *
 * @return 0 on failure, 1 on success
 */
uint8_t set_tx_mode(void)
{
    uint8_t success = 1;
    uint8_t check_value = 0xFF;

    // Turn Radio OFF
    disable_ce();

    // Get current config
    uint8_t config = read_register_single(CONFIG);
    // Modify config register and send back
    config |= (1 << 1);
    config &= ~(1 << 0);
    write_register_single(CONFIG, config);
    // Check value
    check_value = read_register_single(CONFIG);
    if (check_value != config)
        success = 0;

    // Turn Radio ON
    enable_ce();

    return success;
}

/**
 * @brief Setup the NRF24L01 radio module in Rx mode
 *
 * @param None
 *
 * @return 0 on failure, 1 on success
 */
uint8_t set_rx_mode(void)
{
    uint8_t success = 1;
    uint8_t check_value = 0xFF;

    // Turn Radio OFF
    disable_ce();

    // Get current config
    uint8_t config = read_register_single(CONFIG);
    // Modify config register and send back
    config |= (1 << 1);
    config |= (1 << 0);
    write_register_single(CONFIG, config);
    // Check value
    check_value = read_register_single(CONFIG);
    if (check_value != config)
        success = 0;

    // Turn Radio ON
    enable_ce();

    return success;
}

/**
 * @brief Transmit data from the radio
 *
 * @param data Array of values to transmit
 * @param length Length of data array
 *
 * @return None
 */
void transmit_radio(uint8_t *data, uint8_t length)
{
    // Flush buffers
    // flush_rx();
    flush_tx();

    // Clamp the size of the data to within limit
    length = length < P0_PACKET_SIZE ? length : P0_PACKET_SIZE;

    // Make sure radio is OFF
    disable_ce();

    // Load data into txbuffer
    write_register_multi(W_TX_PAYLOAD, data, length);

    // Toggle radio for at least 10us to transmit data
    enable_ce();
    for (uint16_t i = 0; i < 1000; ++i)
    {
        ;
    }
    disable_ce();

    return;
}

/**
 * @brief Check to see is data is available to read on the FIFO
 *
 * @param None
 *
 * @return 0 on none available, 1 on available
 */
uint8_t data_available(void)
{
    struct NRF24_STATUS_DATA status;

    status = StatusRadio();

    return status.RX_P_NO < 7 ? 1 : 0;
}

/**
 * @brief Check to see if the Tx FIFO is full
 *
 * @param None
 *
 * @return 0 on not full, 1 on full
 */
uint8_t tx_FIFO_full(void)
{
    struct NRF24_STATUS_DATA status;

    status = StatusRadio();

    return status.TX_FULL;
}

/**
 * @brief Read the radio Rx FIFO
 *
 * @param data Array of values to transmit
 * @param pps Pipe number correlatred to packet size
 *
 * @return None
 */
void read_radio(RadioPacket *packet, enum PIPE_PACKET_SIZE pps)
{
    // Make sure the array is aligned in memory
    // So that when it gets cast to the struct it lines up
    uint8_t __attribute__((aligned(2))) rx_buffer[pps];

    // Read Rx FIFO register
    read_register_multi(R_RX_PAYLOAD, rx_buffer, pps);

    // Delay per documentation
    for (uint16_t i = 0; i < 1000; ++i)
    {

    }

    // Turn radio on
    enable_ce();

    flush_rx();

    // Cast data array into struct
    *packet = *(RadioPacket *)rx_buffer;

    return;
}

/**
 * @brief Print the contents of a packet
 *
 * @param packet RadioPacket object to print
 *
 * @return None
 */
void print_packet(RadioPacket packet)
{
    usart_write_string("packet_id = ");
    usart_write_number((int32_t)packet.packet_id);
    usart_write_char('\n');
    usart_write_string("flags = ");
    usart_write_number((int32_t)packet.flags);
    usart_write_char('\n');
    usart_write_string("throttle = ");
    usart_write_number((int32_t)packet.throttle);
    usart_write_char('\n');
    usart_write_string("roll = ");
    usart_write_number((int32_t)packet.roll);
    usart_write_char('\n');
    usart_write_string("pitch = ");
    usart_write_number((int32_t)packet.pitch);
    usart_write_char('\n');
    usart_write_string("yaw = ");
    usart_write_number((int32_t)packet.yaw);
    usart_write_char('\n');
    usart_write_string("button = ");
    usart_write_number((int32_t)packet.button);
    usart_write_char('\n');
    usart_write_string("checksum = ");
    usart_write_number((int32_t)packet.checksum);
    usart_write_char('\n');
    usart_write_char('\n');

    return;
}