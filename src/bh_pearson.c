/*
 * Copyright (c) 2026 Christian Gauger-Cosgrove
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * \file	bh_pearson.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <bit_hash.h>

/**
 * \brief Pearson's hash state
 *
 * 
 */
struct pearson_s {
	uint8_t t[256];
	/**<
	 */
	uint8_t h;
	/**<
	 */
};

/**
 * \brief Pearson's hash state initializer
 *
 * Allocates memory for, and then initializes, the state variable for Pearson's
 * hash. There are five pre-defined "auxiliary tables"/"permutation tables" for
 * the hash (if you want to define your own table, refer to the documentation
 * for \ref pearson_init_custom ).
 *
 * The table used when the requested hash is \c PEARSON_ACM is the table defined
 * in Peter K. Pearson's original paper\cite pearson1990 . The table used
 * \c PEARSON_RFC is specified is from RFC 3074\cite rfc3074 . The remaining
 * tables (\c PEARSON_PI, \c PEARSON_E, and \c PEARSON_PHI) are derived from
 * mathematical constants (pi, Euler's number/Napier's constant, and the golden
 * ratio (e.g. phi), respectively). The method of derivation was according to
 * the metholody used by Ron Rivest for the derivation of the MD2 hash/message
 * digest function S-box from the value of pi (in fact, the table derived from
 * pi is an exact match for the MD2 S-box).
 *
 * A genericized C implementation of Rivest's metholody is as follows:
 * \code{.c}
// Sequence of digits 0 through 1; as opposed to a constant, if possible, a
// function may be used.
const int seq[] = {
	...
};

// Index into the sequence.
int seq_idx;

// Return the next value in the sequence.
int next_seq(void) {
	return seq[seq_idx++];
}

// Generate a random number using the in the range 0 to n - 1, with de-biasing
// to prevent modulo bias of the output.
int rand_seq(int n) {
	int x, y;

	x = next_seq();
	y = 10;

	if (n > 10) {
		x = x * 10 + next_seq();
		y = 100;
	}

	if (n > 100) {
		x = x * 10 + next_seq();
		y = 1000;
	}

	if (n < (n * (y / n))) {
		return (x % n);
	} else {
		return rand_seq(n);
	}
}

// Permute the table of values 0 to 255 (inclusive).
void permute_table(int table[256]) {
	int i, j, t;

	for (i = 0; i < 256; ++i)
		table[i] = i;

	for (i = 2, seq_idx = 0; i <= 256; ++i) {
		j = rand_seq(i);
		t = table[j];
		table[j] = table[i];
		table[i] = t;
	}

	return;
}
 * \endcode
 * The shuffle of the permutation is a variation upon the Durstenfeld shuffle
 * (as Ron Rivest calls it, the shuffle is also known more commonly as the
 * Fisher-Yates shuffle).
 *
 * \return Pearson's hash state variable.
 */
pearson_h
pearson_init(
    enum PEARSON_TYPE type	/**<  */
) {
	static const int pearsonTables[5][256] = {
		{  1,  87,  49,  12, /* Table from Pearson's ACM article */
		 176, 178, 102, 166, 121, 193,   6,  84, 249, 230,  44, 163,
		  14, 197, 213, 181, 161,  85, 218,  80,  64, 239,  24, 226,
		 236, 142,  38, 200, 110, 177, 104, 103, 141, 253, 255,  50,
		  77, 101,  81,  18,  45,  96,  31, 222,  25, 107, 190,  70,
		  86, 237, 240,  34,  72, 242,  20, 214, 244, 227, 149, 235,
		  97, 234,  57,  22,  60, 250,  82, 175, 208,   5, 127, 199,
		 111,  62, 135, 248, 174, 169, 211,  58,  66, 154, 106, 195,
		 245, 171,  17, 187, 182, 179,   0, 243, 132,  56, 148,  75,
		 128, 133, 158, 100, 130, 126,  91,  13, 153, 246, 216, 219,
		 119,  68, 223,  78,  83,  88, 201,  99, 122,  11,  92,  32,
		 136, 114,  52,  10, 138,  30,  48, 183, 156,  35,  61,  26,
		 143,  74, 251,  94, 129, 162,  63, 152, 170,   7, 115, 167,
		 241, 206,   3, 150,  55,  59, 151, 220,  90,  53,  23, 131,
		 125, 173,  15, 238,  79,  95,  89,  16, 105, 137, 225, 224,
		 217, 160,  37, 123, 118,  73,   2, 157,  46, 116,   9, 145,
		 134, 228, 207, 212, 202, 215,  69, 229,  27, 188,  67, 124,
		 168, 252,  42,   4,  29, 108,  21, 247,  19, 205,  39, 203,
		 233,  40, 186, 147, 198, 192, 155,  33, 164, 191,  98, 204,
		 165, 180, 117,  76, 140,  36, 210, 172,  41,  54, 159,   8,
		 185, 232, 113, 196, 231,  47, 146, 120,  51,  65,  28, 144,
		 254, 221,  93, 189, 194, 139, 112,  43,  71, 109, 184, 209},
		{251, 175, 119, 215, /* Table from RFC 3074 */
		  81,  14,  79, 191, 103,  49, 181, 143, 186, 157,   0, 232,
		  31,  32,  55,  60, 152,  58,  17, 237, 174,  70, 160, 144,
		 220,  90,  57, 223,  59,   3,  18, 140, 111, 166, 203, 196,
		 134, 243, 124,  95, 222, 179, 197,  65, 180,  48,  36,  15,
		 107,  46, 233, 130, 165,  30, 123, 161, 209,  23,  97,  16,
		  40,  91, 219,  61, 100,  10, 210, 109, 250, 127,  22, 138,
		  29, 108, 244,  67, 207,   9, 178, 204,  74,  98, 126, 249,
		 167, 116,  34,  77, 193, 200, 121,   5,  20, 113,  71,  35,
		 128,  13, 182,  94,  25, 226, 227, 199,  75,  27,  41, 245,
		 230, 224,  43, 225, 177,  26, 155, 150, 212, 142, 218, 115,
		 241,  73,  88, 105,  39, 114,  62, 255, 192, 201, 145, 214,
		 168, 158, 221, 148, 154, 122,  12,  84,  82, 163,  44, 139,
		 228, 236, 205, 242, 217,  11, 187, 146, 159,  64,  86, 239,
		 195,  42, 106, 198, 118, 112, 184, 172,  87,   2, 173, 117,
		 176, 229, 247, 253, 137, 185,  99, 164, 102, 147,  45,  66,
		 231,  52, 141, 211, 194, 206, 246, 238,  56, 110,  78, 248,
		  63, 240, 189,  93,  92,  51,  53, 183,  19, 171,  72,  50,
		  33, 104, 101,  69,   8, 252,  83, 120,  76, 135,  85,  54,
		 202, 125, 188, 213,  96, 235, 136, 208, 162, 129, 190, 132,
		 156,  38,  47,   1,   7, 254,  24,   4, 216, 131,  89,  21,
		  28, 133,  37, 153, 149,  80, 170,  68,   6, 169, 234, 151},
		{ 41,  46,  67, 201, /* Table derived from pi */
		 162, 216, 124,   1,  61,  54,  84, 161, 236, 240,   6,  19,
		  98, 167,   5, 243, 192, 199, 115, 140, 152, 147,  43, 217,
		 188,  76, 130, 202,  30, 155,  87,  60, 253, 212, 224,  22,
		 103,  66, 111,  24, 138,  23, 229,  18, 190,  78, 196, 214,
		 218, 158, 222,  73, 160, 251, 245, 142, 187,  47, 238, 122,
		 169, 104, 121, 145,  21, 178,   7,  63, 148, 194,  16, 137,
		  11,  34,  95,  33, 128, 127,  93, 154,  90, 144,  50,  39,
		  53,  62, 204, 231, 191, 247, 151,   3, 255,  25,  48, 179,
		  72, 165, 181, 209, 215,  94, 146,  42, 172,  86, 170, 198,
		  79, 184,  56, 210, 150, 164, 125, 182, 118, 252, 107, 226,
		 156, 116,   4, 241,  69, 157, 112,  89, 100, 113, 135,  32,
		 134,  91, 207, 101, 230,  45, 168,   2,  27,  96,  37, 173,
		 174, 176, 185, 246,  28,  70,  97, 105,  52,  64, 126,  15,
		  85,  71, 163,  35, 221,  81, 175,  58, 195,  92, 249, 206,
		 186, 197, 234,  38,  44,  83,  13, 110, 133,  40, 132,   9,
		 211, 223, 205, 244,  65, 129,  77,  82, 106, 220,  55, 200,
		 108, 193, 171, 250,  36, 225, 123,   8,  12, 189, 177,  74,
		 120, 136, 149, 139, 227,  99, 232, 109, 233, 203, 213, 254,
		  59,   0,  29,  57, 242, 239, 183,  14, 102,  88, 208, 228,
		 166, 119, 114, 248, 235, 117,  75,  10,  49,  68,  80, 180,
		 143, 237,  31,  26, 219, 153, 141,  51, 159,  17, 131,  20},
		{122,  77, 153,  59, /* Table derived from Euler's number */
		 173, 107,  19, 104, 123, 183,  75,  10, 114, 236, 106,  83,
		 117,  16, 189, 211,  51, 231, 143, 118, 248, 148, 218, 245,
		  24,  61,  66,  73, 205, 185, 134, 215,  35, 213,  41,   0,
		 174, 240, 177, 195, 193,  39,  50, 138, 161, 151,  89,  38,
		 176,  45,  42,  27, 159, 225,  36,  64, 133, 168,  22, 247,
		  52, 216, 142, 100, 207, 234, 125, 229, 175,  79, 220, 156,
		  91, 110,  30, 147,  95, 191,  96,  78,  34, 251, 255, 181,
		  33, 221, 139, 119, 197,  63,  40, 121, 204,   4, 246, 109,
		  88, 146, 102, 235, 223, 214,  92, 224, 242, 170, 243, 154,
		 101, 239, 190,  15, 249, 203, 162, 164, 199, 113, 179,   8,
		  90, 141,  62, 171, 232, 163,  26,  67, 167, 222,  86,  87,
		  71,  11, 226, 165, 209, 144,  94,  20, 219,  53,  49,  21,
		 160, 115, 145,  17, 187, 244,  13,  29,  25,  57, 217, 194,
		  74, 200,  23, 182, 238, 128, 103, 140,  56, 252,  12, 135,
		 178, 152,  84, 111, 126,  47, 132,  99, 105, 237, 186,  37,
		 130,  72, 210, 157, 184,   3,   1,  44,  69, 172,  65,   7,
		 198, 206, 212, 166,  98, 192,  28,   5, 155, 136, 241, 208,
		 131, 124,  80, 116, 127, 202, 201,  58, 149, 108,  97,  60,
		  48,  14,  93,  81, 158, 137,   2, 227, 253,  68,  43, 120,
		 228, 169, 112,  54, 250, 129,  46, 188, 196,  85, 150,   6,
		 254, 180, 233, 230,  31,  76,  55,  18,   9,  32,  82,  70},
		{ 61,   3, 185, 197, /* Table derived from phi */
		  96, 229, 245,  92, 220,  39, 174, 162,  13, 102, 117, 246,
		  98, 101, 189,  78,  43, 155,  94, 104,  38, 167, 198, 215,
		 151,  72,  41, 139,  97, 147, 196,  64, 133, 205, 180,  70,
		 247,  44, 153, 184,  65,  23, 251, 211, 163, 103,  87,  26,
		 233,  27, 100,  52, 203,  15, 235, 255,  62,  16,  91,  74,
		  57,   2, 121, 171, 116,  90, 207, 250, 105, 188, 138,  56,
		 141, 239,  80,  63, 242,  17, 213,  30, 119,   5,  20, 176,
		 132,  47, 212,  37, 232,   7, 238, 218,  32, 195, 150, 140,
		 186, 252, 208, 169, 244, 115, 192, 254,  24, 243,  66, 156,
		 221,  60,  45, 222, 217, 107,  21, 124, 181, 249, 146,  35,
		  51,  54,  85,  67, 126, 143,  68, 194, 173, 204, 106, 114,
		  95, 113, 125,  10,  79, 161, 154, 135,  99, 157, 166, 110,
		 168,  84,  19, 148,  76,   6,  42, 228, 190,  82, 123, 240,
		  89, 199, 111,  49, 227, 231, 131,  34,  36,  33, 182, 145,
		 118, 225, 158,   4, 172,  71, 178,  53, 236,  18,  50,  31,
		 129,   0,  22, 120, 224, 137, 160, 122, 237, 108, 175,  14,
		 193,  93, 144, 128, 248,  75, 170, 183,  58, 234, 159,  46,
		 241, 223, 165, 202, 130,  55, 191,   9,   8,  86, 253,  12,
		 149,  29, 230,  77,  25,  59, 187,  28, 206,  73,  40,  88,
		 142,  83,  48, 177, 127, 112, 201, 214, 200, 209, 109,  81,
		 226, 164,  11,   1, 136, 152, 219, 216, 210, 179, 134,  69}
	};
	struct pearson_s *st;

	st = (struct pearson_s *) malloc(sizeof(struct pearson_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct pearson_s));

	st -> h = 0;
	switch (type) {
	case PEARSON_ACM:
		memcpy(st -> t, pearsonTables[0], 256 * sizeof(uint8_t));
		break;
	case PEARSON_RFC:
		memcpy(st -> t, pearsonTables[1], 256 * sizeof(uint8_t));
		break;
	case PEARSON_PI:
		memcpy(st -> t, pearsonTables[2], 256 * sizeof(uint8_t));
		break;
	case PEARSON_E:
		memcpy(st -> t, pearsonTables[3], 256 * sizeof(uint8_t));
		break;
	case PEARSON_PHI:
		memcpy(st -> t, pearsonTables[4], 256 * sizeof(uint8_t));
		break;
	default:
		free(st);
		return NULL;
	}

	return st;
}

/**
 * \brief Pearson's hash custom initializer
 *
 * Allocates memory for, and then initializes, the state variable for Pearson's
 * hash. Both the "auxiliary table"/"permutation table" and initial value of the
 * hash must be specified.
 *
 * The initializer places \b no constraints on the table definition (beyond that
 * it be a minimum of 256 bytes long); which allows one to supply bad tables to
 * the hashing algorithm.
 *
 * \return Pearson's hash state variable.
 */
pearson_h
pearson_init_custom(
    uint8_t value,	/**<  */
    const uint8_t *table,	/**<  */
    size_t length	/**<  */
) {
	struct pearson_s *st;

	if (length < 256)
		return NULL;

	st = (struct pearson_s *) malloc(sizeof(struct pearson_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct pearson_s));

	st -> h = value;
	memcpy(st -> t, table, 256 * sizeof(uint8_t));

	return st;
}

/**
 * \brief Pearson's hash state destructor
 *
 * Zeroizes and frees the state variable.
 *
 * \return 0 on success
 */
int
pearson_destroy(
    pearson_h *state	/**< [in,out] State variable to be destroyed. */
) {
	struct pearson_s *st;

	st = *state;
	memset(st, 0, sizeof(struct pearson_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 *
 */
uint8_t
pearson_buf(
    const void *buf,		/**< [in] Pointer to the start of the buffer. */
    size_t len,		/**< [in] Length of the buffer (in bytes). */
    pearson_h state	/**< [in,out] State variable. */
) {
	uint8_t *bp, *be;

	bp = (uint8_t *) buf;
	be = bp + len;

	while (bp < be) {
		state -> h = state -> t[state -> h ^ *bp++];
	}

	return state -> h;
}

/**
 *
 */
uint8_t
pearson_str(
    const char *str,		/**< [in] Pointer to the start of the string. */
    pearson_h state	/**< [in,out] State variable. */
) {
	return pearson_buf(str, strlen(str), state);
}

/**
 *
 */
uint8_t
pearson_value(
    pearson_h state	/**<  */
) {
	return state -> h;
}