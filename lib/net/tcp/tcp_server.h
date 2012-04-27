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

#ifndef MMO_TCP_SERVER_H
#define MMO_TCP_SERVER_H

#include <fcntl.h>

#include "net.h"

struct tcp_listener {
	int sock;
	int fdmax;
	int fdmin;
	fd_set master;
	struct net_listener server;
};

void tcp_init(void);
void tcp_exit(void);

#endif
