/*
 * process_web_request.c
 *
 *  Created on: 2022. 8. 1.
 *      Author: Wonju
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"

#include "process_web_request.h"
#include "web_server.h"
#include "../../../common/src/api_sw/logger/logger.h"

char *notfound_header =
		"<html> \
	<head> \
		<title>404</title> \
  		<style type=\"text/css\"> \
		div#request {background: #eeeeee} \
		</style> \
	</head> \
	<body> \
	<h1>404 Page Not Found</h1> \
	<div id=\"request\">";
char *notfound_footer = "</div> \
	</body> \
	</html>";

int percent_val = 0;
int reset_ret_val = 0;
uint8_t is_golden_img = 0;
int cal_flag = 0;
int cal2_flag = 0;
int cal_flag_long = 0;
int fixed_cal_flag = 0;
int routing_add_flag = 0;
int routing_add_flag2 = 0;
int adddelay = 0;
int cal_distance = 0;
int routing_resetflag = 0;
extern int vnfilter;
extern int is_heat_control_ON;
int inten_filter = 0;
int stop_option = 0;	//0 : 123stop, 1 : 12stop, 2 : 1stop

int32_t GetMfsProgress(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char buf[PACKET_SIZE];
	web_param progress[2] = { {"restore", "0"}, {"percentage", "0"}};
	if(percent_val > 70)
	{
		itoa(reset_ret_val, progress[0].param_value, 10);
		itoa(percent_val, progress[1].param_value, 10);
		TransFormJSON(progress, buf, 2);

		if(tcp_write(pcb, buf, 150, 1) != ERR_OK)
		{
			xil_printf("error writing http POST response to socket\n\r");
			xil_printf("http header = %s\r\n", buf);
			return -1;
		}
	}
	else
	{
		if(tcp_write(pcb, buf, 150, 1) != ERR_OK)
		{
			xil_printf("error writing http POST response to socket\n\r");
			xil_printf("http header = %s\r\n", buf);
			return -1;
		}
	}
	return 0;
}

int32_t GetCurrentVth(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char buf[PACKET_SIZE];
	web_param current_vth_param[1] = { {"CurrentVth", "0"}};
//	if(current_vth_offset >= 0)
//	{
	itoa(current_vth_offset, current_vth_param[0].param_value, 10);

	TransFormJSON(current_vth_param, buf, 1);

	if(tcp_write(pcb, buf, 150, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
//	}
	return 0;
}

int32_t UpdateBootImage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	file_download_flag = 1;
	is_golden_img = 0;
	e_file_recv_status = EMPTY;
	content_length = 0;
	recved_size = 0;
	percent_val = 0;
	recv_file_size = 0;
	reset_ret_val = 0;
	memset(Total_buf, 0x00, sizeof(Total_buf));
	do_http_file_ok(pcb, req, rlen, &e_file_recv_status);
	return 1;
}

int32_t ResetHW(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	Xil_Out32(SLCR_UNLOCK_REG, 0xDF0DDF0D);
	Xil_Out32(SLCR_UNLOCK_REG, 0x767B767B);
	Xil_Out32(PS_RST_CTRL_REG, PS_RST_MASK);
	return 1;
}

int32_t GetSystemInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	memset(buf, 0x00, PACKET_SIZE);
	web_param sysinfo_param[20] = { {"serial", "0"}, {"mdate", "0"}, {"firmwarever", "0"}, {"fpgaver", "0"}, {"udate", "0"}, {"mac", "0"},
			{"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"golden_ver", "0"}, {"golden_u_date", "0"}, {"noise_percent", "0"}, {"noiseon", "0"}, {
					"voltageon", "0"}, {"vth_sensitivity", "0"}, {"valid_point_cnt", "0"}, {"noise_cnt_limit", "0"}, {"propagation_delay", "0"}, {
					"upper_offset", "0"}, {"lower_offset", "0"}, };

	int32_t Status = ReadLidarInfo((u8*)&lidar_state_, LIDAR_STATE_SIZE);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}
	strncpy(sysinfo_param[0].param_value, lidar_state_.lidar_info_.serial, sizeof(lidar_state_.lidar_info_.serial));
	strncpy(sysinfo_param[1].param_value, lidar_state_.lidar_info_.manufacturing_date, sizeof(lidar_state_.lidar_info_.manufacturing_date));
//	strncpy(sysinfo_param[2].param_value, lidar_state_.lidar_info_.firmware_ver, sizeof(lidar_state_.lidar_info_.firmware_ver));
//	strncpy(sysinfo_param[3].param_value, lidar_state_.lidar_info_.fpga_ver, sizeof(lidar_state_.lidar_info_.fpga_ver));
#ifdef G32_B1_PPS
	strncpy(sysinfo_param[2].param_value, "F0301", 6);
	strncpy(sysinfo_param[3].param_value, "L0300", 6);
#else
	strncpy(sysinfo_param[2].param_value, "F0208", 6);
	strncpy(sysinfo_param[3].param_value, "L0204", 6);
#endif

	strncpy(sysinfo_param[4].param_value, lidar_state_.lidar_info_.firmware_u_date, sizeof(lidar_state_.lidar_info_.firmware_u_date));
	strncpy(sysinfo_param[5].param_value, lidar_state_.network_setting_.mac, sizeof(lidar_state_.network_setting_.mac));
	strncpy(sysinfo_param[6].param_value, lidar_state_.network_setting_.ip, sizeof(lidar_state_.network_setting_.ip));
	strncpy(sysinfo_param[7].param_value, lidar_state_.network_setting_.subnet, sizeof(lidar_state_.network_setting_.subnet));
	strncpy(sysinfo_param[8].param_value, lidar_state_.network_setting_.gateway, sizeof(lidar_state_.network_setting_.gateway));
	strncpy(sysinfo_param[9].param_value, lidar_state_.lidar_info_.golden_ver, sizeof(lidar_state_.lidar_info_.golden_ver));
	strncpy(sysinfo_param[10].param_value, lidar_state_.lidar_info_.golden_u_date, sizeof(lidar_state_.lidar_info_.golden_u_date));
	strncpy(sysinfo_param[11].param_value, lidar_state_.filter_setting_.noise_percent, sizeof(lidar_state_.filter_setting_.noise_percent));
	TransFormJSON(sysinfo_param, buf, 12);
	if(tcp_write(pcb, buf, 512, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	return 1;
}

int32_t GetHwInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
//	web_param hwinfo_param[15] = { {"laseron", "0"}, {"echo", "0"}, {"rpm", "0"}, {"fovstart", "0"}, {"fovend", "0"}, {"phaselock", "0"}, {"dhcpon",
//			"0"}, {"protocol", "0"}, {"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"noiseon", "0"}, {"voltageon", "0"}, {"interferenceon", "0"}, {
//			"port", "0"}};
//	web_param hwinfo_param[16] = { {"laseron", "0"}, {"echo", "0"}, {"rpm", "0"}, {"fovstart", "0"}, {"fovend", "0"}, {"phaselock", "0"}, {"dhcpon",
//			"0"}, {"protocol", "0"}, {"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"noiseon", "0"}, {"voltageon", "0"}, {"interferenceon", "0"}, {
//			"port", "0"}, {"hzoption", "0"}};

//	web_param hwinfo_param[17] = { {"laseron", "0"}, {"echo", "0"}, {"rpm", "0"}, {"fovstart", "0"}, {"fovend", "0"}, {"phaselock", "0"}, {"dhcpon",
//				"0"}, {"protocol", "0"}, {"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"noiseon", "0"}, {"voltageon", "0"}, {"interferenceon", "0"}, {
//				"port", "0"}, {"hzoption", "0"},{"target_server_ip","0"}};

//	web_param hwinfo_param[18] = { {"laseron", "0"}, {"echo", "0"}, {"rpm", "0"}, {"fovstart", "0"}, {"fovend", "0"}, {"phaselock", "0"}, {"dhcpon",
//				"0"}, {"protocol", "0"}, {"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"noiseon", "0"}, {"voltageon", "0"}, {"interferenceon", "0"}, {
//				"port", "0"}, {"hzoption", "0"}, {"stopoption", "0"}, {"target_server_ip","0"}};

	web_param hwinfo_param[19] = { {"laseron", "0"}, {"echo", "0"}, {"rpm", "0"}, {"fovstart", "0"}, {"fovend", "0"}, {"phaselock", "0"}, {"dhcpon",
				"0"}, {"protocol", "0"}, {"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"noiseon", "0"}, {"voltageon", "0"}, {"interferenceon", "0"}, {
				"port", "0"}, {"hzoption", "0"}, {"stopoption", "0"}, {"target_server_ip","0"},
				{"phase_Deg","0"}};

	int32_t Status = ReadLidarInfo((u8*)&lidar_state_, LIDAR_STATE_SIZE);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	itoa(lidar_state_.laser_setting_.is_laser_on, hwinfo_param[0].param_value, 10);
	itoa(lidar_state_.calibration_setting_.num_of_echo, hwinfo_param[1].param_value, 10);
	sprintf(hwinfo_param[2].param_value, "%.2f", lidar_state_.motor_setting_.motor_rpm);
	sprintf(hwinfo_param[3].param_value, "%.2f", lidar_state_.motor_setting_.fov_start_angle);
	sprintf(hwinfo_param[4].param_value, "%.2f", lidar_state_.motor_setting_.fov_end_angle);
	sprintf(hwinfo_param[5].param_value, "%.2f", lidar_state_.motor_setting_.phase_lock_offset);
	itoa(lidar_state_.network_setting_.is_dhcp_on, hwinfo_param[6].param_value, 10);
	strncpy(hwinfo_param[7].param_value, lidar_state_.network_setting_.protocol, sizeof(lidar_state_.network_setting_.protocol));
	strncpy(hwinfo_param[8].param_value, lidar_state_.network_setting_.ip, sizeof(lidar_state_.network_setting_.ip));

	strncpy(hwinfo_param[9].param_value, lidar_state_.network_setting_.subnet, sizeof(lidar_state_.network_setting_.subnet));
	strncpy(hwinfo_param[10].param_value, lidar_state_.network_setting_.gateway, sizeof(lidar_state_.network_setting_.gateway));
	itoa(lidar_state_.filter_setting_.is_noise_on, hwinfo_param[11].param_value, 10);
	itoa(lidar_state_.filter_setting_.is_voltage_on, hwinfo_param[12].param_value, 10);
	itoa(lidar_state_.filter_setting_.is_interference_on, hwinfo_param[13].param_value, 10);
	strncpy(hwinfo_param[14].param_value, lidar_state_.network_setting_.port, sizeof(lidar_state_.network_setting_.port));
	itoa(lidar_state_.motor_setting_.hz_setting, hwinfo_param[15].param_value, 10);
	itoa(lidar_state_.laser_setting_.stop_option, hwinfo_param[16].param_value, 10);
//	strncpy(hwinfo_param[16].param_value, lidar_state_.network_setting_.target_ip, sizeof(lidar_state_.network_setting_.target_ip)); // hhj test2
	strncpy(hwinfo_param[17].param_value, lidar_state_.network_setting_.target_ip, sizeof(lidar_state_.network_setting_.target_ip)); // hhj test2

	int tempdeg = 0.010986663 * lidar_state_.motor_setting_.pll_point;
	tempdeg = 45 - tempdeg;
	itoa(tempdeg, hwinfo_param[18].param_value, 10);


//	TransFormJSON(hwinfo_param, buf, 15);
//	TransFormJSON(hwinfo_param, buf, 16); // hhj origin
//	TransFormJSON(hwinfo_param, buf, 17); // hhj test2
//	TransFormJSON(hwinfo_param, buf, 18); // hhj test2
	TransFormJSON(hwinfo_param, buf, 19); // 240715 PPS
	if(tcp_write(pcb, buf, 512, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t GetDiagnosticsInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	web_param diagnostics_param1[9] = { {"hv", "0"}, {"td", "0"}, {"temp", "0"}, {"v5", "0"}, {"v25", "0"}, {"v33", "0"}, {"v5raw", "0"},
			{"v15", "0"}, {"vhd", "0"}};
	web_param diagnostics_param2[9] = { {"hv", "0"}, {"td", "0"}, {"temp", "0"}, {"v5", "0"}, {"v25", "0"}, {"v33", "0"}, {"v5raw", "0"},
			{"v15", "0"}, {"vhd", "0"}};
	web_param diagnostics_param3[9] = { {"hv", "0"}, {"td", "0"}, {"temp", "0"}, {"v5", "0"}, {"v25", "0"}, {"v33", "0"}, {"v5raw", "0"},
			{"v15", "0"}, {"vhd", "0"}};
	web_param diagnostics_param4[9] = { {"hv", "0"}, {"td", "0"}, {"temp", "0"}, {"v5", "0"}, {"v25", "0"}, {"v33", "0"}, {"v5raw", "0"},
			{"v15", "0"}, {"vhd", "0"}};
	strcpy(diagnostics_param1[0].param_value, "1.1");
	strcpy(diagnostics_param1[1].param_value, "1.2");
	strcpy(diagnostics_param1[2].param_value, "1.3");
	strcpy(diagnostics_param1[3].param_value, "1.4");
	strcpy(diagnostics_param1[4].param_value, "1.5");
	strcpy(diagnostics_param1[5].param_value, "1.6");
	strcpy(diagnostics_param1[6].param_value, "1.7");
	strcpy(diagnostics_param1[7].param_value, "1.8");
	strcpy(diagnostics_param1[8].param_value, "1.9");

	strcpy(diagnostics_param2[0].param_value, "2.1");
	strcpy(diagnostics_param2[1].param_value, "2.2");
	strcpy(diagnostics_param2[2].param_value, "2.3");
	strcpy(diagnostics_param2[3].param_value, "2.4");
	strcpy(diagnostics_param2[4].param_value, "2.5");
	strcpy(diagnostics_param2[5].param_value, "2.6");
	strcpy(diagnostics_param2[6].param_value, "2.7");
	strcpy(diagnostics_param2[7].param_value, "2.8");
	strcpy(diagnostics_param2[8].param_value, "2.9");

	strcpy(diagnostics_param3[0].param_value, "3.1");
	strcpy(diagnostics_param3[1].param_value, "3.2");
	strcpy(diagnostics_param3[2].param_value, "3.3");
	strcpy(diagnostics_param3[3].param_value, "3.4");
	strcpy(diagnostics_param3[4].param_value, "3.5");
	strcpy(diagnostics_param3[5].param_value, "3.6");
	strcpy(diagnostics_param3[6].param_value, "3.7");
	strcpy(diagnostics_param3[7].param_value, "3.8");
	strcpy(diagnostics_param3[8].param_value, "3.9");

	strcpy(diagnostics_param4[0].param_value, "4.1");
	strcpy(diagnostics_param4[1].param_value, "4.2");
	strcpy(diagnostics_param4[2].param_value, "4.3");
	strcpy(diagnostics_param4[3].param_value, "4.4");
	strcpy(diagnostics_param4[4].param_value, "4.5");
	strcpy(diagnostics_param4[5].param_value, "4.6");
	strcpy(diagnostics_param4[6].param_value, "4.7");
	strcpy(diagnostics_param4[7].param_value, "4.8");
	strcpy(diagnostics_param4[8].param_value, "4.9");

	TransFormJSONArr(diagnostics_param1, diagnostics_param2, diagnostics_param3, diagnostics_param4, buf, 9, 9, 9, 9);

	if(tcp_write(pcb, buf, 1024, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetCommandParameters(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	int instruction_number, parameter1, parameter2;


	char val[100] = {0, };
	char key[100] = "off1";
	GetJsonValueByKey(val, req, key);
	instruction_number = atoi(val);

	memset(val, 0x00, sizeof(val));
	memset(key, 0x00, sizeof(key));
	strcat(key, "off2");
	GetJsonValueByKey(val, req, key);
	parameter1 = atoi(val);

	memset(val, 0x00, sizeof(val));
	memset(key, 0x00, sizeof(key));
	strcat(key, "off3");
	GetJsonValueByKey(val, req, key);
	parameter2 = atoi(val);

	if(instruction_number < NumberOfeLidarCalibrationCmd)
		lidar_calibration_fp[instruction_number](parameter1, parameter2);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetLaserOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "laseronoff";
	GetJsonValueByKey(value, req, key);

	uint8_t result = 0;

	if(strcmp(value, "true") == 0)
		result = 1;

	lidar_state_.laser_setting_.is_laser_on = result;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetEchoNum(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "echo";
	GetJsonValueByKey(value, req, key);

	lidar_state_.calibration_setting_.num_of_echo = atoi(value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetMotorRpm(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "motorrpm";
	GetJsonValueByKey(value, req, key);

	lidar_state_.motor_setting_.motor_rpm = atoi(value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetFovStartAngle(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "fovstartangle";
	GetJsonValueByKey(value, req, key);

	lidar_state_.motor_setting_.fov_start_angle = atoi(value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;

	}
	return 1;
}

int32_t SetFovEndAngle(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "fovendangle";
	GetJsonValueByKey(value, req, key);

	lidar_state_.motor_setting_.fov_end_angle = atoi(value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetPhaseLockOffset(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "phaselockoffset";
	GetJsonValueByKey(value, req, key);

	lidar_state_.motor_setting_.phase_lock_offset = atoi(value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetDhcpOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "dhcponoff";
	GetJsonValueByKey(value, req, key);

	uint8_t result = 0;

	if(strcmp(value, "true") == 0)
		result = 1;

	lidar_state_.network_setting_.is_dhcp_on = result;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetProtocol(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "protocol";
	GetJsonValueByKey(value, req, key);

	strncpy(lidar_state_.network_setting_.protocol, value, sizeof(lidar_state_.network_setting_.protocol));

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetIpAddress(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "ip";
	GetJsonValueByKey(value, req, key);

	strncpy(lidar_state_.network_setting_.ip, value, sizeof(lidar_state_.network_setting_.ip));

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetSubnetMask(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "subnet";
	GetJsonValueByKey(value, req, key);

	strncpy(lidar_state_.network_setting_.subnet, value, sizeof(lidar_state_.network_setting_.subnet));

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetGateway(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "gateway";
	GetJsonValueByKey(value, req, key);

	strncpy(lidar_state_.network_setting_.gateway, value, sizeof(lidar_state_.network_setting_.gateway));

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetNetworkConfig(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "ip";
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.network_setting_.ip, 0x00, sizeof(lidar_state_.network_setting_.ip));
	strncpy(lidar_state_.network_setting_.ip, value, sizeof(lidar_state_.network_setting_.ip));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "subnet");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.network_setting_.subnet, 0x00, sizeof(lidar_state_.network_setting_.subnet));
	strncpy(lidar_state_.network_setting_.subnet, value, sizeof(lidar_state_.network_setting_.subnet));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "gateway");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.network_setting_.gateway, 0x00, sizeof(lidar_state_.network_setting_.gateway));
	strncpy(lidar_state_.network_setting_.gateway, value, sizeof(lidar_state_.network_setting_.gateway));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "port");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.network_setting_.port, 0x00, sizeof(lidar_state_.network_setting_.port));
	strncpy(lidar_state_.network_setting_.port, value, sizeof(lidar_state_.network_setting_.port));

	//hhj test1
//	memset(lidar_state_.network_setting_.target_ip,0x00,sizeof(lidar_state_.network_setting_.target_ip));
//	strncpy(lidar_state_.network_setting_.target_ip,convert_udp_server_ip,sizeof(lidar_state_.network_setting_.target_ip));

	//hhj test2
	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "target_server_ip");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.network_setting_.target_ip, 0x00, sizeof(lidar_state_.network_setting_.target_ip));

//	char compareip = "192.168.1.255";
//	volatile int rescmp =strncmp(compareip,value,10);
//	if( rescmp != 0){
//		strncpy(lidar_state_.network_setting_.target_ip, compareip, sizeof(compareip));
//	}
//	else{
		strncpy(lidar_state_.network_setting_.target_ip, value, sizeof(lidar_state_.network_setting_.target_ip));
//	}

	//Do Test
	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "SAVE_CMD");
	GetJsonValueByKey(value, req, key);
	char do_log_web[32];
	strncpy(do_log_web, value, 32);
//	memset(lidar_state_.network_setting_.target_ip, 0x00, sizeof(lidar_state_.network_setting_.target_ip));


//	char do_log[32] = "[HW_SAVE]20240115_08:00";

//	LinearQspiFlashWriteForLidarChangeInfo(do_log);
	LinearQspiFlashWriteForLidarChangeInfo(do_log_web);



	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*)&lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch2();
	Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*)&lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	Status = ReadLidarInfo((u8*)&lidar_state_, LIDAR_STATE_SIZE);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t GetPassword(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	web_param pwd_response[1] = { {"pwdcheck", "0"}};
	char value[100] = {0, };
	char key[100] = "pwd";
	GetJsonValueByKey(value, req, key);

	if(strcmp(value, password) == 0)
		strcpy(pwd_response[0].param_value, "1");
	else
		strcpy(pwd_response[0].param_value, "0");

	TransFormJSON(pwd_response, buf, 1);

	if(tcp_write(pcb, buf, 256, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetAdminInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "serial";
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.lidar_info_.serial, 0x00, sizeof(lidar_state_.lidar_info_.serial));
	strncpy(lidar_state_.lidar_info_.serial, value, sizeof(lidar_state_.lidar_info_.serial));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "manufacturing_date");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.lidar_info_.manufacturing_date, 0x00, sizeof(lidar_state_.lidar_info_.manufacturing_date));
	strncpy(lidar_state_.lidar_info_.manufacturing_date, value, sizeof(lidar_state_.lidar_info_.manufacturing_date));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "firmware_ver");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.lidar_info_.firmware_ver, 0x00, sizeof(lidar_state_.lidar_info_.firmware_ver));
	strncpy(lidar_state_.lidar_info_.firmware_ver, value, sizeof(lidar_state_.lidar_info_.firmware_ver));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "fpga_ver");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.lidar_info_.fpga_ver, 0x00, sizeof(lidar_state_.lidar_info_.fpga_ver));
	strncpy(lidar_state_.lidar_info_.fpga_ver, value, sizeof(lidar_state_.lidar_info_.fpga_ver));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "firmware_u_date");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.lidar_info_.firmware_u_date, 0x00, sizeof(lidar_state_.lidar_info_.firmware_u_date));
	strncpy(lidar_state_.lidar_info_.firmware_u_date, value, sizeof(lidar_state_.lidar_info_.firmware_u_date));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "mac");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.network_setting_.mac, 0x00, sizeof(lidar_state_.network_setting_.mac));
	strncpy(lidar_state_.network_setting_.mac, value, sizeof(lidar_state_.network_setting_.mac));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "golden_ver");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.lidar_info_.golden_ver, 0x00, sizeof(lidar_state_.lidar_info_.golden_ver));
	strncpy(lidar_state_.lidar_info_.golden_ver, value, sizeof(lidar_state_.lidar_info_.golden_ver));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "golden_u_date");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.lidar_info_.golden_u_date, 0x00, sizeof(lidar_state_.lidar_info_.golden_u_date));
	strncpy(lidar_state_.lidar_info_.golden_u_date, value, sizeof(lidar_state_.lidar_info_.golden_u_date));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "noise_percent");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.filter_setting_.noise_percent, 0x00, sizeof(lidar_state_.filter_setting_.noise_percent));
	strncpy(lidar_state_.filter_setting_.noise_percent, value, sizeof(lidar_state_.filter_setting_.noise_percent));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "vth_sensitivity");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.filter_setting_.vth_sensitivity, 0x00, sizeof(lidar_state_.filter_setting_.vth_sensitivity));
	strncpy(lidar_state_.filter_setting_.vth_sensitivity, value, sizeof(lidar_state_.filter_setting_.vth_sensitivity));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "valid_point_cnt");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.filter_setting_.valid_point_cnt, 0x00, sizeof(lidar_state_.filter_setting_.valid_point_cnt));
	strncpy(lidar_state_.filter_setting_.valid_point_cnt, value, sizeof(lidar_state_.filter_setting_.valid_point_cnt));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "noise_cnt_limit");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.filter_setting_.noise_cnt_limit, 0x00, sizeof(lidar_state_.filter_setting_.noise_cnt_limit));
	strncpy(lidar_state_.filter_setting_.noise_cnt_limit, value, sizeof(lidar_state_.filter_setting_.noise_cnt_limit));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "propagation_delay");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.calibration_setting_.propagation_delay, 0x00, sizeof(lidar_state_.calibration_setting_.propagation_delay));
	strncpy(lidar_state_.calibration_setting_.propagation_delay, value, sizeof(lidar_state_.calibration_setting_.propagation_delay));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "upper_offset");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.calibration_setting_.upper_offset, 0x00, sizeof(lidar_state_.calibration_setting_.upper_offset));
	strncpy(lidar_state_.calibration_setting_.upper_offset, value, sizeof(lidar_state_.calibration_setting_.upper_offset));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "lower_offset");
	GetJsonValueByKey(value, req, key);
	memset(lidar_state_.calibration_setting_.lower_offset, 0x00, sizeof(lidar_state_.calibration_setting_.lower_offset));
	strncpy(lidar_state_.calibration_setting_.lower_offset, value, sizeof(lidar_state_.calibration_setting_.lower_offset));

	//Do Test
//	char do_log[32] = "[Ad_SAVE]20240116_08:00";
	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "SAVE_CMD");
	GetJsonValueByKey(value, req, key);
	char do_log_web[32];
	strncpy(do_log_web, value, 32);
//	memset(lidar_state_.network_setting_.target_ip, 0x00, sizeof(lidar_state_.network_setting_.target_ip));


//	char do_log[32] = "[HW_SAVE]20240115_08:00";

//	LinearQspiFlashWriteForLidarChangeInfo(do_log);
	LinearQspiFlashWriteForLidarChangeInfo(do_log_web);

//	LinearQspiFlashWriteForLidarChangeInfo(do_log);


	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*)&lidar_state_Ch, LIDAR_STATE_SIZE_CH);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch2();
	Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*)&lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	Status = ReadLidarInfo((u8*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	// hhj
	EraseDarkAreaInfo();
	Status = LinearQspiFlashWriteForDarkAreaInfo((char*)&dark_area_state_, DARK_AREA_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}
	Status = ReadDarkAreaInfo((u8*)&dark_area_state_, DARK_AREA_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetUPLog(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "serial";

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "SAVE_CMD");
	GetJsonValueByKey(value, req, key);
	char do_log_web[32];
	strncpy(do_log_web, value, 32);


//	LinearQspiFlashWriteForLidarChangeInfo(do_log);
	LinearQspiFlashWriteForLidarChangeInfo(do_log_web);
	usleep(1);


    if (strncmp(value, "%5BUP_DONE%5D", 13) == 0)
    {
    	char tempvalue[30] = {0, };

    	char year[5] = {0,};
    	char month[3] = {0,};
    	char day[3] = {0,};

        strncpy(year, value + 13, 4);
        strncpy(month, value + 17, 2);
        strncpy(day, value + 19, 2);

        sprintf(tempvalue, "%s%%2F%s%%2F%s", month, day, year);

    	memset(lidar_state_.lidar_info_.firmware_u_date, 0x00, sizeof(lidar_state_.lidar_info_.firmware_u_date));
    	strncpy(lidar_state_.lidar_info_.firmware_u_date, tempvalue, sizeof(lidar_state_.lidar_info_.firmware_u_date));

    	int Status = InitLinearQspiFlash();
    	if(Status != XST_SUCCESS)
    	{
    		xil_printf("Flash Init Failed\r\n");
    		return XST_FAILURE;
    	}

    	EraseLidarInfo();

    	Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
    	if(Status != XST_SUCCESS)
    	{
    		xil_printf("Flash Write Failed\r\n");
    		return XST_FAILURE;
    	}


    }
    else if (strncmp(value, "%5BUP_GDDN%5D", 13) == 0)
    {
    	char tempvalue[30] = {0, };

    	char year[5] = {0,};
    	char month[3] = {0,};
    	char day[3] = {0,};

        strncpy(year, value + 13, 4);
        strncpy(month, value + 17, 2);
        strncpy(day, value + 19, 2);

        sprintf(tempvalue, "%s%%2F%s%%2F%s", month, day, year);

    	memset(lidar_state_.lidar_info_.golden_u_date, 0x00, sizeof(lidar_state_.lidar_info_.golden_u_date));
    	strncpy(lidar_state_.lidar_info_.golden_u_date, tempvalue, sizeof(lidar_state_.lidar_info_.golden_u_date));

    	int Status = InitLinearQspiFlash();
    	if(Status != XST_SUCCESS)
    	{
    		xil_printf("Flash Init Failed\r\n");
    		return XST_FAILURE;
    	}

    	EraseLidarInfo();

    	Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
    	if(Status != XST_SUCCESS)
    	{
    		xil_printf("Flash Write Failed\r\n");
    		return XST_FAILURE;
    	}
    }


	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetRoutingRange(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "routing_range1";
	GetJsonValueByKey(value, req, key);
	routing_range_1 = atof(value);
	char value2[100] = {0, };
	char key2[100] = "routing_range2";
	GetJsonValueByKey(value2, req, key2);
	routing_range_2 = atof(value2);

	lidar_state_.a2z_setting.routing_range_1 = routing_range_1;
	lidar_state_.a2z_setting.routing_range_2 = routing_range_2;

	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}
int32_t GetRoutingRange(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	web_param settings[39] = { {"routing_range1", "0"}, {"routing_range2", "0"}};

	sprintf(settings[0].param_value, "%f", lidar_state_.a2z_setting.routing_range_1);
	sprintf(settings[1].param_value, "%f", lidar_state_.a2z_setting.routing_range_2);
	TransFormJSON(settings, buf, 2);
	if(tcp_write(pcb, buf, 1024, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetRoutingdelay2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	if(strstr(req, "json=") == NULL)
	{
		e_post_cmd_recv_status = CMD_PARSED;
		is_routing_delay_1_cmd = 0;

		char *ptr_file_size = strstr(req, "Content-Length: ");
		ptr_file_size += strlen("Content-Length: ");
		ptr_file_size = strtok(ptr_file_size, "\r");
		//				ptr_file_size = strtok(NULL, "\r");
		//				ptr_file_size = strtok(NULL, "\n");
		post_cmd_data_size = atoi(ptr_file_size);
		xil_printf("recv file size : %d\r\n", recv_file_size);

		return -1;
	}

	char value11[30] = {0, };
	char key11[30] = "ch1";
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch1, 0x00, sizeof(lidar_state_Ch2.delay_ch1));
	strncpy(lidar_state_Ch2.delay_ch1, value11, sizeof(lidar_state_Ch2.delay_ch1));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch3");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch2, 0x00, sizeof(lidar_state_Ch2.delay_ch2));
	strncpy(lidar_state_Ch2.delay_ch2, value11, sizeof(lidar_state_Ch2.delay_ch2));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch5");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch3, 0x00, sizeof(lidar_state_Ch2.delay_ch3));
	strncpy(lidar_state_Ch2.delay_ch3, value11, sizeof(lidar_state_Ch2.delay_ch3));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch7");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch4, 0x00, sizeof(lidar_state_Ch2.delay_ch4));
	strncpy(lidar_state_Ch2.delay_ch4, value11, sizeof(lidar_state_Ch2.delay_ch4));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch9");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch5, 0x00, sizeof(lidar_state_Ch2.delay_ch5));
	strncpy(lidar_state_Ch2.delay_ch5, value11, sizeof(lidar_state_Ch2.delay_ch5));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch11");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch6, 0x00, sizeof(lidar_state_Ch2.delay_ch6));
	strncpy(lidar_state_Ch2.delay_ch6, value11, sizeof(lidar_state_Ch2.delay_ch6));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch13");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch7, 0x00, sizeof(lidar_state_Ch2.delay_ch7));
	strncpy(lidar_state_Ch2.delay_ch7, value11, sizeof(lidar_state_Ch2.delay_ch7));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch15");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch8, 0x00, sizeof(lidar_state_Ch2.delay_ch8));
	strncpy(lidar_state_Ch2.delay_ch8, value11, sizeof(lidar_state_Ch2.delay_ch8));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch17");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch9, 0x00, sizeof(lidar_state_Ch2.delay_ch9));
	strncpy(lidar_state_Ch2.delay_ch9, value11, sizeof(lidar_state_Ch2.delay_ch9));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch19");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch10, 0x00, sizeof(lidar_state_Ch2.delay_ch10));
	strncpy(lidar_state_Ch2.delay_ch10, value11, sizeof(lidar_state_Ch2.delay_ch10));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch21");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch11, 0x00, sizeof(lidar_state_Ch2.delay_ch11));
	strncpy(lidar_state_Ch2.delay_ch11, value11, sizeof(lidar_state_Ch2.delay_ch11));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch23");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch12, 0x00, sizeof(lidar_state_Ch2.delay_ch12));
	strncpy(lidar_state_Ch2.delay_ch12, value11, sizeof(lidar_state_Ch2.delay_ch12));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch25");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch13, 0x00, sizeof(lidar_state_Ch2.delay_ch13));
	strncpy(lidar_state_Ch2.delay_ch13, value11, sizeof(lidar_state_Ch2.delay_ch13));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch27");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch14, 0x00, sizeof(lidar_state_Ch2.delay_ch14));
	strncpy(lidar_state_Ch2.delay_ch14, value11, sizeof(lidar_state_Ch2.delay_ch14));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch29");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch15, 0x00, sizeof(lidar_state_Ch2.delay_ch15));
	strncpy(lidar_state_Ch2.delay_ch15, value11, sizeof(lidar_state_Ch2.delay_ch15));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch31");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch16, 0x00, sizeof(lidar_state_Ch2.delay_ch16));
	strncpy(lidar_state_Ch2.delay_ch16, value11, sizeof(lidar_state_Ch2.delay_ch16));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch2");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch17, 0x00, sizeof(lidar_state_Ch2.delay_ch17));
	strncpy(lidar_state_Ch2.delay_ch17, value11, sizeof(lidar_state_Ch2.delay_ch17));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch4");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch18, 0x00, sizeof(lidar_state_Ch2.delay_ch18));
	strncpy(lidar_state_Ch2.delay_ch18, value11, sizeof(lidar_state_Ch2.delay_ch18));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch6");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch19, 0x00, sizeof(lidar_state_Ch2.delay_ch19));
	strncpy(lidar_state_Ch2.delay_ch19, value11, sizeof(lidar_state_Ch2.delay_ch19));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch8");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch20, 0x00, sizeof(lidar_state_Ch2.delay_ch20));
	strncpy(lidar_state_Ch2.delay_ch20, value11, sizeof(lidar_state_Ch2.delay_ch20));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch10");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch21, 0x00, sizeof(lidar_state_Ch2.delay_ch21));
	strncpy(lidar_state_Ch2.delay_ch21, value11, sizeof(lidar_state_Ch2.delay_ch21));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch12");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch22, 0x00, sizeof(lidar_state_Ch2.delay_ch22));
	strncpy(lidar_state_Ch2.delay_ch22, value11, sizeof(lidar_state_Ch2.delay_ch22));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch14");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch23, 0x00, sizeof(lidar_state_Ch2.delay_ch23));
	strncpy(lidar_state_Ch2.delay_ch23, value11, sizeof(lidar_state_Ch2.delay_ch23));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch16");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch24, 0x00, sizeof(lidar_state_Ch2.delay_ch24));
	strncpy(lidar_state_Ch2.delay_ch24, value11, sizeof(lidar_state_Ch2.delay_ch24));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch18");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch25, 0x00, sizeof(lidar_state_Ch2.delay_ch25));
	strncpy(lidar_state_Ch2.delay_ch25, value11, sizeof(lidar_state_Ch2.delay_ch25));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch20");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch26, 0x00, sizeof(lidar_state_Ch2.delay_ch26));
	strncpy(lidar_state_Ch2.delay_ch26, value11, sizeof(lidar_state_Ch2.delay_ch26));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch22");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch27, 0x00, sizeof(lidar_state_Ch2.delay_ch27));
	strncpy(lidar_state_Ch2.delay_ch27, value11, sizeof(lidar_state_Ch2.delay_ch27));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch24");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch28, 0x00, sizeof(lidar_state_Ch2.delay_ch28));
	strncpy(lidar_state_Ch2.delay_ch28, value11, sizeof(lidar_state_Ch2.delay_ch28));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch26");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch29, 0x00, sizeof(lidar_state_Ch2.delay_ch29));
	strncpy(lidar_state_Ch2.delay_ch29, value11, sizeof(lidar_state_Ch2.delay_ch29));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch28");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch30, 0x00, sizeof(lidar_state_Ch2.delay_ch30));
	strncpy(lidar_state_Ch2.delay_ch30, value11, sizeof(lidar_state_Ch2.delay_ch30));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch30");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch31, 0x00, sizeof(lidar_state_Ch2.delay_ch31));
	strncpy(lidar_state_Ch2.delay_ch31, value11, sizeof(lidar_state_Ch2.delay_ch31));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch32");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch2.delay_ch32, 0x00, sizeof(lidar_state_Ch2.delay_ch32));
	strncpy(lidar_state_Ch2.delay_ch32, value11, sizeof(lidar_state_Ch2.delay_ch32));
	////////////////////////////////////////////////////////////////////////////////////



	EraseLidarInfo();
	int Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_, 0x00, LIDAR_STATE_SIZE);
	Status = ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
	memcpy(&lidar_state_, lidar_state_buffer_, sizeof(struct LidarState));
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*)&lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}


	EraseLidarInfo_Ch2();
	Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*)&lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH2);
	Status = ReadLidarInfo_Ch2(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH2);
	memcpy(&lidar_state_Ch2, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}


	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t GetRoutingdelay2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	web_param sysinfo_lidar_delay_param[32] = { {"ch1", "52"}, {"ch2", "57"}, {"ch3", "28"}, {"ch4", "26"}, {"ch5", "33"}, {"ch6", "40"},
				{"ch7", "45"}, {"ch8", "24"}, {"ch9", "58"}, {"ch10", "65"}, {"ch11", "11"}, {"ch12", "0"}, {"ch13", "13"}, {"ch14", "5"}, {"ch15", "5"},
				{"ch16", "0"}, {"ch17", "52"}, {"ch18", "57"}, {"ch19", "28"}, {"ch20", "26"}, {"ch21", "33"}, {"ch22", "40"}, {"ch23", "45"}, {"ch24",
						"35"}, {"ch25", "58"}, {"ch26", "65"}, {"ch27", "10"}, {"ch28", "0"}, {"ch29", "12"}, {"ch30", "5"}, {"ch31", "5"}, {"ch32", "0"}};

	struct LidarState_Ch lidar_state_tmp;
	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
	Status = ReadLidarInfo_Ch2(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH2);
	memcpy(&lidar_state_tmp, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	strncpy(sysinfo_lidar_delay_param[0].param_value, lidar_state_tmp.delay_ch1, sizeof(lidar_state_tmp.delay_ch1));
	strncpy(sysinfo_lidar_delay_param[1].param_value, lidar_state_tmp.delay_ch17, sizeof(lidar_state_tmp.delay_ch17));
	strncpy(sysinfo_lidar_delay_param[2].param_value, lidar_state_tmp.delay_ch2, sizeof(lidar_state_tmp.delay_ch2));
	strncpy(sysinfo_lidar_delay_param[3].param_value, lidar_state_tmp.delay_ch18, sizeof(lidar_state_tmp.delay_ch18));
	strncpy(sysinfo_lidar_delay_param[4].param_value, lidar_state_tmp.delay_ch3, sizeof(lidar_state_tmp.delay_ch3));
	strncpy(sysinfo_lidar_delay_param[5].param_value, lidar_state_tmp.delay_ch19, sizeof(lidar_state_tmp.delay_ch19));
	strncpy(sysinfo_lidar_delay_param[6].param_value, lidar_state_tmp.delay_ch4, sizeof(lidar_state_tmp.delay_ch4));
	strncpy(sysinfo_lidar_delay_param[7].param_value, lidar_state_tmp.delay_ch20, sizeof(lidar_state_tmp.delay_ch20));
	strncpy(sysinfo_lidar_delay_param[8].param_value, lidar_state_tmp.delay_ch5, sizeof(lidar_state_tmp.delay_ch5));
	strncpy(sysinfo_lidar_delay_param[9].param_value, lidar_state_tmp.delay_ch21, sizeof(lidar_state_tmp.delay_ch21));
	strncpy(sysinfo_lidar_delay_param[10].param_value, lidar_state_tmp.delay_ch6, sizeof(lidar_state_tmp.delay_ch6));
	strncpy(sysinfo_lidar_delay_param[11].param_value, lidar_state_tmp.delay_ch22, sizeof(lidar_state_tmp.delay_ch22));
	strncpy(sysinfo_lidar_delay_param[12].param_value, lidar_state_tmp.delay_ch7, sizeof(lidar_state_tmp.delay_ch7));
	strncpy(sysinfo_lidar_delay_param[13].param_value, lidar_state_tmp.delay_ch23, sizeof(lidar_state_tmp.delay_ch23));
	strncpy(sysinfo_lidar_delay_param[14].param_value, lidar_state_tmp.delay_ch8, sizeof(lidar_state_tmp.delay_ch8));
	strncpy(sysinfo_lidar_delay_param[15].param_value, lidar_state_tmp.delay_ch24, sizeof(lidar_state_tmp.delay_ch24));
	strncpy(sysinfo_lidar_delay_param[16].param_value, lidar_state_tmp.delay_ch9, sizeof(lidar_state_tmp.delay_ch9));
	strncpy(sysinfo_lidar_delay_param[17].param_value, lidar_state_tmp.delay_ch25, sizeof(lidar_state_tmp.delay_ch25));
	strncpy(sysinfo_lidar_delay_param[18].param_value, lidar_state_tmp.delay_ch10, sizeof(lidar_state_tmp.delay_ch10));
	strncpy(sysinfo_lidar_delay_param[19].param_value, lidar_state_tmp.delay_ch26, sizeof(lidar_state_tmp.delay_ch26));
	strncpy(sysinfo_lidar_delay_param[20].param_value, lidar_state_tmp.delay_ch11, sizeof(lidar_state_tmp.delay_ch11));
	strncpy(sysinfo_lidar_delay_param[21].param_value, lidar_state_tmp.delay_ch27, sizeof(lidar_state_tmp.delay_ch27));
	strncpy(sysinfo_lidar_delay_param[22].param_value, lidar_state_tmp.delay_ch12, sizeof(lidar_state_tmp.delay_ch12));
	strncpy(sysinfo_lidar_delay_param[23].param_value, lidar_state_tmp.delay_ch28, sizeof(lidar_state_tmp.delay_ch28));
	strncpy(sysinfo_lidar_delay_param[24].param_value, lidar_state_tmp.delay_ch13, sizeof(lidar_state_tmp.delay_ch13));
	strncpy(sysinfo_lidar_delay_param[25].param_value, lidar_state_tmp.delay_ch29, sizeof(lidar_state_tmp.delay_ch29));
	strncpy(sysinfo_lidar_delay_param[26].param_value, lidar_state_tmp.delay_ch14, sizeof(lidar_state_tmp.delay_ch14));
	strncpy(sysinfo_lidar_delay_param[27].param_value, lidar_state_tmp.delay_ch30, sizeof(lidar_state_tmp.delay_ch30));
	strncpy(sysinfo_lidar_delay_param[28].param_value, lidar_state_tmp.delay_ch15, sizeof(lidar_state_tmp.delay_ch15));
	strncpy(sysinfo_lidar_delay_param[29].param_value, lidar_state_tmp.delay_ch31, sizeof(lidar_state_tmp.delay_ch31));
	strncpy(sysinfo_lidar_delay_param[30].param_value, lidar_state_tmp.delay_ch16, sizeof(lidar_state_tmp.delay_ch16));
	strncpy(sysinfo_lidar_delay_param[31].param_value, lidar_state_tmp.delay_ch32, sizeof(lidar_state_tmp.delay_ch32));

	TransFormJSON(sysinfo_lidar_delay_param, buf, 32);

	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}
int32_t SetAziCal(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char ch[30] = {0, };
	char distance[30] = {0, };

	char key[100] = "serial";


//	azi_cal_.AziOffsets_[0].ch_datas[0] = 0x1234;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "azimuth");
	GetJsonValueByKey(value, req, key);

	memset(ch, 0x00, sizeof(ch));
	memset(key, 0x00, sizeof(key));
	strcat(key, "ch");
	GetJsonValueByKey(ch, req, key);

	memset(distance, 0x00, sizeof(distance));
	memset(key, 0x00, sizeof(key));
	strcat(key, "offset");
	GetJsonValueByKey(distance, req, key);


	int now_azi = atoi(value);
	int now_ch = atoi(ch);
	int now_dis = atoi(distance);


	azi_cal_.azimuth_calibration_info[azi_cal_.azimuth_calibration_setting.calibration_info_count].col = now_azi;
	azi_cal_.azimuth_calibration_info[azi_cal_.azimuth_calibration_setting.calibration_info_count].row = now_ch;
	azi_cal_.azimuth_calibration_info[azi_cal_.azimuth_calibration_setting.calibration_info_count].offset = now_dis;
	azi_cal_.azimuth_calibration_setting.calibration_info_count++;
	if(now_azi == 1 && now_ch == 1 &&  now_dis== 1)
	{
		memset(&azi_cal_,0x00,sizeof(struct AzimuthCal));
	}


	int Status = InitLinearQspiFlash();
	EraseAzioffset();
	Status = LinearQspiFlashWriteForAzioffset((char*) &azi_cal_, AZI_OFFSET_SIZE);


	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}
//int32_t SetAziCal(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
//{
//	char value[100] = {0, };
//	char ch[30] = {0, };
//	char distance[30] = {0, };
//
//	char key[100] = "serial";
//
//
////	azi_cal_.AziOffsets_[0].ch_datas[0] = 0x1234;
//
//
//	memset(value, 0x00, sizeof(value));
//	memset(key, 0x00, sizeof(key));
//	strcat(key, "azimuth");
//	GetJsonValueByKey(value, req, key);
//
//	memset(ch, 0x00, sizeof(ch));
//	memset(key, 0x00, sizeof(key));
//	strcat(key, "ch");
//	GetJsonValueByKey(ch, req, key);
//
//	memset(distance, 0x00, sizeof(distance));
//	memset(key, 0x00, sizeof(key));
//	strcat(key, "offset");
//	GetJsonValueByKey(distance, req, key);
//
//
//	int now_azi = atoi(value);
//	int now_ch = atoi(ch);
//	int now_dis = atoi(distance);
//
//	azi_cal_.AziOffsets_[now_azi].ch_datas[now_ch] = now_dis;
//
//	int Status = InitLinearQspiFlash();
//	EraseAzioffset();
//	Status = LinearQspiFlashWriteForAzioffset((char*) &azi_cal_, AZI_OFFSET_SIZE);
//
//
//	generate_http_header(buf, NULL, 0);
//	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
//	{
//		xil_printf("error writing http POST response to socket\n\r");
//		xil_printf("http header = %s\r\n", buf);
//		return -1;
//	}
//	return 1;
//}

int32_t GetDarkAreaCorrectionSettings(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	web_param settings[39] = { {"EnableCorrection", "0"}, {"EnableZeroDistCorrection", "0"}, {"Channels", ""}, {"Parameter1", "0"}, {"Parameter2", "0"}, {"Parameter3", "0"},
			{"Parameter4", "0"}, {"Ch1", "0"}, {"Ch2", "0"}, {"Ch3", "0"}, {"Ch4", "0"}, {"Ch5", "0"}, {"Ch6", "0"}, {"Ch7", "0"}, {"Ch8", "0"}, {"Ch9", "0"}
	, {"Ch10", "0"}, {"Ch11", "0"}, {"Ch12", "0"}, {"Ch13", "0"}, {"Ch14", "0"}, {"Ch15", "0"}, {"Ch16", "0"}, {"Ch17", "0"}
	, {"Ch18", "0"}, {"Ch19", "0"}, {"Ch20", "0"}, {"Ch21", "0"}, {"Ch22", "0"}, {"Ch23", "0"}, {"Ch24", "0"}, {"Ch25", "0"}
	, {"Ch26", "0"}, {"Ch27", "0"}, {"Ch28", "0"}, {"Ch29", "0"}, {"Ch30", "0"}, {"Ch31", "0"}, {"Ch32", "0"}};

	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
	Status = ReadLidarInfo((u8*)&lidar_state_, LIDAR_STATE_SIZE);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	itoa(lidar_state_.filter_setting_.dark_area_error_correction_setting_.is_enable_dark_area_crror_correction_, settings[0].param_value, 10);
	itoa(lidar_state_.filter_setting_.dark_area_error_correction_setting_.is_enable_zero_dist_cor_, settings[1].param_value, 10);
	for(size_t i = 0 ; i < lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_channel_count_ ; i++)
	{
		char str[10] = {0,};
		itoa(lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_channel_[i], str, 10);
		strcat(settings[2].param_value, str);
		strcat(settings[2].param_value, " ");
	}

	sprintf(settings[3].param_value, "%f", lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_error_cor_param1_);
	sprintf(settings[4].param_value, "%f", lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_error_cor_param2_);
	sprintf(settings[5].param_value, "%f", lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_error_cor_param3_);
	sprintf(settings[6].param_value, "%f", lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_error_cor_param4_);


	TransFormJSON(settings, buf, 39);

	if(tcp_write(pcb, buf, 1024, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}


int32_t GetAdminInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{

//	web_param sysinfo_param[35] = { {"serial", "0"}, {"mdate", "0"}, {"firmwarever", "0"}, {"fpgaver", "0"}, {"udate", "0"}, {"mac", "0"},
//			{"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"golden_ver", "0"}, {"golden_u_date", "0"}, {"noise_percent", "0"}, {"noiseon", "0"}, {
//					"voltageon", "0"}, {"vth_sensitivity", "0"}, {"valid_point_cnt", "0"}, {"noise_cnt_limit", "0"}, {"propagation_delay", "0"}, {
//					"upper_offset", "0"}, {"lower_offset", "0"}, {"IsNoiseOn", "0"}, {"IsGhostOn", "0"}, {"MinVth", "0"}, {"boot_img_name", "0"},{"IsNewNoiseOn", "0"},
//					{"total_start_enc_offset", "0"}, {"max_distance", "0"}, {"ch_dly2", "0"},
//					{"tr_dly", "0"}, {"tf_dly", "0"}, {"EM_pulse", "0"}, {"motor_rpm", "0"},
//					{"total_end_enc_offset", "0"}, {"IsHeatOn", "0"}, {"total_enc_offset", "0"}};


//	web_param sysinfo_param[41] = { {"serial", "0"}, {"mdate", "0"}, {"firmwarever", "0"}, {"fpgaver", "0"}, {"udate", "0"}, {"mac", "0"},
//				{"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"golden_ver", "0"}, {"golden_u_date", "0"}, {"noise_percent", "0"}, {"noiseon", "0"}, {
//						"voltageon", "0"}, {"vth_sensitivity", "0"}, {"valid_point_cnt", "0"}, {"noise_cnt_limit", "0"}, {"propagation_delay", "0"}, {
//						"upper_offset", "0"}, {"lower_offset", "0"}, {"IsNoiseOn", "0"}, {"IsGhostOn", "0"}, {"MinVth", "0"}, {"boot_img_name", "0"},{"IsNewNoiseOn", "0"},
//						{"total_start_enc_offset", "0"}, {"max_distance", "0"}, {"ch_dly2", "0"},
//						{"tr_dly", "0"}, {"tf_dly", "0"}, {"EM_pulse", "0"}, {"motor_rpm", "0"},
//						{"total_end_enc_offset", "0"}, {"IsHeatOn", "0"}, {"total_enc_offset", "0"},
//						{"calib_onoff","0"},{"distance_offset","0"},{"fov_correction","0"}, {"IsFIROn","0"}, {"IsIntensityCorrectionOn","0"},
//						{"phase_Deg","0"}};

//	web_param sysinfo_param[42] = { {"serial", "0"}, {"mdate", "0"}, {"firmwarever", "0"}, {"fpgaver", "0"}, {"udate", "0"}, {"mac", "0"},
//				{"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"golden_ver", "0"}, {"golden_u_date", "0"}, {"noise_percent", "0"}, {"noiseon", "0"}, {
//						"voltageon", "0"}, {"vth_sensitivity", "0"}, {"valid_point_cnt", "0"}, {"noise_cnt_limit", "0"}, {"propagation_delay", "0"}, {
//						"upper_offset", "0"}, {"lower_offset", "0"}, {"IsNoiseOn", "0"}, {"IsGhostOn", "0"}, {"MinVth", "0"}, {"boot_img_name", "0"},{"IsNewNoiseOn", "0"},
//						{"total_start_enc_offset", "0"}, {"max_distance", "0"}, {"ch_dly2", "0"},
//						{"tr_dly", "0"}, {"tf_dly", "0"}, {"EM_pulse", "0"}, {"motor_rpm", "0"},
//						{"total_end_enc_offset", "0"}, {"IsHeatOn", "0"}, {"total_enc_offset", "0"},
//						{"calib_onoff","0"},{"distance_offset","0"},{"fov_correction","0"}, {"IsFIROn","0"}, {"IsIntensityCorrectionOn","0"},
//						{"phase_Deg","0"}, {"IsAziOffset", "0"} };

//	web_param sysinfo_param[43] = { {"serial", "0"}, {"mdate", "0"}, {"firmwarever", "0"}, {"fpgaver", "0"}, {"udate", "0"}, {"mac", "0"},
//				{"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"golden_ver", "0"}, {"golden_u_date", "0"}, {"noise_percent", "0"}, {"noiseon", "0"}, {
//						"voltageon", "0"}, {"vth_sensitivity", "0"}, {"valid_point_cnt", "0"}, {"noise_cnt_limit", "0"}, {"propagation_delay", "0"}, {
//						"upper_offset", "0"}, {"lower_offset", "0"}, {"IsNoiseOn", "0"}, {"IsGhostOn", "0"}, {"MinVth", "0"}, {"boot_img_name", "0"},{"IsNewNoiseOn", "0"},
//						{"total_start_enc_offset", "0"}, {"max_distance", "0"}, {"ch_dly2", "0"},
//						{"tr_dly", "0"}, {"tf_dly", "0"}, {"EM_pulse", "0"}, {"motor_rpm", "0"},
//						{"total_end_enc_offset", "0"}, {"IsHeatOn", "0"}, {"total_enc_offset", "0"},
//						{"calib_onoff","0"},{"distance_offset","0"},{"fov_correction","0"}, {"IsFIROn","0"}, {"IsIntensityCorrectionOn","0"},
//						{"phase_Deg","0"}, {"IsAziOffset", "0"}, {"IsTemperAdj", "0"}};


	web_param sysinfo_param[44] = { {"serial", "0"}, {"mdate", "0"}, {"firmwarever", "0"}, {"fpgaver", "0"}, {"udate", "0"}, {"mac", "0"},
				{"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"golden_ver", "0"}, {"golden_u_date", "0"}, {"noise_percent", "0"}, {"noiseon", "0"}, {
						"voltageon", "0"}, {"vth_sensitivity", "0"}, {"valid_point_cnt", "0"}, {"noise_cnt_limit", "0"}, {"propagation_delay", "0"}, {
						"upper_offset", "0"}, {"lower_offset", "0"}, {"IsNoiseOn", "0"}, {"IsGhostOn", "0"}, {"MinVth", "0"}, {"boot_img_name", "0"},{"IsNewNoiseOn", "0"},
						{"total_start_enc_offset", "0"}, {"max_distance", "0"}, {"ch_dly2", "0"},
						{"tr_dly", "0"}, {"tf_dly", "0"}, {"EM_pulse", "0"}, {"motor_rpm", "0"},
						{"total_end_enc_offset", "0"}, {"IsHeatOn", "0"}, {"total_enc_offset", "0"},
						{"calib_onoff","0"},{"distance_offset","0"},{"fov_correction","0"}, {"IsFIROn","0"}, {"IsIntensityCorrectionOn","0"},
						{"phase_Deg","0"}, {"IsAziOffset", "0"}, {"IsTemperAdj", "0"}, {"IsTemperGain", "0"}};



	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
	Status = ReadLidarInfo((u8*)&lidar_state_, LIDAR_STATE_SIZE);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

//	strncpy(lidar_state_.lidar_info_.boot_image_name, "G32A3230412", 12);

//	strncpy(lidar_state_.lidar_info_.manufacturing_date, "2023-04-05",10);

	strncpy(sysinfo_param[0].param_value, lidar_state_.lidar_info_.serial, sizeof(lidar_state_.lidar_info_.serial));
//	strncpy(sysinfo_param[1].param_value, lidar_state_.lidar_info_.manufacturing_date, sizeof(lidar_state_.lidar_info_.manufacturing_date));
	strncpy(sysinfo_param[1].param_value, "2023-04-05", 10);

//	strncpy(sysinfo_param[2].param_value, lidar_state_.lidar_info_.firmware_ver, sizeof(lidar_state_.lidar_info_.firmware_ver));
//	strncpy(sysinfo_param[3].param_value, lidar_state_.lidar_info_.fpga_ver, sizeof(lidar_state_.lidar_info_.fpga_ver));
#ifdef G32_B1_PPS
	strncpy(sysinfo_param[2].param_value, "F0301", 6);
	strncpy(sysinfo_param[3].param_value, "L0300", 6);
#else
	strncpy(sysinfo_param[2].param_value, "F0208", 6);
	strncpy(sysinfo_param[3].param_value, "L0204", 6);
#endif
	strncpy(sysinfo_param[4].param_value, lidar_state_.lidar_info_.firmware_u_date, sizeof(lidar_state_.lidar_info_.firmware_u_date));
	strncpy(sysinfo_param[5].param_value, lidar_state_.network_setting_.mac, sizeof(lidar_state_.network_setting_.mac));
	strncpy(sysinfo_param[6].param_value, lidar_state_.network_setting_.ip, sizeof(lidar_state_.network_setting_.ip));
	strncpy(sysinfo_param[7].param_value, lidar_state_.network_setting_.subnet, sizeof(lidar_state_.network_setting_.subnet));
	strncpy(sysinfo_param[8].param_value, lidar_state_.network_setting_.gateway, sizeof(lidar_state_.network_setting_.gateway));
	strncpy(sysinfo_param[9].param_value, lidar_state_.lidar_info_.golden_ver, sizeof(lidar_state_.lidar_info_.golden_ver));
	strncpy(sysinfo_param[10].param_value, lidar_state_.lidar_info_.golden_u_date, sizeof(lidar_state_.lidar_info_.golden_u_date));
	strncpy(sysinfo_param[11].param_value, lidar_state_.filter_setting_.noise_percent, sizeof(lidar_state_.filter_setting_.noise_percent));
	itoa(lidar_state_.filter_setting_.is_noise_on, sysinfo_param[12].param_value, 10);
	itoa(lidar_state_.filter_setting_.is_voltage_on, sysinfo_param[13].param_value, 10);
	strncpy(sysinfo_param[14].param_value, lidar_state_.filter_setting_.vth_sensitivity, sizeof(lidar_state_.filter_setting_.vth_sensitivity));
	strncpy(sysinfo_param[15].param_value, lidar_state_.filter_setting_.valid_point_cnt, sizeof(lidar_state_.filter_setting_.valid_point_cnt));
	strncpy(sysinfo_param[16].param_value, lidar_state_.filter_setting_.noise_cnt_limit, sizeof(lidar_state_.filter_setting_.noise_cnt_limit));
	strncpy(sysinfo_param[17].param_value, lidar_state_.calibration_setting_.propagation_delay,
			sizeof(lidar_state_.calibration_setting_.propagation_delay));
	strncpy(sysinfo_param[18].param_value, lidar_state_.calibration_setting_.upper_offset, sizeof(lidar_state_.calibration_setting_.upper_offset));
	strncpy(sysinfo_param[19].param_value, lidar_state_.calibration_setting_.lower_offset, sizeof(lidar_state_.calibration_setting_.lower_offset));
	itoa(lidar_state_.filter_setting_.is_noise_filter_on, sysinfo_param[20].param_value, 10);
	itoa(lidar_state_.filter_setting_.is_remove_ghost_on, sysinfo_param[21].param_value, 10);
	strncpy(sysinfo_param[22].param_value, lidar_state_.filter_setting_.noise_min_value, sizeof(lidar_state_.filter_setting_.noise_min_value));

//	strncpy(sysinfo_param[23].param_value, lidar_state_.lidar_info_.boot_image_name, sizeof(lidar_state_.lidar_info_.boot_image_name));

//	strncpy(sysinfo_param[23].param_value, "G32B1-B0104", 12);
//	strncpy(sysinfo_param[23].param_value, "G32B1-B0105", 12);
//	strncpy(sysinfo_param[23].param_value, "G32B1-B0106", 12);
	strncpy(sysinfo_param[23].param_value, "G32B1-B0112", 12);
	itoa(lidar_state_.filter_setting_.is_new_noise_filter_on, sysinfo_param[24].param_value, 10);
//	itoa(lidar_state_.motor_setting_.total_enc_offset, sysinfo_param[25].param_value, 10);
	itoa(lidar_state_.motor_setting_.start_enc_count, sysinfo_param[25].param_value, 10);
//	itoa(lidar_state_.motor_setting_.bottom_enc_offset, sysinfo_param[26].param_value, 10);
//	itoa(lidar_state_.motor_setting_.top_enc_offset, sysinfo_param[27].param_value, 10);
	itoa(lidar_state_.laser_setting_.max_distance, sysinfo_param[26].param_value, 10);
	itoa(lidar_state_.laser_setting_.ch_interval_2, sysinfo_param[27].param_value, 10);
	itoa(lidar_state_.laser_setting_.tdtr, sysinfo_param[28].param_value, 10);
	itoa(lidar_state_.laser_setting_.tdtf, sysinfo_param[29].param_value, 10);
	itoa(lidar_state_.laser_setting_.em_pulse, sysinfo_param[30].param_value, 10);
	itoa(lidar_state_.motor_setting_.motor_rpm, sysinfo_param[31].param_value, 10);
	itoa(lidar_state_.motor_setting_.end_enc_count, sysinfo_param[32].param_value, 10);
//	itoa(lidar_state_.motor_setting_.bottom_end_enc_offset, sysinfo_param[35].param_value, 10);
//	itoa(lidar_state_.motor_setting_.top_end_enc_offset, sysinfo_param[36].param_value, 10);
//	itoa(is_heat_control_ON, sysinfo_param[33].param_value, 10);
	itoa(lidar_state_.filter_setting_.is_heat_ctrl_on, sysinfo_param[33].param_value, 10);

	itoa(lidar_state_.motor_setting_.total_enc_offset, sysinfo_param[34].param_value, 10);

//	strncpy(sysinfo_param[25].param_value, lidar_state_.motor_setting_.total_enc_offset, sizeof(lidar_state_.motor_setting_.total_enc_offset));
//	TransFormJSON(sysinfo_param, buf, 24);//jihun
//	TransFormJSON(sysinfo_param, buf, 25);//jihun
//	TransFormJSON(sysinfo_param, buf, 30);//jihun
//	TransFormJSON(sysinfo_param, buf, 34);//jihun
//	TransFormJSON(sysinfo_param, buf, 38);//jihun
//	TransFormJSON(sysinfo_param, buf, 34);//jihun
//	TransFormJSON(sysinfo_param, buf, 35);//jihun


	// 240603 hohoyeok
	itoa(lidar_state_.a2z_setting.reserved02, sysinfo_param[35].param_value, 10);

	itoa(lidar_state_.a2z_setting.dist_offset, sysinfo_param[36].param_value, 10);

	itoa(lidar_state_.a2z_setting.fov_correction, sysinfo_param[37].param_value, 10);

	//240614 jh
	itoa(lidar_state_.filter_setting_.is_fir_on, sysinfo_param[38].param_value, 10);

	//240614 hohyeok   IsIntensityCorrectionOn

	itoa(lidar_state_.filter_setting_.is_intensity_correction_on, sysinfo_param[39].param_value, 10);

	int tempdeg = 0.010986663 * lidar_state_.motor_setting_.pll_point;
	tempdeg = 45 - tempdeg;
	itoa(tempdeg, sysinfo_param[40].param_value, 10);

	itoa(lidar_state_.calibration_setting_.azioffset_enable, sysinfo_param[41].param_value, 10);

	itoa(lidar_state_.calibration_setting_.temperadj_enable, sysinfo_param[42].param_value, 10);

	itoa(lidar_state_.calibration_setting_.tempergain_enable, sysinfo_param[43].param_value, 10);



//	TransFormJSON(sysinfo_param, buf, 38);//hohyeok
//	TransFormJSON(sysinfo_param, buf, 43);//240617 hohyeok
	TransFormJSON(sysinfo_param, buf, 44);//241127 jh



//	TDCHistoBramCtrlInit();
//	u32 fine_lut[16][256] = {0,};
//	u32 Addr, Data;
//	int i, j = 0;
//
//	for(j=0; j<16; j++) {
//		TDCChSel(j);
//		usleep(1);
//
//		i = 0;
//		for(Addr = pTDCLutBramCtrlCfg->MemBaseAddress; Addr < pTDCLutBramCtrlCfg->MemBaseAddress + (4*255); Addr+=4) {
//			fine_lut[j][i] = XBram_In32(Addr);
//			//xil_printf("[DEBUG] Fine Lut[%d] = %d\r\n", i, fine_lut[i]);
//			i++;
//		}
//	}
//
//	memset(buf, 0x00, sizeof(buf));
//	size_t channel = 0;
//	char value[20] = {0,};
//	strcat(buf, itoa(fine_lut[channel][100], value, 10));
//	strcat(buf, ",");
//
//	memset(value, 0x00, sizeof(value));
//	strcat(buf, itoa(fine_lut[channel][150], value, 10));
//	strcat(buf, ",");
//
//	memset(value, 0x00, sizeof(value));
//	strcat(buf, itoa(fine_lut[channel][200], value, 10));
//	strcat(buf, ",");





	if(tcp_write(pcb, buf, 1024, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t UpdateGoldenImage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	file_download_flag = 1;
	is_golden_img = 1;
	e_file_recv_status = EMPTY;
	content_length = 0;
	recved_size = 0;
	percent_val = 0;
	recv_file_size = 0;
	reset_ret_val = 0;
	memset(Total_buf, 0x00, sizeof(Total_buf));
	//		memset(pre_boot_image, 0x00, sizeof(Total_buf));
	//		memset(read_buffer, 0x00, sizeof(Total_buf));
	do_http_file_ok(pcb, req, rlen, &e_file_recv_status);
	return 1;
}

int32_t SetUpdateDate(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "updatedate";
	GetJsonValueByKey(value, req, key);

	strncpy(lidar_state_.lidar_info_.firmware_u_date, value, sizeof(lidar_state_.lidar_info_.firmware_u_date));

	EraseLidarInfo();
	int Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*)&lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch2();
	Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*)&lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	Status = ReadLidarInfo((u8*)&lidar_state_, LIDAR_STATE_SIZE);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetNoiseFilterOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "noiseonoff";
	GetJsonValueByKey(value, req, key);

	uint8_t result = 0;

	if(strcmp(value, "true") == 0)
		result = 1;

	lidar_state_.filter_setting_.is_noise_on = result;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetNoiseFilterOnOff2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "noiseonoff";
	GetJsonValueByKey(value, req, key);

	uint8_t result = 0;

	if(strcmp(value, "true") == 0)
		result = 1;
	else
		result = 0;

	lidar_state_.filter_setting_.is_noise_filter_on = result;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetNoiseFilterOnOff4(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "noiseonoff";
	GetJsonValueByKey(value, req, key);

	uint8_t result = 0;

	if(strcmp(value, "true") == 0)
		result = 1;
	else
		result = 0;

	lidar_state_.filter_setting_.is_new_noise_filter_on = result;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetHzOption(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "hzoption";
	GetJsonValueByKey(value, req, key);

	int hzoption = atoi(value);

	if(hzoption < 0 || hzoption >= 5)
	{
		hzoption = 0;
	}

	lidar_state_.motor_setting_.hz_setting = hzoption;

	if(hzoption== 0)
	{
		HZ_VAL = 0;

		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 140); // CH0-CH3	//180
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 140); // CH12-CH15
		LDAllSet();	//set default ld

		if(lidar_state_.motor_setting_.motor_rpm >= 1400)
		{
			MotorCtrlRPMSet(1350);
			usleep(500000);
//			MotorCtrlRPMSet(1300);
//			usleep(15000);
		}
		lidar_state_.motor_setting_.motor_rpm = 1200;
		MotorCtrlRPMSet(lidar_state_.motor_setting_.motor_rpm);
	}
	else if(hzoption == 1)
	{
		HZ_VAL = 1;

		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 140); // CH0-CH3	//180
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 140); // CH12-CH15
		LDAllSet();	//set default ld

		if(lidar_state_.motor_setting_.motor_rpm >= 1400)
		{
			MotorCtrlRPMSet(1350);
			usleep(500000);
//			MotorCtrlRPMSet(1300);
//			usleep(15000);
		}
		lidar_state_.motor_setting_.motor_rpm = 1200;
		MotorCtrlRPMSet(lidar_state_.motor_setting_.motor_rpm);
	}
	else if(hzoption == 2)
	{
		HZ_VAL = 2;

//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 99); // CH0-CH3	//179.9
//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 99); // CH12-CH15
//
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(279));	//1	//180
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(0));	//2
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(398));	//3	//180
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(160));	//4

		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 110); // CH0-CH3	//180
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 110); // CH12-CH15

//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 99); // CH0-CH3	//180
//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 140); // CH12-CH15

		//org
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(279));	//1	//180
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(0));	//2
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(398));	//3	//180
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(160));	//4

		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(300));	//1	//180
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(0));	//2
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(435));	//3	//180
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(165));	//4

//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(330));	//1	//180
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(0));	//2
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(454));	//3	//180
//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(165));	//4


		lidar_state_.motor_setting_.motor_rpm = 1500;
		MotorCtrlRPMSet(lidar_state_.motor_setting_.motor_rpm);
	}
	else if(hzoption == 3)	//1200rpm, bottom only
	{
		HZ_VAL = 0;

		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 140); // CH0-CH3	//180
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 140); // CH12-CH15
		LDAllSet();	//set default ld

		if(lidar_state_.motor_setting_.motor_rpm >= 1400)
		{
			MotorCtrlRPMSet(1350);
			usleep(500000);
//			MotorCtrlRPMSet(1300);
//			usleep(15000);
		}
		lidar_state_.motor_setting_.motor_rpm = 1200;
		MotorCtrlRPMSet(lidar_state_.motor_setting_.motor_rpm);
	}
	else if(hzoption == 4)	////1200rpm, top only
	{
		HZ_VAL = 0;

		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 140); // CH0-CH3	//180
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 140); // CH12-CH15
		LDAllSet();	//set default ld

		if(lidar_state_.motor_setting_.motor_rpm >= 1400)
		{
			MotorCtrlRPMSet(1350);
			usleep(500000);
//			MotorCtrlRPMSet(1300);
//			usleep(15000);
		}
		lidar_state_.motor_setting_.motor_rpm = 1200;
		MotorCtrlRPMSet(lidar_state_.motor_setting_.motor_rpm);
	}


	pass_frame = 2;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetStopOption(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "stopoption";
	GetJsonValueByKey(value, req, key);

	int stopoption = atoi(value);

	if(stopoption < 0 || stopoption >= 3)
	{
		stopoption = 0;
	}
	stop_option = stopoption;
	lidar_state_.laser_setting_.stop_option = stopoption;

	pass_frame = 2;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}


int32_t SetVNoiseFilterOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "vnoiseonoff";
	GetJsonValueByKey(value, req, key);

	if(strcmp(value, "true") == 0)
	{
		vnfilter = 1;
	}
	else
	{
		vnfilter = 0;
	}

	generate_http_header(buf, NULL, 0);

	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetVthFilterOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "voltageonoff";
	GetJsonValueByKey(value, req, key);

	uint8_t result = 0;

	if(strcmp(value, "true") == 0)
		result = 1;

	lidar_state_.filter_setting_.is_voltage_on = result;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t SetGoldenImageUpdateDate(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "updatedate";
	GetJsonValueByKey(value, req, key);

	strncpy(lidar_state_.lidar_info_.golden_u_date, value, sizeof(lidar_state_.lidar_info_.golden_u_date));

	EraseLidarInfo();
	int Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*)&lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch2();
		Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*)&lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

		if(Status != XST_SUCCESS)
		{
			xil_printf("Flash Write Failed\r\n");
			return XST_FAILURE;
		}

	Status = ReadLidarInfo((u8*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int do_http_file_ok(struct tcp_pcb *pcb, char *req, int rlen, enum eFileRecvStatus *recv_status)
{

	char* ptr_content_length = strstr(req, "Content-Length: ");

	strtok(ptr_content_length, " ");
	int i = 0;
	ptr_content_length = strtok(NULL, "\r\n");
	uint8_t is_content_length_digit = 1;
	for(i = 0; i < strlen(ptr_content_length) ; ++i)
	{
		if(isdigit(ptr_content_length[i]) == 0)
			is_content_length_digit = 0;
	}

	if(is_content_length_digit == 1)
	{
		content_length = atoi(ptr_content_length);
		*recv_status = HEADER_PARSED;
	}
	else
	{
		xil_printf("CONTENT_LENGTH PARSING ERR");
		return -1;
	}
	xil_printf("len = %d\r\n", content_length);

	return 0;
}

/* dynamically generate 404 response:
 *	this inserts the original request string in betwween the notfound_header & footer strings
 */
int do_404(struct tcp_pcb *pcb, char *req, int rlen)
{
	int len, hlen;
	int BUFSIZE = 1024;
	char buf[BUFSIZE];
	err_t err;

	len = strlen(notfound_header) + strlen(notfound_footer) + rlen;

	hlen = generate_http_header((char *)buf, "html", len);

	if(tcp_sndbuf(pcb) < hlen)
	{
		xil_printf("cannot send 404 message, tcp_sndbuf = %d bytes, message length = %d bytes\r\n", tcp_sndbuf(pcb), hlen);
		return -1;
	}
	if((err = tcp_write(pcb, buf, hlen, 1)) != ERR_OK)
	{
		xil_printf("%s: error (%d) writing 404 http header\r\n", __FUNCTION__, err);
		return -1;
	}
	tcp_write(pcb, notfound_header, strlen(notfound_header), 1);
	tcp_write(pcb, req, rlen, 1);
	tcp_write(pcb, notfound_footer, strlen(notfound_footer), 1);

	return 0;
}

int32_t GetRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	web_param sysinfo_lidar_delay_param[32] = { {"ch1", "52"}, {"ch2", "57"}, {"ch3", "28"}, {"ch4", "26"}, {"ch5", "33"}, {"ch6", "40"},
			{"ch7", "45"}, {"ch8", "24"}, {"ch9", "58"}, {"ch10", "65"}, {"ch11", "11"}, {"ch12", "0"}, {"ch13", "13"}, {"ch14", "5"}, {"ch15", "5"},
			{"ch16", "0"}, {"ch17", "52"}, {"ch18", "57"}, {"ch19", "28"}, {"ch20", "26"}, {"ch21", "33"}, {"ch22", "40"}, {"ch23", "45"}, {"ch24",
					"35"}, {"ch25", "58"}, {"ch26", "65"}, {"ch27", "10"}, {"ch28", "0"}, {"ch29", "12"}, {"ch30", "5"}, {"ch31", "5"}, {"ch32", "0"}};

	struct LidarState_Ch lidar_state_tmp;
	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_tmp, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	strncpy(sysinfo_lidar_delay_param[0].param_value, lidar_state_tmp.delay_ch1, sizeof(lidar_state_tmp.delay_ch1));
	strncpy(sysinfo_lidar_delay_param[1].param_value, lidar_state_tmp.delay_ch17, sizeof(lidar_state_tmp.delay_ch17));
	strncpy(sysinfo_lidar_delay_param[2].param_value, lidar_state_tmp.delay_ch2, sizeof(lidar_state_tmp.delay_ch2));
	strncpy(sysinfo_lidar_delay_param[3].param_value, lidar_state_tmp.delay_ch18, sizeof(lidar_state_tmp.delay_ch18));
	strncpy(sysinfo_lidar_delay_param[4].param_value, lidar_state_tmp.delay_ch3, sizeof(lidar_state_tmp.delay_ch3));
	strncpy(sysinfo_lidar_delay_param[5].param_value, lidar_state_tmp.delay_ch19, sizeof(lidar_state_tmp.delay_ch19));
	strncpy(sysinfo_lidar_delay_param[6].param_value, lidar_state_tmp.delay_ch4, sizeof(lidar_state_tmp.delay_ch4));
	strncpy(sysinfo_lidar_delay_param[7].param_value, lidar_state_tmp.delay_ch20, sizeof(lidar_state_tmp.delay_ch20));
	strncpy(sysinfo_lidar_delay_param[8].param_value, lidar_state_tmp.delay_ch5, sizeof(lidar_state_tmp.delay_ch5));
	strncpy(sysinfo_lidar_delay_param[9].param_value, lidar_state_tmp.delay_ch21, sizeof(lidar_state_tmp.delay_ch21));
	strncpy(sysinfo_lidar_delay_param[10].param_value, lidar_state_tmp.delay_ch6, sizeof(lidar_state_tmp.delay_ch6));
	strncpy(sysinfo_lidar_delay_param[11].param_value, lidar_state_tmp.delay_ch22, sizeof(lidar_state_tmp.delay_ch22));
	strncpy(sysinfo_lidar_delay_param[12].param_value, lidar_state_tmp.delay_ch7, sizeof(lidar_state_tmp.delay_ch7));
	strncpy(sysinfo_lidar_delay_param[13].param_value, lidar_state_tmp.delay_ch23, sizeof(lidar_state_tmp.delay_ch23));
	strncpy(sysinfo_lidar_delay_param[14].param_value, lidar_state_tmp.delay_ch8, sizeof(lidar_state_tmp.delay_ch8));
	strncpy(sysinfo_lidar_delay_param[15].param_value, lidar_state_tmp.delay_ch24, sizeof(lidar_state_tmp.delay_ch24));
	strncpy(sysinfo_lidar_delay_param[16].param_value, lidar_state_tmp.delay_ch9, sizeof(lidar_state_tmp.delay_ch9));
	strncpy(sysinfo_lidar_delay_param[17].param_value, lidar_state_tmp.delay_ch25, sizeof(lidar_state_tmp.delay_ch25));
	strncpy(sysinfo_lidar_delay_param[18].param_value, lidar_state_tmp.delay_ch10, sizeof(lidar_state_tmp.delay_ch10));
	strncpy(sysinfo_lidar_delay_param[19].param_value, lidar_state_tmp.delay_ch26, sizeof(lidar_state_tmp.delay_ch26));
	strncpy(sysinfo_lidar_delay_param[20].param_value, lidar_state_tmp.delay_ch11, sizeof(lidar_state_tmp.delay_ch11));
	strncpy(sysinfo_lidar_delay_param[21].param_value, lidar_state_tmp.delay_ch27, sizeof(lidar_state_tmp.delay_ch27));
	strncpy(sysinfo_lidar_delay_param[22].param_value, lidar_state_tmp.delay_ch12, sizeof(lidar_state_tmp.delay_ch12));
	strncpy(sysinfo_lidar_delay_param[23].param_value, lidar_state_tmp.delay_ch28, sizeof(lidar_state_tmp.delay_ch28));
	strncpy(sysinfo_lidar_delay_param[24].param_value, lidar_state_tmp.delay_ch13, sizeof(lidar_state_tmp.delay_ch13));
	strncpy(sysinfo_lidar_delay_param[25].param_value, lidar_state_tmp.delay_ch29, sizeof(lidar_state_tmp.delay_ch29));
	strncpy(sysinfo_lidar_delay_param[26].param_value, lidar_state_tmp.delay_ch14, sizeof(lidar_state_tmp.delay_ch14));
	strncpy(sysinfo_lidar_delay_param[27].param_value, lidar_state_tmp.delay_ch30, sizeof(lidar_state_tmp.delay_ch30));
	strncpy(sysinfo_lidar_delay_param[28].param_value, lidar_state_tmp.delay_ch15, sizeof(lidar_state_tmp.delay_ch15));
	strncpy(sysinfo_lidar_delay_param[29].param_value, lidar_state_tmp.delay_ch31, sizeof(lidar_state_tmp.delay_ch31));
	strncpy(sysinfo_lidar_delay_param[30].param_value, lidar_state_tmp.delay_ch16, sizeof(lidar_state_tmp.delay_ch16));
	strncpy(sysinfo_lidar_delay_param[31].param_value, lidar_state_tmp.delay_ch32, sizeof(lidar_state_tmp.delay_ch32));

	TransFormJSON(sysinfo_lidar_delay_param, buf, 32);

	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t CalRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	cal_flag = 1;
//	xil_printf("Flash Read Failed\r\n");
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t CalRoutingDelay2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	cal2_flag = 1;
//	xil_printf("Flash Read Failed\r\n");
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t CalRoutingDelayLong(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	cal_flag_long = 1;
//	xil_printf("Flash Read Failed\r\n");
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t SaveFIRs(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "threshold1";
	GetJsonValueByKey(value, req, key);

	int saves = atoi(value);


	if(saves < 0)
	{
		saves = 0;
	}

	lidar_state_.FIR_setting.threshold1 = saves;



	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "threshold2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.threshold2 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "distance1");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.distance1 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "distance2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.distance2 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "count1");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.count1 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "count2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.count2 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "maxcount");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.max_count = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "coef1");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.FIR_COEF[1] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "coef2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.FIR_COEF[2] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "coef3");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.FIR_COEF[3] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "coef4");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.FIR_COEF[4] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "coef5");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.FIR_COEF[5] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "coef6");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting.FIR_COEF[6] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "ismafon");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	else if(saves > 10)
	{
		saves = 1;
	}
	lidar_state_.FIR_setting.ison = saves;


	//FIR end



	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}


int32_t SaveGFIRs(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{


	//vremove setting
	char value[100] = {0, };
	char key[100] = "Gthreshold1";
	GetJsonValueByKey(value, req, key);

	int saves = atoi(value);

	//Ground FIR start

	if(saves < 0)
	{
		saves = 0;
	}

	lidar_state_.FIR_setting_ground.threshold1 = saves;



	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gthreshold2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.threshold2 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gdistance1");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.distance1 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gdistance2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.distance2 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gcount1");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.count1 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gcount2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.count2 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gmaxcount");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.max_count = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gcoef1");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.FIR_COEF[1] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gcoef2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.FIR_COEF[2] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gcoef3");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.FIR_COEF[3] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gcoef4");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.FIR_COEF[4] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gcoef5");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.FIR_COEF[5] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Gcoef6");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_ground.FIR_COEF[6] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "isGmafon");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	else if(saves > 10)
	{
		saves = 1;
	}
	lidar_state_.FIR_setting_ground.ison = saves;

	//ground FIR end



	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}


int32_t SaveVFIRs(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{


	//vremove setting
	char value[100] = {0, };
	char key[100] = "Vthreshold1";
	GetJsonValueByKey(value, req, key);

	int saves = atoi(value);


	if(saves < 0)
	{
		saves = 0;
	}

	lidar_state_.FIR_setting_vremove.threshold1 = saves;



	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vthreshold2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.threshold2 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vdistance1");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.distance1 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vdistance2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.distance2 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vcount1");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.count1 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vcount2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.count2 = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vmaxcount");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.max_count = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vcoef1");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.FIR_COEF[1] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vcoef2");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.FIR_COEF[2] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vcoef3");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.FIR_COEF[3] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vcoef4");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.FIR_COEF[4] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vcoef5");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.FIR_COEF[5] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "Vcoef6");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	lidar_state_.FIR_setting_vremove.FIR_COEF[6] = saves;


	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "isVon");

	GetJsonValueByKey(value, req, key);

	saves = atoi(value);
	if(saves < 0)
	{
		saves = 0;
	}
	else if(saves > 10)
	{
		saves = 1;
	}
	lidar_state_.FIR_setting_vremove.ison = saves;


	//vremove setting end



	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}


int32_t GetFIRs(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	//240709 fir
//	web_param settings[13] = {
//						{"FIR_threshold1","0"}, {"FIR_threshold2","0"}, {"FIR_distance1","0"}, {"FIR_distance2","0"},
//						{"FIR_count1","0"}, {"FIR_count2","0"} ,{"FIR_maxcount","0"},
//						{"FIR_COEF1","0"}, {"FIR_COEF2","0"}, {"FIR_COEF3","0"},
//						{"FIR_COEF4","0"}, {"FIR_COEF5","0"}, {"FIR_COEF6","0"}};

	//240710 fir
//	web_param settings[14] = {
//						{"FIR_threshold1","0"}, {"FIR_threshold2","0"}, {"FIR_distance1","0"}, {"FIR_distance2","0"},
//						{"FIR_count1","0"}, {"FIR_count2","0"} ,{"FIR_maxcount","0"},
//						{"FIR_COEF1","0"}, {"FIR_COEF2","0"}, {"FIR_COEF3","0"},
//						{"FIR_COEF4","0"}, {"FIR_COEF5","0"}, {"FIR_COEF6","0"}, {"FIR_ismafon","0"}};

	web_param settings[42] = {
			{"FIR_Vthreshold1","0"}, {"FIR_Vthreshold2","0"}, {"FIR_Vdistance1","0"}, {"FIR_Vdistance2","0"},
			{"FIR_Vcount1","0"}, {"FIR_Vcount2","0"} ,{"FIR_Vmaxcount","0"},
			{"FIR_VCOEF1","0"}, {"FIR_VCOEF2","0"}, {"FIR_VCOEF3","0"},
			{"FIR_VCOEF4","0"}, {"FIR_VCOEF5","0"}, {"FIR_VCOEF6","0"}, {"FIR_isVon","0"},

			{"FIR_threshold1","0"}, {"FIR_threshold2","0"}, {"FIR_distance1","0"}, {"FIR_distance2","0"},
			{"FIR_count1","0"}, {"FIR_count2","0"} ,{"FIR_maxcount","0"},
			{"FIR_COEF1","0"}, {"FIR_COEF2","0"}, {"FIR_COEF3","0"},
			{"FIR_COEF4","0"}, {"FIR_COEF5","0"}, {"FIR_COEF6","0"}, {"FIR_ismafon","0"},

			{"FIR_Gthreshold1","0"}, {"FIR_Gthreshold2","0"}, {"FIR_Gdistance1","0"}, {"FIR_Gdistance2","0"},
			{"FIR_Gcount1","0"}, {"FIR_Gcount2","0"} ,{"FIR_Gmaxcount","0"},
			{"FIR_GCOEF1","0"}, {"FIR_GCOEF2","0"}, {"FIR_GCOEF3","0"},
			{"FIR_GCOEF4","0"}, {"FIR_GCOEF5","0"}, {"FIR_GCOEF6","0"}, {"FIR_isGmafon","0"} };



	itoa(lidar_state_.FIR_setting_vremove.threshold1, settings[0].param_value, 10);
	itoa(lidar_state_.FIR_setting_vremove.threshold2, settings[1].param_value, 10);

	itoa(lidar_state_.FIR_setting_vremove.distance1, settings[2].param_value, 10);
	itoa(lidar_state_.FIR_setting_vremove.distance2, settings[3].param_value, 10);

	itoa(lidar_state_.FIR_setting_vremove.count1, settings[4].param_value, 10);
	itoa(lidar_state_.FIR_setting_vremove.count2, settings[5].param_value, 10);

	itoa(lidar_state_.FIR_setting_vremove.max_count, settings[6].param_value, 10);

	itoa(lidar_state_.FIR_setting_vremove.FIR_COEF[1], settings[7].param_value, 10);
	itoa(lidar_state_.FIR_setting_vremove.FIR_COEF[2], settings[8].param_value, 10);
	itoa(lidar_state_.FIR_setting_vremove.FIR_COEF[3], settings[9].param_value, 10);
	itoa(lidar_state_.FIR_setting_vremove.FIR_COEF[4], settings[10].param_value, 10);
	itoa(lidar_state_.FIR_setting_vremove.FIR_COEF[5], settings[11].param_value, 10);
	itoa(lidar_state_.FIR_setting_vremove.FIR_COEF[6], settings[12].param_value, 10);

	itoa(lidar_state_.FIR_setting_vremove.ison, settings[13].param_value, 10);
	//v end


	itoa(lidar_state_.FIR_setting.threshold1, settings[14].param_value, 10);
	itoa(lidar_state_.FIR_setting.threshold2, settings[15].param_value, 10);

	itoa(lidar_state_.FIR_setting.distance1, settings[16].param_value, 10);
	itoa(lidar_state_.FIR_setting.distance2, settings[17].param_value, 10);

	itoa(lidar_state_.FIR_setting.count1, settings[18].param_value, 10);
	itoa(lidar_state_.FIR_setting.count2, settings[19].param_value, 10);

	itoa(lidar_state_.FIR_setting.max_count, settings[20].param_value, 10);

	itoa(lidar_state_.FIR_setting.FIR_COEF[1], settings[21].param_value, 10);
	itoa(lidar_state_.FIR_setting.FIR_COEF[2], settings[22].param_value, 10);
	itoa(lidar_state_.FIR_setting.FIR_COEF[3], settings[23].param_value, 10);
	itoa(lidar_state_.FIR_setting.FIR_COEF[4], settings[24].param_value, 10);
	itoa(lidar_state_.FIR_setting.FIR_COEF[5], settings[25].param_value, 10);
	itoa(lidar_state_.FIR_setting.FIR_COEF[6], settings[26].param_value, 10);

	itoa(lidar_state_.FIR_setting.ison, settings[27].param_value, 10);
	//fir end

	itoa(lidar_state_.FIR_setting_ground.threshold1, settings[28].param_value, 10);
	itoa(lidar_state_.FIR_setting_ground.threshold2, settings[29].param_value, 10);

	itoa(lidar_state_.FIR_setting_ground.distance1, settings[30].param_value, 10);
	itoa(lidar_state_.FIR_setting_ground.distance2, settings[31].param_value, 10);

	itoa(lidar_state_.FIR_setting_ground.count1, settings[32].param_value, 10);
	itoa(lidar_state_.FIR_setting_ground.count2, settings[33].param_value, 10);

	itoa(lidar_state_.FIR_setting_ground.max_count, settings[34].param_value, 10);

	itoa(lidar_state_.FIR_setting_ground.FIR_COEF[1], settings[35].param_value, 10);
	itoa(lidar_state_.FIR_setting_ground.FIR_COEF[2], settings[36].param_value, 10);
	itoa(lidar_state_.FIR_setting_ground.FIR_COEF[3], settings[37].param_value, 10);
	itoa(lidar_state_.FIR_setting_ground.FIR_COEF[4], settings[38].param_value, 10);
	itoa(lidar_state_.FIR_setting_ground.FIR_COEF[5], settings[39].param_value, 10);
	itoa(lidar_state_.FIR_setting_ground.FIR_COEF[6], settings[40].param_value, 10);

	itoa(lidar_state_.FIR_setting_ground.ison, settings[41].param_value, 10);



//	TransFormJSON(settings, buf, 13);
//	TransFormJSON(settings, buf, 14);
	TransFormJSON(settings, buf, 42);

	if(tcp_write(pcb, buf, 1024, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}


int32_t GetMinimumDetctions(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{

	web_param sysinfo_param[5] = {
						{"minimumdetectrange", "0"}, {"minimumdetectbotstart", "0"}, {"minimumdetecttopstart", "0"},
						{"minimumdetectbotend", "0"}, {"minimumdetecttopend", "0"}};


	itoa(lidar_state_.calibration_setting_.minimum_detection_range, sysinfo_param[0].param_value, 10);

	itoa(lidar_state_.calibration_setting_.minimum_detection_range_start1, sysinfo_param[1].param_value, 10);

	itoa(lidar_state_.calibration_setting_.minimum_detection_range_start2, sysinfo_param[2].param_value, 10);

	itoa(lidar_state_.calibration_setting_.minimum_detection_range_stop1, sysinfo_param[3].param_value, 10);

	itoa(lidar_state_.calibration_setting_.minimum_detection_range_stop2, sysinfo_param[4].param_value, 10);


	TransFormJSON(sysinfo_param, buf, 5);

	if(tcp_write(pcb, buf, 1024, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}


int32_t FixedCalRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	fixed_cal_flag = 1;

	char value[100] = {0, };
	char key[100] = "FixedDistance";
	GetJsonValueByKey(value, req, key);

	cal_distance = atoi(value);

	if(cal_distance <= 0 || cal_distance > 30)
	{
		fixed_cal_flag = 0;

	}


//	xil_printf("Flash Read Failed\r\n");
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t AddRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	routing_add_flag = 1;
	char value[100] = {0, };
	char key[100] = "AddRoutingDelay";
	GetJsonValueByKey(value, req, key);

	adddelay = atoi(value);

//	xil_printf("Flash Read Failed\r\n");
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t AddRoutingDelay2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	routing_add_flag2 = 1;
	char value[100] = {0, };
	char key[100] = "AddRoutingDelay";
	GetJsonValueByKey(value, req, key);

	adddelay = atoi(value);

//	xil_printf("Flash Read Failed\r\n");
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}


int32_t SetIntenVth(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{

	char value[100] = {0, };
	char key[100] = "IntensityVth";
	GetJsonValueByKey(value, req, key);

	inten_filter = atoi(value);

//	xil_printf("Flash Read Failed\r\n");
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

}


int32_t SetRegWrite(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{

	char value[100] = {0, };
	char key[100] = "RegAddr";
	GetJsonValueByKey(value, req, key);

	char value2[100] = {0, };
	char key2[100] = "RegVal";
	GetJsonValueByKey(value2, req, key2);

	uint32_t regaddr = strtoul(value, NULL, 16);
	uint32_t regval = strtoul(value2, NULL, 16);

	Xil_Out32(regaddr, regval);

//	adddelay = atoi(value);

//	xil_printf("Flash Read Failed\r\n");
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, sizeof(buf), 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t GetRegRead(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char buf_[PACKET_SIZE];

	char value[10] = {0, };
	char key[100] = "RegAddr";
	GetJsonValueByKey(value, req, key);
	uint32_t regaddr = strtoul(value, NULL, 16);
	uint32_t reg_read = Xil_In32(regaddr);

	web_param reg_read_param[1] = { {"RegVal", "0"}};

	sprintf(reg_read_param[0].param_value, "0x%x", reg_read);

//	itoa(current_vth_offset, reg_read_param[0].param_value, 10);

	TransFormJSON(reg_read_param, buf_, 1);

	if(tcp_write(pcb, buf_, 150, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf_);
		return -1;
	}

	return 0;
}

int32_t GetThresholdVoltage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	web_param sysinfo_lidar_th_param[16] = { {"ch1", "0"}, {"ch2", "0"}, {"ch3", "0"}, {"ch4", "0"}, {"ch5", "0"}, {"ch6", "0"}, {"ch7", "0"}, {"ch8",
			"0"}, {"ch9", "0"}, {"ch10", "0"}, {"ch11", "0"}, {"ch12", "0"}, {"ch13", "0"}, {"ch14", "0"}, {"ch15", "0"}, {"ch16", "0"}, };
	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
	struct LidarState_Ch lidar_state_tmp;
	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_tmp, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	strncpy(sysinfo_lidar_th_param[0].param_value, lidar_state_tmp.th_ch1, sizeof(lidar_state_tmp.th_ch1));
	strncpy(sysinfo_lidar_th_param[1].param_value, lidar_state_tmp.th_ch2, sizeof(lidar_state_tmp.th_ch2));
	strncpy(sysinfo_lidar_th_param[2].param_value, lidar_state_tmp.th_ch3, sizeof(lidar_state_tmp.th_ch3));
	strncpy(sysinfo_lidar_th_param[3].param_value, lidar_state_tmp.th_ch4, sizeof(lidar_state_tmp.th_ch4));
	strncpy(sysinfo_lidar_th_param[4].param_value, lidar_state_tmp.th_ch5, sizeof(lidar_state_tmp.th_ch5));
	strncpy(sysinfo_lidar_th_param[5].param_value, lidar_state_tmp.th_ch6, sizeof(lidar_state_tmp.th_ch6));
	strncpy(sysinfo_lidar_th_param[6].param_value, lidar_state_tmp.th_ch7, sizeof(lidar_state_tmp.th_ch7));
	strncpy(sysinfo_lidar_th_param[7].param_value, lidar_state_tmp.th_ch8, sizeof(lidar_state_tmp.th_ch8));
	strncpy(sysinfo_lidar_th_param[8].param_value, lidar_state_tmp.th_ch9, sizeof(lidar_state_tmp.th_ch9));
	strncpy(sysinfo_lidar_th_param[9].param_value, lidar_state_tmp.th_ch10, sizeof(lidar_state_tmp.th_ch10));
	strncpy(sysinfo_lidar_th_param[10].param_value, lidar_state_tmp.th_ch11, sizeof(lidar_state_tmp.th_ch11));
	strncpy(sysinfo_lidar_th_param[11].param_value, lidar_state_tmp.th_ch12, sizeof(lidar_state_tmp.th_ch12));
	strncpy(sysinfo_lidar_th_param[12].param_value, lidar_state_tmp.th_ch13, sizeof(lidar_state_tmp.th_ch13));
	strncpy(sysinfo_lidar_th_param[13].param_value, lidar_state_tmp.th_ch14, sizeof(lidar_state_tmp.th_ch14));
	strncpy(sysinfo_lidar_th_param[14].param_value, lidar_state_tmp.th_ch15, sizeof(lidar_state_tmp.th_ch15));
	strncpy(sysinfo_lidar_th_param[15].param_value, lidar_state_tmp.th_ch16, sizeof(lidar_state_tmp.th_ch16));

	TransFormJSON(sysinfo_lidar_th_param, buf, 16);

	if(tcp_write(pcb, buf, 512, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t SetRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	if(strstr(req, "json=") == NULL)
	{
		e_post_cmd_recv_status = CMD_PARSED;
		is_routing_delay_1_cmd = 1;

		char *ptr_file_size = strstr(req, "Content-Length: ");
		ptr_file_size += strlen("Content-Length: ");
		ptr_file_size = strtok(ptr_file_size, "\r");
		//				ptr_file_size = strtok(NULL, "\r");
		//				ptr_file_size = strtok(NULL, "\n");
		post_cmd_data_size = atoi(ptr_file_size);
		xil_printf("recv file size : %d\r\n", recv_file_size);

		return -1;
	}

	char value11[30] = {0, };
	char key11[30] = "ch1";
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
	strncpy(lidar_state_Ch.delay_ch1, value11, sizeof(lidar_state_Ch.delay_ch1));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch3");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
	strncpy(lidar_state_Ch.delay_ch2, value11, sizeof(lidar_state_Ch.delay_ch2));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch5");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
	strncpy(lidar_state_Ch.delay_ch3, value11, sizeof(lidar_state_Ch.delay_ch3));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch7");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
	strncpy(lidar_state_Ch.delay_ch4, value11, sizeof(lidar_state_Ch.delay_ch4));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch9");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
	strncpy(lidar_state_Ch.delay_ch5, value11, sizeof(lidar_state_Ch.delay_ch5));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch11");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
	strncpy(lidar_state_Ch.delay_ch6, value11, sizeof(lidar_state_Ch.delay_ch6));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch13");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
	strncpy(lidar_state_Ch.delay_ch7, value11, sizeof(lidar_state_Ch.delay_ch7));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch15");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
	strncpy(lidar_state_Ch.delay_ch8, value11, sizeof(lidar_state_Ch.delay_ch8));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch17");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
	strncpy(lidar_state_Ch.delay_ch9, value11, sizeof(lidar_state_Ch.delay_ch9));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch19");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
	strncpy(lidar_state_Ch.delay_ch10, value11, sizeof(lidar_state_Ch.delay_ch10));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch21");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
	strncpy(lidar_state_Ch.delay_ch11, value11, sizeof(lidar_state_Ch.delay_ch11));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch23");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
	strncpy(lidar_state_Ch.delay_ch12, value11, sizeof(lidar_state_Ch.delay_ch12));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch25");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
	strncpy(lidar_state_Ch.delay_ch13, value11, sizeof(lidar_state_Ch.delay_ch13));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch27");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
	strncpy(lidar_state_Ch.delay_ch14, value11, sizeof(lidar_state_Ch.delay_ch14));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch29");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
	strncpy(lidar_state_Ch.delay_ch15, value11, sizeof(lidar_state_Ch.delay_ch15));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch31");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
	strncpy(lidar_state_Ch.delay_ch16, value11, sizeof(lidar_state_Ch.delay_ch16));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch2");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
	strncpy(lidar_state_Ch.delay_ch17, value11, sizeof(lidar_state_Ch.delay_ch17));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch4");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
	strncpy(lidar_state_Ch.delay_ch18, value11, sizeof(lidar_state_Ch.delay_ch18));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch6");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
	strncpy(lidar_state_Ch.delay_ch19, value11, sizeof(lidar_state_Ch.delay_ch19));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch8");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
	strncpy(lidar_state_Ch.delay_ch20, value11, sizeof(lidar_state_Ch.delay_ch20));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch10");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
	strncpy(lidar_state_Ch.delay_ch21, value11, sizeof(lidar_state_Ch.delay_ch21));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch12");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
	strncpy(lidar_state_Ch.delay_ch22, value11, sizeof(lidar_state_Ch.delay_ch22));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch14");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
	strncpy(lidar_state_Ch.delay_ch23, value11, sizeof(lidar_state_Ch.delay_ch23));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch16");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
	strncpy(lidar_state_Ch.delay_ch24, value11, sizeof(lidar_state_Ch.delay_ch24));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch18");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
	strncpy(lidar_state_Ch.delay_ch25, value11, sizeof(lidar_state_Ch.delay_ch25));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch20");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
	strncpy(lidar_state_Ch.delay_ch26, value11, sizeof(lidar_state_Ch.delay_ch26));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch22");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
	strncpy(lidar_state_Ch.delay_ch27, value11, sizeof(lidar_state_Ch.delay_ch27));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch24");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
	strncpy(lidar_state_Ch.delay_ch28, value11, sizeof(lidar_state_Ch.delay_ch28));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch26");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
	strncpy(lidar_state_Ch.delay_ch29, value11, sizeof(lidar_state_Ch.delay_ch29));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch28");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
	strncpy(lidar_state_Ch.delay_ch30, value11, sizeof(lidar_state_Ch.delay_ch30));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch30");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
	strncpy(lidar_state_Ch.delay_ch31, value11, sizeof(lidar_state_Ch.delay_ch31));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch32");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
	strncpy(lidar_state_Ch.delay_ch32, value11, sizeof(lidar_state_Ch.delay_ch32));
	////////////////////////////////////////////////////////////////////////////////////

	SetRoutingDelayToLidarState(&lidar_state_, lidar_state_Ch);

	EraseLidarInfo();
	int Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_, 0x00, LIDAR_STATE_SIZE);
	Status = ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
	memcpy(&lidar_state_, lidar_state_buffer_, sizeof(struct LidarState));
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*)&lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}


	EraseLidarInfo_Ch2();
	Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*)&lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH2);
	Status = ReadLidarInfo_Ch2(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH2);
	memcpy(&lidar_state_Ch2, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}


	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t SetThresholdVoltage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}

	char value11[30] = {0, };
	char key11[30] = "ch1";
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch1, 0x00, sizeof(lidar_state_Ch.th_ch1));
	strncpy(lidar_state_Ch.th_ch1, value11, sizeof(lidar_state_Ch.th_ch1));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch2");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch2, 0x00, sizeof(lidar_state_Ch.th_ch2));
	strncpy(lidar_state_Ch.th_ch2, value11, sizeof(lidar_state_Ch.th_ch2));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch3");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch3, 0x00, sizeof(lidar_state_Ch.th_ch3));
	strncpy(lidar_state_Ch.th_ch3, value11, sizeof(lidar_state_Ch.th_ch3));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch4");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch4, 0x00, sizeof(lidar_state_Ch.th_ch4));
	strncpy(lidar_state_Ch.th_ch4, value11, sizeof(lidar_state_Ch.th_ch4));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch5");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch5, 0x00, sizeof(lidar_state_Ch.th_ch5));
	strncpy(lidar_state_Ch.th_ch5, value11, sizeof(lidar_state_Ch.th_ch5));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch6");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch6, 0x00, sizeof(lidar_state_Ch.th_ch6));
	strncpy(lidar_state_Ch.th_ch6, value11, sizeof(lidar_state_Ch.th_ch6));

	memset(value11, 0x00, sizeof(value11));

	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch7");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch7, 0x00, sizeof(lidar_state_Ch.th_ch7));
	strncpy(lidar_state_Ch.th_ch7, value11, sizeof(lidar_state_Ch.th_ch7));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch8");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch8, 0x00, sizeof(lidar_state_Ch.th_ch8));
	strncpy(lidar_state_Ch.th_ch8, value11, sizeof(lidar_state_Ch.th_ch8));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch9");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch9, 0x00, sizeof(lidar_state_Ch.th_ch9));
	strncpy(lidar_state_Ch.th_ch9, value11, sizeof(lidar_state_Ch.th_ch9));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch10");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch10, 0x00, sizeof(lidar_state_Ch.th_ch10));
	strncpy(lidar_state_Ch.th_ch10, value11, sizeof(lidar_state_Ch.th_ch10));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch11");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch11, 0x00, sizeof(lidar_state_Ch.th_ch11));
	strncpy(lidar_state_Ch.th_ch11, value11, sizeof(lidar_state_Ch.th_ch11));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch12");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch12, 0x00, sizeof(lidar_state_Ch.th_ch12));
	strncpy(lidar_state_Ch.th_ch12, value11, sizeof(lidar_state_Ch.th_ch12));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch13");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch13, 0x00, sizeof(lidar_state_Ch.th_ch13));
	strncpy(lidar_state_Ch.th_ch13, value11, sizeof(lidar_state_Ch.th_ch13));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch14");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch14, 0x00, sizeof(lidar_state_Ch.th_ch14));
	strncpy(lidar_state_Ch.th_ch14, value11, sizeof(lidar_state_Ch.th_ch14));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch15");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch15, 0x00, sizeof(lidar_state_Ch.th_ch15));
	strncpy(lidar_state_Ch.th_ch15, value11, sizeof(lidar_state_Ch.th_ch15));

	memset(value11, 0x00, sizeof(value11));
	memset(key11, 0x00, sizeof(key11));
	strcat(key11, "ch16");
	GetJsonValueByKey(value11, req, key11);
	memset(lidar_state_Ch.th_ch16, 0x00, sizeof(lidar_state_Ch.th_ch16));
	strncpy(lidar_state_Ch.th_ch16, value11, sizeof(lidar_state_Ch.th_ch16));

	EraseLidarInfo();
	Status = LinearQspiFlashWriteForLidarInfo((char*)&lidar_state_, LIDAR_STATE_SIZE);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch();
	Status = LinearQspiFlashWriteForLidarInfo_Ch((char*)&lidar_state_Ch, LIDAR_STATE_SIZE_CH);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH);
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH);
	memcpy(&lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	EraseLidarInfo_Ch2();
	Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*)&lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Write Failed\r\n");
		return XST_FAILURE;
	}

	memset(lidar_state_buffer_Ch_, 0x00, LIDAR_STATE_SIZE_CH2);
	Status = ReadLidarInfo_Ch2(lidar_state_buffer_Ch_, LIDAR_STATE_SIZE_CH2);
	memcpy(&lidar_state_Ch2, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
}

int32_t SetRegisterValue(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	u32 register_offset = 0;
	u32 register_value = 0;
	char register_name[100];

	char value[100] = {0, };
	char key[100] = "Name";
	GetJsonValueByKey(value, req, key);
	memcpy(register_name, value, sizeof(value));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "offset");
	GetJsonValueByKey(value, req, key);
	register_offset = atoi(value);

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "value");
	GetJsonValueByKey(value, req, key);
	register_value = atoi(value);

	if(register_name[0] != 0 && register_offset % 4 == 0)
		SetHwIpRegisterValue(register_name, register_offset, register_value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t GetDtAilgnInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "dtalign";
	GetJsonValueByKey(value, req, key);

	dt_align_info_.start_vth = atoi(value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}

int32_t GetRegisterValue(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char buf[PACKET_SIZE];
	u32 register_offset = 0;
	u32 register_value = 0;
	char register_name[100];

	char value[100] = {0, };
	char key[100] = "Name";
	GetJsonValueByKey(value, req, key);
	memcpy(register_name, value, sizeof(value));

	memset(value, 0x00, sizeof(value));
	memset(key, 0x00, sizeof(key));
	strcat(key, "offset");
	GetJsonValueByKey(value, req, key);
	register_offset = atoi(value);

	if(register_name[0] != 0 && register_offset % 4 == 0)
		register_value = GetHwIpRegisterValue(register_name, register_offset);

	web_param register_value_param[1] = { {"RegisterValue", "0"}};

	itoa(register_value, register_value_param[0].param_value, 10);

	TransFormJSON(register_value_param, buf, 1);

	if(tcp_write(pcb, buf, 150, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	return 0;
}

int32_t GetLogMessage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{

//	char buf[PACKET_SIZE];
//	char buf[2700];
//	char buf[2850];	//log100
//	char buf[1500];	//log 50
//	char buf[1600];	//log 50
	char buf[1800] = {0x0,};	//log 50
//	char lbuf[13] = "TestTestTest";
//	TransFormJSON(hwinfo_param, buf, 18); // test
//	TransFormJSON("TesteTestTest", buf, 18); // test
//	generate_http_header(buf, NULL, 0);
	strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: ");
	strcat(buf, "text/html"); /* for unknown types */
	strcat(buf, "\r\n");

//	sprintf(lbuf, "Content-length: %d", 12);
//	strcat(buf, lbuf);
//	strcat(buf, "Content-length: 28");
//	strcat(buf, "Content-length: 2700");	//log 100
//	strcat(buf, "Content-length: 1350");	//log 50
//	strcat(buf, "Content-length: 1450");	//log 50
	strcat(buf, "Content-length: 1650");	//log 50
	strcat(buf, "\r\n");

//	strcat(buf, "000000000000");	//data

	strcat(buf, "Connection: close\r\n");
	strcat(buf, "\r\n");


//	for(int i = 0; i<100; i++)
//	for(int i = 0; i<50; i++)	//change log
	for(int i = 0; i<50; i++)	//change log
//		for(int i = 0; i<255; i++)
	{
//		strcat(buf, "[HW_SAVE]20240115_08:00");	//data
//		strcat(buf, "%5BHW_SAVE%5D20240118_11%3A06");	//data
		unsigned char ReadVer[32] = {0x0,};

		LinearQspiFlashReadForLidarChangeInfo(ReadVer, i);
//		strcat(buf, ReadVer);	//data
		if(ReadVer[0] == 0xff)
		{
			usleep(1);
			continue;
		}
		strncat(buf, ReadVer, 29);	//data
		strcat(buf, "<br>");
		usleep(1);
	}

//	strcat(buf, "TestTestTest");	//data
//	strcat(buf, "<br>");
//	strcat(buf, "GoodGoodGood");	//data

//	if(tcp_write(pcb, buf, 150, 1) != ERR_OK)
//	if(tcp_write(pcb, buf, 2850, 1) != ERR_OK)
//	if(tcp_write(pcb, buf, 1500, 1) != ERR_OK)
	if(tcp_write(pcb, buf, 1800, 1) != ERR_OK)
//		if(tcp_write(pcb, buf, 1600, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	return 0;


	char result[LOG_ADDR_INTERVAL * 100] = {0,};
	read_logs(result, 100);
	int BUFSIZE = 1000;
	char* result_ptr;

	signed int fsize, hlen, n;
	err_t err;
	size_t log_message_len = strlen(result);

	fsize = log_message_len;

	hlen = generate_http_header((char *)buf, "htm", fsize);
//	strncat(buf, result, fsize);
	if((err = tcp_write(pcb, buf, hlen, 1)) != ERR_OK)
	{
		xil_printf("error (%d) writing http header to socket\r\n", err);
		xil_printf("attempted to write #bytes = %d, tcp_sndbuf = %d\r\n", hlen, tcp_sndbuf(pcb));
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	result_ptr = result;
	n = BUFSIZE;

	while(fsize > 0)
	{
		int sndbuf;
		sndbuf = tcp_sndbuf(pcb);

		if(sndbuf < BUFSIZE)
		{
			/* not enough space in sndbuf, so send remaining bytes when there is space */
			/* this is done by storing the fd in as part of the tcp_arg, so that the sent
			 callback handler knows to send data */
			http_arg *a = (http_arg *)pcb->callback_arg;
//			a->fd = fd;
			a->fsize = fsize;
			return -1;
		}

		if(fsize < n)
			n = fsize;
//		n = mfs_file_read(fd, (char *)buf, BUFSIZE);

		if((err = tcp_write(pcb, result_ptr, n, 1)) != ERR_OK)
		{
//			xil_printf("error writing file (%s) to socket, remaining unwritten bytes = %d\r\n", filename, fsize - n);
			xil_printf("attempted to lwip_write %d bytes, tcp write error = %d\r\n", n, err);
			break;
		}
		if(fsize >= n)
		{
			fsize -= n;
			result_ptr += n;
		}
		else
		{
			fsize = 0;
		}
	}
	return 0;
}

int32_t SendDarkAreaChannels(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "Channels";
	GetJsonValueByKey(value, req, key);

	char *tok_ptr = strtok(value, "+");

	size_t i = 0;
	for(size_t j = 0 ; j < 32 ; j++)
	{
		lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_channel_[j] = 0;
	}
	lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_channel_count_ = 0;

	while (tok_ptr != NULL)
	{
		lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_channel_[i++] = atoi(tok_ptr);
		lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_channel_count_++;
		tok_ptr = strtok(NULL, "+");
	}

	lidar_state_.filter_setting_.dark_area_error_correction_setting_.is_dark_area_channel_changed_ = true;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;

}

int32_t SendDarkAreaCorrectionParameter1(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "Parameter";
	GetJsonValueByKey(value, req, key);

	lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_error_cor_param1_ = atof(value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;

}

int32_t SendDarkAreaCorrectionParameter2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "Parameter";
	GetJsonValueByKey(value, req, key);

	lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_error_cor_param2_ = atof(value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;

}

int32_t SendDarkAreaCorrectionParameter3(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "Parameter";
	GetJsonValueByKey(value, req, key);

	lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_error_cor_param3_ = atof(value);
	dark_area_state_.darkarea_setting_.dark_area_error_cor_param3_ = atof(value);
	dark_area_state_.darkarea_setting_2.dark_area_error_cor_param3_ = atof(value);
	dark_area_state_.darkarea_setting_3.dark_area_error_cor_param3_ = atof(value);

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;

}

int32_t SendDarkAreaCorrectionParameter4(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "Parameter";
	GetJsonValueByKey(value, req, key);



	lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_error_cor_param4_ = atof(value);
	dark_area_state_.darkarea_setting_.dark_area_error_cor_param4_ = atof(value);
	dark_area_state_.darkarea_setting_2.dark_area_error_cor_param4_ = atof(value);
	dark_area_state_.darkarea_setting_3.dark_area_error_cor_param4_ = atof(value);
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;

}

int32_t SaveTargetChannelDistanceRatio(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "ChannelDistanceRatio";
	GetJsonValueByKey(value, req, key);

	char *tok_ptr = strtok(value, "+");

	char value2[100] = {0, };
	char key2[100] = "GroupNum";
	GetJsonValueByKey(value2, req, key2);
	uint8_t selected_val = atoi(value2);

	DarkAreaErrorCorrectionSetting2 *target_group = &dark_area_state_.darkarea_setting_;
	if(selected_val == 1)
	{
		target_group = &dark_area_state_.darkarea_setting_;
	}
	else if(selected_val == 2)
	{
		target_group = &dark_area_state_.darkarea_setting_2;
	}
	else if(selected_val == 3)
	{
		target_group = &dark_area_state_.darkarea_setting_3;
	}

	size_t i = 0;
	for(size_t j = 0 ; j < 32 ; j++)
	{
		target_group->dark_area_error_cor_param1_[j] = 0.;

	}


	while (tok_ptr != NULL)
	{
		target_group->dark_area_error_cor_param1_[i++] = atof(tok_ptr);



		tok_ptr = strtok(NULL, "+");
	}



	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;
}
int32_t SaveCorrectedDistanceRatio(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "CorrectedDistanceRatio";
	GetJsonValueByKey(value, req, key);

	char *tok_ptr = strtok(value, "+");

	char value2[100] = {0, };
	char key2[100] = "GroupNum";
	GetJsonValueByKey(value2, req, key2);
	uint8_t selected_val = atoi(value2);

	DarkAreaErrorCorrectionSetting2 *target_group = &dark_area_state_.darkarea_setting_;
	if(selected_val == 1)
	{
		target_group = &dark_area_state_.darkarea_setting_;
	}
	else if(selected_val == 2)
	{
		target_group = &dark_area_state_.darkarea_setting_2;
	}
	else if(selected_val == 3)
	{
		target_group = &dark_area_state_.darkarea_setting_3;
	}

	size_t i = 0;
	for(size_t j = 0 ; j < 32 ; j++)
	{
		target_group->dark_area_error_cor_param2_[j] = 0.;
	}


	while (tok_ptr != NULL)
	{
		target_group->dark_area_error_cor_param2_[i++] = atof(tok_ptr);

		tok_ptr = strtok(NULL, "+");
	}



	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;
}
int32_t SaveDarkAreaSearchChannel(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char value[100] = {0, };
	char key[100] = "DarkAreaSearchChannel";
	GetJsonValueByKey(value, req, key);
	char *saveptr1, *saveptr2;

	char *tok_ptr = strtok_r(value, "z",&saveptr1);

	char value2[100] = {0, };
	char key2[100] = "GroupNum";
	GetJsonValueByKey(value2, req, key2);
	uint8_t selected_val = atoi(value2);

	DarkAreaErrorCorrectionSetting2 *target_group = &dark_area_state_.darkarea_setting_;
	if(selected_val == 1)
	{
		target_group = &dark_area_state_.darkarea_setting_;
	}
	else if(selected_val == 2)
	{
		target_group = &dark_area_state_.darkarea_setting_2;
	}
	else if(selected_val == 3)
	{
		target_group = &dark_area_state_.darkarea_setting_3;
	}

	size_t i = 0;
	size_t ii = 0;
	for(size_t j = 0 ; j < 32 ; j++)
	{
		for(size_t jj = 0; jj< 16; jj++)
		{
			target_group->dark_area_search_channel_[j][jj] = 0.;
		}
	}
	target_group->dark_area_channel_count_ = 0;

	while (tok_ptr != NULL)
	{
		char *sub_tok_ptr = strtok_r(tok_ptr,"+",&saveptr2);
		ii = 0;
		while(sub_tok_ptr != NULL)
		{
			target_group->dark_area_search_channel_[i][ii++] = atof(sub_tok_ptr);
			target_group->dark_area_channel_count_++;
			sub_tok_ptr = strtok_r(NULL,"+",&saveptr2);
		}
		tok_ptr = strtok_r(NULL, "z", &saveptr1);
		i++;
	}

	target_group->is_dark_area_channel_changed_ = true;

	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;
}

int32_t SaveAzimuthRange(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{


	char value[100] = {0, };
	char key[100] = "AzimuthRange";
	GetJsonValueByKey(value, req, key);



	char *tok_ptr = strtok(value, "+");
	int i = 0;

	char value2[100] = {0, };
	char key2[100] = "GroupNum";
	GetJsonValueByKey(value2, req, key2);
	uint8_t selected_val = atoi(value2);

	DarkAreaErrorCorrectionSetting2 *target_group = &dark_area_state_.darkarea_setting_;
	if(selected_val == 1)
	{
		target_group = &dark_area_state_.darkarea_setting_;
	}
	else if(selected_val == 2)
	{
		target_group = &dark_area_state_.darkarea_setting_2;
	}
	else if(selected_val == 3)
	{
		target_group = &dark_area_state_.darkarea_setting_3;
	}
	while (tok_ptr != NULL)
	{
		if(i== 0)
		{
			float start_ragne = atof(tok_ptr);
			if(start_ragne < 0) start_ragne = 0;
			target_group->azimuth_range[i++] = (uint16_t)start_ragne;
		}
		else if(i== 1)
		{
			uint16_t end_ragne = (uint16_t)atof(tok_ptr);
			if(end_ragne > target_group->total_azimuth_cnt-1) end_ragne = target_group->total_azimuth_cnt-1;
			target_group->azimuth_range[i++] = end_ragne;
		}


		tok_ptr = strtok(NULL, "+");
	}

	if(tcp_write(pcb, buf, 1024, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;
}
int32_t GetDarkAreaCorrectionSettingParams(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
///uint8_t is_enable_dark_area_crror_correction_;		// 1
//uint8_t dark_area_search_channel_[32][16];			// 512
//uint8_t dark_area_channel_count_;					// 1
//uint8_t is_dark_area_channel_changed_;				// 1
//float dark_area_error_cor_param1_[32];				// 128
//float dark_area_error_cor_param2_[32];				// 128
//float dark_area_error_cor_param3_;					// 8
//float dark_area_error_cor_param4_;					// 8
//uint8_t is_enable_zero_dist_cor_;					// 1
//uint16_t total_azimuth_cnt;							// 2
//uint16_t azimuth_group[2];							// 4

	char value[100] = {0, };
	char key[100] = "SeletedGroup";
	GetJsonValueByKey(value, req, key);

	uint8_t selected_val = atoi(value);

	web_param settings[10] = {
			{"EnableCorrection", "0"},
			{"EnableZeroDistCorrection", "0"},
			{"TargetChannelDistanceRatioParams", ""},
			{"CorrectedDistanceRatioParams", ""},
			{"DarkAreaSearchChannelParams1", ""},
			{"DarkAreaSearchChannelParams2", ""},
			{"Parameter3", "0"},
			{"Parameter4", "0"},
			{"TotalAzimuthCount","0"},
			{"AzimuthRange","0"}

	};


	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
	Status = ReadDarkAreaInfo((u8*)&dark_area_state_, DARK_AREA_SIZE);
	if(dark_area_state_.darkarea_setting_.is_enable_dark_area_crror_correction_ > 2)
	{
		memset(&dark_area_state_,0x00,sizeof(struct DarkAreaErrorCorrectionSettingState));
	}

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
		return XST_FAILURE;
	}
	DarkAreaErrorCorrectionSetting2 *target_group = &dark_area_state_.darkarea_setting_;

	if(selected_val == 1)
	{
		target_group = &dark_area_state_.darkarea_setting_;
	}
	else if(selected_val == 2)
	{
		target_group = &dark_area_state_.darkarea_setting_2;
	}
	else if(selected_val == 3)
	{
		target_group = &dark_area_state_.darkarea_setting_3;
	}

	itoa(target_group->is_enable_dark_area_crror_correction_, settings[0].param_value, 10);
	itoa(target_group->is_enable_zero_dist_cor_, settings[1].param_value, 10);
	for(size_t i = 0 ; i < 32; i++)
	{
		char str1[10] = {0,};
//		sprintf(settings[6].param_value, "%f", lidar_state_.filter_setting_.dark_area_error_correction_setting_.dark_area_error_cor_param4_);
//		sprintf(str1, "%.2f", lidar_state_.darkarea_setting_.dark_area_error_cor_param1_[i]);
		itoa(target_group->dark_area_error_cor_param1_[i], str1, 10);
		strcat(settings[2].param_value, str1);
		strcat(settings[2].param_value, " ");

		char str2[10] = {0,};
//		sprintf(str2, "%.2f", lidar_state_.darkarea_setting_.dark_area_error_cor_param2_[i]);
		itoa(target_group->dark_area_error_cor_param2_[i], str2, 10);
		strcat(settings[3].param_value, str2);
		strcat(settings[3].param_value, " ");

	}

	for(size_t i = 0 ; i < 32; i++)
	{
		uint8_t target_settings_arr_idx = (i < 16)? 4:5;
		for(size_t j = 0; j < 16; j++)
		{
			char str1[10] = {0,};

			itoa(target_group->dark_area_search_channel_[i][j], str1, 10);


			if(j!=15 && j!=0 && str1[0]!='0')
				strcat(settings[target_settings_arr_idx].param_value, " ");
			if(j!=0 && str1[0]=='0') break;
			strcat(settings[target_settings_arr_idx].param_value, str1);



		}
		if(i!=31)
			strcat(settings[target_settings_arr_idx].param_value,"z");
	}
	sprintf(settings[6].param_value, "%f", target_group->dark_area_error_cor_param3_);
	sprintf(settings[7].param_value, "%f", target_group->dark_area_error_cor_param4_);

	itoa(target_group->total_azimuth_cnt, settings[8].param_value, 10);
	sprintf(settings[9].param_value, "%d %d", target_group->azimuth_range[0],target_group->azimuth_range[1]);
	TransFormJSON(settings, buf, 10);

	if(tcp_write(pcb, buf, 1024, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 1;
}
int32_t GetLocked(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	char buf[PACKET_SIZE];
//	web_param current_lock_param[1] = { {"IsLocked", "0"}};
//	int locked = Xil_In32(0x43C50038);
//	itoa(locked, current_lock_param[0].param_value, 10);
//
//	TransFormJSON(current_lock_param, buf, 1);

//	web_param current_lock_param[2] = { {"IsLocked", "0"}, {"IsPLLON", "0"}};
	web_param current_lock_param[3] = { {"IsLocked", "0"}, {"IsPLLON", "0"}, {"AngleErr", "-"}};

#ifdef G32_B1_PPS
	int locked = Xil_In32(0x43C50038);
	itoa(locked, current_lock_param[0].param_value, 10);

	itoa(lidar_state_.motor_setting_.ispllon, current_lock_param[1].param_value, 10);

	itoa(AngleErr, current_lock_param[2].param_value, 10);
#endif
//	TransFormJSON(current_lock_param, buf, 2);
	TransFormJSON(current_lock_param, buf, 3);


	if(tcp_write(pcb, buf, 150, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}
	return 0;
}
void SetRoutingDelayToLidarState(LidarState* lidar_state, LidarState_Ch lidar_state_ch)
{
	strncpy(lidar_state->calibration_setting_.routing_delay[0], lidar_state_ch.delay_ch1, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[1], lidar_state_ch.delay_ch2, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[2], lidar_state_ch.delay_ch3, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[3], lidar_state_ch.delay_ch4, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[4], lidar_state_ch.delay_ch5, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[5], lidar_state_ch.delay_ch6, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[6], lidar_state_ch.delay_ch7, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[7], lidar_state_ch.delay_ch8, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[8], lidar_state_ch.delay_ch9, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[9], lidar_state_ch.delay_ch10, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[10], lidar_state_ch.delay_ch11, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[11], lidar_state_ch.delay_ch12, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[12], lidar_state_ch.delay_ch13, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[13], lidar_state_ch.delay_ch14, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[14], lidar_state_ch.delay_ch15, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[15], lidar_state_ch.delay_ch16, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[16], lidar_state_ch.delay_ch17, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[17], lidar_state_ch.delay_ch18, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[18], lidar_state_ch.delay_ch19, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[19], lidar_state_ch.delay_ch20, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[20], lidar_state_ch.delay_ch21, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[21], lidar_state_ch.delay_ch22, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[22], lidar_state_ch.delay_ch23, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[23], lidar_state_ch.delay_ch24, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[24], lidar_state_ch.delay_ch25, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[25], lidar_state_ch.delay_ch26, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[26], lidar_state_ch.delay_ch27, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[27], lidar_state_ch.delay_ch28, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[28], lidar_state_ch.delay_ch29, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[29], lidar_state_ch.delay_ch30, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[30], lidar_state_ch.delay_ch31, 10);
	strncpy(lidar_state->calibration_setting_.routing_delay[31], lidar_state_ch.delay_ch32, 10);
}

#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
