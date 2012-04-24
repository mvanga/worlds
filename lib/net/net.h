#ifndef MMO_NET_H
#define MMO_NET_H

#include <list.h>
#include <stdint.h>

struct net_listener;

struct net_listener_ops {
	int (*init)(struct net_listener *);
	void (*exit)(struct net_listener *);
	struct net_listener *(*create)(void);
	void (*destroy)(struct net_listener *);
	int (*start)(struct net_listener *);
	void (*poll)(struct net_listener *, int);
	int (*send)(struct net_listener *, int, int, uint8_t *);
	void (*disconnect)(struct net_listener *, int);
};

struct net_client_ops {
	void (*client_connected)(struct net_listener *, int);
	void (*client_disconnected)(struct net_listener *, int);
	void (*client_sent)(struct net_listener *, int, int, char *);
};

/* Represents a class of net listeners. eg. "tcp" or "udp" */
struct net_listener_type {
	char *type;
	struct net_listener_ops *ops;
	struct list_node list;
};

/* Represents an instance of a listener type. eg. "tcp" type on port 10000*/
struct net_listener {
	char *name;
	int port;
	struct net_listener_type *type;
	struct net_client_ops *c_ops;
	struct list_node list;
};

void net_init(void);
void net_exit(void);

/* Register available network server types */
int net_register(struct net_listener_type *);
void net_unregister(struct net_listener_type *);

/* Call ops for server instances */
struct net_listener *net_listener_create(char *, char *, int,
	struct net_client_ops *);
void net_listener_destroy(struct net_listener *);
int net_listener_start(struct net_listener *);
void net_listener_poll(struct net_listener *, int);
int net_listener_send(struct net_listener *, int, int, uint8_t *);
void net_listener_disconnect(struct net_listener *, int);

#endif
