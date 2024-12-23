/*
 * removeVnoise.h
 *
 *  Created on: 2022. 7. 21.
 *      Author: HP
 */

#ifndef __REMOVEVNOISE_H_
#define __REMOVEVNOISE_H_

#include "stdlib.h"
#include <stdio.h>
#include "xil_types.h"
#include "../../definitions/definitions.h"
#include "../flash/flash.h"
#include <xstatus.h>
#include "xil_printf.h"
#include <math.h>
#include "../models/machine_info.h"
#include "../models/fov_data_set.h"

#define target_point_not_exist	3

#define GRP_SIZE				8

#if defined(G32_10HZ)
#define NUM_AZI					1152
#elif defined(G32_25HZ)
#define NUM_AZI					720
#endif

#define MAPPING

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef struct
{
	unsigned int azimuth_index;
	unsigned int stop_index;
} PointInfo;

typedef struct
{
	PointInfo point_info_vector_[NUM_AZI];
	unsigned int point_not_exist_count_;
	unsigned int point_info_vec_cot;
} cloud_info;
typedef struct
{
	unsigned int azimuth_index_list_size;
	unsigned int channel_index;
	unsigned int istop;
} Vnoise_factor;
int ApplyVNoiseFilter_new(ConvertedDataBlock* block_data_bram_noise, volatile RawDataBlock* block_data_total_test);

int ApplyVNoiseFilter(volatile ConvertedDataBlock* block_data_bram_noise, volatile RawDataBlock* block_data_total_test, int AzimuthCount,
		int MOTOR_CAL_VAL_BOTTOM);
void RemoveVNoise(ConvertedDataBlock* block_data_bram_noise, volatile RawDataBlock* block_data_total_test,
		PointInfo azimuth_index_list[], int azimuth_index_list_size, int channel_index, int istop);

void RemoveVNoise_new(ConvertedDataBlock* block_data_bram_noise, volatile RawDataBlock* block_data_total_test,
		PointInfo azimuth_index_list[], Vnoise_factor *vnoise_factor);
void InitVNoiseTrigFunctionMap();

#define TRIG_FUNCTION_ARRAY_SIZE 3600

double sin_approximation_value_map[TRIG_FUNCTION_ARRAY_SIZE];
double cos_approximation_value_map[TRIG_FUNCTION_ARRAY_SIZE];
double tan_approximation_value_map[TRIG_FUNCTION_ARRAY_SIZE];
double atan_approximation_value_map[TRIG_FUNCTION_ARRAY_SIZE];
double atan2_approximation_value_map[TRIG_FUNCTION_ARRAY_SIZE];
uint8_t is_vnoise_trig_function_map_initialized;

#endif
