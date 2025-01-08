#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../../p_poll/p_poll.h"
#include "../../utils/memory/p_memory.h"
#include "../../config.h"

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

int p_soc()
{
    int soc;
    soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc < 0)
    {
        perror("Failed to create socket");
        return soc;
    }
    return soc;
}

struct sockaddr_in *get_clientobj()
{
    struct sockaddr_in *client = pmalloc(sizeof(struct sockaddr_in));
    if (client == NULL)
    {
        perror("Failed to allocate memory for client object");
        return NULL;
    }
    return client;
}

struct sockaddr_in *get_serverobj()
{
    struct sockaddr_in *server = pmalloc(sizeof(struct sockaddr_in));
    if (server == NULL)
    {
        perror("Failed to allocate memory for server object");
        return NULL;
    }

    memset(server, 0, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;
    server->sin_port = htons(PIE_PORT);
    if (inet_pton(AF_INET, "192.168.0.104", &server->sin_addr) <= 0)
    {
        perror("Failed to set IP address for server object");
        free(server);
        return NULL;
    }
    return server;
}

int p_bind(int soc, struct sockaddr *server, int s_size)
{
    int res = bind(soc, server, s_size);
    if (res < 0)
    {
        perror("Failed to bind server object");
    }
    return res;
}

int p_listen(int soc)
{
    int res = listen(soc, PIE_CONNECTIONS_BACKLOG);
    if (res < 0)
    {
        perror("Failed to listen server object");
    }
    return res;
}

int p_accept(int soc)
{
    struct sockaddr_in *client = get_clientobj();
    int cliaddr_len = sizeof(struct sockaddr_in);
    int res = accept(soc, (struct sockaddr *)client, (socklen_t *)&cliaddr_len);
    if (res < 0)
    {
        perror("Failed to listen server object");
        free(client);
    }
    free(client);
    return res;
}

int p_recv(int soc, char *buf)
{
    int bytes_read = recv(soc, buf, MAX_BUFFER_SIZE, 0);
    if (bytes_read < 0)
    {
        perror("Failed to read f\n");
        return bytes_read;
    }
    return bytes_read;
}

int p_send(int soc, void *buf)
{
    int bytes_send = send(soc, (char *)buf, MAX_BUFFER_SIZE, 0);
    if (bytes_send < 0)
    {
        printf("Failed to send fd: %d\n", soc);
        return bytes_send;
    }
    return bytes_send;
}

int initialize_server(struct dependancy *depn)
{
    int res = 0;
    int soc = 0;
    int p_epollfd;
    soc = p_soc();
    if (soc < 0)
    {
        printf("Failed to create socket\n");
        return soc;
    }

    struct sockaddr_in *server = get_serverobj();
    if (!server)
    {
        printf("Failed to create server object\n");
        close(soc); // Close socket to prevent resource leak
        return -1;
    }

    res = p_bind(soc, (struct sockaddr *)server, sizeof(struct sockaddr_in));
    if (res < 0)
    {
        printf("Failed to bind server object\n");
        free(server);
        close(soc);
        return res;
    }

    res = p_listen(soc);
    if (res < 0)
    {
        printf("Failed to listen server object\n");
        free(server);
        close(soc);
        return res;
    }

    /* creating epoll instance */
    p_epollfd = create_epoll_instance();
    if (p_epollfd < 0)
    {
        printf("Failed to create EPOLL instance\n");
        free(server);
        close(soc);
        return p_epollfd;
    }
    printf("server soc %d\n", soc);
    res = update_pepoll_instance(p_epollfd, 1, soc);
    if (res < 0)
    {
        printf("Failed to update LISTENING SOCKET IN EPOLL instance %d\n", p_epollfd);
        free(server);
        close(soc);
        close(p_epollfd);
        return res;
    }

    // Updating clients sockets list in epoll
    res = deploy_clients_epoll(p_epollfd, depn);
    if (res < 0)
    {
        free(server);
        close(soc);
        close(p_epollfd);
        return res;
    }

    printf("PIE is listening on port %d\n", PIE_PORT);
    printf("Ready to accept connections...\n");
    pepoll_wait(p_epollfd, soc);

    // Clean up before exiting
    free(server);
    close(soc);
    close(p_epollfd);

    return res;
}
