#ifndef MMO_TCP_SERVER_H
#define MMO_TCP_SERVER_H

#include <fcntl.h>

#include "net.h"

struct tcp_listener {
	int sock;
	int fdmax;
	int fdmin;
	fd_set master;
	struct net_listener server;
};

void tcp_init(void);
void tcp_exit(void);

#endif
