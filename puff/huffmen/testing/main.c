#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "../tree_impl.h"
#include "../tree.h"

//#include "../fixedTree.h"
//#include "../literalTree_impl.h"
#include "../metaTree.h"
#include "../metaTree_impl.h"

void printTree(struct Puff_Tree tree);

int main() {

	struct Puff_MetaTree tree = {0};

	//uint8_t lengths[Puff_MetaTree_MAXLEAVES] = {3, 3, 3, 3, 3, 2, 4, 4};
	uint8_t lengths[Puff_MetaTree_MAXLEAVES] = { 4, 6, 0, 3, 5, 5, 6, 5, 6, 6, 5, 5, 3, 5, 2, 0, 3, 0, 0 };

	int e = Puff_MetaTree_init(&tree, lengths, 19);
	printf("e%d\n", e);

	printTree(tree.tree);

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
