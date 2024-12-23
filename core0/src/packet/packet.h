/*
 * packet.h
 *
 *  Created on: 2022. 3. 14.
 *      Author: ProDesk
 */

#ifndef SRC_PACKET_PACKET_H_
#define SRC_PACKET_PACKET_H_

#define POINT_CLOUD_DATA_TYPE 0xA5B3C201
#define END_OF_DATA_STREAM 0xA5B3C2AA
#define ERR_OF_DATA_STREAM 0xA5B3C2CC

#include "../definitions/definitions.h"

/* Receive Packet */
#pragma pack(push, 1)

typedef struct{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint32_t nanosecond;
}timestamp;							// 10byte

typedef struct {
	uint16_t tof;					// 2 byte
	uint8_t intensity;				// 1 byte
} Channel; 							// total : 3 byte

typedef struct {
	uint16_t flag;					// 2 byte
	int azimuth;					// 4 byte
	Channel channel_data[16];		// 3 * 16 = 48 byte
} DataBlock;						// Total : 54 byte

typedef struct {
	int packet_id; 					// 4 byte
	uint8_t top_bottom_side; 		// 1 byte
	uint32_t data_type_; 			// 4 byte
#if defined(G32_ES_TEST)
	uint32_t motorRPM;					// 4 byte
	uint32_t VoltageData;				// 4 byte
	uint32_t Voltagefraction;			// 4 byte
	uint8_t frame_rate;				// 7 byte
	uint8_t vertical_angle;				// 7 byte
	char reserved[5];				// 7 byte
#else
	uint8_t frame_rate;				// 7 byte
	uint8_t vertical_angle;				// 7 byte
	// 240509
	int32_t z_mean;				// 7 byte
	timestamp pps_time;				// 10 byte
	char reserved[3];				// 7 byte
//	char reserved[13];				// 7 byte
//	char reserved[17];				// 7 byte
#endif
} DataPacketHeader;					// total : 28 byte

typedef struct {
	DataPacketHeader header;		// 28 byte
	DataBlock data_block[24];		// 1296 byte
	uint32_t time;					// 4 byte
	uint16_t factory;				// 2 byte
} DataPacket;						// total : 1330 byte

typedef struct {
	char prefix[4];
	uint32_t body_length;
} CommandPacketHeader;

typedef struct {
	uint8_t instruction;
	int channel;
	int value;
} CommandPacketBody;

typedef struct {
	uint8_t instruction;
	int azimuth;
	int ch;
	int offset;
} AziPacketBody;

//typedef struct {
//	uint8_t instruction;
//	int encoder_count;
//	int channel;
//	int offset;
//} CommandPacketBody_azical;

typedef struct {
	CommandPacketHeader cmd_header;
	CommandPacketBody cmd_body;
} CommandPacket;

typedef struct {
	CommandPacketHeader cmd_header;
	AziPacketBody cmd_body;
} AziPacket;


#pragma pack(pop)

/* used as indices into kLabel[] */
enum {
	KCONV_UNIT, KCONV_KILO, KCONV_MEGA, KCONV_GIGA,
};

#endif /* SRC_PACKET_PACKET_H_ */
