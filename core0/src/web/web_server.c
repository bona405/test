/*
 * Copyright (c) 2007 Xilinx, Inc.  All rights reserved.
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

/* webserver.c: An example Webserver application using the RAW API
 *	This program serves web pages resident on Xilinx Memory File
 * System (MFS) using lwIP's RAW API. Use of RAW API implies that the
 * webserver is blazingly fast, but the design is not obvious since a
 * lot of the work happens in asynchronous callback functions.
 *
 * The webserver works as follows:
 *	- on every accepted connection, only 1 read is performed to
 * identify the file requested. Further reads are avoided by sending
 * a "Connection: close" in the HTTP response header, as well as setting
 * the callback function to NULL on that pcb
 *	- the read determines what file needs to be set (by parsing
 * "GET / HTTP/1.1" request
 *	- once the file to be sent is determined, tcp_write is called
 * in chunks of size tcp_sndbuf() until the whole file is sent
 *
 */

#include <stdio.h>
#include <string.h>
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "web_server.h"
#ifndef __PPC__
#include "xil_printf.h"
#endif
#include "../../../common/src/api_sw/mfs/xilmfs.h"

/* static variables controlling debug printf's in this file */
static int g_webserver_debug = 1;
static unsigned http_port = 80;
static unsigned http_server_running = 0;

//void platform_init_gpios();

int transfer_web_data()
{
	return 0;
}

err_t http_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	int BUFSIZE = 1400, sndbuf, n;
	char buf[BUFSIZE];
	http_arg *a = (http_arg*) arg;

	if (g_webserver_debug)
		xil_printf("%d (%d): S%d..\r\n", a ? a->count : 0, tpcb->state, len);

	if (tpcb->state > ESTABLISHED)
	{
		if (a)
		{
			pfree_arg(a);
			a = NULL;
		}
		tcp_close(tpcb);
		return ERR_OK;
	}

	if (a->fd == -1 || a->fsize <= 0) /* no more data to be sent */
		return ERR_OK;

	/* read more data out of the file and send it */
	while (1)
	{
		sndbuf = tcp_sndbuf(tpcb);
		if (sndbuf < BUFSIZE)
			return ERR_OK;

//            xil_printf("attempting to read %d bytes, left = %d bytes\r\n", BUFSIZE, a->fsize);
		n = mfs_file_read(a->fd, buf, BUFSIZE);

		if(tcp_write(tpcb, buf, n, 1) != ERR_OK)
		{
			xil_printf("error writing http POST response to socket\n\r");
			xil_printf("http header = %s\r\n", buf);
			return -1;
		}
		a->fsize -= n;

		if (a->fsize <= 0)
		{
			mfs_file_close(a->fd);
			a->fd = 0;
			tcp_close(tpcb);
			break;
		}
	}
	return ERR_OK;
}

// pbuf : 송수신 버퍼
err_t http_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	http_arg *a = (http_arg*) arg;

	if (g_webserver_debug)
		xil_printf("%d (%d): R%d %d..\r\n", a ? a->count : 0, tpcb->state, p->len, p->tot_len);

	// wonju - tcpbase.h enum tcp_state 참조

	//		enum tcp_state {
	//		  CLOSED      = 0,
	//		  LISTEN      = 1,
	//		  SYN_SENT    = 2,
	//		  SYN_RCVD    = 3,
	//		  ESTABLISHED = 4,
	//		  FIN_WAIT_1  = 5,
	//		  FIN_WAIT_2  = 6,
	//		  CLOSE_WAIT  = 7,
	//		  CLOSING     = 8,
	//		  LAST_ACK    = 9,
	//		  TIME_WAIT   = 10
	//		};
	/* do not read the packet if we are not in ESTABLISHED state */
	if (tpcb->state >= 5 && tpcb->state <= 8)
	{
		xil_printf("pcb state = %d", tpcb->state);
		pbuf_free(p);
		return -1;
	}
	else if (tpcb->state > 8)
	{
		xil_printf("pcb state = %d", tpcb->state);
		pbuf_free(p);
		return -1;
	}

	/* acknowledge that we've read the payload */
	// wonju -packet read 완료를 TCP에 알림
	// wonju -그리고 window size 키우기 위해 사용됨?? -> 이부분 제대로 확인.
	tcp_recved(tpcb, p->len);

	/* read and decipher the request */
	/* this function takes care of generating a request, sending it,
	 *	and closing the connection if all data can been sent. If
	 *	not, then it sets up the appropriate arguments to the sent
	 *	callback handler.
	 */
	// wonju - payload 읽은 후에 http 동작 수행
//		xil_printf("callback start\r\n");
	generate_response(tpcb, p->payload, p->len);

	/* free received packet */
	pbuf_free(p);

	return ERR_OK;
}

static err_t http_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	/* keep a count of connection # */
	tcp_arg(newpcb, (void*) palloc_arg());

	//tcp recv, sent callback 등록

	tcp_recv(newpcb, http_recv_callback);
	tcp_sent(newpcb, http_sent_callback);

	return ERR_OK;
}

int start_web_application()
{
	log_trace("start_web_application");
	struct tcp_pcb *pcb;
	err_t err;

	/* initialize file system layer */
	// wonju - mfs 초기화, 근데 xilinx에선 mfs 사용 권장 X -> 대신 ffs 사용 권장함. 이 부분 좀 찾아볼 것.(ffs로도 html 변환 가능한지...)
	platform_init_fs();

	/* initialize devices */
//	platform_init_gpios();
	/* create new TCP PCB structure */
	pcb = tcp_new();
	if (!pcb)
	{
		xil_printf("Error creating PCB. Out of Memory\r\n");
		return -1;
	}

	/* bind to http port 80 */
	// wonju - socket에 addr, port 설정
	err = tcp_bind(pcb, IP_ADDR_ANY, http_port);
	if (err != ERR_OK)
	{
		xil_printf("Unable to bind to port 80: err = %d\r\n", err);
		return -2;
	}

	/* we do not need any arguments to the first callback */
	// wonju - tcp callback 함수에 전달될 인자 설정(여기선 왜 NULL 설정하나???)
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb)
	{
		xil_printf("Out of memory while tcp_listen\r\n");
		return -3;
	}

	/* specify callback to use for incoming connections */
	// wonju - Socket 연결할때 발생하는 callback, 여기서는 web page 연결 시 발생하는 콜백이 되겠지?
	tcp_accept(pcb, http_accept_callback);

	http_server_running = 1;

	return 0;
}

void print_web_app_header()
{
	xil_printf("%20s %6d %s\r\n", "http server", http_port, "Point your web browser to http://192.168.1.10");
}

void InitFp()
{
	web_cmd_processing_fp[kGetProgress] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetMfsProgress;
	web_cmd_processing_fp[kGetCurrentVth] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetCurrentVth;
	web_cmd_processing_fp[kSetUpload] = (int32_t (*)(struct tcp_pcb*, char*, int, int))UpdateBootImage;
	web_cmd_processing_fp[kSetReset] = (int32_t (*)(struct tcp_pcb*, char*, int, int))ResetHW;
	web_cmd_processing_fp[kGetSysinfo] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetSystemInfo;
	web_cmd_processing_fp[kGetHwinfo] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetHwInfo;
	web_cmd_processing_fp[kGetDiagnostics] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetDiagnosticsInfo;
	web_cmd_processing_fp[kSetSendoffset] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetCommandParameters;
	web_cmd_processing_fp[kSetLaseronoff] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetLaserOnOff;
	web_cmd_processing_fp[kSetEcho] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetEchoNum;
	web_cmd_processing_fp[kSetMotorRpm] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetMotorRpm;
	web_cmd_processing_fp[kSetFovStartAngle] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetFovStartAngle;
	web_cmd_processing_fp[kSetFovEndAngle] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetFovEndAngle;
	web_cmd_processing_fp[kSetPhaseLockOffset] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetPhaseLockOffset;
	web_cmd_processing_fp[kSetDhcponoff] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetDhcpOnOff;
	web_cmd_processing_fp[kSetProtocol] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetProtocol;
	web_cmd_processing_fp[kSetIp] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetIpAddress;
	web_cmd_processing_fp[kSetSubnet] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetSubnetMask;
	web_cmd_processing_fp[kSetGateway] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetGateway;
//process_web_request_fp[kGetUpdateImage] = (int32_t (*)(struct tcp_pcb*, char*, int, int));
	web_cmd_processing_fp[kSetnetworkconfig] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetNetworkConfig;
	web_cmd_processing_fp[kGetcheckpwd] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetPassword;
	web_cmd_processing_fp[kSetAdminInfo] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetAdminInfo;
	web_cmd_processing_fp[kGetAdminInfo] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetAdminInfo;
	web_cmd_processing_fp[kSetUploadGolden] = (int32_t (*)(struct tcp_pcb*, char*, int, int))UpdateGoldenImage;
	web_cmd_processing_fp[kSetUpdateDate] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetUpdateDate;
	web_cmd_processing_fp[kSetNoiseonoff] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetNoiseFilterOnOff;
	web_cmd_processing_fp[kSetVNoiseonoff] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetVNoiseFilterOnOff;
	web_cmd_processing_fp[kSetVoltageonoff] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetVthFilterOnOff;
//process_web_request_fp[kSetGoldenVer] = (int32_t (*)(struct tcp_pcb*, char*, int, int));
	web_cmd_processing_fp[kSetGoldenDate] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetGoldenImageUpdateDate;
	web_cmd_processing_fp[kSetLidarDelayInfo] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetRoutingDelay;
	web_cmd_processing_fp[kSetLidarThInfo] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetThresholdVoltage;
	web_cmd_processing_fp[kGetLidarDelayInfo] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetRoutingDelay;
	web_cmd_processing_fp[kGetLidarThInfo] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetThresholdVoltage;
	web_cmd_processing_fp[kRoutingCal] = (int32_t (*)(struct tcp_pcb*, char*, int, int))CalRoutingDelay;
	web_cmd_processing_fp[kAddRoutingDelay] = (int32_t (*)(struct tcp_pcb*, char*, int, int))AddRoutingDelay;
	web_cmd_processing_fp[kRegWrite] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetRegWrite;
	web_cmd_processing_fp[kIntenVth] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetIntenVth;
	web_cmd_processing_fp[kRegRead] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetRegRead;
	web_cmd_processing_fp[kSetNoiseonoff2] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetNoiseFilterOnOff2;
	web_cmd_processing_fp[kSetRegisterValue] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetRegisterValue;
	web_cmd_processing_fp[kGetRegisterValue] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetRegisterValue;
	web_cmd_processing_fp[kGetLogMessage] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetLogMessage;
	web_cmd_processing_fp[kGetDtAlignInfo] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetDtAilgnInfo;
	web_cmd_processing_fp[kSetNoiseonoff4] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetNoiseFilterOnOff4;
	web_cmd_processing_fp[kSetHzOption] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetHzOption;
	web_cmd_processing_fp[kFixedCal] = (int32_t (*)(struct tcp_pcb*, char*, int, int))FixedCalRoutingDelay;
	web_cmd_processing_fp[kSetStopOption] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetStopOption;
	web_cmd_processing_fp[kSetUPLog] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetUPLog;
	web_cmd_processing_fp[kSendDarkAreaChannels] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SendDarkAreaChannels;
	web_cmd_processing_fp[kSendDarkAreaCorrectionParameter1] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SendDarkAreaCorrectionParameter1;
	web_cmd_processing_fp[kSendDarkAreaCorrectionParameter2] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SendDarkAreaCorrectionParameter2;
	web_cmd_processing_fp[kSendDarkAreaCorrectionParameter3] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SendDarkAreaCorrectionParameter3;
	web_cmd_processing_fp[kSendDarkAreaCorrectionParameter4] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SendDarkAreaCorrectionParameter4;
	web_cmd_processing_fp[kGetDarkAreaCorrectionSettings] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetDarkAreaCorrectionSettings;	//delete after

	web_cmd_processing_fp[kSaveTargetChannelDistanceRatio] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SaveTargetChannelDistanceRatio;
	web_cmd_processing_fp[kSaveCorrectedDistanceRatio] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SaveCorrectedDistanceRatio;
	web_cmd_processing_fp[kSaveDarkAreaSearchChannel] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SaveDarkAreaSearchChannel;
	web_cmd_processing_fp[kGetDarkAreaCorrectionSettingParams] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetDarkAreaCorrectionSettingParams;

	web_cmd_processing_fp[kSaveAzimuthRange] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SaveAzimuthRange;
	web_cmd_processing_fp[kSetAziCal] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetAziCal;

	web_cmd_processing_fp[kSetRoutingRange] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetRoutingRange;
	web_cmd_processing_fp[kSetRoutingdelay] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SetRoutingdelay2;
	web_cmd_processing_fp[kGetRoutingRange] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetRoutingRange;
	web_cmd_processing_fp[kRGetRoutingdelay] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetRoutingdelay2;

	web_cmd_processing_fp[kAddRoutingDelay2] = (int32_t (*)(struct tcp_pcb*, char*, int, int))AddRoutingDelay2;
	web_cmd_processing_fp[kRoutingCal2] = (int32_t (*)(struct tcp_pcb*, char*, int, int))CalRoutingDelay2;

	web_cmd_processing_fp[kSaveFIRs] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SaveFIRs;
	web_cmd_processing_fp[kGetFIRs] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetFIRs;

	web_cmd_processing_fp[kGetMinimumDctions] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetMinimumDetctions;

	web_cmd_processing_fp[kIsPhaseLocked] = (int32_t (*)(struct tcp_pcb*, char*, int, int))GetLocked;

	web_cmd_processing_fp[kSaveGFIRs] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SaveGFIRs;
	web_cmd_processing_fp[kSaveVFIRs] = (int32_t (*)(struct tcp_pcb*, char*, int, int))SaveVFIRs;





	lidar_calibration_fp[kChangeVth] = (void (*)(int, int))ChangeVth;
	lidar_calibration_fp[kChangeRoutingDelay] = (void (*)(int, int))ChangeRoutingDelay;
	lidar_calibration_fp[kChangeEmPulse] = (void (*)(int, int))ChangeEmPulse;
	lidar_calibration_fp[kChangeDistanceOffset] = (void (*)(int, int))ChangeDistanceOffset;
	lidar_calibration_fp[kChangeDistanceMinLimit] = (void (*)(int, int))ChangeDistanceMinLimit;
	lidar_calibration_fp[kChangeDistanceMaxLimit] = (void (*)(int, int))ChangeDistanceMaxLimit;
	lidar_calibration_fp[kChangeMinimumDetectionRange] = (void (*)(int, int))ChangeMinimumDetectionRange;
	lidar_calibration_fp[kChangeHorizontalAngleOffset] = (void (*)(int, int))ChangeHorizontalAngleOffset;
	lidar_calibration_fp[kChangeNoiseFilterSettings] = (void (*)(int, int))ChangeNoiseFilterSettings;
	lidar_calibration_fp[kChangeRemoveGhostSettings] = (void (*)(int, int))ChangeRemoveGhostSettings;
	lidar_calibration_fp[kChangeRPM] = (void (*)(int, int))ChangeRPM;
	lidar_calibration_fp[kChangeLdChannelEnable] = (void (*)(int, int))ChangeLdChannelEnable;
	lidar_calibration_fp[kChangeLdChannelDelay] = (void (*)(int, int))ChangeLdChannelDelay;
	lidar_calibration_fp[kChangeLdChannelTdTrDelay] = (void (*)(int, int))ChangeLdChannelTdTrDelay;
	lidar_calibration_fp[kChangeLdChannelTdTfDelay] = (void (*)(int, int))ChangeLdChannelTdTfDelay;
	lidar_calibration_fp[kChangeMotorAccelerationTime] = (void (*)(int, int))ChangeSetMotorAccelerationTime;
	lidar_calibration_fp[kChangeMotorAccelerationResolution] = (void (*)(int, int))ChangeSetMotorAccelerationResolution;
	lidar_calibration_fp[kHistogramStartVth] = (void (*)(int, int))ChangeHistogramStartVth;
	lidar_calibration_fp[kChangeStartEncoderCount] = (void (*)(int, int))ChangeStartEncoderCount;
	lidar_calibration_fp[kChangeEndEncoderCount] = (void (*)(int, int))ChangeEndEncoderCount;
	lidar_calibration_fp[kChangeHorizontalFovAngle] = (void (*)(int, int))ChangeHorizontalFovAngle;
	lidar_calibration_fp[kChangeVerticalFovAngle] = (void (*)(int, int))ChangeVerticalFovAngle;
	lidar_calibration_fp[kChangeTdcMaxDistance] = (void (*)(int, int))ChangeTdcMaxDistance;
	lidar_calibration_fp[kChangeLdDefaultValue] = (void (*)(int, int))ChangeLdDefaultValue;
	lidar_calibration_fp[kChangeDtAlignInfo] = (void (*)(int, int))ChangeDtAlignInfo;
	lidar_calibration_fp[khhjTest] = (void (*)(int, int))hhjwebTest;
	lidar_calibration_fp[kDarkAreaCorrection] = (void (*)(int, int))ChangeDarkAreaCorrectionSetting;
	lidar_calibration_fp[kDarkAreaCorrectionv2] = (void (*)(int, int))ChangeDarkAreaCorrectionSetting_v2;
	lidar_calibration_fp[kChangeRoutingDelay2] = (void (*)(int, int))ChangeRoutingDelay2;



}

void init_webserver()
{
	init_hash_table((volatile struct Node**)web_cmd_string_to_enum_map);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/progress", kGetProgress);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/CurrentVth", kGetCurrentVth);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/upload", kSetUpload);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/reset", kSetReset);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/sysinfo", kGetSysinfo);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/hwinfo", kGetHwinfo);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/diagnostics", kGetDiagnostics);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/sendoffset", kSetSendoffset);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/laseronoff", kSetLaseronoff);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/echo", kSetEcho);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/motorrpm", kSetMotorRpm);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/fovstartangle", kSetFovStartAngle);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/fovendangle", kSetFovEndAngle);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/phaselockoffset", kSetPhaseLockOffset);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/dhcponoff", kSetDhcponoff);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/protocol", kSetProtocol);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/ip", kSetIp);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/subnet", kSetSubnet);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/gateWay", kSetGateway);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/updateimage", kGetUpdateImage);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/networkconfig", kSetnetworkconfig);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/checkpwd", kGetcheckpwd);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/admininfo", kSetAdminInfo);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/admininfo", kGetAdminInfo);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/uploadgolden", kSetUploadGolden);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/updatedate", kSetUpdateDate);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/noiseonoff", kSetNoiseonoff);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/vnoiseonoff", kSetVNoiseonoff);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/voltageonoff", kSetVoltageonoff);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/goldenver", kSetGoldenVer);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/goldendate", kSetGoldenDate);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/routingdelay", kSetLidarDelayInfo);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/vth", kSetLidarThInfo);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/routingdelay", kGetLidarDelayInfo);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/vth", kGetLidarThInfo);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/senddelaycal", kRoutingCal);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/sendadddelay", kAddRoutingDelay);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/sendregwrite", kRegWrite);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/sendIntenVth", kIntenVth);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/sendregread", kRegRead);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/NoiseOnOff2", kSetNoiseonoff2);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/RegisterValue", kSetRegisterValue);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/RegisterValue", kGetRegisterValue);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/GetLogMessage", kGetLogMessage);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/NoiseONOff4", kSetNoiseonoff4);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/sethzoption", kSetHzOption);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/FixedCal", kFixedCal);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/setstopoption", kSetStopOption);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SetUPLog", kSetUPLog);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SendDarkAreaChannels", kSendDarkAreaChannels);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SendDarkAreaCorrectionParameter1", kSendDarkAreaCorrectionParameter1);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SendDarkAreaCorrectionParameter2", kSendDarkAreaCorrectionParameter2);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SendDarkAreaCorrectionParameter3", kSendDarkAreaCorrectionParameter3);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SendDarkAreaCorrectionParameter4", kSendDarkAreaCorrectionParameter4);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/GetDarkAreaCorrectionSettings", kGetDarkAreaCorrectionSettings);	//delete after
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SaveTargetChannelsDistanceRatio", kSaveTargetChannelDistanceRatio);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SaveCorrectedDistanceRatio", kSaveCorrectedDistanceRatio);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SaveDarkAreaSearchChannel", kSaveDarkAreaSearchChannel);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/GetDarkAreaCorrectionSettingParams", kGetDarkAreaCorrectionSettingParams);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SaveAzimuthRange", kSaveAzimuthRange);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SetAziCal", kSetAziCal);	//jh
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SetRoutingRange", kSetRoutingRange);	//hhj

	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/routingdelay2", kSetRoutingdelay);	//hhj
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/GetRoutingRange", kGetRoutingRange);	//hhj
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/routingdelay2", kRGetRoutingdelay);	//hhj

	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/sendadddelay2", kAddRoutingDelay2);	//sjh
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/senddelaycal2", kRoutingCal2);

	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SaveFIRs", kSaveFIRs);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/GetFIRs", kGetFIRs);

	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/Getminimums", kGetMinimumDctions);


	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_get/isphaselocked", kIsPhaseLocked);

	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SaveGFIRs", kSaveGFIRs);
	add_item((volatile struct Node**)web_cmd_string_to_enum_map, "/cmd_set/SaveVFIRs", kSaveVFIRs);

	char pwd[20] = "AutoL5328";
	strcpy(password, pwd);

	InitFp();
//	add_item(post_cmd_hash_tb, "", );
}
