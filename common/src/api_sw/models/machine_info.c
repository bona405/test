/*
 * machine_info.c
 *
 *  Created on: 2021. 11. 18.
 *      Author: ProDesk
 */

#include "machine_info.h"

void InitLidarState(struct LidarState* lidar_state)
{
//	lidar_state->is_laser_on = 1;
//	lidar_state->num_of_echo = 3;
//	lidar_state->motor_rpm = 1500;
//	lidar_state->fov_start_angle = -67.5;
//	lidar_state->fov_end_angle = 67.5;
//	lidar_state->phase_lock_offset = 0;
//	lidar_state->is_dhcp_on = 0;
//	strcpy(lidar_state->protocol, "tcp");
//	strcpy(lidar_state->ip, "192.168.1.101");
//	strcpy(lidar_state->subnet, "255.255.255.1");
//	strcpy(lidar_state->gateway, "192.168.1.1");
//	strcpy(lidar_state->mac, "AKDI87AKS6SDJ");

	memset(lidar_state, 0x00, sizeof(struct LidarState));
}

void InitLidarState_Ch(struct LidarState_Ch* lidar_state)
{
	memset(lidar_state, 0x00, sizeof(struct LidarState_Ch));
}

uint8_t HasFF(struct LidarState* lidar_state)
{
	char *addr = (char*) lidar_state;
	for (int i = 0; i < sizeof(struct LidarState); i++)
	{
		if (*addr == 0xFF)
			return 1;
		addr++;
	}
	return 0;
}

void PrintLidarState(struct LidarState* lidar_state)
{
	xil_printf("is_laser_on : %d\r\n", lidar_state->laser_setting_.is_laser_on);
	xil_printf("num_of_echo : %d\r\n", lidar_state->calibration_setting_.num_of_echo);
	xil_printf("motor_rpm : %f\r\n", lidar_state->motor_setting_.motor_rpm);
	xil_printf("fov_start_angle : %f\r\n", lidar_state->motor_setting_.fov_start_angle);
	xil_printf("fov_end_angle : %f\r\n", lidar_state->motor_setting_.fov_end_angle);
	xil_printf("phase_lock_offset : %f\r\n", lidar_state->motor_setting_.phase_lock_offset);
	xil_printf("is_dhcp_on : %d\r\n", lidar_state->network_setting_.is_dhcp_on);
	xil_printf("protocol : %s\r\n", lidar_state->network_setting_.protocol);
	xil_printf("ip : %s\r\n", lidar_state->network_setting_.ip);
	xil_printf("subnet : %s\r\n", lidar_state->network_setting_.subnet);
	xil_printf("gateway : %s\r\n", lidar_state->network_setting_.gateway);
	xil_printf("mac : %s\r\n", lidar_state->network_setting_.mac);
	xil_printf("--------------------------------------------\r\n");
}
