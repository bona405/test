/*
 * perf_test.c
 *
 *  Created on: 2022. 3. 16.
 *      Author: ProDesk
 */

#include "perf_test.h"
//#ifdef PERF_DEBUG
static volatile XTime start_time[MAX_TB_SIZE];
static volatile XTime end_time[MAX_TB_SIZE];
static volatile struct NodeL* elapsed_time_tb[MAX_TB_SIZE];
static volatile char* key_arr[MAX_TB_SIZE];
char elapsed_time_bytes[1212] = {0, };
char elapsed_time_bytes_core2[1212] = {0, };
//#endif
void init_elapsed_time_tb()
{
#ifdef PERF_DEBUG
	key_arr[0] = "Main";
	key_arr[1] = "NoiseFilter";
	key_arr[2] = "ParseData";
	key_arr[3] = "TCPBuffering";
	key_arr[4] = "TCPSend";
	key_arr[5] = "UDPSend";
	key_arr[6] = "VNoiseFilter";
	key_arr[7] = "prevnoise";
	key_arr[8] = "nextVnoise";

	key_arr[9] = "Temperature";	//241004 Temperature Info

	key_arr[10] = "Temper_NB";	//241004 Temperature Info

	init_hash_table_l((volatile struct NodeL**) elapsed_time_tb, MAX_TB_SIZE);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[0], 0);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[1], 1);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[2], 2);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[3], 3);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[4], 4);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[5], 5);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[6], 6);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[7], 7);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[8], 8);

	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[9], 9);
	add_item_l((volatile struct NodeL**) elapsed_time_tb, (const char*)key_arr[10], 10);
	//	add_item((volatile struct Node**)elapsed_time_tb, "", 0);
#endif
}

void test_start(TestModuleName module_name)
{
#ifdef PERF_DEBUG
	XTime_GetTime(&start_time[module_name]);
#endif
}

void test_end(TestModuleName module_name)
{
#ifdef PERF_DEBUG
	XTime_GetTime(&end_time[module_name]);

	char key[50];

	switch (module_name)
	{
		case kMAIN:
			strcpy(key, key_arr[0]);
			break;
		case kNoiseFilter:
			strcpy(key, key_arr[1]);
			break;
		case kParseData:
			strcpy(key, key_arr[2]);
			break;
		case kTCPBuffering:
			strcpy(key, key_arr[3]);
			break;
		case kTCPSend:
			strcpy(key, key_arr[4]);
			break;
		case kUDPSend:
			strcpy(key, key_arr[5]);
			break;
		case kVNoiseFilter:
			strcpy(key, key_arr[6]);
			break;
		case kVNoiseFilter0:
			strcpy(key, key_arr[7]);
			break;
		case kVNoiseFilter1:
			strcpy(key, key_arr[8]);
			break;
		default:
			break;
	}

	int cmd_hash_value = hash_func(key);
//	dbg_printf("start time : %d\r\n", start_time[module_name] / (COUNTS_PER_SECOND / 1000000));
//	dbg_printf("end time : %d\r\n", end_time[module_name] / (COUNTS_PER_SECOND / 1000000));
//	elapsed_time_tb[cmd_hash_value] += (end_time[module_name] - start_time[module_name]) / (COUNTS_PER_SECOND / 1000000);
//	dbg_printf("hash : %d\r\n", cmd_hash_value);
//	elapsed_time_tb[cmd_hash_value] = 33;
	elapsed_time_tb[cmd_hash_value]->value += (end_time[module_name] - start_time[module_name]) / (COUNTS_PER_SECOND / 1000000);
//	dbg_printf("val : %llu\r\n", elapsed_time_tb[cmd_hash_value]->value);
#endif
}

void temperature_to_tb(float temperature)
{
	char key[50];

	strcpy(key, key_arr[9]);

	int cmd_hash_value = hash_func(key);

	elapsed_time_tb[cmd_hash_value]->value = temperature * 100000;
}

void temperature_NB_to_tb(int nownb)
{
	char key[50];

	strcpy(key, key_arr[10]);

	int cmd_hash_value = hash_func(key);

	elapsed_time_tb[cmd_hash_value]->value = nownb;
}

void calc_test_result()
{

}

void ElapsedTimeTbValue(TestModuleName module_name, int32_t value)
{
	char key[50];

	switch(module_name)
	{
	case kMAIN :
		strcpy(key, key_arr[0]);
		break;
	case kNoiseFilter :
		strcpy(key, key_arr[1]);
		break;
	case kParseData :
		strcpy(key, key_arr[2]);
		break;
	case kTCPBuffering :
		strcpy(key, key_arr[3]);
		break;
	case kTCPSend :
		strcpy(key, key_arr[4]);
		break;
	case kUDPSend :
		strcpy(key, key_arr[5]);
		break;
	case kVNoiseFilter :
		strcpy(key, key_arr[6]);
		break;
	case kVNoiseFilter0 :
		strcpy(key, key_arr[7]);
		break;
	case kVNoiseFilter1 :
		strcpy(key, key_arr[8]);
		break;
	}

	int cmd_hash_value = hash_func(key);
	elapsed_time_tb[cmd_hash_value]->value = value;
}

void reset_elapsed_time_tb()
{
#ifdef PERF_DEBUG
	for (int i = 0; i < MAX_TEST_TB_SIZE; i++)
	{
		if (key_arr[i] != NULL)
		{
			int cmd_hash_value = hash_func((const char*)key_arr[i]);
			elapsed_time_tb[cmd_hash_value]->value = 0;
		}
	}
#endif
}

volatile char buf[50];

void RemoveEnd(char *buf)
{
    int i = 0;
    while (buf[i])
    {
        i++;
    }
    buf[i - 1] = '\0';
}

void test_tb_to_bytes(char* bytes)
{
#ifdef PERF_DEBUG
	memset(bytes, 0x00, 1212);
	strcat(bytes, "{");
	for (int i = 0; i < MAX_TEST_TB_SIZE; i++)
	{
		if (key_arr[i] != NULL)
		{
			int cmd_hash_value = hash_func((const char*)key_arr[i]);
//			dbg_printf("hash : %d\r\n", cmd_hash_value);
			strcat(bytes, "\"");
			strcat(bytes, key_arr[i]);
			strcat(bytes, "\"");

			strcat(bytes, ":");
			memset(buf, 0x00, 50);
//			dbg_printf("val : %llu\r\n", elapsed_time_tb[cmd_hash_value]->value);
//			dbg_printf("buf : %s\r\n", buf);

//			ltoa(elapsed_time_tb[cmd_hash_value]->value, buf, 10);
//			memcpy(buf, &elapsed_time_tb[cmd_hash_value]->value, 8);
			sprintf(buf, "%d", elapsed_time_tb[cmd_hash_value]->value);
//			dbg_printf("buf2 : %s\r\n", buf);
//			strcat(bytes, "\"");
			if(strlen(buf)==0)
				strcat(bytes, "0");
			else
				strcat(bytes, buf);
//			strcat(bytes, "\"");

			strcat(bytes, ",");
		}
	}
	RemoveEnd(bytes);
	strcat(bytes, "}");
#endif
}

void add_filter_info_to_packet(char* bytes, int vth_offset, int noise_point_cnt, int normal_point_cnt, int all_point_cnt, float noise_percent)
{
	RemoveEnd(bytes); // remove "}"

	//add vth_offset
	strcat(bytes, ",");

	strcat(bytes, "\"");
	strcat(bytes, "Vth");
	strcat(bytes, "\"");

	strcat(bytes, ":");

	char value_buffer[8] = {0,};
//	memcpy(value_buffer, &vth_offset, 8);
	sprintf(value_buffer, "%d", vth_offset);
//	strcat(bytes, "\"");
	if(strlen(buf)==0)
		strcat(bytes, "0");
	else
		strcat(bytes, value_buffer);
//	strcat(bytes, "\"");

	//add noise_point_cnt
	strcat(bytes, ",");

	strcat(bytes, "\"");
	strcat(bytes, "NoiseCnt");
	strcat(bytes, "\"");

	strcat(bytes, ":");

	memset(value_buffer, 0x00, 8);
//	memcpy(value_buffer, &noise_point_cnt, 8);
	sprintf(value_buffer, "%d", noise_point_cnt);
//	strcat(bytes, "\"");
	if(strlen(buf)==0)
		strcat(bytes, "0");
	else
		strcat(bytes, value_buffer);
//	strcat(bytes, "\"");

	//add normal_point_cnt
	strcat(bytes, ",");

	strcat(bytes, "\"");
	strcat(bytes, "NormalCnt");
	strcat(bytes, "\"");

	strcat(bytes, ":");

	memset(value_buffer, 0x00, 8);
//	memcpy(value_buffer, &normal_point_cnt, 8);
	sprintf(value_buffer, "%d", normal_point_cnt);
//	strcat(bytes, "\"");
	if(strlen(buf)==0)
		strcat(bytes, "0");
	else
		strcat(bytes, value_buffer);
//	strcat(bytes, "\"");

	//add all_point_cnt
	strcat(bytes, ",");

	strcat(bytes, "\"");
	strcat(bytes, "AllCnt");
	strcat(bytes, "\"");

	strcat(bytes, ":");

	memset(value_buffer, 0x00, 8);
//	memcpy(value_buffer, &all_point_cnt, 8);
	sprintf(value_buffer, "%d", all_point_cnt);
//	strcat(bytes, "\"");
	if(strlen(buf)==0)
		strcat(bytes, "0");
	else
		strcat(bytes, value_buffer);
//	strcat(bytes, "\"");

	//add noise_percent
	strcat(bytes, ",");

	strcat(bytes, "\"");
	strcat(bytes, "NoisePercent");
	strcat(bytes, "\"");

	strcat(bytes, ":");

	memset(value_buffer, 0x00, 8);
//	memcpy(value_buffer, &noise_percent, 8);
	sprintf(value_buffer, "%f", noise_percent);
//	strcat(bytes, "\"");
	if(strlen(buf)==0)
		strcat(bytes, "0");
	else
		strcat(bytes, value_buffer);
//	strcat(bytes, "\"");

	//add "}"
	strcat(bytes, "}");
}
