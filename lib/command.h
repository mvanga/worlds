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

#ifndef MMO_LIB_COMMAND_H
#define MMO_LIB_COMMAND_H

#include "entity.h"
#include "vmap.h"

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

struct s_command_local_chat {
        struct s_command command;
        char *string;
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
struct s_command_local_chat *s_command_local_chat_create(struct s_entity *e, char *string);
void s_command_dispatch(int n);

#endif
