
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/06/28 draft
*
-----------------------------------------------------------------------*/
#define API_XADCPS_IP_LOCAL 
#include "../../../common/src/api_hw/api_hw_xadcps_ip.h"

#define VOLTAGE_DIV_RATIO (21.6) 

static s32 XAdcFractionToInt(float FloatNum);

s32 XAdcPsInit(void)
{
    s32 Status;

    pXAdcPsConfig = XAdcPs_LookupConfig(XADC_DEVICE_ID);
    if(pXAdcPsConfig == NULL) {
        return FAILURE;
    }
    XAdcPs_CfgInitialize(&XAdcInst, pXAdcPsConfig, pXAdcPsConfig->BaseAddress);

    Status = XAdcPs_SelfTest(&XAdcInst);
    if (Status != XST_SUCCESS) {
        return FAILURE;
    }

    XAdcPs_SetSequencerMode(&XAdcInst, XADCPS_SEQ_MODE_CONTINPASS);

    if( Status > 0 ) {
        xil_printf("[NG] XADCPS IP Init Failed\r\n");
        return FAILURE;
    }
    xil_printf("[OK] XADCPS IP Initialized\r\n");

    return SUCCESS;
}

s32 XAdcPsVoltageRead(u32 *voltInt, u32 *voltFrac, u8 type)
{
    u32 rawData;
    float data;

    rawData = XAdcPs_GetAdcData(&XAdcInst, type);
    data = ((((float)(rawData))* (1.0f))/65536.0f) * VOLTAGE_DIV_RATIO;
    *voltInt = (u32)(data);
    *voltFrac = XAdcFractionToInt(data);

    //printf("CURRENT VOLTAGE is %0d.%03d Volts. \r\n", *voltInt, *voltFrac);

    return SUCCESS;
}

s32 XAdcFractionToInt(float FloatNum)
{
	float Temp;

	Temp = FloatNum;
	if (FloatNum < 0) {
		Temp = -(FloatNum);
	}

	return( ((int)((Temp -(float)((int)Temp)) * (1000.0f))));
}
