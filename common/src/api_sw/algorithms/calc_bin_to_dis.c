#include "calc_bin_to_dis.h"
#include "xil_printf.h"
//#include "../definitions/app_config.h"

#ifdef SI_P
extern uint32_t calc_bin_to_dis_fine(uint32_t in_val, uint16_t* out_val, int cal_val ,int clip_min, int clip_max, float decrease_percent, int bd_min, int bd_max)
{

	uint16_t tof = (uint16_t)in_val;
	int tof_final = 0;

	tof_final = tof + cal_val - clip_min;

	if(tof_final < 0)
	{
		tof_final = 0;
	}

	if(0 < tof && tof < bd_max*256)
	{
		*out_val = tof_final;
	}
	else
	{
		*out_val = 0;
	}
	return 0;
}
#else
extern uint32_t calc_bin_to_dis_fine(uint32_t in_val, uint16_t* out_val, int cal_val ,int clip_min, int clip_max, float decrease_percent, int bd_min, int bd_max)
{
	float crs = (float)(((in_val >> 14) & 0x000003ff ) * 0.4545f - 1);
	float fine = (float)(in_val & 0x00003fff) * 0.00002774f;
	float tof = crs-fine;
	uint16_t tof_cal_val = 0;

	if(tof <= -1.0f)
	{
		tof = 0;
		*out_val = 0;
	}
	else if(bd_min < tof && tof < bd_max)
	{
		tof_cal_val = (uint16_t)((tof * 1000 / 4));
		*out_val = tof_cal_val + cal_val - clip_min;
	}
	else
	{
		*out_val = 0;
	}

	return 0;
}
#endif




