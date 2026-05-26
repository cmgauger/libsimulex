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
 * \file	prng_lcg64.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.1.0
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/prng.h>

/**
 * 64-bit linear congruential generator pseudorandom number generator state
 */
struct lcg64_s {
	uint64_t a;
	/**<
	 * additive
	 */
	uint64_t m;
	/**<
	 * multiplier
	 */
	uint64_t s;
	/**<
	 * current position in sequence
	 */
};

/* ========================================================================== */

/**
 * \brief 64-bit Linear Congruential Generator state initializer function
 *
 * This function initializes the state for a 64-bit LCG pseudorandom number
 * generator.  This function selects between the two "built-in" LCG definitions,
 * which are:
 * -# "Primary" LCG \f[
 *         x_{n + 1} = 6726279311198226789 x_{n} + 2531011
 *     \f]
 * -# "Secondary" LCG \f[
 *         x_{n + 1} = 2862933555777941757 x_{n} + 3037000493
 *     \f]
 *
 * \ref lcg64_init_custom
 *
 *
 *
 * \return 64-bit LCG PRNG state if success, NULL if error
 *
 * \param [in] type  primary or secondary algorithm selector
 * \param [in] seed  PRNG seed
 */
lcg64_h
lcg64_init(enum LCG_TYPE type, uint64_t seed) {
	struct lcg64_s *st;

	if (type != LCG_PRIMARY && type != LCG_ALTERNATE)
		return NULL;

	st = (struct lcg64_s *) malloc(sizeof(struct lcg64_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct lcg64_s));
	
	st -> s = seed;
	switch (type) {
	case LCG_PRIMARY:
		st -> a = UINT64_C(0x0000000000269EC3);
		st -> m = UINT64_C(0x5D588B656C078965);
		break;
	case LCG_ALTERNATE:
		/* Constants taken from:
		 * <https://nuclear.llnl.gov/CNP/rng/rngman/node4.html>
		 */
		st -> a = UINT64_C(0x00000000B504F32D);
		st -> m = UINT64_C(0x27BB2EE687B0B0FD);
		break;
	default:
		st -> a = 0;
		st -> m = 0;
		break;
	}

	return st;
}

/**
 * \brief 64-bit Linear Congruential Generator custom state initializer function
 *
 * This function initializes the state for a 64-bit LCG pseudorandom number
 * generator.  This function allows the programmer to select their own additive
 * and multiplier for the LCG.  The general formula of the LCG algorith is as
 * such:
 * \f[
 *     x_{n + 1} = m x_{n} + a
 * \f]
 * Where \f$m\f$ is the multiplier, and \f$a\f$ is the additive.
 *
 *
 *
 * \return 64-bit LCG PRNG state if success, NULL if error
 *
 * \param [in] additive  LCG algorithm additive
 * \param [in] multiplier  LCG algorithm multiplier
 * \param [in] seed  PRNG seed
 */
lcg64_h
lcg64_init_custom(uint64_t additive, uint64_t multiplier, uint64_t seed) {
	struct lcg64_s *st;

	if (additive == 0 || multiplier == 0)
		return NULL;

	st = (struct lcg64_s *) malloc(sizeof(struct lcg64_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct lcg64_s));

	st -> a = additive;
	st -> m = multiplier;
	st -> s = seed;

	return st;
}

/**
 * \brief 64-bit Linear Congruential Generator state destructor function
 *
 * This function destroys a 64-bit LCG pseudorandom number generator state that
 * is passed to it.
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  64-bit LCG PRNG state pointer
 */
int
lcg64_destroy(lcg64_h *state) {
	struct lcg64_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st, 0, sizeof(struct lcg64_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 * \brief 64-bit Linear Congruential Generator pseudorandom function
 *
 * This function implements the actual 64-bit LCG pseudorandom number generator
 * algorithm.
 *
 *
 *
 * \return 64-bit pseudorandom number
 *
 * \param [in,out] state  32-bit LCG PRNG state
 */
uint64_t
lcg64_rand(void *state) {
	struct lcg64_s *st;
	st = (struct lcg64_s *) state;

	st -> s = (st -> m * st -> s) + st -> a;

	return st -> s;
}