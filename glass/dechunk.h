#ifndef GLASS_DECHUNK_H
#define GLASS_DECHUNK_H

#include "Glass/decompress/iNostalgicWriter_forw.h"
#include "Glass/ihdr_forw.h"

#include "utils/iByteTrain_forw.h"

int Glass_Dechunk_go(iByteTrain *bt, Glass_Ihdr *ihdr, Glass_Decompress_iNostalgicWriter *nw);

#endif
