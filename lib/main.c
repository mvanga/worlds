/*
 * This file is part of Worlds, an open source MMO Engine
 * Copyright 2011-2012 Manohar Vanga
 * 
 * Worlds is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * DKV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Worlds.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "entity.h"
#include "player.h"
#include "vmap.h"
#include "command.h"
#include "tilemap.h"
#include "net.h"
#include "tcp_server.h"
#include "enet_server.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define NTIMES	100

void g_connect(struct net_listener *nl, int client)
{
	printf("client %d connected\n", client);
}

void g_disconnect(struct net_listener *nl, int client)
{
	printf("client %d disconnected\n", client);
}

void g_sent(struct net_listener *nl, int client, int len, char *data)
{
	printf("client %d sent: %s\n", client, data);
}

void t_connect(struct net_listener *nl, int client)
{
	printf("wheee client %d connected\n", client);
}

void t_disconnect(struct net_listener *nl, int client)
{
	printf("wheee client %d disconnected\n", client);
}

void t_sent(struct net_listener *nl, int client, int len, char *data)
{
	printf("wheee client %d sent: %s\n", client, data);
}

struct net_listener *login = NULL;
struct net_listener *game = NULL;

struct net_client_ops g_ops = {
	.client_connected = &g_connect,
	.client_disconnected = &g_disconnect,
	.client_sent = &g_sent,
};

struct net_client_ops t_ops = {
	.client_connected = &t_connect,
	.client_disconnected = &t_disconnect,
	.client_sent = &t_sent,
};

int running = 1;

void exit_handler(int sig)
{
	sig = running = 0;
}

int main()
{
	int i;
	struct s_player *p[NTIMES];

	signal(SIGINT, exit_handler);

	s_entities_init();
	s_players_init();
	s_vmaps_init();
	s_commands_init();

	s_tilemaps_init();

	net_init();
#ifdef CONFIG_NET_TCP
	tcp_init();
#endif
#ifdef CONFIG_NET_ENET
	enet_init();
#endif

	login = net_listener_create("global", "tcp", 10000, &g_ops);
	game = net_listener_create("testserv", "enet", 30000, &t_ops);
	assert(game);
	assert(login);
	net_listener_start(login);
	net_listener_start(game);
	while (running) {
		net_listener_poll(login, 0);
		net_listener_poll(game, 0);
	}
	net_listener_destroy(login);
	net_listener_destroy(game);

	struct s_vmap *m = s_vmap_find(0);

	for (i = 0; i < NTIMES; i++) {
		char buf[100];
		sprintf(buf, "dodo%d\n", i);
		p[i] = s_player_create(i+2, i, buf);
		s_player_register(p[i]);
	}

	for (i = 0; i < NTIMES; i++) {
		int x = random()%20;
		int y = random()%20;
		s_command_join_map_create(&p[i]->entity, m, x, y);
	}

        for (i = 0; i <NTIMES; i++) {
                char buf[] = "local chat";
                s_command_local_chat_create(&p[i]->entity, buf);
        }

	for (i = 0; i < NTIMES; i++)
		s_command_quit_map_create(&p[i]->entity);

	s_command_dispatch(NTIMES*4);

	for (i = 0; i < NTIMES; i++) {
		s_player_unregister(p[i]);
		s_player_destroy(p[i]);
	}

#ifdef CONFIG_NET_ENET
	enet_exit();
#endif
#ifdef CONFIG_NET_TCP
	tcp_exit();
#endif
	net_exit();

	s_tilemaps_exit();

	return 0;
}
