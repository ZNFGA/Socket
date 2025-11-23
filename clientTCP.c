
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT "3333"
#define MAXDATASIZE 100

int main(void) {
    char host[256];
    char sendbuf[128], recvbuf[MAXDATASIZE];
    printf("----- PROGRAM CHATTING TCP (Client) -----\n");
    printf("To (hostname or IP): ");
    if (scanf("%255s", host) != 1) return 0;

    struct addrinfo hints, *servinfo, *p;
    int rv, sockfd;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(host, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // coba connect ke first result
    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Tidak bisa connect ke server\n");
        freeaddrinfo(servinfo);
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("Terhubung. Ketik 'quit' untuk keluar.\n");
    while (1) {
        printf("Client: ");
        if (scanf("%127s", sendbuf) != 1) break;
        if (strcmp(sendbuf, "quit") == 0) break;

        if (send(sockfd, sendbuf, strlen(sendbuf), 0) == -1) {
            perror("send");
            break;
        }

        ssize_t numbytes = recv(sockfd, recvbuf, MAXDATASIZE - 1, 0);
        if (numbytes == -1) {
            perror("recv");
            break;
        } else if (numbytes == 0) {
            printf("Server menutup koneksi\n");
            break;
        } else {
            recvbuf[numbytes] = '\0';
            printf("Server: %s\n", recvbuf);
        }
    }

    close(sockfd);
    return 0;
}
