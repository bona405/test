/*
 * dust_gas_filter.h
 *
 *  Created on: 2024. 3. 11.
 *      Author: AutoL
 */

#ifndef SRC_API_SW_ALGORITHMS_DUST_GAS_FILTER_H_
#define SRC_API_SW_ALGORITHMS_DUST_GAS_FILTER_H_

#include <stdint.h>
#include "fov_noise.h"
#include "ground_detection.h"
#include "../../definitions/definitions.h"
#include "../models/fov_data_set.h"
#include "../container/common.h"
#define GET_MAX(val1,val2) (val1>val2)? val1: val2
#define GET_MIN(val1,val2) (val1<val2)? val1: val2

#define NODE_SIZE 512
#define AC_NODE_SIZE 32


int isBigger(uint16_t va1, uint16_t va2, uint16_t compare_val);


#pragma pack(push, 1)
typedef struct NODEP{
	DataPoint* point;
	int32_t azimuth;
	int stop;
	struct NODEP* next;
	size_t data_block_idx;
	uint8_t ch;
}NODEP;

typedef struct HeaderNode {
	NODEP* point;
} HeaderNode;

typedef struct TailNode {
	NODEP* point;
	int cnt;
	unsigned int max_distance;
} TailNode;

HeaderNode headerNodes[NODE_SIZE];
TailNode tailNodes[NODE_SIZE];
#pragma pack(pop)
extern const double elevation_[32];

uint32_t search_point_distance_limit_uint;
uint16_t azimuth_limit;
uint16_t eps;
uint16_t minPts;
uint8_t howToShow;

uint16_t search_idx;
uint16_t prev_search_idx;

uint16_t max_node_idx;

NODEP* createNODEP(DataPoint*p, int32_t azimuth, int stop,size_t data_block_idx, uint8_t ch);

void initClustering(uint16_t dist);
void insertNode(HeaderNode* header, TailNode* tail, DataPoint* p, unsigned int distance, int32_t azimuth, int stop, int32_t azimuth_limit,size_t data_block_idx, uint8_t ch);
void freeAllNode(HeaderNode* header, TailNode* tail);










#endif /* SRC_API_SW_ALGORITHMS_DUST_GAS_FILTER_H_ */
