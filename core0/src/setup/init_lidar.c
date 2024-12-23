/*
 * init_setup_variable.c
 *
 *  Created on: 2022. 7. 29.
 *      Author: Wonju
 */

#include "init_lidar.h"
#include <sleep.h>

int32_t InitNetworkSettings(struct netif *netif, struct LidarState* lidar_state_, struct LidarState_Ch* lidar_state_Ch, struct LidarState_Ch* lidar_state_Ch2)
{
	/* the mac address of the board. this should be unique per board */
	//int test_i = 12345;
	//int test_j = 6789;
	//log_trace("InitNetworkSettings : %d, %d", test_i, test_j);
	log_trace("InitNetworkSettings");
	unsigned char mac_ethernet_address[] = {0x00, 0x0A, 0x35, 0x05, 0x17, 0xF7};
#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || \
			XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1

	ProgramSi5324();
	ProgramSfpPhy();
#endif
#endif

	/* Define this board specific macro in order perform PHY reset
	 * on ZCU102
	 */
#ifdef XPS_BOARD_ZCU102
	IicPhyReset();
#endif

	init_platform();

	xil_printf("\r\n\r\n");
	xil_printf("-----lwIP RAW Mode Application ------\r\n");

	/* initialize lwIP */
	lwip_init();

	/* Add network interface to the netif_list, and set it as default */
	if(!xemac_add(netif, NULL, NULL, NULL, mac_ethernet_address, PLATFORM_EMAC_BASEADDR))
	{
		xil_printf("Error adding N/W interface\r\n");
		return -1;
	}

#if LWIP_IPV6==1
	netif->ip6_autoconfig_enabled = 1;
	netif_create_ip6_linklocal_address(netif, 1);
	netif_ip6_addr_set_state(netif, 0, IP6_ADDR_VALID);
	print_ipv6("\n\rlink local IPv6 address is:", &netif->ip6_addr[0]);
#endif /* LWIP_IPV6 */

	netif_set_default(netif);

	/* now enable interrupts */
	platform_enable_interrupts();

	/* specify that the network if is up */
	netif_set_up(netif);

	//Read Lidar State from flash memory
	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
	}

//	ResetFlashMemory();



	Status = ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
	memcpy(lidar_state_, lidar_state_buffer_, sizeof(struct LidarState));
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
	}
	Status = ReadLidarInfo_Ch(lidar_state_buffer_Ch_, 0x600);
	memcpy(lidar_state_Ch, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
	}

	memset(lidar_state_buffer_Ch_, 0x00, sizeof(lidar_state_buffer_Ch_));
	Status = ReadLidarInfo_Ch2(lidar_state_buffer_Ch_, 0x600);
	memcpy(lidar_state_Ch2, lidar_state_buffer_Ch_, sizeof(struct LidarState_Ch));
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
	}

	Status = ReadDarkAreaInfo(dark_area_state_buffer_, DARK_AREA_SIZE);
	memcpy(&dark_area_state_, &dark_area_state_buffer_, sizeof(struct DarkAreaErrorCorrectionSettingState));
	if(dark_area_state_.darkarea_setting_.is_enable_dark_area_crror_correction_ > 2)
	{
		memset(&dark_area_state_,0x00,sizeof(struct DarkAreaErrorCorrectionSettingState));
	}

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
	}

	routing_range_1 = lidar_state_->a2z_setting.routing_range_1;
	routing_range_2 = lidar_state_->a2z_setting.routing_range_2;
#if (LWIP_IPV6==0)
#if (LWIP_DHCP==1)
	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
	dhcp_start(netif);
	dhcp_timoutcntr = 24;

	while(((netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0))
	xemacif_input(netif);

	if(dhcp_timoutcntr <= 0)
	{

		if((netif->ip_addr.addr) == 0)
		{
			xil_printf("ERROR: DHCP request timed out\r\n");

			assign_default_ip(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
		}
	}

	/* print IP address, netmask and gateway */
#else
	if(strlen(lidar_state_->network_setting_.ip) != 0 && strlen(lidar_state_->network_setting_.subnet) != 0)
	{
		int err = assign_ip(&(netif->ip_addr), &(netif->netmask), &(netif->gw), lidar_state_);
		if(!err)

			assign_default_ip(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
	}
	else

		assign_default_ip(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
#endif
	print_ip_settings(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
#endif /* LWIP_IPV6 */

	/* start the application (web server, rxtest, txtest, etc..) */
	start_applications();
	print_headers();

	if(strlen(lidar_state_->network_setting_.port) != 0 && atoi(lidar_state_->network_setting_.port) <= 65535)
	{
		start_tcp_application(atoi(lidar_state_->network_setting_.port));
		start_udp_application(atoi(lidar_state_->network_setting_.port));
		start_udp_application_for_test(13131);
		start_udp_application_for_logging(31313);
	}
	else
	{
		start_tcp_application(5001);
		start_udp_application(5001);
		start_udp_application_for_test(13131);
		start_udp_application_for_logging(31313);
	}
#ifdef ENABLE_MOTOR_TEST
		start_udp_application_for_motor_test(15151);
#endif

	return XST_SUCCESS;
}
//
//s32 HWInit()
//{
//	FpgaVersionInfoInit();
//    EncCtrlInit();
//	HeatCtrlInit();
//	MotorCtrlInit();
//	TDCCtrlInit();
//	LDCtrlInit();
//	MCP4801CtrlInit();
//	MCP4822CtrlInit();
//	XAdcPsInit();
//	TDCHistoBramCtrlInit();
//	TDCLutBramCtrlInit();
//
//	// sequential required
//	FpgaPrintVersionInfo();
//    EncCtrlSetDefault();
//    usleep(2000);	// NEED
//    MCP4801CtrlSetDefault(); // init NB
//    MCP4822CtrlSetDefault(); // init VTH
//	HeatCtrlSetDefault();
//	MotorCtrlSetDefault();
//	LDCtrlSetDefault();
//
//	xil_printf("[OK] HW Init SUCCESS\r\n");
//
//	return XST_SUCCESS;
//}

s32 HWInit()
{
	FpgaVersionInfoInit();
    EncCtrlInit();
	HeatCtrlInit();
	MotorCtrlInit();
	TDCCtrlInit();
	LDCtrlInit();
	MCP4801CtrlInit();
	MCP4822CtrlInit();
	XAdcPsInit();
	TDCHistoBramCtrlInit();
	TDCLutBramCtrlInit();

	// sequential required
	FpgaPrintVersionInfo();
    EncCtrlSetDefault();
    usleep(2000);	// NEED
    MCP4801CtrlSetDefault();
    MCP4822CtrlSetDefault();
	HeatCtrlSetDefault();
	MotorCtrlSetDefault();
	LDCtrlSetDefault();

	xil_printf("[OK] HW Init SUCCESS\r\n");

	return SUCCESS;
}

void InitRoutingDelay(int routing_delay[32], struct LidarState_Ch *lidar_state_Ch)
{
//	routing_delay[0] = atoi(lidar_state_Ch->delay_ch1);
//	routing_delay[1] = atoi(lidar_state_Ch->delay_ch3);
//	routing_delay[2] = atoi(lidar_state_Ch->delay_ch5);
//	routing_delay[3] = atoi(lidar_state_Ch->delay_ch7);
//	routing_delay[4] = atoi(lidar_state_Ch->delay_ch9);
//	routing_delay[5] = atoi(lidar_state_Ch->delay_ch11);
//	routing_delay[6] = atoi(lidar_state_Ch->delay_ch13);
//	routing_delay[7] = atoi(lidar_state_Ch->delay_ch15);
//	routing_delay[8] = atoi(lidar_state_Ch->delay_ch17);
//	routing_delay[9] = atoi(lidar_state_Ch->delay_ch19);
//	routing_delay[10] = atoi(lidar_state_Ch->delay_ch21);
//	routing_delay[11] = atoi(lidar_state_Ch->delay_ch23);
//	routing_delay[12] = atoi(lidar_state_Ch->delay_ch25);
//	routing_delay[13] = atoi(lidar_state_Ch->delay_ch27);
//	routing_delay[14] = atoi(lidar_state_Ch->delay_ch29);
//	routing_delay[15] = atoi(lidar_state_Ch->delay_ch31);
//	routing_delay[16] = atoi(lidar_state_Ch->delay_ch2);
//	routing_delay[17] = atoi(lidar_state_Ch->delay_ch4);
//	routing_delay[18] = atoi(lidar_state_Ch->delay_ch6);
//	routing_delay[19] = atoi(lidar_state_Ch->delay_ch8);
//	routing_delay[20] = atoi(lidar_state_Ch->delay_ch10);
//	routing_delay[21] = atoi(lidar_state_Ch->delay_ch12);
//	routing_delay[22] = atoi(lidar_state_Ch->delay_ch14);
//	routing_delay[23] = atoi(lidar_state_Ch->delay_ch16);
//	routing_delay[24] = atoi(lidar_state_Ch->delay_ch18);
//	routing_delay[25] = atoi(lidar_state_Ch->delay_ch20);
//	routing_delay[26] = atoi(lidar_state_Ch->delay_ch22);
//	routing_delay[27] = atoi(lidar_state_Ch->delay_ch24);
//	routing_delay[28] = atoi(lidar_state_Ch->delay_ch26);
//	routing_delay[29] = atoi(lidar_state_Ch->delay_ch28);
//	routing_delay[30] = atoi(lidar_state_Ch->delay_ch30);
//	routing_delay[31] = atoi(lidar_state_Ch->delay_ch32);

	routing_delay[0] = atoi(lidar_state_Ch->delay_ch1);
	routing_delay[1] = atoi(lidar_state_Ch->delay_ch2);
	routing_delay[2] = atoi(lidar_state_Ch->delay_ch3);
	routing_delay[3] = atoi(lidar_state_Ch->delay_ch4);
	routing_delay[4] = atoi(lidar_state_Ch->delay_ch5);
	routing_delay[5] = atoi(lidar_state_Ch->delay_ch6);
	routing_delay[6] = atoi(lidar_state_Ch->delay_ch7);
	routing_delay[7] = atoi(lidar_state_Ch->delay_ch8);
	routing_delay[8] = atoi(lidar_state_Ch->delay_ch9);
	routing_delay[9] = atoi(lidar_state_Ch->delay_ch10);
	routing_delay[10] = atoi(lidar_state_Ch->delay_ch11);
	routing_delay[11] = atoi(lidar_state_Ch->delay_ch12);
	routing_delay[12] = atoi(lidar_state_Ch->delay_ch13);
	routing_delay[13] = atoi(lidar_state_Ch->delay_ch14);
	routing_delay[14] = atoi(lidar_state_Ch->delay_ch15);
	routing_delay[15] = atoi(lidar_state_Ch->delay_ch16);
	routing_delay[16] = atoi(lidar_state_Ch->delay_ch17);
	routing_delay[17] = atoi(lidar_state_Ch->delay_ch18);
	routing_delay[18] = atoi(lidar_state_Ch->delay_ch19);
	routing_delay[19] = atoi(lidar_state_Ch->delay_ch20);
	routing_delay[20] = atoi(lidar_state_Ch->delay_ch21);
	routing_delay[21] = atoi(lidar_state_Ch->delay_ch22);
	routing_delay[22] = atoi(lidar_state_Ch->delay_ch23);
	routing_delay[23] = atoi(lidar_state_Ch->delay_ch24);
	routing_delay[24] = atoi(lidar_state_Ch->delay_ch25);
	routing_delay[25] = atoi(lidar_state_Ch->delay_ch26);
	routing_delay[26] = atoi(lidar_state_Ch->delay_ch27);
	routing_delay[27] = atoi(lidar_state_Ch->delay_ch28);
	routing_delay[28] = atoi(lidar_state_Ch->delay_ch29);
	routing_delay[29] = atoi(lidar_state_Ch->delay_ch30);
	routing_delay[30] = atoi(lidar_state_Ch->delay_ch31);
	routing_delay[31] = atoi(lidar_state_Ch->delay_ch32);

//	routing_delay[0] = atoi(lidar_state_Ch->delay_ch1);
//	routing_delay[1] = atoi(lidar_state_Ch->delay_ch17);
//	routing_delay[2] = atoi(lidar_state_Ch->delay_ch2);
//	routing_delay[3] = atoi(lidar_state_Ch->delay_ch18);
//	routing_delay[4] = atoi(lidar_state_Ch->delay_ch3);
//	routing_delay[5] = atoi(lidar_state_Ch->delay_ch19);
//	routing_delay[6] = atoi(lidar_state_Ch->delay_ch4);
//	routing_delay[7] = atoi(lidar_state_Ch->delay_ch20);
//	routing_delay[8] = atoi(lidar_state_Ch->delay_ch5);
//	routing_delay[9] = atoi(lidar_state_Ch->delay_ch21);
//	routing_delay[10] = atoi(lidar_state_Ch->delay_ch6);
//	routing_delay[11] = atoi(lidar_state_Ch->delay_ch22);
//	routing_delay[12] = atoi(lidar_state_Ch->delay_ch7);
//	routing_delay[13] = atoi(lidar_state_Ch->delay_ch23);
//	routing_delay[14] = atoi(lidar_state_Ch->delay_ch8);
//	routing_delay[15] = atoi(lidar_state_Ch->delay_ch24);
//	routing_delay[16] = atoi(lidar_state_Ch->delay_ch9);
//	routing_delay[17] = atoi(lidar_state_Ch->delay_ch25);
//	routing_delay[18] = atoi(lidar_state_Ch->delay_ch10);
//	routing_delay[19] = atoi(lidar_state_Ch->delay_ch26);
//	routing_delay[20] = atoi(lidar_state_Ch->delay_ch11);
//	routing_delay[21] = atoi(lidar_state_Ch->delay_ch27);
//	routing_delay[22] = atoi(lidar_state_Ch->delay_ch12);
//	routing_delay[23] = atoi(lidar_state_Ch->delay_ch28);
//	routing_delay[24] = atoi(lidar_state_Ch->delay_ch13);
//	routing_delay[25] = atoi(lidar_state_Ch->delay_ch29);
//	routing_delay[26] = atoi(lidar_state_Ch->delay_ch14);
//	routing_delay[27] = atoi(lidar_state_Ch->delay_ch30);
//	routing_delay[28] = atoi(lidar_state_Ch->delay_ch15);
//	routing_delay[29] = atoi(lidar_state_Ch->delay_ch31);
//	routing_delay[30] = atoi(lidar_state_Ch->delay_ch16);
//	routing_delay[31] = atoi(lidar_state_Ch->delay_ch32);
}

// 240325
XStatus InitMinimumDetectionRange(LidarState* lidar_state)
{
	//coarse
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG9_OFFSET, lidar_state->calibration_setting_.minimum_detection_range / (u32 )100); // CH0-CH3
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG10_OFFSET, lidar_state->calibration_setting_.minimum_detection_range / (u32 )100); // CH4-CH7
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG11_OFFSET, lidar_state->calibration_setting_.minimum_detection_range / (u32 )100); // CH8-CH11
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG12_OFFSET, lidar_state->calibration_setting_.minimum_detection_range / (u32 )100); // CH12-CH15
	//fine
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG13_OFFSET, lidar_state->calibration_setting_.minimum_detection_range % 100); // CH0-CH3
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG14_OFFSET, lidar_state->calibration_setting_.minimum_detection_range % 100); // CH4-CH7
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG15_OFFSET, lidar_state->calibration_setting_.minimum_detection_range % 100); // CH8-CH11
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG16_OFFSET, lidar_state->calibration_setting_.minimum_detection_range % 100); // CH12-CH15
	if(lidar_state_.calibration_setting_.minimum_detection_range_start1 > (ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2 ||
			lidar_state_.calibration_setting_.minimum_detection_range_start1 < 0)
		lidar_state_.calibration_setting_.minimum_detection_range_start1 = 0;
	if(lidar_state_.calibration_setting_.minimum_detection_range_stop1 < -(ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2||
			lidar_state_.calibration_setting_.minimum_detection_range_stop1 > 0)
		lidar_state_.calibration_setting_.minimum_detection_range_stop1 = 0;
	if(lidar_state_.calibration_setting_.minimum_detection_range_start2 > (ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2 ||
				lidar_state_.calibration_setting_.minimum_detection_range_start2 < 0)
			lidar_state_.calibration_setting_.minimum_detection_range_start2 = 0;
	if(lidar_state_.calibration_setting_.minimum_detection_range_stop2 < -(ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2||
			lidar_state_.calibration_setting_.minimum_detection_range_stop2 > 0)
		lidar_state_.calibration_setting_.minimum_detection_range_stop2 = 0;
	TDCSetDisableStart1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.calibration_setting_.minimum_detection_range_start1);
	TDCSetDisableStop1(ENC_CTRL_LD_STOP_ENC_CNT1 + lidar_state_.calibration_setting_.minimum_detection_range_stop1);
	TDCSetDisableStart2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.calibration_setting_.minimum_detection_range_start2);
	TDCSetDisableStop2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.calibration_setting_.minimum_detection_range_stop2);
	Xil_Out32(0x43C2004C, lidar_state_.calibration_setting_.start_sig);
	return XST_SUCCESS;
}

XStatus InitLdEnabledRange(LidarState* lidar_state)
{
	if(lidar_state_.motor_setting_.start_enc_count < 0 || lidar_state_.motor_setting_.start_enc_count > (ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2)
		lidar_state_.motor_setting_.start_enc_count = ENC_CTRL_LD_START_ENC_CNT1;
	if(lidar_state_.motor_setting_.end_enc_count > 0 || lidar_state_.motor_setting_.end_enc_count < -(ENC_CTRL_LD_STOP_ENC_CNT1 - ENC_CTRL_LD_START_ENC_CNT1) / 2)
		lidar_state_.motor_setting_.end_enc_count = ENC_CTRL_LD_START_ENC_CNT1;

//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.top_enc_offset);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.top_enc_offset);

//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + + lidar_state_.motor_setting_.end_enc_count +lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count+ lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.top_enc_offset);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.total_enc_offset + lidar_state_.motor_setting_.top_enc_offset);

//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count  + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + + lidar_state_.motor_setting_.end_enc_count  + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.top_enc_offset);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.top_enc_offset);

//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count  + lidar_state_.motor_setting_.bottom_enc_offset);
////	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + + lidar_state_.motor_setting_.end_enc_count  + lidar_state_.motor_setting_.bottom_enc_offset);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + + lidar_state_.motor_setting_.end_enc_count  + lidar_state_.motor_setting_.bottom_end_enc_offset);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.top_enc_offset);
////	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.top_enc_offset);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.top_end_enc_offset);


	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.total_enc_offset);
	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.total_enc_offset);
	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count + lidar_state_.motor_setting_.total_enc_offset);
	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count + lidar_state_.motor_setting_.total_enc_offset);


//	EncCtrlSetStartEncCount1(ENC_CTRL_LD_START_ENC_CNT1 + lidar_state_.motor_setting_.start_enc_count);
//	EncCtrlSetStopEncCount1(ENC_CTRL_LD_STOP_ENC_CNT1 + lidar_state_.motor_setting_.end_enc_count);
//	EncCtrlSetStartEncCount2(ENC_CTRL_LD_START_ENC_CNT2 + lidar_state_.motor_setting_.start_enc_count);
//	EncCtrlSetStopEncCount2(ENC_CTRL_LD_STOP_ENC_CNT2 + lidar_state_.motor_setting_.end_enc_count);
	return XST_SUCCESS;
}

XStatus InitLdDefaultValue(LidarState* lidar_state)
{
	if(lidar_state_.laser_setting_.tdtr < 0 || lidar_state_.laser_setting_.tdtr > 1000)
		lidar_state_.laser_setting_.tdtr = 20;
	if(lidar_state_.laser_setting_.tdtf < 0 || lidar_state_.laser_setting_.tdtf > 1000)
		lidar_state_.laser_setting_.tdtf = 10;
	if(lidar_state_.laser_setting_.max_distance < 0 || lidar_state_.laser_setting_.max_distance > 10000)
		lidar_state_.laser_setting_.max_distance = 170;
	if(lidar_state_.laser_setting_.ch_interval_1 < 0 || lidar_state_.laser_setting_.ch_interval_1 > 10000)
		lidar_state_.laser_setting_.ch_interval_1 = 0;
	if(lidar_state_.laser_setting_.ch_interval_2 < 0 || lidar_state_.laser_setting_.ch_interval_2 > 10000)
		lidar_state_.laser_setting_.ch_interval_2 = 160;

	for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
		LDCtrlSetChannelTdTrDelay(ld_channel_index, lidar_state_.laser_setting_.tdtr);
	usleep(2000);
	for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
		LDCtrlSetChannelTdTfDelay(ld_channel_index, lidar_state_.laser_setting_.tdtf);
	usleep(2000);
	TDCCtrlSetMaxDistance((u16)lidar_state_.laser_setting_.max_distance);
	usleep(2000);
	for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
		LDCtrlSetChannelDelay1(ld_channel_index, lidar_state_.laser_setting_.ch_interval_1);
	usleep(2000);
	for(size_t ld_channel_index = 0 ; ld_channel_index < 4 ; ld_channel_index++)
		LDCtrlSetChannelDelay2(ld_channel_index, lidar_state_.laser_setting_.ch_interval_2);
}
