#ifndef DATE_TIME_TEST_H
#define DATE_TIME_TEST_H
/*
 * Copyright (c) 2025 Christian Gauger-Cosgrove
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to dea
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
 * \file	date_time_test.h
 * \copyright	MIT
 * \date	2025
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */

int	 init_date_time_suite(void);
int	 clean_date_time_suite(void);
void	 test_valid_date(void);
void	 test_valid_time(void);
void	 test_tm2abs(void);
void	 test_abs2tm(void);
void	 test_mjd2abs(void);
void	 test_abs2mjd(void);
void	 test_lil2abs(void);
void	 test_abs2lil(void);
void	 test_xcl2abs(void);
void	 test_abs2xcl(void);
void	 test_tm2rt_date(void);
void	 test_tm2rt_time(void);
void	 test_rt2tm_date(void);
void	 test_rt2tm_time(void);
void	 test_tm2rsts_date(void);
void	 test_tm2rsts_time(void);
void	 test_rsts2tm_date(void);
void	 test_rsts2tm_time(void);
void	 test_tm2dos_date(void);
void	 test_tm2dos_time(void);
void	 test_dos2tm_date(void);
void	 test_dos2tm_time(void);
void	 test_tm2cpm_date(void);
void	 test_tm2cpm_time(void);
void	 test_cpm2tm_date(void);
void	 test_cpm2tm_time(void);

#endif /* DATE_TIME_TEST_H */
