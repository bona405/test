/*
 * main_controller.c
 *
 *  Created on: 2022. 12. 15.
 *      Author: Wonju
 */

#include "main_controller.h"

XStatus InitMinimumDetectionRange(LidarState* lidar_state)
{
	//coarse
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG9_OFFSET, lidar_state->calibration_setting_.minimum_detection_range / (u32 )100); // CH0-CH3
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG10_OFFSET, lidar_state->calibration_setting_.minimum_detection_range / (u32 )100); // CH4-CH7
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG11_OFFSET, lidar_state->calibration_setting_.minimum_detection_range / (u32 )100); // CH8-CH11
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG12_OFFSET, lidar_state->calibration_setting_.minimum_detection_range / (u32 )100); // CH12-CH15
	//fine
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG13_OFFSET, lidar_state->calibration_setting_.minimum_detection_range % 100); // CH0-CH3
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG14_OFFSET, lidar_state->calibration_setting_.minimum_detection_range % 100); // CH4-CH7
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG15_OFFSET, lidar_state->calibration_setting_.minimum_detection_range % 100); // CH8-CH11
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG16_OFFSET, lidar_state->calibration_setting_.minimum_detection_range % 100); // CH12-CH15

	if(lidar_state_.calibration_setting_.minimum_detection_range_start1 > (ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2 ||
			lidar_state_.calibration_setting_.minimum_detection_range_start1 < 0)
		lidar_state_.calibration_setting_.minimum_detection_range_start1 = 0;
	if(lidar_state_.calibration_setting_.minimum_detection_range_stop1 < -(ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2||
			lidar_state_.calibration_setting_.minimum_detection_range_stop1 > 0)
		lidar_state_.calibration_setting_.minimum_detection_range_stop1 = 0;
	if(lidar_state_.calibration_setting_.minimum_detection_range_start2 > (ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2 ||
				lidar_state_.calibration_setting_.minimum_detection_range_start2 < 0)
			lidar_state_.calibration_setting_.minimum_detection_range_start2 = 0;
	if(lidar_state_.calibration_setting_.minimum_detection_range_stop2 < -(ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2||
			lidar_state_.calibration_setting_.minimum_detection_range_stop2 > 0)
		lidar_state_.calibration_setting_.minimum_detection_range_stop2 = 0;

	TDCSetDisableStart1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.calibration_setting_.minimum_detection_range_start1);
	TDCSetDisableStop1(ENC_CTRL_LD_STOP_ENC_CNT1 + lidar_state_.calibration_setting_.minimum_detection_range_stop1);
	TDCSetDisableStart2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.calibration_setting_.minimum_detection_range_start2);
	TDCSetDisableStop2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.calibration_setting_.minimum_detection_range_stop2);
	return XST_SUCCESS;
}

XStatus InitLdEnabledRange(LidarState* lidar_state)
{
	if(lidar_state_.motor_setting_.start_enc_count < 0 || lidar_state_.motor_setting_.start_enc_count > (ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2)
		lidar_state_.motor_setting_.start_enc_count = ENC_CTRL_LD_START_ENC_CNT1;
	if(lidar_state_.motor_setting_.start_enc_count < 0 || lidar_state_.motor_setting_.start_enc_count > (ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2)
		lidar_state_.motor_setting_.start_enc_count = ENC_CTRL_LD_START_ENC_CNT1;

//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.top_enc_offset);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.top_enc_offset);

//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + + lidar_state_.motor_setting_.end_enc_count +lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count+ lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.top_enc_offset);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.top_enc_offset);

//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count  + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.top_enc_offset);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count +  lidar_state_.motor_setting_.top_enc_offset);

//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count  + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.bottom_end_enc_offset);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.top_enc_offset);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count +  lidar_state_.motor_setting_.top_end_enc_offset);

	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.total_enc_offset);
	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.total_enc_offset);
	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.total_enc_offset);
	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.total_enc_offset);


//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + lidar_state_.motor_setting_.end_enc_count);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count);
	return XST_SUCCESS;
}

XStatus InitLdDefaultValue(LidarState* lidar_state)
{
	if(lidar_state_.laser_setting_.tdtr < 0 || lidar_state_.laser_setting_.tdtr > 1000)
		lidar_state_.laser_setting_.tdtr = 10;
	if(lidar_state_.laser_setting_.tdtf < 0 || lidar_state_.laser_setting_.tdtf > 1000)
		lidar_state_.laser_setting_.tdtf = 25;
	if(lidar_state_.laser_setting_.max_distance < 0 || lidar_state_.laser_setting_.max_distance > 10000)
		lidar_state_.laser_setting_.max_distance = 170;
	if(lidar_state_.laser_setting_.ch_interval_1 < 0 || lidar_state_.laser_setting_.ch_interval_1 > 10000)
		lidar_state_.laser_setting_.ch_interval_1 = 0;
	if(lidar_state_.laser_setting_.ch_interval_2 < 0 || lidar_state_.laser_setting_.ch_interval_2 > 10000)
		lidar_state_.laser_setting_.ch_interval_2 = 160;

	for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
		LDCtrlSetChannelTdTrDelay(ld_channel_index, lidar_state_.laser_setting_.tdtr);
	usleep(2000);
	for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
		LDCtrlSetChannelTdTfDelay(ld_channel_index, lidar_state_.laser_setting_.tdtf);
	usleep(2000);
	TDCCtrlSetMaxDistance((u16)lidar_state_.laser_setting_.max_distance);
	usleep(2000);
	for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
		LDCtrlSetChannelDelay1(ld_channel_index, lidar_state_.laser_setting_.ch_interval_1);
	usleep(2000);
	for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
		LDCtrlSetChannelDelay2(ld_channel_index, lidar_state_.laser_setting_.ch_interval_2);
}

XStatus HWInit(LidarState* lidar_state)
{
	FpgaVersionInfoInit();
    EncCtrlInit();
//    SetDTEnable();	//230713 DTEnable
    usleep(100);
	HeatCtrlInit();
	MotorCtrlInit();
	TDCCtrlInit();
	LDCtrlInit();
	MCP4801CtrlInit();
	MCP4822CtrlInit();
	XAdcPsInit();
	TDCHistoBramCtrlInit();
	TDCLutBramCtrlInit();
//	UtilLogicInit();

	// sequential required
	FpgaPrintVersionInfo();
	TDCCtrlSetDefault();
	usleep(2000);	// NEED
    EncCtrlSetDefault();
//	EncCtrlSetEncCountOffset(lidar_state->start_enc_count, lidar_state->end_enc_count);
    usleep(2000);	// NEEDAQ
    MCP4801CtrlSetDefault();
//    MCP4822CtrlSetDefault();
//	HeatCtrlSetDefault();
	usleep(2000);	// NEED
	//original source
//	MotorCtrlSetDefault();


	usleep(2000);	// NEED
	LDCtrlSetDefault();
	usleep(2000);
//	UtilLogicSetDefault();

	InitMinimumDetectionRange(lidar_state);
	InitLdEnabledRange(lidar_state);
	InitLdDefaultValue(lidar_state);
	xil_printf("[OK] HW Init SUCCESS\r\n");

	return XST_SUCCESS;
}
