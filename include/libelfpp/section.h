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
 * \file        section.h
 * \brief       Header file declaring a class that represents sections
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT License
 *
 * This header file declares a class that represents a section in an ELF file.
 */

#ifndef LIBELFPP_SECTION_H
#define LIBELFPP_SECTION_H

#include "endianutil.h"
#include <string>
#include <elf.h>

namespace libelfpp {

/// Class representing an ELF file section
class Section {

  friend class ELFFile;

public:
  /// Destructor of \p Section.
  virtual ~Section() {};

  /// Returns the data associated with this section as plain character array.
  ///
  /// \return The data associated with this section
  virtual const char* getData() const = 0;

  /// Returns the data associated with this section as string.
  ///
  /// \return The data associated with this section as string
  virtual const std::string getDataString() const = 0;

  /// Returns the index of this section.
  ///
  /// \return The index of this section
  virtual Elf64_Half getIndex() const = 0;

  /// Returns the name of this section.
  ///
  /// \return The name of this section
  virtual const std::string getName() const = 0;

  /// Returns the section's type.
  ///
  /// \return Type of the section
  virtual Elf64_Word getType() const = 0;

  /// Returns the section's type as string.
  ///
  /// \return Type of the section as string
  virtual const std::string getTypeString() const = 0;

  /// Returns the section's flags.
  ///
  /// \return Flags of this section
  virtual Elf64_Xword getFlags() const = 0;

  /// Returns the flags as a describing string. Every known flag will be
  /// represented by a single character (e.g. Write-Flag will be 'W', Alloc-Flag
  /// will be 'A', ...). For sections will multiple flags set this will return
  /// a string that contains all characters encoding the flags (e.g. Write- and
  /// Alloc-Flag will return 'WA').
  ///
  /// \return The flags of the section as string
  virtual const std::string getFlagsString() const = 0;

  /// Returns the sections \p sh_info field.
  ///
  /// \return \p sh_info of this section
  virtual Elf64_Word getInfo() const = 0;

  /// Returns the sections \p sh_link field.
  ///
  /// \return \p sh_link of this section
  virtual Elf64_Word getLink() const = 0;

  /// Returns the address alignment of this section.
  ///
  /// \return Address alignment of this section
  virtual Elf64_Xword getAddressAlignment() const = 0;

  /// Returns the size of a entry in this section.
  ///
  /// \return Size of entry in this section
  virtual Elf64_Xword getEntrySize() const = 0;

  /// Returns the address of this section.
  ///
  /// \return Address of this section
  virtual Elf64_Addr getAddress() const = 0;

  /// Returns the offset of this section.
  ///
  /// \return Offset of this section
  virtual Elf64_Off getOffset() const = 0;

  /// Returns the size of this section in bytes.
  ///
  /// \return Size of this section in bytes
  virtual Elf64_Xword getSize() const = 0;

  /// Returns the string offset of the name of this section.
  ///
  /// \return String offset of the name of this section
  virtual Elf64_Word getNameStringOffset() const = 0;

protected:
  /// Loads a section from a input stream at a specific stream offset.
  ///
  /// \param stream The input stream to load from
  /// \param offset The offset at which to load the section
  virtual void loadSection(std::ifstream& stream, std::streampos offset) = 0;

  /// Sets the section's member \p Name. This will not touch the file itself.
  ///
  /// \param name The name for the section
  virtual void setName(const std::string& name) = 0;

  /// Sets the section's member \p Index. This will not touch the file itself.
  ///
  /// \param index The index for the section
  virtual void setIndex(const Elf64_Half index) = 0;

}; // end of class Section

} // end of namespace elfpp


#endif //LIBELFPP_SECTION_H
