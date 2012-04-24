#include "tcp_server.h"
#include <container_of.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>

int tcp_listener_init(struct net_listener *l)
{
	struct tcp_listener *server;
	int ssock;
	int opt = 1;
	struct sockaddr_in sin;

	server = container_of(l, struct tcp_listener, server);

	if (server->sock != -1)
		l->ops->exit(l);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(l->port);

	/* create a socket to listen on */
	if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("failed to create socket\n");
		return -1;
	}

	/* set non-blocking */
	fcntl(ssock, F_SETFL, O_NONBLOCK);
	/* reuse the address */
	setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	if (bind(ssock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		printf("failed to bind: %s\n", strerror(errno));
		close(ssock);
		return -1;
	}

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

struct net_listener_ops tcp_ops_type = {
	.name = "tcp",
	.create = NULL,
	.destroy = NULL,
	.init = tcp_listener_init,
	.exit = tcp_listener_exit,
	.start = tcp_listener_start,
	.poll = tcp_listener_poll,
};

void tcp_init(void)
{
	net_register(&tcp_ops_type);
}

void tcp_exit(void)
{
	net_unregister(&tcp_ops_type);
}
