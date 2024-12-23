#ifndef API_XBRAM_HISTO_IP_H
#define API_XBRAM_HISTO_IP_H

#ifdef   API_XBRAM_HISTO_IP_LOCAL
#define  EXT_API_XBRAM_HISTO_IP_DEF
#else
#define  EXT_API_XBRAM_HISTO_IP_DEF   extern
#endif

#include "../../../common/src/api_hw/api_common.h"
#include "xbram.h"

XBram TDCHistoBramCtrl;
XBram_Config *pTDCHistoBramCtrlCfg;

EXT_API_XBRAM_HISTO_IP_DEF s32 TDCHistoBramCtrlInit(void);
EXT_API_XBRAM_HISTO_IP_DEF s32 TDCHistoBramCtrlSelfTest(void);

#endif
