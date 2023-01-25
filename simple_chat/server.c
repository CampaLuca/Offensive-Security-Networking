#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "chat_configuration.h"

struct Client Clients[MAX_ALIVE_CONNECTIONS];
pthread_t pthreads[MAX_ALIVE_CONNECTIONS];
pthread_t management_thread;
int current_connections = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void prepare_message_clients_list(char* buffer) {
    strcat(buffer, "[Server] List:\n");
    for (int i = 0; i < MAX_ALIVE_CONNECTIONS; i++) {
        strcat(buffer, "\n");
        strcat(buffer, Clients[i].name);
    }
}


/**
 * @brief Handle client messages
 * 
 * @param client: the client to be handled
 * @return void* 
 */
void* handler(void* client) {
    struct Client *args = (struct Client*) client;
    
    char receivemsg_buff[1024];
    char sendmsg_buff[1024];    

    while (1) {

        if (args->quit_flag == 1) {
            printf("[-] Closing Connection ....\n"); 
            pthread_mutex_lock(&mutex);

            memset(args->name, '\0', 100);
            close(args->sockID);
            args->sockID = -1;
            args->start_time = -1;
            args->quit_flag = 0;
            current_connections--;

            pthread_mutex_unlock(&mutex);
            printf("[-] Alive connections: %d\n", current_connections);

            break;
        }

        memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));
        if (read(args->sockID, receivemsg_buff, sizeof(receivemsg_buff)-1) > 0) {
            memset(&sendmsg_buff, '\0', sizeof(sendmsg_buff));
            if (strcmp(receivemsg_buff, "list clients") == 0) {
                prepare_message_clients_list(sendmsg_buff);
                write(args->sockID, sendmsg_buff, strlen(sendmsg_buff)); 
            }  
            else {
                char name[100];
                char message[800];
                char full_message[1024];

                for (int i = 0; i < 1024; i++) { full_message[i] = '\0'; }
                for (int i = 0; i < 100; i++) { name[i] = '\0'; }
                for (int i = 0; i < 800; i++) { message[i] = '\0'; }

                if (receivemsg_buff[0] == 's' && receivemsg_buff[1] == 'e' && receivemsg_buff[2] == 'n' && receivemsg_buff[3] == 'd' && receivemsg_buff[4] == ' ') {
                    int indice_scorrimento = 5;
                    while(receivemsg_buff[indice_scorrimento] != ' ' && receivemsg_buff[indice_scorrimento] != '\0') {
                        name[indice_scorrimento-5] = receivemsg_buff[indice_scorrimento];
                        indice_scorrimento++;
                    }

                    if (receivemsg_buff[indice_scorrimento] == '\0') {
                        write(args->sockID, receivemsg_buff, strlen(receivemsg_buff));  // if the message is empty, act as an echo server
                        continue;
                    }

                    int file_descriptor = -1;

                    //printf("Received name: %s\n", name);          

                    for (int i = 0; i < MAX_ALIVE_CONNECTIONS; i++) {
                        
                        if (strncmp(name, Clients[i].name, sizeof(name)) == 0) {
                            
                            file_descriptor = Clients[i].sockID; 
                            break;
                        }
                    }   


                    if (file_descriptor == -1 || file_descriptor == args->sockID) {
                        write(args->sockID, receivemsg_buff, strlen(receivemsg_buff));  // the destination does not exist - act as an echo server
                        continue;
                    }

                    
                    indice_scorrimento++;
                    int old_indice = indice_scorrimento;
                    while(receivemsg_buff[indice_scorrimento] != '\0') {
                        message[indice_scorrimento-old_indice] = receivemsg_buff[indice_scorrimento];
                        indice_scorrimento++;
                    }

                    message[indice_scorrimento-old_indice] = '\0';
                                                    //printf("Received message: %s\n", message);

                    strncpy(full_message, "[\0", 2);
                    strncat(full_message, args->name, strlen(args->name));
                    strncat(full_message, "] ", 3);
                    strncat(full_message, message, strlen(message));

                    //printf("%s\n", full_message);
                    long res = write(file_descriptor, full_message, strlen(full_message));
                    if (res == -1) {
                        printf("Error: %s\n", strerror(errno));
                    }
                    //printf("Sent? %ld\n", res);
                    //printf("To? %d\n", file_descriptor);

                    write(args->sockID, "[+] sent", 8);

                } else {
                    write(args->sockID, receivemsg_buff, strlen(receivemsg_buff)); 
                }
            }

        } else {
            printf("[-] Closing connection...\n");
            pthread_mutex_lock(&mutex);
            current_connections--;
            memset(args->name, '\0', 100);
            close(args->sockID);
            args->sockID = -1;
            args->start_time = -1;
            args->quit_flag = 0;
            pthread_mutex_unlock(&mutex);
            printf("[-] Alive connections: %d\n", current_connections);
            break;
        }
    }

    
    pthread_exit(NULL);

}

/**
 * @brief After the maximum time expires, it sets the quit flag for the client. The thread will end its activity.
 * 
 * @param args NULL
 * @return void* 
 */
void* manage(void* args) {
    while (1) {
        int current_time = (int)time(NULL);
        for (int i = 0; i < MAX_ALIVE_CONNECTIONS; i++) {
            if (Clients[i].start_time > -1 && Clients[i].quit_flag == 0) {
                if (current_time - Clients[i].start_time > MAX_ALIVE_TIME) {
                    pthread_mutex_lock(&mutex);
                    Clients[i].quit_flag = 1;
                    pthread_mutex_unlock(&mutex);
                }
            }
        }
    }
}



int main(int argc, char** argv) {
    //initializing clients struct
    for (int i = 0; i < MAX_ALIVE_CONNECTIONS; i++) {
        Clients[i].start_time = -1;
        Clients[i].quit_flag = 0;
    }

    // struct timeval tv;
    // tv.tv_sec = 3*60;
    // tv.tv_usec = 0;
    int sockfd = 0;
    struct sockaddr_in serv_addr;
        
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(5000);

    //setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,  &(int){1}, sizeof(int) ); // better with TCP
    int bstatus = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    int listen_status = listen(sockfd, 5);
    
    pid_t pid;
    char sendmsg_buff[1024];
    char receivemsg_buff[1024];

    pthread_create(&management_thread, NULL, manage, NULL);
    printf("[+] Server ready to accept connection ...\n");
    while (1) {
        
        int connfd   = accept(sockfd, (struct sockaddr*)NULL, NULL) ;
        if (connfd> -1) {
            memset(&sendmsg_buff, '\0', sizeof(sendmsg_buff));
            memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff));

            if (current_connections < MAX_ALIVE_CONNECTIONS) {

                pthread_mutex_lock(&mutex);
                current_connections++;
                pthread_mutex_unlock(&mutex);
                
            }
            else {
                close(connfd);
                continue;
            }

            
           read(connfd, receivemsg_buff, sizeof(receivemsg_buff)-1); // now it contains the name of the client
            
            // check if the name is already present
            int valid = 1;
            for (int i = 0; i < MAX_ALIVE_CONNECTIONS; i++) {
                if (strncmp(receivemsg_buff, Clients[i].name, 100) == 0) {
                    close(connfd);
                    valid = 0;
                    break;
                }
            }

            if (valid == 0) {
                pthread_mutex_lock(&mutex);
                current_connections--;
                pthread_mutex_unlock(&mutex);
                continue;
            }
          
            pthread_mutex_lock(&mutex);
            int target_index = -1;
            for (int i = 0; i < MAX_ALIVE_CONNECTIONS; i++) {
                if (Clients[i].name[0] == '\0') {
                    target_index = i;
                    break;
                }
            }

            if (target_index == -1) {
                close(connfd);
                pthread_mutex_unlock(&mutex);
                break;
            }

            strncpy(Clients[target_index].name, receivemsg_buff, 100);
            Clients[target_index].sockID = connfd;
            Clients[target_index].start_time = (int)time(NULL);
            
            memset(&receivemsg_buff, '\0', sizeof(receivemsg_buff)); // reset the buffer

            pthread_mutex_unlock(&mutex);

            printf("[+] Connection Accepted\n[+] Alive connections: %d\n", current_connections);
            pthread_create(&pthreads[target_index], NULL, handler, (void *) &Clients[target_index]);
            
        }
   
    }


    close(sockfd);
    
}