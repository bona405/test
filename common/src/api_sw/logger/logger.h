/*
 * log_controller.h
 *
 *  Created on: 2022. 12. 2.
 *      Author: Wonju
 */

#ifndef SRC_CONTROLLER_LOGGER_H_
#define SRC_CONTROLLER_LOGGER_H_

#include <xil_printf.h>
#include <xstatus.h>
#include "../flash/flash.h"

//#define FLASH_MEMORY_ADDR_LOG 0x0000000 // need to move to flash.h and modify name of FLASH_ADDR like FIRST_BOOT_ADDR or LIDAR_STATE_ADDR -> FLASH_MEMORY_ADDR_FIRST_BOOT, FLASH_MEMORY_ADDR_LIDAR_STATE
//#define FLASH_MEMORY_LOG_INFO_ADDR 0xC10000
//#define FLASH_MEMORY_LOG_ADDR 0xC10000 + SUBSECTOR_4K_SIZE
#define LOG_ADDR_INTERVAL 0x100

#ifdef _DEBUG
#define log_printf xil_printf
#define log
#else
#define log_printf xil_printf
#endif

typedef enum eLogLevel
{
	kTrace = 0,
	kDebug = 1,
	kInfo = 2,
	kWarn = 3,
	kError = 4,
	kCritical = 5,
	kOff = 6,
	n_levels
}eLogLevel;

typedef enum eLogType
{
	kConsole = 0,
	kFlash = 1,
	kAssertion = 2,
	n_types
}eLogType;

typedef struct LoggerInfo
{
	union
	{
		struct
		{
			size_t current_log_addr_offset;
		};
		unsigned char padding[SUBSECTOR_4K_SIZE];
	};
}LoggerInfo;

typedef struct LogMessageInfo
{
	u64 time_stamp;
	char log_message[LOG_ADDR_INTERVAL - 8]; // 8 == time stamp
}LogMessageInfo;

void log_trace(const char8 *ctrl1, ...);
void log_debug(const char8 *ctrl1, ...);
void log_info(const char8 *ctrl1, ...);
void log_warn(const char8 *ctrl1, ...);
void log_error(const char8 *ctrl1, ...);
void log_critical(const char8 *ctrl1, ...);
void log_off(const char8 *ctrl1, ...);
char* convert_ctrls_to_bytes(const char8 *ctrl1, ...);
void read_logs(char* result, size_t num);
#endif /* SRC_CONTROLLER_LOGGER_H_ */
