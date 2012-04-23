#ifndef MMO_LIB_ENTITY_H
#define MMO_LIB_ENTITY_H

#include <stdint.h>
#include <list.h>

#include "entity.h"

#define ENTITY_TYPE_SELF	0
#define ENTITY_TYPE_PLAYER	1

struct c_entity {
	int id;
	int type;
};

#define ENTITY_STATE_STATIC	0
#define ENTITY_STATE_MOVING	1

struct s_entity;

typedef void (*update_f)(struct s_entity *, uint64_t);

struct s_entity {
	char *name;
	int id;
	int type;
	struct s_vmap *vmap;
	int x;
	int y;
	int state;
	int target_x;
	int target_y;
	int visibility;
	update_f update;
	/* Can be in the entity list */
	struct list_node elist;
	/* Can be in a general purpose list */
	struct list_node list;
	/* Can be in a map list */
	struct list_node map_list;
	/* Can be in a broadcast list */
	struct list_node bcast_list;
};

void s_entities_init(void);

void s_entity_init(struct s_entity *, int type, char *name, update_f);
int s_entity_register(struct s_entity *entity);
void s_entity_unregister(struct s_entity *entity);

#endif
