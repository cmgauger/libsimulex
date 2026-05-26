/*
 * Copyright (c) 2025 Christian Gauger-Cosgrove
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
 * \file	xchacha20_test.c
 * \copyright	MIT
 * \date	2025
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <CUnit/CUnit.h>

#include <simulex/bit_hash.h>
#include <simulex/prng.h>
#include <simulex/util.h>
#include "util_test.h"

#define	EPSILON	0.000001

#include "test_data/util_test_bounded_rand32_data.h"
#include "test_data/util_test_bounded_rand64_data.h"
#include "test_data/util_test_shuffle32_data.h"
#include "test_data/util_test_shuffle64_data.h"

#if defined(_MSC_VER) && (_MSC_VER < 1900)
static unsigned int	 output_format;
#endif

static int
float_equals(float a, float b, float eps) {
	return fabsf(a - b) < eps;
}

static int
double_equals(double a, double b, double eps) {
	return fabs(a - b) < eps;
}

int
init_util_suite(void) {
	/* Seed the PRNG with the current time; we don't want/need repeatable
	 * random numbers. (Not that it matters either way.)
	 */
	srand((unsigned int) time(NULL));

#if defined(_MSC_VER) && (_MSC_VER < 1900)
	/* For Visual Studio versions below VS 2015, we need to set the format
	 * for exponents in the printf() e/E and g/G formats to the standard
	 * two-digit format as opposed to Microsoft's default of three digits.
	 *
	 * We'll save the exponent format to restore later.
	 */
	output_format = _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
	return 0;
}

int
clean_util_suite(void) {
#if defined(_MSC_VER) && (_MSC_VER < 1900)
	/* Here we restore the exponent format to what the application was using
	 * before we ran the test suite setup function.
	 *
	 * Again this is only needed for Microsoft Visual Studio versions below
	 * Visual Studio 2015.
	 */
	_set_output_format(output_format);
#endif

	return 0;
}

void
test_bounded_rand32(void) {
	lcg32_h prng;
	uint32_t r, s;
	int i, j;

	for (i = 0; i < BRAND32_TESTS; ++i) {
		s = bounded_rand32_test_data[i].seed;
		r = bounded_rand32_test_data[i].range;

		prng = lcg32_init(LCG_PRIMARY, s);

		for (j = 0; j < BRAND32_VALUES; ++j)
			CU_ASSERT_EQUAL(bounded_rand32(r, lcg32_rand, prng),
			    bounded_rand32_test_data[i].value[j]);

		lcg32_destroy(&prng);
	}

	return;
}

void
test_bounded_rand64(void) {
	sm64_h prng;
	uint64_t r, s;
	int i, j;

	for (i = 0; i < BRAND64_TESTS; ++i) {
		s = bounded_rand64_test_data[i].seed;
		r = bounded_rand64_test_data[i].range;

		prng = sm64_init(s);

		for (j = 0; j < BRAND64_VALUES; ++j) {
			CU_ASSERT_EQUAL(bounded_rand64(r, sm64_rand, prng),
			    bounded_rand64_test_data[i].value[j]);
		}

		sm64_destroy(&prng);
	}

	return;
}

void
test_shuffle32(void) {
	uint16_t t[SHUFFLE32_VALUES];
	mt32_h prng;
	int i;

	for (i = 0; i < SHUFFLE32_TESTS; ++i) {
		prng = mt32_init_seed(MT_MT19937, shuffle32_test_data[i].seed);

		memcpy(t, shuffle32_test_data[i].original, SHUFFLE32_VALUES * sizeof(uint16_t));
		CU_ASSERT_EQUAL(shuffle32(t, SHUFFLE32_VALUES, sizeof(uint16_t), mt32_rand, prng), 0);
		CU_ASSERT_EQUAL(memcmp(t, shuffle32_test_data[i].shuffled, SHUFFLE32_VALUES * sizeof(uint16_t)), 0);

		mt32_destroy(&prng);
	}

	return;
}

void
test_shuffle64(void) {
	struct test_struct t[SHUFFLE64_VALUES];
	xs64_h prng;
	int i;

	for (i = 0; i < SHUFFLE64_TESTS; ++i) {
		prng = xs64_init(XS64_XOROSHIRO1024SS, shuffle64_test_data[i].seed);

		memcpy(t, shuffle64_test_data[i].original, SHUFFLE64_VALUES * sizeof(struct test_struct));
		CU_ASSERT_EQUAL(shuffle64(t, SHUFFLE64_VALUES, sizeof(struct test_struct), xs64_rand, prng), 0);
		CU_ASSERT_EQUAL(memcmp(t, shuffle64_test_data[i].shuffled, SHUFFLE64_VALUES * sizeof(struct test_struct)), 0);

		xs64_destroy(&prng);
	}

	return;
}

void
test_float_rand32(void) {
	return;
}

void
test_float_rand64(void) {
	return;
}

void
test_double_rand32(void) {
	return;
}

void
test_double_rand64(void) {
	return;
}