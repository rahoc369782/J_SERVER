#ifndef SERVER_H
#define SERVER_H

int initialize_server();
int p_recv(int soc, char *buf);
int p_send(int soc, void *buf);
int p_accept(int soc);
#endif