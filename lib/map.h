#ifndef MMO_LIB_MAP_H
#define MMO_LIB_MAP_H

#include "entity.h"

struct s_map {
	int id;
	int xlen;
	int ylen;
	uint32_t *tiles;
	struct list_head entities;
	struct list_node list;
};

int s_maps_init(void);
struct s_map *s_map_create(int, int, int);
void s_map_destroy(struct s_map *);
/* Get list of entities to broadcast to (uses entity->visibility) */
struct list_head *bcast_list_create(struct s_entity *e);
void bcast_list_free(struct list_head *l);

void s_map_add_entity(struct s_map *m, struct s_entity *e, int x, int y);
void s_map_remove_entity(struct s_entity *e);

#endif
