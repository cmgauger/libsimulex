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
#include <stdlib.h>
#include <string.h>
#include <CUnit/CUnit.h>

#include <simulex/bit_hash.h>
#include "mdx_test.h"

#define	MDX_TESTS	7
#define	MD2_LENGTH	16
#define	MD4_LENGTH	16
#define	MD5_LENGTH	16

static const struct {
	const char	*message;
	struct {
		uint8_t	 md2[MD2_LENGTH];
		uint8_t	 md4[MD4_LENGTH];
		uint8_t	 md5[MD5_LENGTH];
	} digest;
} mdx_test_data[MDX_TESTS] = {
	{"",
	 {{0x83, 0x50, 0xE5, 0xA3, 0xE2, 0x4C, 0x15, 0x3D,
	   0xF2, 0x27, 0x5C, 0x9F, 0x80, 0x69, 0x27, 0x73},
	  {0x31, 0xD6, 0xCF, 0xE0, 0xD1, 0x6A, 0xE9, 0x31,
	   0xB7, 0x3C, 0x59, 0xD7, 0xE0, 0xC0, 0x89, 0xC0},
	  {0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
	   0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E}}},
	{"a",
	 {{0x32, 0xEC, 0x01, 0xEC, 0x4A, 0x6D, 0xAC, 0x72,
	   0xC0, 0xAB, 0x96, 0xFB, 0x34, 0xC0, 0xB5, 0xD1},
	  {0xBD, 0xE5, 0x2C, 0xB3, 0x1D, 0xE3, 0x3E, 0x46,
	   0x24, 0x5E, 0x05, 0xFB, 0xDB, 0xD6, 0xFB, 0x24},
	  {0x0C, 0xC1, 0x75, 0xB9, 0xC0, 0xF1, 0xB6, 0xA8,
	   0x31, 0xC3, 0x99, 0xE2, 0x69, 0x77, 0x26, 0x61}}},
	{"abc",
	 {{0xDA, 0x85, 0x3B, 0x0D, 0x3F, 0x88, 0xD9, 0x9B,
	   0x30, 0x28, 0x3A, 0x69, 0xE6, 0xDE, 0xD6, 0xBB},
	  {0xA4, 0x48, 0x01, 0x7A, 0xAF, 0x21, 0xD8, 0x52,
	   0x5F, 0xC1, 0x0A, 0xE8, 0x7A, 0xA6, 0x72, 0x9D},
	  {0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
	   0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72}}},
	{"message digest",
	 {{0xAB, 0x4F, 0x49, 0x6B, 0xFB, 0x2A, 0x53, 0x0B,
	   0x21, 0x9F, 0xF3, 0x30, 0x31, 0xFE, 0x06, 0xB0},
	  {0xD9, 0x13, 0x0A, 0x81, 0x64, 0x54, 0x9F, 0xE8,
	   0x18, 0x87, 0x48, 0x06, 0xE1, 0xC7, 0x01, 0x4B},
	  {0xF9, 0x6B, 0x69, 0x7D, 0x7C, 0xB7, 0x93, 0x8D,
	   0x52, 0x5A, 0x2F, 0x31, 0xAA, 0xF1, 0x61, 0xD0}}},
	{"abcdefghijklmnopqrstuvwxyz",
	 {{0x4E, 0x8D, 0xDF, 0xF3, 0x65, 0x02, 0x92, 0xAB,
	   0x5A, 0x41, 0x08, 0xC3, 0xAA, 0x47, 0x94, 0x0B},
	  {0xD7, 0x9E, 0x1C, 0x30, 0x8A, 0xA5, 0xBB, 0xCD,
	   0xEE, 0xA8, 0xED, 0x63, 0xDF, 0x41, 0x2D, 0xA9},
	  {0xC3, 0xFC, 0xD3, 0xD7, 0x61, 0x92, 0xE4, 0x00,
	   0x7D, 0xFB, 0x49, 0x6C, 0xCA, 0x67, 0xE1, 0x3B}}},
	{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
	 {{0xDA, 0x33, 0xDE, 0xF2, 0xA4, 0x2D, 0xF1, 0x39,
	   0x75, 0x35, 0x28, 0x46, 0xC3, 0x03, 0x38, 0xCD},
	  {0x04, 0x3F, 0x85, 0x82, 0xF2, 0x41, 0xDB, 0x35,
	   0x1C, 0xE6, 0x27, 0xE1, 0x53, 0xE7, 0xF0, 0xE4},
	  {0xD1, 0x74, 0xAB, 0x98, 0xD2, 0x77, 0xD9, 0xF5,
	   0xA5, 0x61, 0x1C, 0x2C, 0x9F, 0x41, 0x9D, 0x9F}}},
	{"123456789012345678901234567890123456789012345678901234567890123456789"
	    "01234567890",
	 {{0xD5, 0x97, 0x6F, 0x79, 0xD8, 0x3D, 0x3A, 0x0D,
	   0xC9, 0x80, 0x6C, 0x3C, 0x66, 0xF3, 0xEF, 0xD8},
	  {0xE3, 0x3B, 0x4D, 0xDC, 0x9C, 0x38, 0xF2, 0x19,
	   0x9C, 0x3E, 0x7B, 0x16, 0x4F, 0xCC, 0x05, 0x36},
	  {0x57, 0xED, 0xF4, 0xA2, 0x2B, 0xE3, 0xC9, 0x55,
	   0xAC, 0x49, 0xDA, 0x2E, 0x21, 0x07, 0xB6, 0x7A}}}
};

int
init_mdx_suite(void) {
	return 0;
}

int
clean_mdx_suite(void) {
	return 0;
}

void
test_mdx_init_size_destroy(void) {
	mdx_h mdx;

	/* Test the initializer and digest size function with MDX__ERROR */
	CU_ASSERT_PTR_NULL(mdx = mdx_init(MDX__ERROR));
	CU_ASSERT_EQUAL(mdx_digest_size(mdx), 0);

	/* Test the initializer and digest size function with an invalid type */
	CU_ASSERT_PTR_NULL(mdx = mdx_init(42));
	CU_ASSERT_EQUAL(mdx_digest_size(mdx), 0);

	/* Test the destructor with a NULL pointer, and an already NULL state */
	CU_ASSERT_EQUAL(mdx_destroy(&mdx), 1);
	CU_ASSERT_EQUAL(mdx_destroy(NULL), 1);

	/* Test the initializer, sizing function, and destructor with MDX_MD2 */
	CU_ASSERT_PTR_NOT_NULL(mdx = mdx_init(MDX_MD2));
	CU_ASSERT_EQUAL(mdx_digest_size(mdx), MD2_LENGTH);
	CU_ASSERT_EQUAL(mdx_destroy(&mdx), 0);
	CU_ASSERT_PTR_NULL(mdx);

	/* Test the initializer, sizing function, and destructor with MDX_MD4 */
	CU_ASSERT_PTR_NOT_NULL(mdx = mdx_init(MDX_MD4));
	CU_ASSERT_EQUAL(mdx_digest_size(mdx), MD4_LENGTH);
	CU_ASSERT_EQUAL(mdx_destroy(&mdx), 0);
	CU_ASSERT_PTR_NULL(mdx);

	/* Test the initializer, sizing function, and destructor with MDX_MD5 */
	CU_ASSERT_PTR_NOT_NULL(mdx = mdx_init(MDX_MD5));
	CU_ASSERT_EQUAL(mdx_digest_size(mdx), MD5_LENGTH);
	CU_ASSERT_EQUAL(mdx_destroy(&mdx), 0);
	CU_ASSERT_PTR_NULL(mdx);

	return;
}

void
test_mdx_md2(void) {
	uint8_t md[MD2_LENGTH];
	mdx_h mdx;
	int i;

	/* Initialize the context/state */
	mdx = mdx_init(MDX_MD2);

	/* Iterate through the test vectors */
	for (i = 0; i < MDX_TESTS; ++i) {
		/* Clear the digest */
		memset(md, 0, mdx_digest_size(mdx) * sizeof(uint8_t));

		/* Initialize the MD2 digest state */
		mdx_start(mdx);

		/* Hash the data */
		mdx_update(mdx, mdx_test_data[i].message,
		    strlen(mdx_test_data[i].message));

		/* Get the digest */
		mdx_final(mdx, md, mdx_digest_size(mdx));

		/* Compare the received digest against the expected digest */
		CU_ASSERT_EQUAL(memcmp(md, mdx_test_data[i].digest.md2,
		    mdx_digest_size(mdx) * sizeof(uint8_t)), 0);
	}

	/* Destroy the state/context */
	mdx_destroy(&mdx);

	return;
}

void
test_mdx_md4(void) {
	uint8_t md[MD4_LENGTH];
	mdx_h mdx;
	int i;

	/* Initialize the context/state */
	mdx = mdx_init(MDX_MD4);

	/* Iterate through the test vectors */
	for (i = 0; i < MDX_TESTS; ++i) {
		/* Clear the digest */
		memset(md, 0, mdx_digest_size(mdx) * sizeof(uint8_t));

		/* Initialize the MD2 digest state */
		mdx_start(mdx);

		/* Hash the data */
		mdx_update(mdx, mdx_test_data[i].message,
		    strlen(mdx_test_data[i].message));

		/* Get the digest */
		mdx_final(mdx, md, mdx_digest_size(mdx));

		/* Compare the received digest against the expected digest */
		CU_ASSERT_EQUAL(memcmp(md, mdx_test_data[i].digest.md4,
		    mdx_digest_size(mdx) * sizeof(uint8_t)), 0);
	}

	/* Destroy the state/context */
	mdx_destroy(&mdx);

	return;
}

void
test_mdx_md5(void) {
	uint8_t md[MD5_LENGTH];
	mdx_h mdx;
	int i;

	/* Initialize the context/state */
	mdx = mdx_init(MDX_MD5);

	/* Iterate through the test vectors */
	for (i = 0; i < MDX_TESTS; ++i) {
		/* Clear the digest */
		memset(md, 0, mdx_digest_size(mdx) * sizeof(uint8_t));

		/* Initialize the MD2 digest state */
		mdx_start(mdx);

		/* Hash the data */
		mdx_update(mdx, mdx_test_data[i].message,
		    strlen(mdx_test_data[i].message));

		/* Get the digest */
		mdx_final(mdx, md, mdx_digest_size(mdx));

		/* Compare the received digest against the expected digest */
		CU_ASSERT_EQUAL(memcmp(md, mdx_test_data[i].digest.md5,
		    mdx_digest_size(mdx) * sizeof(uint8_t)), 0);
	}

	/* Destroy the state/context */
	mdx_destroy(&mdx);

	return;
}
