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

#include "player.h"
#include <list.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct list_head players;

void s_players_init(void)
{
	list_head_init(&players);
}

void player_update(struct s_entity *e, uint64_t jiffies)
{
}

struct s_player *s_player_create(int client, int id, char *name)
{
	struct s_player *p;
	char buf[CONFIG_BUFSIZE];

	p = malloc(sizeof(struct s_player));
	if (!p)
		return NULL;

	memset(p, 0, sizeof(*p));
	snprintf(buf, CONFIG_BUFSIZE, "Player%d", id);
	s_entity_init(&p->entity, ENTITY_TYPE_PLAYER, buf, &player_update);
	p->entity.visibility = 10;
	p->entity.client = client;
	p->pid = id;
	p->name = malloc(strlen(name) + 1);
	if (!p->name) {
		free(p);
		return NULL;
	}
	strcpy(p->name, name);

	return p;
}

void s_player_destroy(struct s_player *p)
{
	if (!p)
		return;
	if (p->name)
		free(p->name);
	free(p);
	
}

int s_player_register(struct s_player *p)
{
	int ret;

	ret = s_entity_register(&p->entity);
	if (ret < 0)
		return -1;

	list_add(&players, &p->list);

	return 0;
}

void s_player_unregister(struct s_player *p)
{
	list_del(&p->list);
	s_entity_unregister(&p->entity);
}
