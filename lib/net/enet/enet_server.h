/*
 * This file is part of Worlds, an open source MMO Engine
 * Copyright 2011-2012 Manohar Vanga
 * 
 * Worlds is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * DKV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Worlds.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MMO_ENET_SERVER_H
#define MMO_ENET_SERVER_H

#include <fcntl.h>

#include <enet/enet.h>

#include "net.h"

struct enet_listener_client {
	int id;
	ENetPeer *peer;
	struct list_node list;
};

struct enet_listener {
	ENetHost *shost;
	int next_client_id;
	struct list_head clients;
	struct net_listener server;
};

void enet_init(void);
void enet_exit(void);

#endif
