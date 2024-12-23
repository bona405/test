/*
 * fov_data_set.h
 *
 *  Created on: 2022. 7. 26.
 *      Author: Wonju
 */

#ifndef SRC_MODELS_FOV_DATA_SET_H_
#define SRC_MODELS_FOV_DATA_SET_H_


#include "machine_info.h"
#include "stdlib.h"
#include "../../definitions/definitions.h"
#include "../../api_hw/api_common.h"
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
	RawDataBlock data_blocks[10000];
} SharedDataBlocks;

void SetAzimuthAndDistanceA3(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset);
void SetAzimuthAndDistanceB1(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int hz_option);
void SetAzimuthAndDistanceB1_AziOffset(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int distance_offset, u32 bottom_start_enc_cnt, u32 top_start_enc_cnt, int bottom_horz_offset, int hz_option, Azioffsets_ azioffset);
void SetAzimuthAndDistance(ConvertedDataBlock* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int bottom_horz_offset, int distance_offset);
void SetAzimuthAndDistance32(ConvertedDataBlock32* current_frame_blocks, RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, LidarState* lidar_state, int* routing_delay, int bottom_horz_offset, int distance_offset);
void SetBottomHorizontalOffset(RawDataBlock *block_data_shared, RawDataBlock *temp_block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, int bottom_horz_offset);
void SetTemperatureAdjOffset(RawDataBlock *block_data_shared, uint32_t azimuth_count, uint32_t vertical_angle_count, uint32_t top_bottom_offset, int adj_offset);
#endif /* SRC_MODELS_FOV_DATA_SET_H_ */
