/*
 * init_setup_variable.h
 *
 *  Created on: 2022. 7. 29.
 *      Author: Wonju
 */

#ifndef SRC_SETUP_INIT_LIDAR_H_
#define SRC_SETUP_INIT_LIDAR_H_

#include "../../../common/src/api_sw/models/machine_info.h"
#include "../platform/platform_config.h"
#include "../platform/platform.h"
#include "../../../common/src/api_sw/flash/flash.h"
#include "lwip/init.h"
#include "netif/xadapter.h"
#include "../udp/udp_server.h"
#include "../tcp/tcp_server.h"
#include "stdlib.h"

#include "../../../common/src/api_hw/api_hw_enc_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_heat_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_ld_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_mcp4801_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_mcp4822_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_motor_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_tdc_ip_ctrl.h"
#include "../../../common/src/api_hw/api_hw_version_info_ip.h"
#include "../../../common/src/api_hw/api_hw_xadcps_ip.h"
#include "../../../common/src/api_hw/api_hw_xbram_histo.h"
#include "../../../common/src/api_hw/api_hw_xbram_lut.h"

int32_t InitNetworkSettings(struct netif *netif, struct LidarState* lidar_state_, struct LidarState_Ch* lidar_state_Ch, struct LidarState_Ch* lidar_state_Ch2);
s32 HWInit();
void InitRoutingDelay(int routing_delay[32], struct LidarState_Ch *lidar_state_Ch);



XStatus InitMinimumDetectionRange(LidarState* lidar_state);
XStatus InitLdEnabledRange(LidarState* lidar_state);
#endif /* SRC_SETUP_INIT_LIDAR_H_ */
