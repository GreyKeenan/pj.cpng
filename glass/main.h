#ifndef GLASS_MAIN_H
#define GLASS_MAIN_H

struct Glass_ScanlineImage;
struct iByteTrain;

int Glass_decode(struct iByteTrain *bt, struct Glass_ScanlineImage *destination);

#endif
