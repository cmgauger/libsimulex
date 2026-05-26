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
 * \file	bh_combine.c
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
uint16_t combine16(uint8_t hi, uint8_t lo) {
	uint8_t v[2];

	v[0] = lo;
	v[1] = hi;

	return pack8to16(v);
}

/**
 *
 */
uint32_t combine32(uint16_t hi, uint16_t lo) {
	uint16_t v[2];

	v[0] = lo;
	v[1] = hi;

	return pack16to32(v);
}

/**
 *
 */
uint64_t combine64(uint32_t hi, uint32_t lo) {
	uint32_t v[2];

	v[0] = lo;
	v[1] = hi;

	return pack32to64(v);
}