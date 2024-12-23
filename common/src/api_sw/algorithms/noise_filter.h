#ifndef __NOISE_FILTER_H_
#define __NOISE_FILTER_H_

#include <stdio.h>
#include "../../definitions/definitions.h"
#include "../container/common.h"
#include "../models/fov_data_set.h"
#include "../models/machine_info.h"

#include "xil_types.h"

//Noise Filter Definition
#define MAX_NOISE_CNT	4500
#define MIN_NOISE_CNT	1500
#define MAX_VTH_OFFSET	4
//Noise Filter Definition

volatile static u32 B_ADDR = 0x40300000;
volatile static u32 DAC_Write_Done_Addr = 0x40000008;
int ApplyNoiseFilter(volatile RawDataBlock* block_data_bram_noise, volatile RawDataBlock* block_data_total_test, int AzimuthCount, int *noise_point_cnt, int *frame_point_cnt);
int ApplyNoiseFilter2(TupleII noise_point_index[10000*CHANNEL_SIZE], volatile RawDataBlock* block_data_total_test, int AzimuthCount, unsigned int *noise_point_cnt, unsigned int *frame_point_cnt);
void ApplyVoltageThresholdFilter(int vth_sensitivity, float noise_percent, int noise_min_value, int noise_max_value, int *current_vth_offset,
		int noise_point_cnt, int noise_reference_number, int noise_percent_logic_flag, struct LidarState* lidar_state);
void InitVthFilter(struct LidarState* lidar_state);

struct NoiseCountInfo
{
	int noise_count;
	int vth;
	float average;
	int average_count;
	int max_count;
};

struct NoiseCountInfo noise_count_info_arr[3];

void ResetVthFilterParameter();
int pre_noise_reference_number;
int noise_init_cnt;
int pre_vth;
TupleII noise_point_index[10000*CHANNEL_SIZE];
float noise_reference_value_0;
float noise_reference_value_1;
float noise_reference_value_2;
float noise_reference_value_3;
int noise_min_value;
int noise_max_value;
unsigned int noise_point_cnt, frame_point_cnt;
int noise_determined_number;
int noise_reference_number;
int noise_percent_logic_flag;
int tunnel_count;
int vth_sensitivity;
float noise_percent;
int frame_cnt;
int frame_cnt_value_for_noise;
int noise_filter_max_distance_;

#define MAX_NEIGHBOR_POINT_SIZE_FOR_CLOSE_RANGE 12
void SetNeighborPointIndexForCloseRange();

TupleII frame_neighbor_point_index_list[10000][ONE_SIDE_CHANNEL_COUNT][MAX_NEIGHBOR_POINT_SIZE_FOR_CLOSE_RANGE];
TupleII frame_neighbor_point_indicies[MAX_NEIGHBOR_POINT_SIZE_FOR_CLOSE_RANGE];
size_t neighbor_point_indicies_size;
#endif
