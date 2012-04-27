#ifndef MMO_MODULE_H
#define MMO_MODULE_H

#include "ccan/autodata.h"

struct module {
	const char *name;
	const char *author;
	const char *description;
	int (*init)(void);
	void (*exit)(void);
};

AUTODATA_TYPE(modules, struct module);
#define MODULE_REGISTER(m) \
	AUTODATA(modules, (m))

AUTODATA_TYPE(subsystems, struct module);
#define SUBSYSTEM_REGISTER(m) \
	AUTODATA(subsystems, (m))

#endif
