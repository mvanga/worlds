#ifndef MMO_MODULE_H
#define MMO_MODULE_H

#include "ccan/autodata.h"

#include <stdio.h>

#define MODULES		0
#define SUBSYSTEMS	1

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

#define modules_init()	__modules_init(MODULES);
#define subsys_init()	__modules_init(SUBSYSTEMS);
#define modules_exit()	__modules_exit(MODULES);
#define subsys_exit()	__modules_exit(SUBSYSTEMS);

static int __attribute__((unused)) __modules_init(int type)
{
	unsigned int i;
	size_t num;
	struct module **mods;

	switch (type) {
	case SUBSYSTEMS:
		mods = autodata_get(subsystems, &num);
		break;
	case MODULES:
		mods = autodata_get(modules, &num);
		break;
	default:
		return -1;
	}
	for (i = 0; i < num; i++) {
		int ret = 0;
		if (mods[i]->init)
			ret = mods[i]->init();
		printf("%s: init: %s: %s\n",
			type == SUBSYSTEMS ? "subsys" : "module",
			mods[i]->name,
			(ret < 0) ? "FAIL" : "OK");
		fflush(0);
		if (ret < 0)
			return ret;
	}
	autodata_free(mods);

	return 0;
}

static int __attribute__((unused)) __modules_exit(int type)
{
	int i;
	size_t num;
	struct module **mods;

	switch (type) {
	case SUBSYSTEMS:
		mods = autodata_get(subsystems, &num);
		break;
	case MODULES:
		mods = autodata_get(modules, &num);
		break;
	default:
		return -1;
	}
	for (i = (int)num - 1; i >= 0; i--) {
		printf("%s: exit: %s\n",
			type == SUBSYSTEMS ? "subsys" : "module",
			mods[i]->name);
		fflush(0);
		if (mods[i]->exit)
			mods[i]->exit();
	}
	autodata_free(mods);

	return 0;
}


#endif
