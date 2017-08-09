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
 * \file        simple_example.cpp
 * \brief       A simple example program
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT LICENSE
 *
 * A simple example program that demonstrates basic usage of \p libelfpp.
 */

#include <libelfpp/libelfpp.h>
#include <iostream>

/// Main function of this example program.
///
/// \param argc Number of arguments
/// \param argv Arguments in string array
/// \return Integer value indicating termination state (0 means 'sucess',
///         everything else means 'failure')
int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " ELFFILE\n";
    return 1;
  }

  try {
    libelfpp::ELFFile file = libelfpp::ELFFile(argv[1]);
    auto h = file.getHeader();
    std::cout << "Basic ELF file information:\n\n";
    std::cout << "Filename: " << argv[1] << "\n";
    std::cout << "ELF Type: " << h->getELFTypeString() << "\n";
    std::cout << "Endianess: " << (h->is64Bit() ? "64" : "32") << " Bit\n";
    std::cout << "Encoding: " << (h->isLittleEndian() ? "Little" : "Big") << " Endian\n";
    std::cout << "Entrypoint: " << h->getEntryPoint() << std::hex << " (0x" << h->getEntryPoint() << ")\n";
    std::cout << "ABI: " << h->getABIString() << "\n";
    std::cout << "Machine: " << h->getMachineString() << "\n";
    std::cout << "Version: " << h->getVersion() << "\n";
    std::cout << "Number of Segments: " << file.segments().size() << "\n";
    std::cout << "Number of Sections: " << file.sections().size() << "\n";
    return 0;

  } catch (const std::runtime_error&) {
    std::cerr << "The chosen file does not seem to be a valid ELF file!\n";
    return 1;
  }
}