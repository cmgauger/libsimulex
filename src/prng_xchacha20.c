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
 * \file	xchacha20.c
 * \copyright	MIT
 * \date	2025
 * \author	Christian Gauger-Cosgrove
 * \version	1.0.0
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <simulex/prng.h>

/**
 * \brief Computes the bit-wise left rotation of its arguments
 * \param[in] X input value
 * \param[in] Y rotation in bits
 * \param[in] Z bit-length of input value
 *
 * \return X rotated left by Y bits
 */
#define	ROTL(X,Y,Z)	(((X) << (Y)) | ((X) >> (Z - (Y))))
/**
 * \brief Compute the ChaCha20 quarter round
 *
 * \param[in,out] A 
 * \param[in,out] B 
 * \param[in,out] C 
 * \param[in,out] D 
 */
#define	QR(A, B, C, D)	do {                                                   \
	A += B; D = ROTL(D ^ A, 16, 32);                                       \
	C += D; B = ROTL(B ^ C, 12, 32);                                       \
	A += B; D = ROTL(D ^ A,  8, 32);                                       \
	C += D; B = ROTL(B ^ C,  7, 32);                                       \
} while(0)
/**
 * \brief Packs an array of four bytes into a 32-bit value
 *
 * \param[in] p intput byte array
 *
 * \return 32-bit packed value
 */
#define U8TO32_LE(p)                                                           \
	(((uint32_t)((p)[0])      ) | ((uint32_t)((p)[1]) <<  8) |             \
	 ((uint32_t)((p)[2]) << 16) | ((uint32_t)((p)[3]) << 24))
/**
 * \brief Unpacks a 32-bit value int an array of four bytes
 *
 * \param[out] p output byte array
 * \param[in] v intput 32-bit value
 */
#define U32TO8_LE(p,v)	do {                                                   \
	(p)[0] = ((v)      ) & 0xFF;                                           \
	(p)[1] = ((v) >>  8) & 0xFF;                                           \
	(p)[2] = ((v) >> 16) & 0xFF;                                           \
	(p)[3] = ((v) >> 24) & 0xFF;                                           \
} while(0)
/**
 * \brief Number of full ChaCha20 rounds
 */
#define	ROUNDS	20
/**
 * \brief XChaCha20 key length in bytes
 */
#define	KEY_LEN	32
/**
 * \brief XChaCha20 initialization vector length in bytes
 */
#define	IV_LEN	24
/**
 * \brief XChaCha20 keystream block length in 32-bit words
 */
#define	BLK_LEN	16
/**
 * \brief Number of 32-bit counter words in the block counter
 */
#define	CNT_LEN	2

/**
 * \brief XChaCha20 state structure
 *
 * This structure holds all the information for the implementation of the
 * XChaCha20 PRNG, refer to the \ref xchacha20_rand32 and \ref xchacha20_rand64
 * implementations for specifics of how this structure is used to generate a
 * stream of pseudo-random numbers.
 */
struct xchacha20_s {
	uint32_t keystream[BLK_LEN];	/**< generated output block */
	uint32_t block[CNT_LEN];	/**< block counter words */
	uint8_t key[KEY_LEN];	/**< key */
	uint8_t nonce[IV_LEN];	/**< initialization vector */
	int position;	/**< position in output block */
};

/**
 * \brief XChaCha20 cipher next-state function
 *
 * This is the next-state function for the ChaCha20 cipher, however as it is
 * implemented it is not for cryptographic use, not having been written in a way
 * to avoid side-channel attacks. Instead it is being used as a robust PRNG.
 *
 * Although since XChaCha20 is an ARX type cipher, it is immune to (cache)
 * timing attacks and the like, however power analysis - and the general TEMPEST
 * issues inherent in consumer electronics - offer an in.
 *
 * \return Void.
 */
static void
xchacha20_next(
    struct xchacha20_s *state	/**< [in,out] XChaCha20 state variable */
) {
	uint32_t k[16], t[16];
	int i;

	/* Generate the "subkey" from the key and first 16 bytes of the nonce */
		/* The first four words are a constant */
	k[ 0] = 0x61707865;
	k[ 1] = 0x3320646E;
	k[ 2] = 0x79622D32;
	k[ 3] = 0x6B206574;
		/* The next eight words are the 256-bit key */
	k[ 4] = U8TO32_LE(state -> key     );
	k[ 5] = U8TO32_LE(state -> key +  4);
	k[ 6] = U8TO32_LE(state -> key +  8);
	k[ 7] = U8TO32_LE(state -> key + 12);
	k[ 8] = U8TO32_LE(state -> key + 16);
	k[ 9] = U8TO32_LE(state -> key + 20);
	k[10] = U8TO32_LE(state -> key + 24);
	k[11] = U8TO32_LE(state -> key + 28);
		/* The final four words are the first 128-bits of the nonce */
	k[12] = U8TO32_LE(state -> nonce     );
	k[13] = U8TO32_LE(state -> nonce +  4);
	k[14] = U8TO32_LE(state -> nonce +  8);
	k[15] = U8TO32_LE(state -> nonce + 12);

	/* Perform the rounds but DO NOT do the addition step */
	for (i = 0; i < ROUNDS; i += 2) {
		QR(k[0], k[4], k[ 8], k[12]);
		QR(k[1], k[5], k[ 9], k[13]);
		QR(k[2], k[6], k[10], k[14]);
		QR(k[3], k[7], k[11], k[15]);
		QR(k[0], k[5], k[10], k[15]);
		QR(k[1], k[6], k[11], k[12]);
		QR(k[2], k[7], k[ 8], k[13]);
		QR(k[3], k[4], k[ 9], k[14]);
	}

	/* Utilizing the subkey, prepare for the full round function */
		/* The first four words are a constant */
	t[ 0] = 0x61707865;
	t[ 1] = 0x3320646E;
	t[ 2] = 0x79622D32;
	t[ 3] = 0x6B206574;
		/* The next eight words are the subkey */
	t[ 4] = k[ 0];
	t[ 5] = k[ 1];
	t[ 6] = k[ 2];
	t[ 7] = k[ 3];
	t[ 8] = k[12];
	t[ 9] = k[13];
	t[10] = k[14];
	t[11] = k[15];
		/* The next two words are the 64-bit block counter */
	t[12] = state -> block[0];
	t[13] = state -> block[1];
		/* The final two words are the last 64-bits of the nonce */
	t[14] = U8TO32_LE(state -> nonce + 16);
	t[15] = U8TO32_LE(state -> nonce + 20);

	/* Copy the T array to the K array */
	memcpy(k, t, 16 * sizeof(uint32_t));

	/* Perform the rounds, then do the addition step */
	for (i = 0; i < ROUNDS; i += 2) {
		QR(k[0], k[4], k[ 8], k[12]);
		QR(k[1], k[5], k[ 9], k[13]);
		QR(k[2], k[6], k[10], k[14]);
		QR(k[3], k[7], k[11], k[15]);
		QR(k[0], k[5], k[10], k[15]);
		QR(k[1], k[6], k[11], k[12]);
		QR(k[2], k[7], k[ 8], k[13]);
		QR(k[3], k[4], k[ 9], k[14]);
	}
	for (i = 0; i < 16; ++i)
		k[i] += t[i];

	/* Move the K array to the state variable */
	memcpy(state -> keystream, k, 16 * sizeof(uint32_t));

	return;
}

/**
 * \brief XChaCha20 parameter sizing function
 *
 * This function's purpose is to provide the size of XChaCha20 key and IV
 * parameters.  While one could use macros defined in the header file, doing so
 * pollutes the global name space.
 *
 * \return parameter size
 */
size_t
xchacha20_parameter_length(
    enum XCHACHA20_PARAM param	/**< [in] parameter */
) {
	switch (param) {
	case XCHACHA20_KEY:
		return KEY_LEN;
		break;
	case XCHACHA20_IV:
		return IV_LEN;
		break;
	default:
		return 0;
		break;
	}
}

/**
 * \brief XChaCha20 cipher initializer function
 *
 * This function initializes the XChaCha20 PRNG state.  The initialization
 * function follows the format of the original Bernstein implementation of the
 * XChaCha20 cipher, with a 192-bit nonce and a 64-bit block counter, as opposed
 * to the IETF form (192-bit nonce and 32-bit block counter).
 *
 * Theoretically, as this particular XChaCha20 implementation is not being used
 * for ciphering, the nonce/block counter split could be done away with entirely
 * and a singular 256-bit nonce could be used in its place.
 *
 * \return initialized XChaCha20 state variable
 */
xchacha20_h
xchacha20_init(
    const uint8_t *key,	/**< [in] 256-bit (32 byte) key */
    const uint8_t *nonce,	/**< [in] 192-bit (24 byte) IV/nonce */
    const uint64_t block /**< [in] 64-bit block counter */
) {
	/* Create our temporary state variable */
	struct xchacha20_s *state;
	state = (struct xchacha20_s *) malloc(sizeof(struct xchacha20_s));
	if (state == NULL)
		return NULL;
	memset(state, 0, sizeof(struct xchacha20_s));

	/* Load the key, nonce/IV, and initial block number into the state */
	memset(state -> keystream, 0, 16 * sizeof(uint32_t));
	memcpy(state -> key, key, KEY_LEN * sizeof(uint8_t));
	memcpy(state -> nonce, nonce, IV_LEN * sizeof(uint8_t));
	state -> block[0] = (uint32_t) ((block      ) & 0xFFFFFFFF);
	state -> block[1] = (uint32_t) ((block >> 32) & 0xFFFFFFFF);

	/* Generate the block */
	xchacha20_next(state);

	/* Position at zero */
	state -> position = 0;

	/* Done with initialization */
	return state;
}

/**
 * \brief XChaCha20 cipher destructor function
 *
 * The destructor function is relatively self-explanatory in its operation, the
 * reader is invited to read the implementation as it is only seven lines of
 * code, however a quick summary of its operation is:
 * <ol>
 *     <li>The state variable is zeroized.</li>
 *     <li>The state variable is freed.</li>
 *     <li>The state variable pointer is set to \c NULL .</li>
 * </ol>
 *
 * \return 0
 */
int
xchacha20_destroy(
    xchacha20_h *state	/**< [in,out] pointer to the XChaCha20 state variable */
) {
	struct xchacha20_s *st;

	st = *state;

	memset(st, 0, sizeof(struct xchacha20_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}


/**
 * \brief XChaCha20 64-bit rand() function
 *
 * This function extracts two words (i.e. 64-bits) at a time from the generated
 * block in the XChaCha20 state and returns them, packed into a 64-bit
 * pseudo-random value.
 *
 * Once the sixteen words of the generated block are used up, the block counter
 * is incremented by one, and the next block of words is generated.
 *
 * To check the sanity/function of the PRNG, it has been tested using the
 * TestU01 test suite's "BigCrush" test; however, as TestU01 can only test PRNGs
 * of 32-bit size, the test was run twice: once on the least significant 32 bits
 * of each output, and once on the most significant 32 bits of each output, the
 * results of the tests were:
========= Summary results of BigCrush =========

 Version:          TestU01 1.2.3
 Generator:        xchacha20-64_lsb
 Number of statistics:  160
 Total CPU time:   05:05:40.93

 All tests were passed

========= Summary results of BigCrush =========

 Version:          TestU01 1.2.3
 Generator:        xchacha20-64_msb
 Number of statistics:  160
 Total CPU time:   05:06:53.72

 All tests were passed
 * 
 * Further, the PRNG was also tested with its output bit-reversed, using the
 * again using the BigCrush test, and once again testing the half the output at
 * a time:
========= Summary results of BigCrush =========

 Version:          TestU01 1.2.3
 Generator:        xchacha20-64_lsb_rev
 Number of statistics:  160
 Total CPU time:   05:20:14.61

 All tests were passed

========= Summary results of BigCrush =========

 Version:          TestU01 1.2.3
 Generator:        xchacha20-64_msb_rev
 Number of statistics:  160
 Total CPU time:   05:20:35.91

 All tests were passed
 *
 * Refer to \ref xchacha20_rand32 for the 32-bit version of this function.
 *
 * \return pseudo-randomly generated 64-bit integer
 */
uint64_t
xchacha20_rand64(
    xchacha20_h st	/**< [in,out] XChaCha20 state variable */
) {
	struct xchacha20_s *state;
	uint64_t t;
	uint32_t l, h;

	state = (struct xchacha20_s *) st;

	if (state -> position >= 16) {
		/* Increment the block count */
		l = state -> block[0];
		h = state -> block[1];
		if (l == UINT32_MAX) {
			if (h == UINT32_MAX) {
				h = 0;
			} else {
				h++;
			}
			
			l = 0;
		} else {
			l++;
		}
		state -> block[0] = l;
		state -> block[1] = h;

		/* Generate the block */
		xchacha20_next(state);

		/* Reposition at 0 */
		state -> position = 0;
	}

	/* Grab the low and high words and produce a 64-bit value */
	l = state -> keystream[state -> position++];
	h = state -> keystream[state -> position++];
	t = (((uint64_t)(l)      ) | ((uint64_t)(h) << 32));

	/* Return the 64-bit value */
	return t;
}

/**
 * \brief XChaCha20 32-bit rand() function
 *
 * This function extracts a single word (i.e. 32-bits) at a time from the
 * generated block in the XChaCha20 state and returns it as a 32-bit
 * pseudo-random value.
 *
 * Once the sixteen words of the generated block are used up, the block counter
 * is incremented by one, and the next block of words is generated.
 *
 * To check the sanity/function of the PRNG, it has been tested using the
 * TestU01 test suite's "BigCrush" test, the results of which were:
========= Summary results of BigCrush =========

 Version:          TestU01 1.2.3
 Generator:        xchacha20-32
 Number of statistics:  160
 Total CPU time:   03:33:37.78

 All tests were passed
 * 
 * Further, the PRNG was also tested with its output bit-reversed, using the
 * again using the BigCrush test:
========= Summary results of BigCrush =========

 Version:          TestU01 1.2.3
 Generator:        xchacha20-32_rev
 Number of statistics:  160
 Total CPU time:   03:46:20.26

 All tests were passed
 *
 * Refer to \ref xchacha20_rand64 for the 64-bit version of this function.
 *
 * \return pseudo-randomly generated 32-bit value
 */
uint32_t
xchacha20_rand32(
    xchacha20_h st	/**< [in,out] XChaCha20 state variable */
) {
	struct xchacha20_s *state;
	uint32_t l, h, t;

	state = (struct xchacha20_s *) st;

	if (state -> position >= 16) {
		/* Increment the block count */
		l = state -> block[0];
		h = state -> block[1];
		if (l == UINT32_MAX) {
			if (h == UINT32_MAX) {
				h = 0;
			} else {
				h++;
			}
			
			l = 0;
		} else {
			l++;
		}
		state -> block[0] = l;
		state -> block[1] = h;

		/* Generate the block */
		xchacha20_next(state);

		/* Reposition at 0 */
		state -> position = 0;
	}

	/* Grab the low and high words and produce a 64-bit value */
	t = state -> keystream[state -> position++];

	/* Return the 32-bit value */
	return t;
}
