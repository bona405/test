/*
 * remove_ghost.c
 *
 *  Created on: 2022. 6. 16.
 *      Author: ProDesk
 */

#include "remove_ghost.h"

int num_of_ground_point = 0;

void InitRGParameter(RGParameter* rg_parameter)
{
	rg_parameter->z_offset_ = 0.0;
	rg_parameter->z_offset_2_ = 0.3;
	rg_parameter->vertical_check_num_ = 2;
	rg_parameter->horizontal_check_num_ = 2;
	rg_parameter->x_offset_ = 3;

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
}

void GetPosX(float* pos_x, float distance, float cos_vert_angle, float sin_horz_angle)
{
	*pos_x = distance * cos_vert_angle * sin_horz_angle;
}

void GetPosY(float* pos_y, float distance, float cos_vert_angle, float cos_horz_angle)
{
	*pos_y = distance * cos_vert_angle * cos_horz_angle;
}

void RemoveGhost(RawDataBlock* bram_data_blocks, ConvertedDataBlock* current_frame_blocks, int frame_block_size, GDResult* gd_result, u32 fov_data_block_count)
{
	u32 total_azimuth_count = fov_data_block_count / 2;
	//220714
//	if(gd_result->is_ground_detected_ == 0)
//		return;
//	memset(&is_ground_point, 0x00, FRAME_BLOCK_SIZE * CHANNEL_SIZE);
	float* vertical_angle_arr_p;
	float* sin_vert_angle_arr;
	float* cos_vert_angle_arr;
	num_of_ground_point=0;

	for(int i = 0 ; i < frame_block_size ; ++i)
	{
		int top_bottom_index = 0;
		if(i >= total_azimuth_count)
		{
			vertical_angle_arr_p = vertical_angle_arr_top;
			sin_vert_angle_arr = sin_vert_angle_map_top;
			cos_vert_angle_arr = cos_vert_angle_map_top;
			top_bottom_index = 1;
		}
		else
		{
			vertical_angle_arr_p = vertical_angle_arr_bottom;
			sin_vert_angle_arr = sin_vert_angle_map_bottom;
			cos_vert_angle_arr = cos_vert_angle_map_bottom;
			top_bottom_index = 0;
		}


//		int stop_index = 0;
//		int stop_count = 3;
//		int azimuth_count = 1152;
////		stop_index = i % stop_count;
//		top_bottom_index = (int)(i / (stop_count * azimuth_count)) % 2;
//		current_azimuth = current_frame_blocks[i].azimuth_;
		for(int j = 0 ; j < CHANNEL_SIZE ; ++j)
		{
//			if (i%3 == 0 && gd_result->is_ground_detected_ == 1)
			if(current_frame_blocks[i].distance_[j] < MAX_DISTANCE_M)
			{
				if (i%3 == 0)
				{
					//220714
					if(gd_result->is_ground_detected_ != 1)
						continue;
					float current_distance = current_frame_blocks[i].distance_[j];
					float pos_z;
					GetPosZ(&pos_z, current_distance, sin_vert_angle_arr[j]);
//					if(current_frame_blocks[i].distance_[j] < ground_detect_max_distance_&&
//							gd_result->gounrd_z_mean_ - ground_detect_z_offset_ < pos_z && gd_result->gounrd_z_mean_ + ground_detect_z_offset_ > pos_z)
//					{
//						is_ground_point[i][j] = 1;
//						num_of_ground_point++;
//					}
					//if (distance >= viewer_info_->get_min_range() && distance <= viewer_info_->get_max_range() && ground_min_z_value > pos_z)
					if (gd_result->gounrd_z_mean_ - rg_parameter_.z_offset_2_ + rg_parameter_.z_offset_ > pos_z)
					{
						//get real point's vertical angle

//						float current_vertical_angle = vertical_angle_arr_p[j];
						float pos_x, pos_y;
						GetPosX(&pos_x, current_distance, cos_vert_angle_arr[j], sin_horz_angle_map[i]);
						GetPosY(&pos_y, current_distance, cos_vert_angle_arr[j], cos_horz_angle_map[i]);
						float distance_xy = sqrt(pos_x * pos_x + pos_y * pos_y);
						float real_point_z = pos_z - 2 * (pos_z - gd_result->gounrd_z_mean_);
						float real_point_distance = sqrt(distance_xy * distance_xy + real_point_z * real_point_z);
						float real_point_angle_raw = (atan(real_point_z / distance_xy)) * 57.2957914; // 57.2957914 == 180 / M_PI
//						float real_point_angle;
						int real_point_angle_index = 0;
						/*float test = atanf(pos_z / distance);
						float test2 = atan2f(distance, pos_z);
						cout <<"real point's angle : " << real_angle << endl;*/

						int num_of_channels = 16;
	//					if (viewer_info_->get_vert_angle() == 16.0f)
	//					{
	//						int start_ind = 0;
	//						int end_ind = num_of_channels - 1;
	//						int current_ind = 0;
	//
	//						while (start_ind <= end_ind)
	//						{
	//							if (end_ind - start_ind == 1)
	//							{
	//								if (abs(vertical_angle_arr_16[end_ind] - real_point_angle_raw) > abs(vertical_angle_arr_16[start_ind] - real_point_angle_raw))
	//									real_point_angle = vertical_angle_arr_16[start_ind];
	//								else
	//									real_point_angle = vertical_angle_arr_16[end_ind];
	//								real_point_angle_index = current_ind;
	//								break;
	//							}
	//
	//							current_ind = (start_ind + end_ind) / 2;
	//
	//							if (real_point_angle_raw == vertical_angle_arr_16[current_ind])
	//							{
	//								real_point_angle = real_point_angle_raw;
	//								real_point_angle_index = current_ind;
	//								break;
	//							}
	//							else if (real_point_angle_raw < vertical_angle_arr_16[current_ind])
	//								end_ind = current_ind;
	//							else
	//								start_ind = current_ind;
	//						}
	//					}
	//					else if(viewer_info_->get_vert_angle() == 10.0f)
						{
							int start_ind = 0;
							int end_ind = num_of_channels - 1;
							int current_ind = 0;

							while (start_ind <= end_ind)
							{
								if (end_ind - start_ind == 1)
								{
									if (abs(vertical_angle_arr_p[end_ind] - real_point_angle_raw) > abs(vertical_angle_arr_p[start_ind] - real_point_angle_raw))
//										real_point_angle = vertical_angle_arr_p[start_ind];
										real_point_angle_index = start_ind;
									else
//										real_point_angle = vertical_angle_arr_p[end_ind];
										real_point_angle_index = end_ind;
//									real_point_angle_index = current_ind;
									break;
								}

								current_ind = (start_ind + end_ind) * 0.5;

								if (real_point_angle_raw == vertical_angle_arr_p[current_ind])
								{
//									real_point_angle = real_point_angle_raw;
									real_point_angle_index = current_ind;
									break;
								}
								else if (real_point_angle_raw < vertical_angle_arr_p[current_ind])
									end_ind = current_ind;
								else
									start_ind = current_ind;
							}
						}
						int check_num_vertical = rg_parameter_.vertical_check_num_;

						for (size_t check_ind_vert = 0; check_ind_vert < check_num_vertical; check_ind_vert++)
						{
							if (real_point_angle_index + check_ind_vert >= num_of_channels)
								continue;
							real_point_angle_index += check_ind_vert;
							int real_point_ind = i;
							int top_bottom_ind_offset = frame_block_size * 0.5;
//							int real_point_angle_remainder = real_point_angle_index % 2;
//							int real_point_angle_quotient = real_point_angle_index * 0.5;

//							if (real_point_angle_remainder == 1 && top_bottom_index == 1)
//							{
//								real_point_ind = i;
//							}
//							else if (real_point_angle_remainder == 1 && top_bottom_index == 0)
//							{
//								real_point_ind = i + top_bottom_ind_offset;
//							}
//							else if (real_point_angle_remainder == 0 && top_bottom_index == 1)
//							{
//								real_point_ind = i - top_bottom_ind_offset;
//							}
//							else if (real_point_angle_remainder == 0 && top_bottom_index == 0)
//							{
//								real_point_ind = i;
//							}


								real_point_ind = i;


							//수평
							if(real_point_distance < current_frame_blocks[real_point_ind].distance_[real_point_angle_index] + rg_parameter_.x_offset_ && real_point_distance > current_frame_blocks[real_point_ind].distance_[real_point_angle_index] - rg_parameter_.x_offset_)
							{
								//							pos_x = 0;
								//							pos_y = 0;
								//							pos_z = 0;
								bram_data_blocks[i].distance_[j].distance = 0;
								current_frame_blocks[i].distance_[j] = 0;
							}

							int check_num_horizontal = rg_parameter_.horizontal_check_num_;
							for(size_t check_ind_horiz = 1 ; check_ind_horiz <= check_num_horizontal ; check_ind_horiz++)
							{
								if(real_point_ind + 3 * check_ind_horiz < frame_block_size)
									if(real_point_distance < current_frame_blocks[real_point_ind + 3 * check_ind_horiz].distance_[real_point_angle_index] + rg_parameter_.x_offset_ && real_point_distance > current_frame_blocks[real_point_ind + 3 * check_ind_horiz].distance_[real_point_angle_index] - rg_parameter_.x_offset_)
									{
										//									pos_x = 0;
										//									pos_y = 0;
										//									pos_z = 0;
										bram_data_blocks[i].distance_[j].distance = 0;
										current_frame_blocks[i].distance_[j] = 0;
									}

								if(real_point_ind - 3 * check_ind_horiz > 0)
									if(real_point_distance < current_frame_blocks[real_point_ind - 3 * check_ind_horiz].distance_[real_point_angle_index] + rg_parameter_.x_offset_ && real_point_distance > current_frame_blocks[real_point_ind - 3 * check_ind_horiz].distance_[real_point_angle_index] - rg_parameter_.x_offset_)
									{
										//									pos_x = 0;
										//									pos_y = 0;
										//									pos_z = 0;
										bram_data_blocks[i].distance_[j].distance = 0;
										current_frame_blocks[i].distance_[j] = 0;
									}
							}

							if(top_bottom_index == 1)
							{
								real_point_ind = i- top_bottom_ind_offset;
							}
							else if(top_bottom_index == 0)
							{
								real_point_ind = i + top_bottom_ind_offset;
							}

							//수평
							if(real_point_distance < current_frame_blocks[real_point_ind].distance_[real_point_angle_index] + rg_parameter_.x_offset_ && real_point_distance > current_frame_blocks[real_point_ind].distance_[real_point_angle_index] - rg_parameter_.x_offset_)
							{
								//							pos_x = 0;
								//							pos_y = 0;
								//							pos_z = 0;
								bram_data_blocks[i].distance_[j].distance = 0;
								current_frame_blocks[i].distance_[j] = 0;
							}

//							int check_num_horizontal = rg_parameter_.horizontal_check_num_;
							for(size_t check_ind_horiz = 1 ; check_ind_horiz <= check_num_horizontal ; check_ind_horiz++)
							{
								if(real_point_ind + 3 * check_ind_horiz < frame_block_size)
									if(real_point_distance < current_frame_blocks[real_point_ind + 3 * check_ind_horiz].distance_[real_point_angle_index] + rg_parameter_.x_offset_ && real_point_distance > current_frame_blocks[real_point_ind + 3 * check_ind_horiz].distance_[real_point_angle_index] - rg_parameter_.x_offset_)
									{
										//									pos_x = 0;
										//									pos_y = 0;
										//									pos_z = 0;
										bram_data_blocks[i].distance_[j].distance = 0;
										current_frame_blocks[i].distance_[j] = 0;
									}

								if(real_point_ind - 3 * check_ind_horiz > 0)
									if(real_point_distance < current_frame_blocks[real_point_ind - 3 * check_ind_horiz].distance_[real_point_angle_index] + rg_parameter_.x_offset_ && real_point_distance > current_frame_blocks[real_point_ind - 3 * check_ind_horiz].distance_[real_point_angle_index] - rg_parameter_.x_offset_)
									{
										//									pos_x = 0;
										//									pos_y = 0;
										//									pos_z = 0;
										bram_data_blocks[i].distance_[j].distance = 0;
										current_frame_blocks[i].distance_[j] = 0;
									}
							}

						}



						////수직
						//if(real_point_angle_index < 31)
						//	real_point_angle_index++;
						//real_point_ind = i;
						//top_bottom_ind_offset = fovDataArr.size() / 2;
						//if (real_point_angle_index % 2 == 1 && top_bottom_index == 1)
						//{
						//	real_point_ind = i;
						//}
						//else if (real_point_angle_index % 2 == 1 && top_bottom_index == 0)
						//{
						//	real_point_ind = i + top_bottom_ind_offset;
						//}
						//else if (real_point_angle_index % 2 == 0 && top_bottom_index == 1)
						//{
						//	real_point_ind = i - top_bottom_ind_offset;
						//}
						//else if (real_point_angle_index % 2 == 0 && top_bottom_index == 0)
						//{
						//	real_point_ind = i;
						//}

						//if (real_point_distance < (float)fovDataArr[real_point_ind].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//	&& real_point_distance >(float)fovDataArr[real_point_ind].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//{
						//	pos_x = 0;
						//	pos_y = 0;
						//	pos_z = 0;
						//}

						//if (real_point_ind + 3 < fovDataArr.size())
						//	if (real_point_distance < (float)fovDataArr[real_point_ind + 3].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//		&& real_point_distance >(float)fovDataArr[real_point_ind + 3].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//	{
						//		pos_x = 0;
						//		pos_y = 0;
						//		pos_z = 0;
						//	}

						//if (real_point_ind - 3 < fovDataArr.size())
						//	if (real_point_distance < (float)fovDataArr[real_point_ind - 3].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//		&& real_point_distance >(float)fovDataArr[real_point_ind - 3].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//	{
						//		pos_x = 0;
						//		pos_y = 0;
						//		pos_z = 0;
						//	}

						//if (real_point_ind + 6 < fovDataArr.size())
						//	if (real_point_distance < (float)fovDataArr[real_point_ind + 6].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//		&& real_point_distance >(float)fovDataArr[real_point_ind + 6].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//	{
						//		pos_x = 0;
						//		pos_y = 0;
						//		pos_z = 0;
						//	}

						//if (real_point_ind - 6 < fovDataArr.size())
						//	if (real_point_distance < (float)fovDataArr[real_point_ind - 6].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//		&& real_point_distance >(float)fovDataArr[real_point_ind - 6].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//	{
						//		pos_x = 0;
						//		pos_y = 0;
						//		pos_z = 0;
						//	}

						////수직2
						//if (real_point_angle_index < 31)
						//	real_point_angle_index++;
						//real_point_ind = i;
						//top_bottom_ind_offset = fovDataArr.size() / 2;
						//if (real_point_angle_index % 2 == 1 && top_bottom_index == 1)
						//{
						//	real_point_ind = i;
						//}
						//else if (real_point_angle_index % 2 == 1 && top_bottom_index == 0)
						//{
						//	real_point_ind = i + top_bottom_ind_offset;
						//}
						//else if (real_point_angle_index % 2 == 0 && top_bottom_index == 1)
						//{
						//	real_point_ind = i - top_bottom_ind_offset;
						//}
						//else if (real_point_angle_index % 2 == 0 && top_bottom_index == 0)
						//{
						//	real_point_ind = i;
						//}

						//if (real_point_distance < (float)fovDataArr[real_point_ind].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//	&& real_point_distance >(float)fovDataArr[real_point_ind].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//{
						//	pos_x = 0;
						//	pos_y = 0;
						//	pos_z = 0;
						//}

						//if (real_point_ind + 3 < fovDataArr.size())
						//	if (real_point_distance < (float)fovDataArr[real_point_ind + 3].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//		&& real_point_distance >(float)fovDataArr[real_point_ind + 3].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//	{
						//		pos_x = 0;
						//		pos_y = 0;
						//		pos_z = 0;
						//	}

						//if (real_point_ind - 3 < fovDataArr.size())
						//	if (real_point_distance < (float)fovDataArr[real_point_ind - 3].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//		&& real_point_distance >(float)fovDataArr[real_point_ind - 3].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//	{
						//		pos_x = 0;
						//		pos_y = 0;
						//		pos_z = 0;
						//	}

						//if (real_point_ind + 6 < fovDataArr.size())
						//	if (real_point_distance < (float)fovDataArr[real_point_ind + 6].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//		&& real_point_distance >(float)fovDataArr[real_point_ind + 6].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//	{
						//		pos_x = 0;
						//		pos_y = 0;
						//		pos_z = 0;
						//	}

						//if (real_point_ind - 6 < fovDataArr.size())
						//	if (real_point_distance < (float)fovDataArr[real_point_ind - 6].data_points_[real_point_angle_index / 2].distance_ / 1000 + viewer_info_->get_x_offset()
						//		&& real_point_distance >(float)fovDataArr[real_point_ind - 6].data_points_[real_point_angle_index / 2].distance_ / 1000 - viewer_info_->get_x_offset())
						//	{
						//		pos_x = 0;
						//		pos_y = 0;
						//		pos_z = 0;
						//	}

					}
				}
	//			else if(stop_index > 0)
				else
				{
					float pos_z;
					GetPosZ(&pos_z, current_frame_blocks[i].distance_[j], sin_vert_angle_arr[j]);

//					ground_detect_max_distance_
//					if(current_frame_blocks[i].distance_[j] < ground_detect_max_distance_&&
//							gd_result->gounrd_z_mean_ - ground_detect_z_offset_ < pos_z && gd_result->gounrd_z_mean_ + ground_detect_z_offset_ > pos_z)
//					{
//						is_ground_point[i][j] = 1;
//						num_of_ground_point++;
//					}

					if (gd_result->gounrd_z_mean_ - rg_parameter_.z_offset_2_ + 0.1> pos_z)
//					if (gd_result_.gounrd_z_mean_ > pos_z)
					{
						//continue;
	//					pos_x = 0;
	//					pos_y = 0;
	//					pos_z = 0;
	//					current_frame_data->block_data_bram[i].data_points[j] = 0;
						bram_data_blocks[i].distance_[j].distance = 0;
						current_frame_blocks[i].distance_[j] = 0;

	//					fovDataArr[i].data_points_[j].distance_ = 1000000;
					}
				}
			}
		}
	}

//	for (int i = 0; i < frame_block_size; ++i)
//	{
//		for (size_t j = 0; j < 16; ++j)
//		{
//			if (i%3 != 0)
//				continue;
//			float pos_z;
//			GetPosZ(&pos_z, current_frame_data->block_data_bram[i].data_points[j] / 256.0, vertical_angle_arr_p[j]);
//			if (gd_result->gounrd_z_mean_ - rg_parameter_.z_offset_2_ + rg_parameter_.z_offset_ < pos_z)
//				continue;
//				//if (distance >= viewer_info_->get_min_range() && distance <= viewer_info_->get_max_range() && ground_min_z_value > pos_z)
//
//
//			int i_ind_arr[16];
//			int j_ind_arr[16];
//
//			int num_of_echoes = 3;
//			i_ind_arr[0] = i - 1 * num_of_echoes + 1;
//			i_ind_arr[1] = i + 1;
//			i_ind_arr[2] = i + 1 * num_of_echoes + 1;
//
//			i_ind_arr[3] = i - 1 * num_of_echoes + 1;
//			i_ind_arr[4] = i + 1 * num_of_echoes + 1;
//
//			i_ind_arr[5] = i - 1 * num_of_echoes + 1;
//			i_ind_arr[6] = i + 1;
//			i_ind_arr[7] = i + 1 * num_of_echoes + 1;
//
//			i_ind_arr[8] = i - 1 * num_of_echoes;
//			i_ind_arr[9] = i;
//			i_ind_arr[10] = i + 1 * num_of_echoes;
//
//			i_ind_arr[11] = i - 1 * num_of_echoes;
//			i_ind_arr[12] = i + 1 * num_of_echoes;
//
//			i_ind_arr[13] = i - 1 * num_of_echoes;
//			i_ind_arr[14] = i;
//			i_ind_arr[15] = i + 1 * num_of_echoes;
//
//			j_ind_arr[0] = j - 1;
//			j_ind_arr[1] = j;
//			j_ind_arr[2] = j + 1;
//
//			j_ind_arr[3] = j - 1;
//			j_ind_arr[4] = j + 1;
//
//			j_ind_arr[5] = j - 1;
//			j_ind_arr[6] = j;
//			j_ind_arr[7] = j + 1;
//
//			j_ind_arr[8] = j - 1;
//			j_ind_arr[9] = j;
//			j_ind_arr[10] = j + 1;
//
//			j_ind_arr[11] = j - 1;
//			j_ind_arr[12] = j + 1;
//
//			j_ind_arr[13] = j - 1;
//			j_ind_arr[14] = j;
//			j_ind_arr[15] = j + 1;
//
//			for (size_t stop_i = 0; stop_i < num_of_echoes; stop_i++)
//			{
//				for (size_t comp_i = 0; comp_i < 8; comp_i++)
//				{
//					if (i_ind_arr[comp_i] >= 0 && j_ind_arr[comp_i] >= 0 && i_ind_arr[comp_i] + stop_i < frame_block_size && j_ind_arr[comp_i] < 16)
//						if (is_ground_point[i_ind_arr[comp_i] + stop_i][j_ind_arr[comp_i]] == 1)
//						{
////							lidar_point_cloud_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].point_[0] = 0;
////							lidar_point_cloud_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].point_[1] = 0;
////							lidar_point_cloud_data[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].point_[2] = 0;
//							current_frame_data->block_data_bram[i].data_points[j] = 0;
//							is_ground_point[i][j] = 1;
//							break;
//						}
//				}
//				if(is_ground_point[i][j] == 1)
//					break;
//			}
//		}
//	}
}
