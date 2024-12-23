/*
 * dtc.c
 *
 *  Created on: 2021. 11. 5.
 *      Author: ProDesk
 */
#include "dtc.h"
//#include "sleep.h"
#include "stdio.h"
#include <stdlib.h>
//#include "../definitions/definitions.h"

//
//void instruction_command(int cal_val[32], int* clip_min, int* bd_min, int* bd_max, int* Add_TH_Temp_Val, int* MOTOR_CAL_VAL_BOTTOM,
//		int* current_vth_offset, int* noise_determined_value, float* noise_reference_value, int* vth_sensitivity, int* noise_reference_number, int* noise_percent_logic_flag)
//{
//	if (Instr_flag == 1)
//	{
//		if (Instr == 0)
//		{
//			if (Ch == 0)
//			{
//				for (int i = 0; i < 16; ++i)
//				{
//					Xil_Out32(DAC_BRAM_Address + 0x04 * i, Value * 2);
//
//				}
//				*current_vth_offset = Value;
//			}
//			else if (Ch == 17)
//			{
//				Xil_Out32(DAC_BRAM_Address + 0x04 * 16, Value);
//			}
//			else
//			{
//				Xil_Out32(DAC_BRAM_Address + 0x04 * (Ch - 1), Value * 2);
//			}
//		}
//		else if (Instr == 1)
//		{
//			if (Ch == 0)
//			{
//				for (int i = 0; i < 16; ++i)
//				{
//					cal_val[i] = Value;
//					cal_val[i + 16] = Value;
//				}
//			}
//			else if (0 < Ch && Ch < 33)
//			{
//				cal_val[Ch - 1] = Value;
//			}
//		}
//		else if (Instr == 3)
//		{
//			if (Ch == 0)
//			{
//				MyIpEmBufDelaySet(Value);
//			}
//		}
//		else if (Instr == 2)
//		{
//			if (Ch == 0)
//			{
//				*clip_min = Value;
//			}
//			else if (Ch == 1)
//			{
//				*bd_min = Value;
//			}
//			else if (Ch == 2)
//			{
//				*bd_max = Value;
//			}
//		}
//		else if (Instr == 4)
//		{
//			if (Ch == 0)
//			{
//				Xil_Out32(STOP_START_CNT_Address, Value);
//			}
//		}
//		else if (Instr == 6) // Add TH Value
//		{
//			if (0 <= Ch && Ch <= 16)
//			{
//				*Add_TH_Temp_Val = 0;
//				*Add_TH_Temp_Val = (Value << 7);
//				*Add_TH_Temp_Val |= Ch << 2;
//				*Add_TH_Temp_Val |= 0x02;
//				Xil_Out32(DAC_Write_Done_Address, *Add_TH_Temp_Val);
//			}
//		}
//		else if (Instr == 7) // Add TH Value
//		{
//			if (Ch == 0)
//			{
//				*MOTOR_CAL_VAL_BOTTOM = Value * 234;
//			}
//			if (Ch == 1)
//			{
//				//			NOISE_FILTER_ON = Value;
//			}
//		}
//		else if (Instr == 8)
//		{
//			if (Ch == 0)
//			{
//				*noise_reference_value = (float)Value / 100;
//			}
//			else if(Ch == 1)
//			{
//				*vth_sensitivity = (int)Value;
//			}
//			else if(Ch == 2)
//			{
//				*noise_determined_value = Value;
//			}
//			else if(Ch == 3)
//			{
//				*noise_reference_number = Value;
//			}
//			else if(Ch == 4)
//			{
//				*noise_percent_logic_flag = Value;
//			}
//		}
//		else if(Instr == 9)
//		{
//			switch (Ch) {
//				case 0:
//					gd_parameter_.sensor_height_ = Value;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_GHOST_INFO), (void *)&gd_parameter_, sizeof(GDParameter));
//					break;
//				case 1:
//					gd_parameter_.angle_max_1_ = Value * 0.1;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_GHOST_INFO), (void *)&gd_parameter_, sizeof(GDParameter));
//					break;
//				case 2:
//					gd_parameter_.angle_max_2_ = Value * 0.1;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_GHOST_INFO), (void *)&gd_parameter_, sizeof(GDParameter));
//					break;
//				case 3:
//					gd_parameter_.height_min_1_ = Value * 0.1;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_GHOST_INFO), (void *)&gd_parameter_, sizeof(GDParameter));
//					break;
//				case 4:
//					gd_parameter_.height_min_2_ = Value * 0.1;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_GHOST_INFO), (void *)&gd_parameter_, sizeof(GDParameter));
//					break;
//				case 5:
//					gd_parameter_.distance_diff_ = Value;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_GHOST_INFO), (void *)&gd_parameter_, sizeof(GDParameter));
//					break;
//				case 6:
//					gd_parameter_.abnormal_distance_check_th_ = Value;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_GHOST_INFO), (void *)&gd_parameter_, sizeof(GDParameter));
//					break;
//				case 7:
//					if_parameter_.width_ = Value;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_IF_INFO), (void *)&if_parameter_, sizeof(IFParameter));
//					break;
//				case 8:
//					if_parameter_.height_ = Value;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_IF_INFO), (void *)&if_parameter_, sizeof(IFParameter));
//					break;
//				case 9:
//					if_parameter_.max_range_ = Value * 0.1;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_IF_INFO), (void *)&if_parameter_, sizeof(IFParameter));
//					break;
//				case 10:
//					if_parameter_.min_range_ = Value * 0.1;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_IF_INFO), (void *)&if_parameter_, sizeof(IFParameter));
//					break;
//				case 11:
//					if_parameter_.th_g_ = Value * 0.1;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_IF_INFO), (void *)&if_parameter_, sizeof(IFParameter));
//					break;
//				case 12:
//					if_parameter_.sigma_ = Value * 0.1;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_IF_INFO), (void *)&if_parameter_, sizeof(IFParameter));
//					break;
//				case 13:
//					if_parameter_.deltaR_ = Value * 0.1;
//					memcpy((void *)(SHARED_MEM_OFFSET_FOR_IF_INFO), (void *)&if_parameter_, sizeof(IFParameter));
//					break;
//				case 14:
//					rg_parameter_.z_offset_= Value * 0.1;
//					break;
//				case 15:
//					rg_parameter_.z_offset_2_= Value * 0.1;
//					break;
//				case 16:
//					rg_parameter_.x_offset_= Value;
//					break;
//				case 17:
//					rg_parameter_.horizontal_check_num_= Value;
//					break;
//				case 18:
//					rg_parameter_.vertical_check_num_= Value;
//					break;
//				case 19:
//					rg_parameter_.is_ground_remove_on= Value;
//					break;
//				default:
//					break;
//			}
//		}
//		else if(Instr == 10)
//		{
//			Xil_Out32(oDetection_Offset, Value);
//		}
//
//		if (Instr == 0)
//		{
//			Xil_Out32(DAC_Write_Done_Address, 0x01);
//			usleep(1);
//			Xil_Out32(DAC_Write_Done_Address, 0x00);
//		}
//		Instr = 0;
//		Ch = 0;
//		Value = 0;
//		Instr_flag = 0;
//	}
//}
//
//void dtc_initialize(int* select_val,
//		int* nb_command, int* dtc_command, int* initial_command, int* count_num, int* MOTOR_CAL_VAL_BOTTOM, int* initial_motor_value,
//		int* Add_TH_Temp_Val, int *current_vth_offset, struct LidarState_Ch lidar_state_Ch)
//{
//	if (*select_val == 0)
//	{
//		*nb_command = 0;
//		*dtc_command = 0;
//		*initial_command = 0;
//	}
//
//	if (*count_num < 1000)
//	{
//		*count_num = *count_num + 1;
//	}
//
//	if (*count_num == 20)
//	{
//		*select_val = 1;
//		*MOTOR_CAL_VAL_BOTTOM = *initial_motor_value * 234;
//	}
//	else if (*count_num == 50)
//	{
//		*nb_command = 1;
//	}
//	else if (*count_num == 80)
//	{
//		*dtc_command = 0;
//		*initial_command = 1;
//	}
//
//	if (*nb_command == 1)
//	{
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 16, 162);		// DT default
//		Xil_Out32(DAC_Write_Done_Address, 0x01);
//		usleep(1);
//		Xil_Out32(DAC_Write_Done_Address, 0x00);
//		*nb_command = 0;
//	}
//	else if (*initial_command == 1 && *count_num == 90)
//	{
//
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 0, (atoi(lidar_state_Ch.th_ch1))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 1, (atoi(lidar_state_Ch.th_ch2))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 2, (atoi(lidar_state_Ch.th_ch3))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 3, (atoi(lidar_state_Ch.th_ch4))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 4, (atoi(lidar_state_Ch.th_ch5))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 5, (atoi(lidar_state_Ch.th_ch6))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 6, (atoi(lidar_state_Ch.th_ch7))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 7, (atoi(lidar_state_Ch.th_ch8))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 8, (atoi(lidar_state_Ch.th_ch9))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 9, (atoi(lidar_state_Ch.th_ch10))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 10, (atoi(lidar_state_Ch.th_ch11))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 11, (atoi(lidar_state_Ch.th_ch12))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 12, (atoi(lidar_state_Ch.th_ch13))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 13, (atoi(lidar_state_Ch.th_ch14))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 14, (atoi(lidar_state_Ch.th_ch15))*2);
//		Xil_Out32(DAC_BRAM_Address + 0x04 * 15, (atoi(lidar_state_Ch.th_ch16))*2);
//		Xil_Out32(DAC_Write_Done_Address, 0x01);
//		usleep(1);
//
//		Xil_Out32(DAC_Write_Done_Address, 0x00);
//		*initial_command = 0;
//	}
//	else if (*dtc_command == 1 && *count_num == 100)
//	{
////		Xil_Out32(oDTC_EN, 1);
////		xil_printf("oDTC_EN : %d\n", Xil_In32(oDTC_EN));
////		Xil_Out32(oSel_Stop_CNT, 1);
////		xil_printf("oSel_Stop_CNT : %d\n", Xil_In32(oSel_Stop_CNT));
////
////		*dtc_command = 0;
////		*count_num = 1000;
//	}
//}
