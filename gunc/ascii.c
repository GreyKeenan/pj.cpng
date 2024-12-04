#include "./ascii.h"

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

