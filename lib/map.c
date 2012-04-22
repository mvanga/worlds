#include "map.h"
#include <list.h>

#include <stdlib.h>

struct list_head maps;

int s_maps_init(void)
{
	list_head_init(&maps);
	return 0;
}

struct s_map *s_map_create(int id, int xlen, int ylen)
{
	struct s_map *m;

	m = malloc(sizeof(struct s_map));
	if (!m)
		return NULL;
	m->id = id;
	m->xlen = xlen;
	m->ylen = ylen;
	m->tiles = malloc(sizeof(uint32_t)*xlen*ylen);
	if (!m->tiles) {
		free(m);
		return NULL;
	}
	list_head_init(&m->entities);
	list_add(&maps, &m->list);

	return m;
}

void s_map_destroy(struct s_map *m)
{
	if (m->tiles)
		free(m->tiles);
	list_del(&m->list);
	free(m);
}

void s_map_add_entity(struct s_map *m, struct s_entity *e, int x, int y)
{
	e->map = m;
	e->x = x;
	e->y = y;
	list_add(&m->entities, &e->map_list);
}

void s_map_remove_entity(struct s_entity *e)
{
	list_del(&e->map_list);
}

struct list_head *bcast_list_create(struct s_entity *e)
{
	struct list_head *l;
	struct s_entity *tmp;

	if (!e->map)
		return NULL;
	l = malloc(sizeof(struct list_head));
	if (!l)
		return NULL;
	list_head_init(l);
	list_for_each(&e->map->entities, tmp, map_list) {
		int dist = (tmp->x - e->x)*(tmp->x - e->x) + 
			(tmp->y - e->y)*(tmp->y - e->y);
		int vis = (e->visibility * e->visibility);
		if (dist <= vis && tmp != e) {
			list_add(l, &tmp->bcast_list);
		}
	}
	return l;
}

void bcast_list_free(struct list_head *l)
{
	if (l)
		free(l);
}
