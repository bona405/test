/*
 * dma.h
 *
 *  Created on: 2022. 12. 15.
 *      Author: Wonju
 */

#ifndef SRC_DMA_DMA_H_
#define SRC_DMA_DMA_H_

#include "xaxidma.h"
#include "xscugic.h"
#include "xil_types.h"
#include "../../../common/src/api_hw/api_hw_enc_ip_ctrl.h"
#include "../../../common/src/api_sw/models/fov_data_set.h"
#include "../definitions/definitions.h"
#include "xaxidma_hw.h"

#define RESET_TIMEOUT_COUNTER	10000
#define INTC_HANDLER	XScuGic_InterruptHandler
#define MEM_BASE_ADDR		0xB000000
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x900000)
#define RX_INTR_ID		XPAR_FABRIC_AXI_DMA_0_S2MM_INTROUT_INTR
#define INTC_DEVICE_ID          XPAR_SCUGIC_SINGLE_DEVICE_ID
#define INTC		XScuGic

#define MAX_PKT_LEN		0x200
#define DMA_DATA_BLOCK_SIZE	0x80

#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID

#define SHARED_OCM_MEMORY_BASE   0xFFFF0000
#define SEMAPHORE_VALUE      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE))
#define DATA_BLOCK_COUNT      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 4))
#define BOTTOM_DATA_BLOCK_COUNT      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 8))
#define TOP_DATA_BLOCK_COUNT      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 12))

#define HZ_VAL      (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 16))
#define A2Z_VAL     (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 20))

#define IS_MOTOR_INIT     (*(volatile unsigned long *)(SHARED_OCM_MEMORY_BASE + 24))

int SetupIntrSystem(XScuGic *GicPtr, XAxiDma *AxiDmaPtr, u16 RxIntrId, Xil_InterruptHandler RxIntrHandler);
inline u32 ALAxiDma_SimpleTransfer(XAxiDma *InstancePtr, UINTPTR BuffAddr, u32 Length, int Direction);
inline u32 ALAxiDma_Busy(XAxiDma *InstancePtr, int Direction);
XStatus InitDma();
XAxiDma AxiDma;		/* Instance of the XAxiDma */
INTC Intc;	/* Instance of the Interrupt Controller */

volatile int Error; /* Dma Bus Error occurs */
volatile int RxDone;
volatile int AllFovReceived;
static SharedDataBlocks* shared_data_blocks = (SharedDataBlocks*) SHARED_DDR_BASE;
static SharedDataBlocks fov_data_blocks;
//u32 azimuth_test_arr[10000];
//u32 azimuth_test_cnt;


u32 data_block_count;
u32 last_azimuth_stop_cnt;
u32 bottom_end_index;
u32 top_end_index;
u8 is_first_azimuth_received;
u8 current_echo_count;
u32 echo_err_cnt;
int32_t prev_azimuth;
RawDataBlock current_data_block;


#include "xtime_l.h"

XTime start;
XTime end;

#endif /* SRC_DMA_DMA_H_ */
