/*
 * api_hw.c
 *
 *  Created on: 2023. 2. 3.
 *      Author: wonju
 */


#include "api_hw.h"

void InitHwIpRegisterInfo()
{
	qsort((void *)hw_ip_register_info, IP_SIZE, sizeof(HwIpRegisterInfo), (__compar_fn_t)HwRegisterInfoCompareFn);
}

int HwRegisterInfoCompareFn(void *a, void *b)
{
	return memcmp(((HwIpRegisterInfo*)a)->ip_name, ((HwIpRegisterInfo*)b)->ip_name, 4);
}

void SetHwIpRegisterValue(char* ip_name, u32 reg_offset, u32 value)
{
	HwIpRegisterInfo* hw_ip_reg_info = bsearch((void *)ip_name, (void *)hw_ip_register_info, IP_SIZE, sizeof(HwIpRegisterInfo), (__compar_fn_t)HwRegisterInfoCompareFn);

	if ( hw_ip_reg_info != NULL && reg_offset <= hw_ip_reg_info->max_register_offset * 4)
		hw_ip_reg_info->SetRegisterValue(reg_offset, value);
//	else
//		default_action();`
}

u32 GetHwIpRegisterValue(char* ip_name, u32 reg_offset)
{
	HwIpRegisterInfo* hw_ip_reg_info = bsearch((void *)ip_name, (void *)hw_ip_register_info, IP_SIZE, sizeof(HwIpRegisterInfo), (__compar_fn_t)HwRegisterInfoCompareFn);

	if ( hw_ip_reg_info != NULL && reg_offset <= hw_ip_reg_info->max_register_offset * 4)
		return hw_ip_reg_info->GetRegisterValue(reg_offset);
	else
		return -1;
}
