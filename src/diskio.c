/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2025        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"		/* Basic definitions of FatFs */
#include "diskio.h" /* Declarations FatFs MAI */

/* Example: Declarations of the platform and disk functions in the project */
#include "micro_sd.h"

/* Example: Mapping of physical drive number for each drive */
#define DEV_FLASH 0 /* Map FTL to physical drive 0 */
#define DEV_MMC 1	/* Map MMC/SD card to physical drive 1 */
#define DEV_USB 2	/* Map USB MSD to physical drive 2 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

uint8_t disk_status(void)
{
	if (!send_cmd13())
	{
		usart_write_string("\n-- Micro Sd Card [CMD13] failure --\n\n");
		return RES_ERROR;
	}

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(void)
{
	// Disable SPI
	SPI2->CR1 &= ~(1U << 6);

	// Drop the SPI bus frequency to 100-400 kHz
	// Set clock to fPCLK/128 = 48MHz/128 = 375 kHz
	SPI2->CR1 &= ~(7U << 3);
	// SPI2->CR1 |= (6U << 3);
	SPI2->CR1 |= (7U << 3);

	// Re-enable SPI
	SPI2->CR1 |= (1U << 6);

	delay_millisecond(1);

	// With CS high, clock out at least 74 cycles
	uint8_t tx_clock_out[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t rx_clock_out[10] = {0x00};
	transfer_spi2(tx_clock_out, rx_clock_out, 10);

	// Send CMD0
	if (!send_cmd0())
	{
		usart_write_string("\n-- Micro Sd Card [CMD0] failure --\n\n");
		return RES_ERROR;
	}

	// Send CMD8 with voltage information
	if (!send_cmd8())
	{
		usart_write_string("\n-- Micro Sd Card [CMD8] failure --\n\n");
		return RES_ERROR;
	}

	// Send ACMD41 until no longer IDLE
	uint8_t break_out = 100;
	
	// Pull CS low
    enable_cs_sdcard();

	while (break_out--)
	{
		// Send CMD55 to let the card know the next command is an app command
		if (!send_cmd55())
		{
			usart_write_string("\n-- Micro Sd Card [CMD55] failure --\n\n");
			return RES_ERROR;
		}

		// Keep sending app command until it makes idle 0
		if (send_acmd41())
		{
			break;
		}

		delay_millisecond(1);
	}

	// Pull CS high
    disable_cs_sdcard();

	if (break_out == 255)
	{
		usart_write_string("\n-- Micro Sd Card Failure - Break Out Limit Reached --\n\n");
		return RES_ERROR;
	}

	// Send CMD58
	if (!send_cmd58())
	{
		usart_write_string("\n-- Micro Sd Card [CMD58] failure --\n\n");
		return RES_ERROR;
	}

	// Return SPI2 back to former operating frequency
	// Disable SPI
	SPI2->CR1 &= ~(1U << 6);

	// Set clock to fPCLK/8 = 48MHz/8 = 6MHz
	SPI2->CR1 &= ~(7U << 3);
	SPI2->CR1 |= (2U << 3);

	// Re-enable SPI
	SPI2->CR1 |= (1U << 6);

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DSTATUS disk_read(
	BYTE *buff,	  /* Data buffer to store read data */
	LBA_t sector, /* Start sector in LBA */
	UINT count	  /* Number of sectors to read */
)
{
	// Send read command based on number of sectors read
	if (count == 1)
	{
		// Send CMD17 with sector address
		if (!send_cmd17((uint8_t *)buff, (uint32_t)sector))
		{
			usart_write_string("\n-- Micro Sd Card [CMD17] failure --\n");
			return RES_ERROR;
		}
	}
	else
	{
		// Send CMD18 with sector address
		if (!send_cmd18((uint8_t *)buff, (uint32_t)sector, (uint32_t)count))
		{
			usart_write_string("\n-- Micro Sd Card [CMD18] failure --\n");
			return RES_ERROR;
		}
	}

	return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DSTATUS disk_write(
	const BYTE *buff, /* Data to be written */
	LBA_t sector,	  /* Start sector in LBA */
	UINT count		  /* Number of sectors to write */
)
{
	// Send write command based on number of sectors to write
	if (count == 1)
	{
		// Send CMD24 with sector address
		if (!send_cmd24((uint8_t *)buff, (uint32_t)sector))
		{
			usart_write_string("\n-- Micro Sd Card [CMD24] failure --\n");
			return RES_ERROR;
		}
	}
	else
	{
		// Send CMD25 with sector address
		if (!send_cmd25((uint8_t *)buff, (uint32_t)sector, (uint32_t)count))
		{
			usart_write_string("\n-- Micro Sd Card [CMD25] failure --\n");
			return RES_ERROR;
		}
	}

	return RES_OK;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
// BYTE cmd,  /* Control code */
// void *buff /* Buffer to send/receive control data */
DSTATUS disk_ioctl()
{
	// Wait until card finishes internal write process (MISO goes high)
	if (busy_wait())
	{
		return RES_OK;
	}

	return RES_ERROR;
}
