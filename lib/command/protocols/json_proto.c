#include <worlds/module.h>
#include <worlds/command.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *proto_json_identify(struct dictionary *d)
{
	char *buf;
	JsonNode *n;

	n = json_find_member(d->json, "command");
	if (!n)
		return NULL;
	if (n->tag != JSON_STRING) {
		json_delete(n);
		return NULL;
	}
	buf = malloc(strlen(n->string_) + 1);
	if (!buf) {
		json_delete(n);
		return NULL;
	}
	strcpy(buf, n->string_);
	json_delete(n);

	return buf;
}

struct dictionary *proto_json_normalize(int len, unsigned char *data)
{
	struct dictionary *d;
	const char *json = (const char *)data;

	if (!json_validate((const char *)data)) {
		printf("invalid json\n");
		return NULL;
	}

	d = malloc(sizeof(struct dictionary));
	if (!d)
		return NULL;
	d->json = json_decode(json);
	if (!d->json) {
		printf("failed to decode json string\n");
		free(d);
		return NULL;
	}

	return d;
}

struct command_protocol json_proto = {
	.name = "json",
	.identify = proto_json_identify,
	.normalize = proto_json_normalize,
};

int json_proto_mod_init(void)
{
	return command_protocol_register(&json_proto);
}

void json_proto_mod_exit(void)
{
	command_protocol_register(&json_proto);
}

#ifdef CONFIG_COMMAND_PROTOCOL_JSON
static struct module mod = {
	.name = "command:proto:json",
	.author = "Manohar Vanga",
	.description = "JSON messaging protocol support",
	.init = json_proto_mod_init,
	.exit = json_proto_mod_exit,
};
MODULE_REGISTER(&mod);
#endif
