#ifndef SRHUB_Tree_H
#define SRHUB_Tree_H

#include <stdint.h>
#include <stdbool.h>

#define Shrub_Tree_ROOT 0
	//ROOT doubles as NULL, when child is type NODE, since nothing points back to the root

#define Shrub_Tree_NODE 0
#define Shrub_Tree_LEAF 1

struct Shrub_Tree {
	uint8_t *data;
	uint16_t maxNodes;
	uint16_t currentNodes;
	uint8_t bitsPerChild; // including 1b type
	uint8_t bytesPerNode;
};


int Shrub_Tree_init(struct Shrub_Tree *self, uint8_t *data, uint16_t length, uint16_t maxLeaves);
/*
	data: pointer to amt of memory that can hold the tree
	length: length of the data in bytes, to verify it is large enough
	maxLeaves: how many leaves it should be able to support

	TODO? measure() function which uses maxLeaves to determine necessary data size, as the init() function does internally anyways

	initializes the tree with
		maxNodes (maxLeaves - 1)
		bitsPerChild (log2(maxLeaves) + 1)
		bytesPerNode (ceil(bitsPerChild * 2 / 8))

		the ROOT node initialized to null

	returns 0 on success
*/

enum Shrub_Tree_status {
	Shrub_Tree_ERROR = 1
	, Shrub_Tree_OUTOFBOUNDS
	, Shrub_Tree_HALT
	, Shrub_Tree_TOOMUCH
	, Shrub_Tree_BADCODE
};


int Shrub_Tree_walk(const struct Shrub_Tree *self, bool isRight, uint16_t fromIndex, uint16_t *destination);
/*
	gives left/right value of $fromIndex node to $destination
		bit0 of child indicates LEAF vs NODE
		if == 1, is LEAF
		bitshift >> 1 to get value after that
		*wont* give null node
	returns 0 on success
		returns HALT if child value == null
		returns OUTOFBOUNDS if $fromIndex out of bounds
*/

int Shrub_Tree_birthNode(struct Shrub_Tree *self, bool isRight, uint16_t parent, uint16_t *nNewborn);
/*
	creates new node left/right of $parent
		& sets $parent left/right child value as proper index
	gives index of new node to $nNewborn
	returns 0 on success
		returns HALT if $parent already has a left/right child (not null)
		returns OUTOFBOUNDS if $parent is out of bounds
		returns TOOMUCH if tree already at maxNodes
*/

int Shrub_Tree_growLeaf(struct Shrub_Tree *self, bool isRight, uint16_t parent, uint16_t value);
/*
	sets $parent left/right child as $value
	returns 0 on success
		returns HALT if $parent already has a left/right child (not null)
		returns OUTOFBOUNDS if $parent is out of bounds
		returns TOOMUCH if $value doesnt fit in available bitsize
*/

int Shrub_Tree_enterCode(struct Shrub_Tree *self, uint16_t code, uint16_t length, uint16_t leaf);
/*
	using walk, birthNode, growLeaf to build the tree according to code and place $leaf at the end
	reads the code from MSbit toLSbit, to work with DEFLATE
	length must be > 0 and < 17, to fit in the uint16_t
		length must be <= maxNodes to fit in the data array
	returns 0 on success
		returns BADCODE if code invalid
		passes out error vals from subcalls
*/

#endif
