
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MYPORT "4950"
#define MAXBUFLEN 100

int main(void) {
    char host[256];
    char msg[256];

    printf("----- PROGRAM CHATTING UDP (Client) -----\n");
    printf("To (hostname or IP): ");
    if (scanf("%255s", host) != 1) return 0;

    struct addrinfo hints, *servinfo, *p;
    int rv, sockfd;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo(host, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // buat socket pakai first result
    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "Tidak bisa membuat socket\n");
        freeaddrinfo(servinfo);
        return 2;
    }

    printf("Ketik pesan. Ketik \"quit\" untuk keluar.\n");
    while (1) {
        printf("Me: ");
        if (scanf("%255s", msg) != 1) break;
        if (strcmp(msg, "quit") == 0) break;

        ssize_t sent = sendto(sockfd, msg, strlen(msg), 0, p->ai_addr, p->ai_addrlen);
        if (sent == -1) {
            perror("sendto");
            break;
        }

        // tunggu balasan (blocking)
        char buf[MAXBUFLEN];
        struct sockaddr_storage their_addr;
        socklen_t addr_len = sizeof their_addr;
        ssize_t numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1, 0,
                                    (struct sockaddr *)&their_addr, &addr_len);
        if (numbytes == -1) {
            perror("recvfrom");
            break;
        }
        buf[numbytes] = '\0';
        printf("Server: %s\n", buf);
    }

    freeaddrinfo(servinfo);
    close(sockfd);
    return 0;
}
