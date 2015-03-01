#include "include/HTTPserver.h"
int main()
{
	//////////
	//pid_t pid;
	//int i = 0;
	//while (1)
	//{
	//	int ret;
	//	ret = httpctrl();
	//	pid = fork();
	//	if (pid < 0)
	//	{
	//		LOG(LOG_ERROR, "fork().make error in fork.");
	//		return -1;
	//	}
	//	if (pid > 0)//父进程
	//	{
	//		worker_pids[i] = pid;
	//		LOG(LOG_TRACE, "fork().make a worker process.");
	//		i++;
	//		continue;
	//	}
	//	//子进程工作区
	//	switch (ret)
	//	{
	//	case HTTP_RUN:
	//		httprun(); break;
	//	default:
	//		break;
	//	}
	//	break;
	//}
	httprun();
	return 0;
}
void httprun()
{
	signal(SIGPIPE, SIG_IGN);
	DAEMON//设置为守护进程
	chroot(VIRTUALPATH);//更换程序根目录
	http_mkfifo();//创建fifo文件
	http_msg_fork();//消息处理进程
	httpsvrStart();//运行服务
}
void http_msg_fork()
{
	pid_t pid;
	int i;
	pid = fork();
	if (pid < 0)
	{
		LOG(LOG_ERROR, "fork().make error in fork.");
		exit(EXIT_FAILURE);
	}
	else if (pid > 0)//父进程
	{
		handle_pids[0] = pid;
		LOG(LOG_TRACE, "fork().make a handler process id is %d.", pid);
		return;
	}
	else
	{
		for (i = 1; i < NPROC; i++)
		{
			pid = fork();
			if (pid < 0)
			{
				LOG(LOG_ERROR, "fork().make error in fork the %d loop.", i);
				exit(EXIT_FAILURE);
			}
			if (pid >0)
			{
				handle_pids[i] = pid;
				LOG(LOG_TRACE, "fork().make a handler process id is %d.", pid);
			}
		}
	}
	transfer();
}
void http_mkfifo()
{
	if ((mkfifo(WORKING_FIFO, O_CREAT | 0666) == -1) && (errno != EEXIST))
	{
		LOG(LOG_ERROR, "mkfifo() fail.");
		exit(EXIT_FAILURE);
	}
	else
	{
		LOG(LOG_TRACE, "mkfifo() make a fifo file %s ", WORKING_FIFO);
	}
	if ((mkfifo(HANDLE_FIFO, O_CREAT | 0666) == -1) && (errno != EEXIST))
	{
		LOG(LOG_ERROR, "mkfifo() fail.");
	}
	else
	{
		LOG(LOG_TRACE, "mkfifo() make a fifo file %s", HANDLE_FIFO);
	}
}
