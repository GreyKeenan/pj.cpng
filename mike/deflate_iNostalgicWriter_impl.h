#ifndef MIKE_DEFLATE_INOSTALGICWRITER_IMPL_H
#define MIKE_DEFLATE_INOSTALGICWRITER_IMPL_H

#include <stdint.h>

struct Mike_Deflate_iNostalgicWriter {
	void *vself;

	int (*write)(void *vself, uint8_t byte);
	/*
		writes the byte to some sort of output stream
		returns 0 on success
			returns Mike_Deflate_iNostalgicWriter_TOOFAR if unable to continue writing because of some length limit
	*/
	int (*nostalgize)(const void *vself, uint8_t *destination, uint32_t distanceBack);
	/*
		gives the byte located (distanceBack) bytes back from the current writing position
			current writing position is where the next byte will be written to, not where the previous byte was written.
		returns 0 on success
			returns Mike_Deflate_iNostalgicWriter_TOOFAR if currentPosition - distanceBack < 0
			returns positive value on other error
	*/
};

#endif
