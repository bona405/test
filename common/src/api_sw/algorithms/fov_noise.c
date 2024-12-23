/*
 * fov_noise.c
 *
 *  Created on: 2023. 5. 30.
 *      Author: HP
 */

/*
 * fov_data_set.c
 *
 *  Created on: 2022. 7. 26.
 *      Author: Wonju
 */


#include "fov_noise.h"
#include "../../api_hw/api_hw_enc_ip_ctrl.h"
#include "math.h"

#define BOTTOM_MOTOR_OFFSET 128
#define TOP_MOTOR_OFFSET 2174

#define MAX_DISTANCE1 12
#define MAX_DISTANCE2 12

NoiseDatas noisemap[10000];

void SetAzimuthAndDistanceA3_test3(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt)
{

	memset(&noisemap, 0x0, sizeof(NoiseDatas) * 10000);



	int32_t top_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;
	int32_t bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;

	int32_t clip_min = distance_offset;
	int32_t current_distance = 0;

//	uint8_t is_start_vert_angle_even = 0;
//	if(fov_test_.start_vertical_angle % 2 == 0)
//		is_start_vert_angle_even = 1;
//	uint8_t is_end_vert_angle_even = 0;
//	if(fov_test_.end_vertical_angle % 2 == 0)
//		is_end_vert_angle_even = 1;

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index++)
	{
//		//bottom
		block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176);
		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared[azimuth_index].azimuth_ * 0.001;
		block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176);
		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared[azimuth_index + top_bottom_offset].azimuth_ * 0.001;

		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; vertical_angle_index++)
		{
			//bottom

//			current_distance = block_data_shared->block_data_bram[azimuth_index].distance_[vertical_angle_index] + routing_delay[vertical_angle_index] - clip_min;
			current_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index] - clip_min;
//			if(current_distance <= 0 || current_distance > MAX_DISTANCE  || (vertical_angle_index < 4 && current_distance > 51200) || (vertical_angle_index > 11 && current_distance > 51200) )
			if(current_distance <= 0 || current_distance > 58880  || (vertical_angle_index < 4 && current_distance > 45056) || (vertical_angle_index > 11 && current_distance > 45056) )
			{

				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
				noisemap[azimuth_index].isnoise[vertical_angle_index] = 1;
				noisemap[azimuth_index].ischecked[vertical_angle_index] = 1;
//				(*noise_point_cnt)++;
			}
			else
			{
//				if(is_end_vert_angle_even == 0)
//				{
//					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
//											vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
//					{
//						current_distance = 0;
//						block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
//						noisemap[azimuth_index].isnoise[vertical_angle_index] = 1;
//						noisemap[azimuth_index].ischecked[vertical_angle_index] = 1;
////						(*noise_point_cnt)++;
//					}
//				}
//				else
//				{
//					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
//												vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2)-1)
//					{
//						current_distance = 0;
//						block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
//						noisemap[azimuth_index].isnoise[vertical_angle_index] = 1;
//						noisemap[azimuth_index].ischecked[vertical_angle_index] = 1;
////						(*noise_point_cnt)++;
//					}
//				}

				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = current_distance;
//				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = current_distance / 2.56;
//				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance * 2.56;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = current_distance * DIV_256;
			}
			//top

			current_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;

//			if(current_distance <= 0 || current_distance > MAX_DISTANCE || (vertical_angle_index < 4 && current_distance > 51200) || (vertical_angle_index > 11 && current_distance > 51200))
			if(current_distance <= 0 || current_distance > 58880 || (vertical_angle_index < 4 && current_distance > 45056) || (vertical_angle_index > 11 && current_distance > 45056))
			{
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
				noisemap[azimuth_index + top_bottom_offset].isnoise[vertical_angle_index] = 1;
				noisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] = 1;
//				(*noise_point_cnt)++;
			}
			else
			{
//				if(is_start_vert_angle_even == 0)
//				{
//					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
//						vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
//					{
//						current_distance = 0;
//						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
//						noisemap[azimuth_index + top_bottom_offset].isnoise[vertical_angle_index] = 1;
//						noisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] = 1;
////						(*noise_point_cnt)++;
//					}
//
//				}
//				else
//				{
//					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
//						vertical_angle_index < (fov_test_.start_vertical_angle / 2) - 1 || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
//					{
//						current_distance = 0;
//						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
//						noisemap[azimuth_index + top_bottom_offset].isnoise[vertical_angle_index] = 1;
//						noisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] = 1;
////						(*noise_point_cnt)++;
//					}
//
//				}
//
//				if(is_end_vert_angle_even == 0)
//				{
//					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
//					{
//						current_distance = 0;
//						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
//						noisemap[azimuth_index + top_bottom_offset].isnoise[vertical_angle_index] = 1;
//						noisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] = 1;
////						(*noise_point_cnt)++;
//					}
//				}
//				else
//				{
//					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
//					{
//						current_distance = 0;
//						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
//						noisemap[azimuth_index + top_bottom_offset].isnoise[vertical_angle_index] = 1;
//						noisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] = 1;
////						(*noise_point_cnt)++;
//					}
//				}

				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = current_distance;
//				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = current_distance / 2.56;
//				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance * 2.56;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = current_distance * DIV_256;
			}
		}
	}
//	return;
//	volatile unsigned int Noise_cot = (*noise_point_cnt);
	Nodes nodes[48];//36+12
//	if(intenfilter)	//new noise filter
	{
		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				//if already checked, pass
				if(noisemap[azimuth_index].ischecked[vertical_angle_index] == 1 || noisemap[azimuth_index].isnoise[vertical_angle_index] == 1)
				{
					if(noisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] == 1 || noisemap[azimuth_index + top_bottom_offset].isnoise[vertical_angle_index] == 1)
					{
						continue;
					}
				}

				memset(&nodes, 0x0, sizeof(Nodes)*48);
				int stop_cot = azimuth_index % 3;
				int top_start_azimuth = azimuth_index + top_bottom_offset -3 + (-1 * stop_cot);

				int bottom_start_azimuth = azimuth_index -6 +  (-1 * stop_cot);
				//init nodes
				{
					for(int i = 0; i < 9; ++i)	//top
					{
						//nodes[i].azimuth = -1;
						//nodes[i].ch = -1;

						//nodes[i + 9].azimuth = -1;
						//nodes[i + 9].ch = - 1;
						//continue;
						if(top_start_azimuth + i < top_bottom_offset || top_start_azimuth + i >= azimuth_count*2)
						{
							nodes[i].azimuth = -1;
							nodes[i].ch = -1;

							nodes[i + 9].azimuth = -1;
							nodes[i + 9].ch = - 1;
						}
						else
						{
							nodes[i].azimuth = top_start_azimuth + i;
							nodes[i].ch = vertical_angle_index;
//							block_data_shared[top_start_azimuth + i].distance_[vertical_angle_index].distance = 0;
//							block_data_shared[top_start_azimuth + i].distance_[vertical_angle_index].pulse_width = 0;
							if(vertical_angle_index == 0)
							{
								nodes[i + 9].azimuth = -1;
								nodes[i + 9].ch = - 1;
							}
							else
							{
								nodes[i + 9].azimuth = top_start_azimuth + i;
								nodes[i + 9].ch = vertical_angle_index - 1;

//								block_data_shared[top_start_azimuth + i].distance_[vertical_angle_index-1].distance = 0;
//								block_data_shared[top_start_azimuth + i].distance_[vertical_angle_index-1].pulse_width = 0;
							}
						}
					}

					for(int i = 18; i < 24; ++i)	//bottom same ch left
					{
						if(bottom_start_azimuth + i - 18 < 0 || bottom_start_azimuth + i - 18 >= azimuth_count)
						{
							nodes[i].azimuth = -1;
							nodes[i].ch = -1;
						}
						else
						{
							nodes[i].azimuth = bottom_start_azimuth + i -18;
							nodes[i].ch = vertical_angle_index;
						}
					}

					for(int i = 24; i < 30; ++i)	//bottom same ch right
					{
						if(bottom_start_azimuth + i -18 < 0 || bottom_start_azimuth + i -18 >= azimuth_count)
						{
							nodes[i].azimuth = -1;
							nodes[i].ch = -1;
						}
						else
						{
							nodes[i].azimuth = bottom_start_azimuth + 9 + i - 24;
							nodes[i].ch = vertical_angle_index;
						}
					}

//					for(int i = 0; i < 3; ++i)	//bottom upper, lower ch
					for(int i = 0; i < 9; ++i)	//bottom upper, lower ch
					{
						if(vertical_angle_index == 15)
						{
							nodes[30 + i].azimuth = -1;
							nodes[30 + i].ch = -1;
						}
						else
						{
//							nodes[30 + i].azimuth = bottom_start_azimuth + 6 + i;
							nodes[30 + i].azimuth = bottom_start_azimuth + 3 + i;
							nodes[30 + i].ch = vertical_angle_index + 1;
						}

						if(vertical_angle_index == 0)
						{
							nodes[39 + i].azimuth = -1;
							nodes[39 + i].ch = - 1;
						}
						else
						{
//							nodes[33 + i].azimuth = bottom_start_azimuth + 6 + i;
							nodes[39 + i].azimuth = bottom_start_azimuth + 3 + i;
							nodes[39 + i].ch = vertical_angle_index - 1;
						}
					}
				}


				//1. top check (upper ch, 3azimuth,3stop)
				int real_cot = 0;
				if(block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width > 60)
				{
					++real_cot;
					++real_cot;
				}
				else if(block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width > 29)
				{
					++real_cot;
				}

				for(int ind = 0; ind < 48; ++ind)
				{
					int now_azimuth = nodes[ind].azimuth;
					int now_ch = nodes[ind].ch;
					if(now_azimuth < 0 || now_ch < 0)
					{
						continue;
					}
//					block_data_shared[now_azimuth].distance_[now_ch].distance = 0;
//					block_data_shared[now_azimuth].distance_[now_ch].pulse_width = 0;
					if(noisemap[now_azimuth].isnoise[now_ch] == 1)
					{
						continue;
					}

					int distance_neighbor = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance - block_data_shared[now_azimuth].distance_[now_ch].distance;
//					if(distance_neighbor > 39 || distance_neighbor < -39)
//					if(distance_neighbor > 31 || distance_neighbor < -31)
//					if(distance_neighbor > 52 || distance_neighbor < -52)
					if(distance_neighbor > 116 || distance_neighbor < -116)
					{
						continue;
					}

					if(noisemap[now_azimuth].ischecked[now_ch] == 1)
					{
						//point real
						++real_cot;
//						nodes[ind].cot = real_cot;
//						continue;
					}

					++real_cot;
					 nodes[ind].cot = real_cot;
				}

				if(real_cot >=3) //real point
				{
					for(int ind = 0; ind < 48; ++ind)
					{
						if(nodes[ind].cot >= 1)
						{
							int now_azimuth = nodes[ind].azimuth;
							int now_ch = nodes[ind].ch;

							noisemap[now_azimuth].ischecked[now_ch] = 1;
						}

//						nodes[ind].cot = 0;
					}
//					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
//					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;

				}
				else //noise point
				{
					noisemap[azimuth_index].isnoise[vertical_angle_index] = 1;
					noisemap[azimuth_index].ischecked[vertical_angle_index] = 1;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
					(*noise_point_cnt)++;
//					for(int ind = 0; ind < 36; ++ind)
//					{
//						nodes[ind].cot = 0;
//					}
				}

			}
		}
//		return;
		for(int azimuth_index = top_bottom_offset ; azimuth_index < azimuth_count*2 ; ++azimuth_index)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				if(noisemap[azimuth_index].isnoise[vertical_angle_index] == 1)
				{
					continue;
				}
				if(noisemap[azimuth_index].ischecked[vertical_angle_index] == 0)
				{
					int istopnoise = 1;
					for(int ind = 0; ind <9; ++ind)
					{
						int stop_cot = azimuth_index % 3;
						int bottom_azimuth = ind + azimuth_index -top_bottom_offset +  (-1 * stop_cot);

						if(bottom_azimuth < 0 || bottom_azimuth>=azimuth_count)
						{
							continue;
						}

						if(noisemap[bottom_azimuth].isnoise[vertical_angle_index] == 1)
						{
							continue;
						}

						int distance_neighbor = block_data_shared[bottom_azimuth].distance_[vertical_angle_index].distance - block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
//						if(distance_neighbor > -52 && distance_neighbor < 52)
						if(distance_neighbor > -116 && distance_neighbor < 116)
						{
							istopnoise = 0;
							break;
						}

						if(vertical_angle_index != 15)
						{
							if(noisemap[bottom_azimuth].isnoise[vertical_angle_index+1] == 1)
							{
								continue;
							}
							distance_neighbor = block_data_shared[bottom_azimuth].distance_[vertical_angle_index+1].distance - block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
//							if(distance_neighbor > -52 && distance_neighbor < 52)
							if(distance_neighbor > -116 && distance_neighbor < 116)
							{
								istopnoise = 0;
								break;
							}
						}
					}

					if(istopnoise)
					{
						block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
						block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
						(*noise_point_cnt)++;
					}
				}
			}
		}


	}
}



SimpleNodes2 realbotnodes[3];//18+12
SimpleNodes2 realtopnodes[3];//18+12

SimpleNodes2 postbotnodes2[54];//54
SimpleNodes2 posttopnodes2[54];//54

SimpleNoiseDatas simplenoisemap[10000];


int is_sparse_botnode_init3 = 0;
int is_sparse_topnode_init3 = 0;
int pretop_bottom_offset3 = 0;
//SimpleNodes2 prebotnodes3[30];//18
//SimpleNodes2 pretopnodes3[30];//18
SimpleNodes2 prebotnodes3[24];//18
SimpleNodes2 pretopnodes3[24];//18
void SetAzimuthAndDistanceA3_Sparse(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt, RawDataBlock *last_block_data)
{
	memset(&simplenoisemap, 0x0, sizeof(SimpleNoiseDatas) * 10000);

	if(!is_sparse_botnode_init3 || top_bottom_offset != pretop_bottom_offset3)
	{
		for(int i = 0; i < 3; ++i)
		{
			prebotnodes3[i].azimuthoffset = i + top_bottom_offset;
			prebotnodes3[i].choffset = 0;

			prebotnodes3[i+3].azimuthoffset = i + top_bottom_offset;
			prebotnodes3[i+3].choffset = -1;
		}

		for(int i = 6; i < 9; ++i)
		{
			prebotnodes3[i].azimuthoffset = -3 + i - 6; //leftup
			prebotnodes3[i].choffset = 1;

			prebotnodes3[i+3].azimuthoffset = 3 + i - 6; //rightup
			prebotnodes3[i+3].choffset = 1;

			prebotnodes3[i+6].azimuthoffset = -3 + i - 6; //leftdown
			prebotnodes3[i+6].choffset = -1;

			prebotnodes3[i+9].azimuthoffset = 3 + i - 6; //rightdown
			prebotnodes3[i+9].choffset = -1;

			prebotnodes3[i+12].azimuthoffset = i + top_bottom_offset - 6; //up top
			prebotnodes3[i+12].choffset = 1;

			prebotnodes3[i+15].azimuthoffset = i + top_bottom_offset - 6; //down top
			prebotnodes3[i+15].choffset = -2;


//			prebotnodes3[i+12].azimuthoffset = -9 + i - 6; //leftup
//			prebotnodes3[i+12].choffset = 1;
//
//			prebotnodes3[i+15].azimuthoffset = 9 + i - 6; //rightup
//			prebotnodes3[i+15].choffset = 1;

//			prebotnodes3[i+18].azimuthoffset = -9 + i - 6; //leftdown
//			prebotnodes3[i+18].choffset = -1;
//
//			prebotnodes3[i+21].azimuthoffset = 9 + i - 6; //rightdown
//			prebotnodes3[i+21].choffset = -1;
		}

		int postnodes_idx = 0;

		for(int i = 3; i < 10; i += 3)
		{
			for(int j = 0; j < 3; ++j)
			{
				//left
				postbotnodes2[postnodes_idx +j].azimuthoffset = -1*i + j;
				postbotnodes2[postnodes_idx +j].choffset = 0;

				postbotnodes2[postnodes_idx+3 +j].azimuthoffset = -1*i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+3 +j].choffset = 0;

				postbotnodes2[postnodes_idx+6 +j].azimuthoffset = -1*i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+6 +j].choffset = -1;


				//right
				postbotnodes2[postnodes_idx+9 +j].azimuthoffset = i + j;
				postbotnodes2[postnodes_idx+9 +j].choffset = 0;

				postbotnodes2[postnodes_idx+12 +j].azimuthoffset = i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+12 +j].choffset = 0;

				postbotnodes2[postnodes_idx+15 +j].azimuthoffset = i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+15 +j].choffset = -1;
			}
			postnodes_idx+=18;
		}

		is_sparse_botnode_init3 = 1;


	}

	if(!is_sparse_topnode_init3 || top_bottom_offset != pretop_bottom_offset3)
	{
		for(int i = 0; i<3; ++i)
		{
			pretopnodes3[i].azimuthoffset = i - top_bottom_offset;
			pretopnodes3[i].choffset = 1;

			pretopnodes3[i+3].azimuthoffset = i - top_bottom_offset;
			pretopnodes3[i+3].choffset = 0;
		}

		for(int i = 6; i < 9; ++i)
		{
			pretopnodes3[i].azimuthoffset = -3 + i - 6; //leftup
			pretopnodes3[i].choffset = 1;

			pretopnodes3[i+3].azimuthoffset = 3 + i - 6; //rightup
			pretopnodes3[i+3].choffset = 1;

			pretopnodes3[i+6].azimuthoffset = -3 + i - 6; //leftdown
			pretopnodes3[i+6].choffset = 0;

			pretopnodes3[i+9].azimuthoffset = 3 + i - 6; //rightdown
			pretopnodes3[i+9].choffset = 0;

			pretopnodes3[i+12].azimuthoffset = i - top_bottom_offset - 6; //up bottom
			pretopnodes3[i+12].choffset = 2;

			pretopnodes3[i+15].azimuthoffset = i - top_bottom_offset - 6 ; //down bottom
			pretopnodes3[i+15].choffset = -1;

//			pretopnodes3[i+12].azimuthoffset = -9 + i - 6; //leftup
//			pretopnodes3[i+12].choffset = 1;
//
//			pretopnodes3[i+15].azimuthoffset = 9 + i - 6 ; //rightup
//			pretopnodes3[i+15].choffset = 1;
//
//			pretopnodes3[i+18].azimuthoffset = -9 + i - 6; //leftdown
//			pretopnodes3[i+18].choffset = -1;
//
//			pretopnodes3[i+21].azimuthoffset = 9 + i - 6 ; //rightdown
//			pretopnodes3[i+21].choffset = -1;
		}


		int postnodes_idx = 0;

		for(int i = 3; i < 10; i += 3)
		{
			for(int j = 0; j < 3; ++j)
			{
				//left
				posttopnodes2[postnodes_idx +j].azimuthoffset = -1*i + j;
				posttopnodes2[postnodes_idx +j].choffset = 0;

				posttopnodes2[postnodes_idx+3 +j].azimuthoffset = -1*i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+3 +j].choffset = 1;

				posttopnodes2[postnodes_idx+6 +j].azimuthoffset = -1*i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+6 +j].choffset = 0;


				//right
				posttopnodes2[postnodes_idx+9 +j].azimuthoffset = i + j;
				posttopnodes2[postnodes_idx+9 +j].choffset = 0;

				posttopnodes2[postnodes_idx+12 +j].azimuthoffset = i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+12 +j].choffset = 1;

				posttopnodes2[postnodes_idx+15 +j].azimuthoffset = i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+15 +j].choffset = 0;
			}
			postnodes_idx+=18;
		}


		is_sparse_topnode_init3 = 1;

		pretop_bottom_offset3 = top_bottom_offset;
	}




	int32_t top_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;
	int32_t bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;

	int32_t clip_min = distance_offset;


	int botcurrent_distance = 0;
	int topcurrent_distance = 0;

	const int azimuth_count_2 = azimuth_count*2;

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
	{
//		//bottom
		block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176);
		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared[azimuth_index].azimuth_ * 0.001;
		block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176);
		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared[azimuth_index + top_bottom_offset].azimuth_ * 0.001;

		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; ++vertical_angle_index)
		{
			//bottom
			botcurrent_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index] - clip_min;
			if(botcurrent_distance <= 0 || botcurrent_distance > 58880  || (vertical_angle_index < 4 && botcurrent_distance > 45056) || (vertical_angle_index > 11 && botcurrent_distance > 45056) )
			{
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
//				noisemap[azimuth_index].isnoise[vertical_angle_index] = 1;
				simplenoisemap[azimuth_index].ischecked[vertical_angle_index] = 1;
				botcurrent_distance = 0;
			}
			else
			{
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = botcurrent_distance;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = botcurrent_distance * DIV_256;
			}

			//top
			topcurrent_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;
			if(topcurrent_distance <= 0 || topcurrent_distance > 58880 || (vertical_angle_index < 4 && topcurrent_distance > 45056) || (vertical_angle_index > 11 && topcurrent_distance > 45056))
			{
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
//				noisemap[azimuth_index + top_bottom_offset].isnoise[vertical_angle_index] = 1;
				simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] = 1;
				topcurrent_distance = 0;
			}
			else
			{
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = topcurrent_distance;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = topcurrent_distance * DIV_256;
			}
		}
	}

	{
		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				//if already checked, pass
				int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;

				int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int botcot = 0;
				int topcot = 0;

				int botalready = 0;
				int topalready = 0;

//				if(botdistance == 0 || simplenoisemap[azimuth_index].ischecked[vertical_angle_index])
				if(simplenoisemap[azimuth_index].ischecked[vertical_angle_index])
				{
					botcot = 10;
					botalready = 1;
				}

//				if(topdistance == 0 || simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index])
				if(simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index])
				{
					topcot = 10;
					topalready = 1;
				}

				if(botcot==0)
				{
					int botintensity = block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width;

					if(botintensity > 79)
					{
						++botcot;
						++botcot;
					}
					else if (botintensity > 49)
					{
						++botcot;
						++botcot;
					}
//					else if (botintensity != 0 && botintensity < 20)
//					{
//						botcot--;
//					}
					else if (botintensity > 30)
					{
						++botcot;
					}

//					for(int i = 0; i<18; ++i)
//					{
//						prebotnodes2[i].cot = 0;
//					}
//
//					for(int i = 0; i<54; ++i)
//					{
//						postbotnodes2[i].cot = 0;
//					}

					for(int i = 0; i<3; ++i)
					{
						realbotnodes[i].azimuthoffset = 0;
						realbotnodes[i].choffset = 0;
					}
				}

				if(topcot==0)
				{
					int topintensity = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width;

					if(topintensity > 79)
					{
						++topcot;
						++topcot;
					}
					else if (topintensity > 49)
					{
						++topcot;
						++topcot;
					}
					else if (topintensity >30)
					{
						++topcot;
					}
//					else if (topintensity != 0 && topintensity < 20)
//					{
//						topcot--;
//					}

//					for(int i = 0; i<18; ++i)
//					{
//						pretopnodes2[i].cot = 0;
//					}
//
//					for(int i = 0; i<54; ++i)
//					{
//						posttopnodes2[i].cot = 0;
//					}
					
					for(int i = 0; i<3; ++i)
					{
						realtopnodes[i].azimuthoffset = 0;
						realtopnodes[i].choffset = 0;
					}
				}


				int stop_cot = azimuth_index % 3;
				int bot_now_azimuth = azimuth_index + (-1 * stop_cot);
				int top_now_azimuth = bot_now_azimuth + top_bottom_offset;


//				int botdistance_threshold = 250;
//				int pre_botdistance_threshold = 128;
//				if(botdistance < 257)
//				{
//					botdistance_threshold = 38;
//					pre_botdistance_threshold = 12;
//
//				}
//
//				int topdistance_threshold = 250;
//				int pre_topdistance_threshold = 128;
//				if(topdistance < 257)
//				{
//					topdistance_threshold = 38;
//					pre_topdistance_threshold = 12;
//
//				}

				int botdistance_threshold = 204;
//				int pre_botdistance_threshold = 128;
				if(botdistance < 257)
				{
					botdistance_threshold = 38;
//					pre_botdistance_threshold = 12;

				}

				int topdistance_threshold = 204;
//				int pre_topdistance_threshold = 128;
				if(topdistance < 257)
				{
					topdistance_threshold = 38;
//					pre_topdistance_threshold = 12;

				}

				for(int i = 0; i<3; ++i)
				{
					if(botcot<3)
					{//bot
						int nowazimuth = bot_now_azimuth + i;
						int nowch = vertical_angle_index;
						if(nowazimuth < azimuth_count_2 && nowazimuth >=0 && nowch>=0 && nowch<16 && botdistance > 1000)
						{
							int neighbordistance = botdistance - last_block_data[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
							{
								if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49)
								{
									++botcot;
								}
								++botcot;
								++botcot;
//								prebotnodes2[i].cot = 1;
							}
						}
					}

					if(topcot<3)
					{//top
						int nowazimuth = top_now_azimuth + i;
						int nowch = vertical_angle_index;
//						if(nowazimuth < azimuth_count_2 && nowazimuth >= top_bottom_offset && nowch>=0 && nowch<16)
						if(nowazimuth < azimuth_count_2 && nowazimuth >= 0 && nowch>=0 && nowch<16 && topdistance > 1000)
						{
							int neighbordistance = topdistance - last_block_data[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
							{
								if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49)
								{
									++topcot;
								}
								++topcot;
								++topcot;
//								pretopnodes2[i].cot = 1;
							}
						}
					}
					if(botcot>2 && topcot>2)
					{
						break;
					}
				}

				int botrealcot = 0;
				int toprealcot = 0;

				for(int i = 0; i<24; ++i)
				{
					if(botcot<3)
					{//bot
						int nowazimuth = bot_now_azimuth + prebotnodes3[i].azimuthoffset;
						int nowch = vertical_angle_index + prebotnodes3[i].choffset;
//						if(nowazimuth < azimuth_count && nowazimuth >=0 && nowch>=0 && nowch<16)
						if(nowazimuth < azimuth_count_2 && nowazimuth >=0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = botdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
							{
								realbotnodes[botrealcot].azimuthoffset = nowazimuth;
								realbotnodes[botrealcot].choffset = nowch;
								++botrealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49)
								{
									++botcot;
								}
								++botcot;
								++botcot;
//								prebotnodes2[i].cot = 1;
							}
							if(botcot<3 && botdistance > 1000)
							{
								neighbordistance = botdistance - last_block_data[nowazimuth].distance_[nowch].distance;
								if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
								{
//									realbotnodes[botrealcot].azimuthoffset = nowazimuth;
//									realbotnodes[botrealcot].choffset = nowch;
//									++botrealcot;

									if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49)
									{
										++botcot;
									}
									++botcot;
									++botcot;
	//								prebotnodes2[i].cot +=1;
								}
							}

						}
					}

					if(topcot<3)
					{//top
						int nowazimuth = top_now_azimuth + pretopnodes3[i].azimuthoffset;
						int nowch = vertical_angle_index + pretopnodes3[i].choffset;
//						if(nowazimuth < azimuth_count_2 && nowazimuth >= top_bottom_offset && nowch>=0 && nowch<16)
						if(nowazimuth < azimuth_count_2 && nowazimuth >= 0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = topdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
							{
								realtopnodes[toprealcot].azimuthoffset = nowazimuth;
								realtopnodes[toprealcot].choffset = nowch;
								++toprealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49)
								{
									++topcot;
								}
								++topcot;
								++topcot;
//								pretopnodes2[i].cot = 1;
							}
							if(topcot<3 && topdistance > 1000)
							{
								neighbordistance = topdistance - last_block_data[nowazimuth].distance_[nowch].distance;
								if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
								{
//									realtopnodes[toprealcot].azimuthoffset = nowazimuth;
//									realtopnodes[toprealcot].choffset = nowch;
//									++toprealcot;

									if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49)
									{
										++topcot;
									}
									++topcot;
									++topcot;
	//								pretopnodes2[i].cot +=1;
								}
							}
						}
					}
					if(botcot>2 && topcot>2)
					{
						break;
					}
				}

//				int botpredone = 0;
//				int toppredone = 0;
//
//				if(botcot>2)
//				{
//					botpredone = 1;
//				}
//
//				if(topcot>2)
//				{
//					toppredone = 1;
//				}


//				botdistance_threshold = 256;
				botdistance_threshold = 296;
				if(botdistance < 257)
				{
					botdistance_threshold = 38;

				}
				else if(botdistance < 2000)
				{
					botdistance_threshold = 153;
				}

//				topdistance_threshold = 256;
				topdistance_threshold = 296;
				if(topdistance < 257)
				{
					topdistance_threshold = 38;
				}
				else if (topdistance < 2000)
				{
					topdistance_threshold = 153;
				}

				for(int i = 0; i<54; ++i)
				{
					if(botcot<3)
					{
						int nowazimuth = bot_now_azimuth + postbotnodes2[i].azimuthoffset;
						int nowch = vertical_angle_index + postbotnodes2[i].choffset;
//						if(nowazimuth < azimuth_count && nowazimuth >=0 && nowch>=0 && nowch<16)
						if(nowazimuth < azimuth_count_2 && nowazimuth >=0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = botdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
							{
								realbotnodes[botrealcot].azimuthoffset = nowazimuth;
								realbotnodes[botrealcot].choffset = nowch;
								++botrealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49)
								{
									++botcot;
								}
								++botcot;
//								postbotnodes2[i].cot =1;
							}
//							if(botcot<3 && botdistance > 1000)
//							{
//								neighbordistance = botdistance - last_block_data[nowazimuth].distance_[nowch].distance;
//								if(neighbordistance > -1*botdistance_threshold && neighbordistance <botdistance_threshold)
//								{
//									realbotnodes[botrealcot].azimuthoffset = nowazimuth;
//									realbotnodes[botrealcot].choffset = nowch;
//									botrealcot++;
//
//									botcot++;
//									botcot++;
//	//								prebotnodes2[i].cot +=1;
//								}
//							}
						}
					}

					if(topcot<3)
					{
						int nowazimuth = top_now_azimuth + posttopnodes2[i].azimuthoffset;
						int nowch = vertical_angle_index + posttopnodes2[i].choffset;
//						if(nowazimuth < azimuth_count_2 && nowazimuth >= top_bottom_offset && nowch>=0 && nowch<16)
						if(nowazimuth < azimuth_count_2 && nowazimuth >= 0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = topdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
							{
								realtopnodes[toprealcot].azimuthoffset = nowazimuth;
								realtopnodes[toprealcot].choffset = nowch;
								++toprealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49)
								{
									++topcot;
								}
								++topcot;
//								posttopnodes2[i].cot =1;
							}
//							if(topcot<3 && topdistance > 1000)
//							{
//								neighbordistance = topdistance - last_block_data[nowazimuth].distance_[nowch].distance;
//								if(neighbordistance > -1*topdistance_threshold && neighbordistance <topdistance_threshold)
//								{
//									realtopnodes[botrealcot].azimuthoffset = nowazimuth;
//									realtopnodes[botrealcot].choffset = nowch;
//									toprealcot++;
//
//									topcot++;
//									topcot++;
//	//								pretopnodes2[i].cot +=1;
//								}
//							}
						}
					}
					if(botcot>2 && topcot>2)
					{
						break;
					}
				}


				if(botalready==0 && botcot>2)
				{
					for(int i = 0; i < botrealcot; ++i)
					{
						simplenoisemap[realbotnodes[i].azimuthoffset].ischecked[realbotnodes[i].choffset] = 1;
					}
//					for(int i = 0; i<18; ++i)
//					{
//						if(prebotnodes2[i].cot)
//						{
//							int nowazimuth = bot_now_azimuth + prebotnodes2[i].azimuthoffset;
//							int nowch = vertical_angle_index + prebotnodes2[i].choffset;
//							noisemap[nowazimuth].ischecked[nowch] = 1;
//						}
//					}
//					if(botpredone==0)
//					{
//						for(int i = 0; i<54; ++i)
//						{
//							if(postbotnodes2[i].cot)
//							{
//								int nowazimuth = bot_now_azimuth + postbotnodes2[i].azimuthoffset;
//								int nowch = vertical_angle_index + postbotnodes2[i].choffset;
//								noisemap[nowazimuth].ischecked[nowch] = 1;
//							}
//						}
//					}
				}
				else if(botcot < 3 && botalready == 0)
				// if(botcot < 3 && botalready == 0)
				{
					//noise
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
//					current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;

					(*noise_point_cnt)++;
				}


				if(topalready==0 && topcot>2)
				{
					for(int i = 0; i < toprealcot; ++i)
					{
						simplenoisemap[realtopnodes[i].azimuthoffset].ischecked[realtopnodes[i].choffset] = 1;
					}
//					for(int i = 0; i<18; ++i)
//					{
//						if(pretopnodes2[i].cot)
//						{
//							int nowazimuth = top_now_azimuth + pretopnodes2[i].azimuthoffset;
//							int nowch = vertical_angle_index + pretopnodes2[i].choffset;
//							noisemap[nowazimuth].ischecked[nowch] = 1;
//						}
//					}
//					if(toppredone==0)
//					{
//						for(int i = 0; i<54; ++i)
//						{
//							if(posttopnodes2[i].cot)
//							{
//								int nowazimuth = bot_now_azimuth + posttopnodes2[i].azimuthoffset;
//								int nowch = vertical_angle_index + posttopnodes2[i].choffset;
//								noisemap[nowazimuth].ischecked[nowch] = 1;
//							}
//						}
//					}
				}
				else if(topcot < 3 && topalready == 0)
				// if(topcot < 3 && topalready == 0)
				{
					//noise
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
//					current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;

					(*noise_point_cnt)++;
				}

			}
		}
//		return;
	}
}


void SetAzimuthAndDistanceB1_Sparse(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt, RawDataBlock *last_block_data, int hz_option, int* routing_delay2, int routing_range_1, int routing_range_2)
{


	memset(&simplenoisemap, 0x0, sizeof(SimpleNoiseDatas) * 10000);

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
//		max_distance1 = 44672;
		max_distance1 = 51200;
	}


	if(!is_sparse_botnode_init3 || top_bottom_offset != pretop_bottom_offset3)
	{
		for(int i = 0; i < 3; ++i)
		{
			prebotnodes3[i].azimuthoffset = i + top_bottom_offset;
			prebotnodes3[i].choffset = 0;

			prebotnodes3[i+3].azimuthoffset = i + top_bottom_offset;
			prebotnodes3[i+3].choffset = -1;
		}

		for(int i = 6; i < 9; ++i)
		{
			prebotnodes3[i].azimuthoffset = -3 + i - 6; //leftup
			prebotnodes3[i].choffset = 1;

			prebotnodes3[i+3].azimuthoffset = 3 + i - 6; //rightup
			prebotnodes3[i+3].choffset = 1;

			prebotnodes3[i+6].azimuthoffset = -3 + i - 6; //leftdown
			prebotnodes3[i+6].choffset = -1;

			prebotnodes3[i+9].azimuthoffset = 3 + i - 6; //rightdown
			prebotnodes3[i+9].choffset = -1;

			prebotnodes3[i+12].azimuthoffset = i + top_bottom_offset - 6; //up top
			prebotnodes3[i+12].choffset = 1;

			prebotnodes3[i+15].azimuthoffset = i + top_bottom_offset - 6; //down top
			prebotnodes3[i+15].choffset = -2;
		}

		int postnodes_idx = 0;

		for(int i = 3; i < 10; i += 3)
		{
			for(int j = 0; j < 3; ++j)
			{
				//left
				postbotnodes2[postnodes_idx +j].azimuthoffset = -1*i + j;
				postbotnodes2[postnodes_idx +j].choffset = 0;

				postbotnodes2[postnodes_idx+3 +j].azimuthoffset = -1*i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+3 +j].choffset = 0;

				postbotnodes2[postnodes_idx+6 +j].azimuthoffset = -1*i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+6 +j].choffset = -1;


				//right
				postbotnodes2[postnodes_idx+9 +j].azimuthoffset = i + j;
				postbotnodes2[postnodes_idx+9 +j].choffset = 0;

				postbotnodes2[postnodes_idx+12 +j].azimuthoffset = i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+12 +j].choffset = 0;

				postbotnodes2[postnodes_idx+15 +j].azimuthoffset = i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+15 +j].choffset = -1;
			}
			postnodes_idx+=18;
		}

		is_sparse_botnode_init3 = 1;


	}

	if(!is_sparse_topnode_init3 || top_bottom_offset != pretop_bottom_offset3)
	{
		for(int i = 0; i<3; ++i)
		{
			pretopnodes3[i].azimuthoffset = i - top_bottom_offset;
			pretopnodes3[i].choffset = 1;

			pretopnodes3[i+3].azimuthoffset = i - top_bottom_offset;
			pretopnodes3[i+3].choffset = 0;
		}

		for(int i = 6; i < 9; ++i)
		{
			pretopnodes3[i].azimuthoffset = -3 + i - 6; //leftup
			pretopnodes3[i].choffset = 1;

			pretopnodes3[i+3].azimuthoffset = 3 + i - 6; //rightup
			pretopnodes3[i+3].choffset = 1;

			pretopnodes3[i+6].azimuthoffset = -3 + i - 6; //leftdown
			pretopnodes3[i+6].choffset = 0;

			pretopnodes3[i+9].azimuthoffset = 3 + i - 6; //rightdown
			pretopnodes3[i+9].choffset = 0;

			pretopnodes3[i+12].azimuthoffset = i - top_bottom_offset - 6; //up bottom
			pretopnodes3[i+12].choffset = 2;

			pretopnodes3[i+15].azimuthoffset = i - top_bottom_offset - 6 ; //down bottom
			pretopnodes3[i+15].choffset = -1;
		}


		int postnodes_idx = 0;

		for(int i = 3; i < 10; i += 3)
		{
			for(int j = 0; j < 3; ++j)
			{
				//left
				posttopnodes2[postnodes_idx +j].azimuthoffset = -1*i + j;
				posttopnodes2[postnodes_idx +j].choffset = 0;

				posttopnodes2[postnodes_idx+3 +j].azimuthoffset = -1*i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+3 +j].choffset = 1;

				posttopnodes2[postnodes_idx+6 +j].azimuthoffset = -1*i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+6 +j].choffset = 0;


				//right
				posttopnodes2[postnodes_idx+9 +j].azimuthoffset = i + j;
				posttopnodes2[postnodes_idx+9 +j].choffset = 0;

				posttopnodes2[postnodes_idx+12 +j].azimuthoffset = i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+12 +j].choffset = 1;

				posttopnodes2[postnodes_idx+15 +j].azimuthoffset = i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+15 +j].choffset = 0;
			}
			postnodes_idx+=18;
		}


		is_sparse_topnode_init3 = 1;

		pretop_bottom_offset3 = top_bottom_offset;
	}


//	int32_t top_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;

//	int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? ((float)lidar_state_.a2z_setting.fov_correction / 120000 *176):(176);
//	int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*((float)lidar_state_.a2z_setting.fov_correction / 120000)):(90000);

	int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? (lidar_state_.a2z_setting.fov_correction):(176);
	int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*(((float)lidar_state_.a2z_setting.fov_correction*684) / 120384)):(90000);

	int32_t top_motor_init_value = base_angle - ((EncCtrlGetStartEncCount2()-2048)) * resolution;
	int32_t bottom_motor_init_value = base_angle - EncCtrlGetStartEncCount1() * resolution;

//	int32_t top_motor_init_value = 90000 - ((EncCtrlGetStartEncCount2()-2048)) * 176;
//	int32_t bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;

	int32_t clip_min = distance_offset;

	uint8_t is_start_vert_angle_even = 0;
	if(fov_test_.start_vertical_angle % 2 == 0)
		is_start_vert_angle_even = 1;
	uint8_t is_end_vert_angle_even = 0;
	if(fov_test_.end_vertical_angle % 2 == 0)
		is_end_vert_angle_even = 1;


	int botcurrent_distance = 0;
	int topcurrent_distance = 0;

	const int azimuth_count_2 = azimuth_count*2;

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
	{
//		//bottom
//		block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176);
		block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * resolution);

		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared[azimuth_index].azimuth_ * 0.001;
//		block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176);
		block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * resolution);

		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared[azimuth_index + top_bottom_offset].azimuth_ * 0.001;

		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; ++vertical_angle_index)
		{
			//bottom
//			botcurrent_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index] - clip_min;
			int bottom_raw_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
			if(bottom_raw_distance < routing_range_1)
			{
				botcurrent_distance = bottom_raw_distance + routing_delay2[vertical_angle_index];
			}
			else if(bottom_raw_distance < routing_range_2)
			{
				float slope = (float)(routing_delay[vertical_angle_index] - routing_delay2[vertical_angle_index]) / (routing_range_2 - routing_range_1);
				float intercept = (float)routing_delay[vertical_angle_index] - routing_range_2 * slope;
				botcurrent_distance = bottom_raw_distance + slope * bottom_raw_distance + intercept;
			}
			else
			{
				botcurrent_distance = bottom_raw_distance + routing_delay[vertical_angle_index];
			}
			if(botcurrent_distance <= 0 || botcurrent_distance > max_distance0  || (vertical_angle_index < 4 && botcurrent_distance > max_distance1) || (vertical_angle_index > 11 && botcurrent_distance > max_distance1)
					|| block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width == 0)
//			if(botcurrent_distance <= 0 || botcurrent_distance > 58880  || (vertical_angle_index < 4 && botcurrent_distance > 45056) || (vertical_angle_index > 11 && botcurrent_distance > 45056) )
			{
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
				simplenoisemap[azimuth_index].ischecked[vertical_angle_index] = 1;
				botcurrent_distance = 0;
			}
			else
			{
				if(is_end_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
											vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						botcurrent_distance = 0;

				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
												vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2)-1)
						botcurrent_distance = 0;
				}
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = botcurrent_distance;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = botcurrent_distance * DIV_256;
			}

			//top
//			topcurrent_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;
			//topcurrent_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT];
			int top_raw_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;
			if(top_raw_distance < routing_range_1)
			{
				topcurrent_distance = top_raw_distance + routing_delay2[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT];
			}
			else if(top_raw_distance < routing_range_2)
			{
				float slope = (float)(routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - routing_delay2[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT]) / (routing_range_2 - routing_range_1);
				float intercept = (float)routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - routing_range_2 * slope;
				topcurrent_distance = top_raw_distance + slope * top_raw_distance + intercept;
			}
			else
			{
				topcurrent_distance = top_raw_distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT];
			}
//			if(topcurrent_distance <= 0 || topcurrent_distance > 58880 || (vertical_angle_index < 4 && topcurrent_distance > 45056) || (vertical_angle_index > 11 && topcurrent_distance > 45056))
			if(topcurrent_distance <= 0 || topcurrent_distance > max_distance0 || (vertical_angle_index < 4 && topcurrent_distance > max_distance1) || (vertical_angle_index > 11 && topcurrent_distance > max_distance1)
					|| block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width == 0)
			{
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
				simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] = 1;
				topcurrent_distance = 0;
			}
			else
			{
				if(is_start_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						topcurrent_distance = 0;
				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) - 1 || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						topcurrent_distance = 0;
				}

				if(is_end_vert_angle_even == 0)
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
						topcurrent_distance = 0;
				}
				else
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
						topcurrent_distance = 0;
				}
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = topcurrent_distance;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = topcurrent_distance * DIV_256;
			}
		}
	}

	{
		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				//if already checked, pass
				int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;

				int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int botcot = 0;
				int topcot = 0;

				int botalready = 0;
				int topalready = 0;

//				if(botdistance == 0 || simplenoisemap[azimuth_index].ischecked[vertical_angle_index])
				if(simplenoisemap[azimuth_index].ischecked[vertical_angle_index])
				{
					botcot = 10;
					botalready = 1;
				}

//				if(topdistance == 0 || simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index])
				if(simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index])
				{
					topcot = 10;
					topalready = 1;
				}

				if(botcot==0)
				{
					int botintensity = block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width;

					if(botintensity == 255)
					{
						//notghing or botcot--
						--botcot;
					}
					else if(botintensity > 79)
					{
						++botcot;
						++botcot;
					}
					else if (botintensity > 49)
					{
						++botcot;
						++botcot;
					}
					else if (botintensity > 30)
					{
						++botcot;
					}


					for(int i = 0; i<3; ++i)
					{
						realbotnodes[i].azimuthoffset = 0;
						realbotnodes[i].choffset = 0;
					}
				}

				if(topcot==0)
				{
					int topintensity = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width;

					if(topintensity == 255)
					{
						--topcot;
					}
					else if(topintensity > 79)
					{
						++topcot;
						++topcot;
					}
					else if (topintensity > 49)
					{
						++topcot;
						++topcot;
					}
					else if (topintensity >30)
					{
						++topcot;
					}


					for(int i = 0; i<3; ++i)
					{
						realtopnodes[i].azimuthoffset = 0;
						realtopnodes[i].choffset = 0;
					}
				}


				int stop_cot = azimuth_index % 3;
				int bot_now_azimuth = azimuth_index + (-1 * stop_cot);
				int top_now_azimuth = bot_now_azimuth + top_bottom_offset;


				int botdistance_threshold = 204;

				if(botdistance < 257)
				{
					botdistance_threshold = 38;
				}

				int topdistance_threshold = 204;
				if(topdistance < 257)
				{
					topdistance_threshold = 38;
				}

				for(int i = 0; i<3; ++i)
				{
					if(botcot<3)
					{//bot
						int nowazimuth = bot_now_azimuth + i;
						int nowch = vertical_angle_index;
						if(nowazimuth < azimuth_count_2 && nowazimuth >=0 && nowch>=0 && nowch<16 && botdistance > 1000)
						{
							int neighbordistance = botdistance - last_block_data[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
							{
								if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49
										&& last_block_data[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++botcot;
								}
								++botcot;
								++botcot;
							}
						}
					}

					if(topcot<3)
					{//top
						int nowazimuth = top_now_azimuth + i;
						int nowch = vertical_angle_index;
						if(nowazimuth < azimuth_count_2 && nowazimuth >= 0 && nowch>=0 && nowch<16 && topdistance > 1000)
						{
							int neighbordistance = topdistance - last_block_data[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
							{
								if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49
										&& last_block_data[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++topcot;
								}
								++topcot;
								++topcot;
							}
						}
					}
					if(botcot>2 && topcot>2)
					{
						break;
					}
				}

				int botrealcot = 0;
				int toprealcot = 0;

				for(int i = 0; i<24; ++i)
				{
					if(botcot<3)
					{//bot
						int nowazimuth = bot_now_azimuth + prebotnodes3[i].azimuthoffset;
						int nowch = vertical_angle_index + prebotnodes3[i].choffset;

						if(nowazimuth < azimuth_count_2 && nowazimuth >=0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = botdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
							{
								realbotnodes[botrealcot].azimuthoffset = nowazimuth;
								realbotnodes[botrealcot].choffset = nowch;
								++botrealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49
										&& block_data_shared[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++botcot;
								}
								++botcot;
								++botcot;
							}
							if(botcot<3 && botdistance > 1000)
							{
								neighbordistance = botdistance - last_block_data[nowazimuth].distance_[nowch].distance;
								if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
								{
									if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49
											&& last_block_data[nowazimuth].distance_[nowch].pulse_width != 255)
									{
										++botcot;
									}
									++botcot;
									++botcot;
								}
							}

						}
					}

					if(topcot<3)
					{//top
						int nowazimuth = top_now_azimuth + pretopnodes3[i].azimuthoffset;
						int nowch = vertical_angle_index + pretopnodes3[i].choffset;
//						if(nowazimuth < azimuth_count_2 && nowazimuth >= top_bottom_offset && nowch>=0 && nowch<16)
						if(nowazimuth < azimuth_count_2 && nowazimuth >= 0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = topdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
							{
								realtopnodes[toprealcot].azimuthoffset = nowazimuth;
								realtopnodes[toprealcot].choffset = nowch;
								++toprealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49
										&& block_data_shared[nowazimuth].distance_[nowch].pulse_width !=  255)
								{
									++topcot;
								}
								++topcot;
								++topcot;
							}
							if(topcot<3 && topdistance > 1000)
							{
								neighbordistance = topdistance - last_block_data[nowazimuth].distance_[nowch].distance;
								if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
								{
									if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49
											&& block_data_shared[nowazimuth].distance_[nowch].pulse_width != 255)
									{
										++topcot;
									}
									++topcot;
									++topcot;
								}
							}
						}
					}
					if(botcot>2 && topcot>2)
					{
						break;
					}
				}


//				botdistance_threshold = 256;
				botdistance_threshold = 296;
				if(botdistance < 257)
				{
					botdistance_threshold = 38;

				}
				else if(botdistance < 2000)
				{
					botdistance_threshold = 153;
				}

//				topdistance_threshold = 256;
				topdistance_threshold = 296;
				if(topdistance < 257)
				{
					topdistance_threshold = 38;
				}
				else if (topdistance < 2000)
				{
					topdistance_threshold = 153;
				}

				for(int i = 0; i<54; ++i)
				{
					if(botcot<3)
					{
						int nowazimuth = bot_now_azimuth + postbotnodes2[i].azimuthoffset;
						int nowch = vertical_angle_index + postbotnodes2[i].choffset;

						if(nowazimuth < azimuth_count_2 && nowazimuth >=0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = botdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
							{
								realbotnodes[botrealcot].azimuthoffset = nowazimuth;
								realbotnodes[botrealcot].choffset = nowch;
								++botrealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49
										&& block_data_shared[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++botcot;
								}
								++botcot;
							}
						}
					}

					if(topcot<3)
					{
						int nowazimuth = top_now_azimuth + posttopnodes2[i].azimuthoffset;
						int nowch = vertical_angle_index + posttopnodes2[i].choffset;

						if(nowazimuth < azimuth_count_2 && nowazimuth >= 0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = topdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
							{
								realtopnodes[toprealcot].azimuthoffset = nowazimuth;
								realtopnodes[toprealcot].choffset = nowch;
								++toprealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49 &&
										block_data_shared[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++topcot;
								}
								++topcot;
							}
						}
					}
					if(botcot>2 && topcot>2)
					{
						break;
					}
				}


				if(botalready==0 && botcot>2)
				{
					for(int i = 0; i < botrealcot; ++i)
					{
						simplenoisemap[realbotnodes[i].azimuthoffset].ischecked[realbotnodes[i].choffset] = 1;
					}
				}
				else if(botcot < 3 && botalready == 0)
				{
					//noise
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
//					current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
					(*noise_point_cnt)++;
				}


				if(topalready==0 && topcot>2)
				{
					for(int i = 0; i < toprealcot; ++i)
					{
						simplenoisemap[realtopnodes[i].azimuthoffset].ischecked[realtopnodes[i].choffset] = 1;
					}
				}
				else if(topcot < 3 && topalready == 0)
				{
					//noise
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
//					current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
					(*noise_point_cnt)++;
				}

			}
		}
	}

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index+=3)
	{
		for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
		{
			if(block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance == 0)
			{
				if(block_data_shared[azimuth_index + 1 + top_bottom_offset].distance_[vertical_angle_index].distance == 0 )
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance =
							block_data_shared[azimuth_index +2 + top_bottom_offset].distance_[vertical_angle_index].distance;


					current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] =
							block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance * DIV_256;


					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width =
							block_data_shared[azimuth_index + 2 + top_bottom_offset].distance_[vertical_angle_index].pulse_width;
				}
				else
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance =
							block_data_shared[azimuth_index + 1 + top_bottom_offset].distance_[vertical_angle_index].distance;

					block_data_shared[azimuth_index + 1 + top_bottom_offset].distance_[vertical_angle_index].pulse_width =
							block_data_shared[azimuth_index + 1+ top_bottom_offset].distance_[vertical_angle_index].pulse_width;


					current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] =
							block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance * DIV_256;

				}
			}

			if(block_data_shared[azimuth_index].distance_[vertical_angle_index].distance == 0)
			{
				if(block_data_shared[azimuth_index + 1].distance_[vertical_angle_index].distance == 0)
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance =
							block_data_shared[azimuth_index +2].distance_[vertical_angle_index].distance;

					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width =
							block_data_shared[azimuth_index + 2].distance_[vertical_angle_index].pulse_width;


					current_frame_blocks[azimuth_index].distance_[vertical_angle_index] =
							block_data_shared[azimuth_index].distance_[vertical_angle_index].distance * DIV_256;

				}
				else
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance =
							block_data_shared[azimuth_index + 1].distance_[vertical_angle_index].distance;

					block_data_shared[azimuth_index + 1].distance_[vertical_angle_index].pulse_width =
							block_data_shared[azimuth_index + 1].distance_[vertical_angle_index].pulse_width;


					current_frame_blocks[azimuth_index].distance_[vertical_angle_index] =
							block_data_shared[azimuth_index].distance_[vertical_angle_index].distance * DIV_256;

				}
			}
		}
	}
}


void SetAzimuthAndDistanceB1_Sparse_AziOffset(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt, RawDataBlock *last_block_data, int hz_option, int* routing_delay2, int routing_range_1, int routing_range_2
		, Azioffsets_ azioffset)
{
	memset(&simplenoisemap, 0x0, sizeof(SimpleNoiseDatas) * 10000);

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
//		max_distance1 = 44672;
		max_distance1 = 51200;
	}


	if(!is_sparse_botnode_init3 || top_bottom_offset != pretop_bottom_offset3)
	{
		for(int i = 0; i < 3; ++i)
		{
			prebotnodes3[i].azimuthoffset = i + top_bottom_offset;
			prebotnodes3[i].choffset = 0;

			prebotnodes3[i+3].azimuthoffset = i + top_bottom_offset;
			prebotnodes3[i+3].choffset = -1;
		}

		for(int i = 6; i < 9; ++i)
		{
			prebotnodes3[i].azimuthoffset = -3 + i - 6; //leftup
			prebotnodes3[i].choffset = 1;

			prebotnodes3[i+3].azimuthoffset = 3 + i - 6; //rightup
			prebotnodes3[i+3].choffset = 1;

			prebotnodes3[i+6].azimuthoffset = -3 + i - 6; //leftdown
			prebotnodes3[i+6].choffset = -1;

			prebotnodes3[i+9].azimuthoffset = 3 + i - 6; //rightdown
			prebotnodes3[i+9].choffset = -1;

			prebotnodes3[i+12].azimuthoffset = i + top_bottom_offset - 6; //up top
			prebotnodes3[i+12].choffset = 1;

			prebotnodes3[i+15].azimuthoffset = i + top_bottom_offset - 6; //down top
			prebotnodes3[i+15].choffset = -2;
		}

		int postnodes_idx = 0;

		for(int i = 3; i < 10; i += 3)
		{
			for(int j = 0; j < 3; ++j)
			{
				//left
				postbotnodes2[postnodes_idx +j].azimuthoffset = -1*i + j;
				postbotnodes2[postnodes_idx +j].choffset = 0;

				postbotnodes2[postnodes_idx+3 +j].azimuthoffset = -1*i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+3 +j].choffset = 0;

				postbotnodes2[postnodes_idx+6 +j].azimuthoffset = -1*i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+6 +j].choffset = -1;


				//right
				postbotnodes2[postnodes_idx+9 +j].azimuthoffset = i + j;
				postbotnodes2[postnodes_idx+9 +j].choffset = 0;

				postbotnodes2[postnodes_idx+12 +j].azimuthoffset = i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+12 +j].choffset = 0;

				postbotnodes2[postnodes_idx+15 +j].azimuthoffset = i + j + top_bottom_offset;
				postbotnodes2[postnodes_idx+15 +j].choffset = -1;
			}
			postnodes_idx+=18;
		}

		is_sparse_botnode_init3 = 1;


	}

	if(!is_sparse_topnode_init3 || top_bottom_offset != pretop_bottom_offset3)
	{
		for(int i = 0; i<3; ++i)
		{
			pretopnodes3[i].azimuthoffset = i - top_bottom_offset;
			pretopnodes3[i].choffset = 1;

			pretopnodes3[i+3].azimuthoffset = i - top_bottom_offset;
			pretopnodes3[i+3].choffset = 0;
		}

		for(int i = 6; i < 9; ++i)
		{
			pretopnodes3[i].azimuthoffset = -3 + i - 6; //leftup
			pretopnodes3[i].choffset = 1;

			pretopnodes3[i+3].azimuthoffset = 3 + i - 6; //rightup
			pretopnodes3[i+3].choffset = 1;

			pretopnodes3[i+6].azimuthoffset = -3 + i - 6; //leftdown
			pretopnodes3[i+6].choffset = 0;

			pretopnodes3[i+9].azimuthoffset = 3 + i - 6; //rightdown
			pretopnodes3[i+9].choffset = 0;

			pretopnodes3[i+12].azimuthoffset = i - top_bottom_offset - 6; //up bottom
			pretopnodes3[i+12].choffset = 2;

			pretopnodes3[i+15].azimuthoffset = i - top_bottom_offset - 6 ; //down bottom
			pretopnodes3[i+15].choffset = -1;
		}


		int postnodes_idx = 0;

		for(int i = 3; i < 10; i += 3)
		{
			for(int j = 0; j < 3; ++j)
			{
				//left
				posttopnodes2[postnodes_idx +j].azimuthoffset = -1*i + j;
				posttopnodes2[postnodes_idx +j].choffset = 0;

				posttopnodes2[postnodes_idx+3 +j].azimuthoffset = -1*i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+3 +j].choffset = 1;

				posttopnodes2[postnodes_idx+6 +j].azimuthoffset = -1*i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+6 +j].choffset = 0;


				//right
				posttopnodes2[postnodes_idx+9 +j].azimuthoffset = i + j;
				posttopnodes2[postnodes_idx+9 +j].choffset = 0;

				posttopnodes2[postnodes_idx+12 +j].azimuthoffset = i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+12 +j].choffset = 1;

				posttopnodes2[postnodes_idx+15 +j].azimuthoffset = i + j - top_bottom_offset;
				posttopnodes2[postnodes_idx+15 +j].choffset = 0;
			}
			postnodes_idx+=18;
		}


		is_sparse_topnode_init3 = 1;

		pretop_bottom_offset3 = top_bottom_offset;
	}


//	int32_t top_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;

//	int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? ((float)lidar_state_.a2z_setting.fov_correction / 120000 *176):(176);
//	int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*((float)lidar_state_.a2z_setting.fov_correction / 120000)):(90000);

	int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? (lidar_state_.a2z_setting.fov_correction):(176);
	int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*(((float)lidar_state_.a2z_setting.fov_correction*684) / 120384)):(90000);

	int32_t top_motor_init_value = base_angle - ((EncCtrlGetStartEncCount2()-2048)) * resolution;
	int32_t bottom_motor_init_value = base_angle - EncCtrlGetStartEncCount1() * resolution;

//	int32_t top_motor_init_value = 90000 - ((EncCtrlGetStartEncCount2()-2048)) * 176;
//	int32_t bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;

	int32_t clip_min = distance_offset;

	uint8_t is_start_vert_angle_even = 0;
	if(fov_test_.start_vertical_angle % 2 == 0)
		is_start_vert_angle_even = 1;
	uint8_t is_end_vert_angle_even = 0;
	if(fov_test_.end_vertical_angle % 2 == 0)
		is_end_vert_angle_even = 1;


	int botcurrent_distance = 0;
	int topcurrent_distance = 0;

	const int azimuth_count_2 = azimuth_count*2;

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
	{
//		//bottom
//		block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176);
		block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * resolution);

		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared[azimuth_index].azimuth_ * 0.001;
//		block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176);
		block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * resolution);

		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared[azimuth_index + top_bottom_offset].azimuth_ * 0.001;

		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; ++vertical_angle_index)
		{
			//bottom
//			botcurrent_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index] - clip_min;
			int bottom_raw_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
			if(bottom_raw_distance < routing_range_1)
			{
				botcurrent_distance = bottom_raw_distance + routing_delay2[vertical_angle_index];
			}
			else if(bottom_raw_distance < routing_range_2)
			{
				float slope = (float)(routing_delay[vertical_angle_index] - routing_delay2[vertical_angle_index]) / (routing_range_2 - routing_range_1);
				float intercept = (float)routing_delay[vertical_angle_index] - routing_range_2 * slope;
				botcurrent_distance = bottom_raw_distance + slope * bottom_raw_distance + intercept;
			}
			else
			{
				botcurrent_distance = bottom_raw_distance + routing_delay[vertical_angle_index];
			}
			if(botcurrent_distance <= 0 || botcurrent_distance > max_distance0  || (vertical_angle_index < 4 && botcurrent_distance > max_distance1) || (vertical_angle_index > 11 && botcurrent_distance > max_distance1)
					|| block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width == 0)
//			if(botcurrent_distance <= 0 || botcurrent_distance > 58880  || (vertical_angle_index < 4 && botcurrent_distance > 45056) || (vertical_angle_index > 11 && botcurrent_distance > 45056) )
			{
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
				simplenoisemap[azimuth_index].ischecked[vertical_angle_index] = 1;
				botcurrent_distance = 0;
			}
			else
			{
				if(is_end_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
											vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						botcurrent_distance = 0;

				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
												vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2)-1)
						botcurrent_distance = 0;
				}
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = botcurrent_distance;
				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = botcurrent_distance * DIV_256;

				//add azioffsets
				if(botcurrent_distance <= 128)
				{
					botcurrent_distance +=
							azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2];
				}
				else if(botcurrent_distance <= 256)
				{
					float slope = azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2]
											  - azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2];

					float per_data =  slope/128;

					botcurrent_distance +=
							azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2] + per_data * (botcurrent_distance - 128);
				}
				else if(botcurrent_distance <= 512)
				{
					float slope = azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2]
											  - azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2];

					float per_data =  slope/256;

					botcurrent_distance +=
							azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2] + per_data * (botcurrent_distance - 256);
				}
				else if(botcurrent_distance <= 768)
				{
					float slope = azioffset.AziGroup[3].Offsets[azimuth_index/3].ch[vertical_angle_index*2]
											  - azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2];

					float per_data =  slope/256;

					botcurrent_distance +=
							azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2] + per_data * (botcurrent_distance - 512);
				}
				else
				{
					botcurrent_distance +=
							azioffset.AziGroup[3].Offsets[azimuth_index/3].ch[vertical_angle_index*2];
				}
				if(botcurrent_distance < 0 || botcurrent_distance > max_distance0)
				{
					botcurrent_distance  = 0;
				}

				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = botcurrent_distance;

			}

			//top
//			topcurrent_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;
			//topcurrent_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT];
			int top_raw_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;
			if(top_raw_distance < routing_range_1)
			{
				topcurrent_distance = top_raw_distance + routing_delay2[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT];
			}
			else if(top_raw_distance < routing_range_2)
			{
				float slope = (float)(routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - routing_delay2[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT]) / (routing_range_2 - routing_range_1);
				float intercept = (float)routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - routing_range_2 * slope;
				topcurrent_distance = top_raw_distance + slope * top_raw_distance + intercept;
			}
			else
			{
				topcurrent_distance = top_raw_distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT];
			}
//			if(topcurrent_distance <= 0 || topcurrent_distance > 58880 || (vertical_angle_index < 4 && topcurrent_distance > 45056) || (vertical_angle_index > 11 && topcurrent_distance > 45056))
			if(topcurrent_distance <= 0 || topcurrent_distance > max_distance0 || (vertical_angle_index < 4 && topcurrent_distance > max_distance1) || (vertical_angle_index > 11 && topcurrent_distance > max_distance1)
					|| block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width == 0)
			{
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
				simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] = 1;
				topcurrent_distance = 0;
			}
			else
			{
				if(is_start_vert_angle_even == 0)
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						topcurrent_distance = 0;
				}
				else
				{
					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
						vertical_angle_index < (fov_test_.start_vertical_angle / 2) - 1 || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						topcurrent_distance = 0;
				}

				if(is_end_vert_angle_even == 0)
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
						topcurrent_distance = 0;
				}
				else
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
						topcurrent_distance = 0;
				}
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = topcurrent_distance;
				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = topcurrent_distance * DIV_256;

				//add azioffset
				if(topcurrent_distance <= 128)
				{
					topcurrent_distance +=
							azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];
				}
				else if(topcurrent_distance <= 256)
				{
					float slope = azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1]
											  - azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];

					float per_data =  slope/128;

					topcurrent_distance +=
							azioffset.AziGroup[0].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1] + per_data * (topcurrent_distance - 128);
				}
				else if(topcurrent_distance <= 512)
				{
					float slope = azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1]
											  - azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];

					float per_data =  slope/256;

					topcurrent_distance +=
							azioffset.AziGroup[1].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1] + per_data * (topcurrent_distance - 256);
				}
				else if(topcurrent_distance <= 768)
				{
					float slope = azioffset.AziGroup[3].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1]
											  - azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];

					float per_data =  slope/256;

					topcurrent_distance +=
							azioffset.AziGroup[2].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1] + per_data * (topcurrent_distance - 512);
				}
				else
				{
					topcurrent_distance +=
							azioffset.AziGroup[3].Offsets[azimuth_index/3].ch[vertical_angle_index*2 + 1];
				}

				if(topcurrent_distance < 0 || topcurrent_distance > max_distance0)
				{
					topcurrent_distance = 0;
				}

				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = topcurrent_distance;

			}
		}
	}

	{
		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				//if already checked, pass
				int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;

				int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int botcot = 0;
				int topcot = 0;

				int botalready = 0;
				int topalready = 0;

//				if(botdistance == 0 || simplenoisemap[azimuth_index].ischecked[vertical_angle_index])
				if(simplenoisemap[azimuth_index].ischecked[vertical_angle_index])
				{
					botcot = 10;
					botalready = 1;
				}

//				if(topdistance == 0 || simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index])
				if(simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index])
				{
					topcot = 10;
					topalready = 1;
				}

				if(botcot==0)
				{
					int botintensity = block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width;

					if(botintensity == 255)
					{
						//notghing or botcot--
						--botcot;
					}
					else if(botintensity > 79)
					{
						++botcot;
						++botcot;
					}
					else if (botintensity > 49)
					{
						++botcot;
						++botcot;
					}
					else if (botintensity > 30)
					{
						++botcot;
					}


					for(int i = 0; i<3; ++i)
					{
						realbotnodes[i].azimuthoffset = 0;
						realbotnodes[i].choffset = 0;
					}
				}

				if(topcot==0)
				{
					int topintensity = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width;

					if(topintensity == 255)
					{
						--topcot;
					}
					else if(topintensity > 79)
					{
						++topcot;
						++topcot;
					}
					else if (topintensity > 49)
					{
						++topcot;
						++topcot;
					}
					else if (topintensity >30)
					{
						++topcot;
					}


					for(int i = 0; i<3; ++i)
					{
						realtopnodes[i].azimuthoffset = 0;
						realtopnodes[i].choffset = 0;
					}
				}


				int stop_cot = azimuth_index % 3;
				int bot_now_azimuth = azimuth_index + (-1 * stop_cot);
				int top_now_azimuth = bot_now_azimuth + top_bottom_offset;


				int botdistance_threshold = 204;

				if(botdistance < 257)
				{
					botdistance_threshold = 38;
				}

				int topdistance_threshold = 204;
				if(topdistance < 257)
				{
					topdistance_threshold = 38;
				}

				for(int i = 0; i<3; ++i)
				{
					if(botcot<3)
					{//bot
						int nowazimuth = bot_now_azimuth + i;
						int nowch = vertical_angle_index;
						if(nowazimuth < azimuth_count_2 && nowazimuth >=0 && nowch>=0 && nowch<16 && botdistance > 1000)
						{
							int neighbordistance = botdistance - last_block_data[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
							{
								if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49
										&& last_block_data[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++botcot;
								}
								++botcot;
								++botcot;
							}
						}
					}

					if(topcot<3)
					{//top
						int nowazimuth = top_now_azimuth + i;
						int nowch = vertical_angle_index;
						if(nowazimuth < azimuth_count_2 && nowazimuth >= 0 && nowch>=0 && nowch<16 && topdistance > 1000)
						{
							int neighbordistance = topdistance - last_block_data[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
							{
								if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49
										&& last_block_data[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++topcot;
								}
								++topcot;
								++topcot;
							}
						}
					}
					if(botcot>2 && topcot>2)
					{
						break;
					}
				}

				int botrealcot = 0;
				int toprealcot = 0;

				for(int i = 0; i<24; ++i)
				{
					if(botcot<3)
					{//bot
						int nowazimuth = bot_now_azimuth + prebotnodes3[i].azimuthoffset;
						int nowch = vertical_angle_index + prebotnodes3[i].choffset;

						if(nowazimuth < azimuth_count_2 && nowazimuth >=0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = botdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
							{
								realbotnodes[botrealcot].azimuthoffset = nowazimuth;
								realbotnodes[botrealcot].choffset = nowch;
								++botrealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49
										&& block_data_shared[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++botcot;
								}
								++botcot;
								++botcot;
							}
							if(botcot<3 && botdistance > 1000)
							{
								neighbordistance = botdistance - last_block_data[nowazimuth].distance_[nowch].distance;
								if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
								{
									if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49
											&& last_block_data[nowazimuth].distance_[nowch].pulse_width != 255)
									{
										++botcot;
									}
									++botcot;
									++botcot;
								}
							}

						}
					}

					if(topcot<3)
					{//top
						int nowazimuth = top_now_azimuth + pretopnodes3[i].azimuthoffset;
						int nowch = vertical_angle_index + pretopnodes3[i].choffset;
//						if(nowazimuth < azimuth_count_2 && nowazimuth >= top_bottom_offset && nowch>=0 && nowch<16)
						if(nowazimuth < azimuth_count_2 && nowazimuth >= 0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = topdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
							{
								realtopnodes[toprealcot].azimuthoffset = nowazimuth;
								realtopnodes[toprealcot].choffset = nowch;
								++toprealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49
										&& block_data_shared[nowazimuth].distance_[nowch].pulse_width !=  255)
								{
									++topcot;
								}
								++topcot;
								++topcot;
							}
							if(topcot<3 && topdistance > 1000)
							{
								neighbordistance = topdistance - last_block_data[nowazimuth].distance_[nowch].distance;
								if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
								{
									if(last_block_data[nowazimuth].distance_[nowch].pulse_width > 49
											&& block_data_shared[nowazimuth].distance_[nowch].pulse_width != 255)
									{
										++topcot;
									}
									++topcot;
									++topcot;
								}
							}
						}
					}
					if(botcot>2 && topcot>2)
					{
						break;
					}
				}


//				botdistance_threshold = 256;
				botdistance_threshold = 296;
				if(botdistance < 257)
				{
					botdistance_threshold = 38;

				}
				else if(botdistance < 2000)
				{
					botdistance_threshold = 153;
				}

//				topdistance_threshold = 256;
				topdistance_threshold = 296;
				if(topdistance < 257)
				{
					topdistance_threshold = 38;
				}
				else if (topdistance < 2000)
				{
					topdistance_threshold = 153;
				}

				for(int i = 0; i<54; ++i)
				{
					if(botcot<3)
					{
						int nowazimuth = bot_now_azimuth + postbotnodes2[i].azimuthoffset;
						int nowch = vertical_angle_index + postbotnodes2[i].choffset;

						if(nowazimuth < azimuth_count_2 && nowazimuth >=0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = botdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -botdistance_threshold && neighbordistance <botdistance_threshold)
							{
								realbotnodes[botrealcot].azimuthoffset = nowazimuth;
								realbotnodes[botrealcot].choffset = nowch;
								++botrealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49
										&& block_data_shared[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++botcot;
								}
								++botcot;
							}
						}
					}

					if(topcot<3)
					{
						int nowazimuth = top_now_azimuth + posttopnodes2[i].azimuthoffset;
						int nowch = vertical_angle_index + posttopnodes2[i].choffset;

						if(nowazimuth < azimuth_count_2 && nowazimuth >= 0 && nowch>=0 && nowch<16)
						{
							int neighbordistance = topdistance - block_data_shared[nowazimuth].distance_[nowch].distance;
							if(neighbordistance > -topdistance_threshold && neighbordistance <topdistance_threshold)
							{
								realtopnodes[toprealcot].azimuthoffset = nowazimuth;
								realtopnodes[toprealcot].choffset = nowch;
								++toprealcot;

								if(block_data_shared[nowazimuth].distance_[nowch].pulse_width > 49 &&
										block_data_shared[nowazimuth].distance_[nowch].pulse_width != 255)
								{
									++topcot;
								}
								++topcot;
							}
						}
					}
					if(botcot>2 && topcot>2)
					{
						break;
					}
				}


				if(botalready==0 && botcot>2)
				{
					for(int i = 0; i < botrealcot; ++i)
					{
						simplenoisemap[realbotnodes[i].azimuthoffset].ischecked[realbotnodes[i].choffset] = 1;
					}
				}
				else if(botcot < 3 && botalready == 0)
				{
					//noise
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
//					current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
					(*noise_point_cnt)++;
				}


				if(topalready==0 && topcot>2)
				{
					for(int i = 0; i < toprealcot; ++i)
					{
						simplenoisemap[realtopnodes[i].azimuthoffset].ischecked[realtopnodes[i].choffset] = 1;
					}
				}
				else if(topcot < 3 && topalready == 0)
				{
					//noise
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
//					current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
					(*noise_point_cnt)++;
				}

			}
		}
	}

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index+=3)
	{
		for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
		{
			if(block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance == 0)
			{
				if(block_data_shared[azimuth_index + 1 + top_bottom_offset].distance_[vertical_angle_index].distance == 0 )
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance =
							block_data_shared[azimuth_index +2 + top_bottom_offset].distance_[vertical_angle_index].distance;


					current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] =
							block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance * DIV_256;


					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width =
							block_data_shared[azimuth_index + 2 + top_bottom_offset].distance_[vertical_angle_index].pulse_width;
				}
				else
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance =
							block_data_shared[azimuth_index + 1 + top_bottom_offset].distance_[vertical_angle_index].distance;

					block_data_shared[azimuth_index + 1 + top_bottom_offset].distance_[vertical_angle_index].pulse_width =
							block_data_shared[azimuth_index + 1+ top_bottom_offset].distance_[vertical_angle_index].pulse_width;


					current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] =
							block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance * DIV_256;

				}
			}

			if(block_data_shared[azimuth_index].distance_[vertical_angle_index].distance == 0)
			{
				if(block_data_shared[azimuth_index + 1].distance_[vertical_angle_index].distance == 0)
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance =
							block_data_shared[azimuth_index +2].distance_[vertical_angle_index].distance;

					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width =
							block_data_shared[azimuth_index + 2].distance_[vertical_angle_index].pulse_width;


					current_frame_blocks[azimuth_index].distance_[vertical_angle_index] =
							block_data_shared[azimuth_index].distance_[vertical_angle_index].distance * DIV_256;

				}
				else
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance =
							block_data_shared[azimuth_index + 1].distance_[vertical_angle_index].distance;

					block_data_shared[azimuth_index + 1].distance_[vertical_angle_index].pulse_width =
							block_data_shared[azimuth_index + 1].distance_[vertical_angle_index].pulse_width;


					current_frame_blocks[azimuth_index].distance_[vertical_angle_index] =
							block_data_shared[azimuth_index].distance_[vertical_angle_index].distance * DIV_256;

				}
			}
		}
	}
}


void SetAzimuthAndDistanceB1_Sparse_test(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt, RawDataBlock *last_block_data, int hz_option, int* routing_delay2, int routing_range_1, int routing_range_2)
{
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
		max_distance1 = 44672;
//		max_distance1 = 51200;
	}

//	int32_t top_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;
//	int32_t top_motor_init_value = 90000 - ((EncCtrlGetStartEncCount2()-2048)) * 176;
//	int32_t bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;

//	volatile int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? ((float)lidar_state_.a2z_setting.fov_correction / 120000 *176):(176);
//	volatile int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*((float)lidar_state_.a2z_setting.fov_correction / 120000)):(90000);

	int resolution = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ? (lidar_state_.a2z_setting.fov_correction):(176);
	int base_angle = (lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?(90000*(((float)lidar_state_.a2z_setting.fov_correction*684) / 120384)):(90000);

	int32_t top_motor_init_value = base_angle - ((EncCtrlGetStartEncCount2()-2048)) * resolution;
	int32_t bottom_motor_init_value = base_angle - EncCtrlGetStartEncCount1() * resolution;

	int32_t clip_min = distance_offset;

	uint8_t is_start_vert_angle_even = 0;
	if(fov_test_.start_vertical_angle % 2 == 0)
		is_start_vert_angle_even = 1;
	uint8_t is_end_vert_angle_even = 0;
	if(fov_test_.end_vertical_angle % 2 == 0)
		is_end_vert_angle_even = 1;


	int botcurrent_distance = 0;
	int topcurrent_distance = 0;

	const int azimuth_count_2 = azimuth_count*2;

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
	{
//		//bottom
//		block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * 176);
		block_data_shared[azimuth_index].azimuth_ = (int)(bottom_motor_init_value - (block_data_shared[azimuth_index].azimuth_ - bottom_start_enc_cnt) * resolution);

//		current_frame_blocks[azimuth_index].azimuth_ = block_data_shared[azimuth_index].azimuth_ * 0.001;
//		block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176);
		block_data_shared[azimuth_index + top_bottom_offset].azimuth_ = (int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * resolution);
//		block_data_shared[azimuth_index + top_bottom_offset].azimuth_ =
//								(lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0) ?
//										(((int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176))	*
//												((float)lidar_state_.a2z_setting.fov_correction	/ (bottom_motor_init_value * 2))) :
//										((int)(top_motor_init_value - (block_data_shared[azimuth_index + top_bottom_offset].azimuth_ - top_start_enc_cnt) * 176));
//		current_frame_blocks[azimuth_index + top_bottom_offset].azimuth_ = block_data_shared[azimuth_index + top_bottom_offset].azimuth_ * 0.001;

		float tmp_azimuth = block_data_shared[azimuth_index].azimuth_ * 0.001;
		for(int vertical_angle_index = 0 ; vertical_angle_index < vertical_angle_count ; ++vertical_angle_index)
		{
			//bottom
//			botcurrent_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index] - clip_min;
			int bottom_raw_distance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
			if(bottom_raw_distance < routing_range_1)
			{
				botcurrent_distance = bottom_raw_distance + routing_delay2[vertical_angle_index];
			}
			else if(bottom_raw_distance < routing_range_2)
			{
				float slope = (routing_delay[vertical_angle_index] - routing_delay2[vertical_angle_index]) / (routing_range_2 - routing_range_1);
				float intercept = routing_delay[vertical_angle_index] - routing_range_2 * slope;
				botcurrent_distance = bottom_raw_distance + slope * bottom_raw_distance + intercept;
			}
			else
			{
				botcurrent_distance = bottom_raw_distance + routing_delay[vertical_angle_index];
			}

			if(botcurrent_distance <= 0 || botcurrent_distance > max_distance0  || (vertical_angle_index < 4 && botcurrent_distance > max_distance1) || (vertical_angle_index > 11 && botcurrent_distance > max_distance1))
//			if(botcurrent_distance <= 0 || botcurrent_distance > 58880  || (vertical_angle_index < 4 && botcurrent_distance > 45056) || (vertical_angle_index > 11 && botcurrent_distance > 45056) )
			{
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
//				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = 0;
				simplenoisemap[azimuth_index].ischecked[vertical_angle_index] = 1;
				botcurrent_distance = 0;
			}
			else
			{
				if(is_end_vert_angle_even == 0)
				{
//					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
//											vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
					if(tmp_azimuth < fov_test_.start_horizontal_angle || tmp_azimuth > fov_test_.end_horizontal_angle||
																vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						botcurrent_distance = 0;

				}
				else
				{
//					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
//												vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2)-1)
					if(tmp_azimuth < fov_test_.start_horizontal_angle || tmp_azimuth > fov_test_.end_horizontal_angle||
																	vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2)-1)
						botcurrent_distance = 0;
				}
				block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = botcurrent_distance;
//				current_frame_blocks[azimuth_index].distance_[vertical_angle_index] = botcurrent_distance * DIV_256;
			}

			int top_raw_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;
			if(top_raw_distance < routing_range_1)
			{
				topcurrent_distance = top_raw_distance + routing_delay2[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT];
			}
			else if(top_raw_distance < routing_range_2)
			{
				float slope = (routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - routing_delay2[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT]) / (routing_range_2 - routing_range_1);
				float intercept = routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - routing_range_2 * slope;
				topcurrent_distance = top_raw_distance + slope * top_raw_distance + intercept;
			}
			else
			{
				topcurrent_distance = top_raw_distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT];
			}

			//top
//			topcurrent_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT] - clip_min;
//			topcurrent_distance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance + routing_delay[vertical_angle_index + ONE_SIDE_CHANNEL_COUNT];
//			if(topcurrent_distance <= 0 || topcurrent_distance > 58880 || (vertical_angle_index < 4 && topcurrent_distance > 45056) || (vertical_angle_index > 11 && topcurrent_distance > 45056))
			if(topcurrent_distance <= 0 || topcurrent_distance > max_distance0 || (vertical_angle_index < 4 && topcurrent_distance > max_distance1) || (vertical_angle_index > 11 && topcurrent_distance > max_distance1))
			{
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
//				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = 0;
				simplenoisemap[azimuth_index + top_bottom_offset].ischecked[vertical_angle_index] = 1;
				topcurrent_distance = 0;
			}
			else
			{
				if(is_start_vert_angle_even == 0)
				{
//					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
//						vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
					if(tmp_azimuth < fov_test_.start_horizontal_angle || tmp_azimuth > fov_test_.end_horizontal_angle||
											vertical_angle_index < (fov_test_.start_vertical_angle / 2) || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						topcurrent_distance = 0;
				}
				else
				{
//					if(current_frame_blocks[azimuth_index].azimuth_ < fov_test_.start_horizontal_angle || current_frame_blocks[azimuth_index].azimuth_ > fov_test_.end_horizontal_angle||
//						vertical_angle_index < (fov_test_.start_vertical_angle / 2) - 1 || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
					if(tmp_azimuth < fov_test_.start_horizontal_angle || tmp_azimuth > fov_test_.end_horizontal_angle||
											vertical_angle_index < (fov_test_.start_vertical_angle / 2) - 1 || vertical_angle_index > (fov_test_.end_vertical_angle / 2))
						topcurrent_distance = 0;
				}

				if(is_end_vert_angle_even == 0)
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
						topcurrent_distance = 0;
				}
				else
				{
					if(vertical_angle_index > (fov_test_.end_vertical_angle / 2) - 1)
						topcurrent_distance = 0;
				}
				block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = topcurrent_distance;
//				current_frame_blocks[azimuth_index + top_bottom_offset].distance_[vertical_angle_index] = topcurrent_distance * DIV_256;
			}
		}
	}


}

//		const float FIR_FACTOR[16] = {
////				0, 0, 0, 0,  0, 0.5, 1,  1,   1, 0.5, 0, 0,  0, 0, 0, 0,
////				0, 0, 0, 0,  0, 0.8, 0.9,  1,   0.9, 0.8, 0, 0,  0, 0, 0, 0,
////				0, 0, 0, 0,  0.2, 0.6, 0.8,  1,   0.8, 0.6, 0.2, 0,  0, 0, 0, 0,
////				0, 0.1, 0.2, 0.3,  0.5, 0.8, 1,  1,   1, 0.8, 0.5, 0.3,  0.2, 0.1, 0, 0,
//				0.1, 0.1, 0.2, 0.3,  0.5, 0.8, 1,  1,   1, 0.8, 0.5, 0.3,  0.2, 0.1, 0.1, 0,
//		};

//		const int FIR_FACTOR[15] = {
////				0, 0, 0, 0,  0, 0.5, 1,  1,   1, 0.5, 0, 0,  0, 0, 0, 0,
////				0, 0, 0, 0,  0, 0.8, 0.9,  1,   0.9, 0.8, 0, 0,  0, 0, 0, 0,
////				0, 0, 0, 0,  0.2, 0.6, 0.8,  1,   0.8, 0.6, 0.2, 0,  0, 0, 0, 0,
////				0, 0.1, 0.2, 0.3,  0.5, 0.8, 1,  1,   1, 0.8, 0.5, 0.3,  0.2, 0.1, 0, 0,
//				0, 1, 1, 1,  2, 2, 3,  3,   3, 2, 2, 1,  1, 1, 0
//		};

const int FIR_FACTOR[13] = {

//				0, 0, 0, 0,  0, 0.8, 0.9,  1,   0.9, 0.8, 0, 0,  0, 0, 0, 0,
//				0, 0, 0, 0,  0.2, 0.6, 0.8,  1,   0.8, 0.6, 0.2, 0,  0, 0, 0, 0,
//				0, 0.1, 0.2, 0.3,  0.5, 0.8, 1,  1,   1, 0.8, 0.5, 0.3,  0.2, 0.1, 0, 0,
//		1, 1, 1,  2, 2, 3,  3,   3, 2, 2, 1,  1, 1	//org
		1, 1, 1,  2, 2, 3,  3,   3, 2, 2, 1,  1, 1	//test 040703
//		1, 1, 1,  2, 2, 3,  3,   3, 0, 3, 0,  0, 0	//test 040703
//		1, 1, 1,  2, 2, 3,  3,   3, 2, 2, 1,  1, 1	//test 040703
};
void FIR_Filter(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state)
{
//	const int threshold = 33;
	const int threshold = 33;
//		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; ++azimuth_index)
		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
				const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int counts = 0;
				int counts_top = 0;

				int sum = 0;
				int sum_top = 0;

				for(int nowazi = azimuth_index - 18; nowazi < azimuth_index + 21; nowazi+=3)//only 1stop
//				for(int nowazi = azimuth_index - 18; nowazi < azimuth_index + 21; ++nowazi)	//13
				{
					if(nowazi == azimuth_index || nowazi < 0 || nowazi >= azimuth_count)
					{
						continue;
					}

					int sumdistance = block_data_shared[nowazi].distance_[vertical_angle_index].distance;

//					if(sumdistance == 0)
//						continue;

					int nowazi_top = nowazi + top_bottom_offset;

					volatile int factor = FIR_FACTOR[(nowazi - azimuth_index + 18) / 3];

					if(sumdistance - botdistance < threshold && sumdistance - botdistance > -threshold)
					{
						counts += factor;
//						sum += factor * block_data_shared[nowazi].distance_[vertical_angle_index].distance;
					}

					sumdistance = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
					if(sumdistance - topdistance < threshold && sumdistance - topdistance > -threshold)
					{
						counts_top += factor;
//						sum_top += factor * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
					}
				}


				if(counts < 4 && botdistance < 3072)	//12m
//				if(0)
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts < 7 && botdistance < 1280)	//5m
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}
//				else if(counts>1)	//org
				else if(counts>7)
				{
//					sum /= counts;
//					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum;
				}

				if(counts_top < 4 && topdistance < 3072)
//				if(0)
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts_top < 7 && topdistance < 1280)
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
//				else if(counts_top > 1)
				else if(counts_top > 7)
				{
//					sum_top /= counts_top;
//					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top;
				}
		}
	}
}



const int FIR_LEFT_RIGHT[7] = {
//		0, 3, 2, 2,  1, 1, 1
		0, 3, 2, 2,  1, 1, 0
};

const int FIR_RIGHT[6] = {
0x0,
//				0, 0, 0, 0,  0, 0.8, 0.9,  1,   0.9, 0.8, 0, 0,  0, 0, 0, 0,
//				0, 0, 0, 0,  0.2, 0.6, 0.8,  1,   0.8, 0.6, 0.2, 0,  0, 0, 0, 0,
//				0, 0.1, 0.2, 0.3,  0.5, 0.8, 1,  1,   1, 0.8, 0.5, 0.3,  0.2, 0.1, 0, 0,
//		1, 1, 1,  2, 2, 3,  3,   3, 2, 2, 1,  1, 1	//org
//		1, 1, 1,  2, 2, 3,  3,   3, 2, 2, 1,  1, 1	//test 040703
//		1, 1, 1,  2, 2, 3,  3,   3, 0, 3, 0,  0, 0	//test 040703
//		1, 1, 1,  2, 2, 3,  3,   3, 2, 2, 1,  1, 1	//test 040703
};

void FIR_Filter3(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state)
{
	const int threshold = lidar_state->FIR_setting.threshold1;		//org 33
	const int threshold2 = lidar_state->FIR_setting.threshold2;	//org64

	const int distance1 = lidar_state->FIR_setting.distance1;		//org1280
	const int distance2 = lidar_state->FIR_setting.distance2;		//org3328

	const unsigned int count1 = lidar_state->FIR_setting.count1;	//org7
	const unsigned int count2 = lidar_state->FIR_setting.count2;	//org1

	int max_count = lidar_state->FIR_setting.max_count;			//org 7

	const int ismafON = lidar_state->FIR_setting.ison;			//org 0

//	const int FIR_LEFT_RIGHT[7] = {	0, 3, 2, 2,  1, 1, 0};
	int FIR_COEF[7] = {	0x0,};

	for(int i = 0; i<7; ++i)
	{
		FIR_COEF[i] = lidar_state->FIR_setting.FIR_COEF[i];
	}

	if(max_count < 1)
	{
		max_count = 2;
	}
	else if(max_count > 7)
	{
		max_count = 7;
	}

		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
				const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int counts = 0;
				int counts_top = 0;

				int sum = 0;
				int sum_top = 0;


//				for(int idx = 1; idx < 7; ++idx)//only 1stop
				for(int idx = 1; idx < max_count; ++idx)//only 1stop
				{
					//right
					const int nowazi = azimuth_index + idx*3;
					if(nowazi == azimuth_index || nowazi < 0 || nowazi >= azimuth_count)
					{
						continue;
					}

					int sumdistance = block_data_shared[nowazi].distance_[vertical_angle_index].distance;


					const int nowazi_top = nowazi + top_bottom_offset;

					const int factor = FIR_COEF[idx];

//					if(botdistance > 1280)
					if(botdistance > distance1)
					{
						if(sumdistance - botdistance < threshold2 && sumdistance - botdistance > -threshold2)
						{
							counts += factor;
							if(ismafON)
							{
								sum += factor * block_data_shared[nowazi].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance - botdistance < threshold && sumdistance - botdistance > -threshold)
						{
							counts += factor;
							if(ismafON)
							{
								sum += factor * block_data_shared[nowazi].distance_[vertical_angle_index].distance;
							}
						}
					}


					sumdistance = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
//					if(topdistance > 1280)
					if(topdistance > distance1)
					{
						if(sumdistance - topdistance < threshold2 && sumdistance - topdistance > -threshold2)
						{
							counts_top += factor;
							if(ismafON)
							{
								sum_top += factor * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance - topdistance < threshold && sumdistance - topdistance > -threshold)
						{
							counts_top += factor;
							if(ismafON)
							{
								sum_top += factor * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}

				}

//				for(int idx = 1; idx < 7; ++idx)//only 1stop
				for(int idx = 1; idx < max_count; ++idx)//only 1stop
				{
					//left
					const int nowazi = azimuth_index - (idx*3);
					if(nowazi == azimuth_index || nowazi < 0 || nowazi >= azimuth_count)
					{
						continue;
					}

					int sumdistance = block_data_shared[nowazi].distance_[vertical_angle_index].distance;


					const int nowazi_top = nowazi + top_bottom_offset;

//					volatile int factor = FIR_FACTOR[(nowazi - azimuth_index + 18) / 3];
//					volatile int factor = FIR_LEFT_RIGHT[idx];
					const int factor = FIR_COEF[idx];

					if(botdistance > distance1)
					{
						if(sumdistance - botdistance < threshold2 && sumdistance - botdistance > -threshold2)
						{
							counts += factor;
							if(ismafON)
							{
								sum += factor * block_data_shared[nowazi].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance - botdistance < threshold && sumdistance - botdistance > -threshold)
						{
							counts += factor;
							if(ismafON)
							{
								sum += factor * block_data_shared[nowazi].distance_[vertical_angle_index].distance;
							}
						}
					}

					sumdistance = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

					if(topdistance > distance1)
					{
						if(sumdistance - topdistance < threshold2 && sumdistance - topdistance > -threshold2)
						{
							counts_top += factor;

							if(ismafON)
							{
								sum_top += factor * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance - topdistance < threshold && sumdistance - topdistance > -threshold)
						{
							counts_top += factor;

							if(ismafON)
							{
								sum_top += factor * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}

				}


				if(counts < count1 && botdistance < distance1)	//5m
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts < count2 && botdistance < distance2 && botdistance >= distance1)	//20m
//				if(0)
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}
//				else if(counts < 7 && botdistance < 1280)	//5m
//				else if(counts>1)	//org
				else if(counts>3)
				{
					if(ismafON && botdistance !=0)
					{
						sum /= counts;
						block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum;
					}
				}


				if(counts_top < count1 && topdistance < distance1)	//5m
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts_top < count2 && topdistance < distance2 && topdistance >= distance1)	//20m
//				if(0)
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
//				else if(counts_top < 7 && topdistance < 1280)
//				else if(counts_top > 1)
				else if(counts_top > 3)
				{
					if(ismafON && topdistance !=0 )
					{
						sum_top /= counts_top;
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top;
					}
				}
		}
	}
}


void FIR_Filter_FLAT2(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state)
{
	const int threshold = lidar_state->FIR_setting.threshold1;		//org 33
	const int threshold2 = lidar_state->FIR_setting.threshold2;	//org64

	const int distance1 = lidar_state->FIR_setting.distance1;		//org1280
	const int distance2 = lidar_state->FIR_setting.distance2;		//org3328

	const unsigned int count1 = lidar_state->FIR_setting.count1;	//org7
	const unsigned int count2 = lidar_state->FIR_setting.count2;	//org1

	int max_count = lidar_state->FIR_setting.max_count;			//org 7

	const int ismafON = lidar_state->FIR_setting.ison;			//org 0

//	const int FIR_LEFT_RIGHT[7] = {	0, 3, 2, 2,  1, 1, 0};
	int FIR_COEF[7] = {	0x0,};

	for(int i = 0; i<7; ++i)
	{
		FIR_COEF[i] = lidar_state->FIR_setting.FIR_COEF[i];
//		FIR_COEF[i] = lidar_state->FIR_setting.FIR_COEF[i] & 0x1;
	}

	if(max_count < 1)
	{
		return;
	}
	else if(max_count > 3)
	{
		max_count = 3;
	}

//	if(max_count < 1)
//	{
//		max_count = 2;
//	}
//	else if(max_count > 7)
//	{
//		max_count = 7;
//	}

	//flags
	int temp_cot_bot[2070][16] = {0x0,};
	int temp_cot_top[2070][16] = {0x0,};

	int temp_sum_bot[2070][16] = {0x0,};
	int temp_sum_top[2070][16] = {0x0,};


	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
	{
		for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
		{
			const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
			const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

			if(botdistance == 0 &&  topdistance == 0)
			{
				continue;
			}

			int counts_bot = 0;
			int counts_top = 0;

			int sum_bot = 0;
			int sum_top = 0;


//				for(int idx = 1; idx < 7; ++idx)//only 1stop

			for(int idx = -max_count; idx < max_count; ++idx)
			{
				if(idx == 0)
				{
					continue;
				}

				const int nowazi_bot = azimuth_index + idx*3;

				if(nowazi_bot < 0 || nowazi_bot >= azimuth_count)
				{
					continue;
				}

				const int nowazi_top = nowazi_bot + top_bottom_offset ;

				int sumdistance_bot = block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
				int sumdistance_top = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

				if(sumdistance_bot == 0 && sumdistance_top == 0)
				{
					continue;
				}

				//bot
				if(botdistance != 0 && sumdistance_bot != 0)
				{
					if(botdistance > distance1)
					{
						if(sumdistance_bot - botdistance < threshold2 && sumdistance_bot - botdistance > -threshold2)
						{
							counts_bot += 1;

							if(ismafON)
							{
								sum_bot += block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance_bot - botdistance < threshold && sumdistance_bot - botdistance > -threshold)
						{
							counts_bot += 1;

							if(ismafON)
							{
								sum_bot += block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
							}
						}
					}
				}


				//top
				if(topdistance != 0 && sumdistance_top != 0)
				{
					if(topdistance > distance1)
					{
						if(sumdistance_top - topdistance < threshold2 && sumdistance_top - topdistance > -threshold2)
						{
							counts_top += 1;

							if(ismafON)
							{
								sum_top += block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance_top - topdistance < threshold && sumdistance_top - topdistance > -threshold)
						{
							counts_top += 1;

							if(ismafON)
							{
								sum_top += block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}
				}
			}


			temp_cot_bot[azimuth_index][vertical_angle_index] = counts_bot;
			temp_cot_top[azimuth_index][vertical_angle_index] = counts_top;

			temp_sum_bot[azimuth_index][vertical_angle_index] = sum_bot;
			temp_sum_top[azimuth_index][vertical_angle_index] = sum_top;


//			continue;

			if(botdistance != 0)
			{
				if(counts_bot < count1 && botdistance < distance1)	//5m
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts_bot < count2 && botdistance < distance2 && botdistance >= distance1)	//20m
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}


//					if(ismafON && counts_bot > 1)
//				if(ismafON && counts_bot >= count1)	//test
//				{
//					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum_bot / counts_bot;
//				}
			}



			if(topdistance != 0)
			{
				if(counts_top < count1 && topdistance < distance1)	//5m
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts_top < count2 && topdistance < distance2 && topdistance >= distance1)	//20m
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}


//				if(ismafON && counts_top > 1)
//				{
//					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top / counts_top;
//				}
			}



		}
	}


//	return;

	//do
	//do rerun maf

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
	{
		for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
		{
			const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
			const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

			if(botdistance == 0 &&  topdistance == 0)
			{
				continue;
			}

			int counts_bot = 0;
			int counts_top = 0;

			int sum_bot = 0;
			int sum_top = 0;


			for(int idx = -max_count; idx < max_count; ++idx)
			{
				if(idx == 0)
				{
					continue;
				}

				const int nowazi_bot = azimuth_index + idx*3;

				if(nowazi_bot < 0 || nowazi_bot >= azimuth_count)
				{
					continue;
				}

				const int nowazi_top = nowazi_bot + top_bottom_offset ;

				int sumdistance_bot = block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
				int sumdistance_top = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

				if(sumdistance_bot == 0 && sumdistance_top == 0)
				{
					continue;
				}

				//bot
				if(botdistance != 0 && sumdistance_bot != 0)
				{
					if(botdistance > distance1)
					{
						if(sumdistance_bot - botdistance < threshold2 && sumdistance_bot - botdistance > -threshold2)
						{
							counts_bot += 1;

							sum_bot += block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;

						}
					}
					else
					{
						if(sumdistance_bot - botdistance < threshold && sumdistance_bot - botdistance > -threshold)
						{
							counts_bot += 1;

							sum_bot += block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;

						}
					}
				}


				//top
				if(topdistance != 0 && sumdistance_top != 0)
				{
					if(topdistance > distance1)
					{
						if(sumdistance_top - topdistance < threshold2 && sumdistance_top - topdistance > -threshold2)
						{
							counts_top += 1;

							sum_top += block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
						}
					}
					else
					{
						if(sumdistance_top - topdistance < threshold && sumdistance_top - topdistance > -threshold)
						{
							counts_top += 1;

							sum_top += block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
						}
					}
				}
			}



			if(botdistance != 0 && counts_bot != 0)
			{
				if(ismafON && counts_bot > 2)
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum_bot / counts_bot;
				}
			}


			if(topdistance != 0 && counts_top != 0)
			{
				if(ismafON && counts_top > 2)
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top / counts_top;
				}
			}

		}
	}



//	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
//	{
//		for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
//		{
//			const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
//			const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;
//
//			if(botdistance == 0 &&  topdistance == 0)
//			{
//				continue;
//			}
//
//			const int counts_bot = temp_cot_bot[azimuth_index][vertical_angle_index];
//			const int counts_top = temp_cot_top[azimuth_index][vertical_angle_index];
//
//			const int sum_bot = temp_sum_bot[azimuth_index][vertical_angle_index];
//			const int sum_top = temp_sum_top[azimuth_index][vertical_angle_index];
//
//			if(botdistance != 0 && counts_bot != 0)
//			{
//				if(ismafON && counts_bot > 2)
//				{
//					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum_bot / counts_bot;
//				}
//			}
//
//
//			if(topdistance != 0 && counts_top != 0)
//			{
//				if(ismafON && counts_top > 2)
//				{
//					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top / counts_top;
//				}
//			}
//
//		}
//	}

}

//To DO
void FIR_Filter_COEF1(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state)
{
	const int threshold = lidar_state->FIR_setting.threshold1;		//org 33
	const int threshold2 = lidar_state->FIR_setting.threshold2;	//org64

	const int distance1 = lidar_state->FIR_setting.distance1;		//org1280
	const int distance2 = lidar_state->FIR_setting.distance2;		//org3328

	const unsigned int count1 = lidar_state->FIR_setting.count1;	//org7
	const unsigned int count2 = lidar_state->FIR_setting.count2;	//org1

	int max_count = lidar_state->FIR_setting.max_count;			//org 7

	const int ismafON = lidar_state->FIR_setting.ison;			//org 0


	if(max_count < 1)
	{
		return;
	}
	else if(max_count > 3)
	{
		max_count = 3;
	}



	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
	{
		for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
		{
			const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
			const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

			if(botdistance == 0 &&  topdistance == 0)
			{
				continue;
			}

			int counts_bot = 0;
			int counts_top = 0;


			for(int idx = -max_count; idx < max_count; ++idx)
			{
				if(idx == 0)
				{
					continue;
				}

				const int nowazi_bot = azimuth_index + idx*3;

				if(nowazi_bot < 0 || nowazi_bot >= azimuth_count)
				{
					continue;
				}

				const int nowazi_top = nowazi_bot + top_bottom_offset ;

				int sumdistance_bot = block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
				int sumdistance_top = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

				if(sumdistance_bot == 0 && sumdistance_top == 0)
				{
					continue;
				}

				//bot
				if(botdistance != 0 && sumdistance_bot != 0)
				{
					if(botdistance > distance1)
					{
						if(sumdistance_bot - botdistance < threshold2 && sumdistance_bot - botdistance > -threshold2)
						{
							counts_bot += 1;
						}
					}
					else
					{
						if(sumdistance_bot - botdistance < threshold && sumdistance_bot - botdistance > -threshold)
						{
							counts_bot += 1;
						}
					}
				}


				//top
				if(topdistance != 0 && sumdistance_top != 0)
				{
					if(topdistance > distance1)
					{
						if(sumdistance_top - topdistance < threshold2 && sumdistance_top - topdistance > -threshold2)
						{
							counts_top += 1;
						}
					}
					else
					{
						if(sumdistance_top - topdistance < threshold && sumdistance_top - topdistance > -threshold)
						{
							counts_top += 1;
						}
					}
				}
			}



			if(botdistance != 0)
			{
				if(counts_bot < count1 && botdistance < distance1)	//5m
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts_bot < count2 && botdistance < distance2 && botdistance >= distance1)	//20m
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}
			}


			if(topdistance != 0)
			{
				if(counts_top < count1 && topdistance < distance1)	//5m
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts_top < count2 && topdistance < distance2 && topdistance >= distance1)	//20m
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
			}



		}
	}


//	return;


	//do rerun maf
	if(ismafON == 0)
	{
		return;
	}

	//	const int FIR_LEFT_RIGHT[7] = {	0, 3, 2, 2,  1, 1, 0};
	int FIR_COEF[7] = {	0x0,};

	for(int i = 1; i<7; ++i)
	{
		FIR_COEF[i] = lidar_state->FIR_setting.FIR_COEF[i];
//		FIR_COEF[i] = lidar_state->FIR_setting.FIR_COEF[i] & 0x1;
	}

	for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
	{
		for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
		{
			const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
			const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

			if(botdistance == 0 &&  topdistance == 0)
			{
				continue;
			}

			int counts_bot = 0;
			int counts_top = 0;

			int sum_bot = 0;
			int sum_top = 0;


			for(int idx = -max_count; idx < max_count; ++idx)
			{
				if(idx == 0)
				{
					continue;
				}

				int NOW_COEF = 0;
				if(max_count < 0)
				{
					NOW_COEF = 	FIR_COEF[max_count * -1];
				}
				else
				{
					NOW_COEF = 	FIR_COEF[max_count];
				}


				const int nowazi_bot = azimuth_index + idx*3;

				if(nowazi_bot < 0 || nowazi_bot >= azimuth_count)
				{
					continue;
				}

				const int nowazi_top = nowazi_bot + top_bottom_offset ;

				int sumdistance_bot = block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
				int sumdistance_top = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

				if(sumdistance_bot == 0 && sumdistance_top == 0)
				{
					continue;
				}

				//bot
				if(botdistance != 0 && sumdistance_bot != 0)
				{
					if(botdistance > distance1)
					{
						if(sumdistance_bot - botdistance < threshold2 && sumdistance_bot - botdistance > -threshold2)
						{
							counts_bot += NOW_COEF;

							sum_bot += NOW_COEF * block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;

						}
					}
					else
					{
						if(sumdistance_bot - botdistance < threshold && sumdistance_bot - botdistance > -threshold)
						{
							counts_bot += NOW_COEF;

							sum_bot += NOW_COEF * block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;

						}
					}
				}


				//top
				if(topdistance != 0 && sumdistance_top != 0)
				{
					if(topdistance > distance1)
					{
						if(sumdistance_top - topdistance < threshold2 && sumdistance_top - topdistance > -threshold2)
						{
							counts_top += NOW_COEF;

							sum_top += NOW_COEF * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
						}
					}
					else
					{
						if(sumdistance_top - topdistance < threshold && sumdistance_top - topdistance > -threshold)
						{
							counts_top += NOW_COEF;

							sum_top += NOW_COEF * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
						}
					}
				}
			}



			if(botdistance != 0 && counts_bot != 0)
			{
				if(counts_bot > 2)
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum_bot / counts_bot;
				}
			}


			if(topdistance != 0 && counts_top != 0)
			{
				if(counts_top > 2)
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top / counts_top;
				}
			}

		}
	}


}

//seperate firs
void FIR_Filter_Ground3(RawDataBlock *block_data_shared, TupleII *ground_point_ary, int ground_point_size, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state)
{
	//vremove start

	const int isVON = lidar_state->FIR_setting_vremove.ison;


	int FIR_COEF[7] = {	0x0,};


	if(isVON)	//vremove
	{
		for(int i = 1; i<7; ++i)
		{
			FIR_COEF[i] = lidar_state->FIR_setting_vremove.FIR_COEF[i];
		}

		const int Vthreshold = lidar_state->FIR_setting_vremove.threshold1;		//org 33
		const int Vthreshold2 = lidar_state->FIR_setting_vremove.threshold2;	//org64

		const int Vdistance1 = lidar_state->FIR_setting_vremove.distance1;		//org1280
		const int Vdistance2 = lidar_state->FIR_setting_vremove.distance2;		//org3328

		const unsigned int Vcount1 = lidar_state->FIR_setting_vremove.count1;	//org7
		const unsigned int Vcount2 = lidar_state->FIR_setting_vremove.count2;	//org1

		int Vmax_count = lidar_state->FIR_setting_vremove.max_count;			//org 7

		if(Vmax_count < 1)
		{
			Vmax_count = 0;
		}
		else if(Vmax_count > 6)
		{
			Vmax_count = 6;
		}


		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
				const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int counts_bot = 0;
				int counts_top = 0;


				for(int idx = -Vmax_count; idx < Vmax_count; ++idx)
				{
					if(idx == 0)
					{
						continue;
					}

					int NOW_COEF = 0;

					if(idx < 0)
					{
						NOW_COEF = 	FIR_COEF[idx * -1];
					}
					else
					{
						NOW_COEF = 	FIR_COEF[idx];
					}

					const int nowazi_bot = azimuth_index + idx*3;

					if(nowazi_bot < 0 || nowazi_bot >= azimuth_count)
					{
						continue;
					}

					const int nowazi_top = nowazi_bot + top_bottom_offset ;

					int sumdistance_bot = block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
					int sumdistance_top = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

					if(sumdistance_bot == 0 && sumdistance_top == 0)
					{
						continue;
					}

					//bot
					if(botdistance != 0 && sumdistance_bot != 0 && botdistance < Vdistance2)
					{
						if(botdistance > Vdistance1)
						{
							if(sumdistance_bot - botdistance < Vthreshold2 && sumdistance_bot - botdistance > -Vthreshold2)
							{
								counts_bot += NOW_COEF;
							}
						}
						else
						{
							if(sumdistance_bot - botdistance < Vthreshold && sumdistance_bot - botdistance > -Vthreshold)
							{
								counts_bot += NOW_COEF;
							}
						}
					}


					//top
					if(topdistance != 0 && sumdistance_top != 0 && topdistance < Vdistance2)
					{
						if(topdistance > Vdistance1)
						{
							if(sumdistance_top - topdistance < Vthreshold2 && sumdistance_top - topdistance > -Vthreshold2)
							{
								counts_top += NOW_COEF;
							}
						}
						else
						{
							if(sumdistance_top - topdistance < Vthreshold && sumdistance_top - topdistance > -Vthreshold)
							{
								counts_top += NOW_COEF;
							}
						}
					}
				}



				if(botdistance != 0 && botdistance < Vdistance2)
				{
					if(counts_bot < Vcount1 && botdistance < Vdistance1)	//5m
					{
						block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
						block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
					}
					else if(counts_bot < Vcount2  && botdistance >= Vdistance1)	//20m
					{
						block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
						block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
					}
				}


				if(topdistance != 0 && topdistance < Vdistance2)
				{
					if(counts_top < Vcount1 && topdistance < Vdistance1)	//5m
					{
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
					}
					else if(counts_top < Vcount2  && topdistance >= Vdistance1)	//20m
					{
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
					}
				}
			}
		}
	}


	//vremove end



	//smoothing start

	const int ismafON = lidar_state->FIR_setting.ison;

	//smoothing

	if(ismafON)
	{
		for(int i = 1; i<7; ++i)
		{
			FIR_COEF[i] = lidar_state->FIR_setting.FIR_COEF[i];
		}

		const int threshold = lidar_state->FIR_setting.threshold1;		//org 33
		const int threshold2 = lidar_state->FIR_setting.threshold2;	//org64

		const int distance1 = lidar_state->FIR_setting.distance1;		//org1280
//		const int distance2 = lidar_state->FIR_setting.distance2;		//org3328

		const unsigned int count1 = lidar_state->FIR_setting.count1;	//org7
		const unsigned int count2 = lidar_state->FIR_setting.count2;	//org1

		int max_count = lidar_state->FIR_setting.max_count;			//org 7

		if(max_count < 1)
		{
			max_count = 0;
		}
		else if(max_count > 6)
		{
			max_count = 6;
		}

		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
				const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int counts_bot = 0;
				int counts_top = 0;

				int sum_bot = 0;
				int sum_top = 0;


				for(int idx = -max_count; idx < max_count; ++idx)
				{
					if(idx == 0)
					{
						continue;
					}

					int NOW_COEF = 0;
					if(idx < 0)
					{
						NOW_COEF = 	FIR_COEF[idx * -1];
					}
					else
					{
						NOW_COEF = 	FIR_COEF[idx];
					}


					const int nowazi_bot = azimuth_index + idx*3;

					if(nowazi_bot < 0 || nowazi_bot >= azimuth_count)
					{
						continue;
					}

					const int nowazi_top = nowazi_bot + top_bottom_offset ;

					int sumdistance_bot = block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
					int sumdistance_top = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

					if(sumdistance_bot == 0 && sumdistance_top == 0)
					{
						continue;
					}

					//bot
					if(botdistance != 0 && sumdistance_bot != 0)
					{
						if(botdistance > distance1)
						{
							if(sumdistance_bot - botdistance < threshold2 && sumdistance_bot - botdistance > -threshold2)
							{
								counts_bot += NOW_COEF;

								sum_bot += NOW_COEF * block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;

							}
						}
						else
						{
							if(sumdistance_bot - botdistance < threshold && sumdistance_bot - botdistance > -threshold)
							{
								counts_bot += NOW_COEF;

								sum_bot += NOW_COEF * block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;

							}
						}
					}


					//top
					if(topdistance != 0 && sumdistance_top != 0)
					{
						if(topdistance > distance1)
						{
							if(sumdistance_top - topdistance < threshold2 && sumdistance_top - topdistance > -threshold2)
							{
								counts_top += NOW_COEF;

								sum_top += NOW_COEF * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
						else
						{
							if(sumdistance_top - topdistance < threshold && sumdistance_top - topdistance > -threshold)
							{
								counts_top += NOW_COEF;

								sum_top += NOW_COEF * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}
				}



				if(botdistance != 0 && counts_bot != 0)
				{
					if(botdistance > distance1)
					{
						if(counts_bot > count2)
						{
							counts_bot++;
							sum_bot += botdistance;

							block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum_bot / counts_bot;
						}
					}
					else
					{
						if(counts_bot > count1)
						{
							counts_bot++;
							sum_bot += botdistance;

							block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum_bot / counts_bot;
						}
					}
				}


				if(topdistance != 0 && counts_top != 0)
				{
					if(topdistance > distance1)
					{
						if(counts_top > count2)
						{
							counts_top++;
							sum_top += topdistance;

							block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top / counts_top;
						}
					}
					else
					{
						if(counts_top > count1)
						{
							counts_top++;
							sum_top += topdistance;

							block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top / counts_top;
						}
					}
				}
			}
		}
	}

	//smoothing end



	//ground smoothing start


	const int isGmafON = lidar_state->FIR_setting_ground.ison;


	if(isGmafON)
	{
		for(int i = 1; i<7; ++i)
		{
			FIR_COEF[i] = lidar_state->FIR_setting_ground.FIR_COEF[i];
		}

		const int Gthreshold = lidar_state->FIR_setting_ground.threshold1;		//org 33
		const int Gthreshold2 = lidar_state->FIR_setting_ground.threshold2;	//org64

		const int Gdistance1 = lidar_state->FIR_setting_ground.distance1;		//org1280
		const int Gdistance2 = lidar_state->FIR_setting_ground.distance2;		//org3328

		const unsigned int Gcount1 = lidar_state->FIR_setting_ground.count1;	//org7
		const unsigned int Gcount2 = lidar_state->FIR_setting_ground.count2;	//org1

		int Gmax_count = lidar_state->FIR_setting_ground.max_count;			//org 7

		if(Gmax_count < 1)
		{
			Gmax_count = 0;
		}
		else if(Gmax_count > 6)
		{
			Gmax_count = 6;
		}

		for(int scale = 0; scale < isGmafON; ++scale)		//1x 2x 3x
		{
			//ground
			for(int idx = 0; idx < ground_point_size; ++idx)
			{
				const int ground_azi = ground_point_ary[idx].first;
				const int ground_ch = ground_point_ary[idx].second;

				const int nowdistance = block_data_shared[ground_azi].distance_[ground_ch].distance;

				if(nowdistance == 0)
				{
					continue;
				}

				int counts = 0;

				int total_sum_distance = 0;


				for(int idx = -Gmax_count; idx < Gmax_count; ++idx)
	//			for(int idx = -6; idx < 6; ++idx)
				{
					if(idx == 0)
					{
						continue;
					}

					int NOW_COEF = 0;

					if(idx < 0)
					{
						NOW_COEF = 	FIR_COEF[idx * -1];
					}
					else
					{
						NOW_COEF = 	FIR_COEF[idx];
					}


					const int nowazi = ground_azi + idx*3;


					if(nowazi < 0 || nowazi >= 2*azimuth_count)
					{
						continue;
					}
					else if(ground_azi < azimuth_count && nowazi >= azimuth_count)
					{
						continue;
					}
					else if(ground_azi >= azimuth_count && nowazi < azimuth_count)
					{
						continue;
					}


					const int sumdistance = block_data_shared[nowazi].distance_[ground_ch].distance;

					if(sumdistance == 0)
					{
						continue;
					}


					if(nowdistance > Gdistance1)
					{
						if(nowdistance < Gdistance2)
						{
							if( ((sumdistance - nowdistance) < Gthreshold2) && ((sumdistance - nowdistance) > -Gthreshold2) )
							{
								counts += NOW_COEF;

								total_sum_distance += NOW_COEF * block_data_shared[nowazi].distance_[ground_ch].distance;
							}
						}
						else	//more than distance2
						{
							if( ((sumdistance - nowdistance) < Gthreshold2*2) && ((sumdistance - nowdistance) > -(Gthreshold2*2)) )
							{
								counts += NOW_COEF;

								total_sum_distance += NOW_COEF * block_data_shared[nowazi].distance_[ground_ch].distance;
							}
						}
					}
					else
					{
						if( ((sumdistance - nowdistance) < Gthreshold) && ((sumdistance - nowdistance) > -Gthreshold) )
						{
							counts += NOW_COEF;

							total_sum_distance += NOW_COEF * block_data_shared[nowazi].distance_[ground_ch].distance;
						}
					}
				}


				if(nowdistance != 0 && counts != 0)
				{
					if(nowdistance > Gdistance1)
					{
						if(counts > Gcount2)
						{
							counts++;
							total_sum_distance += nowdistance;
							block_data_shared[ground_azi].distance_[ground_ch].distance = total_sum_distance / counts;
						}
					}
					else
					{
						if(counts > Gcount1)
						{
							counts++;
							total_sum_distance += nowdistance;
							block_data_shared[ground_azi].distance_[ground_ch].distance = total_sum_distance / counts;
						}
					}
				}
			}
		}

	}



}


//seperate firs
void FIR_Filter_Ground4(RawDataBlock *block_data_shared, TupleII *ground_point_ary, int ground_point_size, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state)
{
	//vremove start

	const int isVON = lidar_state->FIR_setting_vremove.ison;


	int FIR_COEF[7] = {	0x0,};


	if(isVON)	//vremove
	{
		for(int i = 1; i<7; ++i)
		{
			FIR_COEF[i] = lidar_state->FIR_setting_vremove.FIR_COEF[i];
		}

		const int Vthreshold = lidar_state->FIR_setting_vremove.threshold1;		//org 33
		const int Vthreshold2 = lidar_state->FIR_setting_vremove.threshold2;	//org64

		const int Vdistance1 = lidar_state->FIR_setting_vremove.distance1;		//org1280
		const int Vdistance2 = lidar_state->FIR_setting_vremove.distance2;		//org3328

		const unsigned int Vcount1 = lidar_state->FIR_setting_vremove.count1;	//org7
		const unsigned int Vcount2 = lidar_state->FIR_setting_vremove.count2;	//org1

		int Vmax_count = lidar_state->FIR_setting_vremove.max_count;			//org 7

		if(Vmax_count < 1)
		{
			Vmax_count = 0;
		}
		else if(Vmax_count > 6)
		{
			Vmax_count = 6;
		}


		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
				const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int counts_bot = 0;
				int counts_top = 0;


				for(int idx = -Vmax_count; idx < Vmax_count; ++idx)
				{
					if(idx == 0)
					{
						continue;
					}

					int NOW_COEF = 0;

					if(idx < 0)
					{
						NOW_COEF = 	FIR_COEF[idx * -1];
					}
					else
					{
						NOW_COEF = 	FIR_COEF[idx];
					}

					const int nowazi_bot = azimuth_index + idx*3;

					if(nowazi_bot < 0 || nowazi_bot >= azimuth_count)
					{
						continue;
					}

					const int nowazi_top = nowazi_bot + top_bottom_offset ;

					int sumdistance_bot = block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
					int sumdistance_top = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

					if(sumdistance_bot == 0 && sumdistance_top == 0)
					{
						continue;
					}

					//bot
					if(botdistance != 0 && sumdistance_bot != 0 && botdistance < Vdistance2)
					{
						if(botdistance > Vdistance1)
						{
							if(sumdistance_bot - botdistance < Vthreshold2 && sumdistance_bot - botdistance > -Vthreshold2)
							{
								counts_bot += NOW_COEF;
							}
						}
						else
						{
							if(sumdistance_bot - botdistance < Vthreshold && sumdistance_bot - botdistance > -Vthreshold)
							{
								counts_bot += NOW_COEF;
							}
						}
					}


					//top
					if(topdistance != 0 && sumdistance_top != 0 && topdistance < Vdistance2)
					{
						if(topdistance > Vdistance1)
						{
							if(sumdistance_top - topdistance < Vthreshold2 && sumdistance_top - topdistance > -Vthreshold2)
							{
								counts_top += NOW_COEF;
							}
						}
						else
						{
							if(sumdistance_top - topdistance < Vthreshold && sumdistance_top - topdistance > -Vthreshold)
							{
								counts_top += NOW_COEF;
							}
						}
					}
				}



				if(botdistance != 0 && botdistance < Vdistance2)
				{
					if(counts_bot < Vcount1 && botdistance < Vdistance1)	//5m
					{
						block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
						block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
					}
					else if(counts_bot < Vcount2  && botdistance >= Vdistance1)	//20m
					{
						block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
						block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
					}
				}


				if(topdistance != 0 && topdistance < Vdistance2)
				{
					if(counts_top < Vcount1 && topdistance < Vdistance1)	//5m
					{
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
					}
					else if(counts_top < Vcount2  && topdistance >= Vdistance1)	//20m
					{
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
					}
				}
			}
		}
	}


	//vremove end



	//smoothing start

	const int ismafON = lidar_state->FIR_setting.ison;

	//smoothing

	if(ismafON)
	{
		for(int i = 1; i<7; ++i)
		{
			FIR_COEF[i] = lidar_state->FIR_setting.FIR_COEF[i];
		}

		const int threshold = lidar_state->FIR_setting.threshold1;		//org 33
		const int threshold2 = lidar_state->FIR_setting.threshold2;	//org64

		const int distance1 = lidar_state->FIR_setting.distance1;		//org1280
//		const int distance2 = lidar_state->FIR_setting.distance2;		//org3328

		const unsigned int count1 = lidar_state->FIR_setting.count1;	//org7
		const unsigned int count2 = lidar_state->FIR_setting.count2;	//org1

		int max_count = lidar_state->FIR_setting.max_count;			//org 7

		if(max_count < 1)
		{
			max_count = 0;
		}
		else if(max_count > 6)
		{
			max_count = 6;
		}

		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
				const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int counts_bot = 0;
				int counts_top = 0;

				int sum_bot = 0;
				int sum_top = 0;


				for(int idx = -max_count; idx < max_count; ++idx)
				{
					if(idx == 0)
					{
						continue;
					}

					int NOW_COEF = 0;
					if(idx < 0)
					{
						NOW_COEF = 	FIR_COEF[idx * -1];
					}
					else
					{
						NOW_COEF = 	FIR_COEF[idx];
					}


					const int nowazi_bot = azimuth_index + idx*3;

					if(nowazi_bot < 0 || nowazi_bot >= azimuth_count)
					{
						continue;
					}

					const int nowazi_top = nowazi_bot + top_bottom_offset ;

					int sumdistance_bot = block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;
					int sumdistance_top = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

					if(sumdistance_bot == 0 && sumdistance_top == 0)
					{
						continue;
					}

					//bot
					if(botdistance != 0 && sumdistance_bot != 0)
					{
						if(botdistance > distance1)
						{
							if(sumdistance_bot - botdistance < threshold2 && sumdistance_bot - botdistance > -threshold2)
							{
								counts_bot += NOW_COEF;

								sum_bot += NOW_COEF * block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;

							}
						}
						else
						{
							if(sumdistance_bot - botdistance < threshold && sumdistance_bot - botdistance > -threshold)
							{
								counts_bot += NOW_COEF;

								sum_bot += NOW_COEF * block_data_shared[nowazi_bot].distance_[vertical_angle_index].distance;

							}
						}
					}


					//top
					if(topdistance != 0 && sumdistance_top != 0)
					{
						if(topdistance > distance1)
						{
							if(sumdistance_top - topdistance < threshold2 && sumdistance_top - topdistance > -threshold2)
							{
								counts_top += NOW_COEF;

								sum_top += NOW_COEF * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
						else
						{
							if(sumdistance_top - topdistance < threshold && sumdistance_top - topdistance > -threshold)
							{
								counts_top += NOW_COEF;

								sum_top += NOW_COEF * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}
				}



				if(botdistance != 0 && counts_bot != 0)
				{
					if(botdistance > distance1)
					{
						if(counts_bot > count2)
						{
							counts_bot++;
							sum_bot += botdistance;

							block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum_bot / counts_bot;
						}
					}
					else
					{
						if(counts_bot > count1)
						{
							counts_bot++;
							sum_bot += botdistance;

							block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum_bot / counts_bot;
						}
					}
				}


				if(topdistance != 0 && counts_top != 0)
				{
					if(topdistance > distance1)
					{
						if(counts_top > count2)
						{
							counts_top++;
							sum_top += topdistance;

							block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top / counts_top;
						}
					}
					else
					{
						if(counts_top > count1)
						{
							counts_top++;
							sum_top += topdistance;

							block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top / counts_top;
						}
					}
				}
			}
		}
	}

	//smoothing end



	//ground smoothing start


	const int isGmafON = lidar_state->FIR_setting_ground.ison;


	if(isGmafON)
	{
		for(int i = 1; i<7; ++i)
		{
			FIR_COEF[i] = lidar_state->FIR_setting_ground.FIR_COEF[i];
		}

		const int Gthreshold = lidar_state->FIR_setting_ground.threshold1;		//org 33
		const int Gthreshold2 = lidar_state->FIR_setting_ground.threshold2;	//org64

		const int Gdistance1 = lidar_state->FIR_setting_ground.distance1;		//org1280
		const int Gdistance2 = lidar_state->FIR_setting_ground.distance2;		//org3328

		const unsigned int Gcount1 = lidar_state->FIR_setting_ground.count1;	//org7
		const unsigned int Gcount2 = lidar_state->FIR_setting_ground.count2;	//org1

		int Gmax_count = lidar_state->FIR_setting_ground.max_count;			//org 7

		if(Gmax_count < 1)
		{
			Gmax_count = 0;
		}
		else if(Gmax_count > 6)
		{
			Gmax_count = 6;
		}

		if(isGmafON == 9)
		{
			//ground nears
			for(int idx = 0; idx < ground_point_size; ++idx)
			{
				const int ground_azi = ground_point_ary[idx].first;
				const int ground_ch = ground_point_ary[idx].second;

				const int nowdistance = block_data_shared[ground_azi].distance_[ground_ch].distance;

				if(nowdistance == 0)
				{
					continue;
				}

				int nearazis[12] = {0x0,};

				int counts = 0;

				int total_sum_distance = 0;


				for(int idx = -Gmax_count; idx < Gmax_count; ++idx)
				{
					if(idx == 0)
					{
						continue;
					}

					int NOW_COEF = 0;

					if(idx < 0)
					{
						NOW_COEF = 	FIR_COEF[idx * -1];
					}
					else
					{
						NOW_COEF = 	FIR_COEF[idx];
					}


					const int nowazi = ground_azi + idx*3;


					if(nowazi < 0 || nowazi >= 2*azimuth_count)
					{
						continue;
					}
					else if(ground_azi < azimuth_count && nowazi >= azimuth_count)
					{
						continue;
					}
					else if(ground_azi >= azimuth_count && nowazi < azimuth_count)
					{
						continue;
					}


					const int sumdistance = block_data_shared[nowazi].distance_[ground_ch].distance;

					if(sumdistance == 0)
					{
						continue;
					}


					if(nowdistance > Gdistance1)
					{
						if(nowdistance < Gdistance2)
						{
							if( ((sumdistance - nowdistance) < Gthreshold2) && ((sumdistance - nowdistance) > -Gthreshold2) )
							{
								nearazis[counts] = nowazi;

								counts += NOW_COEF;

								total_sum_distance += NOW_COEF * block_data_shared[nowazi].distance_[ground_ch].distance;
							}
						}
						else	//more than distance2
						{
							if( ((sumdistance - nowdistance) < Gthreshold2*2) && ((sumdistance - nowdistance) > -(Gthreshold2*2)) )
							{
								nearazis[counts] = nowazi;

								counts += NOW_COEF;

								total_sum_distance += NOW_COEF * block_data_shared[nowazi].distance_[ground_ch].distance;
							}
						}
					}
					else
					{
						if( ((sumdistance - nowdistance) < Gthreshold) && ((sumdistance - nowdistance) > -Gthreshold) )
						{
							nearazis[counts] = nowazi;

							counts += NOW_COEF;

							total_sum_distance += NOW_COEF * block_data_shared[nowazi].distance_[ground_ch].distance;
						}
					}
				}


				if(nowdistance != 0 && counts != 0)
				{
					if(nowdistance > Gdistance1)
					{
						if(counts > Gcount2)
						{
							for(int i = 0; i<2; ++i)
							{
								block_data_shared[nearazis[i]].distance_[ground_ch].distance = (total_sum_distance + block_data_shared[nearazis[i]].distance_[ground_ch].distance) / (counts+1);
							}

//							if(ground_azi != 0 && ground_azi != 3 && ground_azi != azimuth_count)
//							{
//								if(nowdistance - block_data_shared[ground_azi-3].distance_[ground_ch].distance > - (Gthreshold2*2) && nowdistance - block_data_shared[ground_azi-3].distance_[ground_ch].distance < (Gthreshold2*2))
//								{
//									block_data_shared[ground_azi-3].distance_[ground_ch].distance = (total_sum_distance + block_data_shared[ground_azi-3].distance_[ground_ch].distance) / (counts+1);
//								}
//							}
//
//							if(ground_azi != azimuth_count-3 && ground_azi != 2*azimuth_count -3)
//							{
//								if(nowdistance - block_data_shared[ground_azi+3].distance_[ground_ch].distance > - (Gthreshold2*2) && nowdistance - block_data_shared[ground_azi+3].distance_[ground_ch].distance < (Gthreshold2*2))
//								{
//									block_data_shared[ground_azi+3].distance_[ground_ch].distance = (total_sum_distance + block_data_shared[ground_azi+3].distance_[ground_ch].distance) / (counts+1);
//								}
//							}

							counts++;
							total_sum_distance += nowdistance;
							block_data_shared[ground_azi].distance_[ground_ch].distance = total_sum_distance / counts;
						}
					}
					else
					{
						if(counts > Gcount1)
						{
							for(int i = 0; i<2; ++i)
							{
								block_data_shared[nearazis[i]].distance_[ground_ch].distance = (total_sum_distance + block_data_shared[nearazis[i]].distance_[ground_ch].distance) / (counts+1);
							}

//							if(ground_azi != 0 && ground_azi != 3 && ground_azi != azimuth_count)
//							{
//								if(nowdistance - block_data_shared[ground_azi-3].distance_[ground_ch].distance > - (Gthreshold*2) && nowdistance - block_data_shared[ground_azi-3].distance_[ground_ch].distance < (Gthreshold*2))
//								{
//									block_data_shared[ground_azi-3].distance_[ground_ch].distance = (total_sum_distance + block_data_shared[ground_azi-3].distance_[ground_ch].distance) / (counts+1);
//								}
//
////								block_data_shared[ground_azi-3].distance_[ground_ch].distance = (total_sum_distance + block_data_shared[ground_azi-1].distance_[ground_ch].distance) / (counts+1);
//							}
//
//							if(ground_azi != azimuth_count-3 && ground_azi != 2*azimuth_count -3)
//							{
//								if(nowdistance - block_data_shared[ground_azi+3].distance_[ground_ch].distance > - (Gthreshold*2) && nowdistance - block_data_shared[ground_azi+3].distance_[ground_ch].distance < (Gthreshold*2))
//								{
//									block_data_shared[ground_azi+3].distance_[ground_ch].distance = (total_sum_distance + block_data_shared[ground_azi+3].distance_[ground_ch].distance) / (counts+1);
//								}
////								block_data_shared[ground_azi+3].distance_[ground_ch].distance = (total_sum_distance + block_data_shared[ground_azi+1].distance_[ground_ch].distance) / (counts+1);
//							}

							counts++;
							total_sum_distance += nowdistance;
							block_data_shared[ground_azi].distance_[ground_ch].distance = total_sum_distance / counts;
						}
					}
				}
			}

		}
		else
		{
			for(int scale = 0; scale < isGmafON; ++scale)		//1x 2x 3x
			{
				//ground
				for(int idx = 0; idx < ground_point_size; ++idx)
				{
					const int ground_azi = ground_point_ary[idx].first;
					const int ground_ch = ground_point_ary[idx].second;

					const int nowdistance = block_data_shared[ground_azi].distance_[ground_ch].distance;

					if(nowdistance == 0)
					{
						continue;
					}

					int counts = 0;

					int total_sum_distance = 0;


					for(int idx = -Gmax_count; idx < Gmax_count; ++idx)
		//			for(int idx = -6; idx < 6; ++idx)
					{
						if(idx == 0)
						{
							continue;
						}

						int NOW_COEF = 0;

						if(idx < 0)
						{
							NOW_COEF = 	FIR_COEF[idx * -1];
						}
						else
						{
							NOW_COEF = 	FIR_COEF[idx];
						}


						const int nowazi = ground_azi + idx*3;


						if(nowazi < 0 || nowazi >= 2*azimuth_count)
						{
							continue;
						}
						else if(ground_azi < azimuth_count && nowazi >= azimuth_count)
						{
							continue;
						}
						else if(ground_azi >= azimuth_count && nowazi < azimuth_count)
						{
							continue;
						}


						const int sumdistance = block_data_shared[nowazi].distance_[ground_ch].distance;

						if(sumdistance == 0)
						{
							continue;
						}


						if(nowdistance > Gdistance1)
						{
							if(nowdistance < Gdistance2)
							{
								if( ((sumdistance - nowdistance) < Gthreshold2) && ((sumdistance - nowdistance) > -Gthreshold2) )
								{
									counts += NOW_COEF;

									total_sum_distance += NOW_COEF * block_data_shared[nowazi].distance_[ground_ch].distance;
								}
							}
							else	//more than distance2
							{
								if( ((sumdistance - nowdistance) < Gthreshold2*2) && ((sumdistance - nowdistance) > -(Gthreshold2*2)) )
								{
									counts += NOW_COEF;

									total_sum_distance += NOW_COEF * block_data_shared[nowazi].distance_[ground_ch].distance;
								}
							}
						}
						else
						{
							if( ((sumdistance - nowdistance) < Gthreshold) && ((sumdistance - nowdistance) > -Gthreshold) )
							{
								counts += NOW_COEF;

								total_sum_distance += NOW_COEF * block_data_shared[nowazi].distance_[ground_ch].distance;
							}
						}
					}


					if(nowdistance != 0 && counts != 0)
					{
						if(nowdistance > Gdistance1)
						{
							if(counts > Gcount2)
							{
								counts++;
								total_sum_distance += nowdistance;
								block_data_shared[ground_azi].distance_[ground_ch].distance = total_sum_distance / counts;
							}
						}
						else
						{
							if(counts > Gcount1)
							{
								counts++;
								total_sum_distance += nowdistance;
								block_data_shared[ground_azi].distance_[ground_ch].distance = total_sum_distance / counts;
							}
						}
					}
				}
			}
		}



	}



}



void FIR_Filter4(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state)
{
	const int threshold = lidar_state->FIR_setting.threshold1;		//org 33
	const int threshold2 = lidar_state->FIR_setting.threshold2;	//org64

	const int distance1 = lidar_state->FIR_setting.distance1;		//org1280
	const int distance2 = lidar_state->FIR_setting.distance2;		//org3328

	const unsigned int count1 = lidar_state->FIR_setting.count1;	//org7
	const unsigned int count2 = lidar_state->FIR_setting.count2;	//org1

	int max_count = lidar_state->FIR_setting.max_count;			//org 7

	const int ismafON = lidar_state->FIR_setting.ison;			//org 0

//	const int FIR_LEFT_RIGHT[7] = {	0, 3, 2, 2,  1, 1, 0};
	int FIR_COEF[7] = {	0x0,};

	for(int i = 0; i<7; ++i)
	{
		FIR_COEF[i] = lidar_state->FIR_setting.FIR_COEF[i];
	}

	if(max_count < 1)
	{
		max_count = 2;
	}
	else if(max_count > 7)
	{
		max_count = 7;
	}

		for(int azimuth_index = 0 ; azimuth_index < azimuth_count ; azimuth_index += 3)
		{
			for(int vertical_angle_index = 0 ; vertical_angle_index < 16 ; ++vertical_angle_index)
			{
				const int botdistance = block_data_shared[azimuth_index].distance_[vertical_angle_index].distance;
				const int topdistance = block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance;

				if(botdistance == 0 &&  topdistance == 0)
				{
					continue;
				}

				int counts = 0;
				int counts_top = 0;

				int sum = 0;
				int sum_top = 0;


//				for(int idx = 1; idx < 7; ++idx)//only 1stop
				for(int idx = 1; idx < max_count; ++idx)//only 1stop
				{
					//right
					const int nowazi = azimuth_index + idx*3;
					if(nowazi == azimuth_index || nowazi < 0 || nowazi >= azimuth_count)
					{
						continue;
					}

					int sumdistance = block_data_shared[nowazi].distance_[vertical_angle_index].distance;


					const int nowazi_top = nowazi + top_bottom_offset;

					const int factor = FIR_COEF[idx];

//					if(botdistance > 1280)
					if(botdistance > distance1)
					{
						if(sumdistance - botdistance < threshold2 && sumdistance - botdistance > -threshold2)
						{
							counts += factor;
							if(ismafON)
							{
								sum += factor * block_data_shared[nowazi].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance - botdistance < threshold && sumdistance - botdistance > -threshold)
						{
							counts += factor;
							if(ismafON)
							{
								sum += factor * block_data_shared[nowazi].distance_[vertical_angle_index].distance;
							}
						}
					}


					sumdistance = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
//					if(topdistance > 1280)
					if(topdistance > distance1)
					{
						if(sumdistance - topdistance < threshold2 && sumdistance - topdistance > -threshold2)
						{
							counts_top += factor;
							if(ismafON)
							{
								sum_top += factor * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance - topdistance < threshold && sumdistance - topdistance > -threshold)
						{
							counts_top += factor;
							if(ismafON)
							{
								sum_top += factor * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}

				}

//				for(int idx = 1; idx < 7; ++idx)//only 1stop
				for(int idx = 1; idx < max_count; ++idx)//only 1stop
				{
					//left
					const int nowazi = azimuth_index - (idx*3);
					if(nowazi == azimuth_index || nowazi < 0 || nowazi >= azimuth_count)
					{
						continue;
					}

					int sumdistance = block_data_shared[nowazi].distance_[vertical_angle_index].distance;


					const int nowazi_top = nowazi + top_bottom_offset;

//					volatile int factor = FIR_FACTOR[(nowazi - azimuth_index + 18) / 3];
//					volatile int factor = FIR_LEFT_RIGHT[idx];
					const int factor = FIR_COEF[idx];

					if(botdistance > distance1)
					{
						if(sumdistance - botdistance < threshold2 && sumdistance - botdistance > -threshold2)
						{
							counts += factor;
							if(ismafON)
							{
								sum += factor * block_data_shared[nowazi].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance - botdistance < threshold && sumdistance - botdistance > -threshold)
						{
							counts += factor;
							if(ismafON)
							{
								sum += factor * block_data_shared[nowazi].distance_[vertical_angle_index].distance;
							}
						}
					}

					sumdistance = block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;

					if(topdistance > distance1)
					{
						if(sumdistance - topdistance < threshold2 && sumdistance - topdistance > -threshold2)
						{
							counts_top += factor;

							if(ismafON)
							{
								sum_top += factor * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}
					else
					{
						if(sumdistance - topdistance < threshold && sumdistance - topdistance > -threshold)
						{
							counts_top += factor;

							if(ismafON)
							{
								sum_top += factor * block_data_shared[nowazi_top].distance_[vertical_angle_index].distance;
							}
						}
					}

				}


				if(counts < count1 && botdistance < distance1)	//5m
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts < count2 && botdistance < distance2 && botdistance >= distance1)	//20m
//				if(0)
				{
					block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index].distance_[vertical_angle_index].pulse_width = 0;
				}
//				else if(counts < 7 && botdistance < 1280)	//5m
//				else if(counts>1)	//org
				else if(counts>3)
				{
					if(ismafON && botdistance !=0)
					{
						sum /= counts;
						block_data_shared[azimuth_index].distance_[vertical_angle_index].distance = sum;
					}
				}


				if(counts_top < count1 && topdistance < distance1)	//5m
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
				else if(counts_top < count2 && topdistance < distance2 && topdistance >= distance1)	//20m
//				if(0)
				{
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = 0;
					block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].pulse_width = 0;
				}
//				else if(counts_top < 7 && topdistance < 1280)
//				else if(counts_top > 1)
				else if(counts_top > 3)
				{
					if(ismafON && topdistance !=0 )
					{
						sum_top /= counts_top;
						block_data_shared[azimuth_index + top_bottom_offset].distance_[vertical_angle_index].distance = sum_top;
					}
				}
		}
	}
}
