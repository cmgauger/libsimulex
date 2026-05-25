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
 * \file	prng_xs16.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.1.0
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <bit_hash.h>
#include <prng.h>

/**
 * 16-bit xoshiro/xoroshiro pseudorandom number generator state
 */
struct xs16_s {
	uint16_t (*rand)(struct xs16_s *);
	/**<
	 * xoshiro/xoroshiro next random value function
	 */
	uint16_t state[2];
	/**<
	 * current state of the xoshiro/xoroshiro generator
	 */
};

/* ===== 16-bit xoshiro/xoroshiro Support Functions ========================= */

static uint16_t
xoroshiro32pp_next(struct xs16_s *state) {
	uint16_t *s;
	uint16_t s0, s1, r;

	s = state -> state;

	s0 = s[0];
	s1 = s[1];
	r = rotl16(s0 + s1, 9) + s0;

	s1 ^= s0;
	s[0] = (uint16_t) (rotl16(s0, 13) ^ s1 ^ (s1 << 5));
	s[1] = rotl16(s1, 10);

	return r;
}

/* ========================================================================== */

/**
 * \brief 16-bit xoroshiro seed sizing function
 *
 * This function returns the size of the key for the 16-bit xoroshiro
 * pseudorandom number generator algorithm specified.
 *
 *
 *
 * \return length of key/state in 16-bit words
 */
size_t
xs16_state_size(void) {
	return 2;
}

/**
 * \brief 16-bit xoroshiro state initializer function
 *
 * This function initializes the state for a 16-bit xoroshiro pseudorandom
 * number generator. A helper function is provided that returns the required
 * seed length.
 *
 * \cite blackman2021
 * \ref xs16_state_size
 * \ref xs64_init
 * \ref xs32_init
 *
 *
 *
 * \return 16-bit xoroshiro PRNG state if success, NULL if error
 *
 * \param [in] seed  PRNG seed
 */
xs16_h
xs16_init(const uint16_t *seed) {
	struct xs16_s *st;

	if (seed == NULL)
		return NULL;

	st = (struct xs16_s *) malloc(sizeof(struct xs16_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct xs16_s));

	memcpy(st -> state, seed, 2 * sizeof(uint16_t));
	st -> rand = xoroshiro32pp_next;

	return st;
}

/**
 * \brief 16-bit xoroshiro state destructor function
 *
 * This function destroys a 16-bit xoroshiro pseudorandom number
 * generator state that is passed to it.
 *
 * \ref xs64_destroy
 * \ref xs16_destroy
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  16-bit xoroshiro PRNG state pointer
 */
int
xs16_destroy(xs16_h *state) {
	struct xs16_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st, 0, sizeof(struct xs16_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 * \brief 16-bit xoroshiro pseudorandom function
 *
 * This function calls the \c rand() equivalent function for the 16-bit
 * xoroshiro pseudorandom number generator that was initialized by in
 * the initialization function.
 *
 * \ref xs32_init
 * \ref xs64_rand
 *
 *
 *
 * \return 16-bit pseudorandom number
 *
 * \param [in,out] state  16-bit xoroshiro PRNG state
 */
uint16_t
xs16_rand(void *state) {
	struct xs16_s *st;
	st = (struct xs16_s *) state;
	return st -> rand(st);
}