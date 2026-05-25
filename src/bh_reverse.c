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
 * \file	bh_reverse.c
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
uint8_t
rev8(
    uint8_t v		/**<  */
) {
	/* Based on <https://graphics.stanford.edu/~seander/bithacks.html> */

	/* Swap odd and even bits */
	v = (uint8_t) (((v >> 1) & 0x55U) | ((v & 0x55U) << 1));
	/* Swap consecutive bit pairs */
	v = (uint8_t) (((v >> 2) & 0x33U) | ((v & 0x33U) << 2));
	/* Swap nibbles */
	v = (uint8_t) (( v >> 4         ) | ( v          << 4));

	/* Emit result */
	return v;
}

/**
 *
 */
uint16_t
rev16(
    uint16_t v		/**<  */
) {
	/* Based on <https://graphics.stanford.edu/~seander/bithacks.html> */

	/* Swap odd and even bits */
	v = (uint16_t) (((v >> 1) & UINT16_C(0x5555)) |
	    ((v & UINT16_C(0x5555)) << 1));
	/* Swap consecutive bit pairs */
	v = (uint16_t) (((v >> 2) & UINT16_C(0x3333)) |
	    ((v & UINT16_C(0x3333)) << 2));
	/* Swap nibbles */
	v = (uint16_t) (((v >> 4) & UINT16_C(0x0F0F)) |
	    ((v & UINT16_C(0x0F0F)) << 4));
	/* Swap bytes */
	v = (uint16_t) (( v >> 8                    ) |
	    ( v                     << 8));

	/* Emit result */
	return v;
}

/**
 *
 */
uint32_t
rev32(
    uint32_t v		/**<  */
) {
	/* Based on <https://graphics.stanford.edu/~seander/bithacks.html> */

	/* Swap odd and even bits */
	v = ((v >>  1) & UINT32_C(0x55555555)) |
	    ((v & UINT32_C(0x55555555) << 1));
	/* Swap consecutive bit pairs */
	v = ((v >>  2) & UINT32_C(0x33333333)) |
	    ((v & UINT32_C(0x33333333) <<  2));
	/* Swap nibbles */
	v = ((v >>  4) & UINT32_C(0x0F0F0F0F)) |
	    ((v & UINT32_C(0x0F0F0F0F) <<  4));
	/* Swap bytes */
	v = ((v >>  8) & UINT32_C(0x00FF00FF)) |
	    ((v & UINT32_C(0x00FF00FF) <<  8));
	/* Swap 16-bit words */
	v = ( v >> 16                        ) |
	    ( v                        << 16 );

	/* Emit result */
	return v;
}

/**
 *
 */
uint64_t
rev64(
    uint64_t v		/**<  */
) {
	/* Based on <https://graphics.stanford.edu/~seander/bithacks.html> */

	/* Swap odd and even bits */
	v = ((v >>  1) & UINT64_C(0x5555555555555555)) |
	    ((v & UINT64_C(0x5555555555555555)) <<  1);
	/* Swap consecutive bit pairs */
	v = ((v >>  2) & UINT64_C(0x3333333333333333)) |
	    ((v & UINT64_C(0x3333333333333333)) <<  2);
	/* Swap nibbles */
	v = ((v >>  4) & UINT64_C(0x0F0F0F0F0F0F0F0F)) |
	    ((v & UINT64_C(0x0F0F0F0F0F0F0F0F)) <<  4);
	/* Swap bytes */
	v = ((v >>  8) & UINT64_C(0x00FF00FF00FF00FF)) |
	    ((v & UINT64_C(0x00FF00FF00FF00FF)) <<  8);
	/* Swap 16-bit words */
	v = ((v >> 16) & UINT64_C(0x0000FFFF0000FFFF)) |
	    ((v & UINT64_C(0x0000FFFF0000FFFF)) << 16);
	/* Swap 32-bit words */
	v = ( v >> 32                                ) |
	    ( v                                 << 32);

	/* Emit result */
	return v;
}