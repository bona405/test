/*
 * exhaust_filter.h
 *
 *  Created on: 2024. 4. 11.
 *      Author: AutoL
 */

#ifndef SRC_API_SW_ALGORITHMS_EXHAUST_FILTER_H_
#define SRC_API_SW_ALGORITHMS_EXHAUST_FILTER_H_

#include "fov_noise.h"

#include "../perf_test/perf_test.h"

#include <math.h>
#ifndef PI
#define PI 3.14159265358979323846
#endif
extern const double elevation_forz[32];

#define UNKNOWNCLUSTER 0
#define NOTCLUSTER 1
#define CLUSTERED 2

#pragma pack(push, 1)
typedef struct StackNode
{
	DataPoint* p;
	size_t data_block_idx;
	uint8_t ch;
	uint8_t ch_32;
	struct StackNode* prevStackNode;
} StackNode;

typedef struct Stack
{
	StackNode* top;
	struct Stack* prevStack;
	uint16_t StackNodeCnt;
} Stack;
typedef struct StackRoot
{
	Stack* lastStack;
	uint16_t StackCnt;
} StackRoot;
#pragma pack(pop)

void push_node(StackRoot* sr, StackNode* sn, uint8_t root_type);
void labeling_and_destroy_stack(StackRoot *sr, uint8_t *label, size_t minPts,RawDataBlock *frame);
uint8_t* cclabel;// = (uint8_t*)malloc(sizeof(uint8_t)*73728);
uint8_t* aclabel;// = (uint8_t*)malloc(sizeof(uint8_t)*73728);


size_t data_arr_size;
size_t oneChAllEchoPointSize;
size_t oneChOneEchoPointSize;
size_t allChOneEchPointSize;
size_t allPointSize;
uint16_t cc_eps;
uint16_t ac_eps;
uint8_t hz_val;
uint8_t diff_ch_idx_limit;
uint16_t distance_limit;
uint16_t merge_point_diff_distance_limit;
uint8_t remove_method;
float z_offset;

void init_params(size_t data_block_count, float cc_eps_f,float ac_eps_f, uint8_t diff_ch_idx_limit_,uint16_t distance_limit_, uint16_t merge_point_diff_distance_limit_, float z_offset_,uint8_t hz_val_, uint8_t remove_method_);
void exhaust_filter_frame_merge(RawDataBlock* frame1, RawDataBlock* frame2,	size_t distane_limit, float z_limit, ConvertedDataBlock* cf, size_t oneChAllEchoPointSize, uint8_t method_);
void exhaust_filter_channel_clustering(RawDataBlock *frame, size_t minPts, float z_mean);
void exhaust_filter_azimuth_clustering(RawDataBlock *frame, size_t minPts, float z_mean);
void exhaust_filter_remove_noise(RawDataBlock *frame, uint8_t remove_type);

void vnoise_filter_test_ver0(RawDataBlock* frame, float z_mean, uint8_t ac_minPts,uint8_t cc_minPts);

StackNode* list_map[2052 * 32];
size_t list_map_idx;

Stack* stack_map[2052 * 32];
size_t stack_map_idx;

void init_ver2();
void preprocessing2(RawDataBlock *frame, uint8_t ac_minpts2, uint8_t cc_minpts2, float z_mean); // pc to fw

void exhaust_filter_channel_clustering_ver2(RawDataBlock *frame, size_t minPts, float z_mean);
void exhaust_filter_azimuth_clustering_ver2(RawDataBlock *frame, size_t minPts, float z_mean);
void labeling_and_destroy_stack_ver2(StackRoot *sr, uint8_t *label, size_t minPts,RawDataBlock *frame);
void exhaust_filter_remove_noise_ver2(RawDataBlock *frame, uint8_t remove_type);
void push_node_ver2(StackRoot *sr, StackNode *sn, uint8_t root_type);
void push_node_ver2_channel(StackRoot *sr, StackNode *sn);
void push_node_ver2_azimuth(StackRoot *sr, StackNode *sn);
void vnoise_filter_test_ver2(RawDataBlock* frame, float z_mean, uint8_t ac_minPts,uint8_t cc_minPts);
//uint16_t my_array_ch[2052][2052];
//uint16_t my_array_azi[32][32];



void init_ver3();
void exhaust_filter_channel_clustering_ver3(RawDataBlock *frame, size_t minPts, float z_mean);
void exhaust_filter_azimuth_clustering_ver3(RawDataBlock *frame, size_t minPts, float z_mean);

void run_0617(RawDataBlock *frame, uint8_t ac_minpts2, uint8_t cc_minpts2, float z_mean);



#endif /* SRC_API_SW_ALGORITHMS_EXHAUST_FILTER_H_ */
