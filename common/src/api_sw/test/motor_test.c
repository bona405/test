/*
 * motor_test.c
 *
 *  Created on: 2024. 8. 16.
 *      Author: wonju
 */

#include "motor_test.h"

int32_t GetMotorTestInfoFromPL(MotorTestData* motor_test_data)
{
	// original source
//	motor_test_data->fault_alarm = Xil_In32();

// test source
	motor_test_data->fault_alarm++;
	motor_test_data->rpm += 100;
	motor_test_data->rpm_per_one_rotation += 200;
	motor_test_data->offset_90deg += 300;
	motor_test_data->offset_180deg += 400;
	motor_test_data->offset_270deg += 500;
}

int32_t ConvertMotorTestInfoToCsvFormat(MotorTestData* motor_test_data, char* csv_bytes, int32_t csv_bytes_size)
{
	memset(csv_bytes, 0x00, csv_bytes_size);
	int32_t i = 0;
	for( ; i < sizeof(motor_test_data->data) / sizeof(int32_t) - 1 ; ++i)
	{
		char data[20];
		itoa(motor_test_data->data[i], data, 10);
		strcat(csv_bytes, (const char*)data);
		strcat(csv_bytes, ",");
	}
	char data[20];
	itoa(motor_test_data->data[i], data, 10);
	strcat(csv_bytes, (const char*)data);
}
