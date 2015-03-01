#ifndef HTTPSERVER
#define HTTPSERVER
#include "stdafx.h"
#include "httpframe.h"
#include "httpctrl.h"
#include "httptransfer.h"
void httprun();//http服务主体
void http_msg_fork();//创建处理进程，承载消息
void http_mkfifo();//创建fifo文件
int nproc = sysconf(_SC_NPROCESSORS_ONLN);//CPU核数
pid_t worker_pids[PROXY_MAX_PROC], handle_pids[NPROC];
#endif