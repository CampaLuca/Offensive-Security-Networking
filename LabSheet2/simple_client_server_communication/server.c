#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char** argv) {
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int sockfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    // message containers
    char sendmsg_buff[1024];
    char receivemsg_buff[1024];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(&sendmsg_buff, '\0', sizeof(sendmsg_buff));
    memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(5000);

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    int bstatus = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    int listen_status = listen(sockfd, 10);
    if (listen_status == 0) {
        printf("Server is listening...\n");
    } else {
        printf("Server is not able to listen to connections");
    }

    while (1) {

        //reset containers
        memset(&sendmsg_buff, '\0', sizeof(sendmsg_buff));
        memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));

        int connfd = accept(sockfd, (struct sockaddr*)NULL, NULL);
        if (connfd) {
            printf("Server is accepting a connection\n");
        

        
        if (read(connfd, receivemsg_buff, sizeof(receivemsg_buff)-1) > 0) {
            write(connfd, receivemsg_buff, strlen(receivemsg_buff)); 
        } else {
            strcpy(sendmsg_buff, "Welcome Mate!");
            write(connfd, sendmsg_buff, strlen(sendmsg_buff));
        }
        
        
        printf("Connection Received\n");
        
        }

        close(connfd);

    }


    close(sockfd);
    
}