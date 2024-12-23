/*
 * dt_align.c
 *
 *  Created on: 2023. 3. 27.
 *      Author: AutoL
 */

#include "dt_align.h"

void InitDtAlignInfo(DtAlignInfo* dt_align_info, LidarState* lidar_state)
{
	dt_align_info->start_vth = lidar_state->dt_align_settings_.dt_start > 1000 || lidar_state->dt_align_settings_.dt_start <= 0 ? 200 : lidar_state->dt_align_settings_.dt_start;
	dt_align_info->end_vth = lidar_state->dt_align_settings_.dt_end > 2048 || lidar_state->dt_align_settings_.dt_end <= 0 ? 2048 : lidar_state->dt_align_settings_.dt_end;
	dt_align_info->vth_interval = lidar_state->dt_align_settings_.dt_interval > 100 || lidar_state->dt_align_settings_.dt_interval <= 0 ? 30 : lidar_state->dt_align_settings_.dt_interval;
	dt_align_info->check_interval = lidar_state->dt_align_settings_.dt_check_interval > 10 || lidar_state->dt_align_settings_.dt_check_interval <= 0 ? 1 : lidar_state->dt_align_settings_.dt_check_interval;
	dt_align_info->is_align_started = 0;
	dt_align_info->vth_count_limit = 2;

	for(int ind = 0 ; ind < 16 ; ind++)
	{
		dt_align_info->is_vth_check_finished[ind] = 0;
		dt_align_info->vth_zero_count[ind] = 0;
	}

	//JSON Type Array
	dt_align_arr[0] = "1";
	dt_align_arr[1] = "2";
	dt_align_arr[2] = "3";
	dt_align_arr[3] = "4";
	dt_align_arr[4] = "5";
	dt_align_arr[5] = "6";
	dt_align_arr[6] = "7";
	dt_align_arr[7] = "8";
	dt_align_arr[8] = "9";
	dt_align_arr[9] = "10";
	dt_align_arr[10] = "11";
	dt_align_arr[11] = "12";
	dt_align_arr[12] = "13";
	dt_align_arr[13] = "14";
	dt_align_arr[14] = "15";
	dt_align_arr[15] = "16";

	dt_align_tof_arr[0] = "1t";
	dt_align_tof_arr[1] = "2t";
	dt_align_tof_arr[2] = "3t";
	dt_align_tof_arr[3] = "4t";
	dt_align_tof_arr[4] = "5t";
	dt_align_tof_arr[5] = "6t";
	dt_align_tof_arr[6] = "7t";
	dt_align_tof_arr[7] = "8t";
	dt_align_tof_arr[8] = "9t";
	dt_align_tof_arr[9] = "10t";
	dt_align_tof_arr[10] = "11t";
	dt_align_tof_arr[11] = "12t";
	dt_align_tof_arr[12] = "13t";
	dt_align_tof_arr[13] = "14t";
	dt_align_tof_arr[14] = "15t";
	dt_align_tof_arr[15] = "16t";

	init_hash_table((volatile struct Node**) dt_align_hash_tb);

	for (int i = 0; i < 16; i++) {
		add_item((volatile struct Node**) dt_align_hash_tb, (const char*) dt_align_arr[i], -1);
	}
}

uint8_t DtCheckVth(DtAlignInfo* dt_align_info, int* pCurVthOff)
{
	uint8_t vth_check_finished = 0;
	for(int vth_ind = 0 ; vth_ind < 16 ; vth_ind ++)
	{
		if( (current_vth_arr[vth_ind] > dt_align_info->end_vth * 2) || (current_vth_arr[vth_ind] >= 4000) )
		{
			vth_check_finished = 1;
		}
	}

	if( (*pCurVthOff > dt_align_info->end_vth * 2) || (*pCurVthOff >= 4000) )
	{
		vth_check_finished = 1;
	}

	return vth_check_finished;
}

void SetDtCurVthArr(int channel, int* pCurVthOff, int* pCurSingleVthOff)
{
	if(channel == 0)
	{
		for(int ind = 0; ind < 16; ind ++)
		{
			current_vth_arr[ind] = *pCurVthOff;
		}
	}
	else
	{
		if (channel > 0 && channel <= 16)
		{
			current_vth_arr[channel - 1] = *pCurSingleVthOff;
		}
	}
}

void SetChangeVth(int channel, int vth, int* pCurVthOff, int* pCurSingleVthOff)
{
	if (channel == 0)
	{
		MCP4822CtrlSetVthAll(vth);
		*pCurVthOff = vth;
	}
	else if (channel == 17)
	{
		MCP4801CtrlSetValue(vth);
	}
	else
	{
		MCP4822CtrlSetChannelVth(channel - 1, vth);
		*pCurSingleVthOff = vth;
	}
}

char DtVthApplied()
{
	int ind = 0;
	is_vth_applied = 0;

	for(ind = 0 ; ind < 16 ; ind++)
	{
		if(current_vth_arr[ind] != pre_current_vth_arr[ind])
		{
			is_vth_applied = 1;
		}
		pre_current_vth_arr[ind] = current_vth_arr[ind];
	}
	return is_vth_applied;
}

int DtAlignValue(DtAlignInfo* dt_align_info, int* pCurVthOff, int* pCurSingleVthOff)
{
	int value = 0;
	if(dt_align_info->current_channel == 0)
	{
		value = *pCurVthOff + dt_align_info->vth_interval * 2;
	}
	else
	{
		value = *pCurSingleVthOff + dt_align_info->vth_interval * 2;
	}

	return value;
}

void DtCheckDistance(RawDataBlock* dt_align_result, DtAlignInfo* dt_align_info)
{
	for (int azimuth = 1; azimuth < 5; azimuth++) // azimuth loop
	{
		for (int channel = 0; channel < 16; channel++) // channel loop 16
		{
//			for (int stopcount = 0; stopcount < 3; ++stopcount) // 3stop check
//			{
//				if(stopcount != 0) // 1stop에서 검사 끝
//				{
//					continue;
//				}
//				if(dt_align_result[azimuth * 3 + stopcount].distance_[15-channel].distance <= 0 || dt_align_result[azimuth * 3 + stopcount].distance_[15-channel].distance > MAX_DISTANCE) //LD 렌즈에 굴절로 인한 채널 역상
			if(dt_align_result[azimuth * 3].distance_[15-channel].distance <= 0 || dt_align_result[azimuth * 3].distance_[15-channel].distance > MAX_DISTANCE) //LD 렌즈에 굴절로 인한 채널 역상
				{
					dt_align_info->vth_zero_count[15-channel]++;
				}
//			}
		}
	}

	for(int ind = 0 ; ind < 16 ; ind++)
	{
		if(dt_align_info->is_vth_check_finished[ind] == 1)
		{
			continue;
		}

		if(dt_align_info->vth_zero_count[15-ind] >= dt_align_info->vth_count_limit)
		{
			dt_align_info->is_vth_check_finished[ind] = 1;

			memset( current_key, 0x00, sizeof(current_key) );
			strcpy( current_key, dt_align_arr[ind] );
			int cur_index = hash_func(current_key);
			struct Node* cur_node = dt_align_hash_tb[cur_index];

			if(cur_node != NULL && dt_align_hash_tb[cur_index]->value == -1)
			{
				dt_align_hash_tb[cur_index]->value = current_vth_arr[ind];
			}
		}
	}
}

void ConvertDtAlignResultToBytes(char* dt_align_result_bytes)
{
	//JSON type ( 자세한 JSON 구조는 구글링! )
	/*
	 * {"1":vth, "2":vth, ... "16":vth}
	 * {"1t":distance, "2t":distance, ... "16t":distance}
	 *
	 */
	int i = 0;
	char buf[50] = {0, };
	int dt_hash_value = 0;

	memset( dt_align_result_bytes, 0x00, 1212 );
	strcat( dt_align_result_bytes, "{" );
	for (i = 0; i < CHANNEL_SIZE; i++)
	{
		if ( dt_align_arr[i] != NULL )
		{
			dt_hash_value = hash_func((const char*)dt_align_arr[i]);
			strcat(dt_align_result_bytes, "\"");
			strcat(dt_align_result_bytes, dt_align_arr[15 - i]); //LD 렌즈에 굴절로 인한 채널 역상
			strcat(dt_align_result_bytes, "\"");
			strcat(dt_align_result_bytes, ":");
			memset(buf, 0x00, 50);
//			sprintf(buf, "%d", current_vth_arr[i] / 2);
			sprintf(buf, "%d", dt_align_hash_tb[dt_hash_value]->value / 2);

			if( strlen(buf) == 0 ){
				strcat(dt_align_result_bytes, "0");
			}
			else{
				strcat(dt_align_result_bytes, buf);
			}

			strcat(dt_align_result_bytes, ",");
		}
	}

	RemoveEnd(dt_align_result_bytes);
	strcat(dt_align_result_bytes, "}");
}

void ConvertDtAlignTofToBytes(char* dt_align_result_bytes, volatile RawDataBlock* dt_align_block_data, DtAlignInfo* dt_align_info)
{
//	tcp_packet.data_block[i].channel_data[j].tof = blockData->block_data_bram[packet_index * DATA_BLOCK_SIZE + i].CFVal[j];

	int i = 0;
	char tof_buf[50] = {0, };
	char start_vth_buf[50] = {0, };
	char end_vth_buf[50] = {0, };
	char interval_buf[50] = {0, };
	char chk_interval_buf[50] = {0, };

	memset(dt_align_result_bytes, 0x00, 1212);
	strcat(dt_align_result_bytes, "{");


	//****ToF(distance)
	for(i = 0; i < CHANNEL_SIZE; i++)
	{
		if( dt_align_tof_arr[i] != NULL )
		{
			strcat(dt_align_result_bytes, "\"");
			strcat(dt_align_result_bytes, dt_align_tof_arr[i]);
			strcat(dt_align_result_bytes, "\"");
			strcat(dt_align_result_bytes, ":");
//			memset(tof_buf, 0x00, 50);

			sprintf(tof_buf, "%d", dt_align_block_data->distance_[i].distance);

			if( strlen(tof_buf) == 0 ){
				strcat(dt_align_result_bytes, "0");
			}
			else{
				strcat(dt_align_result_bytes, tof_buf);
			}
			strcat(dt_align_result_bytes, ",");
		}
	}


	/* * Start vth * */
	strcat(dt_align_result_bytes, "\"");
	strcat(dt_align_result_bytes, "Start");
	strcat(dt_align_result_bytes, "\"");
	strcat(dt_align_result_bytes, ":");
//	memset(start_vth_buf, 0x00, 50);

	sprintf(start_vth_buf, "%d", dt_align_info->start_vth);

	if( strlen(start_vth_buf) == 0 ){
		strcat(dt_align_result_bytes, "0");
	}
	else{
		strcat(dt_align_result_bytes, start_vth_buf);
	}
	strcat(dt_align_result_bytes, ",");


	/* * End vth * */
	strcat(dt_align_result_bytes, "\"");
	strcat(dt_align_result_bytes, "End");
	strcat(dt_align_result_bytes, "\"");
	strcat(dt_align_result_bytes, ":");
//	memset(end_vth_buf, 0x00, 50);

	sprintf(end_vth_buf, "%d", dt_align_info->end_vth);

	if( strlen(end_vth_buf) == 0 ){
		strcat(dt_align_result_bytes, "0");
	}
	else{
		strcat(dt_align_result_bytes, end_vth_buf);
	}
	strcat(dt_align_result_bytes, ",");


	/* * Interval * */
	strcat(dt_align_result_bytes, "\"");
	strcat(dt_align_result_bytes, "Interval");
	strcat(dt_align_result_bytes, "\"");
	strcat(dt_align_result_bytes, ":");
//	memset(inv_buf, 0x00, 50);

	sprintf(interval_buf, "%d", dt_align_info->vth_interval);

	if( strlen(interval_buf) == 0 ){
		strcat(dt_align_result_bytes, "0");
	}
	else{
		strcat(dt_align_result_bytes, interval_buf);
	}
	strcat(dt_align_result_bytes, ",");


	/* * Check Interval * */
	strcat(dt_align_result_bytes, "\"");
	strcat(dt_align_result_bytes, "CheckInterval");
	strcat(dt_align_result_bytes, "\"");
	strcat(dt_align_result_bytes, ":");
//	memset(chk_inv_buf, 0x00, 50);

	sprintf(chk_interval_buf, "%d", dt_align_info->check_interval);

	if( strlen(chk_interval_buf) == 0 ){
		strcat(dt_align_result_bytes, "0");
	}
	else{
		strcat(dt_align_result_bytes, chk_interval_buf);
	}
	strcat(dt_align_result_bytes, ",");


	RemoveEnd(dt_align_result_bytes);
	strcat(dt_align_result_bytes, "}");
}
