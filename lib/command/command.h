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

#ifndef MMO_COMMAND_NG_H
#define MMO_COMMAND_NG_H

#include <list.h>
#include <json.h>

#include "entity.h"

void commands_ng_init(void);
void commands_ng_exit(void);

struct dictionary {
	struct JsonNode *json;
};

/* A single instance of a specific command type */
struct command {
	struct command_type *type;
	struct s_entity *entity;
	struct dictionary *dict;
	void (*handle)(struct command *);
	struct list_node list;
};

/* A bunch of struct command's */
struct command_set {
	const char *name;
	int (*init)(struct command_set *);
	void (*exit)(struct command_set *);
	struct list_head command_types;
	struct list_node list;
};

/* Register a single command set */
int command_set_register(struct command_set *cset);
/* Unregister a single command set */
void command_set_unregister(struct command_set *cset);
/* Find a given command set by its name */
struct command_set *command_set_find(char *name);

/* This represents a single command type */
struct command_type {
	const char *name;
	struct command *(*create)(struct dictionary *);
	void (*destroy)(struct command *);
	struct list_node list;
};

/* Register a single command type into a command set */
int command_type_register(struct command_set *cset, struct command_type *ct);
/* Unregister a single command type from a command set */
void command_type_unregister(struct command_set *cset, struct command_type *ct);
/* Find a given command type by its name */
struct command_type *command_type_find(struct command_set *cset, char *name);

/* This represents a messaging protocol */
struct command_protocol {
	const char *name;
	int (*init)(struct command_protocol *);
	void (*exit)(struct command_protocol *);
	char *(*identify)(struct dictionary *);
	struct dictionary *(*normalize)(int len, unsigned char *data);
	unsigned char *(*denormalize)(struct dictionary *, int *len);
	struct list_node list;
};

/* Register a single command protocol */
int command_protocol_register(struct command_protocol *cp);
/* Unregister a single command protocol */
void command_protocol_unregister(struct command_protocol *cp);
/* Find a given command protocol by its name */
struct command_protocol *command_protocol_find(char *name);

/* A command manager is associated with a command set and a protocol */
struct command_manager {
	struct command_set *cset;
	struct command_protocol *protocol;
};

/* Create a new command manager */
struct command_manager *command_manager_create(char *cset, char *proto);
void command_manager_destroy(struct command_manager *mgr);

void command_queue_up(struct command *c);
void command_dispatch(int n);

#endif
