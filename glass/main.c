#include "./main.h"

#include "./error.h"
#include "./ihdr_impl.h"

#include "./dechunk.h"
//#include "./defilter.h"


#include "./expandingWriter.h"
#include "utils/expandingWriter.h"
#include "utils/expandingWriter_impl.h"


#include "utils/iByteTrain.h"
#include "utils/iByteTrain_impl.h"

#include "utils/iByteLayer.h"
#include "utils/iByteLayer_impl.h"

#include "utils/autophagicSequence.h"
#include "utils/autophagicSequence_impl.h"

#include <stdlib.h>
#include <stdio.h>

#define PNG_HEADER_LENGTH 8
const uint8_t Glass_PNG_header[PNG_HEADER_LENGTH] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};

int Glass_decode(struct iByteTrain *bt, struct Glass_ScanlineImage *destination) {

	int e = 0;
	uint8_t byte = 0;

	struct Glass_Ihdr ihdr = {0};
	struct ExpandingWriter writer = {0};

	struct Puff_iNostalgicWriter nw = {0};

	// PNG header
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	for (int i = 0; i < PNG_HEADER_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, &byte)) {
			e = Glass_decode_ERROR_EOTL;
			goto finalize;
		}
		
		if (byte != Glass_PNG_header[i]) {
			e = Glass_decode_ERROR_NOTPNG;
			goto finalize;
		}
	}

	// dechunking & decompressing
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	#define STEP 512
	writer = ExpandingWriter_create(NULL, 0, 0, STEP);
	nw = Glass_ExpandingWriter_as_iNostalgicWriter(&writer);

	e = Glass_dechunk(bt, &ihdr, nw);
	if (e) goto finalize;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf(
		"\n"
		"\nwidth: 0x%x, height: 0x%x"
		"\nbitDepth: 0x%x, colorType: 0x%x"
		"\ncompressionMethod: 0x%x, filterMethod: 0x%x"
		"\ninterlaceMethod: 0x%x"
		"\nfilteredData: length: %d, cap: %d"
		"\n",
		ihdr.width, ihdr.height,
		ihdr.bitDepth, ihdr.colorType,
		ihdr.compressionMethod, ihdr.filterMethod,
		ihdr.interlaceMethod,
		writer.writePosition, writer.cap
	);



	finalize:
	if (writer.nData != NULL) {
		free(writer.nData);
	}
	return e;
}
