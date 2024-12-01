#ifndef GUNC_LOG_H
#define GUNC_LOG_H

#include "./pp.h"

//#define Gunc_LOWLOG(s, ...) Gunc_log_f(s, __func__, __LINE__, Gunc_PP_COUNTARGS_UNCAPPED(__VA_ARGS__), __VA_ARGS__)
//#define Gunc_log(...) Gunc_LOWLOG(__VA_ARGS__, 0)

#define Gunc_LOG_ONE(...) __VA_ARGS__, 0
#define Gunc_LOG_MANY(s, ...) s, Gunc_PP_COUNT_32(__VA_ARGS__), __VA_ARGS__

#define Gunc_warn(...) Gunc_log_f(0, "WARNING", __func__, __LINE__, Gunc_PP_JOIN_EXEC(Gunc_LOG_, Gunc_PP_ONEMANY_32(__VA_ARGS__))(__VA_ARGS__))
#define Gunc_err(...) Gunc_log_f(0, "ERROR", __func__, __LINE__, Gunc_PP_JOIN_EXEC(Gunc_LOG_, Gunc_PP_ONEMANY_32(__VA_ARGS__))(__VA_ARGS__))

#define Gunc_nwarn(e, ...) Gunc_log_f(e, "WARNING", __func__, __LINE__, Gunc_PP_JOIN_EXEC(Gunc_LOG_, Gunc_PP_ONEMANY_32(__VA_ARGS__))(__VA_ARGS__))
#define Gunc_nerr(e, ...) Gunc_log_f(e, "ERROR", __func__, __LINE__, Gunc_PP_JOIN_EXEC(Gunc_LOG_, Gunc_PP_ONEMANY_32(__VA_ARGS__))(__VA_ARGS__))

#ifdef DEBUG
#define Gunc_say(...) Gunc_log_f(0, "INFO", __func__, __LINE__, Gunc_PP_JOIN_EXEC(Gunc_LOG_, Gunc_PP_ONEMANY_32(__VA_ARGS__))(__VA_ARGS__))
#else
#define Gunc_say(...)
#endif


void Gunc_log_f(
	int e,
	const char *type,
	const char *functionname,
	const int linenumber,
	const char *message,
	int va_count,
	...
);

#endif
