#ifndef HTTPFRAME
#define HTTPFRAME
#include "stdafx.h"
struct next_buf//下次或以后处理的数据
{
	string sends;
	string recvs;
};
class handle_buf//由于各种原因导致没处理完的fd集合
{
private:
	unsigned start;//第一个还未被访问的起始位置
	unsigned end;//最后一个起始位置
	unsigned fd[EPOLL_SIZE];
public:
	void append(int fd);
	int chkfd(int fd);
	void getend();
	handle_buf()
		:start(0), end(0)
	{}
};
void httpsvrStart();
void recycle_(int &new_fd);//资源回收
int recvData(int &sockfd, string& recvdata);//接收用户请求
int forRecvData(int& sockfd, int& size, string &recvdata);//接收协议体
int sendData(int &sockfd, string& senddata);//响应用户请求
int msghandler(int new_fd);//请求包解析
int msggetback();//将解析后的数据取回,并返回fd
void setReuse(int &sockfd);//地址复用
int setTimeout(int &sockfd);//超时
void sighandler(int sig);//信号处理
#define AGAIN_ERROR errno == ETIMEDOUT || errno == EAGAIN || errno == EINTR
#endif