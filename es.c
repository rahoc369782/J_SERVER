/*

* Author : Rahul Darekar
* Date : 18 August 2023
* Desc : Creating single threaded TCP server which can handle multiple FD's i.e connection
* using epoll mechanism.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
void convert_ip(unsigned char *addr, unsigned int net_byte)
{
    unsigned char bytes[4];
    bytes[0] = (net_byte >> 24) & 0xFF;
    bytes[1] = (net_byte >> 16) & 0xFF;
    bytes[2] = (net_byte >> 8) & 0xFF;
    bytes[3] = net_byte & 0xFF;
    printf("IP Address: %u.%u.%u.%u\n", bytes[0], bytes[1], bytes[2], bytes[3]);
    addr = bytes;
}
int main(int argc, char **argv)
{
    printf("%d , %s , %s", argc, argv[1], argv[2]);
    /* Declaring all required variables */

    unsigned short PORT;       /* port on which server will listen anf bind*/
    char *IP;                  /* IP want to bind and listen on */
    struct sockaddr_in client; /* client address information */
    struct sockaddr_in server; /* server address information */
    int _soc;                  /* Registered socket descriptor for server for accepting connections */
    char buf[20];
    PORT = htons(8000);
    // IP = argv[1];

    // printf("%s:%d", IP, PORT);

    // Get socket for server or create new socket descriptor
    _soc = socket(AF_INET, SOCK_STREAM, 0);
    if (_soc < 0)
    {
        printf("socket creation failed");
        exit(1);
    };

    /* Bind created socket to local port and address */

    server.sin_family = AF_INET;
    server.sin_port = PORT;
    // Set the IP address of the specific interface you want to listen on

    int _bind = bind(_soc, (struct sockaddr *)&server, sizeof(server));
    if (_bind < 0)
    {
        printf("Server binding to local IP and PORT failed");
        exit(2);
    };

    /* Listen to incoming connection with bind socket */

    int _lisn = listen(_soc, 10);
    if (_lisn < 0)
    {
        printf("Server listening stage failed");
        exit(2);
    };

    /* Accept connection which are in backlog */
    while (1)
    {
        int cliaddr_len = sizeof(client);
        int _acc = accept(_soc, (struct sockaddr *)&client, &cliaddr_len);
        if (_acc < 0)
        {
            printf("Failed in accepting connections");
            exit(2);
        }
        unsigned char bytes[16];
        convert_ip(bytes, client.sin_addr.s_addr);
        printf("client connected %s %d \n", bytes, _acc);

        /*
         * Receive the message on the newly connected socket.
         */

        if (recv(_acc, buf, sizeof(buf), 0) == -1)
        {
            exit(6);
        }
        printf("rece %s", buf);
        /*
         * Send the message back to the client.
         */
        if (send(_acc, buf, sizeof(buf), 0) < 0)
        {
            exit(7);
        }
    }
    return 0;
}
