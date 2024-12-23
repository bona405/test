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


u32 enc_start_value;
u32 enc_end_value;
volatile uint32_t datacot = 0;

#if 1

int main()
{
//	Xil_DCacheEnable();
	Xil_DCacheDisable();

	//Disable cache on OCM
	Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);          // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
	Xil_SetTlbAttributes((UINTPTR)SHARED_DDR_BASE, 0x14de2);
	Xil_SetTlbAttributes((UINTPTR)RX_BUFFER_BASE, 0x14de2);

	XStatus status = InitLinearQspiFlash();
	if(status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
	}

	status = ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
	memcpy(&lidar_state_, &lidar_state_buffer_, sizeof(struct LidarState));

	if(status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
	}

	//Init Lidar
	status = HWInit(&lidar_state_);

	if(status != XST_SUCCESS)
	{
	}
	//Set Histogram table
	status = CalculateHistogram(lidar_state_.calibration_setting_.histogram_start_vth);
//	status = CalculateHistogramTest(lidar_state_.histogram_start_vth);
	if(status != XST_SUCCESS)
	{
	}
//	MCP4801CtrlSetValue(16);
	MCP4801CtrlSetValue(40); // 1010 hhj
#if defined(_DTALIGN)
	MotorCtrlRPMSet(0);
#else
	if(lidar_state_.motor_setting_.motor_rpm < 0.0 || lidar_state_.motor_setting_.motor_rpm > 1500.0 || isnanf(lidar_state_.motor_setting_.motor_rpm))
		lidar_state_.motor_setting_.motor_rpm = 1200;
	MotorCtrlRPMSet(lidar_state_.motor_setting_.motor_rpm);

////	MotorCtrlRPMSet(150);
//	//	// 240624 pps
//	//	//motor initiation
//	Xil_Out32(0x43c50060, 0x0000000B);
//	usleep(1000000);
//	Xil_Out32(0x43c50060, 0x00000000);
//	usleep(1);
//	MotorCtrlRPMSet(150);
//	usleep(1);
//	usleep(3000000);
//	MotorCtrlRPMSet(600);
//	usleep(1);
//	Xil_Out32(0x43c50044, 0x00000000);
//	usleep(1);
//
////	//PPS initiation
//	Xil_Out32(0x43C50014, 0x000A0000);
//	usleep(1);
//	Xil_Out32(0x43C50018, 0x000F0000);
//	usleep(1);
//	Xil_Out32(0x43C5001C, 0x02089A22);
//	usleep(1);
//	Xil_Out32(0x43C50020, 0x00000034);
//	usleep(1);
//	Xil_Out32(0x43C50024, 0xFFFFFFCC);
//	usleep(1);
//	Xil_Out32(0x43C50030, 0x00000002);
//	usleep(1);
//	Xil_Out32(0x43C5002C, 0x00000001);
//	usleep(1000000);
//	Xil_Out32(0x43C5002C, 0x00000000);
//
//	Xil_Out32(0x43c80068, 0x003473BC);
//
////	Xil_Out32(0x43C2004C, 0x00);  // ld start delay comp tdc start 0x00~0x05




	if(lidar_state_.motor_setting_.hz_setting < 0 || lidar_state_.motor_setting_.hz_setting>=3)
	{
		lidar_state_.motor_setting_.hz_setting = 0;
	}
	HZ_VAL = lidar_state_.motor_setting_.hz_setting;
	if(lidar_state_.motor_setting_.hz_setting == 2)
	{
		//							IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 99); // CH0-CH3	//180
		//							IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
		//							IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
		//							IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 99); // CH12-CH15

//									IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 99); // CH0-CH3	//180
//									IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
//									IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
//									IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 140); // CH12-CH15
//
//		//							IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(279));	//1	//180
//		//							IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(0));	//2
//		//							IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(398));	//3	//180
//		//							IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(160));	//4
//
//									IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(330));	//1	//180
//									IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(0));	//2
//									IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(454));	//3	//180
//									IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(165));	//4


									IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 110); // CH0-CH3	//180
									IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
									IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
									IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 110); // CH12-CH15

							//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, 99); // CH0-CH3	//180
							//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, 140); // CH4-CH7
							//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, 140); // CH8-CH11
							//		IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, 140); // CH12-CH15

									//org
							//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(279));	//1	//180
							//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(0));	//2
							//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(398));	//3	//180
							//		IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(160));	//4

									IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(300));	//1	//180
									IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(0));	//2
									IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(435));	//3	//180
									IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(165));	//4
	}
#endif
	EncCtrlSetEnable();	//jh
//	MCP4822CtrlSetVthAll(105);

	XAxiDma_Config *CfgPtr;
	int Status;

	u8 *RxBufferPtr;

	RxBufferPtr = (u8 *)RX_BUFFER_BASE;

	/* Initialize the XAxiDma device.
	 */
	CfgPtr = XAxiDma_LookupConfig(DMA_DEV_ID);
	if(!CfgPtr)
	{
		xil_printf("No config found for %d\r\n", DMA_DEV_ID);
		return XST_FAILURE;
	}

	Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Initialization failed %d\r\n", Status);
		return XST_FAILURE;
	}

	if(XAxiDma_HasSg(&AxiDma))
	{
		xil_printf("Device configured as SG mode \r\n");
		return XST_FAILURE;
	}

	/* Disable interrupts, we use polling mode
	 */
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
//	u32 el_time = 0;

	u8 is_ignoring_frame = 0;
//	azimuth_test_cnt = 0;


	const int sizeofRawDataBlock = sizeof(RawDataBlock);

	u8 * RxPacket = (u8 *) RX_BUFFER_BASE;
	u8 * RxPacket0 =RxPacket;
	u8 * RxPacket1 =RxPacket + DMA_DATA_BLOCK_SIZE;
	u8 * RxPacket2 =RxPacket + 2*DMA_DATA_BLOCK_SIZE;

	/*a2z*/ // g32_b1_a2z
//	HZ_VAL = 0;
	A2Z_VAL = 0;
	uint8_t *a2z_val1 = (uint8_t *)&A2Z_VAL;
	uint16_t *a2z_val2 = (uint8_t *)&A2Z_VAL+1;
	uint8_t *a2z_val3 = (uint8_t *)&A2Z_VAL+3;

	*a2z_val2 = lidar_state_.a2z_setting.far_vth;

	IS_MOTOR_INIT = 0x13;

	u32 azimuth;

//	volatile u32 azimuth_buf[1000] = {0,};
//	volatile u32 azimuth_buf_cnt = 0;

	while(1)
	{
//		if(isMotorOK == 0)
//		{
//			XTime_GetTime(&motor_end_time);
//			motor_time = (motor_end_time - motor_start_time) / (COUNTS_PER_SECOND / 1000000);
//			if(motor_time > 3000000)
//			{
//					MotorCtrlRPMSet(600);
//					usleep(1);
//					Xil_Out32(0x43c50044, 0x00000000);
//					usleep(1);
//					isMotorOK = 1;
//			}
//		}



//		XTime_GetTime(&start);
		ALAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)RxBufferPtr,
		MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);

		const int EncCtrlGetStartEncCount1_1 = EncCtrlGetStartEncCount1() + 1;
		const int EncCtrlGetStopEncCount1_1 = EncCtrlGetStopEncCount1() - 1;
		const int EncCtrlGetStartEncCount2_1 = EncCtrlGetStartEncCount2() + 1;
		const int EncCtrlGetStopEncCount2_1 = EncCtrlGetStopEncCount2() - 1;

		const int bot_cot = EncCtrlGetStopEncCount1_1 - EncCtrlGetStartEncCount1_1 + 1;

		while((ALAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA)))
		{
#if defined(_DTALIGN)
			EncOneShot();
			usleep(10);
#endif
		}


//#ifndef __aarch64__
//		Xil_DCacheInvalidateRange((UINTPTR)RxPacket, MAX_PKT_LEN);
//#endif

#if defined(_DTALIGN)
		memcpy( (void *)(SHARED_DDR_BASE + datacot*sizeof(RawDataBlock)), RxPacket, sizeof(RawDataBlock) );

//		memcpy( (void *)(&fov_data_blocks.data_blocks[data_block_count]), RxPacket, sizeof(RawDataBlock) );


		datacot++;
		if(datacot == 30)
		{
			datacot = 0;
			SEMAPHORE_VALUE = 1;
		}
#else
		azimuth = *((u32*)RxPacket);

//		// test pps 240624
//		azimuth_buf[azimuth_buf_cnt++] = azimuth;
//		if(azimuth_buf_cnt > 1000) azimuth_buf_cnt = 1000;
		if(azimuth >= EncCtrlGetStartEncCount1_1 && azimuth <= EncCtrlGetStopEncCount1_1)
		{

				memcpy((void*)(shared_data_blocks) + ((azimuth-EncCtrlGetStartEncCount1_1)*3)*sizeofRawDataBlock, (void*)RxPacket0, sizeofRawDataBlock);
				memcpy((void*)(shared_data_blocks) + ((azimuth-EncCtrlGetStartEncCount1_1)*3 + 1)*sizeofRawDataBlock, (void*)RxPacket1, sizeofRawDataBlock);
				memcpy((void*)(shared_data_blocks) + ((azimuth-EncCtrlGetStartEncCount1_1)*3 + 2)*sizeofRawDataBlock, (void*)RxPacket2, sizeofRawDataBlock);
				data_block_count+=3;


		}
		else if(azimuth >= EncCtrlGetStartEncCount2_1 && azimuth <= EncCtrlGetStopEncCount2_1)
		{

	//			memcpy((void*)(shared_data_blocks) + ((azimuth-(EncCtrlGetStartEncCount2_1 - 768))*3)*sizeofRawDataBlock, (void*)RxPacket0, sizeofRawDataBlock);
	//			memcpy((void*)(shared_data_blocks) + ((azimuth-(EncCtrlGetStartEncCount2_1 - 768))*3 + 1)*sizeofRawDataBlock, (void*)RxPacket1, sizeofRawDataBlock);
	//			memcpy((void*)(shared_data_blocks) + ((azimuth-(EncCtrlGetStartEncCount2_1 - 768))*3 + 2)*sizeofRawDataBlock, (void*)RxPacket2, sizeofRawDataBlock);
				memcpy((void*)(shared_data_blocks) + ((azimuth-(EncCtrlGetStartEncCount2_1 - bot_cot))*3)*sizeofRawDataBlock, (void*)RxPacket0, sizeofRawDataBlock);
				memcpy((void*)(shared_data_blocks) + ((azimuth-(EncCtrlGetStartEncCount2_1 - bot_cot))*3 + 1)*sizeofRawDataBlock, (void*)RxPacket1, sizeofRawDataBlock);
				memcpy((void*)(shared_data_blocks) + ((azimuth-(EncCtrlGetStartEncCount2_1 - bot_cot))*3 + 2)*sizeofRawDataBlock, (void*)RxPacket2, sizeofRawDataBlock);
				data_block_count+=3;


			if(azimuth == EncCtrlGetStopEncCount2_1)
			{
//				/*origin*/
//				if(HZ_VAL == 0)
//				{
//					if(is_ignoring_frame)
//					{
//						DATA_BLOCK_COUNT = data_block_count;
//						SEMAPHORE_VALUE = 1;
//					}
//					is_ignoring_frame = is_ignoring_frame ^ 1;
//					data_block_count = 0;
//				}
//				else
//				{
//					DATA_BLOCK_COUNT = data_block_count;
//					SEMAPHORE_VALUE = 1;
//					data_block_count = 0;
//				}




				// 240416 new a2z
				if(*a2z_val1 == 1)
				{
					status = ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
					memcpy(&lidar_state_, &lidar_state_buffer_, sizeof(struct LidarState));
					*a2z_val2 = lidar_state_.a2z_setting.far_vth;
					*a2z_val1 = 2;
				}

				if(HZ_VAL == 0)
				{
					if (lidar_state_.filter_setting_.is_noise_on == 0)
					{
						*a2z_val3 = 1;
						if (is_ignoring_frame)
						{
							DATA_BLOCK_COUNT = data_block_count;
							SEMAPHORE_VALUE = 1;
						}
						is_ignoring_frame = is_ignoring_frame ^ 1;
						data_block_count = 0;
					}
					else
					{
						switch (lidar_state_.a2z_setting.state) {
						case 1:

//							if(*a2z_val3 == 1)
//							{
//								MCP4822CtrlSetVthAll(lidar_state_.a2z_setting.near_vth);
//							}
//							else{
//
//								memcpy((void*)(shared_data_blocks)+TEMP_DATA, (void*)shared_data_blocks, sizeofRawDataBlock * data_block_count);
//								MCP4822CtrlSetVthAll(*a2z_val2);
//							}


							break;
						case 2:
//							if(*a2z_val3 == 1)
//							{
//								for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
//									SetLdChon(ld_index, (lidar_state_.a2z_setting.near_em) / (u32)100, (lidar_state_.a2z_setting.near_em) % 100);
//								LDAllSet();
//							}
//							else
//							{
//								for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
//									SetLdChon(ld_index, (lidar_state_.a2z_setting.far_em) / (u32)100, (lidar_state_.a2z_setting.far_em) % 100);
//								LDAllSet();
//							}

							break;
						case 3:
							if(*a2z_val3 == 1)
							{
								MCP4822CtrlSetVthAll(lidar_state_.a2z_setting.near_vth);
								for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
									SetLdChon(ld_index, (lidar_state_.a2z_setting.near_em) / (u32)100, (lidar_state_.a2z_setting.near_em) % 100);
								LDAllSet();
							}
							else
							{
								MCP4822CtrlSetVthAll(lidar_state_.a2z_setting.far_vth);
								for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
									SetLdChon(ld_index, (lidar_state_.a2z_setting.far_em) / (u32)100, (lidar_state_.a2z_setting.far_em) % 100);
								LDAllSet();
							}

							break;
						default:
							MCP4822CtrlSetVthAll(250);
							for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
								SetLdChon(ld_index, (105) / (u32)100, (105) % 100);
							LDAllSet();
							break;
						}
						*a2z_val3 = *a2z_val3 ^ 1;
						DATA_BLOCK_COUNT = data_block_count;

						if(lidar_state_.filter_setting_.is_noise_on== 1)
						{
							if(*a2z_val3 == 0) SEMAPHORE_VALUE = 1;
//							SEMAPHORE_VALUE = 1;
						}
						else
						{
							SEMAPHORE_VALUE = 1;
						}
						data_block_count = 0;
					}
				}
				else
				{
					*a2z_val3 = 1;
					DATA_BLOCK_COUNT = data_block_count;
					SEMAPHORE_VALUE = 1;
					data_block_count = 0;
				}

//
//				/*a2z*/
//				if(HZ_VAL == 0)
//				{
//					*a2z_val3 = 1;
//
//					if(is_ignoring_frame)
//					{
//						DATA_BLOCK_COUNT = data_block_count;
//						SEMAPHORE_VALUE = 1;
//					}
//					is_ignoring_frame = is_ignoring_frame ^ 1;
//					data_block_count = 0;
//				}
//				else{
//					if(*a2z_val1 == 1)
//					{
//
//						status = ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
//						memcpy(&lidar_state_, &lidar_state_buffer_, sizeof(struct LidarState));
//						*a2z_val2 = lidar_state_.a2z_setting.far_vth;
//						*a2z_val1 = 2;
//					}
//					switch (lidar_state_.a2z_setting.state) {
//						case 1:
//							if(*a2z_val3 == 1)
//							{
//
//								MCP4822CtrlSetVthAll(lidar_state_.a2z_setting.near_vth);
////								memcpy((void*)(shared_data_blocks)+TEMP_DATA, (void*)shared_data_blocks, sizeofRawDataBlock * data_block_count);
//							}
//							else{
//
//								memcpy((void*)(shared_data_blocks)+TEMP_DATA, (void*)shared_data_blocks, sizeofRawDataBlock * data_block_count);
//								MCP4822CtrlSetVthAll(*a2z_val2);
//							}
//
//							break;
//						case 2:
//							if(*a2z_val3 == 1)
//							{
//								for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
//									SetLdChon(ld_index, (lidar_state_.a2z_setting.near_em) / (u32)100, (lidar_state_.a2z_setting.near_em) % 100);
//								LDAllSet();
//							}
//							else
//							{
//								for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
//									SetLdChon(ld_index, (lidar_state_.a2z_setting.far_em) / (u32)100, (lidar_state_.a2z_setting.far_em) % 100);
//								LDAllSet();
//							}
//
//							break;
//						case 3:
//							if(*a2z_val3 == 1)
//							{
//								MCP4822CtrlSetVthAll(lidar_state_.a2z_setting.near_vth);
//								for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
//									SetLdChon(ld_index, (lidar_state_.a2z_setting.near_em) / (u32)100, (lidar_state_.a2z_setting.near_em) % 100);
//								LDAllSet();
//							}
//							else
//							{
//								MCP4822CtrlSetVthAll(lidar_state_.a2z_setting.far_vth);
//								for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
//									SetLdChon(ld_index, (lidar_state_.a2z_setting.far_em) / (u32)100, (lidar_state_.a2z_setting.far_em) % 100);
//								LDAllSet();
//							}
//
//							break;
//						default:
//							MCP4822CtrlSetVthAll(250);
//							for(size_t ld_index = 0 ; ld_index < 4 ; ld_index++)
//								SetLdChon(ld_index, (105) / (u32)100, (105) % 100);
//							LDAllSet();
//							break;
//					}
//					*a2z_val3 = *a2z_val3 ^ 1;
//					DATA_BLOCK_COUNT = data_block_count;
//					if(lidar_state_.a2z_setting.state == 1 && *a2z_val3 == 1)
//					{
//
//					}
//					else
//					{
//						SEMAPHORE_VALUE = 1;
//					}
//					data_block_count = 0;
//
//				}




			}
		}

#endif
	}
	/* never reached */
	cleanup_platform();
	return 0;
}
#else
int main()
{
//	Xil_DCacheEnable();
	Xil_DCacheDisable();

	//Disable cache on OCM
	Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);          // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
	Xil_SetTlbAttributes((UINTPTR)SHARED_DDR_BASE, 0x14de2);
	Xil_SetTlbAttributes((UINTPTR)RX_BUFFER_BASE, 0x14de2);

	XStatus status = InitLinearQspiFlash();
	if(status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
	}

	status = ReadLidarInfo(lidar_state_buffer_, LIDAR_STATE_SIZE);
	memcpy(&lidar_state_, &lidar_state_buffer_, sizeof(struct LidarState));

	if(status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");
	}

	//Init Lidar
	status = HWInit(&lidar_state_);

#if defined(_DTALIGN)
#else
	enc_start_value = EncCtrlGetStartEncCount1();
	enc_end_value = EncCtrlGetStopEncCount1();

	enc_start_value = EncCtrlGetStartEncCount2();
	enc_end_value = EncCtrlGetStopEncCount2();
#endif

	if(status != XST_SUCCESS)
	{

	}
	//Set Histogram table
	status = CalculateHistogram(lidar_state_.calibration_setting_.histogram_start_vth);
//	status = CalculateHistogramTest(lidar_state_.histogram_start_vth);
	if(status != XST_SUCCESS)
	{

	}
	MCP4801CtrlSetValue(16);
#if defined(_DTALIGN)
	MotorCtrlRPMSet(0);
#else
	if(lidar_state_.motor_setting_.motor_rpm < 0.0 || lidar_state_.motor_setting_.motor_rpm > 1500.0 || isnanf(lidar_state_.motor_setting_.motor_rpm))
		lidar_state_.motor_setting_.motor_rpm = 1200;
	MotorCtrlRPMSet(lidar_state_.motor_setting_.motor_rpm);
#endif
	EncCtrlSetEnable();	//jh
//	MCP4822CtrlSetVthAll(105);

	XAxiDma_Config *CfgPtr;
	int Status;

	u8 *RxBufferPtr;


	RxBufferPtr = (u8 *)RX_BUFFER_BASE;

	/* Initialize the XAxiDma device.
	 */
	CfgPtr = XAxiDma_LookupConfig(DMA_DEV_ID);
	if(!CfgPtr)
	{
		xil_printf("No config found for %d\r\n", DMA_DEV_ID);
		return XST_FAILURE;
	}

	Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr);
	if(Status != XST_SUCCESS)
	{
		xil_printf("Initialization failed %d\r\n", Status);
		return XST_FAILURE;
	}

	if(XAxiDma_HasSg(&AxiDma))
	{
		xil_printf("Device configured as SG mode \r\n");
		return XST_FAILURE;
	}

	/* Disable interrupts, we use polling mode
	 */
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
//	u32 el_time = 0;

	u8 is_ignoring_frame = 0;
	azimuth_test_cnt = 0;
//	size_t azimuth_count = 0;
	while(1)
	{
//		XTime_GetTime(&start);
		ALAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)RxBufferPtr,
		MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);

		while((ALAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA)))
		{
#if defined(_DTALIGN)
			EncOneShot();
			usleep(10);
#endif
		}

		u8 * RxPacket = (u8 *) RX_BUFFER_BASE;
//#ifndef __aarch64__
//		Xil_DCacheInvalidateRange((UINTPTR)RxPacket, MAX_PKT_LEN);
//#endif

#if defined(_DTALIGN)
		memcpy( (void *)(SHARED_DDR_BASE + datacot*sizeof(RawDataBlock)), RxPacket, sizeof(RawDataBlock) );

//		memcpy( (void *)(&fov_data_blocks.data_blocks[data_block_count]), RxPacket, sizeof(RawDataBlock) );


		datacot++;
		if(datacot == 30)
		{
			datacot = 0;
			SEMAPHORE_VALUE = 1;
		}
#else
		azimuth = *((u32*)RxPacket);

		if(is_first_azimuth_received == 0)
			if(azimuth == EncCtrlGetStartEncCount1()+ 1)
			{
				is_first_azimuth_received = 1;
			}

		prev_azimuth = azimuth;
//		azimuth_test_arr[azimuth_test_cnt] = azimuth;
//		azimuth_test_cnt++;
//		if(azimuth_test_cnt > 9000)
//			azimuth_test_cnt = 0;
		if(is_first_azimuth_received == 1)
		{

			if((azimuth >= EncCtrlGetStartEncCount1() + 1 && azimuth <= EncCtrlGetStopEncCount1() - 1)
					|| (azimuth >= EncCtrlGetStartEncCount2() + 1 && azimuth <= EncCtrlGetStopEncCount2() - 1))
			{
#ifdef USE_ONE_SIDE_MIRROR
				if(is_ignoring_frame == 0)
#endif
				{
					memcpy((void*)(&fov_data_blocks.data_blocks[data_block_count]), (void*)RxPacket, sizeof(RawDataBlock));
					data_block_count++;
					memcpy((void*)(&fov_data_blocks.data_blocks[data_block_count]), (void*)(RxPacket + DMA_DATA_BLOCK_SIZE), sizeof(RawDataBlock));
					data_block_count++;
					memcpy((void*)(&fov_data_blocks.data_blocks[data_block_count]), (void*)(RxPacket + 2 * DMA_DATA_BLOCK_SIZE),
							sizeof(RawDataBlock));
					data_block_count++;

					if(azimuth == EncCtrlGetStopEncCount1() - 1)
						BOTTOM_DATA_BLOCK_COUNT = data_block_count;
//					if(azimuth == 2942)
//						TOP_DATA_BLOCK_COUNT = data_block_count;
					if(data_block_count >= 9500)
						data_block_count = 0;
				}
			}

			if(azimuth == EncCtrlGetStopEncCount2() - 1)
			{
//				CheckHistogramData();
#ifdef USE_ONE_SIDE_MIRROR
				if(is_ignoring_frame == 0)
#endif
				{
					DATA_BLOCK_COUNT = data_block_count;
					TOP_DATA_BLOCK_COUNT = data_block_count;
					memcpy(shared_data_blocks, &fov_data_blocks, sizeof(RawDataBlock) * data_block_count);
					SEMAPHORE_VALUE = 1;
				}
				last_azimuth_stop_cnt = 0;
				data_block_count = 0;
				echo_err_cnt = 0;
				is_first_azimuth_received = 0;
#ifdef USE_ONE_SIDE_MIRROR
				is_ignoring_frame = is_ignoring_frame ^ 1;
#endif
			}

		}
#endif
	}
	/* never reached */
	cleanup_platform();
	return 0;
}
#endif


