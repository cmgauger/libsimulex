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
 * \file	bh_purdy.c
 * \copyright	MIT
 * \date	2026
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <openssl/bn.h>
#include <openssl/err.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/bit_hash.h>

/** \cond */
#define	PURDY_COEFF	5
/** \endcond */

static const struct {
	uint64_t c[PURDY_COEFF];
} purdy_salt[] = {
	{{UINT64_C(0xFFFFFFFFFFFFFDB5), UINT64_C(0xFFFFFFFFFFFFFFE3),
	    UINT64_C(0xFFFFFFFFFFFFFF75), UINT64_C(0xFFFFFFFFFFFFFC4D),
	    UINT64_C(0xFFFFFFFFFFFFFEFF)}},
	{{UINT64_C(0xFFFFFFFFFFFFFFCB), UINT64_C(0xFFFFFFFFFFFFFF81),
	    UINT64_C(0xFFFFFFFFFFFFFEFF), UINT64_C(0xFFFFFFFFFFFFFEAF),
	    UINT64_C(0xFFFFFFFFFFFFFE5D)}},
	{{UINT64_C(0xFFFFFFFFFFFFFDF7), UINT64_C(0xFFFFFFFFFFFFFF1B),
	    UINT64_C(0xFFFFFFFFFFFFFE85), UINT64_C(0xFFFFFFFFFFFFFFCB),
	    UINT64_C(0xFFFFFFFFFFFFFC75)}},
	{{UINT64_C(0xFFFFFFFFFFFFFFD5), UINT64_C(0xFFFFFFFFFFFFFE2D),
	    UINT64_C(0xFFFFFFFFFFFFFD9B), UINT64_C(0xFFFFFFFFFFFFFD07),
	    UINT64_C(0xFFFFFFFFFFFFFC39)}},
	{{UINT64_C(0xFFFFFFFFFFFFFCCB), UINT64_C(0xFFFFFFFFFFFFFE19),
	    UINT64_C(0xFFFFFFFFFFFFFF95), UINT64_C(0xFFFFFFFFFFFFFFB9),
	    UINT64_C(0xFFFFFFFFFFFFFD7D)}},
	{{UINT64_C(0xFFFFFFFFFFFFFFF3), UINT64_C(0xFFFFFFFFFFFFFCB9),
	    UINT64_C(0xFFFFFFFFFFFFFE51), UINT64_C(0xFFFFFFFFFFFFFDCD),
	    UINT64_C(0xFFFFFFFFFFFFFF4B)}},
	{{UINT64_C(0xFFFFFFFFFFFFFFAD), UINT64_C(0xFFFFFFFFFFFFFEE5),
	    UINT64_C(0xFFFFFFFFFFFFFCA1), UINT64_C(0xFFFFFFFFFFFFFEA3),
	    UINT64_C(0xFFFFFFFFFFFFFF81)}},
	{{UINT64_C(0xFFFFFFFFFFFFFD1D), UINT64_C(0xFFFFFFFFFFFFFEC7),
	    UINT64_C(0xFFFFFFFFFFFFFF9F), UINT64_C(0xFFFFFFFFFFFFFD55),
	    UINT64_C(0xFFFFFFFFFFFFFF63)}},
	{{UINT64_C(0xFFFFFFFFFFFFFFC3), UINT64_C(0xFFFFFFFFFFFFFF39),
	    UINT64_C(0xFFFFFFFFFFFFFCED), UINT64_C(0xFFFFFFFFFFFFFE6F),
	    UINT64_C(0xFFFFFFFFFFFFFD73)}},
	{{UINT64_C(0xFFFFFFFFFFFFFC21), UINT64_C(0xFFFFFFFFFFFFFDA7),
	    UINT64_C(0xFFFFFFFFFFFFFFED), UINT64_C(0xFFFFFFFFFFFFFE3F),
	    UINT64_C(0xFFFFFFFFFFFFFD19)}},
	{{UINT64_C(0xFFFFFFFFFFFFFFAD), UINT64_C(0xFFFFFFFFFFFFFF4D),
	    UINT64_C(0xFFFFFFFFFFFFFEFF), UINT64_C(0xFFFFFFFFFFFFFEBD),
	    UINT64_C(0xFFFFFFFFFFFFFE95)}}
};

static const uint64_t purdy_modulus = UINT64_C(0xFFFFFFFFFFFFFFC5);
static const uint64_t purdy_n0 = UINT64_C(0x0000000000FFFFFD);
static const uint64_t purdy_n1 = UINT64_C(0x0000000000FFFFC1);

/**
 * \brief VMS Purdy hash function
 *
 * This function calcules the Purdy hash using the parameters established for
 * VAX/VMS.
 *
 * \return 0 on success; 1 on failure
 */
int
purdy(
    uint64_t x, /**< [in] value to be hashed */
    enum PURDY_SALT salt, /**< [in] selected algorithm salt */
    uint64_t *rv /** [out] hashed value */
) {
	BIGNUM *(*BN_load)(const unsigned char *, int, BIGNUM *);
	int (*BN_unload)(const BIGNUM *, unsigned char *, int);
	BIGNUM *cf[PURDY_COEFF], *mo, *ra, *rb, *rc, *rd, *rm, *rx, *xm;
	BN_CTX *context;
	enum PURDY_SALT s;
	uint8_t t[sizeof(uint64_t)];
	uint16_t et;
	int error_flag, i;

	/* Preset variables, this is to make the cleanup section require only a
	 * single jump-point as opposed to nested ones in an RAII pattern.
	 *
	 * Note that the BIGNUM pointers require no direct clean up, because
	 * they are temporary variables created by the BN_CTX context and are
	 * freed when the context is freed.
	 *
	 * We will preset the error flag, and we will only unset it if we do not
	 * at any point jump into the cleanup section.
	 */
	context = NULL;
	error_flag = 1;

	/* Do some basic error checking to establish that:
	 *   1. The return value pointer isn't NULL.
	 *   2. The salt enum is in range.
	 * If the above aren't true, we can bail immediately.
	 */
	if ((rv == NULL) || (salt < PURDY_S0) || (salt > PURDY_UAF))
		goto CLEANUP;
	s = salt - PURDY_S0;

	/* Spool up the OpenSSL BIGNUM context */
	if ((context = BN_CTX_new()) == NULL)
		goto CLEANUP;
	BN_CTX_start(context);

	/* WARNING!  WARNING!  WARNING!  WARNING!  WARNING!  WARNING!  WARNING!
	 *
	 * This function makes use of the OpenSSL "BIGNUM" function set, thus it
	 * is endian-sensitive.  Therefore, we must perform an endianness test
	 * before we can get into the actual hash calculations.
	 */
	et = UINT16_C(0xFF00);
	if (((uint8_t *) &et)[1]) {
		BN_load = BN_lebin2bn;
		BN_unload = BN_bn2lebinpad;
	} else {
		BN_load = BN_bin2bn;
		BN_unload = BN_bn2binpad;
	}

	/* Get the BIGNUMs we need to process the hash */
	if ((mo = BN_CTX_get(context)) == NULL)
		goto CLEANUP;
	if ((ra = BN_CTX_get(context)) == NULL)
		goto CLEANUP;
	if ((rb = BN_CTX_get(context)) == NULL)
		goto CLEANUP;
	if ((rc = BN_CTX_get(context)) == NULL)
		goto CLEANUP;
	if ((rd = BN_CTX_get(context)) == NULL)
		goto CLEANUP;
	if ((rm = BN_CTX_get(context)) == NULL)
		goto CLEANUP;
	if ((rx = BN_CTX_get(context)) == NULL)
		goto CLEANUP;
	if ((xm = BN_CTX_get(context)) == NULL)
		goto CLEANUP;

	/* Load the Purdy hash coefficients */
	for (i = 0; i < PURDY_COEFF; ++i) {
		if ((cf[i] = BN_CTX_get(context)) == NULL)
			goto CLEANUP;

		if ((cf[i] = BN_load((uint8_t *) &purdy_salt[s].c[i],
		    sizeof(uint64_t), cf[i])) == NULL)
			goto CLEANUP;
	}

	/* Step I: reduce "x" by the modulus "m" */
	if ((ra = BN_load((uint8_t *) &x, sizeof(uint64_t), ra)) == NULL)
		goto CLEANUP;
	if ((mo = BN_load((uint8_t *) &purdy_modulus, sizeof(uint64_t), mo)) ==
	    NULL)
		goto CLEANUP;
	if ((rm = BN_copy(rm, mo)) == NULL)
		goto CLEANUP;
	if (!BN_mod(xm, ra, rm, context))
		goto CLEANUP;


	/* Step II: calculate the first part of the equation --
	 *
	 *     modexp(xm,n1) * (modexp(xm,n0-n1) + c[0])
	 */
	if ((rc = BN_load((uint8_t *) &purdy_n1, sizeof(uint64_t), rc)) ==
	    NULL)
		goto CLEANUP;
	if ((rd = BN_load((uint8_t *) &purdy_n0, sizeof(uint64_t), rd)) ==
	    NULL)
		goto CLEANUP;
	if ((rb = BN_copy(rb, rc)) == NULL)
		goto CLEANUP;
	if ((rm = BN_copy(rm, mo)) == NULL)
		goto CLEANUP;
	if ((rx = BN_copy(rx, xm)) == NULL)
		goto CLEANUP;
	if (!BN_mod_exp(ra, rx, rb, rm, context))
		goto CLEANUP;
	if (!BN_sub(rc, rd, rc))
		goto CLEANUP;
	if ((rm = BN_copy(rm, mo)) == NULL)
		goto CLEANUP;
	if ((rx = BN_copy(rx, xm)) == NULL)
		goto CLEANUP;
	if (!BN_mod_exp(rb, rx, rc, rm, context))
		goto CLEANUP;
	if (!BN_add(rb, rb, cf[0]))
		goto CLEANUP;
	if (!BN_mul(ra, ra, rb, context))
		goto CLEANUP;

	/* Step III: calculate the second part of the equation --
	 *
	 *     xm * ((xm * (xm * c[1] + c[2])) + c[3]) + c[4]
	 */
	if ((rx = BN_copy(rx, xm)) == NULL)
		goto CLEANUP;
	if (!BN_mul(rb, rx, cf[1], context))
		goto CLEANUP;
	if (!BN_add(rb, rb, cf[2]))
		goto CLEANUP;
	if ((rx = BN_copy(rx, xm)) == NULL)
		goto CLEANUP;
	if (!BN_mul(rb, rb, rx, context))
		goto CLEANUP;
	if (!BN_add(rb, rb, cf[3]))
		goto CLEANUP;
	if ((rx = BN_copy(rx, xm)) == NULL)
		goto CLEANUP;
	if (!BN_mul(rb, rb, rx, context))
		goto CLEANUP;
	if (!BN_add(rb, rb, cf[4]))
		goto CLEANUP;

	/* Step IV: add [II] to [III] and reduce by "m" */
	if ((rm = BN_copy(rm, mo)) == NULL)
		goto CLEANUP;
	if (!BN_mod_add(rc, ra, rb, rm, context))
		goto CLEANUP;

	/* Unload the hash to the output */
	memset(t, 0, sizeof(uint64_t));
	if ((BN_num_bytes(rc) > (int) sizeof(uint64_t)) || (BN_unload(rc, t,
	    sizeof(uint64_t)) == -1))
		goto CLEANUP;
	memcpy(rv, t, sizeof(uint64_t));

	/* If we've got this far, unset the error flag */
	error_flag = 0;

	/* Clean up */
CLEANUP:
	if (context != NULL) {
		BN_CTX_end(context);
		BN_CTX_free(context);
	}

	return error_flag;
}