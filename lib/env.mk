# Main
obj-y +=  main.o \
	list.o \
	entity.o \
	player.o \
	command.o \
	vmap/vmap.o \
	vmap/tilemap/tilemap.o \
	net/net.o \
	net/tcp_server.o \

cflags-y += -DCONFIG_BUFSIZE=2048
cflags-y += -I./include -I. -I./vmap -I./vmap/tilemap -I./net -Wall -Wextra -ggdb
cflags-y += -D_GNU_SOURCE
cflags-y += -O3 -Wno-unused-parameter

ldflags-y += -lenet

CFLAGS += $(cflags-y)
LDFLAGS += $(ldflags-y)
