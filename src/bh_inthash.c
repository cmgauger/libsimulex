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
 * \file	bh_inthash.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <bit_hash.h>

const struct {
	uint32_t s1;
	uint64_t x1;
	uint32_t s2;
	uint64_t x2;
	uint32_t s3;
} mix64_constants[15] = {
	{33, UINT64_C(0xFF51AFD7ED558CCD), 33, UINT64_C(0xC4CEB9FE1A85EC53), 33},
	{31, UINT64_C(0x7FB5D329728EA185), 27, UINT64_C(0x81DADEF4BC2DD44D), 33},
	{33, UINT64_C(0x64DD81482CBD31D7), 31, UINT64_C(0xE36AA5C613612997), 31},
	{31, UINT64_C(0x99BCF6822B23CA35), 30, UINT64_C(0x14020A57ACCED8B7), 33},
	{33, UINT64_C(0x62A9D9ED799705F5), 28, UINT64_C(0xCB24D0A5C88C35B3), 32},
	{31, UINT64_C(0x79C135C1674B9ADD), 29, UINT64_C(0x54C77C86F6913E45), 30},
	{31, UINT64_C(0x69B0BC90BD9A8C49), 27, UINT64_C(0x3D5E661A2A77868D), 30},
	{30, UINT64_C(0x16A6AC37883AF045), 26, UINT64_C(0xCC9C31A4274686A5), 32},
	{30, UINT64_C(0x294AA62849912F0B), 28, UINT64_C(0x0A9BA9C8A5B15117), 31},
	{32, UINT64_C(0x4CD6944C5CC20B6D), 29, UINT64_C(0xFC12C5B19D3259E9), 32},
	{30, UINT64_C(0xE4C7E495F4C683F5), 32, UINT64_C(0xFDA871BAEA35A293), 33},
	{27, UINT64_C(0x97D461A8B11570D9), 28, UINT64_C(0x02271EB7C6C4CD6B), 32},
	{29, UINT64_C(0x3CD0EB9D47532DFB), 26, UINT64_C(0x63660277528772BB), 33},
	{30, UINT64_C(0xBF58476D1CE4E5B9), 27, UINT64_C(0x94D049BB133111EB), 31},
	{30, UINT64_C(0x4BE98134A5976FD3), 29, UINT64_C(0x3BC0993A5AD19A13), 31}
};

/**
 *
 */
/* <http://zimbry.blogspot.com/2011/09/better-bit-mixing-improving-on.html> */
//-+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
uint64_t
mix64(enum MIX64_TYPE type, uint64_t x) {
	switch (type) {
	case MIX64_MURMUR3_64:
	case MIX64_STAFFORD01:
	case MIX64_STAFFORD02:
	case MIX64_STAFFORD03:
	case MIX64_STAFFORD04:
	case MIX64_STAFFORD05:
	case MIX64_STAFFORD06:
	case MIX64_STAFFORD07:
	case MIX64_STAFFORD08:
	case MIX64_STAFFORD09:
	case MIX64_STAFFORD10:
	case MIX64_STAFFORD11:
	case MIX64_STAFFORD12:
	case MIX64_STAFFORD13:
	case MIX64_STAFFORD14:
		x ^= (x >> mix64_constants[type - MIX64_MURMUR3_64].s1);
		x *= mix64_constants[type - MIX64_MURMUR3_64].x1;
		x ^= (x >> mix64_constants[type - MIX64_MURMUR3_64].s2);
		x *= mix64_constants[type - MIX64_MURMUR3_64].x2;
		x ^= (x >> mix64_constants[type - MIX64_MURMUR3_64].s3);

		return x;
	default:
		return 0;
	}
}

/**
 *
 */
/* <https://wiki.herzbube.ch/index.php/LearningDoxygen#Groups> */
//-+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
uint32_t
mix96(
    uint32_t a,		/**<  */
    uint32_t b,		/**<  */
    uint32_t c		/**<  */
) {
	/* Round I */
	a -= b; a -= c; a ^= c >> 13;
	b -= c; b -= a; b ^= a <<  8;
	c -= a; c -= b; c ^= b >> 13;

	/* Round II */
	a -= b; a -= c; a ^= c >> 12;
	b -= c; b -= a; b ^= a << 16;
	c -= a; c -= b; c ^= b >>  5;

	/* Round III */
	a -= b; a -= c; a ^= c >>  3;
	b -= c; b -= a; b ^= a << 10;
	c -= a; c -= b; c ^= b >> 15;
	
	return c;
}

/**
 * \brief Christopher Wellons' minimal bias 32-bit integer hash function
 *
 * This hash function reaches the theoretical minimum bias limit for a 32-bit
 * integer hash, the hash function is also a "perfect" hash (in that there are
 * no collisions on the mapping of input to output). It consists of three rounds
 * of XORSHIFT then MULTIPLY operations. The hash was "prospected" (the hash
 * parameters were discovered using automated means) by Christopher Wellons
 * (<a href="mailto:wellons@nullprogram.com">wellons@nullprogram.com</a>) in
 * August of 2018, and was released into the public domain.
 *
 * \return hashed version of the input.
 */
uint32_t
triple32(
    uint32_t x		/**< [in] 32-bit (unsigned) integer to be hashed. */
) {
	x ^= x >> 17;
	x *= UINT32_C(0xED5AD4BB);
	x ^= x >> 11;
	x *= UINT32_C(0xAC4C1B51);
	x ^= x >> 15;
	x *= UINT32_C(0x31848BAB);
	x ^= x >> 14;

	return x;
}