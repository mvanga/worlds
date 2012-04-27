# Main
obj-y += \
	main.o \
	entity.o \
	player.o \
#	command.o \

# CCAN libraries
obj-y += \
	list.o \
	json.o \

obj-$(CONFIG_COMMAND) += command/cmdng.o
obj-$(CONFIG_VMAP) += vmap/vmap.o
obj-$(CONFIG_NET) += net/net.o
obj-$(CONFIG_NET_TCP) += net/tcp/tcp_server.o
obj-$(CONFIG_NET_ENET) += net/enet/enet_server.o
obj-$(CONFIG_VMAP_TILEMAP) += vmap/tilemap/tilemap.o
obj-$(CONFIG_CSET_BASIC_GAME) += command/sets/basic_game.o
obj-$(CONFIG_PROTO_JSON) += command/protocols/json_proto.o

cflags-y += -DCONFIG_BUFSIZE=2048
cflags-y += -I./include -I. -Wall -Wextra -ggdb
cflags-y += -D_GNU_SOURCE
cflags-y += -O3 -Wno-unused-parameter

# Define all the CONFIG_* variables for C code to see
cflags-y += $(addprefix -D, $(filter CONFIG_%, $(.VARIABLES)))

cflags-$(CONFIG_COMMAND) += -I./command -I./command/sets -I./command/protocols
cflags-$(CONFIG_NET) += -I./net
cflags-$(CONFIG_VMAP) += -I./vmap

cflags-$(CONFIG_NET_TCP) += -I./net/tcp
cflags-$(CONFIG_NET_ENET) += -I./net/enet
cflags-$(CONFIG_VMAP_TILEMAP) += -I./vmap/tilemap

ldflags-y += -lenet

CFLAGS += $(cflags-y)
LDFLAGS += $(ldflags-y)
