/*************************************************************************
	> File Name: head.h
	> Author:
	> Mail:
	> Created Time: Fri 03 Dec 2021 06:09:13 PM CST
 ************************************************************************/

#ifndef _HEAD_H
#define _HEAD_H
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>// See NOTES 
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <signal.h>
#include <ncurses.h>
#include <locale.h>
#include "color.h"
#include "common.h"
#include "thread_pool.h"
#include "wechat.h"
#include "wechat_ui.h"
#include "dbmysql.h"
#include "mysql/mysql.h"
#ifdef _D
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif
#endif
