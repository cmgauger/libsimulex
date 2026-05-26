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
 * \file	bh_fnv32.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/bit_hash.h>

/** \cond */
#define	FNV_32_PRIME	UINT32_C(0x01000193)
#define	FNV_32_MAGIC	UINT32_C(0x811C9DC5)
/** \endcond */

/**
 * \brief 32-bit Fowler-Noll-Vo hash state
 *
 * 
 */
struct fnv32_s {
	uint32_t h;
	/**<
	 */
	enum FNV_TYPE t;
	/**<
	 */
};

/**
 * \brief 32-bit Fowler-Noll-Vo hash standard initializer
 *
 * Allocates memory for, and then initializes the FNV hash state variable; this
 * version of the function initializes the starting hash value to the
 * FNV-1/FNV-1a offset basis of a standard FNV-1/FNV-1a hash.
 *
 * \return 32-bit FNV-1/FNV-1a state variable
 */
fnv32_h
fnv32_init(
    enum FNV_TYPE type	/**< [in] Type of Fowler-Noll-Vo hash to use. */
) {
	struct fnv32_s *st;

	switch (type) {
	case FNV_1:
	case FNV_1A:
		/* Proceed */
		break;
	case FNV__ERROR:
	default:
		return NULL;
	}

	st = (struct fnv32_s *) malloc(sizeof(struct fnv32_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct fnv32_s));

	st -> h = FNV_32_MAGIC;
	st -> t = type;

	return st;
}

/**
 * \brief 32-bit Fowler-Noll-Vo hash custom initializer
 *
 * Allocates memory for, and then initializes the FNV hash state variable; this
 * version of the function initializes the starting hash value to a custom value
 * specified by the user.
 *
 * \return 32-bit FNV-1/FNV-1a state variable
 */
fnv32_h
fnv32_init_alt(
    enum FNV_TYPE type,	/**< [in] Type of Fowler-Noll-Vo hash to use. */
    uint32_t init	/**< [in] Custom hash initialization value 32-bit). */
) {
	struct fnv32_s *st;

	switch (type) {
	case FNV_1:
	case FNV_1A:
		/* Proceed */
		break;
	case FNV__ERROR:
	default:
		return NULL;
	}

	st = (struct fnv32_s *) malloc(sizeof(struct fnv32_s));
	if (st == NULL)
		return NULL;
	memset(st, 0, sizeof(struct fnv32_s));

	st -> h = init;
	st -> t = type;

	return st;
}

/**
 * \brief 64-bit Fowler-Noll-Vo hash state destructor
 *
 * Zeroizes and frees the state variable.
 *
 * \return 0 on success, 1 on failure
 */
int
fnv32_destroy(
    fnv32_h *state	/**< [in,out] State variable to be destroyed. */
) {
	struct fnv32_s *st;

	if ((state == NULL) || (*state == NULL))
		return 1;

	st = *state;
	memset(st, 0, sizeof(struct fnv32_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 * \brief 32-bit Fowler-Noll-Vo generic buffer hashing function
 *
 * Hashes a generic buffer of any size into the current FNV-1a state.
 *
 * \return Hash value (32-bit).
 */
uint32_t
fnv32_buf(
    const void *buf,	/**< [in] Pointer to the start of the buffer. */
    size_t len,		/**< [in] Length of the buffer (in bytes). */
    fnv32_h state	/**< [in,out] State variable. */
) {
	uint8_t *bp, *be;

	bp = (uint8_t *) buf;
	be = bp + len;

	while (bp < be) {
		switch (state -> t) {
		case FNV_1:
			state -> h *= FNV_32_PRIME;
			state -> h ^= (uint32_t) *bp++;
			break;
		case FNV_1A:
		default:
			state -> h ^= (uint32_t) *bp++;
			state -> h *= FNV_32_PRIME;
			break;
		}
	}

	return state -> h;
}

/**
 * \brief 32-bit Fowler-Noll-Vo string hashing function
 *
 * Hash a null-terminated string (i.e. a C string) into the current FNV-1a
 * state. If the string is not null-terminated, use the generic buffer hashing
 * function instead.
 *
 * \return Hash value (32-bit).
 */
uint32_t
fnv32_str(
    const char *str,	/**< [in] Pointer to the start of the string. */
    fnv32_h state	/**< [in,out] State variable. */
) {
	return fnv32_buf(str, strlen(str), state);
}

/**
 * \brief 32-bit Fowler-Noll-Vo hash value
 *
 * Gets the value of the FNV-1a hash from the state variable.
 *
 * \return Hash value (32-bit).
 */
uint32_t
fnv32_value(
     fnv32_h state	/**< [in] State variable. */
) {
	return state -> h;
}