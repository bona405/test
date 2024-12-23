/*
 * main.h
 *
 *  Created on: 2022. 7. 29.
 *      Author: Wonju
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_


#include <stdio.h>
#include "xparameters.h"
#include "lwipopts.h"
#include "lwip/inet.h"
#include "lwip/priv/tcp_priv.h"
#include "xil_printf.h"
#include "xil_io.h"
#include "xil_mmu.h"
#include "xil_cache.h"
#include "xuartps.h"	// if PS uart is used
#include "xscutimer.h"  // if PS Timer is used
#include "xscugic.h" 	// if PS GIC is used
#include "xil_types.h"
#include "xil_exception.h"	// if interrupt is used
#include "xtime_l.h"
#include "xpseudo_asm.h"
#include "xscugic.h"
#include "xaxidma.h"

#include "./udp/udp_server.h"
#include "./tcp/tcp_server.h"
#include "./definitions/definitions.h"
#include "./setup/init_lidar.h"
#include "./web/web_server.h"
#include "calibration/calibration.h"

//#include "../../../common/src/api_sw/platform/platform.h"
//#include "../../../common/src/api_sw/platform/platform_config.h"
#include "api_hw/api_hw.h"
#include "api_sw/flash/flash.h"
#include "api_sw/models/machine_info.h"
#include "api_sw/models/fov_data_set.h"
#include "api_sw/algorithms/calc_bin_to_dis.h"
#include "api_sw/algorithms/dtc.h"
#include "api_sw/algorithms/noise_filter.h"
#include "api_sw/perf_test/perf_test.h"
#include "api_sw/algorithms/remove_ghost.h"
#include "api_sw/algorithms/removeVnoise.h"
#include "api_sw/algorithms/remove_noise.h"
#include "api_sw/flash/flash.h"
#include "api_sw/algorithms/dt_align.h"
#include "api_sw//algorithms/routing_delay_calibration.h"
#include "api_sw/algorithms/fov_noise.h"
#include "api_sw/algorithms/dust_gas_filter.h"
#include "api_sw/algorithms/exhaust_filter.h"
#include "api_sw/algorithms/dark_area_error_correction.h"
#include "api_sw/algorithms/intensity_correction.h"
#include "api_sw/test/motor_test.h"

#include "xgpiops.h"
#include "xsysmon.h"

#if LWIP_IPV6==1
#include "lwip/ip6_addr.h"
#include "lwip/ip6.h"
#else
#if LWIP_DHCP==1
#include "lwip/dhcp.h"
extern volatile int dhcp_timoutcntr;
#endif
#endif /* LWIP_IPV6 */

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
extern int pass_frame;
extern int stop_option;
#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || \
		 XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
int ProgramSi5324(void);
int ProgramSfpPhy(void);
#endif
#endif

#ifdef XPS_BOARD_ZCU102
#ifdef XPAR_XIICPS_0_DEVICE_ID
int IicPhyReset(void);
#endif
#endif

struct netif server_netif;
u8 is_tcp_connected;

u32 data_block_count = 0;
enum eProtocol protocol = udp;
RawDataBlock block_data_bram[MAX_DISTANCE_COUNT];
RawDataBlock *block_data_shared = (RawDataBlock*) SHARED_DDR_MEMORY_BASE;
RawDataBlock block_data_list[MAX_DISTANCE_COUNT];
RawDataBlock last_block_data_list[MAX_DISTANCE_COUNT];
RawDataBlock temp_block_data_shared[MAX_DISTANCE_COUNT];
RawDataBlock current_raw_data_blocks[MAX_DISTANCE_COUNT];


const int temper_NB_table[165] = {
		65, 65, 65, 64, 64, 64, 63, 63, 63, 62,	//start -40
		62, 62, 61, 61, 61, 60, 60, 60, 59, 59,
		59, 58, 58, 58, 57, 57, 57, 56, 56, 56,
		55, 55, 55, 54, 54, 54, 53, 53, 53, 52, // -10
		52, 52, 51, 51, 51, 50, 50, 50, 49, 49, // 0
		49, 48, 48, 48, 46, 47, 47, 46, 46, 46,
		45, 45, 45, 44, 44, 44, 43, 43, 43, 42,
		42, 42, 41, 41, 41, 40, 40, 40, 39, 39, // 30
		39, 38, 38, 38, 37, 37, 36, 36, 35, 35,
		34, 33, 33, 32, 31, 31, 30, 29, 29, 28,
		27, 27, 26, 25, 25, 24, 23, 23, 22, 21, // 60
		21, 20, 19, 19, 18, 17, 17, 16, 15, 15,
		14, 13, 13, 12, 11, 11, 10,  9,  9,  8, // 80
		 7,  7,  6,  5,  5,  4,  3,  3,  2,  1,
		 1,  0, -1, -1, -2, -3, -3, -4, -5, -5, // 100
		-6, -7, -7, -8, -9, -9,-10,-11,-11, -12, //110
		-13, -13, -14, -15, -15					//124 end
		};


const int temperadj_table[] = {	//54��c
		  12,   12,   12,   12,   12,   12,   12,   12,   12,   12,		//0
		  12,   12,   12,   12,   12,   12,   12,   12,   11,   11,		//10
		  11,   11,   11,   11,   10,   10,   10,   10,   10,    9,		//20
		   9,    9,    9,    8,    8,    8,    7,    7,    7,    6,		//30
		   6,    6,    5,    5,    5,    4,    4,    3,    3,    2,		//40
		   2,    1,    1,    0,    0,   -1,   -1,   -2,   -2,   -3,		//50
		  -3,   -4,   -4,   -5,   -6,   -6,   -7,   -7,   -8,   -9,		//60
		  -9,  -10,  -11,  -11,  -12,  -13,  -14,  -14,  -15,  -16,		//70
		 -17,  -17,  -18,  -19,  -20,  -20,  -21,  -22,  -23,  -24,		//80
		 -25,  -26,  -26,  -27,  -28,  -29,  -30,  -31,  -32,  -33,		//90
		 -34,  -35,  -36,  -37,  -38,  -39,  -40,  -41,  -42,  -43,		//100
		 -44,  -45,  -46,  -47,  -49,  -50,  -51,  -52,  -53,  -54,		//110
		 -55															//120
};


ConvertedDataBlock current_frame_blocks[MAX_DISTANCE_COUNT];
ConvertedDataBlock last_frame_blocks[MAX_DISTANCE_COUNT];
uint8_t intensity_arr[MAX_DISTANCE_COUNT][CHANNEL_SIZE];

#if defined(_DTALIGN)
int dt_align_ch = 0;
int dt_align_value = 0;
int dt_cur_vth_off = 60;
int dt_curt_single_vth_off = 60;
#endif

#endif /* SRC_MAIN_H_ */


