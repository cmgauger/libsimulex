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
 * \file	prng_mt32.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.1.0
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/prng.h>

/** \cond */
#define	MT_N	624
#define	MT_M	397
#define	MT_MIX(u,v)	(((u) & UINT32_C(0x80000000)) | ((v) & UINT32_C(0x7FFFFFFF)))
#define	MT_TWIST(u,v)	((MT_MIX(u,v) >> 1) ^ ((v) & 1 ? UINT32_C(0x9908B0DF) : 0))
/** \endcond */

/**
 * \brief
 */
struct mt32_s {
	uint32_t (*rand)(struct mt32_s *);
	/**<
	 * Mersenne Twister next random value function
	 */
	uint32_t state[MT_N];
	/**<
	 * current state of the Mersenne Twister generator
	 */
	int case_select;
	int position;
};

static uint32_t mt19937(struct mt32_s *);
static uint32_t memt13397_1(struct mt32_s *);
static uint32_t memt13397_2(struct mt32_s *);

/* ===== Mersenne Twister Support Functions ================================= */

/*
 * MT13397 'AR': <https://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/CODES/mt19937ar.c>
 * ME MT13397: <https://www.ritsumei.ac.jp/~harase/memt19937ar.c>
 * ME MT13397 II: <https://www.ritsumei.ac.jp/~harase/memt19937-II.c>
 *
 * TT-800: <https://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/VERSIONS/C-LANG/tt800.c>
 */

/**
 */
static struct mt32_s *
mt32_init_helper_assign(enum MT_TYPE type) {
	struct mt32_s *st;

	st = (struct mt32_s *) malloc(sizeof(struct mt32_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct mt32_s));

	switch (type) {
	case MT_MT19937:
		st -> rand = mt19937;
		st -> case_select = 0;
		st -> position = MT_N;
		break;
	case MT_MEMT19937_1:
		st -> rand = memt13397_1;
		st -> case_select = 1;
		st -> position = 0;
		break;
	case MT_MEMT19937_2:
		st -> rand = memt13397_2;
		st -> case_select = 1;
		st -> position = 0;
		break;
	default:
		st -> rand = NULL;
		st -> case_select = 0;
		st -> position = 0;
	}

	return st;
}

/**
 */
static void
mt32_init_helper_seed(struct mt32_s *state, uint32_t seed) {
	uint32_t i;

	state -> state[0] = seed;

	for (i = 1; i < MT_N; ++i)
		state -> state[i] = (UINT32_C(0x6C078965) * (state ->
		    state[i - 1] ^ (state -> state[i - 1] >> 30)) + i);

	return;
}

/**
 */
static void
mt32_init_helper_array(struct mt32_s *state, const uint32_t *key, size_t
    key_length) {
	size_t k;
	uint32_t i, j;

	mt32_init_helper_seed(state, UINT32_C(0x012BD6AA));

	i = 1, j = 0;
	for (k = (MT_N > key_length) ? MT_N : key_length; k; k--) {
		state -> state[i] = (state -> state[i] ^ ((state ->
		    state[i - 1] ^ (state -> state[i - 1] >> 30)) *
		    UINT32_C(0x0019660D))) + key[j] + j;

		i++;
		if (i >= MT_N) {
			state -> state[0] = state -> state[MT_N - 1];
			i = 1;
		}

		j++;
		if (j >= key_length)
			j = 0;
	}
	
	for (k = MT_N - 1; k; k--) {
		state -> state[i] = (state -> state[i] ^ ((state ->
		    state[i - 1] ^ (state -> state[i - 1] >> 30)) *
		    UINT32_C(0x5D588B65))) - i;


		i++;
		if (i >= MT_N) {
			state -> state[0] = state -> state[MT_N - 1];
			i = 1;
		}
	}

	state -> state[0] = UINT32_C(0x80000000);

	return;
}

/**
 */
static uint32_t
mt19937(struct mt32_s *state) {
	uint32_t y;
	int i;

	if (state -> position >= MT_N) {
		for (i = 0; i < MT_N - MT_M; ++i) {
			y = MT_MIX(state -> state[i], state -> state[i + 1]);
			state -> state[i] = state -> state[i + MT_M] ^
			    MT_TWIST(y, y);
		}

		for (; i < MT_N - 1; ++i) {
			y = MT_MIX(state -> state[i], state -> state[i + 1]);
			state -> state[i] = state -> state[i + (MT_M - MT_N)] ^
			    MT_TWIST(y, y);
		}

		y = MT_MIX(state -> state[MT_N - 1], state -> state[0]);
		state -> state[MT_N - 1] = state -> state[MT_M - 1] ^
		    MT_TWIST(y, y);

		state -> position = 0;

	}

	y  = state -> state[state -> position++];

	/* Tempering */
	y ^= (y >> 11);
	y ^= (y <<  7) & UINT32_C(0x9D2C5680);
	y ^= (y << 15) & UINT32_C(0xEFC60000);
	y ^= (y >> 18);

	return y;
}

/**
 */
static uint32_t
memt13397_1(struct mt32_s *state) {
	uint32_t y;
	int p, s0, s1, s2, s3, s4;
	
	p = state -> position;
	switch (state -> case_select) {
	case 1:
		s0 = p + MT_M;
		s1 = +224;
		s2 = +124;
		s3 =  +24;
		s4 = +324;
		break;
	case 2:
		s0 = p + (MT_M - MT_N);
		s1 = +224;
		s2 = +124;
		s3 =  +24;
		s4 = +324;
		break;
	case 3:
		s0 = p + (MT_M - MT_N);
		s1 = +224;
		s2 = +124;
		s3 =  +24;
		s4 = -300;
		break;
	case 4:
		s0 = p + (MT_M - MT_N);
		s1 = -400;
		s2 = +124;
		s3 =  +24;
		s4 = -300;
		break;
	case 5:
		s0 = p + (MT_M - MT_N);
		s1 = -400;
		s2 = -500;
		s3 =  +24;
		s4 = -300;
		break;
	case 6:
		s0 = p + (MT_M - MT_N);
		s1 = -400;
		s2 = -500;
		s3 = -600;
		s4 = -300;
		break;
	case 7:
		s0 = MT_M - 1;
		s1 = -400;
		s2 = -500;
		s3 = -600;
		s4 = -300;
		break;
	default:
		return 0;
	}

	y = MT_MIX(state -> state[p], state -> state[(p + 1) % MT_N]);
	state -> state[p] = state -> state[s0] ^ MT_TWIST(y, y);
	y  = state -> state[p] ^ ((state -> state[p + s1] << 14) &
	    UINT32_C(0x3CD68000)) ^ ((state -> state[p + s2] <<  3) &
	    UINT32_C(0x576BAD28)) ^ ((state -> state[p + s3] << 18) &
	    UINT32_C(0xD6740000));
	y ^= y >> 11;
	y ^= y <<  7;
	y ^= (state -> state[p + s4] & UINT32_C(0x09040000));

	state -> position++;

	switch (state -> case_select) {
	case 1:
		if (state -> position == (MT_N - MT_M))
			state -> case_select = 2;
		break;
	case 2:
		if(state -> position == 300)
			state -> case_select = 3;
		break;
	case 3:
		if(state -> position == 400)
			state -> case_select = 4;
		break;
	case 4:
		if(state -> position == 500)
			state -> case_select = 5;
		break;
	case 5:
		if(state -> position == 600)
			state -> case_select = 6;
		break;
	case 6:
		if(state -> position == MT_N - 1)
			state -> case_select = 7;
		break;
	case 7:
		state -> position = 0;
		state -> case_select = 1;
		break;
	default:
		return 0;
	}

	return y;
}

/**
 */
static uint32_t
memt13397_2(struct mt32_s *state) {
	uint32_t y;
	int p, s0, s1, s2;
	
	p = state -> position;
	switch (state -> case_select) {
	case 1:
		s0 = p + MT_M;
		s1 = +151;
		s2 =  +36;
		break;
	case 2:
		s0 = p + (MT_M - MT_N);
		s1 = +151;
		s2 =  +36;
		break;
	case 3:
		s0 = p + (MT_M - MT_N);
		s1 = -473;
		s2 =  +36;
		break;
	case 4:
		s0 = p + (MT_M - MT_N);
		s1 = -473;
		s2 = -588;
		break;
	case 5:
		s0 = MT_M - 1;
		s1 = -473;
		s2 = -588;
		break;
	default:
		return 0;
	}
	
	y = MT_MIX(state -> state[p], state -> state[(p + 1) % MT_N]);
	state -> state[p] = state -> state[s0] ^ MT_TWIST(y, state ->
	    state[(p + 1) % MT_N]);
	y  = state -> state[p] ^ (state -> state[p + s1] &
	    UINT32_C(0xB219BEAB));
	y ^= y <<  8;
	y ^= y << 14;
	y ^= (state -> state[p + s2] & UINT32_C(0x56BDE52A));
	
	state -> position++;

	switch (state -> case_select) {
	case 1:
		if (state -> position == (MT_N - MT_M))
			state -> case_select = 2;
		break;
	case 2:
		if (state -> position == 473)
			state -> case_select = 3;
		break;
	case 3:
		if (state -> position == 588)
			state -> case_select = 4;
		break;
	case 4:
		if (state -> position == (MT_N - 1))
			state -> case_select = 5;
		break;
	case 5:
		state -> position = 0;
		state -> case_select = 1;
		break;
	default:
		return 0;
	}

	return y;
}

/* ========================================================================== */

/**
 * \brief Mersenne Twister state initializer function
---+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
 *
 *
 *
 * \return
 *
 * \param [in] type  Mersenne Twister algorithm selection
 * \param [in] seed  PRNG seed
 */
mt32_h
mt32_init_seed(enum MT_TYPE type, uint32_t seed) {
	struct mt32_s *st;

	switch (type) {
	case MT_MT19937:
	case MT_MEMT19937_1:
	case MT_MEMT19937_2:
		st = mt32_init_helper_assign(type);
		if (st == NULL)
			return NULL;
		break;
	case MT__ERROR:
	default:
		return NULL;
	}

	mt32_init_helper_seed(st, seed);

	return st;
}


/**
 * \brief Mersenne Twister state initializer function
---+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
 *
 *
 *
 * \return
 *
 * \param [in] type  Mersenne Twister algorithm selection
 * \param [in] seed  PRNG seed
 * \param [in] length  length of PRNG seed array
 */
mt32_h
mt32_init_array(enum MT_TYPE type, const uint32_t *seed, size_t length) {
	struct mt32_s *st;

	if ((seed == NULL) || (length == 0))
		return NULL;

	switch (type) {
	case MT_MT19937:
	case MT_MEMT19937_1:
	case MT_MEMT19937_2:
		st = mt32_init_helper_assign(type);
		if (st == NULL)
			return NULL;
		break;
	case MT__ERROR:
	default:
		return NULL;
	}

	mt32_init_helper_array(st, seed, length);

	return st;
}

/**
 * \brief Mersenne Twister state destructor function
---+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  Mersenne Twister state pointer
 */
int
mt32_destroy(mt32_h *state) {
	struct mt32_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st, 0, sizeof(struct mt32_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 * \brief 32-bit WELL PRNG pseudorandom function
---+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
 *
 *
 *
 * \return
 *
 * \param [in,out] state  WELL PRNG state
 */
uint32_t
mt32_rand(void *state) {
	struct mt32_s *st;

	if (state == NULL)
		return 0;

	st = (struct mt32_s *) state;
	if (st -> rand == NULL)
		return 0;

	return st -> rand(st);
}