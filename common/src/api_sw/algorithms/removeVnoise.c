/*
 * removeVnoise.c
 *
 *  Created on: 2022. 7. 21.
 *      Author: HP
 */

#include "removeVnoise.h"

#include "../perf_test/perf_test.h"

#define StopCount				3
const double DEG2RAD = PI / 180;
const double cal_var = (360 / (PI * 2));
//#define DEG2RAD					PI / 180
//#define cal_var					(360 / (PI * 2))
const double elevation[32] = {4.84375, 4.53125, 4.21875, 3.90625, 3.59375, 3.28125, 2.96875, 2.65625, 2.34375, 2.03125, 1.71875, 1.40625, 1.09375,
		0.78125, 0.46875, 0.15625, -0.15625, -0.46875, -0.78125, -1.09375, -1.40625, -1.71875, -2.03125, -2.34375, -2.65625, -2.96875, -3.28125,
		-3.59375, -3.90625, -4.21875, -4.53125, -4.84375};

uint8_t is_vnoise_trig_function_map_initialized = 0;
void InitVNoiseTrigFunctionMap()
{
	double degree = 0.0;
	for(int i = 0 ; i < TRIG_FUNCTION_ARRAY_SIZE ; ++i)
	{
		sin_approximation_value_map[i] = sin(degree * DEG2RAD);
		cos_approximation_value_map[i] = cos(degree * DEG2RAD);
		tan_approximation_value_map[i] = tan(degree * DEG2RAD);
		atan_approximation_value_map[i] = atan(tan_approximation_value_map[i]);
		atan2_approximation_value_map[i] = atan2(1, tan_approximation_value_map[i]);
		degree += 0.1;
	}
}

#ifdef MAPPING
void RemoveVNoise(ConvertedDataBlock* block_data_bram_noise, volatile RawDataBlock* block_data_total_test,
		PointInfo azimuth_index_list[], int azimuth_index_list_size, int channel_index, int istop)
{

	double distanceMin = 999;
	int azimuth_index_listIndex = -1;

	//dynamic arr
//	double **azimuth_index_list_map;
//	azimuth_index_list_map = (double**)malloc(sizeof(double*)*azimuth_index_list_size);
//	for(int i = 0; i< azimuth_index_list_size; i++)
//	{
//		azimuth_index_list_map[i] = (double*)calloc(2, sizeof(double));
//	}

#if 0
	double azimuth_index_list_map[NUM_AZI][2] =
	{	0,};

	// V 모양의 꼭지점에 해당하는 포인트 검색
	for(int i = 0; i < azimuth_index_list_size; ++i)
	{
		int azimuth_index_ = azimuth_index_list[i].azimuth_index;
		int stop_index_ = azimuth_index_list[i].stop_index;
		double distance = block_data_bram_noise[azimuth_index_ * StopCount + stop_index_].distance_[channel_index];
		azimuth_index_list_map[i][0] = distance;
		azimuth_index_list_map[i][1] = i;

		if(distance <= distanceMin)
		{
			distanceMin = distance;
			azimuth_index_listIndex = i;
		}
	}

	// 가장 가까이 있는 3점 검색을 위한 정렬
	for(int idx = 0; idx < azimuth_index_list_size - 1; ++idx)
	{
		for(int idx2 = idx + 1; idx2 < azimuth_index_list_size; ++idx2)
		{
			if(azimuth_index_list_map[idx][0] > azimuth_index_list_map[idx2][0])
			{
				double temp = azimuth_index_list_map[idx][0];
				int temp_idx = azimuth_index_list_map[idx][1];
				azimuth_index_list_map[idx][0] = azimuth_index_list_map[idx2][0];
				azimuth_index_list_map[idx2][0] = temp;

				azimuth_index_list_map[idx][1] = azimuth_index_list_map[idx2][1];
				azimuth_index_list_map[idx2][1] = temp_idx;
			}
		}
	}

	// 가상의 꼭지점 생성
	double vertex_distance_new = (azimuth_index_list_map[0][0] + azimuth_index_list_map[1][0] + azimuth_index_list_map[2][0]) / 3;
	int azimuth_index_list_map_01 = (int)azimuth_index_list_map[0][1];
	int azimuth_index_list_map_11 = (int)azimuth_index_list_map[1][1];
	int azimuth_index_list_map_21 = (int)azimuth_index_list_map[2][1];
	int temp_azimuthidx01 = azimuth_index_list[azimuth_index_list_map_01].azimuth_index;
	int temp_azimuthidx11 = azimuth_index_list[azimuth_index_list_map_11].azimuth_index;
	int temp_azimuthidx21 = azimuth_index_list[azimuth_index_list_map_21].azimuth_index;

	int temp_stopidx01 = azimuth_index_list[azimuth_index_list_map_01].stop_index;
	int temp_stopidx11 = azimuth_index_list[azimuth_index_list_map_11].stop_index;
	int temp_stopidx21 = azimuth_index_list[azimuth_index_list_map_21].stop_index;

	double vertex_azimuth_new = (block_data_bram_noise[temp_azimuthidx01 * StopCount + temp_stopidx01].azimuth_
			+ block_data_bram_noise[temp_azimuthidx11 * StopCount + temp_stopidx11].azimuth_
			+ block_data_bram_noise[temp_azimuthidx21 * StopCount + temp_stopidx21].azimuth_) / 3;

#else

	int smalls2[3] = {0, 1, 2};
	int smaals_maxidx = 0;
	double distanceMax = 0;
	for(int i = 0 ; i < 3 ; ++i)
	{
		double distance =
				block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];

		if(distanceMax > distance)
		{
			distanceMax = distance;
			smaals_maxidx = i;
		}
	}
	distanceMax = 0;

	// V 모양의 꼭지점에 해당하는 포인트 검색
	for(int i = 0 ; i < azimuth_index_list_size ; ++i)
	{
		int azimuth_index_ = azimuth_index_list[i].azimuth_index;
		int stop_index_ = azimuth_index_list[i].stop_index;
		double distance = block_data_bram_noise[azimuth_index_ * StopCount + stop_index_].distance_[channel_index];

		if(i >= 3)
		{
			double distance2 = block_data_bram_noise[azimuth_index_list[smalls2[smaals_maxidx]].azimuth_index * StopCount
					+ azimuth_index_list[smalls2[smaals_maxidx]].stop_index].distance_[channel_index];
			if(distance2 > distance)
			{
				smalls2[smaals_maxidx] = i;
				distanceMax = distance;
				for(int k = 0 ; k < 3 ; ++k)
				{
					double temp_dis = block_data_bram_noise[azimuth_index_list[smalls2[k]].azimuth_index * StopCount
							+ azimuth_index_list[smalls2[k]].stop_index].distance_[channel_index];
					if(temp_dis > distanceMax)
					{
						distanceMax = temp_dis;
						smaals_maxidx = k;
					}
				}
			}
		}
		if(distance <= distanceMin)
		{
			distanceMin = distance;
			azimuth_index_listIndex = i;
		}
	}

	// 가상의 꼭지점 생성
	double vertex_distance_new =
			(block_data_bram_noise[azimuth_index_list[smalls2[0]].azimuth_index * StopCount + azimuth_index_list[smalls2[0]].stop_index].distance_[channel_index]
					+ block_data_bram_noise[azimuth_index_list[smalls2[1]].azimuth_index * StopCount + azimuth_index_list[smalls2[1]].stop_index].distance_[channel_index]
					+ block_data_bram_noise[azimuth_index_list[smalls2[2]].azimuth_index * StopCount + azimuth_index_list[smalls2[2]].stop_index].distance_[channel_index])
					/ 3;

	double vertex_azimuth_new = (block_data_bram_noise[azimuth_index_list[smalls2[0]].azimuth_index * StopCount
			+ azimuth_index_list[smalls2[0]].stop_index].azimuth_
			+ block_data_bram_noise[azimuth_index_list[smalls2[1]].azimuth_index * StopCount + azimuth_index_list[smalls2[1]].stop_index].azimuth_
			+ block_data_bram_noise[azimuth_index_list[smalls2[2]].azimuth_index * StopCount + azimuth_index_list[smalls2[2]].stop_index].azimuth_)
			/ 3;

#endif

	//dynamic arr
//	for(int i =0; i<azimuth_index_list_size; i++)
//	{
//		free(azimuth_index_list_map[i]);
//	}
//	free(azimuth_index_list_map);

//	float x_pos_center = 0, y_pos_center = 0;//, pos_z = 0;

	double temp_elevation = elevation[31 - (channel_index * 2) + istop];
	if(temp_elevation < 0)
	{
		temp_elevation *= -1;
	}
	float cos_elevation = cos_approximation_value_map[(int)(temp_elevation * 10)];
	//float cos_elevation = cos(temp_elevation * DEG2RAD);

	float cos_azimuth = 0;
	float sin_azimuth = 0;
	if(vertex_azimuth_new < 0)
	{
		cos_azimuth = cos_approximation_value_map[(int)(vertex_azimuth_new * -1 * 10)];
		sin_azimuth = sin_approximation_value_map[(int)((360 + vertex_azimuth_new) * 10)];
	}
	else
	{
		cos_azimuth = cos_approximation_value_map[(int)(vertex_azimuth_new * 10)];
		sin_azimuth = sin_approximation_value_map[(int)((vertex_azimuth_new) * 10)];
	}
	// float cos_azimuth = cos((vertex_azimuth_new) * DEG2RAD);
	// float sin_azimuth = sin((vertex_azimuth_new) * DEG2RAD);
	float x_pos_center = vertex_distance_new * cos_elevation * cos_azimuth;
	float y_pos_center = vertex_distance_new * cos_elevation * sin_azimuth;

	//군집의 가장 왼쪽 포인트, 가장 오른쪽 포인트의  X, Y 좌표 획득
	double temp_dis1 =
			block_data_bram_noise[azimuth_index_list[0].azimuth_index * StopCount + azimuth_index_list[0].stop_index].distance_[channel_index];
	double temp_azi1 = block_data_bram_noise[azimuth_index_list[0].azimuth_index * StopCount + azimuth_index_list[0].stop_index].azimuth_;

	double x_pos_left = 0;
	double y_pos_left = 0;
	if(temp_azi1 < 0)
	{
		x_pos_left = temp_dis1 * cos_elevation * cos_approximation_value_map[(int)(temp_azi1 * -1 * 10)];
		y_pos_left = temp_dis1 * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi1) * 10)];
	}
	else
	{
		x_pos_left = temp_dis1 * cos_elevation * cos_approximation_value_map[(int)(temp_azi1 * 10)];
		y_pos_left = temp_dis1 * cos_elevation * sin_approximation_value_map[(int)((temp_azi1) * 10)];
	}
	// double x_pos_left = temp_dis1 * cos_elevation * cos((temp_azi1) * DEG2RAD);
	// double y_pos_left = temp_dis1 * cos_elevation * sin((temp_azi1) * DEG2RAD);

	double temp_dis2 = block_data_bram_noise[azimuth_index_list[azimuth_index_list_size - 1].azimuth_index * StopCount
			+ azimuth_index_list[azimuth_index_list_size - 1].stop_index].distance_[channel_index];
	double temp_azi2 = block_data_bram_noise[azimuth_index_list[azimuth_index_list_size - 1].azimuth_index * StopCount
			+ azimuth_index_list[azimuth_index_list_size - 1].stop_index].azimuth_;

	double x_pos_right = 0;
	double y_pos_right = 0;
	if(temp_azi2 < 0)
	{
		x_pos_right = temp_dis2 * cos_elevation * cos_approximation_value_map[(int)(temp_azi2 * -1 * 10)];
		y_pos_right = temp_dis2 * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi2) * 10)];
	}
	else
	{
		x_pos_right = temp_dis2 * cos_elevation * cos_approximation_value_map[(int)(temp_azi2 * 10)];
		y_pos_right = temp_dis2 * cos_elevation * sin_approximation_value_map[(int)(temp_azi2 * 10)];
	}
	// double x_pos_right = temp_dis2 * cos_elevation * cos((temp_azi2)*DEG2RAD);
	// double y_pos_right = temp_dis2 * cos_elevation * sin((temp_azi2)*DEG2RAD);

	//double x_pos_center = lidar_point_cloud_data_[lidar_index].stop_[vertex_point_stop_index].top_bottom_[top_bottom_index].azimuth_[vertexPointIndex].channel_[channel_index].point_[0];
	//double y_pos_center = lidar_point_cloud_data_[lidar_index].stop_[vertex_point_stop_index].top_bottom_[top_bottom_index].azimuth_[vertexPointIndex].channel_[channel_index].point_[1];

//	double x_pos_center = pos_x;
//	double y_pos_center = pos_y;

	// 2개의 가상의 선, 임시 angle 계산
	double angle1 = atan2((y_pos_left - y_pos_center), (x_pos_left - x_pos_center)) * cal_var;
	double angle2 = atan2((y_pos_right - y_pos_center), (x_pos_right - x_pos_center)) * cal_var;

	//double azimuth_center = lidar_point_cloud_data_[lidar_index].stop_[vertex_point_stop_index].top_bottom_[top_bottom_index].azimuth_[vertexPointIndex].azimuth_raw_;
	double azimuth_center = vertex_azimuth_new;

//	double angle_check1 = fabs(angle1 - azimuth_center) * 0.3;
//	double angle_check2 = fabs(azimuth_center - angle2) * 0.3;

//	double angle_check1 = fabs(angle1 - azimuth_center) * 0.18;
//	double angle_check2 = fabs(azimuth_center - angle2) * 0.18;

	double angle_check1 = fabs(angle1 - azimuth_center) * 0.16;
	double angle_check2 = fabs(azimuth_center - angle2) * 0.16;

//	double angle_check1 = fabs(angle1 - azimuth_center) * 0.5;
//	double angle_check2 = fabs(azimuth_center - angle2) * 0.5;

	// double angle_check1 = fabs(angle1 - azimuth_center) / 2;
	// double angle_check2 = fabs(azimuth_center - angle2) / 2;

	double min_angle1 = angle1;
	double min_angle2 = angle2;

	double squared_error_sum_min = 99999999;
	double error_sum_min1 = 99999999;
	double error_sum_min2 = 99999999;

	for(double angle = angle1 - angle_check1 ; angle < angle1 + angle_check1 ; angle += 1.0)
	{
		double a = 0;
		if(angle < 0)
		{
			a = tan_approximation_value_map[(int)((360 + angle) * 10)];
		}
		else
		{
			a = tan_approximation_value_map[(int)(angle * 10)];
		}
		// double a = tan(angle * DEG2RAD);
		double b = y_pos_center - (a * x_pos_center);

		double squared_error_sum = 0;
		double error_sum = 0;
		for(unsigned int i = 0 ; i <= azimuth_index_listIndex ; ++i)
		{
			double temp_dis_ =
					block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
			double temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;
			double x = 0;
			double y = 0;
			if(temp_azi < 0)
			{
				x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
				y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
			}
			else
			{
				x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
				y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
			}
			// double x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
			// double y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//			double x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//			double y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);
			double error = fabs(((a * x) + b) - y);

			squared_error_sum += (error * error);
			error_sum += error;
		}
		if(squared_error_sum <= squared_error_sum_min)
		{
			squared_error_sum_min = squared_error_sum;
			error_sum_min1 = error_sum;
			min_angle1 = angle;
		}
	}

	squared_error_sum_min = 99999999;

	// 꼭지점 기준 오른쪽 점들을 지나는 선의 정확한 angle 획득
	for(double angle = angle2 - angle_check2 ; angle < angle2 + angle_check2 ; angle += 1.0)
	{
		double a = 0;
		if(angle < 0)
		{
			a = tan_approximation_value_map[(int)((360 + angle) * 10)];
		}
		else
		{
			a = tan_approximation_value_map[(int)(angle * 10)];
		}
		// double a = tan(angle * DEG2RAD);
		double b = y_pos_center - (a * x_pos_center);

		double squared_error_sum = 0;
		double error_sum = 0;
		for(unsigned int i = azimuth_index_listIndex ; i < azimuth_index_list_size ; ++i)
		{
			double temp_dis_ =
					block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
			double temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;
			double x = 0;
			double y = 0;
			if(temp_azi < 0)
			{
				x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
				y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
			}
			else
			{
				x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
				y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
			}
			// double x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
			// double y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//			double x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//			double y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

			double error = fabs(((a * x) + b) - y);

			squared_error_sum += (error * error);
			error_sum += error;
		}

		if(squared_error_sum <= squared_error_sum_min)
		{
			squared_error_sum_min = squared_error_sum;
			error_sum_min2 = error_sum;
			min_angle2 = angle;
		}
	}

	// 2개의 선에서 일정 에러값 내에 존재하는 포인트 갯수 계산
	double a1 = 0;
	if(min_angle1 < 0)
	{
		a1 = tan_approximation_value_map[(int)((360 + min_angle1) * 10)];
	}
	else
	{
		a1 = tan_approximation_value_map[(int)((min_angle1) * 10)];
	}
	// double a1 = tan(min_angle1 * DEG2RAD);
	double b1 = y_pos_center - (a1 * x_pos_center);

	double a2 = 0;
	if(min_angle2 < 0)
	{
		a2 = tan_approximation_value_map[(int)((360 + min_angle2) * 10)];
	}
	else
	{
		a2 = tan_approximation_value_map[(int)((min_angle2) * 10)];
	}
	// double a2 = tan(min_angle2 * DEG2RAD);
	double b2 = y_pos_center - (a2 * x_pos_center);

	unsigned int point_in_v = 0;
	for(int i = 0 ; i < azimuth_index_list_size ; ++i)
	{
		double temp_dis_ =
				block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
		double temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;
		double x = 0;
		double y = 0;
		if(temp_azi < 0)
		{
			x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
			y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
		}
		else
		{
			x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
			y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
		}
		// double x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
		// double y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//		double x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//		double y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

		double error1 = fabs(((a1 * x) + b1) - y);
		double error2 = fabs(((a2 * x) + b2) - y);

		if(error1 < 0.07 || error2 < 0.07)
		{
			++point_in_v;
		}
	}

	// 아래 4개 조건 만족시 V 노이즈로 판단
	// 1. 꼭지점이 가운데 존재하는지 체크
	// 2. 포인트들이 2개 선과 얼마나 일치하는지 체크
	// 3. 두 라인의 세타값 체크 (너무 벌어지면 안됨)
	// 4. Azimuth 기준 양쪽 세타값이 동일한지 체크
	int temp_1 = ((double)azimuth_index_listIndex / (azimuth_index_list_size - 1)) * 100;
//	if(temp_1 >= 0.15 && temp_1 <= 0.85)
	if(temp_1 >= 15 && temp_1 <= 85)
//	if ((double)azimuth_index_listIndex / (azimuth_index_list_size - 1) >= 0.15 && (double)azimuth_index_listIndex / (azimuth_index_list_size - 1) <= 0.85)
	{
		if((vertex_distance_new >= 3 && ((double)point_in_v / azimuth_index_list_size) > 0.7
				&& ((error_sum_min1 + error_sum_min2) / azimuth_index_list_size) <= 0.06)
				|| (vertex_distance_new < 3 && ((double)point_in_v / azimuth_index_list_size) > 0.5
						&& ((error_sum_min1 + error_sum_min2) / azimuth_index_list_size) <= 0.08))
		{
//			if(fabs(min_angle1 - min_angle2) < 100.0)
			if(fabs(min_angle1 - min_angle2) < 100)
			{
				if(fabs((min_angle1 - azimuth_center) - (azimuth_center - min_angle2)) < 25.0)
				{
					// 봉을 제외한 노이즈 부분 제거(왼쪽)
					// azimuth 라인과 직교하는 라인 기준, 30도를 넘으면 노이즈 시작
					double margin_angle = 30;
					bool is_noise_start = false;
					for(int i = azimuth_index_listIndex ; i >= 0 ; --i)
					{
						if(i != azimuth_index_listIndex)
						{
							if(is_noise_start == true)
							{

								block_data_total_test[azimuth_index_list[i].azimuth_index * StopCount
										+ azimuth_index_list[i].stop_index].distance_[channel_index].distance = 0;
								//block_data_bram_noise->block_data_bram[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].data_points[channel_index] = 0;
								// lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[0] = 0;
								// lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[1] = 0;
								// lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[2] = 0;
							}
							else
							{
								double temp_dis_ = block_data_bram_noise[azimuth_index_list[i + 1].azimuth_index * StopCount
										+ azimuth_index_list[i + 1].stop_index].distance_[channel_index];
								double temp_azi = block_data_bram_noise[azimuth_index_list[i + 1].azimuth_index * StopCount
										+ azimuth_index_list[i + 1].stop_index].azimuth_;

								double last_x = 0;
								double last_y = 0;
								if(temp_azi < 0)
								{
									last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
									last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
								}
								else
								{
									last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
									last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
								}
								// double last_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								// double last_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//								double last_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double last_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								temp_dis_ =
										block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
								temp_azi =
										block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;

								double cur_x = 0;
								double cur_y = 0;
								if(temp_azi < 0)
								{
									cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
									cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
								}
								else
								{
									cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
									cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
								}
								// double cur_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								// double cur_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//								double cur_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double cur_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								double angle = atan2((cur_y - last_y), (cur_x - last_x)) * cal_var;

								if(((angle > (vertex_azimuth_new - 90 - margin_angle) && angle < (vertex_azimuth_new - 90 + margin_angle))
										|| (angle > (vertex_azimuth_new + 90 - margin_angle) && angle < (vertex_azimuth_new + 90 + margin_angle)))
										== false)
								{
									is_noise_start = true;
								}
							}
						}
					}

					// 봉을 제외한 노이즈 부분 제거(오른쪽)
					// azimuth 라인과 직교하는 라인 기준, 30도를 넘으면 노이즈 시작
					is_noise_start = false;
					for(unsigned int i = azimuth_index_listIndex ; i < azimuth_index_list_size ; ++i)
					{
						if(i != azimuth_index_listIndex)
						{
							if(is_noise_start == true)
							{
								block_data_total_test[azimuth_index_list[i].azimuth_index * StopCount
										+ azimuth_index_list[i].stop_index].distance_[channel_index].distance = 0;
								//block_data_bram_noise->block_data_bram[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].data_points[channel_index] = 0;

								//lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[0] = 0;
								//lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[1] = 0;
								//lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[2] = 0;
							}
							else
							{
								double temp_dis_ = block_data_bram_noise[azimuth_index_list[i - 1].azimuth_index * StopCount
										+ azimuth_index_list[i - 1].stop_index].distance_[channel_index];
								double temp_azi = block_data_bram_noise[azimuth_index_list[i - 1].azimuth_index * StopCount
										+ azimuth_index_list[i - 1].stop_index].azimuth_;
//								double last_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double last_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								double last_x = 0;
								double last_y = 0;
								if(temp_azi < 0)
								{
									last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
									last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
								}
								else
								{
									last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
									last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
								}
								// double last_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								// double last_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);

								temp_dis_ =
										block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
								temp_azi =
										block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;

								double cur_x = 0;
								double cur_y = 0;
								if(temp_azi < 0)
								{
									cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
									cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
								}
								else
								{
									cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
									cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
								}
								// double cur_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								// double cur_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//								double cur_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double cur_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								double angle = atan2((cur_y - last_y), (cur_x - last_x)) * cal_var;

								if(((angle > (vertex_azimuth_new - 90 - margin_angle) && angle < (vertex_azimuth_new - 90 + margin_angle))
										&& (angle > (vertex_azimuth_new + 90 - margin_angle) && angle < (vertex_azimuth_new + 90 + margin_angle)))
										== false)
								{
									is_noise_start = true;
								}
							}
						}
					}

				}
			}
		}
	}
}

#else
void RemoveVNoise(volatile ConvertedDataBlock* block_data_bram_noise, volatile block_data_total_shared* block_data_total_test, PointInfo azimuth_index_list[], int azimuth_index_list_size, int channel_index, int istop)
{

	double distanceMin = 999;
	int azimuth_index_listIndex = -1;

	//dynamic arr
//	double **azimuth_index_list_map;
//	azimuth_index_list_map = (double**)malloc(sizeof(double*)*azimuth_index_list_size);
//	for(int i = 0; i< azimuth_index_list_size; i++)
//	{
//		azimuth_index_list_map[i] = (double*)calloc(2, sizeof(double));
//	}

	double azimuth_index_list_map[NUM_AZI][2] =
	{	0,};

	// V 모양의 꼭지점에 해당하는 포인트 검색
	for (int i = 0; i < azimuth_index_list_size; ++i)
	{
		int azimuth_index_ = azimuth_index_list[i].azimuth_index;
		int stop_index_ = azimuth_index_list[i].stop_index;
		double distance = block_data_bram_noise[azimuth_index_ * StopCount + stop_index_].distance_[channel_index];
		azimuth_index_list_map[i][0] = distance;
		azimuth_index_list_map[i][1] = i;

		if (distance <= distanceMin)
		{
			distanceMin = distance;
			azimuth_index_listIndex = i;
		}
	}

	// 가장 가까이 있는 3점 검색을 위한 정렬
	for (int idx = 0; idx<azimuth_index_list_size-1; ++idx)
	{
		for(int idx2 = idx+1; idx2<azimuth_index_list_size; ++idx2)
		{
			if(azimuth_index_list_map[idx][0] > azimuth_index_list_map[idx2][0])
			{
				double temp = azimuth_index_list_map[idx][0];
				int temp_idx = azimuth_index_list_map[idx][1];
				azimuth_index_list_map[idx][0] = azimuth_index_list_map[idx2][0];
				azimuth_index_list_map[idx2][0] = temp;

				azimuth_index_list_map[idx][1] = azimuth_index_list_map[idx2][1];
				azimuth_index_list_map[idx2][1] = temp_idx;
			}
		}
	}

	// 가상의 꼭지점 생성
	double vertex_distance_new = (azimuth_index_list_map[0][0] + azimuth_index_list_map[1][0] + azimuth_index_list_map[2][0]) / 3;
	int azimuth_index_list_map_01 = (int)azimuth_index_list_map[0][1];
	int azimuth_index_list_map_11 = (int)azimuth_index_list_map[1][1];
	int azimuth_index_list_map_21 = (int)azimuth_index_list_map[2][1];

	//dynamic arr
//	for(int i =0; i<azimuth_index_list_size; i++)
//	{
//		free(azimuth_index_list_map[i]);
//	}
//	free(azimuth_index_list_map);

	int temp_azimuthidx01 = azimuth_index_list[azimuth_index_list_map_01].azimuth_index;
	int temp_azimuthidx11 = azimuth_index_list[azimuth_index_list_map_11].azimuth_index;
	int temp_azimuthidx21 = azimuth_index_list[azimuth_index_list_map_21].azimuth_index;

	int temp_stopidx01 = azimuth_index_list[azimuth_index_list_map_01].stop_index;
	int temp_stopidx11 = azimuth_index_list[azimuth_index_list_map_11].stop_index;
	int temp_stopidx21 = azimuth_index_list[azimuth_index_list_map_21].stop_index;

	double vertex_azimuth_new = (
			block_data_bram_noise[temp_azimuthidx01 * StopCount + temp_stopidx01].azimuth_ +
			block_data_bram_noise[temp_azimuthidx11 * StopCount + temp_stopidx11].azimuth_ +
			block_data_bram_noise[temp_azimuthidx21 * StopCount + temp_stopidx21].azimuth_
	) / 3;

//	float x_pos_center = 0, y_pos_center = 0;//, pos_z = 0;

	double temp_elevation = elevation[31 - (channel_index * 2) + istop];
	// if(temp_elevation < 0)
	// {
	// 	temp_elevation *= -1;
	// }
	// float cos_elevation = cos_approximation_value_map[(int)(temp_elevation*10)];
	float cos_elevation = cos(temp_elevation * DEG2RAD);

	// float cos_azimuth = 0;
	// float sin_azimuth = 0;	
	// if(vertex_azimuth_new < 0)
	// {
	// 	cos_azimuth = cos_approximation_value_map[(int)(vertex_azimuth_new*-1*10)];
	// 	sin_azimuth = sin_approximation_value_map[(int)((360+vertex_azimuth_new)*10)];
	// }
	// else
	// {
	// 	cos_azimuth = cos_approximation_value_map[(int)(vertex_azimuth_new*10)];
	// 	sin_azimuth = sin_approximation_value_map[(int)((vertex_azimuth_new)*10)];
	// }
	float cos_azimuth = cos((vertex_azimuth_new) * DEG2RAD);
	float sin_azimuth = sin((vertex_azimuth_new) * DEG2RAD);
	float x_pos_center = vertex_distance_new * cos_elevation * cos_azimuth;
	float y_pos_center = vertex_distance_new * cos_elevation * sin_azimuth;

	//군집의 가장 왼쪽 포인트, 가장 오른쪽 포인트의  X, Y 좌표 획득
	double temp_dis1 = block_data_bram_noise[azimuth_index_list[0].azimuth_index * StopCount + azimuth_index_list[0].stop_index].distance_[channel_index];
	double temp_azi1 = block_data_bram_noise[azimuth_index_list[0].azimuth_index * StopCount + azimuth_index_list[0].stop_index].azimuth_;

	// double x_pos_left = 0;
	// double y_pos_left = 0;
	// if(temp_azi1 < 0)
	// {
	//  	x_pos_left = temp_dis1 * cos_elevation * cos_approximation_value_map[(int)(temp_azi1*-1*10)];
	//  	y_pos_left = temp_dis1 * cos_elevation * sin_approximation_value_map[(int)((360+temp_azi1)*10)];
	// }
	// else
	// {
	//  	x_pos_left = temp_dis1 * cos_elevation * cos_approximation_value_map[(int)(temp_azi1*10)];
	//  	y_pos_left = temp_dis1 * cos_elevation * sin_approximation_value_map[(int)((temp_azi1)*10)];		
	// }
	double x_pos_left = temp_dis1 * cos_elevation * cos((temp_azi1) * DEG2RAD);
	double y_pos_left = temp_dis1 * cos_elevation * sin((temp_azi1) * DEG2RAD);

	double temp_dis2 = block_data_bram_noise[azimuth_index_list[azimuth_index_list_size-1].azimuth_index * StopCount + azimuth_index_list[azimuth_index_list_size-1].stop_index].distance_[channel_index];
	double temp_azi2 = block_data_bram_noise[azimuth_index_list[azimuth_index_list_size-1].azimuth_index * StopCount + azimuth_index_list[azimuth_index_list_size-1].stop_index].azimuth_;

	// double x_pos_right = 0;
	// double y_pos_right = 0;
	// if(temp_azi2< 0)
	// {
	// 	x_pos_right = temp_dis2 * cos_elevation * cos_approximation_value_map[(int)(temp_azi2*-1*10)];
	// 	y_pos_right = temp_dis2 * cos_elevation * sin_approximation_value_map[(int)((360+temp_azi2)*10)];
	// }
	// else
	// {
	// 	x_pos_right = temp_dis2 * cos_elevation * cos_approximation_value_map[(int)(temp_azi2*10)];
	// 	y_pos_right = temp_dis2 * cos_elevation * sin_approximation_value_map[(int)(temp_azi2*10)];
	// }
	double x_pos_right = temp_dis2 * cos_elevation * cos((temp_azi2)*DEG2RAD);
	double y_pos_right = temp_dis2 * cos_elevation * sin((temp_azi2)*DEG2RAD);

	//double x_pos_center = lidar_point_cloud_data_[lidar_index].stop_[vertex_point_stop_index].top_bottom_[top_bottom_index].azimuth_[vertexPointIndex].channel_[channel_index].point_[0];
	//double y_pos_center = lidar_point_cloud_data_[lidar_index].stop_[vertex_point_stop_index].top_bottom_[top_bottom_index].azimuth_[vertexPointIndex].channel_[channel_index].point_[1];

//	double x_pos_center = pos_x;
//	double y_pos_center = pos_y;

	// 2개의 가상의 선, 임시 angle 계산
	double angle1 = atan2((y_pos_left - y_pos_center), (x_pos_left - x_pos_center)) * cal_var;
	double angle2 = atan2((y_pos_right - y_pos_center), (x_pos_right - x_pos_center)) * cal_var;

	//double azimuth_center = lidar_point_cloud_data_[lidar_index].stop_[vertex_point_stop_index].top_bottom_[top_bottom_index].azimuth_[vertexPointIndex].azimuth_raw_;
	double azimuth_center = vertex_azimuth_new;

	double angle_check1 = fabs(angle1 - azimuth_center) * 0.3;
	double angle_check2 = fabs(azimuth_center - angle2) * 0.3;

//	double angle_check1 = fabs(angle1 - azimuth_center) * 0.5;
//	double angle_check2 = fabs(azimuth_center - angle2) * 0.5;

	// double angle_check1 = fabs(angle1 - azimuth_center) / 2;
	// double angle_check2 = fabs(azimuth_center - angle2) / 2;

	double min_angle1 = angle1;
	double min_angle2 = angle2;

	double squared_error_sum_min = 99999999;
	double error_sum_min1 = 99999999;
	double error_sum_min2 = 99999999;

	for (double angle = angle1 - angle_check1; angle < angle1 + angle_check1; angle += 1.0)
	{
		// double a = 0;
		// if(angle < 0)
		// {
		// 	a = tan_approximation_value_map[(int)((360+angle)*10)];
		// }
		// else
		// {
		// 	a = tan_approximation_value_map[(int)(angle*10)];
		// }
		double a = tan(angle * DEG2RAD);
		double b = y_pos_center - (a * x_pos_center);

		double squared_error_sum = 0;
		double error_sum = 0;
		for (int i = 0; i <= azimuth_index_listIndex; ++i)
		{
			double temp_dis_ = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
			double temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;
			// double x = 0;
			// double y = 0;
			// if(temp_azi < 0)
			// {
			// 	x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi*-1)*10)];
			// 	y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360+temp_azi)*10)];
			// }
			// else
			// {
			// 	x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi)*10)];
			// 	y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi)*10)];
			// }
			double x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
			double y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//			double x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//			double y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);
			double error = fabs(((a * x) + b) - y);

			squared_error_sum += (error * error);
			error_sum += error;
		}
		if (squared_error_sum <= squared_error_sum_min)
		{
			squared_error_sum_min = squared_error_sum;
			error_sum_min1 = error_sum;
			min_angle1 = angle;
		}
	}

	squared_error_sum_min = 99999999;

	// 꼭지점 기준 오른쪽 점들을 지나는 선의 정확한 angle 획득
	for (double angle = angle2 - angle_check2; angle < angle2 + angle_check2; angle += 1.0)
	{
		// double a = 0;
		// if(angle < 0)
		// {
		// 	a = tan_approximation_value_map[(int)((360+angle)*10)];
		// }
		// else
		// {
		// 	a = tan_approximation_value_map[(int)(angle*10)];
		// }
		double a = tan(angle * DEG2RAD);
		double b = y_pos_center - (a * x_pos_center);

		double squared_error_sum = 0;
		double error_sum = 0;
		for (int i = azimuth_index_listIndex; i < azimuth_index_list_size; ++i)
		{
			double temp_dis_ = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
			double temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;
			// double x = 0;
			// double y = 0;
			// if(temp_azi < 0)
			// {
			// 	x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi*-1)*10)];
			// 	y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360+temp_azi)*10)];
			// }	
			// else
			// {
			// 	x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi)*10)];
			// 	y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi)*10)];
			// }		
			double x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
			double y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//			double x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//			double y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

			double error = fabs(((a * x) + b) - y);

			squared_error_sum += (error * error);
			error_sum += error;
		}

		if (squared_error_sum <= squared_error_sum_min)
		{
			squared_error_sum_min = squared_error_sum;
			error_sum_min2 = error_sum;
			min_angle2 = angle;
		}
	}

	// 2개의 선에서 일정 에러값 내에 존재하는 포인트 갯수 계산
	// double a1 = 0;
	// if(min_angle1 < 0)
	// {
	// 	a1 = tan_approximation_value_map[(int)((360+min_angle1)*10)];
	// }
	// else
	// {
	// 	a1 = tan_approximation_value_map[(int)((min_angle1)*10)];;
	// }
	double a1 = tan(min_angle1 * DEG2RAD);
	double b1 = y_pos_center - (a1 * x_pos_center);

	// double a2 = 0;
	// if(min_angle2 < 0)
	// {
	// 	a2 = tan_approximation_value_map[(int)((360+min_angle2)*10)];
	// }
	// else
	// {
	// 	a2 = tan_approximation_value_map[(int)((min_angle2)*10)];;
	// }
	double a2 = tan(min_angle2 * DEG2RAD);
	double b2 = y_pos_center - (a2 * x_pos_center);

	int point_in_v = 0;
	for (int i = 0; i < azimuth_index_list_size; ++i)
	{
		double temp_dis_ = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
		double temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;
		// double x = 0;
		// double y = 0;
		// if(temp_azi < 0)
		// {
		// 	x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi*-1)*10)];
		// 	y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360+temp_azi)*10)];
		// }
		// else
		// {
		// 	x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi)*10)];
		// 	y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi)*10)];
		// }
		double x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
		double y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//		double x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//		double y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

		double error1 = fabs(((a1 * x) + b1) - y);
		double error2 = fabs(((a2 * x) + b2) - y);

		if (error1 < 0.07 || error2 < 0.07)
		{
			++point_in_v;
		}
	}

	// 아래 4개 조건 만족시 V 노이즈로 판단
	// 1. 꼭지점이 가운데 존재하는지 체크
	// 2. 포인트들이 2개 선과 얼마나 일치하는지 체크
	// 3. 두 라인의 세타값 체크 (너무 벌어지면 안됨)
	// 4. Azimuth 기준 양쪽 세타값이 동일한지 체크
	double temp_1 = (double)azimuth_index_listIndex / (azimuth_index_list_size - 1);
	if (temp_1 >= 0.15 && temp_1 <= 0.85)
//	if ((double)azimuth_index_listIndex / (azimuth_index_list_size - 1) >= 0.15 && (double)azimuth_index_listIndex / (azimuth_index_list_size - 1) <= 0.85)
	{
		if ((vertex_distance_new >= 3 && ((double)point_in_v / azimuth_index_list_size) > 0.7 && ((error_sum_min1 + error_sum_min2) / azimuth_index_list_size) <= 0.06) ||
				(vertex_distance_new < 3 && ((double)point_in_v / azimuth_index_list_size) > 0.5 && ((error_sum_min1 + error_sum_min2) / azimuth_index_list_size) <= 0.08))
		{
			if (fabs(min_angle1 - min_angle2) < 100.0)
			{
				if (fabs((min_angle1 - azimuth_center) - (azimuth_center - min_angle2)) < 25.0)
				{
					// 봉을 제외한 노이즈 부분 제거(왼쪽)
					// azimuth 라인과 직교하는 라인 기준, 30도를 넘으면 노이즈 시작
					double margin_angle = 30;
					bool is_noise_start = false;
					for (int i = azimuth_index_listIndex; i >= 0; --i)
					{
						if (i != azimuth_index_listIndex)
						{
							if (is_noise_start == true)
							{

								block_data_total_test->block_data_bram[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].data_points[channel_index] = 0;
								//block_data_bram_noise->block_data_bram[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].data_points[channel_index] = 0;
								// lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[0] = 0;
								// lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[1] = 0;
								// lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[2] = 0;
							}
							else
							{
								double temp_dis_ = block_data_bram_noise[azimuth_index_list[i + 1].azimuth_index * StopCount + azimuth_index_list[i + 1].stop_index].distance_[channel_index];
								double temp_azi = block_data_bram_noise[azimuth_index_list[i + 1].azimuth_index * StopCount + azimuth_index_list[i + 1].stop_index].azimuth_;

								// double last_x = 0;
								// double last_y = 0;
								// if(temp_azi < 0)
								// {
								// 	last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi*-1)*10)];
								// 	last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360+temp_azi)*10)];
								// }
								// else
								// {
								// 	last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi)*10)];
								// 	last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi)*10)];
								// }
								double last_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								double last_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//								double last_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double last_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								temp_dis_ = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
								temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;

								// double cur_x = 0;
								// double cur_y = 0;
								// if(temp_azi < 0)
								// {
								// 	cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi*-1)*10)];
								// 	cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360+temp_azi)*10)];
								// }
								// else
								// {
								// 	cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi)*10)];
								// 	cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi)*10)];
								// }
								double cur_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								double cur_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//								double cur_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double cur_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								double angle = atan2((cur_y - last_y), (cur_x - last_x)) * cal_var;

								if (((angle > (vertex_azimuth_new - 90 - margin_angle) && angle < (vertex_azimuth_new - 90 + margin_angle)) ||
												(angle > (vertex_azimuth_new + 90 - margin_angle) && angle < (vertex_azimuth_new + 90 + margin_angle))) == false)
								{
									is_noise_start = true;
								}
							}
						}
					}

					// 봉을 제외한 노이즈 부분 제거(왼쪽)
					// azimuth 라인과 직교하는 라인 기준, 30도를 넘으면 노이즈 시작
					is_noise_start = false;
					for (int i = azimuth_index_listIndex; i < azimuth_index_list_size; ++i)
					{
						if (i != azimuth_index_listIndex)
						{
							if (is_noise_start == true)
							{
								block_data_total_test->block_data_bram[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].data_points[channel_index] = 0;
								//block_data_bram_noise->block_data_bram[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].data_points[channel_index] = 0;

								//lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[0] = 0;
								//lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[1] = 0;
								//lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[2] = 0;
							}
							else
							{
								double temp_dis_ = block_data_bram_noise[azimuth_index_list[i - 1].azimuth_index * StopCount + azimuth_index_list[i - 1].stop_index].distance_[channel_index];
								double temp_azi = block_data_bram_noise[azimuth_index_list[i - 1].azimuth_index * StopCount + azimuth_index_list[i - 1].stop_index].azimuth_;
//								double last_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double last_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								// double last_x = 0;
								// double last_y = 0;
								// if(temp_azi < 0)
								// {
								// 	last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi*-1)*10)];
								// 	last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360+temp_azi)*10)];
								// }
								// else
								// {
								// 	last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi)*10)];
								// 	last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi)*10)];
								// }
								double last_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								double last_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);

								temp_dis_ = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
								temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;

								// double cur_x = 0;
								// double cur_y = 0;
								// if(temp_azi < 0)
								// {
								// 	cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi*-1)*10)];
								// 	cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360+temp_azi)*10)];
								// }
								// else
								// {
								// 	cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi)*10)];
								// 	cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi)*10)];
								// }
								double cur_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								double cur_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//								double cur_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double cur_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								double angle = atan2((cur_y - last_y), (cur_x - last_x)) * cal_var;

								if (((angle > (vertex_azimuth_new - 90 - margin_angle) && angle < (vertex_azimuth_new - 90 + margin_angle)) &&
												(angle > (vertex_azimuth_new + 90 - margin_angle) && angle < (vertex_azimuth_new + 90 + margin_angle))) == false)
								{
									is_noise_start = true;
								}
							}
						}
					}

				}
			}
		}
	}
}

#endif

void RemoveVNoise_new(ConvertedDataBlock* block_data_bram_noise, volatile RawDataBlock* block_data_total_test,
		PointInfo azimuth_index_list[], Vnoise_factor *vnoise_factor)
{

	double distanceMin = 999;
	int azimuth_index_listIndex = -1;
	unsigned int azimuth_index_list_size = vnoise_factor->azimuth_index_list_size;
	unsigned int channel_index = vnoise_factor->channel_index;
	unsigned int istop = vnoise_factor->istop;
	//dynamic arr
//	double **azimuth_index_list_map;
//	azimuth_index_list_map = (double**)malloc(sizeof(double*)*azimuth_index_list_size);
//	for(int i = 0; i< azimuth_index_list_size; i++)
//	{
//		azimuth_index_list_map[i] = (double*)calloc(2, sizeof(double));
//	}

#if 0

#else

	unsigned int smalls2[3] = {0, 1, 2};
	unsigned int smaals_maxidx = 0;
	double distanceMax = 0;
	for(unsigned int i = 0 ; i < 3 ; ++i)
	{
		double distance =
				block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];

		if(distanceMax > distance)
		{
			distanceMax = distance;
			smaals_maxidx = i;
		}
	}
	distanceMax = 0;

	// V 모양의 꼭지점에 해당하는 포인트 검색
	for(unsigned int i = 0 ; i < azimuth_index_list_size ; ++i)
	{
		unsigned int azimuth_index_ = azimuth_index_list[i].azimuth_index;
		unsigned int stop_index_ = azimuth_index_list[i].stop_index;
		double distance = block_data_bram_noise[azimuth_index_ * StopCount + stop_index_].distance_[channel_index];

		if(i >= 3)
		{
			double distance2 = block_data_bram_noise[azimuth_index_list[smalls2[smaals_maxidx]].azimuth_index * StopCount
					+ azimuth_index_list[smalls2[smaals_maxidx]].stop_index].distance_[channel_index];
			if(distance2 > distance)
			{
				smalls2[smaals_maxidx] = i;
				distanceMax = distance;
				for(unsigned int k = 0 ; k < 3 ; ++k)
				{
					double temp_dis = block_data_bram_noise[azimuth_index_list[smalls2[k]].azimuth_index * StopCount
							+ azimuth_index_list[smalls2[k]].stop_index].distance_[channel_index];
					if(temp_dis > distanceMax)
					{
						distanceMax = temp_dis;
						smaals_maxidx = k;
					}
				}
			}
		}
		if(distance <= distanceMin)
		{
			distanceMin = distance;
			azimuth_index_listIndex = i;
		}
	}

	// 가상의 꼭지점 생성
	double vertex_distance_new =
			(block_data_bram_noise[azimuth_index_list[smalls2[0]].azimuth_index * StopCount + azimuth_index_list[smalls2[0]].stop_index].distance_[channel_index]
					+ block_data_bram_noise[azimuth_index_list[smalls2[1]].azimuth_index * StopCount + azimuth_index_list[smalls2[1]].stop_index].distance_[channel_index]
					+ block_data_bram_noise[azimuth_index_list[smalls2[2]].azimuth_index * StopCount + azimuth_index_list[smalls2[2]].stop_index].distance_[channel_index])
					/ 3;

	double vertex_azimuth_new = (block_data_bram_noise[azimuth_index_list[smalls2[0]].azimuth_index * StopCount
			+ azimuth_index_list[smalls2[0]].stop_index].azimuth_
			+ block_data_bram_noise[azimuth_index_list[smalls2[1]].azimuth_index * StopCount + azimuth_index_list[smalls2[1]].stop_index].azimuth_
			+ block_data_bram_noise[azimuth_index_list[smalls2[2]].azimuth_index * StopCount + azimuth_index_list[smalls2[2]].stop_index].azimuth_)
			/ 3;

#endif

	//dynamic arr
//	for(int i =0; i<azimuth_index_list_size; i++)
//	{
//		free(azimuth_index_list_map[i]);
//	}
//	free(azimuth_index_list_map);

//	float x_pos_center = 0, y_pos_center = 0;//, pos_z = 0;

	double temp_elevation = elevation[31 - (channel_index * 2) + istop];
	if(temp_elevation < 0)
	{
		temp_elevation *= -1;
	}
	double cos_elevation = cos_approximation_value_map[(int)(temp_elevation * 10)];
	//float cos_elevation = cos(temp_elevation * DEG2RAD);

	double cos_azimuth = 0;
	double sin_azimuth = 0;
	if(vertex_azimuth_new < 0)
	{
		cos_azimuth = cos_approximation_value_map[(int)(vertex_azimuth_new * -1 * 10)];
		sin_azimuth = sin_approximation_value_map[(int)((360 + vertex_azimuth_new) * 10)];
	}
	else
	{
		cos_azimuth = cos_approximation_value_map[(int)(vertex_azimuth_new * 10)];
		sin_azimuth = sin_approximation_value_map[(int)((vertex_azimuth_new) * 10)];
	}
	// float cos_azimuth = cos((vertex_azimuth_new) * DEG2RAD);
	// float sin_azimuth = sin((vertex_azimuth_new) * DEG2RAD);
	double x_pos_center = vertex_distance_new * cos_elevation * cos_azimuth;
	double y_pos_center = vertex_distance_new * cos_elevation * sin_azimuth;

	//군집의 가장 왼쪽 포인트, 가장 오른쪽 포인트의  X, Y 좌표 획득
	double temp_dis1 =
			block_data_bram_noise[azimuth_index_list[0].azimuth_index * StopCount + azimuth_index_list[0].stop_index].distance_[channel_index];
	double temp_azi1 = block_data_bram_noise[azimuth_index_list[0].azimuth_index * StopCount + azimuth_index_list[0].stop_index].azimuth_;

	double x_pos_left = 0;
	double y_pos_left = 0;
	if(temp_azi1 < 0)
	{
		x_pos_left = temp_dis1 * cos_elevation * cos_approximation_value_map[(int)(temp_azi1 * -1 * 10)];
		y_pos_left = temp_dis1 * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi1) * 10)];
	}
	else
	{
		x_pos_left = temp_dis1 * cos_elevation * cos_approximation_value_map[(int)(temp_azi1 * 10)];
		y_pos_left = temp_dis1 * cos_elevation * sin_approximation_value_map[(int)((temp_azi1) * 10)];
	}
	// double x_pos_left = temp_dis1 * cos_elevation * cos((temp_azi1) * DEG2RAD);
	// double y_pos_left = temp_dis1 * cos_elevation * sin((temp_azi1) * DEG2RAD);

	double temp_dis2 = block_data_bram_noise[azimuth_index_list[azimuth_index_list_size - 1].azimuth_index * StopCount
			+ azimuth_index_list[azimuth_index_list_size - 1].stop_index].distance_[channel_index];
	double temp_azi2 = block_data_bram_noise[azimuth_index_list[azimuth_index_list_size - 1].azimuth_index * StopCount
			+ azimuth_index_list[azimuth_index_list_size - 1].stop_index].azimuth_;

	double x_pos_right = 0;
	double y_pos_right = 0;
	if(temp_azi2 < 0)
	{
		x_pos_right = temp_dis2 * cos_elevation * cos_approximation_value_map[(int)(temp_azi2 * -1 * 10)];
		y_pos_right = temp_dis2 * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi2) * 10)];
	}
	else
	{
		x_pos_right = temp_dis2 * cos_elevation * cos_approximation_value_map[(int)(temp_azi2 * 10)];
		y_pos_right = temp_dis2 * cos_elevation * sin_approximation_value_map[(int)(temp_azi2 * 10)];
	}
	// double x_pos_right = temp_dis2 * cos_elevation * cos((temp_azi2)*DEG2RAD);
	// double y_pos_right = temp_dis2 * cos_elevation * sin((temp_azi2)*DEG2RAD);

	//double x_pos_center = lidar_point_cloud_data_[lidar_index].stop_[vertex_point_stop_index].top_bottom_[top_bottom_index].azimuth_[vertexPointIndex].channel_[channel_index].point_[0];
	//double y_pos_center = lidar_point_cloud_data_[lidar_index].stop_[vertex_point_stop_index].top_bottom_[top_bottom_index].azimuth_[vertexPointIndex].channel_[channel_index].point_[1];

//	double x_pos_center = pos_x;
//	double y_pos_center = pos_y;

	// 2개의 가상의 선, 임시 angle 계산
	double angle1 = atan2((y_pos_left - y_pos_center), (x_pos_left - x_pos_center)) * cal_var;
	double angle2 = atan2((y_pos_right - y_pos_center), (x_pos_right - x_pos_center)) * cal_var;

	//double azimuth_center = lidar_point_cloud_data_[lidar_index].stop_[vertex_point_stop_index].top_bottom_[top_bottom_index].azimuth_[vertexPointIndex].azimuth_raw_;
	double azimuth_center = vertex_azimuth_new;

//	double angle_check1 = fabs(angle1 - azimuth_center) * 0.3;
//	double angle_check2 = fabs(azimuth_center - angle2) * 0.3;

//	double angle_check1 = fabs(angle1 - azimuth_center) * 0.18;
//	double angle_check2 = fabs(azimuth_center - angle2) * 0.18;

	double angle_check1 = fabs(angle1 - azimuth_center) * 0.15;
	double angle_check2 = fabs(azimuth_center - angle2) * 0.15;

//	double angle_check1 = fabs(angle1 - azimuth_center) * 0.16;
//	double angle_check2 = fabs(azimuth_center - angle2) * 0.16;

//	double angle_check1 = fabs(angle1 - azimuth_center) * 0.5;
//	double angle_check2 = fabs(azimuth_center - angle2) * 0.5;

	// double angle_check1 = fabs(angle1 - azimuth_center) / 2;
	// double angle_check2 = fabs(azimuth_center - angle2) / 2;

	double min_angle1 = angle1;
	double min_angle2 = angle2;

	double squared_error_sum_min = 99999999;
	double error_sum_min1 = 99999999;
	double error_sum_min2 = 99999999;

	for(double angle = angle1 - angle_check1 ; angle < angle1 + angle_check1 ; angle += 1.0)
	{
		double a = 0;
		if(angle < 0)
		{
			a = tan_approximation_value_map[(int)((360 + angle) * 10)];
		}
		else
		{
			a = tan_approximation_value_map[(int)(angle * 10)];
		}
		// double a = tan(angle * DEG2RAD);
		double b = y_pos_center - (a * x_pos_center);

		double squared_error_sum = 0;
		double error_sum = 0;
		for(unsigned int i = 0 ; i <= azimuth_index_listIndex ; ++i)
		{
			double temp_dis_ =
					block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
			double temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;
			double x = 0;
			double y = 0;
			if(temp_azi < 0)
			{
				x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
				y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
			}
			else
			{
				x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
				y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
			}
			// double x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
			// double y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//			double x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//			double y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);
			double error = fabs(((a * x) + b) - y);

			squared_error_sum += (error * error);
			error_sum += error;
		}
		if(squared_error_sum <= squared_error_sum_min)
		{
			squared_error_sum_min = squared_error_sum;
			error_sum_min1 = error_sum;
			min_angle1 = angle;
		}
	}

	squared_error_sum_min = 99999999;

	// 꼭지점 기준 오른쪽 점들을 지나는 선의 정확한 angle 획득
	for(double angle = angle2 - angle_check2 ; angle < angle2 + angle_check2 ; angle += 1.0)
	{
		double a = 0;
		if(angle < 0)
		{
			a = tan_approximation_value_map[(int)((360 + angle) * 10)];
		}
		else
		{
			a = tan_approximation_value_map[(int)(angle * 10)];
		}
		// double a = tan(angle * DEG2RAD);
		double b = y_pos_center - (a * x_pos_center);

		double squared_error_sum = 0;
		double error_sum = 0;
		for(unsigned int i = azimuth_index_listIndex ; i < azimuth_index_list_size ; ++i)
		{
			double temp_dis_ =
					block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
			double temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;
			double x = 0;
			double y = 0;
			if(temp_azi < 0)
			{
				x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
				y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
			}
			else
			{
				x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
				y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
			}
			// double x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
			// double y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//			double x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//			double y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

			double error = fabs(((a * x) + b) - y);

			squared_error_sum += (error * error);
			error_sum += error;
		}

		if(squared_error_sum <= squared_error_sum_min)
		{
			squared_error_sum_min = squared_error_sum;
			error_sum_min2 = error_sum;
			min_angle2 = angle;
		}
	}

	// 2개의 선에서 일정 에러값 내에 존재하는 포인트 갯수 계산
	double a1 = 0;
	if(min_angle1 < 0)
	{
		a1 = tan_approximation_value_map[(int)((360 + min_angle1) * 10)];
	}
	else
	{
		a1 = tan_approximation_value_map[(int)((min_angle1) * 10)];
	}
	// double a1 = tan(min_angle1 * DEG2RAD);
	double b1 = y_pos_center - (a1 * x_pos_center);

	double a2 = 0;
	if(min_angle2 < 0)
	{
		a2 = tan_approximation_value_map[(int)((360 + min_angle2) * 10)];
	}
	else
	{
		a2 = tan_approximation_value_map[(int)((min_angle2) * 10)];
	}
	// double a2 = tan(min_angle2 * DEG2RAD);
	double b2 = y_pos_center - (a2 * x_pos_center);

	unsigned int point_in_v = 0;
//	for(int i = 0 ; i < azimuth_index_list_size ; ++i)
	for(int i = azimuth_index_list_size-1 ; i >=0 ; --i)
	{
		double temp_dis_ =
				block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
		double temp_azi = block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;
		double x = 0;
		double y = 0;
		if(temp_azi < 0)
		{
			x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
			y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
		}
		else
		{
			x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
			y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
		}
		// double x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
		// double y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//		double x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//		double y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

		double error1 = fabs(((a1 * x) + b1) - y);
		double error2 = fabs(((a2 * x) + b2) - y);

		if(error1 < 0.07 || error2 < 0.07)
		{
			++point_in_v;
		}
	}

	// 아래 4개 조건 만족시 V 노이즈로 판단
	// 1. 꼭지점이 가운데 존재하는지 체크
	// 2. 포인트들이 2개 선과 얼마나 일치하는지 체크
	// 3. 두 라인의 세타값 체크 (너무 벌어지면 안됨)
	// 4. Azimuth 기준 양쪽 세타값이 동일한지 체크
	double temp_1 = (double)azimuth_index_listIndex / (azimuth_index_list_size - 1);
	if(temp_1 >= 0.15 && temp_1 <= 0.85)
//	if ((double)azimuth_index_listIndex / (azimuth_index_list_size - 1) >= 0.15 && (double)azimuth_index_listIndex / (azimuth_index_list_size - 1) <= 0.85)
	{
		if((vertex_distance_new >= 3 && ((double)point_in_v / azimuth_index_list_size) > 0.7
				&& ((error_sum_min1 + error_sum_min2) / azimuth_index_list_size) <= 0.06)
				|| (vertex_distance_new < 3 && ((double)point_in_v / azimuth_index_list_size) > 0.5
						&& ((error_sum_min1 + error_sum_min2) / azimuth_index_list_size) <= 0.08))
		{
			if(fabs(min_angle1 - min_angle2) < 100.0)
			{
				if(fabs((min_angle1 - azimuth_center) - (azimuth_center - min_angle2)) < 25.0)
				{
					// 봉을 제외한 노이즈 부분 제거(왼쪽)
					// azimuth 라인과 직교하는 라인 기준, 30도를 넘으면 노이즈 시작
					double margin_angle = 30;
					bool is_noise_start = false;
					for(int i = azimuth_index_listIndex ; i >= 0 ; --i)
					{
						if(i != azimuth_index_listIndex)
						{
							if(is_noise_start == true)
							{

								block_data_total_test[azimuth_index_list[i].azimuth_index * StopCount
										+ azimuth_index_list[i].stop_index].distance_[channel_index].distance = 0;
								//block_data_bram_noise->block_data_bram[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].data_points[channel_index] = 0;
								// lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[0] = 0;
								// lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[1] = 0;
								// lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[2] = 0;
							}
							else
							{
								double temp_dis_ = block_data_bram_noise[azimuth_index_list[i + 1].azimuth_index * StopCount
										+ azimuth_index_list[i + 1].stop_index].distance_[channel_index];
								double temp_azi = block_data_bram_noise[azimuth_index_list[i + 1].azimuth_index * StopCount
										+ azimuth_index_list[i + 1].stop_index].azimuth_;

								double last_x = 0;
								double last_y = 0;
								if(temp_azi < 0)
								{
									last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
									last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
								}
								else
								{
									last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
									last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
								}
								// double last_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								// double last_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//								double last_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double last_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								temp_dis_ =
										block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
								temp_azi =
										block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;

								double cur_x = 0;
								double cur_y = 0;
								if(temp_azi < 0)
								{
									cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
									cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
								}
								else
								{
									cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
									cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
								}
								// double cur_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								// double cur_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//								double cur_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double cur_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								double angle = atan2((cur_y - last_y), (cur_x - last_x)) * cal_var;

								if(((angle > (vertex_azimuth_new - 90 - margin_angle) && angle < (vertex_azimuth_new - 90 + margin_angle))
										|| (angle > (vertex_azimuth_new + 90 - margin_angle) && angle < (vertex_azimuth_new + 90 + margin_angle)))
										== false)
								{
									is_noise_start = true;
								}
							}
						}
					}

					// 봉을 제외한 노이즈 부분 제거(오른쪽)
					// azimuth 라인과 직교하는 라인 기준, 30도를 넘으면 노이즈 시작
					is_noise_start = false;
					for(unsigned int i = azimuth_index_listIndex ; i < azimuth_index_list_size ; ++i)
					{
						if(i != azimuth_index_listIndex)
						{
							if(is_noise_start == true)
							{
								block_data_total_test[azimuth_index_list[i].azimuth_index * StopCount
										+ azimuth_index_list[i].stop_index].distance_[channel_index].distance = 0;
								//block_data_bram_noise->block_data_bram[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].data_points[channel_index] = 0;

								//lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[0] = 0;
								//lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[1] = 0;
								//lidar_point_cloud_data_[lidar_index].stop_[azimuth_index_list[i].stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index_list[i].azimuth_index].channel_[channel_index].point_[2] = 0;
							}
							else
							{
								double temp_dis_ = block_data_bram_noise[azimuth_index_list[i - 1].azimuth_index * StopCount
										+ azimuth_index_list[i - 1].stop_index].distance_[channel_index];
								double temp_azi = block_data_bram_noise[azimuth_index_list[i - 1].azimuth_index * StopCount
										+ azimuth_index_list[i - 1].stop_index].azimuth_;
//								double last_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double last_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								double last_x = 0;
								double last_y = 0;
								if(temp_azi < 0)
								{
									last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
									last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
								}
								else
								{
									last_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
									last_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
								}
								// double last_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								// double last_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);

								temp_dis_ =
										block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].distance_[channel_index];
								temp_azi =
										block_data_bram_noise[azimuth_index_list[i].azimuth_index * StopCount + azimuth_index_list[i].stop_index].azimuth_;

								double cur_x = 0;
								double cur_y = 0;
								if(temp_azi < 0)
								{
									cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi * -1) * 10)];
									cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((360 + temp_azi) * 10)];
								}
								else
								{
									cur_x = temp_dis_ * cos_elevation * cos_approximation_value_map[(int)((temp_azi) * 10)];
									cur_y = temp_dis_ * cos_elevation * sin_approximation_value_map[(int)((temp_azi) * 10)];
								}
								// double cur_x = temp_dis_ * cos_elevation * cos((temp_azi) * DEG2RAD);
								// double cur_y = temp_dis_ * cos_elevation * sin((temp_azi) * DEG2RAD);
//								double cur_x = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * cos((temp_azi)*PI / 180);
//								double cur_y = temp_dis_ * cos(elevation[31 - (channel_index*2)+0] * PI / 180) * sin((temp_azi)*PI / 180);

								double angle = atan2((cur_y - last_y), (cur_x - last_x)) * cal_var;

								if(((angle > (vertex_azimuth_new - 90 - margin_angle) && angle < (vertex_azimuth_new - 90 + margin_angle))
										&& (angle > (vertex_azimuth_new + 90 - margin_angle) && angle < (vertex_azimuth_new + 90 + margin_angle)))
										== false)
								{
									is_noise_start = true;
								}
							}
						}
					}

				}
			}
		}
	}
}

#if 0
int ApplyVNoiseFilter_new(ConvertedDataBlock* block_data_bram_noise, volatile block_data_total_shared* block_data_total_test)
{
	Vnoise_factor vnoise_factor;
//	const int num_of_azimuths = 720;
//	for(int channel_index = 0 ; channel_index < 16 ; ++channel_index) // channel loop 16
	for(int channel_index = 16 - 1 ; channel_index >= 0 ; --channel_index) // channel loop 16
	{
		uint8_t Top_Bottom_Side = 0;
		double last_azimuth = 0;

		// 포인트 군집을 위한 공간 생성
		cloud_info cloud_info_arr[GRP_SIZE] = {0, };

		if(NUM_AZI > 0)
		{
			last_azimuth = block_data_bram_noise[0].azimuth_;
		}

		for(int azimuth_index = 0 ; azimuth_index < NUM_AZI ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
//		for(int azimuth_index = NUM_AZI -1 ; azimuth_index >= 0 ; --azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			double azimuth = block_data_bram_noise[azimuth_index * StopCount + 0].azimuth_;
			// 데이터 누락 예외처리
			if(fabs(last_azimuth - azimuth) > 0.3)
//			if (fabs(last_azimuth - azimuth) > 1)
			{
				last_azimuth = azimuth;
				continue;
			}

			for(int stop_index = 0 ; stop_index < StopCount ; ++stop_index)
			{

				double current_point_distance = block_data_bram_noise[azimuth_index * StopCount + stop_index].distance_[channel_index];

				if(current_point_distance == 0.0 && (azimuth_index == (NUM_AZI - 1) && stop_index == (StopCount - 1)) == 0)
				{
					continue;
				}

				// 각 군집 집단에 포인트가 추가되지 않는 연속 횟수 카운트 (군집화 완료 여부 체크 목적)
				if(stop_index == 0)
				{
//					for(int i = 0 ; i < GRP_SIZE ; ++i)
					for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
					{
						++cloud_info_arr[i].point_not_exist_count_;
						if(cloud_info_arr[i].point_not_exist_count_ > 1000)
						{
							cloud_info_arr[i].point_not_exist_count_ = 10;
						}
					}
				}

				// 군집화된 포인트 처리(V 노이즈 처리)
//				for(int i = 0 ; i < GRP_SIZE ; ++i)
				for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
				{
					// 특정 군집 공간에 포인트 존제 && 현재 포인트가 군집 포인트와 멀리 떨어져 있고 && 포인트가 들어오지 않은지 3회 초과 || 마지막 azimuth 포인트 일경우 최정적으로 처리
					if(cloud_info_arr[i].point_info_vec_cot > 0)
					{
						double pre_point_distance = 0.0;
						pre_point_distance =
								block_data_bram_noise[cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].azimuth_index
										* StopCount + cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].stop_index].distance_[channel_index];
						if((fabs(pre_point_distance - current_point_distance) >= 0.5
								&& cloud_info_arr[i].point_not_exist_count_ > target_point_not_exist)
								|| (azimuth_index == (NUM_AZI - 1) && stop_index == (StopCount - 1)))
						{
							// 군집된 포인트가 3개 이상일때만 처리
							if(cloud_info_arr[i].point_info_vec_cot > 3)
							{
								// V 노이즈 처리
//								RemoveVNoise(block_data_bram_noise, block_data_total_test, cloud_info_arr[i].point_info_vector_,
//										cloud_info_arr[i].point_info_vec_cot, channel_index, Top_Bottom_Side);

								vnoise_factor.azimuth_index_list_size = cloud_info_arr[i].point_info_vec_cot;
								vnoise_factor.channel_index = channel_index;
								vnoise_factor.istop = Top_Bottom_Side;
								RemoveVNoise_new(block_data_bram_noise, block_data_total_test, cloud_info_arr[i].point_info_vector_, &vnoise_factor);
								//xil_printf("test\r\n");
							}

							// 포인트 및 카운트 초기화
//							for(int cot_clear = 0; cot_clear < cloud_info_arr[i].point_info_vec_cot; ++cot_clear)
//							{
//								cloud_info_arr[i].point_info_vector_[cot_clear].azimuth_index = 0;
//							 	cloud_info_arr[i].point_info_vector_[cot_clear].stop_index = 0;
//							}
							memset(cloud_info_arr[i].point_info_vector_, 0, 1152 * sizeof(PointInfo));
							cloud_info_arr[i].point_info_vec_cot = 0;
							cloud_info_arr[i].point_not_exist_count_ = 0;
						}
					}
				}

				int is_inserted = 0;
				// 현재 포인트가 군집화 될수 있는 집단이 있는지 체크
//				for(int i = 0 ; i < GRP_SIZE ; ++i)
				int empty_idx = -1;
				for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
				{
					if(cloud_info_arr[i].point_info_vec_cot > 0)
					{
						double pre_point_distance = 0.0;
						int temp_azi_idx = cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].azimuth_index;
						int temp_stop_idx = cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].stop_index;
						pre_point_distance = block_data_bram_noise[temp_azi_idx * StopCount + temp_stop_idx].distance_[channel_index];
						if(fabs(pre_point_distance - current_point_distance) < 0.5)
						{
							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].azimuth_index = azimuth_index;
							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].stop_index = stop_index;
							++cloud_info_arr[i].point_info_vec_cot;
							cloud_info_arr[i].point_not_exist_count_ = 0;
							is_inserted = 1;
							break;
						}
					}
					else
					{
						if(empty_idx == -1)
						{
							empty_idx = i;
						}
					}
				}

				// 어느 집단에 포함되지 않는다면, 새로운 집단 생성
				if(is_inserted == 0)
				{

					cloud_info_arr[empty_idx].point_info_vector_[cloud_info_arr[empty_idx].point_info_vec_cot].azimuth_index = azimuth_index;
					cloud_info_arr[empty_idx].point_info_vector_[cloud_info_arr[empty_idx].point_info_vec_cot].stop_index = stop_index;
					++(cloud_info_arr[empty_idx].point_info_vec_cot);
					cloud_info_arr[empty_idx].point_not_exist_count_ = 0;
//					for(int i = GRP_SIZE -1 ; i >= 0 ; --i)
////					for(int i = 0 ; i < GRP_SIZE ; ++i)
//					{
//						if(cloud_info_arr[i].point_info_vec_cot == 0)
//						{
//							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].azimuth_index = azimuth_index;
//							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].stop_index = stop_index;
//							++cloud_info_arr[i].point_info_vec_cot;
//							cloud_info_arr[i].point_not_exist_count_ = 0;
//							break;
//						}
//					}
				}
			}
			last_azimuth = azimuth;
		}

		//top
		Top_Bottom_Side = 1;
		last_azimuth = block_data_bram_noise[0].azimuth_;
		memset(cloud_info_arr, 0, GRP_SIZE * sizeof(cloud_info));
		for(int azimuth_index = NUM_AZI ; azimuth_index < NUM_AZI * 2 ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			double azimuth = block_data_bram_noise[azimuth_index * StopCount + 0].azimuth_;
			// 데이터 누락 예외처리
			if(fabs(last_azimuth - azimuth) > 0.3)
			//if (fabs(last_azimuth - azimuth) > 1)
			{
				last_azimuth = azimuth;
				continue;
			}

			for(int stop_index = 0 ; stop_index < StopCount ; stop_index++)

			{
				double current_point_distance = block_data_bram_noise[azimuth_index * StopCount + stop_index].distance_[channel_index];

				if(current_point_distance == 0.0 && (azimuth_index == (NUM_AZI * 2 - 1) && stop_index == (StopCount - 1)) == 0)
				{
					continue;
				}

				// 각 군집 집단에 포인트가 추가되지 않는 연속 횟수 카운트 (군집화 완료 여부 체크 목적)
				if(stop_index == 0)
				{
//					for(int i = 0 ; i < GRP_SIZE ; ++i)
					for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
					{
						++cloud_info_arr[i].point_not_exist_count_;
						if(cloud_info_arr[i].point_not_exist_count_ > 1000)
						{
							cloud_info_arr[i].point_not_exist_count_ = 10;
						}
					}
				}

				// 군집화된 포인트 처리(V 노이즈 처리)
//				for(int i = 0 ; i < GRP_SIZE ; ++i)
				for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
				{
					// 특정 군집 공간에 포인트 존제 && 현재 포인트가 군집 포인트와 멀리 떨어져 있고 && 포인트가 들어오지 않은지 3회 초과 || 마지막 azimuth 포인트 일경우 최정적으로 처리
					if(cloud_info_arr[i].point_info_vec_cot > 0)
					{
						double pre_point_distance = 0.0;
						pre_point_distance =
								block_data_bram_noise[cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].azimuth_index
										* StopCount + cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].stop_index].distance_[channel_index];
						if((fabs(pre_point_distance - current_point_distance) >= 0.5
								&& cloud_info_arr[i].point_not_exist_count_ > target_point_not_exist)
								|| (azimuth_index == (NUM_AZI * 2 - 1) && stop_index == (StopCount - 1)))
						{
							// 군집된 포인트가 3개 이상일때만 처리
							if(cloud_info_arr[i].point_info_vec_cot > 3)
							{
								// V 노이즈 처리
//								RemoveVNoise(block_data_bram_noise, block_data_total_test, cloud_info_arr[i].point_info_vector_,
//										cloud_info_arr[i].point_info_vec_cot, channel_index, Top_Bottom_Side);

								vnoise_factor.azimuth_index_list_size = cloud_info_arr[i].point_info_vec_cot;
								vnoise_factor.channel_index = channel_index;
								vnoise_factor.istop = Top_Bottom_Side;
								RemoveVNoise_new(block_data_bram_noise, block_data_total_test, cloud_info_arr[i].point_info_vector_, &vnoise_factor);
//								xil_printf("test\r\n");
							}

							// 포인트 및 카운트 초기화
//							for(int cot_clear = 0; cot_clear < cloud_info_arr[i].point_info_vec_cot; cot_clear++)
//							{
//								cloud_info_arr[i].point_info_vector_[cot_clear].azimuth_index = 0;
//							 	cloud_info_arr[i].point_info_vector_[cot_clear].stop_index = 0;
//							}
							memset(cloud_info_arr[i].point_info_vector_, 0, 1152 * sizeof(PointInfo));
							cloud_info_arr[i].point_info_vec_cot = 0;
							cloud_info_arr[i].point_not_exist_count_ = 0;
						}
					}
				}

				int is_inserted = 0;
				// 현재 포인트가 군집화 될수 있는 집단이 있는지 체크
//				for(int i = 0 ; i < GRP_SIZE ; ++i)
				int empty_idx = -1;
				for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
				{
					if(cloud_info_arr[i].point_info_vec_cot > 0)
					{
						double pre_point_distance = 0.0;
						int temp_azi_idx = cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].azimuth_index;
						int temp_stop_idx = cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].stop_index;
						pre_point_distance = block_data_bram_noise[temp_azi_idx * StopCount + temp_stop_idx].distance_[channel_index];
						if(fabs(pre_point_distance - current_point_distance) < 0.5)
						{
							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].azimuth_index = azimuth_index;
							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].stop_index = stop_index;
							++cloud_info_arr[i].point_info_vec_cot;
							cloud_info_arr[i].point_not_exist_count_ = 0;
							is_inserted = 1;
							break;
						}
					}
					else
					{
						if(empty_idx == -1)
						{
							empty_idx = i;
						}
					}
				}

				// 어느 집단에 포함되지 않는다면, 새로운 집단 생성
				if(is_inserted == 0)
				{

					cloud_info_arr[empty_idx].point_info_vector_[cloud_info_arr[empty_idx].point_info_vec_cot].azimuth_index = azimuth_index;
					cloud_info_arr[empty_idx].point_info_vector_[cloud_info_arr[empty_idx].point_info_vec_cot].stop_index = stop_index;
					++(cloud_info_arr[empty_idx].point_info_vec_cot);
					cloud_info_arr[empty_idx].point_not_exist_count_ = 0;

//					for(int i = 0 ; i < GRP_SIZE ; ++i)
//					for(int i = GRP_SIZE-1 ; i >= 0; --i)
//					{
//						if(cloud_info_arr[i].point_info_vec_cot == 0)
//						{
//							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].azimuth_index = azimuth_index;
//							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].stop_index = stop_index;
//							++(cloud_info_arr[i].point_info_vec_cot);
//							cloud_info_arr[i].point_not_exist_count_ = 0;
//							break;
//						}
//					}
				}
			}
			last_azimuth = azimuth;
		}
	}
	return 0;
}

#else
int ApplyVNoiseFilter_new(ConvertedDataBlock* block_data_bram_noise, volatile RawDataBlock* block_data_total_test)
{
	Vnoise_factor vnoise_factor;
//	const int num_of_azimuths = 720;
//	for(int channel_index = 0 ; channel_index < 16 ; ++channel_index) // channel loop 16
	for(int channel_index = 16 - 1 ; channel_index >= 0 ; --channel_index) // channel loop 16
	{
		uint8_t Top_Bottom_Side = 0;
		int last_azimuth = 0;

		// 포인트 군집을 위한 공간 생성
		cloud_info cloud_info_arr[GRP_SIZE] = {0, };

		if(NUM_AZI > 0)
		{
			last_azimuth = block_data_bram_noise[0].azimuth_ * 1000;
		}

		for(unsigned int azimuth_index = 0 ; azimuth_index < NUM_AZI ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
//		for(int azimuth_index = NUM_AZI -1 ; azimuth_index >= 0 ; --azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			int azimuth = block_data_bram_noise[azimuth_index * StopCount + 0].azimuth_ * 1000;
			// 데이터 누락 예외처리
			if(abs(last_azimuth - azimuth) > 300)
//			if (fabs(last_azimuth - azimuth) > 1)
			{
				last_azimuth = azimuth;
				continue;
			}

			for(unsigned int stop_index = 0 ; stop_index < StopCount ; ++stop_index)
			{

				double current_point_distance = block_data_bram_noise[azimuth_index * StopCount + stop_index].distance_[channel_index];

				if(current_point_distance == 0.0 && (azimuth_index == (NUM_AZI - 1) && stop_index == (StopCount - 1)) == 0)
				{
					continue;
				}

				// 각 군집 집단에 포인트가 추가되지 않는 연속 횟수 카운트 (군집화 완료 여부 체크 목적)
				if(stop_index == 0)
				{
//					for(int i = 0 ; i < GRP_SIZE ; ++i)
					for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
					{
						++cloud_info_arr[i].point_not_exist_count_;
						if(cloud_info_arr[i].point_not_exist_count_ > 1000)
						{
							cloud_info_arr[i].point_not_exist_count_ = 10;
						}
					}
				}

				// 군집화된 포인트 처리(V 노이즈 처리)
//				for(int i = 0 ; i < GRP_SIZE ; ++i)
				for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
				{
					// 특정 군집 공간에 포인트 존제 && 현재 포인트가 군집 포인트와 멀리 떨어져 있고 && 포인트가 들어오지 않은지 3회 초과 || 마지막 azimuth 포인트 일경우 최정적으로 처리
					if(cloud_info_arr[i].point_info_vec_cot > 0)
					{
						double pre_point_distance = 0.0;
						pre_point_distance =
								block_data_bram_noise[cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].azimuth_index
										* StopCount + cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].stop_index].distance_[channel_index];
						if((fabs(pre_point_distance - current_point_distance) >= 0.5
								&& cloud_info_arr[i].point_not_exist_count_ > target_point_not_exist)
								|| (azimuth_index == (NUM_AZI - 1) && stop_index == (StopCount - 1)))
						{
							// 군집된 포인트가 3개 이상일때만 처리
							if(cloud_info_arr[i].point_info_vec_cot > 3)
							{
								// V 노이즈 처리
//								RemoveVNoise(block_data_bram_noise, block_data_total_test, cloud_info_arr[i].point_info_vector_,
//										cloud_info_arr[i].point_info_vec_cot, channel_index, Top_Bottom_Side);

								vnoise_factor.azimuth_index_list_size = cloud_info_arr[i].point_info_vec_cot;
								vnoise_factor.channel_index = channel_index;
								vnoise_factor.istop = Top_Bottom_Side;
								RemoveVNoise_new(block_data_bram_noise, block_data_total_test, cloud_info_arr[i].point_info_vector_, &vnoise_factor);
								//xil_printf("test\r\n");
							}

							// 포인트 및 카운트 초기화
//							for(int cot_clear = 0; cot_clear < cloud_info_arr[i].point_info_vec_cot; ++cot_clear)
//							{
//								cloud_info_arr[i].point_info_vector_[cot_clear].azimuth_index = 0;
//							 	cloud_info_arr[i].point_info_vector_[cot_clear].stop_index = 0;
//							}
							memset(cloud_info_arr[i].point_info_vector_, 0, 1152 * sizeof(PointInfo));
							cloud_info_arr[i].point_info_vec_cot = 0;
							cloud_info_arr[i].point_not_exist_count_ = 0;
						}
					}
				}

				unsigned int is_inserted = 0;
				// 현재 포인트가 군집화 될수 있는 집단이 있는지 체크
//				for(int i = 0 ; i < GRP_SIZE ; ++i)
				int empty_idx = -1;
				for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
				{
					if(cloud_info_arr[i].point_info_vec_cot > 0)
					{
						double pre_point_distance = 0.0;
						unsigned int temp_azi_idx = cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].azimuth_index;
						unsigned int temp_stop_idx = cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].stop_index;
						pre_point_distance = block_data_bram_noise[temp_azi_idx * StopCount + temp_stop_idx].distance_[channel_index];
						if(fabs(pre_point_distance - current_point_distance) < 0.5)
						{
							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].azimuth_index = azimuth_index;
							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].stop_index = stop_index;
							++cloud_info_arr[i].point_info_vec_cot;
							cloud_info_arr[i].point_not_exist_count_ = 0;
							is_inserted = 1;
							break;
						}
					}
					else
					{
						if(empty_idx == -1)
						{
							empty_idx = i;
						}
					}
				}

				// 어느 집단에 포함되지 않는다면, 새로운 집단 생성
				if(is_inserted == 0)
				{

					cloud_info_arr[empty_idx].point_info_vector_[cloud_info_arr[empty_idx].point_info_vec_cot].azimuth_index = azimuth_index;
					cloud_info_arr[empty_idx].point_info_vector_[cloud_info_arr[empty_idx].point_info_vec_cot].stop_index = stop_index;
					++(cloud_info_arr[empty_idx].point_info_vec_cot);
					cloud_info_arr[empty_idx].point_not_exist_count_ = 0;
//					for(int i = GRP_SIZE -1 ; i >= 0 ; --i)
////					for(int i = 0 ; i < GRP_SIZE ; ++i)
//					{
//						if(cloud_info_arr[i].point_info_vec_cot == 0)
//						{
//							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].azimuth_index = azimuth_index;
//							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].stop_index = stop_index;
//							++cloud_info_arr[i].point_info_vec_cot;
//							cloud_info_arr[i].point_not_exist_count_ = 0;
//							break;
//						}
//					}
				}
			}
			last_azimuth = azimuth;
		}

		//top
		Top_Bottom_Side = 1;
		last_azimuth = block_data_bram_noise[0].azimuth_ * 1000;
		memset(cloud_info_arr, 0, GRP_SIZE * sizeof(cloud_info));
		for(unsigned int azimuth_index = NUM_AZI ; azimuth_index < NUM_AZI * 2 ; ++azimuth_index) // azimuth loop , 720=AzimuthCount/2
		{
			int azimuth = block_data_bram_noise[azimuth_index * StopCount + 0].azimuth_ * 1000;
			// 데이터 누락 예외처리
			if(abs(last_azimuth - azimuth) > 300)
			//if (fabs(last_azimuth - azimuth) > 1)
			{
				last_azimuth = azimuth;
				continue;
			}

			for(unsigned int stop_index = 0 ; stop_index < StopCount ; stop_index++)

			{
				double current_point_distance = block_data_bram_noise[azimuth_index * StopCount + stop_index].distance_[channel_index];

				if(current_point_distance == 0.0 && (azimuth_index == (NUM_AZI * 2 - 1) && stop_index == (StopCount - 1)) == 0)
				{
					continue;
				}

				// 각 군집 집단에 포인트가 추가되지 않는 연속 횟수 카운트 (군집화 완료 여부 체크 목적)
				if(stop_index == 0)
				{
//					for(int i = 0 ; i < GRP_SIZE ; ++i)
					for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
					{
						++cloud_info_arr[i].point_not_exist_count_;
						if(cloud_info_arr[i].point_not_exist_count_ > 1000)
						{
							cloud_info_arr[i].point_not_exist_count_ = 10;
						}
					}
				}

				// 군집화된 포인트 처리(V 노이즈 처리)
//				for(int i = 0 ; i < GRP_SIZE ; ++i)
				for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
				{
					// 특정 군집 공간에 포인트 존제 && 현재 포인트가 군집 포인트와 멀리 떨어져 있고 && 포인트가 들어오지 않은지 3회 초과 || 마지막 azimuth 포인트 일경우 최정적으로 처리
					if(cloud_info_arr[i].point_info_vec_cot > 0)
					{
						double pre_point_distance = 0.0;
						pre_point_distance =
								block_data_bram_noise[cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].azimuth_index
										* StopCount + cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].stop_index].distance_[channel_index];
						if((fabs(pre_point_distance - current_point_distance) >= 0.5
								&& cloud_info_arr[i].point_not_exist_count_ > target_point_not_exist)
								|| (azimuth_index == (NUM_AZI * 2 - 1) && stop_index == (StopCount - 1)))
						{
							// 군집된 포인트가 3개 이상일때만 처리
							if(cloud_info_arr[i].point_info_vec_cot > 3)
							{
								// V 노이즈 처리
//								RemoveVNoise(block_data_bram_noise, block_data_total_test, cloud_info_arr[i].point_info_vector_,
//										cloud_info_arr[i].point_info_vec_cot, channel_index, Top_Bottom_Side);

								vnoise_factor.azimuth_index_list_size = cloud_info_arr[i].point_info_vec_cot;
								vnoise_factor.channel_index = channel_index;
								vnoise_factor.istop = Top_Bottom_Side;
								RemoveVNoise_new(block_data_bram_noise, block_data_total_test, cloud_info_arr[i].point_info_vector_, &vnoise_factor);
//								xil_printf("test\r\n");
							}

							// 포인트 및 카운트 초기화
//							for(int cot_clear = 0; cot_clear < cloud_info_arr[i].point_info_vec_cot; cot_clear++)
//							{
//								cloud_info_arr[i].point_info_vector_[cot_clear].azimuth_index = 0;
//							 	cloud_info_arr[i].point_info_vector_[cot_clear].stop_index = 0;
//							}
							memset(cloud_info_arr[i].point_info_vector_, 0, 1152 * sizeof(PointInfo));
							cloud_info_arr[i].point_info_vec_cot = 0;
							cloud_info_arr[i].point_not_exist_count_ = 0;
						}
					}
				}

				unsigned int is_inserted = 0;
				// 현재 포인트가 군집화 될수 있는 집단이 있는지 체크
//				for(int i = 0 ; i < GRP_SIZE ; ++i)
				int empty_idx = -1;
				for(int i = GRP_SIZE - 1 ; i >= 0 ; --i)
				{
					if(cloud_info_arr[i].point_info_vec_cot > 0)
					{
						double pre_point_distance = 0.0;
						unsigned int temp_azi_idx = cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].azimuth_index;
						unsigned int temp_stop_idx = cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot - 1].stop_index;
						pre_point_distance = block_data_bram_noise[temp_azi_idx * StopCount + temp_stop_idx].distance_[channel_index];
						if(fabs(pre_point_distance - current_point_distance) < 0.5)
						{
							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].azimuth_index = azimuth_index;
							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].stop_index = stop_index;
							++cloud_info_arr[i].point_info_vec_cot;
							cloud_info_arr[i].point_not_exist_count_ = 0;
							is_inserted = 1;
							break;
						}
					}
					else
					{
						if(empty_idx == -1)
						{
							empty_idx = i;
						}
					}
				}

				// 어느 집단에 포함되지 않는다면, 새로운 집단 생성
				if(is_inserted == 0)
				{

					cloud_info_arr[empty_idx].point_info_vector_[cloud_info_arr[empty_idx].point_info_vec_cot].azimuth_index = azimuth_index;
					cloud_info_arr[empty_idx].point_info_vector_[cloud_info_arr[empty_idx].point_info_vec_cot].stop_index = stop_index;
					++(cloud_info_arr[empty_idx].point_info_vec_cot);
					cloud_info_arr[empty_idx].point_not_exist_count_ = 0;

//					for(int i = 0 ; i < GRP_SIZE ; ++i)
//					for(int i = GRP_SIZE-1 ; i >= 0; --i)
//					{
//						if(cloud_info_arr[i].point_info_vec_cot == 0)
//						{
//							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].azimuth_index = azimuth_index;
//							cloud_info_arr[i].point_info_vector_[cloud_info_arr[i].point_info_vec_cot].stop_index = stop_index;
//							++(cloud_info_arr[i].point_info_vec_cot);
//							cloud_info_arr[i].point_not_exist_count_ = 0;
//							break;
//						}
//					}
				}
			}
			last_azimuth = azimuth;
		}
	}
	return 0;
}



#endif
