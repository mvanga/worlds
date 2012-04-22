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
