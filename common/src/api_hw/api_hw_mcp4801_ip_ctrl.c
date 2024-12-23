
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/10/31 draft
*
-----------------------------------------------------------------------*/
#define API_MCP4801_IP_CTRL_LOCAL
#include "../../../common/src/api_hw/api_hw_mcp4801_ip_ctrl.h"

s32 MCP4801CtrlInit(void)
{
    s32 Status;

    Status = MCP4801CtrlSelfTest();

    if( Status > 0 ) {
        xil_printf("[NG] MCP4801 CTRL IP Init Failed\r\n");
        return FAILURE;
    }
    xil_printf("[OK] MCP4801 IP Initialized\r\n");

    return SUCCESS;
}

s32 MCP4801CtrlSelfTest(void)
{
    return SUCCESS;
}

s32 MCP4801CtrlSetDefault(void)
{
	u32 busy = 1;

	MCP4801CtrlDisable();
    MCP4801DataSet(HIGH, ENABLE, 0);
	MCP4801CtrlEnable();
	MCP4801Start();
	usleep(1);
    while(busy) {
    	MCP4801Busy(&busy);
    }

    return SUCCESS;
}

s32 MCP4801CtrlSetValue(u16 nb)
{
	u32 busy = 1;

	MCP4801CtrlDisable();
	MCP4801DataSet(HIGH, ENABLE, nb);
	MCP4801CtrlEnable();
	MCP4801Start();
	usleep(1);
	while(busy) {
		MCP4801Busy(&busy);
	}

	return SUCCESS;
}

s32 MCP4801CtrlEnable(void)
{
    IP_MCP4801_CTRL_mWriteReg(MCP4801_CTRL_BASEADDR, MCP4801_CTRL_REG0_OFFSET, 0x00000001);

    return SUCCESS;
}

s32 MCP4801CtrlDisable(void)
{
    IP_MCP4801_CTRL_mWriteReg(MCP4801_CTRL_BASEADDR, MCP4801_CTRL_REG0_OFFSET, 0x00000000);

    return SUCCESS;
}

s32 MCP4801DataSet(u8 GAn, u8 SHDNn, u16 data)
{
    u32 reg;

    reg = 0x00000000;
    reg |= ( (data & 0x00FF) << 4 );
    reg |= ( (SHDNn & 0x01) << 12 );
    reg |= ( (GAn & 0x01) << 13 );
    reg &= 0x7FFF;

    //xil_printf("[DEBUG] MCP4801 REG Data %x\r\n", reg);

    IP_MCP4801_CTRL_mWriteReg(MCP4801_CTRL_BASEADDR, MCP4801_CTRL_REG2_OFFSET, reg);

    return SUCCESS;
}

s32 MCP4801Start(void)
{
    IP_MCP4801_CTRL_mWriteReg(MCP4801_CTRL_BASEADDR, MCP4801_CTRL_REG1_OFFSET, 0x00000000);
    usleep(1);
    IP_MCP4801_CTRL_mWriteReg(MCP4801_CTRL_BASEADDR, MCP4801_CTRL_REG1_OFFSET, 0x00000001);

    return SUCCESS;
}

s32 MCP4801Busy(u32 *busy)
{
    *busy = IP_MCP4801_CTRL_mReadReg(MCP4801_CTRL_BASEADDR, MCP4801_CTRL_REG3_OFFSET);

    return SUCCESS;
}

u32 GetMCP4801RegisterValue(u32 offset)
{
	return IP_MCP4801_CTRL_mReadReg(MCP4801_CTRL_BASEADDR, offset);
}

XStatus SetMCP4801RegisterValue(u32 offset, u32 value)
{
	IP_MCP4801_CTRL_mWriteReg(MCP4801_CTRL_BASEADDR, offset, value);
	return SUCCESS;
}
