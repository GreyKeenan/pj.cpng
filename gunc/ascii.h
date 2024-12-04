#ifndef GUNC_ASCII_H
#define GUNC_ASCII_H

#include <stdbool.h>

#define Gunc_Ascii_ISCAPITAL 0x20
#define Gunc_Ascii_NOTASCII 0x80

bool Gunc_Ascii_isWhitespace(char c);

static inline bool Gunc_Ascii_isLetter(char c) {
	return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}
static inline bool Gunc_Ascii_isNumeric(char c) {
	return '0' <= c && c <= '9';
}

#endif
