///*
// * lidar_hw_info.h
// *
// *  Created on: 2022. 12. 2.
// *      Author: Wonju
// */
//
//#ifndef SRC_MODELS_LIDAR_HW_INFO_H_
//#define SRC_MODELS_LIDAR_HW_INFO_H_
//
//#include <xil_types.h>
//#include <malloc.h>
//
//#include "../../api_hw/api_hw_enc_ip_ctrl.h"
//#include "../../api_hw/api_hw_heat_ip_ctrl.h"
//#include "../../api_hw/api_hw_ld_ip_ctrl.h"
//#include "../../api_hw/api_hw_mcp4801_ip_ctrl.h"
//#include "../../api_hw/api_hw_mcp4822_ip_ctrl.h"
//#include "../../api_hw/api_hw_motor_ip_ctrl.h"
//#include "../../api_hw/api_hw_version_info_ip.h"
//#include "../../api_hw/api_hw_tdc_ip_ctrl.h"
//
//#define null (void*)0
//
//#pragma pack(push, 4)
//typedef struct IpEncoderCtrl
//{
//	u32 BaseAddr;
//	union IpEncoderCtrlRegisterMap
//	{
//		u32 register_offset[64];
//		struct{
//			u8 reserved[4];
//			u32 enc_resolution;
//			u32 double_enc_cnt_clr;
//			u32 quad_enc_cnt_clr;
//			u32 double_enc_cnt;
//			u32 quad_enc_cnt;
//			u32 double_deg_2pi_cnt;
//			u32 quad_deg_2pi_cnt;
//		};
//	} register_map;
//}IpEncoderCtrl;
//#pragma pack(pop)
//
//#pragma pack(push, 4)
//typedef struct IpHeatCtrl
//{
//	u32 BaseAddr;
//	union IpHeatCtrlRegisterMap
//	{
//		u32 register_offset[64];
//		struct{
//			u32 heat_en;
//			u32 heat_fail;
//			u32 heat_freq_value;
//			u32 heat_duty_value;
//		};
//	}register_map;
//}IpHeatCtrl;
//#pragma pack(pop)
//
//#pragma pack(push, 4)
//typedef struct IpLdCtrl
//{
//	u32 BaseAddr;
//	union IpLdCtrlRegisterMap
//	{
//		u32 register_offset[128];
//		struct{
//			u32 start;
//			u32 ld_ch1_en;
//			u32 ld_ch2_en;
//			u32 ld_ch3_en;
//			u32 ld_ch4_en;
//			u32 ld_ch1_dly;
//			u32 ld_ch2_dly;
//			u32 ld_ch3_dly;
//			u32 ld_ch4_dly;
//			u32 tdtr_ch1_dly;
//			u32 tdtr_ch2_dly;
//			u32 tdtr_ch3_dly;
//			u32 tdtr_ch4_dly;
//			u32 tdtf_ch1_dly;
//			u32 tdtf_ch2_dly;
//			u32 tdtf_ch3_dly;
//			u32 tdtf_ch4_dly;
//			u32 ld_com_dly_sel;
//			u32 en_deg_start1;
//			u32 en_deg_stop1;
//			u32 en_deg_start2;
//			u32 en_deg_stop2;
//		};
//	}register_map;
//}IpLdCtrl;
//#pragma pack(pop)
//
//#pragma pack(push, 4)
//typedef struct IpMcp4801Ctrl
//{
//	u32 BaseAddr;
//	union IpMcp4801CtrlRegisterMap
//	{
//		u32 register_offset[64];
//		struct{
//			u32 setup;
//			u32 status;
//			u32 data;
//		};
//	}register_map;
//}IpMcp4801Ctrl;
//#pragma pack(pop)
//
//#pragma pack(push, 4)
//typedef struct IpMcp4822Ctrl
//{
//	u32 BaseAddr;
//	union IpMcp4822CtrlRegisterMap{
//		u32 register_offset[64];
//		struct{
//			u32 setup;
//			u32 status;
//			u32 select;
//			u32 vth[16];
//		};
//	}register_map;
//}IpMcp4822Ctrl;
//#pragma pack(pop)
//
//#pragma pack(push, 4)
//typedef struct IpMotorCtrl
//{
//	u32 BaseAddr;
//	union IpMotorCtrlRegisterMap{
//		u32 register_offset[64];
//		struct{
//			u32 motor_en;
//			u32 motor_rpm;
//			u32 motor_break;
//			u32 motor_freq_value;
//			u32 ref_clk_freq;
//		};
//	}register_map;
//}IpMotorCtrl;
//#pragma pack(pop)
//
//#pragma pack(push, 4)
//typedef struct IpVersionInfo
//{
//	u32 BaseAddr;
//	union IpVersionInfoRegisterMap{
//		u32 register_offset[64];
//		struct{
//			char str_version1[4];
//			char str_version2[4];
//			char str_version3[4];
//			char str_version4[4];
//		};
//	}register_map;
//}IpVersionInfo;
//#pragma pack(pop)
//
//#pragma pack(push, 4)
//typedef struct IpTdcCtrl
//{
//	u32 BaseAddr;
//	union IpTdcCtrlRegisterMap{
//		u32 register_offset[64];
//		struct{
//			u32 cal_ch_sel;
//			u32 cal_start;
//			u32 cal_clear;
//			u32 cal_done;
//			u32 echo_cnt;
//		};
//	}register_map;
//}IpTdcCtrl;
//#pragma pack(pop)
//
//#pragma pack(push, 4)
//typedef struct LidarRegisterInfo
//{
//	IpEncoderCtrl ip_encoder_ctrl;
//	IpHeatCtrl ip_heat_ctrl;
//	IpLdCtrl ip_ld_ctrl;
//	IpMcp4801Ctrl ip_mcp4801_ctrl;
//	IpMcp4822Ctrl ip_mcp4822_ctrl;
//	IpMotorCtrl ip_motor_ctrl;
//	IpTdcCtrl ip_tdc_ctrl;
//	IpVersionInfo ip_version_info;
//}LidarRegisterInfo;
//#pragma pack(pop)
//
//LidarRegisterInfo* GetLidarRegisterInfo();
//
//void DeleteLidarRegisterInfo();
//
//void InitLidarRegisterInfo(LidarRegisterInfo* lidar_hw_info);
//
//#endif /* SRC_MODELS_LIDAR_HW_INFO_H_ */
