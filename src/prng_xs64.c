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
 * \file	prng_xs64.c
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
 * 64-bit xoshiro/xoroshiro pseudorandom number generator state
 */
struct xs64_s {
	uint64_t (*rand)(struct xs64_s *);
	/**<
	 * xoshiro/xoroshiro next random value function
	 */
	void (*jump)(struct xs64_s *);
	/**<
	 * xoshiro/xoroshiro "short" jump function
	 */
	void (*long_jump)(struct xs64_s *);
	/**<
	 * xoshiro/xoroshiro "long" jump function
	 */
	uint64_t state[16];
	/**<
	 * current state of the xoshiro/xoroshiro generator
	 */
	int p;
	/**<
	 * current "interval" used by the xoroshiro1024 generators
	 */
};

/* ===== 64-bit xoshiro/xoroshiro Support Functions ========================= */

static uint64_t
xoroshiro128pp_next(struct xs64_s *state) {
	uint64_t *s;
	uint64_t s0, s1, r;

	s = state -> state;

	s0 = s[0];
	s1 = s[1];

	r = rotl64(s0 + s1, 17) + s0;

	s1 ^= s0;
	s[0] = rotl64(s0, 49) ^ s1 ^ (s1 << 21);
	s[1] = rotl64(s1, 28);

	return r;
}

static void
xoroshiro128pp_jump(struct xs64_s *state) {
	const uint64_t JUMP[2] = {
	    UINT64_C(0x2BD7A6A6E99C2DDC),
	    UINT64_C(0x0992CCAF6A6FCA05)
	};
	uint64_t *s;
	uint64_t s0, s1;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0; i < 2; ++i) {
		for (b = 0; b < 64; ++b) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
			}
			xoroshiro128pp_next(state);
		}
	}
	
	s[0] = s0;
	s[1] = s1;

	return;
}

static void
xoroshiro128pp_long_jump(struct xs64_s *state) {
	const uint64_t LONG_JUMP[2] = {
	    UINT64_C(0x360FD5F2CF8D5D99),
	    UINT64_C(0x9C6E6877736C46E3)
	};
	uint64_t *s;
	uint64_t s0, s1;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0; i < 2; ++i) {
		for (b = 0; b < 64; ++b) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
			}
			xoroshiro128pp_next(state);
		}
	}
	
	s[0] = s0;
	s[1] = s1;

	return;
}

static uint64_t
xoroshiro128ss_next(struct xs64_s *state) {
	uint64_t *s;
	uint64_t s0, s1, r;

	s = state -> state;

	s0 = s[0];
	s1 = s[1];

	r = rotl64(s0 * 5, 7) * 9;

	s1 ^= s0;
	s[0] = rotl64(s0, 24) ^ s1 ^ (s1 << 16);
	s[1] = rotl64(s1, 37);

	return r;
}

static void
xoroshiro128ss_jump(struct xs64_s *state) {
	const uint64_t JUMP[2] = {
	    UINT64_C(0xDF900294D8F554A5),
	    UINT64_C(0x170865DF4B3201FC)
	};
	uint64_t *s;
	uint64_t s0, s1;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0; i < 2; ++i) {
		for (b = 0; b < 64; ++b) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
			}
			xoroshiro128ss_next(state);
		}
	}
	
	s[0] = s0;
	s[1] = s1;

	return;
}

static void
xoroshiro128ss_long_jump(struct xs64_s *state) {
	const uint64_t LONG_JUMP[2] = {
	    UINT64_C(0xD2A98B26625EEE7B),
	    UINT64_C(0xDDDF9B1090AA7AC1)
	};
	uint64_t *s;
	uint64_t s0, s1;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0; i < 2; ++i) {
		for (b = 0; b < 64; ++b) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
			}
			xoroshiro128ss_next(state);
		}
	}
	
	s[0] = s0;
	s[1] = s1;

	return;
}

static uint64_t
xoshiro256pp_next(struct xs64_s *state) {
	uint64_t *s;
	uint64_t r, t;

	s = state -> state;

	r = rotl64(s[0] + s[3], 23) + s[0];
	t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl64(s[3], 45);

	return r;
}

static void
xoshiro256pp_jump(struct xs64_s *state) {
	const uint64_t JUMP[4] = {
	    UINT64_C(0x180EC6D33CFD0ABA),
	    UINT64_C(0xD5A61266F0C9392C),
	    UINT64_C(0xA9582618E03FC9AA),
	    UINT64_C(0x39ABDC4529B1661C)
	};
	uint64_t *s;
	uint64_t s0, s1, s2, s3;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0, s2 = 0, s3 = 0; i < 4; ++i) {
		for (b = 0; b < 64; ++b) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			xoshiro256pp_next(state);
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;

	return;
}

static void
xoshiro256pp_long_jump(struct xs64_s *state) {
	const uint64_t LONG_JUMP[4] = {
	    UINT64_C(0x76E15D3EFEFDCBBF),
	    UINT64_C(0xC5004E441C522FB3),
	    UINT64_C(0x77710069854EE241),
	    UINT64_C(0x39109BB02ACBE635)
	};
	uint64_t *s;
	uint64_t s0, s1, s2, s3;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0, s2 = 0, s3 = 0; i < 4; ++i) {
		for (b = 0; b < 64; ++b) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			xoshiro256pp_next(state);
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;

	return;
}

static uint64_t
xoshiro256ss_next(struct xs64_s *state) {
	uint64_t *s;
	uint64_t r, t;

	s = state -> state;

	r = rotl64(s[1] * 5, 7) * 9;
	t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl64(s[3], 45);

	return r;
}

static void
xoshiro256ss_jump(struct xs64_s *state) {
	const uint64_t JUMP[4] = {
	    UINT64_C(0x180EC6D33CFD0ABA),
	    UINT64_C(0xD5A61266F0C9392C),
	    UINT64_C(0xA9582618E03FC9AA),
	    UINT64_C(0x39ABDC4529B1661C)
	};
	uint64_t *s;
	uint64_t s0, s1, s2, s3;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0, s2 = 0, s3 = 0; i < 4; ++i) {
		for (b = 0; b < 64; ++b) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			xoshiro256ss_next(state);
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;

	return;
}

static void
xoshiro256ss_long_jump(struct xs64_s *state) {
	const uint64_t LONG_JUMP[4] = {
	    UINT64_C(0x76E15D3EFEFDCBBF),
	    UINT64_C(0xC5004E441C522FB3),
	    UINT64_C(0x77710069854EE241),
	    UINT64_C(0x39109BB02ACBE635)
	};
	uint64_t *s;
	uint64_t s0, s1, s2, s3;
	int b, i;

	s = state -> state;

	for (i = 0, s0 = 0, s1 = 0, s2 = 0, s3 = 0; i < 4; ++i) {
		for (b = 0; b < 64; ++b) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= s[0];
				s1 ^= s[1];
				s2 ^= s[2];
				s3 ^= s[3];
			}
			xoshiro256ss_next(state);
		}
	}

	s[0] = s0;
	s[1] = s1;
	s[2] = s2;
	s[3] = s3;

	return;
}

static uint64_t
xoshiro512pp_next(struct xs64_s *state) {
	uint64_t *s;
	uint64_t r, t;

	s = state -> state;

	r = rotl64(s[0] + s[2], 17) + s[2];
	t = s[1] << 11;

	s[2] ^= s[0];
	s[5] ^= s[1];
	s[1] ^= s[2];
	s[7] ^= s[3];
	s[3] ^= s[4];
	s[4] ^= s[5];
	s[0] ^= s[6];
	s[6] ^= s[7];

	s[6] ^= t;

	s[7] = rotl64(s[7], 21);

	return r;
}

static void
xoshiro512pp_jump(struct xs64_s *state) {
	const uint64_t JUMP[8] = {
	    UINT64_C(0x33ED89B6E7A353F9),
	    UINT64_C(0x760083D7955323BE),
	    UINT64_C(0x2837F2FBB5F22FAE),
	    UINT64_C(0x4B8C5674D309511C),
	    UINT64_C(0xB11AC47A7BA28C25),
	    UINT64_C(0xF1BE7667092BCC1C),
	    UINT64_C(0x53851EFDB6DF0AAF),
	    UINT64_C(0x1EBBC8B23EAF25DB)
	};
	uint64_t *s;
	uint64_t t[8];
	int b, i, w;

	s = state -> state;

	memset(t, 0, 8 * sizeof(uint64_t));

	for (i = 0; i < 8; ++i) {
		for (b = 0; b < 64; ++b) {
			if (JUMP[i] & UINT64_C(1) << b) {
				for (w = 0; w < 8; ++w) {
					t[w] ^= s[w];
				}
			}
			xoshiro512pp_next(state);
		}
	}

	memcpy(s, t, 8 * sizeof(uint64_t));

	return;
}

static void
xoshiro512pp_long_jump(struct xs64_s *state) {
	const uint64_t LONG_JUMP[8] = {
	    UINT64_C(0x11467FEF8F921D28),
	    UINT64_C(0xA2A819F2E79C8EA8),
	    UINT64_C(0xA8299FC284B3959A),
	    UINT64_C(0xB4D347340CA63EE1),
	    UINT64_C(0x1CB0940BEDBFF6CE),
	    UINT64_C(0xD956C5C4FA1F8E17),
	    UINT64_C(0x915E38FD4EDA93BC),
	    UINT64_C(0x5B3CCDFA5D7DACA5)
	};
	uint64_t *s;
	uint64_t t[8];
	int b, i, w;

	s = state -> state;

	memset(t, 0, 8 * sizeof(uint64_t));

	for (i = 0; i < 8; ++i) {
		for (b = 0; b < 64; ++b) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				for (w = 0; w < 8; ++w) {
					t[w] ^= s[w];
				}
			}
			xoshiro512pp_next(state);
		}
	}

	memcpy(s, t, 8 * sizeof(uint64_t));

	return;
}

static uint64_t
xoshiro512ss_next(struct xs64_s *state) {
	uint64_t *s;
	uint64_t r, t;

	s = state -> state;

	r = rotl64(s[1] * 5, 7) * 9;
	t = s[1] << 11;

	s[2] ^= s[0];
	s[5] ^= s[1];
	s[1] ^= s[2];
	s[7] ^= s[3];
	s[3] ^= s[4];
	s[4] ^= s[5];
	s[0] ^= s[6];
	s[6] ^= s[7];

	s[6] ^= t;

	s[7] = rotl64(s[7], 21);

	return r;
}

static void
xoshiro512ss_jump(struct xs64_s *state) {
	const uint64_t JUMP[8] = {
	    UINT64_C(0x33ED89B6E7A353F9),
	    UINT64_C(0x760083D7955323BE),
	    UINT64_C(0x2837F2FBB5F22FAE),
	    UINT64_C(0x4B8C5674D309511C),
	    UINT64_C(0xB11AC47A7BA28C25),
	    UINT64_C(0xF1BE7667092BCC1C),
	    UINT64_C(0x53851EFDB6DF0AAF),
	    UINT64_C(0x1EBBC8B23EAF25DB)
	};
	uint64_t *s;
	uint64_t t[8];
	int b, i, w;

	s = state -> state;

	memset(t, 0, 8 * sizeof(uint64_t));

	for (i = 0; i < 8; ++i) {
		for (b = 0; b < 64; ++b) {
			if (JUMP[i] & UINT64_C(1) << b) {
				for (w = 0; w < 8; ++w) {
					t[w] ^= s[w];
				}
			}
			xoshiro512pp_next(state);
		}
	}

	memcpy(s, t, 8 * sizeof(uint64_t));

	return;
}

static void
xoshiro512ss_long_jump(struct xs64_s *state) {
	const uint64_t LONG_JUMP[8] = {
	    UINT64_C(0x11467FEF8F921D28),
	    UINT64_C(0xA2A819F2E79C8EA8),
	    UINT64_C(0xA8299FC284B3959A),
	    UINT64_C(0xB4D347340CA63EE1),
	    UINT64_C(0x1CB0940BEDBFF6CE),
	    UINT64_C(0xD956C5C4FA1F8E17),
	    UINT64_C(0x915E38FD4EDA93BC),
	    UINT64_C(0x5B3CCDFA5D7DACA5)
	};
	uint64_t *s;
	uint64_t t[8];
	int b, i, w;

	s = state -> state;

	memset(t, 0, 8 * sizeof(uint64_t));

	for (i = 0; i < 8; ++i) {
		for (b = 0; b < 64; ++b) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				for (w = 0; w < 8; ++w) {
					t[w] ^= s[w];
				}
			}
			xoshiro512pp_next(state);
		}
	}

	memcpy(s, t, 8 * sizeof(uint64_t));

	return;
}

static uint64_t
xoroshiro1024pp_next(struct xs64_s *state) {
	uint64_t *s;
	uint64_t s0, s15, r;
	int q;

	s = state -> state;
	q = (state -> p);
	
	s0 = s[state -> p = (state -> p + 1) & 15];
	s15 = s[q];

	r = rotl64(s0 + s15, 23) + s15;

	s15 ^= s0;
	s[q] = rotl64(s0, 25) ^ s15 ^ (s15 << 27);
	s[(state -> p)] = rotl64(s15, 36);

	return r;
}

static void
xoroshiro1024pp_jump(struct xs64_s *state) {
	const uint64_t JUMP[16] = {
	    UINT64_C(0x931197D8E3177F17),
	    UINT64_C(0xB59422E0B9138C5F),
	    UINT64_C(0xF06A6AFB49D668BB),
	    UINT64_C(0xACB8A6412C8A1401),
	    UINT64_C(0x12304EC85F0B3468),
	    UINT64_C(0xB7DFE7079209891E),
	    UINT64_C(0x405B7EEC77D9EB14),
	    UINT64_C(0x34EAD68280C44E4A),
	    UINT64_C(0xE0E4BA3E0AC9E366),
	    UINT64_C(0x8F46EDA8348905B7),
	    UINT64_C(0x328BF4DBAD90D6FF),
	    UINT64_C(0xC8FD6FB31C9EFFC3),
	    UINT64_C(0xE899D452D4B67652),
	    UINT64_C(0x45F387286ADE3205),
	    UINT64_C(0x03864F454A8920BD),
	    UINT64_C(0xA68FA28725B1B384)
	};
	uint64_t *s;
	uint64_t t[16];
	int b, i, w;
	
	s = state -> state;

	memset(t, 0, 16 * sizeof(uint64_t));
	for (i = 0; i < 16; ++i) {
		for (b = 0; b < 64; ++b) {
			if (JUMP[i] & UINT64_C(1) << b) {
				for (w = 0; w < 16; ++w) {
					t[w] ^= s[(w + (state -> p)) & 15];
				}
			}
			xoroshiro1024pp_next(state);
		}
	}

	for (i = 0; i < 16; ++i) {
		s[(i + (state -> p)) & 15] = t[i];
	}

	return;
}

static void
xoroshiro1024pp_long_jump(struct xs64_s *state) {
	const uint64_t LONG_JUMP[16] = {
	    UINT64_C(0x7374156360BBF00F),
	    UINT64_C(0x4630C2EFA3B3C1F6),
	    UINT64_C(0x6654183A892786B1),
	    UINT64_C(0x94F7BFCBFB0F1661),
	    UINT64_C(0x27D8243D3D13EB2D),
	    UINT64_C(0x9701730F3DFB300F),
	    UINT64_C(0x2F293BAAE6F604AD),
	    UINT64_C(0xA661831CB60CD8B6),
	    UINT64_C(0x68280C77D9FE008C),
	    UINT64_C(0x50554160F5BA9459),
	    UINT64_C(0x2FC20B17EC7B2A9A),
	    UINT64_C(0x49189BBDC8EC9F8F),
	    UINT64_C(0x92A65BCA41852CC1),
	    UINT64_C(0xF46820DD0509C12A),
	    UINT64_C(0x52B00C35FBF92185),
	    UINT64_C(0x1E5B3B7F589E03C1)
	};
	uint64_t *s;
	uint64_t t[16];
	int b, i, w;
	
	s = state -> state;

	memset(t, 0, 16 * sizeof(uint64_t));
	for (i = 0; i < 16; ++i) {
		for (b = 0; b < 64; ++b) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				for (w = 0; w < 16; ++w) {
					t[w] ^= s[(w + (state -> p)) & 15];
				}
			}
			xoroshiro1024pp_next(state);
		}
	}

	for (i = 0; i < 16; ++i) {
		s[(i + (state -> p)) & 15] = t[i];
	}

	return;
}

static uint64_t
xoroshiro1024ss_next(struct xs64_s *state) {
	uint64_t *s;
	uint64_t s0, s15, r;
	int q;

	s = state -> state;
	q = (state -> p);
	
	s0 = s[state -> p = (state -> p + 1) & 15];
	s15 = s[q];

	r = rotl64(s0 * 5, 7) * 9;

	s15 ^= s0;
	s[q] = rotl64(s0, 25) ^ s15 ^ (s15 << 27);
	s[(state -> p)] = rotl64(s15, 36);

	return r;
}

static void
xoroshiro1024ss_jump(struct xs64_s *state) {
	const uint64_t JUMP[16] = {
	    UINT64_C(0x931197D8E3177F17),
	    UINT64_C(0xB59422E0B9138C5F),
	    UINT64_C(0xF06A6AFB49D668BB),
	    UINT64_C(0xACB8A6412C8A1401),
	    UINT64_C(0x12304EC85F0B3468),
	    UINT64_C(0xB7DFE7079209891E),
	    UINT64_C(0x405B7EEC77D9EB14),
	    UINT64_C(0x34EAD68280C44E4A),
	    UINT64_C(0xE0E4BA3E0AC9E366),
	    UINT64_C(0x8F46EDA8348905B7),
	    UINT64_C(0x328BF4DBAD90D6FF),
	    UINT64_C(0xC8FD6FB31C9EFFC3),
	    UINT64_C(0xE899D452D4B67652),
	    UINT64_C(0x45F387286ADE3205),
	    UINT64_C(0x03864F454A8920BD),
	    UINT64_C(0xA68FA28725B1B384)
	};
	uint64_t *s;
	uint64_t t[16];
	int b, i, w;

	s = state -> state;

	memset(t, 0, 16 * sizeof(uint64_t));
	for (i = 0; i < 16; ++i) {
		for (b = 0; b < 64; ++b) {
			if (JUMP[i] & UINT64_C(1) << b) {
				for (w = 0; w < 16; ++w) {
					t[w] ^= s[(w + (state -> p)) & 15];
				}
			}
			xoroshiro1024ss_next(state);
		}
	}

	for (i = 0; i < 16; ++i) {
		s[(i + (state -> p)) & 15] = t[i];
	}

	return;
}

static void
xoroshiro1024ss_long_jump(struct xs64_s *state) {
	const uint64_t LONG_JUMP[16] = {
	    UINT64_C(0x7374156360BBF00F),
	    UINT64_C(0x4630C2EFA3B3C1F6),
	    UINT64_C(0x6654183A892786B1),
	    UINT64_C(0x94F7BFCBFB0F1661),
	    UINT64_C(0x27D8243D3D13EB2D),
	    UINT64_C(0x9701730F3DFB300F),
	    UINT64_C(0x2F293BAAE6F604AD),
	    UINT64_C(0xA661831CB60CD8B6),
	    UINT64_C(0x68280C77D9FE008C),
	    UINT64_C(0x50554160F5BA9459),
	    UINT64_C(0x2FC20B17EC7B2A9A),
	    UINT64_C(0x49189BBDC8EC9F8F),
	    UINT64_C(0x92A65BCA41852CC1),
	    UINT64_C(0xF46820DD0509C12A),
	    UINT64_C(0x52B00C35FBF92185),
	    UINT64_C(0x1E5B3B7F589E03C1)
	};
	uint64_t *s;
	uint64_t t[16];
	int b, i, w;
	
	s = state -> state;

	memset(t, 0, 16 * sizeof(uint64_t));
	for (i = 0; i < 16; ++i) {
		for (b = 0; b < 64; ++b) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				for (w = 0; w < 16; ++w) {
					t[w] ^= s[(w + (state -> p)) & 15];
				}
			}
			xoroshiro1024ss_next(state);
		}
	}

	for (i = 0; i < 16; ++i) {
		s[(i + (state -> p)) & 15] = t[i];
	}

	return;
}

/* ===== 64-bit xoshiro/xoroshiro Public Interface ========================== */

/**
 * \brief 64-bit xoshiro/xoroshiro seed sizing function
 *
 * This function returns the size of the key for the 64-bit xoshiro/xoroshiro
 * pseudorandom number generator algorithm specified.
 *
 *
 *
 * \return length of key/state in 32-bit words; or 0 if error
 *
 * \param [in] type  xoshiro/xoroshiro algorithm
 */
size_t
xs64_state_size(enum XS64_TYPE type) {
	size_t length;

	switch (type) {
	case XS64_XOROSHIRO128PP:
	case XS64_XOROSHIRO128SS:
		length = 2;
		break;
	case XS64_XOSHIRO256PP:
	case XS64_XOSHIRO256SS:
		length = 4;
		break;
	case XS64_XOSHIRO512PP:
	case XS64_XOSHIRO512SS:
		length = 8;
		break;
	case XS64_XOROSHIRO1024PP:
	case XS64_XOROSHIRO1024SS:
		length = 16;
		break;
	case XS64__ERROR:
	default:
		length = 0;
		break;
	}

	return length;
}

/**
 * \brief 64-bit xoshiro/xoroshiro state initializer function
 *
 * This function initializes the state for a 64-bit xoshiro/xoroshiro
 * pseudorandom number generator. The different 64-bit-output versions of
 * xoshiro/xoroshiro have been implemented, thus the initializer has a parameter
 * to select the algorithm to be used. Given that the different versions of the
 * pseudorandom number generator require different lengths of seed, a helper
 * function is provided that returns the required seed length.
 *
 * \cite blackman2021
 * \ref xs64_state_size
 * \ref xs16_init
 * \ref xs32_init
 *
 *
 *
 * \return 64-bit xoshiro/xoroshiro PRNG state if success, NULL if error
 *
 * \param [in] type  xoshiro/xoroshiro algorithm
 * \param [in] seed  PRNG seed
 */
xs64_h
xs64_init(enum XS64_TYPE type, const uint64_t *seed) {
	struct xs64_s *st;

	if (seed == NULL)
		return NULL;

	switch (type) {
	case XS64_XOROSHIRO128PP:
	case XS64_XOROSHIRO128SS:
	case XS64_XOSHIRO256PP:
	case XS64_XOSHIRO256SS:
	case XS64_XOSHIRO512PP:
	case XS64_XOSHIRO512SS:
	case XS64_XOROSHIRO1024PP:
	case XS64_XOROSHIRO1024SS:
		/* All is good, proceed. */
		break;
	case XS64__ERROR:
	default:
		return NULL;
		break;
	}

	st = (struct xs64_s *) malloc(sizeof(struct xs64_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct xs64_s));

	memcpy(st -> state, seed, xs64_state_size(type) * sizeof(uint64_t));
	switch (type) {
	case XS64_XOROSHIRO128PP:
		st -> rand = xoroshiro128pp_next;
		st -> jump = xoroshiro128pp_jump;
		st -> long_jump = xoroshiro128pp_long_jump;
		break;
	case XS64_XOROSHIRO128SS:
		st -> rand = xoroshiro128ss_next;
		st -> jump = xoroshiro128ss_jump;
		st -> long_jump = xoroshiro128ss_long_jump;
		break;
	case XS64_XOSHIRO256PP:
		st -> rand = xoshiro256pp_next;
		st -> jump = xoshiro256pp_jump;
		st -> long_jump = xoshiro256pp_long_jump;
		break;
	case XS64_XOSHIRO256SS:
		st -> rand = xoshiro256ss_next;
		st -> jump = xoshiro256ss_jump;
		st -> long_jump = xoshiro256ss_long_jump;
		break;
	case XS64_XOSHIRO512PP:
		st -> rand = xoshiro512pp_next;
		st -> jump = xoshiro512pp_jump;
		st -> long_jump = xoshiro512pp_long_jump;
		break;
	case XS64_XOSHIRO512SS:
		st -> rand = xoshiro512ss_next;
		st -> jump = xoshiro512ss_jump;
		st -> long_jump = xoshiro512ss_long_jump;
		break;
	case XS64_XOROSHIRO1024PP:
		st -> rand = xoroshiro1024pp_next;
		st -> jump = xoroshiro1024pp_jump;
		st -> long_jump = xoroshiro1024pp_long_jump;
		break;
	case XS64_XOROSHIRO1024SS:
		st -> rand = xoroshiro1024ss_next;
		st -> jump = xoroshiro1024ss_jump;
		st -> long_jump = xoroshiro1024ss_long_jump;
		break;
	case XS64__ERROR:
	default:
		st -> rand = NULL;
		st -> jump = NULL;
		st -> long_jump = NULL;
		break;
	}
	st -> p = 0;

	return st;
}

/**
 * \brief 64-bit xoshiro/xoroshiro state destructor function
 *
 * This function destroys a 64-bit xoshiro/xoroshiro pseudorandom number
 * generator state that is passed to it.
 *
 * \ref xs16_destroy
 * \ref xs32_destroy
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  64-bit xoshiro/xoroshiro PRNG state pointer
 */
int
xs64_destroy(xs64_h *state) {
	struct xs64_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st, 0, sizeof(struct xs64_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 * \brief 64-bit xoshiro/xoroshiro pseudorandom function
 *
 * This function calls the \c rand() equivalent function for the 64-bit
 * xoshiro/xoroshiro pseudorandom number generator that was initialized by in
 * the initialization function.
 *
 * \ref xs64_init
 * \ref xs16_rand
 * \ref xs32_rand
 *
 *
 *
 * \return 64-bit pseudorandom number
 *
 * \param [in,out] state  64-bit xoshiro/xoroshiro PRNG state
 */
uint64_t
xs64_rand(void *state) {
	struct xs64_s *st;
	st = (struct xs64_s *) state;
	return st -> rand(st);
}

/**
 * \brief 64-bit xoshiro/xoroshiro stream "jump" function
 *
 * This function calls, if it exists, the "jump" function for the selected
 * xoshiro/xoroshiro pseudorandom number generator. Reference should be made to
 * the individual jump function impementatations for the specific increment the
 * PRNG will be advanced.
 *
 * \ref xs32_jump
 * \ref xoroshiro128pp_jump
 * \ref xoroshiro128ss_jump
 * \ref xoshiro256pp_jump
 * \ref xoshiro256ss_jump
 * \ref xoshiro512pp_jump
 * \ref xoshiro512ss_jump
 * \ref xoroshiro1024pp_jump
 * \ref xoroshiro1024ss_jump
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  64-bit xoshiro/xoroshiro PRNG state
 */
int
xs64_jump(xs64_h state) {
	if (state -> jump != NULL) {
		state -> jump(state);
		return 0;
	} else {
		return 1;
	}
}

/**
 * \brief 64-bit xoshiro/xoroshiro stream "long jump" function
 *
 * This function calls, if it exists, the "long jump" function for the selected
 * xoshiro/xoroshiro pseudorandom number generator. Reference should be made to
 * the individual long jump function implementations for the specific increment
 * the PRNG will be advanced.
 *
 * \ref xs32_long_jump
 * \ref xoroshiro128pp_long_jump
 * \ref xoroshiro128ss_long_jump
 * \ref xoshiro256pp_long_jump
 * \ref xoshiro256ss_long_jump
 * \ref xoshiro512pp_long_jump
 * \ref xoshiro512ss_long_jump
 * \ref xoroshiro1024pp_long_jump
 * \ref xoroshiro1024ss_long_jump
 *
 *
 *
 * \return 0 if success, 1 if failure
 *
 * \param [in,out] state  64-bit xoshiro/xoroshiro PRNG state
 */
int
xs64_long_jump(xs64_h state) {
	if (state -> long_jump != NULL) {
		state -> long_jump(state);
		return 0;
	} else {
		return 1;
	}
}