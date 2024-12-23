/*
 * calibration.c
 *
 *  Created on: 2022. 7. 18.
 *      Author: HP
 */
#include "routing_delay_calibration.h"

int clip_min_temp = 0;
int initial = 0;
uint8_t Detection_value = 60;
int cal_detection_done = 0;
int cal_detection_range_result = 0;
int done_cnt = 0;

extern struct LidarState lidar_state_;
extern struct LidarState_Ch lidar_state_Ch;
int routing_calibration(int cal_val[], ConvertedDataBlock* block_data_bram_noise, u32 fov_data_block_count)
{
	u32 bottom_azimuth_count = fov_data_block_count / 6;
	int tofs_32[32] = {0, };
	for(int channel_index = 0 ; channel_index < 16 ; ++channel_index) // channel loop 16
	{
		for(int azimuth_index = 0 ; azimuth_index < bottom_azimuth_count ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			if(azimuth_index == bottom_azimuth_count/2)
			{
				int current_point_distance = block_data_bram_noise[azimuth_index * 3 + 0].distance_[channel_index] *256;
				tofs_32[channel_index] =current_point_distance;
			}
		}

		for(int azimuth_index = bottom_azimuth_count ; azimuth_index < bottom_azimuth_count*2 ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			if(azimuth_index == bottom_azimuth_count + bottom_azimuth_count/2)
			{
				int current_point_distance = block_data_bram_noise[azimuth_index * 3 + 0].distance_[channel_index] *256;
				tofs_32[channel_index+16] =current_point_distance;
			}
		}
	}

	int tof_32_temp[32] = {0,};
	for(int idx = 0 ; idx < 32 ; idx++)
	{
		tof_32_temp[idx] = tofs_32[idx];
	}

	for(int idx = 0 ; idx < 31 ; ++idx)
	{
		for(int idx2 = idx + 1 ; idx2 < 32 ; ++idx2)
		{
			if(tof_32_temp[idx] > tof_32_temp[idx2])
			{
				int temp = tof_32_temp[idx];
				tof_32_temp[idx] = tof_32_temp[idx2];
				tof_32_temp[idx2] = temp;
			}
		}
	}

	int total_avg = 0;

	for(int i = 0 ; i < 20 ; i++)
	{
		total_avg += tof_32_temp[i + 6];
	}

	total_avg /= 20;

	int temp_test[32] = {0, };
	for(int k = 0 ; k < 32 ; k++)
	{
		temp_test[k] = tofs_32[k] - total_avg;
	}


	for(int k = 0 ; k < 32 ; k++)
	{
		cal_val[k] -= temp_test[k];
	}


//		int debnu2 = 0;
//		if(debnu2)
//		{
//			if(total_avg != dis_300)
//			{
//				clip_min += total_avg - dis_300;
//			}
//		}
		//			int debnu3 = 0;
		//			if(debnu3)
		//			{
		//				int testtemp = 0;
		//				for(int k = 0; k<32; k++)
		//				{
		//					cal_val[k] +=  testtemp;
		//				}
		//			}

//	print(total_avg);
	//flash
	int Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] =
	{ 0, };
	int tempdelay = cal_val[0];
//	char key11[30] = "ch1";
//	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));
	//strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

	tempdelay = cal_val[1];
	memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

	tempdelay = cal_val[2];
	memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

	tempdelay = cal_val[3];
	memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

	tempdelay = cal_val[4];
	memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

	tempdelay = cal_val[5];
	memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

	tempdelay = cal_val[6];
	memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

	tempdelay = cal_val[7];
	memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

	tempdelay = cal_val[8];
	memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

	tempdelay = cal_val[9];
	memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

	tempdelay = cal_val[10];
	memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

	tempdelay = cal_val[11];
	memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

	tempdelay = cal_val[12];
	memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

	tempdelay = cal_val[13];
	memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

	tempdelay = cal_val[14];
	memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

	tempdelay = cal_val[15];
	memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

	tempdelay = cal_val[16];
	memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

	tempdelay = cal_val[17];
	memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

	tempdelay = cal_val[18];
	memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

	tempdelay = cal_val[19];
	memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

	tempdelay = cal_val[20];
	memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

	tempdelay = cal_val[21];
	memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

	tempdelay = cal_val[22];
	memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

	tempdelay = cal_val[23];
	memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

	tempdelay = cal_val[24];
	memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

	tempdelay = cal_val[25];
	memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

	tempdelay = cal_val[26];
	memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

	tempdelay = cal_val[27];
	memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

	tempdelay = cal_val[28];
	memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

	tempdelay = cal_val[29];
	memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

	tempdelay = cal_val[30];
	memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

	tempdelay = cal_val[31];
	memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}



	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	return 0;
}

int routing_calibration2(int cal_val[], ConvertedDataBlock* block_data_bram_noise, u32 fov_data_block_count)
{
	u32 bottom_azimuth_count = fov_data_block_count / 6;
	int tofs_32[32] = {0, };
	for(int channel_index = 0 ; channel_index < 16 ; ++channel_index) // channel loop 16
	{
		for(int azimuth_index = 0 ; azimuth_index < bottom_azimuth_count ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			if(azimuth_index == bottom_azimuth_count/2)
			{
				int current_point_distance = block_data_bram_noise[azimuth_index * 3 + 0].distance_[channel_index] *256;
				tofs_32[channel_index] =current_point_distance;
			}
		}

		for(int azimuth_index = bottom_azimuth_count ; azimuth_index < bottom_azimuth_count*2 ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			if(azimuth_index == bottom_azimuth_count + bottom_azimuth_count/2)
			{
				int current_point_distance = block_data_bram_noise[azimuth_index * 3 + 0].distance_[channel_index] *256;
				tofs_32[channel_index+16] =current_point_distance;
			}
		}
	}

	int tof_32_temp[32] = {0,};
	for(int idx = 0 ; idx < 32 ; idx++)
	{
		tof_32_temp[idx] = tofs_32[idx];
	}

	for(int idx = 0 ; idx < 31 ; ++idx)
	{
		for(int idx2 = idx + 1 ; idx2 < 32 ; ++idx2)
		{
			if(tof_32_temp[idx] > tof_32_temp[idx2])
			{
				int temp = tof_32_temp[idx];
				tof_32_temp[idx] = tof_32_temp[idx2];
				tof_32_temp[idx2] = temp;
			}
		}
	}

	int total_avg = 0;

	for(int i = 0 ; i < 20 ; i++)
	{
		total_avg += tof_32_temp[i + 6];
	}

	total_avg /= 20;

	int temp_test[32] = {0, };
	for(int k = 0 ; k < 32 ; k++)
	{
		temp_test[k] = tofs_32[k] - total_avg;
	}


	for(int k = 0 ; k < 32 ; k++)
	{
		cal_val[k] -= temp_test[k];
	}


//		int debnu2 = 0;
//		if(debnu2)
//		{
//			if(total_avg != dis_300)
//			{
//				clip_min += total_avg - dis_300;
//			}
//		}
		//			int debnu3 = 0;
		//			if(debnu3)
		//			{
		//				int testtemp = 0;
		//				for(int k = 0; k<32; k++)
		//				{
		//					cal_val[k] +=  testtemp;
		//				}
		//			}

//	print(total_avg);
	//flash
	int Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] =
	{ 0, };
	int tempdelay = cal_val[0];
//	char key11[30] = "ch1";
//	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch1, 0x00, sizeof(lidar_state_Ch2.delay_ch1));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch1, value11, sizeof(lidar_state_Ch2.delay_ch1));
	//strncpy(lidar_state_Ch2.delay_ch1, value11, sizeof(lidar_state_Ch2.delay_ch1));

	tempdelay = cal_val[1];
	memset(lidar_state_Ch2.delay_ch2, 0x00, sizeof(lidar_state_Ch2.delay_ch2));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch2, value11, sizeof(lidar_state_Ch2.delay_ch2));

	tempdelay = cal_val[2];
	memset(lidar_state_Ch2.delay_ch3, 0x00, sizeof(lidar_state_Ch2.delay_ch3));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch3, value11, sizeof(lidar_state_Ch2.delay_ch3));

	tempdelay = cal_val[3];
	memset(lidar_state_Ch2.delay_ch4, 0x00, sizeof(lidar_state_Ch2.delay_ch4));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch4, value11, sizeof(lidar_state_Ch2.delay_ch4));

	tempdelay = cal_val[4];
	memset(lidar_state_Ch2.delay_ch5, 0x00, sizeof(lidar_state_Ch2.delay_ch5));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch5, value11, sizeof(lidar_state_Ch2.delay_ch5));

	tempdelay = cal_val[5];
	memset(lidar_state_Ch2.delay_ch6, 0x00, sizeof(lidar_state_Ch2.delay_ch6));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch6, value11, sizeof(lidar_state_Ch2.delay_ch6));

	tempdelay = cal_val[6];
	memset(lidar_state_Ch2.delay_ch7, 0x00, sizeof(lidar_state_Ch2.delay_ch7));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch7, value11, sizeof(lidar_state_Ch2.delay_ch7));

	tempdelay = cal_val[7];
	memset(lidar_state_Ch2.delay_ch8, 0x00, sizeof(lidar_state_Ch2.delay_ch8));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch8, value11, sizeof(lidar_state_Ch2.delay_ch8));

	tempdelay = cal_val[8];
	memset(lidar_state_Ch2.delay_ch9, 0x00, sizeof(lidar_state_Ch2.delay_ch9));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch9, value11, sizeof(lidar_state_Ch2.delay_ch9));

	tempdelay = cal_val[9];
	memset(lidar_state_Ch2.delay_ch10, 0x00, sizeof(lidar_state_Ch2.delay_ch10));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch10, value11, sizeof(lidar_state_Ch2.delay_ch10));

	tempdelay = cal_val[10];
	memset(lidar_state_Ch2.delay_ch11, 0x00, sizeof(lidar_state_Ch2.delay_ch11));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch11, value11, sizeof(lidar_state_Ch2.delay_ch11));

	tempdelay = cal_val[11];
	memset(lidar_state_Ch2.delay_ch12, 0x00, sizeof(lidar_state_Ch2.delay_ch12));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch12, value11, sizeof(lidar_state_Ch2.delay_ch12));

	tempdelay = cal_val[12];
	memset(lidar_state_Ch2.delay_ch13, 0x00, sizeof(lidar_state_Ch2.delay_ch13));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch13, value11, sizeof(lidar_state_Ch2.delay_ch13));

	tempdelay = cal_val[13];
	memset(lidar_state_Ch2.delay_ch14, 0x00, sizeof(lidar_state_Ch2.delay_ch14));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch14, value11, sizeof(lidar_state_Ch2.delay_ch14));

	tempdelay = cal_val[14];
	memset(lidar_state_Ch2.delay_ch15, 0x00, sizeof(lidar_state_Ch2.delay_ch15));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch15, value11, sizeof(lidar_state_Ch2.delay_ch15));

	tempdelay = cal_val[15];
	memset(lidar_state_Ch2.delay_ch16, 0x00, sizeof(lidar_state_Ch2.delay_ch16));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch16, value11, sizeof(lidar_state_Ch2.delay_ch16));

	tempdelay = cal_val[16];
	memset(lidar_state_Ch2.delay_ch17, 0x00, sizeof(lidar_state_Ch2.delay_ch17));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch17, value11, sizeof(lidar_state_Ch2.delay_ch17));

	tempdelay = cal_val[17];
	memset(lidar_state_Ch2.delay_ch18, 0x00, sizeof(lidar_state_Ch2.delay_ch18));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch18, value11, sizeof(lidar_state_Ch2.delay_ch18));

	tempdelay = cal_val[18];
	memset(lidar_state_Ch2.delay_ch19, 0x00, sizeof(lidar_state_Ch2.delay_ch19));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch19, value11, sizeof(lidar_state_Ch2.delay_ch19));

	tempdelay = cal_val[19];
	memset(lidar_state_Ch2.delay_ch20, 0x00, sizeof(lidar_state_Ch2.delay_ch20));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch20, value11, sizeof(lidar_state_Ch2.delay_ch20));

	tempdelay = cal_val[20];
	memset(lidar_state_Ch2.delay_ch21, 0x00, sizeof(lidar_state_Ch2.delay_ch21));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch21, value11, sizeof(lidar_state_Ch2.delay_ch21));

	tempdelay = cal_val[21];
	memset(lidar_state_Ch2.delay_ch22, 0x00, sizeof(lidar_state_Ch2.delay_ch22));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch22, value11, sizeof(lidar_state_Ch2.delay_ch22));

	tempdelay = cal_val[22];
	memset(lidar_state_Ch2.delay_ch23, 0x00, sizeof(lidar_state_Ch2.delay_ch23));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch23, value11, sizeof(lidar_state_Ch2.delay_ch23));

	tempdelay = cal_val[23];
	memset(lidar_state_Ch2.delay_ch24, 0x00, sizeof(lidar_state_Ch2.delay_ch24));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch24, value11, sizeof(lidar_state_Ch2.delay_ch24));

	tempdelay = cal_val[24];
	memset(lidar_state_Ch2.delay_ch25, 0x00, sizeof(lidar_state_Ch2.delay_ch25));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch25, value11, sizeof(lidar_state_Ch2.delay_ch25));

	tempdelay = cal_val[25];
	memset(lidar_state_Ch2.delay_ch26, 0x00, sizeof(lidar_state_Ch2.delay_ch26));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch26, value11, sizeof(lidar_state_Ch2.delay_ch26));

	tempdelay = cal_val[26];
	memset(lidar_state_Ch2.delay_ch27, 0x00, sizeof(lidar_state_Ch2.delay_ch27));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch27, value11, sizeof(lidar_state_Ch2.delay_ch27));

	tempdelay = cal_val[27];
	memset(lidar_state_Ch2.delay_ch28, 0x00, sizeof(lidar_state_Ch2.delay_ch28));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch28, value11, sizeof(lidar_state_Ch2.delay_ch28));

	tempdelay = cal_val[28];
	memset(lidar_state_Ch2.delay_ch29, 0x00, sizeof(lidar_state_Ch2.delay_ch29));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch29, value11, sizeof(lidar_state_Ch2.delay_ch29));

	tempdelay = cal_val[29];
	memset(lidar_state_Ch2.delay_ch30, 0x00, sizeof(lidar_state_Ch2.delay_ch30));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch30, value11, sizeof(lidar_state_Ch2.delay_ch30));

	tempdelay = cal_val[30];
	memset(lidar_state_Ch2.delay_ch31, 0x00, sizeof(lidar_state_Ch2.delay_ch31));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch31, value11, sizeof(lidar_state_Ch2.delay_ch31));

	tempdelay = cal_val[31];
	memset(lidar_state_Ch2.delay_ch32, 0x00, sizeof(lidar_state_Ch2.delay_ch32));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch32, value11, sizeof(lidar_state_Ch2.delay_ch32));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}



	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}




	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));



	EraseLidarInfo_Ch2();

	Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*) &lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH2);
	Status = ReadLidarInfo_Ch2(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH2);
	memcpy(&lidar_state_Ch2, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));


	return 0;
}


int routing_calibration_Long(int cal_val[], ConvertedDataBlock* block_data_bram_noise, u32 fov_data_block_count)
{
	u32 bottom_azimuth_count = fov_data_block_count / 6;
	int tofs_32[32] = {0, };
	for(int channel_index = 0 ; channel_index < 16 ; ++channel_index) // channel loop 16
	{
		for(int azimuth_index = 0 ; azimuth_index < bottom_azimuth_count ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			if(azimuth_index == bottom_azimuth_count/2)
			{
				int current_point_distance = block_data_bram_noise[azimuth_index * 3 + 0].distance_[channel_index] *256;
				tofs_32[channel_index] =current_point_distance;
			}
		}

		for(int azimuth_index = bottom_azimuth_count ; azimuth_index < bottom_azimuth_count*2 ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			if(azimuth_index == bottom_azimuth_count + bottom_azimuth_count/2)
			{
				int current_point_distance = block_data_bram_noise[azimuth_index * 3 + 0].distance_[channel_index] *256;
				tofs_32[channel_index+16] =current_point_distance;
			}
		}
	}

	int tof_32_temp[32] = {0,};
	for(int idx = 0 ; idx < 32 ; idx++)
	{
		tof_32_temp[idx] = tofs_32[idx];
	}

	for(int idx = 0 ; idx < 31 ; ++idx)
	{
		for(int idx2 = idx + 1 ; idx2 < 32 ; ++idx2)
		{
			if(tof_32_temp[idx] > tof_32_temp[idx2])
			{
				int temp = tof_32_temp[idx];
				tof_32_temp[idx] = tof_32_temp[idx2];
				tof_32_temp[idx2] = temp;
			}
		}
	}

	int total_avg = 0;

	for(int i = 0 ; i < 20 ; i++)
	{
		total_avg += tof_32_temp[i + 6];
	}

	total_avg /= 20;

	int temp_test[32] = {0, };
	for(int k = 0 ; k < 32 ; k++)
	{
		temp_test[k] = tofs_32[k] - total_avg;
	}


	for(int k = 0 ; k < 32 ; k++)
	{
		cal_val[k] -= temp_test[k];
	}


//		int debnu2 = 0;
//		if(debnu2)
//		{
//			if(total_avg != dis_300)
//			{
//				clip_min += total_avg - dis_300;
//			}
//		}
		//			int debnu3 = 0;
		//			if(debnu3)
		//			{
		//				int testtemp = 0;
		//				for(int k = 0; k<32; k++)
		//				{
		//					cal_val[k] +=  testtemp;
		//				}
		//			}

//	print(total_avg);
	//flash
	int Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] =
	{ 0, };
	int tempdelay = cal_val[0];
//	char key11[30] = "ch1";
//	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));
	//strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

	tempdelay = cal_val[1];
	memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

	tempdelay = cal_val[2];
	memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

	tempdelay = cal_val[3];
	memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

	tempdelay = cal_val[4];
	memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

	tempdelay = cal_val[5];
	memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

	tempdelay = cal_val[6];
	memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

	tempdelay = cal_val[7];
	memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

	tempdelay = cal_val[8];
	memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

	tempdelay = cal_val[9];
	memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

	tempdelay = cal_val[10];
	memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

	tempdelay = cal_val[11];
	memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

	tempdelay = cal_val[12];
	memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

	tempdelay = cal_val[13];
	memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

	tempdelay = cal_val[14];
	memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

	tempdelay = cal_val[15];
	memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

	tempdelay = cal_val[16];
	memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

	tempdelay = cal_val[17];
	memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

	tempdelay = cal_val[18];
	memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

	tempdelay = cal_val[19];
	memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

	tempdelay = cal_val[20];
	memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

	tempdelay = cal_val[21];
	memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

	tempdelay = cal_val[22];
	memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

	tempdelay = cal_val[23];
	memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

	tempdelay = cal_val[24];
	memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

	tempdelay = cal_val[25];
	memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

	tempdelay = cal_val[26];
	memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

	tempdelay = cal_val[27];
	memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

	tempdelay = cal_val[28];
	memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

	tempdelay = cal_val[29];
	memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

	tempdelay = cal_val[30];
	memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

	tempdelay = cal_val[31];
	memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}



	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	return 0;
}



int routing_calibration_distanceFix(int cal_val[], ConvertedDataBlock* block_data_bram_noise, u32 fov_data_block_count, int real_distance)
{
	real_distance *= 256;

	u32 bottom_azimuth_count = fov_data_block_count / 6;
	int tofs_32[32] = {0, };
	for(int channel_index = 0 ; channel_index < 16 ; ++channel_index) // channel loop 16
	{
		for(int azimuth_index = 0 ; azimuth_index < bottom_azimuth_count ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			if(azimuth_index == bottom_azimuth_count/2)
			{
				int current_point_distance = block_data_bram_noise[azimuth_index * 3 + 0].distance_[channel_index] *256;
				tofs_32[channel_index] =current_point_distance;
			}
		}

		for(int azimuth_index = bottom_azimuth_count ; azimuth_index < bottom_azimuth_count*2 ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			if(azimuth_index == bottom_azimuth_count + bottom_azimuth_count/2)
			{
				int current_point_distance = block_data_bram_noise[azimuth_index * 3 + 0].distance_[channel_index] *256;
				tofs_32[channel_index+16] =current_point_distance;
			}
		}
	}

	for (int idx = 0; idx<32; ++idx)
	{
		cal_val[idx] += real_distance - tofs_32[idx];
	}

//	int tof_32_temp[32] = {0,};
//	for(int idx = 0 ; idx < 32 ; idx++)
//	{
//		tof_32_temp[idx] = tofs_32[idx];
//	}
//
//	for(int idx = 0 ; idx < 31 ; ++idx)
//	{
//		for(int idx2 = idx + 1 ; idx2 < 32 ; ++idx2)
//		{
//			if(tof_32_temp[idx] > tof_32_temp[idx2])
//			{
//				int temp = tof_32_temp[idx];
//				tof_32_temp[idx] = tof_32_temp[idx2];
//				tof_32_temp[idx2] = temp;
//			}
//		}
//	}
//
//	int total_avg = 0;
//
//	for(int i = 0 ; i < 20 ; i++)
//	{
//		total_avg += tof_32_temp[i + 6];
//	}
//
//	total_avg /= 20;
//
//	int temp_test[32] = {0, };
//	for(int k = 0 ; k < 32 ; k++)
//	{
//		temp_test[k] = tofs_32[k] - total_avg;
//	}
//
//
//	for(int k = 0 ; k < 32 ; k++)
//	{
//		cal_val[k] -= temp_test[k];
//	}


//		int debnu2 = 0;
//		if(debnu2)
//		{
//			if(total_avg != dis_300)
//			{
//				clip_min += total_avg - dis_300;
//			}
//		}
		//			int debnu3 = 0;
		//			if(debnu3)
		//			{
		//				int testtemp = 0;
		//				for(int k = 0; k<32; k++)
		//				{
		//					cal_val[k] +=  testtemp;
		//				}
		//			}

//	print(total_avg);
	//flash
	int Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] =
	{ 0, };
	int tempdelay = cal_val[0];
//	char key11[30] = "ch1";
//	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));
	//strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

	tempdelay = cal_val[1];
	memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

	tempdelay = cal_val[2];
	memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

	tempdelay = cal_val[3];
	memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

	tempdelay = cal_val[4];
	memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

	tempdelay = cal_val[5];
	memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

	tempdelay = cal_val[6];
	memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

	tempdelay = cal_val[7];
	memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

	tempdelay = cal_val[8];
	memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

	tempdelay = cal_val[9];
	memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

	tempdelay = cal_val[10];
	memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

	tempdelay = cal_val[11];
	memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

	tempdelay = cal_val[12];
	memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

	tempdelay = cal_val[13];
	memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

	tempdelay = cal_val[14];
	memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

	tempdelay = cal_val[15];
	memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

	tempdelay = cal_val[16];
	memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

	tempdelay = cal_val[17];
	memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

	tempdelay = cal_val[18];
	memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

	tempdelay = cal_val[19];
	memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

	tempdelay = cal_val[20];
	memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

	tempdelay = cal_val[21];
	memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

	tempdelay = cal_val[22];
	memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

	tempdelay = cal_val[23];
	memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

	tempdelay = cal_val[24];
	memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

	tempdelay = cal_val[25];
	memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

	tempdelay = cal_val[26];
	memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

	tempdelay = cal_val[27];
	memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

	tempdelay = cal_val[28];
	memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

	tempdelay = cal_val[29];
	memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

	tempdelay = cal_val[30];
	memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

	tempdelay = cal_val[31];
	memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}



	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	return 0;
}



int routing_calibration_long_distanceFix(int cal_val[], int ave_val[], int total_ave)
{

	for (int idx = 0; idx<32; ++idx)
	{
		cal_val[idx] += total_ave - ave_val[idx];
	}


	//flash
	int Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] =
	{ 0, };
	int tempdelay = cal_val[0];
//	char key11[30] = "ch1";
//	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));
	//strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

	tempdelay = cal_val[1];
	memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

	tempdelay = cal_val[2];
	memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

	tempdelay = cal_val[3];
	memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

	tempdelay = cal_val[4];
	memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

	tempdelay = cal_val[5];
	memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

	tempdelay = cal_val[6];
	memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

	tempdelay = cal_val[7];
	memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

	tempdelay = cal_val[8];
	memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

	tempdelay = cal_val[9];
	memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

	tempdelay = cal_val[10];
	memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

	tempdelay = cal_val[11];
	memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

	tempdelay = cal_val[12];
	memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

	tempdelay = cal_val[13];
	memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

	tempdelay = cal_val[14];
	memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

	tempdelay = cal_val[15];
	memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

	tempdelay = cal_val[16];
	memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

	tempdelay = cal_val[17];
	memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

	tempdelay = cal_val[18];
	memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

	tempdelay = cal_val[19];
	memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

	tempdelay = cal_val[20];
	memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

	tempdelay = cal_val[21];
	memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

	tempdelay = cal_val[22];
	memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

	tempdelay = cal_val[23];
	memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

	tempdelay = cal_val[24];
	memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

	tempdelay = cal_val[25];
	memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

	tempdelay = cal_val[26];
	memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

	tempdelay = cal_val[27];
	memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

	tempdelay = cal_val[28];
	memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

	tempdelay = cal_val[29];
	memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

	tempdelay = cal_val[30];
	memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

	tempdelay = cal_val[31];
	memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}



	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	return 0;
}




int routing_add(int cal_val[], int add_delay)
{

	for(int idx = 0 ; idx < 32 ; idx++)
	{
		cal_val[idx] +=add_delay;
	}


	int Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] =
	{ 0, };
	int tempdelay = cal_val[0];
//	char key11[30] = "ch1";
//	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));
	//strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

	tempdelay = cal_val[1];
	memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

	tempdelay = cal_val[2];
	memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

	tempdelay = cal_val[3];
	memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

	tempdelay = cal_val[4];
	memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

	tempdelay = cal_val[5];
	memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

	tempdelay = cal_val[6];
	memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

	tempdelay = cal_val[7];
	memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

	tempdelay = cal_val[8];
	memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

	tempdelay = cal_val[9];
	memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

	tempdelay = cal_val[10];
	memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

	tempdelay = cal_val[11];
	memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

	tempdelay = cal_val[12];
	memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

	tempdelay = cal_val[13];
	memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

	tempdelay = cal_val[14];
	memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

	tempdelay = cal_val[15];
	memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

	tempdelay = cal_val[16];
	memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

	tempdelay = cal_val[17];
	memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

	tempdelay = cal_val[18];
	memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

	tempdelay = cal_val[19];
	memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

	tempdelay = cal_val[20];
	memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

	tempdelay = cal_val[21];
	memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

	tempdelay = cal_val[22];
	memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

	tempdelay = cal_val[23];
	memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

	tempdelay = cal_val[24];
	memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

	tempdelay = cal_val[25];
	memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

	tempdelay = cal_val[26];
	memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

	tempdelay = cal_val[27];
	memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

	tempdelay = cal_val[28];
	memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

	tempdelay = cal_val[29];
	memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

	tempdelay = cal_val[30];
	memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

	tempdelay = cal_val[31];
	memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();

	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}



	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	return 0;
}

int routing_add2(int cal_val[], int add_delay)
{

	for(int idx = 0 ; idx < 32 ; idx++)
	{
		cal_val[idx] +=add_delay;
	}


	int Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] =
	{ 0, };
	int tempdelay = cal_val[0];
//	char key11[30] = "ch1";
//	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));
	//strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

	tempdelay = cal_val[1];
	memset(lidar_state_Ch2.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

	tempdelay = cal_val[2];
	memset(lidar_state_Ch2.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

	tempdelay = cal_val[3];
	memset(lidar_state_Ch2.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

	tempdelay = cal_val[4];
	memset(lidar_state_Ch2.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

	tempdelay = cal_val[5];
	memset(lidar_state_Ch2.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

	tempdelay = cal_val[6];
	memset(lidar_state_Ch2.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

	tempdelay = cal_val[7];
	memset(lidar_state_Ch2.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

	tempdelay = cal_val[8];
	memset(lidar_state_Ch2.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

	tempdelay = cal_val[9];
	memset(lidar_state_Ch2.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

	tempdelay = cal_val[10];
	memset(lidar_state_Ch2.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

	tempdelay = cal_val[11];
	memset(lidar_state_Ch2.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

	tempdelay = cal_val[12];
	memset(lidar_state_Ch2.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

	tempdelay = cal_val[13];
	memset(lidar_state_Ch2.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

	tempdelay = cal_val[14];
	memset(lidar_state_Ch2.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

	tempdelay = cal_val[15];
	memset(lidar_state_Ch2.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

	tempdelay = cal_val[16];
	memset(lidar_state_Ch2.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

	tempdelay = cal_val[17];
	memset(lidar_state_Ch2.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

	tempdelay = cal_val[18];
	memset(lidar_state_Ch2.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

	tempdelay = cal_val[19];
	memset(lidar_state_Ch2.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

	tempdelay = cal_val[20];
	memset(lidar_state_Ch2.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

	tempdelay = cal_val[21];
	memset(lidar_state_Ch2.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

	tempdelay = cal_val[22];
	memset(lidar_state_Ch2.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

	tempdelay = cal_val[23];
	memset(lidar_state_Ch2.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

	tempdelay = cal_val[24];
	memset(lidar_state_Ch2.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

	tempdelay = cal_val[25];
	memset(lidar_state_Ch2.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

	tempdelay = cal_val[26];
	memset(lidar_state_Ch2.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

	tempdelay = cal_val[27];
	memset(lidar_state_Ch2.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

	tempdelay = cal_val[28];
	memset(lidar_state_Ch2.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

	tempdelay = cal_val[29];
	memset(lidar_state_Ch2.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

	tempdelay = cal_val[30];
	memset(lidar_state_Ch2.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

	tempdelay = cal_val[31];
	memset(lidar_state_Ch2.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch2.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();

	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}


	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));


	EraseLidarInfo_Ch2();

	Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*) &lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH2);
	Status = ReadLidarInfo_Ch2(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH2);
	memcpy(&lidar_state_Ch2, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));



	return 0;
}

int routing_reset(int cal_val[])
{

	for(int idx = 0 ; idx < 32 ; idx++)
	{
		cal_val[idx] = 0;
	}

	int Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] =
	{ 0, };
	int tempdelay = cal_val[0];
//	char key11[30] = "ch1";
//	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));
	//strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

	tempdelay = cal_val[1];
	memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

	tempdelay = cal_val[2];
	memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

	tempdelay = cal_val[3];
	memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

	tempdelay = cal_val[4];
	memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

	tempdelay = cal_val[5];
	memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

	tempdelay = cal_val[6];
	memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

	tempdelay = cal_val[7];
	memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

	tempdelay = cal_val[8];
	memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

	tempdelay = cal_val[9];
	memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

	tempdelay = cal_val[10];
	memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

	tempdelay = cal_val[11];
	memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

	tempdelay = cal_val[12];
	memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

	tempdelay = cal_val[13];
	memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

	tempdelay = cal_val[14];
	memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

	tempdelay = cal_val[15];
	memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

	tempdelay = cal_val[16];
	memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

	tempdelay = cal_val[17];
	memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

	tempdelay = cal_val[18];
	memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

	tempdelay = cal_val[19];
	memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

	tempdelay = cal_val[20];
	memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

	tempdelay = cal_val[21];
	memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

	tempdelay = cal_val[22];
	memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

	tempdelay = cal_val[23];
	memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

	tempdelay = cal_val[24];
	memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

	tempdelay = cal_val[25];
	memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

	tempdelay = cal_val[26];
	memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

	tempdelay = cal_val[27];
	memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

	tempdelay = cal_val[28];
	memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

	tempdelay = cal_val[29];
	memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

	tempdelay = cal_val[30];
	memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

	tempdelay = cal_val[31];
	memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}



	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	return 0;
}



int temp_routing_valid()
{
	int Status = InitLinearQspiFlash();
		if (Status != XST_SUCCESS)
		{
			xil_printf("Flash Init Failed\r\n");
			return XST_FAILURE;
		}

		char value11[30] =
		{ 0, };
		int tempdelay = atoi(lidar_state_Ch.delay_ch1);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		//	char key11[30] = "ch1";
	//	GetJsonValueByKey(value11, req, key11);
		memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));
		//strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

		tempdelay = atoi(lidar_state_Ch.delay_ch2);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

		tempdelay = atoi(lidar_state_Ch.delay_ch3);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

		tempdelay = atoi(lidar_state_Ch.delay_ch4);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

		tempdelay = atoi(lidar_state_Ch.delay_ch5);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

		tempdelay = atoi(lidar_state_Ch.delay_ch6);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

		tempdelay = atoi(lidar_state_Ch.delay_ch7);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

		tempdelay = atoi(lidar_state_Ch.delay_ch8);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

		tempdelay = atoi(lidar_state_Ch.delay_ch9);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

		tempdelay = atoi(lidar_state_Ch.delay_ch10);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

		tempdelay = atoi(lidar_state_Ch.delay_ch11);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

		tempdelay = atoi(lidar_state_Ch.delay_ch12);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

		tempdelay = atoi(lidar_state_Ch.delay_ch13);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

		tempdelay = atoi(lidar_state_Ch.delay_ch14);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

		tempdelay = atoi(lidar_state_Ch.delay_ch15);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

		tempdelay = atoi(lidar_state_Ch.delay_ch16);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

		tempdelay = atoi(lidar_state_Ch.delay_ch17);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

		tempdelay = atoi(lidar_state_Ch.delay_ch18);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

		tempdelay = atoi(lidar_state_Ch.delay_ch19);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

		tempdelay = atoi(lidar_state_Ch.delay_ch20);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

		tempdelay = atoi(lidar_state_Ch.delay_ch21);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

		tempdelay = atoi(lidar_state_Ch.delay_ch22);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

		tempdelay = atoi(lidar_state_Ch.delay_ch23);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

		tempdelay = atoi(lidar_state_Ch.delay_ch24);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

		tempdelay = atoi(lidar_state_Ch.delay_ch25);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

		tempdelay = atoi(lidar_state_Ch.delay_ch26);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

		tempdelay = atoi(lidar_state_Ch.delay_ch27);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

		tempdelay = atoi(lidar_state_Ch.delay_ch28);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

		tempdelay = atoi(lidar_state_Ch.delay_ch29);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

		tempdelay = atoi(lidar_state_Ch.delay_ch30);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

		tempdelay = atoi(lidar_state_Ch.delay_ch31);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

		tempdelay = atoi(lidar_state_Ch.delay_ch32);
		if(tempdelay < -10000 || tempdelay > 0xF000)
		{
			tempdelay = 0;
		}
		memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
		sprintf(value11, "%d", tempdelay);
		strncpy(lidar_state_Ch.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));

		EraseLidarInfo();
		Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
		if (Status != XST_SUCCESS)
		{
			xil_printf("Flash Write Failed\r\n");
			return XST_FAILURE;
		}

		EraseLidarInfo_Ch();
		Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

		if (Status != XST_SUCCESS)
		{
			xil_printf("Flash Write Failed\r\n");
			return XST_FAILURE;
		}



		memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
		Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
		memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

		return 0;
}

int caltest(int cal_val[], int tofs_32[])
{

	int tof_32_temp[32] = {0,};
	
    for(int idx = 0; idx<32; ++idx)
	{
		tof_32_temp[idx] = tofs_32[idx];
	}
	
    for (int idx = 0; idx<31; ++idx)
	{
		for(int idx2 = idx+1; idx2<32; ++idx2)
		{
			if(tof_32_temp[idx] > tof_32_temp[idx2])
			{
				int temp = tof_32_temp[idx];
				tof_32_temp[idx] = tof_32_temp[idx2];
				tof_32_temp[idx2] = temp;
			}
		}
	}

	int total_avg = 0;
	for (int i = 0; i<20; i++)
	{
		total_avg += tof_32_temp[i+6];
	}
	total_avg /= 20;
	int tof_214 = 547;
	for(int k = 0; k<32; k++)
	{
		cal_val[k] +=  total_avg - tofs_32[k] + (tof_214-total_avg);
	}

	//flash
	int Status = InitLinearQspiFlash();
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] =
	{ 0, };
	int tempdelay = cal_val[0];
//	char key11[30] = "ch1";
//	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));
	//strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

	tempdelay = cal_val[1];
	memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

	tempdelay = cal_val[2];
	memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

	tempdelay = cal_val[3];
	memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

	tempdelay = cal_val[4];
	memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

	tempdelay = cal_val[5];
	memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

	tempdelay = cal_val[6];
	memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

	tempdelay = cal_val[7];
	memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

	tempdelay = cal_val[8];
	memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

	tempdelay = cal_val[9];
	memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

	tempdelay = cal_val[10];
	memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

	tempdelay = cal_val[11];
	memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

	tempdelay = cal_val[12];
	memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

	tempdelay = cal_val[13];
	memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

	tempdelay = cal_val[14];
	memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

	tempdelay = cal_val[15];
	memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

	tempdelay = cal_val[16];
	memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

	tempdelay = cal_val[17];
	memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

	tempdelay = cal_val[18];
	memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

	tempdelay = cal_val[19];
	memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

	tempdelay = cal_val[20];
	memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

	tempdelay = cal_val[21];
	memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

	tempdelay = cal_val[22];
	memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

	tempdelay = cal_val[23];
	memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

	tempdelay = cal_val[24];
	memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

	tempdelay = cal_val[25];
	memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

	tempdelay = cal_val[26];
	memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

	tempdelay = cal_val[27];
	memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

	tempdelay = cal_val[28];
	memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

	tempdelay = cal_val[29];
	memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

	tempdelay = cal_val[30];
	memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

	tempdelay = cal_val[31];
	memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
	sprintf(value11, "%d", tempdelay);
	strncpy(lidar_state_Ch.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*) &lidar_state_, LIDAR_STATE_SIZE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*) &lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if (Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}



	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));


    return 0;
}

//int CalibrateMinimumDetectionRange(volatile block_data_total_shared *block_data_shared, int* clip_min)
//{
//	if(initial == 0)
//	{
//		clip_min_temp = *clip_min;
//		initial = 1;
//		clip_min = 0;
//	}
//	else
//	{
//		cal_detection_range_result = Minimum_Detection_Cal(block_data_shared);
//		if(cal_detection_range_result == 1)
//		{
//			Xil_Out32(oDetection_Offset, Detection_value + 1);
//			cal_detection_done = 1;
//			*clip_min = clip_min_temp;
//			DECTECTION_RANGE_FLAG = 0;
//			lidar_state_.detection_range_value = Detection_value + 1;
//			EraseLidarInfo();
//			int Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
//			if(Status != XST_SUCCESS)
//			{
//				xil_printf("Flash Write Failed\r\n");
//				return kFlashFailure;
//			}
//			Status = LinearQspiFlashWriteForLidarInfo_Ch((char*)&lidar_state_Ch, LIDAR_STATE_SIZE_CH);
//			if(Status != XST_SUCCESS)
//			{
//				xil_printf("Flash Write Failed\r\n");
//				return kFlashFailure;
//			}
//			Status = ReadLidarInfo((u8*)&lidar_state_, LIDAR_STATE_SIZE);
//			if(Status != XST_SUCCESS)
//			{
//				xil_printf("Flash Read Failed\r\n");
//				return kFlashFailure;
//			}
//			// Done
//			initial = 0;
//		}
//		else
//		{
//			Detection_value = Detection_value + 1;
//			Xil_Out32(oDetection_Offset, Detection_value);
//		}
//	}
//}


int Minimum_Detection_Cal(volatile RawDataBlock *block_data_shared, u32 fov_data_block_count)
{
	u32 bottom_azimuth_count = fov_data_block_count / 6;
	int **current_point_distance = (int**)malloc(sizeof(int*)*bottom_azimuth_count);
	for(size_t i = 0 ; i < 16 ; ++i)
		current_point_distance[i] = (int*)malloc(sizeof(int)*16);

	int **channel_success = (int**)malloc(sizeof(int*)*bottom_azimuth_count);
	for(size_t i = 0 ; i < 16 ; ++i)
		channel_success[i] = (int*)malloc(sizeof(int)*16);

	int result = 0;
	int cnt = 0;

	for (int i = 144; i < 1008; ++i) // azimuth loop
	{
		for(int j =0; j < 16; ++j)
		{
			current_point_distance[i][j] = block_data_shared[i*3].distance_[j].distance;


			if(current_point_distance[i][j] < 5*256 && current_point_distance[i][j] > 2*256)
			{
				channel_success[i][j] = 1;
			}
			else
			{
				channel_success[i][j] = 0;
			}
		}
	}

	for (int i = 144; i < 1008; ++i) // azimuth loop
	{
		for(int j =0; j < 16; ++j)
		{
			if(channel_success[i][j] == 1)
			{
				cnt++;
			}

		}
	}

	if(cnt < 3)
	{
		if(done_cnt < 1)
		{
			result = 0;
			done_cnt++;
		}
		else
		{
			result = 1;
			done_cnt = 0;
		}

	}
	else
	{
		result = 0;
	}

	for(size_t i = 0 ; i < 16 ; ++i)
		free(current_point_distance[i]);

	free(current_point_distance);

	for(size_t i = 0 ; i < 16 ; ++i)
		free(channel_success[i]);

	free(channel_success);


	return result;
}

