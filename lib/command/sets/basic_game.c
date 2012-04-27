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

#include "module.h"
#include "basic_game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a)                               \
	(sizeof(a) / sizeof(*(a)))

static void command_join_map_handle(struct command *c)
{
	struct s_entity *tmp;
	struct command_join_map *join;

	join = container_of(c, struct command_join_map, command);
	printf("player %s joined map %s at (%d, %d)\n", c->entity->name,
		join->vmap->name, join->x, join->y);
	s_vmap_add_entity(join->vmap, c->entity, join->x, join->y);
	struct s_vmap_bclist *bl = s_vmap_bclist_create(c->entity);
	if (bl) {
		s_vmap_bclist_for_each(bl, tmp) {
			printf("broadcast to entity %s at map %s (%d, %d)\n",
					tmp->name, tmp->vmap->name, tmp->x, tmp->y);
		}
	}
	s_vmap_bclist_free(bl);
	printf("\n");
	dictionary_destroy(c->dict);
	free(join);
}

struct command *bgame_join_map_create(struct dictionary *dict)
{
	int x;
	int y;
	struct s_vmap *map;
	JsonNode *tmp;
	struct command_join_map *join;

	/* get map ID */
	tmp = json_find_member(dict->json, "map_id");
	if (!tmp || tmp->tag != JSON_NUMBER)
		return NULL;
	map = s_vmap_find((uint32_t)tmp->number_);
	if (!map)
		return NULL;
	json_delete(tmp);
	/* get x */
	tmp = json_find_member(dict->json, "x");
	if (!tmp || tmp->tag != JSON_NUMBER)
		return NULL;
	x = (int)tmp->number_;
	json_delete(tmp);
	/* get y */
	tmp = json_find_member(dict->json, "y");
	if (!tmp || tmp->tag != JSON_NUMBER)
		return NULL;
	y = (int)tmp->number_;
	json_delete(tmp);

	join = malloc(sizeof(struct command_join_map));
	if (!join)
		return NULL;

	join->vmap = map;
	join->x = x;
	join->y = y;
	join->command.handle = command_join_map_handle;

	return &join->command;
}

void bgame_join_map_destroy(struct command *cmd)
{
	struct command_join_map *join;

	join = container_of(cmd, struct command_join_map, command);
	free(join);
}

static int bgame_cset_init(struct command_set *);
static void bgame_cset_exit(struct command_set *);

static struct command_set basic_game_cset = {
	.name = "basic_game",
	.init = bgame_cset_init,
	.exit = bgame_cset_exit,
};

static struct command_type bgame_commands[] = {
	{
		.name = "join_map",
		.create = bgame_join_map_create,
		.destroy = bgame_join_map_destroy,
	},
};

static int bgame_cset_init(struct command_set *cset)
{
	int i;
	int ret;

	for (i = 0; i < (int)ARRAY_SIZE(bgame_commands); i++) {
		ret = command_type_register(&basic_game_cset,
			&bgame_commands[i]);
		if (ret < 0)
			goto err;
	}

	return 0;

err:
	while (--i >= 0)
		command_type_unregister(&basic_game_cset, &bgame_commands[i]);
	return ret;
}

static void bgame_cset_exit(struct command_set *cset)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(bgame_commands); i++)
		command_type_unregister(&basic_game_cset, &bgame_commands[i]);
}

int cset_bgame_mod_init(void)
{
	return command_set_register(&basic_game_cset);
}

void cset_bgame_mod_exit(void)
{
	command_set_unregister(&basic_game_cset);
}

#ifdef CONFIG_COMMAND_SET_BGAME
static struct module mod = {
	.name = "command:set:basic-game",
	.author = "Manohar Vanga",
	.description = "Basic network game command set",
	.init = cset_bgame_mod_init,
	.exit = cset_bgame_mod_exit,
};
MODULE_REGISTER(&mod);
#endif
