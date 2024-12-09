#ifndef SHRUB_FixedTree_H
#define SHRUB_FixedTree_H

struct Shrub_LitTree;
extern const struct Shrub_LitTree *Shrub_nFIXEDTREE; // = NULL

int Shrub_FixedTree_init(void);
/*
builds DEFLATE fixed tree
returns 0 if successful, or if already initted
if errs, Shrub_nFIXEDTREE is unchanged
if returns 0, can assume FIXEDTREE != NULL
*/

#endif
