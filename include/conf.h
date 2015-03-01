#ifndef __CONF__
#define __CONF__
#include "locafx.h"
/*共享内存*/
struct sminfo
{
#ifndef KEY_RECV
#define KEY_RECV 1234
#endif // !KEY_RECV
#ifndef KEY_SEND
#define KEY_SEND 1235
#endif // !KEY_SEND
	pid_t pid;
	key_t key;
	int shmid;
	void *share;
};
struct shm_st//被共享的结构体
{
#ifndef TEXT_BUF 
#define TEXT_BUF BUFSIZ*2048
#endif // !TEXT_BUF 

	unsigned wlock;//可读或可写的判据
	unsigned start;//当前停点
	unsigned end;//尾停点
	unsigned last_end;//在最近一次end回到原点时end的位置
	char text[TEXT_BUF];//记录写入和读取的文本
};
/*传输协议*/
struct trans_head
{
	unsigned len;//包体中除协议头外的内容长度
	unsigned fd;//请求的文件描述符
};
/*单片传输最大量*/
#define TRANS_EACH 4194304
/*共享内存传输规则*/
#define HEAD_BUF 24
#define TRANS_RULE  "%12u%12u" //传输时规定的数据样式,顺序分别为,len,fd
/*传输种类配置表*/
#define CONTENT_TYPE_CONF "./conf/content_type.conf"
/*worker相关*/
#define MSG_IP "0.0.0.0"
#define MSG_PORT 80
/*主页*/
#define INDEXPAGE "/index.html"
/*错误页*/
#define ERROR_PAGE "/error.html"
/*transfer 最大epoll连接数*/
#define EPOLL_TRANS_SIZE 2
/*epoll最大连接数*/
#define EPOLL_SIZE	2000
/*未接待续的缓冲*/
#define NBUF_SIZE EPOLL_SIZE*10
#define LOG_MSG_BUF 1024
/*超时时间*/
#define SEC 1 //秒
#define MIRROSEC 0//微秒
/*1表示设置为守护进程,0则反之*/
#define ISDAEMON 0
#if(ISDAEMON)
#define DAEMON daemon(0, 0);//守护进程启动
#else
#define DAEMON //空宏头,关闭守护进程
#endif
/*虚拟目录*/
#define VIRTUALPATH "./tmp/web"
/*监听队列长度*/
#define LSTNUM 10
/*ctrl最多可fork的进程数*/
#define PROXY_MAX_PROC 512 
/*CPU核数*/
//extern int nproc;
#define NPROC 1 
/*工作进程和处理进程集合*/
extern pid_t worker_pids[PROXY_MAX_PROC], handle_pids[NPROC];
#define WORKING_FIFO "./tmp/fifo/working_fifo"
#define HANDLE_FIFO "./tmp/fifo/handle_fifo"
#endif