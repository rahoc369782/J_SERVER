#include <stdio.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include "p_poll.h"
#include "../pie.h"
#include "../utils/memory/p_memory.h"
#include "../config.h"
#include "../status.h"
#include "../server/syscalls/p_server.h"

// Uncomment the following line to enable debugging output
#define DEBUG

int create_epoll_instance()
{
    int pepoll_fd = epoll_create1(0);
    printf("epoll fd %d , %zu\n", pepoll_fd, sizeof(struct epoll_event));
    if (pepoll_fd < 0)
    {
        perror("epoll instance creation failed");
        return pepoll_fd;
    }
    return pepoll_fd;
}

int update_pepoll_instance(int p_fd, int op, int new_fd)
{
    struct epoll_event *event;
    event = pmalloc(sizeof(struct epoll_event));
    event->events = EPOLLIN | EPOLLOUT;
    event->data.fd = new_fd;

    int is_update = epoll_ctl(p_fd, op, new_fd, event);
    if (is_update < 0)
    {
        perror("epoll update failed");
        printf("epoll instance updation failed for socket %d\n", is_update);
        return is_update;
    }
    return is_update;
}

int del_pepoll_fd(int p_fd, int op, int ex_fd)
{
    int is_update = epoll_ctl(p_fd, EPOLL_CTL_DEL, ex_fd, NULL);
    if (is_update < 0)
    {
        perror("epoll_ctl (EPOLL_CTL_DEL)");
        return is_update;
    }
    close(ex_fd);
    return is_update;
}

int deploy_clients_epoll(int p_fd, struct dependancy *depn)
{
    /*
        client contains enitire dependency server connections which is already created
        but still not added in epoll instance.
    */
    short count = 0;
    while (count < MAX_CLIENTS_CONN)
    {
        // ADD client socket in epoll instance for reading in non blocking way.
        printf("socket %d\n", depn->c_conn[count]);
        int add_fd = update_pepoll_instance(p_fd, 1, depn->c_conn[count]);
        printf("socket client added %d \n", add_fd);
        if (add_fd < 0)
        {
            printf("Failed to update in epoll instance \n");
            return -ECLIENTFD;
        }
        count++;
    }
    printf("clients list updated successfully\n");
    return 1;
}

void pepoll_wait(int p_fd, int server_soc)
{
#ifdef DEBUG
    printf("server fd is %d and epoll fd is %d \n", server_soc, p_fd);
#endif
    struct epoll_event p_events[PIE_EPOLL_EVENTSEQUEQU_SIZE];
    while (1)
    {
        int t_events = epoll_wait(p_fd, p_events, PIE_EPOLL_EVENTSEQUEQU_SIZE, -1);
        for (unsigned short i = 0; i < t_events; i++)
        {

            if (p_events[i].data.fd == server_soc)
            {
                int accp = p_accept(server_soc);
                if (accp < 0)
                {
                    printf("Failed to accept new connection\n");
                }
#ifdef DEBUG
                printf("New connection accepted %d\n", accp);
#endif
                int add_fd = update_pepoll_instance(p_fd, 1, accp);
                if (add_fd < 0)
                {
                    printf("Failed to update in epoll instance \n");
                }
                if (p_events[i].events & EPOLLOUT)
                {
                    char response[] = "Hello new client";
                    int sent_bytes = p_send(p_events[i].data.fd, response);
                    if (sent_bytes < 0)
                    {
                        perror("Error writing to socket");
                    }
                }

                continue;
            }

            if (p_events[i].events & EPOLLIN)
            {
                char buf[MAX_BUFFER_SIZE];
                int bytes = p_recv(p_events[i].data.fd, buf);

                if (bytes == 0)
                {
#ifdef DEBUG
                    printf("Connection closing request fd %d\n", p_events[i].data.fd);
#endif
                    int del_fd = del_pepoll_fd(p_fd, 3, p_events[i].data.fd);
                    if (del_fd < 0)
                    {
                        printf("Failed to update in epoll instance \n");
                    }
                    break;
                }

                if (bytes < 0)
                {
                    perror("Error reading from socket");
                }
#ifdef DEBUG
                printf("Received data:%d, %c\n", bytes, buf[0]);

                ///////
                if (buf[0] == 0x64)
                {
                    char response[] = "Data processed";
                    int sent_bytes = p_send(4, response);
                    if (sent_bytes < 0)
                    {
                        perror("Error writing to socket");
                    }
                }

                /////////
                if (p_events[i].events & EPOLLOUT)
                {
                    char response[] = "Data processed";
                    int sent_bytes = p_send(p_events[i].data.fd, response);
                    if (sent_bytes < 0)
                    {
                        perror("Error writing to socket");
                    }
                }
#endif
            }
        }
    }
}
