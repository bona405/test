/*
 * definitions.h
 *
 *  Created on: 2023. 1. 27.
 *      Author: wonju
 */

#ifndef SRC_DEFINITIONS_DEFINITIONS_H_
#define SRC_DEFINITIONS_DEFINITIONS_H_

#define M32			 	 	0
#define G32					1

//#define G32_25HZ
#define G32_10HZ
//#define G32_ES_TEST

//#define _DEBUG
#undef _DEBUG
//#define PERF_DEBUG
//#define FILTER_DEBUG

#define VERTICAL_ANGLE 10

#define MAX_DISTANCE_COUNT 10000

#if defined(G32_10HZ)
#define SCENE_MEMORY_NUMBER		16
//#define FRAME_DATA_COUNT 4620
//#define TOTAL_AZIMUTH_COUNT FRAME_DATA_COUNT/2
//#define BOTTOM_AZIMUTH_COUNT FRAME_DATA_COUNT/6
#define FRAME_RATE_ 25
#define CHANNEL_SIZE 16
#elif defined(G32_25HZ)
#define SCENE_MEMORY_NUMBER		10
#define FRAME_DATA_COUNT 4320
#define TOTAL_AZIMUTH_COUNT 2160
#define BOTTOM_AZIMUTH_COUNT 720
#define FRAME_RATE 25
#define CHANNEL_SIZE 16
#endif

#endif /* SRC_DEFINITIONS_DEFINITIONS_H_ */
