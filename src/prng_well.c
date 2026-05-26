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
 * \file	prng_well.c
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

#define	MASKU		(UINT32_C(0xFFFFFFFF) >> (32 - P))
#define	MASKL		(~MASKU)
#define	MAT0POS(t,v)	((v) ^ ((v) >>   (t) ))
#define	MAT0NEG(t,v)	((v) ^ ((v) << (-(t))))
#define	MAT1(v)		(v)
#define	MAT2(a,v)	(((v) & 1U) ? (((v) >> 1) ^ (a)) : ((v) >> 1))
#define	MAT3POS(t,v)	((v) >> (t))
#define	MAT3NEG(t,v)	((v) << (-(t)))
#define	MAT4POS(t,b,v)	((v) ^ (((v) >>   (t) ) & (b)))
#define	MAT4NEG(t,b,v)	((v) ^ (((v) << (-(t))) & (b)))
#define	MXT5(r,a,ds,dt,v)	(((v) & (dt)) ? \
    (((((v) << (r)) ^ ((v) >> (32 - (r)))) & (ds)) ^ (a)) : \
    ((((v) << (r)) ^ ((v) >> (32 - (r)))) & (ds)))
#define	MAT5(r,a,ds,dt,v)	(((((v) << (r)) ^ ((v) >> (32 - (r)))) & (ds)) ^ (((v) & (dt)) ? (a) : 0))
#define	MAT7(v)		(0)
#define	V0		state -> state[ state -> state_select              ]
#define	VM1		state -> state[(state -> state_select + M1)     % R]
#define	VM2		state -> state[(state -> state_select + M2)     % R]
#define	VM3		state -> state[(state -> state_select + M3)     % R]
#define	VRm1		state -> state[(state -> state_select + R - 1)  % R]
#define	VRm2		state -> state[(state -> state_select + R - 2)  % R]
#define	newV0		state -> state[(state -> state_select + R - 1)  % R]
#define	newV1		state -> state[ state -> state_select              ]
#define	newVRm1		state -> state[(state -> state_select + R - 2)  % R]
#define	newVM2		state -> state[(state -> state_select + M2 + 1) % R]

/**
 * \brief
 */
struct well32_s {
	uint32_t (*rand)(struct well32_s *);
	/**<
	 * WELL PRNG next random value function
	 */
	uint32_t state[1391];
	/**<
	 * current state of the WELL PRNG
	 */
	size_t state_select;
	/**<
	 * tempering flag
	 */
	int temper;
};

/* ===== 32-bit WELL Support Functions ====================================== */

static uint32_t
well512a(struct well32_s *state) {
	const size_t M1 = 13;
	const size_t M2 = 9;
/*	const size_t M3 = 5;	defined in the WELL paper, but not used */
	const size_t R = 16;
	uint32_t z0, z1, z2;

	z0 = VRm1;
	z1 = MAT0NEG(-16, V0) ^ MAT0NEG(-15, VM1);
	z2 = MAT0POS(11, VM2);

	newV1 = z1 ^ z2;
	newV0 = MAT0NEG(-2, z0) ^ MAT0NEG(-18, z1) ^ MAT3NEG(-28, z2) ^
	    MAT4NEG(-5, UINT32_C(0xDA442D24), newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	return state -> state[state -> state_select];
}

static uint32_t
well521a(struct well32_s *state) {
	const size_t M1 = 13;
	const size_t M2 = 11;
	const size_t M3 = 10;
	const size_t P = 23;
	const size_t R = 17;
	uint32_t z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT0NEG(-13, V0) ^ MAT0NEG(-15, VM1);
	z2 = MAT1(VM2) ^ MAT3NEG(-21, VM3);

	newV1 = z1 ^ z2;
	newV0 =  MAT0NEG(-13, z0) ^ MAT3POS(1, z1) ^ MAT7(z2) ^
	    MAT0POS(11, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	return state -> state[state -> state_select];
}

static uint32_t
well521b(struct well32_s *state) {
	const size_t M1 = 11;
/*	const size_t M2 = 10;	defined in the WELL paper, but not used */
	const size_t M3 = 7;
	const size_t P = 23;
	const size_t R = 17;
	uint32_t z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT0NEG(-21, V0) ^ MAT0POS(6, VM1);
	z2 = MAT7(VM2) ^ MAT0NEG(-13, VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT0POS(13, z0) ^ MAT3NEG(-10, z1) ^ MAT3NEG(-5, z2) ^
	    MAT0POS(13, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	return state -> state[state -> state_select];
}

static uint32_t
well607a(struct well32_s *state) {
	const size_t M1 = 16;
	const size_t M2 = 15;
	const size_t M3 = 14;
	const size_t P = 1;
	const size_t R = 19;
	uint32_t z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT0POS(19, V0) ^ MAT0POS(11, VM1);
	z2 = MAT0NEG(-14, VM2) ^ MAT1(VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT0POS(18, z0) ^ MAT1(z1) ^ MAT7(z2) ^ MAT0NEG(-5, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	return state -> state[state -> state_select];
}

static uint32_t
well607b(struct well32_s *state) {
	const size_t M1 = 16;
/*	const size_t M2 = 8;	defined in the WELL paper, but not used */
	const size_t M3 = 13;
	const size_t P = 1;
	const size_t R = 19;
	uint32_t z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT0NEG(-18, V0) ^ MAT0NEG(-14, VM1);
	z2 = MAT7(VM2) ^ MAT0POS(18, VM3);

	newV1  = z1 ^ z2;
	newV0 = MAT0NEG(-24, z0) ^ MAT0POS(5, z1) ^ MAT0NEG(-1, z2) ^
	    MAT7(newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	return state -> state[state -> state_select];
}

static uint32_t
well800a(struct well32_s *state) {
	const size_t M1 = 14;
	const size_t M2 = 18;
	const size_t M3 = 17;
	const size_t R = 25;
	uint32_t y, z0, z1, z2;

	z0 = VRm1;
	z1 = MAT1(V0) ^ MAT0NEG(-15, VM1);
	z2 = MAT0POS(10, VM2) ^ MAT0NEG(-11, VM3);

	newV1  = z1 ^ z2;
	newV0 = MAT0POS(16, z0) ^ MAT3POS(20, z1) ^ MAT1(z2) ^
	    MAT0NEG(-28, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	y = state -> state[state -> state_select];
	switch (state -> temper) {
	case 1:
		y ^= (y <<  7) & UINT32_C(0x93DD1400);
		y ^= (y << 15) & UINT32_C(0xFA118000);
		break;
	case 2:
		y ^= newVM2 & UINT32_C(0x00004880);
		break;
	default:
		/* Do nothing */
		break;
	}

	return y;
}

static uint32_t
well800b(struct well32_s *state) {
	const size_t M1 = 9;
	const size_t M2 = 4;
	const size_t M3 = 22;
	const size_t R = 25;
	uint32_t y, z0, z1, z2;

	z0 = VRm1;
	z1 = MAT0NEG(-29, V0) ^ MAT3NEG(-14, VM1);
	z2 = MAT1(VM2) ^ MAT3POS(19, VM3);

	newV1  = z1 ^ z2;
	newV0 = MAT1(z0) ^ MAT0POS(10, z1) ^ MAT2(UINT32_C(0xD3E43FFD), z2) ^
	    MAT0NEG(-25, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	y = state -> state[state -> state_select];
	switch (state -> temper) {
	case 1:
		y ^= (y <<  7) & UINT32_C(0xE46E1700);
		y ^= (y << 15) & UINT32_C(0x9B868000);
		break;
	case 2:
		y ^= newVM2 & UINT32_C(0x17030806);
		break;
	default:
		/* Do nothing */
		break;
	}

	return y;
}

static uint32_t
well1024a(struct well32_s *state) {
	const size_t M1 = 3;
	const size_t M2 = 24;
	const size_t M3 = 10;
	const size_t R = 32;
	uint32_t z0, z1, z2;

	z0 = VRm1;
	z1 = MAT1(V0) ^ MAT0POS(8, VM1);
	z2 = MAT0NEG(-19, VM2) ^ MAT0NEG(-14, VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT0NEG (-11, z0) ^ MAT0NEG(-7, z1) ^ MAT0NEG(-13, z2);

	state -> state_select = (state -> state_select + R - 1) % R;

	return state -> state[state -> state_select];
}

static uint32_t
well1024b(struct well32_s *state) {
	const size_t M1 = 22;
	const size_t M2 = 25;
	const size_t M3 = 26;
	const size_t R = 32;
	uint32_t z0, z1, z2;

	z0 = VRm1;
	z1 = MAT0NEG(-21, V0) ^ MAT0POS (17, VM1);
	z2 = MAT2(UINT32_C(0x8BDCB91E), VM2) ^ MAT0POS(15, VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT0NEG(-14, z0) ^ MAT0NEG(-21, z1) ^ MAT1(z2);

	state -> state_select = (state -> state_select + R - 1) % R;

	return state -> state[state -> state_select];
}

static uint32_t
well19937a(struct well32_s *state) {
	const size_t M1 = 70;
	const size_t M2 = 179;
	const size_t M3 = 449;
	const size_t P = 31;
	const size_t R = 624;
	uint32_t y, z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT0NEG(-25, V0) ^ MAT0POS(27, VM1);
	z2 = MAT3POS(9, VM2) ^ MAT0POS(1, VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT1(z0) ^ MAT0NEG(-9, z1) ^ MAT0NEG(-21, z2) ^
	    MAT0POS(21, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	y = state -> state[state -> state_select];
	switch (state -> temper) {
	case 1:
		y ^= (y <<  7) & UINT32_C(0xE46E1700);
		y ^= (y << 15) & UINT32_C(0x9B868000);
		break;
	case 2:
		y ^= newVM2 & UINT32_C(0x4118000);
		break;
	default:
		/* Do nothing */
		break;
	}

	return y;
}

static uint32_t
well19937b(struct well32_s *state) {
	const size_t M1 = 203;
	const size_t M2 = 613;
	const size_t M3 = 123;
	const size_t P = 31;
	const size_t R = 624;
	uint32_t y, z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT0POS(7, V0) ^ MAT1(VM1);
	z2 = MAT0POS(12, VM2) ^ MAT0NEG(-10, VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT0NEG(-19, z0) ^ MAT3NEG(-11, z1) ^ MAT0POS(4, z2) ^
	    MAT0NEG (-10, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	y = state -> state[state -> state_select];
	switch (state -> temper) {
	case 1:
		y ^= (y <<  7) & UINT32_C(0xE46E1700);
		y ^= (y << 15) & UINT32_C(0x9B868000);
		break;
	case 2:
		y ^= newVM2 & UINT32_C(0x30200010);
		break;
	default:
		/* Do nothing */
		break;
	}

	return y;
}

static uint32_t
well21701a(struct well32_s *state) {
	const size_t M1 = 151;
	const size_t M2 = 327;
/*	const size_t M3 = 84;	defined in the WELL paper, but not used */
	const size_t P = 27;
	const size_t R = 679;
	uint32_t y, z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT1(V0) ^ MAT0NEG(-26, VM1);
	z2 = MAT0POS(19, VM2) ^ MAT7(VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT0POS(27, z0) ^ MAT0NEG(-11, z1) ^ MXT5(15,
	    UINT32_C(0x86A9D87E), UINT32_C(0xFFFFFFEF), UINT32_C(0x00200000),
	    z2) ^ MAT0NEG(-16, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	y = state -> state[state -> state_select];
	switch (state -> temper) {
	case 1:
		y ^= (y <<  7) & UINT32_C(0x93DD1400);
		y ^= (y << 15) & UINT32_C(0xFA118000);
		break;
	case 2:
		y ^= newVM2 & UINT32_C(0x00001002);
		break;
	default:
		/* Do nothing */
		break;
	}

	return y;
}

static uint32_t
well23209a(struct well32_s *state) {
	const size_t M1 = 667;
	const size_t M2 = 43;
	const size_t M3 = 462;
	const size_t P = 23;
	const size_t R = 726;
	uint32_t y, z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT0POS(28, V0) ^ MAT1(VM1);
	z2 = MAT0POS(18, VM2) ^ MAT0POS(3, VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT0POS(21, z0) ^ MAT0NEG(-17, z1) ^ MAT0NEG(-28, z2) ^
	    MAT0NEG(-1, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	y = state -> state[state -> state_select];
	switch (state -> temper) {
	case 1:
		y ^= (y <<  7) & UINT32_C(0x93DD1400);
		y ^= (y << 15) & UINT32_C(0xFA118000);
		break;
	case 2:
		y ^= newVM2 & UINT32_C(0x05100000);
		break;
	default:
		/* Do nothing */
		break;
	}

	return y;
}

static uint32_t
well23209b(struct well32_s *state) {
	const size_t M1 = 610;
	const size_t M2 = 175;
	const size_t M3 = 662;
	const size_t P = 23;
	const size_t R = 726;
	uint32_t y, z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT2(UINT32_C(0xA8C296D1), V0) ^ MAT1(VM1);
	z2 = MXT5(15, UINT32_C(0x5D6B45CC), UINT32_C(0xFFFEFFFF),
	    UINT32_C(0x00000002), VM2) ^ MAT0NEG(-24, VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT0NEG(-26, z0) ^ MAT1(z1) ^ MAT7(z2) ^ MAT0POS(16, newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	y = state -> state[state -> state_select];
	switch (state -> temper) {
	case 1:
		y ^= (y <<  7) & UINT32_C(0x93DD1400);
		y ^= (y << 15) & UINT32_C(0xFA118000);
		break;
	case 2:
		y ^= newVM2 & UINT32_C(0x34000300);
		break;
	default:
		/* Do nothing */
		break;
	}

	return y;
}

static uint32_t
well44497a(struct well32_s *state) {
	const size_t M1 = 23;
	const size_t M2 = 481;
	const size_t M3 = 229;
	const size_t P = 15;
	const size_t R = 1391;
	uint32_t y, z0, z1, z2;

	z0 = (VRm1 & MASKL) | (VRm2 & MASKU);
	z1 = MAT0NEG(-24, V0) ^ MAT0POS(30, VM1);
	z2 = MAT0NEG(-10, VM2) ^ MAT3NEG(-26, VM3);

	newV1 = z1 ^ z2;
	newV0 = MAT1(z0) ^ MAT0POS(20, z1) ^ MXT5(9, UINT32_C(0xB729FCEC),
	    UINT32_C(0xFBFFFFFF), UINT32_C(0x00020000), z2) ^ MAT1(newV1);

	state -> state_select = (state -> state_select + R - 1) % R;

	y = state -> state[state -> state_select];
	switch (state -> temper) {
	case 1:
		y ^= (y <<  7) & UINT32_C(0x93DD1400);
		y ^= (y << 15) & UINT32_C(0xFA118000);
		break;
	case 2:
		y ^= newVM2 & UINT32_C(0x48000000);
		break;
	default:
		/* Do nothing */
		break;
	}

	return y;
}

/* ========================================================================== */

/**
 * \brief 32-bit WELL PRNG seed sizing function
 *
 * This function returns the size of the key for the Well Equidistributed
 * Long-period Linear (WELL) pseudorandom number generator algorithm specified.
 *
 *
 *
 * \return length of key/state in 32-bit words; or 0 if error
 *
 * \param [in] type  type of WELL PRNG
 */
size_t
well32_state_size(enum WELL32_TYPE type) {
	size_t length;

	switch (type) {
	case WELL32_512A:
		length = 16;
		break;
	case WELL32_521A:
	case WELL32_521B:
		length = 17;
		break;
	case WELL32_607A:
	case WELL32_607B:
		length = 19;
		break;
	case WELL32_800A:
	case WELL32_800C:
	case WELL32_800B:
	case WELL32_800D:
	case WELL32_800E:
	case WELL32_800F:
		length = 25;
		break;
	case WELL32_1024A:
	case WELL32_1024B:
		length = 32;
		break;
	case WELL32_19937A:
	case WELL32_19937B:
	case WELL32_19937C:
	case WELL32_19937D:
	case WELL32_19937E:
	case WELL32_19937F:
		length = 624;
		break;
	case WELL32_21701A:
	case WELL32_21701C:
	case WELL32_21701E:
		length = 679;
		break;
	case WELL32_23209A:
	case WELL32_23209B:
	case WELL32_23209C:
	case WELL32_23209D:
	case WELL32_23209E:
	case WELL32_23209F:
		length = 726;
		break;
	case WELL32_44497A:
	case WELL32_44497C:
	case WELL32_44497E:
		length = 1391;
		break;
	case WELL32__ERROR:
	default:
		length = 0;
		break;
	}

	return length;
}

/**
 * \brief 32-bit WELL PRNG state initializer function
---+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
 *
 *
 *
 * \return
 *
 * \param [in] type  
 * \param [in] seed  PRNG seed
 */
well32_h
well32_init(enum WELL32_TYPE type, const uint32_t *seed) {
	struct well32_s *st;

	if (seed == NULL)
		return NULL;

	switch (type) {
	case WELL32_512A:
	case WELL32_521A:
	case WELL32_521B:
	case WELL32_607A:
	case WELL32_607B:
	case WELL32_800A:
	case WELL32_800B:
	case WELL32_800E:
	case WELL32_800F:
	case WELL32_1024A:
	case WELL32_1024B:
	case WELL32_19937A:
	case WELL32_19937B:
	case WELL32_19937C:
	case WELL32_19937E:
	case WELL32_19937F:
	case WELL32_21701A:
	case WELL32_21701E:
	case WELL32_23209A:
	case WELL32_23209B:
	case WELL32_23209E:
	case WELL32_23209F:
	case WELL32_44497A:
	case WELL32_44497C:
	case WELL32_44497E:
		/* All is good, proceed. */
		break;
	case WELL32_800C:
	case WELL32_800D:
	case WELL32_19937D:
	case WELL32_21701C:
	case WELL32_23209C:
	case WELL32_23209D:
	case WELL32__ERROR:
	default:
		return NULL;
		break;
	}

	st = (struct well32_s *) malloc(sizeof(struct well32_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct well32_s));

	memcpy(st -> state, seed, well32_state_size(type) *
	    sizeof(uint32_t));
	st -> temper = 0;
	switch (type) {
	case WELL32_512A:
		st -> rand = well512a;
		break;
	case WELL32_521A:
		st -> rand = well521a;
		break;
	case WELL32_521B:
		st -> rand = well521b;
		break;
	case WELL32_607A:
		st -> rand = well607a;
		break;
	case WELL32_607B:
		st -> rand = well607b;
		break;
	case WELL32_800E:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_800C:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_800A:
		st -> rand = well800a;
		break;
	case WELL32_800F:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_800D:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_800B:
		st -> rand = well800b;
		break;
	case WELL32_1024A:
		st -> rand = well1024a;
		break;
	case WELL32_1024B:
		st -> rand = well1024b;
		break;
	case WELL32_19937E:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_19937C:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_19937A:
		st -> rand = well19937a;
		break;
	case WELL32_19937F:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_19937D:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_19937B:
		st -> rand = well19937b;
		break;
	case WELL32_21701E:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_21701C:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_21701A:
		st -> rand = well21701a;
		break;
	case WELL32_23209E:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_23209C:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_23209A:
		st -> rand = well23209a;
		break;
	case WELL32_23209F:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_23209D:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_23209B:
		st -> rand = well23209b;
		break;
	case WELL32_44497E:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_44497C:
		st -> temper += 1;
		/* FALLTHROUGH */
	case WELL32_44497A:
		st -> rand = well44497a;
		break;
	case WELL32__ERROR:
		st -> rand = NULL;
		break;
	}
	st -> state_select = 0;

	return st;
}

/**
 * \brief 32-bit WELL PRNG state destructor function
---+++---1---+++---2---+++---3---+++---4---+++---5---+++---6---+++---7---+++---8
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  WELL PRNG state pointer
 */
int
well32_destroy(well32_h *state) {
	struct well32_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st, 0, sizeof(struct well32_s));
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
well32_rand(void *state) {
	struct well32_s *st;

	if (state == NULL)
		return 0;

	st = (struct well32_s *) state;
	if (st -> rand == NULL)
		return 0;

	return st -> rand(st);
}
