/*
 * intensity_correction.c
 *
 *  Created on: 2024. 6. 17.
 *      Author: AutoL
 */

#include "intensity_correction.h"

void intensity_correction_init_params(size_t data_block_count)
{
	data_arr_size_ = data_block_count;
	oneChAllEchoPointNum = data_arr_size_ / 2;

}

void intensity_correction_filter(RawDataBlock *frame)
{
	uint8_t echo_max = 3;

	for(size_t ch_idx = 0; ch_idx < 32; ch_idx++)
	{
		uint8_t isTop = ch_idx %2 ;
		uint8_t ch = ch_idx >> 1;
		for(size_t azimuth_idx = 6; azimuth_idx < oneChAllEchoPointNum - 8; azimuth_idx += 3)
		{
			for(size_t echo_idx = 0; echo_idx < echo_max; echo_idx++)
			{
				int data_block_idx = ((oneChAllEchoPointNum)*isTop) + azimuth_idx + echo_idx;
				DataPoint *tp = &frame[data_block_idx].distance_[ch];

				if(tp->pulse_width == 255)
				{
					DataPoint compare_p[12] =
					{
							frame[(data_block_idx- echo_idx) - 6].distance_[ch],
							frame[(data_block_idx - echo_idx) - 5].distance_[ch],
							frame[(data_block_idx - echo_idx) - 4].distance_[ch],
							frame[(data_block_idx - echo_idx) - 3].distance_[ch],
							frame[(data_block_idx - echo_idx) - 2].distance_[ch],
							frame[(data_block_idx - echo_idx) - 1].distance_[ch],
							frame[(data_block_idx - echo_idx) + 3].distance_[ch],
							frame[(data_block_idx - echo_idx) + 4].distance_[ch],
							frame[(data_block_idx - echo_idx) + 5].distance_[ch],
							frame[(data_block_idx - echo_idx) + 6].distance_[ch],
							frame[(data_block_idx - echo_idx) + 7].distance_[ch],
							frame[(data_block_idx - echo_idx) + 8].distance_[ch]
					};

					int cnt_255 = 0;
					int cnt_not_255 = 0;
					uint32_t intensity_sum = 0;
					for (int cnt = 0; cnt < 12; cnt++)
					{
						if (compare_p[cnt].distance == 0) continue;
						if (abs((int)tp->distance - (int)compare_p[cnt].distance) > 1.0 * 256) continue;

						if (compare_p[cnt].pulse_width == 255)
						{
							cnt_255++;
						}
						else {
							intensity_sum += compare_p[cnt].pulse_width;
							cnt_not_255++;
						}
						if (cnt_255 > 2) break;
					}
					if (cnt_not_255 > 0)
					{
						uint32_t intensity_mean = intensity_sum / cnt_not_255;
						tp->pulse_width = (uint8_t)intensity_mean;
					}
					else {
						tp->distance = 0;
						tp->pulse_width = 0;
					}
				}
			}
		}
	}
}

//void intensity_correction_filter(RawDataBlock *frame)
//{
//	uint8_t echo_max = 3;
//
//	int sum = 0;
//
//		for(size_t ch_idx = 0; ch_idx < 32; ch_idx++)
//		{
//			uint8_t isTop = ch_idx %2 ;
//			uint8_t ch = ch_idx >> 1;
//			for(size_t azimuth_idx = 6; azimuth_idx < oneChAllEchoPointNum - 8; azimuth_idx += 3)
//			{
//				for(size_t echo_idx = 0; echo_idx < echo_max; echo_idx++)
//				{
//					int data_block_idx = ((oneChAllEchoPointNum)*isTop) + azimuth_idx + echo_idx;
//					sum += frame[data_block_idx].distance_[ch].distance;
//					sum *= frame[data_block_idx].distance_[ch].distance;
//					sum -= frame[data_block_idx].distance_[ch].distance;
//
//					int aaaa = sum;
//
//					frame[data_block_idx].distance_[ch].distance = aaaa&0xff;
//					frame[data_block_idx].distance_[ch].pulse_width = aaaa&0xff;
//
//					data_block_idx = ((oneChAllEchoPointNum)*isTop) + azimuth_idx + echo_idx;
//					sum += frame[data_block_idx].distance_[ch].distance;
//					sum *= frame[data_block_idx].distance_[ch].distance;
//					sum -= frame[data_block_idx].distance_[ch].distance;
//
//					aaaa = sum;
//
//					frame[data_block_idx].distance_[ch].distance = aaaa&0xff;
//					frame[data_block_idx].distance_[ch].pulse_width = aaaa&0xff;
//				}
//			}
//		}
//
////		for(size_t ch_idx = 0; ch_idx < 32; ch_idx++)
////		{
////			uint8_t isTop = ch_idx %2 ;
////			uint8_t ch = ch_idx >> 1;
////			for(size_t azimuth_idx = 6; azimuth_idx < oneChAllEchoPointNum - 8; azimuth_idx += 3)
////			{
////				for(size_t echo_idx = 0; echo_idx < echo_max; echo_idx++)
////				{
////					int data_block_idx = ((oneChAllEchoPointNum)*isTop) + azimuth_idx + echo_idx;
////					sum += frame[data_block_idx].distance_[ch].distance;
////					sum *= frame[data_block_idx].distance_[ch].distance;
////					sum -= frame[data_block_idx].distance_[ch].distance;
////
////					int aaaa = sum;
////
////					frame[data_block_idx].distance_[ch].distance = sum&0xff;
////					frame[data_block_idx].distance_[ch].pulse_width = sum&0xff;
////				}
////			}
////		}
//
//
//}
