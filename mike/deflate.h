#ifndef MIKE_DEFLATE_H
#define MIKE_DEFLATE_H

#include "./deflate_forw.h"

#include <stdint.h>

enum Mike_Deflate_Error {
	MIKE_DEFLATE_END = -101

	, MIKE_DEFLATE_ERROR = -100
	, MIKE_DEFLATE_ERROR_STATE_UNKNOWN

	, MIKE_DEFLATE_ERROR_ZLIBHEADER_OVERREAD
	, MIKE_DEFLATE_ERROR_ZLIBHEADER_COMPRESSIONMETHOD
	, MIKE_DEFLATE_ERROR_ZLIBHEADER_WINDOWSIZE
	, MIKE_DEFLATE_ERROR_ZLIBHEADER_VALIDATION
	, MIKE_DEFLATE_ERROR_ZLIBHEADER_HASDICT

	, MIKE_DEFLATE_ERROR_BLOCKHEADER_OVERREAD
	, MIKE_DEFLATE_ERROR_BLOCKHEADER_COMPRESSIONTYPE

	, MIKE_DEFLATE_ERROR_UNCOMPRESSED_LENGTH_OVERREAD
	, MIKE_DEFLATE_ERROR_UNCOMPRESSED_NLEN
	, MIKE_DEFLATE_ERROR_UNCOMPRESSED_OVERREAD
};

/*
int Mike_Deflate_walk(struct Mike_Deflate_State *state, iByteTrain *bt, uint32_t steps);
	intent:
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	interprets [steps] bytes from the byteTrain as a zlib stream
	[steps] may stop before end. Subsequent calls will pick up where left off.

	to determine:
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	storing output?
	storing state of deflation? and returning to it?
	static inline?

	so basically all of it, yk.
*/
int Mike_Deflate_step(Mike_Deflate_State *state, uint8_t byte);

#endif
