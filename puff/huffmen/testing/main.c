#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "../tree_impl.h"
#include "../tree.h"

#include "../fixedTree.h"
#include "../literalTree_impl.h"

void printTree(struct Puff_Tree tree);

int main() {
	/*
	int e = 0;
	uint16_t db = 0;

	struct Puff_Tree tree = {0};

	#define CAP 3 * 287
	uint8_t data[CAP];

	e = Puff_Tree_init(&tree, data, CAP, 288);
	printf("%d\n", e);

	printTree(tree);

	//e = Puff_Tree_walk(&tree, 0, 1, &db);
	e = Puff_Tree_growLeaf(&tree, 0, true, 1);
	printf("%d\n", e);

	printTree(tree);

	e = Puff_Tree_birthNode(&tree, 0, false, NULL);
	printf("%d\n", e);

	printTree(tree);
	*/

	struct Puff_LiteralTree fixedTree = {0};

	int e = Puff_FixedTree_init(&fixedTree);
	printf("e%d\n", e);


	return 0;
}


void printTree(struct Puff_Tree tree) {
	printf(
		"Tree {"
		"\n	data: %p"
		"\n	maxNodeCount: %d"
		"\n	nodeCount: %d"
		"\n	nodeBytes: %d"
		"\n	childBitLength: %d"
		"\n}"
		"\n"
		, tree.data
		, tree.maxNodeCount
		, tree.nodeCount
		, tree.nodeBytes
		, tree.childBitLength
	);

	for (uint32_t i = 0; i < tree.nodeCount * tree.nodeBytes; ++i) {
		printf("[%d]: 0x%x\n", i, tree.data[i]);
	}
}
