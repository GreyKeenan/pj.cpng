#include <stdio.h>

#include "./main.h"
#include "./error.h"
#include "./ihdr_impl.h"
#include "./scanlineImage_impl.h"

#include "./dechunk.h"
#include "./defilter.h"

#include "puff/iNostalgicWriter.h"
#include "puff/iNostalgicWriter_impl.h"

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


int Glass_ExpandingWriter_nostalgize(const void *vself, uint8_t *destination, uint32_t distanceBack) {
	const ExpandingWriter *self = vself;

	if (distanceBack > self->writePosition) {
		return Puff_iNostalgicWriter_TOOFAR;
	}

	if (self->nData == NULL) {
		return 1;
	}
	*destination = self->nData[self->writePosition - distanceBack];
	return 0;
}
static inline Puff_iNostalgicWriter Glass_ExpandingWriter_as_iNostalgicWriter(ExpandingWriter *self) {
	return (Puff_iNostalgicWriter) {
		.vself = self,
		.write = &ExpandingWriter_write,
		.nostalgize = &Glass_ExpandingWriter_nostalgize
	};
}

#define PNG_HEADER_LENGTH 8
const uint8_t Glass_PNG_header[PNG_HEADER_LENGTH] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};

int Glass_decode(iByteTrain *bt, Glass_ScanlineImage *destination) {

	int e = 0;
	uint8_t byte = 0;

	struct Glass_Ihdr ihdr = {0};

	struct ExpandingWriter writer = {0};
	struct Puff_iNostalgicWriter nw = {0};

	struct AutophagicSequence aph = {0};
	struct iByteTrain fBt = {0};
	struct iByteLayer fBl = {0};

	uint8_t *data = NULL;

	// PNG header
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	for (int i = 0; i < PNG_HEADER_LENGTH; ++i) {
		if (iByteTrain_chewchew(bt, &byte)) {
			e = Glass_ERROR_EOTL;
			goto finalize;
		}
		
		if (byte != Glass_PNG_header[i]) {
			e = Glass_ERROR_NOTPNG;
			goto finalize;
		}
	}

	// dechunking & decompressing
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	#define STEP 512
	writer = ExpandingWriter_create(NULL, 0, 0, STEP);
	nw = Glass_ExpandingWriter_as_iNostalgicWriter(&writer);

	e = Glass_Dechunk_go(bt, &ihdr, &nw);
	if (e) goto finalize;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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

	if (writer.nData == NULL) { //shouldnt be possible
		return Glass_ERROR;
	}
	data = writer.nData;

	aph = AutophagicSequence_create(data, writer.writePosition, writer.cap);
	fBt = AutophagicSequence_as_iByteTrain(&aph);
	fBl = AutophagicSequence_as_iByteLayer(&aph);

	e = Glass_Defilter_go(ihdr, &fBt, &fBl);
	if (e) goto finalize;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	printf("unfilteredData: length:%d, cap:%d\n", aph.writePosition, aph.cap);
	printf("\t(overwrote filteredData)\n");

	if (ihdr.interlaceMethod != 0) {
		printf("TODO ERR: adam7 interlace not supported\n");
		return Glass_ERROR;
	}
	if (ihdr.colorType == 3) {
		printf("TODO ERR: indexed-color images not supported\n");
		return Glass_ERROR;
	}

	// give data
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	*destination = (struct Glass_ScanlineImage) {
		.width = ihdr.width, .height = ihdr.height,
		.colorType = ihdr.colorType, .bitDepth = ihdr.bitDepth,
		.data = data, .length = aph.writePosition
	};
	writer.nData = NULL;

	// end
	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	finalize:
	if (writer.nData != NULL) {
		free(writer.nData);
	}
	return e;
}
