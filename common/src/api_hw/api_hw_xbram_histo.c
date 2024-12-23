
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/11/01 Draft
*
-----------------------------------------------------------------------*/
#define API_XBRAM_HISTO_IP_LOCAL
#include "../../../common/src/api_hw/api_hw_xbram_histo.h"

static s32 InitializeECC(XBram_Config *ConfigPtr, u32 EffectiveAddr);

s32 TDCHistoBramCtrlInit(void)
{
	s32 Status;

	Status = TDCHistoBramCtrlSelfTest();
	if( Status > 0 ) {
		xil_printf("[NG] HISTOGRAM BRAM Init Failed\r\n");
		return FAILURE;
	}

	xil_printf("[OK] HISTOGRAM BRAM Initialized\r\n");

	return SUCCESS;
}

s32 TDCHistoBramCtrlSelfTest(void)
{
	s32 Status;

	pTDCHistoBramCtrlCfg = XBram_LookupConfig(XPAR_AXI_BRAM_CTRL_HISTO_0_DEVICE_ID);
	if (pTDCHistoBramCtrlCfg == (XBram_Config *) NULL) {
		return FAILURE;
	}

	Status = XBram_CfgInitialize(&TDCHistoBramCtrl, pTDCHistoBramCtrlCfg, pTDCHistoBramCtrlCfg->CtrlBaseAddress);
	if (Status != XST_SUCCESS) {
		return FAILURE;
	}

    InitializeECC(pTDCHistoBramCtrlCfg, pTDCHistoBramCtrlCfg->CtrlBaseAddress);

	Status = XBram_SelfTest(&TDCHistoBramCtrl, 0);
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
