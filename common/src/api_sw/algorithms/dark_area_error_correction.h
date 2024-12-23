/*
 * dark_area_error_correction.h
 *
 *  Created on: 2024. 6. 14.
 *      Author: wonju
 */

#ifndef SRC_API_SW_ALGORITHMS_DARK_AREA_ERROR_CORRECTION_H_
#define SRC_API_SW_ALGORITHMS_DARK_AREA_ERROR_CORRECTION_H_

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../../definitions/definitions.h"
#include "../models/fov_data_set.h"
#include "./dark_area_error_correction_setting.h"

static uint8_t target_channel_index_vec[32] = {0,};
static uint8_t target_channel_index_vec_count = 0;

void RunErrorCorrectionByDistance2(RawDataBlock* current_frame_data, u32 fov_data_block_count, u32 bottom_data_block_count, struct DarkAreaErrorCorrectionSettingState* dark_area_error_correction_setting)
{

	if(dark_area_error_correction_setting->darkarea_setting_.is_enable_dark_area_crror_correction_ == true)
	{
		double target_channel_distance_ratio[3][32] = {0,};
		double corrected_distance_ratio[3][32] = {0,};

		for(size_t i = 0; i < 32; i++)
		{
			target_channel_distance_ratio[0][i] = (double)dark_area_error_correction_setting->darkarea_setting_.dark_area_error_cor_param1_[i] / (dark_area_error_correction_setting->darkarea_setting_.dark_area_error_cor_param1_[i] + dark_area_error_correction_setting->darkarea_setting_.dark_area_error_cor_param2_[i]);
			corrected_distance_ratio[0][i] = (double)dark_area_error_correction_setting->darkarea_setting_.dark_area_error_cor_param2_[i] / (dark_area_error_correction_setting->darkarea_setting_.dark_area_error_cor_param1_[i] + dark_area_error_correction_setting->darkarea_setting_.dark_area_error_cor_param2_[i]);

			target_channel_distance_ratio[1][i] = (double)dark_area_error_correction_setting->darkarea_setting_2.dark_area_error_cor_param1_[i] / (dark_area_error_correction_setting->darkarea_setting_2.dark_area_error_cor_param1_[i] + dark_area_error_correction_setting->darkarea_setting_2.dark_area_error_cor_param2_[i]);
			corrected_distance_ratio[1][i] = (double)dark_area_error_correction_setting->darkarea_setting_2.dark_area_error_cor_param2_[i] / (dark_area_error_correction_setting->darkarea_setting_2.dark_area_error_cor_param1_[i] + dark_area_error_correction_setting->darkarea_setting_2.dark_area_error_cor_param2_[i]);

			target_channel_distance_ratio[2][i] = (double)dark_area_error_correction_setting->darkarea_setting_3.dark_area_error_cor_param1_[i] / (dark_area_error_correction_setting->darkarea_setting_3.dark_area_error_cor_param1_[i] + dark_area_error_correction_setting->darkarea_setting_3.dark_area_error_cor_param2_[i]);
			corrected_distance_ratio[2][i] = (double)dark_area_error_correction_setting->darkarea_setting_3.dark_area_error_cor_param2_[i] / (dark_area_error_correction_setting->darkarea_setting_3.dark_area_error_cor_param1_[i] + dark_area_error_correction_setting->darkarea_setting_3.dark_area_error_cor_param2_[i]);
		}


		double dark_area_error_cor_param3 = dark_area_error_correction_setting->darkarea_setting_.dark_area_error_cor_param3_ * 256.0;
		double dark_area_error_cor_param4 = dark_area_error_correction_setting->darkarea_setting_.dark_area_error_cor_param4_ * 256.0;


		// Group1
		for(int azimuth_idx = 3*dark_area_error_correction_setting->darkarea_setting_.azimuth_range[0]; azimuth_idx < 3*dark_area_error_correction_setting->darkarea_setting_.azimuth_range[1]; ++azimuth_idx)
		{
			for(int ch_idx =0; ch_idx < 32; ch_idx++)
			{
				if(dark_area_error_correction_setting->darkarea_setting_.dark_area_search_channel_[ch_idx][0] == 0) continue;

				uint8_t isTop = ch_idx % 2;
				uint8_t ch = ch_idx >> 1;

				double distance_cor = 0.0;
				int32_t data_count = 0;
				size_t arr_azimuth_idx = isTop*(fov_data_block_count/2) + azimuth_idx;
				double target_channel_distance = current_frame_data[arr_azimuth_idx].distance_[ch].distance;

				for(int search_idx = 0 ; search_idx < 16; search_idx++)
				{
					uint8_t search_ch = dark_area_error_correction_setting->darkarea_setting_.dark_area_search_channel_[ch_idx][search_idx];
					if(search_ch == 0) break;
					search_ch -= 1;
					uint8_t searchIsTop = search_ch%2;
					uint8_t searchCh = search_ch >> 1;
					size_t search_arr_azimuth_idx = searchIsTop*(fov_data_block_count/2) + azimuth_idx;
					double dark_area_error_cor_dist_limit = (isTop == 1)? dark_area_error_cor_param3:dark_area_error_cor_param4;
					if(fabs(target_channel_distance - current_frame_data[search_arr_azimuth_idx].distance_[searchCh].distance) < dark_area_error_cor_dist_limit)
					{
						distance_cor += current_frame_data[search_arr_azimuth_idx].distance_[searchCh].distance;
						++data_count;
					}


				}
				distance_cor /= data_count;

				if(data_count > 0)
				{
					double distance_final = 0.0;


					distance_final = (target_channel_distance * target_channel_distance_ratio[0][ch_idx])
							+ (distance_cor * corrected_distance_ratio[0][ch_idx]);


					current_frame_data[arr_azimuth_idx].distance_[ch].distance = distance_final;
//					current_frame_data[arr_azimuth_idx].distance_[ch].pulse_width = 0;
				}
			}
		}
		// Group2
		for(int azimuth_idx = 3*dark_area_error_correction_setting->darkarea_setting_2.azimuth_range[0]; azimuth_idx < 3*dark_area_error_correction_setting->darkarea_setting_2.azimuth_range[1]; ++azimuth_idx)
		{
			for(int ch_idx =0; ch_idx < 32; ch_idx++)
			{
				if(dark_area_error_correction_setting->darkarea_setting_2.dark_area_search_channel_[ch_idx][0] == 0) continue;

				uint8_t isTop = ch_idx % 2;
				uint8_t ch = ch_idx >> 1;

				double distance_cor = 0.0;
				int32_t data_count = 0;
				size_t arr_azimuth_idx = isTop*(fov_data_block_count/2) + azimuth_idx;
				double target_channel_distance = current_frame_data[arr_azimuth_idx].distance_[ch].distance;

				for(int search_idx = 0 ; search_idx < 16; search_idx++)
				{
					uint8_t search_ch = dark_area_error_correction_setting->darkarea_setting_2.dark_area_search_channel_[ch_idx][search_idx];
					if(search_ch == 0) break;
					search_ch -= 1;
					uint8_t searchIsTop = search_ch%2;
					uint8_t searchCh = search_ch >> 1;
					size_t search_arr_azimuth_idx = searchIsTop*(fov_data_block_count/2) + azimuth_idx;
					double dark_area_error_cor_dist_limit = (isTop == 1)? dark_area_error_cor_param3:dark_area_error_cor_param4;
					if(fabs(target_channel_distance - current_frame_data[search_arr_azimuth_idx].distance_[searchCh].distance) < dark_area_error_cor_dist_limit)
					{
						distance_cor += current_frame_data[search_arr_azimuth_idx].distance_[searchCh].distance;
						++data_count;
					}


				}
				distance_cor /= data_count;

				if(data_count > 0)
				{
					double distance_final = 0.0;


					distance_final = (target_channel_distance * target_channel_distance_ratio[1][ch_idx])
							+ (distance_cor * corrected_distance_ratio[1][ch_idx]);


					current_frame_data[arr_azimuth_idx].distance_[ch].distance = distance_final;
//					current_frame_data[arr_azimuth_idx].distance_[ch].pulse_width = 100;
				}
			}
		}
		// Group3
		for(int azimuth_idx = 3*dark_area_error_correction_setting->darkarea_setting_3.azimuth_range[0]; azimuth_idx < 3*dark_area_error_correction_setting->darkarea_setting_3.azimuth_range[1]; ++azimuth_idx)
		{
			for(int ch_idx =0; ch_idx < 32; ch_idx++)
			{
				if(dark_area_error_correction_setting->darkarea_setting_3.dark_area_search_channel_[ch_idx][0] == 0) continue;

				uint8_t isTop = ch_idx % 2;
				uint8_t ch = ch_idx >> 1;

				double distance_cor = 0.0;
				int32_t data_count = 0;
				size_t arr_azimuth_idx = isTop*(fov_data_block_count/2) + azimuth_idx;
				double target_channel_distance = current_frame_data[arr_azimuth_idx].distance_[ch].distance;

				for(int search_idx = 0 ; search_idx < 16; search_idx++)
				{
					uint8_t search_ch = dark_area_error_correction_setting->darkarea_setting_3.dark_area_search_channel_[ch_idx][search_idx];
					if(search_ch == 0) break;
					search_ch -= 1;
					uint8_t searchIsTop = search_ch%2;
					uint8_t searchCh = search_ch >> 1;
					size_t search_arr_azimuth_idx = searchIsTop*(fov_data_block_count/2) + azimuth_idx;
					double dark_area_error_cor_dist_limit = (isTop == 1)? dark_area_error_cor_param3:dark_area_error_cor_param4;
					if(fabs(target_channel_distance - current_frame_data[search_arr_azimuth_idx].distance_[searchCh].distance) < dark_area_error_cor_dist_limit)
					{
						distance_cor += current_frame_data[search_arr_azimuth_idx].distance_[searchCh].distance;
						++data_count;
					}


				}
				distance_cor /= data_count;

				if(data_count > 0)
				{
					double distance_final = 0.0;


					distance_final = (target_channel_distance * target_channel_distance_ratio[2][ch_idx])
							+ (distance_cor * corrected_distance_ratio[2][ch_idx]);


					current_frame_data[arr_azimuth_idx].distance_[ch].distance = distance_final;
//					current_frame_data[arr_azimuth_idx].distance_[ch].pulse_width = 255;
				}
			}
		}
	}
}

void RunErrorCorrectionByDistance(RawDataBlock* current_frame_data, u32 fov_data_block_count, u32 bottom_data_block_count, struct DarkAreaErrorCorrectionSetting* dark_area_error_correction_setting)
{
	if(dark_area_error_correction_setting->is_enable_dark_area_crror_correction_ == true)
	{
		double target_channel_distance_ratio = dark_area_error_correction_setting->dark_area_error_cor_param1_ / (dark_area_error_correction_setting->dark_area_error_cor_param1_ + dark_area_error_correction_setting->dark_area_error_cor_param2_);
		double corrected_distance_ratio = dark_area_error_correction_setting->dark_area_error_cor_param2_ / (dark_area_error_correction_setting->dark_area_error_cor_param1_ + dark_area_error_correction_setting->dark_area_error_cor_param2_);
		double dark_area_error_cor_param3 = dark_area_error_correction_setting->dark_area_error_cor_param3_ * 256.0;
		double dark_area_error_cor_param4 = dark_area_error_correction_setting->dark_area_error_cor_param4_ * 256.0;
		if(dark_area_error_correction_setting->is_dark_area_channel_changed_ == true)
		{
			memset(target_channel_index_vec, 0x00, sizeof(target_channel_index_vec) / sizeof(uint8_t));
			target_channel_index_vec_count = 0;
			for(size_t channel_index = 0 ; channel_index < dark_area_error_correction_setting->dark_area_channel_count_ ; channel_index++)
			{
				uint8_t converted_channel = 1;
				if(dark_area_error_correction_setting->dark_area_channel_[channel_index] % 2 == 0)
				{
					converted_channel = 16 + dark_area_error_correction_setting->dark_area_channel_[channel_index] / 2;
				}
				else
				{
					converted_channel = floor(dark_area_error_correction_setting->dark_area_channel_[channel_index] / 2) + 1;
				}


				target_channel_index_vec[channel_index] = converted_channel - 1;
				target_channel_index_vec_count++;
			}

			dark_area_error_correction_setting->is_dark_area_channel_changed_ = false;
		}

		for(int azimuth_index = 0 ; azimuth_index < fov_data_block_count ; ++azimuth_index)
		{
			int32_t channelIndexOffset = 0;
			if(azimuth_index >= bottom_data_block_count)
				channelIndexOffset += 16;

			for(int channel_index = 0 ; channel_index < 16 ; ++channel_index)
			{
				int channel_num = channelIndexOffset + channel_index;

				uint8_t is_target_channel_index = false;
				for(size_t i = 0 ; i < target_channel_index_vec_count ; ++i)
				{
					if (channel_num == target_channel_index_vec[i])
					{
						is_target_channel_index = true;
						break;
					}
				}

				if(is_target_channel_index)
				{
					double distance_cor = 0.0;
					int32_t data_count = 0;
					double target_channel_distance = current_frame_data[azimuth_index].distance_[channel_index].distance;
					if(channel_index > 0 && channel_index < 15)
					{
						if(dark_area_error_correction_setting->is_enable_zero_dist_cor_ == false || (dark_area_error_correction_setting->is_enable_zero_dist_cor_ == true && target_channel_distance != 0))
						{
							if(fabs(target_channel_distance - current_frame_data[azimuth_index].distance_[channel_index - 1].distance) < dark_area_error_cor_param3)
							{
								distance_cor += current_frame_data[azimuth_index].distance_[channel_index - 1].distance;
								++data_count;
							}

							if(fabs(target_channel_distance - current_frame_data[azimuth_index].distance_[channel_index + 1].distance) < dark_area_error_cor_param3)
							{
								distance_cor += current_frame_data[azimuth_index].distance_[channel_index + 1].distance;
								++data_count;
							}
							distance_cor /= data_count;
						}
						else if(dark_area_error_correction_setting->is_enable_zero_dist_cor_ == true && target_channel_distance == 0)
						{
							if((current_frame_data[azimuth_index].distance_[channel_index + 1].distance != 0 && current_frame_data[azimuth_index].distance_[channel_index - 1].distance != 0) && (abs((double)current_frame_data[azimuth_index].distance_[channel_index + 1].distance - current_frame_data[azimuth_index].distance_[channel_index - 1].distance) < dark_area_error_cor_param4))
							{
								distance_cor += current_frame_data[azimuth_index].distance_[channel_index - 1].distance;
								++data_count;
								distance_cor += current_frame_data[azimuth_index].distance_[channel_index + 1].distance;
								++data_count;

								distance_cor /= data_count;
							}
						}
					}
					else if (channel_index == 0)
					{
						if(dark_area_error_correction_setting->is_enable_zero_dist_cor_ == false || (dark_area_error_correction_setting->is_enable_zero_dist_cor_ == true && target_channel_distance != 0))
						{
							if(fabs(target_channel_distance - current_frame_data[azimuth_index].distance_[channel_index + 1].distance) < dark_area_error_cor_param3)
							{
								distance_cor += current_frame_data[azimuth_index].distance_[channel_index + 1].distance;
								++data_count;
							}
						}
						else if(dark_area_error_correction_setting->is_enable_zero_dist_cor_ == true && target_channel_distance == 0)
						{
							if(fabs(target_channel_distance - current_frame_data[azimuth_index].distance_[channel_index + 1].distance) < dark_area_error_cor_param4)
							{
								distance_cor += current_frame_data[azimuth_index].distance_[channel_index + 1].distance;
								++data_count;
							}
						}
					}
					else if(channel_index == 15)
					{
						if(dark_area_error_correction_setting->is_enable_zero_dist_cor_ == false || (dark_area_error_correction_setting->is_enable_zero_dist_cor_ == true && target_channel_distance != 0))
						{
							if(fabs(target_channel_distance - current_frame_data[azimuth_index].distance_[channel_index - 1].distance) < dark_area_error_cor_param3)
							{
								distance_cor += current_frame_data[azimuth_index].distance_[channel_index - 1].distance;
								++data_count;
							}
						}
						else if(dark_area_error_correction_setting->is_enable_zero_dist_cor_ == true && target_channel_distance == 0)
						{
							if(fabs(target_channel_distance - current_frame_data[azimuth_index].distance_[channel_index - 1].distance) < dark_area_error_cor_param4)
							{
								distance_cor += current_frame_data[azimuth_index].distance_[channel_index - 1].distance;
								++data_count;
							}
						}
					}

					if(data_count > 0)
					{
						double distance_final = 0.0;

						if(dark_area_error_correction_setting->is_enable_zero_dist_cor_ == true && target_channel_distance == 0)
						{
							distance_final = distance_cor;
						}
						else
						{
							distance_final = (target_channel_distance * target_channel_distance_ratio)
									+ (distance_cor * corrected_distance_ratio);
						}

						current_frame_data[azimuth_index].distance_[channel_index].distance = distance_final;
					}
				}
			}

		}
	}
}

#endif /* SRC_API_SW_ALGORITHMS_DARK_AREA_ERROR_CORRECTION_H_ */
