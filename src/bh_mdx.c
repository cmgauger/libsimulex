/*
 * Copyright (c) 2025 Christian Gauger-Cosgrove
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
 * \file	bh_mdx.c
 * \copyright	MIT
 * \date	2025
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.9
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/bit_hash.h>

#define	ROTL(X,Y,Z)	(((X) << (Y)) | ((X) >> (Z - (Y))))
#define MD4_F(X,Y,Z)	(((X) & (Y)) | ((~X) & (Z)))
#define	MD4_G(X,Y,Z)	(((X) & (Y)) | ((X) & (Z)) | ((Y) & (Z)))
#define	MD4_H(X,Y,Z)	((X) ^ (Y) ^ (Z))
#define MD4_FF(A,B,C,D,X,S) do {                                       \
	(A) += MD4_F((B), (C), (D)) + (X);                             \
	(A)  = ROTL((A), (S), 32);                                     \
} while (0)
#define	MD4_GG(A,B,C,D,X,S) do {                                       \
	(A) += MD4_G((B), (C), (D)) + (X) + UINT32_C(0x5A827999);      \
	(A)  = ROTL((A), (S), 32);                                     \
} while (0)
#define	MD4_HH(A,B,C,D,X,S) do {                                       \
	(A) += MD4_H((B), (C), (D)) + (X) + UINT32_C(0x6ED9EBA1);      \
	(A)  = ROTL((A), (S), 32);                                     \
} while (0)
#define	MD5_F(X,Y,Z)	(((X) & (Y)) | ((~X) & (Z)))
#define	MD5_G(X,Y,Z)	(((X) & (Z)) | ((Y) & (~Z)))
#define	MD5_H(X,Y,Z)	((X) ^ (Y) ^ (Z))
#define	MD5_I(X,Y,Z)	((Y) ^ ((X) | (~Z)))
#define MD5_FF(A,B,C,D,X,S,AC) do {                                    \
	(A) += MD5_F((B), (C), (D)) + (X) + UINT32_C(AC);              \
	(A)  = ROTL((A), (S), 32);                                     \
	(A) += (B);                                                    \
} while (0)
#define	MD5_GG(A,B,C,D,X,S,AC) do {                                    \
	(A) += MD5_G((B), (C), (D)) + (X) + UINT32_C(AC);              \
	(A)  = ROTL((A), (S), 32);                                     \
	(A) += (B);                                                    \
} while (0)
#define	MD5_HH(A,B,C,D,X,S,AC) do {                                    \
	(A) += MD5_H((B), (C), (D)) + (X) + UINT32_C(AC);              \
	(A)  = ROTL((A), (S), 32);                                     \
	(A) += (B);                                                    \
} while (0)
#define	MD5_II(A,B,C,D,X,S,AC) do {                                    \
	(A) += MD5_I((B), (C), (D)) + (X) + UINT32_C(AC);              \
	(A)  = ROTL((A), (S), 32);                                     \
	(A) += (B);                                                    \
} while (0)
#define	MDX_DIGEST	16
#define	MD2_BLOCK	16
#define	MD2_ROUNDS	18
#define	MD2_BUFFER	48
#define	MD4_CBLOCK	64
#define	MD4_LBLOCK	16
#define	MD4_S11		 3
#define	MD4_S12		 7
#define	MD4_S13		11
#define	MD4_S14		19
#define	MD4_S21		 3
#define	MD4_S22		 5
#define	MD4_S23		 9
#define	MD4_S24		13
#define	MD4_S31		 3
#define	MD4_S32		 9
#define	MD4_S33		11
#define	MD4_S34		15
#define	MD5_CBLOCK	64
#define	MD5_LBLOCK	16
#define	MD5_S11		 7
#define	MD5_S12		12
#define	MD5_S13		17
#define	MD5_S14		22
#define	MD5_S21		 5
#define	MD5_S22		 9
#define	MD5_S23		14
#define	MD5_S24		20
#define	MD5_S31		 4
#define	MD5_S32		11
#define	MD5_S33		16
#define	MD5_S34		23
#define	MD5_S41		 6
#define	MD5_S42		10
#define	MD5_S43		15
#define	MD5_S44		21


struct md2_s {
	uint8_t data[MD2_BLOCK];
	uint8_t cksum[MD2_BLOCK];
	uint8_t state[MD2_BLOCK];
	size_t num;
};

struct md4_s {
	uint32_t data[MD4_LBLOCK];
	uint32_t a, b, c, d;
	uint32_t count[2];
	size_t num;
};

struct md5_s {
	uint32_t data[MD5_LBLOCK];
	uint32_t a, b, c, d;
	uint32_t count[2];
	size_t num;
};

struct mdx_s {
	union {
		void (*md2)(struct md2_s *);
		void (*md4)(struct md4_s *);
		void (*md5)(struct md5_s *);
	} i;
	union {
		void (*md2)(struct md2_s *, const void *, size_t);
		void (*md4)(struct md4_s *, const void *, size_t);
		void (*md5)(struct md5_s *, const void *, size_t);
	} u;
	union {
		void (*md2)(struct md2_s *, uint8_t *);
		void (*md4)(struct md4_s *, uint8_t *);
		void (*md5)(struct md5_s *, uint8_t *);
	} f;
	union {
		struct md2_s *md2;
		struct md4_s *md4;
		struct md5_s *md5;
	} context;
	enum MDX_TYPE type;
};

static const uint8_t MD2_S[256] = {
	0x29, 0x2E, 0x43, 0xC9, 0xA2, 0xD8, 0x7C, 0x01,
	0x3D, 0x36, 0x54, 0xA1, 0xEC, 0xF0, 0x06, 0x13,
	0x62, 0xA7, 0x05, 0xF3, 0xC0, 0xC7, 0x73, 0x8C,
	0x98, 0x93, 0x2B, 0xD9, 0xBC, 0x4C, 0x82, 0xCA,
	0x1E, 0x9B, 0x57, 0x3C, 0xFD, 0xD4, 0xE0, 0x16,
	0x67, 0x42, 0x6F, 0x18, 0x8A, 0x17, 0xE5, 0x12,
	0xBE, 0x4E, 0xC4, 0xD6, 0xDA, 0x9E, 0xDE, 0x49,
	0xA0, 0xFB, 0xF5, 0x8E, 0xBB, 0x2F, 0xEE, 0x7A,
	0xA9, 0x68, 0x79, 0x91, 0x15, 0xB2, 0x07, 0x3F,
	0x94, 0xC2, 0x10, 0x89, 0x0B, 0x22, 0x5F, 0x21,
	0x80, 0x7F, 0x5D, 0x9A, 0x5A, 0x90, 0x32, 0x27,
	0x35, 0x3E, 0xCC, 0xE7, 0xBF, 0xF7, 0x97, 0x03,
	0xFF, 0x19, 0x30, 0xB3, 0x48, 0xA5, 0xB5, 0xD1,
	0xD7, 0x5E, 0x92, 0x2A, 0xAC, 0x56, 0xAA, 0xC6,
	0x4F, 0xB8, 0x38, 0xD2, 0x96, 0xA4, 0x7D, 0xB6,
	0x76, 0xFC, 0x6B, 0xE2, 0x9C, 0x74, 0x04, 0xF1,
	0x45, 0x9D, 0x70, 0x59, 0x64, 0x71, 0x87, 0x20,
	0x86, 0x5B, 0xCF, 0x65, 0xE6, 0x2D, 0xA8, 0x02,
	0x1B, 0x60, 0x25, 0xAD, 0xAE, 0xB0, 0xB9, 0xF6,
	0x1C, 0x46, 0x61, 0x69, 0x34, 0x40, 0x7E, 0x0F,
	0x55, 0x47, 0xA3, 0x23, 0xDD, 0x51, 0xAF, 0x3A,
	0xC3, 0x5C, 0xF9, 0xCE, 0xBA, 0xC5, 0xEA, 0x26,
	0x2C, 0x53, 0x0D, 0x6E, 0x85, 0x28, 0x84, 0x09,
	0xD3, 0xDF, 0xCD, 0xF4, 0x41, 0x81, 0x4D, 0x52,
	0x6A, 0xDC, 0x37, 0xC8, 0x6C, 0xC1, 0xAB, 0xFA,
	0x24, 0xE1, 0x7B, 0x08, 0x0C, 0xBD, 0xB1, 0x4A,
	0x78, 0x88, 0x95, 0x8B, 0xE3, 0x63, 0xE8, 0x6D,
	0xE9, 0xCB, 0xD5, 0xFE, 0x3B, 0x00, 0x1D, 0x39,
	0xF2, 0xEF, 0xB7, 0x0E, 0x66, 0x58, 0xD0, 0xE4,
	0xA6, 0x77, 0x72, 0xF8, 0xEB, 0x75, 0x4B, 0x0A,
	0x31, 0x44, 0x50, 0xB4, 0x8F, 0xED, 0x1F, 0x1A,
	0xDB, 0x99, 0x8D, 0x33, 0x9F, 0x11, 0x83, 0x14
};

/* ==== MD2 (RFC 1319) ====================================================== */
/* <https://datatracker.ietf.org/doc/html/rfc1319> */
static void
md2_block(
    struct md2_s *context,
    const uint8_t *data
) {
	uint8_t state[MD2_BUFFER];
	uint8_t t, *sp1, *sp2;
	int i, j, k;

	sp1 = context -> state;
	sp2 = context -> cksum;
	j = sp2[MD2_BLOCK - 1];
	for (i = 0; i < MD2_BLOCK; ++i) {
		state[i] = sp1[i];
		state[i + MD2_BLOCK] = t = data[i];
		state[i + MD2_BLOCK * 2] = (t ^ sp1[i]);
		j = sp2[i] ^= MD2_S[t ^ j];
	}

	for (i = 0, t = 0; i < MD2_ROUNDS; ++i) {
		for (j = 0; j < MD2_BUFFER; j += 8) {
			for (k = 0; k < 8; ++k)
				t = state[j + k] ^= MD2_S[t];
		}
		t = (t + (uint8_t) i) & 0xFF;
	}

	memcpy(sp1, state, MD2_BLOCK * sizeof(uint8_t));

	return;
}

static void
md2_init(
    struct md2_s *context
) {
	memset(context -> state, 0, MD2_BLOCK * sizeof(uint8_t));
	memset(context -> cksum, 0, MD2_BLOCK * sizeof(uint8_t));
	memset(context -> data, 0, MD2_BLOCK * sizeof(uint8_t));
	context -> num = 0;

	return;
}

static void
md2_update(
    struct md2_s *context,
    const void *data,
    size_t length
) {
	uint8_t *p, *b;

	if (length == 0)
		return;

	p = context -> data;
	b = (uint8_t *) data;

	if (context -> num != 0) {
		if ((context -> num + length) >= MD2_BLOCK) {
			memcpy(&(p[context -> num]), b, (MD2_BLOCK - context ->
			    num) * sizeof(uint8_t));
			md2_block(context, b);
			b += (MD2_BLOCK - context -> num);
			length -= (MD2_BLOCK - context -> num);
			context -> num = 0;
		} else {
			memcpy(&(p[context -> num]), b, length *
			    sizeof(uint8_t));
			context -> num += length;

			return;
		}
	}

	while (length >= MD2_BLOCK) {
		md2_block(context, b);
		b += MD2_BLOCK;
		length -= MD2_BLOCK;
	}

	memcpy(p, b, length * sizeof(uint8_t));
	context -> num = length;

	return;
}

static void
md2_final(
    struct md2_s *context,
    uint8_t *digest
) {
	uint8_t *cp, *p1, *p2;
	size_t i, v;

	cp = context -> data;
	p1 = context -> state;
	p2 = context -> cksum;

	v = MD2_BLOCK - context -> num;
	for (i = context -> num; i < MD2_BLOCK; ++i)
		cp[i] = (uint8_t) v;
	md2_block(context, cp);

	for (i = 0; i < MD2_BLOCK; ++i)
		cp[i] = p2[i];
	md2_block(context, cp);

	for (i = 0; i < MDX_DIGEST; ++i)
		digest[i] = (p1[i] & 0xFF);

	memset(context, 0, sizeof(struct md2_s));

	return;
}

/* ==== MD4 (RFC 1320) ====================================================== */
/* <https://datatracker.ietf.org/doc/html/rfc1320> */
static void
md4_block(
    struct md4_s *context,
    void *in,
    size_t num
) {
	uint8_t *p;
	uint32_t a, b, c, d;
	uint32_t x[MD4_LBLOCK];
	int i;

	p = (uint8_t *) in;

	while (num-- > 0) {
		a = context -> a;
		b = context -> b;
		c = context -> c;
		d = context -> d;

		for (i = 0; i < MD4_LBLOCK; ++i) {
			x[i] = ((uint32_t) p[(4 * i) + 3]) << 24 | 
			       ((uint32_t) p[(4 * i) + 2]) << 16 |
			       ((uint32_t) p[(4 * i) + 1]) <<  8 |
			       ((uint32_t) p[(4 * i)    ]);
		}
		p += MD4_CBLOCK;
		
		MD4_FF(a, b, c, d, x[ 0], MD4_S11);
		MD4_FF(d, a, b, c, x[ 1], MD4_S12);
		MD4_FF(c, d, a, b, x[ 2], MD4_S13);
		MD4_FF(b, c, d, a, x[ 3], MD4_S14);
		MD4_FF(a, b, c, d, x[ 4], MD4_S11);
		MD4_FF(d, a, b, c, x[ 5], MD4_S12);
		MD4_FF(c, d, a, b, x[ 6], MD4_S13);
		MD4_FF(b, c, d, a, x[ 7], MD4_S14);
		MD4_FF(a, b, c, d, x[ 8], MD4_S11);
		MD4_FF(d, a, b, c, x[ 9], MD4_S12);
		MD4_FF(c, d, a, b, x[10], MD4_S13);
		MD4_FF(b, c, d, a, x[11], MD4_S14);
		MD4_FF(a, b, c, d, x[12], MD4_S11);
		MD4_FF(d, a, b, c, x[13], MD4_S12);
		MD4_FF(c, d, a, b, x[14], MD4_S13);
		MD4_FF(b, c, d, a, x[15], MD4_S14);

		MD4_GG(a, b, c, d, x[ 0], MD4_S21);
		MD4_GG(d, a, b, c, x[ 4], MD4_S22);
		MD4_GG(c, d, a, b, x[ 8], MD4_S23);
		MD4_GG(b, c, d, a, x[12], MD4_S24);
		MD4_GG(a, b, c, d, x[ 1], MD4_S21);
		MD4_GG(d, a, b, c, x[ 5], MD4_S22);
		MD4_GG(c, d, a, b, x[ 9], MD4_S23);
		MD4_GG(b, c, d, a, x[13], MD4_S24);
		MD4_GG(a, b, c, d, x[ 2], MD4_S21);
		MD4_GG(d, a, b, c, x[ 6], MD4_S22);
		MD4_GG(c, d, a, b, x[10], MD4_S23);
		MD4_GG(b, c, d, a, x[14], MD4_S24);
		MD4_GG(a, b, c, d, x[ 3], MD4_S21);
		MD4_GG(d, a, b, c, x[ 7], MD4_S22);
		MD4_GG(c, d, a, b, x[11], MD4_S23);
		MD4_GG(b, c, d, a, x[15], MD4_S24);

		MD4_HH(a, b, c, d, x[ 0], MD4_S31);
		MD4_HH(d, a, b, c, x[ 8], MD4_S32);
		MD4_HH(c, d, a, b, x[ 4], MD4_S33);
		MD4_HH(b, c, d, a, x[12], MD4_S34);
		MD4_HH(a, b, c, d, x[ 2], MD4_S31);
		MD4_HH(d, a, b, c, x[10], MD4_S32);
		MD4_HH(c, d, a, b, x[ 6], MD4_S33);
		MD4_HH(b, c, d, a, x[14], MD4_S34);
		MD4_HH(a, b, c, d, x[ 1], MD4_S31);
		MD4_HH(d, a, b, c, x[ 9], MD4_S32);
		MD4_HH(c, d, a, b, x[ 5], MD4_S33);
		MD4_HH(b, c, d, a, x[13], MD4_S34);
		MD4_HH(a, b, c, d, x[ 3], MD4_S31);
		MD4_HH(d, a, b, c, x[11], MD4_S32);
		MD4_HH(c, d, a, b, x[ 7], MD4_S33);
		MD4_HH(b, c, d, a, x[15], MD4_S34);

		context -> a += a;
		context -> b += b;
		context -> c += c;
		context -> d += d;
	}

	return;
}

static void
md4_init(
    struct md4_s *context
) {
	memset(context, 0, sizeof(struct md4_s));

	context -> a = UINT32_C(0x67452301);
	context -> b = UINT32_C(0xEFCDAB89);
	context -> c = UINT32_C(0x98BADCFE);
	context -> d = UINT32_C(0x10325476);

	return;
}

static void
md4_update(
    struct md4_s *context,
    const void *data,
    size_t length
) {
	uint8_t *b, *p;
	uint32_t l;
	size_t n;

	if (length == 0)
		return;

	b = (uint8_t *) data;

	l = (context -> count[0] + (((uint32_t) length) << 3)) & 0xFFFFFFFFU;
	if (l < context -> count[0])
		context -> count[1]++;
	context -> count[1] += (uint32_t) (length >> 29);
	context -> count[0]  = l;

	n = context -> num;
	if (n != 0) {
		p = (uint8_t *) context -> data;

		if (length >= MD4_CBLOCK || length + n >= MD4_CBLOCK) {
			memcpy(p + n, b, (MD4_CBLOCK - n) * sizeof(uint8_t));
			md4_block(context, p, 1);
			n = MD4_CBLOCK - n;
			b += n;
			length -= n;
			context -> num = 0;
			memset(p, 0, MD4_CBLOCK * sizeof(uint8_t));
		} else {
			memcpy(p + n, b, length * sizeof(uint8_t));
			context -> num += (unsigned int) length;

			return;
		}
	}

	n = length / MD4_CBLOCK;
	if (n > 0) {
		md4_block(context, b, n);
		n *= MD4_CBLOCK;
		b += n;
		length -= n;
	}

	if (length != 0) {
		p = (uint8_t *) context -> data;
		context -> num = (unsigned int) length;
		memcpy(p, b, length * sizeof(uint8_t));
	}

	return;
}

static void
md4_final(
    struct md4_s *context,
    uint8_t *digest
) {
	uint8_t *p;
	size_t n;

	p = (uint8_t *) context -> data;
	n = context -> num;

	p[n] = 0x80;
	n++;

	if (n > (MD4_CBLOCK - 8)) {
		memset(p + n, 0, (MD4_CBLOCK - n) * sizeof(uint8_t));
		n = 0;
		md4_block(context, p, 1);
	}

	memset(p + n, 0, (MD4_CBLOCK - 8 - n) * sizeof(uint8_t));
	p[56] = (uint8_t) (context -> count[0]      ) & 0xFF;
	p[57] = (uint8_t) (context -> count[0] >>  8) & 0xFF;
	p[58] = (uint8_t) (context -> count[0] >> 16) & 0xFF;
	p[59] = (uint8_t) (context -> count[0] >> 24) & 0xFF;
	p[60] = (uint8_t) (context -> count[1]      ) & 0xFF;
	p[61] = (uint8_t) (context -> count[1] >>  8) & 0xFF;
	p[62] = (uint8_t) (context -> count[1] >> 16) & 0xFF;
	p[63] = (uint8_t) (context -> count[1] >> 24) & 0xFF;

	md4_block(context, p, 1);
	context -> num = 0;
	memset(p, 0, MD4_CBLOCK * sizeof(uint8_t));

	digest[ 0] = (uint8_t) (context -> a      ) & 0xFF;
	digest[ 1] = (uint8_t) (context -> a >>  8) & 0xFF;
	digest[ 2] = (uint8_t) (context -> a >> 16) & 0xFF;
	digest[ 3] = (uint8_t) (context -> a >> 24) & 0xFF;
	digest[ 4] = (uint8_t) (context -> b      ) & 0xFF;
	digest[ 5] = (uint8_t) (context -> b >>  8) & 0xFF;
	digest[ 6] = (uint8_t) (context -> b >> 16) & 0xFF;
	digest[ 7] = (uint8_t) (context -> b >> 24) & 0xFF;
	digest[ 8] = (uint8_t) (context -> c      ) & 0xFF;
	digest[ 9] = (uint8_t) (context -> c >>  8) & 0xFF;
	digest[10] = (uint8_t) (context -> c >> 16) & 0xFF;
	digest[11] = (uint8_t) (context -> c >> 24) & 0xFF;
	digest[12] = (uint8_t) (context -> d      ) & 0xFF;
	digest[13] = (uint8_t) (context -> d >>  8) & 0xFF;
	digest[14] = (uint8_t) (context -> d >> 16) & 0xFF;
	digest[15] = (uint8_t) (context -> d >> 24) & 0xFF;

	memset(context, 0, sizeof(struct md4_s));

	return;
}

/* ==== MD5 (RFC 1321) ====================================================== */
/* <https://datatracker.ietf.org/doc/html/rfc1321> */
static void
md5_block(
    struct md5_s *context,
    void *in,
    size_t num
) {
	uint8_t *p;
	uint32_t a, b, c, d;
	uint32_t x[MD5_LBLOCK];
	int i;

	p = (uint8_t *) in;

	while (num-- > 0) {
		a = context -> a;
		b = context -> b;
		c = context -> c;
		d = context -> d;

		for (i = 0; i < MD5_LBLOCK; ++i) {
			x[i] = ((uint32_t) p[(4 * i) + 3]) << 24 | 
			       ((uint32_t) p[(4 * i) + 2]) << 16 |
			       ((uint32_t) p[(4 * i) + 1]) <<  8 |
			       ((uint32_t) p[(4 * i)    ]);
		}
		p += MD5_CBLOCK;
		
		MD5_FF(a, b, c, d, x[ 0], MD5_S11, 0xD76AA478);
		MD5_FF(d, a, b, c, x[ 1], MD5_S12, 0xE8C7B756);
		MD5_FF(c, d, a, b, x[ 2], MD5_S13, 0x242070DB);
		MD5_FF(b, c, d, a, x[ 3], MD5_S14, 0xC1BDCEEE);
		MD5_FF(a, b, c, d, x[ 4], MD5_S11, 0xF57C0FAF);
		MD5_FF(d, a, b, c, x[ 5], MD5_S12, 0x4787C62A);
		MD5_FF(c, d, a, b, x[ 6], MD5_S13, 0xA8304613);
		MD5_FF(b, c, d, a, x[ 7], MD5_S14, 0xFD469501);
		MD5_FF(a, b, c, d, x[ 8], MD5_S11, 0x698098D8);
		MD5_FF(d, a, b, c, x[ 9], MD5_S12, 0x8B44F7AF);
		MD5_FF(c, d, a, b, x[10], MD5_S13, 0xFFFF5BB1);
		MD5_FF(b, c, d, a, x[11], MD5_S14, 0x895CD7BE);
		MD5_FF(a, b, c, d, x[12], MD5_S11, 0x6B901122);
		MD5_FF(d, a, b, c, x[13], MD5_S12, 0xFD987193);
		MD5_FF(c, d, a, b, x[14], MD5_S13, 0xA679438E);
		MD5_FF(b, c, d, a, x[15], MD5_S14, 0x49B40821);

		MD5_GG(a, b, c, d, x[ 1], MD5_S21, 0xF61E2562);
		MD5_GG(d, a, b, c, x[ 6], MD5_S22, 0xC040B340);
		MD5_GG(c, d, a, b, x[11], MD5_S23, 0x265E5A51);
		MD5_GG(b, c, d, a, x[ 0], MD5_S24, 0xE9B6C7AA);
		MD5_GG(a, b, c, d, x[ 5], MD5_S21, 0xD62F105D);
		MD5_GG(d, a, b, c, x[10], MD5_S22, 0x02441453);
		MD5_GG(c, d, a, b, x[15], MD5_S23, 0xD8A1E681);
		MD5_GG(b, c, d, a, x[ 4], MD5_S24, 0xE7D3FBC8);
		MD5_GG(a, b, c, d, x[ 9], MD5_S21, 0x21E1CDE6);
		MD5_GG(d, a, b, c, x[14], MD5_S22, 0xC33707D6);
		MD5_GG(c, d, a, b, x[ 3], MD5_S23, 0xF4D50D87);
		MD5_GG(b, c, d, a, x[ 8], MD5_S24, 0x455A14ED);
		MD5_GG(a, b, c, d, x[13], MD5_S21, 0xA9E3E905);
		MD5_GG(d, a, b, c, x[ 2], MD5_S22, 0xFCEFA3F8);
		MD5_GG(c, d, a, b, x[ 7], MD5_S23, 0x676F02D9);
		MD5_GG(b, c, d, a, x[12], MD5_S24, 0x8D2A4C8A);

		MD5_HH(a, b, c, d, x[ 5], MD5_S31, 0xFFFA3942);
		MD5_HH(d, a, b, c, x[ 8], MD5_S32, 0x8771F681);
		MD5_HH(c, d, a, b, x[11], MD5_S33, 0x6D9D6122);
		MD5_HH(b, c, d, a, x[14], MD5_S34, 0xFDE5380C);
		MD5_HH(a, b, c, d, x[ 1], MD5_S31, 0xA4BEEA44);
		MD5_HH(d, a, b, c, x[ 4], MD5_S32, 0x4BDECFA9);
		MD5_HH(c, d, a, b, x[ 7], MD5_S33, 0xF6BB4B60);
		MD5_HH(b, c, d, a, x[10], MD5_S34, 0xBEBFBC70);
		MD5_HH(a, b, c, d, x[13], MD5_S31, 0x289B7EC6);
		MD5_HH(d, a, b, c, x[ 0], MD5_S32, 0xEAA127FA);
		MD5_HH(c, d, a, b, x[ 3], MD5_S33, 0xD4EF3085);
		MD5_HH(b, c, d, a, x[ 6], MD5_S34, 0x04881D05);
		MD5_HH(a, b, c, d, x[ 9], MD5_S31, 0xD9D4D039);
		MD5_HH(d, a, b, c, x[12], MD5_S32, 0xE6DB99E5);
		MD5_HH(c, d, a, b, x[15], MD5_S33, 0x1FA27CF8);
		MD5_HH(b, c, d, a, x[ 2], MD5_S34, 0xC4AC5665);

		MD5_II(a, b, c, d, x[ 0], MD5_S41, 0xF4292244);
		MD5_II(d, a, b, c, x[ 7], MD5_S42, 0x432AFF97);
		MD5_II(c, d, a, b, x[14], MD5_S43, 0xAB9423A7);
		MD5_II(b, c, d, a, x[ 5], MD5_S44, 0xFC93A039);
		MD5_II(a, b, c, d, x[12], MD5_S41, 0x655B59C3);
		MD5_II(d, a, b, c, x[ 3], MD5_S42, 0x8F0CCC92);
		MD5_II(c, d, a, b, x[10], MD5_S43, 0xFFEFF47D);
		MD5_II(b, c, d, a, x[ 1], MD5_S44, 0x85845DD1);
		MD5_II(a, b, c, d, x[ 8], MD5_S41, 0x6FA87E4F);
		MD5_II(d, a, b, c, x[15], MD5_S42, 0xFE2CE6E0);
		MD5_II(c, d, a, b, x[ 6], MD5_S43, 0xA3014314);
		MD5_II(b, c, d, a, x[13], MD5_S44, 0x4E0811A1);
		MD5_II(a, b, c, d, x[ 4], MD5_S41, 0xF7537E82);
		MD5_II(d, a, b, c, x[11], MD5_S42, 0xBD3AF235);
		MD5_II(c, d, a, b, x[ 2], MD5_S43, 0x2AD7D2BB);
		MD5_II(b, c, d, a, x[ 9], MD5_S44, 0xEB86D391);

		context -> a += a;
		context -> b += b;
		context -> c += c;
		context -> d += d;
	}

	return;
}

static void
md5_init(
    struct md5_s *context
) {
	memset(context, 0, sizeof(struct md5_s));

	context -> a = UINT32_C(0x67452301);
	context -> b = UINT32_C(0xEFCDAB89);
	context -> c = UINT32_C(0x98BADCFE);
	context -> d = UINT32_C(0x10325476);

	return;
}

static void
md5_update(
    struct md5_s *context,
    const void *data,
    size_t length
) {
	uint8_t *b, *p;
	uint32_t l;
	size_t n;

	if (length == 0)
		return;

	b = (uint8_t *) data;

	l = (context -> count[0] + (((uint32_t) length) << 3)) & 0xFFFFFFFFU;
	if (l < context -> count[0])
		context -> count[1]++;
	context -> count[1] += (uint32_t) (length >> 29);
	context -> count[0]  = l;

	n = context -> num;
	if (n != 0) {
		p = (uint8_t *) context -> data;

		if (length >= MD5_CBLOCK || length + n >= MD5_CBLOCK) {
			memcpy(p + n, b, (MD5_CBLOCK - n) * sizeof(uint8_t));
			md5_block(context, p, 1);
			n = MD5_CBLOCK - n;
			b += n;
			length -= n;
			context -> num = 0;
			memset(p, 0, MD5_CBLOCK * sizeof(uint8_t));
		} else {
			memcpy(p + n, b, length * sizeof(uint8_t));
			context -> num += (unsigned int) length;

			return;
		}
	}

	n = length / MD5_CBLOCK;
	if (n > 0) {
		md5_block(context, b, n);
		n *= MD5_CBLOCK;
		b += n;
		length -= n;
	}

	if (length != 0) {
		p = (uint8_t *) context -> data;
		context -> num = (unsigned int) length;
		memcpy(p, b, length * sizeof(uint8_t));
	}

	return;
}

static void
md5_final(
    struct md5_s *context,
    uint8_t *digest
) {
	uint8_t *p;
	size_t n;

	p = (uint8_t *) context -> data;
	n = context -> num;

	p[n] = 0x80;
	n++;

	if (n > (MD5_CBLOCK - 8)) {
		memset(p + n, 0, (MD5_CBLOCK - n) * sizeof(uint8_t));
		n = 0;
		md5_block(context, p, 1);
	}

	memset(p + n, 0, (MD5_CBLOCK - 8 - n) * sizeof(uint8_t));
	p[56] = (uint8_t) (context -> count[0]      ) & 0xFF;
	p[57] = (uint8_t) (context -> count[0] >>  8) & 0xFF;
	p[58] = (uint8_t) (context -> count[0] >> 16) & 0xFF;
	p[59] = (uint8_t) (context -> count[0] >> 24) & 0xFF;
	p[60] = (uint8_t) (context -> count[1]      ) & 0xFF;
	p[61] = (uint8_t) (context -> count[1] >>  8) & 0xFF;
	p[62] = (uint8_t) (context -> count[1] >> 16) & 0xFF;
	p[63] = (uint8_t) (context -> count[1] >> 24) & 0xFF;

	md5_block(context, p, 1);
	context -> num = 0;
	memset(p, 0, MD5_CBLOCK * sizeof(uint8_t));

	digest[ 0] = (uint8_t) (context -> a      ) & 0xFF;
	digest[ 1] = (uint8_t) (context -> a >>  8) & 0xFF;
	digest[ 2] = (uint8_t) (context -> a >> 16) & 0xFF;
	digest[ 3] = (uint8_t) (context -> a >> 24) & 0xFF;
	digest[ 4] = (uint8_t) (context -> b      ) & 0xFF;
	digest[ 5] = (uint8_t) (context -> b >>  8) & 0xFF;
	digest[ 6] = (uint8_t) (context -> b >> 16) & 0xFF;
	digest[ 7] = (uint8_t) (context -> b >> 24) & 0xFF;
	digest[ 8] = (uint8_t) (context -> c      ) & 0xFF;
	digest[ 9] = (uint8_t) (context -> c >>  8) & 0xFF;
	digest[10] = (uint8_t) (context -> c >> 16) & 0xFF;
	digest[11] = (uint8_t) (context -> c >> 24) & 0xFF;
	digest[12] = (uint8_t) (context -> d      ) & 0xFF;
	digest[13] = (uint8_t) (context -> d >>  8) & 0xFF;
	digest[14] = (uint8_t) (context -> d >> 16) & 0xFF;
	digest[15] = (uint8_t) (context -> d >> 24) & 0xFF;

	memset(context, 0, sizeof(struct md5_s));

	return;
}

/* ========================================================================== */
mdx_h
mdx_init(
    enum MDX_TYPE type
) {
	struct mdx_s *st;

	switch (type) {
	case MDX_MD2:
	case MDX_MD4:
	case MDX_MD5:
		st = (struct mdx_s *) malloc(sizeof(struct mdx_s));
		if (st == NULL)
			return NULL;
		memset(st, 0, sizeof(struct mdx_s));
		break;
	case MDX__ERROR:
	default:
		return NULL;
	}

	switch (type) {
	case MDX_MD2:
		st -> type = MDX_MD2;
		st -> i.md2 = md2_init;
		st -> u.md2 = md2_update;
		st -> f.md2 = md2_final;
		st -> context.md2 = malloc(sizeof(struct md2_s));
		if (st -> context.md2 == NULL) {
			free(st);
			return NULL;
		} else {
			memset(st -> context.md2, 0, sizeof(struct md2_s));
		}
		break;
	case MDX_MD4:
		st -> type = MDX_MD4;
		st -> i.md4 = md4_init;
		st -> u.md4 = md4_update;
		st -> f.md4 = md4_final;
		st -> context.md4 = malloc(sizeof(struct md4_s));
		if (st -> context.md4 == NULL) {
			free(st);
			return NULL;
		} else {
			memset(st -> context.md4, 0, sizeof(struct md4_s));
		}
		break;
	case MDX_MD5:
		st -> type = MDX_MD5;
		st -> i.md5 = md5_init;
		st -> u.md5 = md5_update;
		st -> f.md5 = md5_final;
		st -> context.md5 = malloc(sizeof(struct md5_s));
		if (st -> context.md5 == NULL) {
			free(st);
			return NULL;
		} else {
			memset(st -> context.md5, 0, sizeof(struct md5_s));
		}
		break;
	case MDX__ERROR:
	default:
		/* NOTREACHED
		 *
		 * ndx_init() should never reach this point; the two cases are
		 * only inserted to remove extraneous "enumeration value not
		 * handled in switch" warnings from the compiler.
		 */
		break;
	}

	return st;
}

int
mdx_destroy(
    mdx_h *state
) {
	struct mdx_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;

	switch (st -> type) {
	case MDX_MD2:
		memset(st -> context.md2, 0, sizeof(struct md2_s));
		free(st -> context.md2);
		st -> context.md2 = NULL;
		break;
	case MDX_MD4:
		memset(st -> context.md4, 0, sizeof(struct md4_s));
		free(st -> context.md4);
		st -> context.md4 = NULL;
		break;
	case MDX_MD5:
		memset(st -> context.md5, 0, sizeof(struct md5_s));
		free(st -> context.md5);
		st -> context.md5 = NULL;
		break;
	case MDX__ERROR:
	default:
		/* NOTREACHED
		 *
		 * ndx_destroy() should never reach this point; the two cases
		 * are only inserted to remove extraneous "enumeration value not
		 * handled in switch" warnings from the compiler.
		 */
		break;
	}

	memset(st, 0, sizeof(struct mdx_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

size_t
mdx_digest_size(
    mdx_h state
) {
	size_t length;

	if (state == NULL)
		return 0;

	switch (state -> type) {
	case MDX_MD2:
	case MDX_MD4:
	case MDX_MD5:
		length = MDX_DIGEST;
		break;
	case MDX__ERROR:
	default:
		length = 0;
		break;
	}

	return length;
}

void
mdx_start(
    mdx_h state
) {
	switch (state -> type) {
	case MDX_MD2:
		state -> i.md2(state -> context.md2);
		break;
	case MDX_MD4:
		state -> i.md4(state -> context.md4);
		break;
	case MDX_MD5:
		state -> i.md5(state -> context.md5);
		break;
	case MDX__ERROR:
	default:
		/* NOTREACHED
		 *
		 * ndx_start() should never reach this point; the two cases are
		 * only inserted to remove extraneous "enumeration value not
		 * handled in switch" warnings from the compiler.
		 */
		break;
	}

	return;
}

void
mdx_update(
    mdx_h state,
    const void *data,
    size_t length
) {
	switch (state -> type) {
	case MDX_MD2:
		state -> u.md2(state -> context.md2, data, length);
		break;
	case MDX_MD4:
		state -> u.md4(state -> context.md4, data, length);
		break;
	case MDX_MD5:
		state -> u.md5(state -> context.md5, data, length);
		break;
	case MDX__ERROR:
	default:
		/* NOTREACHED
		 *
		 * ndx_update() should never reach this point; the two cases are
		 * only inserted to remove extraneous "enumeration value not
		 * handled in switch" warnings from the compiler.
		 */
		break;
	}

	return;
}

int
mdx_final(
    mdx_h state,
    uint8_t *digest,
    size_t length
) {
	if (length < MDX_DIGEST)
		return 1;

	switch (state -> type) {
	case MDX_MD2:
		state -> f.md2(state -> context.md2, digest);
		break;
	case MDX_MD4:
		state -> f.md4(state -> context.md4, digest);
		break;
	case MDX_MD5:
		state -> f.md5(state -> context.md5, digest);
		break;
	case MDX__ERROR:
	default:
		/* NOTREACHED
		 *
		 * ndx_final() should never reach this point; the two cases are
		 * only inserted to remove extraneous "enumeration value not
		 * handled in switch" warnings from the compiler.
		 */
		break;
	}

	return 0;
}
