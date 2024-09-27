#ifndef GLASS_DEFILTER_H
#define GLASS_DEFILTER_H

struct Glass_Ihdr;
struct iByteTrain;
struct iByteLayer;

int Glass_defilter(struct Glass_Ihdr ihdr, struct iByteTrain *bt, struct iByteLayer *bl);

#endif
