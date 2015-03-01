#include "../include/httptransfer.h"
void transfer()
{
	/*in&out init*/
	int ret;
	unsigned total;
	unsigned end;
	string body_in_out;
	trans_head thead = { 0, 0 };
	char handlebuf[HEAD_BUF];
	char pipe_buf[1];
	/*pipe init*/
	int pipe_read_fd, pipe_write_fd;
	/*memory share*/
	shm_st *shm_read = (shm_st *)http_mkshm(KEY_RECV).share;//����
	shm_st *shm_write = (shm_st *)http_mkshm(KEY_SEND).share;//����
	LOG(LOG_TRACE, "transfer() init successfully");
	/*open pipe*/
	if ((pipe_read_fd = open(WORKING_FIFO, O_RDONLY)) == -1 || (pipe_write_fd = open(HANDLE_FIFO, O_WRONLY)) == -1)
	{
		LOG(LOG_ERROR, "open() fifo failed pipe_read_fd = %d pipe_write_fd = %d.", pipe_read_fd, pipe_write_fd);
		exit(EXIT_FAILURE);
	}
	else
	{
		LOG(LOG_TRACE, "open() fifo get '%s' pipe_read_fd=%d,'%s',pipe_write_fd=%d", WORKING_FIFO, pipe_read_fd, HANDLE_FIFO, pipe_write_fd);
	}
	/*no block*/
	setNonblocking(pipe_read_fd);
	/*epoll*/
	epoll_event ev, evs[EPOLL_TRANS_SIZE];
	int nfds, epollfd;
	bzero(&ev, sizeof(ev));
	uint32_t uevent = 0;
	int i, coming_fd;
	/*����epoll*/
	epollfd = epoll_create(EPOLL_TRANS_SIZE);
	/*��pipe_read_fd�����б�*/
	ev.data.fd = pipe_read_fd;
	ev.events = EPOLLIN;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, pipe_read_fd, &ev) < 0)
	{
		LOG(LOG_ERROR, "epoll_ctl() add pipe_read_fd fail.");
		exit(EXIT_FAILURE);
	}
	/*main handle*/
	while (1)
	{
		if ((nfds = epoll_wait(epollfd, evs, EPOLL_SIZE, -1)) < 0)
		{
			LOG(LOG_ERROR, "epoll_wait() fail.");
			exit(EXIT_FAILURE);
		}
		LOG(LOG_TRACE, "epoll_wait() get nfds=%d", nfds);
		for (i = 0; i < nfds; i++)
		{
			coming_fd = evs[i].data.fd;
			LOG(LOG_TRACE, "now the fd is %d", coming_fd);
			if (coming_fd == -1)
				continue;
			uevent = evs[i].events;
			if (uevent & EPOLLIN)
			{		/*�ɶ�*/
				if (coming_fd == pipe_read_fd)
				{
					if ((ret = read(pipe_read_fd, pipe_buf, 1)) < 0)
					{
						LOG(LOG_ERROR, "read() fail.");
						continue;
					}
					else
					{
						while (1)
						{
							shm_read->wlock = LOCK_READ;
							while (shm_read->wlock == LOCK_WRITE)
							{
								sleep(0.005);
								LOG(LOG_TRACE, "busy.");
								continue;
							}
							break;
						}
						/*�����ٽ���*/
						LOG(LOG_TRACE, "handle begin.");
						if (shm_read->start >= shm_read->last_end)
						{
							shm_read->start = 0;
						}
						shm_get_one(body_in_out, shm_read->text, shm_read->start, thead.len, thead.fd);//�õ�body_in
						shm_read->start += (thead.len + 24);
						shm_read->wlock = LOCK_WRITE;//��ȡ�����ݣ�����ʹ�����ڴ�ο�д
						/*�˳��ٽ���*/
						LOG(LOG_TRACE, "handle recv over.start = %d", shm_read->start);
						/*����*/
						DataHandle(body_in_out);
						LOG(LOG_TRACE, "handle over.");
						/*��д*/
						while (1)
						{
							shm_write->wlock = LOCK_WRITE;
							while (shm_write->wlock == LOCK_READ)
							{
								sleep(0.005);
								continue;
							}
							break;
						}
						/*�����ٽ���*/
						total = body_in_out.length();
						thead.len = body_in_out.length() - 24;
						end = shm_write->end;
						if (shm_write->end + total > TEXT_BUF)//β���ռ䲻��,������β��.
						{
							shm_write->last_end = shm_write->end;
							shm_write->end = 0;
						}
						sprintf(handlebuf, TRANS_RULE, thead.len, thead.fd);//�õ�ͷ
						memcpy((char *)&body_in_out[0], handlebuf, 24);//��recv�������
						memcpy(shm_write->text + end, body_in_out.c_str(), total);//�����ڴ���д������
						shm_write->end += total;
						shm_write->wlock = LOCK_READ;//д�����ݣ�����wlockʹ�����ڴ�οɶ�
						/*�˳��ٽ���*/

						/*֪ͨ*/
						while ((ret = write(pipe_write_fd, "", 1)) < 0)
						{
							LOG(LOG_ERROR, "some error in writing");
							sleep(0.005);
						}
					}
				}

			}
		}
	}


}