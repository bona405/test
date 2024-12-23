#ifndef API_LD_IP_CTRL_H
#define API_LD_IP_CTRL_H

#ifdef   API_LD_IP_CTRL_LOCAL
#define  EXT_API_LD_IP_CTRL_DEF
#else
#define  EXT_API_LD_IP_CTRL_DEF   extern
#endif

#include "../../../common/src/api_hw/api_common.h"
#include "ip_ld_ctrl.h"

#define LD_CTRL_BASEADDR        XPAR_IP_LD_CTRL_0_S00_AXILITE_BASEADDR
#define LD_CTRL_REG0_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG0_OFFSET
#define LD_CTRL_REG1_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG1_OFFSET
#define LD_CTRL_REG2_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG2_OFFSET
#define LD_CTRL_REG3_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG3_OFFSET
#define LD_CTRL_REG4_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG4_OFFSET
#define LD_CTRL_REG5_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG5_OFFSET
#define LD_CTRL_REG6_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG6_OFFSET
#define LD_CTRL_REG7_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG7_OFFSET
#define LD_CTRL_REG8_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG8_OFFSET
#define LD_CTRL_REG9_OFFSET     IP_LD_CTRL_S00_AXILite_SLV_REG9_OFFSET
#define LD_CTRL_REG10_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG10_OFFSET
#define LD_CTRL_REG11_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG11_OFFSET
#define LD_CTRL_REG12_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG12_OFFSET
#define LD_CTRL_REG13_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG13_OFFSET
#define LD_CTRL_REG14_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG14_OFFSET
#define LD_CTRL_REG15_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG15_OFFSET
#define LD_CTRL_REG16_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG16_OFFSET
#define LD_CTRL_REG17_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG17_OFFSET
#define LD_CTRL_REG18_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG18_OFFSET
#define LD_CTRL_REG19_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG19_OFFSET
#define LD_CTRL_REG20_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG20_OFFSET
#define LD_CTRL_REG21_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG21_OFFSET
#define LD_CTRL_REG22_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG22_OFFSET
#define LD_CTRL_REG23_OFFSET    IP_LD_CTRL_S00_AXILite_SLV_REG23_OFFSET

#define LD_MAX_CH   4
#define LD_CH1      1
#define LD_CH2      2
#define LD_CH3      3
#define LD_CH4      4

#define LD_CONTROL 0

typedef struct {
#if LD_CONTROL
    u8 chEn[LD_MAX_CH];
#else
	u8 chEn;
	u8 chTdc;
#endif
    u16 chDly[LD_MAX_CH];
    u16 chDly1[LD_MAX_CH];
    u16 chDly2[LD_MAX_CH];
    u8 chTdTrDly[LD_MAX_CH];
    u8 chTdTfDly[LD_MAX_CH];
    u8 chOnCoarse[LD_MAX_CH];
    u8 chOnFine[LD_MAX_CH];
} HW_LD_CFG_OBJ;

EXT_API_LD_IP_CTRL_DEF s32 LDCtrlInit(void);
EXT_API_LD_IP_CTRL_DEF s32 LDCtrlSelfTest(void);
EXT_API_LD_IP_CTRL_DEF s32 LDCtrlSetDefault(void);
EXT_API_LD_IP_CTRL_DEF s32 LDAllSet();
EXT_API_LD_IP_CTRL_DEF s32 LDChSet(u8 ch, HW_LD_CFG_OBJ *ldCfg);
EXT_API_LD_IP_CTRL_DEF s32 LDChCfg(u8 ch, u8 chEn, u16 chDly, u8 chTdTrDly, u8 chTdTfDly, u8 chOnCoarse, u8 chOnFine, HW_LD_CFG_OBJ *ldCfg);
EXT_API_LD_IP_CTRL_DEF s32 LDStart(void);
s32 SetLdChon(u8 ch, u8 chOnCoarse, u8 chOnFine);
s32 LDCtrlSetDefaultValue();
s32 LDCtrlSetChannelEnable(size_t channel_index, u8 channel_enable);
s32 LDCtrlSetChannelDelay2(size_t channel_index, u16 channel_delay);
s32 LDCtrlSetChannelTdTrDelay(size_t channel_index, u16 channel_delay);
s32 LDCtrlSetChannelTdTfDelay(size_t channel_index, u16 channel_delay);
u32 GetLdRegisterValue(u32 offset);
XStatus SetLdRegisterValue(u32 offset, u32 data);
#endif // API_LD_IP_CTRL_H

