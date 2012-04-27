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

#ifndef MMO_LIB_MAP_H
#define MMO_LIB_MAP_H

#include <worlds/entity.h>

#include <stdint.h>

#define s_vmap_bclist_for_each(bcl, t) \
	list_for_each(&bcl->bcast, t, bcast_list)

/* Broadcast list structure */
struct s_vmap_bclist {
	struct s_entity *entity;
	struct list_head bcast;
};

/* Allocate a new broadcast list */
struct s_vmap_bclist *s_vmap_bclist_alloc(void);
/* Free a broadcast list */
void s_vmap_bclist_free(struct s_vmap_bclist *);
/* Add a server entity to a broadcast list */
void s_vmap_bclist_add_entity(struct s_vmap_bclist *, struct s_entity *);

/* Create a broadcast list for a specific entity */
struct s_vmap_bclist *s_vmap_bclist_create(struct s_entity *);

/* Virtual map operations */
struct s_vmap_ops {
	int (*init)(struct s_vmap *);
	int (*exit)(struct s_vmap *);
	int (*add_entity)(struct s_vmap *, struct s_entity *, int, int);
	int (*remove_entity)(struct s_vmap *, struct s_entity *);
	int (*bclist_create)(struct s_vmap *, struct s_vmap_bclist *);
};

/* Virtual map structure */
struct s_vmap {
	uint32_t id;
	char *name;
	struct s_vmap_ops *ops;
	struct list_node list;
};

/* Initialize virtual maps subsystem */
int s_vmaps_init(void);

/* Register/unregister a map */
int s_vmap_register(struct s_vmap *);
void s_vmap_unregister(struct s_vmap *);

/* Find a map by its ID */
struct s_vmap *s_vmap_find(uint32_t id);

/* Add/remove entities from a map */
int s_vmap_add_entity(struct s_vmap *, struct s_entity *, int, int);
int s_vmap_remove_entity(struct s_entity *);

#endif
