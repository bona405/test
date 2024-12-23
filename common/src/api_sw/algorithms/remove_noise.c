/*
 * remove_noise.c
 *
 *  Created on: 2022. 8. 4.
 *      Author: Wonju
 */

#include "remove_noise.h"
size_t neighbor_point_index_size = 0;
uint8_t check_azimuth_first = 0;
uint8_t set_noise_point_intensity = 0;
uint8_t ignore_ground_point = 0;
int remove_noise_min_distance_ = 300;
int close_noise_neighbor_size_ = 2;
unsigned int distance_limit_ = 2;
void InitRemoveNoise()
{
	is_score_map_init = 0;
	double angle = 10.0f;
	int num_of_channel = 32;
	double top_bottom_offset = angle / num_of_channel;
	double angle_start = -angle / 2 + top_bottom_offset / 2;
	for(size_t i = 0 ; i < num_of_channel / 2 ; i++)
	{
		vertical_angle_array_bottom[i] = angle_start + top_bottom_offset * 2 * i;
	}

	angle_start = -angle / 2 + top_bottom_offset / 2 + top_bottom_offset;

	for(size_t i = 0 ; i < num_of_channel / 2 ; i++)
	{
		vertical_angle_array_top[i] = angle_start + top_bottom_offset * 2 * i;
	}
//	vector_init(&lidar_point_cloud_data_score_);
//	VectorResize(&lidar_point_cloud_data_score_, FRAME_BLOCK_SIZE);
}

void InitScoreMap()
{
	memset(&lidar_point_cloud_data_score_, 0x00, MAX_POINT_COUNT * ONE_SIDE_CHANNEL_COUNT * sizeof(RemoveNoiseScore));
	memset(&is_noise_score_calculated, 0x00, MAX_POINT_COUNT * ONE_SIDE_CHANNEL_COUNT);

	is_score_map_init = 1;
}

void GetNeighborPointIndex(TupleII neighbor_point_index[MAX_NEIGHBOR_POINT_SIZE], size_t* frame_neighbor_point_index_size, TupleII target_point_index, int azimuth_index_offset, int dist_neighbor, int top_bottom_index, uint8_t is_add_myself, u32 fov_data_block_count)
{
	u32 total_azimuth_count = fov_data_block_count / 2;
	if(dist_neighbor == 0)
	{

		if(is_add_myself == true)
		{
			TupleII neighbor;

			int azimuth_index = target_point_index.first - azimuth_index_offset;
			if(azimuth_index < 0 || azimuth_index >= fov_data_block_count)
				azimuth_index = -1;

			neighbor.first = azimuth_index;
			neighbor.second = target_point_index.second;
			neighbor_point_index[*frame_neighbor_point_index_size] = neighbor;
			(*frame_neighbor_point_index_size)++;
		}

		//for (int i = 0; i < 3; ++i)
		//{
		//	if (i == target_point_index.stop_index)
		//		continue;

		//	PointIndexInfo neighbor;
		//	neighbor.lidar_index = target_point_index.lidar_index;
		//	neighbor.top_bottom_index = target_point_index.top_bottom_index;
		//	neighbor.channel_index = target_point_index.channel_index;

		//	int azimuth_index = target_point_index.azimuth_index - azimuth_index_offset;
		//	if (azimuth_index < 0 || azimuth_index >= 720)
		//		azimuth_index = -1;

		//	neighbor.azimuth_index = azimuth_index;
		//	neighbor.stop_index = i;
		//	neighbor_point_index.push_back(neighbor);
		//}
		return;
	}

	GetNeighborPointIndex(neighbor_point_index, frame_neighbor_point_index_size, target_point_index, azimuth_index_offset, dist_neighbor - 1, top_bottom_index, 1, fov_data_block_count);

	for(int j = dist_neighbor * -1 ; j <= dist_neighbor ; ++j)
	{
		for(int k = dist_neighbor * -1 ; k <= dist_neighbor ; ++k)
		{
			if(abs(j) + abs(k) == dist_neighbor)
			{
				TupleII neighbor;

				int azimuth_index = target_point_index.first + j - azimuth_index_offset;
				if(azimuth_index < 0 || azimuth_index >= fov_data_block_count)
					azimuth_index = -1;

				neighbor.first = azimuth_index;

				int channel_offset;
				if(top_bottom_index == 0)
				{
					if(k > 0)
						channel_offset = k * 0.5;
					else
						channel_offset = (k - 1) * 0.5;
				}
				else
				{
					if(k > 0)
						channel_offset = (k + 1) * 0.5;
					else
						channel_offset = k * 0.5;
				}

				int channel_index = target_point_index.second + channel_offset;

				if(channel_index < 0 || channel_index >= 16)
					channel_index = -1;

				neighbor.second = channel_index;

//				if(abs(k) % 2 == 1)
//					neighbor.top_bottom_index = (target_point_index.top_bottom_index == 0) ? 1 : 0;
//				else
//					neighbor.top_bottom_index = target_point_index.top_bottom_index;

				neighbor_point_index[*frame_neighbor_point_index_size] = neighbor;
				(*frame_neighbor_point_index_size)++;
			}
		}
	}
}

void SetNeighborPointIndex(u32 fov_data_block_count)
{
	u32 total_azimuth_count = fov_data_block_count / 2;
	int top_bottom_index = 0;
	int azimuth_index_offset = 0;
	TupleII* frame_neighbor_point_index_p;
	for(size_t azimuth_index = 0 ; azimuth_index < fov_data_block_count ; azimuth_index++)
	{
		if(azimuth_index >= total_azimuth_count)
		{
			top_bottom_index = 1;
			azimuth_index_offset = 0;
		}
		else
		{
			top_bottom_index = 0;
			azimuth_index_offset = 0;
		}
		for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; channel_index++)
		{

			TupleII target_point_index;
			target_point_index.first = azimuth_index;
			target_point_index.second = channel_index;

			//add myself
			neighbor_point_index_size = 0;
			int azimuth_index = target_point_index.first - azimuth_index_offset;
			if(azimuth_index < 0 || azimuth_index >= fov_data_block_count)
				azimuth_index = -1;

			frame_neighbor_point_index[neighbor_point_index_size].first = azimuth_index;
			frame_neighbor_point_index[neighbor_point_index_size].second = target_point_index.second;
			neighbor_point_index_size++;

			//add others
			for(int dist_neighbor = 1 ; dist_neighbor <= 3 ; ++dist_neighbor)
			{
				for(int j = dist_neighbor * -1 ; j <= dist_neighbor ; ++j)
				{
					for(int k = dist_neighbor * -1 ; k <= dist_neighbor ; ++k)
					{
						if(abs(j) + abs(k) == dist_neighbor)
						{
							int azimuth_index = target_point_index.first + 3 * j - azimuth_index_offset;
							if(top_bottom_index == 0)
							{
								if(azimuth_index < 0 || azimuth_index >= fov_data_block_count)
								{
									azimuth_index = -1;
								}
								else
								{
									if(abs(k) & 1)
										azimuth_index += total_azimuth_count;
								}
							}
							else
							{
								if(azimuth_index < 0 || azimuth_index >= fov_data_block_count)
								{
										azimuth_index = -1;
								}
								else
								{
									if(abs(k) & 1)
										azimuth_index -= total_azimuth_count;
								}
							}

							if(azimuth_index < 0 || azimuth_index >= fov_data_block_count)
								azimuth_index = -1;

							frame_neighbor_point_index[neighbor_point_index_size].first = azimuth_index;

							int channel_offset;
							if(top_bottom_index == 0)
							{
								if(k > 0)
									channel_offset = k * 0.5;
								else
									channel_offset = (k - 1) * 0.5;
							}
							else
							{
								if(k > 0)
									channel_offset = (k + 1) * 0.5;
								else
									channel_offset = k * 0.5;
							}

							int channel_index = target_point_index.second + channel_offset;

							if(channel_index < 0 || channel_index >= 16)
								channel_index = -1;

							frame_neighbor_point_index[neighbor_point_index_size].second = channel_index;

							//				if(abs(k) % 2 == 1)
							//					neighbor.top_bottom_index = (target_point_index.top_bottom_index == 0) ? 1 : 0;
							//				else
							//					neighbor.top_bottom_index = target_point_index.top_bottom_index;

							neighbor_point_index_size++;
						}
					}
				}
			}
			frame_neighbor_point_index_p = frame_neighbor_point_index;
			memcpy(lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_, frame_neighbor_point_index_p, MAX_NEIGHBOR_POINT_SIZE * sizeof(TupleII));
		}
	}


}

#ifdef _DEBUG
int NeighborExistCheck(TupleII* frame_neighbor_point_index, TupleII target_point_index, double cur_frame_target_point_distance, int bottom_horizontal_offset, ConvertedDataBlock* last_frame_blocks, int top_bottom_ind, u32 fov_data_block_count)
#else
inline int NeighborExistCheck(TupleII* frame_neighbor_point_index, TupleII target_point_index, double cur_frame_target_point_distance, int bottom_horizontal_offset, ConvertedDataBlock* last_frame_blocks, int top_bottom_ind, u32 fov_data_block_count)
#endif
{
	u32 total_azimuth_count = fov_data_block_count / 2;
	int loopIndex = close_noise_neighbor_size_; // Firmware Parameter로 빼 놓을 것(default = 5)

	if (cur_frame_target_point_distance < 5)
		loopIndex = 1;

//	if (neighbor_point_index.size() < loopIndex)
//		loopIndex = neighbor_point_index.size();

	double new_moving_distance = distance_limit_;
//	if (viewer_info_->get_random_noise2_param4() != 0)
//		new_moving_distance = viewer_info_->get_random_noise2_param4();
//
//	if (cur_frame_target_point_distance < 1)
//		new_moving_distance = 0.1;


	int pre_frame_point_count = 0;
	for (int i = 0; i < loopIndex; ++i)
	{
		if(frame_neighbor_point_index[i].first < 0 || frame_neighbor_point_index[i].second < 0)
			continue;

		size_t top_bottom_shift_azimuth_offset = 0;

		if(frame_neighbor_point_index[i].first < total_azimuth_count)
			top_bottom_shift_azimuth_offset = bottom_horizontal_offset;

		int top_bottom_117_azimuth_offset = 0;
#ifdef G32_25HZ
		int neighbor_point_index_top_bottom_index = 0;
		if(frame_neighbor_point_index[i].first >= TOTAL_AZIMUTH_COUNT)
		{
			neighbor_point_index_top_bottom_index = 1;
		}
		if(top_bottom_ind != neighbor_point_index_top_bottom_index)
		{
			if((target_point_index.first > 648 && target_point_index.first < 1512)||(target_point_index.first > 2808 && target_point_index.first < 3672))
			{
				top_bottom_117_azimuth_offset = top_bottom_shift_azimuth_offset - bottom_horizontal_offset;
			}
		}
#endif

		int current_neighbor_point_azimuth = frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset;
							if(current_neighbor_point_azimuth < 0 || current_neighbor_point_azimuth >= fov_data_block_count)
								continue;

		double pre_frame_target_point_distance = last_frame_blocks[current_neighbor_point_azimuth].distance_[frame_neighbor_point_index[i].second];

		if (pre_frame_target_point_distance == 0)
			continue;

		double dis_diff_target_cur_pre = fabs(cur_frame_target_point_distance - pre_frame_target_point_distance);

		if (dis_diff_target_cur_pre < new_moving_distance)//2
		{
			++pre_frame_point_count;
		}
		if(pre_frame_point_count >= 1)
			break;
	}

	return pre_frame_point_count;
}

void CalculateNoiseScore(ConvertedDataBlock* current_frame_blocks, ConvertedDataBlock* last_frame_blocks, int bottom_horizontal_offset, volatile uint8_t is_bottom_horizontal_offset_changed, uint32_t frame_count, u32 fov_data_block_count)
{
	u32 total_azimuth_count = fov_data_block_count / 2;
	test_start(kNoiseFilter);
	int score_max = 100;

	double* vertical_angle_arr_p;
	int top_bottom_index = 0;
	int azimuth_index_offset = 0;
	double target_point_distance;
	int score;
	size_t no_target_count;
	TupleII* frame_neighbor_point_index_p;

	//	uint8_t is_bottom_first = 1;
	//	for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; channel_index++)
	for(size_t azimuth_index = 0 ; azimuth_index < fov_data_block_count ; ++azimuth_index)
	{
		//		if(azimuth_index%3 != 0)
		//			continue;
		//		test_start(kTCPSend);
		if(azimuth_index >= total_azimuth_count)
		{
			vertical_angle_arr_p = vertical_angle_array_top;
			top_bottom_index = 1;
			azimuth_index_offset = 0;
		}
		else
		{
			vertical_angle_arr_p = vertical_angle_array_bottom;
			top_bottom_index = 0;
			azimuth_index_offset = 0;
		}
		//		test_end(kTCPSend);
		//		for(size_t azimuth_index = 0 ; azimuth_index < FRAME_DATA_COUNT ; azimuth_index++)
		for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; ++channel_index)
		{
			//			test_start(kParseData);

			target_point_distance = current_frame_blocks[azimuth_index].distance_[channel_index];

			//elapsed time : 8ms
			if(target_point_distance < remove_noise_min_distance_ && target_point_distance != 0)
			{

				TupleII target_point_index;
				target_point_index.first = azimuth_index;
				target_point_index.second = channel_index;

				//				RunCloseRangeRandomNoiseFilter(lidar_point_cloud_data_, viewer_info_, target_point_index, target_point_distance);

				//				int azimuth_index = target_point_index.first;
				//				int channel_index = target_point_index.second;

				double neighbor_distance = 2.0;

				if(target_point_distance < 16.4)
					neighbor_distance = 1.0;

				//				bool is_bottom_first = false;
				//				if(lidar_point_cloud_data_[0].stop_[0].top_bottom_[0].firtst_channel_angle == -4.8437500000000000)
				//					is_bottom_first = true;
				//
				//				vector < PointIndexInfo > previous_frame_neighbor_point_index;
				//				if(lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].previous_frame_neighbor_point_index_.size() > 0 && lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].is_bottom_first == is_bottom_first)
				//				{
				//					previous_frame_neighbor_point_index = lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].previous_frame_neighbor_point_index_;
				//				}
				//				else
				//				{
				//
				//					int azimuth_index_offset = lidar_point_cloud_data_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_index_offset;
				//
				//					GetNeighborPointIndex(previous_frame_neighbor_point_index, target_point_index, azimuth_index_offset, 3, is_bottom_first);
				//					lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].is_bottom_first = is_bottom_first;
				//					lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].previous_frame_neighbor_point_index_ = previous_frame_neighbor_point_index;	// 인접 포인트 인덱스 리스트 저장
				//				}

				int cur_frame_neighbor_point_count = 0;
				int loopIndex = 13;
				frame_neighbor_point_index_p = lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_;
				//				if(previous_frame_neighbor_point_index.size() < 13)
				//					loopIndex = previous_frame_neighbor_point_index.size();

				for(size_t i = 1 ; i < loopIndex ; ++i)
				{
					if(frame_neighbor_point_index_p[i].first < 0 || frame_neighbor_point_index_p[i].second < 0)
						continue;

					size_t top_bottom_shift_azimuth_offset = 0;

					if(frame_neighbor_point_index_p[i].first < total_azimuth_count)
						top_bottom_shift_azimuth_offset = bottom_horizontal_offset;

					int top_bottom_117_azimuth_offset = 0;
#ifdef G32_25HZ
					int neighbor_point_index_top_bottom_index = 0;
					if(frame_neighbor_point_index[i].first >= TOTAL_AZIMUTH_COUNT)
					{
						neighbor_point_index_top_bottom_index = 1;
					}
					if(top_bottom_index != neighbor_point_index_top_bottom_index)
					{
						if((target_point_index.first > 648 && target_point_index.first < 1512)||(target_point_index.first > 2808 && target_point_index.first < 3672))
						{
							top_bottom_117_azimuth_offset = top_bottom_shift_azimuth_offset - bottom_horizontal_offset;
						}
					}
#endif

					int current_neighbor_point_azimuth = frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset;
					if(current_neighbor_point_azimuth < 0 || current_neighbor_point_azimuth >= fov_data_block_count)
						continue;

					double neighbor_point_distance = current_frame_blocks[current_neighbor_point_azimuth].distance_[frame_neighbor_point_index_p[i].second];

					if(neighbor_point_distance == 0)
						continue;

					double dis_diff_target_neighbor = fabs(neighbor_point_distance - target_point_distance);

					if(dis_diff_target_neighbor < neighbor_distance)
					{
						++cur_frame_neighbor_point_count;
					}

					if(cur_frame_neighbor_point_count >= 2)
						break;

				}

				if(cur_frame_neighbor_point_count < 1)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].is_noise_point_ = true;
				}

				if(cur_frame_neighbor_point_count == 1)
				{
					for(size_t i = 1 ; i < loopIndex ; ++i)
					{
						if(frame_neighbor_point_index_p[i].first < 0 || frame_neighbor_point_index_p[i].second < 0)
							continue;

						size_t top_bottom_shift_azimuth_offset = 0;

						if(frame_neighbor_point_index_p[i].first < total_azimuth_count)
							top_bottom_shift_azimuth_offset = bottom_horizontal_offset;

						int top_bottom_117_azimuth_offset = 0;
#ifdef G32_25HZ
						int neighbor_point_index_top_bottom_index = 0;
						if(frame_neighbor_point_index[i].first >= TOTAL_AZIMUTH_COUNT)
						{
							neighbor_point_index_top_bottom_index = 1;
						}
						if(top_bottom_index != neighbor_point_index_top_bottom_index)
						{
							if((target_point_index.first > 648 && target_point_index.first < 1512)||(target_point_index.first > 2808 && target_point_index.first < 3672))
							{
								top_bottom_117_azimuth_offset = top_bottom_shift_azimuth_offset - bottom_horizontal_offset;
							}
						}
#endif
						int current_neighbor_point_azimuth = frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset;
						if(current_neighbor_point_azimuth < 0 || current_neighbor_point_azimuth >= fov_data_block_count)
							continue;

						double neighbor_point_distance = current_frame_blocks[current_neighbor_point_azimuth].distance_[frame_neighbor_point_index_p[i].second];

						if(neighbor_point_distance == 0)
							continue;

						double dis_diff_target_neighbor = fabs(neighbor_point_distance - target_point_distance);

						if(dis_diff_target_neighbor < neighbor_distance)
						{
							TupleII neighbor_point_index;

							neighbor_point_index.first = current_neighbor_point_azimuth;
							neighbor_point_index.second = frame_neighbor_point_index_p[i].second;
							int top_bottom_ind = 0;
							if(current_neighbor_point_azimuth >= total_azimuth_count)
							{
								top_bottom_ind = 1;
							}
							int neighbor_exist = NeighborExistCheck(lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].frame_neighbor_point_index_, neighbor_point_index, neighbor_point_distance, bottom_horizontal_offset, last_frame_blocks, top_bottom_ind, fov_data_block_count);
							if(neighbor_exist == 0)
							{
								top_bottom_ind = 0;
								if(azimuth_index >= total_azimuth_count)
								{
									top_bottom_ind = 1;
								}
								int target_exist = NeighborExistCheck(lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_, target_point_index, target_point_distance, bottom_horizontal_offset, last_frame_blocks, top_bottom_ind, fov_data_block_count);

								if(neighbor_exist + target_exist < 2)
								{
									lidar_point_cloud_data_score_[frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index_p[i].second].is_noise_point_ = true;
									lidar_point_cloud_data_score_[azimuth_index][channel_index].is_noise_point_ = true;
								}
							}
							break;
						}
					}

				}

				score = lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_;
				no_target_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_;

				//누적 스코어 1감소
				if(score > 0)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score - 1;
				}
				// 현재 프레임, 자기 자신 포인트 스코어
				lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_self_score_ = 0;

				// 해당 Azimuth, Vertical에서 타겟이 감지되지 않은 횟수
				if(no_target_count < 10)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_ = no_target_count + 1;
				}

				//				lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].is_skip_point = true;
				//				test_end(kParseData);
				continue;
			}

			//			if(target_point_distance < remove_noise_min_distance_)
			//				continue;
			//			test_end(kParseData);

			//			test_start(kTCPBuffering);
			// Distance 0일 경우 스코어 감소 후 스킵
			if(target_point_distance == 0)
			{
				score = lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_;
				no_target_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_;

				//누적 스코어 1감소
				if(score > 0)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score - 1;
				}
				// 현재 프레임, 자기 자신 포인트 스코어
				lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_self_score_ = 0;

				// 해당 Azimuth, Vertical에서 타겟이 감지되지 않은 횟수
				if(no_target_count < 10)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_ = no_target_count + 1;
				}
				//				test_end(kTCPBuffering);
				continue;
			}

			// 현재 검사 포인트에 대한 인덱스 정의
			TupleII target_point_index;
			target_point_index.first = azimuth_index;
			target_point_index.second = channel_index;

			frame_neighbor_point_index_p = lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_;
			//			}
			//////////////////////////////////////////////////GetNeighborPointIndex//////////////////////////////////////////////////
			int cur_frame_neighbor_point_count = 0;

			//elapsed time : 10ms
			for(size_t i = 1 ; i < neighbor_point_index_size ; ++i)
			{
				if(frame_neighbor_point_index_p[i].first < 0 || frame_neighbor_point_index_p[i].second < 0)
					continue;

				size_t top_bottom_shift_azimuth_offset = 0;
				if(frame_neighbor_point_index_p[i].first < total_azimuth_count)
					top_bottom_shift_azimuth_offset = bottom_horizontal_offset;

				int top_bottom_117_azimuth_offset = 0;
#ifdef G32_25HZ
				int neighbor_point_index_top_bottom_index = 0;
				if(frame_neighbor_point_index[i].first >= TOTAL_AZIMUTH_COUNT)
				{
					neighbor_point_index_top_bottom_index = 1;
				}
				if(top_bottom_index != neighbor_point_index_top_bottom_index)
				{
					if((target_point_index.first > 648 && target_point_index.first < 1512)||(target_point_index.first > 2808 && target_point_index.first < 3672))
					{
						top_bottom_117_azimuth_offset = top_bottom_shift_azimuth_offset - bottom_horizontal_offset;
					}
				}
#endif
				int current_neighbor_point_azimuth = frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset;
				if(current_neighbor_point_azimuth < 0 || current_neighbor_point_azimuth >= fov_data_block_count)
					continue;

				// 주변 포인트의 Distance 획득
				double neighbor_point_distance = current_frame_blocks[current_neighbor_point_azimuth].distance_[frame_neighbor_point_index_p[i].second];
				//double neighbor_point_distance = lidar_point_cloud_data_[frame_neighbor_point_index[i].lidar_index].stop_[j].top_bottom_[frame_neighbor_point_index[i].top_bottom_index].azimuth_[frame_neighbor_point_index[i].azimuth_index + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset].channel_[frame_neighbor_point_index[i].channel_index].distance_;

				if(neighbor_point_distance == 0)
					continue;

				double dis_diff_target_neighbor = fabs(neighbor_point_distance - target_point_distance);

				// 주변 포인트와의 거리가 0.7m 미만일 경우 인접 포인트 카운트 증가
				if(dis_diff_target_neighbor < 0.7 && neighbor_point_distance != 0)
				{
					++cur_frame_neighbor_point_count;

					// 인접 포인트 갯수가 3개 이상일 경우 검사 종료(3개 이상이면, 검사대상 포인트가 노이즈가 아님이 확실하기때문에...)
					if(cur_frame_neighbor_point_count > 2)
					{
						break;
					}

				}

			}

			lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_neighbor_point_count_ = cur_frame_neighbor_point_count;

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			// 이전 프레임의 주변 포인트 검사 (거리 100m 이하일경우 인접 넓이 1, 100초과일 경우 인접 넓이 3 )
			size_t neighbor_size = 5;
			if(target_point_distance > 100)
				neighbor_size = neighbor_point_index_size;

			// ========================= 1-3. 이전 프레임의 주변 포인트 검사 =========================

			//elapsed time : 12ms
			unsigned int pre_frame_neighbor_point_count = 0;
			for(size_t i = 0 ; i < neighbor_size ; ++i)
			{
				// 예외 처리 및 인덱스 옵셋 계산
				if(frame_neighbor_point_index_p[i].first < 0 || frame_neighbor_point_index_p[i].second < 0)
					continue;

				int top_bottom_shift_azimuth_offset = 0;
				if(frame_neighbor_point_index_p[i].first < total_azimuth_count)
					top_bottom_shift_azimuth_offset = bottom_horizontal_offset;
				//
				//				int top_bottom_shift_azimuth_offset =
				//						lidar_point_cloud_data_last_[frame_neighbor_point_index[i].lidar_index].stop_[frame_neighbor_point_index[i].stop_index].top_bottom_[frame_neighbor_point_index[i].top_bottom_index].azimuth_index_offset;

				int top_bottom_117_azimuth_offset = 0;

#ifdef G32_25HZ
				int neighbor_point_index_top_bottom_index = 0;
				if(frame_neighbor_point_index[i].first >= TOTAL_AZIMUTH_COUNT)
				{
					neighbor_point_index_top_bottom_index = 1;
				}
				if(top_bottom_index != neighbor_point_index_top_bottom_index)
				{
					if((target_point_index.first > 648 && target_point_index.first < 1512)||(target_point_index.first > 2808 && target_point_index.first < 3672))
					{
						top_bottom_117_azimuth_offset = top_bottom_shift_azimuth_offset - bottom_horizontal_offset;
					}
				}
#endif
				int current_neighbor_point_azimuth = frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset;
				if(current_neighbor_point_azimuth < 0 || current_neighbor_point_azimuth >= fov_data_block_count)
					continue;

				double neighbor_point_distance = last_frame_blocks[current_neighbor_point_azimuth].distance_[frame_neighbor_point_index_p[i].second];

				// 검사대상 포인트와 과거프레임의 주변 포인트의 거리가 2m내 일 경우, 스코어 증가 (2m 시속 200km)
				if(fabs(neighbor_point_distance - target_point_distance) < distance_limit_ && neighbor_point_distance != 0)
				{
					++pre_frame_neighbor_point_count;

					// 대상 이웃의 스코어가 이미 계산되었는지 확인

					if(is_noise_score_calculated[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second] == 0)
					{
						bool is_target_changed = false;
						// 포인트의 유효 거리 범위 업데이트 및 타겟 변경 체크
						double distance_min = lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_min;
						double distance_max = lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_max;

						// 인접 포인트의 유효 거리 확인
						if((target_point_distance >= (distance_min - 5)) && (target_point_distance <= (distance_max + 5)))
						{
							double new_distance_min = distance_min;
							double new_distance_max = distance_max;

							if(target_point_distance < distance_min)
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_min = target_point_distance;
								new_distance_min = target_point_distance;
							}

							if(target_point_distance > distance_max)
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_max = target_point_distance;
								new_distance_max = target_point_distance;
							}

							if(fabs(new_distance_max - new_distance_min) > 15)
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_min = target_point_distance - 5;
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_max = target_point_distance + 5;
							}
						}
						else
						{
							lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_min = target_point_distance - 5;
							lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_max = target_point_distance + 5;
							is_target_changed = true;
						}

						// 인접 포인트의 스코어 부여
						if(is_target_changed == false) // 타겟이 바뀌지 않은 경우(유효 거리가 바뀌지 않은 경우)
						{
							// 기존 스코어에서 2증가
							int score = lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].shadow_score_;
							if((score + 2) > score_max)
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].shadow_score_ = score_max;
							}
							else
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].shadow_score_ = score + 2;
							}
						}
						else
						{
							// 리셋후 스코어 2 증가
							lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].shadow_score_ = 2;
						}

						is_noise_score_calculated[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second] = 1;
					}
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			// ========================= 1-4. 노이즈 스코어 조정 =========================
			int cur_frame_self_score = 0;
			unsigned int no_target_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_;
			unsigned int cur_no_target_count = 0;
			int score = lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_;

			if(pre_frame_neighbor_point_count == 0)
			{
				if(score > 0)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score - 1;
				}
			}
			else
			{
				bool is_target_changed = false;
				// 포인트의 유효 거리 범위 업데이트 및 타겟 변경 체크
				double distance_min = lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_min;
				double distance_max = lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_max;
				if((target_point_distance >= (distance_min - 5)) && (target_point_distance <= (distance_max + 5)))
				{
					double new_distance_min = distance_min;
					double new_distance_max = distance_max;

					if(target_point_distance < distance_min)
					{
						lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_min = target_point_distance;
						new_distance_min = target_point_distance;
					}

					if(target_point_distance > distance_max)
					{
						lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_max = target_point_distance;
						new_distance_max = target_point_distance;
					}
					if(fabs(new_distance_max - new_distance_min) > 15)
					{
						lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_min = target_point_distance - 5;
						lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_max = target_point_distance + 5;
					}
				}
				else
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_min = target_point_distance - 5;
					lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_max = target_point_distance + 5;
					is_target_changed = true;
				}

				cur_no_target_count = no_target_count + 1;

				cur_frame_self_score = 3 + (pre_frame_neighbor_point_count - 1) * 2;

				if(cur_no_target_count >= 10)
				{
					--cur_frame_self_score;
					lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_ = 0;
				}

				// 대상 포인트(자기 자신)의 스코어 부여
				if(is_target_changed == false) // 타겟이 바뀌지 않은 경우
				{
					if((score + cur_frame_self_score) > score_max)
						lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score_max;
					else
						lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score + cur_frame_self_score;
				}
				else // 타겟이 바뀐경우
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = cur_frame_self_score;
				}
			}

			lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_self_score_ = cur_frame_self_score; // 현재 프레임에서 추가된 스코어
			lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_ = cur_no_target_count;	// 해당 Azimuth, Vertical에서 타겟이 감지되지 않은 횟수
			lidar_point_cloud_data_score_[azimuth_index][channel_index].pre_frame_neighbor_point_count_ = pre_frame_neighbor_point_count;	// 이전 프레임의 인접 포인트 갯수 저정
			//			if(is_bottom_horizontal_offset_changed == 1 || frame_count < 20)
			//				memcpy(lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_, frame_neighbor_point_index_p, MAX_NEIGHBOR_POINT_SIZE * sizeof(TupleII));
			//			lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_ =
			//					frame_neighbor_point_index;	// 인접 포인트 인덱스 리스트 저장
			lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_size_ = neighbor_point_index_size;
			lidar_point_cloud_data_score_[azimuth_index][channel_index].target_point_index_ = target_point_index;					// 검대대상 포인트 익덱스 저장
			//			test_end(kTCPBuffering);
		}
	}
	test_end(kNoiseFilter);
}

void CalculateNoiseScore2(ConvertedDataBlock* current_frame_blocks, ConvertedDataBlock* last_frame_blocks, int bottom_horizontal_offset, u32 fov_data_block_count)
{
	u32 total_azimuth_count = fov_data_block_count / 2;
	test_start(kNoiseFilter);
	int score_max = 100;

	double* vertical_angle_arr_p;
	int top_bottom_index = 0;
	int azimuth_index_offset = 0;
	double target_point_distance;
	int score;
	size_t no_target_count;
	TupleII* frame_neighbor_point_index_p;

	//	uint8_t is_bottom_first = 1;
//	for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; channel_index++)
	for(size_t azimuth_index = 0 ; azimuth_index < fov_data_block_count ; ++azimuth_index)
	{
//		if(azimuth_index%3 != 0)
//			continue;
//		test_start(kTCPSend);
		if(azimuth_index >= total_azimuth_count)
		{
			vertical_angle_arr_p = vertical_angle_array_top;
			top_bottom_index = 1;
			azimuth_index_offset = 0;
		}
		else
		{
			vertical_angle_arr_p = vertical_angle_array_bottom;
			top_bottom_index = 0;
			azimuth_index_offset = 0;
		}
//		test_end(kTCPSend);
//		for(size_t azimuth_index = 0 ; azimuth_index < FRAME_DATA_COUNT ; azimuth_index++)
		for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; ++channel_index)
		{
//			test_start(kParseData);

			target_point_distance = current_frame_blocks[azimuth_index].distance_[channel_index];

			//elapsed time : 8ms
			if(target_point_distance < remove_noise_min_distance_ && target_point_distance != 0)
			{

				TupleII target_point_index;
				target_point_index.first = azimuth_index;
				target_point_index.second = channel_index;

				//				RunCloseRangeRandomNoiseFilter(lidar_point_cloud_data_, viewer_info_, target_point_index, target_point_distance);

//				int azimuth_index = target_point_index.first;
//				int channel_index = target_point_index.second;

				double neighbor_distance = 2.0;

				if(target_point_distance < 16.4)
					neighbor_distance = 1.0;

				//				bool is_bottom_first = false;
				//				if(lidar_point_cloud_data_[0].stop_[0].top_bottom_[0].firtst_channel_angle == -4.8437500000000000)
				//					is_bottom_first = true;
				//
				//				vector < PointIndexInfo > previous_frame_neighbor_point_index;
				//				if(lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].previous_frame_neighbor_point_index_.size() > 0 && lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].is_bottom_first == is_bottom_first)
				//				{
				//					previous_frame_neighbor_point_index = lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].previous_frame_neighbor_point_index_;
				//				}
				//				else
				//				{
				//
				//					int azimuth_index_offset = lidar_point_cloud_data_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_index_offset;
				//
				//					GetNeighborPointIndex(previous_frame_neighbor_point_index, target_point_index, azimuth_index_offset, 3, is_bottom_first);
				//					lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].is_bottom_first = is_bottom_first;
				//					lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].previous_frame_neighbor_point_index_ = previous_frame_neighbor_point_index;	// 인접 포인트 인덱스 리스트 저장
				//				}

				int cur_frame_neighbor_point_count = 0;
				int loopIndex = 13;
				frame_neighbor_point_index_p = lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_;
				//				if(previous_frame_neighbor_point_index.size() < 13)
				//					loopIndex = previous_frame_neighbor_point_index.size();
				loopIndex = close_noise_neighbor_size_ * close_noise_neighbor_size_ * 2 + close_noise_neighbor_size_ * 2 + 1;

				for(size_t i = 1 ; i < loopIndex ; ++i)
				{
					if(frame_neighbor_point_index_p[i].first < 0 || frame_neighbor_point_index_p[i].second < 0)
						continue;

					int top_bottom_shift_azimuth_offset = 0;
//					if(frame_neighbor_point_index_p[i].first < TOTAL_AZIMUTH_COUNT)
//						top_bottom_shift_azimuth_offset = bottom_horizontal_offset;
					int neighbor_point_index_top_bottom_index = 0;

					if(frame_neighbor_point_index[i].first >= total_azimuth_count)
					{
						neighbor_point_index_top_bottom_index = 1;
					}

					if(bottom_horizontal_offset != 0)
					{
						if(top_bottom_index == 0 && neighbor_point_index_top_bottom_index == 1)
						{
#ifdef G32_25HZ
							top_bottom_shift_azimuth_offset = bottom_horizontal_offset;

							if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
								top_bottom_shift_azimuth_offset += bottom_horizontal_offset;
#endif
						}
						else if(top_bottom_index == 1 && neighbor_point_index_top_bottom_index == 0)
						{
#ifdef G32_25HZ
							top_bottom_shift_azimuth_offset = -bottom_horizontal_offset;

							if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
								top_bottom_shift_azimuth_offset -= bottom_horizontal_offset;
#endif
						}
					}

					int top_bottom_117_azimuth_offset = 0;
//#ifdef G32_25HZ
//					if(top_bottom_index != neighbor_point_index_top_bottom_index)
//					{
//						if((target_point_index.first > 648 && target_point_index.first < 1512)||(target_point_index.first > 2808 && target_point_index.first < 3672))
//						{
//							top_bottom_117_azimuth_offset = top_bottom_shift_azimuth_offset - bottom_horizontal_offset;
//						}
//					}
//#endif

					int current_neighbor_point_azimuth = frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset;
					if(current_neighbor_point_azimuth < 0 || current_neighbor_point_azimuth >= fov_data_block_count)
						continue;

					//for (int j = 0; j < 3; ++j)
					//				{
					// 주변 포인트의 Distance 획득
					double neighbor_point_distance = current_frame_blocks[current_neighbor_point_azimuth].distance_[frame_neighbor_point_index_p[i].second];
					//double neighbor_point_distance = lidar_point_cloud_data_[frame_neighbor_point_index[i].lidar_index].stop_[j].top_bottom_[frame_neighbor_point_index[i].top_bottom_index].azimuth_[frame_neighbor_point_index[i].azimuth_index + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset].channel_[frame_neighbor_point_index[i].channel_index].distance_;

					if(neighbor_point_distance == 0)
						continue;

					double dis_diff_target_neighbor = fabs(neighbor_point_distance - target_point_distance);

					if(dis_diff_target_neighbor < neighbor_distance)
					{
						++cur_frame_neighbor_point_count;
					}

					if(cur_frame_neighbor_point_count >= 2)
						break;

				}

				if(cur_frame_neighbor_point_count < 1)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].is_noise_point_ = true;
				}

				if(cur_frame_neighbor_point_count == 1)
				{
					for(size_t i = 1 ; i < loopIndex ; ++i)
					{
						if(frame_neighbor_point_index_p[i].first < 0 || frame_neighbor_point_index_p[i].second < 0)
							continue;

						int top_bottom_shift_azimuth_offset = 0;
						//					if(frame_neighbor_point_index_p[i].first < TOTAL_AZIMUTH_COUNT)
						//						top_bottom_shift_azimuth_offset = bottom_horizontal_offset;
						int neighbor_point_index_top_bottom_index = 0;

						if(frame_neighbor_point_index[i].first >= total_azimuth_count)
						{
							neighbor_point_index_top_bottom_index = 1;
						}

						if(top_bottom_index == 0 && neighbor_point_index_top_bottom_index == 1)
						{
#ifdef G32_25HZ
							top_bottom_shift_azimuth_offset = bottom_horizontal_offset;

							if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
								top_bottom_shift_azimuth_offset += bottom_horizontal_offset;
#endif
						}
						else if(top_bottom_index == 1 && neighbor_point_index_top_bottom_index == 0)
						{
#ifdef G32_25HZ
							top_bottom_shift_azimuth_offset = -bottom_horizontal_offset;

							if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
								top_bottom_shift_azimuth_offset -= bottom_horizontal_offset;
#endif
						}

						int top_bottom_117_azimuth_offset = 0;
						int current_neighbor_point_azimuth = frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset;
						if(current_neighbor_point_azimuth < 0 || current_neighbor_point_azimuth >= fov_data_block_count)
							continue;

						double neighbor_point_distance = current_frame_blocks[current_neighbor_point_azimuth].distance_[frame_neighbor_point_index_p[i].second];

						if(neighbor_point_distance == 0)
							continue;

						double dis_diff_target_neighbor = fabs(neighbor_point_distance - target_point_distance);

						if(dis_diff_target_neighbor < neighbor_distance)
						{
							TupleII neighbor_point_index;

							neighbor_point_index.first = current_neighbor_point_azimuth;
							neighbor_point_index.second = frame_neighbor_point_index_p[i].second;

							int top_bottom_ind = 0;
							if(current_neighbor_point_azimuth >= total_azimuth_count)
							{
								top_bottom_ind = 1;
							}
							int neighbor_exist = NeighborExistCheck(lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].frame_neighbor_point_index_, neighbor_point_index, neighbor_point_distance, bottom_horizontal_offset, last_frame_blocks, top_bottom_ind, fov_data_block_count);
							if(neighbor_exist == 0)
							{
								top_bottom_ind = 0;
								if(azimuth_index >= total_azimuth_count)
								{
									top_bottom_ind = 1;
								}
								int target_exist = NeighborExistCheck(lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_, target_point_index, target_point_distance, bottom_horizontal_offset, last_frame_blocks, top_bottom_ind, fov_data_block_count);

								if(neighbor_exist + target_exist < 2)
								{
									lidar_point_cloud_data_score_[frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index_p[i].second].is_noise_point_ = true;
									lidar_point_cloud_data_score_[azimuth_index][channel_index].is_noise_point_ = true;
								}
							}
							break;
						}
					}

				}

				score = lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_;
				no_target_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_;

				//누적 스코어 1감소
				if(score > 0)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score - 1;
				}
				// 현재 프레임, 자기 자신 포인트 스코어
				lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_self_score_ = 0;

				// 해당 Azimuth, Vertical에서 타겟이 감지되지 않은 횟수
				if(no_target_count < 10)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_ = no_target_count + 1;
				}

				//				lidar_point_cloud_data_score_[lidar_index].stop_[stop_index].top_bottom_[top_bottom_index].azimuth_[azimuth_index].channel_[channel_index].is_skip_point = true;
//				test_end(kParseData);
				continue;
			}

			//			if(target_point_distance < remove_noise_min_distance_)
			//				continue;
//			test_end(kParseData);

//			test_start(kTCPBuffering);
			// Distance 0일 경우 스코어 감소 후 스킵
			if(target_point_distance == 0)
			{
				score = lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_;
				no_target_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_;

				//누적 스코어 1감소
				if(score > 0)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score - 1;
				}
				// 현재 프레임, 자기 자신 포인트 스코어
				lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_self_score_ = 0;

				// 해당 Azimuth, Vertical에서 타겟이 감지되지 않은 횟수
				if(no_target_count < 10)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_ = no_target_count + 1;
				}
//				test_end(kTCPBuffering);
				continue;
			}

			// 현재 검사 포인트에 대한 인덱스 정의
			TupleII target_point_index;
			target_point_index.first = azimuth_index;
			target_point_index.second = channel_index;

			frame_neighbor_point_index_p = lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_;
			//			}
			//////////////////////////////////////////////////GetNeighborPointIndex//////////////////////////////////////////////////
			int cur_frame_neighbor_point_count = 0;

			//elapsed time : 10ms
			for(size_t i = 1 ; i < neighbor_point_index_size ; ++i)
			{
				if(frame_neighbor_point_index_p[i].first < 0 || frame_neighbor_point_index_p[i].second < 0)
					continue;

				int top_bottom_shift_azimuth_offset = 0;
				//					if(frame_neighbor_point_index_p[i].first < TOTAL_AZIMUTH_COUNT)
				//						top_bottom_shift_azimuth_offset = bottom_horizontal_offset;
				int neighbor_point_index_top_bottom_index = 0;

				if(frame_neighbor_point_index[i].first >= total_azimuth_count)
				{
					neighbor_point_index_top_bottom_index = 1;
				}

				if(top_bottom_index == 0 && neighbor_point_index_top_bottom_index == 1)
				{
#ifdef G32_25HZ
					top_bottom_shift_azimuth_offset = bottom_horizontal_offset;

					if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
						top_bottom_shift_azimuth_offset += bottom_horizontal_offset;
#endif
				}
				else if(top_bottom_index == 1 && neighbor_point_index_top_bottom_index == 0)
				{
#ifdef G32_25HZ
					top_bottom_shift_azimuth_offset = -bottom_horizontal_offset;

					if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
						top_bottom_shift_azimuth_offset -= bottom_horizontal_offset;
#endif
				}

				int top_bottom_117_azimuth_offset = 0;
				int current_neighbor_point_azimuth = frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset;
				if(current_neighbor_point_azimuth < 0 || current_neighbor_point_azimuth >= fov_data_block_count)
					continue;

				// 주변 포인트의 Distance 획득
				double neighbor_point_distance = current_frame_blocks[current_neighbor_point_azimuth].distance_[frame_neighbor_point_index_p[i].second];
				//double neighbor_point_distance = lidar_point_cloud_data_[frame_neighbor_point_index[i].lidar_index].stop_[j].top_bottom_[frame_neighbor_point_index[i].top_bottom_index].azimuth_[frame_neighbor_point_index[i].azimuth_index + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset].channel_[frame_neighbor_point_index[i].channel_index].distance_;

				if(neighbor_point_distance == 0)
					continue;

				double dis_diff_target_neighbor = fabs(neighbor_point_distance - target_point_distance);

				// 주변 포인트와의 거리가 0.7m 미만일 경우 인접 포인트 카운트 증가
				if(dis_diff_target_neighbor < 0.7 && neighbor_point_distance != 0)
				{
					++cur_frame_neighbor_point_count;

					// 인접 포인트 갯수가 3개 이상일 경우 검사 종료(3개 이상이면, 검사대상 포인트가 노이즈가 아님이 확실하기때문에...)
					if(cur_frame_neighbor_point_count > 2)
					{
						break;
					}

				}

			}

			lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_neighbor_point_count_ = cur_frame_neighbor_point_count;

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			// 이전 프레임의 주변 포인트 검사 (거리 100m 이하일경우 인접 넓이 1, 100초과일 경우 인접 넓이 3 )
			size_t neighbor_size = 5;
			if(target_point_distance > 100)
				neighbor_size = neighbor_point_index_size;

			// ========================= 1-3. 이전 프레임의 주변 포인트 검사 =========================

			//elapsed time : 12ms
			unsigned int pre_frame_neighbor_point_count = 0;
			for(size_t i = 0 ; i < neighbor_size ; ++i)
			{
				// 예외 처리 및 인덱스 옵셋 계산
				if(frame_neighbor_point_index_p[i].first < 0 || frame_neighbor_point_index_p[i].second < 0)
					continue;

				int top_bottom_shift_azimuth_offset = 0;
				//					if(frame_neighbor_point_index_p[i].first < TOTAL_AZIMUTH_COUNT)
				//						top_bottom_shift_azimuth_offset = bottom_horizontal_offset;
				int neighbor_point_index_top_bottom_index = 0;

				if(frame_neighbor_point_index[i].first >= total_azimuth_count)
				{
					neighbor_point_index_top_bottom_index = 1;
				}

				if(top_bottom_index == 0 && neighbor_point_index_top_bottom_index == 1)
				{
#ifdef G32_25HZ
					top_bottom_shift_azimuth_offset = bottom_horizontal_offset;

					if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
						top_bottom_shift_azimuth_offset += bottom_horizontal_offset;
#endif
				}
				else if(top_bottom_index == 1 && neighbor_point_index_top_bottom_index == 0)
				{
#ifdef G32_25HZ
					top_bottom_shift_azimuth_offset = -bottom_horizontal_offset;

					if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
						top_bottom_shift_azimuth_offset -= bottom_horizontal_offset;
#endif
				}

				int top_bottom_117_azimuth_offset = 0;
				int current_neighbor_point_azimuth = frame_neighbor_point_index_p[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset;
				if(current_neighbor_point_azimuth < 0 || current_neighbor_point_azimuth >= fov_data_block_count)
					continue;

				double neighbor_point_distance = last_frame_blocks[current_neighbor_point_azimuth].distance_[frame_neighbor_point_index_p[i].second];

				// 검사대상 포인트와 과거프레임의 주변 포인트의 거리가 2m내 일 경우, 스코어 증가 (2m 시속 200km)
				if(fabs(neighbor_point_distance - target_point_distance) < distance_limit_ && neighbor_point_distance != 0)
				{
					++pre_frame_neighbor_point_count;

					// 대상 이웃의 스코어가 이미 계산되었는지 확인

					if(is_noise_score_calculated[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second] == 0)
					{
						bool is_target_changed = false;
						// 포인트의 유효 거리 범위 업데이트 및 타겟 변경 체크
						double distance_min = lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_min;
						double distance_max = lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_max;

						// 인접 포인트의 유효 거리 확인
						if((target_point_distance >= (distance_min - 5)) && (target_point_distance <= (distance_max + 5)))
						{
							double new_distance_min = distance_min;
							double new_distance_max = distance_max;

							if(target_point_distance < distance_min)
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_min = target_point_distance;
								new_distance_min = target_point_distance;
							}

							if(target_point_distance > distance_max)
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_max = target_point_distance;
								new_distance_max = target_point_distance;
							}

							if(fabs(new_distance_max - new_distance_min) > 15)
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_min = target_point_distance - 5;
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_max = target_point_distance + 5;
							}
						}
						else
						{
							lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_min = target_point_distance - 5;
							lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].distance_max = target_point_distance + 5;
							is_target_changed = true;
						}

						// 인접 포인트의 스코어 부여
						if(is_target_changed == false) // 타겟이 바뀌지 않은 경우(유효 거리가 바뀌지 않은 경우)
						{
							// 기존 스코어에서 2증가
							int score = lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].shadow_score_;
							if((score + 2) > score_max)
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].shadow_score_ = score_max;
							}
							else
							{
								lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].shadow_score_ = score + 2;
							}
						}
						else
						{
							// 리셋후 스코어 2 증가
							lidar_point_cloud_data_score_[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second].shadow_score_ = 2;
						}

						is_noise_score_calculated[current_neighbor_point_azimuth][frame_neighbor_point_index_p[i].second] = 1;
					}
				}
			}


			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			// ========================= 1-4. 노이즈 스코어 조정 =========================
			int cur_frame_self_score = 0;
			unsigned int no_target_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_;
			unsigned int cur_no_target_count = 0;
			int score = lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_;

			if(pre_frame_neighbor_point_count == 0)
			{
				if(score > 0)
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score - 1;
				}
			}
			else
			{
				bool is_target_changed = false;
				// 포인트의 유효 거리 범위 업데이트 및 타겟 변경 체크
				double distance_min = lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_min;
				double distance_max = lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_max;
				if((target_point_distance >= (distance_min - 5)) && (target_point_distance <= (distance_max + 5)))
				{
					double new_distance_min = distance_min;
					double new_distance_max = distance_max;

					if(target_point_distance < distance_min)
					{
						lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_min = target_point_distance;
						new_distance_min = target_point_distance;
					}

					if(target_point_distance > distance_max)
					{
						lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_max = target_point_distance;
						new_distance_max = target_point_distance;
					}
					if(fabs(new_distance_max - new_distance_min) > 15)
					{
						lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_min = target_point_distance - 5;
						lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_max = target_point_distance + 5;
					}
				}
				else
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_min = target_point_distance - 5;
					lidar_point_cloud_data_score_[azimuth_index][channel_index].distance_max = target_point_distance + 5;
					is_target_changed = true;
				}

				cur_no_target_count = no_target_count + 1;

				cur_frame_self_score = 3 + (pre_frame_neighbor_point_count - 1) * 2;

				if(cur_no_target_count >= 10)
				{
					--cur_frame_self_score;
					lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_ = 0;
				}

				// 대상 포인트(자기 자신)의 스코어 부여
				if(is_target_changed == false) // 타겟이 바뀌지 않은 경우
				{
					if((score + cur_frame_self_score) > score_max)
						lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score_max;
					else
						lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = score + cur_frame_self_score;
				}
				else // 타겟이 바뀐경우
				{
					lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_ = cur_frame_self_score;
				}
			}

			lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_self_score_ = cur_frame_self_score; // 현재 프레임에서 추가된 스코어
			lidar_point_cloud_data_score_[azimuth_index][channel_index].no_target_count_ = cur_no_target_count;	// 해당 Azimuth, Vertical에서 타겟이 감지되지 않은 횟수
			lidar_point_cloud_data_score_[azimuth_index][channel_index].pre_frame_neighbor_point_count_ = pre_frame_neighbor_point_count;	// 이전 프레임의 인접 포인트 갯수 저정
			//			if(is_bottom_horizontal_offset_changed == 1 || frame_count < 20)
			//				memcpy(lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_, frame_neighbor_point_index_p, MAX_NEIGHBOR_POINT_SIZE * sizeof(TupleII));
			//			lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_ =
			//					frame_neighbor_point_index;	// 인접 포인트 인덱스 리스트 저장
			lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_size_ = neighbor_point_index_size;
			lidar_point_cloud_data_score_[azimuth_index][channel_index].target_point_index_ = target_point_index;					// 검대대상 포인트 익덱스 저장
//			test_end(kTCPBuffering);
		}
	}
	test_end(kNoiseFilter);
}

void RemoveNoisePoints(ConvertedDataBlock* current_frame_blocks, int bottom_horizontal_offset, RawDataBlock *block_data_shared, unsigned int* noise_point_cnt, u32 fov_data_block_count)
{
	u32 total_azimuth_count = fov_data_block_count / 2;
	int top_bottom_index = 0;
	for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; channel_index++)
//	for(size_t azimuth_index = 0 ; azimuth_index < FRAME_DATA_COUNT ; azimuth_index++)
	{
		for(size_t azimuth_index = 0 ; azimuth_index < fov_data_block_count ; azimuth_index++)
//		for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; channel_index++)
		{
			if(azimuth_index >= total_azimuth_count)
					{
						top_bottom_index = 1;
					}
					else
					{
						top_bottom_index = 0;
					}
			if(lidar_point_cloud_data_score_[azimuth_index][channel_index].is_noise_point_ == true)
			{
//				if(set_noise_point_intensity == 1)
//				{
//					remove_noise_intensity[azimuth_index][channel_index] = 255;
//				}
//				else
//				{
					block_data_shared[azimuth_index].distance_[channel_index].distance = 0;
//				}
				(*noise_point_cnt)++;
				lidar_point_cloud_data_score_[azimuth_index][channel_index].is_noise_point_ = false;
				continue;
			}
//			if(ignore_ground_point == 1)
//			{
//				if(is_ground_point[azimuth_index][channel_index] == 1)
//					continue;
//			}
			// ========================= 2-1. 노이즈 여부 판단 =========================
			//
			// 포인트 제거 여부 판단
			uint8_t is_noise = 0;
			int total_score = 100;
			double target_point_distance = current_frame_blocks[azimuth_index].distance_[channel_index];

			if(target_point_distance < remove_noise_min_distance_)
				continue;
//			if(target_point_distance == 0)
//				continue;

			// 해당 포인트의 누적 스코어 획득
			int self_score = lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_;
			if(self_score == 0)
			{
				// ========================= 2-1-1. 누적 스코어가 0일경우 노이즈일 확율 매우 높음 =========================
				// 누적 스코어가 0일 경우, 최종 스코어 0
				total_score = 0;
			}
			else
			{

				int pre_frame_neighbor_point_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].pre_frame_neighbor_point_count_;

				if(pre_frame_neighbor_point_count >= 2)
				{
					// ========================= 2-1-2. 검사 대상 포인트의 이전 프레임의 인접 포인트 수 확인(******** Rule-1 ********) =========================
				}
				//if (pre_frame_neighbor_point_count <= 1)
				else
				{
					// ========================= 2-1-3. [Rule-1]로 판단하기 애매할 경우, 인전포인트들의 발자취 누적 점수로 대상 포인트의 노이즈 여부 판단(******** Rule-2 ********) =========================

					// 이전 프레임의 인접 포인트 수 1개 이하일 경우, 인접 포인트들의 누적 포인트를 확인함
//					int cur_frame_self_score = lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_self_score_;
					TupleII* frame_neighbor_point_index = lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_;
//					memcpy(frame_neighbor_point_index, lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_,
//					MAX_NEIGHBOR_POINT_SIZE * sizeof(TupleII));
//					vector < PointIndexInfo > frame_neighbor_point_index =
//							lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_;
#ifdef G32_25HZ
					TupleII target_point_index = lidar_point_cloud_data_score_[azimuth_index][channel_index].target_point_index_;
#endif

					unsigned int neighbor_size = 5;
					if(target_point_distance > 100)
						neighbor_size = lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_size_;

					int previous_neighbor_score_sum = 0;
					for(size_t i = 0 ; i < neighbor_size ; ++i)
					{
						// 예외 처리 및 인덱스 옵셋 계산
						if(frame_neighbor_point_index[i].first < 0 || frame_neighbor_point_index[i].second < 0)
							continue;
						int top_bottom_shift_azimuth_offset = 0;
						if(frame_neighbor_point_index[i].first < total_azimuth_count)
							top_bottom_shift_azimuth_offset = bottom_horizontal_offset;
//						int top_bottom_shift_azimuth_offset =
//								last_frame_blocks[frame_neighbor_point_index[i].lidar_index].stop_[frame_neighbor_point_index[i].stop_index].top_bottom_[frame_neighbor_point_index[i].top_bottom_index].azimuth_index_offset;

						int top_bottom_117_azimuth_offset = 0;
#ifdef G32_25HZ
						int neighbor_point_index_top_bottom_index = 0;
										if(frame_neighbor_point_index[i].first >= TOTAL_AZIMUTH_COUNT)
										{
											neighbor_point_index_top_bottom_index = 1;
										}
										if(top_bottom_index != neighbor_point_index_top_bottom_index)
										{
											if((target_point_index.first > 648 && target_point_index.first < 1512)||(target_point_index.first > 2808 && target_point_index.first < 3672))
											{
												top_bottom_117_azimuth_offset = top_bottom_shift_azimuth_offset - bottom_horizontal_offset;
											}
										}
#endif
						if(frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset < 0 || frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset >= fov_data_block_count)
							continue;

						double distance_min = lidar_point_cloud_data_score_[frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index[i].second].distance_min;
						double distance_max = lidar_point_cloud_data_score_[frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index[i].second].distance_max;
						double shadow_score = lidar_point_cloud_data_score_[frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index[i].second].shadow_score_;
						double cur_frame_self_score = lidar_point_cloud_data_score_[frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index[i].second].cur_frame_self_score_;

						// 인접 포인트의 누적 포인트 합산 (타겟 포인트와 유효 거리 범위내에 있는 인접 포인트만...)
						if((target_point_distance >= (distance_min - 2)) && (target_point_distance <= (distance_max + 2)))
						{
							previous_neighbor_score_sum += (shadow_score - cur_frame_self_score);
						}

					}
					total_score = previous_neighbor_score_sum;
				}

			}

			//if(total_score <= viewer_info_->get_long_tail_noise_param10())
			if(total_score <= 4)
			{
				//total_score가 4 이하일 경우 노이즈일 가능성이 매우 높은 것으로 판단

				int cur_frame_neighbor_point_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_neighbor_point_count_;
				if(cur_frame_neighbor_point_count <= 2)
				{
					// ========================= 2-1-4. [Rule-1, Rule-2]로 노이즈로 판단되었더라도, 현재 프레임에서 대상 포인트의 인접한 포인트가 *많이* 있을 경우 노이즈가 아닌것으로 판단 (******** Rule-3 ********) =========================

					// 현재 프레임에서 인접한 포인트가 3개 이상일 경우 노이즈가 아닌것으로 최종 판단
					is_noise = true;
				}

			}
			if(is_noise == true)
			{
				// 노이즈 제거
//				current_frame_blocks[azimuth_index].distance_[channel_index] = 0;
//				if(set_noise_point_intensity == 1)
//				{
//					remove_noise_intensity[azimuth_index][channel_index] = 255;
//				}
//				else
//				{
					block_data_shared[azimuth_index].distance_[channel_index].distance = 0;
//				}
				(*noise_point_cnt)++;
			}
		}
	}
}

void RemoveNoisePoints2(ConvertedDataBlock* current_frame_blocks, int bottom_horizontal_offset, RawDataBlock *block_data_shared, unsigned int* noise_point_cnt, u32 fov_data_block_count)
{
	u32 total_azimuth_count = fov_data_block_count / 2;
	int top_bottom_index = 0;
//	for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; channel_index++)
		for(size_t azimuth_index = 0 ; azimuth_index < fov_data_block_count ; ++azimuth_index)
	{
			if(azimuth_index >= total_azimuth_count)
					{
						top_bottom_index = 1;
					}
					else
					{
						top_bottom_index = 0;
					}
//		for(size_t azimuth_index = 0 ; azimuth_index < FRAME_DATA_COUNT ; azimuth_index++)
				for(size_t channel_index = 0 ; channel_index < ONE_SIDE_CHANNEL_COUNT ; ++channel_index)
		{
			if(lidar_point_cloud_data_score_[azimuth_index][channel_index].is_noise_point_ == true)
			{
//				if(set_noise_point_intensity == 1)
//				{
//					remove_noise_intensity[azimuth_index][channel_index] = 255;
//				}
//				else
//				{
					block_data_shared[azimuth_index].distance_[channel_index].distance = 0;
//				}
				(*noise_point_cnt)++;
				lidar_point_cloud_data_score_[azimuth_index][channel_index].is_noise_point_ = false;
				continue;
			}
			//			if(ignore_ground_point == 1)
			//			{
			//				if(is_ground_point[azimuth_index][channel_index] == 1)
			//					continue;
			//			}
			// ========================= 2-1. 노이즈 여부 판단 =========================
			//
			// 포인트 제거 여부 판단
			uint8_t is_noise = 0;
			int total_score = 100;
			double target_point_distance = current_frame_blocks[azimuth_index].distance_[channel_index];

			if(target_point_distance < remove_noise_min_distance_)
				continue;
			//			if(target_point_distance == 0)
			//				continue;

			// 해당 포인트의 누적 스코어 획득
			int self_score = lidar_point_cloud_data_score_[azimuth_index][channel_index].shadow_score_;
			if(self_score == 0)
			{
				// ========================= 2-1-1. 누적 스코어가 0일경우 노이즈일 확율 매우 높음 =========================
				// 누적 스코어가 0일 경우, 최종 스코어 0
				total_score = 0;
			}
			else
			{

				int pre_frame_neighbor_point_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].pre_frame_neighbor_point_count_;

				if(pre_frame_neighbor_point_count >= 2)
				{
					// ========================= 2-1-2. 검사 대상 포인트의 이전 프레임의 인접 포인트 수 확인(******** Rule-1 ********) =========================
				}
				//if (pre_frame_neighbor_point_count <= 1)
				else
				{
					// ========================= 2-1-3. [Rule-1]로 판단하기 애매할 경우, 인전포인트들의 발자취 누적 점수로 대상 포인트의 노이즈 여부 판단(******** Rule-2 ********) =========================

					// 이전 프레임의 인접 포인트 수 1개 이하일 경우, 인접 포인트들의 누적 포인트를 확인함
					//					int cur_frame_self_score = lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_self_score_;
					TupleII* frame_neighbor_point_index = lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_;
					//					memcpy(frame_neighbor_point_index, lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_,
					//					MAX_NEIGHBOR_POINT_SIZE * sizeof(TupleII));
					//					vector < PointIndexInfo > frame_neighbor_point_index =
					//							lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_;
#ifdef G32_25HZ
					TupleII target_point_index = lidar_point_cloud_data_score_[azimuth_index][channel_index].target_point_index_;
#endif

					unsigned int neighbor_size = 5;
					if(target_point_distance > 100)
						neighbor_size = lidar_point_cloud_data_score_[azimuth_index][channel_index].frame_neighbor_point_index_size_;

					int previous_neighbor_score_sum = 0;
					for(size_t i = 0 ; i < neighbor_size ; ++i)
					{
						// 예외 처리 및 인덱스 옵셋 계산
						if(frame_neighbor_point_index[i].first < 0 || frame_neighbor_point_index[i].second < 0)
							continue;
						int top_bottom_shift_azimuth_offset = 0;
						//					if(frame_neighbor_point_index_p[i].first < TOTAL_AZIMUTH_COUNT)
						//						top_bottom_shift_azimuth_offset = bottom_horizontal_offset;
						int neighbor_point_index_top_bottom_index = 0;

						if(frame_neighbor_point_index[i].first >= total_azimuth_count)
						{
							neighbor_point_index_top_bottom_index = 1;
						}

						if(top_bottom_index == 0 && neighbor_point_index_top_bottom_index == 1)
						{
#ifdef G32_25HZ
							top_bottom_shift_azimuth_offset = bottom_horizontal_offset;

							if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
								top_bottom_shift_azimuth_offset += bottom_horizontal_offset;
#endif
						}
						else if(top_bottom_index == 1 && neighbor_point_index_top_bottom_index == 0)
						{
#ifdef G32_25HZ
							top_bottom_shift_azimuth_offset = -bottom_horizontal_offset;

							if((frame_neighbor_point_index[i].first >= 651 && frame_neighbor_point_index[i].first <= 1509) || (frame_neighbor_point_index[i].first >= 2811 && frame_neighbor_point_index[i].first <= 3669))
								top_bottom_shift_azimuth_offset -= bottom_horizontal_offset;
#endif
						}

						int top_bottom_117_azimuth_offset = 0;
						if(frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset < 0 || frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset >= fov_data_block_count)
							continue;

						double distance_min = lidar_point_cloud_data_score_[frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index[i].second].distance_min;
						double distance_max = lidar_point_cloud_data_score_[frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index[i].second].distance_max;
						double shadow_score = lidar_point_cloud_data_score_[frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index[i].second].shadow_score_;
						double cur_frame_self_score = lidar_point_cloud_data_score_[frame_neighbor_point_index[i].first + top_bottom_shift_azimuth_offset + top_bottom_117_azimuth_offset][frame_neighbor_point_index[i].second].cur_frame_self_score_;

						// 인접 포인트의 누적 포인트 합산 (타겟 포인트와 유효 거리 범위내에 있는 인접 포인트만...)
						if((target_point_distance >= (distance_min - 2)) && (target_point_distance <= (distance_max + 2)))
						{
							previous_neighbor_score_sum += (shadow_score - cur_frame_self_score);
						}

					}
					total_score = previous_neighbor_score_sum;
				}

			}

			//if(total_score <= viewer_info_->get_long_tail_noise_param10())
			if(total_score <= 4)
			{
				//total_score가 4 이하일 경우 노이즈일 가능성이 매우 높은 것으로 판단

				int cur_frame_neighbor_point_count = lidar_point_cloud_data_score_[azimuth_index][channel_index].cur_frame_neighbor_point_count_;
				if(cur_frame_neighbor_point_count <= 2)
				{
					// ========================= 2-1-4. [Rule-1, Rule-2]로 노이즈로 판단되었더라도, 현재 프레임에서 대상 포인트의 인접한 포인트가 *많이* 있을 경우 노이즈가 아닌것으로 판단 (******** Rule-3 ********) =========================

					// 현재 프레임에서 인접한 포인트가 3개 이상일 경우 노이즈가 아닌것으로 최종 판단
					is_noise = true;
				}

			}
			if(is_noise == true)
			{
				// 노이즈 제거
				//				current_frame_blocks[azimuth_index].distance_[channel_index] = 0;
//				if(set_noise_point_intensity == 1)
//				{
//					remove_noise_intensity[azimuth_index][channel_index] = 255;
//				}
//				else
//				{
					block_data_shared[azimuth_index].distance_[channel_index].distance = 0;
//				}
				(*noise_point_cnt)++;
			}
		}
	}
}

//void RemoveNoise(ConvertedDataBlock current_frame_blocks[FRAME_DATA_COUNT], ConvertedDataBlock last_frame_blocks[FRAME_DATA_COUNT], int bottom_horizontal_offset, volatile RawDataBlock *block_data_shared, uint8_t is_bottom_horizontal_offset_changed, uint32_t frame_count)
//{
///A
