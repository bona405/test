/*
 * dust_gas_filter.c
 *
 *  Created on: 2024. 3. 11.
 *      Author: AutoL
 */

#include "./dust_gas_filter.h"

const double elevation_[32] = {4.84375, 4.53125, 4.21875, 3.90625, 3.59375, 3.28125, 2.96875, 2.65625, 2.34375, 2.03125, 1.71875, 1.40625, 1.09375,
		0.78125, 0.46875, 0.15625, -0.15625, -0.46875, -0.78125, -1.09375, -1.40625, -1.71875, -2.03125, -2.34375, -2.65625, -2.96875, -3.28125,
		-3.59375, -3.90625, -4.21875, -4.53125, -4.84375};
void initClustering(uint16_t dist)
{
	search_point_distance_limit_uint =dist;
	azimuth_limit = 200;//176;
	eps = 0.1*256;
	minPts = 3;
	howToShow = 0; // 0 is intensity, 1 is distance 0
	search_idx = 0;
	prev_search_idx = 0;
	max_node_idx = 0;
	memset(headerNodes,0x00,sizeof(HeaderNode)*NODE_SIZE);
	memset(tailNodes,0x00,sizeof(TailNode)*NODE_SIZE);
}
int isBigger(uint16_t va1, uint16_t va2, uint16_t compare_val)
{
	uint16_t a = GET_MAX(va1,va2);
	uint16_t b = GET_MIN(va1,va2);
	return ((a-b)>compare_val)?1:0;
}

NODEP* createNODEP(DataPoint*p, int32_t azimuth, int stop,size_t data_block_idx, uint8_t ch)
{
	NODEP * NewNODEP = (NODEP*)malloc(sizeof(NODEP));
	if(NewNODEP != NULL)
	{
		NewNODEP->point = p;
		NewNODEP->azimuth = azimuth;
		NewNODEP->stop = stop;
		NewNODEP->next = NULL;
		NewNODEP->data_block_idx = data_block_idx;
		NewNODEP->ch = ch;
		return NewNODEP;
	}
	return NULL;
}
void insertNode(HeaderNode* header, TailNode* tail, DataPoint* p, unsigned int distance, int32_t azimuth, int stop, int32_t azimuth_limit, size_t data_block_idx, uint8_t ch )
{
	if(tail[NODE_SIZE-1].point != NULL)	return;
	NODEP* newNode = createNODEP(p, azimuth, stop,data_block_idx,ch);

	search_idx = (prev_search_idx - 5 > 0) ? (prev_search_idx - 5) : (0);
//	search_idx = 0;

	NODEP* curNode = tail[search_idx].point;
	while (search_idx < NODE_SIZE-1 && max_node_idx < NODE_SIZE-1)
	{
		if (curNode == NULL) {
			header[search_idx].point = newNode;
			tail[search_idx].point = newNode;
			tail[search_idx].cnt++;
//			tail[search_idx].min_distance = newNode->point->distance;
			tail[search_idx].max_distance = newNode->point->distance;
//			tail[search_idx].mean_distance = newNode->point->distance;
//			p->pulse_width = (search_idx+30)%255;
			break;
		}
		else if ((isBigger(curNode->point->distance, newNode->point->distance, distance)==0) && (abs(curNode->azimuth - newNode->azimuth) <= azimuth_limit))
		{
			curNode->next = newNode;
			tail[search_idx].point = newNode;
			tail[search_idx].cnt++;
			tail[search_idx].max_distance = (tail[search_idx].max_distance > newNode->point->distance) ? (tail[search_idx].max_distance) : (newNode->point->distance);
//			tail[search_idx].min_distance = (tail[search_idx].min_distance < newNode->point->distance) ? (tail[search_idx].min_distance) : (newNode->point->distance);
//			tail[search_idx].mean_distance = (tail[search_idx].mean_distance + newNode->point->distance) / 2;
//			p->pulse_width = (search_idx+30)%255;
			break;
		}

		search_idx++;
		curNode = tail[search_idx].point;
		max_node_idx = (search_idx > max_node_idx) ? search_idx: max_node_idx;

	}
	prev_search_idx = search_idx;
}
void freeAllNode(HeaderNode* header, TailNode* tail)
{
	for (int i = 0; i < NODE_SIZE; i++)
	{
		NODEP* curNode = header[i].point;
		while (curNode != NULL)
		{
			NODEP* tmpRmNode = curNode;
			curNode = curNode->next;
			free(tmpRmNode);
		}
	}
//	free(header);
//	free(tail);
}

