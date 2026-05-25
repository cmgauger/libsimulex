#ifndef DATE_TIME_H
#define DATE_TIME_H
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
 * \file	date_time.h
 * \copyright	MIT
 * \date	2024
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.1
 */
#include <stdint.h>
#include <time.h>

int	 valid_date(struct tm);
int	 valid_time(struct tm);
int	 abs2tm(int, struct tm *);
int	 tm2abs(int *, struct tm);
int	 abs2mjd(int, int *);
int	 mjd2abs(int *, int);
int	 abs2lil(int, int *);
int	 lil2abs(int *, int);
int	 abs2xcl(int, int *);
int	 xcl2abs(int *, int);
int	 tm2rt_date(struct tm, uint16_t *);
int	 tm2rt_time(struct tm, int, int, uint16_t *, uint16_t *);
int	 rt2tm_date(struct tm *, uint16_t);
int	 rt2tm_time(struct tm *, int *, int, uint16_t, uint16_t);
int	 tm2rsts_date(struct tm, uint16_t *);
int	 tm2rsts_time(struct tm, uint16_t *);
int	 rsts2tm_date(struct tm *, uint16_t);
int	 rsts2tm_time(struct tm *, uint16_t);
int	 tm2dos_date(struct tm, uint16_t *);
int	 tm2dos_time(struct tm, uint16_t *);
int	 dos2tm_date(struct tm *, uint16_t);
int	 dos2tm_time(struct tm *, uint16_t);
int	 tm2cpm_date(struct tm, uint16_t *);
int	 tm2cpm_time(struct tm, uint16_t *);
int	 cpm2tm_date(struct tm *, uint16_t);
int	 cpm2tm_time(struct tm *, uint16_t);
int	 tm2dos(struct tm, uint16_t *, uint16_t *);
int	 tm2cpm(struct tm, uint16_t *, uint16_t *);

#endif /* DATE_TIME_H */
