#ifndef __LOG__
#define __LOG__
#include "conf.h"
#include "locafx.h"
enum LOG_LEVER
{
	LOG_TRACE,
	LOG_DEBUG,
	LOG_NORMAL,
	LOG_ERROR
};
int httplog(int level, const char* file, int line, const char* func, const char* format, ...)  __attribute__((format(printf, 5, 6)));
#define LOG(level,format,args...) httplog(level,__FILE__,__LINE__,__FUNCTION__,format,##args);
#endif