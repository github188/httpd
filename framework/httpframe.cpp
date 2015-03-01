#include "../include/httpframe.h"
/*frame*/
static next_buf nb[NBUF_SIZE];
/*pipe*/
static int pipe_write_fd, pipe_read_fd;
char pipe_buf[1];
/*共享内存头*/
static char handlebuf[HEAD_BUF];
/*shm*/
static shm_st *shm_write = NULL;
static shm_st *shm_read = NULL;
/*epoll*/
epoll_event ev, evs[EPOLL_SIZE];
static int epollfd;
void httpsvrStart()
{
	/*打开pipe*/
	if ((pipe_write_fd = open(WORKING_FIFO, O_WRONLY)) == -1 || (pipe_read_fd = open(HANDLE_FIFO, O_RDONLY)) == -1)
	{
		LOG(LOG_ERROR, "open() failed.");
		exit(EXIT_FAILURE);
	}
	else
	{
		LOG(LOG_TRACE, "open() fifo get '%s' pipe_write_fd=%d,'%s',pipe_read_fd=%d", WORKING_FIFO, pipe_write_fd, HANDLE_FIFO, pipe_read_fd);
	}
	/*IP地址与端口号信息*/
	sockaddr_in server, client;
	socklen_t len = 0;
	/*置零*/
	bzero(&server, sizeof(server));
	bzero(&client, sizeof(client));
	/*填充*/
	server.sin_addr.s_addr = inet_addr(MSG_IP);
	server.sin_port = htons(MSG_PORT);
	server.sin_family = AF_INET;
	/*设置共享内存*/
	shm_write = (shm_st *)http_mkshm(KEY_RECV).share;
	shm_read = (shm_st *)http_mkshm(KEY_SEND).share;
	shm_write->start = 0;
	shm_write->end = 0;
	shm_write->last_end = TEXT_BUF;
	shm_read->start = 0;
	shm_read->end = 0;
	shm_read->last_end = TEXT_BUF;
	/*其余定义或声明*/
	int listener, new_fd, ret, nfds;
	int i = 0;
	/*epoll相关*/
	bzero(&ev, sizeof(ev));
	uint32_t uevent = 0;
	/*接纳需下次处理的fd*/
	handle_buf handlerb;
	/*创建套接字,使用IPV4.TCP协议，返回一个 文件描述符*/
	if ((listener = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		LOG(LOG_ERROR, "socket() failed. creating listener socket.");
		exit(EXIT_FAILURE);
	}
	//reuse
	setReuse(listener);
	/*设置为非阻塞*/
	setNonblocking(listener);
	/*绑定套接字与IP*/
	if (bind(listener, (sockaddr *)&server, sizeof(sockaddr)) == -1) {
		LOG(LOG_ERROR, "bind() failed. creating listener bind.");
		exit(1);
	}
	/*监听*/
	if (listen(listener, LSTNUM) == -1) {
		LOG(LOG_ERROR, "listen() failed. creating listen.");
		exit(1);
	}
	/*创建epoll*/
	epollfd = epoll_create(EPOLL_SIZE);
	/*把listener加入列表*/
	ev.data.fd = listener;
	ev.events = EPOLLIN;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl() add listener fail.");
		exit(EXIT_FAILURE);
	}
	/*将pipe_read_fd加入队列*/
	ev.data.fd = pipe_read_fd;
	ev.events = EPOLLHUP | EPOLLIN;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, pipe_read_fd, &ev) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl() add pipe_read_fd fail.");
		exit(EXIT_FAILURE);
	}
	/*将pipe_read_fd加入队列*/
	ev.data.fd = pipe_write_fd;
	ev.events = EPOLLHUP;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, pipe_write_fd, &ev) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl() add pipe_write_fd fail.");
		exit(EXIT_FAILURE);
	}
	/*main working place*/
	while (1)
	{
		string recvdata = "";
		string senddata = "";
		if ((nfds = epoll_wait(epollfd, evs, EPOLL_SIZE, -1)) < 0)
		{
			LOG(LOG_ERROR, "epoll_wait() fail.");
			exit(EXIT_FAILURE);
		}
		LOG(LOG_TRACE, "epoll_wait() get nfds=%d", nfds);
		for (i = 0; i < nfds; i++)
		{
			new_fd = evs[i].data.fd;
			next_buf *one = &nb[new_fd];
			LOG(LOG_TRACE, "now the fd is %d", new_fd);
			if (new_fd == -1)
				continue;
			uevent = evs[i].events;
			if (uevent & EPOLLHUP)
			{
				epoll_ctl(epollfd, EPOLL_CTL_DEL, new_fd, &evs[i]);
				recycle_(new_fd);
			}
			if (uevent & EPOLLIN)
			{
				/*监听*/
				if (new_fd == listener)
				{
					while ((new_fd = accept(listener, (sockaddr *)&client, &len)) > 0)
					{
						setNonblocking(new_fd);
						one = &nb[new_fd];
						//将新fd加入epoll列表
						one->recvs.resize(24);
						one->sends = "";
						ev.data.fd = new_fd;
						ev.events = EPOLLHUP | EPOLLIN;
						epoll_ctl(epollfd, EPOLL_CTL_ADD, new_fd, &ev);
						LOG(LOG_TRACE, "connecting infomation. %s:%d", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
					}
					if (new_fd == -1) {
						if (errno != EAGAIN && errno != ECONNABORTED&& errno != EPROTO && errno != EINTR)
							LOG(LOG_ERROR, "accept() again.");
					}
					continue;
				}
				/*处理*/
				if (new_fd == pipe_read_fd)
				{
					LOG(LOG_TRACE, "pipe_read_fd=%d is ready to read  ", pipe_read_fd);
					if ((ret = msggetback()) > 0)
					{
						//设置为准备可写
						ev.events = EPOLLHUP | EPOLLOUT;
						ev.data.fd = ret;
						epoll_ctl(epollfd, EPOLL_CTL_MOD, ret, &ev);
					}
					if (ret == 0)
					{
						LOG(LOG_NORMAL, "%s", "共享内存被占用");
						handlerb.append(new_fd);
					}
					continue;
				}
				recvdata.assign(one->recvs.c_str(), one->recvs.length());
				if ((ret = recvData(new_fd, recvdata)) == 0)//接收完了
				{
					one->recvs.assign(recvdata.c_str(), recvdata.length());
					LOG(LOG_DEBUG, "recvdata get %s", recvdata.c_str() + 24);
					/*解析*/
					if ((ret = msghandler(new_fd)) == 0)
					{
						handlerb.append(new_fd);
					}
				}
				else if (ret == 1)//没接收完
				{
					LOG(LOG_TRACE, "recvData() try one more");
					one->recvs.assign(recvdata.c_str(), recvdata.length());
					continue;
				}
				else//接收失败
				{
					LOG(LOG_TRACE, "recvData() end");
					epoll_ctl(epollfd, EPOLL_CTL_DEL, new_fd, &evs[i]);
					recycle_(new_fd);
				}
			}
			if (uevent & EPOLLOUT)
			{

				senddata = one->sends;
				LOG(LOG_DEBUG, "senddata get %s", senddata.c_str());
				if ((ret = sendData(new_fd, senddata)) == 0)//发送完
				{
					epoll_ctl(epollfd, EPOLL_CTL_DEL, new_fd, &evs[i]);
					recycle_(new_fd);
				}
				else  if (ret == 1)//没发送完
				{
					LOG(LOG_TRACE, "sendData() timeout and try again");
					one->sends = senddata;//下次接着接收
					continue;
				}
				else//发送失败
				{
					LOG(LOG_ERROR, "sendData() failed.");
					epoll_ctl(epollfd, EPOLL_CTL_DEL, new_fd, &evs[i]);
					recycle_(new_fd);
				}
			}
		}
		LOG(LOG_NORMAL, "another loop!\n");
	}
}
int recvData(int &sockfd, string &recvdata)
{
	int size_recv, total_size = 0;
	char buf[BUFSIZ];
	string value = "";

	while (1)
	{
		bzero(buf, BUFSIZ);
		if ((size_recv = recv(sockfd, buf, BUFSIZ, MSG_DONTWAIT)) == -1)
		{
			if (AGAIN_ERROR)
			{
				recvdata.append(buf);
				return 1;
			}
			else
			{
				LOG(LOG_ERROR, "recv() fail");
				return -1;
			}
		}
		else if (size_recv == 0)
		{
			LOG(LOG_TRACE, "recv() Connection has been closed.");
			return -1;
		}
		recvdata.append(buf);
		if (recvdata.find("\r\n\r\n") != string::npos)
		{
			break;
		}
	}
	getValueByKey(recvdata, "Content-Length", value);//获取包体长度
	total_size = atoi(value.c_str());
	if (forRecvData(sockfd, total_size, recvdata) == -1)//传入包头
	{
		return -1;
	}
	LOG(LOG_TRACE, "client %d recv completed", sockfd);
	return 0;
}
int forRecvData(int& sockfd, int &size, string &recvdata)
{
	int size_recv, dataleft = size;
	char buf[BUFSIZ];//暂存一次接收到的数据
	while (1)
	{
		if (dataleft == 0)
		{
			break;
		}
		bzero(buf, BUFSIZ);
		if ((size_recv = recv(sockfd, buf, BUFSIZ, MSG_DONTWAIT)) == -1)
		{
			if (AGAIN_ERROR)
			{
				recvdata.append(buf);
				return 1;
			}
			else
			{
				LOG(LOG_ERROR, "recv() fail");
				return -1;
			}
		}
		else if (size_recv == 0)
		{
			LOG(LOG_TRACE, "recv() Connection has been closed.");
			return -1;
		}
		recvdata.append(buf);
		dataleft -= size_recv;
	}
	return 0;
}
int sendData(int &sockfd, string &senddata)
{
	int size_send = 0;
	unsigned dataleft = senddata.length();
	const char *respond = senddata.c_str();
	unsigned sendedByte = 0;
	while (1)
	{
		if (sendedByte >= dataleft)
		{
			break;
		}
		if ((size_send = send(sockfd, respond + sendedByte, dataleft - sendedByte, 0)) == -1)
		{
			if (AGAIN_ERROR)
			{
				LOG(LOG_ERROR, "send() failed timeout.");
				senddata = senddata.substr(sendedByte);
				return 1;
			}
			else
			{
				//SIGPIPE, Broken pipe
				return -1;
			}
		}
		else if (size_send == 0)
		{
			LOG(LOG_ERROR, "Send() failed.Connection has been closed.");
			return -1;
		}
		else
		{
			sendedByte += size_send;
		}
	}
	return 0;
}

void setReuse(int &sockfd)
{
	int opt = SO_REUSEADDR;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		LOG(LOG_ERROR, "setsockopt() failed. setting listener setsockopt.");
	}
}

int setTimeout(int &sockfd)
{
	//超时时间
	int sec = SEC;
	int micsec = MIRROSEC;
	struct timeval timeout = { sec, micsec };
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1)
	{
		LOG(LOG_ERROR, "setsockopt() failed. creat newfd timeout .");
		close(sockfd);
		return -1;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1)
	{
		LOG(LOG_ERROR, "setsockopt() failed. creat newfd timeout .");
		close(sockfd);
		return -1;
	}
	return 0;
}

void recycle_(int &new_fd)
{
	ev.data.fd = new_fd;
	epoll_ctl(epollfd, EPOLL_CTL_DEL, new_fd, &ev);
	if (close(new_fd) == 0)
	{
		LOG(LOG_TRACE, "close %d successfully", new_fd);
	}
	else
	{
		LOG(LOG_ERROR, "close() failed.");
	}
}

int msghandler(int new_fd)
{
	unsigned total = nb[new_fd].recvs.length();//包括了共享内存协议头的长度
	unsigned recvs_len = total - 24;
	string recv(nb[new_fd].recvs.c_str(), total);//含要传送的全部数据
	int ret;
	while (1)
	{
		shm_write->wlock = LOCK_WRITE;
		if (shm_write->wlock == LOCK_READ)//如果数据被读取，则等待数据被读取结束之前不能向共享内存中写入文本  
		{
			LOG(LOG_TRACE, "waiting to write,return");
			//return 0;
		}
		else
		{
			break;
		}
	}
	//else
	//{
		/*临界区开始*/
		sprintf(handlebuf, TRANS_RULE, recvs_len, new_fd);//得到头
		memcpy((char *)&recv[0], handlebuf, 24);//向recv添加内容
		if (shm_write->end + total > TEXT_BUF)//尾部空间不足,抛弃掉尾段.
		{
			shm_write->last_end = shm_write->end;
			shm_write->end = 0;
		}
		memcpy(shm_write->text + shm_write->end, recv.c_str(), total);//向共享内存中写入数据
		shm_write->end += total;
		shm_write->wlock = LOCK_READ;//写完数据，设置wlock使共享内存段可读
		/*临界区结束*/
		/*通知子进程可读*/
		if ((ret = write(pipe_write_fd, "", 1) < 0))
		{
			LOG(LOG_ERROR, "write to pipe_write_fd = %d fail()", pipe_write_fd);
			return -1;
		}
		else
		{
			LOG(LOG_TRACE, "write to handler successfully.end = %d", shm_write->end);
			return 1;
		}
	//}
}

int msggetback()
{
	trans_head thead;
	int ret;

	thead.fd = -1;
	while (1)
	{
		shm_read->wlock = LOCK_READ;
		if (shm_read->wlock == LOCK_WRITE)//如果数据被读取，则等待数据被读取结束之前不能向共享内存中写入文本  
		{
			LOG(LOG_TRACE, "waiting to read,return");
			sleep(0.005);
			//thead.fd = 0;
		}
		else
		{
			break;
		}
	}
	//else
	//{
		/*临界区开始*/
		if ((ret = read(pipe_read_fd, pipe_buf, 1)) < 0)//阻塞
		{
			LOG(LOG_ERROR, "read to pipe_read_fd = %d fail()", pipe_read_fd);
		}
		else
		{
			if (shm_read->start >= shm_read->last_end)
			{
				shm_read->start = 0;
			}
			string tmp;
			shm_get_one(tmp, shm_read->text, shm_read->start, thead.len, thead.fd);//得到body
			nb[thead.fd].sends.append(tmp);
			shm_read->start += (thead.len + 24);
			/*临界区结束*/
			LOG(LOG_TRACE, "read from handler successfully,start = %d", shm_read->start);
		}
	//}
	shm_read->wlock = LOCK_WRITE;//d读完数据，设置wlock使共享内存段可写
	return thead.fd;
}
void handle_buf::append(int fd)
{
}
void handle_buf::getend()
{
}
int handle_buf::chkfd(int fd)
{
	return -1;
}
