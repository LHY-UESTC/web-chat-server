/*************************************************************************
	> File Name: wechat_client.c
	> Author:
	> Mail:
	> Created Time: Fri 24 Dec 2021 06:50:03 PM CST
 ************************************************************************/

#include "head.h"
extern WINDOW *msg_win, *sub_msg_win, *info_win, *sub_info_win, *input_win, *sub_input_win;
void *client_work(void *arg) {
    int fd = *(int *)arg;
    struct wechat_msg msg;
    while (1) {
        memset(&msg, 0, sizeof(msg));
        int ret = recv(fd, &msg, sizeof(msg), 0);
        if (ret <= 0) {
            fprintf(stderr, RED"<Error>"NONE" : Server is out service.\n");
            exit(1);
        }
        if (msg.type & WECHAT_WALL) {
            DBG(BLUE"%s"NONE": %s\n", msg.name, msg.msg);
            show_msg(&msg);
        } else if (msg.type & WECHAT_MSG) {
            DBG(GREEN"*%s"NONE": %s\n", msg.name, msg.msg);
            show_msg(&msg);
        } else if (msg.type & WECHAT_SYS) {
            DBG(PINK"Server"NONE": %s\n", msg.msg);
            show_msg(&msg);
        }
    }
}
