/*
 * flash.c
 *
 *  Created on: 2018. 8. 18.
 *      Author: AutoL
 */

/******************************************************************************
 *
 * Copyright (C) 2010 - 2019 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/
/*****************************************************************************/
/**
 *
 * @file xqspips_flash_lqspi_example.c
 *
 *
 * This file contains a design example using the QSPI driver (XQspiPs) in
 * Linear QSPI mode with a serial FLASH device. The example writes to the flash
 * in QSPI mode and reads it back in Linear QSPI mode. This examples performs
 * some transfers in Auto mode and Manual start mode, to illustrate the modes
 * available. It is recommended to use Manual CS + Auto start for
 * best performance.
 * The hardware which this example runs on, must have a serial FLASH (Numonyx
 * N25Q, Winbond W25Q, Spansion S25FL, ISSI IS25WP) for it to run. This example
 * has been tested with the Numonyx Serial Flash (N25Q128) and IS25WP series
 * flash parts.
 *
 * @note
 *
 * None.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who Date     Changes
 * ----- --- -------- -----------------------------------------------
 * 1.00  sdm 11/25/10 First release
 * 2.00a	kka 22/08/12 Updated the example as XQspiPs_PolledTransfer API has
 *		     changed. Changed the prescalar to use divide by 8.
 *		     The user can change the prescalar to a maximum of
 *		     divide by 2 based on the reference clock in the
 *		     system.
 * 	 	     Set the Holdb_dr bit in the configuration register using
 *		     XQSPIPS_HOLD_B_DRIVE_OPTION. Setting this bit
 *		     drives the HOLD bit of the QSPI controller.
 *		     This is required for QSPI to be used in Non QSPI boot
 *		     mode else there needs to be an external pullup on this
 *		     line. See http://www.xilinx.com/support/answers/47596.htm
 * 2.01a sg  02/03/13 Created a function FlashReadID.
 *       ms  04/05/17 Modified Comment lines in functions to
 *                    recognize it as documentation block for doxygen
 *                    generation.
 * 3.5	tjs 07/16/18 Added support for low density ISSI flash parts.
 *		     Added FlashQuadEnable API to enable quad mode in flash.
 * 3.6   akm 04/15/19 Modified FlashQuadEnable, FlashWrie and FlashErase APIs,
 *                    to wait for the on going operation to complete before
 *                    performing the next opeartion.
 *</pre>
 *
 ******************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"	/* SDK generated parameters */
#include "xqspips.h"		/* QSPI device driver */
#include "xil_printf.h"
#include "flash.h"
/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define QSPI_DEVICE_ID		XPAR_XQSPIPS_0_DEVICE_ID

/*
 * The following constants define the commands which may be sent to the FLASH
 * device.
 */
#define WRITE_STATUS_CMD	0x01
#define WRITE_CMD		0x02
#define READ_CMD		0x03
#define WRITE_DISABLE_CMD	0x04
#define READ_STATUS_CMD		0x05
#define WRITE_ENABLE_CMD	0x06
#define FAST_READ_CMD		0x0B
#define DUAL_READ_CMD		0x3B
#define QUAD_READ_CMD		0x6B
#define BULK_ERASE_CMD		0xC7
#define	SEC_ERASE_CMD		0xD8
#define	SUBSEC_4KB_ERASE_CMD		0x20
#define	SUBSEC_32KB_ERASE_CMD		0x52
#define READ_ID			0x9F

/*
 * The following constants define the offsets within a FlashBuffer data
 * type for each kind of data.  Note that the read data offset is not the
 * same as the write data because the QSPI driver is designed to allow full
 * duplex transfers such that the number of bytes received is the number
 * sent and received.
 */
#define COMMAND_OFFSET		0 /* FLASH instruction */
#define ADDRESS_1_OFFSET	1 /* MSB byte of address to read or write */
#define ADDRESS_2_OFFSET	2 /* Middle byte of address to read or write */
#define ADDRESS_3_OFFSET	3 /* LSB byte of address to read or write */
#define DATA_OFFSET		4 /* Start of Data for Read/Write */
#define DUMMY_OFFSET		4 /* Dummy byte offset for fast, dual and quad
				   * reads
				   */
#define DUMMY_SIZE		1 /* Number of dummy bytes for fast, dual and
				   * quad reads
				   */
#define RD_ID_SIZE		4 /* Read ID command + 3 bytes ID response */
#define BULK_ERASE_SIZE		1 /* Bulk Erase command size */
#define SEC_ERASE_SIZE		4 /* Sector Erase command + Sector address */

/*
 * The following constants specify the extra bytes which are sent to the
 * FLASH on the QSPI interface, that are not data, but control information
 * which includes the command and address
 */
#define OVERHEAD_SIZE		4

/*
 * The following constants specify the page size, sector size, and number of
 * pages and sectors for the FLASH.  The page size specifies a max number of
 * bytes that can be written to the FLASH with a single transfer.
 */


/* Flash address to which data is ot be written.*/
#define FIRST_BOOT_ADDR			0x00000000
#define SECOND_BOOT_ADDR		0x00600000
#define FLASH_MEMORY_SIZE		0x01000000

//#define SECOND_BOOT_ADDR		0x40000
#define UNIQUE_VALUE		0x05
/*
 * The following constants specify the max amount of data and the size of the
 * the buffer required to hold the data and overhead to transfer the data to
 * and from the FLASH.
 */
#define MAX_DATA		(PAGE_COUNT * PAGE_SIZE)

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

void FlashErase(XQspiPs *QspiPtr, u32 Address, u32 ByteCount);

//void FlashWrite(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command);
void FlashWrite(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command, u8 *buf);

int FlashReadID(void);

void FlashQuadEnable(XQspiPs *QspiPtr);

//int LinearQspiFlash(XQspiPs *QspiInstancePtr, u16 QspiDeviceId);
int LinearQspiFlash(XQspiPs *QspiInstancePtr, u16 QspiDeviceId, char *buf, int len);

/************************** Variable Definitions *****************************/

/*
 * The instances to support the device drivers are global such that they
 * are initialized to zero each time the program runs. They could be local
 * but should at least be static so they are zeroed.
 */
static XQspiPs QspiInstance;

/*
 * The following variable allows a test value to be added to the values that
 * are written to the FLASH such that unique values can be generated to
 * guarantee the writes to the FLASH were successful
 */
int Test = 5;

/*
 * The following variables are used to read and write to the flash and they
 * are global to avoid having large buffers on the stack
 */
u8 ReadBuffer[5000000];
u8 WriteBuffer[PAGE_SIZE + DATA_OFFSET];

/*****************************************************************************/
/**
 *
 * Main function to call the QSPI Flash example.
 *
 * @param	None
 *
 * @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
 *
 * @note		None
 *
 ******************************************************************************/
//int main(void)
//{
//	int Status;
//
//	xil_printf("Linear QSPI FLASH Example Test \r\n");
//
//	/* Run the Qspi Interrupt example.*/
//	Status = LinearQspiFlashExample(&QspiInstance, QSPI_DEVICE_ID);
//	if (Status != XST_SUCCESS) {
//		xil_printf("Linear QSPI FLASH Example Test Failed\r\n");
//		return XST_FAILURE;
//	}
//
//	xil_printf("Successfully ran Linear QSPI FLASH Example Test\r\n");
//	return XST_SUCCESS;
//}
/*****************************************************************************/
/**
 *
 * The purpose of this function is to illustrate how to use the XQspiPs
 * device driver in Linear mode. This function writes data to the serial
 * FLASH in QSPI mode and reads data in Linear QSPI mode.
 *
 * @param	None.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note		None.
 *
 *****************************************************************************/
int QspiFlashWrite(char *buf, int len)
{
	int Status;
	xil_printf("Linear QSPI FLASH Example Test \r\n");

	/* Run the Qspi Interrupt .*/
	Status = LinearQspiFlash(&QspiInstance, QSPI_DEVICE_ID, buf, len);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Linear QSPI FLASH Example Test Failed\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int AlFlashRead(u8* read_buffer, u32 addr, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, addr, size);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int ReadLidarInfo(u8* read_buffer, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, LIDAR_STATE_ADDR, size);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int ReadLogInfo(u8* read_buffer, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, LOG_INFO_ADDR, size);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}


int ReadLidarInfo_Ch(u8* read_buffer, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, LIDAR_STATE_ADDR_CH, size);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int ReadLidarInfo_Ch2(u8* read_buffer, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, LIDAR_STATE_ADDR_CH2, size);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int ReadFlash(u8* read_buffer, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, FIRST_BOOT_ADDR, size);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int ReadFlashForGoldenImg(u8* read_buffer, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, SECOND_BOOT_ADDR, size);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int InitLinearQspiFlash()
{
	int Status;
	XQspiPs_Config *QspiConfig;

	/* Initialize the QSPI driver so that it's ready to use*/
	QspiConfig = XQspiPs_LookupConfig(QSPI_DEVICE_ID);
	if (NULL == QspiConfig)
	{
		return XST_FAILURE;
	}

	Status = XQspiPs_CfgInitialize(&QspiInstance, QspiConfig, QspiConfig->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/* Perform a self-test to check hardware build*/
	Status = XQspiPs_SelfTest(&QspiInstance);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/*
	 * Initialize the write buffer for a pattern to write to the FLASH
	 * and the read buffer to zero so it can be verified after the read, the
	 * test value that is added to the unique value allows the value to be
	 * changed in a debug environment to guarantee
	 */
	memset(ReadBuffer, 0x00, sizeof(ReadBuffer));

	/*
	 * Set Auto Start and Manual Chip select options and drive the
	 * HOLD_B high.
	 */
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);
//	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
//						     XQSPIPS_MANUAL_START_OPTION |
//						     XQSPIPS_HOLD_B_DRIVE_OPTION);

	return XST_SUCCESS;
}

int LinearQspiFlashErase(int len)
{

	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);
	/* Erase the flash.*/
	//FlashErase(QspiInstancePtr, FIRST_BOOT_ADDR, (page_cnt + 2) * PAGE_SIZE);
	int page_cnt = len / PAGE_SIZE;
	FlashErase(&QspiInstance, FIRST_BOOT_ADDR, (page_cnt + 2) * PAGE_SIZE);

	return XST_SUCCESS;
}

int LinearQspiFlashEraseForLidarInfo(int len)
{

	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);
	/* Erase the flash.*/
	//FlashErase(QspiInstancePtr, FIRST_BOOT_ADDR, (page_cnt + 2) * PAGE_SIZE);
	int page_cnt = len / PAGE_SIZE;
	FlashErase(&QspiInstance, LIDAR_STATE_ADDR, (page_cnt + 2) * PAGE_SIZE);

	return XST_SUCCESS;
}

int LinearQspiFlashEraseForMultiBoot(int len)
{

	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);
	/* Erase the flash.*/
	//FlashErase(QspiInstancePtr, FIRST_BOOT_ADDR, (page_cnt + 2) * PAGE_SIZE);
	int page_cnt = len / PAGE_SIZE;
	FlashErase(&QspiInstance, SECOND_BOOT_ADDR, (page_cnt + 2) * PAGE_SIZE);

	return XST_SUCCESS;
}

int LinearQspiFlashWrite(char *buf, int len)
{
	int Count;
	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + FIRST_BOOT_ADDR, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = len - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + FIRST_BOOT_ADDR, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}

int LinearQspiFlashWriteForGolden(char *buf, int len)
{
	int Count;
	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + SECOND_BOOT_ADDR, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = len - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + SECOND_BOOT_ADDR, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}

int LinearQspiFlashWriteForLidarInfo(char *buf, int len)
{
	int Count;
	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + LIDAR_STATE_ADDR, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = len - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + LIDAR_STATE_ADDR, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}

int LinearQspiFlashWriteForLidarInfo_Ch(char *buf, int len)
{
	int Count;
	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + LIDAR_STATE_ADDR_CH, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = len - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + LIDAR_STATE_ADDR_CH, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}

int LinearQspiFlashWriteForLidarInfo_Ch2(char *buf, int len)
{
	int Count;
	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + LIDAR_STATE_ADDR_CH2, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = len - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + LIDAR_STATE_ADDR_CH2, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}

int LinearQspiFlashEraseWriteForKey(unsigned char *buf)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, LIDAR_KEY_ADDR, SUBSECTOR_4K_SIZE);

	FlashWrite(&QspiInstance, LIDAR_KEY_ADDR, LIDAR_KEY_SIZE, WRITE_CMD, (u8*) (buf));

	return XST_SUCCESS;
}

int LinearQspiFlashEraseWriteForIV(unsigned char *buf)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, LIDAR_IV_ADDR, SUBSECTOR_4K_SIZE);

	FlashWrite(&QspiInstance, LIDAR_IV_ADDR, LIDAR_IV_SIZE, WRITE_CMD, (u8*) (buf));

	return XST_SUCCESS;
}

int LinearQspiFlashWriteForLidarChangeInfo(char *buf)
{
	//Do Test

//	volatile unsigned char * idx;
	int Status = InitLinearQspiFlash();
	volatile unsigned int idx;

	volatile unsigned char idxs[16] = {0x0};

	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	Status = XQspiPs_LqspiRead(&QspiInstance, idxs, LOG_CHANGE_IDX, 16);

	xil_printf("Read val = %d\r\n", idx);

	idxs[0] += 1;
	if(idxs[0] >= 50)
	{
		idxs[0] = 0;
	}
	idx = (unsigned int)idxs[0];

	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, LOG_CHANGE_IDX, 16);

	FlashWrite(&QspiInstance, LOG_CHANGE_IDX, 16, WRITE_CMD, idxs);


	LogInfo logbuffer[50];

	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
//	Status = XQspiPs_LqspiRead(&QspiInstance, ReadVer, LOG_CHANGE_ADDR + LOG_CHANGE_SIZE*idx, LOG_CHANGE_SIZE);
//	Status = XQspiPs_LqspiRead(&QspiInstance, logbuffer, LOG_CHANGE_ADDR + LOG_CHANGE_SIZE*idx, 32*50);
	Status = XQspiPs_LqspiRead(&QspiInstance, (u8*) logbuffer, LOG_CHANGE_ADDR, sizeof(logbuffer));
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	memcpy(&logbuffer[idx], buf, 32);

	Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);


	FlashErase(&QspiInstance, LOG_CHANGE_ADDR, sizeof(logbuffer));
//	PAGE_SIZE

	for(int i = 0; i<50; i++)
	{
		FlashWrite(&QspiInstance, LOG_CHANGE_ADDR+ LOG_CHANGE_SIZE*i, 32, WRITE_CMD, (u8*) (&logbuffer[i]));
		usleep(1);
	}

//	unsigned char ReadVer[32] = {0x0,};	//debug
//
//	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
//	XQSPIPS_HOLD_B_DRIVE_OPTION);
////	Status = XQspiPs_LqspiRead(&QspiInstance, ReadVer, LOG_CHANGE_ADDR + LOG_CHANGE_SIZE*idx, LOG_CHANGE_SIZE);
//	Status = XQspiPs_LqspiRead(&QspiInstance, ReadVer, LOG_CHANGE_ADDR + LOG_CHANGE_SIZE*idx, 32);
//	if (Status != XST_SUCCESS)
//	{
//		return XST_FAILURE;
//	}

	return XST_SUCCESS;
}

int ReadAzioffset(u8* read_buffer, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, AZI_OFFSET_ADDR, size);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int LinearQspiFlashWriteForAzioffset(char *buf, int len)
{
	int Count;
	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + AZI_OFFSET_ADDR, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = len - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + AZI_OFFSET_ADDR, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}

int LinearQspiFlashWriteForTemperoffset(char *buf, int len)
{
	int Count;
	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + TEMPER_OFFSET_ADDR, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = len - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + TEMPER_OFFSET_ADDR, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}

void EraseAzioffset()
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, AZI_OFFSET_ADDR, AZI_OFFSET_SIZE);
}

void EraseTemperoffset()
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, TEMPER_OFFSET_ADDR, TEMPER_OFFSET_SIZE);
}

int LinearQspiFlashReadForLidarChangeInfo(char *buf, int readidx)
{
	//Do Test

//	volatile unsigned char * idx;
	int Status = InitLinearQspiFlash();

	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	Status = XQspiPs_LqspiRead(&QspiInstance, buf, LOG_CHANGE_ADDR + LOG_CHANGE_SIZE*readidx, LOG_CHANGE_SIZE);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int ReadKey(u8* read_buffer)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, LIDAR_KEY_ADDR, LIDAR_KEY_SIZE);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int ReadIV(u8* read_buffer)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, LIDAR_IV_ADDR, LIDAR_IV_SIZE);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int ReadSMTFLAG(u8* read_buffer)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, SMT_ISINIT_ADDR, 16);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int LinearQspiFlashWriteForSMTFLAG(unsigned char *buf)
{
	FlashWrite(&QspiInstance, SMT_ISINIT_ADDR, 16, WRITE_CMD, (u8*) (buf));

	return XST_SUCCESS;
}

int LinearQspiFlashWriteForMultiBoot(char *buf, int len)
{
	int Count;
	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + SECOND_BOOT_ADDR, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = len - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + SECOND_BOOT_ADDR, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}

int LinearQspiFlash(XQspiPs *QspiInstancePtr, u16 QspiDeviceId, char *buf, int len)
{
	int Status;
	u8 UniqueValue;
	int Count;
	int Page;
	XQspiPs_Config *QspiConfig;

	/* Initialize the QSPI driver so that it's ready to use*/
	QspiConfig = XQspiPs_LookupConfig(QspiDeviceId);
	if (NULL == QspiConfig)
	{
		return XST_FAILURE;
	}

	Status = XQspiPs_CfgInitialize(QspiInstancePtr, QspiConfig, QspiConfig->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/* Perform a self-test to check hardware build*/
	Status = XQspiPs_SelfTest(QspiInstancePtr);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	/*
	 * Initialize the write buffer for a pattern to write to the FLASH
	 * and the read buffer to zero so it can be verified after the read, the
	 * test value that is added to the unique value allows the value to be
	 * changed in a debug environment to guarantee
	 */
	memset(ReadBuffer, 0x00, sizeof(ReadBuffer));

	/*
	 * Set Auto Start and Manual Chip select options and drive the
	 * HOLD_B high.
	 */
	XQspiPs_SetOptions(QspiInstancePtr, XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);

	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

//
//	/* Erase the flash.*/
//	//FlashErase(QspiInstancePtr, FIRST_BOOT_ADDR, (page_cnt + 2) * PAGE_SIZE);
	FlashErase(QspiInstancePtr, FIRST_BOOT_ADDR, 64 * 1024 * PAGE_SIZE);

	for (Count = 0; Count < page_cnt; Count++)
	{
		//WriteBuffer[DATA_OFFSET + Count] = (u8)(UniqueValue + Test);
		//memcpy(&WriteBuffer[DATA_OFFSET], buf, sizeof(WriteBuffer) - 4);
		FlashWrite(QspiInstancePtr, (Count * PAGE_SIZE) + FIRST_BOOT_ADDR, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}
	int temp_len = len - write_len;
	FlashWrite(QspiInstancePtr, (page_cnt * PAGE_SIZE) + FIRST_BOOT_ADDR, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	/* Read from the flash in LQSPI mode.*/
	XQspiPs_SetOptions(QspiInstancePtr, XQSPIPS_LQSPI_MODE_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);

	Status = XQspiPs_LqspiRead(QspiInstancePtr, ReadBuffer, FIRST_BOOT_ADDR, write_len);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * This function writes to the  serial FLASH connected to the QSPI interface.
 * All the data put into the buffer must be in the same page of the device with
 * page boundaries being on 256 byte boundaries.
 *
 * @param	QspiPtr is a pointer to the QSPI driver component to use.
 * @param	Address contains the address to write data to in the FLASH.
 * @param	ByteCount contains the number of bytes to write.
 * @param	Command is the command used to write data to the flash. QSPI
 *		device supports only Page Program command to write data to the
 *		flash.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
void FlashWrite(XQspiPs *QspiPtr, u32 Address, u32 ByteCount, u8 Command, u8 *buf)
{
	u8 WriteEnableCmd =
	{ WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] =
	{ READ_STATUS_CMD, 0 }; /* must send 2 bytes */
	u8 FlashStatus[2];

	/*
	 * Send the write enable command to the FLASH so that it can be
	 * written to, this needs to be sent as a seperate transfer before
	 * the write
	 */
	XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

	/*
	 * Setup the write command with the specified address and data for the
	 * FLASH
	 */
	WriteBuffer[COMMAND_OFFSET] = Command;
	WriteBuffer[ADDRESS_1_OFFSET] = (u8) ((Address & 0xFF0000) >> 16);
	WriteBuffer[ADDRESS_2_OFFSET] = (u8) ((Address & 0xFF00) >> 8);
	WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);

	memcpy(&WriteBuffer[DATA_OFFSET], buf, ByteCount);
	/*
	 * Send the write command, address, and data to the FLASH to be
	 * written, no receive buffer is specified since there is nothing to
	 * receive
	 */
	XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL, ByteCount + OVERHEAD_SIZE);

	/*
	 * Wait for the write command to the FLASH to be completed, it takes
	 * some time for the data to be written
	 */
	while (1)
	{
		/*
		 * Poll the status register of the FLASH to determine when it
		 * completes, by sending a read status command and receiving the
		 * status byte
		 */
		XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

		/*
		 * If the status indicates the write is done, then stop waiting,
		 * if a value of 0xFF in the status byte is read from the
		 * device and this loop never exits, the device slave select is
		 * possibly incorrect such that the device status is not being
		 * read
		 */
		FlashStatus[1] |= FlashStatus[0];
		if ((FlashStatus[1] & 0x01) == 0)
		{
			break;
		}
	}
}

/*****************************************************************************/
/**
 *
 * This function erases the sectors in the  serial FLASH connected to the
 * QSPI interface.
 *
 * @param	QspiPtr is a pointer to the QSPI driver component to use.
 * @param	Address contains the address of the first sector which needs to
 *		be erased.
 * @param	ByteCount contains the total size to be erased.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/

void EraseLidarInfo()
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, LIDAR_STATE_ADDR, SUBSECTOR_4K_SIZE);
//	int Count;
//	int write_len = 0;
//	int len = LIDAR_STATE_SIZE;
//	int page_cnt = len / PAGE_SIZE;
//	char buf[LIDAR_STATE_SIZE] = {0,};
//	for (Count = 0; Count < page_cnt; Count++)
//	{
//		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + LIDAR_STATE_ADDR, PAGE_SIZE, WRITE_CMD, (u8*)(buf + write_len));
//		write_len += PAGE_SIZE;
//	}

//	int temp_len = len - write_len;
//	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + LIDAR_STATE_ADDR, temp_len, WRITE_CMD, (u8*)(buf + write_len));
//	write_len += temp_len;

}

void EraseFlashMemory(u32 addr, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, addr, size);
}

int AlFlashWrite(char* buf, u32 addr, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION | XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Count;
	int write_len = 0;
	int page_cnt = size / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + addr, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = size - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + addr, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}


void EraseLidarInfo_Ch()
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, LIDAR_STATE_ADDR_CH, SUBSECTOR_4K_SIZE);
//	int Count;
//	int write_len = 0;
//	int len = LIDAR_STATE_SIZE;
//	int page_cnt = len / PAGE_SIZE;
//	char buf[LIDAR_STATE_SIZE] = {0,};
//	for (Count = 0; Count < page_cnt; Count++)
//	{
//		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + LIDAR_STATE_ADDR, PAGE_SIZE, WRITE_CMD, (u8*)(buf + write_len));
//		write_len += PAGE_SIZE;
//	}

//	int temp_len = len - write_len;
//	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + LIDAR_STATE_ADDR, temp_len, WRITE_CMD, (u8*)(buf + write_len));
//	write_len += temp_len;

}


void EraseLidarInfo_Ch2()
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, LIDAR_STATE_ADDR_CH2, SUBSECTOR_4K_SIZE);
//	int Count;
//	int write_len = 0;
//	int len = LIDAR_STATE_SIZE;
//	int page_cnt = len / PAGE_SIZE;
//	char buf[LIDAR_STATE_SIZE] = {0,};
//	for (Count = 0; Count < page_cnt; Count++)
//	{
//		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + LIDAR_STATE_ADDR, PAGE_SIZE, WRITE_CMD, (u8*)(buf + write_len));
//		write_len += PAGE_SIZE;
//	}

//	int temp_len = len - write_len;
//	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + LIDAR_STATE_ADDR, temp_len, WRITE_CMD, (u8*)(buf + write_len));
//	write_len += temp_len;

}


void EraseBootImgFlash()
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, FIRST_BOOT_ADDR, SECOND_BOOT_ADDR - 1);
}

void EraseGoldenImgFlash()
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

//	FlashErase(&QspiInstance, SECOND_BOOT_ADDR , SECOND_BOOT_ADDR * 2 - 1);
	FlashErase(&QspiInstance, SECOND_BOOT_ADDR , SECOND_BOOT_ADDR - 1);
}

void ResetFlashMemory()
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, FIRST_BOOT_ADDR, FLASH_MEMORY_SIZE - 1);
}

void FlashErase(XQspiPs *QspiPtr, u32 Address, u32 ByteCount)
{

	u8 WriteEnableCmd =
	{ WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] =
	{ READ_STATUS_CMD, 0 }; /* must send 2 bytes */
	u8 FlashStatus[2];
	int Sector;

	/*
	 * If erase size is same as the total size of the flash, use bulk erase
	 * command
	 */
	if (ByteCount == (NUM_SECTORS * SECTOR_SIZE))
	{
		/*
		 * Send the write enable command to the FLASH so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the erase
		 */
		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

		/* Setup the bulk erase command*/
		WriteBuffer[COMMAND_OFFSET] = BULK_ERASE_CMD;

		/*
		 * Send the bulk erase command; no receive buffer is specified
		 * since there is nothing to receive
		 */
		XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
		BULK_ERASE_SIZE);

		/* Wait for the erase command to the FLASH to be completed*/
		while (1)
		{
			/*
			 * Poll the status register of the device to determine
			 * when it completes, by sending a read status command
			 * and receiving the status byte
			 */
			XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

			/*
			 * If the status indicates the write is done, then stop
			 * waiting; if a value of 0xFF in the status byte is
			 * read from the device and this loop never exits, the
			 * device slave select is possibly incorrect such that
			 * the device status is not being read
			 */
			FlashStatus[1] |= FlashStatus[0];
			if ((FlashStatus[1] & 0x01) == 0)
			{
				break;
			}
		}

		return;
	}



	if(ByteCount < SUBSECTOR_32K_SIZE)
	{
		size_t sector_count = ByteCount % SUBSECTOR_4K_SIZE == 0 ? (ByteCount / SUBSECTOR_4K_SIZE) : ((ByteCount / SUBSECTOR_4K_SIZE) + 1);
		for (Sector = 0; Sector < sector_count; Sector++)
		{
			/*
			 * Send the write enable command to the SEEPOM so that it can be
			 * written to, this needs to be sent as a seperate transfer
			 * before the write
			 */
			XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

			/*
			 * Setup the write command with the specified address and data
			 * for the FLASH
			 */
			WriteBuffer[COMMAND_OFFSET] = SUBSEC_4KB_ERASE_CMD;
			WriteBuffer[ADDRESS_1_OFFSET] = (u8) (Address >> 16); // 192
			WriteBuffer[ADDRESS_2_OFFSET] = (u8) (Address >> 8);
			WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);

			/*
			 * Send the sector erase command and address; no receive buffer
			 * is specified since there is nothing to receive
			 */
			XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
			SEC_ERASE_SIZE);

			/*
			 * Wait for the sector erse command to the
			 * FLASH to be completed
			 */
			while (1)
			{
				/*
				 * Poll the status register of the device to determine
				 * when it completes, by sending a read status command
				 * and receiving the status byte
				 */
				XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

				/*
				 * If the status indicates the write is done, then stop
				 * waiting, if a value of 0xFF in the status byte is
				 * read from the device and this loop never exits, the
				 * device slave select is possibly incorrect such that
				 * the device status is not being read
				 */
				FlashStatus[1] |= FlashStatus[0];
				if ((FlashStatus[1] & 0x01) == 0)
				{
					break;
				}
			}
			Address += SUBSECTOR_4K_SIZE;
		}

		return;
	}

	if(ByteCount < SECTOR_SIZE)
	{
		size_t sector_count = ByteCount % SUBSECTOR_32K_SIZE == 0 ? (ByteCount / SUBSECTOR_32K_SIZE) : ((ByteCount / SUBSECTOR_32K_SIZE) + 1);
		for (Sector = 0; Sector < sector_count; Sector++)
		{
			/*
			 * Send the write enable command to the SEEPOM so that it can be
			 * written to, this needs to be sent as a seperate transfer
			 * before the write
			 */
			XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

			/*
			 * Setup the write command with the specified address and data
			 * for the FLASH
			 */
			WriteBuffer[COMMAND_OFFSET] = SUBSEC_32KB_ERASE_CMD;
			WriteBuffer[ADDRESS_1_OFFSET] = (u8) (Address >> 16); // 192
			WriteBuffer[ADDRESS_2_OFFSET] = (u8) (Address >> 8);
			WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);

			/*
			 * Send the sector erase command and address; no receive buffer
			 * is specified since there is nothing to receive
			 */
			XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
			SEC_ERASE_SIZE);

			/*
			 * Wait for the sector erse command to the
			 * FLASH to be completed
			 */
			while (1)
			{
				/*
				 * Poll the status register of the device to determine
				 * when it completes, by sending a read status command
				 * and receiving the status byte
				 */
				XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

				/*
				 * If the status indicates the write is done, then stop
				 * waiting, if a value of 0xFF in the status byte is
				 * read from the device and this loop never exits, the
				 * device slave select is possibly incorrect such that
				 * the device status is not being read
				 */
				FlashStatus[1] |= FlashStatus[0];
				if ((FlashStatus[1] & 0x01) == 0)
				{
					break;
				}
			}
			Address += SUBSECTOR_32K_SIZE;
		}

		return;
	}

	size_t sector_count = ByteCount % SECTOR_SIZE == 0 ? (ByteCount / SECTOR_SIZE) : ((ByteCount / SECTOR_SIZE) + 1);

	for (Sector = 0; Sector < sector_count; Sector++)
	{
		/*
		 * Send the write enable command to the SEEPOM so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the write
		 */
		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

		/*
		 * Setup the write command with the specified address and data
		 * for the FLASH
		 */
		WriteBuffer[COMMAND_OFFSET] = SEC_ERASE_CMD;
		WriteBuffer[ADDRESS_1_OFFSET] = (u8) (Address >> 16); // 192
		WriteBuffer[ADDRESS_2_OFFSET] = (u8) (Address >> 8);
		WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);

		/*
		 * Send the sector erase command and address; no receive buffer
		 * is specified since there is nothing to receive
		 */
		XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
		SEC_ERASE_SIZE);

		/*
		 * Wait for the sector erse command to the
		 * FLASH to be completed
		 */
		while (1)
		{
			/*
			 * Poll the status register of the device to determine
			 * when it completes, by sending a read status command
			 * and receiving the status byte
			 */
			XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

			/*
			 * If the status indicates the write is done, then stop
			 * waiting, if a value of 0xFF in the status byte is
			 * read from the device and this loop never exits, the
			 * device slave select is possibly incorrect such that
			 * the device status is not being read
			 */
			FlashStatus[1] |= FlashStatus[0];
			if ((FlashStatus[1] & 0x01) == 0)
			{
				break;
			}
		}
		Address += SECTOR_SIZE;
	}

	return;
}

void FlashSubSector4KErase(XQspiPs *QspiPtr, u32 Address, u32 ByteCount)
{

	u8 WriteEnableCmd =
	{ WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] =
	{ READ_STATUS_CMD, 0 }; /* must send 2 bytes */
	u8 FlashStatus[2];
	int Sector;

	size_t sector_count = ByteCount % SUBSECTOR_4K_SIZE == 0 ? (ByteCount / SUBSECTOR_4K_SIZE) : ((ByteCount / SUBSECTOR_4K_SIZE) + 1);

	for (Sector = 0; Sector < sector_count; Sector++)
	{
		/*
		 * Send the write enable command to the SEEPOM so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the write
		 */
		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

		/*
		 * Setup the write command with the specified address and data
		 * for the FLASH
		 */
		WriteBuffer[COMMAND_OFFSET] = SUBSEC_4KB_ERASE_CMD;
		WriteBuffer[ADDRESS_1_OFFSET] = (u8) (Address >> 16); // 192
		WriteBuffer[ADDRESS_2_OFFSET] = (u8) (Address >> 8);
		WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);

		/*
		 * Send the sector erase command and address; no receive buffer
		 * is specified since there is nothing to receive
		 */
		XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
		SEC_ERASE_SIZE);

		/*
		 * Wait for the sector erse command to the
		 * FLASH to be completed
		 */
		while (1)
		{
			/*
			 * Poll the status register of the device to determine
			 * when it completes, by sending a read status command
			 * and receiving the status byte
			 */
			XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

			/*
			 * If the status indicates the write is done, then stop
			 * waiting, if a value of 0xFF in the status byte is
			 * read from the device and this loop never exits, the
			 * device slave select is possibly incorrect such that
			 * the device status is not being read
			 */
			FlashStatus[1] |= FlashStatus[0];
			if ((FlashStatus[1] & 0x01) == 0)
			{
				break;
			}
		}
		Address += SUBSECTOR_4K_SIZE;
	}

}

void FlashSubSector32KErase(XQspiPs *QspiPtr, u32 Address, u32 ByteCount)
{

	u8 WriteEnableCmd =
	{ WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] =
	{ READ_STATUS_CMD, 0 }; /* must send 2 bytes */
	u8 FlashStatus[2];
	int Sector;

	size_t sector_count = ByteCount % SUBSECTOR_32K_SIZE == 0 ? (ByteCount / SUBSECTOR_32K_SIZE) : ((ByteCount / SUBSECTOR_32K_SIZE) + 1);

	for (Sector = 0; Sector < sector_count; Sector++)
	{
		/*
		 * Send the write enable command to the SEEPOM so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the write
		 */
		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

		/*
		 * Setup the write command with the specified address and data
		 * for the FLASH
		 */
		WriteBuffer[COMMAND_OFFSET] = SUBSEC_32KB_ERASE_CMD;
		WriteBuffer[ADDRESS_1_OFFSET] = (u8) (Address >> 16); // 192
		WriteBuffer[ADDRESS_2_OFFSET] = (u8) (Address >> 8);
		WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);

		/*
		 * Send the sector erase command and address; no receive buffer
		 * is specified since there is nothing to receive
		 */
		XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
		SEC_ERASE_SIZE);

		/*
		 * Wait for the sector erse command to the
		 * FLASH to be completed
		 */
		while (1)
		{
			/*
			 * Poll the status register of the device to determine
			 * when it completes, by sending a read status command
			 * and receiving the status byte
			 */
			XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

			/*
			 * If the status indicates the write is done, then stop
			 * waiting, if a value of 0xFF in the status byte is
			 * read from the device and this loop never exits, the
			 * device slave select is possibly incorrect such that
			 * the device status is not being read
			 */
			FlashStatus[1] |= FlashStatus[0];
			if ((FlashStatus[1] & 0x01) == 0)
			{
				break;
			}
		}
		Address += SUBSECTOR_32K_SIZE;
	}

}

void FlashSectorErase(XQspiPs *QspiPtr, u32 Address, u32 ByteCount)
{

	u8 WriteEnableCmd =
	{ WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] =
	{ READ_STATUS_CMD, 0 }; /* must send 2 bytes */
	u8 FlashStatus[2];
	int Sector;

	size_t sector_count = ByteCount % SECTOR_SIZE == 0 ? (ByteCount / SECTOR_SIZE) : ((ByteCount / SECTOR_SIZE) + 1);

	for (Sector = 0; Sector < sector_count; Sector++)
	{
		/*
		 * Send the write enable command to the SEEPOM so that it can be
		 * written to, this needs to be sent as a seperate transfer
		 * before the write
		 */
		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

		/*
		 * Setup the write command with the specified address and data
		 * for the FLASH
		 */
		WriteBuffer[COMMAND_OFFSET] = SEC_ERASE_CMD;
		WriteBuffer[ADDRESS_1_OFFSET] = (u8) (Address >> 16); // 192
		WriteBuffer[ADDRESS_2_OFFSET] = (u8) (Address >> 8);
		WriteBuffer[ADDRESS_3_OFFSET] = (u8) (Address & 0xFF);

		/*
		 * Send the sector erase command and address; no receive buffer
		 * is specified since there is nothing to receive
		 */
		XQspiPs_PolledTransfer(QspiPtr, WriteBuffer, NULL,
		SEC_ERASE_SIZE);

		/*
		 * Wait for the sector erse command to the
		 * FLASH to be completed
		 */
		while (1)
		{
			/*
			 * Poll the status register of the device to determine
			 * when it completes, by sending a read status command
			 * and receiving the status byte
			 */
			XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

			/*
			 * If the status indicates the write is done, then stop
			 * waiting, if a value of 0xFF in the status byte is
			 * read from the device and this loop never exits, the
			 * device slave select is possibly incorrect such that
			 * the device status is not being read
			 */
			FlashStatus[1] |= FlashStatus[0];
			if ((FlashStatus[1] & 0x01) == 0)
			{
				break;
			}
		}
		Address += SECTOR_SIZE;
	}

}

/*****************************************************************************/
/**
 *
 * This function reads serial FLASH ID connected to the SPI interface.
 *
 * @param	None.
 *
 * @return	XST_SUCCESS if read id, otherwise XST_FAILURE.
 *
 * @note		None.
 *
 ******************************************************************************/
int FlashReadID(void)
{
	int Status;

	/* Read ID in Auto mode.*/
	WriteBuffer[COMMAND_OFFSET] = READ_ID;
	WriteBuffer[ADDRESS_1_OFFSET] = 0x23; /* 3 dummy bytes */
	WriteBuffer[ADDRESS_2_OFFSET] = 0x08;
	WriteBuffer[ADDRESS_3_OFFSET] = 0x09;

	Status = XQspiPs_PolledTransfer(&QspiInstance, WriteBuffer, ReadBuffer,
	RD_ID_SIZE);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	xil_printf("FlashID=0x%x 0x%x 0x%x\n\r", ReadBuffer[1], ReadBuffer[2], ReadBuffer[3]);

	return XST_SUCCESS;
}

/**
 *
 * This function enables quad mode in the serial flash connected to the
 * SPI interface.
 *
 * @param	QspiPtr is a pointer to the QSPI driver component to use.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
void FlashQuadEnable(XQspiPs *QspiPtr)
{
	u8 WriteEnableCmd =
	{ WRITE_ENABLE_CMD };
	u8 ReadStatusCmd[] =
	{ READ_STATUS_CMD, 0 };
	u8 QuadEnableCmd[] =
	{ WRITE_STATUS_CMD, 0 };
	u8 FlashStatus[2];

	if (ReadBuffer[1] == 0x9D)
	{

		XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));

		QuadEnableCmd[1] = FlashStatus[1] | 1 << 6;

		XQspiPs_PolledTransfer(QspiPtr, &WriteEnableCmd, NULL, sizeof(WriteEnableCmd));

		XQspiPs_PolledTransfer(QspiPtr, QuadEnableCmd, NULL, sizeof(QuadEnableCmd));
		while (1)
		{
			/*
			 * Poll the status register of the FLASH to determine when
			 * Quad Mode is enabled and the device is ready, by sending
			 * a read status command and receiving the status byte
			 */
			XQspiPs_PolledTransfer(QspiPtr, ReadStatusCmd, FlashStatus, sizeof(ReadStatusCmd));
			/*
			 * If 6th bit is set & 0th bit is reset, then Quad is Enabled
			 * and device is ready.
			 */
			if ((FlashStatus[0] == 0x40) && (FlashStatus[1] == 0x40))
			{
				break;
			}
		}
	}
}






void EraseDarkAreaInfo()
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_FORCE_SSELECT_OPTION |
	XQSPIPS_MANUAL_START_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);

	FlashErase(&QspiInstance, DARK_AREA_ADDR, DARK_AREA_SIZE);
}
int LinearQspiFlashWriteForDarkAreaInfo(char *buf, int len)
{
	int Count;
	int write_len = 0;
	int page_cnt = len / PAGE_SIZE;

	for (Count = 0; Count < page_cnt; Count++)
	{
		FlashWrite(&QspiInstance, (Count * PAGE_SIZE) + DARK_AREA_ADDR, PAGE_SIZE, WRITE_CMD, (u8*) (buf + write_len));
		write_len += PAGE_SIZE;
	}

	int temp_len = len - write_len;
	FlashWrite(&QspiInstance, (page_cnt * PAGE_SIZE) + DARK_AREA_ADDR, temp_len, WRITE_CMD, (u8*) (buf + write_len));
	write_len += temp_len;

	return XST_SUCCESS;
}
int ReadDarkAreaInfo(u8* read_buffer, size_t size)
{
	XQspiPs_SetOptions(&QspiInstance, XQSPIPS_LQSPI_MODE_OPTION |
	XQSPIPS_HOLD_B_DRIVE_OPTION);
	int Status = XQspiPs_LqspiRead(&QspiInstance, read_buffer, DARK_AREA_ADDR, size);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

