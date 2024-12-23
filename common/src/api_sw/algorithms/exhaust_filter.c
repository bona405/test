/*
 * exhaust_filter.c
 *
 *  Created on: 2024. 5. 14.
 *      Author: AutoL
 */

/*
 * exhaust_filter.c
 *
 *  Created on: 2024. 4. 11.
 *      Author: AutoL
 */

#include "exhaust_filter.h"
#include "string.h"
/*	Private
 *	Stack Function
 * */
#define DEG2RAD PI / 180.0
const double elevation_forz[32] = {4.84375, 4.53125, 4.21875, 3.90625, 3.59375, 3.28125, 2.96875, 2.65625, 2.34375, 2.03125, 1.71875, 1.40625, 1.09375,
								   0.78125, 0.46875, 0.15625, -0.15625, -0.46875, -0.78125, -1.09375, -1.40625, -1.71875, -2.03125, -2.34375, -2.65625, -2.96875, -3.28125,
								   -3.59375, -3.90625, -4.21875, -4.53125, -4.84375};

const double elevation_forz_rad[32] = {
	4.84375 * DEG2RAD, 4.53125 * DEG2RAD, 4.21875 * DEG2RAD, 3.90625 * DEG2RAD,
	3.59375 * DEG2RAD, 3.28125 * DEG2RAD, 2.96875 * DEG2RAD, 2.65625 * DEG2RAD,
	2.34375 * DEG2RAD, 2.03125 * DEG2RAD, 1.71875 * DEG2RAD, 1.40625 * DEG2RAD,
	1.09375 * DEG2RAD, 0.78125 * DEG2RAD, 0.46875 * DEG2RAD, 0.15625 * DEG2RAD,
	-0.15625 * DEG2RAD, -0.46875 * DEG2RAD, -0.78125 * DEG2RAD, -1.09375 * DEG2RAD,
	-1.40625 * DEG2RAD, -1.71875 * DEG2RAD, -2.03125 * DEG2RAD, -2.34375 * DEG2RAD,
	-2.65625 * DEG2RAD, -2.96875 * DEG2RAD, -3.28125 * DEG2RAD, -3.59375 * DEG2RAD,
	-3.90625 * DEG2RAD, -4.21875 * DEG2RAD, -4.53125 * DEG2RAD, -4.84375 * DEG2RAD};

double sin_elevation_forz_rad[32] = {
	sin(-4.84375 * DEG2RAD), sin(-4.53125 * DEG2RAD), sin(-4.21875 * DEG2RAD), sin(-3.90625 * DEG2RAD),
	sin(-3.59375 * DEG2RAD), sin(-3.28125 * DEG2RAD), sin(-2.96875 * DEG2RAD), sin(-2.65625 * DEG2RAD),
	sin(-2.34375 * DEG2RAD), sin(-2.03125 * DEG2RAD), sin(-1.71875 * DEG2RAD), sin(-1.40625 * DEG2RAD),
	sin(-1.09375 * DEG2RAD), sin(-0.78125 * DEG2RAD), sin(-0.46875 * DEG2RAD), sin(-0.15625 * DEG2RAD),
	sin(0.15625 * DEG2RAD), sin(0.46875 * DEG2RAD), sin(0.78125 * DEG2RAD), sin(1.09375 * DEG2RAD),
	sin(1.40625 * DEG2RAD), sin(1.71875 * DEG2RAD), sin(2.03125 * DEG2RAD), sin(2.34375 * DEG2RAD),
	sin(2.65625 * DEG2RAD), sin(2.96875 * DEG2RAD), sin(3.28125 * DEG2RAD), sin(3.59375 * DEG2RAD),
	sin(3.90625 * DEG2RAD), sin(4.21875 * DEG2RAD), sin(4.53125 * DEG2RAD), sin(4.84375 * DEG2RAD)};

double sin_elevation_forz_rad2[32] = {
	sin(-4.84375 * DEG2RAD) / 256, sin(-4.53125 * DEG2RAD) / 256, sin(-4.21875 * DEG2RAD) / 256, sin(-3.90625 * DEG2RAD) / 256,
	sin(-3.59375 * DEG2RAD) / 256, sin(-3.28125 * DEG2RAD) / 256, sin(-2.96875 * DEG2RAD) / 256, sin(-2.65625 * DEG2RAD) / 256,
	sin(-2.34375 * DEG2RAD) / 256, sin(-2.03125 * DEG2RAD) / 256, sin(-1.71875 * DEG2RAD) / 256, sin(-1.40625 * DEG2RAD) / 256,
	sin(-1.09375 * DEG2RAD) / 256, sin(-0.78125 * DEG2RAD) / 256, sin(-0.46875 * DEG2RAD) / 256, sin(-0.15625 * DEG2RAD) / 256,
	sin(0.15625 * DEG2RAD) / 256, sin(0.46875 * DEG2RAD) / 256, sin(0.78125 * DEG2RAD) / 256, sin(1.09375 * DEG2RAD) / 256,
	sin(1.40625 * DEG2RAD) / 256, sin(1.71875 * DEG2RAD) / 256, sin(2.03125 * DEG2RAD) / 256, sin(2.34375 * DEG2RAD) / 256,
	sin(2.65625 * DEG2RAD) / 256, sin(2.96875 * DEG2RAD) / 256, sin(3.28125 * DEG2RAD) / 256, sin(3.59375 * DEG2RAD) / 256,
	sin(3.90625 * DEG2RAD) / 256, sin(4.21875 * DEG2RAD) / 256, sin(4.53125 * DEG2RAD) / 256, sin(4.84375 * DEG2RAD) / 256};

void push_node(StackRoot *sr, StackNode *sn, uint8_t root_type)
{
	uint16_t push_eps = (root_type == 0) ? cc_eps : ac_eps;

	//	uint16_t push_eps = cc_eps;

	//	if(root_type == 0)
	//	{
	//		push_eps = (sn->p->distance < distance_limit/2)?(cc_eps*0.7):(cc_eps);
	//	}
	//	else if(root_type == 1)
	//	{
	//		push_eps = (sn->p->distance < distance_limit/2)?(ac_eps*0.7):(ac_eps);
	//	}

	Stack *curStack = sr->lastStack;
	int cnt = 0;
	while (curStack != NULL)
	{
		if (curStack->top == NULL)
		{
			sn->prevStackNode = curStack->top;
			curStack->top = sn;
			curStack->StackNodeCnt++;
			break;
		}

		if ((abs((int)curStack->top->p->distance - (int)sn->p->distance) < push_eps))
		{
			if (root_type == 0 && (abs((int)sn->data_block_idx - (int)curStack->top->data_block_idx) <= 6))
			{
				sn->prevStackNode = curStack->top;
				curStack->top = sn;
				curStack->StackNodeCnt++;
				break;
			}
			else if (root_type == 1 && (abs((int)sn->ch - (int)curStack->top->ch) < 2))
			{
				sn->prevStackNode = curStack->top;
				curStack->top = sn;
				curStack->StackNodeCnt++;
				break;
			}
		}

		curStack = curStack->prevStack;
		cnt++;
		//		if(curStack == NULL || cnt > 5)
		if (curStack == NULL)
		{
			Stack *newStack = (Stack *)malloc(sizeof(Stack));
			//			Stack* newStack = stack_map[stack_map_idx++];
			newStack->top = sn;
			newStack->prevStack = sr->lastStack;
			newStack->StackNodeCnt = 0;
			sr->lastStack = newStack;
			sr->StackCnt++;
			break;
		}
	}
}

void labeling_and_destroy_stack(StackRoot *sr, uint8_t *label, size_t minPts, RawDataBlock *frame)
{
	Stack *curStack = sr->lastStack;
	while (curStack != NULL)
	{
		size_t curStackNodeCnt = curStack->StackNodeCnt;
		if (curStackNodeCnt < minPts)
		{
			while (curStack->top != NULL)
			{
				StackNode *tmpNode = curStack->top;

				label[tmpNode->data_block_idx + tmpNode->ch * data_arr_size] =
					NOTCLUSTER;

				curStack->top = curStack->top->prevStackNode;

				free(tmpNode);
			}
		}
		else
		{
			while (curStack->top != NULL)
			{
				StackNode *tmpNode = curStack->top;

				label[tmpNode->data_block_idx + tmpNode->ch * data_arr_size] =
					CLUSTERED;

				curStack->top = curStack->top->prevStackNode;

				free(tmpNode);
			}
		}

		Stack *tmpStack = curStack;
		curStack = curStack->prevStack;
		free(tmpStack);
	}
}
/*
 * Public Function
 * */

void init_params(size_t data_block_count, float cc_eps_f, float ac_eps_f, uint8_t diff_ch_idx_limit_, uint16_t distance_limit_, uint16_t merge_point_diff_distance_limit_, float z_offset_, uint8_t hz_val_, uint8_t remove_method_)
{
	data_arr_size = data_block_count;
	oneChAllEchoPointSize = data_arr_size / 2;
	oneChOneEchoPointSize = oneChAllEchoPointSize / 3;
	allChOneEchPointSize = oneChOneEchoPointSize * 32;
	allPointSize = oneChAllEchoPointSize * 32;

	cc_eps = cc_eps_f * 256; // 0.08 * 256
	ac_eps = ac_eps_f * 256; // 0.05 * 256

	diff_ch_idx_limit = diff_ch_idx_limit_;
	distance_limit = distance_limit_;
	merge_point_diff_distance_limit = merge_point_diff_distance_limit_;

	z_offset = z_offset_;
	hz_val = hz_val_;
	remove_method = remove_method_;
}
void exhaust_filter_frame_merge(RawDataBlock *frame1, RawDataBlock *frame2, size_t distane_limit, float z_limit, ConvertedDataBlock *cf, size_t oneChAllEchoPointSize, uint8_t method_)
{
	//	for (int ch_idx = 0; ch_idx < 32; ch_idx++)
	//	{
	//		uint8_t isTop = ch_idx % 2;
	//		uint8_t ch = ch_idx >> 1;
	for (int ch = 0; ch < 16; ch++)
	{
		for (uint8_t isTop = 0; isTop < 2; isTop++)
		{
			for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
			{
				int data_block_idx = ((oneChAllEchoPointSize)*isTop) + azimuth_idx;

				DataPoint *f1tp_1 = &frame1[data_block_idx].distance_[ch];
				DataPoint *f1tp_2 = &frame1[data_block_idx + 1].distance_[ch];
				DataPoint *f1tp_3 = &frame1[data_block_idx + 2].distance_[ch];

				if ((f1tp_1->distance > distance_limit + 256) || (f1tp_1->distance == 0))
				{
					cf[data_block_idx].azimuth_ = frame1[data_block_idx].azimuth_ * 0.001;
					cf[data_block_idx].distance_[ch] = frame1[data_block_idx].distance_[ch].distance * DIV_256;
					cf[data_block_idx + 1].azimuth_ = frame1[data_block_idx + 1].azimuth_ * 0.001;
					cf[data_block_idx + 1].distance_[ch] = frame1[data_block_idx + 1].distance_[ch].distance * DIV_256;
					cf[data_block_idx + 2].azimuth_ = frame1[data_block_idx + 2].azimuth_ * 0.001;
					cf[data_block_idx + 2].distance_[ch] = frame1[data_block_idx + 2].distance_[ch].distance * DIV_256;
					continue;
				}

				//			float z_position = (f1tp_1->distance / 256) * (sin((elevation_forz[31 - ch_idx])* PI / 180.0));

				//			float z_position = (f1tp_1->distance / 256) * (sin(elevation_forz_rad[31 - ch_idx]));
				//			float z_position2 = (f1tp_2->distance / 256) * (sin(elevation_forz_rad[31 - ch_idx]));
				//			float z_position3 = (f1tp_3->distance / 256) * (sin(elevation_forz_rad[31 - ch_idx]));

				uint8_t ch_idx = 2 * ch + isTop;
				float z_position = (f1tp_1->distance / 256) * sin_elevation_forz_rad[ch_idx];
				float z_position2 = (f1tp_2->distance / 256) * sin_elevation_forz_rad[ch_idx];
				float z_position3 = (f1tp_3->distance / 256) * sin_elevation_forz_rad[ch_idx];

				if (((z_position < z_limit + z_offset) && z_limit != 0.0))
				{
					cf[data_block_idx].azimuth_ = frame1[data_block_idx].azimuth_ * 0.001;
					cf[data_block_idx].distance_[ch] = frame1[data_block_idx].distance_[ch].distance * DIV_256;
					cf[data_block_idx + 1].azimuth_ = frame1[data_block_idx + 1].azimuth_ * 0.001;
					cf[data_block_idx + 1].distance_[ch] = frame1[data_block_idx + 1].distance_[ch].distance * DIV_256;
					cf[data_block_idx + 2].azimuth_ = frame1[data_block_idx + 2].azimuth_ * 0.001;
					cf[data_block_idx + 2].distance_[ch] = frame1[data_block_idx + 2].distance_[ch].distance * DIV_256;
					continue;
				}

				DataPoint *f2tp_1 = &frame2[data_block_idx].distance_[ch];
				DataPoint *f2tp_2 = &frame2[data_block_idx + 1].distance_[ch];
				//			DataPoint *f2tp_3 = &frame2[data_block_idx + 2].distance_[ch];

				DataPoint *f2tp = f2tp_1;
				if (method_ == 1)
				{
					if (f2tp_2->distance <= distance_limit + 256 && f2tp_2->distance > 0)
					{
						f2tp = f2tp_2;
					}
				}
				else if (method_ == 0)
				{
					f2tp = f2tp_1;
				}

				uint8_t closest_idx = 0;
				uint16_t min_distance = 0xFFFF;
				for (size_t f1tp_echo = 0; f1tp_echo < 3; f1tp_echo++)
				{
					if (frame1[data_block_idx + f1tp_echo].distance_[ch].distance == 0)
						continue;

					uint16_t diff_distance = abs((int)frame1[data_block_idx + f1tp_echo].distance_[ch].distance - (int)f2tp->distance);

					if (diff_distance < min_distance)
					{
						min_distance = diff_distance;
						closest_idx = f1tp_echo;
					}
				}

				if ((min_distance > merge_point_diff_distance_limit) || (f2tp->distance == 0))
				{
					if (f1tp_3->distance < distance_limit && f1tp_3->distance > 0 && (z_position3 >= z_limit + z_offset))
					{
						frame1[data_block_idx].distance_[ch].distance = 0;
						frame1[data_block_idx].distance_[ch].pulse_width = 0;
						frame1[data_block_idx + 1].distance_[ch].distance = 0;
						frame1[data_block_idx + 1].distance_[ch].pulse_width = 0;
						frame1[data_block_idx + 2].distance_[ch].distance = 0;
						frame1[data_block_idx + 2].distance_[ch].pulse_width = 0;
					}
					else if (f1tp_2->distance < distance_limit && f1tp_2->distance > 0 && (z_position2 >= z_limit + z_offset))
					{
						//						frame1[data_block_idx].distance_[ch].distance = 0;
						//						frame1[data_block_idx].distance_[ch].pulse_width = 0;
						//						frame1[data_block_idx+1].distance_[ch].distance = 0;
						//						frame1[data_block_idx+1].distance_[ch].pulse_width = 0;

						frame1[data_block_idx].distance_[ch] = frame1[data_block_idx + 2].distance_[ch];
						frame1[data_block_idx + 1].distance_[ch].distance = 0;
						frame1[data_block_idx + 1].distance_[ch].pulse_width = 0;
						frame1[data_block_idx + 2].distance_[ch].distance = 0;
						frame1[data_block_idx + 2].distance_[ch].pulse_width = 0;
					}
					else if (f1tp_1->distance < distance_limit && f1tp_1->distance > 0 && (z_position >= z_limit + z_offset))
					{

						//						frame1[data_block_idx].distance_[ch].distance = 0;
						//						frame1[data_block_idx].distance_[ch].pulse_width = 0;
						if (frame1[data_block_idx + 1].distance_[ch].distance != 0)
						{
							frame1[data_block_idx].distance_[ch] = frame1[data_block_idx + 1].distance_[ch];
						}
						if (frame1[data_block_idx + 2].distance_[ch].distance != 0)
						{
							frame1[data_block_idx + 1].distance_[ch] = frame1[data_block_idx + 2].distance_[ch];
						}

						frame1[data_block_idx + 2].distance_[ch].distance = 0;
						frame1[data_block_idx + 2].distance_[ch].pulse_width = 0;
					}
				}
				else if (closest_idx == 1)
				{

					frame1[data_block_idx].distance_[ch] = frame1[data_block_idx + 1].distance_[ch];
					frame1[data_block_idx + 1].distance_[ch] = frame1[data_block_idx + 2].distance_[ch];
					frame1[data_block_idx + 2].distance_[ch].distance = 0;
					frame1[data_block_idx + 2].distance_[ch].pulse_width = 0;
				}
				else if (closest_idx == 2)
				{
					frame1[data_block_idx].distance_[ch] = frame1[data_block_idx + 2].distance_[ch];
					frame1[data_block_idx + 1].distance_[ch].distance = 0;
					frame1[data_block_idx + 2].distance_[ch].distance = 0;
					frame1[data_block_idx + 1].distance_[ch].pulse_width = 0;
					frame1[data_block_idx + 2].distance_[ch].pulse_width = 0;
				}

				cf[data_block_idx].azimuth_ = frame1[data_block_idx].azimuth_ * 0.001;
				cf[data_block_idx].distance_[ch] = frame1[data_block_idx].distance_[ch].distance * DIV_256;
				cf[data_block_idx + 1].azimuth_ = frame1[data_block_idx + 1].azimuth_ * 0.001;
				cf[data_block_idx + 1].distance_[ch] = frame1[data_block_idx + 1].distance_[ch].distance * DIV_256;
				cf[data_block_idx + 2].azimuth_ = frame1[data_block_idx + 2].azimuth_ * 0.001;
				cf[data_block_idx + 2].distance_[ch] = frame1[data_block_idx + 2].distance_[ch].distance * DIV_256;
			}
		}
	}
}
void exhaust_filter_channel_clustering(RawDataBlock *frame, size_t minPts, float z_mean)
{

	//	cclabel = (uint8_t *) malloc(sizeof(uint8_t) * 73728);
	//	memset(cclabel, UNKNOWNCLUSTER, sizeof(uint8_t) * 73728);

	cclabel = (uint8_t *)malloc(sizeof(uint8_t) * 65664);
	memset(cclabel, UNKNOWNCLUSTER, sizeof(uint8_t) * 65664);

	//	uint8_t echo_max = (hz_val == 0)? 2:1;
	uint8_t echo_max = 2;

	//	for (int ch_idx = 0; ch_idx < 32; ch_idx++)
	//	for (int ch_idx = 0; ch_idx < 32; ch_idx++)
	//	{
	//		uint8_t isTop = ch_idx % 2;
	//		uint8_t ch = ch_idx >> 1;
	for (int ch = 0; ch < 16; ch++)
	{
		for (uint8_t isTop = 0; isTop < 2; isTop++)
		{

			Stack *firstS = (Stack *)malloc(sizeof(Stack));
			firstS->top = NULL;
			firstS->prevStack = NULL;
			firstS->StackNodeCnt = 0;

			StackRoot *ccsr = (StackRoot *)malloc(sizeof(StackRoot));
			ccsr->lastStack = firstS;
			ccsr->StackCnt = 1;
			for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
			{
				for (size_t echo_idx = 0; echo_idx < echo_max; echo_idx++)
				{
					int data_block_idx = ((oneChAllEchoPointSize)*isTop) + azimuth_idx + echo_idx;
					DataPoint *tp = &frame[data_block_idx].distance_[ch];
					if (tp->distance > distance_limit || tp->distance == 0)
						continue;

					if (ch < 8)
					{
						uint8_t ch_idx = 2 * ch + isTop;
						float z_position = (tp->distance / 256) * sin_elevation_forz_rad[ch_idx];

						//				if (z_position < z_mean+z_offset && (z_mean+z_offset+1.0 > z_position))
						if (z_position < z_mean + z_offset)
						{
							// test code
							//					tp->pulse_width = 255;
							continue;
						}
					}

					StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
					newNode->p = tp;
					newNode->prevStackNode = NULL;
					newNode->data_block_idx = data_block_idx;
					newNode->ch = ch;

					push_node(ccsr, newNode, 0);
				}
			}
			labeling_and_destroy_stack(ccsr, cclabel, minPts, frame);
			free(ccsr);
		}
	}

	// test code
	//	free(cclabel);
}
void exhaust_filter_azimuth_clustering(RawDataBlock *frame, size_t minPts, float z_mean)
{
	//	aclabel = (uint8_t*)malloc(sizeof(uint8_t)*73728);
	//	memset(aclabel,UNKNOWNCLUSTER,sizeof(uint8_t)*73728);

	aclabel = (uint8_t *)malloc(sizeof(uint8_t) * 65664);
	memset(aclabel, UNKNOWNCLUSTER, sizeof(uint8_t) * 65664);
	//	uint8_t echo_max = (hz_val == 0)? 2:1;
	uint8_t echo_max = 2;
	for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
	{
		Stack *firstS = (Stack *)malloc(sizeof(Stack));
		firstS->top = NULL;
		firstS->prevStack = NULL;
		firstS->StackNodeCnt = 0;

		StackRoot *acsr = (StackRoot *)malloc(sizeof(StackRoot));
		acsr->lastStack = firstS;
		acsr->StackCnt = 1;

		for (int ch_idx = 0; ch_idx < 32; ch_idx++)
		{
			uint8_t isTop = ch_idx % 2;
			uint8_t ch = ch_idx >> 1;
			for (size_t echo_idx = 0; echo_idx < echo_max; echo_idx++)
			{
				int data_block_idx = ((oneChAllEchoPointSize)*isTop) + azimuth_idx + echo_idx;
				DataPoint *tp = &frame[data_block_idx].distance_[ch];
				if (tp->distance > distance_limit || tp->distance == 0)
					continue;

				if (ch < 8)
				{
					float z_position = (tp->distance / 256) * sin_elevation_forz_rad[ch_idx];
					if (z_position < z_mean + z_offset)
					{
						continue;
					}
				}

				StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
				newNode->p = tp;
				newNode->prevStackNode = NULL;
				newNode->data_block_idx = data_block_idx;
				newNode->ch = ch;

				push_node(acsr, newNode, 1);
			}
		}
		//		labeling_and_destroy_stack(acsr,aclabel,minPts);
		labeling_and_destroy_stack(acsr, aclabel, minPts, frame);
		free(acsr);
	}
}
void exhaust_filter_remove_noise(RawDataBlock *frame, uint8_t remove_type)
{
	for (int label_idx = 0; label_idx < allPointSize; label_idx++)
	{
		if (label_idx % 3 == 2)
			continue;
		if (hz_val == 0)
		{
			if (cclabel[label_idx] == NOTCLUSTER && aclabel[label_idx] == NOTCLUSTER)
			{
				DataPoint *tmpDP = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
				if (tmpDP->distance < distance_limit)
				{
					//
					if (remove_type == 0)
					{
						tmpDP->distance = 0;
					}
					else if (remove_type == 1)
					{
						tmpDP->pulse_width = 255;
					}
				}
			}
			else
			{
				if (remove_type == 1)
				{
					DataPoint *tmpDP = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
					if (cclabel[label_idx] == CLUSTERED)
					{
						tmpDP->pulse_width = 30;
					}
					if (aclabel[label_idx] == CLUSTERED)
					{
						tmpDP->pulse_width = (tmpDP->pulse_width == 30) ? 200 : 150;
					}
				}
			}
		}
		else
		{
			if (cclabel[label_idx] == NOTCLUSTER)
			{
				DataPoint *tmpDP = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
				if (tmpDP->distance < distance_limit)
				{
					//
					if (remove_type == 0)
					{
						tmpDP->distance = 0;
					}
					else if (remove_type == 1)
					{
						tmpDP->pulse_width = 255;
					}
				}
			}
			else
			{
				if (remove_type == 1)
				{
					DataPoint *tmpDP = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
					if (cclabel[label_idx] == CLUSTERED)
					{
						//						tmpDP->pulse_width = (tmpDP->pulse_width == 30)? 200:150;
						tmpDP->pulse_width = 30;
					}
				}
			}
		}
	}
	if (aclabel != NULL)
		free(aclabel);
	if (cclabel != NULL)
		free(cclabel);
}
void vnoise_filter_test_ver0(RawDataBlock *frame, float z_mean, uint8_t ac_minPts, uint8_t cc_minPts)
{
	list_map_idx = 0;

	// 1. Echo2占쏙옙 占쏙옙占쌔쇽옙 클占쏙옙占쏙옙占싶몌옙 占쏙옙占쏙옙
	for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
	{
		// Echo 2 Stack Init
		Stack *Echo2FirstStack = (Stack *)malloc(sizeof(Stack));
		Echo2FirstStack->top = NULL;
		Echo2FirstStack->prevStack = NULL;
		Echo2FirstStack->StackNodeCnt = 0;

		StackRoot *Echo2StackRoot = (StackRoot *)malloc(sizeof(StackRoot));
		Echo2StackRoot->lastStack = Echo2FirstStack;
		Echo2StackRoot->StackCnt = 1;

		for (int ch_idx = 0; ch_idx < 32; ch_idx++)
		{
			uint8_t isTop = ch_idx % 2;
			uint8_t ch = ch_idx >> 1;

			int data_block_idx = ((oneChAllEchoPointSize)*isTop) + azimuth_idx; // only echo 1
			DataPoint *tp = &frame[data_block_idx].distance_[ch];

			// echo1 占신몌옙 占쏙옙
			if (tp->distance > distance_limit || tp->distance == 0)
				continue;

			// echo1占쏙옙 占쌕닥몌옙 占쏙옙占쏙옙占�
			if (ch < 8)
			{
				//				float z_position = (tp->distance / 256) * sin_elevation_forz_rad[ch_idx];
				float z_position = (tp->distance) * sin_elevation_forz_rad2[ch_idx];
				if (z_position < z_mean + z_offset)
					continue;
			}

			DataPoint *tp2 = &frame[data_block_idx + 1].distance_[ch];

			if (tp2->distance > distance_limit || tp2->distance == 0)
				continue;
			// echo 2占쏙옙 占쌕닥몌옙 占쏙옙占쏙옙占�
			if (ch < 8)
			{
				//				float z_position2 = (tp2->distance / 256) * sin_elevation_forz_rad[ch_idx];
				float z_position2 = (tp2->distance) * sin_elevation_forz_rad2[ch_idx];
				// echo 2占쏙옙 占쌕댐옙占쏙옙占쏙옙占쏙옙 echo1占쏙옙 echo2占쏙옙占쏙옙트占쏙옙 占쏙옙占쏙옙
				if (z_position2 < z_mean + z_offset)
				{
					frame[data_block_idx].distance_[ch] = frame[data_block_idx + 1].distance_[ch];
					frame[data_block_idx + 1].distance_[ch].distance = 0;
					frame[data_block_idx + 1].distance_[ch].pulse_width = 0;

					continue;
				}
			}

			StackNode *Echo2newNode = list_map[list_map_idx++];
			Echo2newNode->p = tp2;
			Echo2newNode->prevStackNode = NULL;
			Echo2newNode->data_block_idx = data_block_idx + 1;
			Echo2newNode->ch = ch;

			push_node_ver2_channel(Echo2StackRoot, Echo2newNode);
		}

		// Echo2StackRoot 처占쏙옙
		Stack *curStack = Echo2StackRoot->lastStack;
		while (curStack != NULL)
		{
			size_t curStackNodeCnt = curStack->StackNodeCnt;

			if (curStackNodeCnt >= ac_minPts) // echo2占쏙옙 占쏙옙占쌔쇽옙 클占쏙옙占쏙옙占싶몌옙占쏙옙 占실억옙占쌕뱄옙 echo1 = echo2, echo2 = 0
			{
				while (curStack->top != NULL)
				{
					StackNode *tmpNode = curStack->top;

					if (remove_method == 0)
					{
						frame[tmpNode->data_block_idx - 1].distance_[tmpNode->ch] = frame[tmpNode->data_block_idx].distance_[tmpNode->ch];
						frame[tmpNode->data_block_idx].distance_[tmpNode->ch].distance = 0;
						frame[tmpNode->data_block_idx].distance_[tmpNode->ch].pulse_width = 0;
					}
					else if (remove_method == 1)
					{
						frame[tmpNode->data_block_idx].distance_[tmpNode->ch].pulse_width = 255;
					}

					curStack->top = curStack->top->prevStackNode;

					//					free(tmpNode);
				}
			}
			else // 클占쏙옙占쏙옙占싶몌옙 占싫되억옙占쏙옙占쏙옙  echo2 = 0
			{
				while (curStack->top != NULL)
				{
					StackNode *tmpNode = curStack->top;

					curStack->top = curStack->top->prevStackNode;

					//					free(tmpNode);
				}
			}

			Stack *tmpStack = curStack;
			curStack = curStack->prevStack;
			free(tmpStack);
		}

		if (Echo2StackRoot != NULL)
			free(Echo2StackRoot);
	}
}
void vnoise_filter_test_ver2(RawDataBlock *frame, float z_mean, uint8_t ac_minPts, uint8_t cc_minPts)
{
	list_map_idx = 0;

	// 1. Echo2占쏙옙 占쏙옙占쌔쇽옙 클占쏙옙占쏙옙占싶몌옙 占쏙옙占쏙옙
	for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
	{
		// Echo 2 Stack Init
		Stack *Echo2FirstStack = (Stack *)malloc(sizeof(Stack));
		Echo2FirstStack->top = NULL;
		Echo2FirstStack->prevStack = NULL;
		Echo2FirstStack->StackNodeCnt = 0;

		StackRoot *Echo2StackRoot = (StackRoot *)malloc(sizeof(StackRoot));
		Echo2StackRoot->lastStack = Echo2FirstStack;
		Echo2StackRoot->StackCnt = 1;

		for (int ch_idx = 0; ch_idx < 32; ch_idx++)
		{
			uint8_t isTop = ch_idx % 2;
			uint8_t ch = ch_idx >> 1;

			int data_block_idx = ((oneChAllEchoPointSize)*isTop) + azimuth_idx; // only echo 1
			DataPoint *tp = &frame[data_block_idx].distance_[ch];

			// echo1 占신몌옙 占쏙옙
			if (tp->distance > distance_limit + 256 || tp->distance == 0)
				continue;

			// echo1占쏙옙 占쌕닥몌옙 占쏙옙占쏙옙占�
			if (ch < 8)
			{
				//				float z_position = (tp->distance / 256) * sin_elevation_forz_rad[ch_idx];
				float z_position = (tp->distance) * sin_elevation_forz_rad2[ch_idx];
				if (z_position < z_mean + z_offset)
					continue;
			}

			DataPoint *tp2 = &frame[data_block_idx + 1].distance_[ch];

			if (tp2->distance > distance_limit || tp2->distance == 0)
				continue;
			// echo 2占쏙옙 占쌕닥몌옙 占쏙옙占쏙옙占�
			if (ch < 8)
			{
				//				float z_position2 = (tp2->distance / 256) * sin_elevation_forz_rad[ch_idx];
				float z_position2 = (tp2->distance) * sin_elevation_forz_rad2[ch_idx];
				// echo 2占쏙옙 占쌕댐옙占쏙옙占쏙옙占쏙옙 echo1占쏙옙 echo2占쏙옙占쏙옙트占쏙옙 占쏙옙占쏙옙
				if (z_position2 < z_mean + z_offset)
				{
					frame[data_block_idx].distance_[ch] = frame[data_block_idx + 1].distance_[ch];
					frame[data_block_idx + 1].distance_[ch].distance = 0;
					frame[data_block_idx + 1].distance_[ch].pulse_width = 0;

					continue;
				}
			}

			StackNode *Echo2newNode = (StackNode *)malloc(sizeof(StackNode));
			Echo2newNode->p = tp2;
			Echo2newNode->prevStackNode = NULL;
			Echo2newNode->data_block_idx = data_block_idx + 1;
			Echo2newNode->ch = ch;

			push_node(Echo2StackRoot, Echo2newNode, 1);
		}

		// Echo2StackRoot 처占쏙옙
		Stack *curStack = Echo2StackRoot->lastStack;
		while (curStack != NULL)
		{
			size_t curStackNodeCnt = curStack->StackNodeCnt;

			if (curStackNodeCnt >= ac_minPts) // echo2占쏙옙 占쏙옙占쌔쇽옙 클占쏙옙占쏙옙占싶몌옙占쏙옙 占실억옙占쌕뱄옙 echo1 = echo2, echo2 = 0
			{
				while (curStack->top != NULL)
				{
					StackNode *tmpNode = curStack->top;

					if (remove_method == 0)
					{
						frame[tmpNode->data_block_idx - 1].distance_[tmpNode->ch] = frame[tmpNode->data_block_idx].distance_[tmpNode->ch];
						frame[tmpNode->data_block_idx].distance_[tmpNode->ch].distance = 0;
						frame[tmpNode->data_block_idx].distance_[tmpNode->ch].pulse_width = 0;
					}
					else if (remove_method == 1)
					{
						frame[tmpNode->data_block_idx].distance_[tmpNode->ch].pulse_width = 255;
					}

					curStack->top = curStack->top->prevStackNode;

					free(tmpNode);
				}
			}
			else // 클占쏙옙占쏙옙占싶몌옙 占싫되억옙占쏙옙占쏙옙  echo2 = 0
			{
				while (curStack->top != NULL)
				{
					StackNode *tmpNode = curStack->top;

					curStack->top = curStack->top->prevStackNode;

					free(tmpNode);
				}
			}

			Stack *tmpStack = curStack;
			curStack = curStack->prevStack;
			free(tmpStack);
		}

		if (Echo2StackRoot != NULL)
			free(Echo2StackRoot);
	}
}

void init_ver2()
{
	//	StackNode* list_map[2052 * 32];
	for (int32_t i = 0; i < 2052 * 32; i++)
	{
		list_map[i] = (StackNode *)malloc(sizeof(StackNode));
		stack_map[i] = (Stack *)malloc(sizeof(Stack));
	}

	list_map_idx = 0;
	stack_map_idx = 0;

	cclabel = (uint8_t *)malloc(sizeof(uint8_t) * 65664);
	aclabel = (uint8_t *)malloc(sizeof(uint8_t) * 65664);
}

void exhaust_filter_channel_clustering_ver2(RawDataBlock *frame, size_t minPts, float z_mean)
{
	//	cclabel = (uint8_t *) malloc(sizeof(uint8_t) * 65664);
	memset(cclabel, UNKNOWNCLUSTER, sizeof(uint8_t) * 65664);

	uint8_t echo_max = 2;

	list_map_idx = 0;
	stack_map_idx = 0;

	StackRoot *ccsr = (StackRoot *)malloc(sizeof(StackRoot));

	for (int ch_idx = 0; ch_idx < 32; ch_idx++)
	{
		uint8_t isTop = ch_idx % 2;
		uint8_t ch = ch_idx >> 1;

		//			Stack* firstS = (Stack*)malloc(sizeof(Stack));
		Stack *firstS = stack_map[stack_map_idx++];
		firstS->top = NULL;
		firstS->prevStack = NULL;
		firstS->StackNodeCnt = 0;

		ccsr->lastStack = firstS;
		ccsr->StackCnt = 1;

		for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
		{
			for (size_t echo_idx = 0; echo_idx < echo_max; echo_idx++)
			{
				int data_block_idx = ((oneChAllEchoPointSize)*isTop) + azimuth_idx + echo_idx;
				DataPoint *tp = &frame[data_block_idx].distance_[ch];
				if (tp->distance > distance_limit + 256 || tp->distance == 0)
					continue;

				if (ch < 8)
				{
					//						uint8_t ch_idx = 2*ch + isTop;
					//						float z_position = (tp->distance / 256) * sin_elevation_forz_rad[ch_idx];
					float z_position = (tp->distance) * sin_elevation_forz_rad2[ch_idx];
					if (z_position < z_mean + z_offset)
					{
						continue;
					}
				}

				StackNode *newNode = list_map[list_map_idx++];
				newNode->p = tp;
				newNode->prevStackNode = NULL;
				newNode->data_block_idx = data_block_idx;
				newNode->ch = ch;

				//					push_node_ver2(ccsr,newNode,0);

				push_node_ver2_channel(ccsr, newNode);
			}
		}
		labeling_and_destroy_stack_ver2(ccsr, cclabel, minPts, frame);
	}
	free(ccsr);
}
// void exhaust_filter_azimuth_clustering_ver2(RawDataBlock *frame, size_t minPts, float z_mean)
//{
//
//		memset(aclabel, UNKNOWNCLUSTER, sizeof(uint8_t) * allPointSize);
//		//	uint8_t echo_max = (hz_val == 0)? 2:1;
//		uint8_t echo_max = 2;
//
//		list_map_idx = 0;
//		stack_map_idx = 0;
//		StackRoot* acsr = (StackRoot*)malloc(sizeof(StackRoot));
//
//
//
//		for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
//		{
//			Stack* firstS = stack_map[stack_map_idx++];
//			firstS->top = NULL;
//			firstS->prevStack = NULL;
//			firstS->StackNodeCnt = 0;
//
//
//			acsr->lastStack = firstS;
//			acsr->StackCnt = 1;
//
//			uint8_t print_cnt = 0;
//
//			for (size_t ch = 0; ch < 16; ch++)
//			{
//				for (size_t isTop_a = 0; isTop_a < 2; isTop_a++)
//				{
//
//					size_t ch_idx = 2 * ch + isTop_a;
//
//
//					for (size_t echo_idx = 0; echo_idx < echo_max; echo_idx++)
//					{
//
//						int data_block_idx = ((oneChAllEchoPointSize)*isTop_a) + azimuth_idx + echo_idx;
////						FovDataPoint* tp = &fov_data->data()[data_block_idx].data_points_[ch];
//						DataPoint* tp = &frame[data_block_idx].distance_[ch];
//
//						if (tp->distance > distance_limit || tp->distance == 0)
//						{
//							continue;
//						}
//
//
//						float z_position = (tp->distance) * sin_elevation_forz_rad2[ch_idx];
//						if (ch < 8)
//						{
//
//
//							if (z_position < z_mean + z_offset)
//							{
//								continue;
//							}
//						}
//
//
//						StackNode* newNode = list_map[list_map_idx++];
//						newNode->p = tp;
//						newNode->prevStackNode = NULL;
//						newNode->data_block_idx = data_block_idx;
//						newNode->ch = ch;
//						newNode->ch_32 = (uint8_t)ch_idx;
//
//
//						push_node_azimuth(acsr, newNode);
//
//
//					}
//				}
//
//
//			}
//
//			labeling(acsr, aclabel, ac_minPts);
//
//
//
//
//
//		}
//		free(acsr);
//}
void exhaust_filter_azimuth_clustering_ver2(RawDataBlock *frame, size_t minPts, float z_mean)
{
	//	aclabel = (uint8_t*)malloc(sizeof(uint8_t)*73728);
	//	memset(aclabel,UNKNOWNCLUSTER,sizeof(uint8_t)*73728);

	//	aclabel = (uint8_t*)malloc(sizeof(uint8_t)*65664);
	memset(aclabel, UNKNOWNCLUSTER, sizeof(uint8_t) * 65664);
	//	uint8_t echo_max = (hz_val == 0)? 2:1;
	uint8_t echo_max = 2;

	list_map_idx = 0;
	stack_map_idx = 0;
	StackRoot *acsr = (StackRoot *)malloc(sizeof(StackRoot));

	for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
	{
		//		Stack* firstS = (Stack*)malloc(sizeof(Stack));
		Stack *firstS = stack_map[stack_map_idx++];
		firstS->top = NULL;
		firstS->prevStack = NULL;
		firstS->StackNodeCnt = 0;

		acsr->lastStack = firstS;
		acsr->StackCnt = 1;

		for (int ch_idx = 0; ch_idx < 32; ch_idx++)
		{
			uint8_t isTop = ch_idx % 2;
			uint8_t ch = ch_idx >> 1;
			for (size_t echo_idx = 0; echo_idx < echo_max; echo_idx++)
			{
				int data_block_idx = ((oneChAllEchoPointSize)*isTop) + azimuth_idx + echo_idx;
				DataPoint *tp = &frame[data_block_idx].distance_[ch];
				if (tp->distance > distance_limit + 256 || tp->distance == 0)
					continue;

				if (ch < 8)
				{
					//					float z_position = (tp->distance / 256) * sin_elevation_forz_rad[ch_idx];
					float z_position = (tp->distance) * sin_elevation_forz_rad2[ch_idx];
					if (z_position < z_mean + z_offset)
					{
						continue;
					}
				}

				StackNode *newNode = list_map[list_map_idx++];
				newNode->p = tp;
				newNode->prevStackNode = NULL;
				newNode->data_block_idx = data_block_idx;
				newNode->ch = ch;
				newNode->ch_32 = (uint8_t)ch_idx;

				//				push_node_ver2(acsr,newNode,1);

				push_node_ver2_azimuth(acsr, newNode);
			}
		}
		labeling_and_destroy_stack_ver2(acsr, aclabel, minPts, frame);
	}
	free(acsr);
}
void labeling_and_destroy_stack_ver2(StackRoot *sr, uint8_t *label, size_t minPts, RawDataBlock *frame)
{
	Stack *curStack = sr->lastStack;
	while (curStack != NULL)
	{
		size_t curStackNodeCnt = curStack->StackNodeCnt;
		if (curStackNodeCnt < minPts)
		{
			while (curStack->top != NULL)
			{
				StackNode *tmpNode = curStack->top;

				label[tmpNode->data_block_idx + tmpNode->ch * data_arr_size] = NOTCLUSTER;

				curStack->top = curStack->top->prevStackNode;
			}
		}
		else
		{
			while (curStack->top != NULL)
			{
				StackNode *tmpNode = curStack->top;

				label[tmpNode->data_block_idx + tmpNode->ch * data_arr_size] = CLUSTERED;

				curStack->top = curStack->top->prevStackNode;
			}
		}

		//		Stack* tmpStack = curStack;
		curStack = curStack->prevStack;
		//		free(tmpStack);
	}
}
void exhaust_filter_remove_noise_ver2(RawDataBlock *frame, uint8_t remove_type)
{
	for (int label_idx = 0; label_idx < allPointSize; label_idx++)
	{
		if (label_idx % 3 == 2)
			continue;
		if (hz_val == 0)
		{
			if (cclabel[label_idx] == NOTCLUSTER && aclabel[label_idx] == NOTCLUSTER)
			{
				DataPoint *tmpDP = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
				if (tmpDP->distance < distance_limit)
				{
					//
					if (remove_type == 0)
					{
						tmpDP->distance = 0;
					}
					else if (remove_type == 1)
					{
						tmpDP->pulse_width = 255;
					}
				}
			}
			else
			{
				if (remove_type == 1)
				{
					DataPoint *tmpDP = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
					if (cclabel[label_idx] == CLUSTERED)
					{
						tmpDP->pulse_width = 30;
					}
					if (aclabel[label_idx] == CLUSTERED)
					{
						tmpDP->pulse_width = (tmpDP->pulse_width == 30) ? 200 : 150;
					}
				}
			}
		}
		else
		{
			if (cclabel[label_idx] == NOTCLUSTER)
			{
				DataPoint *tmpDP = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
				if (tmpDP->distance < distance_limit)
				{
					//
					if (remove_type == 0)
					{
						tmpDP->distance = 0;
					}
					else if (remove_type == 1)
					{
						tmpDP->pulse_width = 255;
					}
				}
			}
			else
			{
				if (remove_type == 1)
				{
					DataPoint *tmpDP = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
					if (cclabel[label_idx] == CLUSTERED)
					{
						//						tmpDP->pulse_width = (tmpDP->pulse_width == 30)? 200:150;
						tmpDP->pulse_width = 30;
					}
				}
			}
		}
	}
	//	if(aclabel != NULL)	free(aclabel);
	//	if(cclabel != NULL) free(cclabel);
}
void push_node_ver2(StackRoot *sr, StackNode *sn, uint8_t root_type)
{
	uint16_t push_eps = (root_type == 0) ? cc_eps : ac_eps;

	//	uint16_t push_eps = cc_eps;

	//	if(root_type == 0)
	//	{
	//		push_eps = (sn->p->distance < distance_limit/2)?(cc_eps*0.7):(cc_eps);
	//	}
	//	else if(root_type == 1)
	//	{
	//		push_eps = (sn->p->distance < distance_limit/2)?(ac_eps*0.7):(ac_eps);
	//	}

	Stack *curStack = sr->lastStack;
	int cnt = 0;
	while (curStack != NULL)
	{
		if (curStack->top == NULL)
		{
			sn->prevStackNode = curStack->top;
			curStack->top = sn;
			curStack->StackNodeCnt++;
			break;
		}

		if ((abs((int)curStack->top->p->distance - (int)sn->p->distance) < push_eps))
		{
			if (root_type == 0 && (abs((int)sn->data_block_idx - (int)curStack->top->data_block_idx) <= 6))
			{
				sn->prevStackNode = curStack->top;
				curStack->top = sn;
				curStack->StackNodeCnt++;
				break;
			}
			else if (root_type == 1 && (abs((int)sn->ch - (int)curStack->top->ch) < 2))
			{
				sn->prevStackNode = curStack->top;
				curStack->top = sn;
				curStack->StackNodeCnt++;
				break;
			}
		}

		curStack = curStack->prevStack;
		cnt++;

		if(curStack == NULL || cnt > 5)
//		if (curStack == NULL)
		{
			//			Stack* newStack = (Stack*)malloc(sizeof(Stack));
			Stack *newStack = stack_map[stack_map_idx++];
			newStack->top = sn;
			newStack->prevStack = sr->lastStack;
			newStack->StackNodeCnt = 0;
			sr->lastStack = newStack;
			sr->StackCnt++;
			break;
		}
	}
}
inline void push_node_ver2_channel(StackRoot *sr, StackNode *sn)
{
	uint16_t push_eps = cc_eps;

	Stack *curStack = sr->lastStack;
	int cnt = 0;
	while (curStack != NULL)
	{
		if (curStack->top == NULL)
		{
			sn->prevStackNode = curStack->top;
			curStack->top = sn;
			curStack->StackNodeCnt++;
			break;
		}

		if ((abs((int)curStack->top->p->distance - (int)sn->p->distance) < push_eps))
		{
			if (abs((int)sn->data_block_idx - (int)curStack->top->data_block_idx) <= 6)
			{
				sn->prevStackNode = curStack->top;
				curStack->top = sn;
				curStack->StackNodeCnt++;
				break;
			}
		}

		curStack = curStack->prevStack;
		cnt++;
		if(curStack == NULL || cnt > lidar_state_.a2z_setting.far_em)
//		if (curStack == NULL)
		{
			//			Stack* newStack = (Stack*)malloc(sizeof(Stack));
			Stack *newStack = stack_map[stack_map_idx++];
			newStack->top = sn;
			newStack->prevStack = sr->lastStack;
			newStack->StackNodeCnt = 0;
			sr->lastStack = newStack;
			sr->StackCnt++;
			break;
		}
	}
}
inline void push_node_ver2_azimuth(StackRoot *sr, StackNode *sn)
{
	float tmp_val = ((sn->p->distance / 256.0) + 10.0) / 15.0;

	//	uint16_t push_eps = sn->p->distance* 0.05* tmp_val;

	uint16_t push_eps = sn->p->distance * (float)((ac_eps / 256.0)) * tmp_val;

	//	uint16_t push_eps = ac_eps;

	//	uint16_t push_eps = cc_eps;

	//	if(root_type == 0)
	//	{
	//		push_eps = (sn->p->distance < distance_limit/2)?(cc_eps*0.7):(cc_eps);
	//	}
	//	else if(root_type == 1)
	//	{
	//		push_eps = (sn->p->distance < distance_limit/2)?(ac_eps*0.7):(ac_eps);
	//	}

	Stack *curStack = sr->lastStack;
	int cnt = 0;
	while (curStack != NULL)
	{
		if (curStack->top == NULL)
		{
			sn->prevStackNode = curStack->top;
			curStack->top = sn;
			curStack->StackNodeCnt++;
			break;
		}

		if ((abs((int)curStack->top->p->distance - (int)sn->p->distance) < push_eps))
		{
			if (abs((int)sn->ch_32 - (int)curStack->top->ch_32) <= 2)
			{
				sn->prevStackNode = curStack->top;
				curStack->top = sn;
				curStack->StackNodeCnt++;
				break;
			}
		}

		curStack = curStack->prevStack;
		cnt++;
		if(curStack == NULL || cnt > lidar_state_.a2z_setting.far_em)
//		if (curStack == NULL)
		{
			//			Stack* newStack = (Stack*)malloc(sizeof(Stack));
			Stack *newStack = stack_map[stack_map_idx++];
			newStack->top = sn;
			newStack->prevStack = sr->lastStack;
			newStack->StackNodeCnt = 1;
			sr->lastStack = newStack;
			sr->StackCnt++;
			break;
		}
	}
}
// array

void init_ver3()
{
}
void exhaust_filter_channel_clustering_ver3(RawDataBlock *frame, size_t minPts, float z_mean)
{
	//	cclabel = (uint8_t *) malloc(sizeof(uint8_t) * 65664);
	//	memset(cclabel, UNKNOWNCLUSTER, sizeof(uint8_t) * 65664);
	//
	//	uint8_t echo_max = 2;
	//
	//	size_t cluster_idx = 0; // 0~2052...
	//	memset(my_array_ch, UNKNOWNCLUSTER, sizeof(size_t) *2052*2052);
	//
	//	size_t cluster_top_idx[2052] = {0,};
	//
	//	for (int ch = 0; ch < 16; ch++)
	//	{
	//		for(uint8_t isTop = 0; isTop < 2; isTop ++)
	//		{
	//			for (int32_t azimuth_idx = 0; azimuth_idx < oneChAllEchoPointSize; azimuth_idx += 3)
	//			{
	//				for (size_t echo_idx = 0; echo_idx < echo_max; echo_idx++)
	//				{
	//					int data_block_idx = ((oneChAllEchoPointSize)*isTop) + azimuth_idx + echo_idx;
	//					DataPoint *tp = &frame[data_block_idx].distance_[ch];
	//					if (tp->distance > distance_limit || tp->distance == 0)	continue;
	//
	//
	//					if(ch <8)
	//					{
	//						uint8_t ch_idx = 2*ch + isTop;
	//						float z_position = (tp->distance / 256) * sin_elevation_forz_rad[ch_idx];
	//
	//						if (z_position < z_mean+z_offset)
	//						{
	//							continue;
	//						}
	//					}
	//
	//
	//
	//////					StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
	////					StackNode* newNode = list_map[list_map_idx++];
	////					newNode->p = tp;
	////					newNode->prevStackNode = NULL;
	////					newNode->data_block_idx = data_block_idx;
	////					newNode->ch = ch;
	//
	//					size_t cur_top_idx = cluster_top_idx[cluster_idx];
	//					if( cur_top_idx == 0)
	//					{
	//						my_array_ch[cluster_idx][cur_top_idx] = azimuth_idx;
	//					}
	//					else{
	//
	//					}
	//
	////					push_node(ccsr,newNode,0);
	//				}
	//			}
	////			labeling_and_destroy_stack_ver2(ccsr,cclabel,minPts,frame);
	//
	//		}
	//	}
}
void exhaust_filter_azimuth_clustering_ver3(RawDataBlock *frame, size_t minPts, float z_mean)
{
	return;
}

void preprocessing2(RawDataBlock *frame, uint8_t ac_minpts2, uint8_t cc_minpts2, float z_mean)
{


	exhaust_filter_channel_clustering_ver2(frame, cc_minpts2, z_mean);


	exhaust_filter_azimuth_clustering_ver2(frame, ac_minpts2, z_mean);

	for (int label_idx = 0; label_idx < allPointSize; label_idx++)
	{
		if (label_idx % 3 != 1)
			continue;
		if (cclabel[label_idx] == CLUSTERED && aclabel[label_idx] == CLUSTERED)
		{
			DataPoint *tmpDP2 = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
			if (tmpDP2->distance == 0)
				continue;
			DataPoint *tmpDP1 = &frame[(label_idx % data_arr_size) - 1].distance_[label_idx / data_arr_size];

			//			tmpDP2->pulse_width = 255;

			tmpDP1->distance = tmpDP2->distance;
			tmpDP1->pulse_width = tmpDP2->pulse_width;
			//			tmpDP1->pulse_width = 255;
			tmpDP2->distance = 0;
			tmpDP2->pulse_width = 0;
		}
	}
}
void run_0617(RawDataBlock *frame, uint8_t ac_minpts2, uint8_t cc_minpts2, float z_mean)
{
	exhaust_filter_channel_clustering_ver2(frame, cc_minpts2, z_mean);
	exhaust_filter_azimuth_clustering_ver2(frame, ac_minpts2, z_mean);

	for (int label_idx = 0; label_idx < allPointSize; label_idx++)
	{
		if (label_idx % 3 != 1)
			continue;
		if (cclabel[label_idx] == CLUSTERED && aclabel[label_idx] == CLUSTERED)
		{
			DataPoint *tmpDP2 = &frame[label_idx % data_arr_size].distance_[label_idx / data_arr_size];
			if (tmpDP2->distance == 0)
				continue;
			DataPoint *tmpDP1 = &frame[(label_idx % data_arr_size) - 1].distance_[label_idx / data_arr_size];


			tmpDP1->distance = tmpDP2->distance;
			tmpDP1->pulse_width = tmpDP2->pulse_width;

			tmpDP2->distance = 0;
			tmpDP2->pulse_width = 0;

			cclabel[label_idx] = UNKNOWNCLUSTER;
			aclabel[label_idx] = UNKNOWNCLUSTER;

			cclabel[label_idx-1] = CLUSTERED;
			aclabel[label_idx-1] = CLUSTERED;
		}
	}


}
