/*
 * remove_noise.h
 *
 *  Created on: 2022. 8. 4.
 *      Author: Wonju
 */

#ifndef SRC_ALGORITHMS_REMOVE_NOISE_H_
#define SRC_ALGORITHMS_REMOVE_NOISE_H_

#define MAX_NEIGHBOR_POINT_SIZE 25

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "../container/common.h"
#include "../container/vector.h"
#include "../../definitions/definitions.h"
#include "../models/fov_data_set.h"
#include "../perf_test/perf_test.h"

#define MAX_POINT_COUNT 10000

#pragma pack(push, 1)
typedef struct RemoveNoiseScore
{
	double vertical_angle_;
	uint8_t shadow_score_;
	uint8_t cur_frame_self_score_;

	double neighbor_score_;
	uint8_t no_target_count_;
	uint8_t pre_frame_neighbor_point_count_;
	uint8_t cur_frame_neighbor_point_count_;

	double distance_min;
	double distance_max;
	TupleII target_point_index_;
	TupleII frame_neighbor_point_index_[MAX_NEIGHBOR_POINT_SIZE]; // TupleII
	uint16_t frame_neighbor_point_index_size_;
	uint8_t is_noise_point_;
}RemoveNoiseScore;
#pragma pack(pop)

TupleII frame_neighbor_point_index[MAX_NEIGHBOR_POINT_SIZE];
size_t neighbor_point_index_size;
RemoveNoiseScore lidar_point_cloud_data_score_[MAX_POINT_COUNT][ONE_SIDE_CHANNEL_COUNT];
uint8_t is_noise_score_calculated[MAX_POINT_COUNT][ONE_SIDE_CHANNEL_COUNT];
double vertical_angle_array_top[16];
double vertical_angle_array_bottom[16];
uint8_t is_score_map_init;
uint8_t check_azimuth_first;
uint8_t set_noise_point_intensity;
uint8_t ignore_ground_point;
int remove_noise_min_distance_;
int close_noise_neighbor_size_;
unsigned int distance_limit_;
uint32_t frame_count;

void InitRemoveNoise();

void InitScoreMap();
void SetNeighborPointIndex(u32 fov_data_block_count);
//void RemoveNoise(ConvertedDataBlock current_frame_blocks[FRAME_DATA_COUNT], ConvertedDataBlock last_frame_blocks[FRAME_DATA_COUNT], int bottom_horizontal_offset, volatile RawDataBlock *block_data_shared, uint8_t is_bottom_horizontal_offset_changed, uint32_t frame_count);

void CalculateNoiseScore(ConvertedDataBlock* current_frame_blocks, ConvertedDataBlock* last_frame_blocks, int bottom_horizontal_offset, volatile uint8_t is_bottom_horizontal_offset_changed, uint32_t frame_count, u32 fov_data_block_count);
void CalculateNoiseScore2(ConvertedDataBlock* current_frame_blocks, ConvertedDataBlock* last_frame_blocks, int bottom_horizontal_offset, u32 fov_data_block_count);

inline void GetNeighborPointIndex(TupleII neighbor_point_index[MAX_NEIGHBOR_POINT_SIZE], size_t* frame_neighbor_point_index_size, TupleII target_point_index, int azimuth_index_offset, int dist_neighbor, int top_bottom_index, uint8_t is_add_myself, u32 fov_data_block_count);


void RemoveNoisePoints(ConvertedDataBlock* current_frame_blocks, int bottom_horizontal_offset, RawDataBlock *block_data_shared, unsigned int* noise_point_cnt, u32 fov_data_block_count);
void RemoveNoisePoints2(ConvertedDataBlock* current_frame_blocks, int bottom_horizontal_offset, RawDataBlock *block_data_shared, unsigned int* noise_point_cnt, u32 fov_data_block_count);
#endif /* SRC_ALGORITHMS_REMOVE_NOISE_H_ */
