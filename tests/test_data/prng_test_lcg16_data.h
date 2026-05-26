#ifndef PRNG_TEST_LCG16_DATA_H
#define PRNG_TEST_LCG16_DATA_H
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

static const struct {
	uint16_t seed;
	struct {
		uint16_t value[TEST_POINTS];
	} primary;
	struct {
		uint16_t value[TEST_POINTS];
	} alternate;
	struct {
		uint16_t multiplier;
		uint16_t additive;
		uint16_t value[TEST_POINTS];
	} custom;
} prng_lcg16_test_data[TEST_SETS] = {
	{UINT16_C(0x37AE),  /* Seed I */
	 {{20639,  3346, 31563, 40086, 34903, 28474, 45763,  5630,
	   19855, 27106, 33723, 52198,  6215, 25866,  9779, 26190}},
	 {{ 7959, 51354, 63803,  2086, 21567, 53586, 39203, 42782,
	   26855, 29834, 56459, 19606, 50447, 10818, 60275, 45198}},
	 {UINT16_C(0xB82D),  UINT16_C(0xA715),
	  {32939, 11556, 30313, 61322, 62039, 51296, 57333,  7462,
	    5827, 53084, 14913, 40066, 39407, 32280,  4429,  2462}}},
	{UINT16_C(0x56FE),  /* Seed II */
	 {{45199, 29410, 16059, 56550, 60231, 48650, 53555, 18254,
	    9087, 61874, 60715, 48694, 53559, 30170, 51875, 59806}},
	 {{63367, 38250, 17963, 54902, 45487, 21794, 49939, 62574,
	   15191, 65370, 21371, 59622, 64639,  3090,  8035, 27614}},
	 {UINT16_C(0x7895),  UINT16_C(0x1DA5),
	  {53115, 34876, 34961, 37386, 52599, 31720, 64429, 45398,
	   42163, 57812, 60937, 57570, 61743, 34304,  7077, 36014}}},
	{UINT16_C(0xAA54),  /* Seed III */
	 {{29265, 10088, 61965, 36892, 22697, 39280, 58661, 24676,
	    3713, 43512, 17341,  9004, 43993, 41216, 58837, 24692}},
	 {{56377, 55568, 65101, 45036, 22977, 65000, 54165, 39428,
	   43209,  9024, 44637, 49308, 37201, 57624,  5797, 64436}},
	 {UINT16_C(0xD265),  UINT16_C(0x69C5),
	  {34025, 64434, 47871, 24160, 26021, 56798,  3675, 47532,
	   49825, 17738, 29687, 51256, 22493, 24054, 18387, 54788}}},
	{UINT16_C(0xA715),  /* Seed IV */
	 {{11316, 50161,    72, 43181, 47100, 23625, 51280, 24005,
	    3652, 25633,  3800,  7773, 14092, 62329,  7136, 49781}},
	 {{45188, 41033, 50624, 43485, 36636, 28881, 31640, 47653,
	     564, 41689, 25072, 57325, 25036, 65121,  4296, 41781}},
	 {UINT16_C(0x9B45),  UINT16_C(0x3571),
	  {62746,   883, 50288, 63393, 28374, 42783,  1484, 18797,
	   65234,  2571, 37736, 59513,  8462, 38967, 35140, 24773}}},
	{UINT16_C(0xA708),  /* Seed V */
	 {{ 5357, 25788, 39561, 11024, 32261, 22276, 63073, 60824,
	   12957, 56268,  6585, 13984,  2741, 31508, 40081, 52776}},
	 {{ 6133, 35364, 64041, 41568,  9405, 54972, 33969, 22072,
	   7941, 22484, 20665, 55952,  3789, 25964,  9793, 51048}},
	 {UINT16_C(0x8DD5),  UINT16_C(0x551B),
	  {46787, 52314, 60925, 45468,  2023,  9038, 43777, 10736,
	   27083, 10754, 25285,  1028, 57455, 13430, 63817, 62424}}}
};

#endif /* PRNG_TEST_LCG16_DATA_H */
