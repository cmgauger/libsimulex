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
 * \file	bit_hash_test.c
 * \copyright	MIT
 * \date	2025
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/CUnit.h>

#include <simulex/bit_hash.h>
#include "bit_hash_test.h"

#include "test_data/bit_hash_test_fnv_data.h"
#include "test_data/bit_hash_test_murmur_data.h"
#include "test_data/bit_hash_test_purdy_data.h"

/**
 * \brief
 */
int
init_bit_hash_suite(void) {
	return 0;
}

/**
 * \brief
 */
int
clean_bit_hash_suite(void) {
	return 0;
}

/**
 * \brief
 */
void
test_fnv32(void) {
	fnv32_h fnv32_b, fnv32a_b, fnv32_s, fnv32a_s, fnv32_x;
	int i;

	/* Test the initializer with invalid values */
	CU_ASSERT_PTR_NULL(fnv32_x = fnv32_init(FNV__ERROR));
	CU_ASSERT_PTR_NULL(fnv32_x = fnv32_init(42));

	/* Test the custom-basis initializer with invalid values */
	CU_ASSERT_PTR_NULL(fnv32_x = fnv32_init_alt(FNV__ERROR,
	    UINT32_C(1701)));
	CU_ASSERT_PTR_NULL(fnv32_x = fnv32_init_alt(42, UINT32_C(1701)));

	/* Test the destructor with a NULL pointer and an already NULL sate */
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 1);
	CU_ASSERT_EQUAL(fnv32_destroy(NULL), 1);

	/* Test initialization/destruction of regular FNV-1 */
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init(FNV_1));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);

	/* Test initialization/destruction of regular FNV-1a */
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init(FNV_1A));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);

	/* Test initialization/destruction of custom basis FNV-1 */
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init_alt(FNV_1,
	    UINT32_C(0x00000000)));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init_alt(FNV_1,
	    UINT32_C(0x000FF1CE)));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init_alt(FNV_1,
	    UINT32_C(0xDEADBEEF)));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init_alt(FNV_1,
	    UINT32_C(0xFFFFFFFF)));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);

	/* Test initialization/destruction of custom basis FNV-1a */
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init_alt(FNV_1A,
	    UINT32_C(0x00000000)));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init_alt(FNV_1A,
	    UINT32_C(0x000FF1CE)));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init_alt(FNV_1A,
	    UINT32_C(0xDEADBEEF)));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);
	CU_ASSERT_PTR_NOT_NULL(fnv32_x = fnv32_init_alt(FNV_1A,
	    UINT32_C(0xFFFFFFFF)));
	CU_ASSERT_EQUAL(fnv32_destroy(&fnv32_x), 0);
	CU_ASSERT_PTR_NULL(fnv32_x);

	/* Run through the test cases */
	for (i = 0; i < FNV_TESTS; ++i) {
		/* Initializer tests */
		fnv32_b = fnv32_init(FNV_1);
		fnv32a_b = fnv32_init(FNV_1A);
		if (fnv_test_data[i].have_string) {
			fnv32_s = fnv32_init(FNV_1);
			fnv32a_s = fnv32_init(FNV_1A);
		}

		/* Buffer hashing tests */
		CU_ASSERT_EQUAL(fnv32_buf(fnv_test_data[i].datum.data,
		    fnv_test_data[i].datum.len_buffer, fnv32_b),
		    fnv_test_data[i].result.fnv32.buffer);
		CU_ASSERT_EQUAL(fnv32_buf(fnv_test_data[i].datum.data,
		    fnv_test_data[i].datum.len_buffer, fnv32a_b),
		    fnv_test_data[i].result.fnv32a.buffer);
		if (fnv_test_data[i].have_string) {
			CU_ASSERT_EQUAL(fnv32_buf(fnv_test_data[i].datum.data,
			    fnv_test_data[i].datum.len_string, fnv32_s),
			    fnv_test_data[i].result.fnv32.string);
			CU_ASSERT_EQUAL(fnv32_buf(fnv_test_data[i].datum.data,
			    fnv_test_data[i].datum.len_string, fnv32a_s),
			    fnv_test_data[i].result.fnv32a.string);
		}

		/* Buffer result tests */
		CU_ASSERT_EQUAL(fnv32_value(fnv32_b),
		    fnv_test_data[i].result.fnv32.buffer);
		CU_ASSERT_EQUAL(fnv32_value(fnv32a_b),
		    fnv_test_data[i].result.fnv32a.buffer);
		if (fnv_test_data[i].have_string) {
			CU_ASSERT_EQUAL(fnv32_value(fnv32_s),
			    fnv_test_data[i].result.fnv32.string);
			CU_ASSERT_EQUAL(fnv32_value(fnv32a_s),
			    fnv_test_data[i].result.fnv32a.string);
		}

		/* Destructor tests */
		fnv32_destroy(&fnv32_b);
		fnv32_destroy(&fnv32a_b);
		if (fnv_test_data[i].have_string) {
			fnv32_destroy(&fnv32_s);
			fnv32_destroy(&fnv32a_s);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_fnv64(void) {
	fnv64_h fnv64_b, fnv64a_b, fnv64_s, fnv64a_s, fnv64_x;
	int i;

	/* Test the initializer with invalid values */
	CU_ASSERT_PTR_NULL(fnv64_x = fnv64_init(FNV__ERROR));
	CU_ASSERT_PTR_NULL(fnv64_x = fnv64_init(42));

	/* Test the custom-basis initializer with invalid values */
	CU_ASSERT_PTR_NULL(fnv64_x = fnv64_init_alt(FNV__ERROR,
	    UINT32_C(1701)));
	CU_ASSERT_PTR_NULL(fnv64_x = fnv64_init_alt(42, UINT32_C(1701)));

	/* Test the destructor with a NULL pointer and an already NULL sate */
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 1);
	CU_ASSERT_EQUAL(fnv64_destroy(NULL), 1);

	/* Test initialization/destruction of regular FNV-1 */
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init(FNV_1));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);

	/* Test initialization/destruction of regular FNV-1a */
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init(FNV_1A));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);

	/* Test initialization/destruction of custom basis FNV-1 */
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init_alt(FNV_1,
	    UINT64_C(0x0000000000000000)));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init_alt(FNV_1,
	    UINT64_C(0x000FF1CECAFEBABE)));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init_alt(FNV_1,
	    UINT64_C(0xDEADBEEFDEADBEEF)));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init_alt(FNV_1,
	    UINT64_C(0xFFFFFFFFFFFFFFFF)));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);

	/* Test initialization/destruction of custom basis FNV-1a */
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init_alt(FNV_1A,
	    UINT64_C(0x0000000000000000)));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init_alt(FNV_1A,
	    UINT64_C(0x000FF1CECAFEBABE)));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init_alt(FNV_1A,
	    UINT64_C(0xDEADBEEFDEADBEEF)));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);
	CU_ASSERT_PTR_NOT_NULL(fnv64_x = fnv64_init_alt(FNV_1A,
	    UINT64_C(0xFFFFFFFFFFFFFFFF)));
	CU_ASSERT_EQUAL(fnv64_destroy(&fnv64_x), 0);
	CU_ASSERT_PTR_NULL(fnv64_x);

	/* Run through the test cases */
	for (i = 0; i < FNV_TESTS; ++i) {
		/* Initializer tests */
		fnv64_b = fnv64_init(FNV_1);
		fnv64a_b = fnv64_init(FNV_1A);
		if (fnv_test_data[i].have_string) {
			fnv64_s = fnv64_init(FNV_1);
			fnv64a_s = fnv64_init(FNV_1A);
		}

		/* Buffer hashing tests */
		CU_ASSERT_EQUAL(fnv64_buf(fnv_test_data[i].datum.data,
		    fnv_test_data[i].datum.len_buffer, fnv64_b),
		    fnv_test_data[i].result.fnv64.buffer);
		CU_ASSERT_EQUAL(fnv64_buf(fnv_test_data[i].datum.data,
		    fnv_test_data[i].datum.len_buffer, fnv64a_b),
		    fnv_test_data[i].result.fnv64a.buffer);
		if (fnv_test_data[i].have_string) {
			CU_ASSERT_EQUAL(fnv64_buf(fnv_test_data[i].datum.data,
			    fnv_test_data[i].datum.len_string, fnv64_s),
			    fnv_test_data[i].result.fnv64.string);
			CU_ASSERT_EQUAL(fnv64_buf(fnv_test_data[i].datum.data,
			    fnv_test_data[i].datum.len_string, fnv64a_s),
			    fnv_test_data[i].result.fnv64a.string);
		}

		/* Buffer result tests */
		CU_ASSERT_EQUAL(fnv64_value(fnv64_b),
		    fnv_test_data[i].result.fnv64.buffer);
		CU_ASSERT_EQUAL(fnv64_value(fnv64a_b),
		    fnv_test_data[i].result.fnv64a.buffer);
		if (fnv_test_data[i].have_string) {
			CU_ASSERT_EQUAL(fnv64_value(fnv64_s),
			    fnv_test_data[i].result.fnv64.string);
			CU_ASSERT_EQUAL(fnv64_value(fnv64a_s),
			    fnv_test_data[i].result.fnv64a.string);
		}

		/* Destructor tests */
		fnv64_destroy(&fnv64_b);
		fnv64_destroy(&fnv64a_b);
		if (fnv_test_data[i].have_string) {
			fnv64_destroy(&fnv64_s);
			fnv64_destroy(&fnv64a_s);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_murmur1(void) {
	int i;

	/* Run through the test cases */
	for (i = 0; i < MURMUR_TESTS; ++i) {
		CU_ASSERT_EQUAL(murmur1(murmur_test_data[i].vector,
		    murmur_test_data[i].length, murmur_test_data[i].seed),
		    murmur_test_data[i].murmurhash1);
	}

	return;
}

/**
 * \brief
 */
void
test_murmur2(void) {
	int i;

	/* Run through the test cases */
	for (i = 0; i < MURMUR_TESTS; ++i) {
		CU_ASSERT_EQUAL(murmur2(murmur_test_data[i].vector,
		    murmur_test_data[i].length, murmur_test_data[i].seed,
		    MURMUR2_2), murmur_test_data[i].murmurhash2);
		CU_ASSERT_EQUAL(murmur2(murmur_test_data[i].vector,
		    murmur_test_data[i].length, murmur_test_data[i].seed,
		    MURMUR2_2A), murmur_test_data[i].murmurhash2a);
	}

	return;
}

/**
 * \brief
 */
void
test_murmur3(void) {
	int i;

	/* Run through the test cases */
	for (i = 0; i < MURMUR_TESTS; ++i) {
		CU_ASSERT_EQUAL(murmur3(murmur_test_data[i].vector,
		    murmur_test_data[i].length, murmur_test_data[i].seed),
		    murmur_test_data[i].murmurhash3);
	}

	return;
}

/**
 * \brief
 */
void
test_purdy(void) {
	uint64_t x;
	int i, j;

	x = 0;

	/* Test the error checking */
	CU_ASSERT_EQUAL(purdy(0, PURDY_UAF, NULL), 1);
	CU_ASSERT_EQUAL(purdy(0, PURDY__ERROR, &x), 1);
	CU_ASSERT_EQUAL(purdy(0, 42, &x), 1);

	/* Iterate through the test cases */
	for (i = 0; i < PURDY_TESTS; ++i) {
		for (j = 0; j < 11; ++j) {
			CU_ASSERT_EQUAL(purdy(purdy_test_data[i].vector, j + 1,
			    &x), 0);
			CU_ASSERT_EQUAL(x, purdy_test_data[i].result[j]);
		}
	}

	return;
}
