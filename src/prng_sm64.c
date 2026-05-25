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
 * \file	prng_sm64.c
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
 * SplitMix64 pseudorandom number generator state
 */
struct sm64_s {
	uint64_t gamma;
	/**<
	 * additive for the Weyl-sequence
	 */
	uint64_t state;
	/**<
	 * current state of the SplitMix64 generator
	 */
};

/* ===== SplitMix64 Support Functions ======================================= */

static uint64_t
sm64_next_seed(sm64_h state) {
	return (state -> state += state -> gamma);
}

static uint64_t
sm64_mix_gamma(uint64_t z) {
	int n;

	z = (z ^ (z >> 33)) * UINT64_C(0xFF51AFD7ED558CCD);
	z = (z ^ (z >> 33)) * UINT64_C(0xC4CEB9FE1A85EC53);
	z = (z ^ (z >> 33)) | UINT64_C(0x0000000000000001);

	n = pc64(z ^ (z >> 1));

	return (n < 24) ? z ^ UINT64_C(0xAAAAAAAAAAAAAAAA) : z;
}

/* ========================================================================== */

/**
 * \brief SplitMix64 state initializer function
 *
 * This function initializes the state for a SplitMix64 pseudorandom number
 * generator.  This is based on the "default" generator state, using a "gamma"
 * value derived from the golden ratio (phi).
 *
 *
 *
 * \return SplitMix64 PRNG state if success, NULL if error
 *
 * \param [in] seed  PRNG seed
 */
sm64_h
sm64_init(uint64_t seed) {
	struct sm64_s *st;

	st = (struct sm64_s *) malloc(sizeof(struct sm64_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct sm64_s));

	st -> state = seed;
	st -> gamma = UINT64_C(0x9E3779B97F4A7C15);

	return st;
}

/**
 * \brief SplitMix64 custom state initializer function
 *
 * This function initializes the state for a SplitMix64 pseudorandom number
 * generator.  Unlike the "standard" initializer, the additive of the Weyl
 * sequence which is the nucleus of the SplitMix64 generator (i.e. gamma) is
 * defined by the user.
 *
 * Per paper that describes SplitMix, the Weyl sequence additive is supposed to
 * meet certain criteria (mainly that there should be approximately twenty-four
 * 1-0 or 0-1 transitions in the gamma value, and that the gamma value must be
 * odd); however, the transition count criteria is not checked for this custom
 * initializer, though the requirement that the gamma value be both non-zero and
 * odd is.
 *
 *
 *
 * \return SplitMix64 PRNG state if success, NULL if error
 *
 * \param [in] gamma  SplitMix algorithm Weyl sequence additive
 * \param [in] seed  PRNG seed
 */
sm64_h
sm64_init_custom(uint64_t gamma, uint64_t seed) {
	struct sm64_s *st;

	if ((gamma == 0) || ((gamma & 1) == 0))
		return NULL;

	st = (struct sm64_s *) malloc(sizeof(struct sm64_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct sm64_s));

	st -> state = seed;
	st -> gamma = gamma;

	return st;
}

/**
 * \brief SplitMix64 state splitting function
 *
 * This function implements the "splittable" part of the SplitMix64 pseudorandom
 * number generator family.  It should be noted, per O'Neill's article on bugs
 * in various SplitMix64 implementations, this implementation of the stream
 * splitting function is based upon the known-good implementation that is a part
 * of JDK8 (as the implementation in the original article is incorrect).
 *
 *
 *
 * \return SplitMix64 PRNG state if success, NULL if error
 *
 * \param [in,out] state  SplitMix64 PRNG state
 */
sm64_h sm64_split(sm64_h source_state) {
	struct sm64_s *new_state;

	if (source_state == NULL)
		return NULL;

	new_state = (struct sm64_s *) malloc(sizeof(struct sm64_s));
	if (new_state == NULL)
		return NULL;
	memset(new_state, 0, sizeof(struct sm64_s));

	new_state -> state = sm64_rand(source_state);
	new_state -> gamma = sm64_mix_gamma(sm64_next_seed(source_state));

	return new_state;
}

/**
 * \brief SplitMix64 state destructor function
 *
 * This function destroys a SplitMix64 pseudorandom number generator state that
 * is passed to it.
 *
 * \return 0 if success, 1 if failure
 */
int
sm64_destroy(
    sm64_h *state /**< [in,out] SplitMix64 PRNG state pointer */
) {
	struct sm64_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st, 0, sizeof(struct sm64_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 * \brief SplitMix64 pseudorandom function
 *
 * This function implements the actual SplitMix64 pseudorandom number generator
 * algorithm.
 *
 *
 *
 * \return 64-bit pseudorandom number
 *
 * \param [in,out] state  SplitMix64 PRNG state
 */
uint64_t
sm64_rand(void *state) {
	struct sm64_s *st;
	st = (struct sm64_s *) state;
	return mix64(MIX64_STAFFORD13, sm64_next_seed(st));
}