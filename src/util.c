/*
 * Copyright (c) 2022 Christian Gauger-Cosgrove
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
 * \file	util.c
 * \copyright	MIT
 * \date	2022
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.5
 */
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bit_hash.h>
#include <util.h>

/**
 * \brief Hash function fo Andrew Kensler's \c permute() function
 *
 */
static uint32_t
util_hash(
    uint32_t index,	/**< [in]  */
    uint32_t mask,	/**< [in]  */
    uint32_t seed	/**< [in]  */
) {
	index ^= seed;
	index *= UINT32_C(0xE170893D);
	index ^= seed >> 16;
	index ^= (index & mask) >> 4;
	index ^= seed >> 8;
	index *= UINT32_C(0x0929EB3F);
	index ^= seed >> 23;
	index ^= (index & mask) >> 1;
	index *= 1 | seed >> 27;
	index *= UINT32_C(0x6935FA69);
	index ^= (index & mask) >> 11;
	index *= UINT32_C(0x74DCB303);
	index ^= (index & mask) >> 2;
	index *= UINT32_C(0x9E501CC3);
	index ^= (index & mask) >> 2;
	index *= UINT32_C(0xC860A3DF);
	index &= mask;
	index ^= index >> 5;

	return index;
}

/**
 *
 */
uint32_t
bounded_rand32(uint32_t range, uint32_t (*rng)(void *), void *rng_state) {
	uint32_t mask, x;
	int shift;

	--range;
	shift = clz32(range | 1);

	mask = ~UINT32_C(0);
	mask >>= shift;

	do {
		x = (rng(rng_state) >> shift) & mask;;
	} while (x > range);

	return x;
}

/**
 * \brief
 *
 */
uint32_t
permute(
    uint32_t index,	/**< [in]  */
    uint32_t length,	/**< [in]  */
    uint32_t seed	/**< [in]  */
) {
	uint32_t mask;

	mask = length -1;
	mask |= mask >> 1;
	mask |= mask >> 2;
	mask |= mask >> 4;
	mask |= mask >> 8;
	mask |= mask >> 16;

	do {
		index =	util_hash(index, mask, seed);
	} while (index >= length);

	return (index + seed) % length;
}

/**
 *
 */
int
shuffle32(void *list, const size_t length, const size_t size, uint32_t (*rng)(void *), void *rng_state) {
	char *a, *t;
	size_t i, p;

	if (1 > (INT_MAX - length))
		return 1;

	a = (char *) list;

	t = (char *) malloc(size * sizeof(char));
	if (t == NULL)
		return 1;

	for (i = (length - 1); i > 0; --i) {
		p = (size_t) bounded_rand32((uint32_t) (i + 1), rng, rng_state);

		if (p != i) {
			memcpy(t, &a[size * i], size);
			memcpy(&a[size * i], &a[size * p], size);
			memcpy(&a[size * p], t, size);
		}
	}

	free(t);

	return 0;
}

/**
 * \brief De-biased bounded rand() function
 *
 * This function is a de-biased bounded pseudo-random number function, built to
 * make use of a generic 64-bit pseudo-random number generator. The function
 * operates according to the same principles as the de-biased bounded random
 * function provided in Java.
 *
 * \param[in]  range  Upper limit (exclusive) of the number to be emitted.
 * \param[in]  rng  Pseudo-random number generator function
 * \param[in,out]  rng_state  State variable for the PRNG
 *
 * \return Pseudorandomly generated integer value between [0,range).
 */
uint64_t
bounded_rand64(uint64_t range, uint64_t (*rng)(void *), void *rng_state) {
	uint64_t mask, x;
	int shift;

	--range;
	shift = clz64(range | 1);

	mask = ~UINT64_C(0);
	mask >>= shift;

	do {
		x = (rng(rng_state) >> shift) & mask;
	} while (x > range);

	return x;
}

/**
 * \brief Type-generic Fisher-Yates shuffle using a generic 64-bit PRNG
 *
 * This is a genericized Fisher-Yates shuffle, utilizing the de-biased bounded
 * random number function \ref bounded_rand defined earlier. There is no defined
 * maximum size of array to shuffle, however it should be noted that the size of
 * the state variable of the pseudo-random number generator will limit the array
 * size in terms of ensuring all possible permutations are generated.
 *
 * Refer to the \ref limitCipher "table of maximal list lengths for ciphers" for
 * the maximum list lengths that guaraneed all possible permutations are
 * generated.
 *
<table>
<caption id="limitCipher"></caption>
<tr><th>Cipher<sup>1</sup></th>
    <th>Block Size</th>
    <th>Key Size</th>
    <th colspan="2">Nonce Size<sup>2</sup></th>
    <th>State Size<sup>3</sup></th>
    <th>List Length</th></tr>
<tr><td>DES</td><td>64</td><td>56</td><td colspan="2">64</td>
    <td>120</td><td>32</td></tr>
<tr><td>GOST 28147-89 "Magma"</td><td>64</td><td>256</td>
    <td colspan="2">64</td><td>320</td><td>67</td></tr>
<tr><td>GOST R 34.12-2015 "Kuznyechik"</td><td>128</td><td>256</td>
    <td colspan="2">128</td><td>384</td><td>78</td></tr>
<tr><td rowspan="3">AES</td><td rowspan="3">128</td><td>128</td>
    <td rowspan="3" colspan="2">128</td><td>256</td><td>57</td></tr>
<tr><td>192</td><td>320</td><td>67</td></tr>
<tr><td>256</td><td>384</td><td>78</td></tr>
<tr><td rowspan="2">ChaCha20<sup>4</sup></td><td rowspan="4">512</td>
    <td rowspan="4">256</td><td>64</td><td>64</td><td>384</td><td>78</td></tr>
<tr><td>96</td><td>32</td><td>384</td><td>78</td></tr>
<tr><td rowspan="2">XChaCha20<sup>4</sup></td><td rowspan="2">192</td><td>64</td>
    <td>512</td><td>98</td></tr>
<tr><td>32</td><td>480</td><td>93</td></tr>
</table>
 *
 * There are some notes in \ref limitCipher which require further explanation:
 *     (1) The block ciphers (DES, AES, and GOST) are assumed to be operating in
 *     output feedback (OFB) or counter (CTR) mode; whereas the stream ciphers
 *     (ChaCha20, and XChaCha20) are being operated in their standard mode.
 *     (2) The nonce size also includes the size of the block count for the
 *     ChaCha series ciphers.
 *     (3) The state size corresponds to the key size of the cipher, plus the
 *     size of its nonce (including he block count as mentioned in point 2).
 *     (4) Both the original definition of the ChaCha series ciphers as per
 *     Bernstein, and the IETF version are defined here. Note that for the basic
 *     ChaCha20 form, both Bernstein and IETF forms are the same overall state
 *     size, however for the XChaCha20 form the Bernstein form provides a larger
 *     state.
 *
 * In the \ref linitPRNG "table of maximal list lengths for PRNGs" various sizes
 * of PRNG state (in bits) are listed with the maximal length of list that is
 * guaranteed to have all of its possible permutations generated by a PRNG of
 * that size.
 *
<table>
<caption id="linitPRNG"></caption>
<tr><th>State Size</th><th>List Length</th></tr>
<tr><td>8</td><td>5</td></tr>
<tr><td>16</td><td>8</td></tr>
<tr><td>32</td><td>12</td></tr>
<tr><td>64</td><td>20</td></tr>
<tr><td>128</td><td>34</td></tr>
<tr><td>256</td><td>57</td></tr>
<tr><td>512</td><td>98</td></tr>
<tr><td>521</td><td>99</td></tr>
<tr><td>607</td><td>112</td></tr>
<tr><td>800</td><td>139</td></tr>
<tr><td>1024</td><td>170</td></tr>
<tr><td>19937</td><td>2080</td></tr>
<tr><td>21701</td><td>2239</td></tr>
<tr><td>23209</td><td>2374</td></tr>
<tr><td>44497</td><td>4199</td></tr>
</table>
 *
 * \param[in,out]  list  Void pointer to the head of the array of items that are
 *     to be shuffled.
 * \param[in]  length  Number of total items which are to be shuffled.
 * \param[in]  size  Size of each item in the array.
 * \param[in]  rng  Pseudo-random number generator function
 * \param[in,out]  rng_state  State variable for the PRNG
 *
 * \return 0
 */
int
shuffle64(void *list, const size_t length, const size_t size, uint64_t (*rng)(void *), void *rng_state) {
	char *a, *t;
	size_t i, p;

	a = (char *) list;

	t = (char *) malloc(size * sizeof(char));
	if (t == NULL)
		return 1;

	for (i = (length - 1); i > 0; --i) {
		p = (size_t) bounded_rand64(i + 1, rng, rng_state);

		memcpy(t, &a[size * i], size);
		memcpy(&a[size * i], &a[size * p], size);
		memcpy(&a[size * p], t, size);
	}

	free(t);

	return 0;
}

/**
 * \brief
 */
float
float_rand32(uint32_t (*rng)(void *), void *rng_state) {
	uint32_t x;
	float f;

	/* Get a [pseudo-]random value */
	x = rng(rng_state);

	/* Twiddle the bits to make it an IEEE 754 'float' in [1.0,2.0) */
	x = ((x >> 9) & UINT32_C(0x007FFFFF)) | UINT32_C(0x3F800000);
	memcpy(&f, &x, 4);

	/* Get the value in the range [0.0,1.0) and return it */
	return (f - 1.0f);
}

/**
 * \brief
 */
double
double_rand32(uint32_t (*rng)(void *), void *rng_state) {
	uint64_t x, y;
	double d;

	/* Get two [pseudo-]random values, one after the other */
	x = rng(rng_state);
	y = rng(rng_state);

	/* Twiddle the bits to make it an IEEE 754 'double' in [1.0,2.0) */
	x = (x >> 6) & UINT64_C(0x0000000003FFFFFF);
	y = (y >> 6) & UINT64_C(0x0000000003FFFFFF);
	x = (((x << 26) | y) & UINT64_C(0x000FFFFFFFFFFFFF)) | UINT64_C(0x3FF0000000000000);
	memcpy(&d, &x, 8);

	/* Get the value in the range [0.0,1.0) and return it */
	return (d - 1.0);
}

/**
 * \brief
 */
float
float_rand64(uint64_t (*rng)(void *), void *rng_state) {
	uint64_t y;
	uint32_t x;
	float f;

	/* Get a [pseudo-]random value */
	y = rng(rng_state);

	/* Twiddle the bits to make it an IEEE 754 'float' in [1.0,2.0) */
	x = ((uint32_t) (y >> 41) & UINT32_C(0x007FFFFF)) |
	    UINT32_C(0x3F800000);
	memcpy(&f, &x, 4);

	/* Get the value in the range [0.0,1.0) and return it */
	return (f - 1.0f);
}

/**
 * \brief
 */
double
double_rand64(uint64_t (*rng)(void *), void *rng_state) {
	uint64_t x;
	double d;

	/* Get a [pseudo-]random value */
	x = rng(rng_state);

	/* Twiddle the bits to make it an IEEE 754 'double' in [1.0,2.0) */
	x = ((x >> 12) & UINT64_C(0x000FFFFFFFFFFFFF)) | UINT64_C(0x3FF0000000000000);
	memcpy(&d, &x, 8);

	/* Get the value in the range [0.0,1.0) and return it */
	return (d - 1.0);
}

/*

<https://prng.di.unimi.it/random_real.c>

float
random_real(void)
{
    int exponent = -32;
    uint32_t significand;
    unsigned shift;
    while (__predict_false((significand = random32()) == 0)) {
        exponent -= 32;
        if (__predict_false(exponent < -149))
            return 0;
    }
    shift = clz32(significand);
    if (shift != 0) {
        exponent -= shift;
        significand <<= shift;
        significand |= (random32() >> (32 - shift));
    }
    significand |= 1;
    return ldexpf((float)significand, exponent);
}

double
random_real(void)
{
	int exponent = -64;
	uint64_t significand;
	unsigned shift;

	// Read zeros into the exponent until we hit a one; the rest
	// will go into the significand.
	while (__predict_false((significand = random64()) == 0)) {
		exponent -= 64;

		// If the exponent falls below -1074 = emin + 1 - p,
		// the exponent of the smallest subnormal, we are
		// guaranteed the result will be rounded to zero.  This
		// case is so unlikely it will happen in realistic
		// terms only if random64 is broken.
		if (__predict_false(exponent < -1074))
			return 0;
	}

	// There is a 1 somewhere in significand, not necessarily in
	// the most significant position.  If there are leading zeros,
	// shift them into the exponent and refill the less-significant
	// bits of the significand.  Can't predict one way or another
	// whether there are leading zeros: there's a fifty-fifty
	// chance, if random64 is uniformly distributed.
	shift = clz64(significand);
	if (shift != 0) {
		exponent -= shift;
		significand <<= shift;
		significand |= (random64() >> (64 - shift));
	}

	// Set the sticky bit, since there is almost surely another 1
	// in the bit stream.  Otherwise, we might round what looks
	// like a tie to even when, almost surely, were we to look
	// further in the bit stream, there would be a 1 breaking the
	// tie.
	significand |= 1;

	// Finally, convert to double (rounding) and scale by
	// 2^exponent.
	return ldexp((double)significand, exponent);
}
*/
