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
 * \file	bh_unpack.c
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
unpack64to8(
    uint64_t v,		/**<  */
    uint8_t *u		/**<  */
) {
	u[7] = (uint8_t) ((v >> 56) & 0xFF);
	u[6] = (uint8_t) ((v >> 48) & 0xFF);
	u[5] = (uint8_t) ((v >> 40) & 0xFF);
	u[4] = (uint8_t) ((v >> 32) & 0xFF);
	u[3] = (uint8_t) ((v >> 24) & 0xFF);
	u[2] = (uint8_t) ((v >> 16) & 0xFF);
	u[1] = (uint8_t) ((v >>  8) & 0xFF);
	u[0] = (uint8_t) ((v      ) & 0xFF);

	return;
}

/**
 *
 */
void
unpack64to16(
    uint64_t v,		/**<  */
    uint16_t *u		/**<  */
) {
	u[3] = (uint16_t) ((v >> 48) & 0xFFFF);
	u[2] = (uint16_t) ((v >> 32) & 0xFFFF);
	u[1] = (uint16_t) ((v >> 16) & 0xFFFF);
	u[0] = (uint16_t) ((v      ) & 0xFFFF);

	return;
}

/**
 *
 */
void
unpack64to32(
    uint64_t v,		/**<  */
    uint32_t *u		/**<  */
) {
	/*  */
	u[1] = (uint32_t) ((v >> 32) & 0xFFFFFFFF);
	u[0] = (uint32_t) ((v      ) & 0xFFFFFFFF);

	return;
}

/**
 *
 */
void
unpack32to8(
    uint32_t v,		/**<  */
    uint8_t *u		/**<  */
) {
	u[3] = (uint8_t) ((v >> 24) & 0xFF);
	u[2] = (uint8_t) ((v >> 16) & 0xFF);
	u[1] = (uint8_t) ((v >>  8) & 0xFF);
	u[0] = (uint8_t) ((v      ) & 0xFF);

	return;
}

/**
 *
 */
void
unpack32to16(
    uint32_t v,		/**<  */
    uint16_t *u		/**<  */
) {
	u[1] = (uint16_t) ((v >> 16) & 0xFFFF);
	u[0] = (uint16_t) ((v      ) & 0xFFFF);

	return;
}

/**
 *
 */
void
unpack16to8(
    uint16_t v,		/**<  */
    uint8_t *u		/**<  */
) {
	u[1] = (uint8_t) ((v >>  8) & 0xFF);
	u[0] = (uint8_t) ((v      ) & 0xFF);

	return;
}