#include "../include/common.h"
void shm_get_one(string& body, char *src, unsigned &start, unsigned &len, unsigned& fd)
{
	char buf[13];
	memcpy(buf, src + start, 12);
	buf[12] = 0;
	len = atoi(buf);//得到len
	memcpy(buf, src + start + 12, 12);
	buf[12] = 0;
	fd = atoi(buf);//得到fd
	body.resize(len);
	memcpy((char *)body.data(), src + start + 24, len);
}
void getValueByKey(string& data, string key, string &value)
{
	size_t init = data.find(key);
	size_t end = 0, start = 0;
	if (init != string::npos)
	{
		start = data.find(":", init);
		if (start != string::npos)
		{
			start += 1;
			end = data.find("\r\n", start);
			if (end != string::npos)
			{
				end -= 1;
				value = data.substr(start, end - start + 1);
			}
		}
	}
}

void setNonblocking(int &sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) == -1) {
		LOG(LOG_ERROR, "fcntl() failed. setting Nonblocking .");
	}
}

sminfo http_mkshm(key_t key)
{
	sminfo smi;
	void * share = NULL;
	int shmid;
	if ((shmid = shmget(key, sizeof(shm_st), 0666 | IPC_CREAT)) == -1)//创建共享内存  
	{
		LOG(LOG_ERROR, "shmget() failed.key = %d.", key);
		exit(EXIT_FAILURE);
	}
	if ((share = shmat(shmid, (void*)0, 0)) == (void*)-1)//将共享内存连接到当前进程的地址空间  
	{
		LOG(LOG_ERROR, "shmat() failed.key = %d,shmid=%d", key, shmid);
		exit(EXIT_FAILURE);
	}
	LOG(LOG_TRACE, "Memory attached at %p", share);
	smi.key = key;
	smi.shmid = shmid;
	smi.share = share;
	smi.pid = getpid();
	return smi;
}