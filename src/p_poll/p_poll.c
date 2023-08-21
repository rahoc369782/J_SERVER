#include <stdio.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include "p_poll.h"
#include "../config.h"
#include "../server/syscalls/p_server.h"

#define MAX_BUFFER_SIZE 1024

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
    event = malloc(sizeof(struct epoll_event));
    event->events = EPOLLIN | EPOLLOUT;
    event->data.fd = new_fd;

    int is_update = epoll_ctl(p_fd, op, new_fd, event);
    if (is_update < 0)
    {
        printf("epoll instance creation failed");
        return is_update;
    }
    return is_update;
}

int del_pepoll_fd(int p_fd, int op, int new_fd)
{
    int is_update = epoll_ctl(p_fd, EPOLL_CTL_DEL, new_fd, NULL);
    if (is_update < 0)
    {
        perror("epoll_ctl (EPOLL_CTL_DEL)");
        return is_update;
    }
    return is_update;
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
            char buf[MAX_BUFFER_SIZE];

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
                continue;
            }

            if (p_events[i].events & EPOLLIN)
            {
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
                    continue;
                }

                if (bytes < 0)
                {
                    perror("Error reading from socket");
                }
#ifdef DEBUG
                buf[bytes] = '\0';
                printf("Received data:%d, %s\n", bytes, buf);
#endif
            }

            // if (p_events[i].events & EPOLLOUT)
            // {
            //     char response[] = "Data processed";
            //     int sent_bytes = p_send(p_events[i].data.fd, response, strlen(response));
            //     if (sent_bytes < 0)
            //     {
            //         perror("Error writing to socket");
            //     }

            //     update_pepoll_instance(p_fd, EPOLL_CTL_MOD | EPOLL_CTL_DEL, p_events[i].data.fd);
            // }
        }
    }
}
