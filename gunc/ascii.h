#ifndef GUNC_ASCII
#define GUNC_ASCII

#include <stdbool.h>

#define Gunc_Ascii_ISCAPITAL 0x20
#define Gunc_Ascii_NOTASCII 0x80

bool Gunc_Ascii_isWhitespace(char c) {
	switch (c) {
		case '\t':
		case '\n':
		case 0x0b: //vertical tab
		case 0x0c: //form feed
		case '\r':
		case ' ':
			return true;
		default:
			return false;
	}
}

static inline bool Gunc_Ascii_isLetter(char c) {
	return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}
static inline bool Gunc_Ascii_isNumeric(char c) {
	return '0' <= c && c <= '9';
}

#endif
