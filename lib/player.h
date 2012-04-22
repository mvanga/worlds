#ifndef MMO_LIB_PLAYER_H
#define MMO_LIB_PLAYER_H

#include "entity.h"

struct c_player {
	int id;
	char *name;
	int map;
	int x;
	int y;
	int state;
	int target_x;
	int target_y;
};

struct s_player {
	int fd;
	int pid;
	struct s_entity entity;
	char *name;
	struct list_node list;
};

void s_players_init(void);
struct s_player *s_player_create(int fd, int pid, char *name);
void s_player_destroy(struct s_player *p);
int s_player_register(struct s_player *p);
void s_player_unregister(struct s_player *p);

#endif
