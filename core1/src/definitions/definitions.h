/*
 * configure.h
 *
 *  Created on: 2019. 11. 19.
 *      Author: M031025
 */

#ifndef SRC_CONFIGURE_H_
#define SRC_CONFIGURE_H_
#include "xil_types.h"
#include "../../../common/src/definitions/definitions.h"

#define SHARED_DDR_BASE		0xA000000
#define TEMP_DATA   0x0100000
//#define G32_25HZ
//#define G32_10HZ
//#define G32_ES_TEST
#define VERTICAL_ANGLE 10

//#define _DEBUG
//#define PERF_DEBUG
#undef USE_ONE_SIDE_MIRROR


//#define _DTALIGN
#undef _DTALIGN

#endif /* SRC_CONFIGURE_H_ */
