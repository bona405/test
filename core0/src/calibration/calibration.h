/*
 * api_hw.h
 *
 *  Created on: 2022. 8. 2.
 *      Author: Wonju
 */

#ifndef SRC_CALIBRATION_CALIBRATION_H_
#define SRC_CALIBRATION_CALIBRATION_H_

#include <stdio.h>
#include "../definitions/definitions.h"
#include "../../../common/src/api_sw/models/machine_info.h"
#include "../../../common/src/api_sw/algorithms/ground_detection.h"
#include "../../../common/src/api_sw/algorithms/remove_ghost.h"
#include "../../../common/src/api_sw/algorithms/noise_filter.h"
#include "../../../common/src/api_sw/algorithms/remove_noise.h"
#include <xil_io.h>
#include "xil_types.h"
#include <xstatus.h>
#include "../../../common/src/api_hw/api_hw_enc_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_heat_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_ld_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_mcp4801_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_mcp4822_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_motor_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_tdc_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_version_info_ip.h"
#include "../../../common/src/api_hw/api_hw_xadcps_ip.h"
#include "../../../common/src/api_hw/api_hw_xbram_histo.h"
#include "../../../common/src/api_hw/api_hw_xbram_lut.h"

#define START_VTH 0
#define END_VTH 1
#define VTH_INTERVAL 2
#define CHK_INTERVAL 3

int current_vth_offset;
int AngleErr;
int routing_delay[32]; //SPF
int routing_delay2[32]; //SPF
float routing_range_1;
float routing_range_2;
int clip_min;
int max_distance;
int min_distance;
int lower_offset; // G32 A1 -5 /M32 A1 3 (1st sample ) / M32 A1 0 (1st sample )
int bottom_horizontal_offset;
int total_azimuth_offset;
//extern int debug_num;
extern int cal_flag;
extern int cal2_flag;
extern int cal_flag_long;
extern int fixed_cal_flag;
//extern int debug_num2;
extern int routing_add_flag;
extern int routing_add_flag2;

extern int routing_resetflag;
extern int adddelay;
extern int cal_distance;
volatile uint8_t is_bottom_horizontal_offset_changed;
int detection_range;
extern void instruction_command(int cal_val[32], int* clip_min, int* bd_min, int* bd_max, int* MOTOR_CAL_VAL_BOTTOM, int * current_vth_offset, int* noise_determined_value,
		float* noise_reference_value, int* vth_sensitivity, int* noise_reference_number, int* noise_percent_logic_flag);


void ChangeVth(int parameter1, int parameter2);
void ChangeRoutingDelay(int parameter1, int parameter2);
void ChangeDistanceOffset(int parameter1, int parameter2);
void ChangeEmPulse(int parameter1, int parameter2);
void ChangeDistanceMinLimit(int parameter1, int parameter2);
void ChangeDistanceMaxLimit(int parameter1, int parameter2);
void ChangeHorizontalAngleOffset(int parameter1, int parameter2);
void ChangeNoiseFilterSettings(int parameter1, int parameter2);
void ChangeMinimumDetectionRange(int prameter1, int parameter2);
void ChangeRemoveGhostSettings(int parameter1, int parameter2);

void InitChannelVth(struct LidarState_Ch* lidar_state_ch);
void ChangeRPM(int parameter1, int parameter2);
void ChangeSetMotorAccelerationTime(int parameter1, int parameter2);
void ChangeSetMotorAccelerationResolution(int parameter1, int parameter2);
void ChangeHistogramStartVth(int parameter1, int parameter2);
void ChangeStartEncoderCount(int parameter1, int parameter2);
void ChangeEndEncoderCount(int parameter1, int parameter2);
void ChangeHorizontalFovAngle(int parameter1, int parameter2);
void ChangeVerticalFovAngle(int parameter1, int parameter2);
void ChangeTdcMaxDistance(int parameter1, int parameter2);
void ChangeLdDefaultValue(int parameter1, int parameter2);
void ChangeDtAlignInfo(int parameter1, int parameter2);
//A2Z
void hhjwebTest(int parameter1, int parameter2);
void ChangeDarkAreaCorrectionSetting(int parameter1, int parameter2);
void ChangeDarkAreaCorrectionSetting_v2(int parameter1, int parameter2);

void ChangeLdChannelEnable(int parameter1, int parameter2);
void ChangeLdChannelDelay(int parameter1, int parameter2);
void ChangeLdChannelTdTrDelay(int parameter1, int parameter2);
void ChangeLdChannelTdTfDelay(int parameter1, int parameter2);

void ChangeRoutingDelay2(int parameter1, int parameter2);

#endif /* SRC_CALIBRATION_CALIBRATION_H_ */
