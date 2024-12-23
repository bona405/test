
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/11/01 Draft
*
-----------------------------------------------------------------------*/
#define API_XBRAM_LUT_IP_LOCAL
#include "../../../common/src/api_hw/api_hw_xbram_lut.h"

static s32 InitializeECC(XBram_Config *ConfigPtr, u32 EffectiveAddr);

s32 TDCLutBramCtrlInit(void)
{
	s32 Status;

	Status = TDCLutBramCtrlSelfTest();
	if( Status > 0 ) {
		xil_printf("[NG] LUT BRAM Init Failed\r\n");
		return FAILURE;
	}

	xil_printf("[OK] LUT BRAM Initialized\r\n");

	return SUCCESS;
}

s32 TDCLutBramCtrlSelfTest(void)
{
	s32 Status;

	pTDCLutBramCtrlCfg = XBram_LookupConfig(XPAR_AXI_BRAM_CTRL_LUT_0_DEVICE_ID);
	if (pTDCLutBramCtrlCfg == (XBram_Config *) NULL) {
		return FAILURE;
	}

	Status = XBram_CfgInitialize(&TDCLutBramCtrl, pTDCLutBramCtrlCfg, pTDCLutBramCtrlCfg->CtrlBaseAddress);
	if (Status != XST_SUCCESS) {
		return FAILURE;
	}

    InitializeECC(pTDCLutBramCtrlCfg, pTDCLutBramCtrlCfg->CtrlBaseAddress);

	Status = XBram_SelfTest(&TDCLutBramCtrl, 0);
	if (Status != XST_SUCCESS) {
		return FAILURE;
	}

	return SUCCESS;
}

s32 InitializeECC(XBram_Config *ConfigPtr, u32 EffectiveAddr)
{
	u32 Addr;
	volatile u32 Data;

	if (ConfigPtr->EccPresent &&
	    ConfigPtr->EccOnOffRegister &&
	    ConfigPtr->EccOnOffResetValue == 0 &&
	    ConfigPtr->WriteAccess != 0) {
		for (Addr = ConfigPtr->MemBaseAddress; Addr < ConfigPtr->MemHighAddress; Addr+=4) {
			Data = XBram_In32(Addr);
			XBram_Out32(Addr, Data);
		}
		XBram_WriteReg(EffectiveAddr, XBRAM_ECC_ON_OFF_OFFSET, 0);
	}

    return SUCCESS;
}
