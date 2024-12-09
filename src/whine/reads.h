#ifndef WHINE_READS_H
#define WHINE_READS_H

#include <stdint.h>

struct Gunc_iByteStream;

int Whine_read_int32(struct Gunc_iByteStream *bs, uint32_t *destination);
/*
	reads a big-endian int32
	writes to $destination as proper endianness
	returns 0 on success
*/

#endif
