///*
// * lidar_hw_info.c
// *
// *  Created on: 2022. 12. 2.
// *      Author: Wonju
// */
//
//
//#include "lidar_hw_info.h"
//
//LidarRegisterInfo* GetLidarRegisterInfo()
//{
//	static LidarRegisterInfo* lidar_hw_info_ = null;
//	if(lidar_hw_info_ == null)
//	{
//		lidar_hw_info_ = (LidarRegisterInfo*)malloc(sizeof(LidarRegisterInfo));
//		InitLidarRegisterInfo(lidar_hw_info_);
//	}
//
//	return lidar_hw_info_;
//}
//
//void DeleteLidarRegisterInfo()
//{
//	LidarRegisterInfo* lidar_hw_info = GetLidarRegisterInfo();
//	free(lidar_hw_info);
//}
//
//void InitLidarRegisterInfo(LidarRegisterInfo* lidar_hw_info)
//{
//	lidar_hw_info->ip_encoder_ctrl.BaseAddr = ENC_CTRL_BASEADDR;
//	lidar_hw_info->ip_heat_ctrl.BaseAddr = HEAT_CTRL_BASEADDR;
//	lidar_hw_info->ip_ld_ctrl.BaseAddr = LD_CTRL_BASEADDR;
//	lidar_hw_info->ip_mcp4801_ctrl.BaseAddr = MCP4801_CTRL_BASEADDR;
//	lidar_hw_info->ip_mcp4822_ctrl.BaseAddr = MCP4822_CTRL_BASEADDR;
//	lidar_hw_info->ip_motor_ctrl.BaseAddr = MOTOR_CTRL_BASEADDR;
//	lidar_hw_info->ip_tdc_ctrl.BaseAddr = TDC_CTRL_BASEADDR;
//	lidar_hw_info->ip_version_info.BaseAddr = VERSION_INFO_BASEADDR;
//
//	u32* register_offset = lidar_hw_info->ip_encoder_ctrl.register_map.register_offset;
//	int register_offset_size = sizeof(lidar_hw_info->ip_encoder_ctrl.register_map.register_offset) / sizeof(lidar_hw_info->ip_encoder_ctrl.register_map.register_offset[0]);
//	for(int i = 0 ; i < register_offset_size ; i++)
//		register_offset[i] = i<<2;
//
//	register_offset = lidar_hw_info->ip_heat_ctrl.register_map.register_offset;
//	register_offset_size = sizeof(lidar_hw_info->ip_heat_ctrl.register_map.register_offset) / sizeof(lidar_hw_info->ip_heat_ctrl.register_map.register_offset[0]);
//	for(int i = 0 ; i < register_offset_size ; i++)
//		register_offset[i] = i<<2;
//
//	register_offset = lidar_hw_info->ip_ld_ctrl.register_map.register_offset;
//	register_offset_size = sizeof(lidar_hw_info->ip_ld_ctrl.register_map.register_offset) / sizeof(lidar_hw_info->ip_ld_ctrl.register_map.register_offset[0]);
//	for(int i = 0 ; i < register_offset_size ; i++)
//		register_offset[i] = i<<2;
//
//	register_offset = lidar_hw_info->ip_mcp4801_ctrl.register_map.register_offset;
//	register_offset_size = sizeof(lidar_hw_info->ip_mcp4801_ctrl.register_map.register_offset) / sizeof(lidar_hw_info->ip_mcp4801_ctrl.register_map.register_offset[0]);
//	for(int i = 0 ; i < register_offset_size ; i++)
//		register_offset[i] = i<<2;
//
//	register_offset = lidar_hw_info->ip_mcp4822_ctrl.register_map.register_offset;
//	register_offset_size = sizeof(lidar_hw_info->ip_mcp4822_ctrl.register_map.register_offset) / sizeof(lidar_hw_info->ip_mcp4822_ctrl.register_map.register_offset[0]);
//	for(int i = 0 ; i < register_offset_size ; i++)
//		register_offset[i] = i<<2;
//
//	register_offset = lidar_hw_info->ip_motor_ctrl.register_map.register_offset;
//	register_offset_size = sizeof(lidar_hw_info->ip_motor_ctrl.register_map.register_offset) / sizeof(lidar_hw_info->ip_motor_ctrl.register_map.register_offset[0]);
//	for(int i = 0 ; i < register_offset_size ; i++)
//		register_offset[i] = i<<2;
//
//	register_offset = lidar_hw_info->ip_tdc_ctrl.register_map.register_offset;
//	register_offset_size = sizeof(lidar_hw_info->ip_tdc_ctrl.register_map.register_offset) / sizeof(lidar_hw_info->ip_tdc_ctrl.register_map.register_offset[0]);
//	for(int i = 0 ; i < register_offset_size ; i++)
//		register_offset[i] = i<<2;
//
//	register_offset = lidar_hw_info->ip_version_info.register_map.register_offset;
//	register_offset_size = sizeof(lidar_hw_info->ip_version_info.register_map.register_offset) / sizeof(lidar_hw_info->ip_version_info.register_map.register_offset[0]);
//	for(int i = 0 ; i < register_offset_size ; i++)
//		register_offset[i] = i<<2;
//}
//
//void SaveLidarRegisterInfoToFlash()
//{
////	LidarHwInfo lidar_hw_info_for_flash_mem;
////	u32 res = xil_out32(GetLidarHwInfo()->ip_encoder_ctrl);
////	u32 res = xil_out32(lidar_hw_info_->ip_tdc_ctrl.register_map.cal_clear);
////
////	FlashWrite(lidar_hw_info_for_flash_mem)
//}
