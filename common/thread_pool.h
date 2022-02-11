/*************************************************************************
	> File Name: thread_pool.h
	> Author:
	> Mail:
	> Created Time: Fri 03 Dec 2021 06:21:57 PM CST
 ************************************************************************/

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
struct task_queue{
    int head, tail, size, total;
    int *fd;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

void task_queue_init(struct task_queue *taskQueue, int size);
void task_queue_push(struct task_queue *taskQueue, int fd);
int task_queue_pop(struct task_queue *taskQuue);

void *thread_run(void *arg);
#endif
