/*
 * main_controller.h
 *
 *  Created on: 2022. 12. 15.
 *      Author: Wonju
 */

#ifndef SRC_CONTROLLER_MAIN_CONTROLLER_H_
#define SRC_CONTROLLER_MAIN_CONTROLLER_H_

#include "xil_types.h"
#include "xstatus.h"
#include "../../../common/src/api_sw/models/machine_info.h"
#include "../../../common/src/api_hw/api_hw.h"

XStatus HWInit(LidarState* lidar_state);

#endif /* SRC_CONTROLLER_MAIN_CONTROLLER_H_ */
