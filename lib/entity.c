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

#include <worlds/module.h>
#include <worlds/entity.h>

#include <string.h>
#include <stdlib.h>

static int next_id = 0;
struct list_head entities;

void s_entity_init(struct s_entity *entity, int type, char *name, update_f u)
{
	memset(entity, 0, sizeof(*entity));
	entity->id = next_id++;
	entity->type = type;
	entity->name = malloc(strlen(name) + 1);
	assert(entity->name != NULL);
	strcpy(entity->name, name);
	entity->update = u;
	entity->client = -1;
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

struct s_entity *s_entity_search_by_cid(int client)
{
	struct s_entity *t;

	list_for_each(&entities, t, elist) {
		if (t->client == -1)
			continue;
		if (t->client == client)
			return t;
	}
	return NULL;
}

int entity_subsys_init(void)
{
	list_head_init(&entities);
	return 0;
}

void entity_subsys_exit(void)
{
	return;
}

#ifdef CONFIG_ENTITY
static struct module subsys = {
	.name = "entity",
	.author = "Manohar Vanga",
	.description = "Entity subsystem",
	.init = entity_subsys_init,
	.exit = entity_subsys_exit,
};
SUBSYSTEM_REGISTER(&subsys);
#endif
