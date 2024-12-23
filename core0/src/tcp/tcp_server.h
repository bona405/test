/*
 * Copyright (C) 2018 - 2019 Xilinx, Inc.
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

#ifndef __TCP_PERF_SERVER_H_
#define __TCP_PERF_SERVER_H_

#include "lwipopts.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "xil_printf.h"
#include "../platform/platform.h"
#include "../packet/packet.h"
#include "../definitions/definitions.h"
#include "../../../common/src/api_sw/models/fov_data_set.h"

/* used as type of print */
enum tcp_measure_t {
	TCP_BYTES,
	TCP_SPEED
};

/* Report type */
enum tcp_report_type {
	/* The Intermediate report */
	TCP_INTER_REPORT,
	/* The server side test is done */
	TCP_DONE_SERVER,
	/* Remote side aborted the test */
	TCP_ABORTED_REMOTE
};

struct tcp_interim_report {
	u64_t start_time;
	u64_t last_report_time;
	u32_t total_bytes;
	u32_t report_interval_time;
};

struct tcp_perf_stats {
	u8_t client_id;
	u64_t start_time;
	u64_t end_time;
	u64_t total_bytes;
	struct tcp_interim_report i_report;
};

/* seconds between periodic bandwidth reports */
#define INTERIM_REPORT_INTERVAL 5

/* server port to listen on/connect to */
#define TCP_CONN_PORT 5001

#define TCP_SEND_BUFCNT 6
#define TCP_SEND_BUFSIZE (TCP_SEND_BUFCNT*TCP_MSS)

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
#define MAX_SEND_RETRY 10

/* Number of parallel UDP clients */
#define NUM_OF_PARALLEL_CLIENTS 1
static DataPacket tcp_packet;
void start_tcp_application(int port);
int is_send_buf_available();
unsigned char tcp_send_packet();
void insert_data_to_sndbuf(int packet_id, volatile RawDataBlock* blockData, uint8_t packet_index,
		int pack_cnt, u32 fov_data_block_count);
#endif /* __TCP_PERF_SERVER_H_ */
