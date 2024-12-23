
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/10/31 draft
*
-----------------------------------------------------------------------*/
#define API_VERSION_INFO_IP_LOCAL
#include "../../../common/src/api_hw/api_hw_version_info_ip.h"

s32 FpgaVersionInfoInit(void)
{
    s32 Status;

    Status = FpgaVersionInfoSelfTest();

    if( Status > 0 ) {
        xil_printf("[NG] VERSION INFO IP Init Failed\r\n");
        return FAILURE;
    }
    xil_printf("[OK] VERSION INFO IP Initialized\r\n");

    return SUCCESS;
}

s32 FpgaVersionInfoSelfTest(void)
{
    return SUCCESS;
}

s32 FpgaPrintVersionInfo(void)
{
    u32 ver[4];

    ver[0] = IP_VERSION_INFO_mReadReg(VERSION_INFO_BASEADDR, VERSION_INFO_REG0_OFFSET);
    ver[1] = IP_VERSION_INFO_mReadReg(VERSION_INFO_BASEADDR, VERSION_INFO_REG1_OFFSET);
    ver[2] = IP_VERSION_INFO_mReadReg(VERSION_INFO_BASEADDR, VERSION_INFO_REG2_OFFSET);
    ver[3] = IP_VERSION_INFO_mReadReg(VERSION_INFO_BASEADDR, VERSION_INFO_REG3_OFFSET);

    xil_printf("FPGA VERSION = ");
    xil_printf("%c%c%c%c", (ver[3]>>24)&0xff,(ver[3]>>16)&0xff,(ver[3]>>8)&0xff,(ver[3]>>0)&0xff );
    xil_printf("%c%c%c%c", (ver[2]>>24)&0xff,(ver[2]>>16)&0xff,(ver[2]>>8)&0xff,(ver[2]>>0)&0xff );
    xil_printf("%c%c%c%c", (ver[1]>>24)&0xff,(ver[1]>>16)&0xff,(ver[1]>>8)&0xff,(ver[1]>>0)&0xff );
    xil_printf("%c%c%c%c", (ver[0]>>24)&0xff,(ver[0]>>16)&0xff,(ver[0]>>8)&0xff,(ver[0]>>0)&0xff );
    xil_printf("\r\n");

    return SUCCESS;
}

s32 FpgaReadVersionInfo(u8 *Version)
{
    u32 ver[4];

    ver[0] = IP_VERSION_INFO_mReadReg(VERSION_INFO_BASEADDR, VERSION_INFO_REG0_OFFSET);
    ver[1] = IP_VERSION_INFO_mReadReg(VERSION_INFO_BASEADDR, VERSION_INFO_REG1_OFFSET);
    ver[2] = IP_VERSION_INFO_mReadReg(VERSION_INFO_BASEADDR, VERSION_INFO_REG2_OFFSET);
    ver[3] = IP_VERSION_INFO_mReadReg(VERSION_INFO_BASEADDR, VERSION_INFO_REG3_OFFSET);

    Version[15] = (ver[3]>>24)&0xff;
    Version[14] = (ver[3]>>16)&0xff;
    Version[13] = (ver[3]>>8)&0xff;
    Version[12] = (ver[3]>>0)&0xff;
    Version[11] = (ver[2]>>24)&0xff;
    Version[10] = (ver[2]>>16)&0xff;
    Version[9] = (ver[2]>>8)&0xff;
    Version[8] = (ver[2]>>0)&0xff;
    Version[7] = (ver[1]>>24)&0xff;
    Version[6] = (ver[1]>>16)&0xff;
    Version[5] = (ver[1]>>8)&0xff;
    Version[4] = (ver[1]>>0)&0xff;
    Version[3] = (ver[0]>>24)&0xff;
    Version[2] = (ver[0]>>16)&0xff;
    Version[1] = (ver[0]>>8)&0xff;
    Version[0] = (ver[0]>>0)&0xff;

    return SUCCESS;
}

u32 GetVersionRegisterValue(u32 offset)
{
	return IP_VERSION_INFO_mReadReg(VERSION_INFO_BASEADDR, offset);
}

XStatus SetVersionRegisterValue(u32 offset, u32 value)
{
	IP_VERSION_INFO_mWriteReg(VERSION_INFO_BASEADDR, offset, value);
	return SUCCESS;
}
