#ifndef _UNIVERSAL_WAVELET_TREE__
#define _UNIVERSAL_WAVELET_TREE__

#include <stdio.h>

#include "dbv.h"
#include "memory.h"
#include "utils.h"

typedef struct {
  int32_t ncount_;
  int32_t scount_;
  DBVStructRef DBV_;
} UWT_Struct;

#define UWTStructRef UWT_Struct*

/*
 * Initialize UWT_Struct object.
 *
 * @param  UWT__  UWT structure reference.
 * @param  scount__  Number of symbols.
 */
void UWT_Init(UWTStructRef UWT__, int32_t scount__);

/*
 * Free all memory associated with the object.
 *
 * @param  UWT__  UWT structure reference.
 */
void UWT_Free(UWTStructRef UWT__);

/*
 * Delete symbol from given position.
 *
 * @param  UWT__  UWT structure reference.
 * @param  pos__  Delete position index.
 */
void UWT_Delete(UWTStructRef UWT__, uint32_t pos__);

/*
 * Insert new symbol into the given position.
 *
 * @param  UWT__  UWT structure reference.
 * @param  pos__  Position of newly inserted symbol.
 * @param  symb__  To be inserted symbol.
 */
void UWT_Insert(UWTStructRef UWT__, uint32_t pos__, uint8_t symb__);

/*
 * Get symbol from given position.
 *
 * @param  UWT__  UWT structure reference.
 * @param  pos__  Query position.
 */
uint8_t UWT_Get(UWTStructRef UWT__, uint32_t pos__);

/*
 * Print underlying bit vectors.
 *
 * @param  UWT__  UWT structure reference.
 */
void UWT_Print_BitVectors(UWTStructRef UWT__);

/*
 * Print symbols saved in the structure.
 *
 * This function is not very effective and
 * should be only used for testing purposes.
 *
 * @param  UWT__  UWT structure reference.
 */
void UWT_Print_Symbols(UWTStructRef UWT__);

/*
 * Rank given symbol in the Wavelet tree.
 *
 * @param  UWT__  UWT structure reference.
 * @param  pos__  Query position.
 * @param  symb__  Query symbol.
 */
int32_t UWT_Rank(UWTStructRef UWT__, uint32_t pos__, uint8_t symb__);

/*
 * Select given symbol in WT_Struct.
 *
 * @node This function uses division which is slower than mulitplication and
 * therefore it is slower than corresponding rank function.
 *
 * @param  UWT__  UWT structure reference.
 * @param  num__  Query count.
 * @param  symb__  Query symbol.
 */
int32_t UWT_Select(UWTStructRef UWT__, uint32_t num__, uint8_t symb__);

/*
 * Symbols in the universal wavelet tree must be mapped to correct values.
 *
 * If the number of symbols n is equal to some power of two, then you should
 * simply map each symbol to numbers between 0 and n-1, if n is not equal to
 * the power of two, map symbols to the first lower power of two as described
 * before. Then take all numbers in interval <lower power, higher power) and
 * sort them by their bit value from right to left (reverse bit order). Then
 * assign them to symbols in ascending order.
 *
 * @Example: n = 13
 *  first lower power is 8 -> we can map some symbols to numbers from 0 to 7
 *  all numbers in higher interval <8, 16) and their reversed values:
 *    8  -> 1000 -> 0001
 *    9  -> 1001 -> 1001
 *    10 -> 1010 -> 0101
 *    11 -> 1011 -> 1101
 *    12 -> 1100 -> 0011
 *    13 -> 1101 -> 1011
 *    14 -> 1110 -> 0111
 *    15 -> 1111 -> 1111
 *  sorted:
 *    8, 12, 10, 14, 9, 13, 11, 15
 *  first five numbers will be used for mapping function (13 - 8 = 5).
 */
static const uint8_t universal_map[256] = {
    0,   1,   2,   3,   4,   6,   5,   7,   8,   12,  10,  14,  9,   13,  11,  15,  16,  24,  20,
    28,  18,  26,  22,  30,  17,  25,  21,  29,  19,  27,  23,  31,  32,  48,  40,  56,  36,  52,
    44,  60,  34,  50,  42,  58,  38,  54,  46,  62,  33,  49,  41,  57,  37,  53,  45,  61,  35,
    51,  43,  59,  39,  55,  47,  63,  64,  96,  80,  112, 72,  104, 88,  120, 68,  100, 84,  116,
    76,  108, 92,  124, 66,  98,  82,  114, 74,  106, 90,  122, 70,  102, 86,  118, 78,  110, 94,
    126, 65,  97,  81,  113, 73,  105, 89,  121, 69,  101, 85,  117, 77,  109, 93,  125, 67,  99,
    83,  115, 75,  107, 91,  123, 71,  103, 87,  119, 79,  111, 95,  127, 128, 192, 160, 224, 144,
    208, 176, 240, 136, 200, 168, 232, 152, 216, 184, 248, 132, 196, 164, 228, 148, 212, 180, 244,
    140, 204, 172, 236, 156, 220, 188, 252, 130, 194, 162, 226, 146, 210, 178, 242, 138, 202, 170,
    234, 154, 218, 186, 250, 134, 198, 166, 230, 150, 214, 182, 246, 142, 206, 174, 238, 158, 222,
    190, 254, 129, 193, 161, 225, 145, 209, 177, 241, 137, 201, 169, 233, 153, 217, 185, 249, 133,
    197, 165, 229, 149, 213, 181, 245, 141, 205, 173, 237, 157, 221, 189, 253, 131, 195, 163, 227,
    147, 211, 179, 243, 139, 203, 171, 235, 155, 219, 187, 251, 135, 199, 167, 231, 151, 215, 183,
    247, 143, 207, 175, 239, 159, 223, 191, 255};

#endif
