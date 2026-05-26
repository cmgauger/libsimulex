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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <CUnit/CUnit.h>

#include <simulex/prng.h>
#include "prng_test.h"

#define	TEST_SETS	5
#define	TEST_POINTS	16

#include "test_data/prng_test_lcg16_data.h"
#include "test_data/prng_test_lcg32_data.h"
#include "test_data/prng_test_lcg64_data.h"
#include "test_data/prng_test_sm64_data.h"
#include "test_data/prng_test_mt32_data.h"
#include "test_data/prng_test_well32_data.h"

/**
 * \brief
 */
int
init_prng_suite(void) {
	return 0;
}

/**
 * \brief
 */
int
clean_prng_suite(void) {
	return 0;
}

/**
 * \brief
 */
void
test_lcg16(void) {
	lcg16_h lcg16;
	uint16_t test_array[TEST_POINTS];
	int i, j;

	/* Test the initializer and destructor functions */
	CU_ASSERT_PTR_NULL(lcg16 = lcg16_init(LCG__ERROR, 0));
	CU_ASSERT_PTR_NULL(lcg16 = lcg16_init(42, 0));
	CU_ASSERT_PTR_NULL(lcg16 = lcg16_init_custom(0, 42, 0));
	CU_ASSERT_PTR_NULL(lcg16 = lcg16_init_custom(42, 0, 0));
	CU_ASSERT_EQUAL(lcg16_destroy(&lcg16), 1);
	CU_ASSERT_EQUAL(lcg16_destroy(NULL), 1);
	CU_ASSERT_PTR_NOT_NULL(lcg16 = lcg16_init(LCG_PRIMARY, 0));
	CU_ASSERT_EQUAL(lcg16_destroy(&lcg16), 0);
	CU_ASSERT_PTR_NOT_NULL(lcg16 = lcg16_init(LCG_ALTERNATE, 0));
	CU_ASSERT_EQUAL(lcg16_destroy(&lcg16), 0);

	/* Iterate through the test sets */
	for (i = 0; i < TEST_SETS; ++i) {
		/* Test the LCG_PRIMARY algoritm first */
		lcg16 = lcg16_init(LCG_PRIMARY, prng_lcg16_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = lcg16_rand(lcg16);
		CU_ASSERT_EQUAL(memcmp(prng_lcg16_test_data[i].primary.value,
		    test_array, TEST_POINTS * sizeof(uint16_t)), 0);
		lcg16_destroy(&lcg16);

		/* Test the LCG_ALTERNATE algoritm first */
		lcg16 = lcg16_init(LCG_ALTERNATE, prng_lcg16_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = lcg16_rand(lcg16);
		CU_ASSERT_EQUAL(memcmp(prng_lcg16_test_data[i].alternate.value,
		    test_array, TEST_POINTS * sizeof(uint16_t)), 0);
		lcg16_destroy(&lcg16);

		/* Test the custom 16-bit LCG now */
		lcg16 = lcg16_init_custom(
		    prng_lcg16_test_data[i].custom.additive,
		    prng_lcg16_test_data[i].custom.multiplier,
		    prng_lcg16_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = lcg16_rand(lcg16);
		CU_ASSERT_EQUAL(memcmp(prng_lcg16_test_data[i].custom.value,
		    test_array, TEST_POINTS * sizeof(uint16_t)), 0);
		lcg16_destroy(&lcg16);
	}

	return;
}


/**
 * \brief
 */
void
test_lcg32(void) {
	lcg32_h lcg32;
	uint32_t test_array[TEST_POINTS];
	int i, j;

	/* Test the initializer and destructor functions */
	CU_ASSERT_PTR_NULL(lcg32 = lcg32_init(LCG__ERROR, 0));
	CU_ASSERT_PTR_NULL(lcg32 = lcg32_init(42, 0));
	CU_ASSERT_PTR_NULL(lcg32 = lcg32_init_custom(0, 42, 0));
	CU_ASSERT_PTR_NULL(lcg32 = lcg32_init_custom(42, 0, 0));
	CU_ASSERT_EQUAL(lcg32_destroy(&lcg32), 1);
	CU_ASSERT_EQUAL(lcg32_destroy(NULL), 1);
	CU_ASSERT_PTR_NOT_NULL(lcg32 = lcg32_init(LCG_PRIMARY, 0));
	CU_ASSERT_EQUAL(lcg32_destroy(&lcg32), 0);
	CU_ASSERT_PTR_NOT_NULL(lcg32 = lcg32_init(LCG_ALTERNATE, 0));
	CU_ASSERT_EQUAL(lcg32_destroy(&lcg32), 0);

	/* Iterate through the test sets */
	for (i = 0; i < TEST_SETS; ++i) {
		/* Test the LCG_PRIMARY algoritm first */
		lcg32 = lcg32_init(LCG_PRIMARY, prng_lcg32_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = lcg32_rand(lcg32);
		CU_ASSERT_EQUAL(memcmp(prng_lcg32_test_data[i].primary.value,
		    test_array, TEST_POINTS * sizeof(uint32_t)), 0);
		lcg32_destroy(&lcg32);

		/* Test the LCG_ALTERNATE algoritm first */
		lcg32 = lcg32_init(LCG_ALTERNATE, prng_lcg32_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = lcg32_rand(lcg32);
		CU_ASSERT_EQUAL(memcmp(prng_lcg32_test_data[i].alternate.value,
		    test_array, TEST_POINTS * sizeof(uint32_t)), 0);
		lcg32_destroy(&lcg32);

		/* Test the custom 32-bit LCG now */
		lcg32 = lcg32_init_custom(
		    prng_lcg32_test_data[i].custom.additive,
		    prng_lcg32_test_data[i].custom.multiplier,
		    prng_lcg32_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = lcg32_rand(lcg32);
		CU_ASSERT_EQUAL(memcmp(prng_lcg32_test_data[i].custom.value,
		    test_array, TEST_POINTS * sizeof(uint32_t)), 0);
		lcg32_destroy(&lcg32);
	}

	return;
}

/**
 * \brief
 */
void
test_mt32(void) {
	mt32_h mt32;
	uint32_t test_seed[5] = {
		UINT32_C(0x57ECf1AC), UINT32_C(0xF930F1CF),
		UINT32_C(0xD1B26F78), UINT32_C(0x512472E1),
		UINT32_C(0xEF5FF367)
	};
	uint32_t test_array[PRNG_MT32_TEST_POINTS];
	int i, j;

	/* Test the initializer and destructor functions */
	CU_ASSERT_PTR_NULL(mt32 = mt32_init_seed(MT__ERROR, 1701));
	CU_ASSERT_PTR_NULL(mt32 = mt32_init_seed(420, 1701));
	CU_ASSERT_PTR_NULL(mt32 = mt32_init_array(MT__ERROR, test_seed, 1));
	CU_ASSERT_PTR_NULL(mt32 = mt32_init_array(69, test_seed, 1));
	CU_ASSERT_PTR_NULL(mt32 = mt32_init_array(MT_MT19937, NULL, 1));
	CU_ASSERT_PTR_NULL(mt32 = mt32_init_array(MT_MT19937, test_seed, 0));
	CU_ASSERT_EQUAL(mt32_destroy(&mt32), 1);
	CU_ASSERT_EQUAL(mt32_destroy(NULL), 1);
	CU_ASSERT_PTR_NOT_NULL(mt32 = mt32_init_seed(MT_MT19937, 1701));
	CU_ASSERT_EQUAL(mt32_destroy(&mt32), 0);
	CU_ASSERT_PTR_NOT_NULL(mt32 = mt32_init_seed(MT_MEMT19937_1, 1701));
	CU_ASSERT_EQUAL(mt32_destroy(&mt32), 0);
	CU_ASSERT_PTR_NOT_NULL(mt32 = mt32_init_seed(MT_MEMT19937_2, 1701));
	CU_ASSERT_EQUAL(mt32_destroy(&mt32), 0);
	CU_ASSERT_PTR_NOT_NULL(mt32 = mt32_init_array(MT_MT19937, test_seed,
	    5));
	CU_ASSERT_EQUAL(mt32_destroy(&mt32), 0);
	CU_ASSERT_PTR_NOT_NULL(mt32 = mt32_init_array(MT_MEMT19937_1, test_seed,
	    5));
	CU_ASSERT_EQUAL(mt32_destroy(&mt32), 0);
	CU_ASSERT_PTR_NOT_NULL(mt32 = mt32_init_array(MT_MEMT19937_2, test_seed,
	    5));
	CU_ASSERT_EQUAL(mt32_destroy(&mt32), 0);
	CU_ASSERT_EQUAL(mt32_rand(NULL), 0);
	CU_ASSERT_EQUAL(mt32_rand(mt32), 0);

	/* Iterate through the test sets for single word seeds */
	for (i = 0; i < TEST_SETS; ++i) {
		/* Test the "basic" Mersenne Twister */
		mt32 = mt32_init_seed(MT_MT19937,
		    prng_mt32_test_data_seed[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < PRNG_MT32_TEST_POINTS; ++j)
			test_array[j] = mt32_rand(mt32);
		CU_ASSERT_EQUAL(memcmp(test_array,
		    prng_mt32_test_data_seed[i].value.mt19937,
		    PRNG_MT32_TEST_POINTS * sizeof(uint32_t)), 0);
		mt32_destroy(&mt32);

		/* Test the v1 Maximally Equidistributed Mersenne Twister */
		mt32 = mt32_init_seed(MT_MEMT19937_1,
		    prng_mt32_test_data_seed[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < PRNG_MT32_TEST_POINTS; ++j)
			test_array[j] = mt32_rand(mt32);
		CU_ASSERT_EQUAL(memcmp(test_array,
		    prng_mt32_test_data_seed[i].value.memt19937_1,
		    PRNG_MT32_TEST_POINTS * sizeof(uint32_t)), 0);
		mt32_destroy(&mt32);

		/* Test the v2 Maximally Equidistributed Mersenne Twister */
		mt32 = mt32_init_seed(MT_MEMT19937_2,
			prng_mt32_test_data_seed[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < PRNG_MT32_TEST_POINTS; ++j)
			test_array[j] = mt32_rand(mt32);
		CU_ASSERT_EQUAL(memcmp(test_array,
		    prng_mt32_test_data_seed[i].value.memt19937_2,
		    PRNG_MT32_TEST_POINTS * sizeof(uint32_t)), 0);
		mt32_destroy(&mt32);
	}

	/* Iterate through the test sets for array seeds */
	for (i = 0; i < TEST_SETS; ++i) {
		/* Test the "basic" Mersenne Twister */
		mt32 = mt32_init_array(MT_MT19937,
		    prng_mt32_test_data_array[i].seed.data,
		    prng_mt32_test_data_array[i].seed.length);

		/* Iterate through the data points */
		for (j = 0; j < PRNG_MT32_TEST_POINTS; ++j)
			test_array[j] = mt32_rand(mt32);
		CU_ASSERT_EQUAL(memcmp(test_array,
		    prng_mt32_test_data_array[i].value.mt19937,
		    PRNG_MT32_TEST_POINTS * sizeof(uint32_t)), 0);
		mt32_destroy(&mt32);

		/* Test the v1 Maximally Equidistributed Mersenne Twister */
		mt32 = mt32_init_array(MT_MEMT19937_1,
		    prng_mt32_test_data_array[i].seed.data,
		    prng_mt32_test_data_array[i].seed.length);

		/* Iterate through the data points */
		for (j = 0; j < PRNG_MT32_TEST_POINTS; ++j)
			test_array[j] = mt32_rand(mt32);
		CU_ASSERT_EQUAL(memcmp(test_array,
		    prng_mt32_test_data_array[i].value.memt19937_1,
		    PRNG_MT32_TEST_POINTS * sizeof(uint32_t)), 0);
		mt32_destroy(&mt32);

		/* Test the v2 Maximally Equidistributed Mersenne Twister */
		mt32 = mt32_init_array(MT_MEMT19937_2,
			prng_mt32_test_data_array[i].seed.data,
			prng_mt32_test_data_array[i].seed.length);

		/* Iterate through the data points */
		for (j = 0; j < PRNG_MT32_TEST_POINTS; ++j)
			test_array[j] = mt32_rand(mt32);
		CU_ASSERT_EQUAL(memcmp(test_array,
		    prng_mt32_test_data_array[i].value.memt19937_2,
		    PRNG_MT32_TEST_POINTS * sizeof(uint32_t)), 0);
		mt32_destroy(&mt32);
	}

	/* Perform the "oversize" array seed tests */
	mt32 = mt32_init_array(MT_MT19937,
	    prng_mt32_test_data_oversize.seed.data,
	    prng_mt32_test_data_oversize.seed.length);
	for (i = 0; i < PRNG_MT32_TEST_POINTS; ++i)
		test_array[i] = mt32_rand(mt32);
	CU_ASSERT_EQUAL(memcmp(prng_mt32_test_data_oversize.value.mt19937,
	    test_array, PRNG_MT32_TEST_POINTS * sizeof(uint32_t)), 0);
	mt32_destroy(&mt32);

	mt32 = mt32_init_array(MT_MEMT19937_1,
	    prng_mt32_test_data_oversize.seed.data,
	    prng_mt32_test_data_oversize.seed.length);
	for (i = 0; i < PRNG_MT32_TEST_POINTS; ++i)
		test_array[i] = mt32_rand(mt32);
	CU_ASSERT_EQUAL(memcmp(prng_mt32_test_data_oversize.value.memt19937_1,
	    test_array, PRNG_MT32_TEST_POINTS * sizeof(uint32_t)), 0);
	mt32_destroy(&mt32);

	mt32 = mt32_init_array(MT_MEMT19937_2,
	    prng_mt32_test_data_oversize.seed.data,
	    prng_mt32_test_data_oversize.seed.length);
	for (i = 0; i < PRNG_MT32_TEST_POINTS; ++i)
		test_array[i] = mt32_rand(mt32);
	CU_ASSERT_EQUAL(memcmp(prng_mt32_test_data_oversize.value.memt19937_2,
	    test_array, PRNG_MT32_TEST_POINTS * sizeof(uint32_t)), 0);
	mt32_destroy(&mt32);

	return;
}

/**
 * \brief
 */
void
test_well32(void) {
	well32_h well32;
	uint32_t x[TEST_POINTS];
	enum WELL32_TYPE j;
	int i, k, l;

	for (i = 0; i < TEST_SETS; ++i) {
		for (j = WELL32_512A; j <= WELL32_44497E; ++j) {
			well32 = well32_init(j, prng_well32_test_data[i].seed);

			memset(x, 0, TEST_POINTS * sizeof(uint32_t));
			if ((j != WELL32_800C) && (j != WELL32_800D) &&
			    (j != WELL32_19937D) && (j != WELL32_21701C) &&
			    (j != WELL32_23209C) && (j != WELL32_23209D)) {
				for (k = 0; k < TEST_POINTS; ++k) {
					if (k == (TEST_POINTS / 2))
						for (l = 0; l < WELL_SKIP; ++l)
							well32_rand(well32);

					x[k] = well32_rand(well32);
				}
			}
			well32_destroy(&well32);

			switch (j) {
			case WELL32_512A:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well512a,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_521A:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well521a,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_521B:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well521b,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_607A:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well607a,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_607B:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well607b,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_800A:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well800a,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_800B:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well800b,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_800E:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well800e,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_800F:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well800f,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_1024A:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well1024a,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_1024B:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well1024b,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_19937A:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well19937a,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_19937B:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well19937b,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_19937C:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well19937c,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_19937E:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well19937e,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_19937F:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well19937f,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_21701A:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well21701a,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_21701E:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well21701e,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_23209A:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well23209a,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_23209B:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well23209b,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_23209E:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well23209e,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_23209F:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well23209f,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_44497A:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well44497a,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_44497C:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well44497c,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_44497E:
				CU_ASSERT_EQUAL(memcmp(x,
				    prng_well32_test_data[i].values.well44497e,
				    TEST_POINTS * sizeof(uint32_t)), 0);
				break;
			case WELL32_800C:
			case WELL32_800D:
			case WELL32_19937D:
			case WELL32_21701C:
			case WELL32_23209C:
			case WELL32_23209D:
				/* These tests don't do anythig. */
				break;
			default:
				/* This cannot happen. */
				CU_FAIL("invalid test number");
				break;
			}
		}
	}

	return;
}

/**
 * \brief
 */
void
test_lcg64(void) {
	lcg64_h lcg64;
	uint64_t test_array[TEST_POINTS];
	int i, j;

	/* Test the initializer and destructor functions */
	CU_ASSERT_PTR_NULL(lcg64 = lcg64_init(LCG__ERROR, 0));
	CU_ASSERT_PTR_NULL(lcg64 = lcg64_init(42, 0));
	CU_ASSERT_PTR_NULL(lcg64 = lcg64_init_custom(0, 42, 0));
	CU_ASSERT_PTR_NULL(lcg64 = lcg64_init_custom(42, 0, 0));
	CU_ASSERT_EQUAL(lcg64_destroy(&lcg64), 1);
	CU_ASSERT_EQUAL(lcg64_destroy(NULL), 1);
	CU_ASSERT_PTR_NOT_NULL(lcg64 = lcg64_init(LCG_PRIMARY, 0));
	CU_ASSERT_EQUAL(lcg64_destroy(&lcg64), 0);
	CU_ASSERT_PTR_NOT_NULL(lcg64 = lcg64_init(LCG_ALTERNATE, 0));
	CU_ASSERT_EQUAL(lcg64_destroy(&lcg64), 0);

	/* Iterate through the test sets */
	for (i = 0; i < TEST_SETS; ++i) {
		/* Test the LCG_PRIMARY algoritm first */
		lcg64 = lcg64_init(LCG_PRIMARY, prng_lcg64_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = lcg64_rand(lcg64);
		CU_ASSERT_EQUAL(memcmp(prng_lcg64_test_data[i].primary.value,
		    test_array, TEST_POINTS * sizeof(uint64_t)), 0);
		lcg64_destroy(&lcg64);

		/* Test the LCG_ALTERNATE algoritm first */
		lcg64 = lcg64_init(LCG_ALTERNATE, prng_lcg64_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = lcg64_rand(lcg64);
		CU_ASSERT_EQUAL(memcmp(prng_lcg64_test_data[i].alternate.value,
		    test_array, TEST_POINTS * sizeof(uint64_t)), 0);
		lcg64_destroy(&lcg64);

		/* Test the custom 64-bit LCG now */
		lcg64 = lcg64_init_custom(
		    prng_lcg64_test_data[i].custom.additive,
		    prng_lcg64_test_data[i].custom.multiplier,
		    prng_lcg64_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = lcg64_rand(lcg64);
		CU_ASSERT_EQUAL(memcmp(prng_lcg64_test_data[i].custom.value,
		    test_array, TEST_POINTS * sizeof(uint64_t)), 0);
		lcg64_destroy(&lcg64);
	}

	return;
}

/**
 * \brief
 */
void
test_sm64(void) {
	sm64_h sm64, split;
	uint64_t test_array[TEST_POINTS];
	int i, j, k;

	/* Test the initializer and destructor functions */
	CU_ASSERT_PTR_NULL(sm64 = sm64_init_custom(0, 0));
	CU_ASSERT_PTR_NULL(sm64 = sm64_init_custom(2, 0));
	CU_ASSERT_PTR_NULL(split = sm64_split(sm64));
	CU_ASSERT_EQUAL(sm64_destroy(&split), 1);
	CU_ASSERT_EQUAL(sm64_destroy(&sm64), 1);
	CU_ASSERT_EQUAL(sm64_destroy(NULL), 1);
	CU_ASSERT_PTR_NOT_NULL(sm64 = sm64_init(0));
	CU_ASSERT_EQUAL(sm64_destroy(&sm64), 0);
	CU_ASSERT_PTR_NOT_NULL(sm64 = sm64_init_custom(1701, 0));
	CU_ASSERT_PTR_NOT_NULL(split = sm64_split(sm64));
	CU_ASSERT_EQUAL(sm64_destroy(&split), 0);
	sm64_destroy(&sm64);

	/* Iterate through the test sets */
	for (i = 0; i < TEST_SETS; ++i) {
		/* Test the basic/"golden gamma" version first */
		sm64 = sm64_init(prng_sm64_test_data[i].seed);

		/* Iterate through the data points */
		for (j = 0; j < TEST_POINTS; ++j)
			test_array[j] = sm64_rand(sm64);
		CU_ASSERT_EQUAL(memcmp(prng_sm64_test_data[i].base.value,
		    test_array, TEST_POINTS * sizeof(uint64_t)), 0);
		sm64_destroy(&sm64);

		/* Test the custom-gamma value version now */
		for (j = 0; j < TEST_SETS; ++j) {
			/* Initialize the custom-gamma generator */
			sm64 = sm64_init_custom(
			    prng_sm64_test_data[i].custom[j].gamma,
			    prng_sm64_test_data[i].seed);
			
			/* Iterate through the data points */
			for (k = 0; k < TEST_POINTS; ++k)
				test_array[k] = sm64_rand(sm64);
			CU_ASSERT_EQUAL(memcmp(test_array,
			    prng_sm64_test_data[i].custom[j].value,
			    TEST_POINTS * sizeof(uint64_t)), 0);
			sm64_destroy(&sm64);
		}

		/* Test the splitting feature now */
		/* Initialize the "source" generator, using the basic/"golden
		 * gamma" generator.
		 */
		sm64 = sm64_init(prng_sm64_test_data[i].seed);
		for (j = 0; j < TEST_SETS; ++j) {
			/* Split the generator */
			split = sm64_split(sm64);

			/* Iterate through the data points */
			for (k = 0; k < TEST_POINTS; ++k)
				test_array[k] = sm64_rand(split);
			CU_ASSERT_EQUAL(memcmp(test_array,
			    prng_sm64_test_data[i].split[j].value,
			    TEST_POINTS * sizeof(uint64_t)), 0);
			sm64_destroy(&split);
		}
		sm64_destroy(&sm64);
	}
//-+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8

	return;
}
