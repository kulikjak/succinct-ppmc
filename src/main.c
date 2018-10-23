#include <stdio.h>
#include "compressor.h"

int main() {

	int idx = 0;

	FILE* ofp = fopen("testfile.bin", "wb");
	if (ofp == NULL) {
	  fprintf(stderr, "Can't open output file testfile.bin!\n");
	  exit(1);
	}

	compressor C;
	Compressor_Init(&C, ofp);

	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_C);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_C);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_C);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_G);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_T);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);

	Compressor_Finalize(&C);
	Compressor_Free(&C);

	fclose(ofp);

	return 0;
}
