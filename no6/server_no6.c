//
// Created by tt on 19. 12. 15..
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define PORT 3490
#define QLEN 6

#define EPOLL_SIZE 128
#define BUF_SIZE 128
#define MAX_CLNT 4

void *handle_clnt(int epfd, int sockfd);
void send_msg(char *msg, int len);

int client_count = 0;
int client_socks[MAX_CLNT];

int main(int argc, char *argv[]) {
    int i;
    int server_sock, client_sock;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int alen;

    struct epoll_event event;
    struct epoll_event *ep_events;
    int epfd;
    int event_cnt = 0;
    int option = 1;
    socklen_t optlen;

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind() failed");
        exit(1);
    }

    if (listen(server_sock, QLEN) < 0) {
        fprintf(stderr, "listen failed\n");
        exit(1);
    }

    alen = sizeof(client_addr);
    epfd = epoll_create(EPOLL_SIZE);
    ep_events = (struct epoll_event *) malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    event.events = EPOLLIN;
    event.data.fd = server_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_sock, &event);
    fprintf(stderr, "Server up and running.\n");

    while (1) {
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1) fprintf(stderr, "epoll_wait() error!\n");
        for (i = 0; i < event_cnt; i++) {
            if (ep_events[i].data.fd == server_sock) {
                if ((client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &alen)) < 0) {
                    fprintf(stderr, "accept failed\n");
                    exit(1);
                }
                client_socks[client_count++] = client_sock;
                option = 1;
                optlen = sizeof(option);
                setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, (void *) &option, optlen);
                event.events = EPOLLIN;
                event.data.fd = client_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sock, &event);
                printf("connected client: %d\n", client_sock);
            } else {
                handle_clnt(epfd, ep_events[i].data.fd);
            }
        }
    }
    close(server_sock);
    close(epfd);
    return 0;
}

void *handle_clnt(int epoll_fd, int client_sock){
    int i;
    int recv = 0, str_len = 0;
    int readcnt = 0;
    char msg[BUF_SIZE];

    if ((str_len = read(client_sock, &msg[recv], BUF_SIZE)) == -1 ){
        printf("read() error!\n");
        exit(1);
    }
    recv += str_len;
    send_msg(msg, recv);
    memset(msg, 0, BUF_SIZE);

    close(client_sock);
    for (i = 0; i < client_count; i++) {
        if (client_sock == client_socks[i]){
            while(i++<client_count - 1){
                client_socks[i] = client_socks[i + 1];
            }
            break;
        }
    }
    client_count--;
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sock, NULL);
    return NULL;
}

void send_msg(char *msg, int len){
    int i, j;

    for (i = 0; i < client_count; i++)
        write(client_socks[i], msg, BUF_SIZE);
}
