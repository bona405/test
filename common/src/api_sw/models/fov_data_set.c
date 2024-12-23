/*
 * fov_data_set.c
 *
 *  Created on: 2022. 7. 26.
 *      Author: Wonju
 */


#include "fov_data_set.h"
#include "../../api_hw/api_hw_enc_ip_ctrl.h"
#include "math.h"

#define BOTTOM_MOTOR_OFFSET 128
#define TOP_MOTOR_OFFSET 2174

void SetAzimuthAndDistanceA3(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset)
{
	uint32_t same_azimuth_count = 0;
	int32_t pre_azimuth = 0;
	int32_t top_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;
	int32_t bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;
//	int32_t top_motor_init_value = 67500;
//	int32_t bottom_motor_init_value = 67500;
#if defined(G32_25HZ)
	bottom_motor_init_value -= bottom_horz_offset * 234;
#endif

//	int32_t* routing_delay = lidar_state->calibration_setting_.routing_delay;
	int32_t clip_min = distance_offset;
	int32_t current_distance = 0;

	uint8_t is_start_vert_angle_even = 0;
	if(fov_test_.start_vertical_angle % 2 == 0)
		is_start_vert_angle_even = 1;
	uint8_t is_end_vert_angle_even = 0;
	if(fov_test_.end_vertical_angle % 2 == 0)
		is_end_vert_angle_even = 1;
	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index++)
	{
//		//bottom
//		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared->block_data_bram[azimuth_index].azimuth_;
//		//top
//		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].azimuth_;

		if(pre_azimuth == block_data_shared[azimuth_index].azimuth_)
		{
			same_azimuth_count++;
		}
		else
		{
			same_azimuth_count = 0;
		}

		pre_azimuth = block_data_shared[azimuth_index].azimuth_;


		if(same_azimuth_count < 3)
		{

			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176);
			current_frame_blocks[azimuth_index].azimuth_ = block_data_shared[azimuth_index].azimuth_ * 0.001;
			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176);
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared[azimuth_index + top_bottom_offset].azimuth_ * 0.001;
		}
		else
		{

			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176 - 88);
			current_frame_blocks[azimuth_index].azimuth_ = (block_data_shared[azimuth_index].azimuth_) * 0.001;
			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176 - 88);
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = (block_data_shared[azimuth_index + top_bottom_offset].azimuth_) * 0.001;
		}



		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; vertical_angle_index++)
		{
			//bottom

//			current_distance = block_data_shared->block_data_bram[azimuth_index].distance_[vertical_angle_index] + routing_delay[vertical_angle_index] - clip_min;
			current_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index] - clip_min;
			if(current_distance < 0 || current_distance > MAX_DISTANCE)
			{

				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
			}
			else
			{
				if(is_end_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
											vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
											current_distance = 0;

				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
												vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2)-1)
												current_distance = 0;
				}

				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = current_distance * DIV_256;
			}
			//top

			current_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;

			if(current_distance < 0 || current_distance > MAX_DISTANCE)
			{
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
			}
			else
			{
				if(is_start_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						current_distance = 0;
				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) - 1 || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						current_distance = 0;
				}

				if(is_end_vert_angle_even == 0)
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
						current_distance = 0;
				}
				else
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
											current_distance = 0;
				}

				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = current_distance * DIV_256;
			}
		}

//		int err = 0;
//		if(pre_azim - current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ > 0.3)
//			err = 1;

//		pre_azim = current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_;
	}
}

void SetAzimuthAndDistanceB1(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int hz_option)
{

	int tmp_azi_cnt = (azimuth_count/3) - 1;
//	volatile int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? (lidar_state_.a2z_setting.fov_correction / tmp_azi_cnt):(176);
//	volatile int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? ((float)lidar_state_.a2z_setting.fov_correction / 120000 *176):(176);
//	volatile int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(lidar_state_.a2z_setting.fov_correction/2 + EncCtrlGetStartEncCount1()*resolution):(90000);
//	volatile int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*((float)lidar_state_.a2z_setting.fov_correction / 120000)):(90000);
	int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? (lidar_state_.a2z_setting.fov_correction):(176);
	int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*(((float)lidar_state_.a2z_setting.fov_correction*684) / 120384)):(90000);

	uint32_t same_azimuth_count = 0;
	int32_t pre_azimuth = 0;
//	int32_t top_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;
//	int32_t top_motor_init_value = 450096 - EncCtrlGetStartEncCount2() * 176;
//	int32_t top_motor_init_value = 90000 - ((EncCtrlGetStartEncCount2()-2048)) * 176;
//	int32_t top_motor_init_value = 112230 - ((EncCtrlGetStartEncCount2()-2048)) * 219;
	int32_t top_motor_init_value = base_angle - ((EncCtrlGetStartEncCount2()-2048)) * resolution;
//	int32_t top_motor_init_value = 67472;

//	int32_t bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;
//	int32_t bottom_motor_init_value = 112230 - EncCtrlGetStartEncCount1() * 219;
	int32_t bottom_motor_init_value = base_angle - EncCtrlGetStartEncCount1() * resolution;

//	int32_t bottom_motor_init_value = 67472;
//	int32_t top_motor_init_value = 67500;
//	int32_t bottom_motor_init_value = 67500;
#if defined(G32_25HZ)
	bottom_motor_init_value -= bottom_horz_offset * 234;
#endif

//	int32_t* routing_delay = lidar_state->calibration_setting_.routing_delay;
	int32_t clip_min = distance_offset;
	int32_t current_distance = 0;

	int max_distance0;
	int max_distance1;
	if(hz_option == 0)//10hz
	{
		max_distance0 = MAX_DISTANCE;
		max_distance1 = MAX_DISTANCE;
	}
	else if (hz_option == 1)//20hz
	{
		max_distance0 = MAX_DISTANCE;
		max_distance1 = MAX_DISTANCE;
	}
//	else if (hz_option == 2)//25hz or error val
	else//25hz or error val
	{
		max_distance0 = MAX_DISTANCE;
//		max_distance1 = 51200;
//		max_distance1 = 44672;	//org 179m
		max_distance1 = 51200;
//		max_distance1 = MAX_DISTANCE;
	}




	uint8_t is_start_vert_angle_even = 0;
	if(fov_test_.start_vertical_angle % 2 == 0)
		is_start_vert_angle_even = 1;
	uint8_t is_end_vert_angle_even = 0;
	if(fov_test_.end_vertical_angle % 2 == 0)
		is_end_vert_angle_even = 1;
	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
	{
//		//bottom
//		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared->block_data_bram[azimuth_index].azimuth_;
//		//top
//		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].azimuth_;

		if(pre_azimuth == block_data_shared[azimuth_index].azimuth_)
		{
			same_azimuth_count++;
		}
		else
		{
			same_azimuth_count = 0;
		}

		pre_azimuth = block_data_shared[azimuth_index].azimuth_;


		if(same_azimuth_count < 3)
		{

//			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176);
//			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 219);
			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * resolution);
//			block_data_shared[azimuth_index].azimuth_ =
//							(lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?
//									(((int) (bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_- bottom_start_enc_cnt) * 176))	*
//											((float)lidar_state_.a2z_setting.fov_correction	/ (bottom_motor_init_value * 2))) :
//									((int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176));
			current_frame_blocks[azimuth_index].azimuth_ = block_data_shared[azimuth_index].azimuth_ * 0.001;
//			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176);
//			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 219);
			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * resolution);
//			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ =
//									(lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?
//											(((int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176))	*
//													((float)lidar_state_.a2z_setting.fov_correction	/ (bottom_motor_init_value * 2))) :
//											((int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176));
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared[azimuth_index + top_bottom_offset].azimuth_ * 0.001;
		}
		else
		{

			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176 - 88);
			current_frame_blocks[azimuth_index].azimuth_ = (block_data_shared[azimuth_index].azimuth_) * 0.001;
			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176 - 88);
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = (block_data_shared[azimuth_index + top_bottom_offset].azimuth_) * 0.001;
		}



		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; vertical_angle_index++)
		{
			//bottom

//			current_distance = block_data_shared->block_data_bram[azimuth_index].distance_[vertical_angle_index] + routing_delay[vertical_angle_index] - clip_min;
			current_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index] - clip_min;
//			current_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
			if(current_distance <= 0 || current_distance > max_distance0  || (vertical_angle_index < 4 && current_distance > max_distance1) || (vertical_angle_index > 11 && current_distance > max_distance1) )
//			if(0)
//			if(current_distance < 0 || current_distance > MAX_DISTANCE)
//			if(current_distance < 0 || current_distance > 65535)
			{

//				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 2560;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
			}
			else
			{
				if(is_end_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
											vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
											current_distance = 0;

				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
												vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2)-1)
												current_distance = 0;
				}

				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = current_distance * DIV_256;
			}
			//top

			current_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;
//			current_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

			if(current_distance <= 0 || current_distance > max_distance0  || (vertical_angle_index < 4 && current_distance > max_distance1) || (vertical_angle_index > 11 && current_distance > max_distance1) )
//			if(current_distance < 0 || current_distance > MAX_DISTANCE)
//			if(current_distance < 0 || current_distance > 65535)
			{
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
//				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 2560;
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
			}
			else
			{
				if(is_start_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						current_distance = 0;
				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) - 1 || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						current_distance = 0;
				}

				if(is_end_vert_angle_even == 0)
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
						current_distance = 0;
				}
				else
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
											current_distance = 0;
				}

				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = current_distance * DIV_256;
			}
		}

//		int err = 0;
//		if(pre_azim - current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ > 0.3)
//			err = 1;

//		pre_azim = current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_;
	}
}


void SetAzimuthAndDistanceB1_AziOffset(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int hz_option,
		Azioffsets_ azioffset)
{

	int tmp_azi_cnt = (azimuth_count/3) - 1;
//	volatile int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? (lidar_state_.a2z_setting.fov_correction / tmp_azi_cnt):(176);
//	volatile int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? ((float)lidar_state_.a2z_setting.fov_correction / 120000 *176):(176);
//	volatile int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(lidar_state_.a2z_setting.fov_correction/2 + EncCtrlGetStartEncCount1()*resolution):(90000);
//	volatile int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*((float)lidar_state_.a2z_setting.fov_correction / 120000)):(90000);
	int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? (lidar_state_.a2z_setting.fov_correction):(176);
	int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*(((float)lidar_state_.a2z_setting.fov_correction*684) / 120384)):(90000);
	uint32_t same_azimuth_count = 0;
	int32_t pre_azimuth = 0;
//	int32_t top_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;
//	int32_t top_motor_init_value = 450096 - EncCtrlGetStartEncCount2() * 176;
//	int32_t top_motor_init_value = 90000 - ((EncCtrlGetStartEncCount2()-2048)) * 176;
//	int32_t top_motor_init_value = 112230 - ((EncCtrlGetStartEncCount2()-2048)) * 219;
	int32_t top_motor_init_value = base_angle - ((EncCtrlGetStartEncCount2()-2048)) * resolution;
//	int32_t top_motor_init_value = 67472;

//	int32_t bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;
//	int32_t bottom_motor_init_value = 112230 - EncCtrlGetStartEncCount1() * 219;
	int32_t bottom_motor_init_value = base_angle - EncCtrlGetStartEncCount1() * resolution;

//	int32_t bottom_motor_init_value = 67472;
//	int32_t top_motor_init_value = 67500;
//	int32_t bottom_motor_init_value = 67500;
#if defined(G32_25HZ)
	bottom_motor_init_value -= bottom_horz_offset * 234;
#endif

//	int32_t* routing_delay = lidar_state->calibration_setting_.routing_delay;
	int32_t clip_min = distance_offset;
	int32_t current_distance = 0;

	int max_distance0;
	int max_distance1;
	if(hz_option == 0)//10hz
	{
		max_distance0 = MAX_DISTANCE;
		max_distance1 = MAX_DISTANCE;
	}
	else if (hz_option == 1)//20hz
	{
		max_distance0 = MAX_DISTANCE;
		max_distance1 = MAX_DISTANCE;
	}
//	else if (hz_option == 2)//25hz or error val
	else//25hz or error val
	{
		max_distance0 = MAX_DISTANCE;
//		max_distance1 = 51200;
//		max_distance1 = 44672;	//org 179m
		max_distance1 = 51200;
//		max_distance1 = MAX_DISTANCE;
	}




	uint8_t is_start_vert_angle_even = 0;
	if(fov_test_.start_vertical_angle % 2 == 0)
		is_start_vert_angle_even = 1;
	uint8_t is_end_vert_angle_even = 0;
	if(fov_test_.end_vertical_angle % 2 == 0)
		is_end_vert_angle_even = 1;
	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
	{
//		//bottom
//		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared->block_data_bram[azimuth_index].azimuth_;
//		//top
//		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].azimuth_;

		if(pre_azimuth == block_data_shared[azimuth_index].azimuth_)
		{
			same_azimuth_count++;
		}
		else
		{
			same_azimuth_count = 0;
		}

		pre_azimuth = block_data_shared[azimuth_index].azimuth_;


		if(same_azimuth_count < 3)
		{

//			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176);
//			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 219);
			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * resolution);
//			block_data_shared[azimuth_index].azimuth_ =
//							(lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?
//									(((int) (bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_- bottom_start_enc_cnt) * 176))	*
//											((float)lidar_state_.a2z_setting.fov_correction	/ (bottom_motor_init_value * 2))) :
//									((int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176));
			current_frame_blocks[azimuth_index].azimuth_ = block_data_shared[azimuth_index].azimuth_ * 0.001;
//			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176);
//			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 219);
			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * resolution);
//			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ =
//									(lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?
//											(((int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176))	*
//													((float)lidar_state_.a2z_setting.fov_correction	/ (bottom_motor_init_value * 2))) :
//											((int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176));
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared[azimuth_index + top_bottom_offset].azimuth_ * 0.001;
		}
		else
		{

			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176 - 88);
			current_frame_blocks[azimuth_index].azimuth_ = (block_data_shared[azimuth_index].azimuth_) * 0.001;
			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176 - 88);
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = (block_data_shared[azimuth_index + top_bottom_offset].azimuth_) * 0.001;
		}



		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; vertical_angle_index++)
		{
			//bottom

//			current_distance = block_data_shared->block_data_bram[azimuth_index].distance_[vertical_angle_index] + routing_delay[vertical_angle_index] - clip_min;
			current_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index] - clip_min;
//			current_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
			if(current_distance <= 0 || current_distance > max_distance0  || (vertical_angle_index < 4 && current_distance > max_distance1) || (vertical_angle_index > 11 && current_distance > max_distance1) )
//			if(0)
//			if(current_distance < 0 || current_distance > MAX_DISTANCE)
//			if(current_distance < 0 || current_distance > 65535)
			{

//				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 2560;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
			}
			else
			{
				if(is_end_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
											vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
											current_distance = 0;

				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
												vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2)-1)
												current_distance = 0;
				}

				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = current_distance * DIV_256;

				//add azioffsets
				if(current_distance <= 128)
				{
					current_distance +=
							azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2];
				}
				else if(current_distance <= 256)
				{
					float slope = azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2]
											  - azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2];

					float per_data =  slope/128;

					current_distance+=
							azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2] + per_data * (current_distance - 128);
				}
				else if(current_distance <= 512)
				{
					float slope = azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2]
											  - azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2];

					float per_data =  slope/256;

					current_distance +=
							azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2] + per_data * (current_distance - 256);
				}
				else if(current_distance <= 768)
				{
					float slope = azioffset.AziGroup[3].Offsets[azimuth_index/3].ch[vertical_angle_index*2]
											  - azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2];

					float per_data =  slope/256;

					current_distance +=
							azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2] + per_data * (current_distance - 512);
				}
				else
				{
					current_distance +=
							azioffset.AziGroup[3].Offsets[azimuth_index/3].ch[vertical_angle_index*2];
				}

				if(current_distance < 0 || current_distance > max_distance0)
				{
					current_distance = 0;
				}
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = current_distance;

			}
			//top

			current_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;
//			current_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

			if(current_distance <= 0 || current_distance > max_distance0  || (vertical_angle_index < 4 && current_distance > max_distance1) || (vertical_angle_index > 11 && current_distance > max_distance1) )
//			if(current_distance < 0 || current_distance > MAX_DISTANCE)
//			if(current_distance < 0 || current_distance > 65535)
			{
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
//				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 2560;
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
			}
			else
			{
				if(is_start_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						current_distance = 0;
				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) - 1 || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						current_distance = 0;
				}

				if(is_end_vert_angle_even == 0)
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
						current_distance = 0;
				}
				else
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
											current_distance = 0;
				}

				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = current_distance * DIV_256;

				//add top azioffset
				if(current_distance <= 128)
				{
					current_distance +=
							azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];
				}
				else if(current_distance <= 256)
				{
					float slope = azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1]
											  - azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];

					float per_data =  slope/128;

					current_distance +=
							azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1] + per_data * (current_distance - 128);
				}
				else if(current_distance <= 512)	//wrong
				{
					float slope = azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1]
											  - azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];

					float per_data =  slope/256;

					current_distance +=
							azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1] + per_data * (current_distance - 256);
				}
				else if(current_distance <= 768)
				{
					float slope = azioffset.AziGroup[3].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1]
											  - azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];

					float per_data =  slope/256;

					current_distance +=
							azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1] + per_data * (current_distance - 512);
				}
				else
				{
					current_distance +=
							azioffset.AziGroup[3].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];
				}
				if(current_distance < 0 || current_distance > max_distance0)
				{
					current_distance = 0;
				}
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = current_distance;

			}
		}

//		int err = 0;
//		if(pre_azim - current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ > 0.3)
//			err = 1;

//		pre_azim = current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_;
	}
}



void SetAzimuthCal(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, AzimuthCal azi_cal)
{
//	for(int azimuth_index = 0 ; azimuth_index < azimuth_count + top_bottom_offset; azimuth_index+=3)
//	{
//		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; vertical_angle_index++)
//		{
////			block_data_shared[azimuth_index].azimuth_
//			block_data_shared[azimuth_index].distance_[vertical_angle_index].distance
//			+= azi_cal.AziOffsets_[block_data_shared[azimuth_index].azimuth_].ch_datas[vertical_angle_index];
//		}
//	}
}

void SetAzimuthCal2(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, AzimuthCal azi_cal)
{
//	for(int azimuth_index = 0 ; azimuth_index < azimuth_count + top_bottom_offset; azimuth_index+=3)
//	{
//		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; vertical_angle_index++)
//		{
////			block_data_shared[azimuth_index].azimuth_
//			block_data_shared[azimuth_index].distance_[vertical_angle_index].distance
//			+= azi_cal.AziOffsets_[block_data_shared[azimuth_index].azimuth_].ch_datas[vertical_angle_index];
//		}
//	}

	for(size_t azi_cnt_idx = 0; azi_cnt_idx< azi_cal.azimuth_calibration_setting.calibration_info_count; azi_cnt_idx++)
	{
		uint16_t azimuth_idx = azi_cal.azimuth_calibration_info[azi_cnt_idx].col;
		uint8_t channel_idx = azi_cal.azimuth_calibration_info[azi_cnt_idx].row;
		float dist_offset = azi_cal.azimuth_calibration_info[azi_cnt_idx].offset;

		int dist_stop1 = (int)block_data_shared[azimuth_idx].distance_[channel_idx].distance;
		int dist_stop2 = (int)block_data_shared[azimuth_idx+1].distance_[channel_idx].distance;
		int dist_stop3 = (int)block_data_shared[azimuth_idx+2].distance_[channel_idx].distance;
		if(dist_stop1 != 0 && (dist_stop1 + dist_offset < 0xFFFF) && ((dist_stop1 + dist_offset >0 )))
		{
			block_data_shared[azimuth_idx].distance_[channel_idx].distance += dist_offset;
		}
		if(dist_stop2 != 0 && (dist_stop2 + dist_offset < 0xFFFF) && ((dist_stop2 + dist_offset >0 )))
		{
			block_data_shared[azimuth_idx].distance_[channel_idx].distance += dist_offset;
		}
		if(dist_stop3 != 0 && (dist_stop3 + dist_offset < 0xFFFF) && ((dist_stop3 + dist_offset >0 )))
		{
			block_data_shared[azimuth_idx].distance_[channel_idx].distance += dist_offset;
		}
//
//		if(block_data_shared[azimuth_idx+1].distance_[channel_idx].distance!=0)
//		{
////			block_data_shared[azimuth_idx+1].distance_[channel_idx].distance += dist_offset;
//		}
//		if(block_data_shared[azimuth_idx+2].distance_[channel_idx].distance!=0)
//		{
////			block_data_shared[azimuth_idx+2].distance_[channel_idx].distance += dist_offset;
//		}

	}
}


void SetTemperatureAdjOffset(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, int adj_offset)
{
	for(int azimuth_index = 0 ; azimuth_index < azimuth_count + top_bottom_offset; ++azimuth_index)
	{
		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; vertical_angle_index++)
		{
			if(block_data_shared[azimuth_index].distance_[vertical_angle_index].distance > 40)
			{
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance += adj_offset;
			}
		}
	}
}



void SetAzimuthAndDistance(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int bottom_horz_offset, int distance_offset)
{
	uint32_t same_azimuth_count = 0;
	int32_t pre_azimuth = 0;
	int32_t top_motor_init_value = 67500;
	int32_t bottom_motor_init_value = 67500;
#if defined(G32_25HZ)
	bottom_motor_init_value -= bottom_horz_offset * 234;
#endif
	uint32_t bottom_motor_offset = 103;
	uint32_t top_motor_offset = 1655;
//	int32_t* routing_delay = lidar_state->calibration_setting_.routing_delay;
	int32_t clip_min = distance_offset;
	int32_t current_distance = 0;
	float pre_azim = 67.5;
	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index++)
	{
//		//bottom
//		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared->block_data_bram[azimuth_index].azimuth_;
//		//top
//		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].azimuth_;

		if(pre_azimuth == block_data_shared[azimuth_index].azimuth_)
		{
			same_azimuth_count++;
		}
		else
		{
			same_azimuth_count = 0;
		}

		pre_azimuth = block_data_shared[azimuth_index].azimuth_;


		if(same_azimuth_count < 3)
		{

			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - BOTTOM_MOTOR_OFFSET) * 234);
			current_frame_blocks[azimuth_index].azimuth_ = block_data_shared[azimuth_index].azimuth_ * 0.001;
			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - TOP_MOTOR_OFFSET) * 234);
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared[azimuth_index + top_bottom_offset].azimuth_ * 0.001;
		}
		else
		{

			block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - BOTTOM_MOTOR_OFFSET) * 234 - 117);
			current_frame_blocks[azimuth_index].azimuth_ = (block_data_shared[azimuth_index].azimuth_) * 0.001;
			block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - TOP_MOTOR_OFFSET) * 234 - 117);
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = (block_data_shared[azimuth_index + top_bottom_offset].azimuth_) * 0.001;
		}


		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; vertical_angle_index++)
		{
			//bottom

//			current_distance = block_data_shared->block_data_bram[azimuth_index].distance_[vertical_angle_index] + routing_delay[vertical_angle_index] - clip_min;
			current_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index] - clip_min;
			if(current_distance < 0 || current_distance > MAX_DISTANCE)
			{

				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
			}
			else
			{

				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = current_distance * DIV_256;
			}
			//top

			current_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;

			if(current_distance < 0 || current_distance > MAX_DISTANCE)
			{

				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
			}
			else
			{

				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = current_distance * DIV_256;
			}
		}

//		int err = 0;
//		if(pre_azim - current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ > 0.3)
//			err = 1;

//		pre_azim = current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_;
	}
}

void SetAzimuthAndDistance32(ConvertedDataBlock32* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int bottom_horz_offset, int distance_offset)
{
	uint32_t same_azimuth_count = 0;
	int32_t pre_azimuth = 0;
	int32_t motor_init_value = 67500;
	uint32_t bottom_motor_offset = 103;
	uint32_t top_motor_offset = 1655;
//	int32_t* routing_delay = lidar_state->calibration_setting_.routing_delay;
	int32_t clip_min = distance_offset;
	int32_t current_distance = 0;
	float pre_azim = 67.5;
	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index++)
	{
//		//bottom
//		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared->block_data_bram[azimuth_index].azimuth_;
//		//top
//		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].azimuth_;

		if(pre_azimuth == block_data_shared[azimuth_index].azimuth_)
		{
			same_azimuth_count++;
		}
		else
		{
			same_azimuth_count = 0;
		}

		pre_azimuth = block_data_shared[azimuth_index].azimuth_;

#if defined(G32_10HZ)
//		block_data_shared->block_data_bram[azimuth_index].Azimuth = (int)(motor_init_value - (block_data_shared->block_data_bram[azimuth_index].Azimuth - BOTTOM_MOTOR_OFFSET) * 234 - 117);
		current_frame_blocks[azimuth_index].azimuth_ = (block_data_shared[azimuth_index].azimuth_) * 0.001;
//		block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].Azimuth = (int)(motor_init_value - (block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].Azimuth - TOP_MOTOR_OFFSET) * 234 - 117);
//		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = (block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].Azimuth) * 0.001;
#elif defined(G32_25HZ)
		if(same_azimuth_count < 3)
		{

			block_data_shared->block_data_bram[azimuth_index].Azimuth = (int)(motor_init_value - (block_data_shared->block_data_bram[azimuth_index].Azimuth - BOTTOM_MOTOR_OFFSET) * 234);
			current_frame_blocks[azimuth_index].azimuth_ = block_data_shared->block_data_bram[azimuth_index].Azimuth * 0.001;
			block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].Azimuth = (int)(motor_init_value - (block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].Azimuth - TOP_MOTOR_OFFSET) * 234);
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].Azimuth * 0.001;
		}
		else
		{

			block_data_shared->block_data_bram[azimuth_index].Azimuth = (int)(motor_init_value - (block_data_shared->block_data_bram[azimuth_index].Azimuth - BOTTOM_MOTOR_OFFSET) * 234 - 117);
			current_frame_blocks[azimuth_index].azimuth_ = (block_data_shared->block_data_bram[azimuth_index].Azimuth) * 0.001;
			block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].Azimuth = (int)(motor_init_value - (block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].Azimuth - TOP_MOTOR_OFFSET) * 234 - 117);
			current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = (block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].Azimuth) * 0.001;
		}
#endif

		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; vertical_angle_index++)
		{

			int bottom_vert_ind = 2*vertical_angle_index;
			int top_vert_ind = 2*vertical_angle_index + 1;

			//bottom

//			current_distance = block_data_shared->block_data_bram[azimuth_index].distance_[vertical_angle_index] + routing_delay[vertical_angle_index] - clip_min;
			current_distance = block_data_shared[azimuth_index].distance_[bottom_vert_ind].distance + routing_delay[vertical_angle_index] - clip_min;
			if(current_distance < 0 || current_distance > MAX_DISTANCE)
			{

//				block_data_shared->block_data_bram[azimuth_index].data_points[vertical_angle_index].distance = 0;
//				block_data_shared->block_data_bram[azimuth_index].data_points[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[bottom_vert_ind] = 0;
			}
			else
			{

//				block_data_shared->block_data_bram[azimuth_index].data_points[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index].distance_[bottom_vert_ind] = current_distance * DIV_256;
			}
			//top

			current_distance = block_data_shared[azimuth_index].distance_[top_vert_ind].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;

			if(current_distance < 0 || current_distance > MAX_DISTANCE)
			{

//				block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].data_points[vertical_angle_index].distance = 0;
//				block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].data_points[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[top_vert_ind] = 0;
			}
			else
			{

//				block_data_shared->block_data_bram[azimuth_index + top_bottom_offset].data_points[vertical_angle_index].distance = current_distance;
				current_frame_blocks[azimuth_index].distance_[top_vert_ind] = current_distance * DIV_256;
			}
		}

//		int err = 0;
//		if(pre_azim - current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ > 0.3)
//			err = 1;

//		pre_azim = current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_;
	}
}

void SetBottomHorizontalOffset(RawDataBlock *block_data_shared, RawDataBlock *temp_block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, int bottom_horz_offset)
{
	int32_t current_distance = 0;
	uint16_t current_pulse_width = 0;

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
	{
		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; ++vertical_angle_index)
		{
			//bottom

			//			current_distance = block_data_shared->block_data_bram[azimuth_index].distance_[vertical_angle_index] + routing_delay[vertical_angle_index] - clip_min;
			if(azimuth_index + bottom_horz_offset < azimuth_count)
			{
				current_distance = temp_block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
				current_pulse_width = temp_block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width;
				if(current_distance < 0 || current_distance > MAX_DISTANCE)
				{
					block_data_shared[azimuth_index + bottom_horz_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + bottom_horz_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
				else
				{
					block_data_shared[azimuth_index + bottom_horz_offset].distance_[vertical_angle_index].distance = current_distance;
					block_data_shared[azimuth_index + bottom_horz_offset].distance_[vertical_angle_index].pulse_width = current_pulse_width;
				}
			}
		}
	}

	if(bottom_horz_offset > 0 && bottom_horz_offset < 300)
	{
		for(int azimuth_index = 0 ; azimuth_index < bottom_horz_offset ; ++azimuth_index)
			for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; ++vertical_angle_index)			{
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
			}
	}
	else if(bottom_horz_offset < 0 && bottom_horz_offset > -300)
	{
//		for(int azimuth_index = azimuth_count - 1 ; azimuth_index >= azimuth_count - bottom_horz_offset ; azimuth_index--)
		for(int azimuth_index = azimuth_count - 1 ; azimuth_index >= azimuth_count + bottom_horz_offset ; --azimuth_index)
			for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; ++vertical_angle_index)
			{
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
			}
	}
}
