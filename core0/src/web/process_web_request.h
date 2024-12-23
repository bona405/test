/*
 * process_web_request.h
 *
 *  Created on: 2022. 8. 1.
 *      Author: Wonju
 */

#ifndef SRC_WEB_PROCESS_WEB_REQUEST_H_
#define SRC_WEB_PROCESS_WEB_REQUEST_H_

#define PACKET_SIZE 1460
#define PS_RST_CTRL_REG (XPS_SYS_CTRL_BASEADDR + LIDAR_STATE_SIZE)
#define SLCR_UNLOCK_REG (XPS_SYS_CTRL_BASEADDR + 0x008)
#define PS_RST_MASK 0x1 // PS software reset

#include "./web_utils.h"
#include <stdint.h>
#include "lwip/tcp.h"
#include "xil_io.h"
#include "../../../common/src/api_sw/flash/flash.h"
#include <string.h>
#include <xstatus.h>
#include "../../../common/src/api_sw/models/machine_info.h"
#include "../../../common/src/api_sw/algorithms/dt_align.h"
#include "../../../common/src/api_hw/api_common.h"
#include "../calibration/calibration.h"
#include "../../../common/src/api_hw/api_hw.h"
#include <malloc.h>

enum ePostCmdRecvStatus
{
	NONE, CMD_PARSED, CMD_RECV_FINISHED
};

enum eFileRecvStatus
{
	EMPTY, HEADER_PARSED, PAYLOAD_HEADER_PARSED, PAYLOAD_DATA_PARSED, WAIT_BYTE_ARRAY, FILE_RECV_SEUCCES
};

enum ePostCmd
{
	kGetProgress, kGetCurrentVth, kSetUpload, kSetReset, kGetSysinfo, kGetHwinfo, kGetDiagnostics, kSetSendoffset, kSetLaseronoff, kSetEcho, kSetMotorRpm,
	kSetFovStartAngle, kSetFovEndAngle, kSetPhaseLockOffset, kSetDhcponoff, kSetProtocol, kSetIp, kSetSubnet, kSetGateway, kGetUpdateImage,
	kSetnetworkconfig, kGetcheckpwd, kSetAdminInfo, kGetAdminInfo, kSetUploadGolden, kSetUpdateDate, kSetNoiseonoff, kSetVoltageonoff, kSetVNoiseonoff, kSetGoldenVer,
	kSetGoldenDate, kSetLidarDelayInfo, kSetLidarThInfo, kGetLidarDelayInfo, kGetLidarThInfo, kRoutingCal, kAddRoutingDelay, kRegWrite, kRegRead, kIntenVth, kSetNoiseonoff2,
	kSetRegisterValue, kGetRegisterValue, kGetLogMessage, kGetDtAlignInfo, kSetNoiseonoff4, kSetHzOption, kFixedCal, kSetStopOption, kSetUPLog,
	kSendDarkAreaChannels, kSendDarkAreaCorrectionParameter1, kSendDarkAreaCorrectionParameter2, kSendDarkAreaCorrectionParameter3, kSendDarkAreaCorrectionParameter4,
	kGetDarkAreaCorrectionSettings,kSaveTargetChannelDistanceRatio,kSaveCorrectedDistanceRatio,kSaveDarkAreaSearchChannel,kGetDarkAreaCorrectionSettingParams,kSaveAzimuthRange,
	kSetAziCal,kSetRoutingRange,kSetRoutingdelay,kGetRoutingRange,kRGetRoutingdelay,
	kAddRoutingDelay2, kRoutingCal2,
	kSaveFIRs, kGetFIRs, kGetMinimumDctions,kIsPhaseLocked, kSaveGFIRs, kSaveVFIRs,
	NumberOfePostCmd

};


enum eLidarCalibrationCmd
{
	kChangeVth, kChangeRoutingDelay, kChangeEmPulse, kChangeDistanceOffset, kChangeDistanceMinLimit, kChangeDistanceMaxLimit, /*Instr4, Instr6, */
	kChangeMinimumDetectionRange, kChangeHorizontalAngleOffset, kChangeNoiseFilterSettings, kChangeRemoveGhostSettings, kChangeRPM,
	kChangeLdChannelEnable, kChangeLdChannelDelay, kChangeLdChannelTdTrDelay, kChangeLdChannelTdTfDelay, kChangeMotorAccelerationTime,
	kChangeMotorAccelerationResolution, kHistogramStartVth, kChangeStartEncoderCount, kChangeEndEncoderCount,
	kChangeHorizontalFovAngle, kChangeVerticalFovAngle, kChangeTdcMaxDistance, kChangeLdDefaultValue,kChangeDtAlignInfo,khhjTest, kDarkAreaCorrection,kDarkAreaCorrectionv2,
	kChangeRoutingDelay2,
	NumberOfeLidarCalibrationCmd

};

extern int pass_frame;

int percent_val;
int reset_ret_val;
uint8_t is_golden_img;
enum eFileRecvStatus e_file_recv_status;

uint8_t file_download_flag;

size_t content_length;
size_t recved_size;
enum ePostCmdRecvStatus e_post_cmd_recv_status;
uint8_t is_routing_delay_1_cmd;
size_t recv_file_size;

uint8_t is_noise_on2;
char buf[PACKET_SIZE];

char password[20];
size_t post_cmd_data_size;
char Total_buf[0x5fffff];

int32_t GetMfsProgress(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetCurrentVth(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t UpdateBootImage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t ResetHW(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetSystemInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetHwInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetDiagnosticsInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetCommandParameters(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetLaserOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetEchoNum(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetMotorRpm(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetFovStartAngle(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetFovEndAngle(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetPhaseLockOffset(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetDhcpOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetProtocol(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetIpAddress(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetSubnetMask(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetGateway(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetNetworkConfig(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetPassword(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetAdminInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetUPLog(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetThresholdVoltage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetAdminInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetFIRs(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetMinimumDetctions(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t UpdateGoldenImage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetUpdateDate(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetNoiseFilterOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetVNoiseFilterOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetVthFilterOnOff(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetGoldenImageUpdateDate(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetThresholdVoltage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t CalRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t CalRoutingDelay2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t CalRoutingDelayLong(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SaveFIRs(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SaveGFIRs(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SaveVFIRs(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t AddRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t AddRoutingDelay2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetRegWrite(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetIntenVth(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetRegRead(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetNoiseFilterOnOff2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetNoiseFilterOnOff4(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetHzOption(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetStopOption(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t FixedCalRoutingDelay(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetRegisterValue(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetRegisterValue(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetLogMessage(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetDtAilgnInfo(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SendDarkAreaChannels(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SendDarkAreaCorrectionParameter1(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SendDarkAreaCorrectionParameter2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SendDarkAreaCorrectionParameter3(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SendDarkAreaCorrectionParameter4(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetDarkAreaCorrectionSettings(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SaveTargetChannelDistanceRatio(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SaveCorrectedDistanceRatio(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SaveDarkAreaSearchChannel(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetDarkAreaCorrectionSettingParams(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);

int32_t SaveAzimuthRange(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetAziCal(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);

int32_t SetRoutingRange(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t SetRoutingdelay2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetRoutingRange(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);
int32_t GetRoutingdelay2(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);

int32_t GetLocked(struct tcp_pcb *pcb, char *req, int rlen, int response_wait);


void SetRoutingDelayToLidarState(LidarState* lidar_state, LidarState_Ch lidar_state_ch);
int do_404(struct tcp_pcb *pcb, char *req, int rlen);
int do_http_file_ok(struct tcp_pcb *pcb, char *req, int rlen, enum eFileRecvStatus *recv_status);


#endif /* SRC_WEB_PROCESS_WEB_REQUEST_H_ */
