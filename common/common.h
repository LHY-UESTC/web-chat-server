/*************************************************************************
	> File Name: common.h
	> Author:
	> Mail:
	> Created Time: Fri 03 Dec 2021 06:09:29 PM CST
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
char conf_ans[512];
int socket_create(int port);
int socket_connect(const char *ip, int port);
int make_block(int fd);
int make_nonblock(int fd);
char *get_conf_value(const char *file, const char *key);
#endif
