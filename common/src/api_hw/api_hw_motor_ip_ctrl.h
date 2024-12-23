#ifndef API_MOTOR_IP_CTRL_H
#define API_MOTOR_IP_CTRL_H

#ifdef   API_MOTOR_IP_CTRL_LOCAL
#define  EXT_API_MOTOR_IP_CTRL_DEF
#else
#define  EXT_API_MOTOR_IP_CTRL_DEF   extern
#endif
#include "../../../common/src/api_hw/api_common.h"
#include "ip_motor_ctrl.h"

#define MOTOR_CTRL_BASEADDR      XPAR_IP_MOTOR_CTRL_0_S00_AXILITE_BASEADDR
#define MOTOR_CTRL_REG0_OFFSET   IP_MOTOR_CTRL_S00_AXILite_SLV_REG0_OFFSET
#define MOTOR_CTRL_REG1_OFFSET   IP_MOTOR_CTRL_S00_AXILite_SLV_REG1_OFFSET
#define MOTOR_CTRL_REG2_OFFSET   IP_MOTOR_CTRL_S00_AXILite_SLV_REG2_OFFSET
#define MOTOR_CTRL_REG3_OFFSET   IP_MOTOR_CTRL_S00_AXILite_SLV_REG3_OFFSET


EXT_API_MOTOR_IP_CTRL_DEF s32 MotorCtrlInit(void);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorCtrlSelftest(void);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorCtrlSetDefault(void);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorCtrlSet(u32 rpm, u32 refclk);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorCtrlRefClkSet(u32 refclk);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorCtrlRPMSet(u32 rpm);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorBreakDisable(void);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorBreakEnable(void);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorRPMCheck(u32 *rpm);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorDisable(void);
EXT_API_MOTOR_IP_CTRL_DEF s32 MotorEnable(void);

u32 GetMotorRegisterValue(u32 offset);
XStatus SetMotorRegisterValue(u32 offset, u32 value);

EXT_API_MOTOR_IP_CTRL_DEF s32 MotorCtrlPPSInit();

#endif // API_MOTOR_IP_CTRL_H
