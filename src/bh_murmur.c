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
 * \file	bh_murmur.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <bit_hash.h>

/** \cond */
#define	MURMUR1_MAGIC	UINT32_C(0xC6A4a793)
#define	MURMUR2_MAGIC	UINT32_C(0x5BD1E995)
#define	MURMUR64_MAGIC	UINT64_C(0xC6A4A7935BD1E995)
/** \endcond */

/**
 * \brief MurmurHash2 internal hashing/scrambling function
 *
 * This is the integer hash function which is used internally within the the
 * overall hash function to scramble the inputs to MurmurHash2/2A.
 *
 *
 *
 * \return 32-bit value, hashed/scrambled
 *
 * \param [in] h  32-bit value to be hashed/scrambled with \c k
 * \param [in] k  32-bit value to be hashed/scrambled with \c h
 */
static uint32_t
murmur2_scramble(uint32_t h, uint32_t k) {
	k *= MURMUR2_MAGIC;
	k ^= (k >> 24);
	k *= MURMUR2_MAGIC;

	h *= MURMUR2_MAGIC;
	h ^= k;

	return h;
}

/**
 * \brief MurmurHash2 finalization hashing/scrambling function
 *
 * This is the integer hash function which finalizes the output of
 * MurmurHash2/2A, i.e. it causes all the bits to avalanche.
 *
 * \return 32-bit value, hashed/scrambled.
 *
 * \param [in] h  32-bit value to be hashed/scrambled
 */
static uint32_t
murmur2_finalize(uint32_t h) {
	h ^= (h >> 13);
	h *= MURMUR2_MAGIC;
	h ^= (h >> 15);

	return h;
}
/**
 * \brief
 * <a href="https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp">MurMurHash2</a>
 */
static uint32_t
murmur2_hash(
    const void *buf,		/**< [in] Pointer to the start of the buffer. */
    size_t len,		/**< [in] Length of the buffer (in bytes). */
    uint32_t seed	/**< [in] Initial hash seed value. */
) {
	uint8_t *data;
	uint32_t h, k;
	size_t i;

	data = (uint8_t *) buf;

	h = seed ^ (uint32_t) len;

	for (i = (len >> 2); i; --i) {
		k = pack8to32(data);
		data += sizeof(uint32_t);

		h = murmur2_scramble(h, k);
	}

	switch (len & 3) {
	case 3:
		h ^= ((uint32_t) data[2]) << 16;
		/* FALLTHROUGH */
	case 2:
		h ^= ((uint32_t) data[1]) <<  8;
		/* FALLTHROUGH */
	case 1:
		h ^= data[0];
		h *= MURMUR2_MAGIC;
	}

	h = murmur2_finalize(h);

	return h;
}
/**
 * \brief
 * <a href="https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp">MurMurHash2</a>
 */
static uint32_t
murmur2a_hash(
    const void *buf,		/**< [in] Pointer to the start of the buffer. */
    size_t len,		/**< [in] Length of the buffer (in bytes). */
    uint32_t seed	/**< [in] Initial hash seed value. */
) {
	uint8_t *data;
	uint32_t h, k;
	size_t i;

	data = (uint8_t *) buf;

	h = seed;

	for (i = (len >> 2); i; --i) {
		k = pack8to32(data);
		data += sizeof(uint32_t);

		h = murmur2_scramble(h, k);
	}

	for (i = (len & 3), k = 0; i; --i) {
		k <<= 8;
		k |= data[i - 1];
	}

	h = murmur2_scramble(h, k);
	h = murmur2_scramble(h, (uint32_t) len);

	h = murmur2_finalize(h);

	return h;
}

/**
 * \brief MurmurHash3 internal hashing/scrambling function
 *
 * This is the integer hash function which is used internally within the the
 * overall hash function to scramble the inputs to MurmurHash3.
 *
 * \return 32-bit value, hashed/scrambled
 */
static uint32_t
murmur3_scramble(
    uint32_t k		/**< [in] 32-bit value to be hashed/scrambled. */
) {
	k *= UINT32_C(0xCC9E2D51);
	k  = (k << 15) | (k >> 17);
	k *= UINT32_C(0x1B873593);

	return k;
}

/**
 * \brief MurmurHash3 finalization hashing/scrambling function
 *
 * This is the integer hash function which finalizes the output of MurmurHash3,
 * i.e. it causes all the bits to avalanche.
 *
 * \return 32-bit value, hashed/scrambled.
 */
static uint32_t
murmur3_finalize(
    uint32_t k		/**< [in] 32-bit value to be hashed/scrambled. */
) {
	k ^= k >> 16;
	k *= UINT32_C(0x85EBCA6B);
	k ^= k >> 13;
	k *= UINT32_C(0xC2B2AE35);
	k ^= k >> 16;

	return k;
}

/**
 * \brief
 * <a href="https://github.com/aappleby/smhasher/blob/master/src/MurmurHash1.cpp">MurMurHash2</a>
 */
uint32_t
murmur1(
    const void *buf,		/**< [in] Pointer to the start of the buffer. */
    size_t len,		/**< [in] Length of the buffer (in bytes). */
    uint32_t seed	/**< [in] Initial hash seed value. */
) {
	uint8_t *data;
	uint32_t h, k;
	size_t i;

	data = (uint8_t *) buf;
	h = seed ^ (uint32_t) (len * MURMUR1_MAGIC);

	for (i = (len >> 2); i; --i) {
		k = pack8to32(data);
		data += sizeof(uint32_t);

		h += k;
		h *= MURMUR1_MAGIC;
		h ^= h >> 16;
	}

	switch (len & 3) {
	case 3:
		h += ((uint32_t) data[2]) << 16;
		/* FALLTHROUGH */
	case 2:
		h += ((uint32_t) data[1]) <<  8;
		/* FALLTHROUGH */
	case 1:
		h += data[0];
		h *= MURMUR1_MAGIC;
		h ^= h >> 16;
	}

	h *= MURMUR1_MAGIC;
	h ^= h >> 10;
	h *= MURMUR1_MAGIC;
	h ^= h >> 17;

	return h;
}

/**
 * \brief
 * <a href="https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp">MurMurHash2</a>
 */
uint32_t
murmur2(
    const void *buf,		/**< [in] Pointer to the start of the buffer. */
    size_t len,		/**< [in] Length of the buffer (in bytes). */
    uint32_t seed,	/**< [in] Initial hash seed value. */
    enum MURMUR2_TYPE type  /**< [in]  */
) {
	if (type == MURMUR2_2) {
		return murmur2_hash(buf, len, seed);
	} else if (type == MURMUR2_2A) {
		return murmur2a_hash(buf, len, seed);
	} else {
		return 0;
	}

}

/**
 * \brief 32-bit MurmurHash3 generic buffer hashing function
 *
 * Hashes a generic buffer of any size using an endian-ambivalent version of the
 * 32-bit MurmurHash3 algorithm. Implementation is based upon that in the Austin
 * Appleby's "SMHasher" test suite, on GitHub:
 * <a href="https://github.com/aappleby/smhasher/tree/master">SMHasher</a>
 *
 *\return Hash value (32-bit).
 */
uint32_t
murmur3(
    const void *buf,		/**< [in] Pointer to the start of the buffer. */
    size_t len,		/**< [in] Length of the buffer (in bytes). */
    uint32_t seed	/**< [in] Initial hash seed value. */
) {
	uint8_t *data;
	uint32_t h, k;
	size_t i;
	
	data = (uint8_t *) buf;
	h = seed;

	for (i = (len >> 2); i; --i) {
		k = pack8to32(data);
		data += sizeof(uint32_t);

		h ^= murmur3_scramble(k);
		h  = (h << 13) | (h >> 19);
		h  = h * 5 + UINT32_C(0xE6546B64);
	}

	for (i = (len & 3), k = 0; i; --i) {
		k <<= 8;
		k |= data[i - 1];
	}
	h ^= murmur3_scramble(k);

	/* Finalize */
	h ^= (uint32_t) len;
	h  = murmur3_finalize(h);
	
	return h;
}