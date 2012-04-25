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

#include "vmap.h"
#include <list.h>

#include <stdlib.h>

static struct list_head vmaps;

int s_vmaps_init(void)
{
	list_head_init(&vmaps);
	return 0;
}

struct s_vmap_bclist *s_vmap_bclist_alloc(void)
{
	struct s_vmap_bclist *bcl;

	bcl = malloc(sizeof(struct s_vmap_bclist));
	if (!bcl)
		return NULL;
	bcl->entity = NULL;
	list_head_init(&bcl->bcast);

	return bcl;
}

void s_vmap_bclist_free(struct s_vmap_bclist *bcl)
{
	if (!bcl)
		return;
	free(bcl);
}

void s_vmap_bclist_add_entity(struct s_vmap_bclist *bcl, struct s_entity *e)
{
	list_add(&bcl->bcast, &e->bcast_list);
}

struct s_vmap_bclist *s_vmap_bclist_create(struct s_entity *e)
{
	int ret;
	struct s_vmap_bclist *bcl;

	if (!e)
		return NULL;
	if (!e->vmap)
		return NULL;
	bcl = s_vmap_bclist_alloc();
	if (!bcl)
		return NULL;
	bcl->entity = e;
	ret = e->vmap->ops->bclist_create(e->vmap, bcl);
	if (ret < 0) {
		s_vmap_bclist_free(bcl);
		return NULL;
	}

	return bcl;
}

int s_vmap_register(struct s_vmap *vmap)
{
	/* TODO: Make sure all ops are set here */
	list_add(&vmaps, &vmap->list);
	vmap->ops->init(vmap);
	return 0;
}

void s_vmap_unregister(struct s_vmap *vmap)
{
	vmap->ops->exit(vmap);
	list_del(&vmap->list);
}

struct s_vmap *s_vmap_find(uint32_t id)
{
	struct s_vmap *t;

	list_for_each(&vmaps, t, list) {
		if (t->id == id)
			return t;
	}

	return NULL;
}

int s_vmap_add_entity(struct s_vmap *vmap, struct s_entity *e, int x, int y)
{
	if (!vmap)
		return -1;
	return vmap->ops->add_entity(vmap, e, x, y);
}

int s_vmap_remove_entity(struct s_entity *e)
{
	struct s_vmap *vmap;

	vmap = e->vmap;
	if (!vmap)
		return -1;
	return vmap->ops->remove_entity(vmap, e);
}
