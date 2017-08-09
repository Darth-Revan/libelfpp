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
 * \file        segment.h
 * \brief       Header file declaring a class that represents segments
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT License
 *
 * This header file declares a class that represents a segment in an ELF file.
 */

#ifndef LIBELFPP_SEGMENT_H
#define LIBELFPP_SEGMENT_H

#include <elf.h>
#include <string>
#include <vector>

namespace libelfpp {

/// Class representing an ELF file segment
class Segment {

  /// Base class for ELF files. Must be friend class, so that is able to create
  /// new Segments.
  friend class ELFFile;

public:
  /// Destructor of \p Segment.
  virtual ~Segment() {};

  /// Returns the index of this segment.
  ///
  /// \return The index of this segment
  virtual Elf64_Half getIndex() const = 0;

  /// Returns the segment's type.
  ///
  /// \return Type of the segment
  virtual Elf64_Word getType() const = 0;

  /// Returns the segment's type as string.
  ///
  /// \return Type of the segment as string
  virtual const std::string getTypeString() const = 0;

  /// Returns the section's segment.
  ///
  /// \return Flags of this segment
  virtual Elf64_Word getFlags() const = 0;

  /// Returns the segment's flags as string.
  ///
  /// \return The segment's flags as string
  virtual const std::string getFlagsString() const = 0;

  /// Returns the address alignment of this segment.
  ///
  /// \return Address alignment of this segment
  virtual Elf64_Xword getAddressAlignment() const = 0;

  /// Returns the virtual address of this segment.
  ///
  /// \return Virtual address of this segment
  virtual Elf64_Addr getVirtualAddress() const = 0;

  /// Returns the physical address of this segment.
  ///
  /// \return Physical address of this segment
  virtual Elf64_Addr getPhysicalAddress() const = 0;

  /// Returns the file size of this segment.
  ///
  /// \return File size of this segment
  virtual Elf64_Xword getFileSize() const = 0;

  /// Returns the memory size of this segment.
  ///
  /// \return Memory size of this segment
  virtual Elf64_Xword getMemorySize() const = 0;

  /// Returns the offset of this segment.
  ///
  /// \return Offset of this segment
  virtual Elf64_Off getOffset() const = 0;

  /// Returns the data associated with this segment as plain character array.
  ///
  /// \return The data associated with this segment
  virtual const char* getData() const = 0;

  /// Returns the data associated with this segment as string.
  ///
  /// \return The data associated with this segment as string
  virtual const std::string getDataString() const = 0;

  /// Returns the number of sections associated with this segment.
  ///
  /// \return Number of sections associated with this section
  virtual Elf64_Half getSectionNumber() const = 0;

  /// Returns a vector of indices, where each index represents the index of
  /// a section associated with this particular segment.
  ///
  /// \return A vector of associated sections
  virtual const std::vector<Elf64_Half>& getAssociatedSections() const = 0;

protected:
  /// Loads a segment from a input stream at a specific stream offset.
  ///
  /// \param stream The input stream to load from
  /// \param offset The offset at which to load the segment
  virtual void loadSegment(std::ifstream& stream, std::streampos offset) = 0;

  /// Sets the segment's member \p Index. This will not touch the file itself.
  ///
  /// \param index The index for the segment
  virtual void setIndex(const Elf64_Half index) = 0;

  /// Adds the section with index \p index to the list of associated sections.
  ///
  /// \param index Index of the section to add
  /// \return The number of sections associated with the segment after adding
  ///         the new one.
  virtual Elf64_Half addSectionIndex(const Elf64_Half index) = 0;

}; // end of class Segment

} // end of namespace elfpp

#endif //LIBELFPP_SEGMENT_H
