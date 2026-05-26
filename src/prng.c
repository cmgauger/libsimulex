/*
 * Copyright (c) 2024 Christian Gauger-Cosgrove
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
 * \file	prng.c
 * \copyright	MIT
 * \date	2024
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/bit_hash.h>
#include <simulex/prng.h>

/**
 * 64-bit Bays-Durham shuffle state
 */
struct bd64_s {
	void *s;
	uint64_t (*r)(void *);
	uint64_t *t;
	size_t p;
	size_t l;
};

/**
 * 64-bit Maclaren-Marsaglia algorithm state
 */
struct mm64_s {
	void *is;
	void *vs;
	uint64_t (*ir)(void *);
	uint64_t (*vr)(void *);
	uint64_t *t;
	size_t l;
};

/**
 * 32-bit Bays-Durham shuffle state
 */
struct bd32_s {
	void *s;
	uint32_t (*r)(void *);
	uint32_t *t;
	size_t p;
	size_t l;
};

/**
 * 32-bit Maclaren-Marsaglia algorithm state
 */
struct mm32_s {
	void *is;
	void *vs;
	uint32_t (*ir)(void *);
	uint32_t (*vr)(void *);
	uint32_t *t;
	size_t l;
};

/* ========================================================================== */

bd64_h
/* https://peteroupc.github.io/bdshuffle.pdf */
bd64_init(size_t table_size, uint64_t (*rng)(void *), void *rng_state) {
	struct bd64_s *st;
	uint64_t t;
	size_t i;

	for (i = 1, t = 2; i < 64; ++i, t <<= 1) {
		if (t == table_size)
			break;
	}

	if ((table_size < 2) || (i == 64))
		return NULL;

	st = (struct bd64_s *) malloc(sizeof(struct bd64_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct bd64_s));

	st -> t = (uint64_t *) malloc(table_size * sizeof(uint64_t));
	if (st -> t == NULL) {
		free(st);
		return NULL;
	}
	memset(st -> t, 0, table_size * sizeof(uint64_t));

	for (i = 0; i < table_size; ++i)
		st -> t[i] = rng(rng_state);
	st -> p = (rng(rng_state) >> clz64(table_size)) & (table_size - 1);
	st -> l = table_size;
	st -> r = rng;
	st -> s = rng_state;

	return st;
}

int
bd64_destroy(bd64_h *state) {
	struct bd64_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st -> t, 0, st -> l * sizeof(uint64_t));
	free(st -> t);

	memset(st, 0, sizeof(struct bd64_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

uint64_t bd64_rand(void *state) {
	struct bd64_s *st;
	uint64_t k;

	st = (struct bd64_s *) state;
	k = st -> t[st -> p];
	st -> t[st -> p] = st -> r(st -> s);
	st -> p = (k >> clz64(st -> l)) & (st -> l - 1);

	return k;
}

/* ========================================================================== */

mm64_h
/* https://dl.acm.org/doi/10.1145/321250.321257 */
mm64_init(size_t table_size, uint64_t (*index_rng)(void *), void *index_state,
    uint64_t (*value_rng)(void *), void *value_state) {
	struct mm64_s *st;
	uint64_t t;
	size_t i;

	for (i = 1, t = 2; i < 64; ++i, t <<= 1) {
		if (t == table_size)
			break;
	}

	if ((table_size < 2) || (i == 64))
		return NULL;

	st = (struct mm64_s *) malloc(sizeof(struct mm64_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct mm64_s));

	st -> t = (uint64_t *) malloc(table_size * sizeof(uint64_t));
	if (st -> t == NULL) {
		free(st);
		return NULL;
	}
	memset(st -> t, 0, table_size * sizeof(uint64_t));

	for (i = 0; i < table_size; ++i) {
		st -> t[i] = value_rng(value_state);
	}
	st -> l = table_size;
	st -> ir = index_rng;
	st -> vr = value_rng;
	st -> is = index_state;
	st -> vs = value_state;

	return st;
}

int
mm64_destroy(mm64_h *state) {
	struct mm64_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st -> t, 0, st -> l * sizeof(uint64_t));
	free(st -> t);

	memset(st, 0, sizeof(struct mm64_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

uint64_t
mm64_rand(void *state) {
	struct mm64_s *st;
	uint64_t k;
	size_t p;

	st = (struct mm64_s *) state;
	p = (st -> ir(st -> is) >> clz64(st -> l)) & (st -> l - 1);
	k = st -> t[p];
	st -> t[p] = st -> vr(st -> vs);

	return k;
}

//-+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
