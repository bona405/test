/*
 * machine_info.h
 *
 *  Created on: 2021. 11. 18.
 *      Author: ProDesk
 */

#ifndef SRC_MACHINE_INFO_H_
#define SRC_MACHINE_INFO_H_

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "xil_printf.h"
#include "../flash/flash.h"
#include "../algorithms/dark_area_error_correction_setting.h"

typedef struct NetworkSetting
{
	union
	{
		struct
		{
			uint8_t is_dhcp_on;
			char protocol[20];
			char ip[20];
			char subnet[20];
			char gateway[20];
			char mac[30];
			char port[20];
			char target_ip[20]; // hhj test
		};
		char padding[0x100];
	};
} NetworkSetting;

typedef struct MotorSetting
{
	union
	{
		struct
		{
			float motor_rpm;
			float fov_start_angle;
			float fov_end_angle;
			float phase_lock_offset;
			uint32_t motor_acceleration_time;
			uint32_t motor_acceleration_resolution;
			int32_t start_enc_count;
			int32_t end_enc_count;
			uint8_t hz_setting;	//0 : 10hz  1 : 20hz 2: 25hz
			int16_t total_enc_offset;
//			int16_t bottom_enc_offset;
//			int16_t top_enc_offset;
//			int16_t bottom_end_enc_offset;
//			int16_t top_end_enc_offset;
			uint8_t ispllon;	//
			uint32_t pll_point;	//
		};
		char padding[0x50];
	};
} MotorSetting;

typedef struct CalibrationSetting
{
	union
	{
		struct
		{
			uint8_t num_of_echo;
			char propagation_delay[30];
			char upper_offset[30];
			char lower_offset[30]; // 200
			char routing_delay[32][10];
			uint8_t detection_range_value;	//old
			uint32_t minimum_detection_range;
			uint32_t histogram_start_vth;
			int32_t minimum_detection_range_start1;
			int32_t minimum_detection_range_stop1;
			int32_t minimum_detection_range_start2;
			int32_t minimum_detection_range_stop2;
			int32_t total_azimuth_offset;
			uint8_t start_sig;
			uint8_t azioffset_enable;
			uint8_t temperadj_enable;
			uint8_t tempergain_enable;
		};
		char padding[0x500];
	};
} CalibrationSetting;

typedef struct FilterSetting
{
	union
	{
		struct
		{
			uint8_t is_noise_on;
			uint8_t is_voltage_on;
			uint8_t is_interference_on; // 3
			char noise_percent[10];
			char vth_sensitivity[10];
			char valid_point_cnt[10];
			char noise_cnt_limit[10];
			uint8_t is_noise_filter_on;
			uint8_t is_remove_ghost_on;
			char noise_min_value[10];
			uint8_t is_adaptive_pulse_width_on;
			uint8_t is_new_noise_filter_on;
			uint8_t is_heat_ctrl_on;
			uint8_t is_fir_on;
			struct DarkAreaErrorCorrectionSetting dark_area_error_correction_setting_;
			uint8_t is_intensity_correction_on;
		};
		char padding[0x100];
	};
} FilterSetting;

typedef struct LaserSetting
{
	union
	{
		struct
		{
			uint8_t is_laser_on;
			uint32_t em_pulse;
			uint32_t tdtr;
			uint32_t tdtf;
			uint32_t max_distance;
			uint32_t ch_interval_1;
			uint32_t ch_interval_2;
			uint8_t stop_option;
		};
		char padding[0x50];
	};
} LaserSetting;

typedef struct LidarInfo
{
	union
	{
		struct
		{
			char serial[20];
			char manufacturing_date[30];
			char firmware_ver[20];
			char fpga_ver[20];
			char firmware_u_date[30]; // 230
			char golden_ver[20];
			char golden_u_date[30];
			char boot_image_name[30];
		};
		char padding[0x100];
	};
} LidarInfo;

typedef struct DtAlignSettings
{
	union
	{
		struct
		{
			int32_t dt_start;
			int32_t dt_end;
			int32_t dt_interval;
			int32_t dt_check_interval;
			int32_t dt_average_count;
		};
		char padding[0x50];
	};
}DtAlignSettings;
typedef struct A2ZSetting
{
	union
	{
		struct
		{
			uint8_t near_dist;
			uint32_t far_vth;
			uint32_t near_vth;
			uint32_t far_em;
			uint32_t near_em;
			uint8_t state;
			uint8_t merge_dist;
			uint8_t clustering_onoff;
			uint8_t z_offset;
			uint8_t cc_eps;
			uint8_t cc_minpts;
			uint8_t ac_eps;
			uint8_t ac_minpts;
			uint8_t reserved01;
			uint8_t reserved02;
			int dist_offset;
			int fov_correction;
			float routing_range_1;
			float routing_range_2;
		};
		char padding[0x50];

	};

}A2ZSetting;

typedef struct DarkAreaErrorCorrectionSetting2
{
	union
	{
		struct
		{
			uint8_t is_enable_dark_area_crror_correction_;		// 1
			uint8_t dark_area_search_channel_[32][16];			// 512
			uint8_t dark_area_channel_count_;					// 1
			uint8_t is_dark_area_channel_changed_;				// 1
			uint8_t dark_area_error_cor_param1_[32];				// 32
			uint8_t dark_area_error_cor_param2_[32];				// 32
			float dark_area_error_cor_param3_;					// 8
			float dark_area_error_cor_param4_;					// 8
			uint8_t is_enable_zero_dist_cor_;					// 1
			uint16_t total_azimuth_cnt;							// 2
			uint16_t azimuth_range[2];							// 4
		};														// 794 -> 0x31a
		char padding[0x280];

	};

}DarkAreaErrorCorrectionSetting2;


typedef struct FIRSetting
{
	union
	{
		struct
		{
			uint32_t threshold1;
			uint32_t threshold2;
			uint8_t	 max_count;
			uint32_t distance1;
			uint32_t distance2;
			uint8_t	 count1;
			uint8_t	 count2;
			uint8_t FIR_COEF[7];
			uint8_t	 ison;
		};
		char padding[0x50];
	};
} FIRSetting;


typedef struct LidarState
{
	union
	{
		struct
		{
			LidarInfo lidar_info_;						//0x100
			LaserSetting laser_setting_;				//0x50
			MotorSetting motor_setting_;				//0x50
			CalibrationSetting calibration_setting_;	//0x500
			NetworkSetting network_setting_;			//0x100
			FilterSetting filter_setting_;				//0x100
			DtAlignSettings dt_align_settings_;			//0x50
			A2ZSetting a2z_setting;						//0x50
			FIRSetting FIR_setting;						//0x50
			FIRSetting FIR_setting_vremove;				//0x50
			FIRSetting FIR_setting_ground;				//0x50
		};
		unsigned char padding[LIDAR_STATE_SIZE];
	};

} LidarState;

typedef struct DarkAreaErrorCorrectionSettingState
{
	union
	{
		struct
		{
			DarkAreaErrorCorrectionSetting2 darkarea_setting_;
			DarkAreaErrorCorrectionSetting2 darkarea_setting_2;
			DarkAreaErrorCorrectionSetting2 darkarea_setting_3;
		};
		unsigned char padding[DARK_AREA_SIZE];
	};
}DarkAreaErrorCorrectionSettingState;

typedef struct FovTest
{
	float start_horizontal_angle;
	float end_horizontal_angle;
	int32_t start_vertical_angle;
	uint32_t end_vertical_angle;
} FovTest;

typedef struct LidarState_Ch
{
	union
	{
		struct
		{
			char delay_ch1[10];
			char delay_ch2[10];
			char delay_ch3[10];
			char delay_ch4[10];
			char delay_ch5[10];
			char delay_ch6[10];
			char delay_ch7[10];
			char delay_ch8[10];
			char delay_ch9[10];
			char delay_ch10[10];
			char delay_ch11[10];
			char delay_ch12[10];
			char delay_ch13[10];
			char delay_ch14[10];
			char delay_ch15[10];
			char delay_ch16[10];
			char delay_ch17[10];
			char delay_ch18[10];
			char delay_ch19[10];
			char delay_ch20[10];
			char delay_ch21[10];
			char delay_ch22[10];
			char delay_ch23[10];
			char delay_ch24[10];
			char delay_ch25[10];
			char delay_ch26[10];
			char delay_ch27[10];
			char delay_ch28[10];
			char delay_ch29[10];
			char delay_ch30[10];
			char delay_ch31[10];
			char delay_ch32[10];
			char th_ch1[30];
			char th_ch2[30];
			char th_ch3[30];
			char th_ch4[30];
			char th_ch5[30];
			char th_ch6[30];
			char th_ch7[30];
			char th_ch8[30];
			char th_ch9[30];
			char th_ch10[30];
			char th_ch11[30];
			char th_ch12[30];
			char th_ch13[30];
			char th_ch14[30];
			char th_ch15[30];
			char th_ch16[30];
		};
		unsigned char padding[SUBSECTOR_4K_SIZE];
	};
} LidarState_Ch;
typedef enum eProtocol
{
	tcp, udp
} eProtocol;

//azimuth offset
typedef struct AziOffsets
{
	int ch_datas[16];
} AziOffsets;



typedef struct AzimuthCalibrationSetting
{
	size_t calibration_info_count;
}AzimuthCalibrationSetting;

typedef struct AzimuthCalibrationInfo
{
	uint16_t col;
	uint8_t row;
	float offset;
}AzimuthCalibrationInfo;


typedef struct Azioffsets_Ch{
    int ch[32];
}Azioffsets_Ch;

typedef struct Azioffsets_Azi{
    Azioffsets_Ch Offsets[684];
}Azioffsets_Azi;

typedef struct Azioffsets_{
	union
	{
		struct
		{
		    Azioffsets_Azi AziGroup[4];
		};
		unsigned char padding[AZI_OFFSET_SIZE];
	};
}Azioffsets_;



//dataset[col].channel[row].distance += offset;
typedef struct AzimuthCal
{
	union
	{
		struct
		{
//			AziOffsets AziOffsets_[4096];
			AzimuthCalibrationSetting azimuth_calibration_setting;
			AzimuthCalibrationInfo azimuth_calibration_info[21888];
		};
		unsigned char padding[SUBSECTOR_32K_SIZE*6];
	};
} AzimuthCal;


struct LidarState lidar_state_;
struct FovTest fov_test_;
struct LidarState_Ch lidar_state_Ch;
struct LidarState_Ch lidar_state_Ch2;
struct DarkAreaErrorCorrectionSettingState dark_area_state_;
struct AzimuthCal azi_cal_;


typedef struct Temperoffsets_Ch{
	int temperature;
	int nb_bias;
    int ch[32];
}Temperoffsets_Ch;

struct Azioffsets_ azi_offsets_;

struct Temperoffsets_Ch temper_offsets_;

void InitLidarState(struct LidarState* lidar_state);

uint8_t HasFF(struct LidarState* lidar_state);

void PrintLidarState(struct LidarState* lidar_state);

#endif /* SRC_MACHINE_INFO_H_ */

