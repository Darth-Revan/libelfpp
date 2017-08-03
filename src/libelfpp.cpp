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
 * \file        libelfpp.cpp
 * \brief       Main source file for \p libelfpp
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT LICENSE
 *
 * This source file is the main source file for \p libelfpp and implements the
 * library's public interface.
 */

#include "libelfpp/libelfpp.h"
#include <fstream>
#include <cstring>
#include <elf.h>

namespace libelfpp {

// Implementation of constructor
ELFFile::ELFFile(const std::string& filename) : Filename(filename) {
  std::ifstream Input(filename);
  if (!Input.good()) {
    throw std::runtime_error("File does not exist!");
  }

  unsigned char e_ident[EI_NIDENT];
  Input.seekg(0);
  Input.read(reinterpret_cast<char*>(&e_ident), sizeof(e_ident));

  // check if file is ELF file
  if (Input.gcount() != sizeof(e_ident) ||
      std::memcmp(e_ident, ELFMAG, std::strlen(ELFMAG)) != 0) {
    throw std::runtime_error("Invalid magic number!");
  }

  switch (e_ident[EI_CLASS]) {
  case ELFCLASS32:
    Is64Bit = false;
    break;
  case ELFCLASS64:
    Is64Bit = true;
    break;
  default:
    throw std::runtime_error("Invalid ELF file class!");
  }

  switch (e_ident[EI_DATA]) {
  case ELFDATA2LSB:
    IsLittleEndian = true;
    break;
  case ELFDATA2MSB:
    IsLittleEndian = false;
    break;
  default:
    throw std::runtime_error("Invalid ELF encoding!");
  }

  Converter = std::make_shared<EndianessConverter>(IsLittleEndian);

  Input.close();
}

} // end of namespace libelfpp