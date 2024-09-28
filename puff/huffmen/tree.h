#ifndef PUFF_HUFFMEN_TREE_H
#define PUFF_HUFFMEN_TREE_H

#include <stdint.h>

struct Puff_Huffmen_Tree;


int Puff_Huffmen_Tree_init(struct Puff_Huffmen_Tree *self, uint8_t *data, uint16_t cap, uint16_t maxLeaves);
/*
	data: ptr to an amt of memory which will fit the tree
	cap: length of data's memory in bytes
	maxLeaves: the maximum number of leaves the tree can support / how many codes there will be if its a properly constructed huffman tree

	calculates
		maxNodes
		childBitLength
		nodeBytes
	validates that tree will fit within cap

	initializes the ROOT of the tree as NULL so is ready to birthNode/growLeaf

	returns 0 on success
		see definition for validation failure return values

	TODO
		a little weird because, in order to create the memory, you need maxNodes & nodeBytes values anyways.
		makes sense for the specific use case here, but in theory a measure(maxLeaves) function (or just allocating memory for within the struct) would probably be more effective
*/

#define Puff_Huffmen_Tree_ISLEAF 2
#define Puff_Huffmen_Tree_ISNODE 1

#define Puff_Huffmen_Tree_OUTOFBOUNDS -1
#define Puff_Huffmen_Tree_HALT -2
#define Puff_Huffmen_Tree_COLLISION -2
#define Puff_Huffmen_Tree_TOOMANYKIDS -3
#define Puff_Huffmen_Tree_BADVALUE -3

#define Puff_Huffmen_Tree_IMPOSSIBLE -100
	//returned when an error occurs where it shouldnt be possible

int Puff_Huffmen_Tree_walk(const struct Puff_Huffmen_Tree *self, uint16_t parentIndex, _Bool lr, uint16_t *destination);
/*
	gets the node/value of the child of parent node
	returns:
		ISNODE if child is a node
			&& gives child index to *destination
		ISLEAF if child is a leaf
			&& gives value to *destination

		OUTOFBOUNDS if fromNode is out of range
		HALT if child is null index (root)
*/

int Puff_Huffmen_Tree_birthNode(struct Puff_Huffmen_Tree *self, uint16_t parentIndex, _Bool lr, uint16_t *nNewborn);
/*
	creates new node as child of parent node
	returns 0 on success & gives newborn index to *nNewborn
		OUTOFBOUNDS if parentIndex is out of range
		COLLISION if attempting to set a child value that is not null
		TOOMANYKIDS if theres no more space in array for another node
*/

int Puff_Huffmen_Tree_growLeaf(struct Puff_Huffmen_Tree *self, uint16_t parentIndex, _Bool lr, uint16_t value);
/*
	sets child value of parent node as a LEAF
	returns 0 on success
		OUTOFBOUNDS if parentIndex is out of range
		COLLISION if attempting to set a child value that is not null
		BADVALUE if value is invalid / too large to fit within childBitLength
*/

#endif
