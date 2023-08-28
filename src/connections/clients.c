#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include "../config.h"
#include "../utils/memory/p_memory.h"
#define MAX 80
#define PORT 8080
#define SA struct sockaddr  

int *c_clinet()
{
    int *conn = pmalloc(sizeof(int) * MAX_CLIENTS_CONN);

    if (conn == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < MAX_CLIENTS_CONN; i++)
    {
        conn[i] = 0;
    }

    for (int i = 0; i < MAX_CLIENTS_CONN; i++)
    {
        int sockfd;
        struct sockaddr_in servaddr;

        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        printf("adding %d\n", sockfd);
        if (sockfd == -1)
        {
            printf("socket creation failed...\n");
            exit(0);
        }
        else
        {
            printf("Socket successfully created..\n");
            bzero(&servaddr, sizeof(servaddr));

            // assign IP, PORT
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            servaddr.sin_port = htons(PORT);

            // connect the client socket to server socket
            if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
            {
                printf("connection with the server failed...\n");
                exit(0);
            }
            else
            {
                printf("connected to the server.. %d \n", sockfd);
                for (int i = 0; i < MAX_CLIENTS_CONN; i++)
                {
                    if (conn[i] == 0)
                    {
                        printf("addedin %d", i);
                        conn[i] = sockfd;
                        break;
                    }
                }
            }
        }
    }
    return conn;
}
