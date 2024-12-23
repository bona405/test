/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"


#include <string.h>
#include <stdlib.h>
#include <xstatus.h>
#include <xil_io.h>

#include "lwip/inet.h"
#include <pthread.h>
#include "http_response.h"
#ifndef __PPC__
#include "xil_printf.h"
#endif
#include "../../../common/src/api_sw/flash/flash.h"
#include "../../../common/src/api_sw/mfs/mfs_config.h"
#include "../../../common/src/api_sw/models/machine_info.h"
#include "../../../common/src/api_sw/container/hash_table.h"
#include "web_server.h"

//int percent_val = 0;
//uint8_t is_golden_img = 0;
//int reset_ret_val = 0;


char boundary[] = "----WebKitFormBoundary";
web_param progress[2] = { {"restore", "0"}, {"percentage", "0"}};
web_param sysinfo_param[20] = { {"serial", "0"}, {"mdate", "0"}, {"firmwarever", "0"}, {"fpgaver", "0"}, {"udate", "0"}, {"mac", "0"}, {"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"golden_ver", "0"}, {"golden_u_date", "0"}, {"noise_percent", "0"}, {"noiseon", "0"}, {"voltageon", "0"}, {
		"vth_sensitivity", "0"}, {"valid_point_cnt", "0"}, {"noise_cnt_limit", "0"}, {"propagation_delay", "0"}, {"upper_offset", "0"}, {"lower_offset", "0"}, };
web_param sysinfo_lidar_delay_param[32] = { {"ch1", "52"}, {"ch2", "57"}, {"ch3", "28"}, {"ch4", "26"}, {"ch5", "33"}, {"ch6", "40"}, {"ch7", "45"}, {"ch8", "24"}, {"ch9", "58"}, {"ch10", "65"}, {"ch11", "11"}, {"ch12", "0"}, {"ch13", "13"}, {"ch14", "5"}, {"ch15", "5"}, {"ch16", "0"},
		{"ch17", "52"}, {"ch18", "57"}, {"ch19", "28"}, {"ch20", "26"}, {"ch21", "33"}, {"ch22", "40"}, {"ch23", "45"}, {"ch24", "35"}, {"ch25", "58"}, {"ch26", "65"}, {"ch27", "10"}, {"ch28", "0"}, {"ch29", "12"}, {"ch30", "5"}, {"ch31", "5"}, {"ch32", "0"}};
web_param sysinfo_lidar_th_param[16] = { {"ch1", "0"}, {"ch2", "0"}, {"ch3", "0"}, {"ch4", "0"}, {"ch5", "0"}, {"ch6", "0"}, {"ch7", "0"}, {"ch8", "0"}, {"ch9", "0"}, {"ch10", "0"}, {"ch11", "0"}, {"ch12", "0"}, {"ch13", "0"}, {"ch14", "0"}, {"ch15", "0"}, {"ch16", "0"}, };
web_param hwinfo_param[15] = { {"laseron", "0"}, {"echo", "0"}, {"rpm", "0"}, {"fovstart", "0"}, {"fovend", "0"}, {"phaselock", "0"}, {"dhcpon", "0"}, {"protocol", "0"}, {"ip", "0"}, {"subnet", "0"}, {"gate", "0"}, {"noiseon", "0"}, {"voltageon", "0"}, {"interferenceon", "0"}, {"port", "0"}};
web_param diagnostics_param1[9] = { {"hv", "0"}, {"td", "0"}, {"temp", "0"}, {"v5", "0"}, {"v25", "0"}, {"v33", "0"}, {"v5raw", "0"}, {"v15", "0"}, {"vhd", "0"}};
web_param diagnostics_param2[9] = { {"hv", "0"}, {"td", "0"}, {"temp", "0"}, {"v5", "0"}, {"v25", "0"}, {"v33", "0"}, {"v5raw", "0"}, {"v15", "0"}, {"vhd", "0"}};
web_param diagnostics_param3[9] = { {"hv", "0"}, {"td", "0"}, {"temp", "0"}, {"v5", "0"}, {"v25", "0"}, {"v33", "0"}, {"v5raw", "0"}, {"v15", "0"}, {"vhd", "0"}};
web_param diagnostics_param4[9] = { {"hv", "0"}, {"td", "0"}, {"temp", "0"}, {"v5", "0"}, {"v25", "0"}, {"v33", "0"}, {"v5raw", "0"}, {"v15", "0"}, {"vhd", "0"}};
web_param post_response[1] = { {"Send", "OK"}};
web_param pwd_response[1] = { {"pwdcheck", "0"}};

int do_http_post(struct tcp_pcb *pcb, char *req, int rlen, int response_wait)
{
	memcpy(req_buf, req, rlen);
	char post_command[100] = {0, };
	GetCommand(post_command, req);
	int cmd_hash_value = hash_func(post_command);

	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
	if(web_cmd_string_to_enum_map[cmd_hash_value] != 0)
		if(web_cmd_string_to_enum_map[cmd_hash_value]->value < NumberOfePostCmd)
			web_cmd_processing_fp[web_cmd_string_to_enum_map[cmd_hash_value]->value](pcb, req_buf, rlen, response_wait);

//	IS_LIDAR_STATE_UPDATED = 1;
//	memcpy((void *)SHARED_MEM_OFFSET_FOR_LIDAR_STATE, (void *)(&lidar_state_), sizeof(lidar_state_));
	return 0;
}


/* respond for a file GET request */
int do_http_get(struct tcp_pcb *pcb, char *req, int rlen)
{
	int BUFSIZE = 1400;
	char filename[MAX_FILENAME]; //256
	char buf[BUFSIZE];
	signed int fsize, hlen, n;
	int fd;
	char *fext;
	err_t err;


//	xil_printf("GET GET 1111\r\n");

	/* determine file name */
	extract_file_name(filename, req, rlen, MAX_FILENAME);

	if(strstr(filename, "pwd") != 0)
	{
		xil_printf("pwd");
	}
	/* respond with 404 if not present */
	if(mfs_exists_file(filename) != 1)
	{
		xil_printf("requested file %s not found, returning 404 123123\r\n", filename);
		do_404(pcb, req, rlen);
		return -1;
	}

	/* respond with correct file */

	/* debug statement on UART */
	xil_printf("http GET: %s\r\n", filename);

	/* get a pointer to file extension */
	fext = get_file_extension((char*)filename);

	fd = mfs_file_open(filename, MFS_MODE_READ);
	if(fd == -1)
	{
		platform_init_fs();
		extract_file_name(filename, req, rlen, MAX_FILENAME);
		if(mfs_exists_file(filename) != 1)
		{
			xil_printf("requested file %s not found, returning 404 222222\r\n", filename);
			do_404(pcb, req, rlen);
			return -1;
		}
		fext = get_file_extension((char*)filename);
		fd = mfs_file_open(filename, MFS_MODE_READ);
		return -1;
	}

	/* obtain file size,
	 * note that lseek with offset 0, MFS_SEEK_END does not move file pointer */
	fsize = mfs_file_lseek(fd, 0, MFS_SEEK_END);
	if(fsize == -1)
	{
		xil_printf("\r\nFile Read Error\r\n");
		return -1;
	}

	/* write the http headers */
	hlen = generate_http_header((char *)buf, fext, fsize);
	if((err = tcp_write(pcb, buf, hlen, 1)) != ERR_OK)
	{
		xil_printf("error (%d) writing http header to socket\r\n", err);
		xil_printf("attempted to write #bytes = %d, tcp_sndbuf = %d\r\n", hlen, tcp_sndbuf(pcb));
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	/* now write the file */
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
			a->fd = fd;
			a->fsize = fsize;
			return -1;
		}

		n = mfs_file_read(fd, (char *)buf, BUFSIZE);

		if((err = tcp_write(pcb, buf, n, 1)) != ERR_OK)
		{
			xil_printf("error writing file (%s) to socket, remaining unwritten bytes = %d\r\n", filename, fsize - n);
			xil_printf("attempted to lwip_write %d bytes, tcp write error = %d\r\n", n, err);
			break;
		}
		if(fsize >= n)
			fsize -= n;
		else
			fsize = 0;
	}

	mfs_file_close(fd);
//	tcp_close(pcb)

	return 0;
}

enum http_req_type
{
	HTTP_GET, HTTP_POST, HTTP_UNKNOWN
};
enum http_req_type decode_http_request(char *req, int l)
{
	char *get_str = "GET";
	char *post_str = "POST";

	if(!strncmp(req, get_str, strlen(get_str)))
		return HTTP_GET;

	if(!strncmp(req, post_str, strlen(post_str)))
		return HTTP_POST;

	return HTTP_UNKNOWN;
}

void dump_payload(char *p, int len)
{
	int i, j;

	for(i = 0; i < len ; i += 16)
	{
		for(j = 0; j < 16 ; j++)
		{
			xil_printf("%c ", p[i + j]);
		}
		xil_printf("\r\n");
	}
	xil_printf("total len = %d\r\n", len);
}

char file_extension[20] = {0,};

int parse_payload_header(struct tcp_pcb *pcb, char *req, int rlen, enum eFileRecvStatus *recv_status)
{
//	size_t ptr_payload_end = strlen(req);
	char* ptr_payload_start = req;
	char *ptr_boundary_key = strstr(req, boundary);
	ptr_boundary_key = strtok(ptr_boundary_key, "\r\n");

	if(strncmp(ptr_boundary_key, boundary, sizeof(boundary) - 1) == 0)
	{
		*recv_status = PAYLOAD_HEADER_PARSED;
		xil_printf("PTR BOUNDARY OK\r\n");
	}
	else
	{
		xil_printf("PAYLOAD_HEADER_PARSED ERR\r\n");
		return -1;
	}

	char *ptr_file_size = strstr(strtok(NULL, ""), "file_size");

	ptr_file_size = strtok(ptr_file_size, "\n");
	ptr_file_size = strtok(NULL, "\n");
	recv_file_size = atoi(strtok(NULL, "\r"));
	xil_printf("recv file size : %d\r\n", recv_file_size);

	char *ptr_file_name = strstr(strtok(NULL, ""), "filename");
	ptr_file_name = strtok(ptr_file_name, "\"");
	ptr_file_name = strtok(NULL, "\"");

	char* ptr_firm_ver = strstr(ptr_file_name, "v");
	if(ptr_firm_ver != NULL)
	{
		if(is_golden_img)
		{
			char* ptr_firm_ver_end = strstr(ptr_firm_ver, "bin");
			if(ptr_firm_ver_end != NULL)
				strncpy(lidar_state_.lidar_info_.golden_ver, ptr_firm_ver + 1, ptr_firm_ver_end - ptr_firm_ver - 2);
		}
		else
		{
			char* ptr_firm_ver_end = strstr(ptr_firm_ver, "_");
			if(ptr_firm_ver_end != NULL)
				strncpy(lidar_state_.lidar_info_.firmware_ver, ptr_firm_ver + 1, ptr_firm_ver_end - ptr_firm_ver - 1);
		}

	}

	char* ptr_fpga_ver_t = strstr(ptr_firm_ver, "v");
	char* ptr_fpga_ver = strstr(ptr_fpga_ver_t + 1, "v");

	if(ptr_fpga_ver != NULL)
	{
		char* ptr_fpga_ver_end = strstr(ptr_fpga_ver, "bin");
		if(ptr_fpga_ver_end != NULL)
		{
			if(!is_golden_img)
				strncpy(lidar_state_.lidar_info_.fpga_ver, ptr_fpga_ver + 1, ptr_fpga_ver_end - ptr_fpga_ver - 2);
		}
	}

	xil_printf("file name : %s \r\n", ptr_file_name);
	char *ptr_file_array = strstr(strtok(NULL, ""), "\r\n\r\n");
	ptr_file_array += strlen("\r\n\r\n");

	xil_printf("data length : %d \r\n", strlen(ptr_file_array));
	xil_printf("data length2 : %d \r\n", rlen - (ptr_file_array - ptr_payload_start));

//	size_t recv_data_size = rlen - (ptr_file_array - ptr_payload_start);
//
//	memcpy(Total_buf, ptr_file_array, recv_data_size);
//	recved_size += recv_data_size;

	//wonju csv

	strcpy(file_extension, get_file_extension(ptr_file_name));
//			strtok(ptr_file_name, ".");
//	file_extension = strtok(NULL, "\r\n");
	size_t recv_data_size = 0;
	char* ptr_webkit_end = strstr(ptr_file_array, "\r\n\r\n------WebKitFormBoundary");
	if(ptr_webkit_end == NULL)
	{
		recv_data_size = rlen - (ptr_file_array - ptr_payload_start);
		memcpy(Total_buf, ptr_file_array, recv_data_size);
		recved_size += recv_data_size;
	}
	else
	{
		recv_data_size = ptr_webkit_end - ptr_file_array;
		memcpy(Total_buf, ptr_file_array, recv_data_size);
		recved_size += recv_data_size;
	}

	return 0;
}

char current_req[4096];

//int parse_csv_data(struct tcp_pcb *pcb, char *req, int rlen, enum eFileRecvStatus *recv_status)
//{
//	memcpy(current_req, req, strlen(req));
//	if(recved_size + rlen <= recv_file_size)
//	{
//		memcpy(Total_buf + recved_size, req, rlen);
//		recved_size += rlen;
//		percent_val = (int)(((float)recved_size / content_length) * 50);
//	}
//	else
//	{
//		memcpy(Total_buf + recved_size, req, recv_file_size - recved_size);
//		recved_size += recv_file_size - recved_size;
//
//		if(recved_size != recv_file_size)
//		{
//			xil_printf("recved_size != recv_file_size\r\n");
//			return -1;
//		}
//
//		int BUFSIZE = 1024;
//		char buf[BUFSIZE];
//		generate_http_header(buf, NULL, 0);
//		if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
//		{
//			xil_printf("error writing http POST response to socket\n\r");
//			xil_printf("http header = %s\r\n", buf);
//		}
//
//		int32_t row = 0;
//		int32_t col = 0;
//		double offset = 0.0;
//
//
//		char* value;
//		int32_t col_number = 0;
//		if(Total_buf != NULL)
//		{
//			value = strtok(Total_buf, "\n");
//			while(value != NULL)
//			{
//				value = strtok(NULL, ",");
//				col = atoi(value);
//				col_number++;
//				value = strtok(NULL, ",");
//				row = atoi(value);
//				value = strtok(NULL, "\r");
//				offset = atof(value);
//				value = strtok(NULL, "\n");
//
//
//
//
//			}
//		}
//
//		is_golden_img = 0;
//		percent_val = 100;
//		file_upload_flag = 0;
//		*recv_status = FILE_RECV_SEUCCES;
//		file_download_flag = 0;
//		content_length = 0;
//		recved_size = 0;
//		recv_file_size = 0;
//	}
//}

// 240630 hhj
int parse_csv_data(struct tcp_pcb *pcb, char *req, int rlen, enum eFileRecvStatus *recv_status)
{
	memcpy(current_req, req, strlen(req));
	if(recved_size + rlen <= recv_file_size)
	{
		memcpy(Total_buf + recved_size, req, rlen);
		recved_size += rlen;
		percent_val = (int)(((float)recved_size / content_length) * 50);
	}
	else
	{
		memcpy(Total_buf + recved_size, req, recv_file_size - recved_size);
		recved_size += recv_file_size - recved_size;

		if(recved_size != recv_file_size)
		{
			xil_printf("recved_size != recv_file_size\r\n");
			return -1;
		}

		int BUFSIZE = 1024;
		char buf[BUFSIZE];
		generate_http_header(buf, NULL, 0);
		if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
		{
			xil_printf("error writing http POST response to socket\n\r");
			xil_printf("http header = %s\r\n", buf);
		}

//		int32_t row = 0;
//		int32_t col = 0;
//		double offset = 0.0;

		volatile int32_t row = 0;
		volatile int32_t col = 0;
		volatile float offset = 0.0;


		char* value;
//		int32_t col_number = 0;
		volatile uint32_t col_number = 0;
		if(Total_buf != NULL)
		{
			value = strtok(Total_buf, "\n");
			while(value != NULL)
			{
				value = strtok(NULL, ",");
				col = atoi(value);			// azimuth

				value = strtok(NULL, ",");	// channel
				row = atoi(value);
				value = strtok(NULL, "\r\n"); // offset
				offset = atof(value)*256;
//				value = strtok(NULL, "\n");

				size_t tmp_azimuth = col*3;   // 0~683
				size_t tmp_channel= row;   // 0~32
				uint8_t isTop = tmp_channel % 2;
				uint8_t ch = tmp_channel >> 1;

				if(dark_area_state_.darkarea_setting_.total_azimuth_cnt > 0)
				{
					size_t data_block_idx = tmp_azimuth + (isTop*2052);
//					azi_cal_.AziOffsets_[data_block_idx].ch_datas[ch] = (int)(offset*256);
//					azi_cal_.AziOffsets_[data_block_idx+1].ch_datas[ch] = (int)(offset*256);
//					azi_cal_.AziOffsets_[data_block_idx+2].ch_datas[ch] = (int)(offset*256);

					azi_cal_.azimuth_calibration_info[col_number].col = data_block_idx;
					azi_cal_.azimuth_calibration_info[col_number].row = ch;
					azi_cal_.azimuth_calibration_info[col_number].offset = offset;
					col_number++;
				}

			}
			if(col_number > 0)
			{
				azi_cal_.azimuth_calibration_setting.calibration_info_count = col_number;
				int Status = InitLinearQspiFlash();
				EraseAzioffset();
				Status = LinearQspiFlashWriteForAzioffset((char*) &azi_cal_, AZI_OFFSET_SIZE);
			}

		}

		is_golden_img = 0;
		percent_val = 100;
		file_upload_flag = 0;
		*recv_status = FILE_RECV_SEUCCES;
		file_download_flag = 0;
		content_length = 0;
		recved_size = 0;
		recv_file_size = 0;
	}
}

// 240912
int parse_csv_data2(struct tcp_pcb *pcb, char *req, int rlen, enum eFileRecvStatus *recv_status)
{
	memcpy(current_req, req, strlen(req));
	if(recved_size + rlen <= recv_file_size)
	{
		memcpy(Total_buf + recved_size, req, rlen);
		recved_size += rlen;
		percent_val = (int)(((float)recved_size / content_length) * 50);
	}
	else
	{
		memcpy(Total_buf + recved_size, req, recv_file_size - recved_size);
		recved_size += recv_file_size - recved_size;

		if(recved_size != recv_file_size)
		{
			xil_printf("recved_size != recv_file_size\r\n");
			return -1;
		}

		int BUFSIZE = 1024;
		char buf[BUFSIZE];
		generate_http_header(buf, NULL, 0);
		if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
		{
			xil_printf("error writing http POST response to socket\n\r");
			xil_printf("http header = %s\r\n", buf);
		}


		char* value;

		if(Total_buf != NULL)
		{
			int ch_cot = 0;

			int group_cot = 0;

			int nowidx = 0;

			value = strtok(Total_buf, ",");

			while(value != NULL)
			{
				azi_offsets_.AziGroup[group_cot].Offsets[nowidx].ch[ch_cot] = atoi(value);
				nowidx++;

				if(nowidx == 683)
				{
					value = strtok(NULL, "\n");
					continue;
				}
				if(nowidx == 684)
				{
					nowidx = 0;
					ch_cot++;

					if(ch_cot == 32)
					{
						ch_cot = 0;
						group_cot++;
						if(group_cot == 4)
						{
							break;
						}
					}
				}
				value = strtok(NULL, ",");
			}

			//reverse datas -60 ~ 60 -> 60 ~ -60
//			for(int groupidx = 0; groupidx < 4; ++groupidx)
//			{
//				for(int aziidx = 0; aziidx < 342; ++aziidx)
//				{
//					for(int ch = 0; ch < 32; ++ch)
//					{
//						int temp;
//
//						temp = azi_offsets_.AziGroup[groupidx].Offsets[aziidx].ch[ch];
//
//						 azi_offsets_.AziGroup[groupidx].Offsets[aziidx].ch[ch] =
//								 azi_offsets_.AziGroup[groupidx].Offsets[684 - 1 - aziidx].ch[ch];
//
//						 azi_offsets_.AziGroup[groupidx].Offsets[684 - 1 - aziidx].ch[ch] = temp;
//					}
//				}
//			}

			EraseAzioffset();
			LinearQspiFlashWriteForAzioffset((char*) &azi_offsets_, AZI_OFFSET_SIZE);



		}

		is_golden_img = 0;
		percent_val = 100;
		file_upload_flag = 0;
		*recv_status = FILE_RECV_SEUCCES;
		file_download_flag = 0;
		content_length = 0;
		recved_size = 0;
		recv_file_size = 0;
	}
}


// 241018
int parse_csv_data3(struct tcp_pcb *pcb, char *req, int rlen, enum eFileRecvStatus *recv_status)
{
	memcpy(current_req, req, strlen(req));
	if(recved_size + rlen <= recv_file_size)
	{
		memcpy(Total_buf + recved_size, req, rlen);
		recved_size += rlen;
		percent_val = (int)(((float)recved_size / content_length) * 50);
	}
	else
	{
		memcpy(Total_buf + recved_size, req, recv_file_size - recved_size);
		recved_size += recv_file_size - recved_size;

		if(recved_size != recv_file_size)
		{
			xil_printf("recved_size != recv_file_size\r\n");
			return -1;
		}

		int BUFSIZE = 1024;
		char buf[BUFSIZE];
		generate_http_header(buf, NULL, 0);
		if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
		{
			xil_printf("error writing http POST response to socket\n\r");
			xil_printf("http header = %s\r\n", buf);
		}


		char* value;

		if(Total_buf != NULL)
		{
			int ch_cot = 0;

			int group_cot = 0;

			int nowidx = 0;

			value = strtok(Total_buf, ",");

			while(value != NULL)
			{
				azi_offsets_.AziGroup[group_cot].Offsets[nowidx].ch[ch_cot] = atoi(value);
				nowidx++;

				if(nowidx == 683)
				{
					value = strtok(NULL, "\n");
					continue;
				}
				if(nowidx == 684)
				{
					nowidx = 0;
					ch_cot++;

					if(ch_cot == 32)
					{
						ch_cot = 0;
						group_cot++;
						if(group_cot == 4)
						{
							break;
						}
					}
				}
				value = strtok(NULL, ",");
			}

			//reverse datas -60 ~ 60 -> 60 ~ -60
//			for(int groupidx = 0; groupidx < 4; ++groupidx)
//			{
//				for(int aziidx = 0; aziidx < 342; ++aziidx)
//				{
//					for(int ch = 0; ch < 32; ++ch)
//					{
//						int temp;
//
//						temp = azi_offsets_.AziGroup[groupidx].Offsets[aziidx].ch[ch];
//
//						 azi_offsets_.AziGroup[groupidx].Offsets[aziidx].ch[ch] =
//								 azi_offsets_.AziGroup[groupidx].Offsets[684 - 1 - aziidx].ch[ch];
//
//						 azi_offsets_.AziGroup[groupidx].Offsets[684 - 1 - aziidx].ch[ch] = temp;
//					}
//				}
//			}

			EraseTemperoffset();
			LinearQspiFlashWriteForTemperoffset((char*) &temper_offsets_, TEMPER_OFFSET_SIZE);



		}

		is_golden_img = 0;
		percent_val = 100;
		file_upload_flag = 0;
		*recv_status = FILE_RECV_SEUCCES;
		file_download_flag = 0;
		content_length = 0;
		recved_size = 0;
		recv_file_size = 0;
	}
}




uint32_t SetBootImageSettingsToFlash(char* boot_image_arr, struct LidarState* lidar_state)
{
	if(boot_image_arr[0] != 0x7b) // check {
		return 0;

	char boot_image_info_arr[MAX_BOOT_SETTING_SIZE] = {0, };
	memset(boot_image_info_arr, 0x00, MAX_BOOT_SETTING_SIZE);
	strncpy(boot_image_info_arr, boot_image_arr, MAX_BOOT_SETTING_SIZE - 1);

	char result_value[40] = {0,};
	GetJsonStringValueByKey(result_value, boot_image_info_arr, "name");
	strncpy(lidar_state_.lidar_info_.boot_image_name, result_value, sizeof lidar_state_.lidar_info_.boot_image_name);

	memset(result_value, 0x0, sizeof result_value);
	GetJsonStringValueByKey(result_value, boot_image_info_arr, "mfgDate");
	strncpy(lidar_state_.lidar_info_.manufacturing_date, result_value, sizeof lidar_state_.lidar_info_.manufacturing_date);

	char* json_end_ptr = strchr(boot_image_info_arr, '}');
	if(json_end_ptr == NULL)
		return 0;
	else
		return (json_end_ptr - boot_image_info_arr + 1);
}

int parse_payload_data(struct tcp_pcb *pcb, char *req, int rlen, enum eFileRecvStatus *recv_status)
{

	memcpy(current_req, req, strlen(req));
	if(recved_size + rlen <= recv_file_size)
	{
		memcpy(Total_buf + recved_size, req, rlen);
		recved_size += rlen;
		percent_val = (int)(((float)recved_size / content_length) * 50);
	}
	else
	{

		e_file_recv_status = PAYLOAD_DATA_PARSED;



		memcpy(Total_buf + recved_size, req, recv_file_size - recved_size);
//		for(int ii = (recv_file_size - recved_size) ; ii < rlen ; ii++)
//			xil_printf("%x ", req[ii]);
		recved_size += recv_file_size - recved_size;

//		percent_val = 60;

		if(recved_size != recv_file_size)
		{
			xil_printf("recved_size != recv_file_size\r\n");
			return -1;
		}

		int BUFSIZE = 1024;
		char buf[BUFSIZE];
		generate_http_header(buf, NULL, 0);
		if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
		{
			xil_printf("error writing http POST response to socket\n\r");
			xil_printf("http header = %s\r\n", buf);
		}

//		memset(pre_boot_image, 0x00, sizeof(pre_boot_image));
////		Status = ReadFlash(pre_boot_image, 0x8000 * 200);
//		Status = ReadFlash(pre_boot_image, 0x5FFFFF);
//		if (Status != XST_SUCCESS)
//		{
//			xil_printf("Flash Read Failed\r\n");
//			return XST_FAILURE;
//		}
		if(is_golden_img == 0)
		{
			int Status = InitLinearQspiFlash();
			if(Status != XST_SUCCESS)
			{
				xil_printf("Flash Init Failed\r\n");
				return XST_FAILURE;
			}

//			memset(lidar_state_buffer_, 0x00, LIDAR_STATE_SIZE);
//			Status = ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
//			memcpy(&lidar_state_, lidar_state_buffer_, sizeof(struct LidarState));
//			if (Status != XST_SUCCESS)
//			{
//				xil_printf("Flash Read Failed\r\n");
//				return XST_FAILURE;
//			}

			//add secure boot code
//			EraseBootImgFlash();

//			GetBootImageSettings(Total_buf, &lidar_state_);


			uint32_t setting_json_size = 0;
//			uint32_t setting_json_size = SetBootImageSettingsToFlash(Total_buf, &lidar_state_);
			uint32_t hash_size = 32;

//			strncpy(lidar_state_.lidar_info_.boot_image_name, "G32A3230412", 12);
//
//			strncpy(lidar_state_.lidar_info_.manufacturing_date, "2023-04-05",10);

		    unsigned char magicnumber[4] = {0,};
		    strncpy(magicnumber, (unsigned char *)Total_buf + 36 + setting_json_size, 4);
		    if(strncmp(magicnumber, "XNLX", 4))
		    {
		    	//encrypted
//			    BYTE IV[] = { 191,3,255,196,44,3,77,84,12,87,147,124,37,167,240,65 };
//			    BYTE Key[] = { 53,212,149,108,10,211,182,75,200,249,98,19,178,181,149,238,252,232,244,100,19,17,50,30,129,123,156,118,51,113,181,135 };

			    BYTE IV[16] = {0x0,};
			    BYTE Key[32] = {0x0,};
			    ReadKey(Key);
			    ReadIV(IV);

			    int length = recv_file_size - (hash_size + setting_json_size);

			    //sha 256
			    BYTE buf_imagesha[SHA256_BLOCK_SIZE] = {0x0,};
			    for(int i = 0; i<SHA256_BLOCK_SIZE; i++)
			    {
			    	buf_imagesha[i] = (BYTE)*(Total_buf + setting_json_size + i);
			    }

			    unsigned char* decbuffer = (unsigned char*)malloc(sizeof(unsigned char) * length);

			    AES_CBC_Decrypt(decbuffer, Total_buf+(hash_size + setting_json_size), length, Key, IV);

			    //sha256 by decrypted
			    BYTE buf_sha[SHA256_BLOCK_SIZE];
			    BYTE hash1[SHA256_BLOCK_SIZE] = { 0, };
			    SHA256_CTX ctx;
			    int pass = 1;

			    sha256_init(&ctx);
			    sha256_update(&ctx, decbuffer, length);
			    sha256_final(&ctx, buf_sha);

			    pass = pass && !memcmp(hash1, buf_sha, SHA256_BLOCK_SIZE);

			    int shaflag = 0;

			    for (int i = 0; i < SHA256_BLOCK_SIZE; i++)
				{
			    	if(buf_sha[i] != buf_imagesha[i])
			    	{
//				    	printf("Fail %x", buf_sha[i]);
				    	shaflag = 1;
			    	}
				}
			    if(shaflag)
			    {
			    	//try Code_KEY_IV
				    BYTE Code_Key[32] = {78,157,158,76,177,43,99,107,218,211,76,50,50,72,211,61,107,74,131,100,51,134,185,93,6,228,68,191,228,41,234,167};
				    BYTE Code_IV[16] = {78,39,202,51,187,110,78,136,119,74,22,171,252,253,156,196};

				    AES_CBC_Decrypt(decbuffer, Total_buf+32, length, Code_Key, Code_IV);

				    pass = 1;

				    sha256_init(&ctx);
				    sha256_update(&ctx, decbuffer, length);
				    sha256_final(&ctx, buf_sha);

				    pass = pass && !memcmp(hash1, buf_sha, SHA256_BLOCK_SIZE);

				    shaflag = 0;

				    for (int i = 0; i < SHA256_BLOCK_SIZE; i++)
					{
				    	if(buf_sha[i] != buf_imagesha[i])
				    	{
	//				    	printf("Fail %x", buf_sha[i]);
					    	shaflag = 1;
				    	}
					}
				    if(shaflag)
				    {
					    free(decbuffer);
				    	return -1;
				    }
			    }
				EraseBootImgFlash();
			    Status = LinearQspiFlashWrite(decbuffer, length);
			    free(decbuffer);
		    }
		    else
		    {
		    	//not encrypted
				EraseBootImgFlash();
		    	Status = LinearQspiFlashWrite(Total_buf + setting_json_size, recv_file_size);
		    }

			//end secure boot code
//			Status = LinearQspiFlashWrite(Total_buf, recv_file_size);
			if(Status != XST_SUCCESS)
			{
				xil_printf("Flash Write Failed\r\n");
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

			EraseLidarInfo_Ch2();
			Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*)&lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

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
		}
		else if(is_golden_img == 1)
		{
			int Status = InitLinearQspiFlash();
			if(Status != XST_SUCCESS)
			{
				xil_printf("Flash Init Failed\r\n");
				return XST_FAILURE;
			}
			unsigned char magicnumber[4] = {0,};
			strncpy(magicnumber, (unsigned char *)Total_buf + 36, 4);
			if(strncmp(magicnumber, "XNLX", 4))
			{
				//encrypted
				BYTE IV[16] = {0x0,};
				BYTE Key[32] = {0x0,};
				ReadKey(Key);
				ReadIV(IV);


				int length = recv_file_size - 32;
			    //sha 256
			    BYTE buf_imagesha[SHA256_BLOCK_SIZE] = {0x0,};
			    for(int i = 0; i<SHA256_BLOCK_SIZE; i++)
			    {
			    	buf_imagesha[i] = (BYTE)Total_buf[i];
			    }

			    unsigned char* decbuffer = (unsigned char*)malloc(sizeof(unsigned char) * length);

			    AES_CBC_Decrypt(decbuffer, Total_buf+32, length, Key, IV);

			    //sha256 by decrypted
			    BYTE buf_sha[SHA256_BLOCK_SIZE];
			    BYTE hash1[SHA256_BLOCK_SIZE] = { 0, };
			    SHA256_CTX ctx;
			    int pass = 1;

			    sha256_init(&ctx);
			    sha256_update(&ctx, decbuffer, length);
			    sha256_final(&ctx, buf_sha);

			    pass = pass && !memcmp(hash1, buf_sha, SHA256_BLOCK_SIZE);

			    int shaflag = 0;

			    for (int i = 0; i < SHA256_BLOCK_SIZE; i++)
				{
			    	if(buf_sha[i] != buf_imagesha[i])
			    	{
//				    	printf("Fail %x", buf_sha[i]);
				    	shaflag = 1;
			    	}
				}
			    if(shaflag)
			    {
			    	//try Code_KEY_IV
				    BYTE Code_Key[32] = {78,157,158,76,177,43,99,107,218,211,76,50,50,72,211,61,107,74,131,100,51,134,185,93,6,228,68,191,228,41,234,167};
				    BYTE Code_IV[16] = {78,39,202,51,187,110,78,136,119,74,22,171,252,253,156,196};

				    AES_CBC_Decrypt(decbuffer, Total_buf+32, length, Code_Key, Code_IV);

				    pass = 1;

				    sha256_init(&ctx);
				    sha256_update(&ctx, decbuffer, length);
				    sha256_final(&ctx, buf_sha);

				    pass = pass && !memcmp(hash1, buf_sha, SHA256_BLOCK_SIZE);

				    shaflag = 0;

				    for (int i = 0; i < SHA256_BLOCK_SIZE; i++)
					{
				    	if(buf_sha[i] != buf_imagesha[i])
				    	{
	//				    	printf("Fail %x", buf_sha[i]);
					    	shaflag = 1;
				    	}
					}
				    if(shaflag)
				    {
					    free(decbuffer);
				    	return -1;
				    }
			    }

				if(Status != XST_SUCCESS)
				{
					xil_printf("Flash Init Failed\r\n");
					return XST_FAILURE;
				}
				EraseGoldenImgFlash();
			    Status = LinearQspiFlashWriteForGolden(decbuffer, length);
			    free(decbuffer);

			}
			else
			{
				//not encrypted
				EraseGoldenImgFlash();
				Status = LinearQspiFlashWriteForGolden(Total_buf, recv_file_size);
			}

//			EraseGoldenImgFlash();
//			Status = LinearQspiFlashWriteForGolden(Total_buf, recv_file_size);
			if(Status != XST_SUCCESS)
			{
				xil_printf("Flash Write Failed\r\n");
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

			EraseLidarInfo_Ch2();
			Status = LinearQspiFlashWriteForLidarInfo_Ch2((char*)&lidar_state_Ch2, LIDAR_STATE_SIZE_CH2);

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
		}

		percent_val = 90;

		xil_printf("Flash Write SUCCESS\r\n");
		is_golden_img = 0;
		percent_val = 100;
		file_upload_flag = 0;
		*recv_status = FILE_RECV_SEUCCES;
		file_download_flag = 0;
		content_length = 0;
		recved_size = 0;
		recv_file_size = 0;
		xil_printf("Flash Write Success !!! \r\n");
	}
	return 0;
}

int parse_post_cmd_data(struct tcp_pcb *pcb, char *req, int rlen, enum ePostCmdRecvStatus *recv_status)
{
	if(strstr(req, "json=") == NULL)
	{
		e_post_cmd_recv_status = NONE;
		return -1;
	}

	memcpy(post_cmd_data + post_cmd_recved_size, req, strlen(req));
	post_cmd_recved_size += strlen(req);
	if(post_cmd_recved_size < post_cmd_data_size)
	{
		return 1;
	}

	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
#if 1
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
#else
	char value[30] = {0, };
		char key[30] = "ch1";
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch1, 0x00, sizeof(lidar_state_Ch.delay_ch1));
		strncpy(lidar_state_Ch.delay_ch1, value, sizeof(lidar_state_Ch.delay_ch1));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch2");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch2, 0x00, sizeof(lidar_state_Ch.delay_ch2));
		strncpy(lidar_state_Ch.delay_ch2, value, sizeof(lidar_state_Ch.delay_ch2));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch3");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch3, 0x00, sizeof(lidar_state_Ch.delay_ch3));
		strncpy(lidar_state_Ch.delay_ch3, value, sizeof(lidar_state_Ch.delay_ch3));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch4");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch4, 0x00, sizeof(lidar_state_Ch.delay_ch4));
		strncpy(lidar_state_Ch.delay_ch4, value, sizeof(lidar_state_Ch.delay_ch4));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch5");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch5, 0x00, sizeof(lidar_state_Ch.delay_ch5));
		strncpy(lidar_state_Ch.delay_ch5, value, sizeof(lidar_state_Ch.delay_ch5));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch6");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch6, 0x00, sizeof(lidar_state_Ch.delay_ch6));
		strncpy(lidar_state_Ch.delay_ch6, value, sizeof(lidar_state_Ch.delay_ch6));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch7");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch7, 0x00, sizeof(lidar_state_Ch.delay_ch7));
		strncpy(lidar_state_Ch.delay_ch7, value, sizeof(lidar_state_Ch.delay_ch7));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch8");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch8, 0x00, sizeof(lidar_state_Ch.delay_ch8));
		strncpy(lidar_state_Ch.delay_ch8, value, sizeof(lidar_state_Ch.delay_ch8));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch9");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch9, 0x00, sizeof(lidar_state_Ch.delay_ch9));
		strncpy(lidar_state_Ch.delay_ch9, value, sizeof(lidar_state_Ch.delay_ch9));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch10");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch10, 0x00, sizeof(lidar_state_Ch.delay_ch10));
		strncpy(lidar_state_Ch.delay_ch10, value, sizeof(lidar_state_Ch.delay_ch10));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch11");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch11, 0x00, sizeof(lidar_state_Ch.delay_ch11));
		strncpy(lidar_state_Ch.delay_ch11, value, sizeof(lidar_state_Ch.delay_ch11));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch12");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch12, 0x00, sizeof(lidar_state_Ch.delay_ch12));
		strncpy(lidar_state_Ch.delay_ch12, value, sizeof(lidar_state_Ch.delay_ch12));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch13");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch13, 0x00, sizeof(lidar_state_Ch.delay_ch13));
		strncpy(lidar_state_Ch.delay_ch13, value, sizeof(lidar_state_Ch.delay_ch13));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch14");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch14, 0x00, sizeof(lidar_state_Ch.delay_ch14));
		strncpy(lidar_state_Ch.delay_ch14, value, sizeof(lidar_state_Ch.delay_ch14));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch15");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch15, 0x00, sizeof(lidar_state_Ch.delay_ch15));
		strncpy(lidar_state_Ch.delay_ch15, value, sizeof(lidar_state_Ch.delay_ch15));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch16");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch16, 0x00, sizeof(lidar_state_Ch.delay_ch16));
		strncpy(lidar_state_Ch.delay_ch16, value, sizeof(lidar_state_Ch.delay_ch16));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch17");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch17, 0x00, sizeof(lidar_state_Ch.delay_ch17));
		strncpy(lidar_state_Ch.delay_ch17, value, sizeof(lidar_state_Ch.delay_ch17));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch18");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch18, 0x00, sizeof(lidar_state_Ch.delay_ch18));
		strncpy(lidar_state_Ch.delay_ch18, value, sizeof(lidar_state_Ch.delay_ch18));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch19");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch19, 0x00, sizeof(lidar_state_Ch.delay_ch19));
		strncpy(lidar_state_Ch.delay_ch19, value, sizeof(lidar_state_Ch.delay_ch19));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch20");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch20, 0x00, sizeof(lidar_state_Ch.delay_ch20));
		strncpy(lidar_state_Ch.delay_ch20, value, sizeof(lidar_state_Ch.delay_ch20));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch21");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch21, 0x00, sizeof(lidar_state_Ch.delay_ch21));
		strncpy(lidar_state_Ch.delay_ch21, value, sizeof(lidar_state_Ch.delay_ch21));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch22");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch22, 0x00, sizeof(lidar_state_Ch.delay_ch22));
		strncpy(lidar_state_Ch.delay_ch22, value, sizeof(lidar_state_Ch.delay_ch22));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch23");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch23, 0x00, sizeof(lidar_state_Ch.delay_ch23));
		strncpy(lidar_state_Ch.delay_ch23, value, sizeof(lidar_state_Ch.delay_ch23));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch24");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch24, 0x00, sizeof(lidar_state_Ch.delay_ch24));
		strncpy(lidar_state_Ch.delay_ch24, value, sizeof(lidar_state_Ch.delay_ch24));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch25");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch25, 0x00, sizeof(lidar_state_Ch.delay_ch25));
		strncpy(lidar_state_Ch.delay_ch25, value, sizeof(lidar_state_Ch.delay_ch25));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch26");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch26, 0x00, sizeof(lidar_state_Ch.delay_ch26));
		strncpy(lidar_state_Ch.delay_ch26, value, sizeof(lidar_state_Ch.delay_ch26));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch27");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch27, 0x00, sizeof(lidar_state_Ch.delay_ch27));
		strncpy(lidar_state_Ch.delay_ch27, value, sizeof(lidar_state_Ch.delay_ch27));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch28");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch28, 0x00, sizeof(lidar_state_Ch.delay_ch28));
		strncpy(lidar_state_Ch.delay_ch28, value, sizeof(lidar_state_Ch.delay_ch28));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch29");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch29, 0x00, sizeof(lidar_state_Ch.delay_ch29));
		strncpy(lidar_state_Ch.delay_ch29, value, sizeof(lidar_state_Ch.delay_ch29));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch30");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch30, 0x00, sizeof(lidar_state_Ch.delay_ch30));
		strncpy(lidar_state_Ch.delay_ch30, value, sizeof(lidar_state_Ch.delay_ch30));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch31");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch31, 0x00, sizeof(lidar_state_Ch.delay_ch31));
		strncpy(lidar_state_Ch.delay_ch31, value, sizeof(lidar_state_Ch.delay_ch31));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch32");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch.delay_ch32, 0x00, sizeof(lidar_state_Ch.delay_ch32));
		strncpy(lidar_state_Ch.delay_ch32, value, sizeof(lidar_state_Ch.delay_ch32));
#endif

	////////////////////////////////////////////////////////////////////////////////////

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

	int BUFSIZE = 1460;
	char buf[BUFSIZE];
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	memset(post_cmd_data, 0x00, sizeof(post_cmd_data));
	post_cmd_recved_size = 0;
	*recv_status = CMD_RECV_FINISHED;
	is_routing_delay_1_cmd = 2;

	return 0;
}

int parse_post_cmd_data2(struct tcp_pcb *pcb, char *req, int rlen, enum ePostCmdRecvStatus *recv_status)
{
	if(strstr(req, "json=") == NULL)
	{
		e_post_cmd_recv_status = NONE;
		return -1;
	}

	memcpy(post_cmd_data + post_cmd_recved_size, req, strlen(req));
	post_cmd_recved_size += strlen(req);
	if(post_cmd_recved_size < post_cmd_data_size)
	{
		return 1;
	}

	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
		return XST_FAILURE;
	}
#if 1
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
#else
	char value[30] = {0, };
		char key[30] = "ch1";
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch1, 0x00, sizeof(lidar_state_Ch2.delay_ch1));
		strncpy(lidar_state_Ch2.delay_ch1, value, sizeof(lidar_state_Ch2.delay_ch1));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch2");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch2, 0x00, sizeof(lidar_state_Ch2.delay_ch2));
		strncpy(lidar_state_Ch2.delay_ch2, value, sizeof(lidar_state_Ch2.delay_ch2));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch3");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch3, 0x00, sizeof(lidar_state_Ch2.delay_ch3));
		strncpy(lidar_state_Ch2.delay_ch3, value, sizeof(lidar_state_Ch2.delay_ch3));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch4");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch4, 0x00, sizeof(lidar_state_Ch2.delay_ch4));
		strncpy(lidar_state_Ch2.delay_ch4, value, sizeof(lidar_state_Ch2.delay_ch4));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch5");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch5, 0x00, sizeof(lidar_state_Ch2.delay_ch5));
		strncpy(lidar_state_Ch2.delay_ch5, value, sizeof(lidar_state_Ch2.delay_ch5));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch6");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch6, 0x00, sizeof(lidar_state_Ch2.delay_ch6));
		strncpy(lidar_state_Ch2.delay_ch6, value, sizeof(lidar_state_Ch2.delay_ch6));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch7");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch7, 0x00, sizeof(lidar_state_Ch2.delay_ch7));
		strncpy(lidar_state_Ch2.delay_ch7, value, sizeof(lidar_state_Ch2.delay_ch7));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch8");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch8, 0x00, sizeof(lidar_state_Ch2.delay_ch8));
		strncpy(lidar_state_Ch2.delay_ch8, value, sizeof(lidar_state_Ch2.delay_ch8));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch9");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch9, 0x00, sizeof(lidar_state_Ch2.delay_ch9));
		strncpy(lidar_state_Ch2.delay_ch9, value, sizeof(lidar_state_Ch2.delay_ch9));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch10");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch10, 0x00, sizeof(lidar_state_Ch2.delay_ch10));
		strncpy(lidar_state_Ch2.delay_ch10, value, sizeof(lidar_state_Ch2.delay_ch10));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch11");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch11, 0x00, sizeof(lidar_state_Ch2.delay_ch11));
		strncpy(lidar_state_Ch2.delay_ch11, value, sizeof(lidar_state_Ch2.delay_ch11));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch12");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch12, 0x00, sizeof(lidar_state_Ch2.delay_ch12));
		strncpy(lidar_state_Ch2.delay_ch12, value, sizeof(lidar_state_Ch2.delay_ch12));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch13");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch13, 0x00, sizeof(lidar_state_Ch2.delay_ch13));
		strncpy(lidar_state_Ch2.delay_ch13, value, sizeof(lidar_state_Ch2.delay_ch13));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch14");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch14, 0x00, sizeof(lidar_state_Ch2.delay_ch14));
		strncpy(lidar_state_Ch2.delay_ch14, value, sizeof(lidar_state_Ch2.delay_ch14));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch15");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch15, 0x00, sizeof(lidar_state_Ch2.delay_ch15));
		strncpy(lidar_state_Ch2.delay_ch15, value, sizeof(lidar_state_Ch2.delay_ch15));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch16");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch16, 0x00, sizeof(lidar_state_Ch2.delay_ch16));
		strncpy(lidar_state_Ch2.delay_ch16, value, sizeof(lidar_state_Ch2.delay_ch16));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch17");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch17, 0x00, sizeof(lidar_state_Ch2.delay_ch17));
		strncpy(lidar_state_Ch2.delay_ch17, value, sizeof(lidar_state_Ch2.delay_ch17));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch18");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch18, 0x00, sizeof(lidar_state_Ch2.delay_ch18));
		strncpy(lidar_state_Ch2.delay_ch18, value, sizeof(lidar_state_Ch2.delay_ch18));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch19");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch19, 0x00, sizeof(lidar_state_Ch2.delay_ch19));
		strncpy(lidar_state_Ch2.delay_ch19, value, sizeof(lidar_state_Ch2.delay_ch19));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch20");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch20, 0x00, sizeof(lidar_state_Ch2.delay_ch20));
		strncpy(lidar_state_Ch2.delay_ch20, value, sizeof(lidar_state_Ch2.delay_ch20));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch21");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch21, 0x00, sizeof(lidar_state_Ch2.delay_ch21));
		strncpy(lidar_state_Ch2.delay_ch21, value, sizeof(lidar_state_Ch2.delay_ch21));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch22");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch22, 0x00, sizeof(lidar_state_Ch2.delay_ch22));
		strncpy(lidar_state_Ch2.delay_ch22, value, sizeof(lidar_state_Ch2.delay_ch22));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch23");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch23, 0x00, sizeof(lidar_state_Ch2.delay_ch23));
		strncpy(lidar_state_Ch2.delay_ch23, value, sizeof(lidar_state_Ch2.delay_ch23));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch24");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch24, 0x00, sizeof(lidar_state_Ch2.delay_ch24));
		strncpy(lidar_state_Ch2.delay_ch24, value, sizeof(lidar_state_Ch2.delay_ch24));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch25");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch25, 0x00, sizeof(lidar_state_Ch2.delay_ch25));
		strncpy(lidar_state_Ch2.delay_ch25, value, sizeof(lidar_state_Ch2.delay_ch25));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch26");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch26, 0x00, sizeof(lidar_state_Ch2.delay_ch26));
		strncpy(lidar_state_Ch2.delay_ch26, value, sizeof(lidar_state_Ch2.delay_ch26));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch27");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch27, 0x00, sizeof(lidar_state_Ch2.delay_ch27));
		strncpy(lidar_state_Ch2.delay_ch27, value, sizeof(lidar_state_Ch2.delay_ch27));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch28");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch28, 0x00, sizeof(lidar_state_Ch2.delay_ch28));
		strncpy(lidar_state_Ch2.delay_ch28, value, sizeof(lidar_state_Ch2.delay_ch28));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch29");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch29, 0x00, sizeof(lidar_state_Ch2.delay_ch29));
		strncpy(lidar_state_Ch2.delay_ch29, value, sizeof(lidar_state_Ch2.delay_ch29));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch30");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch30, 0x00, sizeof(lidar_state_Ch2.delay_ch30));
		strncpy(lidar_state_Ch2.delay_ch30, value, sizeof(lidar_state_Ch2.delay_ch30));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch31");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch31, 0x00, sizeof(lidar_state_Ch2.delay_ch31));
		strncpy(lidar_state_Ch2.delay_ch31, value, sizeof(lidar_state_Ch2.delay_ch31));

		memset(value, 0x00, sizeof(value));
		memset(key, 0x00, sizeof(key));
		strcat(key, "ch32");
		GetJsonValueByKey(value, post_cmd_data, key);
		memset(lidar_state_Ch2.delay_ch32, 0x00, sizeof(lidar_state_Ch2.delay_ch32));
		strncpy(lidar_state_Ch2.delay_ch32, value, sizeof(lidar_state_Ch2.delay_ch32));
#endif

	////////////////////////////////////////////////////////////////////////////////////

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

	int BUFSIZE = 1460;
	char buf[BUFSIZE];
	generate_http_header(buf, NULL, 0);
	if(tcp_write(pcb, buf, 128, 1) != ERR_OK)
	{
		xil_printf("error writing http POST response to socket\n\r");
		xil_printf("http header = %s\r\n", buf);
		return -1;
	}

	memset(post_cmd_data, 0x00, sizeof(post_cmd_data));
	post_cmd_recved_size = 0;
	*recv_status = CMD_RECV_FINISHED;
	is_routing_delay_1_cmd = 2;

	return 0;
}

void tcp_recv_callback(char *payload, int len)
{

	memcpy(_recv_buffer, &payload[0], len);
	_recv_buffer[len] = '\0';

	CommandPacket* packet;
	packet = (CommandPacket*)payload;

	int instruction_number, parameter1, parameter2;
	instruction_number = packet->cmd_body.instruction;
	parameter1 = packet->cmd_body.channel;
	parameter2 = packet->cmd_body.value;
	if(instruction_number < NumberOfeLidarCalibrationCmd)
		lidar_calibration_fp[instruction_number](parameter1, parameter2);
}

/* generate and write out an appropriate response for the http request */
/* 	this assumes that tcp_sndbuf is high enough to send atleast 1 packet */
int generate_response(struct tcp_pcb *pcb, char *http_req, int http_req_len)
{
	enum http_req_type request_type = decode_http_request(http_req, http_req_len);

	switch(request_type)
	{
	case HTTP_GET :
		return do_http_get(pcb, http_req, http_req_len);
	case HTTP_POST :
		return do_http_post(pcb, http_req, http_req_len, 0x00);
	default :
	{
//			int cmd_value = 0;
//			cmd_value = (int)(http_req[0] + (http_req[1] << 8) + (http_req[2] << 16) + (http_req[3] << 24));
//			if(cmd_value == 0xF4E3D2C1)
//			{
//				tcp_recv_callback(http_req, http_req_len);
//			}
//			else
//			{
//				if (e_file_recv_status == HEADER_PARSED)
//				{
//					parse_payload_header(pcb, http_req, http_req_len, &e_file_recv_status);
//				}
//				else if (e_file_recv_status == PAYLOAD_HEADER_PARSED)
//				{
//					parse_payload_data(pcb, http_req, http_req_len, &e_file_recv_status);
//				}
//				else if(e_post_cmd_recv_status == CMD_PARSED)
//				{
//					parse_post_cmd_data(pcb, http_req, http_req_len, &e_post_cmd_recv_status);
//				}
//			}

		if(e_file_recv_status == HEADER_PARSED)
		{
			parse_payload_header(pcb, http_req, http_req_len, &e_file_recv_status);
		}
		else if(e_file_recv_status == PAYLOAD_HEADER_PARSED)
		{
			if(strncmp(file_extension, "csv2", strlen("csv2")) == 0)
			{
				parse_csv_data3(pcb, http_req, http_req_len, &e_file_recv_status);
			}
			else if(strncmp(file_extension, "csv", strlen("csv")) == 0)
			{
//				parse_csv_data(pcb, http_req, http_req_len, &e_file_recv_status);
				parse_csv_data2(pcb, http_req, http_req_len, &e_file_recv_status);
			}
			else
			{
				parse_payload_data(pcb, http_req, http_req_len, &e_file_recv_status);
			}

		}
		else if(e_post_cmd_recv_status == CMD_PARSED)
		{
			if(is_routing_delay_1_cmd == 1)
				parse_post_cmd_data(pcb, http_req, http_req_len, &e_post_cmd_recv_status);
			else if(is_routing_delay_1_cmd == 0)
				parse_post_cmd_data2(pcb, http_req, http_req_len, &e_post_cmd_recv_status);
		}
		else
		{
			tcp_recv_callback(http_req, http_req_len);
		}

	}
	}

	return 0;
}






void GetCommand(char* result, char* src)
{
	char* command_start = strstr(src, "/cmd");
	char* command_end = strstr(command_start, "?");
	strncpy(result, command_start, command_end - command_start);
}


//void SetRoutingDelayToLidarState(LidarState* lidar_state, LidarState_Ch lidar_state_ch)
//{
//	strncpy(lidar_state->calibration_setting_.routing_delay[0], lidar_state_ch.delay_ch1, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[1], lidar_state_ch.delay_ch2, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[2], lidar_state_ch.delay_ch3, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[3], lidar_state_ch.delay_ch4, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[4], lidar_state_ch.delay_ch5, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[5], lidar_state_ch.delay_ch6, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[6], lidar_state_ch.delay_ch7, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[7], lidar_state_ch.delay_ch8, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[8], lidar_state_ch.delay_ch9, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[9], lidar_state_ch.delay_ch10, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[10], lidar_state_ch.delay_ch11, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[11], lidar_state_ch.delay_ch12, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[12], lidar_state_ch.delay_ch13, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[13], lidar_state_ch.delay_ch14, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[14], lidar_state_ch.delay_ch15, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[15], lidar_state_ch.delay_ch16, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[16], lidar_state_ch.delay_ch17, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[17], lidar_state_ch.delay_ch18, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[18], lidar_state_ch.delay_ch19, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[19], lidar_state_ch.delay_ch20, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[20], lidar_state_ch.delay_ch21, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[21], lidar_state_ch.delay_ch22, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[22], lidar_state_ch.delay_ch23, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[23], lidar_state_ch.delay_ch24, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[24], lidar_state_ch.delay_ch25, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[25], lidar_state_ch.delay_ch26, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[26], lidar_state_ch.delay_ch27, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[27], lidar_state_ch.delay_ch28, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[28], lidar_state_ch.delay_ch29, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[29], lidar_state_ch.delay_ch30, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[30], lidar_state_ch.delay_ch31, 10);
//	strncpy(lidar_state->calibration_setting_.routing_delay[31], lidar_state_ch.delay_ch32, 10);
//}
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
