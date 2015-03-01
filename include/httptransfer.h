#ifndef HTTP_TRANSFER
#define HTTP_TRANSFER
#include "stdafx.h"
#include "analyhttp.h"
struct io_buf
{
	string body;
};
void transfer();//接收来自主进程的数据，解析数据，将解析后的数据发送到主进程
#endif