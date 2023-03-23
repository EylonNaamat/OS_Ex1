//
// Created by eylon on 11/04/2022.
//
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define PORT_NUM 10000
#define LOCALHOST "127.0.0.1"

int main(){
    char buf[256];
    int num = 0;

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock == -1){
        printf("socket creation failed");
        return -1;
    }

    struct sockaddr_in server_ad;
    memset(&server_ad, 0, sizeof(server_ad));

    server_ad.sin_family = AF_INET;
    server_ad.sin_addr.s_addr = inet_addr(LOCALHOST);
    server_ad.sin_port = htons(PORT_NUM);

    if(bind(server_sock, (struct sockaddr *)(&server_ad), sizeof(server_ad)) == -1){
        printf("%s", "socket bind failed");
        return -1;
    }

    if(listen(server_sock, 10) == -1){
        printf("%s", "socket listening failed");
        return -1;
    }


    struct sockaddr_in client_ad;
    socklen_t cl_len = sizeof(client_ad);

    while(1){
        memset(&client_ad, 0, sizeof(client_ad));
        cl_len = sizeof(client_ad);
        int client_sock = accept(server_sock, (struct sockaddr *)(&client_ad), (&cl_len));
        if(client_sock == -1){
            printf("%s", "socket accept failed");
            return -1;
        }
        while((num = read(client_sock,buf,256)) > 0){
            write(1, buf, num);
//            buf[num] = '\0';
        }
        close(client_sock);
        printf("\n");
    }
    return 0;
}