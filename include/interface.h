#ifndef INTERFACE
#define INTERFACE
#pragma once
#include "log.h"
#include "conf.h"
#include "common.h"
extern void shm_get_one(string& body, char *src, unsigned &start, unsigned &len, unsigned& fd);
extern void getValueByKey(string& data, string key, string &value);
extern void setNonblocking(int& sockfd);
extern sminfo http_mkshm(key_t key);
#endif // !INTERFACE