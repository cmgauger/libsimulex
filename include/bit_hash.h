#ifndef BIT_HASH_H
#define BIT_HASH_H
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
 * \file	bit_hash.h
 * \copyright	MIT
 * \date	2024
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>

typedef struct fnv64_s *fnv64_h;
typedef struct fnv32_s *fnv32_h;
typedef struct crc32_s *crc32_h;
typedef struct crc16_s *crc16_h;
typedef struct mdx_s *mdx_h;
typedef struct pearson_s *pearson_h;

enum FNV_TYPE {
	FNV__ERROR,
	FNV_1,
	FNV_1A
};

enum MURMUR2_TYPE {
	MURMUR2__ERROR,
	MURMUR2_2,
	MURMUR2_2A
};

enum MDX_TYPE {
	MDX__ERROR,
	MDX_MD2,
	MDX_MD4,
	MDX_MD5
};

enum PURDY_SALT {
	PURDY__ERROR,
	PURDY_S0,
	PURDY_S1,
	PURDY_S2,
	PURDY_S3,
	PURDY_S4,
	PURDY_S5,
	PURDY_S6,
	PURDY_S7,
	PURDY_S8,
	PURDY_S9,
	PURDY_UAF
};

enum CRC32_TYPE {
	CRC32__ERROR,
	CRC32_ISO_HDLC,
	CRC32_ANSI_ADCCP,
	CRC32_IEEE_802_3,
	CRC32_ITU_V42,		// P: EDB88320, I: FFFFFFFF, O: FFFFFFFF, R: T/T
	CRC32_AUTODIN_II,
	CRC32_JAMCRC,		// P: EDB88320, I: FFFFFFFF, O: 00000000, R: T/T
	CRC32_CASTAGNOLI,
	CRC32_INTERLAKEN,
	CRC32_C,		// P: 82F63B78, I: FFFFFFFF, O: FFFFFFFF, R: T/T
	CRC32_D,		// P: D419CC15, I: FFFFFFFF, O: FFFFFFFF, R: T/T
	CRC32_K1,		// P: EB31D82E, I: FFFFFFFF, O: 00000000, R: T/T / Koopman {1,3,28}, MEF
	CRC32_K2,		// P: 992C1A4C, I: FFFFFFFF, O: 00000000, R: T/T / Koopman {1,1,30}
	CRC32_AIXM,
	CRC32_Q,		// P: D5828281, I: 00000000, O: 00000000, R: F/F
	CRC32_AUTOSAR		// P: C8DF352F, I: FFFFFFFF, O: FFFFFFFF, R: T/T
};

enum CRC16_TYPE {
	CRC16_ERROR,
	CRC16_IBM,		// P: A001, I: 0000, O: 0000, R: T/T
	CRC16_CCITT,		// P: 8408, I: 0000, O: 0000, R: T/T
	CRC16_X25,		// P: 8408, I: FFFF, O: FFFF, R: T/T
	CRC16_MODBUS,		// P: A001, I: FFFF, O: 0000, R: T/T
	CRC16_PROFIBUS,		// P: F3B8, I: FFFF, O: FFFF, R: F/F
	CRC16_OPENSAFETY_A,	// P: AC9A, I: 0000, O: 0000, R: F/F
	CRC16_OPENSAFETY_B,	// P: DAAE, I: 0000, O: 0000, R: F/F
	CRC16_TELEDISK,		// P: E905, I: 0000, O: 0000, R: F/F
	CRC16_AUTOSAR		// P: 8408, I: FFFF, O: FFFF, R: F/F
};

enum PEARSON_TYPE {
	PEARSON__ERROR,
	PEARSON_ACM,
	PEARSON_RFC,
	PEARSON_PI,
	PEARSON_E,
	PEARSON_PHI
};

enum COLLAPSE_TYPE {
	COLLAPSE__ERROR,
	COLLAPSE_V,
	COLLAPSE_S,
	COLLAPSE_X
};

enum MIX64_TYPE {
	MIX64__ERROR,
	MIX64_MURMUR3_64,
	MIX64_STAFFORD01,
	MIX64_STAFFORD02,
	MIX64_STAFFORD03,
	MIX64_STAFFORD04,
	MIX64_STAFFORD05,
	MIX64_STAFFORD06,
	MIX64_STAFFORD07,
	MIX64_STAFFORD08,
	MIX64_STAFFORD09,
	MIX64_STAFFORD10,
	MIX64_STAFFORD11,
	MIX64_STAFFORD12,
	MIX64_STAFFORD13,
	MIX64_STAFFORD14
};

/* 64-bit FNV-1a Hash */
fnv64_h fnv64_init(enum FNV_TYPE);
fnv64_h fnv64_init_alt(enum FNV_TYPE, uint64_t);
int fnv64_destroy(fnv64_h *);
uint64_t fnv64_buf(const void *, size_t, fnv64_h);
uint64_t fnv64_str(const char *, fnv64_h);
uint64_t fnv64_value(fnv64_h);

/* 32-bit FNV-1a Hash */
fnv32_h fnv32_init(enum FNV_TYPE);
fnv32_h fnv32_init_alt(enum FNV_TYPE, uint32_t);
int fnv32_destroy(fnv32_h *);
uint32_t fnv32_buf(const void *, size_t, fnv32_h);
uint32_t fnv32_str(const char *, fnv32_h);
uint32_t fnv32_value(fnv32_h);

/* MD2/MD4/MD5 Hash */
mdx_h	 mdx_init(enum MDX_TYPE);
int	 mdx_destroy(mdx_h *);
size_t	 mdx_digest_size(mdx_h);
void	 mdx_start(mdx_h);
void	 mdx_update(mdx_h, const void *, size_t);
int	 mdx_final(mdx_h, uint8_t *, size_t);

/* 8-bit Pearson hash */
pearson_h pearson_init(enum PEARSON_TYPE);
pearson_h pearson_init_custom(uint8_t, const uint8_t *, size_t);
int pearson_destroy(pearson_h *);
uint8_t pearson_buf(const void *, size_t, pearson_h);
uint8_t pearson_str(const char *, pearson_h);
uint8_t pearson_value(pearson_h);

/* MurmurHash hashes */
uint32_t murmur1(const void *, size_t, uint32_t);
uint32_t murmur2(const void *, size_t, uint32_t, enum MURMUR2_TYPE);
uint32_t murmur3(const void *, size_t, uint32_t);

/* Checksums */
crc32_h crc32_init(enum CRC32_TYPE);
int crc32_destroy(crc32_h *);
uint32_t crc32_calc(const void *, size_t, const crc32_h);
crc16_h crc16_init(enum CRC16_TYPE);
int crc16_destroy(crc16_h *);
uint16_t crc16_calc(const void *, size_t, const crc16_h);
uint32_t adler32(const void *, size_t);

/* Hashes */
uint64_t collapse(enum COLLAPSE_TYPE, uint64_t, const void *, size_t);
uint64_t mix64(enum MIX64_TYPE, uint64_t);
uint32_t mix96(uint32_t, uint32_t, uint32_t);
uint32_t triple32(uint32_t);
int purdy(uint64_t, enum PURDY_SALT, uint64_t *);
int comp128(const uint8_t *, const uint8_t *, uint32_t *, uint64_t *);

/* Bit Twiddling Hacks: <https://graphics.stanford.edu/~seander/bithacks.html>
 *     Interleaves ("Morton's Number")
 */
uint64_t interleave32(uint32_t, uint32_t);
uint32_t interleave16(uint16_t, uint16_t);
uint16_t interleave8(uint8_t, uint8_t);
/*     Rotations */
uint64_t rotl64(uint64_t, int);
uint64_t rotr64(uint64_t, int);
uint32_t rotl32(uint32_t, int);
uint32_t rotr32(uint32_t, int);
uint16_t rotl16(uint16_t, int);
uint16_t rotr16(uint16_t, int);
/*     Bit-Reversal */
uint64_t rev64(uint64_t);
uint32_t rev32(uint32_t);
uint16_t rev16(uint16_t);
uint8_t rev8(uint8_t);
/*     Population Count */
int pc64(uint64_t);
int pc32(uint32_t);
int pc16(uint16_t);
int pc8(uint8_t);
/*     Count Leading Zeroes (CLZ) */
int clz64(uint64_t);
int clz32(uint32_t);
int clz16(uint16_t);
int clz8(uint8_t);
/*     Unpack */
void unpack64to8(uint64_t, uint8_t *);
void unpack64to16(uint64_t, uint16_t *);
void unpack64to32(uint64_t, uint32_t *);
void unpack32to8(uint32_t, uint8_t *);
void unpack32to16(uint32_t, uint16_t *);
void unpack16to8(uint16_t, uint8_t *);
/*     Pack */
uint64_t pack32to64(uint32_t *);
uint64_t pack16to64(uint16_t *);
uint64_t pack8to64(uint8_t *);
uint32_t pack16to32(uint16_t *);
uint32_t pack8to32(uint8_t *);
uint16_t pack8to16(uint8_t *);
/*     Split */
void split32(uint64_t, uint32_t *, uint32_t *);
void split16(uint32_t, uint16_t *, uint16_t *);
void split8(uint16_t, uint8_t *, uint8_t *);
/*     Combine */
uint64_t combine64(uint32_t, uint32_t);
uint32_t combine32(uint16_t, uint16_t);
uint16_t combine16(uint8_t, uint8_t);
/*     Extractors */
uint32_t extract32from64(uint64_t, size_t);
uint16_t extract16from64(uint64_t, size_t);
uint16_t extract16from32(uint32_t, size_t);
uint8_t extract8from64(uint64_t, size_t);
uint8_t extract8from32(uint32_t, size_t);
uint8_t extract8from16(uint16_t, size_t);
/*     High & Low Halves */
uint32_t hi32(uint64_t);
uint32_t lo32(uint64_t);
uint16_t hi16(uint32_t);
uint16_t lo16(uint32_t);
uint8_t hi8(uint16_t);
uint8_t lo8(uint16_t);
/*     Miscellaneous */
uint64_t dispatch64(uint32_t);

#endif /* BIT_HASH_H */
