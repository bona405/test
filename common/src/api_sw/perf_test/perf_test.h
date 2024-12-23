/*
 * perf_test.h
 *
 *  Created on: 2022. 3. 16.
 *      Author: ProDesk
 */

#ifndef SRC_PERF_TEST_PERF_TEST_H_
#define SRC_PERF_TEST_PERF_TEST_H_
#define PERF_DEBUG
#include "xtime_l.h"
#include "../../definitions/definitions.h"
#include "../container/hash_table.h"
#include "string.h"
#include "stdlib.h"

typedef enum TestModuleName
{
	kMAIN, kNoiseFilter, kParseData, kTCPBuffering, kTCPSend, kUDPSend, kVNoiseFilter, kVNoiseFilter0, kVNoiseFilter1, kTemperature
}TestModuleName;



void init_elapsed_time_tb();
void test_start(TestModuleName module_name);
void test_end(TestModuleName module_name);
void temperature_to_tb(float temperature);
void temperature_NB_to_tb(int nownb);
void calc_test_result();
void reset_elapsed_time_tb();
void test_tb_to_bytes(char* bytes);
void add_filter_info_to_packet(char* bytes, int vth_offset, int noise_point_cnt, int normal_point_cnt, int all_point_cnt, float noise_percent);
char elapsed_time_bytes[1212];
char elapsed_time_bytes_core2[1212];
void ElapsedTimeTbValue(TestModuleName module_name, int32_t value);
#endif /* SRC_PERF_TEST_PERF_TEST_H_ */
