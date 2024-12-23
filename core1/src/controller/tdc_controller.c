/*
 * tdc_controller.c
 *
 *  Created on: 2022. 12. 15.
 *      Author: Wonju
 */

#include "tdc_controller.h"
#include "sleep.h"
#include "xtime_l.h"
#include "../../../common/src/api_hw/api_common.h"


XStatus CalculateHistogram(uint32_t histogram_start_vth)
{
	u32 HistoDone = 0;
	u32 i, j;
	u32 Addr, Data;
	u32 fine_histogram[256] = {0,};
	u32 fine_lut[256] = {0,};

	XTime histo_start_time;
	XTime histo_end_time;
	if(histogram_start_vth == 0)
		histogram_start_vth = 20;
	u32 current_vth_for_histogram = histogram_start_vth;

	MCP4822CtrlSetVthAll(current_vth_for_histogram);
	TDCHistoClear();
	TDCHistoStart();

	XTime_GetTime(&histo_start_time);
//	while( !(HistoDone == 0x0000FFFF) ) {
	while( !(HistoDone == 0x0000000F) ) {
		XTime_GetTime(&histo_end_time);
		if((histo_end_time - histo_start_time) / (COUNTS_PER_SECOND / 1000000) > 2000000)
		{
			if(current_vth_for_histogram > 1)
				current_vth_for_histogram-=1;
			MCP4822CtrlSetVthAll(current_vth_for_histogram);
		}
		TDCHistoDone(&HistoDone);
		xil_printf("[WAIT] TDC Fine Histogram Done[0x%X]\r\n", HistoDone);
		usleep(1);
	};
	TDCHistoStop();
	xil_printf("[OK] TDC Fine Histogram Done[0x%X]\r\n", HistoDone);
	xil_printf("[OK] TDC Fine Histogram Count\r\n");
//	MCP4822CtrlSetVthAll(105);

//	for(j=0; j<16; j++) {
	for(j=0; j<4; j++) {
		TDCChSel(j);
		usleep(1);

		i = 0;
		for(Addr = pTDCHistoBramCtrlCfg->MemBaseAddress; Addr < pTDCHistoBramCtrlCfg->MemBaseAddress + (4*255); Addr+=4) {
			Data = XBram_In32(Addr);
			fine_histogram[i] = Data;
			if( i == 0 ) {
				fine_lut[0] = Data;
			}
			else {
				fine_lut[i] = fine_lut[i-1] + Data;
				if(fine_lut[i] > 262143)
				fine_lut[i] = 262143;
			}
//			xil_printf("[DEBUG] Fine Histogram[%d] = %d\r\n", i, fine_histogram[i]);
			i++;
		}

		i = 0;
		for(Addr = pTDCLutBramCtrlCfg->MemBaseAddress; Addr < pTDCLutBramCtrlCfg->MemBaseAddress + (4*255); Addr+=4) {
			XBram_Out32(Addr, fine_lut[i]);
			//xil_printf("[DEBUG] Fine Lut[%d] = %d\r\n", i, fine_lut[i]);
			i++;
		}
	}

	xil_printf("[OK] TDC Calibration Done\r\n");

	return SUCCESS;
}

XStatus CheckHistogramData()
{

	u32 fine_lut[16][256] = {0,};
	u32 Addr, Data;
	int i, j = 0;

	for(j=0; j<16; j++) {
		TDCChSel(j);
		usleep(1);



		i = 0;
		for(Addr = pTDCLutBramCtrlCfg->MemBaseAddress; Addr < pTDCLutBramCtrlCfg->MemBaseAddress + (4*255); Addr+=4) {
			fine_lut[j][i] = XBram_In32(Addr);
			//xil_printf("[DEBUG] Fine Lut[%d] = %d\r\n", i, fine_lut[i]);
			i++;
		}
	}

	xil_printf("[OK] TDC Calibration Done\r\n");

	return SUCCESS;
}



XStatus CalculateHistogramTest(uint32_t histogram_start_vth)
{
	u32 HistoDone = 0;
	u32 i, j;
	u32 Addr, Data;
	float fine_lut[30][16][256] = {0,};
	volatile int fine_lut_int[30][16][256] = {0,};

	XTime histo_start_time;
	XTime histo_end_time;
	u32 current_vth_for_histogram = histogram_start_vth;
//	u32 current_vth_for_histogram = 0;

	size_t vth_ind = 0;
	for(size_t vth_ind = 0 ; vth_ind < 1 ; vth_ind++)
	{
		HistoDone = 0;
		MCP4822CtrlSetVthAll(current_vth_for_histogram);
		size_t histo_cnt = 0;
		while(histo_cnt < 1)
		{
			TDCHistoClear();
			TDCHistoStart();
			while( !(HistoDone == 0x0000FFFF) ) {
				TDCHistoDone(&HistoDone);
				xil_printf("[WAIT] TDC Fine Histogram Done[0x%X]\r\n", HistoDone);
				usleep(1);
			};
			TDCHistoStop();
			xil_printf("[OK] TDC Fine Histogram Done[0x%X]\r\n", HistoDone);
			xil_printf("[OK] TDC Fine Histogram Count\r\n");
			histo_cnt++;

			for(j = 0; j < 16 ; j++)
			{
				TDCChSel(j);
				usleep(1);

				i = 0;
				for(Addr = pTDCHistoBramCtrlCfg->MemBaseAddress; Addr < pTDCHistoBramCtrlCfg->MemBaseAddress + (4 * 255) ; Addr += 4)
				{
					Data = XBram_In32(Addr);
					fine_lut[vth_ind][j][i] += fine_lut[vth_ind][j][i] * (histo_cnt - 1) + Data;
					fine_lut[vth_ind][j][i] /= histo_cnt;
					i++;
				}
			}
		}
		current_vth_for_histogram++;
	}

	for(int q = 0 ; q < 30 ; q ++)
		for(int w = 0 ; w < 16 ; w ++)
			for(int e = 0 ; e < 256 ; e ++)
				fine_lut_int[q][w][e] = (int)fine_lut[q][w][e];


//	MCP4822CtrlSetVthAll(105);

	for(j=0; j<16; j++) {
		TDCChSel(j);
		usleep(1);

		i = 0;
		for(size_t lut_ind = 0 ; lut_ind < 255 ; lut_ind++)
		{
			fine_lut[0][j][i + 1] += fine_lut[0][j][i];
			i++;
		}
		i = 0;
		for(Addr = pTDCLutBramCtrlCfg->MemBaseAddress; Addr < pTDCLutBramCtrlCfg->MemBaseAddress + (4*255); Addr+=4) {
//			fine_lut_int[0][j][i] = (int)fine_lut[0][j][i];
			XBram_Out32(Addr, (u32)fine_lut[0][j][i]);
			//xil_printf("[DEBUG] Fine Lut[%d] = %d\r\n", i, fine_lut[i]);
			i++;
		}
	}

	xil_printf("[OK] TDC Calibration Done\r\n");

	return SUCCESS;
}
