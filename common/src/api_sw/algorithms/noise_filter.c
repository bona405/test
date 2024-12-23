#include "noise_filter.h"
#include "stdlib.h"
#include "xil_io.h"
//#include "../definitions/app_config.h"

int noise_determined_number = 2;
int tunnel_count = 1;
float noise_reference_value_0 = 0.05;
float noise_reference_value_1 = 0.1;
float noise_reference_value_2 = 0.2;
float noise_reference_value_3 = 0.25;
int noise_min_value = 120;
//int noise_max_value = 300;
int noise_max_value = 330;
int noise_percent_logic_flag = 0;
float noise_percent = 0;
int frame_cnt = 0;
int frame_cnt_value_for_noise = 0;
_Bool IsMask();
uint16_t calc_dis();
void GetIndex();
int Getdistanceindex();

#ifdef SI_P
int valid_distance = 128;
int noise_filter_max_distance_ = 70 * 256;
#else
int valid_distance = 1;
#endif
size_t neighbor_point_indicies_size = 0;
#ifdef SI_P
int Getdistanceindex(unsigned int distance)
{
//	unsigned int result = 0;

	if (distance < 4096) // 16m 40cm
	{
//		result = 256;
		return 256;
	}
	else // 32m 80cm
	{
		return 512;
	}

//	else if (distance < 8192) // 32m 80cm
//	{
//		result = 512;
//	}
//	else
//	{
//		result = 512;
//	}
//	return result;
}
#else
int Getdistanceindex(unsigned int distance)
{
	unsigned int result = 0;

	if (distance < 40) // 16m 40cm
	{
		result = 2;
	}
	else if (distance < 80) // 32m 80cm
	{
		result = 3;
	}
	else
	{
		result = 4;
	}
	return result;
}
#endif

float m_round(float n)
{
	int r;
	if (n >= 0)
	{
		r = n + 0.5;
		return r;
	}
	else
	{
		r = n - 0.5;
		return r;
	}
}

void ResetVthFilterParameter()
{
	noise_count_info_arr[0].vth = 0;
	noise_count_info_arr[1].vth = 0;
	noise_count_info_arr[2].vth = 0;

	noise_count_info_arr[0].average = 10000.0;
	noise_count_info_arr[1].average = 10000.0;
	noise_count_info_arr[2].average = 10000.0;

	noise_count_info_arr[0].average_count = 0;
	noise_count_info_arr[1].average_count = 0;
	noise_count_info_arr[2].average_count = 0;

	noise_count_info_arr[0].noise_count = 0;
	noise_count_info_arr[1].noise_count = 0;
	noise_count_info_arr[2].noise_count = 0;

	noise_count_info_arr[0].max_count = 0;
	noise_count_info_arr[1].max_count = 0;
	noise_count_info_arr[2].max_count = 0;
}

void InitVthFilter(struct LidarState* lidar_state)
{
	noise_reference_value_0 = (float)atoi(lidar_state->filter_setting_.noise_percent) / 100;
	vth_sensitivity = atoi(lidar_state->filter_setting_.vth_sensitivity);
	noise_determined_number = atoi(lidar_state->filter_setting_.valid_point_cnt);
	noise_reference_number = atoi(lidar_state->filter_setting_.noise_cnt_limit);
	noise_min_value = atoi(lidar_state->filter_setting_.noise_min_value);
}

void ApplyVoltageThresholdFilter(int vth_sensitivity, float noise_percent, int noise_min_value, int noise_max_value, int *current_vth_offset,
		int noise_point_cnt, int noise_reference_number, int noise_percent_logic_flag, struct LidarState* lidar_state)
{

	int cur_vth = *current_vth_offset;

	if (pre_noise_reference_number != noise_reference_number)
	{
		ResetVthFilterParameter();
		pre_noise_reference_number = noise_reference_number;
	}


	int v_offset_command_on = 0;
	int noise_percent_state = 0;

	float noise_percent_margin = noise_reference_value_0 - noise_percent;
	int noise_count_margin = noise_reference_number - noise_point_cnt;
	float noise_count_margin_div_1000 = 0;
	int noise_count_margin_abs = abs(noise_count_margin);

	if (noise_reference_number == 0)
	{
		noise_reference_number = 100;
	}

	int ctrl_vth_offset = 0;
	if (noise_point_cnt > (float) noise_reference_number * 1.2 || noise_point_cnt < (float) noise_reference_number * 0.8)
	{
		if (noise_count_margin_abs > MAX_NOISE_CNT)
		{
			if (noise_count_margin < 0)
				ctrl_vth_offset = MAX_VTH_OFFSET;
			else
				ctrl_vth_offset = -MAX_VTH_OFFSET;
		}
		else if (noise_count_margin_abs > MIN_NOISE_CNT)
		{
			if (noise_count_margin < 0)
				ctrl_vth_offset += m_round((float) (noise_count_margin_abs) / 1000);
			else
				ctrl_vth_offset -= m_round((float) (noise_count_margin_abs) / 1000);
		}
		else
		{
			if (noise_count_margin < 0)
				ctrl_vth_offset = 1;
			else
				ctrl_vth_offset = -1;
		}
//		else
//		{
//			float vth_offset = m_round((float) (noise_count_margin_abs - 16) / 64);
//			if (noise_count_margin < 0)
//				ctrl_vth_offset += vth_offset;
//			else
//				ctrl_vth_offset -= vth_offset;
//		}
	}
//	else
//	{
//		if (noise_count_margin_abs > MAX_NOISE_CNT)
//		{
//			ctrl_vth_offset = MAX_VTH_OFFSET;
//		}
//		else if (noise_count_margin_abs > MIN_NOISE_CNT)
//		{
//			if (noise_count_margin < 0)
//				ctrl_vth_offset += m_round((float) (noise_count_margin_abs + 151) / 581);
//			else
//				ctrl_vth_offset -= m_round((float) (noise_count_margin_abs + 151) / 581);
//		}
//		else
//		{
//			float vth_offset = m_round((float) (noise_count_margin_abs - 16) / 64);
//			if (noise_count_margin < 0)
//				ctrl_vth_offset += vth_offset;
//			else
//				ctrl_vth_offset -= vth_offset;
//		}
//	}

	if (noise_percent_margin > 0 && noise_percent_margin < 0.01)
	{
		noise_percent_margin = 0.01;
	}
	if (noise_percent_margin < 0 && noise_percent_margin > -0.01)
	{
		noise_percent_margin = -0.01;
	}

	if (noise_percent_logic_flag == 1)
	{
		*current_vth_offset = *current_vth_offset - (int) m_round(noise_percent_margin * 100 * vth_sensitivity);
	}
	else
	{

		*current_vth_offset = *current_vth_offset + ctrl_vth_offset * vth_sensitivity;
	}

	uint8_t is_member_of_arr = 0;

	int max_average = noise_count_info_arr[0].average;
	int min_average = noise_count_info_arr[0].average;
	int max_average_ind = 0;
	int min_average_ind = 0;

	for (int ind = 0; ind < 3; ind++)
	{
		if (noise_count_info_arr[ind].vth == cur_vth)
		{
			if(noise_count_info_arr[ind].average_count > 5)
			{
				if (noise_count_info_arr[ind].average * 1.3 < noise_count_margin_abs)
				{
					ResetVthFilterParameter();
					break;
				}
			}
		}
	}


	for (int ind = 0; ind < 3; ind++)
	{
		if (noise_count_info_arr[ind].vth == *current_vth_offset)
		{
			is_member_of_arr = 1;
		}
	}



	for (int ind = 0; ind < 3; ind++)
	{
		if (noise_count_info_arr[ind].vth == cur_vth)
		{

			is_member_of_arr = 1;

			if(noise_count_info_arr[ind].average_count > 5)
				noise_count_info_arr[ind].average_count--;

			noise_count_info_arr[ind].average = (float) (noise_count_info_arr[ind].average * noise_count_info_arr[ind].average_count
					+ noise_count_margin_abs) / (float) (noise_count_info_arr[ind].average_count + 1);
			noise_count_info_arr[ind].average_count++;
			noise_count_info_arr[ind].vth = cur_vth;

			if(noise_count_margin_abs > noise_count_info_arr[ind].noise_count)
				noise_count_info_arr[ind].noise_count = noise_count_margin_abs;
		}
	}

	for (int ind = 0; ind < 3; ind++)
	{
		if (min_average > noise_count_info_arr[ind].average)
		{
			min_average = noise_count_info_arr[ind].average;
			min_average_ind = ind;
		}

		if (max_average < noise_count_info_arr[ind].average)
		{
			max_average = noise_count_info_arr[ind].average;
			max_average_ind = ind;
		}
	}

	if (is_member_of_arr == 0)
	{
		uint8_t is_arr_full = 1;
		for (int ind = 0; ind < 3; ind++)
		{
			if(noise_count_info_arr[max_average_ind].vth == 0)
			{
				is_arr_full = 0;
				break;
			}
		}
		if(is_arr_full)
		{
			ResetVthFilterParameter();
		}
		noise_count_info_arr[max_average_ind].average = (float) noise_count_margin_abs;
		noise_count_info_arr[max_average_ind].average_count = 1;
		noise_count_info_arr[max_average_ind].vth = cur_vth;
		noise_count_info_arr[max_average_ind].noise_count = noise_count_margin_abs;
	}
	else
	{
		if(noise_count_info_arr[min_average_ind].average_count > 3)
		{
			*current_vth_offset = noise_count_info_arr[min_average_ind].vth;
		}
	}

	if(pre_vth == *current_vth_offset)
	{
		noise_init_cnt++;
		if(noise_init_cnt > 1000)
					noise_init_cnt = 10;
	}
	else
	{

			noise_init_cnt = 0;
	}
	pre_vth = *current_vth_offset;

	v_offset_command_on = 1;
	noise_percent_state = 5;

	if (noise_percent_state != 0 && v_offset_command_on == 1)
	{
		if (*current_vth_offset < noise_min_value)
			*current_vth_offset = noise_min_value;
		if (*current_vth_offset > noise_max_value)
			*current_vth_offset = noise_max_value;

//		MCP4822CtrlSetVthAll(*current_vth_offset);
		if(lidar_state_.filter_setting_.is_noise_on == 1)
		{

		}
		else{
			MCP4822CtrlSetVthAll(*current_vth_offset);
		}

//		Xil_Out32(DAC_Write_Done_Addr, 0x01);
//		usleep(1);
//		Xil_Out32(DAC_Write_Done_Addr, 0x00);
	}
}
//
//int ApplyNoiseFilter2(TupleII noise_point_index[FRAME_DATA_COUNT*CHANNEL_SIZE], volatile RawDataBlock* block_data_total_test,
//		int AzimuthCount, unsigned int *noise_point_count, unsigned int *frame_point_count)
//{
//	uint32_t noise_point_index_cnt = 0;
//	int valid_cnt = 0;
//	int frame_point_cnt = *frame_point_count;
//	int noise_point_cnt = 0;
//	int StopCount = 3;
//	_Bool isValid = FALSE;
//	uint32_t current_point_distance;
//	int azimuthStopIndex = 0;
//	int channelIndex = 0;
//	uint32_t compare_point_distance = 0;
////	block_data* block_data_bram_noise = (block_data*)block_data_total_test->block_data_bram;
//	int azimuth_index = 0;;
//	for (int i = 0; i < AzimuthCount; ++i) // azimuth loop
//	{
//		for (int k = 0; k < StopCount; ++k)
//		{
//			azimuth_index = i * StopCount + k;
//			for (int j = 0; j < 16; ++j) // channel loop 16
//			{
//				current_point_distance = block_data_total_test[azimuth_index].distance_[j].distance;
////				unsigned int current_point_distance = calc_dis(block_data_bram_noise->block_data_bram[i * StopCount + k].data_points[j]);
//
//				valid_cnt = 0;
//
//				if (current_point_distance > 0 && current_point_distance < noise_filter_max_distance_)
//				{
//					frame_point_cnt++;
//				}
//				else
//				{
//
////					block_data_bram_noise[i * StopCount + k].data_points[j] = 0;
//					continue;
//				}
//
////				_Bool isValid = FALSE;
//				isValid = FALSE;
//				for (int l = 0; l < 8; ++l)
//				{
//					if (IsMask(l, i, j, AzimuthCount) == TRUE)
//						continue;
//
//					for (int m = 0; m < StopCount; ++m)
//					{
////						int azimuthStopIndex = 0;
////						int channelIndex = 0;
//						azimuthStopIndex = 0;
//						channelIndex = 0;
//						GetIndex(l, i, j, m, &azimuthStopIndex, &channelIndex, AzimuthCount);
//
////						uint16_t compare_point_distance = calc_dis(block_data_bram_noise->block_data_bram[azimuthStopIndex].data_points[channelIndex]);
//						compare_point_distance = block_data_total_test[azimuthStopIndex].distance_[channelIndex].distance;
//
//						if (compare_point_distance > 0)
//						{
//							if (abs(current_point_distance - compare_point_distance) <= Getdistanceindex(current_point_distance))
//							{
//								valid_cnt++;
//
//								if (valid_cnt == noise_determined_number)
//								{
//									isValid = TRUE;
//									break;
//								}
//
//							}
//						}
//					}
//
//					if (isValid == TRUE)
//					{
//						valid_cnt = 0;
//						break;
//					}
//				}
//
//				if (isValid == FALSE)
//				{
//					noise_point_index[noise_point_index_cnt].first = azimuth_index;
//					noise_point_index[noise_point_index_cnt].second = j;
//					noise_point_index_cnt++;
////					block_data_total_test->block_data_bram[i * StopCount + k].data_points[j] = 0;
//					noise_point_cnt++;
//				}
//			}
//		}
//	}
//
//	*noise_point_count = noise_point_cnt;
//	*frame_point_count = frame_point_cnt;
//
//	for(int i=0;i<noise_point_index_cnt;i++)
//	{
//		block_data_total_test[noise_point_index[i].first].distance_[noise_point_index[i].second].distance = 0;
//	}
//
//}
//
//
//int ApplyNoiseFilter(volatile RawDataBlock* block_data_bram_noise, volatile RawDataBlock* block_data_total_test,
//		int AzimuthCount, int *noise_point_count, int *frame_point_count)
//{
//	int valid_cnt = 0;
//	int frame_point_cnt = *frame_point_count;
//	int noise_point_cnt = 0;
//	int StopCount = 3;
//	_Bool isValid = FALSE;
//	uint32_t current_point_distance;
//	int azimuthStopIndex = 0;
//	int channelIndex = 0;
//	uint32_t compare_point_distance = 0;
//	for (int i = 0; i < AzimuthCount; ++i) // azimuth loop
//	{
//		for (int k = 0; k < StopCount; ++k)
//		{
//
//			for (int j = 0; j < 16; ++j) // channel loop 16
//			{
//				current_point_distance = block_data_bram_noise[i * StopCount + k].distance_[j].distance;
////				unsigned int current_point_distance = calc_dis(block_data_bram_noise->block_data_bram[i * StopCount + k].data_points[j]);
//
//				valid_cnt = 0;
//
//				if (current_point_distance >= valid_distance)
//				{
//					frame_point_cnt++;
//				}
//				else
//				{
//					block_data_bram_noise[i * StopCount + k].distance_[j].distance = 0;
//					continue;
//				}
//
////				_Bool isValid = FALSE;
//				isValid = FALSE;
//				for (int l = 0; l < 8; ++l)
//				{
//					if (IsMask(l, i, j, AzimuthCount) == TRUE)
//						continue;
//
//					for (int m = 0; m < StopCount; ++m)
//					{
////						int azimuthStopIndex = 0;
////						int channelIndex = 0;
//						azimuthStopIndex = 0;
//						channelIndex = 0;
//						GetIndex(l, i, j, m, &azimuthStopIndex, &channelIndex, AzimuthCount);
//
////						uint16_t compare_point_distance = calc_dis(block_data_bram_noise->block_data_bram[azimuthStopIndex].data_points[channelIndex]);
//						compare_point_distance = block_data_bram_noise[azimuthStopIndex].distance_[channelIndex].distance;
//
//						if (compare_point_distance >= 1)
//						{
//							if (abs(current_point_distance - compare_point_distance) <= Getdistanceindex(current_point_distance))
//							{
//								valid_cnt++;
//
//								if (valid_cnt == noise_determined_number)
//								{
//									isValid = TRUE;
//									break;
//								}
//
//							}
//						}
//					}
//
//					if (isValid == TRUE)
//					{
//						valid_cnt = 0;
//						break;
//					}
//				}
//
//				if (isValid == FALSE)
//				{
//					block_data_total_test[i * StopCount + k].distance_[j].distance = 0;
//					noise_point_cnt++;
//				}
//			}
//		}
//	}
//
//	*noise_point_count = noise_point_cnt;
//	*frame_point_count = frame_point_cnt;
//
//}
//
//_Bool IsMask(int position, int azimuthIndex, int channelIndex, int AzimuthCount)
//{
//	_Bool isTop = TRUE;
//
//	if (azimuthIndex < BOTTOM_AZIMUTH_COUNT)
//	{
//		isTop = FALSE;
//	}
//	else
//	{
//		isTop = TRUE;
//	}
//
//	switch (position)
//	{
//		case 0:
//			if (azimuthIndex == 0 || azimuthIndex == BOTTOM_AZIMUTH_COUNT || (isTop == TRUE && channelIndex == 15))
//				return TRUE;
//			break;
//		case 1:
//			if (isTop == TRUE && channelIndex == 15)
//				return TRUE;
//			break;
//		case 2:
//			if (azimuthIndex == AzimuthCount - 1 || azimuthIndex == BOTTOM_AZIMUTH_COUNT - 1 || (isTop == TRUE && channelIndex == 15))
//				return TRUE;
//			break;
//		case 3:
//			if (azimuthIndex == AzimuthCount - 1 || azimuthIndex == BOTTOM_AZIMUTH_COUNT - 1)
//				return TRUE;
//			break;
//		case 4:
//			if (azimuthIndex == AzimuthCount - 1 || azimuthIndex == BOTTOM_AZIMUTH_COUNT - 1 || (isTop == FALSE && channelIndex == 0))
//				return TRUE;
//			break;
//		case 5:
//			if (isTop == FALSE && channelIndex == 0)
//				return TRUE;
//			break;
//		case 6:
//			if (azimuthIndex == 0 || azimuthIndex == BOTTOM_AZIMUTH_COUNT || (isTop == FALSE && channelIndex == 0))
//				return TRUE;
//			break;
//		case 7:
//			if (azimuthIndex == 0 || azimuthIndex == BOTTOM_AZIMUTH_COUNT)
//				return TRUE;
//			break;
//	}
//	return FALSE;
//}
//
//#ifdef SI_P
//uint16_t calc_dis(uint32_t in_val)
//{
//	uint16_t crs = (uint16_t) (in_val) ;
////	float fine = (float)(in_val & 0x00003fff) * 0.00002774f;
////	float tof = crs-fine;
////	uint16_t tof_cal_val = tof * 1000;
//	return crs;
//}
//#else
//uint16_t calc_dis(uint32_t in_val)
//{
//	uint16_t crs = (uint16_t) ((in_val >> 14) & 0x000003ff);
////	float fine = (float)(in_val & 0x00003fff) * 0.00002774f;
////	float tof = crs-fine;
////	uint16_t tof_cal_val = tof * 1000;
//	return crs;
//}
//#endif
//
//inline void GetIndex(int position, int azimuthIndex, int channelIndex, int StopIndex, int* targetAzimuthStopIndex, int* targetChannelIndex, int AzimuthCount)
//{
//	_Bool isTop = TRUE;
//	int newAzimuthIndex = 0;
//	int StopCount = 3;
//
//	if (azimuthIndex < BOTTOM_AZIMUTH_COUNT)
//	{
//		isTop = FALSE;
//		newAzimuthIndex = (azimuthIndex + BOTTOM_AZIMUTH_COUNT);
//	}
//	else
//	{
//		isTop = TRUE;
//		newAzimuthIndex = (azimuthIndex - BOTTOM_AZIMUTH_COUNT);
//	}
//
//	switch (position)
//	{
//		case 0:
//
//			if (isTop == TRUE)
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex - 1) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex + 1;
//			}
//			else
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex - 1) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex + 0;
//			}
//			break;
//		case 1:
//			if (isTop == TRUE)
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex - 0) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex + 1;
//			}
//			else
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex - 0) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex + 0;
//			}
//			break;
//		case 2:
//			if (isTop == TRUE)
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex + 1) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex + 1;
//			}
//			else
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex + 1) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex + 0;
//			}
//			break;
//		case 3:
//			*targetAzimuthStopIndex = (azimuthIndex + 1) * StopCount + StopIndex;
//			*targetChannelIndex = channelIndex - 0;
//			break;
//		case 4:
//			if (isTop == TRUE)
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex + 1) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex + 0;
//			}
//			else
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex + 1) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex - 1;
//			}
//			break;
//		case 5:
//			if (isTop == TRUE)
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex + 0) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex + 0;
//			}
//			else
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex + 0) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex - 1;
//			}
//			break;
//		case 6:
//			if (isTop == TRUE)
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex - 1) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex + 0;
//			}
//			else
//			{
//				*targetAzimuthStopIndex = (newAzimuthIndex - 1) * StopCount + StopIndex;
//				*targetChannelIndex = channelIndex - 1;
//			}
//			break;
//		case 7:
//			*targetAzimuthStopIndex = (azimuthIndex - 1) * StopCount + StopIndex;
//			*targetChannelIndex = channelIndex + 0;
//			break;
//	}
//}
//
//void SetNeighborPointIndexForCloseRange()
//{
//	int top_bottom_index = 0;
//	int azimuth_index_offset = 0;
//	TupleII* frame_neighbor_point_index_p;
//	for(size_t azimuth_index = 0 ; azimuth_index < FRAME_DATA_COUNT ; azimuth_index++)
//	{
//		if(azimuth_index >= TOTAL_AZIMUTH_COUNT)
//		{
//			top_bottom_index = 1;
//			azimuth_index_offset = 0;
//		}
//		else
//		{
//			top_bottom_index = 0;
//			azimuth_index_offset = 0;
//		}
//		for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; channel_index++)
//		{
//
//			TupleII target_point_index;
//			target_point_index.first = azimuth_index;
//			target_point_index.second = channel_index;
//
//			//add myself
//			neighbor_point_indicies_size = 0;
//			int azimuth_index = target_point_index.first - azimuth_index_offset;
//			if(azimuth_index < 0 || azimuth_index >= FRAME_DATA_COUNT)
//				azimuth_index = -1;
//
//			frame_neighbor_point_indicies[neighbor_point_indicies_size].first = azimuth_index;
//			frame_neighbor_point_indicies[neighbor_point_indicies_size].second = target_point_index.second;
//			neighbor_point_indicies_size++;
//
//			//add others
//			for(int dist_neighbor = 1 ; dist_neighbor <= 3 ; ++dist_neighbor)
//			{
//				for(int j = dist_neighbor * -1 ; j <= dist_neighbor ; ++j)
//				{
//					for(int k = dist_neighbor * -1 ; k <= dist_neighbor ; ++k)
//					{
//						if(abs(j) + abs(k) == dist_neighbor)
//						{
//							int azimuth_index = target_point_index.first + 3 * j - azimuth_index_offset;
//							if(top_bottom_index == 0)
//							{
//								if(azimuth_index < 0 || azimuth_index >= FRAME_DATA_COUNT)
//								{
//									azimuth_index = -1;
//								}
//								else
//								{
//									if(abs(k) & 1)
//										azimuth_index += TOTAL_AZIMUTH_COUNT;
//								}
//							}
//							else
//							{
//								if(azimuth_index < 0 || azimuth_index >= FRAME_DATA_COUNT)
//								{
//										azimuth_index = -1;
//								}
//								else
//								{
//									if(abs(k) & 1)
//										azimuth_index -= TOTAL_AZIMUTH_COUNT;
//								}
//							}
//
//							if(azimuth_index < 0 || azimuth_index >= FRAME_DATA_COUNT)
//								azimuth_index = -1;
//
//							frame_neighbor_point_indicies[neighbor_point_indicies_size].first = azimuth_index;
//
//							int channel_offset;
//							if(top_bottom_index == 0)
//							{
//								if(k > 0)
//									channel_offset = k * 0.5;
//								else
//									channel_offset = (k - 1) * 0.5;
//							}
//							else
//							{
//								if(k > 0)
//									channel_offset = (k + 1) * 0.5;
//								else
//									channel_offset = k * 0.5;
//							}
//
//							int channel_index = target_point_index.second + channel_offset;
//
//							if(channel_index < 0 || channel_index >= 16)
//								channel_index = -1;
//
//							frame_neighbor_point_indicies[neighbor_point_indicies_size].second = channel_index;
//
//							//				if(abs(k) % 2 == 1)
//							//					neighbor.top_bottom_index = (target_point_index.top_bottom_index == 0) ? 1 : 0;
//							//				else
//							//					neighbor.top_bottom_index = target_point_index.top_bottom_index;
//
//							neighbor_point_indicies_size++;
//						}
//					}
//				}
//			}
//			frame_neighbor_point_index_p = frame_neighbor_point_indicies;
//			memcpy(frame_neighbor_point_index_list[azimuth_index][channel_index], frame_neighbor_point_index_p, MAX_NEIGHBOR_POINT_SIZE_FOR_CLOSE_RANGE * sizeof(TupleII));
//		}
//	}
//
//
//}
