#include "map.h"

#include <list.h>

#define vmap_to_tilemap(vmap) \
	container_of(vmap, struct s_tilemap, vmap)

struct list_head tilemaps;

struct s_tilemap {
	char *name;
	int xlen;
	int ylen;
	uint32_t *tiles;
	struct s_vmap vmap;
	struct list_head entities;
	struct list_node list;
};

int tilemap_init(struct s_vmap *vmap)
{
	struct s_tilemap *tmap;
	
	tmap = vmap_to_tilemap(vmap);
	list_head_init(&tmap->entities);

	return 0;
}

int tilemap_exit(struct s_vmap *vmap)
{
	return 0;
}

int tilemap_add_entity(struct s_vmap *vmap, struct s_entity *e, int x, int y)
{
	struct s_tilemap *tmap = vmap_to_tilemap(vmap);

	if (x > tmap->xlen || y > tmap->xlen || x < 0 || y < 0)
		return -1;
	e->vmap = vmap;
	e->x = x;
	e->y = y;
	list_add(&tmap->entities, &e->map_list);

	return 0;
}

int tilemap_remove_entity(struct s_vmap *vmap, struct s_entity *e)
{
	list_del(&e->map_list);
	e->vmap = NULL;
	e->x = -1;
	e->y = -1;

	return 0;
}

int tilemap_bclist_create(struct s_vmap *vmap, struct s_vmap_bclist *bcl)
{
	struct s_entity *tmp;
	struct s_entity *e;
	struct s_tilemap *tmap;
	
	e = bcl->entity;
	tmap = vmap_to_tilemap(vmap);
	list_for_each(&tmap->entities, tmp, map_list) {
		int dist = (tmp->x - e->x)*(tmp->x - e->x) + 
			(tmp->y - e->y)*(tmp->y - e->y);
		int vis = (e->visibility * e->visibility);
		if (dist <= vis && tmp != e)
			s_vmap_bclist_add_entity(bcl, tmp);
	}

	return 0;
}

struct s_vmap_ops tilemap_ops = {
	.init = &tilemap_init,
	.exit = &tilemap_exit,
	.add_entity = &tilemap_add_entity,
	.remove_entity = &tilemap_remove_entity,
	.bclist_create = &tilemap_bclist_create,
};

struct s_tilemap test = {
	.xlen = 50,
	.ylen = 50,
	.name = "test-map",
	.vmap = {
		.id = 0,
		.ops = &tilemap_ops,
	},
};

void s_tilemaps_init(void)
{
	s_vmap_register(&test.vmap);
}

void s_tilemaps_exit(void)
{
	s_vmap_unregister(&test.vmap);
}
