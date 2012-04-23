#include "entity.h"
#include "player.h"
#include "vmap.h"
#include "command.h"
#include "tilemap.h"

#include <stdio.h>
#include <stdlib.h>

#define NTIMES	100

int main()
{
	int i;
	struct s_player *p[NTIMES];

	s_entities_init();
	s_players_init();
	s_vmaps_init();
	s_commands_init();

	s_tilemaps_init();

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

	s_tilemaps_exit();

	return 0;
}
