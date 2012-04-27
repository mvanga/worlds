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

#include "net.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static struct list_head net_types;
static struct list_head net_listeners;

void net_init(void)
{
	list_head_init(&net_types);
	list_head_init(&net_listeners);
}

void net_exit(void)
{
}

static void net_listener_generic_client_sent(struct net_listener *nl,
	int client, int len, char *data)
{
	char *type;
	struct dictionary *d;
	struct command *c;
	struct command_type *ct;

	if (!nl->mgr->protocol->normalize)
		return;

	d = nl->mgr->protocol->normalize(len, (unsigned char *)data);
	if (!d)	
		return;
	if (!json_check(d->json, NULL)) {
		printf("failed to validate json object\n");
		goto done;
	}
	type = nl->mgr->protocol->identify(d);
	if (!type) {
		printf("failed to identify type\n");
		goto done;
	}
	printf("got type: %s\n", type);
	ct = command_type_find(nl->mgr->cset, (char *)type);
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
	c->entity = s_entity_search_by_cid(client);
	if (!c->entity)
		goto no_entity;
	c->type = ct;
	c->dict = d;
	command_queue_up(c);

	return;

no_entity:
	ct->destroy(c);
done:
	json_delete(d->json);
	free(d);
	return;
}

static struct net_listener_type *net_listener_find(char *name)
{
	struct net_listener_type *t;

	list_for_each(&net_types, t, list)
		if (strcmp(t->type, name) == 0)
			return t;

	return NULL;
}

int net_register(struct net_listener_type *nlt)
{
	if (!nlt)
		return -1;
	if (!nlt->type || !nlt->ops)
		return -1;
	list_add(&net_types, &nlt->list);

	return 0;
}

void net_unregister(struct net_listener_type *nlt)
{
	list_del(&nlt->list);
}

int net_listener_init(struct net_listener *nl)
{
	if (!nl)
		return -1;
	if (!nl->type->ops->init)
		return -1;
	return nl->type->ops->init(nl);
}

void net_listener_exit(struct net_listener *nl)
{
	if (!nl)
		return;
	if (!nl->type->ops->exit)
		return;
	return nl->type->ops->exit(nl);
}

struct net_listener *net_listener_create(char *name, char *tname, int port,
	char *cset, char *proto, struct net_client_ops *c_ops)
{
	int ret;
	struct net_listener *nl;
	struct net_listener_type *type;

	if (!tname || !name || !proto || !cset)
		goto err;

	type = net_listener_find(tname);
	if (!type)
		goto err;

	nl = type->ops->create();
	if (!nl)
		goto err;

	nl->name = malloc(strlen(name) + 1);
	if (!nl->name)
		goto alloc_fail;

	strcpy(nl->name, name);
	nl->port = port;
	nl->c_ops = c_ops;
	nl->c_ops->client_sent = net_listener_generic_client_sent;
	nl->type = type;
	nl->mgr = command_manager_create(cset, proto);
	if (!nl->mgr)
		goto mgr_fail;

	ret = net_listener_init(nl);
	if (ret < 0)
		goto init_fail;


	list_add(&net_listeners, &nl->list);

	return nl;

init_fail:
	command_manager_destroy(nl->mgr);
mgr_fail:
	free(nl->name);
alloc_fail:
	type->ops->destroy(nl);
err:
	return NULL;
}

void net_listener_destroy(struct net_listener *nl)
{
	if (!nl)
		return;

	net_listener_exit(nl);
	if (nl->name)
		free(nl->name);
	list_del(&nl->list);

	command_manager_destroy(nl->mgr);
	nl->type->ops->destroy(nl);
}

int net_listener_start(struct net_listener *nl)
{
	if (!nl)
		return -1;
	if (!nl->type->ops->start)
		return -1;
	return nl->type->ops->start(nl);
}

void net_listener_poll(struct net_listener *nl, int timeout)
{
	if (!nl)
		return;
	if (!nl->type->ops->poll)
		return;
	return nl->type->ops->poll(nl, timeout);
}

int net_listener_send(struct net_listener *nl, int client, int len,
	uint8_t *data)
{
	if (!nl)
		return -1;
	if (!nl->type->ops->send)
		return -1;
	return nl->type->ops->send(nl, client, len, data);
}

void net_listener_disconnect(struct net_listener *nl, int client)
{
	if (!nl)
		return;
	if (!nl->type->ops->disconnect)
		return;
	return nl->type->ops->disconnect(nl, client);
}
