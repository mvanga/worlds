#ifndef MMO_NET_H
#define MMO_NET_H

#include <list.h>

struct net_listener;

struct net_listener_ops {
	char *name;
	int (*init)(struct net_listener *);
	void (*exit)(struct net_listener *);
	int (*start)(struct net_listener *);
	void (*poll)(struct net_listener *, int);
	void (*send)(struct net_listener *, int);
	void (*disconnect)(struct net_listener *, int);
	struct list_node list;
};

struct net_client_ops {
	void (*client_connected)(struct net_listener *, int);
	void (*client_disconnected)(struct net_listener *, int);
	void (*client_sent)(struct net_listener *, int, int, char *);
};

struct net_listener {
	char *name;
	int port;
	struct net_listener_ops *ops;
	struct net_client_ops *c_ops;
	struct list_node list;
};

/* Register available network server types */
int net_register(struct net_listener_ops *);
/*{
	list_add(list_of_server_types, ops);
}*/
int net_unregister(struct net_listener_ops *);

struct net_listener *net_create_server(char *type, int port,
	struct net_client_ops *);
/*{
	for_each_registered_server_type(t) {
		if (strcmp(t->name, type) == 0)
			break;
	}
	struct net_listener *l = t->create();
	l->port = port;
	l->c_ops = cops;
	return l;
}*/

static int net_listener_init(struct net_listener *l)
{
	l->ops->init(l);
}

static void net_listener_exit(struct net_listener *l)
{
	l->ops->exit(l);
}

static int net_listener_start(struct net_listener *);
static void net_listener_poll(struct net_listener *, int);
static void net_listener_send(struct net_listener *, int);
static void net_listener_disconnect(struct net_listener *, int);

#if 0
tcp_init();
udp_init();
struct net_listener *s1;
struct net_listener *s2;
s1 = net_create_server("tcp", 10000, &cops);
s2 = net_create_server("udp", 20000, &cops);
net_listener_init(s1);
net_listener_init(s2);
net_listener_start(s1);
net_listener_start(s2);
while (1) {
	net_listener_poll(s1);
	net_listener_poll(s2);
}

tcp_exit();
udp_exit();
#endif

#endif
