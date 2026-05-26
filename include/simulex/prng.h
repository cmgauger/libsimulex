#ifndef PRNG_H
#define PRNG_H
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
 * \file	prng.h
 * \copyright	MIT
 * \date	2024
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>

typedef struct bd64_s *bd64_h;
typedef struct mm64_s *mm64_h;
typedef struct sm64_s *sm64_h;
typedef struct xs64_s *xs64_h;
typedef struct lcg64_s *lcg64_h;
typedef struct xchacha20_s *xchacha20_h;
typedef struct bd32_s *bd32_h;
typedef struct mm32_s *mm32_h;
typedef struct xs32_s *xs32_h;
typedef struct well32_s *well32_h;
typedef struct mt32_s *mt32_h;
typedef struct lcg32_s *lcg32_h;
typedef struct xs16_s *xs16_h;
typedef struct lcg16_s *lcg16_h;

enum XCHACHA20_PARAM {
	XCHACHA20_KEY,
	XCHACHA20_IV
};

enum XS64_TYPE {
	XS64__ERROR,
	XS64_XOROSHIRO128PP,
	XS64_XOROSHIRO128SS,
	XS64_XOSHIRO256PP,
	XS64_XOSHIRO256SS,
	XS64_XOSHIRO512PP,
	XS64_XOSHIRO512SS,
	XS64_XOROSHIRO1024PP,
	XS64_XOROSHIRO1024SS
};

enum XS32_TYPE {
	XS32__ERROR,
	XS32_XOROSHIRO64SS,
	XS32_XOSHIRO128PP,
	XS32_XOSHIRO128SS
};

enum WELL32_TYPE {
	WELL32__ERROR,
	WELL32_512A,
	WELL32_521A,
	WELL32_521B,
	WELL32_607A,
	WELL32_607B,
	WELL32_800A,
	WELL32_800B,
	WELL32_800C,
	WELL32_800D,
	WELL32_800E,
	WELL32_800F,
	WELL32_1024A,
	WELL32_1024B,
	WELL32_19937A,
	WELL32_19937B,
	WELL32_19937C,
	WELL32_19937D,
	WELL32_19937E,
	WELL32_19937F,
	WELL32_21701A,
	WELL32_21701C,
	WELL32_21701E,
	WELL32_23209A,
	WELL32_23209B,
	WELL32_23209C,
	WELL32_23209D,
	WELL32_23209E,
	WELL32_23209F,
	WELL32_44497A,
	WELL32_44497C,
	WELL32_44497E
};

enum MT_TYPE {
	MT__ERROR,
	MT_MT19937,
	MT_MEMT19937_1,
	MT_MEMT19937_2
};

enum LCG_TYPE {
	LCG__ERROR,
	LCG_PRIMARY,
	LCG_ALTERNATE
};

/* 64-bit & 32-bit xchacha20 PRNG */
size_t xchacha20_parameter_length(enum XCHACHA20_PARAM);
xchacha20_h xchacha20_init(const uint8_t *, const uint8_t *, const uint64_t);
int xchacha20_destroy(xchacha20_h *);
uint64_t xchacha20_rand64(xchacha20_h);
uint32_t xchacha20_rand32(xchacha20_h);

/* 64-bit Bays-Durham Algorithm */
bd64_h bd64_init(size_t, uint64_t (*)(void *), void *);
int bd64_destroy(bd64_h *);
uint64_t bd64_rand(void *);

/* 64-bit Maclaren-Marsaglia Algorithm */
mm64_h mm64_init(size_t, uint64_t (*)(void *), void *, uint64_t (*)(void *), void *);
int mm64_destroy(mm64_h *);
uint64_t mm64_rand(void *);

/* SplitMix64 PRNG */
sm64_h sm64_init(uint64_t);
sm64_h sm64_init_custom(uint64_t, uint64_t);
sm64_h sm64_split(sm64_h);
int sm64_destroy(sm64_h *);
uint64_t sm64_rand(void *);

/* 64-bit xoshiro/xoroshiro PRNGs */
size_t xs64_state_size(enum XS64_TYPE);
xs64_h xs64_init(enum XS64_TYPE, const uint64_t *);
int xs64_destroy(xs64_h *);
uint64_t xs64_rand(void *);
int xs64_jump(xs64_h);
int xs64_long_jump(xs64_h);

/* 64-bit Linear Congruential Generator PRNGs */
lcg64_h lcg64_init(enum LCG_TYPE, uint64_t);
lcg64_h lcg64_init_custom(uint64_t, uint64_t, uint64_t);
int lcg64_destroy(lcg64_h *);
uint64_t lcg64_rand(void *);

/* 32-bit Bays-Durham Algorithm */
bd32_h bd32_init(size_t, uint32_t (*)(void *), void *);
int bd32_destroy(bd32_h *);
uint32_t bd32_rand(void *);

/* 32-bit Maclaren-Marsaglia Algorithm */
mm32_h mm32_init(size_t, uint32_t (*)(void *), void *, uint32_t (*)(void *), void *);
int mm32_destroy(mm32_h *);
uint32_t mm32_rand(void *);

/* 32-bit xoshiro/xoroshiro PRNGs */
size_t xs32_state_size(enum XS32_TYPE);
xs32_h xs32_init(enum XS32_TYPE, const uint32_t *);
int xs32_destroy(xs32_h *);
uint32_t xs32_rand(void *);
int xs32_jump(xs32_h);
int xs32_long_jump(xs32_h);

/* 32-bit WELL (Well Equidistributed Long-period Linear) PRNGs */
size_t well32_state_size(enum WELL32_TYPE);
well32_h well32_init(enum WELL32_TYPE, const uint32_t *);
int well32_destroy(well32_h *);
uint32_t well32_rand(void *);

/* 32-bit Mersenne Twister PRNG */
mt32_h mt32_init_seed(enum MT_TYPE, uint32_t);
mt32_h mt32_init_array(enum MT_TYPE, const uint32_t *, size_t);
int mt32_destroy(mt32_h *);
uint32_t mt32_rand(void *);

/* 32-bit Linear Congruential Generator PRNGs */
lcg32_h lcg32_init(enum LCG_TYPE, uint32_t);
lcg32_h lcg32_init_custom(uint32_t, uint32_t, uint32_t);
int lcg32_destroy(lcg32_h *);
uint32_t lcg32_rand(void *);

/* 16-bit xoroshiro PRNG */
size_t xs16_state_size(void);
xs16_h xs16_init(const uint16_t *);
int xs16_destroy(xs16_h *);
uint16_t xs16_rand(void *);

/* 16-bit Linear Congruential Generator PRNGs */
lcg16_h lcg16_init(enum LCG_TYPE, uint16_t);
lcg16_h lcg16_init_custom(uint16_t, uint16_t, uint16_t);
int lcg16_destroy(lcg16_h *);
uint16_t lcg16_rand(void *);

#endif /* PRNG_H */
