#ifndef COMMON
#define COMMON
#include "conf.h"
#include "log.h"
#include "locafx.h"
/*��shm�л�ȡĳһ����Ϣ,����shm��ɾ��
bodyָһ����Ϣ������
srcָ����Դ
lenָ�����͵����ݵ����峤��
fdָ�����͵��������ڵ��ļ�������
�����ڴ��β���������ڴ�ͷ
*/
void shm_get_one(string& body, char *src, unsigned &start, unsigned &len, unsigned& fd);
/*//ͨ��KEY�ҵ���Ӧ��VALUE
dataָ����Դ
keyָҪ���ҵ��ַ���
valueָ���ص��ַ���
*/
void getValueByKey(string& data, string key, string &value);
/*��fd����Ϊ������*/
void setNonblocking(int& sockfd);
/*��ȡ�����ڴ�
����key,��������ڴ治�����򴴽�,���򷵻ع����ڴ�ṹ��smi
����sminfo
key��KEY_RECV���
*/
sminfo http_mkshm(key_t key);
#endif // !COMMON
