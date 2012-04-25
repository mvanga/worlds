#include "enet_server.h"
#include "net.h"

#include <container_of.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int enet_listener_add_client(struct enet_listener *el, int cid, ENetPeer *p)
{
	struct enet_listener_client *c;

	c = malloc(sizeof(struct enet_listener_client));
	if (!c)
		return -1;

	c->id = cid;
	c->peer = p;
	list_add(&el->clients, &c->list);

	return 0;
}

struct enet_listener_client *enet_listener_find_client(struct enet_listener *el, ENetPeer *p)
{
	struct enet_listener_client *c;

	list_for_each(&el->clients, c, list)
		if (c->peer == p)
			return c;
	return NULL;
}

int enet_listener_remove_client(struct enet_listener *el, ENetPeer *p)
{
	struct enet_listener_client *c;

	c = enet_listener_find_client(el, p);
	if (!c)
		return -1;

	list_del(&c->list);

	return 0;
}

struct net_listener *enet_listener_create(void)
{
	struct enet_listener *s;

	s = malloc(sizeof(struct enet_listener));
	if (!s)
		return NULL;
	memset(s, 0, sizeof(struct enet_listener));

	return &s->server;
}

void enet_listener_destroy(struct net_listener *l)
{
	struct enet_listener *server;

	server = container_of(l, struct enet_listener, server);

	free(server);
}

int enet_listener_init(struct net_listener *l)
{
	struct enet_listener *server;

	server = container_of(l, struct enet_listener, server);

	list_head_init(&server->clients);
	server->next_client_id = 0;
	/* Initialize enet */
	enet_initialize();

	return 0;
}

void enet_listener_exit(struct net_listener *l)
{
	struct enet_listener *server;

	server = container_of(l, struct enet_listener, server);
	enet_host_destroy(server->shost);
}

int enet_listener_start(struct net_listener *l)
{
	ENetAddress addr;
	struct enet_listener *server;

	server = container_of(l, struct enet_listener, server);

	/* Create a listener and start it */
	addr.port = htons(l->port);
	addr.host = ENET_HOST_ANY;
	server->shost = enet_host_create(&addr, 100000, 1, 0, 0);

	if (!server->shost) {
		printf("unable to initialize new enet listener\n");
		return -1;
	}

	return 0;
}

void enet_listener_poll(struct net_listener *l, int usec)
{
	ENetEvent event;
	struct enet_listener_client *client;
	struct enet_listener *server;

	server = container_of(l, struct enet_listener, server);

	if (enet_host_service(server->shost, &event, usec/1000) > 0) {
		unsigned int client_id = 0;
		if ((client = enet_listener_find_client(server, event.peer)))
			client_id = client->id;

		switch (event.type) {
			case ENET_EVENT_TYPE_NONE:
				break;
			case ENET_EVENT_TYPE_CONNECT:
				client_id = server->next_client_id++;
				if (enet_listener_add_client(server, client_id, event.peer) < 0) {
					printf("unable to add new entry to client list\n");
					return;
				}
				if (l->c_ops->client_connected)
					l->c_ops->client_connected(l, client_id);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				if (enet_listener_remove_client(server, event.peer) < 0) {
					printf("no client connected with id %d\n", client_id);
					return;
				}
				if (l->c_ops->client_disconnected)
					l->c_ops->client_disconnected(l, client_id);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				if (l->c_ops->client_sent)
					l->c_ops->client_sent(l, client_id, event.packet->dataLength, (char *)event.packet->data);
				enet_packet_destroy(event.packet);
				break;
			default:
				break;
		}
	}
}

struct net_listener_ops enet_ops = {
	.create = enet_listener_create,
	.destroy = enet_listener_destroy,
	.init = enet_listener_init,
	.exit = enet_listener_exit,
	.start = enet_listener_start,
	.poll = enet_listener_poll,
};

struct net_listener_type enet_net_type = {
	.type = "enet",
	.ops = &enet_ops,
};

void enet_init(void)
{
	net_register(&enet_net_type);
}

void enet_exit(void)
{
	net_unregister(&enet_net_type);
}
