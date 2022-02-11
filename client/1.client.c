/*************************************************************************
	> File Name: 1.client.c
	> Author: 
	> Mail:
	> Created Time: Wed 22 Dec 2021 06:36:12 PM CST
 ************************************************************************/
//./client -p 8888 -n luohy -P luohy123 -m 1 -s 0
//./client -p 8888 -n edg -P edg123 -m 2 -s 0
#include "../common/head.h"
const char *conf = "./wechat.conf";
void *client_work(void *arg);
int sockfd;
WINDOW *msg_win, *sub_msg_win, *info_win, *sub_info_win, *input_win, *sub_input_win;
void logout(int signum) {
    struct wechat_msg msg;
    msg.type = WECHAT_FIN;
    send(sockfd, &msg, sizeof(msg), 0);
    exit(0);
}
int main(int argc, char **argv) {
    int opt, server_port = 0, sex = -1,  mode = 1;
    char server_ip[20] = {0}, name[20] = {0}, passwd[20] = {0};
    setlocale(LC_ALL, "");
    while ((opt = getopt(argc, argv, "h:p:s:n:P:m:")) != -1) {
        switch (opt) {
            case 'h':
                strcpy(server_ip, optarg);
                break;
            case 'p':
                server_port = atoi(optarg);
                break;
            case 's':
                sex = atoi(optarg);
                break;
            case 'n':
                strcpy(name, optarg);
                break;
            case 'P':
                strcpy(passwd, optarg);
                break;
            case 'm':
                mode = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s -p port -P passwd -n name -h host -s sex -m mode.\n", argv[0]);
                exit(1);
        }
    }

    if (!strlen(name)) strcpy(name, get_conf_value(conf, "NAME"));
    if (!strlen(passwd)) strcpy(passwd, get_conf_value(conf, "PASSWD"));
    if (!strlen(server_ip)) strcpy(server_ip, get_conf_value(conf, "SERVERIP"));
    if (sex < 0) sex = atoi(get_conf_value(conf, "SEX"));
    if (!server_port) server_port = atoi(get_conf_value(conf, "SERVERPORT"));

    #ifdef UI
    init_ui();
    #endif
    DBG(BLUE"<D>"NONE" : name = %s.\n", name);
    DBG(BLUE"<D>"NONE" : passwd = %s.\n", passwd);
    DBG(BLUE"<D>"NONE" : server_ip = %s.\n", server_ip);
    DBG(BLUE"<D>"NONE" : server_port = %d.\n", server_port);
    DBG(BLUE"<D>"NONE" : sex = %s.\n", sex ? "男" : "女");

    if ((sockfd = socket_connect(server_ip, server_port)) < 0) {
        perror("socket_connect");
        exit(1);
    }
    
    DBG(BLUE"<D>"NONE" :connected to server with sockfd %d.\n", sockfd);
    struct wechat_msg msg;
    bzero(&msg, sizeof(struct wechat_msg));
    strcpy(msg.name, name);
    strcpy(msg.passwd, passwd);
    msg.sex = sex;

    if (!mode) {
        DBG(BLUE"<D>"NONE" : Program running in signup mode\n");
        msg.type = WECHAT_SIGNUP;
        send(sockfd, (void *)&msg, sizeof(msg), 0);
    } else {
        DBG(BLUE"<D>"NONE" : Program running in signin mode\n");
        msg.type = WECHAT_SIGNIN;
        send(sockfd, (void *)&msg, sizeof(msg), 0);
    }
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    if (select(sockfd + 1, &rfds, NULL, NULL, &tv) <= 0) {
        fprintf(stderr, RED"<System>"BLUE"server is out of service.\n"NONE);
        exit(1);
    }
    int ret = recv(sockfd, (void *)&msg, sizeof(struct wechat_msg), 0);
    if (ret <= 0) {
        fprintf(stderr, RED"<System>"BLUE"Something is wrong with server.\n"NONE);
        exit(1);
    }
    if (msg.type & WECHAT_ACK) {
        DBG(GREEN"<Success>"NONE" : Server return a success.\n");
        if (!mode) {
            DBG(CYAN"Please run again.\n"NONE);
            exit(0);
        }
    } else {
        DBG(RED"<Failure>"NONE" : Server return a failure.\n");
        exit(1);
    }
    
    pthread_t tid;
    pthread_create(&tid, NULL, client_work, (void *)&sockfd);

    signal(SIGINT, logout);//终止进程,中断进程 Ctrl+C
    while (1) {
        memset(msg.msg, 0, sizeof(msg.msg));
        echo();
        nocbreak();
        mvwscanw(input_win, 1, 1, "%[^\n]s", msg.msg);
        msg.type = WECHAT_WALL;
        if (msg.msg[0] == '@') {
            msg.type = WECHAT_MSG;
            strncpy(msg.to, msg.msg + 1, strchr(msg.msg, ' ') - msg.msg -1); //@luohy 你好。
            show_msg(&msg);
        }
        else if (msg.msg[0] == '#') {
            msg.type = WECHAT_SYS;
            strcpy(msg.from, name);
            show_msg(&msg);
        }
        if (!strlen(msg.msg)) continue;
        send(sockfd, (void *)&msg, sizeof(msg), 0);
        wclear(input_win);
        box(input_win, 0, 0);
        noecho();
        cbreak();
    }

    return 0;
}
