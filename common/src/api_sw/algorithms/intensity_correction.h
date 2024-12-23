/*
 * intensity_correction.h
 *
 *  Created on: 2024. 6. 17.
 *      Author: AutoL
 */

#ifndef SRC_API_SW_ALGORITHMS_INTENSITY_CORRECTION_H_
#define SRC_API_SW_ALGORITHMS_INTENSITY_CORRECTION_H_

#include "fov_noise.h"

size_t data_arr_size_;
size_t oneChAllEchoPointNum;


void intensity_correction_init_params(size_t data_block_count);
void intensity_correction_filter(RawDataBlock *frame);

#endif /* SRC_API_SW_ALGORITHMS_INTENSITY_CORRECTION_H_ */
