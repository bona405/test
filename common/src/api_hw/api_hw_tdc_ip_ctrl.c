
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/11/29 draft
*
-----------------------------------------------------------------------*/
#define API_TDC_IP_CTRL_LOCAL
#include "../../../common/src/api_hw/api_hw_tdc_ip_ctrl.h"

s32 TDCCtrlInit(void)
{
    s32 Status;

    Status = TDCCtrlSelfTest();

    if( Status > 0 ) {
        xil_printf("[NG] TDC CTRL IP Init Failed\r\n");
        return FAILURE;
    }
    xil_printf("[OK] TDC IP Initialized\r\n");

    TDCEchoCnt(3);

    return SUCCESS;
}


s32 TDCMaxDistance(void)
{
    const u16 max_distance = 170; // 12.12ns * 150

	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, max_distance); // CH0-CH3
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, max_distance); // CH4-CH7
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, max_distance); // CH8-CH11
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, max_distance); // CH12-CH15

    return SUCCESS;
}

s32 TDCCtrlSetMaxDistance(u16 max_distance)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG5_OFFSET, max_distance); // CH0-CH3
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG6_OFFSET, max_distance); // CH4-CH7
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG7_OFFSET, max_distance); // CH8-CH11
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG8_OFFSET, max_distance); // CH12-CH15

    return SUCCESS;
}

s32 TDCRoutingDelay(void)
{
    const u16 routing_dly_coarse_LDStop_en = 5; // ref 330Mhz, 3ns * 19
    const u8 routing_dly_fine_LDStop_en = 0; // 0-15
    const u16 routing_dly_coarse_LDStart = 0; // ref 330Mhz, 3ns * 1

	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG9_OFFSET, routing_dly_coarse_LDStop_en); // CH0-CH3
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG10_OFFSET, routing_dly_coarse_LDStop_en); // CH4-CH7
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG11_OFFSET, routing_dly_coarse_LDStop_en); // CH8-CH11
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG12_OFFSET, routing_dly_coarse_LDStop_en); // CH12-CH15
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG13_OFFSET, routing_dly_fine_LDStop_en); // CH0-CH3
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG14_OFFSET, routing_dly_fine_LDStop_en); // CH4-CH7
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG15_OFFSET, routing_dly_fine_LDStop_en); // CH8-CH11
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG16_OFFSET, routing_dly_fine_LDStop_en); // CH12-CH15

	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG17_OFFSET, routing_dly_coarse_LDStart); // CH0-CH3
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG18_OFFSET, routing_dly_coarse_LDStart); // CH4-CH7
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG19_OFFSET, routing_dly_coarse_LDStart); // CH8-CH11
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG20_OFFSET, routing_dly_coarse_LDStart); // CH12-CH15

    return SUCCESS;
}

s32 TDCSetDisableStart1(u32 value)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG21_OFFSET, value);
	return SUCCESS;
}

s32 TDCSetDisableStop1(u32 value)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG22_OFFSET, value);
	return SUCCESS;
}

s32 TDCSetDisableStart2(u32 value)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG23_OFFSET, value);
	return SUCCESS;
}

s32 TDCSetDisableStop2(u32 value)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG24_OFFSET, value);
	return SUCCESS;
}

s32 TDCSetPulseWidthScaleIndex(u32 value)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG25_OFFSET, value);
	return SUCCESS;
}

s32 TDCCtrlSelfTest(void)
{
    return SUCCESS;
}

s32 TDCCtrlSetDefault(void)
{
	TDCEchoCnt(3);
	TDCMaxDistance();
	TDCRoutingDelay();

    return SUCCESS;
}

s32 TDCHistoStart(void)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG1_OFFSET, 0x0000FFFF);

    return SUCCESS;
}

s32 TDCHistoStop(void)
{
    IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG1_OFFSET, 0x00000000);

    return SUCCESS;
}

s32 TDCHistoClear(void)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG3_OFFSET, 0x00000000);
	usleep(1);
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG2_OFFSET, 0x0000FFFF);
    usleep(1);
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG2_OFFSET, 0x00000000);

//	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG1_OFFSET, 0x00000001);
//    usleep(1);
//	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG1_OFFSET, 0x00000000);

    return SUCCESS;
}

s32 TDCHistoDone(u32 *done)
{
//    *done = IP_TDC_CTRL_mReadReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG2_OFFSET);
    *done = IP_TDC_CTRL_mReadReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG3_OFFSET);

    return SUCCESS;
}


s32 TDCChSel(u32 selch)
{
	u32 reg;

	reg = (0x00000001<<selch);

	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG0_OFFSET, reg);
    usleep(1);

    return SUCCESS;
}

s32 TDCEchoCnt(u32 echo_count)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG4_OFFSET, echo_count);
    usleep(1);
//    IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG0_OFFSET, 0x00000000);

//	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG0_OFFSET, 0x00000001);
//    usleep(10);
//    IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, TDC_CTRL_REG0_OFFSET, 0x00000000);

    return SUCCESS;
}

u32 GetTdcRegisterValue(u32 offset)
{
	return IP_TDC_CTRL_mReadReg(TDC_CTRL_BASEADDR, offset);
}

XStatus SetTdcRegisterValue(u32 offset, u32 value)
{
	IP_TDC_CTRL_mWriteReg(TDC_CTRL_BASEADDR, offset, value);
	return SUCCESS;
}
