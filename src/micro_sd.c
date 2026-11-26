/*
 * file: micro_sd.c
 * description: Contains init and helper functions for interface with micro sd card reader
 * author: Ryan Wagner
 * date: November 6, 2026
 * notes:
 */

#include "micro_sd.h"

// Global SD Card Response Structs
R1_return r1 = {0};
R2_return r2 = {0};
R3_return r3 = {0};
R7_return r7 = {0};

/**
 * @brief Function to set the memory of an object
 *
 * @param None
 *
 * @return None
 */
void *memset(void *dest, int val, size_t len)
{
    unsigned char *ptr = (unsigned char *)dest;
    while (len-- > 0)
    {
        *ptr++ = (unsigned char)val;
    }
    return dest;
}

/**
 * @brief Function to compare the memory values of one object to another
 *
 * @param None
 *
 * @return None
 */
int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
        {
            return (p1[i] < p2[i]) ? -1 : 1;
        }
    }
    return 0;
}

/**
 * @brief Function to copy the memory contatins of one object to another
 *
 * @param None
 *
 * @return None
 */
void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    while (n--)
    {
        *d++ = *s++;
    }
    return dest;
}

/**
 * @brief Function to compare the contents of a char array (string)
 *
 * @param None
 *
 * @return None
 */
char *strchr(const char *s, int c)
{
    while (*s != (char)c)
    {
        if (*s == '\0')
        {
            return NULL;
        }
        s++;
    }
    return (char *)s;
}

/**
 * @brief Function to check the values of an R7 response
 *
 * @param echo_back Echo correction byte for comparison
 *
 * @return 1 on success, 0 on failure
 */
static uint8_t check_r7_response(uint8_t echo_back)
{
    uint8_t success = 1;

    // print_register("R7 response", r7.raw_r7, DECIMAL);

    // Voltage accepted if 1
    if (r7.volt_accpt != 1)
    {
        usart_write_string("Voltage NOT accepted\n");
        success = 0;
    }

    // Check if echo the same
    if (r7.echo != echo_back)
    {
        usart_write_string("Echo back NOT the same\n");
        success = 0;
    }

    return success;
}

/**
 * @brief Function to check the values of an R2 response
 *
 * @param None
 *
 * @return 1 on success, 0 on failure
 */
static uint8_t check_r2_response(void)
{
    uint8_t success = 1;

    if (r2.locked)
    {
        success = 0;
        usart_write_string("Card is Locked\n");
    }
    if (r2.erase_lock_fail)
    {
        success = 0;
        usart_write_string("WP Erase Skip | Lock/Unlock CMD Failed\n");
    }
    if (r2.error)
    {
        success = 0;
        usart_write_string("Error\n");
    }
    if (r2.cc_error)
    {
        success = 0;
        usart_write_string("CC Error\n");
    }
    if (r2.ecc_fail)
    {
        success = 0;
        usart_write_string("Card ECC Error\n");
    }
    if (r2.wp_violation)
    {
        success = 0;
        usart_write_string("WP Violation\n");
    }
    if (r2.erase_param)
    {
        success = 0;
        usart_write_string("Erase Parameter\n");
    }
    if (r2.out_of_range)
    {
        success = 0;
        usart_write_string("Out of Range | CSD Overwrite\n");
    }
    return success;
}

/**
 * @brief Function to check the values of on R1 response
 *
 * @param None
 *
 * @return 1 on success, 0 on failure
 */
static uint8_t check_r1_response(void)
{
    if (r1.raw_r1 > 127)
    {
        return 0;
    }

    uint8_t success = 1;
    uint8_t flags = 0;

    if (r1.idle_state)
    {
        // Everything is fine, just idling
        // usart_write_string("In Idle State\n");
        ;
        flags++;
    }
    if (r1.erase_reset)
    {
        success = 0;
        flags++;
        usart_write_string("Erase Reset\n");
    }
    if (r1.illegal_cmd)
    {
        success = 0;
        flags++;
        usart_write_string("Illegal Command\n");
    }
    if (r1.com_crc_error)
    {
        success = 0;
        flags++;
        usart_write_string("Com CRC Error\n");
    }
    if (r1.erase_seq_error)
    {
        success = 0;
        flags++;
        usart_write_string("Erase Sequence Error\n");
    }
    if (r1.addr_error)
    {
        success = 0;
        flags++;
        usart_write_string("Address Error\n");
    }
    if (r1.param_error)
    {
        success = 0;
        flags++;
        usart_write_string("Parameter Error\n");
    }

    if (flags > 1)
    {
        usart_write_string("  Too many flags [");
        usart_write_number((int32_t)flags);
        usart_write_string("]. Probably a false response value.\n");
        success = 0;
    }

    return success;
}

/**
 * @brief Function to send an empty byte to clear the MISO line
 *
 * @param None
 *
 * @return None
 */
static void send_empty(void)
{
    // Transmit empty byte to clear MISO
    uint8_t tx_byte[1] = {0xFF};
    uint8_t rx_byte[1] = {0x00};
    transfer_spi2(tx_byte, rx_byte, 1);

    return;
}

/**
 * @brief Function to send any SD Card command, arguement, and correction code
 *
 * @param cmd Command number
 * @param arg Argument
 * @param crc Code correction byte
 *
 * @return 1 on success, 0 on failure
 */
static uint8_t send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t tx[6] = {0x40 | cmd, arg >> 24, arg >> 16, arg >> 8, arg, crc};
    uint8_t rx[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t _tx[1] = {0xFF}, _rx[1] = {0xFF};

    transfer_spi2(tx, rx, 6);

    for (uint8_t i = 0; i < 10; i++)
    {
        transfer_spi2(_tx, _rx, 1);
        r1.raw_r1 = _rx[0];
        if (check_r1_response())
        {
            return 1;
        }
        _rx[0] = 0xFF;
    }

    return 0;
}

/**
 * @brief Function to "wait" until a certain byte is received
 *
 * @param value Byte value to wait for
 *
 * @return 1 on success, 0 on failure
 */
uint8_t wait_till(uint8_t value)
{
    // Wait until 'value' is received
    uint8_t tx_byte[1] = {0xFF};
    uint8_t rx_byte[1] = {0x00};

    for (uint8_t i = 0; i < 100; i++)
    {
        transfer_spi2(tx_byte, rx_byte, 1);

        if (rx_byte[0] == value)
        {
            return 1;
        }

        delay_millisecond(1);
    }

    return 0;
}

/**
 * @brief Function to read N-many blocks from the SD card
 *
 * @param num_blocks Number of blocks to read
 * @param buffer Empty buffer that will store the read data
 *
 * @return 1 on success, 0 on failure
 */
static uint8_t read_block(uint32_t num_blocks, uint8_t *buffer)
{
    uint8_t tx_byte[1] = {0xFF};
    uint8_t rx_byte[1] = {0x00};

    // Read 512 bytes and discard the extra 2 CRC bytes
    uint8_t tx_buff[512];
    memset(tx_buff, 0xFF, 512);

    for (uint32_t i = 0; i < num_blocks; i++)
    {
        // Wait until Start Block Token arrives
        if (!wait_till(0xFE))
        {
            disable_cs_sdcard();
            return 0;
        }

        transfer_spi2(tx_buff, &buffer[i * 512], 512);

        // Read the two CRC bytes
        transfer_spi2(tx_byte, rx_byte, 1);
        transfer_spi2(tx_byte, rx_byte, 1);
    }

    return 1;
}

/**
 * @brief Function to write N-many blocks to the SD card
 *
 * @param num_blocks Number of blocks to write
 * @param buffer Buffer with data to write
 *
 * @return 1 on success, 0 on failure
 */
static uint8_t write_block(uint32_t num_blocks, const uint8_t *buffer)
{
    while (num_blocks)
    {
        // Send 0xFE token
        uint8_t tx_token[2] = {0xFF, 0xFE};
        uint8_t rx_token[2] = {0x00, 0x00};
        transfer_spi2(tx_token, rx_token, 2);

        // Send 512 bytes from buffer
        uint8_t rx_buffer[512] = {0x00};
        transfer_spi2(buffer, rx_buffer, 512);

        // Send 2 dummy CRC bytes
        uint8_t tx_dummy_crc[3] = {0xFF, 0xFF, 0xFF};
        uint8_t rx_dummy_crc[3] = {0x00, 0x00, 0x00};
        transfer_spi2(tx_dummy_crc, rx_dummy_crc, 3);

        // Check for data response
        if ((rx_dummy_crc[2] & 0x1F) != 0x05)
        {
            usart_write_string("Did not get 0x01 response after CRC when writing\n");
            return 0;
        }

        num_blocks--;
    }

    if (!wait_till(0xFF))
    {
        usart_write_string("Did not stop being busy for writing\n");
        return 0;
    }

    return 1;
}

/**
 * @brief Function to send the CMD0 command
 *
 * @param None
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd0(void)
{
    // Pull CS low
    enable_cs_sdcard();

    uint8_t pass = send_cmd(0, 0x00, 0x95);

    // Pull CS high
    disable_cs_sdcard();

    // Reset MISO
    send_empty();

    if (pass)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Function to send the CMD8 command
 *
 * @param None
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd8(void)
{
    // Pull CS low
    enable_cs_sdcard();

    uint8_t pass = send_cmd(8, 0x01AA, 0x87);

    // Get the rest of he R7 response
    if (pass)
    {
        uint8_t tx[4] = {0xFF, 0xFF, 0xFF, 0xFF};
        uint8_t rx[4] = {0xFF, 0xFF, 0xFF, 0xFF};
        transfer_spi2(tx, rx, 4);

        r7.raw_r7 = ((uint32_t)(rx[0]) << 24) | ((uint32_t)(rx[1]) << 16) | ((uint32_t)(rx[2]) << 8) | (uint32_t)(rx[3]);
        if (check_r7_response(0xAA))
        {
            pass = 1;
        }
        else
        {
            pass = 0;
        }
    }

    // Pull CS high
    disable_cs_sdcard();

    // Reset MISO
    send_empty();

    if (pass)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Function to send the CMD12 command
 *
 * @param None
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd12(void)
{
    if (!send_cmd(8, 0x01AA, 0x87))
    {
        return 0;
    }

    if (!wait_till(0xFF))
    {
        return 0;
    }

    return 1;
}

/**
 * @brief Function to send the CMD13 command
 *
 * @param None
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd13(void)
{
    // Pull CS low
    enable_cs_sdcard();

    uint8_t pass = send_cmd(13, 0x00, 0x00);

    // Get the rest of the R2 response
    if (pass)
    {
        uint8_t tx[1] = {0xFF};
        uint8_t rx[1] = {0xFF};
        transfer_spi2(tx, rx, 1);

        r2.raw_r2 = rx[0];
        if (check_r2_response())
        {
            pass = 1;
        }
        else
        {
            pass = 0;
        }
    }

    // Pull CS high
    disable_cs_sdcard();

    // Reset MISO
    send_empty();

    if (pass)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Function to send the CMD17 command
 *
 * @param buffer Empty buffer for reading a block of data
 * @param addr Address to read data from
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd17(uint8_t *buffer, uint32_t addr)
{
    // Pull CS low
    enable_cs_sdcard();

    uint8_t pass = send_cmd(17, addr, 0x00);

    // Start reading process
    if (!read_block(1, buffer))
    {
        usart_write_string("\n-- Micro Sd Card Reading Block Failure --\n");
        disable_cs_sdcard();
        return 0;
    }

    // Pull CS high
    disable_cs_sdcard();

    // Reset MISO
    send_empty();

    if (pass)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Function to send the CMD18 command
 *
 * @param buffer Empty buffer for reading N-many blocks of data
 * @param addr Address to read data from
 * @param count Number of blocks to read
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd18(uint8_t *buffer, uint32_t addr, uint32_t count)
{
    // Pull CS low
    enable_cs_sdcard();

    uint8_t pass = send_cmd(18, addr, 0x00);

    // Start reading process
    if (!read_block(count, buffer))
    {
        usart_write_string("\n-- Micro Sd Card Reading Block Failure --\n");
        disable_cs_sdcard();
        return 0;
    }

    // Send Stop Transmission
    if (!send_cmd12())
    {
        usart_write_string("\n-- Micro Sd Card [CMD12] Failure --\n");
        disable_cs_sdcard();
        return 0;
    }

    // Pull CS high
    disable_cs_sdcard();

    // Reset MISO
    send_empty();

    if (pass)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Function to send the CMD24 command
 *
 * @param buffer Filled buffer for writing a block of data
 * @param addr Address to write data to
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd24(const uint8_t *buffer, uint32_t addr)
{
    // Pull CS low
    enable_cs_sdcard();

    uint8_t pass = send_cmd(24, addr, 0x00);

    // Start writing process
    if (!pass || !write_block(1, buffer))
    {
        usart_write_string("\n-- Micro Sd Card Writing Block Failure --\n");
        pass = 0;
    }

    // Pull CS high
    disable_cs_sdcard();

    // Reset MISO
    send_empty();

    if (pass)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Function to send the CMD25 command
 *
 * @param buffer Filled buffer for writing N-many blocks of data
 * @param addr Address to write data to
 * @param count Number of blocks to write
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd25(const uint8_t *buffer, uint32_t addr, uint32_t count)
{
    // Pull CS low
    enable_cs_sdcard();

    uint8_t pass = send_cmd(25, addr, 0x00);

    // Start writing process
    if (!pass || !write_block(count, buffer))
    {
        usart_write_string("\n-- Micro Sd Card Writing Block Failure --\n");
        pass = 0;
    }

    // Send Stop Transmission
    if (!pass || !send_cmd12())
    {
        usart_write_string("\n-- Micro Sd Card [CMD12] Failure --\n");
        pass = 0;
    }

    // Pull CS high
    disable_cs_sdcard();

    // Reset MISO
    send_empty();

    if (pass)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Function to send the CMD55 command
 *
 * @param None
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd55(void)
{
    // Send CMD55
    uint8_t rx_cmd55[20] = {0x00};
    uint8_t CMD55[20] = {0x77, 0x00, 0x00, 0x00, 0x00, 0x65, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    transfer_spi2(CMD55, rx_cmd55, 20);

    for (uint8_t i = 6; i < 20; i++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_cmd55[i];

        // Check R1 response for errors
        if (check_r1_response())
        {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Function to send the CMD58 command
 *
 * @param None
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_cmd58(void)
{
    // Send CMD58

    // Pull CS low
    enable_cs_sdcard();

    uint8_t pass = send_cmd(58, 0x00, 0xFD);

    // Get the rest of the R3 response
    if (pass)
    {
        uint8_t tx[4] = {0xFF, 0xFF, 0xFF, 0xFF};
        uint8_t rx[4] = {0xFF, 0xFF, 0xFF, 0xFF};
        transfer_spi2(tx, rx, 4);

        r3.raw_r3 = ((uint32_t)(rx[0]) << 24) | ((uint32_t)(rx[1]) << 16) | ((uint32_t)(rx[2]) << 8) | (uint32_t)(rx[3]);
    }

    // Pull CS high
    disable_cs_sdcard();

    // Reset MISO
    send_empty();

    if (pass)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief Function to send the CMD41 command
 *
 * @param None
 *
 * @return 1 on success, 0 on failure
 */
uint8_t send_acmd41(void)
{
    // Send ACMD41
    uint8_t rx_acmd41[10] = {0x00};
    uint8_t ACMD41[10] = {0x69, 0x40, 0x00, 0x00, 0x00, 0x77, 0xFF, 0xFF, 0xFF, 0xFF};

    transfer_spi2(ACMD41, rx_acmd41, 10);

    for (uint8_t i = 6; i < 10; i++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_acmd41[i];

        // Check R1 response for errors
        if (check_r1_response() && (r1.raw_r1 == 0))
        {
            return 1;
        }
    }

    return 0;
}
