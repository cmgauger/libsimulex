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
 * \file	bh_rotate.c
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
uint16_t
rotl16(
    uint16_t x,		/**<  */
    int k		/**<  */
) {
	int t;

	t = k % 16;
	if (t == 0)
		return x;

	return (uint16_t) (((x << t)) | ((x >> (16 - t))));
}

/**
 *
 */
uint16_t
rotr16(
    uint16_t x,		/**<  */
    int k		/**<  */
) {
	int t;

	t = k % 16;
	if (t == 0)
		return x;

	return (uint16_t) (((x >> t)) | ((x << (16 - t))));
}

/**
 *
 */
uint32_t
rotl32(
    uint32_t x,		/**<  */
    int k		/**<  */
) {
	int t;

	t = k % 32;
	if (t == 0)
		return x;

	return (x << t) | (x >> (32 - t));
}

/**
 *
 */
uint32_t
rotr32(
    uint32_t x,		/**<  */
    int k		/**<  */
) {
	int t;

	t = k % 32;
	if (t == 0)
		return x;

	return (x >> t) | (x << (32 - t));
}

/**
 *
 */
uint64_t
rotl64(
    uint64_t x,		/**<  */
    int k		/**<  */
) {
	int t;

	t = k % 64;
	if (t == 0)
		return x;

	return (x << t) | (x >> (64 - t));
}

/**
 *
 */
uint64_t
rotr64(
    uint64_t x,		/**<  */
    int k		/**<  */
) {
	int t;

	t = k % 64;
	if (t == 0)
		return x;

	return (x >> t) | (x << (64 - t));
}