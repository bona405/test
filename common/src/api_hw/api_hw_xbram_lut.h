#ifndef API_XBRAM_LUT_IP_H
#define API_XBRAM_LUT_IP_H

#ifdef   API_XBRAM_LUT_IP_LOCAL
#define  EXT_API_XBRAM_LUT_IP_DEF
#else
#define  EXT_API_XBRAM_LUT_IP_DEF   extern
#endif

#include "../../../common/src/api_hw/api_common.h"
#include "xbram.h"

XBram TDCLutBramCtrl;
XBram_Config *pTDCLutBramCtrlCfg;

EXT_API_XBRAM_LUT_IP_DEF s32 TDCLutBramCtrlInit(void);
EXT_API_XBRAM_LUT_IP_DEF s32 TDCLutBramCtrlSelfTest(void);

#endif
