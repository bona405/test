/*
 * log_controller.c
 *
 *  Created on: 2022. 12. 2.
 *      Author: Wonju
 */

#include "logger.h"
#include "../flash/flash.h"
#include "xtime_l.h"

static eLogLevel log_level = kTrace;
static eLogType log_type = kConsole;

volatile static LoggerInfo logger_info_;

XStatus init_logger()
{
	int Status = InitLinearQspiFlash();
	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
	}

	Status = ReadLidarInfo(&logger_info_, LIDAR_STATE_SIZE);

	if(logger_info_.current_log_addr_offset > 0x2EDFFF)
		logger_info_.current_log_addr_offset = 0x00;

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Read Failed\r\n");

		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

void set_log_level(eLogLevel level)
{
	log_level = level;
}

void set_log_type(eLogLevel type)
{
	log_type = type;
}

void log_trace(const char8 *ctrl1, ...)
{

	if(log_type == kConsole)
	{
		if(log_level <= kTrace)
			log_printf(ctrl1);
	}
	else
	{
//		xil_printf(ctrl1);
		char* log_bytes;
		log_bytes = convert_ctrls_to_bytes(ctrl1);

		int Status = InitLinearQspiFlash();
		if(Status != XST_SUCCESS)
		{
			xil_printf("Flash Init Failed\r\n");
		}

		AlFlashRead((u8*)&logger_info_, LOG_INFO_ADDR, sizeof(logger_info_));

		if(logger_info_.current_log_addr_offset > 0x2EDFFF)
			logger_info_.current_log_addr_offset = 0x00;

		if(logger_info_.current_log_addr_offset % SUBSECTOR_4K_SIZE == 0)
			EraseFlashMemory(LOG_ADDR + logger_info_.current_log_addr_offset, SUBSECTOR_4K_SIZE);

		XTime current_time;
		XTime_GetTime(&current_time);
		LogMessageInfo log_message_info = {0, };
		log_message_info.time_stamp = current_time / COUNTS_PER_SECOND;
		memcpy(log_message_info.log_message, log_bytes, sizeof(log_message_info.log_message));
		AlFlashWrite(&log_message_info, LOG_ADDR + logger_info_.current_log_addr_offset, LOG_ADDR_INTERVAL);

		logger_info_.current_log_addr_offset +=LOG_ADDR_INTERVAL;
		EraseFlashMemory(LOG_INFO_ADDR, SUBSECTOR_4K_SIZE);
		AlFlashWrite(&logger_info_, LOG_INFO_ADDR, sizeof(logger_info_));


//		LoggerInfo test_logger_info;
//		AlFlashRead((u8*)&test_logger_info, LOG_INFO_ADDR, sizeof(test_logger_info));
//
//		strcat(read_buffer1, "TESTABCD");
//
//		Status = InitLinearQspiFlash();
//		if(Status != XST_SUCCESS)
//		{
//			xil_printf("Flash Init Failed\r\n");
//		}
//
//		LogFlashWrite(read_buffer1, LOG_ADDR, strlen(read_buffer1));
//		ReadFlashMemory(read_buffer1, LOG_ADDR, strlen(read_buffer1));
//
//		char read_buffer2[0x500];
//		ReadFlashMemory(read_buffer2, LOG_ADDR, 0x400);
//		xil_printf("test");
		// add log_level_flash??
		// WriteLogToFlash(ctrl1);

	}
}

void log_debug(const char8 *ctrl1, ...)
{
	if(log_type == kConsole)
	{
		if(log_level <= kDebug)
			log_printf(ctrl1);
	}
	else
	{
		// add log_level_flash??
		// WriteLogToFlash(ctrl1);
	}
}

void log_info(const char8 *ctrl1, ...)
{
	if(log_type == kConsole)
	{
		if(log_level <= kInfo)
			log_printf(ctrl1);
	}
	else
	{
		// add log_level_flash??
		// WriteLogToFlash(ctrl1);
	}
}

void log_warn(const char8 *ctrl1, ...)
{
	if(log_type == kConsole)
	{
		if(log_level <= kWarn)
			log_printf(ctrl1);
	}
	else
	{
		// add log_level_flash??
		// WriteLogToFlash(ctrl1);
	}
}

void log_error(const char8 *ctrl1, ...)
{
	if(log_type == kConsole)
	{
		if(log_level <= kError)
			log_printf(ctrl1);
	}
	else
	{
		// add log_level_flash??
		// WriteLogToFlash(ctrl1);
	}
}

void log_critical(const char8 *ctrl1, ...)
{
	if(log_type == kConsole)
	{
		if(log_level <= kCritical)
			log_printf(ctrl1);
	}
	else
	{
		// add log_level_flash??
		// WriteLogToFlash(ctrl1);
	}
}

void log_off(const char8 *ctrl1, ...)
{
	if(log_type == kConsole)
	{
		if(log_level <= kOff)
			log_printf(ctrl1);
	}
	else
	{
		// add log_level_flash??
		// WriteLogToFlash(ctrl1);
	}
}

void read_logs(char* result, size_t num)
{
	XStatus Status = InitLinearQspiFlash();

	if(Status != XST_SUCCESS)
	{
		xil_printf("Flash Init Failed\r\n");
	}

	LoggerInfo logger_info;

	AlFlashRead((u8*)&logger_info, LOG_INFO_ADDR, sizeof(logger_info));

	strncat(result, "{", strlen("{"));

	for(size_t i=0 ; i<num ; i++)
	{
		if(logger_info_.current_log_addr_offset > 0x2EDFFF)
			logger_info_.current_log_addr_offset = 0x00;
		if(logger_info_.current_log_addr_offset < 0)
			logger_info_.current_log_addr_offset = 0x2EDF00;
		LogMessageInfo log_message_info;
		AlFlashRead(&log_message_info, LOG_ADDR + logger_info.current_log_addr_offset, LOG_ADDR_INTERVAL);
		if(log_message_info.log_message[0] == 0xff)
		{
			logger_info.current_log_addr_offset -= LOG_ADDR_INTERVAL;
			continue;
		}
		char tmp[10] = {0,};
		strncat(result, "\"", strlen("\""));
		strncat(result, "a", strlen("a"));
		strncat(result, itoa(i, tmp, 10), strlen(itoa(i, tmp, 10)));
		strncat(result, "\"", strlen("\""));
		strncat(result, ":", strlen(":"));
		strncat(result, "\"", strlen("\""));
		strncat(result, log_message_info.log_message, strlen(log_message_info.log_message));
		strncat(result, "\"", strlen("\""));
		if(i < num-1)
			strncat(result, ",", strlen(","));
	}
	strncat(result, "}", strlen("}"));
}

typedef struct params_s {
    s32 len;
    s32 num1;
    s32 num2;
    char8 pad_character;
    s32 do_padding;
    s32 left_flag;
    s32 unsigned_flag;
} params_t;

static s32 getnum( charptr* linep)
{
    s32 n;
    s32 ResultIsDigit = 0;
    charptr cptr;
    n = 0;
    cptr = *linep;
	if(cptr != NULL){
		ResultIsDigit = isdigit(((s32)*cptr));
	}
    while (ResultIsDigit != 0) {
		if(cptr != NULL){
			n = ((n*10) + (((s32)*cptr) - (s32)'0'));
			cptr += 1;
			if(cptr != NULL){
				ResultIsDigit = isdigit(((s32)*cptr));
			}
		}
		ResultIsDigit = isdigit(((s32)*cptr));
	}
    *linep = ((charptr )(cptr));
    return(n);
}

static void padding( const s32 l_flag, const struct params_s *par)
{
    s32 i;

    if ((par->do_padding != 0) && (l_flag != 0) && (par->len < par->num1)) {
		i=(par->len);
        for (; i<(par->num1); i++) {
#ifdef STDOUT_BASEADDRESS
            outbyte( par->pad_character);
#endif
		}
    }
}

static void outnum(char* bytes, const s32 n, const s32 base, struct params_s *par)
{
    s32 negative;
	s32 i;
    char8 outbuf[32];
    const char8 digits[] = "0123456789ABCDEF";
    u32 num;
    for(i = 0; i<32; i++) {
	outbuf[i] = '0';
    }

    /* Check if number is negative                   */
    if ((par->unsigned_flag == 0) && (base == 10) && (n < 0L)) {
        negative = 1;
		num =(-(n));
    }
    else{
        num = n;
        negative = 0;
    }

    /* Build number (backwards) in outbuf            */
    i = 0;
    do {
		outbuf[i] = digits[(num % base)];
		i++;
		num /= base;
    } while (num > 0);

    if (negative != 0) {
		outbuf[i] = '-';
		i++;
	}

    outbuf[i] = '\0';
    i--;

    /* Move the converted number to the buffer and   */
    /* add in the padding where needed.              */
    par->len = (s32)strlen(outbuf);
    padding( !(par->left_flag), par);
    while (&outbuf[i] >= outbuf) {
#ifdef STDOUT_BASEADDRESS
//	outbyte( outbuf[i] );
//    	strncat(bytes, outbuf[i], 1);
    	*bytes = outbuf[i];
    	bytes++;
#endif
		i--;
}
    padding( par->left_flag, par);
}

static void outs(char* bytes, const charptr lp, struct params_s *par)
{
    charptr LocalPtr;
	LocalPtr = lp;
    /* pad on left if needed                         */
	if(LocalPtr != NULL) {
		par->len = (s32)strlen( LocalPtr);
	}
    padding( !(par->left_flag), par);

    /* Move string to the buffer                     */
    while (((*LocalPtr) != (char8)0) && ((par->num2) != 0)) {
		(par->num2)--;
#ifdef STDOUT_BASEADDRESS
//        outbyte(*LocalPtr);
//		strncat(bytes, *LocalPtr, 1);
		*bytes = *LocalPtr;
		bytes++;
#endif
		LocalPtr += 1;
}

    /* Pad on right if needed                        */
    /* CR 439175 - elided next stmt. Seemed bogus.   */
    /* par->len = strlen( lp)                      */
    padding( par->left_flag, par);
}

char log_buffer[LOG_ADDR_INTERVAL * 2];

char* convert_ctrls_to_bytes(const char8 *ctrl1, ...)
{
	memset(log_buffer, 0x00, sizeof(log_buffer));
	char* bytes = log_buffer;
	s32 Check;
#if defined (__aarch64__) || defined (__arch64__)
	s32 long_flag;
#endif
	s32 dot_flag;

	params_t par;

	char8 ch;
	va_list argp;
	char8 *ctrl = (char8 *)ctrl1;

	va_start(argp, ctrl1);

	while((ctrl != NULL) && (*ctrl != (char8)0))
	{

		/* move format string chars to buffer until a  */
		/* format control is found.                    */
		if(*ctrl != '%')
		{
#ifdef STDOUT_BASEADDRESS
//			outbyte(*ctrl);
			*bytes = *ctrl;
			bytes++;
//			strncat(bytes, *ctrl, 1);
#endif
			ctrl += 1;
			continue;
		}

		/* initialize all the flags for this format.   */
		dot_flag = 0;
#if defined (__aarch64__) || defined (__arch64__)
		long_flag = 0;
#endif
		par.unsigned_flag = 0;
		par.left_flag = 0;
		par.do_padding = 0;
		par.pad_character = ' ';
		par.num2 = 32767;
		par.num1 = 0;
		par.len = 0;

		try_next : if(ctrl != NULL)
		{
			ctrl += 1;
		}
		if(ctrl != NULL)
		{
			ch = *ctrl;
		}
		else
		{
			ch = *ctrl;
		}

		if(isdigit((s32)ch) != 0)
		{
			if(dot_flag != 0)
			{
				par.num2 = getnum(&ctrl);
			}
			else
			{
				if(ch == '0')
				{
					par.pad_character = '0';
				}
				if(ctrl != NULL)
				{
					par.num1 = getnum(&ctrl);
				}
				par.do_padding = 1;
			}
			if(ctrl != NULL)
			{
				ctrl -= 1;
			}
			goto try_next;
		}

		char num_result[15] = {0, };
		char string_result[0x100] = {0, };

		switch(tolower((s32 )ch))
		{
		case '%' :
#ifdef STDOUT_BASEADDRESS
//			outbyte('%');
//			strncat(bytes, '%', 1);
			*bytes = '%';
			bytes++;
#endif
			Check = 1;
			break;

		case '-' :
			par.left_flag = 1;
			Check = 0;
			break;

		case '.' :
			dot_flag = 1;
			Check = 0;
			break;

		case 'l' :
#if defined (__aarch64__) || defined (__arch64__)
			long_flag = 1;
#endif
			Check = 0;
			break;

		case 'u' :
			par.unsigned_flag = 1;
			/* fall through */
		case 'i' :
		case 'd' :
#if defined (__aarch64__) || defined (__arch64__)
			if (long_flag != 0)
			{
				outnum1((s64)va_arg(argp, s64), 10L, &par);
			}
			else
			{
				outnum( va_arg(argp, s32), 10L, &par);
			}
#else
			outnum(num_result, va_arg(argp, s32), 10L, &par);

			for(size_t i=0 ; i<strlen(num_result) ; i++)
			{
				*bytes = num_result[i];
				bytes++;
			}

//			strncat(bytes, num_result, strlen(num_result));
#endif
			Check = 1;
			break;
		case 'p' :
#if defined (__aarch64__) || defined (__arch64__)
			par.unsigned_flag = 1;
			outnum1((s64)va_arg(argp, s64), 16L, &par);
			Check = 1;
			break;
#endif
		case 'X' :
		case 'x' :
			par.unsigned_flag = 1;
#if defined (__aarch64__) || defined (__arch64__)
			if (long_flag != 0)
			{
				outnum1((s64)va_arg(argp, s64), 16L, &par);
			}
			else
			{
				outnum((s32)va_arg(argp, s32), 16L, &par);
			}
#else
//			outnum((s32)va_arg(argp, s32), 16L, &par);
			outnum(num_result, va_arg(argp, s32), 16L, &par);

			for(size_t i=0 ; i<strlen(num_result) ; i++)
			{
				*bytes = num_result[i];
				bytes++;
			}

//			strncat(bytes, num_result, strlen(num_result));
#endif
			Check = 1;
			break;

		case 's' :
			outs(string_result, va_arg(argp, char *), &par);

			for(size_t i=0 ; i<strlen(string_result) ; i++)
			{
				*bytes = string_result[i];
				bytes++;
			}

//			strncat(bytes, string_result, strlen(string_result));
			Check = 1;
			break;

		case 'c' :
#ifdef STDOUT_BASEADDRESS
//			outbyte(va_arg(argp, s32));
//			strncat(bytes, va_arg(argp, s32), 1);
			*bytes = va_arg(argp, s32);
			bytes++;
#endif
			Check = 1;
			break;

		case '\\' :
			switch(*ctrl)
			{
			case 'a' :
#ifdef STDOUT_BASEADDRESS
//				outbyte(((char8)0x07));
//				strncat(bytes, ((char8)0x07), 1);
				*bytes = (char8)0x07;
				bytes++;
#endif
				break;
			case 'h' :
#ifdef STDOUT_BASEADDRESS
//				outbyte(((char8)0x08));
//				strncat(bytes, ((char8)0x08), 1);
				*bytes = (char8)0x08;
				bytes++;
#endif
				break;
			case 'r' :
#ifdef STDOUT_BASEADDRESS
//				outbyte(((char8)0x0D));
//				strncat(bytes, ((char8)0x0D), 1);
				*bytes = (char8)0x0D;
				bytes++;
#endif
				break;
			case 'n' :
#ifdef STDOUT_BASEADDRESS
//				outbyte(((char8)0x0D));
//				strncat(bytes, ((char8)0x0D), 1);
				*bytes = (char8)0x0D;
				bytes++;
//				outbyte(((char8)0x0A));
//				strncat(bytes, ((char8)0x0A), 1);
				*bytes = (char8)0x0A;
				bytes++;
#endif
				break;
			default :
#ifdef STDOUT_BASEADDRESS
//				outbyte(*ctrl);
//				strncat(bytes, *ctrl, 1);
				*bytes = *ctrl;
				bytes++;
#endif
				break;
			}
			ctrl += 1;
			Check = 0;
			break;

		default :
			Check = 1;
			break;
		}
		if(Check == 1)
		{
			if(ctrl != NULL)
			{
				ctrl += 1;
			}
			continue;
		}
		goto try_next;
	}
	va_end(argp);

	if(strlen(log_buffer) >= LOG_ADDR_INTERVAL - 8) // 8 == timestamp
		log_buffer[LOG_ADDR_INTERVAL - 8 - 1] = 0;
//	else
//		for(size_t i=strlen(log_buffer) ; i<LOG_ADDR_INTERVAL ; i++)
//			log_buffer[i] = 0;

	return (char*)log_buffer;
}
