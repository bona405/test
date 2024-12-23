/*
 * dt_align.h
 *
 *  Created on: 2023. 3. 27.
 *      Author: AutoL
 */

#ifndef SRC_API_SW_ALGORITHMS_DT_ALIGN_H_
#define SRC_API_SW_ALGORITHMS_DT_ALIGN_H_

#include <xil_types.h>
#include "xtime_l.h"
#include "lwip/udp.h"
#include "../models/fov_data_set.h"
//#include "../../../core0/src/udp/udp_server.h"
#include "../container/hash_table.h"

char* dt_align_arr[MAX_TB_SIZE];
char* dt_align_tof_arr[MAX_TB_SIZE];
int current_vth_arr[16];
int pre_current_vth_arr[16];
char dt_align_udp_payload[1212];
char dt_align_tof_udp_payload[1212];
char is_vth_applied;
volatile struct Node* dt_align_hash_tb[MAX_TB_SIZE];
volatile char current_key[50];

XTime dt_align_start;
XTime dt_align_end;
uint64_t dt_elapsed_us;

typedef struct DtAlignInfo
{
	uint8_t is_align_started;
	uint32_t start_vth;
	uint32_t end_vth;
	uint32_t vth_interval;
	uint32_t check_interval;
	uint32_t current_channel;
	uint32_t vth_count_limit;
	uint32_t vth_zero_count[16];
	uint8_t is_vth_check_finished[16];

}DtAlignInfo;
typedef struct DtAlignResult
{
	uint32_t vth[16];
	uint16_t distance[16];
}DtAlignResult;

struct DtAlignInfo dt_align_info_;

void InitDtAlignInfo(DtAlignInfo* dt_align_info, LidarState* lidar_state);
uint8_t DtCheckVth(DtAlignInfo* dt_align_info, int* pCurVthOff);
void SetDtCurVthArr(int channel, int* pCurVthOff, int* pCurSingleVthOff);
void SetChangeVth(int channel, int vth, int* pCurVthOff, int* pCurSingleVthOff);
char DtVthApplied();
int DtAlignValue(DtAlignInfo* dt_align_info, int* pCurVthOff, int* pCurSingleVthOff);
//void GetDtAlignResult(RawDataBlock* dt_align_result);
void ConvertDtAlignResultToBytes(char* dt_align_result_bytes);
void ConvertDtAlignTofToBytes(char* dt_align_result_bytes, volatile RawDataBlock* dt_align_block_data, DtAlignInfo* dt_align_info);
void DtCheckDistance(RawDataBlock* dt_align_result, DtAlignInfo* dt_align_info);

#endif /* SRC_API_SW_ALGORITHMS_DT_ALIGN_H_ */
