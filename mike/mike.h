#ifndef MIKE_H
#define MIKE_H

#include "./scanlineImage_forw.h"

#include "utils/iByteTrain_forw.h"

int Mike_decode(iByteTrain *bt, Mike_ScanlineImage *destination);

#endif
