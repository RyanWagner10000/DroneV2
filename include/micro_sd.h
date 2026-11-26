/*
 * file: micro_sd.h
 * description: header file for micro sd card reader
 * author: Ryan Wagner
 * date: November 6, 2026
 * notes:
 */

#ifndef MICRO_SD_H
#define MICRO_SD_H

#include <stdint.h>
#include <string.h>
#include "spi2.h"
#include "timer10.h"
#include "usart.h"

typedef union
{
    uint8_t raw_r1;
    struct
    {
        uint8_t idle_state : 1;
        uint8_t erase_reset : 1;
        uint8_t illegal_cmd : 1;
        uint8_t com_crc_error : 1;
        uint8_t erase_seq_error : 1;
        uint8_t addr_error : 1;
        uint8_t param_error : 1;
        uint8_t resv : 1;
    };
} R1_return;

typedef union
{
    uint8_t raw_r2;
    struct
    {
        uint8_t locked : 1;
        uint8_t erase_lock_fail : 1;
        uint8_t error : 1;
        uint8_t cc_error : 1;
        uint8_t ecc_fail : 1;
        uint8_t wp_violation : 1;
        uint8_t erase_param : 1;
        uint8_t out_of_range : 1;
    };
} R2_return;

typedef union
{
    uint32_t raw_r3;
    struct
    {
        uint16_t reserved0 : 15;
        uint8_t _27_28 : 1;
        uint8_t _28_29 : 1;
        uint8_t _29_30 : 1;
        uint8_t _30_31 : 1;
        uint8_t _31_32 : 1;
        uint8_t _32_33 : 1;
        uint8_t _33_34 : 1;
        uint8_t _34_35 : 1;
        uint8_t _35_36 : 1;
        uint8_t _18_switch : 1;
        uint8_t reserved1 : 4;
        uint8_t uhs_2_card_status : 1;
        uint8_t card_capacity_status : 1;
        uint8_t card_power_up_status : 1;
    };
} R3_return;

typedef union
{
    uint32_t raw_r7;
    struct
    {
        uint32_t echo : 8;
        uint32_t volt_accpt : 4;
        uint32_t rsvd : 16;
        uint32_t cmd_ver : 4;
    };
} R7_return;

uint8_t wait_till(uint8_t value);
uint8_t send_cmd0(void);
uint8_t send_cmd8(void);
uint8_t send_cmd12(void);
uint8_t send_cmd13(void);
uint8_t send_cmd17(uint8_t *buffer, uint32_t addr);
uint8_t send_cmd18(uint8_t *buffer, uint32_t addr, uint32_t count);
uint8_t send_cmd24(const uint8_t *buffer, uint32_t addr);
uint8_t send_cmd25(const uint8_t *buffer, uint32_t addr, uint32_t count);
uint8_t send_cmd55(void);
uint8_t send_cmd58(void);
uint8_t send_acmd41(void);

#endif // MICRO_SD_H