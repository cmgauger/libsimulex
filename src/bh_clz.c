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
 * \file	bh_clz.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>

#include <bit_hash.h>

/**
 *
 */
int
clz8(
    uint8_t x		/**<  */
) {
	uint8_t p[4];

	p[0] = 0;
	p[1] = 0;
	p[2] = 0;
	p[3] = x;

	return clz32(pack8to32(p) - 24);
}

/**
 *
 */
int
clz16(
    uint16_t x		/**<  */
) {
	uint16_t p[2];

	p[0] = 0;
	p[1] = x;

	return clz32(pack16to32(p) - 16);
}

/**
 *
 */
int
clz32(
    uint32_t x		/**<  */
) {
	const int table[] = {
		 0,  9,  1, 10, 13, 21,  2, 29,
		11, 14, 16, 18, 22, 25,  3, 30,
		 8, 12, 20, 28, 15, 17, 24,  7,
		19, 27, 23,  6, 26,  5,  4, 31
	};
	const int y[] = {1, 2, 4, 8, 16};
	int i;

	for (i = 0; i < 5; ++i)
		x |= (x >> y[i]);

	return 31 - table[((x * UINT32_C(0x07C4ACDD)) >> 27) & 0x1F];
}

/**
 *
 */
int
clz64(
    uint64_t x		/**<  */
) {
	uint32_t h, l;

	split64(x, &h, &l);

	if (h == 0) {
		return 32 + clz32(l);
	} else {
		return clz32(h);
	}
}