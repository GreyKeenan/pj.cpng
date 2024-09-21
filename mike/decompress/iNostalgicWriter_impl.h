#ifndef MIKE_DECOMPRESS_INOSTALGICWRITER_IMPL_H
#define MIKE_DECOMPRESS_INOSTALGICWRITER_IMPL_H

#include <stdint.h>

#define Mike_Decompress_iNostalgicWriter_TOOFAR -1

struct Mike_Decompress_iNostalgicWriter {
	void *vself;

	int (*write)(void *vself, uint8_t byte);
	/*
		should be identical to iByteLayer.lay()
	*/
	int (*nostalgize)(const void *vself, uint8_t *destination, uint32_t distanceBack);
	/*
		gives the byte located (distanceBack) bytes back from the current writing position
			current writing position is where the next byte will be written to, not where the previous byte was written.
		returns 0 on success
			returns Mike_Decompress_iNostalgicWriter_TOOFAR if currentPosition - distanceBack < 0
			returns positive value on other error
	*/
};

#endif
