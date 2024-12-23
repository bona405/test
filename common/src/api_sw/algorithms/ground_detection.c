/*
 * ground_detection.c
 *
 *  Created on: 2022. 6. 16.
 *      Author: ProDesk
 */

#include "./ground_detection.h"

float ground_detect_max_distance_ = 30;
float ground_detect_z_offset_ = 0.2;

const int neighborx_[24] = { -2, -1,  0,  1,  2,-2, -1,  0,  1,  2,-2,-1, 1, 2, -2,-1,0, 1, 2,-2,-1, 0, 1, 2 };
const int neighbory_[24] = { -2, -2, -2, -2, -2,-1, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2 };
unsigned int last_ground_seg_sensor_height_change_cnt = 0;
//void GetPosX(int &pos_x, int distance)
//
//pos_x = distance * cos(elevation * PI / 180) * cos((azimuth_offset)*PI / 180);
//			pos_y = distance * cos(elevation * PI / 180) * sin((azimuth_offset)*PI / 180);
//			pos_z = distance * sin(elevation * PI / 180) + z_axes_offset;

void GetPosZ(float* pos_z, float distance, float sin_vertical_angle)
{
	*pos_z = distance * sin_vertical_angle;
}

void InitGDParameter(GDParameter* gd_parameter)
{
	ground_point_indices_num = 0;
	gd_parameter->detection_start_azimuth_ = 60;
	gd_parameter->max_range_ = 50.0;
	gd_parameter->min_range_ = 1.0;
	gd_parameter->angle_max_1_ = 0.2;
	gd_parameter->angle_max_2_ = 1;
	gd_parameter->height_min_1_ = 0.2;
	gd_parameter->height_min_2_ = 0.5;
	gd_parameter->sensor_height_ = -999.0;
	gd_parameter->abnormal_distance_check_th_ = 10.0;
	gd_parameter->distance_diff_ = 2.0;
	gd_parameter->max_height_ = 2;
	gd_parameter->min_height_ = -1.0;
	gd_parameter->ground_point_limit_ = 30;
	gd_parameter->ground_z_devi_limit_ = 0.3;
	gd_parameter->ground_seg_sensor_height_ = -999.0;
	gd_parameter->ground_seg_sensor_height_change_cnt_ = 0;
	memset(ground_point_indices, 0x00, MAX_GROUND_POINT_NUM * sizeof(TupleII));

	float angle = 10.0f;
	int num_of_channel = 32;
	float top_bottom_offset = angle / num_of_channel;
	float angle_start = -angle / 2 + top_bottom_offset / 2;
	for (int i = 0; i < num_of_channel / 2; i++)
	{
		vertical_angle_arr_bottom[i] = angle_start + top_bottom_offset * 2 * i;
	}

	angle_start = -angle / 2 + top_bottom_offset / 2 + top_bottom_offset;

	for (int i = 0; i < num_of_channel / 2; i++)
	{
		vertical_angle_arr_top[i] = angle_start + top_bottom_offset * 2 * i;
	}

//	angle = 10.0f;
//	num_of_channel = 32;
//	top_bottom_offset = angle / num_of_channel;
//	angle_start = -angle / 2 + top_bottom_offset / 2;
//
//	for (size_t i = 0; i < num_of_channel; i++)
//	{
//		vertical_angle_arr[i] = angle_start + top_bottom_offset * i;
//	}
}

void InitGDResult(GDResult* gd_result)
{
//	gd_result->ground_z_value_ = -999.0;
	gd_result->ground_point_cnt_ = 0;
	gd_result->is_ground_detected_ = 0;
	gd_result->gounrd_z_mean_ = -999.0;
	gd_result->ground_z_deviation_ = 0.0;
}

void SetIntensity(RawDataBlock* current_frame_data, RawDataBlock* intensity_data)
{
	for(int i = 0 ; i < 4320 ; i++)
	{
		for(int j = 0 ; j < 16 ; j++)
		{
			intensity_data[i].distance_[j].distance = 0;
		}
	}

	for(int ground_point_index = 0; ground_point_index < ground_point_indices_num ; ground_point_index++)
	{
		int i = ground_point_indices[ground_point_index].first;
		int j = ground_point_indices[ground_point_index].second;
		intensity_data[i].distance_[j].distance = 200;
	}

}

void DetectGround(ConvertedDataBlock* current_frame_data, uint8_t is_bottom_first, u32 fov_data_block_count)
{
	u32 total_azimuth_count = fov_data_block_count;
	memset(region_minz_, 100.0, sizeof(region_minz_));
	memset(cloud_index_, -1, sizeof(cloud_index_));
	for(int i = 0; i<IMAGE_HEIGHT ; i++)
	{
		memset(region_[i], 0x00, IMAGE_WIDTH);
		memset(range_image[i], 0x00, IMAGE_WIDTH * sizeof(RGB));

	}
	ground_point_indices_num = 0;
//	ground_point_indices_num = 0;
	gd_parameter_.sensor_height_ = -999.0;
	int bottom_start_ind = 0;
	int bottom_end_ind = total_azimuth_count;
	if(is_bottom_first == 0)
	{
		bottom_start_ind = total_azimuth_count;
		bottom_end_ind = fov_data_block_count;
	}
	//sensor height
	if (gd_parameter_.ground_seg_sensor_height_ == -999.0 || last_ground_seg_sensor_height_change_cnt != gd_parameter_.ground_seg_sensor_height_change_cnt_)
	{
//		float sensor_height_vec[fov_data_block_count] = {-999.0, };
		float *sensor_height_vec;
		sensor_height_vec = (float*)malloc(sizeof(float)*fov_data_block_count);
		int sensor_height_vec_ind = 0;

		float* sin_vert_angle_arr;


		sin_vert_angle_arr = sin_vert_angle_map_bottom;
//		else
//			vertical_angle_arr_p = vertical_angle_arr_top;

		for(int i = bottom_start_ind ; i < bottom_end_ind ; i+=GD_INDEX_OFFSET)
		{
//			if(i == TOTAL_AZIMUTH_COUNT)
//			{
//				if(is_bottom_first == 1)
//					vertical_angle_arr_p = vertical_angle_arr_top;
//				else
//					vertical_angle_arr_p = vertical_angle_arr_bottom;
//			}

			if(current_frame_data[i].distance_[0] < 250.0)
			{
				float sensor_height_z;
				GetPosZ(&sensor_height_z, current_frame_data[i].distance_[0], sin_vert_angle_arr[0]);
				if(sensor_height_z != 0)
				{
					if(sensor_height_vec_ind < fov_data_block_count)
					{
						sensor_height_vec[sensor_height_vec_ind] = sensor_height_z;
		//				GetPosZ(&sensor_height_vec[sensor_height_vec_ind], current_frame_data[i].distance_[0] / 256.0, vertical_angle_arr_p[i]);
						sensor_height_vec_ind++;

						if(gd_parameter_.sensor_height_ < sensor_height_z)
							gd_parameter_.sensor_height_ = sensor_height_z;
					}
				}
			}
		}


//		float median_sensor_height_sum = 0;
//		int median_sensor_height_count = 0;
//
//		//for (int i = (int)(sensor_height_vec.size() * 0.25); i < (int)(sensor_height_vec.size() * 0.75); ++i)
//		for (int i = (int)(sensor_height_vec_ind * 0.95); i < (int)(sensor_height_vec_ind * 1.0); ++i)
//		{
//			median_sensor_height_sum += sensor_height_vec[i];
//			++median_sensor_height_count;
//		}
//		gd_parameter_.sensor_height_ = median_sensor_height_sum / median_sensor_height_count;


		if (last_ground_seg_sensor_height_change_cnt != gd_parameter_.ground_seg_sensor_height_change_cnt_)
		{
			gd_parameter_.ground_seg_sensor_height_ = gd_parameter_.sensor_height_;
		}

		free(sensor_height_vec);
	}
	else
	{
		gd_parameter_.sensor_height_ = gd_parameter_.ground_seg_sensor_height_;
	}

	float* sin_vert_angle_arr;
//	if(is_bottom_first == 1)
	sin_vert_angle_arr = sin_vert_angle_map_bottom;
//	else
//		vertical_angle_arr_p = vertical_angle_arr_top;

	for (int azimuth_index = bottom_start_ind; azimuth_index < bottom_end_ind; azimuth_index+=GD_INDEX_OFFSET)
	{
//		if(azimuth_index == TOTAL_AZIMUTH_COUNT)
//		{
//			if(is_bottom_first == 1)
//				vertical_angle_arr_p = vertical_angle_arr_top;
//			else
//				vertical_angle_arr_p = vertical_angle_arr_bottom;
//		}

		if(azimuth_index < gd_parameter_.detection_start_azimuth_ || azimuth_index >= bottom_end_ind - gd_parameter_.detection_start_azimuth_)
			continue;


		int last_threshold_point_index = -1;
		int last_valid_channel_Index = -1;
		uint8_t is_last_channel_ground = 0;
		uint8_t is_current_channel_ground = 0;
		float pre_height = gd_parameter_.sensor_height_;
		float pre_distance = 0;
		int lost_point_count = 0;
		int8_t is_lost_point_exist = 0;
		int8_t start_ground_on = 1;

//		if(ground_point_indices_num > 19000)
//			break;

		for (int vertical_index = 0; vertical_index < 8; vertical_index++)
		{
			float current_distance = current_frame_data[azimuth_index].distance_[vertical_index];
			if(current_distance < gd_parameter_.min_range_ || current_distance > gd_parameter_.max_range_)
			{
//				++lost_point_count;
//				is_lost_point_exist = 1;
				continue;
			}



			float cur_distance = current_distance;
			float cur_height;
			GetPosZ(&cur_height, cur_distance, sin_vert_angle_arr[vertical_index]);

			if(cur_height > gd_parameter_.max_height_)
			{
				++lost_point_count;
				is_lost_point_exist = 1;
				continue;
			}

			if (start_ground_on == 1)
			{
				if (is_lost_point_exist == 0)
				{
					float current_angle = fabsf(cur_height - pre_height) / fabsf(cur_distance - pre_distance);

					// First Case
					if (current_angle <= gd_parameter_.angle_max_1_)
					{
						// is ground point
						if(ground_point_indices_num < MAX_GROUND_POINT_NUM)
						{
						ground_point_indices[ground_point_indices_num].first = azimuth_index;
						ground_point_indices[ground_point_indices_num].second = vertical_index;
						ground_point_indices_num++;
						}
						is_current_channel_ground = 1;
					}
					// not required???
					else if (current_angle <= gd_parameter_.angle_max_2_ && fabsf(cur_height - pre_height) <= gd_parameter_.height_min_1_)
					{
						// is ground point
						if(ground_point_indices_num < MAX_GROUND_POINT_NUM)
												{
						ground_point_indices[ground_point_indices_num].first = azimuth_index;
						ground_point_indices[ground_point_indices_num].second = vertical_index;
						ground_point_indices_num++;
												}
						is_current_channel_ground = 1;
					}
					else
					{
						uint8_t is_live_dramatically = 0;
						// KHG case 2
						// 낮은 채널에서 두 채널간 거리가 극히 짧음 (Autol 라이다 결점 / KHG case 5 유사 원인)
						if (cur_distance < gd_parameter_.abnormal_distance_check_th_)
						{
							float pre_height_temp = gd_parameter_.sensor_height_;
							float pre_distance_temp = 0;
							float next_height_temp = 0;
							float next_distance_temp = 0;

							for (int iter = vertical_index - 2; iter >= 0; --iter)
							{
								pre_distance_temp = current_frame_data[azimuth_index].distance_[iter];
								if (pre_distance_temp != 0)
								{
									GetPosZ(&pre_height_temp, pre_distance_temp, sin_vert_angle_arr[iter]);
									break;
								}
							}
							float angle_pre = fabsf(cur_height - pre_height_temp) / fabsf(cur_distance - pre_distance_temp);

							float angle_next = 100;
							for (int iter = vertical_index + 1; iter < 16; ++iter)
							{
								next_distance_temp = current_frame_data[azimuth_index].distance_[iter];
								if (next_distance_temp != 0)
								{
									GetPosZ(&next_height_temp, next_distance_temp, sin_vert_angle_arr[iter]);
									angle_next = fabsf(cur_height - next_height_temp) / fabsf(cur_distance - next_distance_temp);
									break;
								}
							}

							if (((pre_distance_temp != 0 && fabsf(cur_distance - pre_distance_temp) < gd_parameter_.distance_diff_) || pre_distance_temp == 0)
									&& fabsf(cur_distance - next_distance_temp) < gd_parameter_.distance_diff_ && angle_pre <= gd_parameter_.angle_max_1_ && angle_next <= gd_parameter_.angle_max_1_)
							{
								if(ground_point_indices_num < MAX_GROUND_POINT_NUM)
														{
								ground_point_indices[ground_point_indices_num].first = azimuth_index;
								ground_point_indices[ground_point_indices_num].second = vertical_index;
								ground_point_indices_num++;
														}
								is_current_channel_ground = 1;
								is_live_dramatically = 1;
							}
						}
						if (is_live_dramatically == 0)
						{
//							if (current_angle > gd_parameter_.angle_max_2_)
//							{
//								// KHG case 3
//								if(last_valid_channel_Index >= 0)
//									lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[last_valid_channel_Index].is_ground_obj_detect_ = false;
//							}
							last_threshold_point_index = last_valid_channel_Index;
							start_ground_on = 0;
						}
					}
				}
				else
				{
					float height_between_lost_point = 0;
					// KHG case 4
					// 시작 채널에서 누락 포인트 존재 할 경우 (Autol 라이다 결점_ Stop1에서 최초에 누락 포인트 있으면 안됨)
					if (lost_point_count == vertical_index)
						height_between_lost_point = gd_parameter_.height_min_2_;
					else
						height_between_lost_point = gd_parameter_.height_min_1_;

					// Second Case
					if (fabsf(cur_height - pre_height) <= height_between_lost_point)
					{
						if(ground_point_indices_num < MAX_GROUND_POINT_NUM)
												{
						ground_point_indices[ground_point_indices_num].first = azimuth_index;
						ground_point_indices[ground_point_indices_num].second = vertical_index;
						ground_point_indices_num++;
												}
						is_current_channel_ground = 1;
					}
					else
					{
//						if (last_valid_channel_Index >= 0 && lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[last_valid_channel_Index].is_ground_obj_detect_ == true)
						if (last_valid_channel_Index >= 0 && is_last_channel_ground == 1)
							last_threshold_point_index = last_valid_channel_Index;
						start_ground_on = 0;
					}
				}

				// KHG case 5
				// 낮은 채널에서 거리 역전 현상 발생 (Autol 라이다 결점)
				if (cur_distance > gd_parameter_.abnormal_distance_check_th_)
				{
					// third Case
					if (pre_distance > cur_distance)
					{
//							lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[channel_index].is_ground_obj_detect_ = false;

//							if (last_valid_channel_Index >= 0 && lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[last_valid_channel_Index].is_ground_obj_detect_ == true)
						if (last_valid_channel_Index >= 0 && is_last_channel_ground == 1)
							last_threshold_point_index = last_valid_channel_Index;
						start_ground_on = 0;
					}
				}
			}
			else
			{
				// Fourth Case
				if (cur_height < pre_height)
				{
					float last_threshold_point_height = 0;

					if (last_threshold_point_index == -1)
					{
						last_threshold_point_height = gd_parameter_.sensor_height_;
					}
					else
					{
//						last_threshold_point_height = lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[last_threshold_point_index].point_[2];
						GetPosZ(&last_threshold_point_height, current_frame_data[azimuth_index].distance_[last_threshold_point_index], sin_vert_angle_arr[last_threshold_point_index]);
					}

					if (fabsf(cur_height - last_threshold_point_height) <= gd_parameter_.height_min_1_)
					{
//						start_ground_on == 1;
						start_ground_on = 1;
//						lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[channel_index].is_ground_obj_detect_ = true;
						if(ground_point_indices_num < MAX_GROUND_POINT_NUM)
												{
						ground_point_indices[ground_point_indices_num].first = azimuth_index;
						ground_point_indices[ground_point_indices_num].second = vertical_index;
						ground_point_indices_num++;
												}
						is_current_channel_ground = 1;
					}
				}
			}
			pre_height = cur_height;
			pre_distance = cur_distance;
			is_lost_point_exist = 0;
			last_valid_channel_Index = vertical_index;
			is_last_channel_ground = is_current_channel_ground;
			is_current_channel_ground = 0;
		}
	}
	last_ground_seg_sensor_height_change_cnt = gd_parameter_.ground_seg_sensor_height_change_cnt_;
}

//void DetectGround(raw_data_block* current_frame_data, uint8_t is_bottom_first)
//{
//	memset(region_minz_, 100.0, sizeof(region_minz_));
//	memset(cloud_index_, -1, sizeof(cloud_index_));
//	for(int i = 0; i<IMAGE_HEIGHT ; i++)
//	{
//		memset(region_[i], 0x00, IMAGE_WIDTH);
//		memset(range_image[i], 0x00, IMAGE_WIDTH * sizeof(RGB));
//
//	}
//	ground_point_indices_num = 0;
////	ground_point_indices_num = 0;
//	gd_parameter_.sensor_height_ = -999.0;
//	int bottom_start_ind = 0;
//	int bottom_end_ind = TOTAL_AZIMUTH_COUNT;
//	if(is_bottom_first == 0)
//	{
//		bottom_start_ind = TOTAL_AZIMUTH_COUNT;
//		bottom_end_ind = FRAME_DATA_COUNT;
//	}
//	//sensor height
//	if (gd_parameter_.ground_seg_sensor_height_ == -999.0 || last_ground_seg_sensor_height_change_cnt != gd_parameter_.ground_seg_sensor_height_change_cnt_)
//	{
//		float sensor_height_vec[FRAME_DATA_COUNT] = {-999.0, };
//		int sensor_height_vec_ind = 0;
//
//		float* vertical_angle_arr_p;
//
//
//			vertical_angle_arr_p = vertical_angle_arr_bottom;
////		else
////			vertical_angle_arr_p = vertical_angle_arr_top;
//
//		for(int i = bottom_start_ind ; i < bottom_end_ind ; i+=15)
//		{
////			if(i == TOTAL_AZIMUTH_COUNT)
////			{
////				if(is_bottom_first == 1)
////					vertical_angle_arr_p = vertical_angle_arr_top;
////				else
////					vertical_angle_arr_p = vertical_angle_arr_bottom;
////			}
//
//			if(current_frame_data[i].distance_[0] != 0xffff)
//			{
//				float sensor_height_z;
//				GetPosZ(&sensor_height_z, current_frame_data[i].distance_[0] / 256.0, vertical_angle_arr_p[0]);
//				if(sensor_height_z != 0)
//				{
//					sensor_height_vec[sensor_height_vec_ind] = sensor_height_z;
//	//				GetPosZ(&sensor_height_vec[sensor_height_vec_ind], current_frame_data[i].distance_[0] / 256.0, vertical_angle_arr_p[i]);
//					sensor_height_vec_ind++;
//					if(gd_parameter_.sensor_height_ < sensor_height_z)
//						gd_parameter_.sensor_height_ = sensor_height_z;
//				}
//			}
//		}
//
//
////		float median_sensor_height_sum = 0;
////		int median_sensor_height_count = 0;
////
////		//for (int i = (int)(sensor_height_vec.size() * 0.25); i < (int)(sensor_height_vec.size() * 0.75); ++i)
////		for (int i = (int)(sensor_height_vec_ind * 0.95); i < (int)(sensor_height_vec_ind * 1.0); ++i)
////		{
////			median_sensor_height_sum += sensor_height_vec[i];
////			++median_sensor_height_count;
////		}
////		gd_parameter_.sensor_height_ = median_sensor_height_sum / median_sensor_height_count;
//
//
//		if (last_ground_seg_sensor_height_change_cnt != gd_parameter_.ground_seg_sensor_height_change_cnt_)
//		{
//			gd_parameter_.ground_seg_sensor_height_ = gd_parameter_.sensor_height_;
//		}
//	}
//	else
//	{
//		gd_parameter_.sensor_height_ = gd_parameter_.ground_seg_sensor_height_;
//	}
//
//	float* vertical_angle_arr_p;
////	if(is_bottom_first == 1)
//		vertical_angle_arr_p = vertical_angle_arr_bottom;
////	else
////		vertical_angle_arr_p = vertical_angle_arr_top;
//
//	for (int azimuth_index = bottom_start_ind; azimuth_index < bottom_end_ind; azimuth_index+=15)
//	{
////		if(azimuth_index == TOTAL_AZIMUTH_COUNT)
////		{
////			if(is_bottom_first == 1)
////				vertical_angle_arr_p = vertical_angle_arr_top;
////			else
////				vertical_angle_arr_p = vertical_angle_arr_bottom;
////		}
//
//		if(azimuth_index < gd_parameter_.detection_start_azimuth_ || azimuth_index >= FRAME_DATA_COUNT - gd_parameter_.detection_start_azimuth_)
//			continue;
//
//
//		int last_threshold_point_index = -1;
//		int last_valid_channel_Index = -1;
//		uint8_t is_last_channel_ground = 0;
//		uint8_t is_current_channel_ground = 0;
//		float pre_height = 0;
//		float pre_distance = 0;
//		int lost_point_count = 0;
//		int8_t is_lost_point_exist = 0;
//		int8_t start_ground_on = 1;
//
////		if(ground_point_indices_num > 19000)
////			break;
//
//		for (int vertical_index = 0; vertical_index < 8; vertical_index++)
//		{
//			float current_distance = current_frame_data[azimuth_index].distance_[vertical_index] / 256.0;
//			if(current_distance < gd_parameter_.min_range_ || current_distance > gd_parameter_.max_range_)
//			{
////				++lost_point_count;
////				is_lost_point_exist = 1;
//				continue;
//			}
//
//
//
//			float cur_distance = current_distance;
//			float cur_height;
//			GetPosZ(&cur_height, cur_distance, vertical_angle_arr_p[vertical_index]);
//
//			if(cur_height > gd_parameter_.max_height_)
//			{
//				++lost_point_count;
//				is_lost_point_exist = 1;
//				continue;
//			}
//
//			if (start_ground_on == 1)
//			{
//				if (is_lost_point_exist == 0)
//				{
//					float current_angle = fabsf(cur_height - pre_height) / fabsf(cur_distance - pre_distance);
//
//					// First Case
//					if (current_angle <= gd_parameter_.angle_max_1_)
//					{
//						// is ground point
//						ground_point_indices[ground_point_indices_num].first = azimuth_index;
//						ground_point_indices[ground_point_indices_num].second = vertical_index;
//						ground_point_indices_num++;
//						is_current_channel_ground = 1;
//					}
//					// not required???
//					else if (current_angle <= gd_parameter_.angle_max_2_ && fabsf(cur_height - pre_height) <= gd_parameter_.height_min_1_)
//					{
//						// is ground point
//						ground_point_indices[ground_point_indices_num].first = azimuth_index;
//						ground_point_indices[ground_point_indices_num].second = vertical_index;
//						ground_point_indices_num++;
//						is_current_channel_ground = 1;
//					}
//					else
//					{
//						uint8_t is_live_dramatically = 0;
//						// KHG case 2
//						// 낮은 채널에서 두 채널간 거리가 극히 짧음 (Autol 라이다 결점 / KHG case 5 유사 원인)
//						if (cur_distance < gd_parameter_.abnormal_distance_check_th_)
//						{
//							float pre_height_temp = gd_parameter_.sensor_height_;
//							float pre_distance_temp = 0;
//							float next_height_temp = 0;
//							float next_distance_temp = 0;
//
//							for (int iter = vertical_index - 2; iter >= 0; --iter)
//							{
//								pre_distance_temp = current_frame_data[azimuth_index].distance_[iter] / 256.0;
//								if (pre_distance_temp != 0)
//								{
//									GetPosZ(&pre_height_temp, pre_distance_temp, vertical_angle_arr_p[iter]);
//									break;
//								}
//							}
//							float angle_pre = fabsf(cur_height - pre_height_temp) / fabsf(cur_distance - pre_distance_temp);
//
//							float angle_next = 100;
//							for (int iter = vertical_index + 1; iter < 16; ++iter)
//							{
//								next_distance_temp = current_frame_data[azimuth_index].distance_[iter] / 256.0;
//								if (next_distance_temp != 0)
//								{
//									GetPosZ(&next_height_temp, next_distance_temp, vertical_angle_arr_p[iter]);
//									angle_next = fabsf(cur_height - next_height_temp) / fabsf(cur_distance - next_distance_temp);
//									break;
//								}
//							}
//
//							if (((pre_distance_temp != 0 && fabsf(cur_distance - pre_distance_temp) < gd_parameter_.distance_diff_) || pre_distance_temp == 0)
//									&& fabsf(cur_distance - next_distance_temp) < gd_parameter_.distance_diff_ && angle_pre <= gd_parameter_.angle_max_1_ && angle_next <= gd_parameter_.angle_max_1_)
//							{
//								ground_point_indices[ground_point_indices_num].first = azimuth_index;
//								ground_point_indices[ground_point_indices_num].second = vertical_index;
//								ground_point_indices_num++;
//								is_current_channel_ground = 1;
//								is_live_dramatically = 1;
//							}
//						}
//						if (is_live_dramatically == 0)
//						{
////							if (current_angle > gd_parameter_.angle_max_2_)
////							{
////								// KHG case 3
////								if(last_valid_channel_Index >= 0)
////									lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[last_valid_channel_Index].is_ground_obj_detect_ = false;
////							}
//							last_threshold_point_index = last_valid_channel_Index;
//							start_ground_on = 0;
//						}
//					}
//				}
//				else
//				{
//					float height_between_lost_point = 0;
//					// KHG case 4
//					// 시작 채널에서 누락 포인트 존재 할 경우 (Autol 라이다 결점_ Stop1에서 최초에 누락 포인트 있으면 안됨)
//					if (lost_point_count == vertical_index)
//						height_between_lost_point = gd_parameter_.height_min_2_;
//					else
//						height_between_lost_point = gd_parameter_.height_min_1_;
//
//					// Second Case
//					if (fabsf(cur_height - pre_height) <= height_between_lost_point)
//					{
//						ground_point_indices[ground_point_indices_num].first = azimuth_index;
//						ground_point_indices[ground_point_indices_num].second = vertical_index;
//						ground_point_indices_num++;
//						is_current_channel_ground = 1;
//					}
//					else
//					{
////						if (last_valid_channel_Index >= 0 && lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[last_valid_channel_Index].is_ground_obj_detect_ == true)
//						if (last_valid_channel_Index >= 0 && is_last_channel_ground == 1)
//							last_threshold_point_index = last_valid_channel_Index;
//						start_ground_on = 0;
//					}
//				}
//
//				// KHG case 5
//				// 낮은 채널에서 거리 역전 현상 발생 (Autol 라이다 결점)
//				if (cur_distance > gd_parameter_.abnormal_distance_check_th_)
//				{
//					// third Case
//					if (pre_distance > cur_distance)
//					{
////							lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[channel_index].is_ground_obj_detect_ = false;
//
////							if (last_valid_channel_Index >= 0 && lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[last_valid_channel_Index].is_ground_obj_detect_ == true)
//						if (last_valid_channel_Index >= 0 && is_last_channel_ground == 1)
//							last_threshold_point_index = last_valid_channel_Index;
//						start_ground_on = 0;
//					}
//				}
//			}
//			else
//			{
//				// Fourth Case
//				if (cur_height < pre_height)
//				{
//					float last_threshold_point_height = 0;
//
//					if (last_threshold_point_index == -1)
//					{
//						last_threshold_point_height = gd_parameter_.sensor_height_;
//					}
//					else
//					{
////						last_threshold_point_height = lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[last_threshold_point_index].point_[2];
//						GetPosZ(&last_threshold_point_height, current_frame_data[azimuth_index].distance_[last_threshold_point_index] / 256.0, vertical_angle_arr_p[last_threshold_point_index]);
//					}
//
//					if (fabsf(cur_height - last_threshold_point_height) <= gd_parameter_.height_min_1_)
//					{
////						start_ground_on == 1;
//						start_ground_on = 1;
////						lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[channel_index].is_ground_obj_detect_ = true;
//						ground_point_indices[ground_point_indices_num].first = azimuth_index;
//						ground_point_indices[ground_point_indices_num].second = vertical_index;
//						ground_point_indices_num++;
//						is_current_channel_ground = 1;
//					}
//				}
//			}
//			pre_height = cur_height;
//			pre_distance = cur_distance;
//			is_lost_point_exist = 0;
//			last_valid_channel_Index = vertical_index;
//			is_last_channel_ground = is_current_channel_ground;
//			is_current_channel_ground = 0;
//		}
//	}
//	last_ground_seg_sensor_height_change_cnt = gd_parameter_.ground_seg_sensor_height_change_cnt_;
//}

void DownSampling(RawDataBlock* current_frame_data, uint8_t is_bottom_first, u32 fov_data_block_count)
{
	memset(region_minz_, 100, sizeof(region_minz_));
	memset(cloud_index_, -1, sizeof(cloud_index_));
	for(int i = 0; i<IMAGE_HEIGHT ; i++)
	{
		memset(region_[i], 0x00, IMAGE_WIDTH);
		memset(range_image[i], 0x00, IMAGE_WIDTH * sizeof(RGB));

	}
	ground_point_indices_num = 0;
	if(is_bottom_first)
	{
		u32 total_azimuth_count = fov_data_block_count / 2;
		for(int i = 0 ; i < total_azimuth_count ; i+=GD_INDEX_OFFSET)
		{
			for(int j = 0 ; j < 6 ; ++j)
			{
				if(current_frame_data[i].distance_[j].distance < 10240 && current_frame_data[i].distance_[j].distance > 1280) // 40 * 256 = 10240, 5 * 256 = 1280
				{
					if(ground_point_indices_num < MAX_GROUND_POINT_NUM)
											{
					ground_point_indices[ground_point_indices_num].first = i;
					ground_point_indices[ground_point_indices_num].second = j;
					ground_point_indices_num++;
											}
				}

			}
		}
	}
	else
	{
		u32 total_azimuth_count = fov_data_block_count / 2;
		for(int i = total_azimuth_count ; i < fov_data_block_count ; i+=GD_INDEX_OFFSET)
		{
			for(int j = 0 ; j < 6 ; ++j)
			{
				if(current_frame_data[i].distance_[j].distance < 10240 && current_frame_data[i].distance_[j].distance > 1280) // 40 * 256 = 10240, 5 * 256 = 1280
				{
					if(ground_point_indices_num < MAX_GROUND_POINT_NUM)
											{
					ground_point_indices[ground_point_indices_num].first = i;
					ground_point_indices[ground_point_indices_num].second = j;
					ground_point_indices_num++;
											}
				}

			}
		}
	}
}
//
//void DetectGround(rx_data_block* current_frame_blocks)
//{
//
//	//sensor height
//
//	for (int azimuth_index = 0; azimuth_index < TOTAL_AZIMUTH_COUNT; azimuth_index++)
//	{
//		if(azimuth_index < gd_parameter_.detection_start_azimuth_ || azimuth_index >= TOTAL_AZIMUTH_COUNT - gd_parameter_.detection_start_azimuth_)
//			continue;
//
//		int last_threshold_point_index = -1;
//		int last_valid_channel_Index = -1;
//		float pre_height = gd_parameter_.sensor_height_;
//		float pre_distance = 0;
//		int lost_point_count = 0;
//		int8_t is_lost_point_exist = 0;
//		int8_t start_ground_on = 1;
//
//		for (int vertical_index = 0; vertical_index < CHANNEL_COUNT; vertical_index++)
//		{
//			if(current_frame_blocks[azimuth_index].distance_[vertical_index] < gd_parameter_.min_range_ || current_frame_blocks[azimuth_index].distance_[vertical_index] > gd_parameter_.max_range_)
//				continue;
//
//			float cur_distance = current_frame_blocks[azimuth_index].distance_[vertical_index];
//			float cur_height;
//			GetPosZ(&cur_height, cur_distance, vertical_angle_arr[vertical_index]);
//
//			if (start_ground_on == 1)
//			{
//				if (is_lost_point_exist == 1)
//				{
//					float current_angle = abs(cur_height - pre_height) / abs(cur_distance - pre_distance);
//
//					if (current_angle <= gd_parameter_.angle_max_1_)
//					{
//						// is ground point
//						ground_point_indices[ground_point_indices_num].first = azimuth_index;
//						ground_point_indices[ground_point_indices_num].second = vertical_index;
//						ground_point_indices_num++;
//					}
//					// not required???
//					else if (current_angle <= gd_parameter_.angle_max_2_ && abs(cur_height - pre_height) <= gd_parameter_.height_min_1_)
//					{
//						// is ground point
//						ground_point_indices[ground_point_indices_num].first = azimuth_index;
//						ground_point_indices[ground_point_indices_num].second = vertical_index;
//						ground_point_indices_num++;
//					}
//					else
//					{
//						uint8_t is_live_dramatically = 0;
//						if (cur_distance < gd_parameter_.abnormal_distance_check_th_)
//						{
//							float pre_height_temp = gd_parameter_.sensor_height_;
//							float pre_distance_temp = 0;
//							float next_height_temp = 0;
//							float next_distance_temp = 0;
//
//							for (int iter = vertical_index - 2; iter >= 0; --iter)
//							{
//								pre_distance_temp = current_frame_blocks[azimuth_index].distance_[iter];
//								if (pre_distance_temp != 0)
//								{
//									pre_height_temp = lidar_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth__[azimuth_index].channel_[iter].point_[2];
//									break;
//								}
//							}
//						}
//					}
//				}
//				else
//				{
//
//				}
//			}
//			else
//			{
//
//			}
//		}
//	}
//}

void InitIFParameter(IFParameter* if_parameter)
{
	if_parameter->width_ = 1152;
	if_parameter->height_= 16;
	if_parameter->max_range_ = 70.0;
	if_parameter->min_range_ = 2.0;
	if_parameter->th_g_ = 0.2;
	if_parameter->sigma_= 7.0;
	if_parameter->deltaR_= 2.0;
	if_parameter->length_= (int)((if_parameter->max_range_ - if_parameter->min_range_) / if_parameter->deltaR_);

	memset(region_minz_, 100, sizeof(region_minz_));
	memset(cloud_index_, -1, sizeof(cloud_index_));
	cloud_num = 0;
}

void InitTrigFunctionMap(ConvertedDataBlock* current_frame_data, uint8_t is_bottom_first, u32 fov_data_block_count)
{
	u32 total_azimuth_count = fov_data_block_count / 2;
	int bottom_start_ind = 0;
	int bottom_end_ind = total_azimuth_count;
	if(is_bottom_first == 0)
	{
		bottom_start_ind = total_azimuth_count;
		bottom_end_ind = fov_data_block_count;
	}
	for(int i = 0 ; i < fov_data_block_count ; ++i)
	{
		sin_horz_angle_map[i] = sinf((float)current_frame_data[bottom_start_ind].azimuth_ * 0.017453288);
		cos_horz_angle_map[i] = cosf((float)current_frame_data[bottom_start_ind].azimuth_ * 0.017453288);
		bottom_start_ind++;
	}

	for(int i = 0 ; i < CHANNEL_COUNT / 2 ; ++i)
	{
		cos_vert_angle_map_bottom[i] = cosf((float)vertical_angle_arr_bottom[i] * 0.017453288);
		sin_vert_angle_map_bottom[i] = sinf((float)vertical_angle_arr_bottom[i] * 0.017453288);

		cos_vert_angle_map_top[i] = cosf((float)vertical_angle_arr_top[i] * 0.017453288);
		sin_vert_angle_map_top[i] = sinf((float)vertical_angle_arr_top[i] * 0.017453288);
	}
}

void DistanceToXYZ(float distance, float vertical_angle, float azimuth, float* pos_x, float* pos_y, float* pos_z)
{
	*pos_x = distance * cosf(vertical_angle * 0.017453288) * sinf(azimuth*0.017453288);
	*pos_y = distance * cosf(vertical_angle * 0.017453288) * cosf(azimuth*0.017453288);
	*pos_z = distance * sinf(vertical_angle * 0.017453288);
}

void DistanceToXYZCoord(float distance, int azimuth_index, int vertical_index, float* pos_x, float* pos_y, float* pos_z)
{
	*pos_x = distance * cos_vert_angle_map_bottom[vertical_index] * cos_horz_angle_map[azimuth_index];
	*pos_y = distance * cos_vert_angle_map_bottom[vertical_index] * sin_horz_angle_map[azimuth_index];
	*pos_z = distance * sin_vert_angle_map_bottom[vertical_index];

//	*pos_x = distance * cos_vert_angle_map_bottom[vertical_index] * sin_horz_angle_map[azimuth_index];
//	*pos_y = distance * cos_vert_angle_map_bottom[vertical_index] * cos_horz_angle_map[azimuth_index];
//	*pos_z = distance * sin_vert_angle_map_bottom[vertical_index];
}

void ConvertDataToXYZPoints(ConvertedDataBlock* current_frame_data, uint8_t is_bottom_first, uint8_t (*remove_noise_intensity)[CHANNEL_SIZE])
{
	cloud_num=0;
//	float* vertical_angle_arr_p;
//	if(is_bottom_first == 1)
//		vertical_angle_arr_p = vertical_angle_arr_bottom;
//	else
//		vertical_angle_arr_p = vertical_angle_arr_top;

	int azimuth_index = 0;
	int vertical_index = 0;
	for(int ground_point_index = 0; ground_point_index < ground_point_indices_num ; ground_point_index++)
	{
		if(cloud_num < MAX_GROUND_POINT_NUM)
		{
//		int channel_number = 0;
		azimuth_index = ground_point_indices[ground_point_index].first;
		vertical_index = ground_point_indices[ground_point_index].second;

//		remove_noise_intensity[azimuth_index][vertical_index] = 255;
//		if(azimuth_index == TOTAL_AZIMUTH_COUNT)
//		{
//			if(is_bottom_first == 1)
//				vertical_angle_arr_p = vertical_angle_arr_top;
//			else
//				vertical_angle_arr_p = vertical_angle_arr_bottom;
//		}
//
//		if(azimuth_index >= TOTAL_AZIMUTH_COUNT)
//		{
//			if(is_bottom_first == 1)
//				channel_number = vertical_index * 2 + 1;
//			else
//				channel_number = vertical_index * 2;
//		}
//		else
//		{
//			if(is_bottom_first == 1)
//				channel_number = vertical_index * 2;
//			else
//				channel_number = vertical_index * 2 + 1;
//		}

//		float x, y, z;
//		DistanceToXYZ((float)current_frame_data[azimuth_index].distance_[vertical_index] / 256.0, vertical_angle_arr_p[vertical_index], (float)current_frame_data[azimuth_index].azimuth_ / 1000.0, &x, &y, &z);
//		DistanceToXYZ((float)current_frame_data[azimuth_index].distance_[vertical_index] * 0.00390625, vertical_angle_arr_p[vertical_index], (float)current_frame_data[azimuth_index].azimuth_ * 0.001, &cloud_[cloud_num].y, &cloud_[cloud_num].x, &cloud_[cloud_num].z);
		DistanceToXYZCoord((float)current_frame_data[azimuth_index].distance_[vertical_index], azimuth_index, vertical_index, &cloud_[cloud_num].y, &cloud_[cloud_num].x, &cloud_[cloud_num].z);
//		cloud_[cloud_num].x = y;
//		cloud_[cloud_num].y = x;
//		cloud_[cloud_num].z = z;
		cloud_[cloud_num].r = vertical_index;
		cloud_[cloud_num].a = current_frame_data[azimuth_index].azimuth_;
		cloud_num++;
		}
	}
}

void GetMeanZ()
{
	float mean_z = 0;
	for(int i=0;i<cloud_num;i++)
	{
		mean_z += cloud_[i].z;
	}
	mean_z/=cloud_num;
}

void calAngle(float x, float y, float* temp_tangle)
{
	if (x == 0 && y == 0) {
		*temp_tangle = 0;
	}
	else if (y >= 0) {
		*temp_tangle = (float)atan2(y, x);
	}
	else if (y <= 0) {
		*temp_tangle = (float)atan2(y, x) + 2 * M_PI;
	}
}

void calRange(const PointXYZ* p, float* range)
{
	*range = sqrt(p->x * p->x + p->y * p->y);
}

float getMinValue(float x, float y)
{
	if (x > y)
		return y;
	return x;

}

void calRangeDiff(const PointXYZ* p1, const PointXYZ* p2, float* range)
{
	*range = sqrt((p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y) + (p1->z - p2->z) * (p1->z - p2->z));
}

void RangeProjection()
{
	float u = 0;
	float range = 0;
	for (int i = 0; i < cloud_num; ++i)
	{
//		if(cloud_[i].a >= 9)
//		{
//			u = cloud_[i].a;
//		}
//		else
//		{
//			u = cloud_[i].a + 360;
//		}
		calAngle(cloud_[i].x, cloud_[i].y, &u);
		calRange(&cloud_[i], &range);

		if (range < if_parameter_.min_range_ || range > if_parameter_.max_range_)
			continue;

		int col = round((if_parameter_.width_ - 1) * (u * 57.29579) / 180.0); // 57.29579 == 180 / M_PI
//		int col = round((if_parameter_.width_ - 1) * cloud_[i].a * 0.00555); // 57.29579 == 180 / M_PI , * 0.00555 == / 180
		int ind = cloud_[i].r;
//		int region = (int)((range - if_parameter_.min_range_) / if_parameter_.deltaR_);
		int region = (int)((range - if_parameter_.min_range_) * 0.5);

		int region_index = col * if_parameter_.length_ + region;
		int index = col * if_parameter_.height_ + ind;

		if(region_index < 0 || region_index >= IMAGE_WIDTH * 34)
			continue;

		if(ind < 0 || ind >= IMAGE_HEIGHT)
			continue;

		if(col < 0 || col >= IMAGE_WIDTH)
			continue;

		if(index < 0 || index >= POINT_SIZE)
			continue;
//		range_image_.at<cv::Vec3b>(ind, col) = cv::Vec3b(0, 255, 0);
		range_image[ind][col].b = 0;
		range_image[ind][col].g = 255;
		range_image[ind][col].r = 0;


		region_minz_[region_index] = getMinValue(region_minz_[region_index], cloud_[i].z);
		region_[ind][col] = (uint8_t)region;

		cloud_index_[index] = i;
	}
//	if (show_ground_image)
//	{
//		cv::Mat show_image;
//		cv::flip(range_image_, show_image, 0);
//		cv::imshow("TEst", show_image);
//		cv::waitKey(0);
//	}
}

void RECM()
{
	float pre_th = 0.;
	int remainder = 0;
	for (int i = 0; i < POINT_SIZE; ++i, ++remainder) {

		while(remainder>if_parameter_.length_) remainder-=if_parameter_.length_;

//		if (i % if_parameter_.length_ == 0)
		if (remainder == 0)
		{
			pre_th = region_minz_[i];
		}
		else
		{
//			region_minz_[i] = getMinValue(region_minz_[i], pre_th + if_parameter_.deltaR_ * (float)tan(if_parameter_.sigma_ * 0.017453288));
//			region_minz_[i] = getMinValue(region_minz_[i], pre_th + if_parameter_.deltaR_ * 0.1227845609);
			region_minz_[i] = getMinValue(region_minz_[i], pre_th + 0.2455691218);

			pre_th = region_minz_[i];
		}
	}

//	int r_cnt = 0;
	for (int i = 0; i < if_parameter_.width_; ++i)
	{
		for (int j = 0; j < if_parameter_.height_ / 2; ++j)
		{
			int index = i * if_parameter_.height_ + j;
			if(index < 0 || index >= POINT_SIZE)
				continue;
			int id = cloud_index_[index];
			if (id != -1)
			{
				int region_i = region_[j][i];
				if(i * if_parameter_.length_ + region_i >= 0 && i * if_parameter_.length_ + region_i < IMAGE_WIDTH * 34)
				{
				float th_height = region_minz_[i * if_parameter_.length_ + region_i];

				if(id < MAX_GROUND_POINT_NUM && id >= 0)
				{
				if (cloud_[id].z >= (th_height + if_parameter_.th_g_))
				{
	//				range_image_.at<cv::Vec3b>(j, i) = cv::Vec3b(0, 0, 255);
					range_image[j][i].b = 0;
					range_image[j][i].g = 0;
					range_image[j][i].r = 255;
	//				r_cnt++;
				}
				}
				}
			}
		}
	}

//	xil_printf(r_cnt);
//	if (show_ground_image)
//	{
//		cv::Mat show_image;
//		cv::flip(range_image_, show_image, 0);
//		cv::imshow("TEst", show_image);
//		cv::waitKey(0);
//	}

}

void dialationn(uint8_t vec[IMAGE_HEIGHT][IMAGE_WIDTH], uint8_t dilated_ground_channel[IMAGE_HEIGHT][IMAGE_WIDTH], int rows, int cols)
{

    for (int i = 0; i < rows; i ++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (vec[i][j] != 255)
            	continue;

//			if (i > 0) {
//				dilated_ground_channel[i-1][j] = 255;
//			}
//			if (j > 0) {
//				dilated_ground_channel[i][j-1] = 255;
//			}
//			if (i + 1<rows) {
//				dilated_ground_channel[i+1][j] = 255;
//			}
//			if (j + 1<cols) {
//				dilated_ground_channel[i][j+1] = 255;
//			}

            if (i > 0) {
				dilated_ground_channel[i-1][j] = 255;
			}
            if (i > 1) {
				dilated_ground_channel[i-2][j] = 255;
			}
			if (j > 0) {
				dilated_ground_channel[i][j-1] = 255;
			}
			if (j > 1) {
				dilated_ground_channel[i][j-2] = 255;
			}
			if (i + 1<rows) {
				dilated_ground_channel[i+1][j] = 255;
			}
			if (i + 2<rows) {
				dilated_ground_channel[i+2][j] = 255;
			}
			if (j + 1<cols) {
				dilated_ground_channel[i][j+1] = 255;
			}
			if (j + 2<cols) {
				dilated_ground_channel[i][j+2] = 255;
			}
        }
    }
//    swap(out, vec);
}

void JCP()
{
//	vector<cv::Mat> channels;
//	cv::split(range_image_, channels);
//	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
//	cv::dilate(channels[2], channels[2], element);
//	cv::merge(channels, range_image_);
//	uint8_t dilated_ground_channel[IMAGE_HEIGHT][IMAGE_WIDTH] = {0,};
//	uint8_t ground_channel[IMAGE_HEIGHT][IMAGE_WIDTH] = {0,};
	for(int i = 0; i<IMAGE_HEIGHT ; i++)
	{
		memset(ground_channel[i], 0x00, IMAGE_WIDTH);
		memset(dilated_ground_channel[i], 0x00, IMAGE_WIDTH);
	}

	for(int i = 0; i<IMAGE_HEIGHT ; i++)
	{
		for(int j = 0; j<IMAGE_WIDTH ; j++)
		{
			ground_channel[i][j] = range_image[i][j].r;
		}
	}

	dialationn(ground_channel, dilated_ground_channel, IMAGE_HEIGHT, IMAGE_WIDTH);

	for(int i = 0; i<IMAGE_HEIGHT ; i++)
	{
		for(int j = 0; j<IMAGE_WIDTH ; j++)
		{
			range_image[i][j].r = dilated_ground_channel[i][j];
		}
	}


//	PixelIndex qt[MAX_GROUND_POINT_NUM];
//	int qt_ind = 0;
//	for (int i = 0; i < if_parameter_.width_; ++i) {
//		for (int j = 0; j < if_parameter_.height_; ++j) {
//			if (range_image[j][i].b == 0 && range_image[j][i].g == 255 && range_image[j][i].r == 255)
//			{
////				PixelIndex id;
////				id.x = i;
////				id.y = j;
//				if (cloud_index_[j * if_parameter_.height_ + i] != -1) {
//					qt[qt_ind].x = i;
//					qt[qt_ind].y = j;
//					qt_ind++;
////					qt.push(id);
////					range_image_.at<cv::Vec3b>(j, i) = cv::Vec3b(255, 0, 0);
//					range_image[j][i].b = 255;
//					range_image[j][i].g = 0;
//					range_image[j][i].r = 0;
//				}
//				else {
////					range_image_.at<cv::Vec3b>(j, i) = cv::Vec3b(0, 0, 255);
//					range_image[j][i].b = 0;
//					range_image[j][i].g = 0;
//					range_image[j][i].r = 255;
//				}
//			}
//		}
//	}
////	if (show_ground_image)
////	{
////		cv::Mat show_image;
////		cv::flip(range_image_, show_image, 0);
////		cv::imshow("TEst", show_image);
////		cv::waitKey(0);
////	}
//	int qt_size = qt_ind;
//	qt_ind = 0;
//	while (qt_ind < qt_size)
//	{
//		PixelIndex id = qt[qt_ind];
//		int cloud_id = id.x * if_parameter_.height_ + id.y;
//		float D[24];
//		int mask[24];
//		float sumD = 0;
//		for (int i = 0; i < 24; ++i)
//		{
//			int nx = neighborx_[i] + id.x;
//			int ny = neighbory_[i] + id.y;
//
//			int ncloud_id = nx * if_parameter_.height_ + ny;
//			float range_diff = 0;
//
//			if (nx < 0 || nx >= if_parameter_.width_ || ny < 0 || ny >= if_parameter_.height_ || cloud_index_[ncloud_id] == -1)
//			{
//				D[i] = 0;
//				sumD += D[i];
//				mask[i] = -1;
//				continue;
//			}
//
//			calRangeDiff(&cloud_[cloud_index_[cloud_id]], &cloud_[cloud_index_[ncloud_id]], &range_diff);
//			if (range_diff > 0) {
//				D[i] = 0;
//				sumD += D[i];
//			}
//			else {
//				D[i] = (exp(-5 * range_diff));
//				sumD += D[i];
//			}
//			if (range_image[ny][nx].b == 255 && range_image[ny][nx].g == 0 && range_image[ny][nx].r == 0)
//			{
//				mask[i] = 2;
//			}
//			else if (range_image[ny][nx].b == 0 && range_image[ny][nx].g == 255 && range_image[ny][nx].r == 0)
//			{
//				mask[i] = 1;
//			}
//			else if (range_image[ny][nx].b == 0 && range_image[ny][nx].g == 0 && range_image[ny][nx].r == 255)
//			{
//				mask[i] = 0;
//			}
//		}
//
//		float W[24];
////		W = D / sumD;
//		for(int i = 0; i < 24 ; i++)
//		{
//			W[i] = D[i] / sumD;
//		}
//
//		float score_r = 0;
//		float score_g = 0;
//		for (int i = 0; i < 24; ++i)
//		{
//			if (mask[i] == 0)
//			{
//				score_r += W[i];
//			}
//			else if (mask[i] == 1)
//			{
//				score_g += W[i];
//			}
//		}
//
//		if (score_r > score_g) {
//			range_image[id.y][id.x].b = 0;
//			range_image[id.y][id.x].g = 0;
//			range_image[id.y][id.x].r = 255;
//		}
//		else {
//			range_image[id.y][id.x].b = 0;
//			range_image[id.y][id.x].g = 255;
//			range_image[id.y][id.x].r = 0;
//		}
//
//		qt_ind++;
//	}
//	if (show_ground_image)
//	{
//		cv::Mat show_image;
//		cv::flip(range_image_, show_image, 0);
//		cv::imshow("TEst", show_image);
//		cv::waitKey(0);
//	}
}

void GetGroundDetectionResult(GDResult* gd_result)
{

	gd_result->gounrd_z_mean_ = 0;
	gd_result->ground_z_deviation_ = 0;
	gd_result->ground_point_cnt_ = 0;
	gd_result->is_ground_detected_ = 0;
	for (int i = 0; i < if_parameter_.width_; ++i)
	{
		for (int j = 0; j < if_parameter_.height_ / 2; ++j)
		{
			if(i * if_parameter_.height_ + j >= 0 && i * if_parameter_.height_ + j < POINT_SIZE)
			{
			int index = cloud_index_[i * if_parameter_.height_ + j];
			if (index == -1)
				continue;
			if (range_image[j][i].b == 0 && range_image[j][i].g == 255 && range_image[j][i].r == 0)
			{
				gd_result->gounrd_z_mean_ += cloud_[index].z;
				++(gd_result->ground_point_cnt_);
			}
//			else if (range_image[j][i].b == 0 && range_image[j][i].g == 0 && range_image[j][i].r == 255) {
//				cloud_ob.push_back(cloud_[index]);
//			}
			}
		}
	}

//	for (int i = 0; i < cloud_num; i++)
//	{
//		result += cloud_[i].z;
//	}
//	if(gd_result->ground_point_cnt_ > gd_parameter_.ground_point_limit_)
	{
		gd_result->gounrd_z_mean_ /= gd_result->ground_point_cnt_;  // 이부분인듯
		// add 240613
		if(gd_result->ground_point_cnt_ == 0)
			gd_result->gounrd_z_mean_  = 0;
		gd_result->is_ground_detected_ = 1;

		for (int i = 0; i < if_parameter_.width_; ++i)
		{
			for (int j = 0; j < if_parameter_.height_ / 2; ++j)
			{
				int index = cloud_index_[i * if_parameter_.height_ + j];
				if (index == -1)
					continue;
				if (range_image[j][i].b == 0 && range_image[j][i].g == 255 && range_image[j][i].r == 0)
				{
					if(index < cloud_num && index >= 0)
						gd_result->ground_z_deviation_ += powf(cloud_[index].z - gd_result->gounrd_z_mean_, 2);

				}
	//			else if (range_image[j][i].b == 0 && range_image[j][i].g == 0 && range_image[j][i].r == 255) {
	//				cloud_ob.push_back(cloud_[index]);
	//			}
			}
		}
		gd_result->ground_z_deviation_ /= gd_result->ground_point_cnt_;
		if (gd_result->ground_point_cnt_ == 0)
			gd_result->ground_z_deviation_ = 0;
		gd_result->ground_z_deviation_ = sqrtf(gd_result->ground_z_deviation_);

		if (gd_result->ground_z_deviation_ > gd_parameter_.ground_z_devi_limit_)
			gd_result->is_ground_detected_ = 0;
	}
}
