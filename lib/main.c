#include "entity.h"
#include "player.h"
#include "vmap.h"
#include "command.h"
#include "tilemap.h"
#include "net.h"
#include "tcp_server.h"

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

struct net_listener *global;

struct net_client_ops g_ops = {
	.client_connected = &g_connect,
	.client_disconnected = &g_disconnect,
	.client_sent = &g_sent,
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
	tcp_init();

	global = net_listener_create("global", "tcp", 10000, &g_ops);
	net_listener_start(global);
	while (running)
		net_listener_poll(global, 1000);
	net_listener_destroy(global);

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

	tcp_exit();
	net_exit();

	s_tilemaps_exit();

	return 0;
}
