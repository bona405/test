
/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/08/16 draft
*
-----------------------------------------------------------------------*/
#define API_LD_IP_CTRL_LOCAL
#include "../../../common/src/api_hw/api_hw_ld_ip_ctrl.h"
#include <sleep.h>
static HW_LD_CFG_OBJ ld_cfg;

s32 SetLdChon(u8 ch, u8 chOnCoarse, u8 chOnFine)
{
	ld_cfg.chOnCoarse[ch] = chOnCoarse;
	ld_cfg.chOnFine[ch] = chOnFine;

	return SUCCESS;
}

s32 LDCtrlInit(void)
{
	s32 Status;


	Status = LDCtrlSelfTest();
	Status |= LDChCfg(LD_CH1, DISABLE, 0, 0, 0, 0, 0, &ld_cfg);
	Status |= LDChCfg(LD_CH2, DISABLE, 0, 0, 0, 0, 0, &ld_cfg);
	Status |= LDChCfg(LD_CH3, DISABLE, 0, 0, 0, 0, 0, &ld_cfg);
	Status |= LDChCfg(LD_CH4, DISABLE, 0, 0, 0, 0, 0, &ld_cfg);
    Status |= LDAllSet();

	if( Status > 0 ) {
		xil_printf("[NG] LD CONTROL IP Init Failed\r\n");
		return FAILURE;
	}
	xil_printf("[OK] LD CONTROL IP Initialized\r\n");

    return SUCCESS;
}
s32 LDCtrlSelfTest(void)
{
    return SUCCESS;
}

s32 LDCtrlSetDefaultValue()
{
	const u8 ChInterval1 = 0; // 12.12ns * 150 , Operation interval per channel
	const u8 ChInterval2 = 160; // 12.12ns * 150 , Operation interval per channel
//	const u8 trdly = 5;
//	const u8 tfdly = 15;
	const u8 trdly = 20;
	const u8 tfdly = 10;
	const u8 OnTimeCoarse = 1; // 3ns 0~15
	const u8 OnTimeFine = 	0; // about 300ps 0~15

	for(size_t ld_channel_ind = 0 ; ld_channel_ind < 4 ; ld_channel_ind++)
	{
		ld_cfg.chDly1[ld_channel_ind] = ChInterval1;
		ld_cfg.chDly2[ld_channel_ind] = ChInterval2;
	}

#if defined(G32B0)
	LDChCfg(LD_CH1, ENABLE, ChInterval1 + (ChInterval2 + trdly + tfdly) * 0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH2, ENABLE, ChInterval1 + (ChInterval2 + trdly + tfdly) * 0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH3, ENABLE, ChInterval1 + (ChInterval2 + trdly + tfdly) * 0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH4, ENABLE, ChInterval1 + (ChInterval2 + trdly + tfdly) * 0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
#elif defined(G32A3)
//	LDChCfg(LD_CH1, ENABLE, ChInterval1 + (ChInterval2+trdly+tfdly)*2, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
//	LDChCfg(LD_CH2, ENABLE, ChInterval1 + (ChInterval2+trdly+tfdly)*0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
//	LDChCfg(LD_CH3, ENABLE, ChInterval1 + (ChInterval2+trdly+tfdly)*3, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
//	LDChCfg(LD_CH4, ENABLE, ChInterval1 + (ChInterval2+trdly+tfdly)*1, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);

	LDChCfg(LD_CH1, ENABLE, ChInterval1 + (ChInterval2)*2, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH2, ENABLE, ChInterval1 + (ChInterval2)*0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH3, ENABLE, ChInterval1 + (ChInterval2)*3, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH4, ENABLE, ChInterval1 + (ChInterval2)*1, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
#endif


	return SUCCESS;
}


s32 LDCtrlSetChannelEnable(size_t channel_index, u8 channel_enable)
{
#if LD_CONTROL
	ld_cfg.chEn[channel_index] = channel_enable;
#else

	ld_cfg.chTdc &= 0x0;

	switch(channel_index){
    case LD_CH1:
    	if(channel_enable == ENABLE){
    		ld_cfg.chEn |= (u8)0x1; //8'b0001
    	}
    	else{
    		ld_cfg.chEn &= (u8)0xE; //8'b1110
    	}
        break;
    case LD_CH2:
    	if(channel_enable == ENABLE){
    		ld_cfg.chEn |= (u8)0x2; //8'b0010
    	}
    	else{
    		ld_cfg.chEn &= (u8)0xD; //8'b1101
    	}
        break;
    case LD_CH3:
    	if(channel_enable == ENABLE){
    		ld_cfg.chEn |= (u8)0x4; //8'b0100
    	}
    	else{
    		ld_cfg.chEn &= (u8)0xB; //8'b1011
    	}
        break;
    case LD_CH4:
    	if(channel_enable == ENABLE){
    		ld_cfg.chEn |= (u8)0x8; //8'b1000
    	}
    	else{
    		ld_cfg.chEn &= (u8)0x7; //8'b0111
    	}
        break;
	default:
		break;
	}
#endif
    LDAllSet();

    return SUCCESS;
}

s32 LDCtrlSetChannelDelay2(size_t channel_index, u16 channel_delay)
{
#if defined(G32B0)
	switch(channel_index)
	{
	case 0 :
		ld_cfg.chDly[channel_index] = ld_cfg.chDly1[channel_index] + 0 * (channel_delay + ld_cfg.chTdTrDly[channel_index] + ld_cfg.chTdTfDly[channel_index]);
		break;
	case 1 :
		ld_cfg.chDly[channel_index] = ld_cfg.chDly1[channel_index] + 0 * (channel_delay + ld_cfg.chTdTrDly[channel_index] + ld_cfg.chTdTfDly[channel_index]);
		break;
	case 2 :
		ld_cfg.chDly[channel_index] = ld_cfg.chDly1[channel_index] + 0 * (channel_delay + ld_cfg.chTdTrDly[channel_index] + ld_cfg.chTdTfDly[channel_index]);
		break;
	case 3 :
		ld_cfg.chDly[channel_index] = ld_cfg.chDly1[channel_index] + 0 * (channel_delay + ld_cfg.chTdTrDly[channel_index] + ld_cfg.chTdTfDly[channel_index]);
		break;
	}
#elif defined(G32A3)
	switch(channel_index)
	{
	case 0 :
		ld_cfg.chDly2[channel_index] = channel_delay;
//		ld_cfg.chDly[channel_index] = ld_cfg.chDly1[channel_index] + 2 * (channel_delay + ld_cfg.chTdTrDly[channel_index] + ld_cfg.chTdTfDly[channel_index]);
		ld_cfg.chDly[channel_index] = ld_cfg.chDly1[channel_index] + 2 * (channel_delay);
		break;
	case 1 :
		ld_cfg.chDly2[channel_index] = channel_delay;
		ld_cfg.chDly[channel_index] = ld_cfg.chDly1[channel_index] + 0 * (channel_delay);
		break;
	case 2 :
		ld_cfg.chDly2[channel_index] = channel_delay;
		ld_cfg.chDly[channel_index] = ld_cfg.chDly1[channel_index] + 3 * (channel_delay);
		break;
	case 3 :
		ld_cfg.chDly2[channel_index] = channel_delay;
		ld_cfg.chDly[channel_index] = ld_cfg.chDly1[channel_index] + 1 * (channel_delay);
		break;
	}
#endif
    LDAllSet();

    return SUCCESS;
}

s32 LDCtrlSetChannelDelay1(size_t channel_index, u16 channel_delay)
{
#if defined(G32B0)
	switch(channel_index)
	{
	case 0 :
		ld_cfg.chDly[channel_index] = channel_delay + 0 * (ld_cfg.chDly2[channel_index] + ld_cfg.chTdTrDly[channel_index] + ld_cfg.chTdTfDly[channel_index]);
		break;
	case 1 :
		ld_cfg.chDly[channel_index] = channel_delay + 0 * (ld_cfg.chDly2[channel_index] + ld_cfg.chTdTrDly[channel_index] + ld_cfg.chTdTfDly[channel_index]);
		break;
	case 2 :
		ld_cfg.chDly[channel_index] = channel_delay + 0 * (ld_cfg.chDly2[channel_index] + ld_cfg.chTdTrDly[channel_index] + ld_cfg.chTdTfDly[channel_index]);
		break;
	case 3 :
		ld_cfg.chDly[channel_index] = channel_delay + 0 * (ld_cfg.chDly2[channel_index] + ld_cfg.chTdTrDly[channel_index] + ld_cfg.chTdTfDly[channel_index]);
		break;
	}
#elif defined(G32A3)
	switch(channel_index)
	{
	case 0 :
		ld_cfg.chDly1[channel_index] = channel_delay;
		ld_cfg.chDly[channel_index] = channel_delay + 2 * (ld_cfg.chDly2[channel_index]);
		break;
	case 1 :
		ld_cfg.chDly1[channel_index] = channel_delay;
		ld_cfg.chDly[channel_index] = channel_delay + 0 * (ld_cfg.chDly2[channel_index]);
		break;
	case 2 :
		ld_cfg.chDly1[channel_index] = channel_delay;
		ld_cfg.chDly[channel_index] = channel_delay + 3 * (ld_cfg.chDly2[channel_index]);
		break;
	case 3 :
		ld_cfg.chDly1[channel_index] = channel_delay;
		ld_cfg.chDly[channel_index] = channel_delay + 1 * (ld_cfg.chDly2[channel_index]);
		break;
	}
#endif

    LDAllSet();

    return SUCCESS;
}

s32 LDCtrlSetChannelTdTrDelay(size_t channel_index, u16 channel_delay)
{
	ld_cfg.chTdTrDly[channel_index] = channel_delay;
    LDAllSet();

    return SUCCESS;
}

s32 LDCtrlSetChannelTdTfDelay(size_t channel_index, u16 channel_delay)
{
	ld_cfg.chTdTfDly[channel_index] = channel_delay;
    LDAllSet();

    return SUCCESS;
}

s32 LDCtrlSetDefault(void)
{
	const u8 ChInterval1 = 0; // 12.12ns * 150 , Operation interval per channel
	const u8 ChInterval2 = 160; // 12.12ns * 150 , Operation interval per channel
	const u8 trdly = 10;
	const u8 tfdly = 25;
	const u8 OnTimeCoarse = 1; // 3ns 0~15
	const u8 OnTimeFine = 0; // about 300ps 0~15

#if defined(G32B0)
	LDChCfg(LD_CH1, ENABLE, ChInterval1 + (ChInterval2 + trdly + tfdly) * 0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH2, ENABLE, ChInterval1 + (ChInterval2 + trdly + tfdly) * 0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH3, ENABLE, ChInterval1 + (ChInterval2 + trdly + tfdly) * 0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH4, ENABLE, ChInterval1 + (ChInterval2 + trdly + tfdly) * 0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
#elif defined(G32A3)
//	LDChCfg(LD_CH1, ENABLE, ChInterval1 + (ChInterval2+trdly+tfdly)*2, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
//	LDChCfg(LD_CH2, ENABLE, ChInterval1 + (ChInterval2+trdly+tfdly)*0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
//	LDChCfg(LD_CH3, ENABLE, ChInterval1 + (ChInterval2+trdly+tfdly)*3, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
//	LDChCfg(LD_CH4, ENABLE, ChInterval1 + (ChInterval2+trdly+tfdly)*1, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);

	LDChCfg(LD_CH1, ENABLE, ChInterval1 + (ChInterval2)*2, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH2, ENABLE, ChInterval1 + (ChInterval2)*0, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH3, ENABLE, ChInterval1 + (ChInterval2)*3, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
	LDChCfg(LD_CH4, ENABLE, ChInterval1 + (ChInterval2)*1, trdly, tfdly, OnTimeCoarse, OnTimeFine, &ld_cfg);
#endif

	LDAllSet();

	for(size_t ld_channel_ind = 0 ; ld_channel_ind < 4 ; ld_channel_ind++)
	{
		ld_cfg.chDly1[ld_channel_ind] = ChInterval1;
		ld_cfg.chDly2[ld_channel_ind] = ChInterval2;
	}

	//old source
/*    HW_LD_CFG_OBJ ldCfgTmp;
    const u8 max_distance = 150; // 12.12ns * 200
    const u8 trdly = 20;

//    LDChCfg(LD_CH1, ENABLE, (max_distance+trdly)*2, trdly, max_distance-2, 4, &ldCfgTmp);
//    LDChCfg(LD_CH2, ENABLE, (max_distance+trdly)*0, trdly, max_distance-2, 4, &ldCfgTmp);
//    LDChCfg(LD_CH3, ENABLE, (max_distance+trdly)*3, trdly, max_distance-2, 4, &ldCfgTmp);
//    LDChCfg(LD_CH4, ENABLE, (max_distance+trdly)*1, trdly, max_distance-2, 4, &ldCfgTmp);//lmg1025 -> 1020

//    LDChCfg(LD_CH1, ENABLE, (max_distance+trdly)*2, trdly, max_distance-2, 4, &ldCfgTmp);
//    LDChCfg(LD_CH2, ENABLE, (max_distance+trdly)*0, trdly, max_distance-2, 4, &ldCfgTmp);
//    LDChCfg(LD_CH3, ENABLE, (max_distance+trdly)*3, trdly, max_distance-2, 4, &ldCfgTmp);
//    LDChCfg(LD_CH4, ENABLE, (max_distance+trdly)*1, trdly, max_distance-2, 4, &ldCfgTmp);//lmg1025 -> 1020

//    LDChCfg(LD_CH1, DISABLE, (max_distance+trdly)*2, trdly, max_distance-2, 4, &ldCfgTmp);
//    LDChCfg(LD_CH2, ENABLE, (max_distance+trdly)*0, trdly, max_distance-2, 4, &ldCfgTmp);
//    LDChCfg(LD_CH3, DISABLE, (max_distance+trdly)*3, trdly, max_distance-2, 4, &ldCfgTmp);
//    LDChCfg(LD_CH4, DISABLE, (max_distance+trdly)*1, trdly, max_distance-2, 4, &ldCfgTmp);//lmg1025 -> 1020

//    LDChCfg(LD_CH1, ENABLE, (max_distance+trdly)*0, trdly, max_distance-2, 4, &ldCfgTmp);
//	LDChCfg(LD_CH2, ENABLE, (max_distance+trdly)*0, trdly, max_distance-2, 4, &ldCfgTmp);
//	LDChCfg(LD_CH3, ENABLE, (max_distance+trdly)*0, trdly, max_distance-2, 4, &ldCfgTmp);
//	LDChCfg(LD_CH4, ENABLE, (max_distance+trdly)*0, trdly, max_distance-2, 4, &ldCfgTmp);

    LDChCfg(LD_CH1, ENABLE, (max_distance+trdly)*0, trdly, 115, 4, &ldCfgTmp);
	LDChCfg(LD_CH2, ENABLE, (max_distance+trdly)*0, trdly, 115, 4, &ldCfgTmp);
	LDChCfg(LD_CH3, ENABLE, (max_distance+trdly)*0, trdly, 115, 4, &ldCfgTmp);
	LDChCfg(LD_CH4, ENABLE, (max_distance+trdly)*0, trdly, 115, 4, &ldCfgTmp);
    LDAllSet(&ldCfgTmp);*/

    return SUCCESS;
}

s32 LDAllSet()
{
    LDChSet(LD_CH1, &ld_cfg);
    LDChSet(LD_CH2, &ld_cfg);
    LDChSet(LD_CH3, &ld_cfg);
    LDChSet(LD_CH4, &ld_cfg);

    return SUCCESS;
}

s32 LDChSet(u8 ch, HW_LD_CFG_OBJ *ldCfg)
{
    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG17_OFFSET, (u32)(ldCfg->chOnCoarse[0]));
    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG18_OFFSET, (u32)(ldCfg->chOnFine[0]));
//    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG17_OFFSET, (u32)(ldCfg->chOnCoarse[1]));
//    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG18_OFFSET, (u32)(ldCfg->chOnFine[1]));
//    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG17_OFFSET, (u32)(ldCfg->chOnCoarse[2]));
//    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG18_OFFSET, (u32)(ldCfg->chOnFine[2]));
//    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG17_OFFSET, (u32)(ldCfg->chOnCoarse[3]));
//    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG18_OFFSET, (u32)(ldCfg->chOnFine[3]));
#if LD_CONTROL
    switch(ch) {
        case LD_CH1:
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG1_OFFSET, (u32)(ldCfg->chEn[0]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(ldCfg->chDly[0]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG9_OFFSET, (u32)(ldCfg->chTdTrDly[0]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG13_OFFSET, (u32)(ldCfg->chTdTfDly[0]));
            break;
        case LD_CH2:
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG2_OFFSET, (u32)(ldCfg->chEn[1]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(ldCfg->chDly[1]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG10_OFFSET, (u32)(ldCfg->chTdTrDly[1]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG14_OFFSET, (u32)(ldCfg->chTdTfDly[1]));
            break;
        case LD_CH3:
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG3_OFFSET, (u32)(ldCfg->chEn[2]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(ldCfg->chDly[2]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG11_OFFSET, (u32)(ldCfg->chTdTrDly[2]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG15_OFFSET, (u32)(ldCfg->chTdTfDly[2]));
            break;
        case LD_CH4:
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG4_OFFSET, (u32)(ldCfg->chEn[3]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(ldCfg->chDly[3]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG12_OFFSET, (u32)(ldCfg->chTdTrDly[3]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG16_OFFSET, (u32)(ldCfg->chTdTfDly[3]));
            break;
        default:
            break;
    }
#else
    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG1_OFFSET, (u8)(ldCfg->chEn));
    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG2_OFFSET, (u8)(ldCfg->chTdc));

    switch(ch) {
        case LD_CH1:
//            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG1_OFFSET, (u32)(ldCfg->chEn[0]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG5_OFFSET, (u32)(ldCfg->chDly[0]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG9_OFFSET, (u32)(ldCfg->chTdTrDly[0]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG13_OFFSET, (u32)(ldCfg->chTdTfDly[0]));
            break;
        case LD_CH2:
//            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG2_OFFSET, (u32)(ldCfg->chEn[1]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG6_OFFSET, (u32)(ldCfg->chDly[1]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG10_OFFSET, (u32)(ldCfg->chTdTrDly[1]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG14_OFFSET, (u32)(ldCfg->chTdTfDly[1]));
            break;
        case LD_CH3:
//            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG3_OFFSET, (u32)(ldCfg->chEn[2]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG7_OFFSET, (u32)(ldCfg->chDly[2]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG11_OFFSET, (u32)(ldCfg->chTdTrDly[2]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG15_OFFSET, (u32)(ldCfg->chTdTfDly[2]));
            break;
        case LD_CH4:
//            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG4_OFFSET, (u32)(ldCfg->chEn[3]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG8_OFFSET, (u32)(ldCfg->chDly[3]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG12_OFFSET, (u32)(ldCfg->chTdTrDly[3]));
            IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG16_OFFSET, (u32)(ldCfg->chTdTfDly[3]));
            break;
        default:
            break;
    }
#endif
    return SUCCESS;
}

s32 LDChCfg(u8 ch, u8 chEn, u16 chDly, u8 chTdTrDly, u8 chTdTfDly, u8 chOnCoarse, u8 chOnFine, HW_LD_CFG_OBJ *ldCfg)
{
    switch(ch) {
        case LD_CH1:
#if LD_CONTROL
            ldCfg->chEn[0] = chEn;
#else
        	if(chEn == ENABLE){
        		ldCfg->chEn |= (u8)0x1;
        	}
        	else{
        		ldCfg->chEn &= (u8)0xE;
        	}
#endif
            ldCfg->chDly[0] = chDly;
            ldCfg->chTdTrDly[0] = chTdTrDly;
            ldCfg->chTdTfDly[0] = chTdTfDly;
            ldCfg->chOnCoarse[0] = chOnCoarse;
            ldCfg->chOnFine[0] = chOnFine;
            break;
        case LD_CH2:
#if LD_CONTROL
            ldCfg->chEn[1] = chEn;
#else
        	if(chEn == ENABLE){
        		ldCfg->chEn |= (u8)0x2;
        	}
        	else{
        		ldCfg->chEn &= (u8)0xD;
        	}
#endif
            ldCfg->chDly[1] = chDly;
            ldCfg->chTdTrDly[1] = chTdTrDly;
            ldCfg->chTdTfDly[1] = chTdTfDly;
            ldCfg->chOnCoarse[1] = chOnCoarse;
            ldCfg->chOnFine[1] = chOnFine;
            break;
        case LD_CH3:
#if LD_CONTROL
            ldCfg->chEn[2] = chEn;
#else
        	if(chEn == ENABLE){
        		ldCfg->chEn |= (u8)0x4;
        	}
        	else{
        		ldCfg->chEn &= (u8)0xB;
        	}
#endif
            ldCfg->chDly[2] = chDly;
            ldCfg->chTdTrDly[2] = chTdTrDly;
            ldCfg->chTdTfDly[2] = chTdTfDly;
            ldCfg->chOnCoarse[2] = chOnCoarse;
            ldCfg->chOnFine[2] = chOnFine;
            break;
        case LD_CH4:
#if LD_CONTROL
            ldCfg->chEn[3] = chEn;
#else
        	if(chEn == ENABLE){
        		ldCfg->chEn |= (u8)0x8;
        	}
        	else{
        		ldCfg->chEn &= (u8)0x7;
        	}
#endif
            ldCfg->chDly[3] = chDly;
            ldCfg->chTdTrDly[3] = chTdTrDly;
            ldCfg->chTdTfDly[3] = chTdTfDly;
            ldCfg->chOnCoarse[3] = chOnCoarse;
            ldCfg->chOnFine[3] = chOnFine;
            break;
        default:
            break;
    }

    return SUCCESS;
}

s32 LDStart(void)
{
    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG0_OFFSET, 0x00000001);
    usleep(1);
    IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, LD_CTRL_REG0_OFFSET, 0x00000000);

    return SUCCESS;
}

u32 GetLdRegisterValue(u32 offset)
{
	return IP_LD_CTRL_mReadReg(LD_CTRL_BASEADDR, offset);
}

XStatus SetLdRegisterValue(u32 offset, u32 value)
{
	IP_LD_CTRL_mWriteReg(LD_CTRL_BASEADDR, offset, value);
	return SUCCESS;
}
