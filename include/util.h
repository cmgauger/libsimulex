#ifndef UTIL_H
#define UTIL_H
/*
 * Copyright (c) 2022 Christian Gauger-Cosgrove
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
 * \file	util.h
 * \copyright	MIT
 * \date	2022
 * \author	Christian Gauger-Cosgrove
 * \version	0.0.5
 */
#include <stdint.h>

uint64_t bounded_rand64(uint64_t, uint64_t (*)(void *), void *);
uint32_t bounded_rand32(uint32_t, uint32_t (*)(void *), void *);
uint32_t permute(uint32_t, uint32_t, uint32_t);
double real64_rand64(uint64_t (*)(void *), void *);
double real64_rand32(uint32_t (*)(void *), void *);
float real32_rand64(uint64_t (*)(void *), void *);
float real32_rand32(uint32_t (*)(void *), void *);
int shuffle64(void *, const size_t, const size_t, uint64_t (*)(void *), void *);
int shuffle32(void *, const size_t, const size_t, uint32_t (*)(void *), void *);

#endif /* UTIL_H */
