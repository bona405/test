/*
 * calibration.h
 *
 *  Created on: 2022. 7. 18.
 *      Author: HP
 */

#ifndef __CALIBRATION_H_
#define __CALIBRATION_H_
#include "stdlib.h"
#include <stdio.h>
#include "xil_types.h"

#include "../flash/flash.h"
#include <xstatus.h>
#include "xil_printf.h"
#include <xil_io.h>
#include "../../definitions/definitions.h"

#include "../models/machine_info.h"
#include "../models/fov_data_set.h"
int clip_min_temp;
int initial;
uint8_t Detection_value;
int cal_detection_done;
int cal_detection_range_result;

int caltest(int cal_val[], int tofs_32[]);
int routing_calibration(int cal_val[], ConvertedDataBlock* block_data_bram_noise, u32 fov_data_block_count);
int routing_calibration2(int cal_val[], ConvertedDataBlock* block_data_bram_noise, u32 fov_data_block_count);
//int routing_cal_ave(int cal_val[], ConvertedDataBlock* block_data_bram_noise, u32 fov_data_block_count);
int routing_calibration_distanceFix(int cal_val[], ConvertedDataBlock* block_data_bram_noise, u32 fov_data_block_count, int real_distance);
int routing_calibration_long_distanceFix(int cal_val[], int ave_val[], int total_ave);
int routing_add(int cal_val[], int add_delay);
int routing_add2(int cal_val[], int add_delay);
int routing_reset(int cal_val[]);
int temp_routing_valid();
int CalibrateMinimumDetectionRange(volatile RawDataBlock *block_data_shared, int* clip_min);
int Minimum_Detection_Cal(volatile RawDataBlock *block_data_shared, u32 fov_data_block_count);
#endif /* __CALIBRATION_H_ */
