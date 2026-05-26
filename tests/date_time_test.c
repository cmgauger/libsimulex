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
 * \file	date_time_test.c
 * \copyright	MIT
 * \date	2025
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CUnit/CUnit.h>

#include <simulex/date_time.h>
#include "date_time_test.h"

#include "test_data/date_time_test_data.h"

/**
 * \brief
 */
static void
set_datetime(struct tm *dt, int yy, int mm, int dd, int hh, int nn, int ss) {
	/* Clear the struct */
	memset(dt, 0, sizeof(struct tm));

	/* Set date */
	dt -> tm_year = yy - 1900;
	dt -> tm_mon  = mm - 1;
	dt -> tm_mday = dd;

	/* Set time */
	dt -> tm_hour = hh;
	dt -> tm_min  = nn;
	dt -> tm_sec  = ss;

	return;
}

/**
 * \brief
 */
static void
set_date(struct tm *dt, int yy, int mm, int dd) {
	/* Clear the struct */
	memset(dt, 0, sizeof(struct tm));

	/* Set date */
	dt -> tm_year = yy - 1900;
	dt -> tm_mon  = mm - 1;
	dt -> tm_mday = dd;

	return;
}

/**
 * \brief
 */
static void
set_time(struct tm *dt, int hh, int mm, int ss) {
	/* Clear the struct */
	memset(dt, 0, sizeof(struct tm));

	/* Set time */
	dt -> tm_hour = hh;
	dt -> tm_min  = mm;
	dt -> tm_sec  = ss;

	return;
}

/**
 * \brief
 */
int
init_date_time_suite(void) {
	return 0;
}

/**
 * \brief
 */
int
clean_date_time_suite(void) {
	return 0;
}

/**
 * \brief
 */
void
test_valid_date(void) {
	struct tm t;

	/* Valid dates and times */
	set_datetime(&t, 1881, 11, 21, 22, 22, 19);
	CU_ASSERT_EQUAL(valid_date(t), 1);
	set_datetime(&t, 1862,  9, 28,  6, 35, 57);
	CU_ASSERT_EQUAL(valid_date(t), 1);
	set_datetime(&t, 2062,  4, 15, 15, 52, 11);
	CU_ASSERT_EQUAL(valid_date(t), 1);
	set_datetime(&t, 2007,  2, 26, 14,  2, 46);
	CU_ASSERT_EQUAL(valid_date(t), 1);
	set_datetime(&t, 1972,  2, 23, 11, 27, 53);
	CU_ASSERT_EQUAL(valid_date(t), 1);

	/* Invalid date -- year out of range; for the test, the year value in
	 * the struct must be set manually, given that the set_datetime()
	 * function automatically adjusts the provided year value.
	 */
	set_datetime(&t, 0, 8, 12, 14, 38, 53);
	t.tm_year = 2147481748;
	CU_ASSERT_EQUAL(valid_date(t), 0);

	/* Invalid date -- month out of range */
	set_datetime(&t, 2040,  0, 19,  6, 17, 37);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 1806, 13,  7,  6, 36, 17);
	CU_ASSERT_EQUAL(valid_date(t), 0);

	/* Invalid date -- day out of range */
	set_datetime(&t, 1957,  6, -4,  0, 27, 45);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 2029,  1, 34, 15, 39, 30);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 1993,  2, 33,  5, 30, 19);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 1842,  3, 35,  4, 25,  9);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 1816,  4, 35,  6, 58, 36);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 1816,  5, 33,  5, 20, 33);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 1961,  6, 33, 14,  3, 54);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 2090,  7, 34, 18,  9,  4);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 2021,  8, 36,  1,  6, 53);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 2091,  9, 32,  8, 15, 46);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 1937, 10, 34, 16, 46,  4);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 1960, 11, 32,  0, 35, 23);
	CU_ASSERT_EQUAL(valid_date(t), 0);
	set_datetime(&t, 1992, 12, 33,  3, 37, 48);
	CU_ASSERT_EQUAL(valid_date(t), 0);

	return;
}

/**
 * \brief
 */
void
test_valid_time(void) {
	struct tm t;

	/* Valid dates and times */
	set_datetime(&t, 1881, 11, 21, 22, 22, 19);
	CU_ASSERT_EQUAL(valid_time(t), 1);
	set_datetime(&t, 1862,  9, 28,  6, 35, 57);
	CU_ASSERT_EQUAL(valid_time(t), 1);
	set_datetime(&t, 2062,  4, 15, 15, 52, 11);
	CU_ASSERT_EQUAL(valid_time(t), 1);
	set_datetime(&t, 2007,  2, 26, 14,  2, 46);
	CU_ASSERT_EQUAL(valid_time(t), 1);
	set_datetime(&t, 1972,  2, 23, 11, 27, 53);
	CU_ASSERT_EQUAL(valid_time(t), 1);

	/* Invalid time -- hour out of range */
	set_datetime(&t, 1919,  7,  1, -4, 16,  9);
	CU_ASSERT_EQUAL(valid_time(t), 0);
	set_datetime(&t, 1863,  2, 25, 27, 13, 47);
	CU_ASSERT_EQUAL(valid_time(t), 0);

	/* Invalid time -- minute out of rane */
	set_datetime(&t, 1866, 10,  3, 14, -1, 13);
	CU_ASSERT_EQUAL(valid_time(t), 0);
	set_datetime(&t, 1806,  9,  4,  6, 64, 40);
	CU_ASSERT_EQUAL(valid_time(t), 0);

	/* Invalid time -- second out of rage */
	set_datetime(&t, 2067,  7, 11, 13, 35, -2);
	CU_ASSERT_EQUAL(valid_time(t), 0);
	set_datetime(&t, 1900,  6,  5, 10, 47, 63);
	CU_ASSERT_EQUAL(valid_time(t), 0);

	return;
}

/**
 * \brief
 */
void
test_abs2tm(void) {
	struct tm c, t;
	int a, i;

	/* Run through the error checks:
	 *   1. NULL destination variable
	 *   2. Invalid absolute date (value greater than 105649066)
	 *   3. Invalid absolute date (value less than -109094767)
	 * The time value is ignored.
	 */
	set_datetime(&t, 1970, 12, 26, 13,  7,  8);
	a = 8675309;
	CU_ASSERT_EQUAL(abs2tm(a, NULL), 1);

	set_datetime(&t, 1998,  6, 10, 16, 38, 30);
	a = 105649067;
	CU_ASSERT_EQUAL(abs2tm(a, &t), 1);
	CU_TEST((t.tm_year == 98) && (t.tm_mon ==  5) && (t.tm_mday == 10) &&
	    (t.tm_hour == 16) && (t.tm_min == 38) && (t.tm_sec == 30));

	set_datetime(&t, 2003,  6, 20, 11, 16, 22);
	a = -109094768;
	CU_ASSERT_EQUAL(abs2tm(a, &t), 1);
	CU_TEST((t.tm_year == 103) && (t.tm_mon ==  5) && (t.tm_mday == 20) &&
	    (t.tm_hour == 11) && (t.tm_min == 16) && (t.tm_sec == 22));

	/* set_datetime(&t, 2009, 12, 12,  3, 24, 53); *
	 * set_datetime(&t, 2037,  7, 12, 22, 32, 14); */

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Clear the structs */
		memset(&c, 0, sizeof(struct tm));
		memset(&t, 0, sizeof(struct tm));
		
		/* Run the test */
		set_date(&t, test_data[i].date.year, test_data[i].date.month,
		    test_data[i].date.day);
		CU_ASSERT_EQUAL(abs2tm(test_data[i].absolute_date, &c), 0);
		CU_TEST((c.tm_year == t.tm_year) && (c.tm_mon == t.tm_mon) &&
		    (c.tm_mday == t.tm_mday));
	}

	return;
}

/**
 * \brief
 */
void
test_tm2abs(void) {
	struct tm t;
	int a, i;

	/* Run through the function error checks:
	 *   1. NULL destination variable
	 *   2. Invalid year (less than -5879488)
	 *   3. Invalid year (greater than 5879490)
	 *   4. Invalid month (less than 1)
	 *   5. Invalid month (greater than 12)
	 *   6. Invalid day (less tan 1)
	 *   7. Invalid day (greater than 31)
	 * The time value is ignored.
	 */
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);
	CU_ASSERT_EQUAL(tm2abs(NULL, t), 1);
	
	set_datetime(&t, -5879489, 10, 28, 13,  0,  0);
	a = 1701;
	CU_ASSERT_EQUAL(tm2abs(&a, t), 1);
	CU_ASSERT_EQUAL(a, 1701);

	set_datetime(&t, 5879491, 10, 28, 13,  0,  0);
	a = 1701;
	CU_ASSERT_EQUAL(tm2abs(&a, t), 1);
	CU_ASSERT_EQUAL(a, 1701);

	set_datetime(&t, 1987, 0, 28, 13,  0,  0);
	a = 1701;
	CU_ASSERT_EQUAL(tm2abs(&a, t), 1);
	CU_ASSERT_EQUAL(a, 1701);

	set_datetime(&t, 1987, 13, 28, 13,  0,  0);
	a = 1701;
	CU_ASSERT_EQUAL(tm2abs(&a, t), 1);
	CU_ASSERT_EQUAL(a, 1701);

	set_datetime(&t, 1987, 10,  0, 13,  0,  0);
	a = 1701;
	CU_ASSERT_EQUAL(tm2abs(&a, t), 1);
	CU_ASSERT_EQUAL(a, 1701);

	set_datetime(&t, 1987, 10, 32, 13,  0,  0);
	a = 1701;
	CU_ASSERT_EQUAL(tm2abs(&a, t), 1);
	CU_ASSERT_EQUAL(a, 1701);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Run the test */
		set_date(&t, test_data[i].date.year, test_data[i].date.month,
		    test_data[i].date.day);
		CU_ASSERT_EQUAL(tm2abs(&a, t), 0);
		CU_ASSERT_EQUAL(a, test_data[i].absolute_date);
	}

	return;
}

/**
 * \brief
 */
void
test_abs2mjd(void) {
	int i, t;

	/* Test the error checks:
	 *   1. NULL destination variable
	 *   2. Absolute date out of range (less than -2146805072)
	 */
	CU_ASSERT_EQUAL(abs2mjd(0, NULL), 1);

	t = 59650;
	CU_ASSERT_EQUAL(abs2mjd(-2146805073, &t), 1);
	CU_TEST(t == 59650);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Run the tests */
		CU_ASSERT_EQUAL(abs2mjd(test_data[i].absolute_date, &t), 0);
		CU_ASSERT_EQUAL(t, test_data[i].modified_julian_date);
	}

	return;
}

/**
 * \brief
 */
void
test_mjd2abs(void) {
	int i, t;

	/* Test the error checks:
	 *   1. NULL destination variable
	 *   2. Modified Julian Date out of range (greater than 2146805071)
	 */
	CU_ASSERT_EQUAL(mjd2abs(NULL, 0), 1);

	t = 74656;
	CU_ASSERT_EQUAL(mjd2abs(&t, 2146805072), 1);
	CU_TEST(t == 74656);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Run the tests */
		CU_ASSERT_EQUAL(mjd2abs(&t, test_data[i].modified_julian_date),
		    0);
		CU_ASSERT_EQUAL(t, test_data[i].absolute_date);
	}

	return;
}

/**
 * \brief
 */
void
test_abs2lil(void) {
	int i, t;

	/* Test the error checks:
	 *   1. NULL destination variable
	 *   2. Absolute date out of range (less than -2146905913)
	 */
	CU_ASSERT_EQUAL(abs2lil(0, NULL), 1);

	t = 74913;
	CU_ASSERT_EQUAL(abs2lil(-2146905914, &t), 1);
	CU_TEST(t == 74913);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Run the tests */
		CU_ASSERT_EQUAL(abs2lil(test_data[i].absolute_date, &t), 0);
		CU_ASSERT_EQUAL(t, test_data[i].lilian_date);
	}

	return;
}

/**
 * \brief
 */
void
test_lil2abs(void) {
	int i, t;

	/* Test the error checks:
	 *   1. NULL destination variable
	 *   2. Lilian date out of range (greater than 2146905912)
	 */
	CU_ASSERT_EQUAL(lil2abs(NULL, 0), 1);

	t = -32768;
	CU_ASSERT_EQUAL(lil2abs(&t, 2146905913), 1);
	CU_TEST(t == -32768);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Run the tests */
		CU_ASSERT_EQUAL(lil2abs(&t, test_data[i].lilian_date), 0);
		CU_ASSERT_EQUAL(t, test_data[i].absolute_date);
	}

	return;
}

/**
 * \brief
 */
void
test_abs2xcl(void) {
	int i, t;

	/* Test the error checks:
	 *   1. NULL destination variable
	 *   2. Absolute date out of range (less than -2146790053)
	 */
	CU_ASSERT_EQUAL(abs2xcl(0, NULL), 1);

	t = 74913;
	CU_ASSERT_EQUAL(abs2xcl(-2146790054, &t), 1);
	CU_TEST(t == 74913);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Run the tests */
		CU_ASSERT_EQUAL(abs2xcl(test_data[i].absolute_date, &t), 0);
		CU_ASSERT_EQUAL(t, test_data[i].excel_date);
	}

	return;
}

/**
 * \brief
 */
void
test_xcl2abs(void) {
	int i, t;

	/* Test the error checks:
	 *   1. NULL destination variable
	 *   2. Excel date out of range (greater than 2146790053)
	 *   3. Excel date equal to the fictitious "February 29, 1900" date.
	 */
	CU_ASSERT_EQUAL(xcl2abs(NULL, 0), 1);

	t = -32768;
	CU_ASSERT_EQUAL(xcl2abs(&t, 2146790054), 1);
	CU_TEST(t == -32768);

	t = -65536;
	CU_ASSERT_EQUAL(xcl2abs(&t, 60), 1);
	CU_TEST(t == -65536);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Run the tests */
		CU_ASSERT_EQUAL(xcl2abs(&t, test_data[i].excel_date), 0);
		CU_ASSERT_EQUAL(t, test_data[i].absolute_date);
	}

	return;
}

/**
 * \brief
 */
void
test_tm2rt_date(void) {
	struct tm t;
	uint16_t td;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable NULL
	 *   2. date before January 1, 1972
	 *   3. date after December 31, 2099
	 */
	td = 1701;

	/* Error: 1 */
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);
	CU_ASSERT_EQUAL(tm2rt_date(t, NULL), 1);
	
	/* Error: 2 */
	set_datetime(&t, 1970,  1,  1,  0,  0,  0);
	CU_ASSERT_EQUAL(tm2rt_date(t, &td), 1);
	CU_ASSERT_EQUAL(td, 1701);

	/* Error: 3 */
	set_datetime(&t, 2266,  9,  6, 19, 30,  0);
	CU_ASSERT_EQUAL(tm2rt_date(t, &td), 1);
	CU_ASSERT_EQUAL(td, 1701);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {

		/* Populate the source struct */
		set_date(&t, test_data[i].date.year, test_data[i].date.month,
		    test_data[i].date.day);

		/* Run the test, if possible */
		if (test_data[i].rt11.valid) {
			CU_ASSERT_EQUAL(tm2rt_date(t, &td), 0);
			CU_ASSERT_EQUAL(td, test_data[i].rt11.date_word);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_tm2rt_time(void) {
	struct tm t;
	uint16_t th, tl;
	int i;

	/* Run through the function error checks:
	 *   1. destination variables NULL
	 *     a. time_hi set NULL
	 *     b. time_lo set NULL
	 *     c. all set NULL
	 *   3. invalid clock frequency
	 *   4. invalid number of clock ticks
	 *     a. less than 0 ticks
	 *     b. more than 59 ticks at 60 Hz
	 *     c. more than 49 ticks at 50 Hz
	 */
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);
	th = 42;
	tl = 59650;

	/* Error: 1.A */
	CU_ASSERT_EQUAL(tm2rt_time(t, 0, 60, NULL, &tl), 1);
	CU_ASSERT_EQUAL(th, 42);
	CU_ASSERT_EQUAL(tl, 59650);

	/* Error: 2.B */
	CU_ASSERT_EQUAL(tm2rt_time(t, 0, 60, &th, NULL), 1);
	CU_ASSERT_EQUAL(th, 42);
	CU_ASSERT_EQUAL(tl, 59650);

	/* Error: 2.C */
	CU_ASSERT_EQUAL(tm2rt_time(t, 0, 60, NULL, NULL), 1);
	CU_ASSERT_EQUAL(th, 42);
	CU_ASSERT_EQUAL(tl, 59650);

	/* Error: 3 */
	CU_ASSERT_EQUAL(tm2rt_time(t, 0, 0, &th, &tl), 1);
	CU_ASSERT_EQUAL(th, 42);
	CU_ASSERT_EQUAL(tl, 59650);

	/* Error: 4.A */
	CU_ASSERT_EQUAL(tm2rt_time(t, -1, 60, &th, &tl), 1);
	CU_ASSERT_EQUAL(th, 42);
	CU_ASSERT_EQUAL(tl, 59650);

	/* Error: 4.A */
	CU_ASSERT_EQUAL(tm2rt_time(t, 77, 50, &th, &tl), 1);
	CU_ASSERT_EQUAL(th, 42);
	CU_ASSERT_EQUAL(tl, 59650);

	/* Error: 4.C */
	CU_ASSERT_EQUAL(tm2rt_time(t, 77, 60, &th, &tl), 1);
	CU_ASSERT_EQUAL(th, 42);
	CU_ASSERT_EQUAL(tl, 59650);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the source struct */
		set_time(&t, test_data[i].date.hour, test_data[i].date.minute,
		    test_data[i].date.second);

		/* Run the test */
		CU_ASSERT_EQUAL(tm2rt_time(t, test_data[i].rt11.ticks,
		    test_data[i].rt11.frequency, &th, &tl), 0);
		CU_ASSERT_EQUAL(th, test_data[i].rt11.time_word_hi);
		CU_ASSERT_EQUAL(tl, test_data[i].rt11.time_word_lo);
	}

	return;
}

/**
 * \brief
 */
void
test_rt2tm_date(void) {
	struct tm t, v;
	int i;

	/* Run through the function error checks:
	 *   1. the destination variable is set to NULL
	 *   2. invalid date word
	 *     a. month < 1
	 *     b. month > 12
	 *     c. day < 1
	 *     d. day > end_of_month
	 */
	memset(&t, 0, sizeof(struct tm));
	memset(&v, 0, sizeof(struct tm));
	set_datetime(&t, 2047,  9,  9, 18, 50, 19);
	set_datetime(&v, 2047,  9,  9, 18, 50, 19);

	/* Error: 1 */
	CU_ASSERT_EQUAL(rt2tm_date(NULL, 24297), 1);

	/* Error: 2.A */
	CU_ASSERT_EQUAL(rt2tm_date(&t, 33670), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.B */
	CU_ASSERT_EQUAL(rt2tm_date(&t, 14075), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.C */
	CU_ASSERT_EQUAL(rt2tm_date(&t, 52253), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.D */
	CU_ASSERT_EQUAL(rt2tm_date(&t, 35771), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	CU_ASSERT_EQUAL(rt2tm_date(&t, 19420), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	CU_ASSERT_EQUAL(rt2tm_date(&t, 54266), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	CU_ASSERT_EQUAL(rt2tm_date(&t,  7148), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	CU_ASSERT_EQUAL(rt2tm_date(&t, 10214), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	CU_ASSERT_EQUAL(rt2tm_date(&t, 28641), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Iterate through the test dataset */
	memset(&v, 0, sizeof(struct tm));
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the source struct */
		set_date(&v, test_data[i].date.year, test_data[i].date.month,
		    test_data[i].date.day);

		/* Run the test, if possible */
		if (test_data[i].rt11.valid) {
			CU_ASSERT_EQUAL(rt2tm_date(&t,
			    test_data[i].rt11.date_word), 0);
			CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_rt2tm_time(void) {
	struct tm t, v;
	int i, tk;

	/* Run through the function error checks:
	 *   1. any combination of destination variable set to NULL
	 *     a. dt set to NULL
	 *     b. ticks set to NULL
	 *     c. both set to NULL
	 *   2. invalid clock frequency
	 *   3. invalid time words
	 *     a. time_hi|time_lo > 5183999 when frequency is 60 Hz
	 *     b. time_hi|time_lo > 4319999 when frequency is 50 Hz
	 */
	memset(&v, 0, sizeof(struct tm));
	memset(&t, 0, sizeof(struct tm));
	set_datetime(&v, 2084,  5, 27, 22,  3, 27);
	set_datetime(&t, 2084,  5, 27, 22,  3, 27);
	tk = 38;

	/* Error: 1.A */
	CU_ASSERT_EQUAL(rt2tm_time(NULL, &tk, 50, 61, 36923), 1);
	CU_ASSERT_EQUAL(tk, 38);

	/* Error: 1.B */
	CU_ASSERT_EQUAL(rt2tm_time(&t, NULL, 60, 21, 54511), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 1.C */
	CU_ASSERT_EQUAL(rt2tm_time(NULL, NULL, 60, 29, 53232), 1);

	/* Error: 2 */
	CU_ASSERT_EQUAL(rt2tm_time(&t, &tk, 0, 38, 27203), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	CU_ASSERT_EQUAL(tk, 38);

	/* Error: 3.A */
	CU_ASSERT_EQUAL(rt2tm_time(&t, &tk, 60, 79, 6656), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	CU_ASSERT_EQUAL(tk, 38);

	/* Error: 3.B */
	CU_ASSERT_EQUAL(rt2tm_time(&t, &tk, 50, 65, 60160), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	CU_ASSERT_EQUAL(tk, 38);

	/* Iterate through the test dataset */
	memset(&v, 0, sizeof(struct tm));
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the source struct */
		set_time(&v, test_data[i].date.hour, test_data[i].date.minute,
		    test_data[i].date.second);

		/* Run the test */
		CU_ASSERT_EQUAL(rt2tm_time(&t, &tk, test_data[i].rt11.frequency,
		    test_data[i].rt11.time_word_hi,
		    test_data[i].rt11.time_word_lo), 0);
		CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
		CU_ASSERT_EQUAL(tk, test_data[i].rt11.ticks);
	}

	return;
}

/**
 * \brief
 */
void
test_tm2rsts_date(void) {
	struct tm t;
	uint16_t td;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable NULL
	 *   2. date out of range
	 *     a. date before January 1, 1970
	 *     b. date after December 31, 2035
	 */
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);
	td = 1701;

	/* Error: 1 */
	CU_ASSERT_EQUAL(tm2rsts_date(t, NULL), 1);

	/* Error: 2.A */
	set_datetime(&t, 1965,  9, 23, 11, 41,  4);
	CU_ASSERT_EQUAL(tm2rsts_date(t, &td), 1);
	CU_ASSERT_EQUAL(td, 1701);

	/* Error: 2.B */
	set_datetime(&t, 2038,  1, 19,  3, 14,  8);
	CU_ASSERT_EQUAL(tm2rsts_date(t, &td), 1);
	CU_ASSERT_EQUAL(td, 1701);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the source struct */
		set_date(&t, test_data[i].date.year, test_data[i].date.month,
		    test_data[i].date.day);

		/* Run the test, if possible */
		if (test_data[i].rsts_e.valid) {
			CU_ASSERT_EQUAL(tm2rsts_date(t, &td), 0);
			CU_ASSERT_EQUAL(td, test_data[i].rsts_e.date_word);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_tm2rsts_time(void) {
	struct tm t;
	uint16_t tt;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable NULL
	 */
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);

	/* Error: 1 */
	CU_ASSERT_EQUAL(tm2rsts_time(t, NULL), 1);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the source struct */
		set_time(&t, test_data[i].date.hour, test_data[i].date.minute,
		    test_data[i].date.second);

		/* Run the test */
		CU_ASSERT_EQUAL(tm2rsts_time(t, &tt), 0);
		CU_ASSERT_EQUAL(tt, test_data[i].rsts_e.time_word);
	}

	return;
}

/**
 * \brief
 */
void
test_rsts2tm_date(void) {
	struct tm t, v;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable set to NULL
	 *   2. invalid date word
	 *     a. day < 1
	 *     b. day > 365, in a regular year
	 *     c. day > 366, in a leap year
	 */
	memset(&v, 0, sizeof(struct tm));
	memset(&t, 0, sizeof(struct tm));
	set_datetime(&v, 1987, 10, 28, 13,  0,  0);
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);

	/* Error: 1 */
	CU_ASSERT_EQUAL(rsts2tm_date(NULL, 17301), 1);

	/* Error: 2.A */
	CU_ASSERT_EQUAL(rsts2tm_date(&t, 17000), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.B */
	CU_ASSERT_EQUAL(rsts2tm_date(&t, 17366), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.C */
	CU_ASSERT_EQUAL(rsts2tm_date(&t, 18367), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Iterate through the test dataset */
	memset(&v, 0, sizeof(struct tm));
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the date structure */
		set_date(&v, test_data[i].date.year, test_data[i].date.month,
		    test_data[i].date.day);

		/* Run the test */
		if (test_data[i].rsts_e.valid) {
			CU_ASSERT_EQUAL(rsts2tm_date(&t,
			    test_data[i].rsts_e.date_word), 0);
			CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_rsts2tm_time(void) {
	struct tm t, v;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable (tm) set to NULL
	 *   2. invalid time word
	 *     a. time < 1
	 *     b. time > 1440
	 */
	memset(&v, 0, sizeof(struct tm));
	memset(&t, 0, sizeof(struct tm));
	set_datetime(&v, 1987, 10, 28, 13,  0,  0);
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);

	/* Error: 1 */
	CU_ASSERT_EQUAL(rsts2tm_time(NULL, 660), 1);
	
	/* Error: 2.A */
	CU_ASSERT_EQUAL(rsts2tm_time(&t, 0), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.A */
	CU_ASSERT_EQUAL(rsts2tm_time(&t, 1441), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Iterate through the test dataset */
	memset(&v, 0, sizeof(struct tm));
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the time structure */
		set_time(&v, test_data[i].date.hour, test_data[i].date.minute,
		    0);

		/* Run the test */
		CU_ASSERT_EQUAL(rsts2tm_time(&t, test_data[i].rsts_e.time_word),
		    0);
		CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	}

	return;
}

/**
 * \brief
 */
void
test_tm2dos_date(void) {
	struct tm t;
	uint16_t td;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable NULL
	 *   2. date before January 1, 1980
	 *   3. date after December 31, 2107
	 */
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);
	td = 1701;
	CU_ASSERT_EQUAL(tm2dos_date(t, NULL), 1);

	set_datetime(&t, 1970,  1,  1,  0,  0,  0);
	CU_ASSERT_EQUAL(tm2dos_date(t, &td), 1);
	CU_ASSERT_EQUAL(td, 1701);

	set_datetime(&t, 2266,  9,  6, 19, 30,  0);
	CU_ASSERT_EQUAL(tm2dos_date(t, &td), 1);
	CU_ASSERT_EQUAL(td, 1701);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		set_date(&t, test_data[i].date.year,
		    test_data[i].date.month, test_data[i].date.day);

		/* Run the test */
		if (test_data[i].dos.valid) {
			CU_ASSERT_EQUAL(tm2dos_date(t, &td), 0);
			CU_ASSERT_EQUAL(td, test_data[i].dos.date_word);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_tm2dos_time(void) {
	struct tm t;
	uint16_t tt;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable NULL
	 */
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);
	tt = 35264;
	CU_ASSERT_EQUAL(tm2dos_time(t, NULL), 1);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		set_time(&t, test_data[i].date.hour, test_data[i].date.minute,
		    test_data[i].date.second);

		/* Run the test */
		CU_ASSERT_EQUAL(tm2dos_time(t, &tt), 0);
		CU_ASSERT_EQUAL(tt, test_data[i].dos.time_word);
	}

	return;
}

/**
 * \brief
 */
void
test_dos2tm_date(void) {
	struct tm t, v;
	int i;

	/* Run through the function error checks:
	 *   1. the destination variable is set to NULL
	 *   2. invalid date word
	 *     a. month < 1
	 *     b. month > 12
	 *     c. day < 1
	 *     d. day > end_of_month
	 */
	memset(&t, 0, sizeof(struct tm));
	memset(&v, 0, sizeof(struct tm));
	set_datetime(&t, 2047,  9,  9, 18, 50, 19);
	set_datetime(&v, 2047,  9,  9, 18, 50, 19);

	/* Error: 1 */
	CU_ASSERT_EQUAL(dos2tm_date(NULL, 24297), 1);

	/* Error: 2.A */
//	CU_ASSERT_EQUAL(dos2tm_date(&t, 33670), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.B */
//	CU_ASSERT_EQUAL(dos2tm_date(&t, 14075), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.C */
//	CU_ASSERT_EQUAL(dos2tm_date(&t, 52253), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.D */
//	CU_ASSERT_EQUAL(dos2tm_date(&t, 35771), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(dos2tm_date(&t, 19420), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(dos2tm_date(&t, 54266), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(dos2tm_date(&t,  7148), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(dos2tm_date(&t, 10214), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(dos2tm_date(&t, 28641), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Iterate through the test dataset */
	memset(&v, 0, sizeof(struct tm));
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the source struct */
		set_date(&v, test_data[i].date.year, test_data[i].date.month,
		    test_data[i].date.day);

		/* Run the test, if possible */
		if (test_data[i].dos.valid) {
			CU_ASSERT_EQUAL(dos2tm_date(&t,
			    test_data[i].dos.date_word), 0);
			CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_dos2tm_time(void) {
	struct tm t, v;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable (dt) set to NULL
	 *   2. invalid time word
	 *     a. hour > 23
	 *     b. minute > 59
	 *     c. second > 59
	 */
	memset(&v, 0, sizeof(struct tm));
	memset(&t, 0, sizeof(struct tm));
	set_datetime(&v, 1987, 10, 28, 13,  0,  0);
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);

	/* Error: 1 */
	CU_ASSERT_EQUAL(dos2tm_time(NULL, 660), 1);
	
	/* Error: 2.A */
	CU_ASSERT_EQUAL(dos2tm_time(&t, 49967), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.B */
	CU_ASSERT_EQUAL(dos2tm_time(&t, 12189), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.C */
	CU_ASSERT_EQUAL(dos2tm_time(&t, 21854), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Iterate through the test dataset */
	memset(&v, 0, sizeof(struct tm));
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the source struct; mask off the least significant
		 * bit of the second, since it is not possible for the MS-DOS
		 * timestamp to store the least significant bit.
		 */
		set_time(&v, test_data[i].date.hour, test_data[i].date.minute,
		    (test_data[i].date.second & 0x3E));

		/* Run the test */
		CU_ASSERT_EQUAL(dos2tm_time(&t, test_data[i].dos.time_word), 0);
		CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	}

	return;
}

/**
 * \brief
 */
void
test_tm2cpm_date(void) {
	struct tm t;
	uint16_t td;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable NULL
	 *   2. date before January 1, 1978
	 *   3. date after June 6, 2157
	 */
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);
	td = 1701;
	CU_ASSERT_EQUAL(tm2cpm_date(t, NULL), 1);

	set_datetime(&t, 1970,  1,  1,  0,  0,  0);
	CU_ASSERT_EQUAL(tm2cpm_date(t, &td), 1);
	CU_ASSERT_EQUAL(td, 1701);

	set_datetime(&t, 2266,  9,  6, 19, 30,  0);
	CU_ASSERT_EQUAL(tm2cpm_date(t, &td), 1);
	CU_ASSERT_EQUAL(td, 1701);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		set_date(&t, test_data[i].date.year,
		    test_data[i].date.month, test_data[i].date.day);

		/* Run the test */
		if (test_data[i].cpm.valid) {
			CU_ASSERT_EQUAL(tm2cpm_date(t, &td), 0);
			CU_ASSERT_EQUAL(td, test_data[i].cpm.date_word);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_tm2cpm_time(void) {
	struct tm t;
	uint16_t tt;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable NULL
	 */
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);
	tt = 35264;
	CU_ASSERT_EQUAL(tm2cpm_time(t, NULL), 1);

	/* Iterate through the test dataset */
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		set_time(&t, test_data[i].date.hour, test_data[i].date.minute,
		    test_data[i].date.second);

		/* Run the test */
		CU_ASSERT_EQUAL(tm2cpm_time(t, &tt), 0);
		CU_ASSERT_EQUAL(tt, test_data[i].cpm.time_word);
	}

	return;
}

/**
 * \brief
 */
void
test_cpm2tm_date(void) {
	struct tm t, v;
	int i;

	/* Run through the function error checks:
	 *   1. the destination variable is set to NULL
	 *   2. invalid date word
	 *     a. month < 1
	 *     b. month > 12
	 *     c. day < 1
	 *     d. day > end_of_month
	 */
	memset(&t, 0, sizeof(struct tm));
	memset(&v, 0, sizeof(struct tm));
	set_datetime(&t, 2047,  9,  9, 18, 50, 19);
	set_datetime(&v, 2047,  9,  9, 18, 50, 19);

	/* Error: 1 */
//	CU_ASSERT_EQUAL(cpm2tm_date(NULL, 24297), 1);

	/* Error: 2.A */
//	CU_ASSERT_EQUAL(cpm2tm_date(&t, 33670), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.B */
//	CU_ASSERT_EQUAL(cpm2tm_date(&t, 14075), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.C */
//	CU_ASSERT_EQUAL(cpm2tm_date(&t, 52253), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.D */
//	CU_ASSERT_EQUAL(cpm2tm_date(&t, 35771), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(cpm2tm_date(&t, 19420), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(cpm2tm_date(&t, 54266), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(cpm2tm_date(&t,  7148), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(cpm2tm_date(&t, 10214), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
//	CU_ASSERT_EQUAL(cpm2tm_date(&t, 28641), 1);
//	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Iterate through the test dataset */
	memset(&v, 0, sizeof(struct tm));
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the source struct */
		set_date(&v, test_data[i].date.year, test_data[i].date.month,
		    test_data[i].date.day);

		/* Run the test, if possible */
		if (test_data[i].cpm.valid) {
			CU_ASSERT_EQUAL(cpm2tm_date(&t,
			    test_data[i].cpm.date_word), 0);
			CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
		}
	}

	return;
}

/**
 * \brief
 */
void
test_cpm2tm_time(void) {
	struct tm t, v;
	int i;

	/* Run through the function error checks:
	 *   1. destination variable (dt) set to NULL
	 *   2. invalid time word
	 *     a. hour > 23
	 *     b. minute > 59
	 *     c. second > 59
	 */
	memset(&v, 0, sizeof(struct tm));
	memset(&t, 0, sizeof(struct tm));
	set_datetime(&v, 1987, 10, 28, 13,  0,  0);
	set_datetime(&t, 1987, 10, 28, 13,  0,  0);

	/* Error: 1 */
	CU_ASSERT_EQUAL(cpm2tm_time(NULL, 660), 1);
	
	/* Error: 2.A */
	CU_ASSERT_EQUAL(cpm2tm_time(&t, 49967), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.B */
	CU_ASSERT_EQUAL(cpm2tm_time(&t, 12189), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Error: 2.C */
	CU_ASSERT_EQUAL(cpm2tm_time(&t, 21854), 1);
	CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);

	/* Iterate through the test dataset */
	memset(&v, 0, sizeof(struct tm));
	for (i = 0; i < DATE_TIME_TESTS; ++i) {
		/* Populate the source struct; given CP/M time words can only
		 * represent hours and minutes, we set seconds as zero.
		 */
		set_time(&v, test_data[i].date.hour, test_data[i].date.minute,
		    0);

		/* Run the test */
		CU_ASSERT_EQUAL(cpm2tm_time(&t, test_data[i].cpm.time_word), 0);
		CU_ASSERT_EQUAL(memcmp(&t, &v, sizeof(struct tm)), 0);
	}

	return;
}
