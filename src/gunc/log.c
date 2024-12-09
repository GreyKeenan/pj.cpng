#include "./log.h"

#include <stdio.h>
#include <stdarg.h>

#define SETEXT_LENGTH 50

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

void Gunc_title_f(
	char h,
	//const char *functionname,
	//const int linenumber,
	const char *message,
	int va_count,
	...
) {

	va_list vls;
	va_start(vls, va_count);

	fprintf(stderr, "\n\n");
	vfprintf(stderr, message, vls);
	fprintf(stderr, "\n");
	for (int i = 0; i < SETEXT_LENGTH; ++i) {
		fprintf(stderr, "%c", h);
	}
	fprintf(stderr, "\n");

	va_end(vls);
}
