#ifndef __CONF__
#define __CONF__
#include "locafx.h"
/*�����ڴ�*/
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
struct shm_st//������Ľṹ��
{
#ifndef TEXT_BUF 
#define TEXT_BUF BUFSIZ*2048
#endif // !TEXT_BUF 

	unsigned wlock;//�ɶ����д���о�
	unsigned start;//��ǰͣ��
	unsigned end;//βͣ��
	unsigned last_end;//�����һ��end�ص�ԭ��ʱend��λ��
	char text[TEXT_BUF];//��¼д��Ͷ�ȡ���ı�
};
/*����Э��*/
struct trans_head
{
	unsigned len;//�����г�Э��ͷ������ݳ���
	unsigned fd;//������ļ�������
};
/*��Ƭ���������*/
#define TRANS_EACH 4194304
/*�����ڴ洫�����*/
#define HEAD_BUF 24
#define TRANS_RULE  "%12u%12u" //����ʱ�涨��������ʽ,˳��ֱ�Ϊ,len,fd
/*�����������ñ�*/
#define CONTENT_TYPE_CONF "./conf/content_type.conf"
/*worker���*/
#define MSG_IP "0.0.0.0"
#define MSG_PORT 80
/*��ҳ*/
#define INDEXPAGE "/index.html"
/*����ҳ*/
#define ERROR_PAGE "/error.html"
/*transfer ���epoll������*/
#define EPOLL_TRANS_SIZE 2
/*epoll���������*/
#define EPOLL_SIZE	2000
/*δ�Ӵ����Ļ���*/
#define NBUF_SIZE EPOLL_SIZE*10
#define LOG_MSG_BUF 1024
/*��ʱʱ��*/
#define SEC 1 //��
#define MIRROSEC 0//΢��
/*1��ʾ����Ϊ�ػ�����,0��֮*/
#define ISDAEMON 0
#if(ISDAEMON)
#define DAEMON daemon(0, 0);//�ػ���������
#else
#define DAEMON //�պ�ͷ,�ر��ػ�����
#endif
/*����Ŀ¼*/
#define VIRTUALPATH "./tmp/web"
/*�������г���*/
#define LSTNUM 10
/*ctrl����fork�Ľ�����*/
#define PROXY_MAX_PROC 512 
/*CPU����*/
//extern int nproc;
#define NPROC 1 
/*�������̺ʹ�����̼���*/
extern pid_t worker_pids[PROXY_MAX_PROC], handle_pids[NPROC];
#define WORKING_FIFO "./tmp/fifo/working_fifo"
#define HANDLE_FIFO "./tmp/fifo/handle_fifo"
#endif