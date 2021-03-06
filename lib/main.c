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
#include <worlds/entity.h>
#include <worlds/vmap.h>
#include <worlds/net.h>
#include <worlds/logger.h>

#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define NTIMES	100

void g_connect(struct net_listener *nl, int client)
{
	static int id = 0;
	struct s_player *p;
	char buf[100];

	printf("client %d connected\n", client);
	sprintf(buf, "dodo%d\n", id);
	p = s_player_create(client, id, buf);
	s_player_register(p);
}

void g_disconnect(struct net_listener *nl, int client)
{
	struct s_player *p;
	struct s_entity *e;

	printf("client %d disconnected\n", client);
	e = s_entity_search_by_cid(client);
	if (!e) {
		printf("no entity with id %d\n", client);
		return;
	}
	p = container_of(e, struct s_player, entity);
	s_player_unregister(p);
	s_player_destroy(p);
}

void t_connect(struct net_listener *nl, int client)
{
	printf("wheee client %d connected\n", client);
}

void t_disconnect(struct net_listener *nl, int client)
{
	printf("wheee client %d disconnected\n", client);
}

struct net_listener *login = NULL;
struct net_listener *game = NULL;

struct net_client_ops g_ops = {
	.client_connected = &g_connect,
	.client_disconnected = &g_disconnect,
};

struct net_client_ops t_ops = {
	.client_connected = &t_connect,
	.client_disconnected = &t_disconnect,
};

int running = 1;

void exit_handler(int sig)
{
	running = 0;
	printf("Exiting\n");
}

int main()
{
	signal(SIGINT, exit_handler);

	subsys_init();
	modules_init();
	printf("\n");

	s_players_init();

	login = net_listener_create("global", "tcp", 10000, "basic_game", "json", &g_ops);
	if (!login) {
		printf("whoops login\n");
		exit(1);
	}
	game = net_listener_create("testserv", "enet", 30000, "basic_game", "json", &t_ops);
	if (!game) {
		printf("whoops game\n");
		exit(1);
	}
	assert(game);
	assert(login);
	net_listener_start(login);
	net_listener_start(game);
	while (running) {
		net_listener_poll(login, 0);
		net_listener_poll(game, 0);
		command_dispatch(10);
	}
	net_listener_destroy(login);
	net_listener_destroy(game);

	printf("\n");
	modules_exit();
	subsys_exit();

	return 0;
}
