/*
 * remove_ghost.h
 *
 *  Created on: 2022. 6. 16.
 *      Author: ProDesk
 */

#ifndef SRC_ALGORITHMS_REMOVE_GHOST_H_
#define SRC_ALGORITHMS_REMOVE_GHOST_H_


#include <string.h>
#include <math.h>
#include <stdlib.h>
//#include "../definitions/definitions.h"
#include "./ground_detection.h"


typedef struct RemoveGhostParameter
{
	float z_offset_;
	float z_offset_2_;
	float x_offset_;
	int vertical_check_num_;
	int horizontal_check_num_;
	uint8_t is_ground_remove_on;
}RGParameter;


RGParameter rg_parameter_;
int num_of_ground_point;
//uint8_t is_ground_point[FRAME_BLOCK_SIZE][CHANNEL_SIZE];
float vertical_angle_arr_top[16];
float vertical_angle_arr_bottom[16];
void RemoveGhost(RawDataBlock* bram_data_blocks, ConvertedDataBlock* current_frame_blocks, int frame_block_size, GDResult* gd_result, u32 fov_data_block_count);
void GetPosX(float* pos_x, float distance, float vertical_angle, float azimuth);
void GetPosY(float* pos_y, float distance, float vertical_angle, float azimuth);
void InitRGParameter(RGParameter* rg_parameter);

#endif /* SRC_ALGORITHMS_REMOVE_GHOST_H_ */
