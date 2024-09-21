#include <stdio.h>

#include "./mike.h"
#include "./error.h"
#include "./ihdr_impl.h"

#include "./dechunk/dechunk.h"

#include "./decompress/iNostalgicWriter_forw.h"
#include "./decompress/iNostalgicWriter.h"

#include "./defilter/defilter.h"

#include "utils/expandingWriter_forw.h"
#include "utils/expandingWriter.h"

#include "utils/iByteTrain.h"
#include "utils/iByteTrain_impl.h"
#include "utils/iByteTrain_forw.h"

#include "utils/iByteLayer.h"
#include "utils/iByteLayer_impl.h"
#include "utils/iByteLayer_forw.h"

#include "utils/autophagicSequence.h"
#include "utils/autophagicSequence_impl.h"
#include "utils/autophagicSequence_forw.h"

#include <stdlib.h>


int Mike_ExpandingWriter_nostalgize(const void *vself, uint8_t *destination, uint32_t distanceBack) {
	const ExpandingWriter *self = vself;

	if (distanceBack > self->writePosition) {
		return Mike_Decompress_iNostalgicWriter_TOOFAR;
	}

	if (self->nData == NULL) {
		return 1;
	}
	*destination = self->nData[self->writePosition - distanceBack];
	return 0;
}
static inline Mike_Decompress_iNostalgicWriter Mike_ExpandingWriter_as_iNostalgicWriter(ExpandingWriter *self) {
	return (Mike_Decompress_iNostalgicWriter) {
		.vself = self,
		.write = &ExpandingWriter_write,
		.nostalgize = &Mike_ExpandingWriter_nostalgize
	};
}

#define PNG_HEADER_LENGTH 8
const uint8_t mike_PNG_header[PNG_HEADER_LENGTH] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};

int Mike_decode(iByteTrain *bt) {

	int e = 0;
	uint8_t byte = 0;

	// PNG header
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	for (int i = 0; i < PNG_HEADER_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, &byte)) {
			e = Mike_ERROR_EOTL;
			goto finalize;
		}
		
		if (byte != mike_PNG_header[i]) {
			e = Mike_ERROR_NOTPNG;
			goto finalize;
		}
	}

	// dechunking & decompressing
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	struct mike_Ihdr ihdr = {0};

	#define STEP 512
	struct ExpandingWriter writer = ExpandingWriter_create(NULL, 0, 0, STEP);
	struct Mike_Decompress_iNostalgicWriter nw = Mike_ExpandingWriter_as_iNostalgicWriter(&writer);

	e = mike_Dechunk_go(bt, &ihdr, &nw);
	if (e) goto finalize;

	printf("\n");
	printf("HEXADECIMAL:\nwidth: %x, height: %x\nbitDepth: %x, colorType: %x\ncompressionMethod: %x, filterMethod: %x\ninterlaceMethod: %x\n",
		ihdr.width, ihdr.height,
		ihdr.bitDepth, ihdr.colorType,
		ihdr.compressionMethod, ihdr.filterMethod,
		ihdr.interlaceMethod
	);
	printf("\n");

	printf("filteredData: length:%d, cap:%d\n", writer.writePosition, writer.cap);

	// defiltering
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		// could handle this as I go within the iNostalgicWriter in the future

	if (writer.nData == NULL) { //shouldnt be possible
		return Mike_ERROR;
	}
	struct AutophagicSequence aph = AutophagicSequence_create(writer.nData, writer.writePosition, writer.cap);
	struct iByteTrain fBt = AutophagicSequence_as_iByteTrain(&aph);
	struct iByteLayer fBl = AutophagicSequence_as_iByteLayer(&aph);

	//e = mike_Defilter_go(ihdr, &filteredBt, &filteredBl);
	e = mike_Defilter_go(ihdr, &fBt, &fBl);
	if (e) goto finalize;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("unfilteredData: length:%d, cap:%d\n", aph.writePosition, aph.cap);
	printf("\t(overwrote filteredData)\n");

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	finalize:
	if (writer.nData != NULL) {
		free(writer.nData);
	}
	return e;
}
