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
 * \file	bh_vms.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/bit_hash.h>

/**
 * \brief VMS \c LGI$HPWD string "collapse" function
 *
 * This function "collapses" a buffer (of arbitrary length) into a 64-bit value,
 * by cycling around the eight bytes of the that make up the value, and adding
 * in the bytes of the input string. The \ref COLLAPSE_TYPE specifies which
 * "flavour" of the algorithm to use; with \c _V and \c _S being the same 
 * algorithm as used in the VAX/VMS \c LGI$HPWD password hashing function
 * (corresponding to the \c _V and \c _S modes thereof), \c _X is a modification
 * of the algorithm to rotate the bits on every eighth character (unlike in the
 * \c _S mode where, due to the way the algorithm is implemented, the first time
 * the rotation occurs is not necessarily after the eighth character is
 * processed), with an "extra" rotation after the buffer is processed (to
 * account for/accomodate the case where there are less than eight bytes of
 * input to be processed).
 *
 * The \c collapse() functions is further modified from the implementation in
 * \c LGI$HPWD in that it now operates on an arbitrary buffer of any possible
 * length, as opposed to only a string (with a maximum length of 31 characters).
 *
 * \returns 64-bit value into which the buffer has been "collapsed."
 */
uint64_t
collapse(
    enum COLLAPSE_TYPE type,	/**< [in] "Collapse" algorithm to be used. */
    uint64_t initial,	/**< [in] Initial value of the 64-bit buffer. */
    const void *data,	/**< [in] Pointer to beginning of data buffer. */
    size_t length	/**< [in] Length of data buffer (in bytes). */
) {
	uint8_t *buffer;
	uint8_t set[8];
	uint64_t output;
	uint32_t a, b;
	size_t i, j, x;

	/* Exit if the length is zero otherwise setup the buffer */
	if (length == 0)
		return initial;
	
	/*  */
//-+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
	buffer = (uint8_t *) data;

	/* Breakout the initial value to the working set */
	unpack64to8(initial, set);

	switch (type) {
	case COLLAPSE_V:
	case COLLAPSE_S:
		for (i = length, j = 0; i != 0; --i, ++j) {
			/*  */
			x = i & 0x07;

			/*  */
			set[x] += buffer[j];

			/*  */
			if ((type == COLLAPSE_S) && (x == 7)) {
				/* Pack working set into two 32-bit values */
				a = pack8to32(set);
				b = pack8to32(set + 4);
				
				/* Rotate the two 32-bit words */
				a = rotl32(a, 1);
				b = rotl32(b, 1);

				/* Unpack the two 32-bit words */
				unpack32to8(a, set);
				unpack32to8(b, set + 4);
			}
		}
		break;
	case COLLAPSE_X:
		for (i = 0, j = 0; i < length; ++i, ++j) {
			/*  */
			x = i & 0x07;

			/*  */
			set[x] += buffer[j];

			/*  */
			if (x == 7) {
				/* Pack working set into two 32-bit values */
				a = pack8to32(set);
				b = pack8to32(set + 4);

				/* Rotate the two 32-bit words */
				a = rotl32(a, 1);
				b = rotl32(b, 1);

				/* Unpack the two 32-bit words */
				unpack32to8(a, set);
				unpack32to8(b, set + 4);
			}
		}

		/* Pack working set into two 32-bit values */
		a = pack8to32(set);
		b = pack8to32(set + 4);

		/* Rotate the two 32-bit words */
		a = rotl32(a, 1);
		b = rotl32(b, 1);

		/* Unpack the two 32-bit words */
		unpack32to8(a, set);
		unpack32to8(b, set + 4);

		break;
	default:
		return initial;
		break;
	}

	/* Pack the working set back into the 64-bit word */
	output = pack8to64(set);

	/* Return the packed output word */
	return output;
}

/**
 *
 */
//-+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
uint64_t
dispatch64(
    uint32_t x		/**<  */
) {
	return UINT64_C(0x0000000000000000) |
	    ((uint64_t) (x & UINT32_C(0xFF000000)) << 24) |
	    ((uint64_t) (x & UINT32_C(0x00FF0000)) << 16) |
	    ((uint64_t) (x & UINT32_C(0x0000FF00)) << 16) |
	    ((uint64_t) (x & UINT32_C(0x000000FF)) <<  8);
}