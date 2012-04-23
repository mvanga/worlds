#ifndef MMO_LIB_COMMAND_H
#define MMO_LIB_COMMAND_H

#include "entity.h"
#include "map.h"

#include <list.h>

struct s_command;

typedef void (*handle_f)(struct s_command *);

struct s_command {
	struct s_entity *entity;
	/*
	 * Does the state modification to a specific entity
	 * as well as broadcasts the updates to nearby entities
	 */
	handle_f handle;
	struct list_node list;
};

struct s_command_join_map {
	struct s_command command;
	struct s_vmap *vmap;
	int x;
	int y;
};

struct s_command_quit_map {
	struct s_command command;
};

struct s_command_move {
	struct s_command command;
	int tx;
	int ty;
};


void s_commands_init(void);
void s_command_init(struct s_command *c, struct s_entity *e, handle_f h);
struct s_command_join_map *s_command_join_map_create(struct s_entity *e,
	struct s_vmap *m, int x, int y);
struct s_command_quit_map *s_command_quit_map_create(struct s_entity *e);
void s_command_dispatch(int n);

#endif
