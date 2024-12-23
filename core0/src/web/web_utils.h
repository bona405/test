/*
 * web_utils.h
 *
 *  Created on: 2022. 7. 29.
 *      Author: Wonju
 */

#ifndef SRC_WEB_WEB_UTILS_H_
#define SRC_WEB_WEB_UTILS_H_

#include "../../../common/src/api_sw/models/machine_info.h"

#define MAX_BOOT_SETTING_SIZE 2048

#pragma pack(push, 1)
typedef struct
{
	const char *param_name;
	char param_value[120];
} web_param;
#pragma pack(pop)

typedef struct
{
	int count;
	int fd;
	int fsize;
} http_arg;

int is_cmd_Update(char *buf);
int generate_http_header(char *buf, char *fext, int fsize);
int TransFormJSON(web_param *web_param_ptr, char *buf, int n);
int TransFormJSONArr(web_param *web_param_ptr1, web_param *web_param_ptr2, web_param *web_param_ptr3, web_param *web_param_ptr4, char *buf, int n1, int n2, int n3, int n4);
void GetJsonValueByKey(char* result, char* src, char* key);
void GetJsonStringValueByKey(char* result, char* src, char* key);
int32_t GetBootImageSettings(char* boot_image, struct LidarState* lidar_state);
void extract_file_name(char *filename, char *req, int rlen, int maxlen);
int cmdParse(char *buf);
#endif /* SRC_WEB_WEB_UTILS_H_ */
