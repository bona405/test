
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/10/31 draft
*
-----------------------------------------------------------------------*/
#define API_MCP4822_IP_CTRL_LOCAL
#include "../../../common/src/api_hw/api_hw_mcp4822_ip_ctrl.h"

s32 MCP4822CtrlSetChannelVth(u8 channel, u16 vth)
{
	u32 busy = 1;

	MCP4822CtrlDisable();
	MCP4822ChSet(0x0000FFFF);   // CH ALL ENABLE
	MCP4822DataSet(channel, HIGH, ENABLE, vth);
	MCP4822CtrlEnable();
	MCP4822Start();
	usleep(1);
	while(busy) {
		MCP4822Busy(&busy);
	}

	return SUCCESS;
}

s32 MCP4822CtrlInit(void)
{
    s32 Status;

    Status = MCP4822CtrlSelfTest();

    if( Status > 0 ) {
        xil_printf("[NG] MCP4822 CTRL IP Init Failed\r\n");
        return FAILURE;
    }
    xil_printf("[OK] MCP4822 IP Initialized\r\n");

    return SUCCESS;
}

s32 MCP4822CtrlSelfTest(void)
{
    return SUCCESS;
}

s32 MCP4822CtrlSetDefault(void)
{
	u32 vth = 100;
	u32 busy = 1;

    MCP4822CtrlDisable();
    MCP4822ChSet(0x0000FFFF);   // CH ALL ENABLE
    MCP4822DataSet(0, HIGH, ENABLE, vth);
    MCP4822DataSet(1, HIGH, ENABLE, vth);
    MCP4822DataSet(2, HIGH, ENABLE, vth);
    MCP4822DataSet(3, HIGH, ENABLE, vth);
    MCP4822DataSet(4, HIGH, ENABLE, vth);
    MCP4822DataSet(5, HIGH, ENABLE, vth);
    MCP4822DataSet(6, HIGH, ENABLE, vth);
    MCP4822DataSet(7, HIGH, ENABLE, vth);
    MCP4822DataSet(8, HIGH, ENABLE, vth);
    MCP4822DataSet(9, HIGH, ENABLE, vth);
    MCP4822DataSet(10, HIGH, ENABLE, vth);
    MCP4822DataSet(11, HIGH, ENABLE, vth);
    MCP4822DataSet(12, HIGH, ENABLE, vth);
    MCP4822DataSet(13, HIGH, ENABLE, vth);
    MCP4822DataSet(14, HIGH, ENABLE, vth);
    MCP4822DataSet(15, HIGH, ENABLE, vth);
    MCP4822CtrlEnable();
    MCP4822Start();
    usleep(1);
    while(busy) {
    	MCP4822Busy(&busy);
    }

    return SUCCESS;
}


s32 MCP4822CtrlSetVthAll(u32 vth)
{
	u32 busy = 1;

    MCP4822CtrlDisable();
    MCP4822ChSet(0x0000FFFF);   // CH ALL ENABLE
    MCP4822DataSet(0, HIGH, ENABLE, vth);
    MCP4822DataSet(1, HIGH, ENABLE, vth);
    MCP4822DataSet(2, HIGH, ENABLE, vth);
    MCP4822DataSet(3, HIGH, ENABLE, vth);
    MCP4822DataSet(4, HIGH, ENABLE, vth);
    MCP4822DataSet(5, HIGH, ENABLE, vth);
    MCP4822DataSet(6, HIGH, ENABLE, vth);
    MCP4822DataSet(7, HIGH, ENABLE, vth);
    MCP4822DataSet(8, HIGH, ENABLE, vth);
    MCP4822DataSet(9, HIGH, ENABLE, vth);
    MCP4822DataSet(10, HIGH, ENABLE, vth);
    MCP4822DataSet(11, HIGH, ENABLE, vth);
    MCP4822DataSet(12, HIGH, ENABLE, vth);
    MCP4822DataSet(13, HIGH, ENABLE, vth);
    MCP4822DataSet(14, HIGH, ENABLE, vth);
    MCP4822DataSet(15, HIGH, ENABLE, vth);
    MCP4822CtrlEnable();
    MCP4822Start();
    usleep(1);
    while(busy) {
    	MCP4822Busy(&busy);
    }

    return SUCCESS;
}

s32 MCP4822CtrlEnable(void)
{
    IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG0_OFFSET, 0x00000001);

    return SUCCESS;
}

s32 MCP4822CtrlDisable(void)
{
    IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG0_OFFSET, 0x00000000);

    return SUCCESS;
}

s32 MCP4822Start(void)
{
    IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG1_OFFSET, 0x00000000);
    usleep(1);
    IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG1_OFFSET, 0x00000001);

    return SUCCESS;
}

s32 MCP4822ChSet(u32 ch)
{
    IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG2_OFFSET, ch);

    return SUCCESS;
}

s32 MCP4822DataSet(u8 ch, u8 GAn, u8 SHDNn, u16 data)
{
    u32 reg;

    reg = 0x00000000;
    reg |= ( (data & 0x0FFF) << 0 );
    reg |= ( (SHDNn & 0x01) << 12 );
    reg |= ( (GAn & 0x01) << 13 );

    //xil_printf("[DEBUG] MCP4822 REG Data %x\r\n", reg);

    switch (ch) {
        case 0:
            reg &= 0x7FFF;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG3_OFFSET, reg); // CH A
            break;
        case 1:
            reg |= 0x8000;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG4_OFFSET, reg); // CH B
            break;
        case 2:
            reg &= 0x7FFF;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG5_OFFSET, reg);
            break;
        case 3:
            reg |= 0x8000;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG6_OFFSET, reg);
            break;
        case 4:
            reg &= 0x7FFF;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG7_OFFSET, reg);
            break;
        case 5:
            reg |= 0x8000;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG8_OFFSET, reg);
            break;
        case 6:
            reg &= 0x7FFF;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG9_OFFSET, reg);
            break;
        case 7:
            reg |= 0x8000;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG10_OFFSET, reg);
            break;
        case 8:
            reg &= 0x7FFF;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG11_OFFSET, reg);
            break;
        case 9:
            reg |= 0x8000;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG12_OFFSET, reg);
            break;
        case 10:
            reg &= 0x7FFF;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG13_OFFSET, reg);
            break;
        case 11:
            reg |= 0x8000;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG14_OFFSET, reg);
            break;
        case 12:
            reg &= 0x7FFF;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG15_OFFSET, reg);
            break;
        case 13:
            reg |= 0x8000;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG16_OFFSET, reg);
            break;
        case 14:
            reg &= 0x7FFF;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG17_OFFSET, reg);
            break;
        case 15:
            reg |= 0x8000;
            IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG18_OFFSET, reg);
            break;
        default:
            break;
    }

    return SUCCESS;
}

s32 MCP4822Busy(u32 *busy)
{
    *busy = IP_MCP4822_CTRL_mReadReg(MCP4822_CTRL_BASEADDR, MCP4822_CTRL_REG19_OFFSET);

    return SUCCESS;
}

u32 GetMCP4822RegisterValue(u32 offset)
{
	return IP_MCP4822_CTRL_mReadReg(MCP4822_CTRL_BASEADDR, offset);
}

XStatus SetMCP4822RegisterValue(u32 offset, u32 value)
{
	IP_MCP4822_CTRL_mWriteReg(MCP4822_CTRL_BASEADDR, offset, value);
	return SUCCESS;
}

