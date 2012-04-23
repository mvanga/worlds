#include "command.h"
#include "vmap.h"

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
	free(c);
}


static void s_command_quit_map_handle(struct s_command *c)
{
	struct s_entity *tmp;
	
	if (!c->entity->vmap)
		return;
	
	printf("player %s quit map %s at (%d %d)\n", c->entity->name,
		c->entity->vmap->name, c->entity->x, c->entity->y);
	
	struct s_vmap_bclist *bl = s_vmap_bclist_create(c->entity);
	if (bl) {
		s_vmap_bclist_for_each(bl, tmp) {
			printf("broadcast to entity %s at map %s (%d, %d)\n",
					tmp->name, tmp->vmap->name, tmp->x, tmp->y);
		}
	}
	s_vmap_remove_entity(c->entity);
	s_vmap_bclist_free(bl);
	printf("\n");
	free(c);
}

static void s_command_local_chat_handle(struct s_command *c)
{
        struct s_command_local_chat *chat;
        struct s_entity *tmp;

        if (!c->entity->vmap)
                return;

        chat = container_of(c, struct s_command_local_chat, command);
        printf("player %s broadcasting message %s\n", c->entity->name, chat->string);

        struct s_vmap_bclist *bl = s_vmap_bclist_create(c->entity);
        if (bl) {
                s_vmap_bclist_for_each(bl, tmp) {
                        printf("broadcast message %s to entity %s at map %d (%d, %d)\n",
                               chat->string, tmp->name, tmp->vmap->id, tmp->x, tmp->y);
                }
        }
        s_vmap_bclist_free(bl);
        printf("\n");
        free(chat->string);
        free(c);
}

struct s_command_join_map *s_command_join_map_create(struct s_entity *e,
	struct s_vmap *m, int x, int y)
{
	struct s_command_join_map *c;

	c = malloc(sizeof(struct s_command_join_map));
	if (!c || !m)
		return NULL;
	s_command_init(&c->command, e, s_command_join_map_handle);
	c->vmap = m;
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

struct s_command_local_chat *s_command_local_chat_create(struct s_entity *e, char *string)
{
        struct s_command_local_chat *c;

        c = malloc(sizeof(struct s_command_local_chat));
        c->string = malloc(strlen(string) + 1);
        strcpy(c->string, string);
        if (!c)
                return NULL;
        s_command_init(&c->command, e, s_command_local_chat_handle);
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
