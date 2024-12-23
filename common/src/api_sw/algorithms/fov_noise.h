/*
 * fov_data_set.h
 *
 *  Created on: 2022. 7. 26.
 *      Author: Wonju
 */

#ifndef SRC_MODELS_FOV_DATA_SET_H_
#define SRC_MODELS_FOV_DATA_SET_H_


#include "../models/machine_info.h"
#include "stdlib.h"
#include "../../definitions/definitions.h"


#include "../container/common.h"


//#include <arm_neon.h> //neon

//#define MAX_DISTANCE 50200
#define MAX_DISTANCE 64000
#define MAX_DISTANCE_M 250.0
//#define MAX_DISTANCE_M 196.0
#define DIV_256 0.00390625
#define ONE_SIDE_CHANNEL_COUNT 16

#pragma pack(push, 1)
typedef struct
{
	uint16_t			distance;
	uint16_t			pulse_width;			// 4 byte
}DataPoint;

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t pps_cnt[3];
	int16_t angle_diff;
	uint8_t fsm_status;
	uint8_t motor_freq_adpt[3];
	uint8_t iDPLL_lock;
}G32PPSTime;

#pragma pack(pop)

typedef struct
{
	int32_t			azimuth_;
	DataPoint			distance_[16];			// 4 byte
	G32PPSTime pps_info;
}RawDataBlock;

typedef struct converted_data_block
{
	float	azimuth_;
	float	distance_[16];			// 4 byte
}ConvertedDataBlock;

typedef struct ConvertedDataBlock32
{
	float	azimuth_;
	float	distance_[32];			// 4 byte
}ConvertedDataBlock32;

typedef struct
{
	RawDataBlock data_blocks[10000];	//org
//	RawDataBlock data_blocks[5000];
} SharedDataBlocks;

typedef struct
{
	unsigned char isnoise[16];
	unsigned char ischecked[16];
}NoiseDatas;

typedef struct
{
	unsigned char isnoise[16];
	unsigned char ischecked[16];
	int count[16];
}NoiseDatas2;

typedef struct
{
	unsigned char ischecked[16];
}SimpleNoiseDatas;

typedef struct
{
	int azimuth;
	int ch;
	int cot;
	int cot2;
}Nodes;

typedef struct
{
	int azimuthoffset;
	int choffset;
	int cot;
}SimpleNodes;

typedef struct
{
	int azimuthoffset;
	int choffset;
}SimpleNodes2;


typedef struct
{
	int azimuthoffset;
	int choffset;
}PreNode;



void SetAzimuthAndDistanceA3_test3(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt);
void SetAzimuthAndDistanceA3_Sparse(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt, RawDataBlock *last_block_data);
void SetAzimuthAndDistanceB1_Sparse(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt, RawDataBlock *last_block_data, int hz_option, int* routing_delay2, int routing_range_1, int routing_range_2);
void SetAzimuthAndDistanceB1_Sparse_AziOffset(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt, RawDataBlock *last_block_data, int hz_option, int* routing_delay2, int routing_range_1, int routing_range_2
		, Azioffsets_ azioffset);
void SetAzimuthAndDistanceB1_Sparse_test(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int intenfilter, unsigned int* noise_point_cnt, RawDataBlock *last_block_data, int hz_option, int* routing_delay2, int routing_range_1, int routing_range_2);
void FIR_Filter(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state);
void FIR_Filter2(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state);
void FIR_Filter3(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state);
void FIR_Filter_FLAT2(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state);
void FIR_Filter_COEF1(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state);
void FIR_Filter_Ground2(RawDataBlock *block_data_shared, TupleII *ground_point_ary, int ground_point_size, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state);
void FIR_Filter_Ground3(RawDataBlock *block_data_shared, TupleII *ground_point_ary, int ground_point_size, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state);
void FIR_Filter_Ground4(RawDataBlock *block_data_shared, TupleII *ground_point_ary, int ground_point_size, uint32_t azimuth_count, uint32_t top_bottom_offset, LidarState* lidar_state);

#endif /* SRC_MODELS_FOV_DATA_SET_H_ */
