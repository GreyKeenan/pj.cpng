#ifndef GUNC_OVERFLOW_H
#define GUNC_OVERFLOW_H

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>


static inline bool Gunc_uCantMultiply(uint64_t a, uint64_t b, uint64_t limit) {
	return b > 0 && a > limit / b;
}
/*
static inline bool Gunc_sCantMultiply(int64_t a, int64_t b, uint64_t max, int64_t min) {
}

static inline bool Gunc_uCantAdd(uint64_t a, uint64_t b, uint64_t limit) {
}
static inline bool Gunc_sCantAdd(int64_t a, int64_t b, uint64_t max, int64_t min) {
}

static inline bool Gunc_uCantSubtract(uint64_t a, uint64_t b, uint64_t limit) {
}
static inline bool Gunc_sCantSubtract(int64_t a, int64_t b, uint64_t max, int64_t min) {
}
*/

#endif
