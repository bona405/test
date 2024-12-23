/*
 * Copyright (c) 2007 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "main.h"
#include "xtime_l.h"



volatile XTime motor_init_start_time, motor_init_end_time;
volatile uint8_t motor_stop_flag;
volatile uint8_t motor_init_state;
volatile uint8_t isMotorInit;
volatile uint8_t prevstat;

int now_nb = 40;

void MotorInit()
{
	if(isMotorInit == 1)
	{
		motor_stop_flag = Xil_In8(0x43C50058);

		if(motor_stop_flag == 1 && IS_MOTOR_INIT == 0x13)
		{
			if(prevstat == 1)
			{
				motor_init_state++;
				usleep(1);
			}
			else
			{
				motor_init_state = 0;
			}

			if(motor_init_state > 10)
			{
				motor_init_state = 0;
				XTime_GetTime(&motor_init_start_time);
				MotorCtrlRPMSet(150);

				while(1)
				{
					XTime_GetTime(&motor_init_end_time);
					if((motor_init_end_time - motor_init_start_time) / (COUNTS_PER_SECOND / 1000000) > 3.0)
					{
						MotorCtrlRPMSet(1200);
						break;
					}
					else
					{
						usleep(1);
					}
				}
			}


//			if(motor_init_state == 0)
//			{
//				XTime_GetTime(&motor_init_start_time);
//				MotorCtrlRPMSet(150);
//				motor_init_state = 1;
//			}
//			else if(motor_init_state == 1)
//			{
//				XTime_GetTime(&motor_init_end_time);
//				double period_time = (motor_init_end_time - motor_init_start_time) / (COUNTS_PER_SECOND / 1000000);
//				if(period_time > 3.0)
//				{
//					MotorCtrlRPMSet(1200);
//					motor_init_state = 2;
//				}
//			}
//			else if(motor_init_state == 2)
//			{
//				if(motor_stop_flag == 1)
//				{
//					motor_init_state = 0;
//				}
//			}


//			if(motor_init_state == 0)
//			{
//				XTime_GetTime(&motor_init_start_time);
//				MotorCtrlRPMSet(150);
//				motor_init_state = 1;
//			}
//			else if(motor_init_state == 1)
//			{
//				XTime_GetTime(&motor_init_end_time);
//				double period_time = (motor_init_end_time - motor_init_start_time) / (COUNTS_PER_SECOND / 1000000);
//				if(period_time > 3.0)
//				{
//					MotorCtrlRPMSet(1200);
//					motor_init_state = 2;
//				}
//			}
//			else if(motor_init_state == 2)
//			{
//				if(motor_stop_flag == 0)
//				{
//					motor_init_state = 0;
//				}
//			}
		}
		prevstat = motor_stop_flag;
	}
}

int main() {
#if defined(G32_ES_TEST)
	XTime tEnd, tStart, XTime;
	u32 motorRPM;
	u32 VoltageData, Voltagefraction;
#endif
	//OCM 영역 cache disable
	Xil_DCacheEnable();
	Xil_SetTlbAttributes(0xFFFF0000, 0x14de2); // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
	Xil_SetTlbAttributes((UINTPTR) SHARED_DDR_MEMORY_BASE, 0x14de2); // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0

	// run core1
	Xil_Out32(CPU1STARTADR, 0x0B000000); //  0x0B000000 : core2 ddr 시작 주소(ps7_ddr_0)

	//flush instruction buffer
	dmb();
	sev();



	struct netif *netif = &server_netif;
	// Init netwrok parameters and active lwip
	XStatus error = InitNetworkSettings(netif, &lidar_state_, &lidar_state_Ch, &lidar_state_Ch2);
	if (error != XST_SUCCESS) {
		log_error("InitNetworkSettings");
		xil_printf("Error InitNetworkSettings, Error number = %d\r\n", error);
	}

	// Init web command hash 	table
	init_webserver();

	Xil_DCacheEnable();

	//Set Local Variables
	if (strcmp(lidar_state_.network_setting_.protocol, "tcp") == 0)
		protocol = tcp;
	else if (strcmp(lidar_state_.network_setting_.protocol, "udp") == 0)
		protocol = udp;
	else
		protocol = udp;


	init_elapsed_time_tb();

	InitRoutingDelay(routing_delay, &lidar_state_Ch);
	InitRoutingDelay(routing_delay2, &lidar_state_Ch2);
	InitVthFilter(&lidar_state_);
	ResetVthFilterParameter();


	//set near distance reflect
	TDCSetDisableStart1(
			lidar_state_.calibration_setting_.minimum_detection_range_start1);
	TDCSetDisableStop1(
			lidar_state_.calibration_setting_.minimum_detection_range_stop1);
	TDCSetDisableStart2(
			lidar_state_.calibration_setting_.minimum_detection_range_start2);
	TDCSetDisableStop2(
			lidar_state_.calibration_setting_.minimum_detection_range_stop2);

#if defined(G32_ES_TEST)
	motorRPM = 0;
	VoltageData = 0;
	Voltagefraction = 0;
	XTime_GetTime(&tStart);
	XAdcPsInit();
#endif


	//PHY Reset
    XGpioPs Gpio;
    XGpioPs_Config *Config;

    Config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    if (Config == NULL)
    {
        return XST_FAILURE;
    }

    XGpioPs_CfgInitialize(&Gpio, Config, Config->BaseAddr);


    //PHY Reset End

//    usleep(100);		//4ms reset width
//
//    XGpioPs_SetDirectionPin(&Gpio, 37, 1);
//    XGpioPs_SetOutputEnablePin(&Gpio, 37, 1);
//    XGpioPs_SetDirectionPin(&Gpio, 38, 1);
//    XGpioPs_SetOutputEnablePin(&Gpio, 38, 1);
//
//    XGpioPs_WritePin(&Gpio, 37, 0);
////    usleep(4000);		//4ms reset width
//    XGpioPs_WritePin(&Gpio, 38, 0);
//
//    usleep(4000);

    XGpioPs_SetDirectionPin(&Gpio, 11, 1);
    XGpioPs_SetOutputEnablePin(&Gpio, 11, 1);

    XGpioPs_WritePin(&Gpio, 11, 0);
    usleep(4000);		//4ms reset width
    XGpioPs_WritePin(&Gpio, 11, 1);




	//Init Parameters for RemoveGhost
	InitRGParameter(&rg_parameter_);
	InitGDParameter(&gd_parameter_);
	InitIFParameter(&if_parameter_);

	InitRemoveNoise();


	//set LD
	LDCtrlSetDefaultValue();

	for (size_t ld_index = 0; ld_index < 4; ld_index++)
	{
		SetLdChon(ld_index, lidar_state_.laser_setting_.em_pulse / (u32) 100,
				lidar_state_.laser_setting_.em_pulse % 100);
	}

	LDAllSet();
	//set LD End

	u8 is_enc_ctrl_enabled = EncCtrlGetEnable();

	InitChannelVth(&lidar_state_Ch);
	bottom_horizontal_offset = atoi(
			lidar_state_.calibration_setting_.lower_offset);
	total_azimuth_offset =
			lidar_state_.calibration_setting_.total_azimuth_offset;
	InitHwIpRegisterInfo();
	is_trig_function_map_initialized = 0;

	fov_test_.start_horizontal_angle = -180.0;
	fov_test_.end_horizontal_angle = 180.0;
	fov_test_.start_vertical_angle = 0;
	fov_test_.end_vertical_angle = 100;

	InitMinimumDetectionRange(&lidar_state_);
	InitLdEnabledRange(&lidar_state_);
	InitLdDefaultValue(&lidar_state_);
	if (lidar_state_.motor_setting_.hz_setting == 2)
	{
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 110); // CH0-CH3	//180
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 110); // CH12-CH15

		//org
		//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(279));	//1	//180
		//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(0));	//2
		//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(398));	//3	//180
		//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(160));	//4

		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET,
				(u32) (300));	//1	//180
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32) (0));	//2
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET,
				(u32) (435));	//3	//180
		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET,
				(u32) (165));	//4
	}

#if defined(_DTALIGN)
	InitDtAlignInfo(&dt_align_info_, &lidar_state_);
#endif

	u32 fov_data_block_count = EncCtrlGetStopEncCount1()
			- EncCtrlGetStartEncCount1() + EncCtrlGetStopEncCount2()
			- EncCtrlGetStartEncCount2() - 2;
//	u32 total_azimuth_count = fov_data_block_count / 2;
	fov_data_block_count *= 3;
	data_block_count = 4608;
//	volatile u32 bottom_data_block_count = 2304;

	u32 bottom_data_block_count;
	u32 top_data_block_count = 4608;

//	u32 top_data_block_count = 4598;

	if (lidar_state_.laser_setting_.stop_option < 0
			|| lidar_state_.laser_setting_.stop_option > 2)
	{
		lidar_state_.laser_setting_.stop_option = 0;
	}

	TDCSetPulseWidthScaleIndex(1);

	const int sizeofRawDataBlock = sizeof(RawDataBlock);

//	volatile XTime tStart_all, tEnd_all;
//	volatile double elapsedTime_all;

	//sysmon test
//====================================================================================//
//	volatile u32 TempRawData;
//    volatile float TempData;
//    int Status_ADC;
//    volatile XSysMon_Config *ConfigPtr;
//    volatile XSysMon XADCMonInst;
//    volatile XSysMon *XADCInstPtr = &XADCMonInst;
//    XSysMon_GetStatus(XADCInstPtr);
//	ConfigPtr = XSysMon_LookupConfig(0);
//	Status_ADC = XSysMon_CfgInitialize(XADCInstPtr,ConfigPtr,ConfigPtr->BaseAddress);
	//====================================================================================//


// Temperature


	//====================================================================================//
	u32 TempRawData;
	float TempData;
//	float prevTempData;
	int Status_ADC;
	XSysMon_Config *ConfigPtr;
	XSysMon XADCMonInst;
	XSysMon *XADCInstPtr = &XADCMonInst;
//    XSysMon_GetStatus(XADCInstPtr);
	ConfigPtr = XSysMon_LookupConfig(0);
	Status_ADC = XSysMon_CfgInitialize(XADCInstPtr,ConfigPtr,ConfigPtr->BaseAddress);
	XSysMon_SelfTest(XADCInstPtr);
	XSysMon_SetSequencerMode(XADCInstPtr, XSM_SEQ_MODE_SAFE);
	XSysMon_SetAvg(XADCInstPtr, XSM_AVG_16_SAMPLES);
	XSysMon_SetSeqInputMode(XADCInstPtr, XSM_SEQ_CH_AUX00);
	XSysMon_SetSeqAcqTime(XADCInstPtr, XSM_SEQ_CH_AUX15 |
							XSM_SEQ_CH_AUX00);

	 XSysMon_SetSeqAvgEnables(XADCInstPtr, XSM_SEQ_CH_TEMP |
							XSM_SEQ_CH_VCCAUX |
							XSM_SEQ_CH_AUX00 |
							XSM_SEQ_CH_AUX15);

	 XSysMon_SetSeqChEnables(XADCInstPtr, XSM_SEQ_CH_TEMP |
							XSM_SEQ_CH_VCCAUX |
							XSM_SEQ_CH_AUX00 |
							XSM_SEQ_CH_AUX15);

	 XSysMon_SetAdcClkDivisor(XADCInstPtr, 32);

	XSysMon_SetSequencerMode(XADCInstPtr, XSM_SEQ_MODE_CONTINPASS);

	XSysMon_GetStatus(XADCInstPtr); /* Clear the old status */
	while ((XSysMon_GetStatus(XADCInstPtr) & XSM_SR_EOS_MASK) !=
			XSM_SR_EOS_MASK);

	//	XSysMon_SetAlarmEnables(XADCInstPtr, 0x0);
		//====================================================================================//


	/*a2z*/
	uint8_t *a2z_val1 = (uint8_t *) &A2Z_VAL;
	uint16_t *a2z_val2 = (uint8_t *) &A2Z_VAL + 1;
	uint8_t *a2z_val3 = (uint8_t *) &A2Z_VAL + 3;
	init_ver2();
	float azimuth_factor = 0.0;
	int32_t top_motor_init_value = 90000 - ((EncCtrlGetStartEncCount2()-2048)) * 176;
	int32_t bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;


//	//read azioffsets
//	ReadAzioffset(azi_offset_buffer_, AZI_OFFSET_SIZE);
//	int check_azi_flash_state_cnt = 0;
//	for(size_t i = 0; i< 100; i++)
//	{
//		if(azi_offset_buffer_[i] == 0xFF)
//		{
//			check_azi_flash_state_cnt++;
//		}
//	}
//	if(check_azi_flash_state_cnt == 100)
//	{
//		memset(&azi_offset_buffer_,0x00,AZI_OFFSET_SIZE);
//	}
//
//	memcpy(&azi_cal_, &azi_offset_buffer_, sizeof(struct AzimuthCal));
//	int Status = InitLinearQspiFlash();
//	EraseAzioffset();
//	Status = LinearQspiFlashWriteForAzioffset((char*) &azi_cal_, AZI_OFFSET_SIZE);


	//read new azioffset
	ReadAzioffset(azi_offset_buffer_, AZI_OFFSET_SIZE);

	int check_azi_flash_state_cnt = 0;
	for(size_t i = 0; i< 100; i++)
	{
		if(azi_offset_buffer_[i] == 0xFF)
		{
			check_azi_flash_state_cnt++;
		}
	}
	if(check_azi_flash_state_cnt == 100)
	{
		memset(&azi_offset_buffer_,0x00,AZI_OFFSET_SIZE);
	}

	memcpy(&azi_offsets_, &azi_offset_buffer_, AZI_OFFSET_SIZE);



	//
//	/*flash init start*/
//	int Status = InitLinearQspiFlash();
//	EraseAzioffset();
//	memset(&azi_cal_,0x00,sizeof(struct AzimuthCal));
//	Status = LinearQspiFlashWriteForAzioffset((char*) &azi_cal_, AZI_OFFSET_SIZE);
//	/*flash init end*/

//	azi_cal_
//	azi_cal_ReadLidarInfo_Ch(lidar_state_buffer_Ch_, AZI_OFFSET_SIZE);

	int cal_long_status = 0;
	int cal_long_distances[32][8] = {0x0,};
//	int cal_long_distances[32] = {0x0,};

	int pll_resetcot = 0;

	int temper_cot = 0;
	float Inittemp;

	isMotorInit = 1;
	motor_stop_flag = 0;
	motor_init_state = 0;

	lidar_state_.filter_setting_.dark_area_error_correction_setting_.is_dark_area_channel_changed_ = true;

	MotorTestData motor_test_data = {0,};

	// Temperature bias 241010 values
	volatile u32 fpga_temperature_raw = 0;
	volatile u32 fpga_temperature_raw2 = 0;
	volatile double fpga_temperature  = 0; // [0.123040771484375 =  503.975/4096]
	volatile int base_temperature = 60;
	volatile int base_nb = 40;

	volatile double low_temperature_factor = 0.023996;
	volatile double low_temperature_offset = 21.24504;
	volatile double high_temperature_factor = 0.06616;
	volatile double high_temperature_offset = 19.604;
	volatile double base_nb_val = high_temperature_factor*60+high_temperature_offset;
	volatile double tmp_nb = 0.0;
	volatile u16 cmd_nb = 0;


	int prev_temper = 0;
	int now_temper = 0;




	while (1) {
		/*a2z*/
		if (*a2z_val1 == 2) {
			ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
			memcpy(&lidar_state_, &lidar_state_buffer_,
					sizeof(struct LidarState));
			*a2z_val1 = 0;
		}

		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}



//		volatile double low_temperature_factor = 0.023996;
//			volatile double low_temperature_offset = 21.24504;
//			volatile double high_temperature_factor = 0.06616;
//			volatile double high_temperature_offset = 19.604;
		// Temperature bias 241010 start

//		if(lidar_state_.a2z_setting.reserved01 == 1)
//		{
//			fpga_temperature_raw = Xil_In32(0x43c70200);
//			fpga_temperature_raw2 = (fpga_temperature_raw >> 4) & 0xFFF;
//			fpga_temperature = fpga_temperature_raw2 * 0.123040771 - 273.15; // [0.123040771484375 =  503.975/4096]
//			tmp_nb = 0.0;
//
//	//		fpga_temperature = -30.0;
//			if(fpga_temperature < 40)
//			{
//				tmp_nb = (low_temperature_factor*fpga_temperature + low_temperature_offset - base_nb_val) / 0.09;
//			}
//			else
//			{
//	//			tmp_nb = (high_temperature_factor*fpga_temperature + 22.129 - base_nb) / 0.09;
//	//			tmp_nb = high_temperature_factor*(fpga_temperature - 60) / 0.09;
//				tmp_nb = (fpga_temperature - 60)*0.7351;
//			}
//			double tmp_cmd_nb = 40.0 - tmp_nb;
//			cmd_nb = (uint16_t)tmp_cmd_nb;
//
//			MCP4801CtrlSetValue(cmd_nb);
//		}

		// Temperature bias 241010 end


		//For ES test
#if defined(G32_ES_TEST)
		XTime_GetTime(&tEnd);
		XTime = 1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND / 1000);
		if(XTime >= 500)
		{
			XTime_GetTime(&tStart);
			MotorRPMCheck(&motorRPM);
			xil_printf("motorRPM = %d\r\n", motorRPM);
			XAdcPsVoltageRead(&VoltageData, &Voltagefraction, XADCPS_CH_VPVN);
			xil_printf("Current Volt is %0d.%03d Volts.\r\n", VoltageData, Voltagefraction);
		}
#endif

		//motor acc test source
//		XTime_GetTime(&tEnd);
//		XTime = 1.0 * (tEnd - tStart) / (COUNTS_PER_SECOND / 1000000);
//		if(XTime >= 1000)
//		{
//			XTime_GetTime(&tStart);
//
//			MotorRPMCheck(&motorRPM);
//			ElapsedTimeTbValue(kMAIN, motorRPM);
//			test_tb_to_bytes(elapsed_time_bytes);
//			udp_send_packet_for_test(elapsed_time_bytes);
//		}

		xemacif_input(netif);

		transfer_data();


		if (file_upload_flag == 0 && file_download_flag == 0) {
			if (SEMAPHORE_VALUE == 1) {
#ifdef ENABLE_MOTOR_TEST
				int32_t motor_test_return = GetMotorTestInfoFromPL(&motor_test_data);
				char motor_test_csv_result[1300] = {0,};
				motor_test_return = ConvertMotorTestInfoToCsvFormat(&motor_test_data, motor_test_csv_result, sizeof(motor_test_csv_result));
				udp_send_packet_for_motor_test(motor_test_csv_result);
#endif

				reset_elapsed_time_tb();
				test_start(kMAIN);
				const int EncCtrlGetStartEncCount1_1 =
						EncCtrlGetStartEncCount1() + 1;
				const int EncCtrlGetStopEncCount1_1 = EncCtrlGetStopEncCount1()
						- 1;
				const int EncCtrlGetStartEncCount2_1 =
						EncCtrlGetStartEncCount2() + 1;
				const int EncCtrlGetStopEncCount2_1 = EncCtrlGetStopEncCount2()
						- 1;
				data_block_count = EncCtrlGetStopEncCount1_1
						- EncCtrlGetStartEncCount1_1 + EncCtrlGetStopEncCount2_1
						- EncCtrlGetStartEncCount2_1 + 2;


				data_block_count *= 3;
				fov_data_block_count = data_block_count;

				bottom_data_block_count = (EncCtrlGetStopEncCount1_1
						- EncCtrlGetStartEncCount1_1 + 1) * 3;
				top_data_block_count = data_block_count;
				if (DATA_BLOCK_COUNT != data_block_count) {
//					printf("wrong azimuth count!");
//					continue;
				}
				if (pass_frame != 0) {
					if (pass_frame < 0) {
						pass_frame = 2;
					}
					pass_frame--;
					SEMAPHORE_VALUE = 0;
					continue;
				}

#if defined(_DTALIGN)
				memcpy((void*)block_data_list, (void*)block_data_shared, sizeof(RawDataBlock) * 120);
//				GetDtAlignResult(block_data_list);
//				char* dt_align_result;
//				ConvertDtAlignResultToBytes(dt_align_result);
//				UdpSendDtAlignResult(dt_align_result);

				RawDataBlock* dt_align_result = (RawDataBlock *)block_data_list;

				uint8_t vth_check_finished = 0;
				int ind = 0;

				// vth 변경하면서 최대 vth, distance 저장

				//DT_Align 처음 시작시
				if(dt_align_info_.is_align_started == 0)
				{
					dt_align_ch = dt_align_info_.current_channel;
					dt_align_value = dt_align_info_.start_vth;
					XTime_GetTime(&dt_align_start);

					dt_align_info_.is_align_started = 1;
				}
				else
				{
					vth_check_finished = DtCheckVth(&dt_align_info_, &dt_cur_vth_off);

					if(vth_check_finished) // current_vth_arr 또는 current_vth_offset이 최대값을 넘을 때 -> vth 초기 셋팅
					{
						for(int vth_ind = 0; vth_ind < 16; vth_ind ++)
						{
							current_vth_arr[vth_ind] = dt_align_info_.start_vth;
						}

						//Send Vth to UDP
						ConvertDtAlignResultToBytes(dt_align_udp_payload);
						udp_send_packet_for_test(dt_align_udp_payload);

						dt_align_ch = dt_align_info_.current_channel;
						dt_align_value = dt_align_info_.start_vth;

						for(int ind = 0; ind < 16; ind++)
						{
							int cur_index = hash_func((const char*)dt_align_arr[ind]);
							struct Node* cur_node = dt_align_hash_tb[cur_index];
							if(cur_node != NULL)
							{
								dt_align_hash_tb[cur_index]->value = -1;
							}
						}

						for(ind = 0; ind < 16; ind++)
						{
							dt_align_info_.is_vth_check_finished[ind] = 0;
							dt_align_info_.vth_zero_count[ind] = 0;
						}

						//Set change vth
						SetChangeVth(dt_align_ch, dt_align_value, &dt_cur_vth_off, &dt_curt_single_vth_off);

						SEMAPHORE_VALUE = 0;
					}
					else
					{
						XTime_GetTime(&dt_align_end);
						dt_elapsed_us = (dt_align_end - dt_align_start) / (COUNTS_PER_SECOND / 1000000);

						if(dt_elapsed_us >= (u64)dt_align_info_.check_interval * 2 * 1000)
						{
							dt_align_ch = dt_align_info_.current_channel;

							dt_align_value = DtAlignValue(&dt_align_info_, &dt_cur_vth_off, &dt_curt_single_vth_off);

							XTime_GetTime(&dt_align_start);

							//Set change vth
							SetChangeVth(dt_align_ch, dt_align_value, &dt_cur_vth_off, &dt_curt_single_vth_off);

							SEMAPHORE_VALUE = 0;
							usleep(10);

							//distance 확인용
							ConvertDtAlignTofToBytes(dt_align_tof_udp_payload, dt_align_result, &dt_align_info_);
							udp_send_packet_for_test(dt_align_tof_udp_payload);

							//Set current_vth_arr
							SetDtCurVthArr(dt_align_ch, &dt_cur_vth_off, &dt_curt_single_vth_off);

							//is vth_applied
							is_vth_applied = DtVthApplied();

							for(ind = 0; ind < 16; ind++)
							{
								if(dt_align_info_.is_vth_check_finished[ind] == 1)
								{
									continue;
								}
								dt_align_info_.vth_zero_count[15-ind] = 0;
							}

							if(is_vth_applied == 1)
							{
								DtCheckDistance(dt_align_result, &dt_align_info_);
							}
						}
					}
				}

#else
//				test_start(kTCPBuffering);

//				data_block_count = DATA_BLOCK_COUNT;

//				if(data_block_count == fov_data_block_count)

//					u32 bottom_data_block_count = BOTTOM_DATA_BLOCK_COUNT;
//					u32 top_data_block_count = TOP_DATA_BLOCK_COUNT;
//					u32 bottom_data_block_count = 2304;
//					u32 top_data_block_count = 4608;

//				XTime_GetTime(&tStart_all);	//measure time

//				int stop_option = 2;	//0 : 123stop, 1 : 12stop, 2 : 1stop
				// 240604 hohyeok
//				if (protocol == udp) {
//					stop_option = lidar_state_.laser_setting_.stop_option;
//					if (stop_option == 1)	//12stop
//							{
//						int minuscot = 0;
//						for (int idx = 0; idx < data_block_count; ++idx) {
//							block_data_list[idx - minuscot].azimuth_ =
//									block_data_list[idx].azimuth_;
//							for (int ch = 0; ch < 16; ++ch) {
//								block_data_list[idx - minuscot].distance_[ch].distance =
//										block_data_list[idx].distance_[ch].distance;
//								block_data_list[idx - minuscot].distance_[ch].pulse_width =
//										block_data_list[idx].distance_[ch].pulse_width;
//							}
//							if (idx % 3 == 2) {
//								++minuscot;
//							}
//						}
//
//						u32 packet_id = 0;
//						u32 send_start_azimuth_index = 0;
//						u32 send_finish_azimuth_index = bottom_data_block_count
//								/ 3 * 2;
//						u8 top_bottom_side = 0;
//						u32 send_packet_count = ceil(
//								(send_finish_azimuth_index
//										- send_start_azimuth_index)
//										/ (float) (DATA_BLOCK_SIZE));
//						do {
//							udp_send_packet_a3(packet_id, send_packet_count,
//									&block_data_list, FRAME_RATE,
//									VERTICAL_ANGLE, intensity_arr,
//									send_start_azimuth_index,
//									send_finish_azimuth_index, top_bottom_side);
//							send_start_azimuth_index += DATA_BLOCK_SIZE;
//							++packet_id;
//						} while (send_start_azimuth_index
//								< send_finish_azimuth_index);
//
//						send_start_azimuth_index = bottom_data_block_count / 3
//								* 2;
//						send_finish_azimuth_index = top_data_block_count / 3
//								* 2;
//						top_bottom_side = 1;
//						send_packet_count += ceil(
//								(send_finish_azimuth_index
//										- send_start_azimuth_index)
//										/ (float) (DATA_BLOCK_SIZE));
//						do {
//							udp_send_packet_a3(packet_id, send_packet_count,
//									&block_data_list, FRAME_RATE,
//									VERTICAL_ANGLE, intensity_arr,
//									send_start_azimuth_index,
//									send_finish_azimuth_index, top_bottom_side);
//
//							send_start_azimuth_index += DATA_BLOCK_SIZE;
//							++packet_id;
//						} while (send_start_azimuth_index
//								< send_finish_azimuth_index);
//					} else if (stop_option == 2) //only 1stop
//							{
//						for (int idx = 0; idx < data_block_count / 3; ++idx) {
//							block_data_list[idx].azimuth_ = block_data_list[idx
//									* 3].azimuth_;
//							for (int ch = 0; ch < 16; ++ch) {
//								block_data_list[idx].distance_[ch].distance =
//										block_data_list[idx * 3].distance_[ch].distance;
//								block_data_list[idx].distance_[ch].pulse_width =
//										block_data_list[idx * 3].distance_[ch].pulse_width;
//							}
//						}
//
//						u32 packet_id = 0;
//						u32 send_start_azimuth_index = 0;
//						u32 send_finish_azimuth_index = bottom_data_block_count
//								/ 3;
//						u8 top_bottom_side = 0;
//						u32 send_packet_count = ceil(
//								(send_finish_azimuth_index
//										- send_start_azimuth_index)
//										/ (float) (DATA_BLOCK_SIZE));
//						do {
//							udp_send_packet_a3(packet_id, send_packet_count,
//									&block_data_list, FRAME_RATE,
//									VERTICAL_ANGLE, intensity_arr,
//									send_start_azimuth_index,
//									send_finish_azimuth_index, top_bottom_side);
//							send_start_azimuth_index += DATA_BLOCK_SIZE;
//							++packet_id;
//						} while (send_start_azimuth_index
//								< send_finish_azimuth_index);
//
//						send_start_azimuth_index = bottom_data_block_count / 3;
//						send_finish_azimuth_index = top_data_block_count / 3;
//						top_bottom_side = 1;
//						send_packet_count += ceil(
//								(send_finish_azimuth_index
//										- send_start_azimuth_index)
//										/ (float) (DATA_BLOCK_SIZE));
//						do {
//							udp_send_packet_a3(packet_id, send_packet_count,
//									&block_data_list, FRAME_RATE,
//									VERTICAL_ANGLE, intensity_arr,
//									send_start_azimuth_index,
//									send_finish_azimuth_index, top_bottom_side);
//
//							send_start_azimuth_index += DATA_BLOCK_SIZE;
//							++packet_id;
//						} while (send_start_azimuth_index
//								< send_finish_azimuth_index);
//					}
////					if(stop_option==0)
//					else	//123stop
//					{
////						udp_send_packet_for_test("Test");
////						test_start(kTCPSend);
//						u32 packet_id = 0;
//						u32 send_start_azimuth_index = 0;
//						u32 send_finish_azimuth_index = bottom_data_block_count;
//						u8 top_bottom_side = 0;
//						u32 send_packet_count = ceil(
//								(send_finish_azimuth_index
//										- send_start_azimuth_index)
//										/ (float) DATA_BLOCK_SIZE);
//#if defined(G32_ES_TEST)
//						MotorRPMCheck(&motorRPM);
//#endif
//						do {
//#if defined(G32_ES_TEST)
//							udp_send_packet_a3(packet_id, send_packet_count, &block_data_list, motorRPM, VoltageData, Voltagefraction, intensity_arr,
//									send_start_azimuth_index, send_finish_azimuth_index, top_bottom_side);
//#else
//							udp_send_packet_a4(packet_id, send_packet_count,
//									&block_data_list, FRAME_RATE,
//									VERTICAL_ANGLE, intensity_arr,
//									send_start_azimuth_index,
//									send_finish_azimuth_index, top_bottom_side,
//									gd_result_.gounrd_z_mean_);
////						udp_send_packet_a3(packet_id, send_packet_count, &block_data_list, FRAME_RATE, VERTICAL_ANGLE, intensity_arr,
////															send_start_azimuth_index, send_finish_azimuth_index, top_bottom_side);
//#endif
//							send_start_azimuth_index += DATA_BLOCK_SIZE;
////							packet_id++;
//							++packet_id;
//						} while (send_start_azimuth_index
//								< send_finish_azimuth_index);
//
//						send_start_azimuth_index = bottom_data_block_count;
//						send_finish_azimuth_index = top_data_block_count;
//						top_bottom_side = 1;
//						send_packet_count += ceil(
//								(send_finish_azimuth_index
//										- send_start_azimuth_index)
//										/ (float) DATA_BLOCK_SIZE);
//						do {
//#if defined(G32_ES_TEST)
//							udp_send_packet_a3(packet_id, send_packet_count, &block_data_list, motorRPM, VoltageData, Voltagefraction, intensity_arr,
//									send_start_azimuth_index, send_finish_azimuth_index, top_bottom_side);
//#else
//							//code for intensity test
////							for(size_t i = 0 ; i < bottom_data_block_count ; i++)
////							{
////								for(size_t j = 0 ; j < 16 ; j ++)
////									block_data_list[i + bottom_data_block_count].distance_[j].distance = block_data_list[i].distance_[j].pulse_width;
////							}
//
//							udp_send_packet_a4(packet_id, send_packet_count,
//									&block_data_list, FRAME_RATE,
//									VERTICAL_ANGLE, intensity_arr,
//									send_start_azimuth_index,
//									send_finish_azimuth_index, top_bottom_side,
//									gd_result_.gounrd_z_mean_);
////						udp_send_packet_a3(packet_id, send_packet_count, &block_data_list, FRAME_RATE, VERTICAL_ANGLE, intensity_arr,
////								send_start_azimuth_index, send_finish_azimuth_index, top_bottom_side);
//#endif
//							send_start_azimuth_index += DATA_BLOCK_SIZE;
////							packet_id++;
//							++packet_id;
//						} while (send_start_azimuth_index
//								< send_finish_azimuth_index);
////						test_end(kTCPSend);
////						test_end(kMAIN);
//
//					}
//				} else {
//					//add tcp source
//				}

//					test_start(kParseData);
				if (is_enc_ctrl_enabled == 0) {
					InitChannelVth(&lidar_state_Ch);
					is_enc_ctrl_enabled = EncCtrlGetEnable();
					for (size_t ld_index = 0; ld_index < 4; ld_index++) {
						SetLdChon(ld_index,
								lidar_state_.laser_setting_.em_pulse
										/ (u32) 100,
								lidar_state_.laser_setting_.em_pulse % 100);
					}

					LDAllSet();

					InitMinimumDetectionRange(&lidar_state_);
					InitLdEnabledRange(&lidar_state_);
					InitLdDefaultValue(&lidar_state_);

					if (lidar_state_.motor_setting_.hz_setting == 2) {
						IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR,
								TDC_CTRL_REG5_OFFSET, 110); // CH0-CH3	//180
						IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR,
								TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
						IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR,
								TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
						IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR,
								TDC_CTRL_REG8_OFFSET, 110); // CH12-CH15

						IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR,
								LD_CTRL_REG5_OFFSET, (u32) (300));	//1	//180
						IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR,
								LD_CTRL_REG6_OFFSET, (u32) (0));	//2
						IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR,
								LD_CTRL_REG7_OFFSET, (u32) (435));	//3	//180
						IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR,
								LD_CTRL_REG8_OFFSET, (u32) (165));	//4
					}
				}


				memcpy((void*) block_data_list, (void*) block_data_shared,
						sizeofRawDataBlock * data_block_count);

				//azical
//				azi_cal_
//				if(azi_cal_.AziOffsets_[0].ch_datas[0] == 0x1234) //magic num
//				{
//					azi_cal_
//					SetAzimuthCal(block_data_list, bottom_data_block_count, ONE_SIDE_CHANNEL_COUNT,
//							bottom_data_block_count, azi_cal_);


//				}

				//hhj 240630
				test_start(kUDPSend);
//				if(azi_cal_.azimuth_calibration_setting.calibration_info_count > 0)
//				{
//					SetAzimuthCal2(block_data_list, bottom_data_block_count, ONE_SIDE_CHANNEL_COUNT,
//																	bottom_data_block_count, azi_cal_);
//				}

			    TempRawData = XSysMon_GetAdcData(XADCInstPtr, XSM_CH_TEMP);
			    TempData = XSysMon_RawToTemperature(TempRawData);

//				if(lidar_state_.calibration_setting_.temperadj_enable)
//				{
//
//
//				    float Temper_Factor = 60 - TempData;	//60 is saturation temperature
//
//					int tmper_adj_offset = 0.0025 * Temper_Factor * 256;	//0.0025m per 1
//
//					SetTemperatureAdjOffset(block_data_list, bottom_data_block_count, ONE_SIDE_CHANNEL_COUNT,
//										bottom_data_block_count, tmper_adj_offset);
//				}



			    // 241011
			    if(lidar_state_.calibration_setting_.temperadj_enable && TempData > -100.0)
				{

//					float Temper_Factor = 53 - TempData;	//60 is saturation temperature
//
//					int tmper_adj_offset = 0.0025 * Temper_Factor * 256;	//0.0025m per 1

					int temper_idx = (int)(round(TempData));	//60 is saturation temperature
					if(temper_idx < 0)
					{
						temper_idx = 0;
					}
					else if(temper_idx > 120)
					{
						temper_idx = 120;
					}

					int tmper_adj_offset = temperadj_table[temper_idx];	//0.0025m per 1

					SetTemperatureAdjOffset(block_data_list, bottom_data_block_count, ONE_SIDE_CHANNEL_COUNT,
										bottom_data_block_count, tmper_adj_offset);

					//set nb bias

//					now_temper = (int)round(TempData);
//
//					if(now_temper != prev_temper)
//					{
//						prev_temper = now_temper;
//						int temper_table_index = now_temper + 40; //table start from -40
//
//						if(temper_table_index < 0)
//						{
//							temper_table_index = 0;
//						}
//						else if(temper_table_index > 164)
//						{
//							temper_table_index = 164;
//						}
//
//						now_nb = temper_NB_table[temper_table_index];
//
//						if(now_nb < 0)
//						{
//							now_nb = 0;
//						}
//
//						MCP4801CtrlSetValue(now_nb);
//
//					}
//
//					temperature_NB_to_tb(now_nb);
				}


			    if(lidar_state_.calibration_setting_.tempergain_enable && TempData > -100.0)
			    {
					//set nb bias

					now_temper = (int)round(TempData);

					if(now_temper != prev_temper)
					{
						prev_temper = now_temper;
						int temper_table_index = now_temper + 40; //table start from -40

						if(temper_table_index < 0)
						{
							temper_table_index = 0;
						}
						else if(temper_table_index > 164)
						{
							temper_table_index = 164;
						}

						now_nb = temper_NB_table[temper_table_index];

						if(now_nb < 0)
						{
							now_nb = 0;
						}

						MCP4801CtrlSetValue(now_nb);

					}

					temperature_NB_to_tb(now_nb);
			    }
			    else
			    {
			    	temperature_NB_to_tb(now_nb);
			    }

//			    prevTempData = TempData;

				temperature_to_tb(TempData);

				test_end(kUDPSend);


				size_t data_arr_size = data_block_count;
				size_t oneChAllEchoPointSize = data_arr_size / 2;
				size_t oneChOneEchoPointSize = oneChAllEchoPointSize / 3;

				uint8_t diff_ch_idx_limit = 3;
				uint16_t distance_limit =
						(uint16_t) lidar_state_.a2z_setting.near_dist * 256;
//				uint16_t distance_limit = 1536;

				uint16_t merge_point_diff_distance_limit =
						(uint16_t) lidar_state_.a2z_setting.merge_dist * 256;
//				uint16_t merge_point_diff_distance_limit = 128;

//				SEMAPHORE_VALUE = 0;


				memcpy((void*) temp_block_data_shared, (void*) &block_data_list,
						fov_data_block_count * sizeofRawDataBlock);

				SetBottomHorizontalOffset(block_data_list,
						temp_block_data_shared, bottom_data_block_count,
						ONE_SIDE_CHANNEL_COUNT, bottom_horizontal_offset * 3);

				if (total_azimuth_offset > -300 && total_azimuth_offset < 300) {
//					for(int azimuth_index = 0 ; azimuth_index < total_azimuth_count * 6 ; azimuth_index++)
//						block_data_list[azimuth_index].azimuth_ += total_azimuth_offset;
					for (int azimuth_index = 0;
							azimuth_index < data_block_count; azimuth_index++)
						block_data_list[azimuth_index].azimuth_ +=
								total_azimuth_offset;
				}


				if (lidar_state_.filter_setting_.is_new_noise_filter_on == 1) {

					noise_point_cnt = 0;

//					SetAzimuthAndDistanceB1_Sparse(current_frame_blocks, block_data_list, data_block_count / 2, ONE_SIDE_CHANNEL_COUNT,
//							bottom_data_block_count, &lidar_state_, routing_delay, clip_min, EncCtrlGetStartEncCount1_1, EncCtrlGetStartEncCount2_1, bottom_horizontal_offset, inten_filter, &noise_point_cnt, last_block_data_list, HZ_VAL);

					if(lidar_state_.calibration_setting_.azioffset_enable)
					{
						SetAzimuthAndDistanceB1_Sparse_AziOffset(current_frame_blocks,
								block_data_list, bottom_data_block_count,
								ONE_SIDE_CHANNEL_COUNT, bottom_data_block_count,
								&lidar_state_, routing_delay, clip_min,
								EncCtrlGetStartEncCount1_1,
								EncCtrlGetStartEncCount2_1,
								bottom_horizontal_offset, inten_filter,
								&noise_point_cnt, last_block_data_list, HZ_VAL, routing_delay2, (int)(routing_range_1 * 256), (int)(routing_range_2 * 256),
								azi_offsets_);
					}
					else
					{
						SetAzimuthAndDistanceB1_Sparse(current_frame_blocks,
								block_data_list, bottom_data_block_count,
								ONE_SIDE_CHANNEL_COUNT, bottom_data_block_count,
								&lidar_state_, routing_delay, clip_min,
								EncCtrlGetStartEncCount1_1,
								EncCtrlGetStartEncCount2_1,
								bottom_horizontal_offset, inten_filter,
								&noise_point_cnt, last_block_data_list, HZ_VAL, routing_delay2, (int)(routing_range_1 * 256), (int)(routing_range_2 * 256));
					}

					memcpy((void*) last_block_data_list,
							(void*) &block_data_list,
							fov_data_block_count * sizeofRawDataBlock);

					if (lidar_state_.filter_setting_.is_voltage_on) {
						noise_reference_value_1 = (noise_reference_value_0
								+ 0.05);
						noise_reference_value_2 =
								(noise_reference_value_1 + 0.1);
						noise_reference_value_3 = (noise_reference_value_2
								+ 0.05);

						ApplyVoltageThresholdFilter(vth_sensitivity,
								noise_percent, noise_min_value, noise_max_value,
								&current_vth_offset, noise_point_cnt,
								noise_reference_number,
								noise_percent_logic_flag, &lidar_state_);
						//a2z
						*a2z_val2 = current_vth_offset;
					}
				} else {

//					SetAzimuthAndDistanceB1(current_frame_blocks, block_data_list, data_block_count / 2, ONE_SIDE_CHANNEL_COUNT,
//							bottom_data_block_count, &lidar_state_, routing_delay, clip_min, EncCtrlGetStartEncCount1_1, EncCtrlGetStartEncCount2_1, bottom_horizontal_offset, HZ_VAL);

					if(lidar_state_.calibration_setting_.azioffset_enable)
					{
						SetAzimuthAndDistanceB1_AziOffset(current_frame_blocks,
								block_data_list, bottom_data_block_count,
								ONE_SIDE_CHANNEL_COUNT, bottom_data_block_count,
								&lidar_state_, routing_delay, clip_min,
								EncCtrlGetStartEncCount1_1,
								EncCtrlGetStartEncCount2_1,
								bottom_horizontal_offset, HZ_VAL, azi_offsets_);
					}
					else
					{
						SetAzimuthAndDistanceB1(current_frame_blocks,
								block_data_list, bottom_data_block_count,
								ONE_SIDE_CHANNEL_COUNT, bottom_data_block_count,
								&lidar_state_, routing_delay, clip_min,
								EncCtrlGetStartEncCount1_1,
								EncCtrlGetStartEncCount2_1,
								bottom_horizontal_offset, HZ_VAL);
					}

				}

				if (lidar_state_.filter_setting_.is_noise_on == 1) {
					init_params(data_block_count,
							lidar_state_.a2z_setting.cc_eps * 0.01,
							lidar_state_.a2z_setting.ac_eps * 0.01,
							diff_ch_idx_limit, distance_limit,
							merge_point_diff_distance_limit,
							lidar_state_.a2z_setting.z_offset * 0.01,
							(uint8_t) HZ_VAL,
							lidar_state_.a2z_setting.clustering_onoff);

				}
//				top_motor_init_value = 90000 - ((EncCtrlGetStartEncCount2()-2048)) * 176;
//				bottom_motor_init_value = 90000 - EncCtrlGetStartEncCount1() * 176;

//				top_motor_init_value = 90000 - ((EncCtrlGetStartEncCount2()-2048)) * 176;
//				top_motor_init_value = 112230 - ((EncCtrlGetStartEncCount2()-2048)) * 219;
//				bottom_motor_init_value = 112230 - EncCtrlGetStartEncCount1() * 219;
//
//				if(lidar_state_.a2z_setting.reserved02 == 1  && lidar_state_.a2z_setting.fov_correction != 0)
//				{
//					for (int isTop = 0; isTop < 2; isTop++)
//					{
//						for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
//						{
//							for (size_t echo_idx = 0; echo_idx < 3; echo_idx++)
//							{
//								int data_block_idx = ((oneChAllEchoPointSize)*isTop) + azimuth_idx + echo_idx;
//								azimuth_factor = (isTop == 0) ? ((float)lidar_state_.a2z_setting.fov_correction	/ (bottom_motor_init_value * 2)):((float)lidar_state_.a2z_setting.fov_correction	/ (top_motor_init_value * 2));
//								block_data_list[data_block_idx].azimuth_ = block_data_list[data_block_idx].azimuth_* (azimuth_factor);
//								current_frame_blocks[data_block_idx].azimuth_ =block_data_list[data_block_idx].azimuth_*0.001;
//							}
//						}
//					}
//				}



//					test_end(kParseData);
				if (lidar_state_.filter_setting_.is_remove_ghost_on == 1) {
//						memset(&intensity_arr, 0x00, fov_data_block_count * CHANNEL_SIZE);
					uint8_t is_bottom_first = 1;
					if (is_trig_function_map_initialized == 0) {
						InitTrigFunctionMap(current_frame_blocks,
								is_bottom_first, fov_data_block_count);
						is_trig_function_map_initialized = 1;
					}
//						test_start(kParseData);
					DetectGround(current_frame_blocks, is_bottom_first,
							fov_data_block_count);
					ConvertDataToXYZPoints(current_frame_blocks,
							is_bottom_first, intensity_arr);
					RangeProjection();
					RECM();
					JCP();
					GetGroundDetectionResult(&gd_result_);


					RemoveGhost((RawDataBlock*) &block_data_list,
							current_frame_blocks, fov_data_block_count,
							&gd_result_, fov_data_block_count);
//						test_end(kParseData);


					// ground_point_indices : 검출된 지면 포인트 집합
					// ground_point_indices_num : ground_point_indices의 개수
					// Groud Detect Test Code
//					for(size_t ind = 0 ; ind < ground_point_indices_num ; ++ind)
//					{
//						size_t azimuth_index = ground_point_indices[ind].first;
//						size_t vertical_index = ground_point_indices[ind].second;
//
//						block_data_list[azimuth_index].distance_[vertical_index].pulse_width = 255;
//					}


				}

				if (lidar_state_.filter_setting_.is_noise_filter_on == 1) {
					frame_point_cnt = 0;
					noise_point_cnt = 0;
					noise_percent = 0;
					if (is_score_map_init == 0) {
//						test_start(kParseData);
						InitScoreMap();
						SetNeighborPointIndex(fov_data_block_count);
//						SetNeighborPointIndexForCloseRange();
//						test_end(kParseData);
					}

					if (last_frame_blocks != NULL) {
						if (check_azimuth_first == 0)
							CalculateNoiseScore2(current_frame_blocks,
									last_frame_blocks,
									bottom_horizontal_offset * 3,
									fov_data_block_count);
						else
							CalculateNoiseScore(current_frame_blocks,
									last_frame_blocks,
									bottom_horizontal_offset * 3,
									is_bottom_horizontal_offset_changed,
									frame_count, fov_data_block_count);
						is_bottom_horizontal_offset_changed = 0;

					}

					memcpy(last_frame_blocks, current_frame_blocks,
							sizeof(ConvertedDataBlock) * fov_data_block_count);
//						test_start(kUDPSend);
//						if(check_azimuth_first == 0)
					RemoveNoisePoints2(current_frame_blocks,
							bottom_horizontal_offset * 3, block_data_list,
							&noise_point_cnt, fov_data_block_count);
//						else
//							RemoveNoisePoints(current_frame_blocks, bottom_horizontal_offset * 3, block_data_list,
//									&noise_point_cnt, fov_data_block_count);
//						test_end(kUDPSend);

					if (lidar_state_.filter_setting_.is_voltage_on) {
						noise_reference_value_1 = (noise_reference_value_0
								+ 0.05);
						noise_reference_value_2 =
								(noise_reference_value_1 + 0.1);
						noise_reference_value_3 = (noise_reference_value_2
								+ 0.05);

						ApplyVoltageThresholdFilter(vth_sensitivity,
								noise_percent, noise_min_value, noise_max_value,
								&current_vth_offset, noise_point_cnt,
								noise_reference_number,
								noise_percent_logic_flag, &lidar_state_);
						//a2z
						*a2z_val2 = current_vth_offset;
					}
				}

				// nan check
//				if(isnanf(gd_result_.gounrd_z_mean_))
//				{
//					gd_result_.gounrd_z_mean_ = -0.6;
//				}



				if(lidar_state_.filter_setting_.is_noise_on == 1 && HZ_VAL == 0)
				{

					test_start(kNoiseFilter);
					///
					preprocessing2(block_data_list, lidar_state_.a2z_setting.ac_minpts, lidar_state_.a2z_setting.cc_minpts, gd_result_.gounrd_z_mean_);
					test_end(kNoiseFilter);

					test_start(kParseData);
					exhaust_filter_azimuth_clustering_ver2(block_data_list, lidar_state_.a2z_setting.ac_minpts, gd_result_.gounrd_z_mean_);
					test_end(kParseData);

					test_start(kTCPBuffering);
					///
					exhaust_filter_channel_clustering_ver2(block_data_list, lidar_state_.a2z_setting.cc_minpts, gd_result_.gounrd_z_mean_);
					test_end(kTCPBuffering);

					test_start(kTCPSend);
					exhaust_filter_remove_noise_ver2(block_data_list, lidar_state_.a2z_setting.clustering_onoff);
					test_end(kTCPSend);

				}
////				// test...sometime...
//				if(lidar_state_.filter_setting_.is_noise_on == 1 && HZ_VAL == 0)
//				{
//					run_0617(block_data_list,lidar_state_.a2z_setting.ac_minpts,lidar_state_.a2z_setting.cc_minpts,gd_result_.gounrd_z_mean_);
//					exhaust_filter_remove_noise_ver2(block_data_list, lidar_state_.a2z_setting.clustering_onoff);
//				}


				if(lidar_state_.filter_setting_.is_intensity_correction_on)
				{

					intensity_correction_init_params(data_block_count);
					intensity_correction_filter(block_data_list);

				}

//				RunErrorCorrectionByDistance(block_data_list, fov_data_block_count, bottom_data_block_count, &lidar_state_.filter_setting_.dark_area_error_correction_setting_);
				test_start(kUDPSend);
				dark_area_state_.darkarea_setting_.total_azimuth_cnt = fov_data_block_count/6;
				dark_area_state_.darkarea_setting_2.total_azimuth_cnt = fov_data_block_count/6;
				dark_area_state_.darkarea_setting_3.total_azimuth_cnt = fov_data_block_count/6;
				RunErrorCorrectionByDistance2(block_data_list, fov_data_block_count, bottom_data_block_count, &dark_area_state_);
				test_end(kUDPSend);

				if(lidar_state_.filter_setting_.is_fir_on)
				{
					FIR_Filter_Ground4(block_data_list, ground_point_indices, ground_point_indices_num, bottom_data_block_count, bottom_data_block_count,
							&lidar_state_);
				}


//				XTime_GetTime(&tEnd_all);	//measure time
//				elapsedTime_all = 2*((double)(tEnd_all - tStart_all) * 1000000.0) / (double)XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ;
//				printf("time : %f\r\n", elapsedTime_all);

				// sysmon test
//				    TempRawData = XSysMon_GetAdcData(XADCInstPtr, XSM_CH_TEMP);
//				    TempData = XSysMon_RawToTemperature(TempRawData);
//					printf("tempdata : %f\r\n", TempData);

//				    XAdcPs XAdcInst;
//				    XAdcPs_Config *ConfigPtr;
//				    XAdcPs_LookupConfig(XADC_DEVICE_ID);
//				    XAdcPs_CfgInitialize(&XAdcInst, ConfigPtr, ConfigPtr->BaseAddress);
//				    XAdcPs_SetSequencerMode(&XAdcInst, XADCPS_SEQ_MODE_SINGCHAN);
////				    XAdcPs_SetSingleChParams(&XAdcInst, XADCPS_CH_TEMP, TRUE);

//				test_end(kTCPBuffering);


				test_end(kMAIN);



				test_tb_to_bytes(elapsed_time_bytes);
				udp_send_packet_for_test(elapsed_time_bytes);
				SEMAPHORE_VALUE = 0;

				//for logging
				unsigned char send_temp[210] = {0x0,};

				if(temper_cot < 50)
				{
					temper_cot++;
				}


				if( temper_cot >= 50)
				{
					temper_cot = 0;
					sprintf(send_temp, "NB Bias: %d , Temperature: %.2f deg , distance / PulseWidth: " , now_nb, TempData);

					for(int i = 0; i<16; ++i)
					{
						unsigned char num_str[10];

						sprintf(num_str, "%d/%d", block_data_list[342*3].distance_[i].distance, block_data_list[342*3].distance_[i].pulse_width);

						strcat(send_temp , num_str);
//						if(i != 15)
						{
							strcat(send_temp , ", ");
						}
					}


					udp_send_packet_for_logging(send_temp);


				}



				// frame_rate bug modify
				uint8_t FRAME_RATE = 10;
				if(HZ_VAL == 0) FRAME_RATE = 10;
				else if(HZ_VAL == 1) FRAME_RATE = 20;
				else if(HZ_VAL == 2) FRAME_RATE = 25;
				else FRAME_RATE = 10;
				// 240604 hohyeok
				if (protocol == udp) {
						stop_option = lidar_state_.laser_setting_.stop_option;
//					stop_option = 2;
						if (stop_option == 1)	//12stop
								{
							int minuscot = 0;
							for (int idx = 0; idx < data_block_count; ++idx) {
								block_data_list[idx - minuscot].azimuth_ =
										block_data_list[idx].azimuth_;
								for (int ch = 0; ch < 16; ++ch) {
									block_data_list[idx - minuscot].distance_[ch].distance =
											block_data_list[idx].distance_[ch].distance;
									block_data_list[idx - minuscot].distance_[ch].pulse_width =
											block_data_list[idx].distance_[ch].pulse_width;
								}
								if (idx % 3 == 2) {
									++minuscot;
								}
							}

							u32 packet_id = 0;
							u32 send_start_azimuth_index = 0;
							u32 send_finish_azimuth_index = bottom_data_block_count
									/ 3 * 2;
							u8 top_bottom_side = 0;
							u32 send_packet_count = ceil(
									(send_finish_azimuth_index
											- send_start_azimuth_index)
											/ (float) (DATA_BLOCK_SIZE));
							do {
//								udp_send_packet_a3(packet_id, send_packet_count,
//										&block_data_list, FRAME_RATE,
//										VERTICAL_ANGLE, intensity_arr,
//										send_start_azimuth_index,
//										send_finish_azimuth_index, top_bottom_side);
								if(lidar_state_.motor_setting_.hz_setting != 4)
								{
									udp_send_packet_a4(packet_id, send_packet_count,
																			&block_data_list, FRAME_RATE,
																			VERTICAL_ANGLE, intensity_arr,
																			send_start_azimuth_index,
																			send_finish_azimuth_index, top_bottom_side,
																			gd_result_.gounrd_z_mean_);
									++packet_id;
								}

								send_start_azimuth_index += DATA_BLOCK_SIZE;
//								++packet_id;
							} while (send_start_azimuth_index
									< send_finish_azimuth_index);

							send_start_azimuth_index = bottom_data_block_count / 3
									* 2;
							send_finish_azimuth_index = top_data_block_count / 3
									* 2;
							top_bottom_side = 1;

							if(lidar_state_.motor_setting_.hz_setting == 4)
							{
								send_packet_count = ceil(
										(send_finish_azimuth_index
												- send_start_azimuth_index)
												/ (float) DATA_BLOCK_SIZE);
							}
							else
							{
								send_packet_count += ceil(
										(send_finish_azimuth_index
												- send_start_azimuth_index)
												/ (float) DATA_BLOCK_SIZE);
							}
//							send_packet_count += ceil(
//									(send_finish_azimuth_index
//											- send_start_azimuth_index)
//											/ (float) (DATA_BLOCK_SIZE));
							do {
//								udp_send_packet_a3(packet_id, send_packet_count,
//										&block_data_list, FRAME_RATE,
//										VERTICAL_ANGLE, intensity_arr,
//										send_start_azimuth_index,
//										send_finish_azimuth_index, top_bottom_side);
								if(lidar_state_.motor_setting_.hz_setting != 3)
								{
									udp_send_packet_a4(packet_id, send_packet_count,
																			&block_data_list, FRAME_RATE,
																			VERTICAL_ANGLE, intensity_arr,
																			send_start_azimuth_index,
																			send_finish_azimuth_index, top_bottom_side,
																			gd_result_.gounrd_z_mean_);
								}


								send_start_azimuth_index += DATA_BLOCK_SIZE;
								++packet_id;
							} while (send_start_azimuth_index
									< send_finish_azimuth_index);
						} else if (stop_option == 2) //only 1stop
								{
							for (int idx = 0; idx < data_block_count / 3; ++idx) {
								block_data_list[idx].azimuth_ = block_data_list[idx
										* 3].azimuth_;
								for (int ch = 0; ch < 16; ++ch) {
									block_data_list[idx].distance_[ch].distance =
											block_data_list[idx * 3].distance_[ch].distance;
									block_data_list[idx].distance_[ch].pulse_width =
											block_data_list[idx * 3].distance_[ch].pulse_width;
								}
							}

							u32 packet_id = 0;
							u32 send_start_azimuth_index = 0;
							u32 send_finish_azimuth_index = bottom_data_block_count
									/ 3;
							u8 top_bottom_side = 0;
							u32 send_packet_count = ceil(
									(send_finish_azimuth_index
											- send_start_azimuth_index)
											/ (float) (DATA_BLOCK_SIZE));
							do {
//								udp_send_packet_a3(packet_id, send_packet_count,
//										&block_data_list, FRAME_RATE,
//										VERTICAL_ANGLE, intensity_arr,
//										send_start_azimuth_index,
//										send_finish_azimuth_index, top_bottom_side);
								if(lidar_state_.motor_setting_.hz_setting != 4)
								{
									udp_send_packet_a4(packet_id, send_packet_count,
																			&block_data_list, FRAME_RATE,
																			VERTICAL_ANGLE, intensity_arr,
																			send_start_azimuth_index,
																			send_finish_azimuth_index, top_bottom_side,
																			gd_result_.gounrd_z_mean_);
									++packet_id;
								}

								send_start_azimuth_index += DATA_BLOCK_SIZE;
//								++packet_id;
							} while (send_start_azimuth_index
									< send_finish_azimuth_index);

							send_start_azimuth_index = bottom_data_block_count / 3;
							send_finish_azimuth_index = top_data_block_count / 3;
							top_bottom_side = 1;

							if(lidar_state_.motor_setting_.hz_setting == 4)
							{
								send_packet_count = ceil(
										(send_finish_azimuth_index
												- send_start_azimuth_index)
												/ (float) DATA_BLOCK_SIZE);
							}
							else
							{
								send_packet_count += ceil(
										(send_finish_azimuth_index
												- send_start_azimuth_index)
												/ (float) DATA_BLOCK_SIZE);
							}
//							send_packet_count += ceil(
//									(send_finish_azimuth_index
//											- send_start_azimuth_index)
//											/ (float) (DATA_BLOCK_SIZE));
							do {
//								udp_send_packet_a3(packet_id, send_packet_count,
//										&block_data_list, FRAME_RATE,
//										VERTICAL_ANGLE, intensity_arr,
//										send_start_azimuth_index,
//										send_finish_azimuth_index, top_bottom_side);
								if(lidar_state_.motor_setting_.hz_setting != 3)
								{
									udp_send_packet_a4(packet_id, send_packet_count,
																			&block_data_list, FRAME_RATE,
																			VERTICAL_ANGLE, intensity_arr,
																			send_start_azimuth_index,
																			send_finish_azimuth_index, top_bottom_side,
																			gd_result_.gounrd_z_mean_);
								}

								send_start_azimuth_index += DATA_BLOCK_SIZE;
								++packet_id;
							} while (send_start_azimuth_index
									< send_finish_azimuth_index);
						}
	//					if(stop_option==0)
						else	//123stop
						{
	//						udp_send_packet_for_test("Test");
	//						test_start(kTCPSend);
							u32 packet_id = 0;
							u32 send_start_azimuth_index = 0;
							u32 send_finish_azimuth_index = bottom_data_block_count;
							u8 top_bottom_side = 0;
							u32 send_packet_count = ceil(
									(send_finish_azimuth_index
											- send_start_azimuth_index)
											/ (float) DATA_BLOCK_SIZE);
	#if defined(G32_ES_TEST)
							MotorRPMCheck(&motorRPM);
	#endif
							do {
	#if defined(G32_ES_TEST)
								udp_send_packet_a3(packet_id, send_packet_count, &block_data_list, motorRPM, VoltageData, Voltagefraction, intensity_arr,
										send_start_azimuth_index, send_finish_azimuth_index, top_bottom_side);
	#else
//								udp_send_packet_a4(packet_id, send_packet_count,
//										&block_data_list, FRAME_RATE,
//										VERTICAL_ANGLE, intensity_arr,
//										send_start_azimuth_index,
//										send_finish_azimuth_index, top_bottom_side,
//										gd_result_.gounrd_z_mean_);

								if(lidar_state_.motor_setting_.hz_setting != 4)
								{
									udp_send_packet_a4(packet_id, send_packet_count,
													&block_data_list, FRAME_RATE,
													VERTICAL_ANGLE, intensity_arr,
													send_start_azimuth_index,
													send_finish_azimuth_index, top_bottom_side,
													fpga_temperature*100);
									++packet_id;
								}

//								fpga_temperature
	//						udp_send_packet_a3(packet_id, send_packet_count, &block_data_list, FRAME_RATE, VERTICAL_ANGLE, intensity_arr,
	//															send_start_azimuth_index, send_finish_azimuth_index, top_bottom_side);
	#endif
								send_start_azimuth_index += DATA_BLOCK_SIZE;
	//							packet_id++;
//								++packet_id;
							} while (send_start_azimuth_index
									< send_finish_azimuth_index);

							send_start_azimuth_index = bottom_data_block_count;
							send_finish_azimuth_index = top_data_block_count;
							top_bottom_side = 1;
							if(lidar_state_.motor_setting_.hz_setting == 4)
							{
								send_packet_count = ceil(
										(send_finish_azimuth_index
												- send_start_azimuth_index)
												/ (float) DATA_BLOCK_SIZE);
							}
							else
							{
								send_packet_count += ceil(
										(send_finish_azimuth_index
												- send_start_azimuth_index)
												/ (float) DATA_BLOCK_SIZE);
							}

							do {
	#if defined(G32_ES_TEST)
								udp_send_packet_a3(packet_id, send_packet_count, &block_data_list, motorRPM, VoltageData, Voltagefraction, intensity_arr,
										send_start_azimuth_index, send_finish_azimuth_index, top_bottom_side);
	#else
								//code for intensity test
	//							for(size_t i = 0 ; i < bottom_data_block_count ; i++)
	//							{
	//								for(size_t j = 0 ; j < 16 ; j ++)
	//									block_data_list[i + bottom_data_block_count].distance_[j].distance = block_data_list[i].distance_[j].pulse_width;
	//							}

//								udp_send_packet_a4(packet_id, send_packet_count,
//										&block_data_list, FRAME_RATE,
//										VERTICAL_ANGLE, intensity_arr,
//										send_start_azimuth_index,
//										send_finish_azimuth_index, top_bottom_side,
//										gd_result_.gounrd_z_mean_);

								if(lidar_state_.motor_setting_.hz_setting != 3)
								{
									udp_send_packet_a4(packet_id, send_packet_count,
																					&block_data_list, FRAME_RATE,
																					VERTICAL_ANGLE, intensity_arr,
																					send_start_azimuth_index,
																					send_finish_azimuth_index, top_bottom_side,
																					fpga_temperature*100);
								}

	//						udp_send_packet_a3(packet_id, send_packet_count, &block_data_list, FRAME_RATE, VERTICAL_ANGLE, intensity_arr,
	//								send_start_azimuth_index, send_finish_azimuth_index, top_bottom_side);
	#endif
								send_start_azimuth_index += DATA_BLOCK_SIZE;
	//							packet_id++;
								++packet_id;
							} while (send_start_azimuth_index
									< send_finish_azimuth_index);
	//						test_end(kTCPSend);
	//						test_end(kMAIN);

						}
					} else {
						//add tcp source
					}

#ifdef G32_B1_PPS
				if(lidar_state_.motor_setting_.ispllon)
				{
					//phase lock
					int now_status = 0;

					now_status = Xil_In32(0x43C50028);

					AngleErr = block_data_list[0].pps_info.angle_diff;

					if(now_status == 1)
					{
						Xil_Out32(0x43C50044, 0x0f);

						Xil_Out32(0x43C50030, lidar_state_.motor_setting_.pll_point);	//write
					}
					else
					{
						//resetcount
						pll_resetcot++;
						if(pll_resetcot > 300)
						{
							Xil_Out32(0x43C5002C, 1);
							usleep(1);
							pll_resetcot = 0;
							Xil_Out32(0x43C5002C, 0);
						}
					}

//					volatile debugnums = 143;
//					printf("ispllon%d\r\n", debugnums);
				}
#endif

				if (cal2_flag) {

					cal2_flag = 0;
		//			volatile u32 fov_data_block_count = EncCtrlGetStopEncCount1() - EncCtrlGetStartEncCount1() + EncCtrlGetStopEncCount2() - EncCtrlGetStartEncCount2();
					volatile u32 fov_data_block_count = EncCtrlGetStopEncCount1()
							- EncCtrlGetStartEncCount1() + EncCtrlGetStopEncCount2()
							- EncCtrlGetStartEncCount2() - 2;
					fov_data_block_count *= 3;
					routing_calibration2(routing_delay2, current_frame_blocks,
							fov_data_block_count);

//					if(cal_flag_long == 1 && cal_long_status == 0)
//					{
//						memset(cal_long_distances, 0x0, sizeof(cal_long_distances));
//						cal_long_status = 9;
//					}
//					else
//					{
//						if(cal_long_status == 1)
//						{
//
//
//							//last
//		//					cal_flag_long = 0;
//		//					cal_long_status = 0;
//
//							//sorting -> del min, max
//
//							int total_ave = 0;
//
//							int aves[32] = {0x0,};
//
//							for(int ch = 0; ch < 32; ++ch)
//							{
//
//								for(int col = 0; col < 8; ++col)
//								{
//									for(int col2 = 0; col2 < 7 - col; ++col2)
//									{
//										if(cal_long_distances[ch][col2] > cal_long_distances[ch][col2+1])
//										{
//											int temp = cal_long_distances[ch][col2];
//											cal_long_distances[ch][col2] = cal_long_distances[ch][col2+1];
//											cal_long_distances[ch][col2+1] = temp;
//										}
//									}
//								}
//
//								for(int idx = 1; idx < 7; ++idx)
//								{
//									aves[ch] += cal_long_distances[ch][idx];
//								}
//
//								aves[ch] /= 6;
//
//							}
//
//							for(int idx = 0; idx<32; ++idx)
//							{
//								total_ave += aves[idx];
//							}
//
//							total_ave /= 32;
//
//							routing_calibration_long_distanceFix(routing_delay, aves, total_ave);
//
//							//cal
//							printf("total_ave = %d \r\n", total_ave);
//							cal_flag_long = 0;
//							cal_long_status = 0;
//						}
//						else
//						{
//
//							for(int channel_index = 0 ; channel_index < 16 ; ++channel_index) // channel loop 16
//							{
//								cal_long_distances[channel_index][(cal_long_status - 9)*-1] =
//									block_data_list[bottom_data_block_count/2].distance_[channel_index].distance;
//
//								cal_long_distances[channel_index+16][(cal_long_status- 9)*-1] =
//									block_data_list[bottom_data_block_count/2 + bottom_data_block_count].distance_[channel_index].distance;
//							}
//		//					routing_cal_ave(cal_long_distances, current_frame_blocks,
//		//							fov_data_block_count);
//							--cal_long_status;
//						}
//					}
				}
#endif
			}
		}

#ifdef G32_B1_PPS
		 MotorInit();
#endif
		if (cal_flag) {
			cal_flag = 0;
//			volatile u32 fov_data_block_count = EncCtrlGetStopEncCount1() - EncCtrlGetStartEncCount1() + EncCtrlGetStopEncCount2() - EncCtrlGetStartEncCount2();
			volatile u32 fov_data_block_count = EncCtrlGetStopEncCount1()
					- EncCtrlGetStartEncCount1() + EncCtrlGetStopEncCount2()
					- EncCtrlGetStartEncCount2() - 2;
			fov_data_block_count *= 3;
			routing_calibration(routing_delay, current_frame_blocks,
					fov_data_block_count);
		}



		if (fixed_cal_flag) {
			fixed_cal_flag = 0;
			volatile u32 fov_data_block_count = EncCtrlGetStopEncCount1()
					- EncCtrlGetStartEncCount1() + EncCtrlGetStopEncCount2()
					- EncCtrlGetStartEncCount2() - 2;
			fov_data_block_count *= 3;
			routing_calibration_distanceFix(routing_delay, current_frame_blocks,
					fov_data_block_count, cal_distance);
		}

		if (routing_add_flag) {
			routing_add_flag = 0;
			routing_add(routing_delay, adddelay);
			adddelay = 0;
		}

		if (routing_add_flag2) {
			routing_add_flag2 = 0;
			routing_add2(routing_delay2, adddelay);
			adddelay = 0;
		}

		if (routing_resetflag) {
			routing_reset(routing_delay);
			routing_resetflag = 0;
		}
	}
	/* never reached */
	cleanup_platform();
	return 0;
}
