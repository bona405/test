/*
 * dma.c
 *
 *  Created on: 2022. 12. 15.
 *      Author: Wonju
 */

#include "dma.h"
#include "../../../common/src/api_sw/time/altime.h"

volatile int Error = 0; /* Dma Bus Error occurs */
volatile int RxDone = 0;
volatile int AllFovReceived = 0;

u32 data_block_count = 0;
u32 last_azimuth_stop_cnt = 0;
u32 bottom_end_index = 0;
u32 top_end_index = 0;
u8 is_first_azimuth_received = 0;
u8 current_echo_count = 0;
u32 echo_err_cnt = 0;
int32_t prev_azimuth = -1;
u32 azimuth = 0;

u32 el_time = 0;

void RxIntrHandler(void *Callback)
{
	XTime_GetTime(&start);
	u32 IrqStatus;
	int TimeOut;
	XAxiDma *AxiDmaInst = (XAxiDma *)Callback;
	//jihun
//	printf("rxintr hand\n");
	/* Read pending interrupts */
	IrqStatus = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DEVICE_TO_DMA);

	/* Acknowledge pending interrupts */
	XAxiDma_IntrAckIrq(AxiDmaInst, IrqStatus, XAXIDMA_DEVICE_TO_DMA);

	/*
	 * If no interrupt is asserted, we do not do anything
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		return;
	}

	/*
	 * If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		Error = 1;

		/* Reset could fail and hang
		 * NEED a way to handle this or do not call it??
		 */
		XAxiDma_Reset(AxiDmaInst);

		TimeOut = RESET_TIMEOUT_COUNTER;

		while (TimeOut) {
			if(XAxiDma_ResetIsDone(AxiDmaInst)) {
				break;
			}

			TimeOut -= 1;
		}

		return;
	}

	/*
	 * If completion interrupt is asserted, then set RxDone flag
	 */
	if ((IrqStatus & XAXIDMA_IRQ_IOC_MASK)) {

		XAxiDma_SimpleTransfer(&AxiDma,(UINTPTR) RX_BUFFER_BASE, MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA);

		u8 * RxPacket = (u8 *) RX_BUFFER_BASE;
		u32 * RxData = (u32 *) RX_BUFFER_BASE;
//#ifndef __aarch64__
//	Xil_DCacheInvalidateRange((UINTPTR)RxPacket, MAX_PKT_LEN);
//#endif

		azimuth = *((u32*)RxPacket);
		RxDone = 1;


//		if((azimuth >= 128 && azimuth <= 896) || azimuth >= 2174 && azimuth <= 2942)
//		{
//
//			memcpy((void*)(&shared_data_blocks->data_blocks[data_block_count]), (void*)RxPacket, sizeof(DataBlock));
//			data_block_count++;
////		memcpy((void*)(&shared_data_blocks->data_blocks[data_block_count]), (void*)(RxPacket + DMA_DATA_BLOCK_SIZE), sizeof(DataBlock));
////		data_block_count++;
////		memcpy((void*)(&shared_data_blocks->data_blocks[data_block_count]), (void*)(RxPacket + 2 * DMA_DATA_BLOCK_SIZE), sizeof(DataBlock));
////		data_block_count++;
//		}
//		if(azimuth == 2941)
//		{
//			data_block_count = 0;
//		}


		//-----------------------------------------------------------------------------------------

//			if((azimuth >= 70 && azimuth <= 452) || (azimuth >= 1104 && azimuth <= 1487))

		if(is_first_azimuth_received == 0)
			if(azimuth == EncCtrlGetStartEncCount1() + 1)
//			if(azimuth == 128)
			{
				is_first_azimuth_received = 1;
			}

//		if(prev_azimuth == azimuth)
//			current_echo_count++;
//		else
//		{
//			if(current_echo_count != 3)
//				echo_err_cnt++;
//			current_echo_count = 1;
//		}

		prev_azimuth = azimuth;
		if(is_first_azimuth_received == 1)
		{
			if((azimuth >= EncCtrlGetStartEncCount1() + 1 && azimuth <= EncCtrlGetStopEncCount1()) ||
				azimuth >= EncCtrlGetStartEncCount2() + 1 && azimuth <= EncCtrlGetStopEncCount2())
//			if(data_block_count < 9000)
			{
//				if(azimuth == EncCtrlReadStartDegree1() + 1)
//				{
//					dma_packet_cnt = 0;
//				}DMA_DATA_BLOCK_SIZE

//				shared_data_blocks->data_blocks[data_block_count].azimuth = azimuth;
//				for(int i = 0; i < 16 ; i++)
//				{
//					shared_data_blocks->data_blocks[data_block_count].data_points[i].distance = *(RxData + (i + 1));
//				}

				memcpy((void*)(&shared_data_blocks->data_blocks[data_block_count]), (void*)RxPacket ,sizeof(RawDataBlock));
				data_block_count++;
				memcpy((void*)(&shared_data_blocks->data_blocks[data_block_count]), (void*)(RxPacket+DMA_DATA_BLOCK_SIZE) ,sizeof(RawDataBlock));
				data_block_count++;
				memcpy((void*)(&shared_data_blocks->data_blocks[data_block_count]), (void*)(RxPacket+2*DMA_DATA_BLOCK_SIZE) ,sizeof(RawDataBlock));
				data_block_count++;

				if(azimuth == EncCtrlGetStopEncCount1())
//				if(azimuth == 895)
					BOTTOM_DATA_BLOCK_COUNT = data_block_count;
				if(azimuth == EncCtrlGetStopEncCount2())
//				if(azimuth == 2941)
					TOP_DATA_BLOCK_COUNT = data_block_count;
			}

//			memcpy((void*)(&shared_data_blocks->data_blocks[dma_packet_cnt]), (void*)RxPacket ,sizeof(DataBlock));


//			 = RxPacket;
//			if(dma_packet_cnt < 10000)
//				dma_packet_cnt++;

			if(azimuth == EncCtrlGetStopEncCount2())
//			if(azimuth == 2941)
			{
				last_azimuth_stop_cnt++;
//				if(last_azimuth_stop_cnt == 3)
				{
//					memcpy((void*)(SHARED_DDR_BASE), (void*)(&fov_data_blocks[0]) ,sizeof(DataBlock));
//					memcpy((void*)(&shared_data_blocks->data_blocks[0]), (void*)(&fov_data_blocks[0]) ,sizeof(DataBlock) * dma_packet_cnt);
					DATA_BLOCK_COUNT = data_block_count;
					SEMAPHORE_VALUE = 1;
					last_azimuth_stop_cnt = 0;
					data_block_count = 0;
					echo_err_cnt = 0;
					is_first_azimuth_received = 0;
				}
			}

		}
	}

	XTime_GetTime(&end);
	el_time = GetElapsedMicroSec(start, end);

}


XStatus InitDma()
{
	XStatus Status;
	XAxiDma_Config *Config;

	Config = XAxiDma_LookupConfig(DMA_DEV_ID);
	if(!Config)
	{
		xil_printf("No config found for %d\r\n", DMA_DEV_ID);

		return XST_FAILURE;
	}

	Status = XAxiDma_CfgInitialize(&AxiDma, Config);
	if(Status != XST_SUCCESS)
	{
		return XST_FAILURE;
		xil_printf("Status=%x\r\n", Status);
	}

	if(XAxiDma_HasSg(&AxiDma))
	{
		xil_printf("Device configured as SG mode \r\n");
		return XST_FAILURE;
	}

	Status = SetupIntrSystem(&Intc, &AxiDma, RX_INTR_ID, (Xil_InterruptHandler)RxIntrHandler);
	if(Status != XST_SUCCESS)
	{

		xil_printf("Failed intr setup\r\n");
		return XST_FAILURE;
	}

	/* Disable all interrupts before setup */
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

	/* Enable all interrupts */
	XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);

	XAxiDma_IntrEnable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);

//	fov_data_blocks = (DataBlock*)malloc(sizeof(DataBlock) * 15000);
}

int SetupIntrSystem(XScuGic *GicPtr, XAxiDma *AxiDmaPtr, u16 RxIntrId, Xil_InterruptHandler RxIntrHandler)
{
	int Status;
	XScuGic_Config *IntcConfig;

	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);
	if(NULL == IntcConfig)
	{
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(GicPtr, IntcConfig, IntcConfig->CpuBaseAddress);
	if(Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	XScuGic_InterruptMaptoCpu(GicPtr, 1, 63U);

	XScuGic_SetPriorityTriggerType(GicPtr, RxIntrId, 0xA0, 0x3);

	Status = XScuGic_Connect(GicPtr, RxIntrId, (Xil_InterruptHandler)RxIntrHandler, AxiDmaPtr);
	if(Status != XST_SUCCESS)
	{
		return Status;
	}

	XScuGic_Enable(GicPtr, RxIntrId);
	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)INTC_HANDLER, (void *)GicPtr);

	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

u32 ALAxiDma_SimpleTransfer(XAxiDma *InstancePtr, UINTPTR BuffAddr, u32 Length,
	int Direction)
{
	int RingIndex = 0;

	XAxiDma_WriteReg(InstancePtr->RxBdRing[RingIndex].ChanBase, XAXIDMA_DESTADDR_OFFSET, LOWER_32_BITS(BuffAddr));
	if(InstancePtr->AddrWidth > 32)
		XAxiDma_WriteReg(InstancePtr->RxBdRing[RingIndex].ChanBase, XAXIDMA_DESTADDR_MSB_OFFSET, UPPER_32_BITS(BuffAddr));

	XAxiDma_WriteReg(InstancePtr->RxBdRing[RingIndex].ChanBase, XAXIDMA_CR_OFFSET,
			XAxiDma_ReadReg(InstancePtr->RxBdRing[RingIndex].ChanBase, XAXIDMA_CR_OFFSET)| XAXIDMA_CR_RUNSTOP_MASK);
	/* Writing to the BTT register starts the transfer
	 */
	XAxiDma_WriteReg(InstancePtr->RxBdRing[RingIndex].ChanBase, XAXIDMA_BUFFLEN_OFFSET, Length);

	return XST_SUCCESS;
}

u32 ALAxiDma_Busy(XAxiDma *InstancePtr, int Direction)
{

	return ((XAxiDma_ReadReg(InstancePtr->RegBase +
				(XAXIDMA_RX_OFFSET * Direction),
				XAXIDMA_SR_OFFSET) &
				XAXIDMA_IDLE_MASK) ? FALSE : TRUE);
}
