/*
 * tdc_controller.h
 *
 *  Created on: 2022. 12. 15.
 *      Author: Wonju
 */

#ifndef SRC_CONTROLLER_TDC_CONTROLLER_H_
#define SRC_CONTROLLER_TDC_CONTROLLER_H_

#include "xstatus.h"

#include "../../../common/src/api_hw/api_hw_xbram_histo.h"
#include "../../../common/src/api_hw/api_hw_xbram_lut.h"
#include "../../../common/src/api_hw/api_hw.h"
#include "../definitions/definitions.h"

XStatus CalculateHistogram();
XStatus CalculateHistogramTest(uint32_t histogram_start_vth);
XStatus CheckHistogramData();



#endif /* SRC_CONTROLLER_TDC_CONTROLLER_H_ */
