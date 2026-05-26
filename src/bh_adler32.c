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
 * \file	bh_adler32.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/bit_hash.h>

/** \cond */
#define	ADLER32_MOD	UINT16_C(0xFFF1)
/** \endcond */

/**
 * \brief Mark Adler's 32-bit checksum
 *
 * A quick 32-bit checksum developed by Mark Adler in 1995\cite zlib; it is made
 * up of two 16-bit checksums concatenated together, and is derived from the
 * Fletcher-32 checksum\cite rfc1950.
 *
 * The Adler-32 checksum has better error detection/collision resistance than
 * the Fletcher-16 checksum, but less than Fletcher-32, and also less than that
 * provided by CRC-32. Note however that it has much higher throughput than both
 * CRC-32 and Fletcher-32.
 *
 * \return Checksum value (32-bit).
 */
//-+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
uint32_t
adler32(
    const void *message,	/**< [in] Pointer to the start of the buffer. */
    size_t length	/**< [in] Length of the buffer (in bytes). */
) {
	uint8_t *data;
	uint16_t a, b;
	size_t i;

	data = (uint8_t *) message;

	for (i = 0, a = 1, b = 0; i < length; ++i) {
		a = (uint16_t) ((a + data[i]) % ADLER32_MOD);
		b = (uint16_t) ((b + a) % ADLER32_MOD);
	}

	return (uint32_t) ((b << 16) | a);
}