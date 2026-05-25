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
 * \file	bh_crc16.c
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
#define	CRC_TABLE_SIZE	256
/** \endcond */

/**
 * \brief 16-bit cyclic redundancy check state
 *
 * 
 */
struct crc16_s {
	uint16_t table[CRC_TABLE_SIZE];
	/**<
	 */
	uint16_t poly;
	/**<
	 */
	uint16_t init;
	/**<
	 */
	uint16_t xor;
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
crc16_h
crc16_init(
    enum CRC16_TYPE type	/**<  */
) {
	struct crc16_s *crc16;
	uint16_t t_crc, t_mask;
	size_t i, j;

	crc16 = (struct crc16_s *) malloc(sizeof(struct crc16_s));
	if (crc16 == NULL)
		return NULL;

	memset(crc16, 0, sizeof(struct crc16_s));

	switch (type) {
	case CRC16_IBM:
		/*   Poly: X'A001'
		 *   Init: X'0000'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'0000'
		 */
		crc16 -> poly = rev16(UINT16_C(0xA001));
		crc16 -> init =       UINT16_C(0x0000);
		crc16 -> xor  =       UINT16_C(0x0000);
		crc16 -> ref  = 1;
		break;
	case CRC16_CCITT:
		/*   Poly: X'8408'
		 *   Init: X'0000'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'0000'
		 */
		crc16 -> poly = rev16(UINT16_C(0x8408));
		crc16 -> init =       UINT16_C(0x0000);
		crc16 -> xor  =       UINT16_C(0x0000);
		crc16 -> ref  = 1;
		break;
	case CRC16_X25:
		/*   Poly: X'8408'
		 *   Init: X'FFFF'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'FFFF'
		 */
		crc16 -> poly = rev16(UINT16_C(0x8408));
		crc16 -> init =       UINT16_C(0xFFFF);
		crc16 -> xor  =       UINT16_C(0xFFFF);
		crc16 -> ref  = 1;
		break;
	case CRC16_MODBUS:
		/*   Poly: X'A001'
		 *   Init: X'FFFF'
		 *  RefIn: TRUE
		 * RefOut: TRUE
		 * XorOut: X'0000'
		 */
		crc16 -> poly = rev16(UINT16_C(0xA001));
		crc16 -> init =       UINT16_C(0xFFFF);
		crc16 -> xor  =       UINT16_C(0x0000);
		crc16 -> ref  = 1;
		break;
	case CRC16_PROFIBUS:
		/*   Poly: X'F3B8'
		 *   Init: X'FFFF'
		 *  RefIn: FALSE
		 * RefOut: FALSE
		 * XorOut: X'FFFF'
		 */
		crc16 -> poly = rev16(UINT16_C(0xF3B8));
		crc16 -> init =       UINT16_C(0xFFFF);
		crc16 -> xor  =       UINT16_C(0xFFFF);
		crc16 -> ref  = 0;
		break;
	case CRC16_OPENSAFETY_A:
		/*   Poly: X'AC9A'
		 *   Init: X'0000'
		 *  RefIn: FALSE
		 * RefOut: FALSE
		 * XorOut: X'0000'
		 */
		crc16 -> poly = rev16(UINT16_C(0xAC9A));
		crc16 -> init =       UINT16_C(0x0000);
		crc16 -> xor  =       UINT16_C(0x0000);
		crc16 -> ref  = 0;
		break;
	case CRC16_OPENSAFETY_B:
		/*   Poly: X'DAAE'
		 *   Init: X'0000'
		 *  RefIn: FALSE
		 * RefOut: FALSE
		 * XorOut: X'0000'
		 */
		crc16 -> poly = rev16(UINT16_C(0xDAAE));
		crc16 -> init =       UINT16_C(0x0000);
		crc16 -> xor  =       UINT16_C(0x0000);
		crc16 -> ref  = 0;
		break;
	case CRC16_TELEDISK:
		/*   Poly: X'E905'
		 *   Init: X'0000'
		 *  RefIn: FALSE
		 * RefOut: FALSE
		 * XorOut: X'0000'
		 */
		crc16 -> poly = rev16(UINT16_C(0xE905));
		crc16 -> init =       UINT16_C(0x0000);
		crc16 -> xor  =       UINT16_C(0x0000);
		crc16 -> ref  = 0;
		break;
	case CRC16_AUTOSAR:
		/*   Poly: X'8408'
		 *   Init: X'FFFF'
		 *  RefIn: FALSE
		 * RefOut: FALSE
		 * XorOut: X'FFFF'
		 */
		crc16 -> poly = rev16(UINT16_C(0x8408));
		crc16 -> init =       UINT16_C(0xFFFF);
		crc16 -> xor  =       UINT16_C(0xFFFF);
		crc16 -> ref  = 0;
		break;
	default:
		free(crc16);
		return NULL;
	}

	for (i = 0; i < CRC_TABLE_SIZE; ++i) {
		t_crc = (uint8_t) i;
		for (j = 8; j > 0; j--) {
			t_mask = (t_crc & 1) ? UINT16_C(0xFFFF) : 0;
			t_crc = (t_crc >> 1) ^ (crc16 -> poly & t_mask);
		}
		crc16 -> table[i] = t_crc;
	}

	return crc16;
}

/**
 * \brief 32-bit cyclic redundancy check state destructor
 *
 * Zeroizes and frees the state variable.
 *
 * \return 0 on success
 */
int
crc16_destroy(
    crc16_h *state	/**< [in,out] State variable to be destroyed. */
) {
	struct crc16_s *st;

	st = *state;
	memset(st, 0, sizeof(struct crc16_s));
	free(st);
	st = NULL;
	
	*state = NULL;

	return 0;
}

/**
 *
 */
uint16_t
crc16_calc(
    const void *message,	/**< [in] Pointer to the start of the buffer. */
    size_t length,	/**< [in] Length of the buffer (in bytes). */
    const crc16_h crc16	/**< [in] State variable. */
) {
	/* <https://web.archive.org/web/20190108202303/http://www.hackersdelight.org/hdcodetxt/crc.c.txt> */
	uint8_t *data;
	uint16_t byte, crc;
	size_t i;

	data = (uint8_t *) message;
	crc = crc16 -> init;

	for (i = 0; i < length; ++i) {
		byte = data[i];

		if (!(crc16 -> ref))
			byte = rev8((uint8_t) byte);

		crc = (crc >> 8) ^ crc16 -> table[lo8((crc ^ byte) & UINT16_C(0x00FF))];
	}

	if (!(crc16 -> ref))
		crc = rev16(crc);

	return (crc ^ crc16 -> xor);
}