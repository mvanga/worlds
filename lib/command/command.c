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

#include "command.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <errno.h>

static struct list_head sets;
static struct list_head protocols;
static struct list_head command_queue;

void commands_ng_init(void)
{
	list_head_init(&command_queue);
	list_head_init(&sets);
	list_head_init(&protocols);
}

void commands_ng_exit(void)
{
	return;
}

void command_queue_up(struct command *c)
{
	if (!c)
		return;
	list_add_tail(&command_queue, &c->list);
}

void command_dispatch(int n)
{
	struct command *c;

	while (--n >= 0) {
		if (list_empty(&command_queue))
			return;
		c = list_top(&command_queue, struct command, list);
		list_del(&c->list);
		c->handle(c);
	}
}

/* Register a single command set */
int command_set_register(struct command_set *cset)
{
	int ret = 0;

	if (!cset)
		return -EINVAL;

	if (command_set_find((char *)cset->name))
		return -EEXIST;

	list_head_init(&cset->command_types);

	if (cset->init)
		ret = cset->init(cset);
	if (ret < 0)
		return ret;

	list_add(&sets, &cset->list);

	return 0;
}

/* Unregister a single command set */
void command_set_unregister(struct command_set *cset)
{
	if (!cset)
		return;

	if (cset->exit)
		cset->exit(cset);

	list_del(&cset->list);
}

/* Find a given command set by its name */
struct command_set *command_set_find(char *name)
{
	struct command_set *t;

	list_for_each(&sets, t, list)
		if (strcmp(name, t->name) == 0)
			return t;
	return NULL;
}

/* Register a single command type into a command set */
int command_type_register(struct command_set *cset, struct command_type *ct)
{
	if (!cset || !ct)
		return -EINVAL;

	if (command_type_find(cset, (char *)ct->name))
		return -EEXIST;

	list_add(&cset->command_types, &ct->list);

	return 0;
}

/* Unregister a single command type from a command set */
void command_type_unregister(struct command_set *cset, struct command_type *ct)
{
	if (!cset || !ct)
		return;

	/* command type is not part of this command set */
	if (!command_type_find(cset, (char *)ct->name))
		return;

	list_del(&ct->list);
}

/* Find a given command type by its name */
struct command_type *command_type_find(struct command_set *cset, char *name)
{
	struct command_type *t;

	if (!cset || !name)
		return NULL;

	list_for_each(&cset->command_types, t, list)
		if (strcmp(t->name, name) == 0)
			return t;

	return NULL;
}

/* Register a single command protocol */
int command_protocol_register(struct command_protocol *cp)
{
	if (!cp)
		return -EINVAL;

	if (!cp->normalize || !cp->identify)
		return -EINVAL;

	if (command_protocol_find((char *)cp->name))
		return -EEXIST;

	list_add(&protocols, &cp->list);

	return 0;
}

/* Unregister a single command protocol */
void command_protocol_unregister(struct command_protocol *cp)
{
	if (!cp)
		return;
	list_del(&cp->list);
}

/* Find a given command protocol by its name */
struct command_protocol *command_protocol_find(char *name)
{
	struct command_protocol *t;

	list_for_each(&protocols, t, list)
		if (strcmp(t->name, name) == 0)
			return t;
	return NULL;
}

/* Create a new command manager */
struct command_manager *command_manager_create(char *cset, char *proto)
{
	struct command_set *s;
	struct command_protocol *p;
	struct command_manager *mgr;

	if (!(p = command_protocol_find(proto)))
		return NULL;
	if (!(s = command_set_find(cset)))
		return NULL;

	mgr = malloc(sizeof(struct command_manager));
	if (!mgr)
		return NULL;

	mgr->cset = s;
	mgr->protocol = p;

	return mgr;
}

void command_manager_destroy(struct command_manager *mgr)
{
	if (mgr)
		free(mgr);
}

int command_manager_handle(struct command_manager *mgr, struct s_entity *e,
	struct dictionary *d)
{
	char *type;
	struct command *c;
	struct command_type *ct;

	if (!mgr || !d)
		return -EINVAL;

	if (!json_check(d->json, NULL)) {
		printf("failed to validate json object\n");
		goto done;
	}
	type = mgr->protocol->identify(d);
	if (!type) {
		printf("failed to identify type\n");
		goto done;
	}
	printf("got type: %s\n", type);
	ct = command_type_find(mgr->cset, (char *)type);
	free(type);
	if (!ct) {
		printf("failed to find command_type\n");
		goto done;
	}
	c = ct->create(d);
	if (!c) {
		printf("failed to create command\n");
		goto done;
	}
	c->entity = e;
	if (!c->entity)
		goto no_entity;
	c->type = ct;
	c->dict = d;
	command_queue_up(c);

	return 0;

no_entity:
	ct->destroy(c);
done:
	json_delete(d->json);
	free(d);
	return -EINVAL;
}
