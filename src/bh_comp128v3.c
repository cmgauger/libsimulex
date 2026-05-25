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
 * \file	bh_comp128v3.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <bit_hash.h>

const uint8_t
comp128_t0[] = {
	197, 235,  60, 151,  98,  96,   3, 100, 248, 118,  42, 117, 172, 211,
	181, 203,  61, 126, 156,  87, 149, 224,  55, 132, 186,  63, 238, 255,
	 85,  83, 152,  33, 160, 184, 210, 219, 159,  11, 180, 194, 130, 212,
	147,   5, 215,  92,  27,  46, 113, 187,  52,  25, 185,  79, 221,  48,
	 70,  31, 101,  15, 195, 201,  50, 222, 137, 233, 229, 106, 122, 183,
	178, 177, 144, 207, 234, 182,  37, 254, 227, 231,  54, 209, 133,  65,
	202,  69, 237, 220, 189, 146, 120,  68,  21, 125,  38,  30,   2, 155,
	 53, 196, 174, 176,  51, 246, 167,  76, 110,  20,  82, 121, 103, 112,
	 56, 173,  49, 217, 252,   0, 114, 228, 123,  12,  93, 161, 253, 232,
	240, 175, 67, 128,   22, 158,  89,  18,  77, 109, 190,  17,  62,   4,
	153, 163,  59, 145, 138,   7,  74, 205,  10, 162,  80,  45, 104, 111,
	150, 214, 154,  28, 191, 169, 213,  88, 193, 198, 200, 245,  39, 164,
	124,  84,  78,   1, 188, 170,  23,  86, 226, 141,  32,   6, 131, 127,
	199,  40, 135,  16,  57,  71,  91, 225, 168, 242, 206,  97, 166,  44,
	 14,  90, 236, 239, 230, 244, 223, 108, 102, 119, 148, 251,  29, 216,
	  8,   9, 249, 208,  24, 105,  94,  34,  64,  95, 115,  72, 134, 204,
	 43, 247, 243, 218,  47,  58,  73, 107, 241, 179, 116,  66,  36, 143,
	 81, 250, 139,  19,  13, 142, 140, 129, 192,  99, 171, 157, 136,  41,
	 75,  35, 165,  26
};

const uint8_t
comp128_t1[] = {
	170,  42,  95, 141, 109,  30,  71,  89,  26, 147, 231, 205, 239, 212,
	124, 129, 216,  79,  15, 185, 153,  14, 251, 162,   0, 241, 172, 197,
	 43,  10, 194, 235,   6,  20,  72,  45, 143, 104, 161, 119,  41, 136,
	 38, 189, 135,  25,  93,  18, 224, 171, 252, 195,  63,  19,  58, 165,
	 23,  55, 133, 254, 214, 144, 220, 178, 156,  52, 110, 225,  97, 183,
	140,  39,  53,  88, 219, 167,  16, 198,  62, 222,  76, 139, 175,  94,
	 51, 134, 115,  22,  67,   1, 249, 217,   3,   5, 232, 138,  31,  56,
	116, 163,  70, 128, 234, 132, 229, 184, 244,  13,  34,  73, 233, 154,
	179, 131, 215, 236, 142, 223,  27,  57, 246, 108, 211,   8, 253,  85,
	 66, 245, 193,  78, 190,   4,  17,   7, 150, 127, 152, 213,  37, 186,
	  2, 243,  46, 169,  68, 101,  60, 174, 208, 158, 176,  69, 238, 191,
	 90,  83, 166, 125,  77,  59,  21,  92,  49, 151, 168,  99,   9,  50,
	146, 113, 117, 228,  65, 230,  40,  82,  54, 237, 227, 102,  28,  36,
	107,  24,  44, 126, 206, 201,  61, 114, 164, 207, 181,  29,  91,  64,
	221, 255,  48, 155, 192, 111, 180, 210, 182, 247, 203, 148, 209,  98,
	173,  11,  75, 123, 250, 118,  32,  47, 240, 202,  74, 177, 100,  80,
	196,  33, 248,  86, 157, 137, 120, 130,  84, 204, 122,  81, 242, 188,
	200, 149, 226, 218, 160, 187, 106,  35,  87, 105,  96, 145, 199, 159,
	 12, 121, 103, 112
};

/**
 * \brief
 *
 * 
 */
static void
comp128_scramble(
    uint8_t *r,		/**< [in,out]  */
    uint8_t *x		/**< [in]  */
) {
	uint8_t t[16];
	uint8_t m[32];
	int i, j, k, z;

	memset(t, 0, 16);
	memset(m, 0, 16);

	memcpy(m     , r, 16);
	memcpy(m + 16, x, 16);

	memset(r, 0, 16);

	for (i = 0; i < 5; ++i) {
		j = 0;

		for (z = 0; z < 16; ++z)
			t[z] = comp128_t0[comp128_t1[m[z + 16]] ^ m[z]];

		while ((1 << i) > j) {
			k = 0;

			while ((1 << (4 - i)) > k) {
				m[(((2 * k) + 1) << i) + j] =
				    comp128_t0[comp128_t1[t[(k << i) + j]] ^
				    m[(k << i) + 16 + j]];
				m[(k << (i + 1)) + j] = t[(k << i) + j];
				k++;
			}
			j++;
		}
	}

	for (i = 0; i < 16; ++i)
		for (j = 0; j < 8; ++j)
			r[i] = (uint8_t) (r[i] ^ (((m[((19 * (j + 8 * i) + 19) %
			    256) / 8] >> (3 * j + 3) % 8) & 1) << j));

	return;
}

/**
 * \brief COMP128 v3 hash/compression function
 *
 *
 */
int
comp128(
    const uint8_t *k,	/**< [in]  */
    const uint8_t *c,	/**< [in]  */
    uint32_t *r,	/**< [out]  */
    uint64_t *s		/**< [out]  */
) {
	uint8_t km[16];
	uint8_t rm[16];
	uint8_t x[16];
	uint8_t b[16];
	int i;

	for (i = 0; i < 8; ++i) {
		km[     i] = k[15 - i];
		km[15 - i] = k[     i];
		rm[     i] = c[15 - i];
		rm[15 - i] = c[     i];
	}

	for (i = 0; i < 16; ++i)
		x[i] = km[i] ^ rm[i];

	for (i = 0; i < 8; ++i)
		comp128_scramble(rm, x);

	for (i = 0; i < 16; ++i)
		b[i] = rm[i];

	for (i = 0; i < 4; ++i) {
		b[8 + i - 4] = b[8 + i    ];
		b[8 + i    ] = b[8 + i + 4];
	}

	*r = pack8to32(b    );
	*s = pack8to64(b + 4);

	return 0;
}