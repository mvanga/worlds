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

#include <worlds/module.h>
#include <worlds/net.h>
#include <worlds/logger.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <errno.h>

static struct list_head net_types;
static struct list_head net_listeners;

static void net_listener_generic_client_sent(struct net_listener *nl,
	int client, int len, char *data)
{
	struct s_entity *e;
	struct dictionary *d;

	log_trace("searching dictionary for client: %d\n", client);
	e = s_entity_search_by_cid(client);
	if (!e) {
		log_warn("failed to find client with id: %d\n", client);
		return;
	}
	log_trace("found entity '%s' with client id %d\n", e->name, client);
	log_trace("converting data bytes of client %d to dict\n", client);
	d = nl->mgr->protocol->normalize(len, (unsigned char *)data);
	if (!d) {
		log_warn("failed to convert data of client %d to dictionary\n",
			client);
		return;
	}
	log_trace("handling message of client %d with comm_manager\n", client);
	command_manager_handle(nl->mgr, e, d);
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

	if (!name)
		log_warn("no name provided for new server\n");
	if (!tname) {
		log_err("invalid network protocol specified for server %s\n",
			name);
		goto err;
	}
	if (!proto) {
		log_err("no data protocol specified for server %s\n", name);
		goto err;
	}
	if (!cset) {
		log_err("no command set specified for server %s\n", name);
		goto err;
	}

	type = net_listener_find(tname);
	if (!type) {
		log_err("failed to find net listener type: '%s'\n", tname);
		goto err;
	}
	log_trace("found net listener type: '%s'\n", tname);

	nl = type->ops->create();
	if (!nl) {
		log_err("failed to create new net listener of type '%s'\n",
			tname);
		goto err;
	}
	log_trace("successfully created new net listener of type '%s'\n", tname);

	nl->name = malloc(strlen(name) + 1);
	if (!nl->name) {
		log_err("failed to allocate name of net listener: %s\n", name);
		goto alloc_fail;
	}
	log_trace("allocated name for net listener: %s\n", name);
	strcpy(nl->name, name);
	log_trace("net listener: %s: port = %d\n", name, port);
	nl->port = port;
	nl->c_ops = c_ops;
	nl->c_ops->client_sent = net_listener_generic_client_sent;
	nl->type = type;
	nl->mgr = command_manager_create(cset, proto);
	if (!nl->mgr) {
		log_err("create command manager failed for cset=%s, proto=%s\n",
			cset, proto);
		goto mgr_fail;
	}
	log_trace("created command_manager for cset=%s, proto=%s\n", cset,
		proto);

	ret = net_listener_init(nl);
	if (ret < 0) {
		log_err("failed to initialize net listener: %s\n", name);
		goto init_fail;
	}
	log_trace("initialized net listener: %s\n", name);

	log_trace("adding net listener %s to list\n", name);
	list_add(&net_listeners, &nl->list);

	log_trace("net listener %s successfully created\n\n", name);

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
	if (!nl) {
		log_warn("net listener parameter is null\n");
		return;
	}

	net_listener_exit(nl);
	log_trace("removing network listener '%s' from list\n", nl->name);
	list_del(&nl->list);
	if (nl->name) {
		log_trace("freeing 'name' field of network listener '%s'\n",
			nl->name);
		free(nl->name);
	}

	log_trace("destroying command manager for net listener\n");
	command_manager_destroy(nl->mgr);
	log_trace("calling destroy function for net listener\n");
	nl->type->ops->destroy(nl);
	log_trace("successfully destroyed net listener\n\n");
}

int net_listener_start(struct net_listener *nl)
{
	if (!nl) {
		log_warn("net listener parameter is null\n");
		return -EINVAL;
	}
	if (!nl->type->ops->start) {
		log_warn("'start' unsupported in network listener %s\n",
			nl->type->type);
		return -ENOTSUP;
	}
	return nl->type->ops->start(nl);
}

void net_listener_poll(struct net_listener *nl, int timeout)
{
	if (!nl) {
		log_warn("net listener parameter is null\n");
		return;
	}
	if (!nl->type->ops->poll) {
		log_warn("'poll' unsupported in network listener %s\n",
			nl->type->type);
		return;
	}
	return nl->type->ops->poll(nl, timeout);
}

int net_listener_send(struct net_listener *nl, int client, int len,
	uint8_t *data)
{
	if (!nl) {
		log_warn("net listener parameter is null\n");
		return -EINVAL;
	}
	if (!nl->type->ops->send) {
		log_warn("'send' unsupported in network listener %s\n",
			nl->type->type);
		return -ENOTSUP;
	}
	return nl->type->ops->send(nl, client, len, data);
}

void net_listener_disconnect(struct net_listener *nl, int client)
{
	if (!nl) {
		log_warn("net listener parameter is null\n");
		return;
	}
	if (!nl->type->ops->disconnect) {
		log_warn("'disconnect' unsupported in network listener %s\n",
			nl->type->type);
		return;
	}
	return nl->type->ops->disconnect(nl, client);
}

int net_subsys_init(void)
{
	list_head_init(&net_types);
	list_head_init(&net_listeners);

	return 0;
}

void net_subsys_exit(void)
{
	return;
}

#ifdef CONFIG_NET
static struct module subsys = {
	.name = "net",
	.author = "Manohar Vanga",
	.description = "Networking subsystem",
	.init = net_subsys_init,
	.exit = net_subsys_exit,
};
SUBSYSTEM_REGISTER(&subsys);
#endif
