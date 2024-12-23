#ifndef API_HEAT_IP_CTRL_H
#define API_HEAT_IP_CTRL_H

#ifdef   API_HEAT_IP_CTRL_LOCAL
#define  EXT_API_HEAT_IP_CTRL_DEF
#else
#define  EXT_API_HEAT_IP_CTRL_DEF   extern
#endif

#include "../../../common/src/api_hw/api_common.h"
#include "ip_heat_ctrl.h"

#define HEAT_CTRL_BASEADDR      XPAR_IP_HEAT_CTRL_0_S00_AXILITE_BASEADDR
#define HEAT_CTRL_REG0_OFFSET   IP_HEAT_CTRL_S00_AXILite_SLV_REG0_OFFSET
#define HEAT_CTRL_REG1_OFFSET   IP_HEAT_CTRL_S00_AXILite_SLV_REG1_OFFSET
#define HEAT_CTRL_REG2_OFFSET   IP_HEAT_CTRL_S00_AXILite_SLV_REG2_OFFSET
#define HEAT_CTRL_REG3_OFFSET   IP_HEAT_CTRL_S00_AXILite_SLV_REG3_OFFSET
#define HEAT_CTRL_REG4_OFFSET   IP_HEAT_CTRL_S00_AXILite_SLV_REG4_OFFSET
#define HEAT_CTRL_REG5_OFFSET   IP_HEAT_CTRL_S00_AXILite_SLV_REG5_OFFSET
#define HEAT_CTRL_REG6_OFFSET   IP_HEAT_CTRL_S00_AXILite_SLV_REG6_OFFSET
#define HEAT_CTRL_REG7_OFFSET   IP_HEAT_CTRL_S00_AXILite_SLV_REG7_OFFSET

EXT_API_HEAT_IP_CTRL_DEF s32 HeatCtrlInit(void);
EXT_API_HEAT_IP_CTRL_DEF s32 HeatCtrlSelftest(void);
EXT_API_HEAT_IP_CTRL_DEF s32 HeatCtrlSetDefault();
EXT_API_HEAT_IP_CTRL_DEF s32 HeatCtrlSet(u32 freq, u32 duty);
EXT_API_HEAT_IP_CTRL_DEF s32 HeatCtrlDutySet(u32 duty);
EXT_API_HEAT_IP_CTRL_DEF s32 HeatCtrlFreqSet(u32 freq);
EXT_API_HEAT_IP_CTRL_DEF s32 HeatCtrlCheck(void);
EXT_API_HEAT_IP_CTRL_DEF s32 HeatCtrlDisable(void);
EXT_API_HEAT_IP_CTRL_DEF s32 HeatCtrlEnable(void);
u32 GetHeatRegisterValue(u32 offset);
XStatus SetHeatRegisterValue(u32 offset, u32 value);

#endif // API_HEAT_IP_CTRL_H

