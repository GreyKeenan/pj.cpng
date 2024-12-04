#ifndef SHRUB_FixedTree_H
#define SHRUB_FixedTree_H

struct Shrub_LitTree;
extern const struct Shrub_LitTree *Shrub_FTREE; // = NULL

int Shrub_FixedTree_init(void);
/*
builds DEFLATE fixed tree
returns 0 if successful, or if already initted
fi errs, Shrub_FTREE is unchanged
*/

#endif
