#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>



int main(int argc, char** argv) {

    if (argc < 4) {
        printf("Wrong number of parameters, missing address, port and client name");
        return 1;
    }
    int sockfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    // message containers
    char sendmsg_buff[1024];
    char sendmsg_buff2[1024];
    char receivemsg_buff[1024];

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(&sendmsg_buff, '\0', sizeof(sendmsg_buff));
    memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connfd = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        printf("[-] Connection with the server failed...\n");
        exit(0);
    }
    else
        printf("[+] Connected to the server..\n");

    memset(&sendmsg_buff, '\0', sizeof(sendmsg_buff));
    memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));

    if (argc > 3) {
        for (int j = 3; j < argc; j++) {
            strcat(sendmsg_buff, argv[j]);
        }

        //sleep(5);
        //strcpy(sendmsg_buff, argv[3]);
        write(sockfd, sendmsg_buff, strlen(sendmsg_buff));
    
    }
    
    printf("[HELPER] You can interact with the server with the following commands:\n\t - list clients\n\t - send <name> <message>\n\n");

    pid_t child = fork();

    if (child == 0) {
        while (1) {
            memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));
            int res = read(sockfd, receivemsg_buff, sizeof(receivemsg_buff)-1);
            if (res < 2) {
                close(sockfd);
                break;
            }

            printf("%s\n", receivemsg_buff);   
        }
        exit(0);
    } else {
        while (1) {       
            memset(&sendmsg_buff, '\0', sizeof(sendmsg_buff));
            scanf("%[^\n]", sendmsg_buff);
            // reset the buffer otherwise loop
            while (getchar() != '\n')
                continue;
            
            if (write(sockfd, sendmsg_buff, strlen(sendmsg_buff)) == -1) {
                break;
            }          
        }
    }
    
    
    close(sockfd);


    return 0;
}
