#ifndef WORLDS_LOGGER_H
#define WORLDS_LOGGER_H

#include <stdarg.h>
#include <stdio.h>

#ifdef CONFIG_LOGGER
#define debug_print(o, level, msg, ...) \
	fprintf(o, "%-7s [%s:%d]: " msg , #level, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug_print(file, level, msg, ...)
#endif

#define log_trace(msg, ...)	debug_print(stdout, TRACE, msg, ##__VA_ARGS__)
#define log_dbg(msg, ...)	debug_print(stdout, DEBUG, msg, ##__VA_ARGS__)
#define log_info(msg, ...)	debug_print(stdout, INFO, msg, ##__VA_ARGS__)
#define log_warn(msg, ...)	debug_print(stderr, WARNING, msg, ##__VA_ARGS__)
#define log_err(msg, ...)	debug_print(stderr, ERROR, msg, ##__VA_ARGS__)
#define log_fatal(msg, ...)	debug_print(stderr, FATAL, msg, ##__VA_ARGS__)

#endif
