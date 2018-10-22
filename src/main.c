#include "compression.h"

int main() {

	int idx = 0;

	compressor C;
	Compressor_Init(&C);

	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_C);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);
	idx = Compressor_Compress_symbol(&C, idx, VALUE_A);

	Compressor_Finalize(&C);
	Compressor_Free(&C);

	return 0;
}
