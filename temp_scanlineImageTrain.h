//TEMPORARY

struct Imbridge {
	Mike_ScanlineImage *image;

	uint64_t position;
};


int Imbridge_chewchew(void *vself, uint8_t *nDestination) {
	struct Imbridge *self = vself;

	if (self->position >= self->image->length) {
		return iByteTrain_ENDOFTHELINE;
	}

	if (nDestination != NULL) {
		*nDestination = self->image->data[self->position];
	}
	self->position++;

	return 0;
}

int Imbridge_gauge(const void *vself, uint32_t *nWidth, uint32_t *nHeight) {
	const struct Imbridge *self = vself;

	if (nWidth != NULL) {
		*nWidth = self->image->width;
	}
	if (nHeight != NULL) {
		*nHeight = self->image->height;
	}

	return 0;
}
int Imbridge_choochoo(void *vself, uint32_t *nDestination) {
	struct Imbridge *self = vself;
	int e = 0;
	uint8_t byte = 0;
	uint32_t pixel = 0;

	if (self->position >= self->image->length) {
		return Sdaubler_iImageTrain_ENDOFTHELINE;
	}
	
	switch (self->image->colorType) {
		case 2:
			switch (self->image->bitDepth) {
				case 8:
					for (int i = 0; i < 3; ++i) {
						if (Imbridge_chewchew(self, &byte)) {
							return 3;
						}
						pixel = pixel | byte;
						pixel = pixel << 8;
					}
					pixel = pixel | 255; //alpha
					break;
				case 16:
				default:
					return 2;
			}
			break;
		case 0: //grey
		case 3: //index
		case 4: //greya
		case 6: //rgba
		default:
			return 1;
	}

	if (nDestination != NULL) {
		*nDestination = pixel;
	}

	return 0;
}

static inline Sdaubler_iImageTrain Imbridge_as_iImageTrain(struct Imbridge *self) {
	return (Sdaubler_iImageTrain) {
		.vself = self,
		.gauge = &Imbridge_gauge,
		.choochoo = &Imbridge_choochoo
	};
}
