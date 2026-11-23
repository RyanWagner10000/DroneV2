/*
 * file: micro_sd.c
 * description: Contains init and helper functions for interface with micro sd card reader
 * author: Ryan Wagner
 * date: November 6, 2026
 * notes:
 */

#include "micro_sd.h"

// Default commands including buffer space at the end for return data
// #define CMD_LEN (6)

// static uint8_t CMD0_GO_IDLE_STATE[CMD_LEN] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
// static uint8_t CMD1_SEND_OP_COND[CMD_LEN] = {0x41, 0x00, 0x00, 0x00, 0x00, 0xF9};
// static uint8_t CMD8_SEND_IF_COND[CMD_LEN] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};
// static uint8_t CMD9_SEND_CSD[CMD_LEN] = {0x49, 0x00, 0x00, 0x00, 0x00, 0xAF};
// static uint8_t CMD10_SEND_CID[CMD_LEN] = {0x4A, 0x00, 0x00, 0x00, 0x00, 0x1B};
// static uint8_t CMD12_STOP_TRANSMISSION[CMD_LEN] = {0x4C, 0x00, 0x00, 0x00, 0x00, 0x61};
// static uint8_t CMD13_SEND_STATUS[CMD_LEN] = {0x4D, 0x00, 0x00, 0x00, 0x00, 0x0D};
// static uint8_t CMD16_SET_BLOCKLEN[CMD_LEN] = {0x50, 0x00, 0x00, 0x00, 0x00, 0x39};
// static uint8_t CMD17_READ_SINGLE_BLOCK[CMD_LEN] = {0x51, 0x00, 0x00, 0x00, 0x00, 0x55};
// static uint8_t CMD18_READ_MULTIPLE_BLOCK[CMD_LEN] = {0x52, 0x00, 0x00, 0x00, 0x00, 0xE1};
// static uint8_t CMD24_WRITE_BLOCK[CMD_LEN] = {0x58, 0x00, 0x00, 0x00, 0x00, 0x6F};
// static uint8_t CMD25_WRITE_MULTIPLE_BLOCK[CMD_LEN] = {0x59, 0x00, 0x00, 0x00, 0x00, 0x03};
// static uint8_t CMD27_PROGRAM_CSD[CMD_LEN] = {0x5B, 0x00, 0x00, 0x00, 0x00, 0xDB};
// static uint8_t CMD32_ERASE_WR_BLK_START_ADDR[CMD_LEN] = {0x60, 0x00, 0x00, 0x00, 0x00, 0xDF};
// static uint8_t CMD33_ERASE_WR_BLK_END_ADDR[CMD_LEN] = {0x61, 0x00, 0x00, 0x00, 0x00, 0xB3};
// static uint8_t CMD38_ERASE[CMD_LEN] = {0x66, 0x00, 0x00, 0x00, 0x00, 0xA5};
// static uint8_t CMD55_APP_CMD[CMD_LEN] = {0x77, 0x00, 0x00, 0x00, 0x00, 0x65};
// static uint8_t CMD56_GEN_CMD[CMD_LEN] = {0x78, 0x00, 0x00, 0x00, 0x00, 0x25};
// static uint8_t CMD58_READ_OCR[CMD_LEN] = {0x7A, 0x00, 0x00, 0x00, 0x00, 0xFD};
// static uint8_t CMD59_CRC_ON_OFF[CMD_LEN] = {0x7B, 0x00, 0x00, 0x00, 0x00, 0x91};
// static uint8_t ACMD41_SD_SEND_OP_COND[CMD_LEN] = {0x69, 0x40, 0x00, 0x00, 0x00, 0x77};

R1_return r1 = {0};
R2_return r2 = {0};
R3_return r3 = {0};
R7_return r7 = {0};

void *memset(void *dest, int val, size_t len)
{
    unsigned char *ptr = (unsigned char *)dest;
    while (len-- > 0)
    {
        *ptr++ = (unsigned char)val;
    }
    return dest;
}

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

uint8_t busy_wait()
{
    // Wait until 0xFF arrives
    uint8_t tx_byte[1] = {0xFF};
    uint8_t rx_byte[1] = {0x00};
    for (uint8_t i = 0; i < 100; i++)
    {
        transfer_spi2(tx_byte, rx_byte, 1);
        if (rx_byte[0] == 0xFF)
        {
            return 1;
        }

        delay_millisecond(1);
    }

    return 0;
}

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
        return 0;
    }

    return success;
}

// static uint8_t wait_till(uint8_t value)
// {
//     // Wait until 'value' is received
//     uint8_t tx_byte[1] = {0xFF};
//     uint8_t rx_byte[1] = {0x00};

//     for (uint8_t i = 0; i < 100; i++)
//     {
//         transfer_spi2(tx_byte, rx_byte, 1);

//         if (rx_byte[0] == value)
//         {
//             return 1;
//         }

//         delay_millisecond(1);
//     }

//     return 0;
// }

static uint8_t wait_start_block(void)
{
    // Wait until Start Block Token arrives
    uint8_t tx_byte[1] = {0xFF};
    uint8_t rx_byte[1] = {0x00};

    for (uint8_t i = 0; i < 100; i++)
    {
        transfer_spi2(tx_byte, rx_byte, 1);

        if (rx_byte[0] == 0xFE)
        {
            return 1;
        }

        delay_millisecond(1);
    }

    return 0;
}

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
        if (!wait_start_block())
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

    if (!busy_wait())
    {
        usart_write_string("Did not stop being busy for writing\n");
        return 0;
    }

    return 1;
}

uint8_t send_cmd0(void)
{
    // Send CMD0
    uint8_t rx_cmd0[10] = {0x00};
    uint8_t CMD0[10] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95, 0xFF, 0xFF, 0xFF, 0xFF};

    // Pull CS low
    enable_cs_sdcard();

    transfer_spi2(CMD0, rx_cmd0, 10);

    // Pull CS high
    disable_cs_sdcard();

    for (uint8_t i = 6; i < 10; i++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_cmd0[i];
        if (check_r1_response())
        {
            return 1;
        }
    }

    return 0;
}

uint8_t send_cmd8(void)
{
    // Send CMD8
    uint8_t rx_cmd8[20] = {0x00};
    uint8_t CMD8[20] = {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Pull CS low
    enable_cs_sdcard();

    transfer_spi2(CMD8, rx_cmd8, 20);

    // Pull CS high
    disable_cs_sdcard();

    // Response is R1 & R7 format
    for (uint8_t i = 6; i < 20; i++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_cmd8[i];

        // Check R1 response for errors
        if (check_r1_response() && ((i + 4) < 20))
        {
            r7.raw_r7 = ((uint32_t)(rx_cmd8[i + 1]) << 24) | ((uint32_t)(rx_cmd8[i + 2]) << 16) | ((uint32_t)(rx_cmd8[i + 3]) << 8) | (uint32_t)(rx_cmd8[i + 4]);
            // Check R7 response for voltage acceptance and echo
            if (check_r7_response(0xAA))
            {
                return 1;
            }

            break;
        }
    }

    return 0;
}

uint8_t send_cmd12(void)
{
    // Send CMD12
    uint8_t rx_cmd12[10] = {0x00};
    uint8_t CMD12[10] = {0x4C, 0x00, 0x00, 0x00, 0x00, 0x95, 0xFF, 0xFF, 0xFF, 0xFF};

    transfer_spi2(CMD12, rx_cmd12, 10);

    uint8_t kick_first = 0;

    for (uint8_t i = 6; i < 10; i++)
    {
        if (!kick_first)
        {
            kick_first++;
            continue;
        }

        // Response is R1 format
        r1.raw_r1 = rx_cmd12[i];

        // Check R1 response for errors
        if (!check_r1_response())
        {
            return 0;
        }
    }

    // Wait for MISO to go high
    while (~(GPIOB->IDR & (1U << 14)))
    {
        ;
    }

    return 1;
}

uint8_t send_cmd13(void)
{
    // Send CMD13
    uint8_t rx_cmd13[15] = {0x00};
    uint8_t CMD13[15] = {0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t index = 0;

    enable_cs_sdcard();

    transfer_spi2(CMD13, rx_cmd13, 15);

    for (index = 6; index < 14; index++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_cmd13[index];

        // Check R1 response for errors
        if (check_r1_response())
        {
            r2.raw_r2 = rx_cmd13[index + 1];

            if (check_r2_response())
            {
                break;
            }
        }
    }

    if (index == 15)
    {
        usart_write_string(" R1 response was not found for CMD13");
        disable_cs_sdcard();
        return 0;
    }

    // If response was anything but good
    if (r1.raw_r1 != 0)
    {
        usart_write_string("R1 response was NOT 0\n");
        disable_cs_sdcard();
        return 0;
    }
    else if (r2.raw_r2 != 0)
    {
        usart_write_string("R2 response was NOT 0\n");
        disable_cs_sdcard();
        return 0;
    }

    disable_cs_sdcard();
    return 1;
}

uint8_t send_cmd17(uint8_t *buffer, uint32_t addr)
{
    // Send CMD17
    uint8_t rx_cmd17[20] = {0x00};
    uint8_t CMD17[20] = {0x51, addr >> 24, addr >> 16, addr >> 8, addr, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t index = 0;

    enable_cs_sdcard();

    transfer_spi2(CMD17, rx_cmd17, 20);

    for (index = 6; index < 20; index++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_cmd17[index];

        // Check R1 response for errors
        if (check_r1_response())
        {
            break;
        }
    }

    if (index == 20)
    {
        usart_write_string("Did not find valid R1 response for CMD 17\n");
    }

    // If response was anything but good
    if (r1.raw_r1 != 0)
    {
        usart_write_string("R1 response was NOT 0\n");
        disable_cs_sdcard();
        return 0;
    }

    // Start reading process
    if (!read_block(1, buffer))
    {
        usart_write_string("\n-- Micro Sd Card Reading Block Failure --\n");
        disable_cs_sdcard();
        return 0;
    }

    disable_cs_sdcard();

    uint8_t tx_byte[1] = {0xFF};
    uint8_t rx_byte[1] = {0x00};
    transfer_spi2(tx_byte, rx_byte, 1);

    return 1;
}

uint8_t send_cmd18(uint8_t *buffer, uint32_t addr, uint32_t count)
{
    // Send CMD17
    uint8_t rx_cmd18[20] = {0x00};
    uint8_t CMD18[20] = {0x52, addr >> 24, addr >> 16, addr >> 8, addr, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t index = 0;

    transfer_spi2(CMD18, rx_cmd18, 20);

    for (index = 6; index < 20; index++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_cmd18[index];

        // Check R1 response for errors
        if (check_r1_response())
        {
            break;
        }
    }

    if (index == 20)
    {
        usart_write_string("Did not find valid R1 response for CMD 18\n");
        disable_cs_sdcard();
        return 0;
    }

    // If response was anything but good
    if (r1.raw_r1 != 0)
    {
        usart_write_string("R1 response was NOT 0\n");
        disable_cs_sdcard();
        return 0;
    }

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

    disable_cs_sdcard();

    uint8_t tx_byte[1] = {0xFF};
    uint8_t rx_byte[1] = {0x00};
    transfer_spi2(tx_byte, rx_byte, 1);

    return 1;
}

uint8_t send_cmd24(const uint8_t *buffer, uint32_t addr)
{
    // Send CMD24
    uint8_t rx_cmd24[20] = {0x00};
    uint8_t CMD24[20] = {0x58, addr >> 24, addr >> 16, addr >> 8, addr, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t index = 0;

    enable_cs_sdcard();

    transfer_spi2(CMD24, rx_cmd24, 20);

    for (index = 6; index < 20; index++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_cmd24[index];

        // Check R1 response for errors
        if (check_r1_response())
        {
            break;
        }
    }

    if (index == 20)
    {
        usart_write_string("Did not find valid R1 response for CMD 24\n");
        disable_cs_sdcard();
        return 0;
    }

    // If response was anything but good
    if (r1.raw_r1 != 0)
    {
        usart_write_string("R1 response was NOT 0\n");
        disable_cs_sdcard();
        return 0;
    }

    // Start writing process
    if (!write_block(1, buffer))
    {
        usart_write_string("\n-- Micro Sd Card Writing Block Failure --\n");
        disable_cs_sdcard();
        return 0;
    }

    disable_cs_sdcard();

    uint8_t tx_byte[1] = {0xFF};
    uint8_t rx_byte[1] = {0x00};
    transfer_spi2(tx_byte, rx_byte, 1);

    return 1;
}

uint8_t send_cmd25(const uint8_t *buffer, uint32_t addr, uint32_t count)
{
    // Send CMD25
    uint8_t rx_cmd25[20] = {0x00};
    uint8_t CMD25[20] = {0x59, addr >> 24, addr >> 16, addr >> 8, addr, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t index = 0;

    enable_cs_sdcard();

    transfer_spi2(CMD25, rx_cmd25, 20);

    for (uint8_t index = 6; index < 20; index++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_cmd25[index];

        // Check R1 response for errors
        if (check_r1_response())
        {
            break;
        }
    }

    if (index == 20)
    {
        usart_write_string("Did not find valid R1 response for CMD 25\n");
        disable_cs_sdcard();
        return 0;
    }

    // If response was anything but good
    if (r1.raw_r1 != 0)
    {
        usart_write_string("R1 response was NOT 0\n");
        disable_cs_sdcard();
        return 0;
    }

    // Start writing process
    if (!write_block(count, buffer))
    {
        usart_write_string("\n-- Micro Sd Card Writing Block Failure --\n");
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

    disable_cs_sdcard();

    uint8_t tx_byte[1] = {0xFF};
    uint8_t rx_byte[1] = {0x00};
    transfer_spi2(tx_byte, rx_byte, 1);

    return 1;
}

uint8_t send_cmd55(void)
{
    // Send CMD55 6 bytes + 1 byte for return
    uint8_t rx_cmd55[20] = {0x00};
    uint8_t CMD55[20] = {0x77, 0x00, 0x00, 0x00, 0x00, 0x65, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Pull CS low
    // enable_cs_sdcard();

    transfer_spi2(CMD55, rx_cmd55, 20);

    // Pull CS high
    // disable_cs_sdcard();

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

uint8_t send_cmd58(void)
{
    // Send CMD58 6 bytes with 5 extra for return
    uint8_t rx_cmd58[20] = {0x00};
    uint8_t CMD58[20] = {0x7A, 0x00, 0x00, 0x00, 0x00, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Pull CS low
    enable_cs_sdcard();

    transfer_spi2(CMD58, rx_cmd58, 20);

    // Pull CS high
    disable_cs_sdcard();

    // Response is R1 & R3 format
    for (uint8_t i = 6; i < 20; i++)
    {
        // Response is R1 format
        r1.raw_r1 = rx_cmd58[i];

        // Check R1 response for errors
        if (check_r1_response() && ((i + 4) < 20))
        {
            r3.raw_r3 = (rx_cmd58[i + 1] << 24) | (rx_cmd58[i + 2] << 16) | (rx_cmd58[i + 3] << 8) | rx_cmd58[i + 4];

            return 1;
        }
    }

    return 0;
}

uint8_t send_acmd41(void)
{
    // Send ACMD41 6 bytes with 1 extra for return
    uint8_t rx_acmd41[10] = {0x00};
    uint8_t ACMD41[10] = {0x69, 0x40, 0x00, 0x00, 0x00, 0x77, 0xFF, 0xFF, 0xFF, 0xFF};

    // Pull CS low
    // enable_cs_sdcard();

    transfer_spi2(ACMD41, rx_acmd41, 10);

    // Pull CS high
    // disable_cs_sdcard();

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
