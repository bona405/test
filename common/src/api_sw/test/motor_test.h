/*
 * motor_test.h
 *
 *  Created on: 2024. 8. 16.
 *      Author: wonju
 */

#ifndef SRC_API_SW_TEST_MOTOR_TEST_H_
#define SRC_API_SW_TEST_MOTOR_TEST_H_

#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
typedef struct MotorTestData
{
	union
	{
		struct
		{
			int32_t fault_alarm;
			int32_t rpm;
			int32_t rpm_per_one_rotation;
			int32_t offset_90deg;
			int32_t offset_180deg;
			int32_t offset_270deg;
		};
		int32_t data[6];
	};
} MotorTestData;

int32_t GetMotorTestInfoFromPL(MotorTestData* motor_test_data);
int32_t ConvertMotorTestInfoToCsvFormat(MotorTestData* motor_test_data, char* csv_bytes, int32_t csv_bytes_size);

#endif /* SRC_API_SW_TEST_MOTOR_TEST_H_ */
