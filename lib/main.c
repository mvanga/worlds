#include "entity.h"
#include "player.h"
#include "map.h"
#include "command.h"

#include <stdio.h>
#include <stdlib.h>

#define NTIMES	10000

int main()
{
	int i;
	struct s_player *p[NTIMES];

	s_entities_init();
	s_players_init();
	s_maps_init();
	s_commands_init();

	struct s_map *m = s_map_create(0, 500, 500);

	for (i = 0; i < NTIMES; i++) {
		char buf[100];
		sprintf(buf, "dodo%d\n", i);
		p[i] = s_player_create(i+2, i, buf);
		s_player_register(p[i]);
	}

	for (i = 0; i < NTIMES; i++) {
		int x = random()%500;
		int y = random()%500;
		s_command_join_map_create(&p[i]->entity, m, x, y);
	}

	s_command_dispatch(NTIMES*2);

	for (i = 0; i < NTIMES; i++) {
		s_player_unregister(p[i]);
		s_player_destroy(p[i]);
	}

	s_map_destroy(m);

	return 0;
}
