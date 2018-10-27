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
	printf("idx %d\n", idx);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_C);
	printf("idx %d\n", idx);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_C);
	printf("idx %d\n", idx);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_C);
	printf("idx %d\n", idx);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_G);
	printf("idx %d\n", idx);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_T);
	printf("idx %d\n", idx);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);
	printf("idx %d\n", idx);

	Compressor_Finalize(&C);
	Compressor_Free(&C);

	fclose(ofp);

	printf("----------------------------------------\n");

	FILE* ifp = fopen("testfile.bin", "rb");
	if (ifp == NULL) {
	  fprintf(stderr, "Can't open input file testfile.bin!\n");
	  exit(1);
	}

	decompressor D;
	Decompressor_Init(&D, ifp);

	idx = 0;
	idx = Decompressor_Decompress_symbol(&D, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Decompressor_Decompress_symbol(&D, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Decompressor_Decompress_symbol(&D, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Decompressor_Decompress_symbol(&D, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Decompressor_Decompress_symbol(&D, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Decompressor_Decompress_symbol(&D, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Decompressor_Decompress_symbol(&D, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Decompressor_Decompress_symbol(&D, idx, VALUE_A);
	printf("idx %d\n", idx);
	idx = Decompressor_Decompress_symbol(&D, idx, VALUE_A);
	printf("idx %d\n", idx);

	Decompressor_Finalize(&D);
	Decompressor_Free(&D);

	fclose(ifp);

	return 0;
}
