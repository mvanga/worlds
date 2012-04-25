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

#include "entity.h"

#include <string.h>
#include <stdlib.h>

static int next_id = 0;
struct list_head entities;

void s_entities_init(void)
{
	list_head_init(&entities);
}

void s_entity_init(struct s_entity *entity, int type, char *name, update_f u)
{
	memset(entity, 0, sizeof(*entity));
	entity->id = next_id++;
	entity->type = type;
	entity->name = malloc(strlen(name) + 1);
	assert(entity->name != NULL);
	strcpy(entity->name, name);
	entity->update = u;
}

int s_entity_register(struct s_entity *entity)
{
	if (!entity)
		return -1;
	list_add(&entities, &entity->elist);

	return 0;
}

void s_entity_unregister(struct s_entity *entity)
{
	if (!entity)
		return;
	if (entity->name)
		free(entity->name);
	list_del(&entity->elist);
}
