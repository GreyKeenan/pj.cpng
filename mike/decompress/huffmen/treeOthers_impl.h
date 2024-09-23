#ifndef MIKE_DECOMPRESS_HUFFMEN_TREEOTHERS_IMPL_H
#define MIKE_DECOMPRESS_HUFFMEN_TREEOTHERS_IMPL_H

#include <stdint.h>

#define mike_decompress_huffmen_TreeOthers_ENTRYSIZE 2

#define mike_decompress_huffmen_TreeOthers_SHIFT_RIGHTBRANCH 0
#define mike_decompress_huffmen_TreeOthers_SHIFT_RIGHTTYPE 6
#define mike_decompress_huffmen_TreeOthers_SHIFT_LEFTBRANCH 7
#define mike_decompress_huffmen_TreeOthers_SHIFT_LEFTTYPE 13

#define mike_decompress_huffmen_TreeOthers_MASK_BRANCH 0x3f
#define mike_decompress_huffmen_TreeOthers_MASK_TYPE 0x01

#define mike_decompress_huffmen_TreeOthers_NULL 0x3f
#define mike_decompress_huffmen_TreeOthers_TYPE_NODE 0
#define mike_decompress_huffmen_TreeOthers_TYPE_VALUE 1


#define mike_decompress_huffmen_TreeMeta_LENGTH 18
#define mike_decompress_huffmen_TreeDistance_LENGTH 29

struct mike_decompress_Huffmen_TreeMeta {
	uint8_t data[mike_decompress_huffmen_TreeMeta_LENGTH * mike_decompress_huffmen_TreeOthers_ENTRYSIZE];
};
struct mike_decompress_Huffmen_TreeDistance {
	uint8_t data[mike_decompress_huffmen_TreeDistance_LENGTH * mike_decompress_huffmen_TreeOthers_ENTRYSIZE];
};
/*
	every ENTRYSIZE bytes is a new entry.
	entries' bytes appear MSB to LSB
*/


#endif
