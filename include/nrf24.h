/*
 * file: nrf24.h
 * description: header file for NRF24L01
 * author: Ryan Wagner
 * date: September 22, 2026
 * notes:
 */

#ifndef NRF24_H
#define NRF24_H

#include <stdint.h>
#include "spi3.h"
#include "usart.h"

// Commands
// #define R_REGISTER 0x00
#define W_REGISTER 0x20
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define REUSE_TX_PL 0xE3
#define R_RX_PL_WID 0x60
#define W_ACK_PAYLOAD 0xA8
#define W_TX_PAYLOAD_NO_ACK 0xB0
#define NOP 0xFF

// Registers
#define CONFIG 0x00
#define EN_AA 0x01
#define EN_RXADDR 0x02
#define SETUP_AW 0x03
#define SETUP_RETR 0x04
#define RF_CH 0x05
#define RF_SETUP 0x06
#define STATUS 0x07
#define OBSERVE_TX 0x08
#define CD 0x09
#define RX_ADDR_P0 0x0A
#define RX_ADDR_P1 0x0B
#define RX_ADDR_P2 0x0C
#define RX_ADDR_P3 0x0D
#define RX_ADDR_P4 0x0E
#define RX_ADDR_P5 0x0F
#define TX_ADDR 0x10
#define RX_PW_P0 0x11
#define RX_PW_P1 0x12
#define RX_PW_P2 0x13
#define RX_PW_P3 0x14
#define RX_PW_P4 0x15
#define RX_PW_P5 0x16
#define FIFO_STATUS 0x17
#define DYNPD 0x1C
#define FEATURE 0x1D

// Constants for setting radio registers
// Can be 3, 4, or 5 but needs to be consistant with the init function
#define ADDRESS_WIDTH 5

struct NRF24_STATUS_DATA
{
    uint8_t RX_DR, TX_DS, MAX_RT, TX_FULL, TX_EMPTY, RX_EMPTY, RX_FULL, TX_REUSE, TX_FULL_F, RX_P_NO, ARC_CNT, PLOS_CNT, STATUS_BYTE, FIFO_STATUS_BYTE;
};

// Packet structure from controller: 12 bytes
#pragma pack(push, 1)
typedef struct
{
    uint8_t packet_id;
    uint8_t flags;
    int16_t throttle;
    int16_t roll;
    int16_t pitch;
    int16_t yaw;    // LB & RB values
    uint8_t button; // bits: A, B, X, Y
    uint8_t checksum;
} RadioPacket;
#pragma pack(pop)

enum PIPE_PACKET_SIZE
{
    P0_PACKET_SIZE = 12,
    P1_PACKET_SIZE = 32,
    P2_PACKET_SIZE = 32,
    P3_PACKET_SIZE = 32,
    P4_PACKET_SIZE = 32,
    P5_PACKET_SIZE = 32
};

uint8_t init_radio(uint8_t channel);
void print_radio_settings(void);
uint8_t set_tx_mode(void);
uint8_t set_rx_mode(void);
void transmit_radio(uint8_t *data, uint8_t length);
uint8_t data_available(void);
uint8_t tx_FIFO_full(void);
void read_radio(RadioPacket *packet, enum PIPE_PACKET_SIZE pps);
void print_packet(RadioPacket packet);

#endif // NRF24_H