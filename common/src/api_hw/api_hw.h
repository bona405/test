/*
 * api_hw.h
 *
 *  Created on: 2023. 2. 3.
 *      Author: wonju
 */

#ifndef SRC_API_HW_API_HW_H_
#define SRC_API_HW_API_HW_H_

#include "xstatus.h"
#include "api_hw_enc_ip_ctrl.h"
#include "api_hw_heat_ip_ctrl.h"
#include "api_hw_ld_ip_ctrl.h"
#include "api_hw_mcp4801_ip_ctrl.h"
#include "api_hw_mcp4822_ip_ctrl.h"
#include "api_hw_motor_ip_ctrl.h"
#include "api_hw_tdc_ip_ctrl.h"
#include "api_hw_version_info_ip.h"
//#include "api_hw_util_logic_ip.h"
#include <stdlib.h>

#define IP_SIZE 10

typedef struct HwIpRegisterInfo{
char ip_name[20];
u32 base_addr;
u32 (*GetRegisterValue)(u32 offset);
XStatus (*SetRegisterValue)(u32 offset, u32 value);
u32 max_register_offset;
}HwIpRegisterInfo;

static HwIpRegisterInfo hw_ip_register_info[IP_SIZE] =
{
	{"EncCtrl", ENC_CTRL_BASEADDR, GetEncRegisterValue, SetEncRegisterValue, 13},
	{"HeatCtrl", HEAT_CTRL_BASEADDR, GetHeatRegisterValue, SetHeatRegisterValue, 8},
	{"LdCtrl", LD_CTRL_BASEADDR, GetLdRegisterValue, SetLdRegisterValue, 24},
	{"MCP4801Ctrl", MCP4801_CTRL_BASEADDR, GetMCP4801RegisterValue, SetMCP4801RegisterValue, 5},
	{"MCP4822Ctrl", MCP4822_CTRL_BASEADDR, GetMCP4822RegisterValue, SetMCP4822RegisterValue, 21},
	{"MotorCtrl", MOTOR_CTRL_BASEADDR, GetMotorRegisterValue, SetMotorRegisterValue, 8},
	{"TdcCtrl", TDC_CTRL_BASEADDR, GetTdcRegisterValue, SetTdcRegisterValue, 21},
	{"VersionInfo", VERSION_INFO_BASEADDR, GetVersionRegisterValue, SetVersionRegisterValue, 4}
};

int HwRegisterInfoCompareFn(void *a, void *b);
void SetHwIpRegisterValue(char* ip_name, u32 reg_offset, u32 value);
u32 GetHwIpRegisterValue(char* ip_name, u32 reg_offset);


#endif /* SRC_API_HW_API_HW_H_ */
