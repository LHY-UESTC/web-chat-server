/*************************************************************************
	> File Name: wechat.h
	> Author:
	> Mail:
	> Created Time: Wed 22 Dec 2021 07:08:46 PM CST
 ************************************************************************/

#ifndef _WECHAT_H
#define _WECHAT_H
#define MAXUSERS 100
struct wechat_user {
    char name[20];
    char passwd[20];
    int sex;
    int fd;
    int is_online;
};

#define WECHAT_SIGNUP 0x01
#define WECHAT_SIGNIN 0x02
#define WECHAT_SIGNOUT 0x04
#define WECHAT_ACK 0x08
#define WECHAT_NAK 0x10
#define WECHAT_WALL 0x20
#define WECHAT_MSG 0x40
#define WECHAT_SYS 0x80
#define WECHAT_FIN 0x100
#define WECHAT_HEART_BEAT 0x200
struct wechat_msg {
    int type;
    char name[20];
    char passwd[20];
    int sex;
    char msg[512];
    char to[20];
    char from[20];
};

int add_to_reactor(int epollfd, int fd);
void *sub_reactor(void *arg);
void send_to_all(struct wechat_msg *msg);
void send_to_user(struct wechat_msg *msg);
void sys_online_person(struct wechat_msg *msg);
#endif
