#ifndef MMO_ENET_SERVER_H
#define MMO_ENET_SERVER_H

#include <fcntl.h>

#include <enet/enet.h>

#include "net.h"

struct enet_listener_client {
	int id;
	ENetPeer *peer;
	struct list_node list;
};

struct enet_listener {
	ENetHost *shost;
	int next_client_id;
	struct list_head clients;
	struct net_listener server;
};

void enet_init(void);
void enet_exit(void);

#endif
