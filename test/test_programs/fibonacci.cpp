/*
 * MIT License
 *
 * Copyright (c) 2017 Kevin Kirchner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
 * \file        hello_world.cpp
 * \brief       Source file implementing a function to calculate fibonacci
 *              numbers to be used to
 *              test \p libelfpp
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT License
 *
 * This source file implements a simple program that calculates Fibonacci
 * numbers. This should be compiled into an ELF file and then used to test
 * \p libelfpp.
 */

#include <iostream>

/// Calculates and prints all Fibonacci numbers that are smaller than \p max.
///
/// \param max The upper limit (exclusive) of Fibonacci numbers to print
void fibonacci(const unsigned long long max) {
  unsigned long long first = 0, second = 1, next = 0;
  while (next < max) {
    std::cout << first << " ";
    next = first + second;
    first = second;
    second = next;
  }
}

int main() {
  fibonacci(10000000);
  return 0;
}