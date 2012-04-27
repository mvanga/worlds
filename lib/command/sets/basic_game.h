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

#ifndef MMO_LIB_COMMAND_SET_BASIC_GAME_H
#define MMO_LIB_COMMAND_SET_BASIC_GAME_H

#include "command.h"
#include "vmap.h"

#include <ccan/list.h>

struct command_join_map {
	struct command command;
	struct s_vmap *vmap;
	int x;
	int y;
};

struct command_quit_map {
	struct command command;
};

struct command_local_chat {
        struct command command;
        char *string;
};

struct command_move {
	struct command command;
	int tx;
	int ty;
};

void cset_basic_game_init(void);
void cset_basic_game_exit(void);

#endif
