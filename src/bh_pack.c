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
 * \file	bh_pack.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>

#include <simulex/bit_hash.h>

/**
 *
 */
uint16_t
pack8to16(
    uint8_t *u		/**<  */
) {
	uint16_t v;

	v = (uint16_t) ((((uint16_t) u[1]) << 8) | (((uint16_t) u[0])     ));

	return v;
}

/**
 *
 */
uint32_t
pack8to32(
    uint8_t *u		/**<  */
) {
	uint32_t v;

	v = (((uint32_t) u[3]) << 24) | (((uint32_t) u[2]) << 16) |
	    (((uint32_t) u[1]) <<  8) | (((uint32_t) u[0])      );

	return v;
}

/**
 *
 */
uint64_t
pack8to64(
    uint8_t *u		/**<  */
) {
	uint64_t v;

	v = (((uint64_t) u[7]) << 56) | (((uint64_t) u[6]) << 48) |
	    (((uint64_t) u[5]) << 40) | (((uint64_t) u[4]) << 32) |
	    (((uint64_t) u[3]) << 24) | (((uint64_t) u[2]) << 16) |
	    (((uint64_t) u[1]) <<  8) | (((uint64_t) u[0])      );

	return v;
}

/**
 *
 */
uint32_t
pack16to32(
    uint16_t *u		/**<  */
) {
	uint32_t v;

	v = (((uint32_t) u[1]) << 16) | (((uint32_t) u[0])      );

	return v;
}

/**
 *
 */
uint64_t
pack16to64(
    uint16_t *u		/**<  */
) {
	uint64_t v;

	v = (((uint64_t) u[3]) << 48) | (((uint64_t) u[2]) << 32) |
	    (((uint64_t) u[1]) << 16) | (((uint64_t) u[0])      );

	return v;
}

/**
 *
 */
uint64_t
pack32to64(
    uint32_t *u		/**<  */
) {
	uint64_t v;

	v = (((uint64_t) u[1]) << 32) | (((uint64_t) u[0])      );

	return v;
}