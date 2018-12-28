#include <stdio.h>

#include "compressor.h"
#include "defines.h"
#include "utils.h"

#define IO_BUFFER_SIZE 1024

#define MAIN_VERBOSE(func) \
  if (MAIN_VERBOSE_) {     \
    func                   \
  }

static void usage(char* program__) {
  fprintf(stderr,
          "\nUsage: %s [-e | -d] [-h] [file] [-o [file]] \n\n"
          "-e: Encode\n"
          "-d: Decode\n"
          "-h: This help\n"
          "-o: Output file [file]\n",
          program__);

  exit(EXIT_FAILURE);
}

typedef enum {
  UNKNOWN,
  ENCODE,
  DECODE
} compressor_mode;

const char* const mode_str[] = {"UNKNOWN", "ENCODE", "DECODE"};

static void main_encode(FILE* ifp__, FILE* ofp__) {
  char ibuffer[IO_BUFFER_SIZE];
  int32_t total, idx, buffer_len;
  compressor C;
  Graph_value symbol;

  MAIN_VERBOSE(
    printf("Starting compression\n");
  )

  Process_Init(&C);
  Compression_Start(ofp__);

  buffer_len = 0;
  idx = total = 0;

  /* keep space in file header for number of symbols */
  fwrite(&total, sizeof(total), 1, ofp__);

  do {
    if (buffer_len <= idx) {
      if (!fgets(ibuffer, IO_BUFFER_SIZE, (FILE*) ifp__))
        break;

      buffer_len = strlen(ibuffer);
      idx = 0;
    }

    switch (ibuffer[idx]) {
      case 'a':
      case 'A':
        symbol = VALUE_A;
        break;
      case 'c':
      case 'C':
        symbol = VALUE_C;
        break;
      case 'g':
      case 'G':
        symbol = VALUE_G;
        break;
      case 't':
      case 'T':
        symbol = VALUE_T;
        break;
      case ' ':
      case '\n':
      case '\t':
        idx++;
        continue;
      default:
        fprintf(stderr, "Unexpected symbol in input file %c.\n", ibuffer[idx]);
        fprintf(stderr,
                "File can contain four letters of dna (both lower and uppercase) and spaces\n");
        fclose(ifp__);
        fclose(ofp__);
        exit(EXIT_FAILURE);
    }

    Compressor_Compress_symbol(&C, symbol);
    idx++;
    total++;
  } while (true);

  Compression_Finalize();
  Process_Free(&C);

  /* save total number of letters into the header of the output file */
  fseek(ofp__, 0, SEEK_SET);
  fwrite(&total, sizeof(total), 1, ofp__);

  MAIN_VERBOSE(
    printf("Finished compression\n");
  )
}

static void main_decode(FILE* ifp__, FILE* ofp__) {
  char obuffer[IO_BUFFER_SIZE];
  int32_t i, idx, total;
  compressor C;
  Graph_value val;

  MAIN_VERBOSE(
    printf("Starting decompression\n");
  )

  /* keep space in file header for number of symbols */
  if (!fread(&total, sizeof(total), 1, ifp__)) {
    fprintf(stderr, "Cannot read total size from the file\n");
    fclose(ifp__);
    fclose(ofp__);
    exit(EXIT_FAILURE);
  }

  Process_Init(&C);
  Decompression_Start(ifp__);

  idx = 0;
  for (i = 0; i < total; i++) {
    Decompressor_Decompress_symbol(&C, &val);

    switch (val) {
      case VALUE_A:
        obuffer[idx++] = 'A';
        break;
      case VALUE_C:
        obuffer[idx++] = 'C';
        break;
      case VALUE_G:
        obuffer[idx++] = 'G';
        break;
      case VALUE_T:
        obuffer[idx++] = 'T';
        break;
      default:
        /* Other Graph_values cannot be outputs */
        break;
    }

    if (idx == IO_BUFFER_SIZE) {
      fwrite(obuffer, sizeof(char), IO_BUFFER_SIZE, ofp__);
      idx = 0;
    }
  }

  /* output rest of the buffer */
  if (idx)
    fwrite(obuffer, sizeof(char), idx, ofp__);

  Decompression_Finalize();
  Process_Free(&C);

  MAIN_VERBOSE(
    printf("Finished decompression\n");
  )
}

/*
 * Main compressor program. Parse command line arguments and perform all actions
 * to compress or decompress the code.
 */
int main(int argc, char* argv[]) {
  int32_t i;
  bool expect_ofile = false;

  char* ofile = NULL;
  char* ifile = NULL;

  FILE *ofp, *ifp;

  compressor_mode mode = UNKNOWN;

  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {

      /* raise an error if no file is given after -o flag */
      if (expect_ofile) {
        fprintf(stderr, "Filename expected after -o\n");
        usage(argv[0]);
      }

      switch (argv[i][1]) {
        case 'e':
          mode = ENCODE;
          break;
        case 'd':
          mode = DECODE;
          break;
        case 'o':
          expect_ofile = true;
          break;
        case 'h':
          usage(argv[0]);
          break;
        default:
          fprintf(stderr, "Unexpected argument %s.\n", argv[i]);
          usage(argv[0]);
          break;
      }
    } else {
      if (expect_ofile) {
        if (ofile != NULL) {
          fprintf(stderr, "Only one file can be specified as an output file\n");
          exit(EXIT_FAILURE);
        }
        ofile = argv[i];
      } else {
        if (ifile != NULL) {
          fprintf(stderr, "Only one file can be specified as an ipnut file\n");
          exit(EXIT_FAILURE);
        }
        ifile = argv[i];
      }
    }
  }

  if (mode == UNKNOWN) {
    fprintf(stderr, "You must select eighter encoding or decoding mode\n");
    usage(argv[0]);
  }

  if (ifile == NULL) {
    fprintf(stderr, "No input file specified\n");
    usage(argv[0]);
  }
  if (ofile == NULL) {
    fprintf(stderr, "No output file specified\n");
    usage(argv[0]);
  }

  if (mode == ENCODE)
    ifp = fopen(ifile, "r");
  else if (mode == DECODE)
    ifp = fopen(ifile, "rb");

  if (ifp == NULL) {
    fprintf(stderr, "Can't open input file %s!\n", ifile);
    exit(EXIT_FAILURE);
  }

  if (mode == ENCODE)
    ofp = fopen(ofile, "wb");
  else if (mode == DECODE)
    ofp = fopen(ofile, "w");

  if (ofp == NULL) {
    fprintf(stderr, "Can't open output file %s!\n", ofile);
    fclose(ifp);
    exit(EXIT_FAILURE);
  }

  MAIN_VERBOSE(
    printf("Mode: %s\n", mode_str[mode]);
    printf("Input file: %s\n", ifile);
    printf("Output file: %s\n", ofile);
  )

  init_memory_profiling();
  init_time_profiling();

  if (mode == ENCODE)
    main_encode(ifp, ofp);
  else if (mode == DECODE)
    main_decode(ifp, ofp);

  finish_time_profiling();
  finish_memory_profiling();

  fclose(ifp);
  fclose(ofp);
}
