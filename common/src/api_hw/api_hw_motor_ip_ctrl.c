
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/06/23 draft
*
-----------------------------------------------------------------------*/
#define API_MOTOR_IP_CTRL_LOCAL
#include "../../../common/src/api_hw/api_hw_motor_ip_ctrl.h"

#define REFCLK  82500000.0f    // LOGIC Ref clk 100Mhz
#if defined(G32_B1_PPS)
#define RATIO 10
//#elif defined(G32_B1)
#else
#define RATIO 5 				// DRIVER IC A89307 RATIO
#endif


s32 MotorCtrlInit(void)
{
	s32 Status;
    u32 MotorRPM;

	Status = MotorCtrlSelftest();
	Status |= MotorDisable();
	Status |= MotorBreakDisable();
	Status |= MotorCtrlSet(0,REFCLK);
	Status |= MotorRPMCheck(&MotorRPM);
	Status |= MotorEnable();

	#ifdef G32_B1_PPS
		MotorCtrlPPSInit();
	#endif

	if( Status > 0 ) {
		xil_printf("[NG] MOTOR CONTROL IP Init Failed\r\n");
		return FAILURE;
	}
	xil_printf("[OK] MOTOR CONTROL IP Initialized\r\n");

    return SUCCESS;
}

s32 MotorCtrlSelftest(void)
{
    return SUCCESS;
}

s32 MotorCtrlSetDefault(void)
{
    MotorCtrlSet(0, (u32)REFCLK);
    //MotorCtrlSet(0, (u32)REFCLK);

    return SUCCESS;
}

s32 MotorCtrlSet(u32 rpm, u32 refclk)
{
    MotorCtrlRPMSet(rpm);
    MotorCtrlRefClkSet(refclk);

    return SUCCESS;
}

s32 MotorCtrlRefClkSet(u32 refclk)
{
//	IP_MOTOR_CTRL_mWriteReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG4_OFFSET, refclk);

    return SUCCESS;
}

s32 MotorCtrlRPMSet(u32 rpm)
{
    u32 freq;
    freq = (u32)(REFCLK/(rpm/RATIO));  // RPM=Hz*Ratio
	IP_MOTOR_CTRL_mWriteReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG3_OFFSET, freq);

    return SUCCESS;
}

s32 MotorBreakDisable(void)
{
    u32 reg;
    
	reg = IP_MOTOR_CTRL_mReadReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG2_OFFSET);
    reg &= 0xfffffffe; // disable
	IP_MOTOR_CTRL_mWriteReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG2_OFFSET, reg);

	return SUCCESS;
}

s32 MotorBreakEnable(void)
{
    u32 reg;
    
	reg = IP_MOTOR_CTRL_mReadReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG2_OFFSET);
    reg |= 0x00000001; // enable
	IP_MOTOR_CTRL_mWriteReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG2_OFFSET, reg);

	return SUCCESS;
}

s32 MotorRPMCheck(u32 *rpm)
{
	*rpm = IP_MOTOR_CTRL_mReadReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG1_OFFSET);

	return SUCCESS;
}

s32 MotorDisable(void)
{
    u32 reg;
    
	reg = IP_MOTOR_CTRL_mReadReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG0_OFFSET);
    reg &= 0xfffffffe; // disable
	IP_MOTOR_CTRL_mWriteReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG0_OFFSET, reg);

	return SUCCESS;
}

s32 MotorEnable(void)
{
    u32 reg;
    
	reg = IP_MOTOR_CTRL_mReadReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG0_OFFSET);
    reg |= 0x00000001; // enable
	IP_MOTOR_CTRL_mWriteReg(MOTOR_CTRL_BASEADDR, MOTOR_CTRL_REG0_OFFSET, reg);

	return SUCCESS;
}

u32 GetMotorRegisterValue(u32 offset)
{
	return IP_MOTOR_CTRL_mReadReg(MOTOR_CTRL_BASEADDR, offset);
}

XStatus SetMotorRegisterValue(u32 offset, u32 value)
{
	IP_MOTOR_CTRL_mWriteReg(MOTOR_CTRL_BASEADDR, offset, value);
	return SUCCESS;
}

s32 MotorCtrlPPSInit()
{
	// MOTOR init
	Xil_Out32(0x43c50060, 0x0000000B);
//	usleep(1000000);
//	usleep(1000);
	usleep(10);
	Xil_Out32(0x43c50060, 0x00000000);
	usleep(1);
//	Xil_Out32(0x43c5000C, 0x0053EC60); //150RPM -> 1200RPM
//	MotorCtrlRPMSet(300);
	usleep(1);
//	usleep(3000000);
//	usleep(100000);	//success
//	usleep(50000);	//success
//	usleep(1000);	//success

//	MotorCtrlRPMSet(600);

//	usleep(1000);	//success

//	Xil_Out32(0x43c5000C, 0x000A7D8C);
	MotorCtrlRPMSet(1200);
	usleep(1);
	Xil_Out32(0x43c50044, 0x00000000);
	usleep(1);

	// PPS init
    Xil_Out32(0x43C50014, 0x000A0000);
    usleep(1);
//    Xil_Out32(0x43C50018, 0x000F0000);
    Xil_Out32(0x43C50018, 0x00140000);
    usleep(1);
    Xil_Out32(0x43C5001C, 0x02089A22);
    usleep(1);
    Xil_Out32(0x43C50020, 0x00000034);
    usleep(1);
    Xil_Out32(0x43C50024, 0xFFFFFFCC);
    usleep(1);
    Xil_Out32(0x43C50030, 0x00000002);
    usleep(1);
    Xil_Out32(0x43C5002C, 0x00000001);
    usleep(1000000);
    Xil_Out32(0x43C5002C, 0x00000000);
    usleep(1);
    Xil_Out32(0x43c80068, 0x003473BC);
    usleep(1);
    Xil_Out32(0x43C5003C, 0x00000100);
	return SUCCESS;
}
