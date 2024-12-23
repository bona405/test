
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/05/06 draft
*
-----------------------------------------------------------------------*/
#define API_HEAT_IP_CTRL_LOCAL
#include "../../../common/src/api_hw/api_hw_heat_ip_ctrl.h"

//#define FREQ_10HZ   10000000    // Ref clk 100Mhz
#define FREQ_10HZ   8250000    // Ref clk 100Mhz
//#define DUTY_10PCT  1000000     // Ref clk 100Mhz DUTY 10%
#define DUTY_10PCT  825000     // Ref clk 100Mhz DUTY 10%

s32 HeatCtrlInit(void)
{
	s32 Status;

	Status = HeatCtrlSelftest();
	Status |= HeatCtrlSet(0,0);
	Status |= HeatCtrlEnable();
	Status |= HeatCtrlCheck();
	Status |= HeatCtrlDisable();

	if( Status > 0 ) {
		xil_printf("[NG] HEAT CONTROL IP Init Failed\r\n");
		return FAILURE;
	}
	xil_printf("[OK] HEAT CONTROL IP Initialized\r\n");

    return SUCCESS;
}

s32 HeatCtrlSelftest(void)
{
    return SUCCESS;
}

// use for test
s32 HeatCtrlSetDefault()
{
    HeatCtrlFreqSet(FREQ_10HZ);
    HeatCtrlDutySet(DUTY_10PCT);

    return SUCCESS;
}

s32 HeatCtrlSet(u32 freq, u32 duty)
{
    HeatCtrlFreqSet(freq);
    HeatCtrlDutySet(duty);

    return SUCCESS;
}

s32 HeatCtrlDutySet(u32 duty)
{
	IP_HEAT_CTRL_mWriteReg(HEAT_CTRL_BASEADDR, HEAT_CTRL_REG3_OFFSET, duty);

    return SUCCESS;
}

s32 HeatCtrlFreqSet(u32 freq)
{
	IP_HEAT_CTRL_mWriteReg(HEAT_CTRL_BASEADDR, HEAT_CTRL_REG2_OFFSET, freq);

    return SUCCESS;
}

s32 HeatCtrlCheck(void)
{
    u32 reg;

	reg = IP_HEAT_CTRL_mReadReg(HEAT_CTRL_BASEADDR, HEAT_CTRL_REG1_OFFSET);
	reg = (reg&0x00000001);
    if( reg == 0x00000001 ) {
		//xil_printf("[NG] HEAT Failed\r\n");
		return FAILURE;
    }

	return SUCCESS;
}

s32 HeatCtrlDisable(void)
{
    u32 reg;
    
	reg = IP_HEAT_CTRL_mReadReg(HEAT_CTRL_BASEADDR, HEAT_CTRL_REG0_OFFSET);
    reg &= 0xfffffffe; // disable
	IP_HEAT_CTRL_mWriteReg(HEAT_CTRL_BASEADDR, HEAT_CTRL_REG0_OFFSET, reg);

	return SUCCESS;
}

s32 HeatCtrlEnable(void)
{
    u32 reg;
    
	reg = IP_HEAT_CTRL_mReadReg(HEAT_CTRL_BASEADDR, HEAT_CTRL_REG0_OFFSET);
    reg |= 0x00000001; // enable
	IP_HEAT_CTRL_mWriteReg(HEAT_CTRL_BASEADDR, HEAT_CTRL_REG0_OFFSET, reg);

	return SUCCESS;
}

u32 GetHeatRegisterValue(u32 offset)
{
	return IP_HEAT_CTRL_mReadReg(HEAT_CTRL_BASEADDR, offset);
}

XStatus SetHeatRegisterValue(u32 offset, u32 value)
{
	IP_HEAT_CTRL_mWriteReg(HEAT_CTRL_BASEADDR, offset, value);
	return SUCCESS;
}
