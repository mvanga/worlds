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
