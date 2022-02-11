/*************************************************************************
	> File Name: common.c
	> Author:
	> Mail:
	> Created Time: Sat 03 Jul 2021 07:28:26 PM CST
 ************************************************************************/

#include "head.h"

int make_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) return -1;
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}
int make_block(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) return -1;
    flags &= ~O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}

int socket_create(int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family  = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)); 
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0) {
        return -1;
    }
    if (listen(sockfd, 8) < 0) {
        return -1;
    }
    return sockfd;
}


int socket_connect(const char *ip, int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }
    return sockfd;
}

char *get_conf_value(const char *filename, const char *key) {
    bzero(conf_ans, sizeof(conf_ans));
    FILE *fp;
    char *line = NULL, *sub = NULL;
    ssize_t len = 0, nread = 0;
    if (filename == NULL || key == NULL) {
        return NULL;
    }
    if ((fp = fopen(filename, "r")) == NULL) {
        return NULL;
    }
 
    while ((nread = getline(&line, &len, fp)) != -1) {
        if ((sub = strstr(line, key)) == NULL) continue;
        if (line[strlen(key)] == '=' && sub == line) {
            strcpy(conf_ans, line + strlen(key) + 1);
            if (conf_ans[strlen(conf_ans) - 1] == '\n') 
                conf_ans[strlen(conf_ans) - 1] = '\0';
        }
    }
    free(line);
    fclose(fp);
    return conf_ans;
}


