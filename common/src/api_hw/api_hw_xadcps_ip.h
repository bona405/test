#ifndef API_XADCPS_IP_H
#define API_XADCPS_IP_H

#ifdef   API_XADCPS_IP_LOCAL
#define  EXT_API_XADCPS_IP_DEF
#else
#define  EXT_API_XADCPS_IP_DEF   extern
#endif
#include "../../../common/src/api_hw/api_common.h"
#include "xadcps.h"

#define XADC_DEVICE_ID  XPAR_XADCPS_0_DEVICE_ID

XAdcPs XAdcInst;
XAdcPs_Config *pXAdcPsConfig;

EXT_API_XADCPS_IP_DEF s32 XAdcPsInit(void);
EXT_API_XADCPS_IP_DEF s32 XAdcPsVoltageRead(u32 *voltInt, u32 *voltFrac, u8 type);

#endif // API_XADCPS_IP_H
