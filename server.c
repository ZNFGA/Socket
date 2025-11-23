
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MYPORT "4950"      // port sebagai string untuk getaddrinfo
#define MAXBUFLEN 100

int start_udp_server_and_loop(void);

int main(void) {
    printf("----- PROGRAM CHATTING UDP (Server) -----\n");
    start_udp_server_and_loop();
    return 0;
}

int start_udp_server_and_loop(void) {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    char buf[MAXBUFLEN];
    char s[INET6_ADDRSTRLEN];
    ssize_t numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;      // AF_INET atau AF_INET6 (boleh)
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;      // gunakan alamat lokal

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // bind ke first valid result
    for (p = servinfo; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("bind");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Gagal bind socket\n");
        freeaddrinfo(servinfo);
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("Server UDP: menunggu pesan di port %s ...\n", MYPORT);

    // client_last: simpan alamat client terakhir agar bisa dikirim balasan
    struct sockaddr_storage client_last;
    socklen_t client_last_len = 0;
    int have_client_last = 0;

    while (1) {
        addr_len = sizeof their_addr;
        numbytes = recvfrom(sockfd, buf, MAXBUFLEN - 1 , 0,
                            (struct sockaddr *)&their_addr, &addr_len);
        if (numbytes == -1) {
            perror("recvfrom");
            continue;
        }
        buf[numbytes] = '\0';
        // dapatkan IP string
        if (their_addr.ss_family == AF_INET) {
            struct sockaddr_in *sa = (struct sockaddr_in *)&their_addr;
            inet_ntop(AF_INET, &(sa->sin_addr), s, sizeof s);
        } else { // AF_INET6
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&their_addr;
            inet_ntop(AF_INET6, &(sa6->sin6_addr), s, sizeof s);
        }

        printf("%s : \"%s\"\n", s, buf);

        // simpan alamat client terakhir
        client_last = their_addr;
        client_last_len = addr_len;
        have_client_last = 1;

        // Opsional: kita bisa otomatis balas ack
        const char *ack = "Message received";
        if (sendto(sockfd, ack, strlen(ack), 0,
                   (struct sockaddr *)&their_addr, addr_len) == -1) {
            perror("sendto ack");
        }

        // Anda bisa menambahkan mekanisme untuk mengirim pesan dari server ke client
        // mis. membaca stdin dan sendto(client_last)
    }

    close(sockfd);
    return 0;
}
