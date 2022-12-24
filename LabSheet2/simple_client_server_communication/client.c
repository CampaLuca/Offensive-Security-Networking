#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc, char** argv) {

    if (argc < 3) {
        printf("Wrong number of parameters, missing address and port");
        return 1;
    }
    int sockfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    // message containers
    char sendmsg_buff[1024];
    char receivemsg_buff[1024];

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(&sendmsg_buff, '\0', sizeof(sendmsg_buff));
    memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connfd = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    memset(&sendmsg_buff, '\0', sizeof(sendmsg_buff));
    memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));

    if (argc > 3) {
        strcpy(sendmsg_buff, argv[3]);
        write(sockfd, sendmsg_buff, strlen(sendmsg_buff));
    
    }
    
    memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));
    read(sockfd, receivemsg_buff, sizeof(receivemsg_buff)-1);
    printf("Received: %s\n", receivemsg_buff);

    close(sockfd);


    return 0;
}
