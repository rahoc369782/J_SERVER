#ifndef PPOLL_H
#define PPOLL_H
#include "../pie.h"

int create_epoll_instance();
int update_pepoll_instance(int p_fd, int op, int new_fd);
int deploy_clients_epoll(int p_fd, struct dependancy *depn);
void pepoll_wait(int p_fd, int server_soc);

#endif