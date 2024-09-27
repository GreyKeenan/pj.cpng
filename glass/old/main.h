#ifndef GLASS_MAIN_H
#define GLASS_MAIN_H

#include "./scanlineImage_forw.h"

#include "utils/iByteTrain_forw.h"

int Glass_decode(iByteTrain *bt, Glass_ScanlineImage *destination);

#endif
