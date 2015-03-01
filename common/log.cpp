#include "../include/log.h"
int httplog(int level, const char* file, int line, const char* func, const char* format, ...)
{
	//TODO:重入问题

	static char _buf[LOG_MSG_BUF];
	static const char *logName[] = { "TRACE", "DEBUG", "NORMAL", "ERROR" };
	//错误处理
	static char errno_msg[256];
	char *errmsg = NULL;

	if (level<0 || level>3)
		return -1;
	if (level == 1)
		return 0;
	ofstream out;
	out.open("./tmp/log/httpsvr.log",ios_base::out|ios_base::app);

	va_list args;
	va_start(args, format);
	vsnprintf(_buf, LOG_MSG_BUF, format, args);
	va_end(args);

	struct timeval _now;
	gettimeofday(&_now, NULL);
	struct tm _tm;
	localtime_r(&_now.tv_sec, &_tm);

	out << "[" << logName[level] << "]"
		<< "[" << getpid() << "]"
		<< "["
		<< setw(4) << _tm.tm_year + 1900 << "-"
		<< setfill('0') << setw(2) << _tm.tm_mon + 1 << "-"
		<< setfill('0') << setw(2) << _tm.tm_mday << " "
		<< setfill('0') << setw(2) << _tm.tm_hour << ":"
		<< setfill('0') << setw(2) << _tm.tm_min << ":"
		<< setfill('0') << setw(2) << _tm.tm_sec
		<< "]"
		<< file << " " << line << " " << func << " " << _buf;
	if (strncmp(logName[level], "ERROR", 5) == 0)
	{
		errmsg = strerror_r(errno, errno_msg, sizeof(errno_msg));
		out << " errno = " << errno
			<< " msg = " << errmsg;
	}
	out << endl;
	out.close();
	return 0;
}