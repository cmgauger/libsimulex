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
 * \file	bh_split.c
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
void
split8(
    uint16_t x,		/**<  */
    uint8_t *hi,	/**<  */
    uint8_t *lo		/**<  */
) {
	*hi = hi8(x);
	*lo = lo8(x);

	return;
}

/**
 *
 */
void
split16(
    uint32_t x,		/**<  */
    uint16_t *hi,	/**<  */
    uint16_t *lo	/**<  */
) {
	*hi = hi16(x);
	*lo = lo16(x);

	return;
}

/**
 *
 */
void
split32(
    uint64_t x,		/**<  */
    uint32_t *hi,	/**<  */
    uint32_t *lo	/**<  */
) {
	*hi = hi32(x);
	*lo = lo32(x);

	return;
}