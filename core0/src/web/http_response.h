/*
 * http_response.h
 *
 *  Created on: 2022. 7. 29.
 *      Author: Wonju
 */

#ifndef SRC_WEB_HTTP_RESPONSE_H_
#define SRC_WEB_HTTP_RESPONSE_H_

#define MAX_FILENAME 256

#define SLCR_LOCK_REG (XPS_SYS_CTRL_BASEADDR + 0x004)



#include "lwip/tcp.h"
#include "./web_utils.h"
#include "../packet/packet.h"
#include "../definitions/definitions.h"
#include "../../../common/src/api_sw/models/machine_info.h"
#include "../../../common/src/api_sw/container/hash_table.h"
#include "../platform/platform_fs.h"
#include "./process_web_request.h"

//boot image secure
#include "../secrue/decryption.h"
#include "../secrue/sha.h"

//enum eFileRecvStatus
//{
//	EMPTY, HEADER_PARSED, PAYLOAD_HEADER_PARSED, PAYLOAD_DATA_PARSED, WAIT_BYTE_ARRAY, FILE_RECV_SEUCCES
//};

//enum ePostCmdRecvStatus
//{
//	NONE, CMD_PARSED, CMD_RECV_FINISHED
//};

//enum ePostCmd
//{
//	kGetProgress, kSetUpload, kSetReset, kGetSysinfo, kGetHwinfo, kGetDiagnostics, kSetSendoffset, kSetLaseronoff, kSetEcho, kSetMotorRpm,
//	kSetFovStartAngle, kSetFovEndAngle, kSetPhaseLockOffset, kSetDhcponoff, kSetProtocol, kSetIp, kSetSubnet, kSetGateway, kGetUpdateImage,
//	kSetnetworkconfig, kGetcheckpwd, kSetAdminInfo, kGetAdminInfo, kSetUploadGolden, kSetUpdateDate, kSetNoiseonoff, kSetVoltageonoff, kSetGoldenVer,
//	kSetGoldenDate, kSetLidarDelayInfo, kSetLidarThInfo, kGetLidarDelayInfo, kGetLidarThInfo,
//};

//size_t content_length;
//enum ePostCmdRecvStatus e_post_cmd_recv_status;
//enum eFileRecvStatus e_file_recv_status;
uint8_t _recv_buffer[100];
//struct LidarState lidar_state_;
//struct LidarState_Ch lidar_state_Ch;
//size_t post_cmd_data_size;
size_t post_cmd_recved_size;
char post_cmd_data[4096];

//size_t recved_size;
//char password[20];
//size_t recv_file_size;
//uint8_t file_download_flag;
uint8_t file_upload_flag;

//int percent_val;
//uint8_t is_golden_img;
//int reset_ret_val;
char req_buf[1330];



//extern int Instr;
//extern int Ch;
//extern int Value;
//extern int Instr_flag;

void GetCommand(char* result, char* src);
//void SetRoutingDelay(LidarState* lidar_state, LidarState_Ch lidar_state_ch);



#endif /* SRC_WEB_HTTP_RESPONSE_H_ */
