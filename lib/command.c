#include "command.h"
#include "map.h"

#include <stdio.h>
#include <stdlib.h>

struct list_head comm_queue;

void s_commands_init(void)
{
	list_head_init(&comm_queue);
}

void s_command_init(struct s_command *c, struct s_entity *e, handle_f h)
{
	c->entity = e;
	c->handle = h;
}

static void s_command_join_map_handle(struct s_command *c)
{
	struct s_entity *tmp;
	struct s_command_join_map *join;

	join = container_of(c, struct s_command_join_map, command);
	printf("player %s joined map %d at (%d, %d)\n", c->entity->name,
		join->map->id, join->x, join->y);
	s_map_add_entity(join->map, c->entity, join->x, join->y);
	struct list_head *bl = bcast_list_create(c->entity);
	if (bl) {
		list_for_each(bl, tmp, bcast_list) {
			printf("broadcast to entity %s at map %d (%d, %d)\n",
					tmp->name, tmp->map->id, tmp->x, tmp->y);
		}
	}
	bcast_list_free(bl);
	printf("\n");
	free(c);
}


static void s_command_quit_map_handle(struct s_command *c)
{
	struct s_command_quit_map *quit;
	struct s_entity *tmp;
	
	if (!c->entity->map)
		return;
	
	quit = container_of(c, struct s_command_quit_map, command);
	printf("player %s quit map %d at (%d %d)\n", c->entity->name,
		c->entity->map->id, c->entity->x, c->entity->y);
	
	struct list_head *bl = bcast_list_create(c->entity);
	if (bl) {
		list_for_each(bl, tmp, bcast_list) {
			printf("broadcast to entity %s at map %d (%d, %d)\n",
					tmp->name, tmp->map->id, tmp->x, tmp->y);
		}
	}
	s_map_remove_entity(c->entity);
	bcast_list_free(bl);
	printf("\n");
	free(c);
}

struct s_command_join_map *s_command_join_map_create(struct s_entity *e,
	struct s_map *m, int x, int y)
{
	struct s_command_join_map *c;

	c = malloc(sizeof(struct s_command_join_map));
	if (!c)
		return NULL;
	s_command_init(&c->command, e, s_command_join_map_handle);
	c->map = m;
	c->x = x;
	c->y = y;

	list_add_tail(&comm_queue, &c->command.list);

	return c;
}

struct s_command_quit_map *s_command_quit_map_create(struct s_entity *e)
{
	struct s_command_quit_map *c;

	c = malloc(sizeof(struct s_command_quit_map));
	if (!c)
		return NULL;
	s_command_init(&c->command, e, s_command_quit_map_handle);
	list_add_tail(&comm_queue, &c->command.list);

	return c;
}

void s_command_dispatch(int n)
{
	struct s_command *c;

	while (--n >= 0) {
		if (list_empty(&comm_queue))
			return;
		c = list_top(&comm_queue, struct s_command, list);
		list_del(&c->list);
		c->handle(c);
	}
}
