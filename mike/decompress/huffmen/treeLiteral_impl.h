#ifndef MIKE_DECODE_HUFFMEN_TREELITERAL_IMPL_H
#define MIKE_DECODE_HUFFMEN_TREELITERAL_IMPL_H

#include <stdint.h>

/* imo this is more cumbersome than just using bitwise operations on a single num
struct mike_decode_Huffmen_TreeLiteral_Entry {
	uint8_t right;
	uint8_t left;

	uint8_t rightLSB : 1;
	uint8_t leftLSB : 1;

	uint8_t rightIsNode : 1;
	uint8_t leftIsNode : 1;
};
*/

#define mike_decode_huffmen_TreeLiteral_LENGTH 287

#define mike_decode_huffmen_TreeLitreral_ENTRYSIZE 3

#define mike_decode_huffmen_TreeLitral_SHIFT_RIGHTBRANCH 0
#define mike_decode_huffmen_TreeLitral_SHIFT_RIGHTTYPE 9
#define mike_decode_huffmen_TreeLitral_SHIFT_LEFTBRANCH 10
#define mike_decode_huffmen_TreeLitral_SHIFT_LEFTTYPE 19
// shift then mask
#define mike_decode_huffmen_TreeLitral_MASK_BRANCH 0x01ff
#define mike_decode_huffmen_TreeLitral_MASK_TYPE 0x01

#define mike_decode_huffmen_TreeLitral_NULL 0x01ff
#define mike_decode_huffmen_TreeLitral_TYPE_NODE 0
#define mike_decode_huffmen_TreeLitral_TYPE_VALUE 1

struct mike_decode_Huffmen_TreeLiteral {
	uint8_t data[mike_decode_Huffmen_TreeLiteral_ENTRYSIZE * mike_decode_Huffmen_TreeLiteral_LENGTH];
	/*
		every ENTRYSIZE bytes = 1 entry
		the bytes appear MSB to LSB
	*/
};

#endif
