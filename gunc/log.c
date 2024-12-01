#include "./log.h"

#include <stdio.h>
#include <stdarg.h>

void Gunc_log_f(
	int e,
	const char *type,
	const char *functionname,
	const int linenumber,
	const char *message,
	int va_count,
	...
) {

	va_list vls;
	va_start(vls, va_count);

	fprintf(stderr, "\n%s", type);
	if (e) {
		fprintf(stderr, " (%d)", e);
	}
	fprintf(stderr, ": %s()\n %d | ", functionname, linenumber);
	vfprintf(stderr, message, vls);
	fprintf(stderr, "\n");

	va_end(vls);
}
