/*  Copyright 2001 Rien Croonenborghs, Ben Kibbey, Shaun Jackman, Ivan Brozovic
 *
 *  This file is part of lcab.
 *  lcab is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  lcab is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with lcab; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "warn.h"
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

int current_level = DEBUG;
FILE *_out;
FILE *_err;

void init_logging(FILE *out, FILE *err)
{
	_out = out;
	_err = err;
}

void set_log_level(log_level level)
{
	current_level = level;
}

const char *level_name(log_level l)
{
	switch (l) {
	case TRACE:
		return "trace";
		break;
	case DEBUG:
		return "debug";
		break;
	case INFO:
		return "info";
		break;
	case WARN:
		return "warn";
		break;
	case ERROR:
		return "error";
		break;
	case FATAL:
		return "fatal";
		break;
	default:
		return " ";
		break;
	}
}

void _log(log_level level, FILE *to, const char *fmt, va_list ap)
{
	if (level > current_level) return;

	if (current_level > INFO) {
		fprintf(to, "%5s ", level_name(level));
	} else if (current_level <= INFO) {
		if (level == WARN)
			fprintf(to, "Warning: ");
		if (level == ERROR)
			fprintf(to, "Error: ");
		if (level == FATAL)
			fprintf(to, "Fatal: ");
	}
	if (fmt != 0)
		vfprintf(to, fmt, ap);

	if (errno)
		fprintf(to, " : %s", strerror(errno));

	fprintf(to, "\n");
}


#define LOG_AT_LEVEL(_fn_, _level_, _stream_) \
	void _fn_(const char *fmt, ...) { \
		va_list argptr; \
		va_start(argptr, fmt); \
		_log(_level_, _stream_, fmt, argptr); \
		va_end(argptr); \
	}

#define OUT _out ? _out : stdout
#define ERR _err ? _err : stderr

LOG_AT_LEVEL(trace, TRACE, OUT)
LOG_AT_LEVEL(debug, DEBUG, OUT)
LOG_AT_LEVEL(info, INFO, OUT)
LOG_AT_LEVEL(warn, WARN, ERR)
LOG_AT_LEVEL(error, ERROR, ERR)
LOG_AT_LEVEL(fatal, FATAL, ERR)
