#ifndef MIKE_DEFILTER_H
#define MIKE_DEFILTER_H

#include "mike/ihdr_forw.h"

#include "utils/iByteTrain_forw.h"
#include "utils/iByteLayer_forw.h"

int mike_Defilter_go(mike_Ihdr ihdr, iByteTrain *bt, iByteLayer *bl);

#endif
