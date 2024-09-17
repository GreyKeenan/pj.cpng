#ifndef MIKE_DEFLATE_H
#define MIKE_DEFLATE_H

#include "utils/iByteTrain_forw.h"

int Mike_Deflate_walk(iByteTrain *bt, uint32_t steps);
/*
	intent:
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	interprets [steps] bytes from the byteTrain as a zlib stream
	[steps] may stop before end. Subsequent calls will pick up where left off.

	to determine:
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	storing output?
	storing state of deflation? and returning to it?
	static inline?

	so basically all of it, yk.
*/

#endif
