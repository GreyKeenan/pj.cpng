#ifndef GUNC_PP_H
#define GUNC_PP_H

#define Gunc_PP_ERROR

#define Gunc_PP_COUNT_N(...) (sizeof((int[]){0, __VA_ARGS__}) / sizeof(int) - 1)

#define Gunc_PP_UNSEQ_4(_1, _2, _3, _4, r, ...) r
#define Gunc_PP_NSEQ_4 4, 3, 2, 1, 0
#define Gunc_PP_MSEQ_4 MANY, MANY, MANY, ONE, ZERO
#define Gunc_PP_JSEQ_4(...) Gunc_PP_UNSEQ_4(__VA_ARGS__)
#define Gunc_PP_COUNT_4(...) Gunc_PP_JSEQ_4(__VA_ARGS__, Gunc_PP_NSEQ_4)
#define Gunc_PP_ONEMANY_4(...) Gunc_PP_JSEQ_4(__VA_ARGS__, Gunc_PP_MSEQ_4)

#define Gunc_PP_UNSEQ_32( \
	_1, _2, _3, _4, _5, _6, _7, _8, \
	_9, _10, _11, _12, _13, _14, _15, _16, \
	_17, _18, _19, _20, _21, _22, _23, _24, \
	_25, _26, _27, _28, _29, _30, _31, _32, \
	r, ...) r
#define Gunc_PP_NSEQ_32 \
	32, 31, 30, 29, 28, 27, 26, 25, \
	24, 23, 22, 21, 20, 19, 18, 17, \
	16, 15, 14, 13, 12, 11, 10, 9, \
	8, 7, 6, 5, 4, 3, 2, 1, 0
#define Gunc_PP_MSEQ_32 \
	MANY, MANY, MANY, MANY, MANY, MANY, MANY, MANY, \
	MANY, MANY, MANY, MANY, MANY, MANY, MANY, MANY, \
	MANY, MANY, MANY, MANY, MANY, MANY, MANY, MANY, \
	MANY, MANY, MANY, MANY, MANY, MANY, MANY, ONE, ZERO
#define Gunc_PP_JSEQ_32(...) Gunc_PP_UNSEQ_32(__VA_ARGS__)
#define Gunc_PP_COUNT_32(...) Gunc_PP_JSEQ_32(__VA_ARGS__, Gunc_PP_NSEQ_32)
#define Gunc_PP_ONEMANY_32(...) Gunc_PP_JSEQ_32(__VA_ARGS__, Gunc_PP_MSEQ_32)


#define Gunc_PP_JOIN_NOEXEC(a, b) a##b
#define Gunc_PP_JOIN_EXEC(a, b) Gunc_PP_JOIN_NOEXEC(a, b)

#endif
