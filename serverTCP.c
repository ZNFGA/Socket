
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MYPORT 3333
#define BACKLOG 10
#define MAXDATASIZE 100

int main(void) {
    int listen_fd, new_fd;
    struct sockaddr_in my_addr, their_addr;
    socklen_t sin_size;
    char buf[MAXDATASIZE];
    int yes = 1;

    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        close(listen_fd);
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), '\0', 8);

    if (bind(listen_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        close(listen_fd);
        exit(1);
    }

    if (listen(listen_fd, BACKLOG) == -1) {
        perror("listen");
        close(listen_fd);
        exit(1);
    }

    printf("----- PROGRAM CHATTING TCP (Server) -----\n");
    printf("Server menunggu koneksi di port %d ...\n", MYPORT);

    while (1) {
        sin_size = sizeof their_addr;
        new_fd = accept(listen_fd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        printf("Server mendapat koneksi dari %s\n", inet_ntoa(their_addr.sin_addr));

        // komunikasi dengan client
        ssize_t numbytes;
        while ((numbytes = recv(new_fd, buf, MAXDATASIZE - 1, 0)) > 0) {
            buf[numbytes] = '\0';
            printf("Client: %s\n", buf);

            // respon manual dari server (bisa diubah jadi input dari stdin)
            char reply[128];
            printf("Server: ");
            if (scanf("%127s", reply) != 1) {
                // jika tidak ada input, kirim pesan default
                strncpy(reply, "OK", sizeof(reply));
            }
            if (send(new_fd, reply, strlen(reply), 0) == -1) {
                perror("send");
            }
        }

        if (numbytes == -1) {
            perror("recv");
        } else {
            printf("Koneksi ditutup oleh client\n");
        }

        close(new_fd);
        // loop kembali untuk accept koneksi berikutnya
    }

    close(listen_fd);
    return 0;
}
