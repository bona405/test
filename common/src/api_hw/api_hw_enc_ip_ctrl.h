#ifndef API_ENC_IP_CTRL_H
#define API_ENC_IP_CTRL_H

#ifdef   API_ENC_IP_CTRL_LOCAL
#define  EXT_API_ENC_IP_CTRL_DEF
#else
#define  EXT_API_ENC_IP_CTRL_DEF   extern
#endif

#include "../../../common/src/api_hw/api_common.h"
#include "ip_encoder_ctrl.h"

#define ENC_CTRL_BASEADDR       XPAR_IP_ENCODER_CTRL_0_S00_AXILITE_BASEADDR
#define ENC_CTRL_REG0_OFFSET    IP_ENCODER_CTRL_S00_AXILite_SLV_REG0_OFFSET
#define ENC_CTRL_REG1_OFFSET    IP_ENCODER_CTRL_S00_AXILite_SLV_REG1_OFFSET
#define ENC_CTRL_REG2_OFFSET    IP_ENCODER_CTRL_S00_AXILite_SLV_REG2_OFFSET
#define ENC_CTRL_REG3_OFFSET    IP_ENCODER_CTRL_S00_AXILite_SLV_REG3_OFFSET
#define ENC_CTRL_REG4_OFFSET    IP_ENCODER_CTRL_S00_AXILite_SLV_REG4_OFFSET
#define ENC_CTRL_REG5_OFFSET    IP_ENCODER_CTRL_S00_AXILite_SLV_REG5_OFFSET
#define ENC_CTRL_REG6_OFFSET    IP_ENCODER_CTRL_S00_AXILite_SLV_REG6_OFFSET
#define ENC_CTRL_REG7_OFFSET    IP_ENCODER_CTRL_S00_AXILite_SLV_REG7_OFFSET
#define ENC_CTRL_REG8_OFFSET	IP_ENCODER_CTRL_S00_AXILite_SLV_REG8_OFFSET
#define ENC_CTRL_REG9_OFFSET	IP_ENCODER_CTRL_S00_AXILite_SLV_REG9_OFFSET
#define ENC_CTRL_REG10_OFFSET	IP_ENCODER_CTRL_S00_AXILite_SLV_REG10_OFFSET
#define ENC_CTRL_REG11_OFFSET	IP_ENCODER_CTRL_S00_AXILite_SLV_REG11_OFFSET
#define ENC_CTRL_REG12_OFFSET	IP_ENCODER_CTRL_S00_AXILite_SLV_REG12_OFFSET

#define ENC_CTRL_REG13_OFFSET	IP_ENCODER_CTRL_S00_AXILite_SLV_REG13_OFFSET
#define ENC_CTRL_REG14_OFFSET	IP_ENCODER_CTRL_S00_AXILite_SLV_REG14_OFFSET
//#define ENC_CTRL_REG17_OFFSET	IP_ENCODER_CTRL_S00_AXILite_SLV_REG17_OFFSET

//#define ENC_CTRL_LD_START_ENC_CNT1 128
#define ENC_CTRL_LD_START_ENC_CNT1 170
//#define ENC_CTRL_LD_STOP_ENC_CNT1 897
#define ENC_CTRL_LD_STOP_ENC_CNT1 855
//#define ENC_CTRL_LD_START_ENC_CNT2 2174
//#define ENC_CTRL_LD_STOP_ENC_CNT2 2943

//#define ENC_CTRL_LD_START_ENC_CNT2 2176
#define ENC_CTRL_LD_START_ENC_CNT2 2218
//#define ENC_CTRL_LD_STOP_ENC_CNT2 2945
#define ENC_CTRL_LD_STOP_ENC_CNT2 2903

XStatus EncCtrlInit(void);
XStatus EncCtrlSelfTest(void);
XStatus EncCtrlSetDefault(void);

//XStatus EncCtrlSetEncCountOffset(int32_t start_enc_count_offset, int32_t end_enc_count_offset);

u8 EncCtrlGetEnable(void);
XStatus EncCtrlSetEnable(void);
XStatus EncCtrlSetDisable(void);

u32 EncCtrlGetResolutionMode();
XStatus EncCtrlSetResolutionMode(u32 mode);

XStatus SetDTEnable();

u32 EncCtrlGetStartEncCount1();
XStatus EncCtrlSetStartEncCount1(u32 enc_count);

u32 EncCtrlGetStartEncCount2();
XStatus EncCtrlSetStartEncCount2(u32 enc_count);

u32 EncCtrlGetStopEncCount1();
XStatus EncCtrlSetStopEncCount1(u32 enc_count);

u32 EncCtrlGetStopEncCount2();
XStatus EncCtrlSetStopEncCount2(u32 enc_count);

s32 EncDoubleCntClear(void);
s32 EncQuadCntClear(void);
s32 EncDoubleCntRead(u32 *cnt);
s32 EncQuadCntRead(u32 *cnt);
s32 EncDoubleRoundCntRead(u32 *cnt);
s32 EncQuadRoundCntRead(u32 *cnt);
s32 EncOneShot(void);

u32 GetEncRegisterValue(u32 offset);
XStatus SetEncRegisterValue(u32 offset, u32 value);

XStatus SetPseudoEncEdge(u32 value);

#pragma pack(push, 4)
typedef struct EncCtrlInfo
{
	u32 base_addr;
	union EncCtrlRegisterMap
	{
		u32 register_value[13];
		struct{
			u32 is_enabled;
			u32 enc_resolution;
			u32 double_enc_cnt_clr;
			u32 quad_enc_cnt_clr;
			u32 double_enc_cnt;
			u32 quad_enc_cnt;
			u32 double_deg_2pi_cnt;
			u32 quad_deg_2pi_cnt;
			u32 oneshot_enc;
			u32 en_deg_start1;
			u32 en_deg_stop1;
			u32 en_deg_start2;
			u32 en_deg_stop2;
		};
	} register_map;
}EncCtrlInfo;
#pragma pack(pop)

#endif // API_VERSION_INFO_IP_H

