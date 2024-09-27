#ifndef GLASS_DEFILTER_H
#define GLASS_DEFILTER_H

#include "Glass/ihdr_forw.h"

#include "utils/iByteTrain_forw.h"
#include "utils/iByteLayer_forw.h"

int Glass_Defilter_go(Glass_Ihdr ihdr, iByteTrain *bt, iByteLayer *bl);

#endif
