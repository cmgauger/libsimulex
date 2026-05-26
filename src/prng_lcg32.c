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
 * \file	prng_lcg32.c
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
 * 32-bit linear congruential generator pseudorandom number generator state
 */
struct lcg32_s {
	uint32_t a;
	/**<
	 * additive
	 */
	uint32_t m;
	/**<
	 * multiplier
	 */
	uint32_t s;
	/**<
	 * current position in sequence
	 */
};

/* ========================================================================== */

/**
 * \brief 32-bit Linear Congruential Generator state initializer function
 *
 * This function initializes the state for a 32-bit LCG pseudorandom number
 * generator.  This function selects between the two "built-in" LCG definitions,
 * which are:
 * -# "Primary" LCG \f[
 *         x_{n + 1} = 6726279311198226789 x_{n} + 2531011
 *     \f]
 * -# "Secondary" LCG \f[
 *         x_{n + 1} = 2862933555777941757 x_{n} + 3037000493
 *     \f]
 *
 * \ref lcg32_init_custom
 *
 *
 *
 * \return 32-bit LCG PRNG state if success, NULL if error
 *
 * \param [in] type  primary or secondary algorithm selector
 * \param [in] seed  PRNG seed
 */
lcg32_h
lcg32_init(enum LCG_TYPE type, uint32_t seed) {
	struct lcg32_s *st;

	if (type != LCG_PRIMARY && type != LCG_ALTERNATE)
		return NULL;

	st = (struct lcg32_s *) malloc(sizeof(struct lcg32_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct lcg32_s));

	st -> s = seed;
	switch (type) {
	case LCG_PRIMARY:
		st -> a = UINT32_C(0x00006073);
		st -> m = UINT32_C(0x41C64E6D);
		break;
	case LCG_ALTERNATE:
		st -> a = UINT32_C(0x00000001);
		st -> m = UINT32_C(0x6C078965);
		break;
	default:
		st -> a = 0;
		st -> m = 0;
		break;
	}

	return st;
}

/**
 * \brief 32-bit Linear Congruential Generator custom state initializer function
 *
 * This function initializes the state for a 32-bit LCG pseudorandom number
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
 * \return 32-bit LCG PRNG state if success, NULL if error
 *
 * \param [in] additive  LCG algorithm additive
 * \param [in] multiplier  LCG algorithm multiplier
 * \param [in] seed  PRNG seed
 */
lcg32_h
lcg32_init_custom(uint32_t additive, uint32_t multiplier, uint32_t seed) {
	struct lcg32_s *st;

	if (additive == 0 || multiplier == 0)
		return NULL;

	st = (struct lcg32_s *) malloc(sizeof(struct lcg32_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct lcg32_s));

	st -> a = additive;
	st -> m = multiplier;
	st -> s = seed;

	return st;
}

/**
 * \brief 32-bit Linear Congruential Generator state destructor function
 *
 * This function destroys a 32-bit LCG pseudorandom number generator state that
 * is passed to it.
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  32-bit LCG PRNG state pointer
 */
int
lcg32_destroy(lcg32_h *state) {
	struct lcg32_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st, 0, sizeof(struct lcg32_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 * \brief 32-bit Linear Congruential Generator pseudorandom function
 *
 * This function implements the actual 32-bit LCG pseudorandom number generator
 * algorithm.
 *
 *
 *
 * \return 32-bit pseudorandom number
 *
 * \param [in,out] state  32-bit LCG PRNG state
 */
uint32_t
lcg32_rand(void *state) {
	struct lcg32_s *st;
	st = (struct lcg32_s *) state;

	st -> s = (st -> m * st -> s) + st -> a;

	return st -> s;
}