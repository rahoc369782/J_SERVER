#ifndef PPOLL_H
#define PPOLL_H

int create_epoll_instance();
int update_pepoll_instance(int p_fd, int op, int new_fd);
void pepoll_wait(int p_fd, int server_soc);

#endif