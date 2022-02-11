/*************************************************************************
	> File Name: wechat.c
	> Author: 
	> Mail:
	> Created Time: Wed 22 Dec 2021 08:30:01 PM CST
 ************************************************************************/

#include "head.h"
#define SUB_MAXEVENTS 5
extern struct wechat_user *users;
extern struct task_queue *taskQueue0;
extern struct task_queue *taskQueue1;
extern int epollfd0, epollfd1;

void send_to_all(struct wechat_msg *msg) {
    for (int i = 0; i < MAXUSERS; i++) {
        if (users[i].is_online) {
            send(i, msg, sizeof(struct wechat_msg), 0);
        }
    }
}

void send_to_user(struct wechat_msg *msg) {
    for (int i = 0; i < MAXUSERS; i++) {
        if (users[i].is_online && !strcmp(users[i].name, msg->to)) {
            send(i, msg, sizeof(struct wechat_msg), 0);
        }
    }
}

void sys_online_person(struct wechat_msg *msg) {
    int cnt = 0, ind = 0;
    for (int i = 0; i < MAXUSERS; i++) {
        if (users[i].is_online) cnt++;
        if (!strcmp(users[i].name, msg->from)) ind = i;
    }
    char buffer[5];
    sprintf(buffer, "%d", cnt);
    char* str = "当前在线人数为:";
    strcpy(msg->msg, str);
    strcpy(msg->msg + strlen(str), buffer);
    send(ind, msg, sizeof(struct wechat_msg), 0);
}

int add_to_reactor(int epollfd, int fd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    //make_nonblock(fd);
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        return -1;
    }
    struct wechat_msg msg;
    sprintf(msg.msg, "你的好友 %s 上线了,快和他打个招呼吧.", users[fd].name);
    msg.type = WECHAT_SYS;
    send_to_all(&msg);
    return 0;
}

void *sub_reactor(void *arg) {
    int epollfd = *(int *)arg;
    struct task_queue *taskQueue;
    if (epollfd == epollfd0) taskQueue = taskQueue0;
    else taskQueue = taskQueue1;

    struct epoll_event ev, events[SUB_MAXEVENTS];
    for (;;) {
        int nfds = epoll_wait(epollfd, events, SUB_MAXEVENTS, -1);
        //DBG(BLUE"<epoll_wait>"NONE" : epoll return.\n");
        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;
            if (events[i].events & EPOLLIN) {
                task_queue_push(taskQueue, fd);
            }
            if (events[i].events & EPOLLHUP) {
                epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
            }
        } 
    }
}


