/*
 * api_hw.c
 *
 *  Created on: 2022. 8. 2.
 *      Author: Wonju
 */

#include "calibration.h"

#include "../../../common/src/api_sw/flash/flash.h"
#include "../../../common/src/api_sw/models/machine_info.h"
#include "../../../common/src/api_sw/algorithms/dt_align.h"
#include "sleep.h"
#include <xil_types.h>
int pass_frame = 3;
int current_vth_offset = 60;
int max_distance = 250;
int min_distance = 0;
int bottom_horizontal_offset = 0;
int is_heat_control_ON = 0;
volatile uint8_t is_bottom_horizontal_offset_changed = 0;
int vnfilter = 0;
extern int now_nb;
void ChangeVth(int parameter1, int parameter2)
{
	if(parameter1 == 0)
	{
		MCP4822CtrlSetVthAll(parameter2);
		current_vth_offset = parameter2;
	}
	else if(parameter1 == 17)
	{
		MCP4801CtrlSetValue(parameter2);
		now_nb = parameter2;
	}
	else
	{
		MCP4822CtrlSetChannelVth(parameter1 - 1, parameter2);
	}

}

void ChangeRoutingDelay(int parameter1, int parameter2)
{
	if(parameter1 == 0)
	{
		for(int i = 0 ; i < 16 ; ++i)
		{
			routing_delay[i] = parameter2;
			routing_delay[i + 16] = parameter2;
		}
	}
	else if(0 < parameter1 && parameter1 < 33)
	{
		routing_delay[parameter1 - 1] = parameter2;
	}
}
void ChangeRoutingDelay2(int parameter1, int parameter2)
{
	if(parameter1 == 0)
	{
		for(int i = 0 ; i < 16 ; ++i)
		{
			routing_delay2[i] = parameter2;
			routing_delay2[i + 16] = parameter2;
		}
	}
	else if(0 < parameter1 && parameter1 < 33)
	{
		routing_delay2[parameter1 - 1] = parameter2;
	}
	return;
}

void ChangeDistanceOffset(int parameter1, int parameter2)
{

	clip_min = parameter2;

}

void ChangeEmPulse(int parameter1, int parameter2)
{
	for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
		SetLdChon(ld_index, parameter2 / (u32)100, parameter2 % 100);
	LDAllSet();

	lidar_state_.laser_setting_.em_pulse = parameter2;
}

void ChangeDistanceMinLimit(int parameter1, int parameter2)
{
	min_distance = parameter2;
}

void ChangeDistanceMaxLimit(int parameter1, int parameter2)
{
	max_distance = parameter2;
}

void ChangeHorizontalAngleOffset(int parameter1, int parameter2)
{
	if(parameter1 == 0)
	{
		bottom_horizontal_offset = parameter2;
		is_bottom_horizontal_offset_changed = 0;
	}
	else
	{
		lidar_state_.calibration_setting_.total_azimuth_offset = parameter2;
		total_azimuth_offset = parameter2;
	}
	pass_frame = 3;
//	usleep(15000);
}

void ChangeNoiseFilterSettings(int parameter1, int parameter2)
{
	if(parameter1 == 0)
	{
		noise_reference_value_0 = (float)parameter2 / 100;
	}
	else if(parameter1 == 1)
	{
		vth_sensitivity = (int)parameter2;
	}
	else if(parameter1 == 2)
	{
		noise_determined_number = parameter2;
	}
	else if(parameter1 == 3)
	{
		noise_reference_number = parameter2;
	}
	else if(parameter1 == 4)
	{
		noise_percent_logic_flag = parameter2;
	}
	else if(parameter1 == 5)
	{
		noise_min_value = parameter2;
		itoa(parameter2, lidar_state_.filter_setting_.noise_min_value, 10);
	}
	else if(parameter1 == 6)
	{
		check_azimuth_first = parameter2;
	}
	else if(parameter1 == 7)
	{
		set_noise_point_intensity = parameter2;
	}
	else if(parameter1 == 8)
	{
		ignore_ground_point = parameter2;
	}
	else if(parameter1 == 9)
	{
		noise_filter_max_distance_ = parameter2 * 256;
		remove_noise_min_distance_ = parameter2;
	}
	else if(parameter1 == 10)
	{
		close_noise_neighbor_size_ = parameter2;
	}
	else if(parameter1 == 11)
	{
		distance_limit_ = parameter2;
	}
	else if(parameter1 == 17)
	{
		if(parameter2 == 0)
		{
			lidar_state_.filter_setting_.is_fir_on = 0;
		}
		else
		{
			lidar_state_.filter_setting_.is_fir_on = 1;
		}
	}
	else if(parameter1 == 18)
	{
		if(parameter2 == 0)
		{
			lidar_state_.filter_setting_.is_intensity_correction_on = 0;
		}
		else
		{
			lidar_state_.filter_setting_.is_intensity_correction_on = 1;
		}

	}
	else if(parameter1 == 19)
	{
		if(parameter2 == 0)
		{
			lidar_state_.calibration_setting_.azioffset_enable = 0;
		}
		else
		{
			lidar_state_.calibration_setting_.azioffset_enable = 1;
		}
	}
	else if(parameter1 == 20)
	{
		if(parameter2 == 0)
		{
			lidar_state_.calibration_setting_.temperadj_enable = 0;
		}
		else
		{
			lidar_state_.calibration_setting_.temperadj_enable = 1;
		}
	}
	else if(parameter1 == 21)
	{
		//gas filter
		if(parameter2 == 0)
		{
			lidar_state_.filter_setting_.is_noise_on = 0;
		}
		else
		{
			lidar_state_.filter_setting_.is_noise_on = 1;
		}
	}
	else if(parameter1 == 22)
	{
		//noise filter2
		if(parameter2 == 0)
		{
			lidar_state_.filter_setting_.is_noise_filter_on = 0;
		}
		else
		{
			lidar_state_.filter_setting_.is_noise_filter_on = 1;
		}
	}
	else if(parameter1 == 23)
	{
		//new noise filter
		if(parameter2 == 0)
		{
			lidar_state_.filter_setting_.is_new_noise_filter_on = 0;
		}
		else
		{
			lidar_state_.filter_setting_.is_new_noise_filter_on = 1;
		}
	}
	else if(parameter1 == 24)
	{
		if(parameter2 == 0)
		{
			lidar_state_.calibration_setting_.tempergain_enable = 0;
		}
		else
		{
			lidar_state_.calibration_setting_.tempergain_enable = 1;
		}
	}
	else if(parameter1 == 99)
	{
		//Disable All Filter and Direct Detections
		if(parameter2 == 0)
		{
			lidar_state_.filter_setting_.is_fir_on = 0;
			lidar_state_.filter_setting_.is_intensity_correction_on = 0;
			lidar_state_.calibration_setting_.azioffset_enable = 0;
			lidar_state_.filter_setting_.is_noise_on = 0;
			lidar_state_.filter_setting_.is_noise_filter_on = 0;
			lidar_state_.filter_setting_.is_new_noise_filter_on = 0;

			rg_parameter_.is_ground_remove_on = 0;
			lidar_state_.filter_setting_.is_remove_ghost_on = 0;

			lidar_state_.a2z_setting.reserved02 = 0;

			dark_area_state_.darkarea_setting_.is_enable_dark_area_crror_correction_ = 0;
			dark_area_state_.darkarea_setting_2.is_enable_dark_area_crror_correction_ = 0;
			dark_area_state_.darkarea_setting_3.is_enable_dark_area_crror_correction_ = 0;


			ChangeMinimumDetectionRange(0, 0);
			usleep(1);
			ChangeMinimumDetectionRange(1, 0);
			usleep(1);
			ChangeMinimumDetectionRange(2, 0);
			usleep(1);
			ChangeMinimumDetectionRange(3, 0);
			usleep(1);
			ChangeMinimumDetectionRange(4, 0);

			ChangeHorizontalAngleOffset(0, 0);
		}
		else
		{
			lidar_state_.filter_setting_.is_fir_on = 1;
			lidar_state_.filter_setting_.is_intensity_correction_on = 1;
			lidar_state_.calibration_setting_.azioffset_enable = 1;
			lidar_state_.filter_setting_.is_noise_on = 1;
			lidar_state_.filter_setting_.is_noise_filter_on = 1;
			lidar_state_.filter_setting_.is_new_noise_filter_on = 1;

			rg_parameter_.is_ground_remove_on = 1;
			lidar_state_.filter_setting_.is_remove_ghost_on = 1;

			lidar_state_.a2z_setting.reserved02 = 1;

			dark_area_state_.darkarea_setting_.is_enable_dark_area_crror_correction_ = 1;
			dark_area_state_.darkarea_setting_2.is_enable_dark_area_crror_correction_ = 1;
			dark_area_state_.darkarea_setting_3.is_enable_dark_area_crror_correction_ = 1;

			dark_area_state_.darkarea_setting_.is_dark_area_channel_changed_ = true;
			dark_area_state_.darkarea_setting_2.is_dark_area_channel_changed_ = true;
			dark_area_state_.darkarea_setting_3.is_dark_area_channel_changed_ = true;


			ChangeMinimumDetectionRange(0, 100);
			usleep(1);
			ChangeMinimumDetectionRange(1, 100);
			usleep(1);
			ChangeMinimumDetectionRange(2, -100);
			usleep(1);
			ChangeMinimumDetectionRange(3, 100);
			usleep(1);
			ChangeMinimumDetectionRange(4, -100);
		}
	}
}

// 240325
void ChangeMinimumDetectionRange(int parameter1, int parameter2)
{
	switch(parameter1)
	{
	case 0:
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG9_OFFSET, parameter2 / (u32 )100);
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG10_OFFSET, parameter2 / (u32 )100);
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG11_OFFSET, parameter2 / (u32 )100);
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG12_OFFSET, parameter2 / (u32 )100);
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG13_OFFSET, parameter2 % 100);
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG14_OFFSET, parameter2 % 100);
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG15_OFFSET, parameter2 % 100);
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG16_OFFSET, parameter2 % 100);
		lidar_state_.calibration_setting_.minimum_detection_range = parameter2;
		break;
	case 1:
		TDCSetDisableStart1(ENC_CTRL_LD_START_ENC_CNT1 + parameter2);
		lidar_state_.calibration_setting_.minimum_detection_range_start1 = parameter2;
		break;
	case 2:
		TDCSetDisableStop1(ENC_CTRL_LD_STOP_ENC_CNT1 + parameter2);
		lidar_state_.calibration_setting_.minimum_detection_range_stop1 = parameter2;
		break;
	case 3:
		TDCSetDisableStart2(ENC_CTRL_LD_START_ENC_CNT2 + parameter2);
		lidar_state_.calibration_setting_.minimum_detection_range_start2 = parameter2;
		break;
	case 4:
		TDCSetDisableStop2(ENC_CTRL_LD_STOP_ENC_CNT2 + parameter2);
		lidar_state_.calibration_setting_.minimum_detection_range_stop2 = parameter2;
		break;
	case 5:
		lidar_state_.filter_setting_.is_adaptive_pulse_width_on = parameter2;
		break;
	case 6:
//		if(lidar_state_.is_adaptive_pulse_width_on == 1)
		TDCSetPulseWidthScaleIndex(parameter2);
		break;
	case 10:
		if(parameter2 > 7 || parameter2 < 0)
		{
			parameter2 = 0;
		}
		Xil_Out32(0x43C2004C, parameter2);
		lidar_state_.calibration_setting_.start_sig = parameter2;
		break;
	default :
		break;
	}
}

void ChangeRemoveGhostSettings(int parameter1, int parameter2)
{
	switch(parameter1)
	{
	case 0 :
		gd_parameter_.sensor_height_ = parameter2;
		break;
	case 1 :
		gd_parameter_.angle_max_1_ = parameter2 * 0.1;
		break;
	case 2 :
		gd_parameter_.angle_max_2_ = parameter2 * 0.1;
		break;
	case 3 :
		gd_parameter_.height_min_1_ = parameter2 * 0.1;
		break;
	case 4 :
		gd_parameter_.height_min_2_ = parameter2 * 0.1;
		break;
	case 5 :
		gd_parameter_.distance_diff_ = parameter2;
		break;
	case 6 :
		gd_parameter_.abnormal_distance_check_th_ = parameter2;
		break;
	case 7 :
		if_parameter_.width_ = parameter2;
		break;
	case 8 :
		if_parameter_.height_ = parameter2;
		break;
	case 9 :
		if_parameter_.max_range_ = parameter2 * 0.1;
		break;
	case 10 :
		if_parameter_.min_range_ = parameter2 * 0.1;
		break;
	case 11 :
		if_parameter_.th_g_ = parameter2 * 0.1;
		break;
	case 12 :
		if_parameter_.sigma_ = parameter2 * 0.1;
		break;
	case 13 :
		if_parameter_.deltaR_ = parameter2 * 0.1;
		break;
	case 14 :
		rg_parameter_.z_offset_ = parameter2 * 0.1;
		break;
	case 15 :
		rg_parameter_.z_offset_2_ = parameter2 * 0.1;
		break;
	case 16 :
		rg_parameter_.x_offset_ = parameter2;
		break;
	case 17 :
		rg_parameter_.horizontal_check_num_ = parameter2;
		break;
	case 18 :
		rg_parameter_.vertical_check_num_ = parameter2;
		break;
	case 19 :
		rg_parameter_.is_ground_remove_on = parameter2;
		lidar_state_.filter_setting_.is_remove_ghost_on = parameter2;
		break;
	case 20 :
		vnfilter = parameter2;
		break;
	case 22 :
		if(parameter2 >= 1)
		{
			routing_add_flag = 1;
			adddelay = -1 * parameter2;
		}
		break;
	case 23 :
		if(parameter2 >= 1)
		{
			cal_flag = 1;
		}
		break;
	case 24 :
		if(parameter2 >= 1)
		{
			routing_resetflag = 1;
		}
		break;
	case 25 :
		if(parameter2 > 0 && parameter2 < 10)
		{
			fixed_cal_flag = 1;
			cal_distance = parameter2;
		}
		break;
	case 26 :
		if(parameter2 >= 1)
		{
			routing_add_flag = 1;
			adddelay = parameter2;
		}
		break;
	default :
		break;
	}
}


void InitChannelVth(struct LidarState_Ch* lidar_state_ch)
{
	ChangeVth(1, atoi(lidar_state_ch->th_ch16));
	ChangeVth(2, atoi(lidar_state_ch->th_ch15));
	ChangeVth(3, atoi(lidar_state_ch->th_ch14));
	ChangeVth(4, atoi(lidar_state_ch->th_ch13));
	ChangeVth(5, atoi(lidar_state_ch->th_ch12));
	ChangeVth(6, atoi(lidar_state_ch->th_ch11));
	ChangeVth(7, atoi(lidar_state_ch->th_ch10));
	ChangeVth(8, atoi(lidar_state_ch->th_ch9));
	ChangeVth(9, atoi(lidar_state_ch->th_ch8));
	ChangeVth(10, atoi(lidar_state_ch->th_ch7));
	ChangeVth(11, atoi(lidar_state_ch->th_ch6));
	ChangeVth(12, atoi(lidar_state_ch->th_ch5));
	ChangeVth(13, atoi(lidar_state_ch->th_ch4));
	ChangeVth(14, atoi(lidar_state_ch->th_ch3));
	ChangeVth(15, atoi(lidar_state_ch->th_ch2));
	ChangeVth(16, atoi(lidar_state_ch->th_ch1));
}

void ChangeRPM(int parameter1, int parameter2)
{
	if(parameter1 == 0)
	{
		lidar_state_.motor_setting_.motor_rpm = parameter2;
		MotorCtrlRPMSet(parameter2);
	}
	else
	{
		if(parameter1 == 1 && parameter2 == 1)
		{
//			HeatCtrlEnable();
			is_heat_control_ON = 1;
		    HeatCtrlFreqSet(8250000);
		    HeatCtrlDutySet(825000);
//		    HeatCtrlDutySet(3025000);
		}
		else if(parameter1 == 1 && parameter2 == 0)
		{
			is_heat_control_ON = 0;
		    HeatCtrlFreqSet(8250000);
		    HeatCtrlDutySet(0);
//			HeatCtrlDisable();
		}

		if(parameter1 == 2)
		{
			SetPseudoEncEdge(parameter2);
		}

		//Motor Debug
		if(parameter1 == 7)
		{
#ifdef G32_B1_PPS
			if(parameter2 == 1)
			{
				int now_status = 0;

				lidar_state_.motor_setting_.ispllon = 1;

				now_status = Xil_In32(0x43C50028);

				if(now_status == 1)
				{
					Xil_Out32(0x43C50044, 0x0f);
				}
			}
			else
			{
				lidar_state_.motor_setting_.ispllon = 0;
				Xil_Out32(0x43C50044, 0x00);
			}
#else
			lidar_state_.motor_setting_.ispllon = 0;
#endif


//			printf("Status %d Debug!", now_status);
		}

		if(parameter1 == 8)
		{
			//phase degree
			if(parameter2 < -60 || parameter2 > 60)
			{
				parameter2 = 0;
			}

			int plldeg = 0;

			plldeg = 45 - parameter2;

			plldeg = (float)plldeg / 0.010986663;



			lidar_state_.motor_setting_.pll_point = plldeg;
#ifdef G32_B1_PPS
			Xil_Out32(0x43C50030, plldeg);
#endif
//			printf("Status %d Debug!", now_status);
		}
		//End
	}

}

void ChangeSetMotorAccelerationTime(int parameter1, int parameter2)
{
	lidar_state_.motor_setting_.motor_acceleration_time = parameter2;
}

void ChangeSetMotorAccelerationResolution(int parameter1, int parameter2)
{
	lidar_state_.motor_setting_.motor_acceleration_resolution = parameter2;
}

void ChangeHistogramStartVth(int parameter1, int parameter2)
{
	lidar_state_.calibration_setting_.histogram_start_vth = parameter2;
}

void ChangeLdChannelEnable(int parameter1, int parameter2)
{
	if(parameter1 == 0)
		for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
			LDCtrlSetChannelEnable(ld_channel_index+1, parameter2);
	else if(parameter1 > 0 && parameter1 <= 4)
		LDCtrlSetChannelEnable(parameter1, parameter2);
/*
	if(parameter2 == 0)
	{
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG1_OFFSET, 0x0);
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG2_OFFSET, 0x0);
	}
	else if(parameter2 == 1)
	{
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG1_OFFSET, 0xf);
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG2_OFFSET, 0x0);
	}
	*/
}

void ChangeLdChannelDelay(int parameter1, int parameter2)
{
	if(parameter1 == 0)
		for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
			LDCtrlSetChannelDelay2(ld_channel_index, parameter2);
	else if(parameter1 > 0 && parameter1 <= 4)
		LDCtrlSetChannelDelay2(parameter1 - 1, parameter2);
	else
		for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
			LDCtrlSetChannelDelay1(ld_channel_index, parameter2);
}

void ChangeLdChannelTdTrDelay(int parameter1, int parameter2)
{
	if(parameter1 == 0)
		for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
			LDCtrlSetChannelTdTrDelay(ld_channel_index, parameter2);
	else if(parameter1 > 0 && parameter1 <= 4)
		LDCtrlSetChannelTdTrDelay(parameter1 - 1, parameter2);
}

void ChangeLdChannelTdTfDelay(int parameter1, int parameter2)
{
	if(parameter1 == 0)
		for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
			LDCtrlSetChannelTdTfDelay(ld_channel_index, parameter2);
	else if(parameter1 > 0 && parameter1 <= 4)
		LDCtrlSetChannelTdTfDelay(parameter1 - 1, parameter2);
}

void ChangeStartEncoderCount(int parameter1, int parameter2)
{
	if(parameter1 == 0)
	{
		//u32 current_start_enc_count = EncCtrlGetStartEncCount1();
//		int32_t new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT1 + parameter2;
//		int32_t new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.bottom_enc_offset;
		int32_t new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.total_enc_offset;

		if(new_start_enc_count >= 0)
		{
			EncCtrlSetStartEncCount1(new_start_enc_count);
		}


		//current_start_enc_count = EncCtrlGetStartEncCount2();
//		new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.top_enc_offset;
		new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.total_enc_offset;

		if(new_start_enc_count >= 0)
		{
			EncCtrlSetStartEncCount2(new_start_enc_count);
		}

//		is_trig_function_map_initialized = 0;

		lidar_state_.motor_setting_.start_enc_count = parameter2;
	}
	else if(parameter1 == 1)
	{	//total encoder offset
		if (parameter2 > -100 && parameter2 < 100)
		{
			lidar_state_.motor_setting_.total_enc_offset = parameter2;
//			int32_t new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.bottom_enc_offset;
			int32_t new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.start_enc_count;
			int32_t new_stop_enc_count = ENC_CTRL_LD_STOP_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.end_enc_count;

			if(new_start_enc_count >=0)	//bottom start
			{
				EncCtrlSetStartEncCount1(new_start_enc_count);
			}

			if(new_stop_enc_count >=0)	//bottom end
			{
				EncCtrlSetStopEncCount1(new_stop_enc_count);
			}

			new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.start_enc_count;
			new_stop_enc_count = ENC_CTRL_LD_STOP_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.end_enc_count;

			if(new_start_enc_count >=0)	//bottom start
			{
				EncCtrlSetStartEncCount2(new_start_enc_count);
			}

			if(new_stop_enc_count >=0)	//bottom end
			{
				EncCtrlSetStopEncCount2(new_stop_enc_count);
			}
//
//			new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.top_enc_offset;
//
//			if(new_start_enc_count >= 0)
//			{
//				EncCtrlSetStartEncCount2(new_start_enc_count);
//
//				EncCtrlSetStopEncCount2(new_start_enc_count + 768 + 1);
//			}

		}
//		u32 current_start_enc_count = EncCtrlGetStartEncCount2();
//		int32_t new_start_enc_count = current_start_enc_count + parameter2;
//
//		if(new_start_enc_count >= 0)
//			EncCtrlSetStartEncCount2(new_start_enc_count);
	}
	else if(parameter1 == 2)
	{	//bottom encoder offset
//		if (parameter2 > -100 && parameter2 < 100)
//		{
//			lidar_state_.motor_setting_.bottom_enc_offset = parameter2;
//
////			int32_t new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.total_enc_offset;
//			int32_t new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.start_enc_count;
//			if(new_start_enc_count >= 0)
//			{
//				EncCtrlSetStartEncCount1(new_start_enc_count);
////				EncCtrlSetStopEncCount1(new_start_enc_count + 768 + 1);
//			}
//		}
//		u32 current_start_enc_count = EncCtrlGetStartEncCount2();
//		int32_t new_start_enc_count = current_start_enc_count + parameter2;
//
//		if(new_start_enc_count >= 0)
//			EncCtrlSetStartEncCount2(new_start_enc_count);
	}
	else if(parameter1 == 3)
	{	//top encoder offset
//		if (parameter2 > -100 && parameter2 < 100)
//		{
//			lidar_state_.motor_setting_.top_enc_offset = parameter2;
////			int32_t new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.total_enc_offset;
//			int32_t new_start_enc_count = ENC_CTRL_LD_START_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.start_enc_count;
//
//			if(new_start_enc_count >= 0)
//			{
//				EncCtrlSetStartEncCount2(new_start_enc_count);
//
////				EncCtrlSetStopEncCount2(new_start_enc_count + 768 + 1);
//			}
//		}
//		u32 current_start_enc_count = EncCtrlGetStartEncCount2();
//		int32_t new_start_enc_count = current_start_enc_count + parameter2;
//
//		if(new_start_enc_count >= 0)
//			EncCtrlSetStartEncCount2(new_start_enc_count);
	}
//	usleep(15000);//change encoder and sleep.
	pass_frame = 3;
	is_trig_function_map_initialized = 0;
//	usleep(100000);//change encoder and sleep.
}

void ChangeEndEncoderCount(int parameter1, int parameter2)
{
	if(parameter1 == 0)
	{
		//u32 current_end_enc_count = EncCtrlGetStopEncCount1();
//		int32_t new_end_enc_count = ENC_CTRL_LD_STOP_ENC_CNT1 + parameter2;
//		int32_t new_end_enc_count = ENC_CTRL_LD_STOP_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.bottom_end_enc_offset;
		int32_t new_end_enc_count = ENC_CTRL_LD_STOP_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.total_enc_offset;

		if(new_end_enc_count >= 0)
		{
			EncCtrlSetStopEncCount1(new_end_enc_count);
		}


		//current_end_enc_count = EncCtrlGetStopEncCount2();
//		new_end_enc_count = ENC_CTRL_LD_STOP_ENC_CNT2 + parameter2;
//		new_end_enc_count = ENC_CTRL_LD_STOP_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.top_end_enc_offset;
		new_end_enc_count = ENC_CTRL_LD_STOP_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.total_enc_offset;

		if(new_end_enc_count >= 0)
		{
			EncCtrlSetStopEncCount2(new_end_enc_count);
		}

//		is_trig_function_map_initialized = 0;

		lidar_state_.motor_setting_.end_enc_count = parameter2;
	}
	else if(parameter1 == 1)
	{
//		u32 current_end_enc_count = EncCtrlGetStopEncCount2();
//		int32_t new_end_enc_count = current_end_enc_count + parameter2;
//
//		if(new_end_enc_count >= 0)
//			EncCtrlSetStopEncCount2(new_end_enc_count);
	}
	else if(parameter1 == 2)	//bottom end encoder offset
	{
//		if (parameter2 > -100 && parameter2 < 100)
//		{
//			lidar_state_.motor_setting_.bottom_end_enc_offset = parameter2;
//
//			int32_t new_end_enc_count = ENC_CTRL_LD_STOP_ENC_CNT1 + parameter2 + lidar_state_.motor_setting_.end_enc_count;
//			if(new_end_enc_count >= 0)
//			{
//				EncCtrlSetStopEncCount1(new_end_enc_count);
////				EncCtrlSetStopEncCount1(new_start_enc_count + 768 + 1);
//			}
//		}

	}
	else if(parameter1 == 3)	//top end encoder offset
	{
//		if (parameter2 > -100 && parameter2 < 100)
//		{
//			lidar_state_.motor_setting_.top_end_enc_offset = parameter2;
//
//			int32_t new_end_enc_count = ENC_CTRL_LD_STOP_ENC_CNT2 + parameter2 + lidar_state_.motor_setting_.end_enc_count;
//			if(new_end_enc_count >= 0)
//			{
//				EncCtrlSetStopEncCount2(new_end_enc_count);
////				EncCtrlSetStopEncCount1(new_start_enc_count + 768 + 1);
//			}
//		}

	}

	pass_frame = 3;
	is_trig_function_map_initialized = 0;
//	usleep(15000);//change encoder and sleep.
}

void ChangeHorizontalFovAngle(int parameter1, int parameter2)
{
	if(parameter1 == 0)
		fov_test_.start_horizontal_angle = (float)parameter2 / 10;
	else if(parameter1 == 1)
		fov_test_.end_horizontal_angle = (float)parameter2 / 10;
}

void ChangeVerticalFovAngle(int parameter1, int parameter2)
{
	if(parameter1 == 0)
		fov_test_.start_vertical_angle = parameter2;
	else if(parameter1 == 1)
		fov_test_.end_vertical_angle = parameter2;

}

void ChangeTdcMaxDistance(int parameter1, int parameter2)
{
	if(parameter2 > 0)
		TDCCtrlSetMaxDistance((u16)parameter2);
}

void ChangeLdDefaultValue(int parameter1, int parameter2)
{
	switch(parameter1)
	{
	case 0 :
		lidar_state_.laser_setting_.tdtr = parameter2;
		for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
			LDCtrlSetChannelTdTrDelay(ld_channel_index, parameter2);
		break;
	case 1 :
		lidar_state_.laser_setting_.tdtf = parameter2;
		for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
			LDCtrlSetChannelTdTfDelay(ld_channel_index, parameter2);
		break;
	case 2 :
		lidar_state_.laser_setting_.max_distance = parameter2;
		TDCCtrlSetMaxDistance((u16)parameter2);
		break;
	case 3 :
		lidar_state_.laser_setting_.ch_interval_1 = parameter2;
		for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
			LDCtrlSetChannelDelay1(ld_channel_index, parameter2);
		break;
	case 4 :
		lidar_state_.laser_setting_.ch_interval_2 = parameter2;
		for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
			LDCtrlSetChannelDelay2(ld_channel_index, parameter2);
		break;
	}
}

void ChangeDtAlignInfo(int parameter1, int parameter2)
{
	switch(parameter1)
	{
	case 1:
		dt_align_info_.start_vth = parameter2;
		break;
	case 2:
		dt_align_info_.end_vth = parameter2;
		break;
	case 3:
		dt_align_info_.vth_interval = parameter2;
		break;
	case 4:
		dt_align_info_.check_interval = parameter2;
		break;
	case 5:
		dt_align_info_.vth_count_limit = parameter2;
		break;
	case 6:
	{
//		usleep(10);
//
//		for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
//		{
//			SetLdChon(ld_index, parameter2 / (u32)100, parameter2 % 100);
//		}
//		LDAllSet();
//
//		usleep(10);
//
//		lidar_state_.laser_setting_.em_pulse = parameter2;

		int Status = InitLinearQspiFlash();
		if (Status != XST_SUCCESS)
		{
			xil_printf("Flash Init Failed\r\n");
//			return XST_FAILURE;
		}

		memset(lidar_state_buffer_, 0x00, LIDAR_STATE_SIZE);
		Status = ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
		memcpy(&lidar_state_, lidar_state_buffer_, sizeof(struct LidarState));
		if (Status != XST_SUCCESS)
		{
			xil_printf("Flash Read Failed\r\n");
//			return XST_FAILURE;
		}

		lidar_state_.dt_align_settings_.dt_start = dt_align_info_.start_vth;
		lidar_state_.dt_align_settings_.dt_end = dt_align_info_.end_vth;
		lidar_state_.dt_align_settings_.dt_interval = dt_align_info_.vth_interval;
		lidar_state_.dt_align_settings_.dt_check_interval = dt_align_info_.check_interval;


		EraseLidarInfo();
		Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
		if (Status != XST_SUCCESS)
		{
			xil_printf("Flash Write Failed\r\n");
//			return XST_FAILURE;
		}
	}
	case 7:
		dt_align_info_.current_channel = parameter2;
		break;
	default:
		break;
	}
}
void hhjwebTest(int parameter1, int parameter2)
{
//	A2Z_VAL &= ~(0x00ffffff);
//	A2Z_VAL |= parameter1&0xFF;
//	A2Z_VAL |= ((parameter2&0xFFFF) << 8);
	uint8_t *a2z_val1 = (uint8_t *)&A2Z_VAL;

	switch (parameter1) {
		case 200:
			lidar_state_.a2z_setting.near_dist = parameter2*0.1;
			break;
		case 201:
			lidar_state_.a2z_setting.far_vth = parameter2;
			break;
		case 202:
			lidar_state_.a2z_setting.near_vth = parameter2;
			break;
//		case 203:
//			lidar_state_.a2z_setting.far_em = parameter2;
//			break;
//		case 204:
//			lidar_state_.a2z_setting.near_em = parameter2;
//			break;
		case 205:
			lidar_state_.a2z_setting.state = parameter2;
			break;
		case 206:
			lidar_state_.a2z_setting.merge_dist = parameter2*0.1;
			break;
		case 207:
			EraseLidarInfo();
			LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
			*a2z_val1 = 1;
			break;
		case 208:
			lidar_state_.a2z_setting.clustering_onoff = parameter2;
			break;
		case 209:
			lidar_state_.a2z_setting.reserved01 = parameter2;
			break;
		case 100:
			lidar_state_.a2z_setting.cc_eps = parameter2;
			break;
		case 101:
			lidar_state_.a2z_setting.cc_minpts = parameter2;
			break;
		case 102:
			lidar_state_.a2z_setting.ac_eps = parameter2;
			break;
		case 103:
			lidar_state_.a2z_setting.ac_minpts = parameter2;
			break;
		case 104:
			lidar_state_.a2z_setting.z_offset = parameter2;
			break;
		case 301:
			lidar_state_.a2z_setting.dist_offset = parameter2;
			break;
		case 302:
			lidar_state_.a2z_setting.fov_correction = parameter2;
			break;
		case 303:
			lidar_state_.a2z_setting.reserved02 = parameter2;
			break;
		default:
			break;
	}
}

void ChangeDarkAreaCorrectionSetting(int parameter1, int parameter2)
{
//	uint8_t is_enable_dark_area_crror_correction_;
//	uint8_t dark_area_channel_[32];
//	uint8_t dark_area_channel_count_;
//	uint8_t is_dark_area_channel_changed_;
//	double dark_area_error_cor_param1_;
//	double dark_area_error_cor_param2_;
//	double dark_area_error_cor_param3_;
//	double dark_area_error_cor_param4_;
//	uint8_t is_enable_zero_dist_cor_;
	switch(parameter1)
	{
	case 0 :
		lidar_state_.filter_setting_.dark_area_error_correction_setting_.is_enable_dark_area_crror_correction_ = parameter2;
		if(parameter2 == 1)
			lidar_state_.filter_setting_.dark_area_error_correction_setting_.is_dark_area_channel_changed_ = true;
		break;
	case 1 :
		lidar_state_.filter_setting_.dark_area_error_correction_setting_.is_enable_zero_dist_cor_ = parameter2;
		break;
	case 2 :
		break;
	case 3 :
		break;
	case 4 :
		break;
	case 5 :
		break;

	default :
		break;
	}
}
void ChangeDarkAreaCorrectionSetting_v2(int parameter1, int parameter2)
{
	{
		switch(parameter1)
		{
		case 0 :
			dark_area_state_.darkarea_setting_.is_enable_dark_area_crror_correction_ = parameter2;
			dark_area_state_.darkarea_setting_2.is_enable_dark_area_crror_correction_ = parameter2;
			dark_area_state_.darkarea_setting_3.is_enable_dark_area_crror_correction_ = parameter2;
			if(parameter2 == 1)
			{
				dark_area_state_.darkarea_setting_.is_dark_area_channel_changed_ = true;
				dark_area_state_.darkarea_setting_2.is_dark_area_channel_changed_ = true;
				dark_area_state_.darkarea_setting_3.is_dark_area_channel_changed_ = true;
			}

			break;
		case 1 :
			dark_area_state_.darkarea_setting_.is_enable_zero_dist_cor_ = parameter2;
			dark_area_state_.darkarea_setting_2.is_enable_zero_dist_cor_ = parameter2;
			dark_area_state_.darkarea_setting_3.is_enable_zero_dist_cor_ = parameter2;
			break;
		case 2 :
			break;
		case 3 :
			break;
		case 4 :
			break;
		case 5 :
			break;

		default :
			break;
		}
	}
}
