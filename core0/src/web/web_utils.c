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

#include <string.h>

#include "lwip/sys.h"

#include "../../../common/src/api_sw/mfs/mfs_config.h"
#include "web_utils.h"
#include "xstatus.h"

#include <stdbool.h>
#include <string.h>

#define CMD_GET_STATE		0x01
#define CMD_GET_INFO		0x02
#define CMD_GET_CONFIG		0x03
#define CMD_GET_PROGRESS 	0x04
#define CMD_GET_RESET		0x05

#define CMD_SET_PARAM		0x11
#define CMD_SET_UPLOAD		0x15

#define CMD_UNKNOWN			0xFF

// 전체 데이터를 받는 버퍼

typedef enum _TOKEN_TYPE
{
	TOKEN_STRING,		// 문자열 토큰
	TOKEN_NUMBER,		// 숫자 토큰
} TOKEN_TYPE;

typedef struct _TOKEN
{
	TOKEN_TYPE type; // 토큰 종류
	union			// 두 종류 중 한 종류만 저장할 것이므로 공용체로 만듦
	{
		char *string;		// 문자열 포인터
		double number;		// 실수형 숫자
	};

	bool isArrary;

} TOKEN;

#define TOKEN_COUNT	20

typedef struct _JSON
{
	TOKEN tokens[TOKEN_COUNT];
} JSON;

void parseJSON(char *doc, int size, JSON *json); // JSON 파싱 함수

void print_http_request(char *buf, int n)
{
	printf("%s\r\n", buf);
}

int is_cmd_led(char *buf)
{
	/* skip past 'POST /' */
	buf += 6;

	/* then check for cmd/ledxhr */
	return (!strncmp(buf, "cmd", 3) && !strncmp(buf + 4, "ledxhr", 6));
}

int is_cmd_switch(char *buf)
{
	/* skip past 'POST /' */
	buf += 6;

	/* then check for cmd/ledxhr */
	return (!strncmp(buf, "cmd", 3) && !strncmp(buf + 4, "switchxhr", 9));
}

// boot.bin 파일 전송을 받는 다는 명령어를 받을때
int is_cmd_Update(char *buf)
{
	xil_printf("CMD Update Check\r\n");

	// 문자열 잘라내기
	char *ptr = NULL;
	//ptr = strstr(buf, "{");
	ptr = strchr(buf, '{');
	//char *ptr_2 = strtok(NULL, "{");
	// 문자열 길이
	int len = strlen(ptr);
	JSON json =
	{ 0, };

	parseJSON(ptr, len, &json);
//
//	//xil_printf("strchr len : %d\r\n", len);
//	xil_printf("[0] : %s\r\n", json.tokens[0].string);
//	xil_printf("[1] : %s\r\n", json.tokens[1].string);
//	xil_printf("[2] : %s\r\n", json.tokens[2].string);
//	//xil_printf("[3] : %d\r\n", json.tokens[3].number);
	xil_printf("[3] : %s\r\n", json.tokens[3].string);
//	xil_printf("[4] : %s\r\n", json.tokens[4].string);
//	xil_printf("[5] : %s\r\n", json.tokens[5].string);
//	xil_printf("[6] : %s\r\n", json.tokens[6].string);
//	xil_printf("[7] : %s\r\n", json.tokens[7].string);
	return atoi(json.tokens[3].string);
}

int is_cmd_Progress(char *buf)
{
	xil_printf("CMD Progress Check\r\n");

	return cmdParse(buf);
}

int cmdParse(char *buf)
{
	/* skip past 'POST /' */
	buf += 6;

	int cmd_str_len = strcspn(buf, "?");
//	xil_printf("cmd_str_len %d\r\n", cmd_str_len);

	char cmd_str[20] = "";
	char *cmd_type = NULL;
	char *param_str = NULL;
//	char cmd_type[10] = "";

//	char param_str[10] = "";
	if (cmd_str_len > 20)
	{
		xil_printf("unsupported cmd! too much lenght !!!!!!!! \r\n");

		return CMD_UNKNOWN;
	}

	strncpy(cmd_str, buf, cmd_str_len);

	//xil_printf("cmd_str %s\r\n", cmd_str);
//
	cmd_type = strtok(cmd_str, "/");

	if (!strcmp(cmd_type, "cmd_get"))
	{
		param_str = strtok(NULL, "/");

		if (!strcmp(param_str, "progress"))
			return CMD_GET_PROGRESS;
		else if (!strcmp(param_str, "state"))
			return CMD_GET_STATE;
		else if (!strcmp(param_str, "info"))
			return CMD_GET_STATE;
		else if (!strcmp(param_str, "config"))
			return CMD_GET_CONFIG;
		else if (!strcmp(param_str, "reset"))
			return CMD_GET_RESET;
		else
		{
			xil_printf("unsupported get cmd!\r\n");
			return CMD_UNKNOWN;
		}
	}
	else if (!strcmp(cmd_type, "cmd_set"))
	{
		param_str = strtok(NULL, "/");

		if (!strcmp(param_str, "param"))
			return CMD_GET_PROGRESS;
		else if (!strcmp(param_str, "upload"))
			return CMD_SET_UPLOAD;

		return CMD_SET_PARAM;
	}
	else
	{
		xil_printf("unsupported cmd!\r\n");
		return CMD_UNKNOWN;
	}
}

// 여기다 progress 관련 함수 구현

void extract_file_name(char *filename, char *req, int rlen, int maxlen)
{
	char *fstart, *fend;

	/* first locate the file name in the request */
	/* requests are of the form GET /path/to/filename HTTP... */

	req += strlen("GET ");

	if (*req == '/')
		req++;

	fstart = req; /* start marker */

	while (*req != ' ') /* file name finally ends in a space */
		req++;

	fend = req - 1; /* end marker */

	if (fend < fstart)
	{
		strcpy(filename, "index.html");
		return;
	}

	/* make sure filename is of reasonable size */
	if (fend - fstart > maxlen)
	{
		*fend = 0;
		strcpy(filename, "404.html");
		printf("Request filename is too long\r\n");
		return;
	}

	/* copy over the filename */
	strncpy(filename, fstart, fend - fstart + 1);
	filename[fend - fstart + 1] = 0;

	/* if last character is a '/', append index.html */
	if (*fend == '/')
		strcat(filename, "index.html");
}

char *get_file_extension(char *fname)
{
	char *fext = fname + strlen(fname) - 1;

	while (fext > fname)
	{
		if (*fext == '.')
			return fext + 1;
		fext--;
	}

	return (char*)NULL;
}

int generate_http_header(char *buf, char *fext, int fsize)
{
	char lbuf[40];

	strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: ");

	if (fext == NULL)
		strcat(buf, "text/html"); /* for unknown types */
	else if (!strncmp(fext, "htm", 3))
		strcat(buf, "text/html"); /* html */
	else if (!strncmp(fext, "jpg", 3))
		strcat(buf, "image/jpeg");
	else if (!strncmp(fext, "gif", 3))
		strcat(buf, "image/gif");
	else if (!strncmp(fext, "json", 4))
		strcat(buf, "application/json");
	else if (!strncmp(fext, "js", 2))
		strcat(buf, "application/javascript");
	else if (!strncmp(fext, "pdf", 2))
		strcat(buf, "application/pdf");
	else if (!strncmp(fext, "css", 2))
		strcat(buf, "text/css");
	else
		strcat(buf, "text/plain"); /* for unknown types */
	strcat(buf, "\r\n");

	sprintf(lbuf, "Content-length: %d", fsize);
	strcat(buf, lbuf);
	strcat(buf, "\r\n");

	strcat(buf, "Connection: close\r\n");
	strcat(buf, "\r\n");

	return strlen(buf);
}

static int p_arg_count = 0;
static http_arg httpArgArray[1000];
static int httpArgArrayIndex = 0;
http_arg *palloc_arg()
{
	http_arg *a;
	a = &(httpArgArray[httpArgArrayIndex]);
	httpArgArrayIndex++;
	if (httpArgArrayIndex == 1000)
		httpArgArrayIndex = 0;
	a->count = p_arg_count++;
	a->fd = -1;
	a->fsize = 0;

	return a;
}

void pfree_arg(http_arg *arg)
{
	;
}

void parseJSON(char *doc, int size, JSON *json) // JSON 파싱 함수
{
	int tokenIndex = 0;	// 토큰 인덱스
	int pos = 0;		// 문자 검색 위치를 저장하는 변수

	if (doc[pos] != '{') // 문자의 시작이 {인지 검사
	{
		return;
	}

	pos++;		// 다음 문자로

	while (pos < size)	// 문서 크기 만큼 반복
	{
		switch (doc[pos])
		// 문서의 종류에 따라 분기
		{
			case '"':		// 문자가 "이면 문자열
			{
				// 문자열의 시작 위치를 구함. 맨 앞의 "를 제외하기 위해 + 1
				char *begin = doc + pos + 1;

				// 문자열의 끝 위치를 구함. 다음 "의 위치
				char *end = strchr(begin, '"');
				if (end == NULL)	// "가 없으면 잘못된 문법이므로
				{
					break;		// 반복을 종료
				}

				int stringLength = end - begin;		// 문자열의 실제 길이는 끝 위치 - 시작위치

				// 토큰 배열에 문자열 저장
				// 토큰 종류는 문자열
				json->tokens[tokenIndex].type = TOKEN_STRING;

				// 문자열 길이 + NULL 공간만큼 메모리 할당
				if (stringLength > 100)
				{
					// 전역 변수 버퍼로 할당
				}
				else
				{
					json->tokens[tokenIndex].string = malloc(stringLength + 1);
				}

				// 할당한 메모리를 0으로 초기화
				memset(json->tokens[tokenIndex].string, 0, stringLength + 1);

				// 문서에서 문자열을 토큰에 저장
				// 문자열 시작 위치에서 문자열 길이만큼만 보갓

				memcpy(json->tokens[tokenIndex].string, begin, stringLength);

				tokenIndex++;	// 토큰 인덱스 증가

				pos = pos + stringLength + 1; // 현재 위치 + 문자열 길이 + "(+ 1)

			}
				break;
			case '[':			// 문자가 [ 이면 배열
			{
				pos++;			// 다음 문자로

				while (doc[pos] != ']')	// 닫는 ]가 나오면 반복 종료
				{
					// 여기서는 문자열 배열만 처리
					if (doc[pos] == '"') // 문자가 "이면 문자열
					{
						// 문자열의 시작 위치를 구함. 맨 앞의 "를 제외하기 위해 + 1
						char *begin = doc + pos + 1;

						// 문자열의 끝 위치를 구함. 다음 "의 위치
						char *end = strchr(begin, '"');
						if (end == NULL)	// "가 없으면 잘못된 문법이므로
						{
							break;		// 반복 종료
						}

						int stringLength = end - begin; 	// 문자열의 실제 길이는 끝 위치 - 시작 위치

						// 토큰 배열에 문자열 저장
						// 토큰 종류는 문자열
						json->tokens[tokenIndex].type = TOKEN_STRING;

						// 문자열 길이 + NULL 공간만큼 메모리 할당
						json->tokens[tokenIndex].string = malloc(stringLength + 1);

						// 할당한 메모리를 0으로 초기화
						memset(json->tokens[tokenIndex].string, 0, stringLength + 1);

						// 문서에서 문자열을 토큰에 저장
						// 문자열 시작 위치에서 문자열 길이만큼만 보갓

						memcpy(json->tokens[tokenIndex].string, begin, stringLength);

						tokenIndex++;	// 토큰 인덱스 증가

						pos = pos + stringLength + 1; // 현재 위치 + 문자열 길이 + "(+ 1)

					}
				}
			}
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':    // 문자가 숫자이면
			case '6':
			case '7':
			case '8':
			case '9':
			case '-':              // -는 음수일 때
			{
				// 문자열의 시작 위치를 구함
				char *begin = doc + pos;
				char *end;
				char *buffer;

				// 문자열의 끝 위치를 구함. ,가 나오거나
				end = strchr(doc + pos, ',');
				if (end == NULL)
				{
					// } 가 나오면 문자열이 끝남
					end = strchr(doc + pos, '}');
					if (end == NULL)    // }가 없으면 잘못된 문법이므로
						break;          // 반복을 종료
				}

				int stringLength = end - begin;    // 문자열의 실제 길이는 끝 위치 - 시작 위치

				// 문자열 길이 + NULL 공간만큼 메모리 할당
				buffer = malloc(stringLength + 1);
				// 할당한 메모리를 0으로 초기화
				memset(buffer, 0, stringLength + 1);

				// 문서에서 문자열을 버퍼에 저장
				// 문자열 시작 위치에서 문자열 길이만큼만 복사
				memcpy(buffer, begin, stringLength);

				// 토큰 종류는 숫자
				json->tokens[tokenIndex].type = TOKEN_NUMBER;
				// 문자열을 숫자로 변환하여 토큰에 저장
				json->tokens[tokenIndex].number = atof(buffer);

				free(buffer);    // 버퍼 해제

				tokenIndex++;    // 토큰 인덱스 증가

				pos = pos + stringLength + 1;    // 현재 위치 + 문자열 길이 + , 또는 }(+ 1)
			}
				break;
		}

		pos++;
	}

}

void freeJSON(JSON *json)	// JSON 해제 함수
{
	for (int i = 0; i < TOKEN_COUNT; i++)	// 토큰 개수만큼 반복
	{
		if (json->tokens[i].type == TOKEN_STRING) // 토큰 종류가 문자열이면
			free(json->tokens[i].string);		// 동적 메모리 해제
	}
}

int TransFormJSON(web_param *web_param_ptr, char *buf, int n)
{
	int string_len = 0;
	int len = 0;
	char *p = NULL;

	for(int i = 0 ; i < (n - 1) ; i++)
	{
		string_len += strlen(web_param_ptr[i].param_name) + strlen(web_param_ptr[i].param_value) + 6; // plus one ':', four '"', one ','
	}
	string_len += strlen(web_param_ptr[n - 1].param_name) + strlen(web_param_ptr[n - 1].param_value) + 5; // plus one ':', four '"'
	string_len += 2; // plus "{}"

	len = generate_http_header(buf, "json", string_len);
	p = (char *)buf + len;

	//====================== JSON Format ===========================//
	strcat(p, "{");
	for(int i = 0 ; i < (n - 1) ; i++)
	{
		strcat(p, "\"");
		strcat(p, web_param_ptr[i].param_name);
		strcat(p, "\"");
		strcat(p, ":");
		strcat(p, "\"");
		strcat(p, web_param_ptr[i].param_value);
		strcat(p, "\"");
		strcat(p, ",");
	}

	strcat(p, "\"");
	strcat(p, web_param_ptr[n - 1].param_name);
	strcat(p, "\"");
	strcat(p, ":");
	strcat(p, "\"");
	strcat(p, web_param_ptr[n - 1].param_value);
	strcat(p, "\"");
	strcat(p, "}");
	//=============================================================//

	return len;

}

int TransFormJSONArr(web_param *web_param_ptr1, web_param *web_param_ptr2, web_param *web_param_ptr3, web_param *web_param_ptr4, char *buf, int n1, int n2, int n3, int n4)
{
	int string_len = 0;
	int len = 0;
	//int n = 2;
	char *p = NULL;

	for(int i = 0 ; i < (n1 - 1) ; i++)
	{
		string_len += strlen(web_param_ptr1[i].param_name) + strlen(web_param_ptr1[i].param_value) + 6; // plus one ':', four '"', one ','
	}
	string_len += strlen(web_param_ptr1[n1 - 1].param_name) + strlen(web_param_ptr1[n1 - 1].param_value) + 5; // plus one ':', four '"'
	string_len += 2; // plus "{}"

	for(int i = 0 ; i < (n2 - 1) ; i++)
	{
		string_len += strlen(web_param_ptr2[i].param_name) + strlen(web_param_ptr2[i].param_value) + 6; // plus one ':', four '"', one ','
	}
	string_len += strlen(web_param_ptr2[n2 - 1].param_name) + strlen(web_param_ptr2[n2 - 1].param_value) + 5; // plus one ':', four '"'
	string_len += 2; // plus "{}"

	for(int i = 0 ; i < (n3 - 1) ; i++)
	{
		string_len += strlen(web_param_ptr3[i].param_name) + strlen(web_param_ptr3[i].param_value) + 6; // plus one ':', four '"', one ','
	}
	string_len += strlen(web_param_ptr3[n3 - 1].param_name) + strlen(web_param_ptr3[n3 - 1].param_value) + 5; // plus one ':', four '"'
	string_len += 2; // plus "{}"

	for(int i = 0 ; i < (n4 - 1) ; i++)
	{
		string_len += strlen(web_param_ptr4[i].param_name) + strlen(web_param_ptr4[i].param_value) + 6; // plus one ':', four '"', one ','
	}
	string_len += strlen(web_param_ptr4[n4 - 1].param_name) + strlen(web_param_ptr4[n4 - 1].param_value) + 5; // plus one ':', four '"'
	string_len += 2; // plus "{}"

	string_len += 5; // plus "[,,,]"

	len = generate_http_header(buf, "json", string_len);
	p = (char *)buf + len;

	//====================== JSON Format ===========================//
	strcat(p, "[");
	strcat(p, "{");
	for(int i = 0 ; i < (n1 - 1) ; i++)
	{
		strcat(p, "\"");
		strcat(p, web_param_ptr1[i].param_name);
		strcat(p, "\"");
		strcat(p, ":");
		strcat(p, "\"");
		strcat(p, web_param_ptr1[i].param_value);
		strcat(p, "\"");
		strcat(p, ",");
	}

	strcat(p, "\"");
	strcat(p, web_param_ptr1[n1 - 1].param_name);
	strcat(p, "\"");
	strcat(p, ":");
	strcat(p, "\"");
	strcat(p, web_param_ptr1[n1 - 1].param_value);
	strcat(p, "\"");
	strcat(p, "}");
	strcat(p, ",");

	strcat(p, "{");
	for(int i = 0 ; i < (n2 - 1) ; i++)
	{
		strcat(p, "\"");
		strcat(p, web_param_ptr2[i].param_name);
		strcat(p, "\"");
		strcat(p, ":");
		strcat(p, "\"");
		strcat(p, web_param_ptr2[i].param_value);
		strcat(p, "\"");
		strcat(p, ",");
	}

	strcat(p, "\"");
	strcat(p, web_param_ptr2[n2 - 1].param_name);
	strcat(p, "\"");
	strcat(p, ":");
	strcat(p, "\"");
	strcat(p, web_param_ptr2[n2 - 1].param_value);
	strcat(p, "\"");
	strcat(p, "}");
	strcat(p, ",");

	strcat(p, "{");
	for(int i = 0 ; i < (n3 - 1) ; i++)
	{
		strcat(p, "\"");
		strcat(p, web_param_ptr3[i].param_name);
		strcat(p, "\"");
		strcat(p, ":");
		strcat(p, "\"");
		strcat(p, web_param_ptr3[i].param_value);
		strcat(p, "\"");
		strcat(p, ",");
	}

	strcat(p, "\"");
	strcat(p, web_param_ptr3[n3 - 1].param_name);
	strcat(p, "\"");
	strcat(p, ":");
	strcat(p, "\"");
	strcat(p, web_param_ptr3[n3 - 1].param_value);
	strcat(p, "\"");
	strcat(p, "}");
	strcat(p, ",");

	strcat(p, "{");
	for(int i = 0 ; i < (n4 - 1) ; i++)
	{
		strcat(p, "\"");
		strcat(p, web_param_ptr4[i].param_name);
		strcat(p, "\"");
		strcat(p, ":");
		strcat(p, "\"");
		strcat(p, web_param_ptr4[i].param_value);
		strcat(p, "\"");
		strcat(p, ",");
	}

	strcat(p, "\"");
	strcat(p, web_param_ptr4[n4 - 1].param_name);
	strcat(p, "\"");
	strcat(p, ":");
	strcat(p, "\"");
	strcat(p, web_param_ptr4[n4 - 1].param_value);
	strcat(p, "\"");
	strcat(p, "}");

	strcat(p, "]");
	//=============================================================//

	return len;

}

void GetJsonValueByKey(char* result, char* src, char* key)
{
	//{"data":"value"}

	//{"ch1":"value","ch2":"value","ch3":"value","ch4":"value"}

	char* temp, *val_start, *val_end;
	key = strcat(key, "%22%3A%22");
	// key " : "

	temp = strstr((char*)src, key);

	// *key":"
	if(temp == NULL)
		return;
	// key":"*
	val_start = temp + strlen(key);

	val_end = strstr(val_start, "%22");

	strncpy(result, val_start, (uint32_t)val_end - (uint32_t)val_start);
}

void GetJsonStringValueByKey(char* result, char* src, char* key)
{
	//{"data":"value"}

	//{"ch1":"value","ch2":"value","ch3":"value","ch4":"value"}

	char* temp, *val_start, *val_end;
	key = strcat(key, "\":\"");
	// key " : "

	temp = strstr((char*)src, key);

	// *key":"
	if(temp == NULL)
		return;
	// key":"*
	val_start = temp + strlen(key);

	val_end = strstr(val_start, "\"");

	strncpy(result, val_start, (uint32_t)val_end - (uint32_t)val_start);
}

int32_t GetBootImageSettings(char* boot_image, struct LidarState* lidar_state)
{
	if(boot_image[0] != 0x7b) // check {
		return XST_FAILURE;

	char json_char_arr[MAX_BOOT_SETTING_SIZE];

	memset(json_char_arr, 0x00, MAX_BOOT_SETTING_SIZE);

	strncpy(json_char_arr, boot_image, MAX_BOOT_SETTING_SIZE - 1);

	char* current_pos = json_char_arr;
	uint32_t max_boot_image_settings_addr = current_pos + MAX_BOOT_SETTING_SIZE;

	while(*current_pos != 0x7d || *current_pos < max_boot_image_settings_addr || *current_pos != '\0') // check }
	{
		current_pos++;
		if(*current_pos == 0x22) //
		{
			char* start_ptr = current_pos + 1;
			char* end_ptr = strchr(start_ptr, '"');
			if(end_ptr == NULL)
				break;

			int32_t length = end_ptr - start_ptr;

			strncpy(start_ptr, lidar_state->lidar_info_.boot_image_name, length);

			current_pos += length + 1;
		}
	}

//	char* boot_name_ptr = strstr(boot_image, "name\":\"");

//	strncpy(boot_name_ptr, lidar_state->boot_image_name, strstr(boot_name_ptr, "\"") - boot_name_ptr - 1);
}
