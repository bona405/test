/*
 * Copyright (C) 2017 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

/* Connection handle for a UDP Client session */

#include "udp_server.h"
#define PERF_DEBUG

extern struct netif server_netif;
static struct udp_pcb *pcb[NUM_OF_PARALLEL_CLIENTS];
#ifdef PERF_DEBUG
static struct udp_pcb *pcb_for_test;
#endif
static struct udp_pcb *pcb_for_logging;
static struct udp_pcb *s_pcb;
static struct udp_pcb *pcb_for_motor_test;
static struct perf_stats client;
int _recv_size = 0;

int len = sizeof(udp_packet);



static void print_udp_conn_stats(void)
{
	xil_printf("[%3d] local %s port %d connected with ", client.client_id, inet_ntoa(server_netif.ip_addr), pcb[0]->local_port);
//	xil_printf("[%3d] local %s port %d connected with ", client.client_id, inet_ntoa(server_netif.ip_addr), pcb[1]->local_port);
	xil_printf("%s port %d\r\n", inet_ntoa(pcb[0]->remote_ip), pcb[0]->remote_port);
//	xil_printf("%s port %d\r\n", inet_ntoa(pcb[1]->remote_ip), pcb[1]->remote_port);
	xil_printf("[ ID] Interval\t\tTransfer   Bandwidth\n\r");
}

static void stats_buffer(char* outString, double data, enum measure_t type)
{
	int conv = KCONV_UNIT;
	const char *format;
	double unit = 1024.0;

	if (type == SPEED)
		unit = 1000.0;

	while (data >= unit && conv <= KCONV_GIGA)
	{
		data /= unit;
		conv++;
	}

	/* Fit data in 4 places */
	if (data < 9.995)
	{ /* 9.995 rounded to 10.0 */
		format = "%4.2f %c"; /* #.## */
	}
	else if (data < 99.95)
	{ /* 99.95 rounded to 100 */
		format = "%4.1f %c"; /* ##.# */
	}
	else
	{
		format = "%4.0f %c"; /* #### */
	}
	sprintf(outString, format, data, conv);
}

/* The report function of a UDP client session */
static void udp_conn_report(u64_t diff, enum report_type report_type)
{
	u64_t total_len;
	double duration, bandwidth = 0;
	char data[16], perf[16], time[64];

	if (report_type == INTER_REPORT)
	{
		total_len = client.i_report.total_bytes;
	}
	else
	{
		client.i_report.last_report_time = 0;
		total_len = client.total_bytes;
	}

	/* Converting duration from milliseconds to secs,
	 * and bandwidth to bits/sec.
	 */
	duration = diff / 1000.0; /* secs */
	if (duration)
		bandwidth = (total_len / duration) * 8.0;

	stats_buffer(data, total_len, BYTES);
	stats_buffer(perf, bandwidth, SPEED);
	/* On 32-bit platforms, xil_printf is not able to print
	 * u64_t values, so converting these values in strings and
	 * displaying results
	 */
	sprintf(time, "%4.1f-%4.1f sec", (double) client.i_report.last_report_time, (double) (client.i_report.last_report_time + duration));
	xil_printf("[%3d] %s  %sBytes  %sbits/sec\n\r", client.client_id, time, data, perf);

	if (report_type == INTER_REPORT)
		client.i_report.last_report_time += duration;
	else
		xil_printf("[%3d] sent %llu datagrams\n\r", client.client_id, client.cnt_datagrams);
}

static void reset_stats(void)
{
	client.client_id++;
	/* Print connection statistics */
	print_udp_conn_stats();
	/* Save start time for final report */
	client.start_time = get_time_ms();
	client.total_bytes = 0;
	client.cnt_datagrams = 0;

	/* Initialize Interim report paramters */
	client.i_report.start_time = 0;
	client.i_report.total_bytes = 0;
	client.i_report.last_report_time = 0;
}

#if defined(G32_ES_TEST)
void udp_send_packet(int packet_id, RawDataBlock* blockData, uint32_t packet_index, u32 motorRPM,u32 VoltageData,u32 Voltagefraction, uint8_t (*intensity)[CHANNEL_SIZE], u32 fov_data_block_count)
#else
void udp_send_packet(int packet_id, RawDataBlock* blockData, uint32_t packet_index, uint8_t frame_rate, uint8_t vertical_angle, uint8_t (*intensity)[CHANNEL_SIZE], u32 fov_data_block_count)
#endif
{
	u32 total_azimuth_count = fov_data_block_count / 2;
//	dbg_printf("dbg_print test\r\n");
	int *payload;
	u8_t i;
	u8_t retries = MAX_SEND_RETRY; // 10
	struct pbuf *packet;
	err_t err;

	uint32_t send_size = 0;

	udp_packet.header.packet_id = packet_id;
	if(packet_index * DATA_BLOCK_SIZE < total_azimuth_count)
		udp_packet.header.top_bottom_side = 0;
	else
		udp_packet.header.top_bottom_side = 1;
#if defined(G32_ES_TEST)
	udp_packet.header.motorRPM = motorRPM;
	udp_packet.header.VoltageData = VoltageData;
	udp_packet.header.Voltagefraction = Voltagefraction;
	udp_packet.header.frame_rate = (uint8_t)FRAME_RATE;
	udp_packet.header.vertical_angle = (uint8_t)VERTICAL_ANGLE;
#else
	udp_packet.header.frame_rate = frame_rate;
	udp_packet.header.vertical_angle = vertical_angle;

#endif

	if (packet_index == 0)
		udp_packet.header.data_type_ = POINT_CLOUD_DATA_TYPE;
	else if (packet_index == (POINT8_MEMORY_SIZE * SCENE_MEMORY_NUMBER) / 2 - 1 || packet_index == POINT8_MEMORY_SIZE * SCENE_MEMORY_NUMBER - 1)
		udp_packet.header.data_type_ = END_OF_DATA_STREAM;
	else if (packet_index > POINT8_MEMORY_SIZE * SCENE_MEMORY_NUMBER - 1)
	{
		udp_packet.header.data_type_ = ERR_OF_DATA_STREAM;
		xil_printf("cnt : %d\r\n", packet_index);
	}

	else
		udp_packet.header.data_type_ = POINT_CLOUD_DATA_TYPE;

	for (int i = 0; i < 24; ++i)
	{
		udp_packet.data_block[i].flag = 0xFFEE;
		// 240603 hohyeok
		udp_packet.data_block[i].azimuth = blockData[packet_index * DATA_BLOCK_SIZE + i].azimuth_;
//		udp_packet.data_block[i].azimuth = (lidar_state_.a2z_setting.reserved02 == 1)? (blockData[packet_index * DATA_BLOCK_SIZE + i].azimuth_ * lidar_state_.a2z_setting.fov_correction/120.0):(blockData[packet_index * DATA_BLOCK_SIZE + i].azimuth_);
		for (int j = 0; j < 16; ++j)
		{
			// 240603 hohyeok
			udp_packet.data_block[i].channel_data[j].tof = blockData[packet_index * DATA_BLOCK_SIZE + i].distance_[j].distance;
//			udp_packet.data_block[i].channel_data[j].tof = (lidar_state_.a2z_setting.reserved02 == 1)? (blockData[packet_index * DATA_BLOCK_SIZE + i].distance_[j].distance + (lidar_state_.a2z_setting.dist_offset*2.56)):(blockData[packet_index * DATA_BLOCK_SIZE + i].distance_[j].distance);
//			udp_packet.data_block[i].channel_data[j].intensity = intensity[packet_index * DATA_BLOCK_SIZE + i][j];			//blockData[i].PL_DATA[j].CFVal;
			udp_packet.data_block[i].channel_data[j].intensity = (unsigned int)((double)(blockData[packet_index * DATA_BLOCK_SIZE + i].distance_[j].pulse_width));
			if(inten_filter)
			{
//				if(udp_packet.data_block[i].channel_data[j].intensity < 50)
				if(udp_packet.data_block[i].channel_data[j].intensity < inten_filter)
				{
					udp_packet.data_block[i].channel_data[j].tof = 0;
					udp_packet.data_block[i].channel_data[j].intensity = 0;
				}
			}
		}
	}
	udp_packet.time = 0x11112222;
	udp_packet.factory = 0x11;

	int len = sizeof(udp_packet);

	for (i = 0; i < NUM_OF_PARALLEL_CLIENTS; i++)
	{
		send_size = len;
		packet = pbuf_alloc(PBUF_TRANSPORT, send_size, PBUF_POOL);
		payload = (int*) (packet->payload);

		if (!packet)
		{
			xil_printf("error allocating pbuf to send\r\n");
			return;
		}
		else
		{
			memcpy(&payload[0], (void *) &udp_packet, send_size);
		}

		while (retries)
		{
			err = udp_send(pcb[i], packet);

			if (err != ERR_OK)
			{
				xil_printf("Error on udp_send: %d\r\n", err);
				retries--;
				usleep(100);
			}
			else
			{
				client.total_bytes += UDP_SEND_BUFSIZE;
				client.cnt_datagrams++;
				client.i_report.total_bytes += UDP_SEND_BUFSIZE;
				break;
			}
		}

		if (!retries)
		{
			/* Terminate this app */
			u64_t now = get_time_ms();
			u64_t diff_ms = now - client.start_time;
			xil_printf("Too many udp_send() retries, ");
			xil_printf("Terminating application\n\r");
			udp_conn_report(diff_ms, UDP_DONE_CLIENT);
			xil_printf("UDP test failed\n\r");
			udp_remove(pcb[i]);
			pcb[i] = NULL;
		}

		pbuf_free(packet);
		/* For ZynqMP SGMII, At high speed,
		 * "pack dropped, no space" issue observed.
		 * To avoid this, added delay of 2us between each
		 * packets.
		 */
#if defined (__aarch64__) && defined (XLWIP_CONFIG_INCLUDE_AXI_ETHERNET_DMA)
		usleep(2);
#endif /* __aarch64__ */

	}
}

#if defined(G32_ES_TEST)
void udp_send_packet_a3(u32 packet_id, u32 send_packet_count, RawDataBlock* blockData, u32 motorRPM,u32 VoltageData,u32 Voltagefraction, uint8_t (*intensity)[CHANNEL_SIZE], u32 send_start_azimuth_index, u32 send_finish_azimuth_index, u8 top_bottom_side)
#else
void udp_send_packet_a3(u32 packet_id, u32 send_packet_count, RawDataBlock* blockData, uint8_t frame_rate, uint8_t vertical_angle, uint8_t (*intensity)[CHANNEL_SIZE], u32 send_start_azimuth_index, u32 send_finish_azimuth_index, u8 top_bottom_side)
#endif
{
//	dbg_printf("dbg_print test\r\n");
	int *payload;
	u8_t i;
	u8_t retries = MAX_SEND_RETRY; // 10
	struct pbuf *packet;

	err_t err;

	uint32_t send_size = 0;

	udp_packet.header.packet_id = packet_id;
	udp_packet.header.top_bottom_side = top_bottom_side;

#if defined(G32_ES_TEST)
	udp_packet.header.motorRPM = motorRPM;
	udp_packet.header.VoltageData = VoltageData;
	udp_packet.header.Voltagefraction = Voltagefraction;
	udp_packet.header.frame_rate = (uint8_t)FRAME_RATE;
	udp_packet.header.vertical_angle = (uint8_t)VERTICAL_ANGLE;
#else
	udp_packet.header.frame_rate = frame_rate;
	udp_packet.header.vertical_angle = vertical_angle;

#endif

	if (packet_id == 0)
		udp_packet.header.data_type_ = POINT_CLOUD_DATA_TYPE;
//	else if (packet_id == (POINT8_MEMORY_SIZE * SCENE_MEMORY_NUMBER) / 2 - 1 || packet_id == POINT8_MEMORY_SIZE * SCENE_MEMORY_NUMBER - 1)
	else if (packet_id == send_packet_count - 1)
		udp_packet.header.data_type_ = END_OF_DATA_STREAM;
//	else if (packet_id > POINT8_MEMORY_SIZE * SCENE_MEMORY_NUMBER - 1)
//	{
//		udp_packet.header.data_type_ = ERR_OF_DATA_STREAM;
//		xil_printf("cnt : %d\r\n", packet_id);
//	}

	else
		udp_packet.header.data_type_ = POINT_CLOUD_DATA_TYPE;

//	if((bottom_data_block_count - (packet_id)) / 24 == 0 || (top_data_block_count - (packet_id)) / 24 == 0)
//		udp_packet.header.data_type_ = END_OF_DATA_STREAM;

	for (int i = 0; i < DATA_BLOCK_SIZE; ++i)
	{
		if(send_start_azimuth_index + i < send_finish_azimuth_index)
		{
			udp_packet.data_block[i].flag = 0xFFEE;
			// 240603 hohyeok
			udp_packet.data_block[i].azimuth = blockData[send_start_azimuth_index + i].azimuth_;
//			udp_packet.data_block[i].azimuth = (lidar_state_.a2z_setting.reserved02 == 1)? (blockData[send_start_azimuth_index + i].azimuth_ * (lidar_state_.a2z_setting.fov_correction/120.0)):(blockData[send_start_azimuth_index + i].azimuth_);
			for (int j = 0; j < 16; ++j)
			{
				// 240603 hohyeok
				udp_packet.data_block[i].channel_data[j].tof = blockData[send_start_azimuth_index + i].distance_[j].distance;
//				udp_packet.data_block[i].channel_data[j].tof = (lidar_state_.a2z_setting.reserved02 == 1)? (blockData[send_start_azimuth_index + i].distance_[j].distance + (lidar_state_.a2z_setting.dist_offset*2.56)):(blockData[send_start_azimuth_index + i].distance_[j].distance);

//				udp_packet.data_block[i].channel_data[j].intensity = intensity[send_start_azimuth_index + i][j];			//blockData[i].PL_DATA[j].CFVal;
//				udp_packet.data_block[i].channel_data[j].intensity = (unsigned int)((double)(blockData[send_start_azimuth_index + i].distance_[j].pulse_width));
				udp_packet.data_block[i].channel_data[j].intensity = (unsigned int)((blockData[send_start_azimuth_index + i].distance_[j].pulse_width));
//				udp_packet.data_block[i].channel_data[j].intensity = (unsigned int)(((double)(blockData[send_start_azimuth_index + i].distance_[j].pulse_width)/65535.0) * 255.0);
				if(inten_filter)
				{
	//				if(udp_packet.data_block[i].channel_data[j].intensity < 50)
					if(inten_filter == 1)
					{
						if(udp_packet.data_block[i].channel_data[j].intensity == 0 || udp_packet.data_block[i].channel_data[j].intensity == 255)
						{
							udp_packet.data_block[i].channel_data[j].tof = 0;
							udp_packet.data_block[i].channel_data[j].intensity = 0;
						}
					}
					else if(udp_packet.data_block[i].channel_data[j].intensity < inten_filter && udp_packet.data_block[i].channel_data[j].intensity != 0)
					{
						udp_packet.data_block[i].channel_data[j].tof = 0;
						udp_packet.data_block[i].channel_data[j].intensity = 0;
					}
				}
//				if(top_bottom_side)		//Debug
//				{
//					udp_packet.data_block[i].channel_data[j].intensity = 255;
//					udp_packet.data_block[i].channel_data[j].tof = 256;
//				}
//				else
//				{
//					udp_packet.data_block[i].channel_data[j].intensity = 0;
//					udp_packet.data_block[i].channel_data[j].tof = 300;
//				}
			}
		}
		else
			memset(&udp_packet.data_block[i], 0x00, sizeof(DataBlock) * (DATA_BLOCK_SIZE-i));  // 데이터 손실 방지
//			memset(&udp_packet.data_block[0], 0x00, sizeof(DataBlock) * DATA_BLOCK_SIZE);
	}
	udp_packet.time = 0x11112222;
	udp_packet.factory = 0x11;

//	int len = sizeof(udp_packet);

	for (i = 0; i < NUM_OF_PARALLEL_CLIENTS; i++)
	{
		send_size = len;
		packet = pbuf_alloc(PBUF_TRANSPORT, send_size, PBUF_POOL);
		payload = (int*) (packet->payload);

		if (!packet)
		{
			xil_printf("error allocating pbuf to send\r\n");
			return;
		}
		else
		{
			memcpy(&payload[0], (void *) &udp_packet, send_size);
		}

		while (retries)
		{
			err = udp_send(pcb[i], packet);

			if (err != ERR_OK)
			{
				xil_printf("Error on udp_send: %d\r\n", err);
				retries--;
				usleep(100);
			}
			else
			{
//				client.total_bytes += UDP_SEND_BUFSIZE;
//				client.cnt_datagrams++;
//				client.i_report.total_bytes += UDP_SEND_BUFSIZE;
				break;
			}
		}

		if (!retries)
		{
			/* Terminate this app */
			u64_t now = get_time_ms();
			u64_t diff_ms = now - client.start_time;
			xil_printf("Too many udp_send() retries, ");
			xil_printf("Terminating application\n\r");
			udp_conn_report(diff_ms, UDP_DONE_CLIENT);
			xil_printf("UDP test failed\n\r");
			udp_remove(pcb[i]);
			pcb[i] = NULL;
		}

		pbuf_free(packet);
		/* For ZynqMP SGMII, At high speed,
		 * "pack dropped, no space" issue observed.
		 * To avoid this, added delay of 2us between each
		 * packets.
		 */
#if defined (__aarch64__) && defined (XLWIP_CONFIG_INCLUDE_AXI_ETHERNET_DMA)
		usleep(2);
#endif /* __aarch64__ */

	}
}

void start_udp_application(int port)
{
	err_t err;
	ip_addr_t remote_addr;
	u32_t i;

//	err = inet_aton(UDP_SERVER_IP_ADDRESS, &remote_addr);
	char tmpip[20] = {0,};
	char lidarip[20] = {0,};
//	memset(lidar_state_.network_setting_.target_ip, 0x00, 20);

	strcpy(tmpip,lidar_state_.network_setting_.target_ip);

	strcpy(lidarip,lidar_state_.network_setting_.ip);

	int is_uni_valid = 1;

	if(strlen(tmpip) < 7 || strlen(tmpip) > 15)
		is_uni_valid = 0;

	int dot_count = 0;

	for(int i = 0; i<strlen(tmpip); ++i)
	{
		if(tmpip[i] == '.')
		{
			dot_count++;
		}
	}

	if(dot_count != 3)
		is_uni_valid = 0;

	for(int i = 0; i<strlen(tmpip); ++i)
	{
		if(!isdigit(tmpip[i]) && tmpip[i] != '.')
		{
			is_uni_valid = 0;
			break;
		}
	}

	if(is_uni_valid)
	{
		err = inet_aton(tmpip, &remote_addr);
	}
	else
	{
		err = inet_aton(UDP_SERVER_IP_ADDRESS, &remote_addr);
	}


	if (!err)
	{
		xil_printf("Invalid Server IP address: %d\r\n", err);
		return;
	}

	for (i = 0; i < NUM_OF_PARALLEL_CLIENTS; i++)
	{
		/* Create Client PCB */
		pcb[i] = udp_new();
		if (!pcb[i])
		{
			xil_printf("Error in PCB creation. out of memory\r\n");
			return;
		}

		err = udp_connect(pcb[i], &remote_addr, port);

		if (err != ERR_OK)
		{
			xil_printf("udp_client: Error on udp_connect: %d\r\n", err);
			udp_remove(pcb[i]);
			return;
		}
	}
	/* Wait for successful connection */
	usleep(10);
	reset_stats();

	s_pcb = udp_new();
	if (!s_pcb)
	{
		xil_printf("UDP server: Error creating PCB. Out of Memory\r\n");
		return;
	}

	err = udp_bind(s_pcb, IP_ADDR_ANY, port); //UDP_CONN_PORT  IP_ADDR_BROADCAST
	//s_pcb->remote_ip = remote_addr;
	if (err != ERR_OK)
	{
		xil_printf("UDP server: Unable to bind to port");
		xil_printf(" %d: err = %d\r\n", port, err);
		udp_remove(s_pcb);
		return;
	}

	/* specify callback to use for incoming connections */
	//	udp_recv(s_pcb, udp_recv_perf_traffic, NULL);
	usleep(10);
	return;
}

void start_udp_application_for_test(int port)
{
#ifdef PERF_DEBUG
	err_t err;
	ip_addr_t remote_addr;

	err = inet_aton(UDP_SERVER_IP_ADDRESS, &remote_addr);
	if (!err)
	{
		xil_printf("Invalid Server IP address: %d\r\n", err);
		return;
	}

	/* Create Client PCB */
	pcb_for_test = udp_new();
	if (!pcb_for_test)
	{
		xil_printf("Error in PCB creation. out of memory\r\n");
		return;
	}

	err = udp_connect(pcb_for_test, &remote_addr, port);

	if (err != ERR_OK)
	{
		xil_printf("udp_client: Error on udp_connect: %d\r\n", err);
		udp_remove(pcb_for_test);
		return;
	}

	/* Wait for successful connection */
	usleep(10);

#endif
	return;
}

void start_udp_application_for_logging(int port)
{
#ifdef PERF_DEBUG
	err_t err;
	ip_addr_t remote_addr;

	err = inet_aton(UDP_SERVER_IP_ADDRESS, &remote_addr);
	if (!err)
	{
		xil_printf("Invalid Server IP address: %d\r\n", err);
		return;
	}

	/* Create Client PCB */
	pcb_for_logging = udp_new();
	if (!pcb_for_logging)
	{
		xil_printf("Error in PCB creation. out of memory\r\n");
		return;
	}

	err = udp_connect(pcb_for_logging, &remote_addr, port);

	if (err != ERR_OK)
	{
		xil_printf("udp_client: Error on udp_connect: %d\r\n", err);
		udp_remove(pcb_for_logging);
		return;
	}

	/* Wait for successful connection */
	usleep(10);

#endif
	return;
}

void start_udp_application_for_motor_test(int port)
{
#ifdef PERF_DEBUG
	err_t err;
	ip_addr_t remote_addr;

	err = inet_aton(UDP_SERVER_IP_ADDRESS, &remote_addr);
	if (!err)
	{
		xil_printf("Invalid Server IP address: %d\r\n", err);
		return;
	}

	/* Create Client PCB */
	pcb_for_motor_test = udp_new();
	if (!pcb_for_motor_test)
	{
		xil_printf("Error in PCB creation. out of memory\r\n");
		return;
	}

	err = udp_connect(pcb_for_motor_test, &remote_addr, port);

	if (err != ERR_OK)
	{
		xil_printf("udp_client: Error on udp_connect: %d\r\n", err);
		udp_remove(pcb_for_motor_test);
		return;
	}

	/* Wait for successful connection */
	usleep(10);

#endif
	return;
}

void udp_send_packet_a4(u32 packet_id, u32 send_packet_count, RawDataBlock* blockData, uint8_t frame_rate, uint8_t vertical_angle, uint8_t (*intensity)[CHANNEL_SIZE], u32 send_start_azimuth_index, u32 send_finish_azimuth_index, u8 top_bottom_side, float z_mean_float)
{
//	dbg_printf("dbg_print test\r\n");
	int *payload;
	u8_t i;
	u8_t retries = MAX_SEND_RETRY; // 10
	struct pbuf *packet;

	err_t err;

	uint32_t send_size = 0;

	udp_packet.header.packet_id = packet_id;
	udp_packet.header.top_bottom_side = top_bottom_side;

#if defined(G32_ES_TEST)
	udp_packet.header.motorRPM = motorRPM;
	udp_packet.header.VoltageData = VoltageData;
	udp_packet.header.Voltagefraction = Voltagefraction;
	udp_packet.header.frame_rate = (uint8_t)FRAME_RATE;
	udp_packet.header.vertical_angle = (uint8_t)VERTICAL_ANGLE;
#else
	udp_packet.header.frame_rate = frame_rate;
	udp_packet.header.vertical_angle = vertical_angle;
//	double tmp_z_mean = z_mean_float*100.0;
	udp_packet.header.z_mean = (int32_t)(z_mean_float); //////////////////////////////////
#endif

#if defined(G32_B1_PPS)
	char hexStrYear[255];
	char hexStrMonth[255];
	char hexStrDay[255];
	char hexStrHour[255];
	char hexStrMinute[255];
	char hexStrSecond[255];
	snprintf(hexStrYear,sizeof(hexStrYear),"%02x",blockData[send_start_azimuth_index].pps_info.year);
	snprintf(hexStrMonth,sizeof(hexStrMonth),"%02x",blockData[send_start_azimuth_index].pps_info.month);
	snprintf(hexStrDay,sizeof(hexStrDay),"%02x",blockData[send_start_azimuth_index].pps_info.day);
	snprintf(hexStrHour,sizeof(hexStrHour),"%02x",blockData[send_start_azimuth_index].pps_info.hour);
	snprintf(hexStrMinute,sizeof(hexStrMinute),"%02x",blockData[send_start_azimuth_index].pps_info.minute);
	snprintf(hexStrSecond,sizeof(hexStrSecond),"%02x",blockData[send_start_azimuth_index].pps_info.second);
	udp_packet.header.pps_time.year = atoi(hexStrYear);
	udp_packet.header.pps_time.month = atoi(hexStrMonth);
	udp_packet.header.pps_time.day = atoi(hexStrDay);
	udp_packet.header.pps_time.hour = atoi(hexStrHour);
	udp_packet.header.pps_time.minute = atoi(hexStrMinute);
	udp_packet.header.pps_time.second = atoi(hexStrSecond);


	uint8_t cur_hz = 20;
	if(lidar_state_.motor_setting_.hz_setting == 3)	cur_hz = 25;
	double time_factor = 1.0/(32768.0 * cur_hz);

	float nanosecond = (float)(blockData[send_start_azimuth_index].pps_info.pps_cnt[0] | blockData[send_start_azimuth_index].pps_info.pps_cnt[1] << 8 | blockData[send_start_azimuth_index].pps_info.pps_cnt[2] << 16)* time_factor;
	udp_packet.header.pps_time.nanosecond = nanosecond*1e9;
#endif
	if (packet_id == 0)
		udp_packet.header.data_type_ = POINT_CLOUD_DATA_TYPE;
//	else if (packet_id == (POINT8_MEMORY_SIZE * SCENE_MEMORY_NUMBER) / 2 - 1 || packet_id == POINT8_MEMORY_SIZE * SCENE_MEMORY_NUMBER - 1)
	else if (packet_id == send_packet_count - 1)
		udp_packet.header.data_type_ = END_OF_DATA_STREAM;
//	else if (packet_id > POINT8_MEMORY_SIZE * SCENE_MEMORY_NUMBER - 1)
//	{
//		udp_packet.header.data_type_ = ERR_OF_DATA_STREAM;
//		xil_printf("cnt : %d\r\n", packet_id);
//	}

	else
		udp_packet.header.data_type_ = POINT_CLOUD_DATA_TYPE;

//	if((bottom_data_block_count - (packet_id)) / 24 == 0 || (top_data_block_count - (packet_id)) / 24 == 0)
//		udp_packet.header.data_type_ = END_OF_DATA_STREAM;

	for (int i = 0; i < DATA_BLOCK_SIZE; ++i)
	{
		if(send_start_azimuth_index + i < send_finish_azimuth_index)
		{
			udp_packet.data_block[i].flag = 0xFFEE;
			// 240603 hohyeok
			udp_packet.data_block[i].azimuth = blockData[send_start_azimuth_index + i].azimuth_;
//			udp_packet.data_block[i].azimuth = (lidar_state_.a2z_setting.reserved02 == 1)? (blockData[send_start_azimuth_index + i].azimuth_ * (lidar_state_.a2z_setting.fov_correction/12000.0)):(blockData[send_start_azimuth_index + i].azimuth_);
			for (int j = 0; j < 16; ++j)
			{
				// 240603 hohyeok
				udp_packet.data_block[i].channel_data[j].tof = blockData[send_start_azimuth_index + i].distance_[j].distance;
//				udp_packet.data_block[i].channel_data[j].tof = (lidar_state_.a2z_setting.reserved02 == 1)? (blockData[send_start_azimuth_index + i].distance_[j].distance + lidar_state_.a2z_setting.dist_offset):(blockData[send_start_azimuth_index + i].distance_[j].distance);
//				udp_packet.data_block[i].channel_data[j].intensity = intensity[send_start_azimuth_index + i][j];			//blockData[i].PL_DATA[j].CFVal;
//				udp_packet.data_block[i].channel_data[j].intensity = (unsigned int)((double)(blockData[send_start_azimuth_index + i].distance_[j].pulse_width));
				udp_packet.data_block[i].channel_data[j].intensity = (unsigned int)((blockData[send_start_azimuth_index + i].distance_[j].pulse_width));
//				udp_packet.data_block[i].channel_data[j].intensity = (unsigned int)(((double)(blockData[send_start_azimuth_index + i].distance_[j].pulse_width)/65535.0) * 255.0);
				if(inten_filter)
				{
	//				if(udp_packet.data_block[i].channel_data[j].intensity < 50)
					if(inten_filter == 1)
					{
						if(udp_packet.data_block[i].channel_data[j].intensity == 0 || udp_packet.data_block[i].channel_data[j].intensity == 255)
						{
							udp_packet.data_block[i].channel_data[j].tof = 0;
							udp_packet.data_block[i].channel_data[j].intensity = 0;
						}
					}
					else if(udp_packet.data_block[i].channel_data[j].intensity < inten_filter && udp_packet.data_block[i].channel_data[j].intensity != 0)
					{
						udp_packet.data_block[i].channel_data[j].tof = 0;
						udp_packet.data_block[i].channel_data[j].intensity = 0;
					}
				}
//				if(top_bottom_side)		//Debug
//				{
//					udp_packet.data_block[i].channel_data[j].intensity = 255;
//					udp_packet.data_block[i].channel_data[j].tof = 256;
//				}
//				else
//				{
//					udp_packet.data_block[i].channel_data[j].intensity = 0;
//					udp_packet.data_block[i].channel_data[j].tof = 300;
//				}
			}
		}
		else
			memset(&udp_packet.data_block[i], 0x00, sizeof(DataBlock) * (DATA_BLOCK_SIZE-i));  // 데이터 손실 방지
//			memset(&udp_packet.data_block[0], 0x00, sizeof(DataBlock) * DATA_BLOCK_SIZE);
	}
	//test
	uint32_t motor_freq_adpt = blockData[send_start_azimuth_index].pps_info.motor_freq_adpt[0] | (blockData[send_start_azimuth_index].pps_info.motor_freq_adpt[1] << 8) | (blockData[send_start_azimuth_index].pps_info.motor_freq_adpt[2] << 16);
	udp_packet.header.z_mean =  motor_freq_adpt |  (blockData[send_start_azimuth_index].pps_info.iDPLL_lock << 24);
	udp_packet.time = (blockData[send_start_azimuth_index].pps_info.angle_diff) | (blockData[send_start_azimuth_index].pps_info.fsm_status << 16);
	udp_packet.factory = 0x11;
	udp_packet.header.z_mean = (int32_t)(z_mean_float);
//	int len = sizeof(udp_packet);

	for (i = 0; i < NUM_OF_PARALLEL_CLIENTS; i++)
	{
		send_size = len;
		packet = pbuf_alloc(PBUF_TRANSPORT, send_size, PBUF_POOL);
		payload = (int*) (packet->payload);

		if (!packet)
		{
			xil_printf("error allocating pbuf to send\r\n");
			return;
		}
		else
		{
			memcpy(&payload[0], (void *) &udp_packet, send_size);
		}

		while (retries)
		{
			err = udp_send(pcb[i], packet);

			if (err != ERR_OK)
			{
				xil_printf("Error on udp_send: %d\r\n", err);
				retries--;
//				usleep(100);
				usleep(20);
			}
			else
			{
//				client.total_bytes += UDP_SEND_BUFSIZE;
//				client.cnt_datagrams++;
//				client.i_report.total_bytes += UDP_SEND_BUFSIZE;
				break;
			}
		}

		if (!retries)
		{
			/* Terminate this app */
			u64_t now = get_time_ms();
			u64_t diff_ms = now - client.start_time;
			xil_printf("Too many udp_send() retries, ");
			xil_printf("Terminating application\n\r");
			udp_conn_report(diff_ms, UDP_DONE_CLIENT);
			xil_printf("UDP test failed\n\r");
			udp_remove(pcb[i]);
			pcb[i] = NULL;
		}

		pbuf_free(packet);
		/* For ZynqMP SGMII, At high speed,
		 * "pack dropped, no space" issue observed.
		 * To avoid this, added delay of 2us between each
		 * packets.
		 */
#if defined (__aarch64__) && defined (XLWIP_CONFIG_INCLUDE_AXI_ETHERNET_DMA)
		usleep(2);
#endif /* __aarch64__ */

	}
}

void udp_send_packet_for_test(char* send_data)
{

	int send_size = 1212;
	int retries = 10;
	struct pbuf *packet = pbuf_alloc(PBUF_TRANSPORT, send_size, PBUF_POOL);
	int *payload = (int*) (packet->payload);

	if (!packet)
	{
		xil_printf("error allocating pbuf to send\r\n");
		return;
	}
	else
	{
		memcpy(&payload[0], (void *) send_data, send_size);
	}

	while (retries)
	{
		err_t err = udp_send(pcb_for_test, packet);

		if (err != ERR_OK)
		{
			xil_printf("Error on udp_send: %d\r\n", err);
			retries--;
			usleep(100);
		}
		else
		{
			client.total_bytes += UDP_SEND_BUFSIZE;
			client.cnt_datagrams++;
			client.i_report.total_bytes += UDP_SEND_BUFSIZE;
			break;
		}
	}

	if (!retries)
	{
		/* Terminate this app */
		u64_t now = get_time_ms();
		u64_t diff_ms = now - client.start_time;
		xil_printf("Too many udp_send() retries, ");
		xil_printf("Terminating application\n\r");
		udp_conn_report(diff_ms, UDP_DONE_CLIENT);
		xil_printf("UDP test failed\n\r");
		udp_remove(pcb_for_test);
		pcb_for_test = NULL;
	}

	pbuf_free(packet);
	/* For ZynqMP SGMII, At high speed,
	 * "pack dropped, no space" issue observed.
	 * To avoid this, added delay of 2us between each
	 * packets.
	 */

}

void udp_send_packet_for_logging(char* send_data)
{

#ifdef PERF_DEBUG

	int send_size = 1212;
	int retries = 10;
	struct pbuf *packet = pbuf_alloc(PBUF_TRANSPORT, send_size, PBUF_POOL);
	int *payload = (int*) (packet->payload);

	if (!packet)
	{
		xil_printf("error allocating pbuf to send\r\n");
		return;
	}
	else
	{
		memcpy(&payload[0], (void *) send_data, send_size);
	}

	while (retries)
	{
		err_t err = udp_send(pcb_for_logging, packet);

		if (err != ERR_OK)
		{
			xil_printf("Error on udp_send: %d\r\n", err);
			retries--;
			usleep(100);
		}
		else
		{
			client.total_bytes += UDP_SEND_BUFSIZE;
			client.cnt_datagrams++;
			client.i_report.total_bytes += UDP_SEND_BUFSIZE;
			break;
		}
	}

	if (!retries)
	{
		/* Terminate this app */
		u64_t now = get_time_ms();
		u64_t diff_ms = now - client.start_time;
		xil_printf("Too many udp_send() retries, ");
		xil_printf("Terminating application\n\r");
		udp_conn_report(diff_ms, UDP_DONE_CLIENT);
		xil_printf("UDP test failed\n\r");
		udp_remove(pcb_for_logging);
		pcb_for_logging = NULL;
	}

	pbuf_free(packet);
	/* For ZynqMP SGMII, At high speed,
	 * "pack dropped, no space" issue observed.
	 * To avoid this, added delay of 2us between each
	 * packets.
	 */
#if defined (__aarch64__) && defined (XLWIP_CONFIG_INCLUDE_AXI_ETHERNET_DMA)
	usleep(2);
#endif /* __aarch64__ */

#endif
}

void udp_send_packet_for_motor_test(char* send_data)
{

#ifdef PERF_DEBUG

	int send_size = 1212;
	int retries = 10;
	struct pbuf *packet = pbuf_alloc(PBUF_TRANSPORT, send_size, PBUF_POOL);
	int *payload = (int*) (packet->payload);

	if (!packet)
	{
		xil_printf("error allocating pbuf to send\r\n");
		return;
	}
	else
	{
		memcpy(&payload[0], (void *) send_data, send_size);
	}

	while (retries)
	{
		err_t err = udp_send(pcb_for_motor_test, packet);

		if (err != ERR_OK)
		{
			xil_printf("Error on udp_send: %d\r\n", err);
			retries--;
			usleep(100);
		}
		else
		{
			client.total_bytes += UDP_SEND_BUFSIZE;
			client.cnt_datagrams++;
			client.i_report.total_bytes += UDP_SEND_BUFSIZE;
			break;
		}
	}

	if (!retries)
	{
		/* Terminate this app */
		u64_t now = get_time_ms();
		u64_t diff_ms = now - client.start_time;
		xil_printf("Too many udp_send() retries, ");
		xil_printf("Terminating application\n\r");
		udp_conn_report(diff_ms, UDP_DONE_CLIENT);
		xil_printf("UDP test failed\n\r");
		udp_remove(pcb_for_motor_test);
		pcb_for_motor_test = NULL;
	}

	pbuf_free(packet);
	/* For ZynqMP SGMII, At high speed,
	 * "pack dropped, no space" issue observed.
	 * To avoid this, added delay of 2us between each
	 * packets.
	 */
#if defined (__aarch64__) && defined (XLWIP_CONFIG_INCLUDE_AXI_ETHERNET_DMA)
	usleep(2);
#endif /* __aarch64__ */

#endif
}

#ifdef PERF_DEBUG
int udp_send_boot_image(char* send_data, uint32_t data_size)
{
	int packet_size = 1330;
	size_t sent_size = 0;
	int retries = 10;
	int err_count = 0;
	while(sent_size < data_size - packet_size * 2)
	{
		usleep(100);
		struct pbuf *packet = pbuf_alloc(PBUF_TRANSPORT, packet_size, PBUF_POOL);
		int *payload = (int*) (packet->payload);

		if (!packet)
		{
			xil_printf("error allocating pbuf to send\r\n");
			return;
		}
		else
		{
			memcpy(&payload[0], (void *) &send_data[sent_size], packet_size);
		}

		while (retries)
		{
			err_t err = udp_send(pcb_for_test, packet);

			if (err != ERR_OK)
			{
				xil_printf("Error on udp_send: %d\r\n", err);
				retries--;
				usleep(100);
			}
			else
			{
				client.total_bytes += UDP_SEND_BUFSIZE;
				client.cnt_datagrams++;
				client.i_report.total_bytes += UDP_SEND_BUFSIZE;
				break;
			}
		}

		if (!retries)
		{
			err_count++;
			/* Terminate this app */
			u64_t now = get_time_ms();
			u64_t diff_ms = now - client.start_time;
			xil_printf("Too many udp_send() retries, ");
			xil_printf("Terminating application\n\r");
			udp_conn_report(diff_ms, UDP_DONE_CLIENT);
			xil_printf("UDP test failed\n\r");
			udp_remove(pcb_for_test);
			pcb_for_test = NULL;
		}



		pbuf_free(packet);
		sent_size+=packet_size;
	}
	/* For ZynqMP SGMII, At high speed,
	 * "pack dropped, no space" issue observed.
	 * To avoid this, added delay of 2us between each
	 * packets.
	 */
#if defined (__aarch64__) && defined (XLWIP_CONFIG_INCLUDE_AXI_ETHERNET_DMA)
	usleep(2);
#endif /* __aarch64__ */

	if(err_count > 0)
		return -1;
	else
		return 0;
}
#endif

#if LWIP_IPV6==1
static void print_ipv6(char *msg, ip_addr_t *ip)
{
	print(msg);
	xil_printf(" %s\n\r", inet6_ntoa(*ip));
}
#else

void print_ip(char *msg, ip_addr_t *ip)
{
	print(msg);
	xil_printf("%d.%d.%d.%d\r\n", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));
}
void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
	print_ip("Board IP:       ", ip);
	print_ip("Netmask :       ", mask);
	print_ip("Gateway :       ", gw);
}
void assign_default_ip(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{
	int err;

	xil_printf("Configuring default IP %s \r\n", DEFAULT_IP_ADDRESS);

	err = inet_aton(DEFAULT_IP_ADDRESS, ip);
	if(!err)
		xil_printf("Invalid default IP address: %d\r\n", err);

	err = inet_aton(DEFAULT_IP_MASK, mask);
	if(!err)
		xil_printf("Invalid default IP MASK: %d\r\n", err);

	err = inet_aton(DEFAULT_GW_ADDRESS, gw);
	if(!err)
		xil_printf("Invalid default gateway address: %d\r\n", err);
}
int assign_ip(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw, struct LidarState* lidar_state_)
{
	int err = 0;

	xil_printf("Configuring default IP %s \r\n", lidar_state_->network_setting_.ip);

	err = inet_aton(lidar_state_->network_setting_.ip, ip);
	if(!err)
		xil_printf("Invalid default IP address: %d\r\n", err);

	err = inet_aton(lidar_state_->network_setting_.subnet, mask);
	if(!err)
		xil_printf("Invalid default IP MASK: %d\r\n", err);

	if(strlen(lidar_state_->network_setting_.gateway) != 0)
		err = inet_aton(lidar_state_->network_setting_.gateway, gw);

	return err;
}
#endif /* LWIP_IPV6 */
