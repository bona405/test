/*
 * configure.h
 *
 *  Created on: 2019. 11. 19.
 *      Author: M031025
 */

#ifndef SRC_CONFIGURE_H_
#define SRC_CONFIGURE_H_
#include "xil_types.h"
#include "../../../common/src/definitions/definitions.h"

#ifdef WJ_DEBUG
#include "xil_printf.h"
//#define dbg_printf
#define dbg_printf xil_printf
#else
#define dbg_printf
#endif

#define DATA_BLOCK_SIZE 24
#define POINT8_MEMORY_SIZE		18


//#define G32_ES_TEST
#undef _DTALIGN


#define CPU1STARTADR 0xfffffff0
#define sev() __asm__("sev")

#define DEFAULT_IP_ADDRESS	"192.168.1.101"
#define DEFAULT_IP_MASK		"255.255.255.0"
#define DEFAULT_GW_ADDRESS	"192.168.1.1"

#if Packet_pre == 1
#if (M32 == 1)
#define M32_MULTI_STOPS 	1
#define M32_WIDE_ANGLE		1
#endif

#if G32 == 1
#define G32_MULTI_STOPS 	3
#define G32_WIDE_ANGLE		1
#endif

#define V_CHANNELS				32
#if(G32_MULTI_STOPS == 3 && G32_WIDE_ANGLE == 1)
#define H_POINTS			560
#define MULTI_STOPS		3
#elif(G32_MULTI_STOPS == 7 && G32_WIDE_ANGLE == 1)
#define H_POINTS			560
#define MULTI_STOPS		7
#endif

#elif Packet_new == 1
#if (M32 == 1)
#define M32_MULTI_STOPS 	1
#define M32_WIDE_ANGLE		1
#endif

#if G32 == 1
#define G32_MULTI_STOPS 	1			// DO NOT TOUCH
#define G32_WIDE_ANGLE		1			// DO NOT TOUCH
#endif

#define V_CHANNELS				32
#if(M32_MULTI_STOPS == 1 && M32_WIDE_ANGLE == 1)
//		#define H_POINTS			480//560
#define H_POINTS			536
#define MULTI_STOPS		3
#elif(G32_MULTI_STOPS == 1 && G32_WIDE_ANGLE == 1)
#define H_POINTS			536
#define MULTI_STOPS		3
#endif
#endif

volatile static u32 DAC_BRAM_Address = 0x40300000;
volatile static u32 DAC_Write_Done_Address = 0x40000008;
volatile static u32 STOP_START_CNT_Address = 0x4000000C;
volatile static u32 oDTC_EN = 0x40000000;
volatile static u32 oSel_Stop_CNT = 0x40000004;
volatile static u32 Write_Done_Address = 0x40000010;
volatile static u32 oDetection_Offset = 0x4000001C; // 4 bit

/* Define a data memory space which is visible to both CPUs, the OCM is ideal
 * since it is the lowest latency memory which is visible to both ARM cores.
 */
#define SHARED_OCM_MEMORY_BASE   0xFFFF0000

#define SHARED_DDR_MEMORY_BASE   0xA000000
#define SHARED_DDR_MEMORY_SIZE   0x0FFFFFF
#define TEMP_DATA   0x0100000

/* Declare a value stored in OCM space which is visible to both CPUs. */
#define SEMAPHORE_VALUE      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE))
#define DATA_BLOCK_COUNT      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 4))
#define BOTTOM_DATA_BLOCK_COUNT      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 8))
#define TOP_DATA_BLOCK_COUNT      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 12))

#define HZ_VAL      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 16))
#define A2Z_VAL     (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 20))

#define IS_MOTOR_INIT     (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 24))

//#define MAX_DISTANCE 64000
//#define MAX_DISTANCE_M 250.0
#define DIV_256 0.00390625
#define ONE_SIDE_CHANNEL_COUNT 16
//#define ENABLE_MOTOR_TEST
#endif /* SRC_CONFIGURE_H_ */
