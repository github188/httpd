#ifndef COMMON
#define COMMON
#include "conf.h"
#include "log.h"
#include "locafx.h"
/*从shm中获取某一条信息,并从shm中删除
body指一条信息的主体
src指数据源
len指被传送的内容的主体长度
fd指被传送的内容属于的文件描述符
共享内存的尾部包含有内存头
*/
void shm_get_one(string& body, char *src, unsigned &start, unsigned &len, unsigned& fd);
/*//通过KEY找到对应的VALUE
data指数据源
key指要查找的字符串
value指返回的字符串
*/
void getValueByKey(string& data, string key, string &value);
/*将fd设置为非阻塞*/
void setNonblocking(int& sockfd);
/*获取共享内存
传入key,如果共享内存不存在则创建,否则返回共享内存结构体smi
返回sminfo
key与KEY_RECV相关
*/
sminfo http_mkshm(key_t key);
#endif // !COMMON
