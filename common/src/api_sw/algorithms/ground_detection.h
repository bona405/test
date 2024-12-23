/*
 * ground_detection.h
 *
 *  Created on: 2022. 6. 16.
 *      Author: ProDesk
 */

#ifndef SRC_ALGORITHMS_GROUND_DETECTION_H_
#define SRC_ALGORITHMS_GROUND_DETECTION_H_

#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "../../definitions/definitions.h"
#include "../models/fov_data_set.h"
#include "../container/common.h"

#define MAX_GROUND_POINT_NUM 10000
#define IMAGE_HEIGHT 16
#if defined(G32_10HZ)
#define IMAGE_WIDTH 769

#elif defined(G32_25HZ)
#define IMAGE_WIDTH 720

#endif

#define GD_INDEX_OFFSET 3
#define ECHO_NUMBER  3

#define CHANNEL_COUNT 32
#define CHANNEL_SIZE 16
#define raw_data_block_COUNT 16

typedef struct GroundDetectionParameter
{
	float max_range_;
	float min_range_;
	int detection_start_azimuth_;
	float angle_max_1_;
	float angle_max_2_;
	float height_min_1_;
	float height_min_2_;
	float sensor_height_;
	float abnormal_distance_check_th_;
	float distance_diff_;
	float max_height_;
	float min_height_;
	int ground_point_limit_;
	float ground_z_devi_limit_;
	float ground_seg_sensor_height_;
	unsigned int ground_seg_sensor_height_change_cnt_;
}GDParameter;

typedef struct GroundDetectionResult
{
//	float ground_z_value_;
	int ground_point_cnt_;
	uint8_t is_ground_detected_;
	float gounrd_z_mean_;
	float ground_z_deviation_;
}GDResult;

float ground_detect_max_distance_;
float ground_detect_z_offset_;

GDParameter gd_parameter_;
GDResult gd_result_;
TupleII ground_point_indices[MAX_GROUND_POINT_NUM];
int ground_point_indices_num;
float vertical_angle_arr_top[16];
float vertical_angle_arr_bottom[16];
//float vertical_angle_arr[32];

void InitGDParameter(GDParameter* gd_parameter);
void InitGDResult(GDResult* gd_result);

void DetectGround(ConvertedDataBlock* current_frame_data, uint8_t is_bottom_first, u32 fov_data_block_count);
//void DetectGround(rx_data_block* current_frame_blocks);

void GetPosZ(float* pos_z, float distance, float sin_vertical_angle);

void DownSampling(RawDataBlock* current_frame_data, uint8_t is_bottom_first, u32 fov_data_block_count);
typedef struct ImageFilterParameter
{
	int width_;
	int height_;
	float max_range_;
	float min_range_;
	float th_g_; // ground threshold offset
	float sigma_; // maximum road slope
	float deltaR_; // check distance interval(m) // A large value of deltaR causes under-segmentation
	int length_;

}IFParameter;

typedef struct PointXYZ
{
	float x;
	float y;
	float z;
	int r;
	float a;
}PointXYZ;

IFParameter if_parameter_;
void InitIFParameter(IFParameter* if_parameter);
void ConvertDataToXYZPoints(ConvertedDataBlock* current_frame_data, uint8_t is_bottom_first, uint8_t (*remove_noise_intensity)[CHANNEL_SIZE]);
void DistanceToXYZ(float distance, float vertical_angle, float azimuth, float* pos_x, float* pos_y, float* pos_z);
void DistanceToXYZCoord(float distance, int azimuth_index, int vertical_index, float* pos_x, float* pos_y, float* pos_z);

PointXYZ cloud_[MAX_GROUND_POINT_NUM];
int cloud_num;
void calAngle(float x, float y, float* temp_tangle);
void calRange(const PointXYZ* p, float* range);
void RangeProjection();
void RECM();
void JCP();
void calRangeDiff(const PointXYZ* p1, const PointXYZ* p2, float* range);
void GetGroundDetectionResult(GDResult* gd_result);
void GetMeanZ();
#define POINT_SIZE IMAGE_WIDTH * IMAGE_HEIGHT

typedef struct RGB
{
	uint8_t b;
	uint8_t g;
	uint8_t r;
}RGB;

typedef struct PixelIndex
{
	int x;
	int y;
}PixelIndex;

RGB range_image[IMAGE_HEIGHT][IMAGE_WIDTH];
uint8_t ground_channel[IMAGE_HEIGHT][IMAGE_WIDTH];
uint8_t dilated_ground_channel[IMAGE_HEIGHT][IMAGE_WIDTH];
//uint8_t ground_channel[CHANNEL_COUNT][MAX_GROUND_POINT_NUM / CHANNEL_COUNT];
//uint8_t dilated_ground_channel[CHANNEL_COUNT][MAX_GROUND_POINT_NUM / CHANNEL_COUNT];
uint8_t region_[IMAGE_HEIGHT][IMAGE_WIDTH];
float region_minz_[IMAGE_WIDTH * 34];
int cloud_index_[POINT_SIZE];
const int neighborx_[24];
const int neighbory_[24];
unsigned int last_ground_seg_sensor_height_change_cnt;

float cos_vert_angle_map_bottom[CHANNEL_COUNT/2];
float sin_vert_angle_map_bottom[CHANNEL_COUNT/2];
float cos_vert_angle_map_top[CHANNEL_COUNT/2];
float sin_vert_angle_map_top[CHANNEL_COUNT/2];
float sin_horz_angle_map[MAX_GROUND_POINT_NUM];
float cos_horz_angle_map[MAX_GROUND_POINT_NUM];
void SetIntensity(RawDataBlock* current_frame_data, RawDataBlock* intensity_data);
void InitTrigFunctionMap(ConvertedDataBlock* current_frame_data, uint8_t is_bottom_first, u32 fov_data_block_count);
uint8_t is_trig_function_map_initialized;

#endif /* SRC_ALGORITHMS_GROUND_DETECTION_H_ */
