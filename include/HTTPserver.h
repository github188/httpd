#ifndef HTTPSERVER
#define HTTPSERVER
#include "stdafx.h"
#include "httpframe.h"
#include "httpctrl.h"
#include "httptransfer.h"
void httprun();//http��������
void http_msg_fork();//����������̣�������Ϣ
void http_mkfifo();//����fifo�ļ�
int nproc = sysconf(_SC_NPROCESSORS_ONLN);//CPU����
pid_t worker_pids[PROXY_MAX_PROC], handle_pids[NPROC];
#endif