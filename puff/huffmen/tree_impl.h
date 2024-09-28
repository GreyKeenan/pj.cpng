#ifndef PUFF_TREE_IMPL_H
#define PUFF_TREE_IMPL_H

#include <stdint.h>

#define Puff_Tree_TYPE_NODE 0
#define Puff_Tree_TYPE_LEAF 1
#define Puff_Tree_ROOT 0

struct Puff_Tree {
/*
	O(log n) lookup of codes

	there is wasted storage space to byte-align nodes in the array

	node bit layout
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	[leading high-bits for padding][childBitLength-bit rightChild][1-bit rightChildType][childBitLength-bit leftchild][1-bit leftChildType]

	expected to load the bytes into an uint32 & bitshift/mask to get out values

	bytes are in LSB-first order

	leading high-bits may or may not be 0s

	NULL value consists of a type of NODE + a value of ROOT
		since nothing points back to the ROOT

*/

	uint8_t *data;
	/*
		references an array of data that should be able to hold nodeCount * nodeByte bytes

		will not be allocated or freed by Tree

		external node indexes passed around by funcs index by nodeCount, not by actual bytes in arr
	*/

	uint16_t maxNodeCount;
	uint16_t nodeCount;
	/*
		num of nodes
		is num of leaves/values - 1
	*/
	uint8_t nodeBytes;
	/*
		num of bytes each node occupies

		should be >= ceil((childBitLength * 2 + 2)/8)
		should be <= 4
			so can load handle the node within a uint32
	*/

	uint8_t childBitLength;
	/*
		num of bits the child values occupy
		used to shift them out of node'g bytes
	*/
};

#endif
