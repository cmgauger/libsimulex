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
 * \file	date_time.c
 * \copyright	MIT
 * \date	2024
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 *
 * \brief conversion routines for various date and/or time representations
 *
 * The functions in this module convert between several different date and/or
 * time representations; the primary representation forms for dates and times
 * are either:
 * <ol>
 *     <li>The standard <tt>struct tm</tt> structure from the standard library.
 *     With an "extended" interpretation of the <tt>tm_year</tt> component.</li>
 *     <li>One or both of an "absolute date" value (represented as a signed
 *     integer) and a "time of day" value (represented as a floating point
 *     number).</li>
 * </ol>
 * Algorithms are implemented which convert between the above representation
 * forms, as well as to other representations.  The other representations refer
 * to:
 * <ol>
 *     <li>Generic, numerical representations:
 *     <ol>
 *         <li>Modified Julian Date, the "reduced" version of the Julian Date
 * as introduced by the Smithsonian Astrophysical Observatory in 1957.</li>
 *         <li>Lilian Date, the count of days in the Gregorian calendar.  That
 * is to say, the number of days starting with October 15, 1582 (inclusive),
 * which is when the Gregorian calendar was introduced.</li>
 *         <li>Microsoft Excel <tt>DATE()</tt>, an almost continuous count of
 * days from (and including) January 1, 1900; with a discontinuity of one day
 * between the dates of February 28, 1900 and March 1, 1900 due to the erroneous
 * handling of leap years originating in Lotus 1-2-3.</li>
 *     </ol></li>
 *     <li>Operating system/file system internal representations:
 *         <li>Digital Equipment Corporation RT-11 date and time format,
-----------------------------------------------------------+-----------+-------+
 *         <li>Digital Equipment Corporation RSTS/E date and time format,
-----------------------------------------------------------+-----------+-------+
 *         <li>Digital Research CP/M v3.1 (and C.B. Falconer's DOS+ v2.5 for
 * CP/M v2.2) date and time format,
-----------------------------------------------------------+-----------+-------+
 *         <li>Microsoft MS-DOS date and time format,
-----------------------------------------------------------+-----------+-------+
 *     </ol></li>
 * </ol>
 * 
-----------------------------------------------------------+-----------+-------+
 */
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <date_time.h>

const int days_in_month[2][12] = {
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

const int ordinal_days[2][12] = {
	{  0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334},
	{  0,  31,  60,  91, 121, 152, 182, 213, 244, 273, 305, 335}
};

/**
 * \brief integer floored division function
 *
 * Calculates the division of the numerator by the denominator and returns the
 * largest integer less than or equal to the result.  Two error conditions are
 * verified before the calculation is run, namely that of the denominator being
 * zero (to avoid division by zero) and that of the numerator being equal to
 * <tt>INT_MIN</tt> (to avoid overflow); if either error condition is valid, a
 * zero is returned.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * \ref rmq
 *
 *
 *
 * \return largest integer less than or equal to the numerator divided by the
 *     denominator; zero if an erroneous input is provided
 *
 * \param[in]  numerator  dividend
 * \param[in]  denominator  divisor
 */
static int
flr(int numerator, int denominator) {
	if ((denominator == 0) || (numerator == INT_MIN))
		return 0;

	return (numerator / denominator - (((numerator % denominator) != 0) &&
	    ((numerator ^ denominator) < 0)));
}

/**
 * \brief integer modulo function, Knuth's floored division type
 *
 * Calculates the modulo of the numerator divided by the denominator, using
 * Knuth's floored division algorithm, that is:
 * \f[
 *     x - y \left\lfloor \frac{x}{y} \right\rfloor
 * \f]
 * Two error conditions are verified before the calculation is run, namely: that
 * of the denominator being zero (to avoid division by zero), and that of the
 * numerator being equal to <tt>INT_MIN</tt> (to avoid overflow). If either
 * error condition is encountered, a zero is returned.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * \ref rmq
 *
 *
 *
 * \return modulo of the numerator divided by the denominator; zero if an
 *     erroneous input is provided
 *
 * \param[in]  numerator  dividend
 * \param[in]  denominator  divisor
 */
static int
mod(int numerator, int denominator) {
	int64_t n, d, m;
	int f;

	if ((denominator == 0) || (numerator == INT_MIN))
		return 0;

	n = numerator;
	d = denominator;
	f = flr(numerator, denominator);

	m = (n - (d * f));

	if ((m < INT_MIN) || (m > INT_MAX))
		return 0;

	return (int) m;
}

/**
 * \brief combined integer divison and modulo function
 *
 * This function combines the functions of the <tt>flr</tt> and <tt>mod</tt>
 * functions previously defined, to provide a less verbose method of calling
 * both if both the quotient and remainder of the division are required.  As
 * this function wraps <tt>flr</tt> and <tt>mod</tt> the error checks applied
 * to both, apply to this function.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * \ref flr
 * \ref mod
 *
 *
 *
 * \return quotient of the numerator divided by the denominator; zero if an
 *     erroneous input is provided
 *
 * \param[in]  numerator  dividend
 * \param[in]  denominator  divisor
 * \param[out]  remainder  remainder
 * \param[out]  quotient  quotient
 */
static int
rmq(int numerator, int denominator, int *remainder, int *quotient) {
	int r, q;

	q = flr(numerator, denominator);
	r = mod(numerator, denominator);

	if (remainder != NULL)
		*remainder = r;

	if (quotient != NULL)
		*quotient = q;

	return q;
}

/**
 * \brief leap year test routine
 *
 * This routine tests if a specified year is a leap year in the proleptic
 * Gregorian calendar.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 *
 *
 * \return 0 if regular year, 1 if leap year
 *
 * \param[in]  year  year, in the proleptic Gregorian calendar
 */
static int
leap(int year) {
	return ((mod(year, 4) == 0) && ((mod(year, 100) != 0) ||
	    (mod(year, 400) == 0)));
}

/**
 * \brief <tt>struct tm</tt> date validation routine
 *
 * This function validates that a date stored in a <tt>struct tm</tt> structure
 * is valid.  "Validity" being defined as having values in the correct ranges
 * for the year, month, and day variables.  Only the year, month, and day values
 * of the <tt>struct tm</tt> structure are verified, the remainder of the
 * structure is ignored.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 *
 *
 * \return 1 if valid; 0 if not
 *
 * \param[in]  dt  date/time structure to be validated
 */
int
valid_date(struct tm dt) {
	int l;
	
	/* Verify that tm_year is within an acceptable range. */
	if (dt.tm_year > 2147481747)
		return 0;

	/* Establish if the year specified is a leap year Gregorian calendar. */
	l = leap(dt.tm_year + 1900);

	/* Check that the tm_mon, tm_mday values are all in valid ranges -- this
	 * is what we needed the leap year data for.  The correctness of the
	 * tm_year value was established earlier.
	 */
	if ((dt.tm_mon < 0) || (dt.tm_mon > 11) ||
	    (dt.tm_mday < 1) || (dt.tm_mday > days_in_month[l][dt.tm_mon]))
		return 0;

	return 1;
}

/**
 * \brief <tt>struct tm</tt> time validation routine
 *
 * This function validates that a time stored in a <tt>struct tm</tt> structure
 * is valid.  The definition of "validity" being that the value of the hour,
 * minute, and second variables are within the correct ranges.  This function
 * does not attempt to account for leap seconds, and considers <tt>tm_sec</tt>
 * being set to a value of 60 to be an error.  Only the hour, minute, and second
 * values of the <tt>struct tm</tt> structure are verified, the remainder of the
 * structure is ignored.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}

 * \endcode
 *
 *
 *
 * \return 1 if valid; 0 if not
 *
 * \param[in]  dt  date/time structure to be validated
 */
int
valid_time(struct tm dt) {
	/* Check that the tm_hour, tm_min, and tm_sec values are all in valid
	 * ranges.  Leap seconds are not recognized, as stated previously.
	 */
	if ((dt.tm_hour < 0) || (dt.tm_hour > 23) ||
	    (dt.tm_min < 0) || (dt.tm_min > 59) ||
	    (dt.tm_sec < 0) || (dt.tm_sec > 59))
		return 0;

	return 1;
}

/**
 * \brief converts an absolute date to <tt>struct tm</tt> date
 *
 * Converts an absolute date, that is: a count of days from midnight January 1,
 * 1 on the proleptic Gregorian calendar (inclusive), to a date in a <tt>struct
 * tm</tt>.  The algorithm is modified from that in Montenbruck and Pfleger's
 * "Astronomy on the Personal Computer" used to convert a Modified Julian Date
 * to a year, month, and day tuple.
 *
 * Absolute date values are constrained to the range -109094767 to 105649066,
 * inclusive. This is to avoid arithmetic overflows during calculation.  The
 * pointer to the output <tt>struct tm</tt> is checked to ensure it is not null,
 * as a simple sanity check.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 521 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>tm2abs</tt>.
 *
 * \cite montenbruck1994
 * \ref tm2abs
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[in]  abs  absolute date containing the source date value
 * \param[out]  dt  <tt>struct tm</tt> containing the converted date value
 */
int abs2tm(int abs, struct tm *dt) {
	int a, b, c, d, e, f;
	int dd, mm, yy;
	
	if ((dt == NULL) || (abs > 105649066) || (abs < -109094767))
		return 1;

	a = abs + 1721425;
	b = flr(4 * a - 7468865, 146097);
	c = a + b - flr(b, 4) + 1525;
	d = flr(20 * c - 2442, 7305);
	e = 365 * d + flr(d, 4);
	f = flr(10000 * (c - e), 306001);

	dd = c - e - flr(306001 * f, 10000);
	mm = f - 1 - 12 * flr(f, 14);
	yy = d - 4715 - flr(7 + mm, 10);

	memset(dt, 0, sizeof(struct tm));
	dt -> tm_year = yy - 1900;
	dt -> tm_mon  = mm - 1;
	dt -> tm_mday = dd;

	return 0;
}

/**
 * \brief converts a <tt>struct tm</tt> date to an absolute date
 *
 * This function converts a year, month, and day tuple, as stored in a
 * <tt>struct tm</tt> into an absolute date.  The algorithm used is a
 * translation of that used in Emacs for the same purpose, which appears to be
 * an adaptation of the algorithm in "Calendrical Calculations" by Dershowitz
 * and Reingold.  The stored year, month, and day are assumed to be on the
 * proleptic Gregorian calendar.
 *
 * The input to this function is restricted, such that the date value in the
 * input <tt>struct tm</tt> must be valid; further to prevent arithmetic
 * overflows, the year must be in the range -5879488 to 5879490, inclusive.  The
 * pointer to the absolute date output value is tested to ensure it is not null.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>abs2tm</tt>.
 *
 * \cite dershowitz2008
 * \ref abs2tm
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[out]  abs  absolute date containing the converted date value
 * \param[in]  dt  <tt>struct tm</tt> containing the source date value
 */
int tm2abs(int *abs, struct tm dt) {
	int day_count, dd, mm, yy;
	
	if ((abs == NULL) || !valid_date(dt) ||
	    (dt.tm_year > 5877590) || (dt.tm_year < -5881388))
		return 1;

	yy = dt.tm_year + 1899;
	mm = dt.tm_mon;
	dd = dt.tm_mday;

	day_count = (yy * 365) + flr(yy, 4) - flr(yy, 100) + flr(yy, 400) + dd +
	    (31 * mm) - ((mm > 1) ? ((23 + 4 * (mm + 1)) / 10) : 0) +
	    ((mm > 1) ? leap(yy + 1) : 0);

	*abs = day_count;

	return 0;
}

/**
 * \brief converts an absolute date to a Modified Julian Date
 *
 * This function performs the conversion between an absolute date value, and a
 * Modified Julian Date.  The Modified Julian Date is a date representation
 * invented by the Smithsonian Astrophysical Observatory in the 1957; the MJD
 * being the count of days since midnight November 17, 1858.
 *
 * The conversion is a simple offset, and as such is implemented as a simple
 * subtraction.
 *
 * To guard against signed overflow, the absolute date value is checked to
 * ensure it is larger than -2146805072.  If an error condition occurs, the
 * conversion will not proceed.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>mjd2abs</tt>.
 *
 * \ref mjd2abs
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[in]  abs  absolute date containing the source date value
 * \param[out]  mjd  Modified Julian Date containing the converted date value
 */
int abs2mjd(int abs, int *mjd) {
	if ((mjd == NULL) || (abs < -2146805072))
		return 1;

	*mjd = abs - 678576;

	return 0;
}

/**
 * \brief converts a Modified Julian Date to an absolute date
 *
 * This function performs the conversion of a Modified Julian Date value to an
 * absolute date value.
 *
 * The conversion is a simple offset, and as such is implemented as a simple
 * addition.
 *
 * To guard against signed overflow, the Modified Julian Date value is checked
 * to ensure it is less than 2146805071.  If an error condition occurs, the
 * conversion will not proceed.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>abs2mjd</tt>.
 *
 * \ref abs2mjd
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[out]  abs  absolute date containing the converted date value
 * \param[in]  mjd  Modified Julian Date containing the source date value
 */
int mjd2abs(int *abs, int mjd) {
	if ((abs == NULL) || (mjd > 2146805071))
		return 1;

	*abs = mjd + 678576;

	return 0;
}

/**
 * \brief converts an absolute date to a Lilian Date
 *
 * This function converts an absolute date value to a Lilian date value.  A
 * Lilian date being similar in concept to the absolute date, though with a
 * different epoch: the number of days since the beginning of the Gregorian
 * calendar (that is, the number of days since October 15, 1582).
 *
 * The conversion is a simple offset, and as such is implemented as a simple
 * subtraction.
 *
 * To guard against signed overflow, the absolute date value is checked to
 * ensure it is larger than -2146905913.  If an error condition occurs, the
 * conversion will not proceed.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>lil2abs</tt>.
 *
 * \ref lil2abs
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[in]  abs  absolute date containing the source date value
 * \param[out]  lil  Lilian date containing the converted date value
 */
int abs2lil(int abs, int *lil) {
	if ((lil == NULL) || (abs < -2146905913))
		return 1;

	*lil = abs - 577735;

	return 0;
}

/**
 * \brief converts a Lilian date to an absolute date
 *
 * This function comverts a Lilian date value to am absolute date value.
 *
 * The conversion is a simple offset, and as such is implemented as a simple
 * subtraction.
 *
 * To guard against signed overflow, the Lilian date value is checked to ensure
 * it is less than 2146905912.  If an error condition occurs, the conversion
 * will not proceed.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>abs2lil</tt>.
 *
 * \ref abs2lil
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[out]  abs  absolute date containing the converted date value
 * \param[in]  lil  Lilian date containing the source date value
 */
int lil2abs(int *abs, int lil) {
	if ((abs == NULL) || (lil > 2146905912))
		return 1;

	*abs = lil + 577735;

	return 0;
}

/**
 * \brief converts an absolute date to a Excel Date
 *
 * This function converts an absolute date value to the Microsoft Office Excel
 * date value.  The Excel date value almost, but not quite, corresponds to a
 * count of days from January 1, 1900 (inclusive); save that in Microsoft's
 * original implementation of Excel, they incorrectly implemented 1900 as a leap
 * year. 
 *
 * The conversion is relatively simple, first offsetting the absolute date value
 * into the appropriate range using a simple subtraction; then if the resulting
 * date corresponds to February 28, 1900 the date value is decremented by one.
 *
 * To guard against signed overflow, the absolute date value is checked to
 * ensure it is greater than -2146790053.  If an error condition occurs, the
 * conversion will not proceed.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>xcl2abs</tt>.
 *
 * \ref xcl2abs
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[in]  abs  absolute date containing the source date value
 * \param[out]  xcl  Excel date containing the converted date value
 */
int
abs2xcl(int abs, int *xcl) {
	if ((xcl == NULL) || (abs < -2146790053))
		return 1;

	*xcl = abs - 693594;

	if (abs < 693655)
		(*xcl)--;

	return 0;
}

/**
 * \brief converts an Excel date to an absolute date
 *
 * This function performs the conversion between a Microsoft Office Excel date
 * value and an absolute date value.
 *
 * The conversion process is relatively simple, first checking whether the Excel
 * date value corresponds to a date before and including February 28, 1900 or a
 * a date after and including March 1, 1900; the correct offset is then added to
 * the value.
 *
 * To guard against signed overflow, the Excel date value is checked to ensure
 * it is smaller than 2146790053.  Further, the Excel date value is checked to
 * ensure that it does not correspond to the erroneous "February 29, 1900" date.
 * If an error condition occurs, the conversion will not proceed.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>abs2xcl</tt>.
 *
 * \ref abs2xcl
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[out]  abs  absolute date containing the converted date value
 * \param[in]  lil  Excel date containing the source date value
 */
int
xcl2abs(int *abs, int xcl) {
	if ((abs == NULL) || (xcl > 2146790053) || (xcl == 60))
		return 1;

	if (xcl > 60) {
		*abs = xcl + 693594;
	} else if (xcl < 60) {
		*abs = xcl + 693595;
	} else {
		/* This condition should never be reached. */
		return 1;
	}

	return 0;
}

/**
 * \brief convert a <tt>struct tm</tt> date into an RT-11 date word
 *
 * This function converts a date stored in a <tt>struct tm</tt> structure into
 * an RT-11 date word.  The format of the RT-11 date word is according to that
 * which is documented in the "RT-11 System Macro Library Manual."
 *
 * RT-11 can only represent dates between January 1, 1972 and December 31, 2099
 * (inclusive), thus beyond checking that the date_word pointer is valid, and
 * that the date is valid, the error checks ensure that the date is within the
 * representable range.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>rt2tm_date</tt>.
 *
 * \cite rt11sml
 * \ref rt2tm_date
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[out]  date_word  word containing the converted date value
 * \param[in]  dt  <tt>struct tm</tt> containing the source date value
 */
int
tm2rt_date(struct tm dt, uint16_t *date_word) {
	uint16_t temp_date;
	int y;

	if ((date_word == NULL) || !valid_date(dt) || (dt.tm_year < 72) ||
	    (dt.tm_year > 199))
		return 1;

	y = dt.tm_year - 72;
	
	temp_date = (uint16_t) (((y & 0x60) <<  9) | (((dt.tm_mon + 1) &
	    0x0F) << 10) | ((dt.tm_mday & 0x1F) <<  5) | (y & 0x1F));

	*date_word = temp_date;

	return 0;
}

/**
 * \brief convert a <tt>struct tm</tt> time into a pair of RT-11 time words
 *
 * This function converts the tuple of a time (stored in a <tt>struct tm</tt>),
 * a line frequency (of either 50 or 60 Hz), and a number of sub-second ticks,
 * to a pair of RT-11 time words as described in the "RT-11 System Macro Library
 * Manual."
 *
 * The errors which are checked for (and which will preclude a successful time
 * conversion) are:
 * <ul>
 *     <li>either destination date word pointer being <tt>NULL</tt></li>
 *     <li>the time to be converted being invalid</li>
 *     <li>the line frequency not being either 50 Hz or 60 Hz</li>
 *     <li>the number of sub-second ticks being less than zero, or greater than
 * the number of line-frequency clock pulses per second</li>
 * </ul>
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 509 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>rt2tm_time</tt>.
 *
 * \cite rt11sml
 * \ref rt2tm_time
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[out]  time_word_hi  word containing the high-order bits of the
 *     converted time value
 * \param[out]  time_word_lo  word containing the low-order bits of the
 *     converted time value
 * \param[in]  dt  <tt>struct tm</tt> containing the source time value
 * \param[in]  ticks  number of sub-second ticks of the line-time clock
 * \param[in]  frequency  line-time clock tick rate (in Hertz)
 */
int
tm2rt_time(struct tm dt, int ticks, int frequency, uint16_t *time_word_hi,
    uint16_t *time_word_lo) {
	uint32_t temp_time;
	if ((time_word_hi == NULL) || (time_word_lo == NULL) ||
	    !valid_time(dt) || (frequency != 50 && frequency != 60) ||
	    (ticks < 0) || (ticks > (frequency - 1)))
		return 1;

	temp_time = (uint32_t) (frequency * ((dt.tm_hour * 3600) + (dt.tm_min *
	    60) + dt.tm_sec) + ticks);

	*time_word_hi = (uint16_t) ((temp_time & 0xFFFF0000) >> 16);
	*time_word_lo = (uint16_t) (temp_time & 0x0000FFFF);

	return 0;
}

/**
 * \brief convert an RT-11 date word to a <tt>struct tm</tt> date
 *
 * This function will convert an RT-11 format date word into a date stored in
 * the standard library <tt>struct tm</tt> structure, so long as the RT-11 date
 * word is valid.  Refer to the "RT-11 System Macro Library Manual" for the
 * format of the RT-11 date word.
 *
 * The errors checked by this function are checked at two discrete points. The
 * first error check is to ensure that the structure in which the date will be
 * stored is present (or, at least, that the pointer passed to the function is
 * not <tt>NULL</tt>). The second set of error conditions are checked after the
 * date word is converted (but before the results are stored in the structure),
 * the resulting month and day values are checked to ensure they are valid.
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 521 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is <tt>rt2tm_date</tt>.
 *
 * \cite rt11sml
 * \ref rt2tm_date
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[in]  date_word  word containing the source date value
 * \param[out]  dt  <tt>struct tm</tt> containing the converted date value
 */
int
rt2tm_date(struct tm *dt, uint16_t date_word) {
	int yy, mm, dd;

	if (dt == NULL)
		return 1;

	yy = (((date_word >>  9) & 0x60) | (date_word & 0x1F)) + 1972;
	mm = ((date_word >> 10) & 0x0F);
	dd = ((date_word >>  5) & 0x1F);

	/* Because the valid range of years for an RT-11 date is between 1972
	 * and 2099 (inclusive), instead of the full Gregorian leap year check,
	 * a simple modulo 4 can be done.  Given that the day number in the date
	 * word is five bits long, it is impossible for January, March, May,
	 * July, August, October, or December to fail the second half of the day
	 * range test.
	 */
	if ((mm < 1 || mm > 12) || (dd < 1 || dd > days_in_month[(yy % 4) ==
	    0][mm - 1]))
		return 1;

	memset(dt, 0, sizeof(struct tm));
	dt -> tm_year = yy - 1900;
	dt -> tm_mon = mm - 1;
	dt -> tm_mday = dd;

	return 0;
}

/**
 * \brief convert a pair of RT-11 time words to a <tt>struct tm</tt> time
 *
 * This function will convert the tuple of a pair of RT-11 time words plus the
 * line-time clock frequency, into a time value stored in the standard library
 * <tt>struct tm</tt> structure plus a number of sub-second ticks.  The RT-11
 * time format is documented in the "RT-11 System Macro Library
 * Manual"\cite rt11sml.
 *
 * The errors which this function checks against are:
 * <ul>
 *     <li>the validity of the <tt>struct tm</tt> structure pointer</li>
 *     <li>the validity of the pointer to the sub-second ticks integer</li>
 *     <li>the validity of the line-frequency (either 50 Hz or 60 Hz)</li>
 *     <li>the combined time words are in a valid range for the specified line
 * frequency</li>
 * </ul>
 *
 * Results of running the Bounded Model Checker for C (CBMC) on this function:
 * \code{.unparsed}
** 0 of 521 failed (1 iterations)
VERIFICATION SUCCESSFUL
 * \endcode
 *
 * The inverse of this function is \ref rt2tm_time "<tt>tm2rt_time</tt>".
 *
 *
 *
 *
 * \return 0 if conversion successful; 1 if conversion failed
 *
 * \param[in]  time_word_hi  word containing the high-order bits of the source
 *     time value
 * \param[in]  time_word_lo  word containing the low-order bits of the source
 *     time value
 * \param[out]  dt  <tt>struct tm</tt> containing the source time value
 * \param[out]  ticks  number of sub-second ticks of the line-time clock
 * \param[in]  frequency  line-time clock tick rate (in Hertz)
 */
int
rt2tm_time(struct tm *dt, int *ticks, int frequency, uint16_t time_word_hi,
    uint16_t time_word_lo) {
	int32_t temp_time;
	int hh, mm, ss, tt;
	
	if (time_word_hi > 32767)
		return 1;

	temp_time = (int32_t) (((int32_t) time_word_hi << 16) |
	    ((int32_t) time_word_lo));

	if ((dt == NULL) || (ticks == NULL) || (frequency != 50 && frequency !=
	    60) || (frequency == 50 && temp_time > 4319999) || (frequency ==
	    60 && temp_time > 5183999))
		return 1;
	
	rmq(temp_time, frequency, &tt, &temp_time);
	rmq(temp_time, 60, &ss, &temp_time);
	rmq(temp_time, 60, &mm, &temp_time);
	rmq(temp_time, 24, &hh, &temp_time);

	if (temp_time != 0)
		return 1;
	
	memset(dt, 0, sizeof(struct tm));
	dt -> tm_hour = hh;
	dt -> tm_min = mm;
	dt -> tm_sec = ss;

	*ticks = tt;

	return 0;
}

/**
 * \brief convert a <tt>struct tm</tt> date to an RSTS/E date word
-----------------------------------------------------------+-----------+-------+
 */
int
tm2rsts_date(struct tm dt, uint16_t *date_word) {
	uint16_t t;
	int l, y;

	if ((date_word == NULL) || !valid_date(dt) ||
	    (dt.tm_year < 70 || dt.tm_year > 135))
		return 1;

	/* Normally calculating a leap year involves checking that the year is
	 * either divisible by 4 while not divisible by 100, 200, or 300; but
	 * as the range of valid years for a RSTS/E time stamp is only 1970 to
	 * 2035, one can simply the test to a simple divisible-by-4 check.
	*/
	l = ((dt.tm_year + 1900) % 4) == 0;

	y = dt.tm_year - 70;

	t = (uint16_t) ((y * 1000) + (ordinal_days[l][dt.tm_mon] + dt.tm_mday));

	*date_word = t;

	return 0;
}

/**
 * \brief convert a <tt>struct tm</tt> time to an RSTS/E time word
-----------------------------------------------------------+-----------+-------+
 */
int
tm2rsts_time(struct tm dt, uint16_t *time_word) {
	uint16_t t;

	if ((time_word == NULL) || !valid_time(dt))
		return 1;

	t = (uint16_t) (1440 - ((dt.tm_hour * 60) + dt.tm_min));

	*time_word = t;

	return 0;
}

/**
 * \brief convert an RSTS/E date word to a <tt>struct tm</tt> date
-----------------------------------------------------------+-----------+-------+
 */
int
rsts2tm_date(struct tm *dt, uint16_t date_word) {
	int i, l, t, y;

	/* Check that we actually have a struct tm to populate */
	if (dt == NULL)
		return 1;

	/* Checking the date word for errors needs to be split up, as some
	 * processing of the date word needs to be done before the upper bound
	 * of the range can be known, the reason for this is that the date word
	 * encodes the day-of-year as an ordinal day number, as such, based on
	 * based on the year the range for the date word can be either:
	 *     1 <= date_word <= 365
	 * or
	 *     1 <= date_word <= 366
	 * Once the year is determined to be a regular or leap year, we check
	 * the upper range of the date word.
	 *
	 * As was mentioned in the tm2rsts_date() function, the leap year check
	 * for a RSTS/E date word can be simplified to a simple divisible-by-4
	 * test, given that the only valid year range is from 1970 to 2035.
	 *
	 * The conversion process is a few steps. First the ordinal day number
	 * is extracted from the date word, by taking the date word modulo 1000.
	 * Next the year is then calculated by subtracting the ordinal day
	 * number from the date word, dividing the result of the subtraction by
	 * 1000, and finally adding 1970; the third step then determinues if the
	 * year is a leap year, or not. Step four, the month of the year is
	 * determined by comparing the ordinal day number against the sum of
	 * days to the end of the previous month. Fifth, the day of the month is
	 * calculated by subtracting the previously mentioned sum of days to the
	 * end of the previous month from the ordinal day number. Finally the
	 * standard time structure is cleared and populated.
	 */
	t = ((int) date_word) % 1000;
	if (t < 1)
		return 1;

	y = (((int) date_word) - t) / 1000 + 1970;
	l = (y % 4) == 0;
	if (t > (l ? 366 : 365))
		return 1;

	for (i = 11; (i > 0) && (ordinal_days[l][i] > t); i--);

	memset(dt, 0, sizeof(struct tm));
	dt -> tm_year = y - 1900;
	dt -> tm_mon = i;
	dt -> tm_mday = t - ordinal_days[l][i];

	return 0;
}

/**
 * \brief convert an RSTS/E time word to a <tt>struct tm</tt> time
-----------------------------------------------------------+-----------+-------+
 */
int
rsts2tm_time(struct tm *dt, uint16_t time_word) {
	int t;

	/* Check that we actually have a struct tm to populate */
	if (dt == NULL)
		return 1;

	/* First thing's first, check that the time word isn't out of bounds,
	 * that is: 1 <= time_word <= 1440
	 * Abort if that's not true.
	 *
	 * If the time word isn't invalid, we first need to convert it from the
	 * RSTS/E "minutes-before-midnight" value to the more simple (and easily
	 * understood) "minutes-after-midnight" form. Next, we extract the hour
	 * and minute values from that. The RSTS/E time word does not encode the
	 * number of seconds in the minute, so we set the seconds value to 0.
	 */
	if ((time_word < 1) || (time_word > 1440))
		return 1;
	t = 1440 - ((int) time_word);

	memset(dt, 0, sizeof(struct tm));
	rmq(t, 60, &(dt -> tm_min), &(dt -> tm_hour));
	dt -> tm_sec = 0;

	return 0;
}

/**
 * \brief convert a <tt>struct tm</tt> date to an MS-DOS date word
-----------------------------------------------------------+-----------+-------+
 */
int
tm2dos_date(struct tm dt, uint16_t *date_word) {
	uint16_t t;
	int y;

	if ((date_word == NULL) || !valid_date(dt) || (dt.tm_year < 80) ||
	    (dt.tm_year > 207))
		return 1;

	y = dt.tm_year - 80;

	t = (uint16_t) (((y & 0x7F) << 9) | (((dt.tm_mon + 1) & 0x0F) << 5) |
	    (dt.tm_mday & 0x1F));

	*date_word = t;

	return 0;
}

/**
 * \brief convert a <tt>struct tm</tt> time to an MS-DOS time word
-----------------------------------------------------------+-----------+-------+
 */
int
tm2dos_time(struct tm dt, uint16_t *time_word) {
	uint16_t t;

	if ((time_word == NULL) || !valid_time(dt))
		return 1;

	t = (uint16_t) (((dt.tm_hour & 0x1F) << 11) | ((dt.tm_min & 0x3F) <<
	    5) | ((dt.tm_sec & 0x3E) >> 1));

	*time_word = t;

	return 0;
}

/**
 * \brief convert an MS-DOS date word to a <tt>struct tm</tt> date
-----------------------------------------------------------+-----------+-------+
 */
int
dos2tm_date(struct tm *dt, uint16_t date_word) {
	int yy, mm, dd;

	/* Make sure the destination struct actually exists... */
	if (dt == NULL)
		return 1;

	yy = ((date_word >>  9) & 0x7F) + 1980;
	mm = ((date_word >>  5) & 0x0F);
	dd = ((date_word      ) & 0x1F);

	/* The valid range of years for an MS-DOS date is between 1980 and 2107
	 * so, unfortunately, the proper leap year check must be done; as 2100
	 * is not a leap year.
	 * Luckily, just like for the RT-11 date word, the day number is stored
	 * in five bits; so it is impossible for January, March, May, July,
	 * August, October, or December to have a too large day count.
	 */
	if ((mm < 1 || mm > 12) || (dd < 1 || dd > days_in_month[leap(yy)][mm -
	    1]))
		return 1;

	memset(dt, 0, sizeof(struct tm));
	dt -> tm_year = yy - 1900;
	dt -> tm_mon = mm - 1;
	dt -> tm_mday = dd;

	return 0;
}

/**
 * \brief convert an MS-DOS time word to a <tt>struct tm</tt> time
-----------------------------------------------------------+-----------+-------+
 */
int
dos2tm_time(struct tm *dt, uint16_t time_word) {
	int hh, mm, ss;

	/* Does the struct we're going to put the time in exist? */
	if (dt == NULL)
		return 1;

	/*  */
	hh = ((time_word >> 11) & 0x1F);
	mm = ((time_word >>  5) & 0x3F);
	ss = ((time_word <<  1) & 0x3F);

	/*  */
	if ((hh > 23) || (mm > 59) || (ss > 59))
		return 1;
	
	memset(dt, 0, sizeof(struct tm));
	dt -> tm_hour = hh;
	dt -> tm_min = mm;
	dt -> tm_sec = ss;

	return 0;
}

/**
 * \brief convert a <tt>struct tm</tt> date to an CP/M date word
-----------------------------------------------------------+-----------+-------+
 */
int
tm2cpm_date(struct tm dt, uint16_t *date_word) {
	int a;

	/* We need to break the error checking into two phases; the first phase
	 * is to verify that the destination word exists and that the struct tm
	 * with the date to be converted represents a valid date.
	 */
	if ((date_word == NULL) || !valid_date(dt))
		return 1;

	/* We now convert the date tuple in the struct into an absolute date,
	 * which we then compare to the valid range of dates.
	 */
	if(tm2abs(&a, dt))
		return 1;
	if ((a < 722085) || (a > 787619))
		return 1;

	/* We now convert the date into range, and populat the date word. */
	a -= 722084;
	*date_word = (uint16_t) a;

	return 0;
}

/**
 * \brief convert a <tt>struct tm</tt> time to an CP/M time word
-----------------------------------------------------------+-----------+-------+
 */
int
tm2cpm_time(struct tm dt, uint16_t *time_word) {
	uint16_t t;
	int a, b;

	/* Check that both the destination word for the CP/M time exists, and
	 * that the time tuple to be converted is valid.
	 */
	if ((time_word == NULL) || !valid_time(dt))
		return 1;

	/* The CP/M time word is just the BCD representation of the hour and
	 * minute; with the hour in the high byte, and minute in the low.  Thus
	 * conversion is relatively simple; we'll start with the hour first.
	 */
	rmq(dt.tm_hour, 10, &a, &b);
	t = (uint16_t) (((b & 0x0F) << 12) | ((a & 0x0F) << 8));

	/* Convert the minute. */
	rmq(dt.tm_min,  10, &a, &b);
	t |= (uint16_t) (((b & 0x0F) << 4) | (a & 0x0F));

	/* Populate the time word. */
	*time_word = t;

	return 0;
}

/**
 * \brief convert an CP/M date word to a <tt>struct tm</tt> date
-----------------------------------------------------------+-----------+-------+
 */
int
cpm2tm_date(struct tm *dt, uint16_t date_word) {
	int a;

	/* We can't do anything if the date structure doesn't exist. */
	if (dt == NULL)
		return 1;

	/*  */
	a = 722084 + (int) date_word;

	memset(dt, 0, sizeof(struct tm));
	abs2tm(a, dt);

	return 0;
}

/**
 * \brief convert an CP/M time word to a <tt>struct tm</tt> time
-----------------------------------------------------------+-----------+-------+
 */
int
cpm2tm_time(struct tm *dt, uint16_t time_word) {
	int hh, mm;

	/* Check the time structure exists */
	if (dt == NULL)
		return 1;

	/* Convert the time word into the hour and minute. */
	hh = (((time_word & 0xF000) >> 12) * 10) + ((time_word & 0x0F00) >> 8);
	mm = (((time_word & 0x00F0) >>  4) * 10) + ((time_word & 0x000F)     );

	/* Check that the time is valid:
	 *   1. Hour not greater than 23.
	 *   2. Minute not greater than 59.
	 */
	if ((hh > 23) || (mm > 59))
		return 1;

	/* Populate the time struct */
	memset(dt, 0, sizeof(struct tm));
	dt -> tm_hour = hh;
	dt -> tm_min = mm;
	dt -> tm_sec = 0;

	return 0;
}
