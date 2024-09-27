#ifndef GLASS_DECHUNK_H
#define GLASS_DECHUNK_H

struct iByteTrain;
struct Puff_iNostalgicWriter;
struct Glass_Ihdr;

int Glass_dechunk(struct iByteTrain *bt, struct Glass_Ihdr *ihdr, struct Puff_iNostalgicWriter nw);

#endif
