#ifndef MMO_LIB_MAP_H
#define MMO_LIB_MAP_H

#include "entity.h"

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
