/*
 * This file is part of Worlds, an open source MMO Engine
 * Copyright 2011-2012 Manohar Vanga
 * 
 * Worlds is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Worlds is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Worlds.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <worlds/module.h>
#include <worlds/net.h>

#include "tcp_server.h"

#include <ccan/container_of.h>

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

struct net_listener *tcp_listener_create(void)
{
	struct tcp_listener *s;

	s = malloc(sizeof(struct tcp_listener));
	if (!s)
		return NULL;
	memset(s, 0, sizeof(struct tcp_listener));

	return &s->server;
}

void tcp_listener_destroy(struct net_listener *nl)
{
	struct tcp_listener *server;

	server = container_of(nl, struct tcp_listener, server);

	free(server);
}

int tcp_listener_init(struct net_listener *l)
{
	struct tcp_listener *server;
	int ssock;
	int opt = 1;
	struct addrinfo hints, *ai;
	char buf[CONFIG_BUFSIZE];

	server = container_of(l, struct tcp_listener, server);

	if (server->sock != -1)
		l->type->ops->exit(l);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	sprintf(buf, "%d", l->port);
	if (getaddrinfo(NULL, buf, &hints, &ai) != 0) {
		printf("getaddrinfo failed\n");
		return -1;
	}

	/* create a socket to listen on */
	if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("failed to create socket\n");
		return -1;
	}
	ssock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (ssock < 0) {
		printf("failed to get a valid socket\n");
		return -1;
	}

	/* set non-blocking */
	fcntl(ssock, F_SETFL, O_NONBLOCK);
	/* reuse the address */
	setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	if (bind(ssock, ai->ai_addr, ai->ai_addrlen) < 0) {
		printf("failed to bind: %s\n", strerror(errno));
		close(ssock);
		return -1;
	}
	if (listen(ssock, 10) < 0)
		return -1;

	freeaddrinfo(ai);
	/* initialize the server structure */
	server->sock = ssock;
	FD_ZERO(&server->master);
	FD_SET(ssock, &server->master);
	server->fdmax = ssock;
	server->fdmin = ssock;

	return 0;
}

void tcp_listener_exit(struct net_listener *l)
{
	struct tcp_listener *server;

	server = container_of(l, struct tcp_listener, server);
        if (server->sock == -1)
                return;
        close(server->sock);
        server->sock = -1;
}

int tcp_listener_start(struct net_listener *l)
{
	struct tcp_listener *server;
	int ssock;

	server = container_of(l, struct tcp_listener, server);
	ssock = server->sock;
	if (listen(ssock, 10) == -1) {
		printf("failed to listen\n");
		close(ssock);
		return -1;
	}
	return 0;
}

void tcp_listener_poll(struct net_listener *l, int usec)
{
	struct tcp_listener *serv;
	int i;
	int nbytes;
	struct timeval tv;
	char buf[CONFIG_BUFSIZE];
	fd_set read_fds;

	serv = container_of(l, struct tcp_listener, server);

	if (serv->sock == -1)
		return;

	/* clear the descriptor set and add our socket to it */
	bcopy((char*)&serv->master, (char*)&read_fds, sizeof(read_fds));

	tv.tv_sec = usec/1000000;
	tv.tv_usec = usec%1000000;

	if (select(serv->fdmax + 1, &read_fds, NULL, NULL, &tv) == -1)
		return;

	if (FD_ISSET(serv->sock, &read_fds)) {
		/* new connection */
		int newfd;
		struct sockaddr_in new;
		struct sockaddr *sa = (struct sockaddr *)&new;

		socklen_t alen = sizeof(new);
		if ((newfd = accept(serv->sock, sa, &alen)) < 0)
			return;

		FD_SET(newfd, &serv->master);
		if (newfd > serv->fdmax)
			serv->fdmax = newfd;
		if (newfd < serv->fdmin)
			serv->fdmin = newfd;
		if (l->c_ops->client_connected)
			l->c_ops->client_connected(l, newfd);
		return;
	}

	for (i = serv->fdmin; i <= serv->fdmax; i++) {
		if (FD_ISSET(i, &read_fds) && FD_ISSET(i, &serv->master)) {
			memset(&buf, 0, sizeof(buf));
			if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
				if (l->c_ops->client_disconnected)
					l->c_ops->client_disconnected(l, i);
				close(i);
				FD_CLR(i, &serv->master);
			} else {
				/* we got some data */
				if (l->c_ops->client_sent)
					l->c_ops->client_sent(l, i, nbytes, buf);
			}
		}
	}

}

struct net_listener_ops tcp_ops = {
	.create = tcp_listener_create,
	.destroy = tcp_listener_destroy,
	.init = tcp_listener_init,
	.exit = tcp_listener_exit,
	.start = tcp_listener_start,
	.poll = tcp_listener_poll,
};

struct net_listener_type tcp_net_type = {
	.type = "tcp",
	.ops = &tcp_ops,
};

int tcp_mod_init(void)
{
	return net_register(&tcp_net_type);
}

void tcp_mod_exit(void)
{
	net_unregister(&tcp_net_type);
}

#ifdef CONFIG_NET_TCP
static struct module mod = {
	.name = "net:tcp",
	.author = "Manohar Vanga",
	.description = "TCP networking module",
	.init = tcp_mod_init,
	.exit = tcp_mod_exit,
};
MODULE_REGISTER(&mod);
#endif
