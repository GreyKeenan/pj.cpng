#ifndef PUFF_ERROR_H
#define PUFF_ERROR_H

enum Puff_step_Error {

	Puff_step_END = 1
	, Puff_step_ERROR
	, Puff_step_ERROR_IMPOSSIBLE

	, Puff_step_ERROR_STATE

	, Puff_step_ERROR_UNALIGNEDUNCOMPRESSED

	, Puff_step_ERROR_BLOCKHEADER_BITSREAD
	, Puff_step_ERROR_COMPRESSIONTYPE

	, Puff_step_ERROR_UNCOMPRESSED_LENGTH_BYTESREAD
	, Puff_step_ERROR_NLEN

	
	, Puff_step_ERROR_FIXED_INIT
	, Puff_step_ERROR_FIXED_WALK


	, Puff_step_DIRECTIVE_FINISHBYTE
		//should never occur outside the module
};

#endif
