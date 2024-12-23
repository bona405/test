/*-----------------------------------------------------------------------
* MODIFICATION HISTORY
* Ver   Who       Date       Changes
* ----- --------- ---------- --------------------------------------------
* 0.01  WHJeong   2022/05/06 draft
*
-----------------------------------------------------------------------*/

#ifndef API_COMMON_H
#define API_COMMON_H

#include "xparameters.h"
#include "xil_types.h"
#include "xil_io.h"
#include "xil_printf.h"
#include "xstatus.h"

#define SUCCESS	0
#define FAILURE	1

#define ENABLE	1
#define DISABLE	0

#define OUTPUT 1
#define INPUT 0

#define HIGH 1
#define LOW 0

#define ON 1
#define OFF 0

#define G32_B1_PPS	// PPS
//#undef G32_B1_PPS	// PPS

typedef enum eHwIpCtrl
{
	kEncCtrl, kHeatCtrl, kLdCtrl, kMcp4801Ctrl, kMcp4822Ctrl, kMotorCtrl, kTdcCtrl, kVersionInfo, NumberOfeHwIpCtrl
}eHwIpCtrl;

#define G32A3
//#define G32B0


#endif // API_COMMON_H
