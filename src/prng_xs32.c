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
 * \file	prng_xs32.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.1.0
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/bit_hash.h>
#include <simulex/prng.h>

/**
 * 32-bit xoshiro/xoroshiro pseudorandom number generator state
 */
struct xs32_s {
	uint32_t (*rand)(struct xs32_s *);
	/**<
	 * xoshiro/xoroshiro next random value function
	 */
	void (*jump)(struct xs32_s *);
	/**<
	 * xoshiro/xoroshiro "short" jump function
	 */
	void (*long_jump)(struct xs32_s *);
	/**<
	 * xoshiro/xoroshiro "long" jump function
	 */
	uint32_t state[4];
	/**<
	 * current state of the xoshiro/xoroshiro generator
	 */
};

/* ===== 32-bit xoshiro/xoroshiro Support Functions ========================= */

static uint32_t
xoroshiro64ss_next(struct xs32_s *state) {
	uint32_t *s;
	uint32_t s0, s1, r;

	s = state -> state;

	s0 = s[0];
	s1 = s[1];
	r = rotl32(s0 * UINT32_C(0x9E3779BB), 5) * 5;

	s1 ^= s0;
	s[0] = rotl32(s0, 26) ^ s1 ^ (s1 << 9);
	s[1] = rotl32(s1, 13);

	return r;
}

static uint32_t
xoshiro128pp_next(struct xs32_s *state) {
	uint32_t *s;
	uint32_t t, r;

	s = state -> state;

	r = rotl32(s[0] + s[3], 7) + s[0];
	t = s[1] << 9;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl32(s[3], 11);

	return r;
}

static void
xoshiro128pp_jump(struct xs32_s *state) {
	const uint32_t JUMP[4] = {
	    UINT32_C(0x8764000B),
	    UINT32_C(0xF542D2D3),
	    UINT32_C(0x6FA035C3),
	    UINT32_C(0x77F2DB5B)
	};
	uint32_t *s;
	uint32_t s0, s1, s2, s3;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0, s2 = 0, s3 = 0; i < 4; ++i) {
		for (b = 0; b < 32; ++b) {
			if (JUMP[i] & UINT32_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}

			xoshiro128pp_next(state);
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;

	return;
}

static void
xoshiro128pp_long_jump(struct xs32_s *state) {
	const uint32_t LONG_JUMP[4] = {
	    UINT32_C(0xB523952E),
	    UINT32_C(0x0B6F099F),
	    UINT32_C(0xCCF5A0EF),
	    UINT32_C(0x1C580662)
	};
	uint32_t *s;
	uint32_t s0, s1, s2, s3;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0, s2 = 0, s3 = 0; i < 4; ++i) {
		for (b = 0; b < 32; ++b) {
			if (LONG_JUMP[i] & UINT32_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}

			xoshiro128pp_next(state);
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;

	return;
}

static uint32_t
xoshiro128ss_next(struct xs32_s *state) {
	uint32_t *s;
	uint32_t t, r;

	s = state -> state;

	r = rotl32(s[1] * 5, 7) * 9;
	t = s[1] << 9;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl32(s[3], 11);

	return r;
}

static void
xoshiro128ss_jump(struct xs32_s *state) {
	const uint32_t JUMP[4] = {
	    UINT32_C(0x8764000B),
	    UINT32_C(0xF542D2D3),
	    UINT32_C(0x6FA035C3),
	    UINT32_C(0x77F2DB5B)
	};
	uint32_t *s;
	uint32_t s0, s1, s2, s3;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0, s2 = 0, s3 = 0; i < 4; ++i) {
		for (b = 0; b < 32; ++b) {
			if (JUMP[i] & UINT32_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}

			xoshiro128ss_next(state);
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;

	return;
}

static void
xoshiro128ss_long_jump(struct xs32_s *state) {
	const uint32_t LONG_JUMP[4] = {
	    UINT32_C(0xB523952E),
	    UINT32_C(0x0B6F099F),
	    UINT32_C(0xCCF5A0EF),
	    UINT32_C(0x1C580662)
	};
	uint32_t *s;
	uint32_t s0, s1, s2, s3;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0, s2 = 0, s3 = 0; i < 4; ++i) {
		for (b = 0; b < 32; ++b) {
			if (LONG_JUMP[i] & UINT32_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}

			xoshiro128ss_next(state);
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;

	return;
}

/* ===== 32-bit xoshiro/xoroshiro Public Interface ========================== */

/**
 * \brief 32-bit xoshiro/xoroshiro seed sizing function
 *
 * This function returns the size of the key for the 32-bit xoshiro/xoroshiro
 * pseudorandom number generator algorithm specified.
 *
 *
 *
 * \return length of key/state in 32-bit words; or 0 if error
 *
 * \param [in] type  xoshiro/xoroshiro algorithm
 */
size_t
xs32_state_size(enum XS32_TYPE type) {
	size_t length;

	switch (type) {
	case XS32_XOROSHIRO64SS:
		length = 2;
		break;
	case XS32_XOSHIRO128SS:
	case XS32_XOSHIRO128PP:
		length = 4;
		break;
	case XS32__ERROR:
	default:
		length = 0;
		break;
	}

	return length;
}

/**
 * \brief 32-bit xoshiro/xoroshiro state initializer function
 *
 * This function initializes the state for a 32-bit xoshiro/xoroshiro
 * pseudorandom number generator. The different 32-bit-output versions of
 * xoshiro/xoroshiro have been implemented, thus the initializer has a parameter
 * to select the algorithm to be used. Given that the different versions of the
 * pseudorandom number generator require different lengths of seed, a helper
 * function is provided that returns the required seed length.
 *
 * \cite blackman2021
 * \ref xs32_state_size
 * \ref xs16_init
 * \ref xs64_init
 *
 *
 *
 * \return 32-bit xoshiro/xoroshiro PRNG state if success, NULL if error
 *
 * \param [in] type  xoshiro/xoroshiro algorithm
 * \param [in] seed  PRNG seed
 */
xs32_h
xs32_init(enum XS32_TYPE type, const uint32_t *seed) {
	struct xs32_s *st;

	if (seed == NULL)
		return NULL;

	switch (type) {
	case XS32_XOROSHIRO64SS:
	case XS32_XOSHIRO128SS:
	case XS32_XOSHIRO128PP:
		/* All is good, proceed. */
		break;
	case XS32__ERROR:
	default:
		return NULL;
		break;
	}

	st = (struct xs32_s *) malloc(sizeof(struct xs32_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct xs32_s));

	memcpy(st -> state, seed, xs32_state_size(type) * sizeof(uint32_t));
	switch (type) {
	case XS32_XOROSHIRO64SS:
		st -> rand = xoroshiro64ss_next;
		st -> jump = NULL;
		st -> long_jump = NULL;
		break;
	case XS32_XOSHIRO128SS:
		st -> rand = xoshiro128ss_next;
		st -> jump = xoshiro128ss_jump;
		st -> long_jump = xoshiro128ss_long_jump;
		break;
	case XS32_XOSHIRO128PP:
		st -> rand = xoshiro128pp_next;
		st -> jump = xoshiro128pp_jump;
		st -> long_jump = xoshiro128pp_long_jump;
		break;
	default:
		st -> rand = NULL;
		st -> jump = NULL;
		st -> long_jump = NULL;
		break;
	}

	return st;
}

/**
 * \brief 32-bit xoshiro/xoroshiro state destructor function
 *
 * This function destroys a 32-bit xoshiro/xoroshiro pseudorandom number
 * generator state that is passed to it.
 *
 * \ref xs16_destroy
 * \ref xs64_destroy
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  32-bit xoshiro/xoroshiro PRNG state pointer
 */
int
xs32_destroy(xs32_h *state) {
	struct xs32_s *st;

	st = *state;
	memset(st, 0, sizeof(struct xs32_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 * \brief 32-bit xoshiro/xoroshiro pseudorandom function
 *
 * This function calls the \c rand() equivalent function for the 32-bit
 * xoshiro/xoroshiro pseudorandom number generator that was initialized by in
 * the initialization function.
 *
 * \ref xs16_rand
 * \ref xs64_rand
 *
 *
 *
 * \return 32-bit pseudorandom number
 *
 * \param [in,out] state  32-bit xoshiro/xoroshiro PRNG state
 */
uint32_t
xs32_rand(void *state) {
	struct xs32_s *st;
	st = (struct xs32_s *) state;
	return st -> rand(st);
}

/**
 * \brief 32-bit xoshiro/xoroshiro stream "jump" function
 *
 * This function calls, if it exists, the "jump" function for the selected
 * xoshiro/xoroshiro pseudorandom number generator. Reference should be made to
 * the individual jump function impementatations for the specific increment the
 * PRNG will be advanced.
 *
 * \ref xs64_jump
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  32-bit xoshiro/xoroshiro PRNG state
 */
int
xs32_jump(xs32_h state) {
	if (state -> jump != NULL) {
		state -> jump(state);
		return 0;
	} else {
		return 1;
	}
}

/**
 * \brief 32-bit xoshiro/xoroshiro stream "long jump" function
 *
 * This function calls, if it exists, the "long jump" function for the selected
 * xoshiro/xoroshiro pseudorandom number generator. Reference should be made to
 * the individual long jump function implementations for the specific increment
 * the PRNG will be advanced.
 *
 * \ref xs64_long_jump
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  32-bit xoshiro/xoroshiro PRNG state
 */
int
xs32_long_jump(xs32_h state) {
	if (state -> long_jump != NULL) {
		state -> long_jump(state);
		return 0;
	} else {
		return 1;
	}
}