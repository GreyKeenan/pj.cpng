#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "../tree_impl.h"
#include "../tree.h"

void printTree(struct Mike_Decompress_Huffmen_Tree tree);

int main() {
	int e = 0;
	uint16_t db = 0;

	struct Mike_Decompress_Huffmen_Tree tree = {0};

	#define CAP 1000
	uint8_t data[CAP];

	e = Mike_Decompress_Huffmen_Tree_init(&tree, data, CAP, 287, 3, 9);
	if (e) {
		printf("error: %d\n", e);
	}

	printTree(tree);

	//e = Mike_Decompress_Huffmen_Tree_walk(&tree, 0, 1, &db);
	e = Mike_Decompress_Huffmen_Tree_growLeaf(&tree, 0, true, 1);
	printf("%d\n", e);

	printTree(tree);

	e = Mike_Decompress_Huffmen_Tree_birthNode(&tree, 0, false, &db);
	printf("%d\n", e);

	printTree(tree);


	return 0;
}


void printTree(struct Mike_Decompress_Huffmen_Tree tree) {
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
