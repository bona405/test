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

#ifndef __UDP_PERF_CLIENT_H_
#define __UDP_PERF_CLIENT_H_

#include "lwipopts.h"
#include "xlwipconfig.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/udp.h"
#include "lwip/inet.h"
#include "xil_printf.h"
#include "../platform/platform.h"
#include "../definitions/definitions.h"
#include "../packet/packet.h"
#include "../../../common/src/api_sw/models/machine_info.h"
#include "../web/http_response.h"
#include "../../../common/src/api_hw/api_common.h"

extern int inten_filter;
/* used as type of print */
enum measure_t
{
	BYTES, SPEED
};

/* Report Type */
enum report_type
{
	/* The Intermediate report */
	INTER_REPORT,
	/* The client side test is done */
	UDP_DONE_CLIENT,
	/* Remote side aborted the test */
	UDP_ABORTED_REMOTE
};

struct interim_report
{
	u64_t start_time;
	u64_t last_report_time;
	u32_t total_bytes;
};

struct perf_stats
{
	u8_t client_id;
	u64_t start_time;
	u64_t total_bytes;
	u64_t cnt_datagrams;

	u64_t cnt_dropped_datagrams;
	u32_t cnt_out_of_order_datagrams;
	s32_t expected_datagram_id;
	struct interim_report i_report;
};

/* seconds between periodic bandwidth reports */
#define INTERIM_REPORT_INTERVAL 5

/* Client port to connect */
#define UDP_CONN_PORT 5001

/* time in seconds to transmit packets */
#define UDP_TIME_INTERVAL 200

/* Server to connect with */
#define UDP_SERVER_IP_ADDRESS "192.168.1.255"
//#define UDP_SERVER_IP_ADDRESS "255.255.255.255"

/* UDP buffer length in bytes */
#define UDP_SEND_BUFSIZE 1440
//#define UDP_SEND_BUFSIZE 2000

/* MAX UDP send retries */
#define MAX_SEND_RETRY 5

/* Number of parallel UDP clients */
#define NUM_OF_PARALLEL_CLIENTS 1

/* Report interval time in ms */
#define REPORT_INTERVAL_TIME (INTERIM_REPORT_INTERVAL * 1000)

/* End time in ms */
#define END_TIME (UDP_TIME_INTERVAL * 1000)

#if defined(G32_ES_TEST)
void udp_send_packet(int packet_id, RawDataBlock* blockData, uint32_t packet_index, u32 motorRPM,u32 VoltageData,u32 Voltagefraction, uint8_t (*intensity)[CHANNEL_SIZE], u32 fov_data_block_count);
void udp_send_packet_a3(u32 packet_id, u32 send_packet_count, RawDataBlock* blockData, u32 motorRPM,u32 VoltageData,u32 Voltagefraction, uint8_t (*intensity)[CHANNEL_SIZE], u32 send_start_azimuth_index, u32 send_finish_azimuth_index, u8 top_bottom_side);
#else
void udp_send_packet(int packet_id, RawDataBlock* blockData, uint32_t packet_index, uint8_t frame_rate, uint8_t vertical_angle, uint8_t (*intensity)[CHANNEL_SIZE], u32 fov_data_block_count);
void udp_send_packet_a3(u32 packet_id, u32 send_packet_count, RawDataBlock* blockData, uint8_t frame_rate, uint8_t vertical_angle, uint8_t (*intensity)[CHANNEL_SIZE], u32 send_start_azimuth_index, u32 send_finish_azimuth_index, u8 top_bottom_side);
#endif
void start_udp_application(int port);
void start_udp_application_for_test(int port);
void start_udp_application_for_logging(int port);
void udp_send_packet_for_test(char* send_data);
void udp_send_packet_for_logging(char* send_data);
void platform_enable_interrupts(void);
void start_applications(void);
void print_headers(void);
void transfer_data(void);
void print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);
int assign_ip(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw, struct LidarState* lidar_state_);
void assign_default_ip(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);
void udp_send_packet_a4(u32 packet_id, u32 send_packet_count, RawDataBlock* blockData, uint8_t frame_rate, uint8_t vertical_angle, uint8_t (*intensity)[CHANNEL_SIZE], u32 send_start_azimuth_index, u32 send_finish_azimuth_index, u8 top_bottom_side, float z_mean_float);
static DataPacket udp_packet;
uint8_t _recv_buffer[100];
int _recv_size;

char* convert_udp_server_ip;

#ifdef PERF_DEBUG
int udp_send_boot_image(char* send_data, uint32_t data_size);
#endif

#ifdef ENABLE_MOTOR_TEST
void start_udp_application_for_motor_test(int port);
void udp_send_packet_for_motor_test(char* send_data);
#endif

#endif /* __UDP_PERF_CLIENT_H_ */

