#ifndef MIKE_DECHUNK_H
#define MIKE_DECHUNK_H

#include "mike/decompress/iNostalgicWriter_forw.h"
#include "mike/ihdr_forw.h"

#include "utils/iByteTrain_forw.h"

int mike_Dechunk_go(iByteTrain *bt, mike_Ihdr *ihdr, Mike_Decompress_iNostalgicWriter *nw);

#endif
