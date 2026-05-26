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
 * \file	bh_crc32.c
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
#define	CRC_TABLE_SIZE	256
/** \endcond */

/**
 * \brief 32-bit cyclic redundancy check state
 *
 * 
 */
struct crc32_s {
	/* <https://stackoverflow.com/questions/75948294/purpose-of-refin-and-refout-parameters-in-crc-rocksoft-model>
	 * <https://crccalc.com/>
	 * <http://users.ece.cmu.edu/~koopman/crc/crc32.html>
	 * <https://reveng.sourceforge.io/crc-catalogue/>
	 */
	uint32_t table[CRC_TABLE_SIZE];
	/**<
	 */
	uint32_t poly;
	/**<
	 */
	uint32_t init;
	/**<
	 */
	uint32_t xor;
	/**<
	 */
	int ref;
	/**<
	 */
};

/**
 * \brief 32-bit cyclic redundancy check state initializer
 *
 * Allocates memory for, and then initializes, the CRC state variable. Several
 * common 32-bit CRCs are defined (and aliases for them as well). The CRCs are
 * defined in terms of their Rocksoft model parameters.
 *
 * \return 32-bit cyclic redundancy check state variable
 */
crc32_h
crc32_init(
    enum CRC32_TYPE type	/**<  */
) {
	struct crc32_s *crc32;

	crc32 = (struct crc32_s *) malloc(sizeof(struct crc32_s));
	if (crc32 == NULL)
		return NULL;

	memset(crc32, 0, sizeof(struct crc32_s));

	switch (type) {
	case CRC32_ISO_HDLC:
	case CRC32_ANSI_ADCCP:
	case CRC32_IEEE_802_3:
	case CRC32_ITU_V42:
		/*   Poly: X'04C11DB7'
		 *   Init: X'FFFFFFFF'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'FFFFFFFF'
		 */
		crc32 -> poly = rev32(UINT32_C(0x04C11DB7));
		crc32 -> init =       UINT32_C(0xFFFFFFFF);
		crc32 -> xor  =       UINT32_C(0xFFFFFFFF);
		crc32 -> ref  = 1;
		break;
	case CRC32_AUTODIN_II:
	case CRC32_JAMCRC:
		/*   Poly: X'04C11DB7'
		 *   Init: X'FFFFFFFF'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'00000000'
		 */
		crc32 -> poly = rev32(UINT32_C(0x04C11DB7));
		crc32 -> init =       UINT32_C(0xFFFFFFFF);
		crc32 -> xor  =       UINT32_C(0x00000000);
		crc32 -> ref  = 1;
		break;
	case CRC32_CASTAGNOLI:
	case CRC32_INTERLAKEN:
	case CRC32_C:
		/*   Poly: X'1EDC6F41'
		 *   Init: X'FFFFFFFF'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'FFFFFFFF'
		 */
		crc32 -> poly = rev32(UINT32_C(0x1EDC6F41));
		crc32 -> init =       UINT32_C(0xFFFFFFFF);
		crc32 -> xor  =       UINT32_C(0xFFFFFFFF);
		crc32 -> ref  = 1;
		break;
	case CRC32_D:
		/*   Poly: X'A833982B'
		 *   Init: X'FFFFFFFF'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'FFFFFFFF'
		 */
		crc32 -> poly = rev32(UINT32_C(0xA833982B));
		crc32 -> init =       UINT32_C(0xFFFFFFFF);
		crc32 -> xor  =       UINT32_C(0xFFFFFFFF);
		crc32 -> ref  = 1;
		break;
	case CRC32_K1:
		/*   Poly: X'741B8CD7'
		 *   Init: X'FFFFFFFF'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'00000000'
		 */
		crc32 -> poly = rev32(UINT32_C(0x741B8CD7));
		crc32 -> init =       UINT32_C(0xFFFFFFFF);
		crc32 -> xor  =       UINT32_C(0x00000000);
		crc32 -> ref  = 1;
		break;
	case CRC32_K2:
		/*   Poly: X'32583499'
		 *   Init: X'FFFFFFFF'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'00000000'
		 */
		crc32 -> poly = rev32(UINT32_C(0x32583499));
		crc32 -> init =       UINT32_C(0xFFFFFFFF);
		crc32 -> xor  =       UINT32_C(0x00000000);
		crc32 -> ref  = 1;
		break;
	case CRC32_AIXM:
	case CRC32_Q:
		/*   Poly: X'814141AB'
		 *   Init: X'00000000'
		 *  RefIn: FALSE
		 * RefOut: FALSE
		 * XorOut: X'00000000'
		 */
		crc32 -> poly = rev32(UINT32_C(0x814141AB));
		crc32 -> init =       UINT32_C(0x00000000);
		crc32 -> xor  =       UINT32_C(0x00000000);
		crc32 -> ref  = 0;
		break;
	case CRC32_AUTOSAR:
		/*   Poly: X'F4ACFB13'
		 *   Init: X'FFFFFFFF'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'FFFFFFFF'
		 */
		crc32 -> poly = rev32(UINT32_C(0xF4ACFB13));
		crc32 -> init =       UINT32_C(0xFFFFFFFF);
		crc32 -> xor  =       UINT32_C(0xFFFFFFFF);
		crc32 -> ref  = 1;
		break;
	default:
		free(crc32);
		return NULL;
	}

	return crc32;
}

/**
 * \brief 32-bit cyclic redundancy check state destructor
 *
 * Zeroizes and frees the state variable.
 *
 * \return 0 on success
 */
int
crc32_destroy(
    crc32_h *state	/**< [in,out] State variable to be destroyed. */
) {
	struct crc32_s *st;

	st = *state;
	memset(st, 0, sizeof(struct crc32_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 *
 */
uint32_t
crc32_calc(
    const void *message,	/**< [in] Pointer to the start of the buffer. */
    size_t length,	/**< [in] Length of the buffer (in bytes). */
    const crc32_h crc32	/**< [in] State variable. */
) {
	/* <https://web.archive.org/web/20190108202303/http://www.hackersdelight.org/hdcodetxt/crc.c.txt> */
	uint8_t *data;
	uint32_t byte, crc, mask;
	size_t i, j;

	data = (uint8_t *) message;
	crc = crc32 -> init;

	for (i = 0; i < length; ++i) {
		byte = data[i];

		if (!(crc32 -> ref))
			byte = rev8((uint8_t) byte);

		crc = crc ^ byte;
		for (j = 8; j > 0; j--) {
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (crc32 -> poly & mask);
		}
	}

	if (!(crc32 -> ref))
		crc = rev32(crc);

	return (crc ^ crc32 -> xor);
}