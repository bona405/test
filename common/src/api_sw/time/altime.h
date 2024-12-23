/*
 * altime.h
 *
 *  Created on: 2022. 12. 15.
 *      Author: Wonju
 */

#ifndef SRC_SW_API_TIME_ALTIME_H_
#define SRC_SW_API_TIME_ALTIME_H_

#include "xtime_l.h"

unsigned long GetElapsedMicroSec(XTime start_time, XTime end_time)
{
//#ifdef PERF_DEBUG
	return abs(1.0 * (end_time - start_time) / (COUNTS_PER_SECOND/1000000));
//#endif
//	return 0;
}

unsigned long GetElapsedMilliSec(XTime start_time, XTime end_time)
{
#ifdef PERF_DEBUG
	return abs(1.0 * (end_time - start_time) / (COUNTS_PER_SECOND/1000));
#endif
	return 0;
}

#endif /* SRC_SW_API_TIME_ALTIME_H_ */
