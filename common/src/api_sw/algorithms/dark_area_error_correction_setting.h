/*
 * dark_area_error_correction_setting.h
 *
 *  Created on: 2024. 6. 14.
 *      Author: wonju
 */

#ifndef SRC_API_SW_ALGORITHMS_DARK_AREA_ERROR_CORRECTION_SETTING_H_
#define SRC_API_SW_ALGORITHMS_DARK_AREA_ERROR_CORRECTION_SETTING_H_


#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct DarkAreaErrorCorrectionSetting
{
	uint8_t is_enable_dark_area_crror_correction_;
	uint8_t dark_area_channel_[32];
	uint8_t dark_area_channel_count_;
	uint8_t is_dark_area_channel_changed_;
	double dark_area_error_cor_param1_;
	double dark_area_error_cor_param2_;
	double dark_area_error_cor_param3_;
	double dark_area_error_cor_param4_;
	uint8_t is_enable_zero_dist_cor_;
}DarkAreaErrorCorrectionSetting;

#endif /* SRC_API_SW_ALGORITHMS_DARK_AREA_ERROR_CORRECTION_SETTING_H_ */
