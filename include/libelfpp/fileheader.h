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
 * \file        fileheader.h
 * \brief       Header file declaring ELF file header class
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT License
 *
 * This header file declares a class representing a ELF file header.
 */

#ifndef LIBELFPP_FILEHEADER_H
#define LIBELFPP_FILEHEADER_H

#include "endianutil.h"
#include <fstream>
#include <memory>
#include <ostream>
#include <elf.h>

namespace libelfpp {

/// Class representing the ELF file's header
class ELFFileHeader {

public:
  /// Destructor of \p ELFFileHeader
  virtual ~ELFFileHeader() {};

  /// Returns the ELF file's class (\p true if file class is \p ELFCLASS64,
  /// \p false otherwise).
  ///
  /// \return The ELF file's class
  virtual bool is64Bit() const = 0;

  /// Returns the ELF file's version.
  ///
  /// \return The ELF file's version
  virtual unsigned char getVersion() const = 0;

  /// Returns the ELF file's encoding (\p true if encoding is little endian,
  /// \p false otherwise).
  ///
  /// \return The ELF file's encoding
  virtual bool isLittleEndian() const = 0;

  /// Returns the ELF file's ABI. See elf.h header file for details.
  ///
  /// \return The ELF file's ABI
  virtual unsigned int getABI() const = 0;

  /// Returns the ELF file's ABI as string.
  ///
  /// \return The ELF file's ABI as string
  virtual const std::string getABIString() const = 0;

  /// Returns the ELF file's type. See elf.h header file for details.
  ///
  /// \return The ELF file's type
  virtual unsigned int getELFType() const = 0;

  /// Returns the ELF file's type as string.
  ///
  /// \return The ELF file's type as string
  virtual const std::string getELFTypeString() const = 0;

  /// Returns the ELF file's machine architecture.  See elf.h header file for
  /// details.
  ///
  /// \return The ELF file's machine architecture
  virtual unsigned int getMachine() const = 0;

  /// Returns the ELF file's machine architecture as string.
  ///
  /// \return The ELF file's machine architecture as string
  virtual const std::string getMachineString() const = 0;

  /// Returns the ELF file's entry point.
  ///
  /// \return The ELF file's entry point
  virtual Elf64_Addr getEntryPoint() const = 0;

  /// Returns the ELF file's section header number.
  ///
  /// \return The ELF file's section header number
  virtual Elf64_Half getSectionHeaderNumber() const = 0;

  /// Returns the ELF file's section header offset.
  ///
  /// \return The ELF file's section header offset
  virtual Elf64_Off getSectionHeaderOffset() const = 0;

  /// Returns the size of the section headers.
  ///
  /// \return Size of the section headers.
  virtual Elf64_Half getSectionHeaderSize() const = 0;

  /// Returns the ELF file's program header number.
  ///
  /// \return The ELF file's program header number
  virtual Elf64_Half getProgramHeaderNumber() const = 0;

  /// Returns the ELF file's program header offset.
  ///
  /// \return The ELF file's program header offset
  virtual Elf64_Off getProgramHeaderOffset() const = 0;

  /// Returns the size of the program headers.
  ///
  /// \return Size of the program headers.
  virtual Elf64_Half getProgramHeaderSize() const = 0;

  /// Returns the flags field of the file header.
  ///
  /// \return Contents of the flags field of the file header
  virtual Elf64_Word getFlags() const = 0;

  /// Returns the size of the file header in bytes.
  ///
  /// \return The size of the file header in bytes.
  virtual Elf64_Half getHeaderSize() const = 0;

  /// Returns the ELF file's section header's string table index.
  ///
  /// \return The ELF file's section header's string table index
  virtual Elf64_Half getSectionHeaderStringTableIndex() const = 0;

}; // end of class ELFFileHeader

} // end of namespace elfpp

#endif //LIBELFPP_FILEHEADER_H
