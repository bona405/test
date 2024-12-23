/*
 * flash.h
 *
 *  Created on: 2021. 11. 23.
 *      Author: ProDesk
 */

#ifndef SRC_FLASH_H_
#define SRC_FLASH_H_

#define LIDAR_STATE_ADDR	0xC01000
#define LIDAR_STATE_ADDR_CH	0xC02000
#define LIDAR_STATE_ADDR_CH2	0xC03000
#define LOG_INFO_ADDR 		0xC10000
#define LOG_ADDR LOG_INFO_ADDR + SUBSECTOR_4K_SIZE

#define LOG_CHANGE_IDX  	0xC20000
//#define LOG_CHANGE_ADDR 	0xC20040
#define LOG_CHANGE_ADDR 	0xC21000
#define LOG_CHANGE_SIZE 	0x20

#define LIDAR_KEY_ADDR		0xC03000
#define LIDAR_IV_ADDR		0xC04000

#define DARK_AREA_ADDR		0xC24000
//#define DARK_AREA_SIZE		SUBSECTOR_32K_SIZE
#define DARK_AREA_SIZE		SUBSECTOR_4K_SIZE

#define LIDAR_KEY_SIZE		0x20
#define LIDAR_IV_SIZE		0x10

#define LIDAR_STATE_SIZE	SUBSECTOR_4K_SIZE
#define LIDAR_STATE_SIZE_CH	SUBSECTOR_4K_SIZE
#define LIDAR_STATE_SIZE_CH2	SUBSECTOR_4K_SIZE


#define AZI_OFFSET_ADDR		0xD00000
#define AZI_OFFSET_SIZE		0x060000	//384kb


#define TEMPER_OFFSET_ADDR	0xF00000
#define TEMPER_OFFSET_SIZE	0x060000	//To Do

//#define DARK_AREA_ADDR		0xC24000
//#define DARK_AREA_SIZE		SUBSECTOR_32K_SIZE


#define LIDAR_KEY_SIZE		0x20
#define LIDAR_IV_SIZE		0x10

#define SMT_ISINIT_ADDR		0x00C1900
#define SMT_ISINIT_SIZE		0x1
#define SMT_ISINIT_FLG		0x66

#define SECTOR_SIZE		0x10000
#define SUBSECTOR_4K_SIZE 	0x1000
#define SUBSECTOR_32K_SIZE 	0x8000
#define NUM_SECTORS		0x100
#define NUM_PAGES		0x10000
#define PAGE_SIZE		256

/* Number of flash pages to be written.*/
#define PAGE_COUNT		16

typedef struct LogInfo
{
	unsigned char LogLine[32];
}LogInfo;

u8 lidar_state_buffer_[LIDAR_STATE_SIZE];
u8 lidar_state_buffer_Ch_[LIDAR_STATE_SIZE_CH];
u8 dark_area_state_buffer_[DARK_AREA_SIZE];
u8 azi_offset_buffer_[AZI_OFFSET_SIZE];
int QspiFlashWrite(char *buf, int len);
int InitLinearQspiFlash();
int LinearQspiFlashErase(int len);
int LinearQspiFlashWrite(char *buf, int len);
int LinearQspiFlashWriteForGolden(char *buf, int len);
int ReadFlash(u8* read_buffer, size_t size);
int ReadLidarInfo(u8* read_buffer, size_t size);
int ReadLogInfo(u8* read_buffer, size_t size);
int ReadLidarInfo_Ch(u8* read_buffer, size_t size);
int ReadLidarInfo_Ch2(u8* read_buffer, size_t size);
int LinearQspiFlashWriteForMultiBoot(char *buf, int len);
void EraseBootImgFlash();
void EraseGoldenImgFlash();
void EraseLidarInfo();
void EraseLidarInfo_Ch();
void EraseLidarInfo_Ch2();

int LinearQspiFlashEraseForMultiBoot(int len);
int LinearQspiFlashWriteForLidarInfo(char *buf, int len);
int LinearQspiFlashWriteForLidarInfo_Ch(char *buf, int len);
int LinearQspiFlashWriteForLidarInfo_Ch2(char *buf, int len);
int LinearQspiFlashEraseWriteForKey(unsigned char *buf);
int LinearQspiFlashEraseWriteForIV(unsigned char *buf);
int LinearQspiFlashWriteForLidarChangeInfo(char *buf);
//int LinearQspiFlashReadForLidarChangeInfo(char *buf);
int LinearQspiFlashReadForLidarChangeInfo(char *buf, int readidx);
int ReadKey(u8* read_buffer);
int ReadIV(u8* read_buffer);
int ReadFlashForGoldenImg(u8* read_buffer, size_t size);
void ResetFlashMemory();

void EraseDarkAreaInfo();
int LinearQspiFlashWriteForDarkAreaInfo(char *buf, int len);
int ReadDarkAreaInfo(u8* read_buffer, size_t size);
int ReadAzioffset(u8* read_buffer, size_t size);
int LinearQspiFlashWriteForAzioffset(char *buf, int len);
int LinearQspiFlashWriteForTemperoffset(char *buf, int len);
void EraseAzioffset();
void EraseTemperoffset();
#endif /* SRC_FLASH_H_ */
