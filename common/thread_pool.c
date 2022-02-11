/*************************************************************************
	> File Name: thread_pool.c
	> Author:
	> Mail:
	> Created Time: Fri 03 Dec 2021 06:22:02 PM CST
 ************************************************************************/

#include "head.h"
extern char *data[1024];
extern int epollfd;
extern pthread_mutex_t mutex[1024];
extern struct wechat_user *users;

//初始化
void task_queue_init(struct task_queue *taskQueue, int size) {
    taskQueue->size = size;
    taskQueue->total = taskQueue->head = taskQueue->tail = 0;
    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
    taskQueue->fd = calloc(size, sizeof(int));
    return;
}

//入队
void task_queue_push(struct task_queue *taskQueue, int fd) {
    pthread_mutex_lock(&taskQueue->mutex);
    if (taskQueue->total == taskQueue->size) {
        DBG("taskQueue is full!\n");
        pthread_mutex_unlock(&taskQueue->mutex);//
        return;
    }
    DBG("<Push> : %d\n", fd);
    taskQueue->fd[taskQueue->tail] = fd;
    taskQueue->total++;
    if (++taskQueue->tail == taskQueue->size) {
        DBG("taskQueue tail reach end!\n");
        taskQueue->tail = 0;
    }
    pthread_mutex_unlock(&taskQueue->mutex);
    pthread_cond_signal(&taskQueue->cond);//
}


//出队
int task_queue_pop(struct task_queue *taskQueue) {
    pthread_mutex_lock(&taskQueue->mutex);
    while (taskQueue->total == 0) {//惊群效应
        DBG("taskQueue is empty!\n");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
        //函数将解锁mutex参数指向的互斥锁，并使当前线程阻塞在cond参数指向的条件变量上
        //pthread_cond_wait()被唤醒时，它解除阻塞，并且尝试获取锁（不一定拿到锁）
    }
    int fd = taskQueue->fd[taskQueue->head];
    DBG("<Pop> : %d\n", fd);
    taskQueue->total--;
    if (++taskQueue->head == taskQueue->size) {
        DBG("taskQueue head reached end!\n");
        taskQueue->head = 0;
    }
    pthread_mutex_unlock(&taskQueue->mutex);
    return fd;
}

void do_work(int fd) {
    struct wechat_msg msg;
    memset(&msg, 0, sizeof(msg));
    int ret = recv(fd, (void *)&msg, sizeof(msg), 0);
    if (ret <= 0) {
        DBG(RED"Connect Close by Peer.\n"NONE);
        //epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
        users[fd].is_online = 0;
        close(fd);
    } 
    if (sizeof(msg) != ret) {
        DBG(RED"<ChatMsgErr>"NONE" : size error.\n");
        users[fd].is_online = 0;
        close(fd);
        return;
    }
    if (msg.type & WECHAT_WALL) {
        send_to_all(&msg);
    } else if (msg.type & WECHAT_MSG) {
        send_to_user(&msg);
    } else if (msg.type & WECHAT_FIN) {
        DBG(YELLOW"<Logout>"NONE" : "RED"%s logout.\n", msg.name);
        msg.type = WECHAT_SYS;
        sprintf(msg.msg, "你的好友 %s 离开了.", users[fd].name);
        close(fd);
        users[fd].is_online = 0;
        //epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
        send_to_all(&msg);
    } else if (msg.type & WECHAT_SYS) {
        //Remote system call
        //查询在线人数
        sys_online_person(&msg);
    }

    DBG(YELLOW"<Recv> : %s\n", msg.msg);
    return;
}

void *thread_run(void *arg) {
    struct task_queue *taskQueue = (struct task_queue*) arg;
    pthread_detach(pthread_self());
    while (1) {
        int fd = task_queue_pop(taskQueue);
        do_work(fd);
        DBG(YELLOW"do_work\n"NONE);
    }
}
