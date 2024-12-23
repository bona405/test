#ifndef API_VERSION_INFO_IP_H
#define API_VERSION_INFO_IP_H

#ifdef   API_VERSION_INFO_IP_LOCAL
#define  EXT_API_VERSION_INFO_IP_DEF
#else
#define  EXT_API_VERSION_INFO_IP_DEF   extern
#endif

#include "../../../common/src/api_hw/api_common.h"
#include "ip_version_info.h"

#define VERSION_INFO_BASEADDR       XPAR_IP_VERSION_INFO_0_S00_AXILITE_BASEADDR
#define VERSION_INFO_REG0_OFFSET    IP_VERSION_INFO_S00_AXILite_SLV_REG0_OFFSET
#define VERSION_INFO_REG1_OFFSET    IP_VERSION_INFO_S00_AXILite_SLV_REG1_OFFSET
#define VERSION_INFO_REG2_OFFSET    IP_VERSION_INFO_S00_AXILite_SLV_REG2_OFFSET
#define VERSION_INFO_REG3_OFFSET    IP_VERSION_INFO_S00_AXILite_SLV_REG3_OFFSET

EXT_API_VERSION_INFO_IP_DEF s32 FpgaVersionInfoInit(void);
EXT_API_VERSION_INFO_IP_DEF s32 FpgaVersionInfoSelfTest(void);
EXT_API_VERSION_INFO_IP_DEF s32 FpgaPrintVersionInfo(void);
EXT_API_VERSION_INFO_IP_DEF s32 FpgaReadVersionInfo(u8 *Version);

u32 GetVersionRegisterValue(u32 offset);
XStatus SetVersionRegisterValue(u32 offset, u32 value);

#endif // API_VERSION_INFO_IP_H

