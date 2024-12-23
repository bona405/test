/*-----------------------------------------------------------------------
 * MODIFICATION HISTORY
 * Ver   Who       Date       Changes
 * ----- --------- ---------- --------------------------------------------
 * 0.01  WHJeong   2022/10/31 draft
 *
 -----------------------------------------------------------------------*/
#define API_ENC_IP_CTRL_LOCAL
#include "api_hw_enc_ip_ctrl.h"
#include "../api_sw/logger/logger.h"
#include "sleep.h"

XStatus EncCtrlInit(void)
{
	s32 Status;

	Status = EncCtrlSelfTest();
	EncCtrlSetDisable();

	if(Status > 0)
	{
		xil_printf("[NG] ENC CTRL IP Init Failed\r\n");
		return FAILURE;
	}
	xil_printf("[OK] ENC CTRL IP Initialized\r\n");

	return SUCCESS;
}

XStatus EncCtrlSelfTest(void)
{
	return SUCCESS;
}

XStatus EncCtrlSetDefault(void)
{
#if defined(G32B0)
	EncCtrlSetResolutionMode(0x00000001);
#elif defined(G32A3)
	EncCtrlSetResolutionMode(0x00000000);
//	EncCtrlSetResolutionMode(0x00000001);
#endif
	EncCtrlSetDisable();

//	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG9_OFFSET, 128 + 60);
//	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG10_OFFSET, 897 - 52);
//	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG11_OFFSET, 2174 + 60);
//	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG12_OFFSET, 2943 - 52);

	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG9_OFFSET, ENC_CTRL_LD_START_ENC_CNT1);
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG10_OFFSET, ENC_CTRL_LD_STOP_ENC_CNT1);
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG11_OFFSET, ENC_CTRL_LD_START_ENC_CNT2);
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG12_OFFSET, ENC_CTRL_LD_STOP_ENC_CNT2);

//  IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG9_OFFSET, 0x00000000);
//  IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG10_OFFSET, 0xFFFFFFFF);
//  EncCtrlEnable();
//  EncCtrlDisable();
	return SUCCESS;
}

//XStatus EncCtrlSetEncCountOffset(int32_t start_enc_count_offset, int32_t end_enc_count_offset)
//{
//#if defined(G32B0)
//	EncCtrlSetResolutionMode(0x00000001);
//#elif defined(G32A3)
//	EncCtrlSetResolutionMode(0x00000000);
//#endif
//	EncCtrlSetDisable();
//
//	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG9_OFFSET, 128 + start_enc_count_offset);
//	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG10_OFFSET, 897 + start_enc_count_offset);
//	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG11_OFFSET, 2174 + end_enc_count_offset);
//	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG12_OFFSET, 2943 + end_enc_count_offset);
//
////  IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG9_OFFSET, 0x00000000);
////  IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG10_OFFSET, 0xFFFFFFFF);
////  EncCtrlEnable();
////  EncCtrlDisable();
//	return SUCCESS;
//}

u8 EncCtrlGetEnable(void)
{
	return IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG0_OFFSET);
}

XStatus EncCtrlSetEnable(void)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG0_OFFSET, 0x00000001);

	return SUCCESS;
}

XStatus EncCtrlSetDisable(void)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG0_OFFSET, 0x00000000);

	return SUCCESS;
}

u32 EncCtrlGetResolutionMode()
{
	return IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG1_OFFSET);
}

XStatus EncCtrlSetResolutionMode(u32 mode)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG1_OFFSET, mode);

	return SUCCESS;
}

XStatus SetPseudoEncEdge(u32 value)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG14_OFFSET, value);
	return SUCCESS;
}

XStatus SetDTEnable()
{

//	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG17_OFFSET, 1);
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG13_OFFSET, 1);
	return SUCCESS;
}

u32 EncCtrlGetStartEncCount1()
{
	return IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG9_OFFSET);
}

XStatus EncCtrlSetStartEncCount1(u32 enc_count)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG9_OFFSET, enc_count);
	return SUCCESS;
}

u32 EncCtrlGetStartEncCount2()
{
	return IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG11_OFFSET);
}

XStatus EncCtrlSetStartEncCount2(u32 enc_count)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG11_OFFSET, enc_count);
	return SUCCESS;
}

u32 EncCtrlGetStopEncCount1()
{
	return IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG10_OFFSET);
}

XStatus EncCtrlSetStopEncCount1(u32 enc_count)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG10_OFFSET, enc_count);
	return SUCCESS;
}

u32 EncCtrlGetStopEncCount2()
{
	return IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG12_OFFSET);
}

XStatus EncCtrlSetStopEncCount2(u32 enc_count)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG12_OFFSET, enc_count);
	return SUCCESS;
}

s32 EncDoubleCntClear(void)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG2_OFFSET, 0x00000000);
	usleep(1);
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG2_OFFSET, 0x00000001);

	return SUCCESS;
}

s32 EncQuadCntClear(void)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG3_OFFSET, 0x00000000);
	usleep(1);
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG3_OFFSET, 0x00000001);

	return SUCCESS;
}

s32 EncDoubleCntRead(u32 *cnt)
{
	*cnt = IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG4_OFFSET);

	return SUCCESS;
}

s32 EncQuadCntRead(u32 *cnt)
{
	*cnt = IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG5_OFFSET);

	return SUCCESS;
}

s32 EncDoubleRoundCntRead(u32 *cnt)
{
	*cnt = IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG6_OFFSET);

	return SUCCESS;
}

s32 EncQuadRoundCntRead(u32 *cnt)
{
	*cnt = IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG7_OFFSET);

	return SUCCESS;
}

s32 EncOneShot(void)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG8_OFFSET, 0x00000001);
	usleep(1);
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, ENC_CTRL_REG8_OFFSET, 0x00000000);

	return SUCCESS;
}

u32 GetEncRegisterValue(u32 offset)
{
	return IP_ENCODER_CTRL_mReadReg(ENC_CTRL_BASEADDR, offset);
}

XStatus SetEncRegisterValue(u32 offset, u32 value)
{
	IP_ENCODER_CTRL_mWriteReg(ENC_CTRL_BASEADDR, offset, value);
	return SUCCESS;
}
