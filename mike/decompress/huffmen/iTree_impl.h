#ifndef MIKE_DECOMPRESS_HUFFMEN_iTREE_IMPL_H
#define MIKE_DECOMPRESS_HUFFMEN_iTREE_IMPL_H

#include <stdint.h>

// -1 ... -16
#define Mike_Decompress_Huffmen_iTree_UNSPECIFIEDCOLLISION -17
#define Mike_Decompress_Huffmen_iTree_LENGTH -18
#define Mike_Decompress_Huffmen_iTree_VALUE -19

#define Mike_Decompress_Huffmen_iTree_STUB -20

struct Mike_Decompress_Huffmen_iTree {
	void *vself;

	int (*addCode)(void *vself, uint16_t code, uint8_t codeLength, uint16_t value);
	/*
		code is read from LSbit to MSbit
	
		returns 0 on success
			
			returns -1 ... -16 to indicate a collision
				shows which step of the code the collision occured on
			returns UNSPECIFIEDCOLLISION to indicate a collision without specifying

			returns LENGTH if codeLength is invalid
			returns VALUE if value is invalid

			returns positive value on vself-specific error
	*/

	int (*walk)(void *vself, _Bool side, uint16_t *nValue);
	/*
		walks r/l down the tree until it hits a value
			starts at root
			walks r/l according to 'side' each call
			when hits a value, returns value to *nValue
			when hits NULL r/l, returns err (below)

		returns:
			0 if walked successfully to the next node & it was not a value
			VALUE if encountered a leaf
			STUB if node trying to walk to is NULL

			positive value on vself-specific error

		for any non-0 return, current node should be set back to the root
	*/

	/*
	'value' for both functions should refer to the same format/type/unit.
	*/
};

#endif
