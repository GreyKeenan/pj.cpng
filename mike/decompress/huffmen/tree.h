#ifndef MIKE_DECOMPRESS_HUFFMEN_TREE_H
#define MIKE_DECOMPRESS_HUFFMEN_TREE_H

#include <stdint.h>

struct Mike_Decompress_Huffmen_Tree;


int Mike_Decompress_Huffmen_Tree_init(struct Mike_Decompress_Huffmen_Tree *self, uint8_t *data, uint16_t cap, uint16_t uniqueValueCount, uint8_t nodeBytes, uint8_t childBitLength);
/*
	validates values
		see definition for details
	initializes ROOT node w/ NULL children

	returns 0 on success
		see definition implementation for validation error return vals
*/

#define Mike_Decompress_Huffmen_Tree_ISLEAF 2
#define Mike_Decompress_Huffmen_Tree_ISNODE 1
#define Mike_Decompress_Huffmen_Tree_COLLISION -1
#define Mike_Decompress_Huffmen_Tree_OUTOFBOUNDS -2
#define Mike_Decompress_Huffmen_Tree_HALT -3
#define Mike_Decompress_Huffmen_Tree_TOOMANYKIDS -3
#define Mike_Decompress_Huffmen_Tree_BADVALUE -3

int Mike_Decompress_Huffmen_Tree_walk(const struct Mike_Decompress_Huffmen_Tree *self, uint16_t fromNode, _Bool handedness, uint16_t *destination);
/*
	gets the node/value of the child of fromNode
	returns:
		ISNODE if child is a node
			&& gives child index to *destination
		ISLEAF if child is a leaf
			&& gives value to *destination

		OUTOFBOUNDS if fromNode is out of range
			out of range defined as past nodeCount, even if within cap
		HALT if child is null index (root)
*/

int Mike_Decompress_Huffmen_Tree_birthChild(struct Mike_Decompress_Huffmen_Tree *self, uint16_t *fromNode, _Bool handedness);
/*
	creates new node as child of fromNode
	returns 0 on success & gives child index to *fromNode
		OUTOFBOUNDS if fromNode is out of range
			out of range defined as past nodeCount, even if within cap
		COLLISION if child value being set is not null
		TOOMANYKIDS if theres no more space in array for another node
*/

int Mike_Decompress_Huffmen_Tree_setLeafChild(struct Mike_Decompress_Huffmen_Tree *self, uint16_t fromNode, _Bool handedness, uint16_t value);
/*
	sets child of fromNode as leaf w/ value
	returns 0 on success
		OUTOFBOUNDS if fromNode is out of range
			out of range defined as past nodeCount, even if within cap
		COLLISION if child value being set is not null
		BADVALUE if value is invalid
			invalid defined here as: value > 2^self.childBitLength - 1
*/

#endif
