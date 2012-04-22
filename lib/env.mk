# Main
obj-y +=  main.o \
	list.o \
	entity.o \
	player.o \
	map.o \
	command.o \

cflags-y += -DCONFIG_BUFSIZE=2048
cflags-y += -I./include -Wall -Wextra -ggdb
cflags-y += -D_GNU_SOURCE
cflags-y += -O3 -Wno-unused-parameter

ldflags-y += -lenet

CFLAGS += $(cflags-y)
LDFLAGS += $(ldflags-y)
