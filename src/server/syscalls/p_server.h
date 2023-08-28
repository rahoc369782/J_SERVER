#ifndef SERVER_H
#define SERVER_H
#include "../../pie.h"

int initialize_server(struct dependancy *);
int p_recv(int soc, char *buf);
int p_send(int soc, void *buf);
int p_accept(int soc);

#endif