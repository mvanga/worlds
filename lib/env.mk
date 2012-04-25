# Main
obj-y +=  main.o \
	list.o \
	entity.o \
	player.o \
	command.o \
	vmap/vmap.o \
	vmap/tilemap/tilemap.o \
	net/net.o \

obj-$(CONFIG_NET_TCP) += \
	net/tcp/tcp_server.o \

obj-$(CONFIG_NET_ENET) += \
	net/enet/enet_server.o \

cflags-y += -DCONFIG_BUFSIZE=2048
cflags-y += -I./include -I. -I./vmap -I./vmap/tilemap -I./net -Wall -Wextra -ggdb
cflags-y += -D_GNU_SOURCE
cflags-y += -O3 -Wno-unused-parameter

cflags-$(CONFIG_NET_TCP) += -I./net/tcp -DCONFIG_NET_TCP
cflags-$(CONFIG_NET_ENET) += -I./net/enet -DCONFIG_NET_ENET

ldflags-y += -lenet

CFLAGS += $(cflags-y)
LDFLAGS += $(ldflags-y)
