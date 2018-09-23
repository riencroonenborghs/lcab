#ifndef WARN_H
#define WARN_H
#include <stdio.h>

typedef enum {
	TRACE	= 99,
	DEBUG	= 76,
	INFO	= 51,
	WARN	= 26,
	ERROR	= 1,
	FATAL	= 0,
} log_level;
void init_logging(FILE *out, FILE *err);
void set_log_level(log_level level);

#define LOG_AT_LEVELH(_fn_) void _fn_(const char *fmt, ...);
LOG_AT_LEVELH(trace)
LOG_AT_LEVELH(debug)
LOG_AT_LEVELH(info)
LOG_AT_LEVELH(warn)
LOG_AT_LEVELH(error)
LOG_AT_LEVELH(fatal)
#endif
