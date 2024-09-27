#ifndef GLASS_DECHUNK_H
#define GLASS_DECHUNK_H

#include "./ihdr_forw.h"

#include "puff/iNostalgicWriter_forw.h"

#include "utils/iByteTrain_forw.h"

int Glass_Dechunk_go(iByteTrain *bt, Glass_Ihdr *ihdr, Puff_iNostalgicWriter *nw);

#endif
