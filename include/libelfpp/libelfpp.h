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
 * \file        libelfpp.h
 * \brief       Main header file for \p libelfpp
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT LICENSE
 *
 * This header file is the main header file for \p libelfpp and is the public
 * interface for the library.
 */

#ifndef LIBELFPP_LIBELFPP_H
#define LIBELFPP_LIBELFPP_H

#include "fileheader.h"
#include "segment.h"
#include "section.h"
#include <ostream>
#include <memory>


namespace libelfpp {

/// Returns the library's version number as string.
///
/// \return The library's version number
inline const std::string getVersionString() {
  return std::string(ELFPP_VERSION);
}


/// Class representing an ELF file;
class ELFFile final {

private:
  /// Holds the name of the underlying file
  const std::string Filename;

  /// \p true if file is little endian
  bool IsLittleEndian;

  /// \p true if file is 64 bit file
  bool Is64Bit;

  /// Holds a shared pointer to a endianess converter
  std::shared_ptr<EndianessConverter> Converter;

  /// Holds a shared pointer to the file header
  std::shared_ptr<ELFFileHeader> FileHeader;

  /// Holds pointers to all segments of the file
  std::vector<std::shared_ptr<Segment>> Segments;

  /// Holds pointers to all sections of the file
  std::vector<std::shared_ptr<Section>> Sections;

  /// Holds a pointer to the file's primary string section
  std::shared_ptr<StringSection> StrSection;

  /// Holds a pointer to the file's dynamic section
  std::shared_ptr<DynamicSection> DynamicSec;

  /// Loads all segmetns from the file stream \p stream and stores them in the
  /// vector \p Segements.
  ///
  /// \param stream A file input stream to read from
  /// \return Number of segments loaded
  Elf64_Half loadSegmentsFromFile(std::ifstream& stream);

  /// Loads all sections from the file stream \p stream and stores them in
  /// the vector \p VecSections.
  ///
  /// \param stream A file input stream to read from
  /// \return Number of sections loaded
  Elf64_Half loadSectionsFromFile(std::ifstream& stream);

public:
  /// Constructor of \p ELFFile. Creates a new instance of the class or throws
  /// an \p runtime_exception if something goes wrong.
  ///
  /// \param filename Path to the file to create an instance upon
  /// \throws std::runtime_exception If something goes wrong
  ELFFile(const std::string& filename);

  /// Copy constructor of \p ELFFile.
  ///
  /// \param other The instance to copy
  ELFFile(const ELFFile& other) : Filename(other.Filename),
                                  IsLittleEndian(other.IsLittleEndian),
                                  Is64Bit(other.Is64Bit),
                                  Converter(other.Converter),
                                  FileHeader(other.FileHeader),
                                  Segments(other.Segments),
                                  StrSection(other.StrSection),
                                  DynamicSec(other.DynamicSec) {}

  /// Destructor of \p ELFFile.
  ~ELFFile() {
    Converter.reset();
    FileHeader.reset();
    Segments.clear();
    Sections.clear();
    StrSection.reset();
    DynamicSec.reset();
  }

  /// Returns the name of the underlying file a string.
  ///
  /// \return Name of file
  const std::string getName() const {
    return Filename;
  }

  /// Returns a constant shared pointer to a object that represents the file's
  /// header.
  ///
  /// \return Pointer to the file header
  const std::shared_ptr<ELFFileHeader> getHeader() const {
    return FileHeader;
  }

  /// Returns a pointer to the object representing the file's primary string
  /// section.
  ///
  /// \return Pointer to the string section
  const std::shared_ptr<StringSection> getStringSection() const {
    return StrSection;
  }

  /// Returns a pointer to the object representing the file's primary dynamic
  /// section.
  ///
  /// \return Pointer to the dynamic section
  const std::shared_ptr<DynamicSection> getDynamicSection() const {
    return DynamicSec;
  }

  /// Returns a constant reference to the vector of segments in this
  /// ELF file.
  ///
  /// @return Reference to a std::vector containing std::shared_ptr<Segment>
  const std::vector<std::shared_ptr<Segment>>& segments() const {
    return Segments;
  }

  /// Returns a constant reference to the vector of sections in this
  /// ELF file.
  ///
  /// @return Reference to a std::vector containing std::shared_ptr<Section>
  const std::vector<std::shared_ptr<Section>>& sections() const {
    return Sections;
  }


  /// Overrides the stream operator << for \p ELFFile.
  ///
  /// \param stream The output stream to write \p ELFFile to
  /// \param file Instance of \p ELFFile
  /// \return The output stream \p stream
  friend std::ostream& operator<<(std::ostream& stream, const ELFFile& file);

  /// Overrides the comparison operator ==. Returns \p true if \p lhs and \p rhs
  /// point to the same file, \p false otherwise.
  ///
  /// \param lhs The first object
  /// \param rhs The second object
  /// \return \p true if both are equal, \p false otherwise
  friend bool operator==(const ELFFile& lhs, const ELFFile& rhs);

  /// Overrides the comparison operator !=. Returns \p false if \p lhs and \p rhs
  /// point to the same file, \p true otherwise.
  ///
  /// \param lhs The first object
  /// \param rhs The second object
  /// \return \p false if both are equal, \p true otherwise
  friend bool operator!=(const ELFFile& lhs, const ELFFile& rhs);

}; // end of class ELFFile


// Overrides the stream operator <<
std::ostream& operator<<(std::ostream &stream, const ELFFile &file) {
  stream << "ELFFile (" << file.Filename << ")\n";
  return stream;
}

// Operator ==
bool operator==(const ELFFile &lhs, const ELFFile &rhs) {
  return (lhs.Filename == rhs.Filename);
}

// Operator !=
bool operator!=(const ELFFile &lhs, const ELFFile &rhs) {
  return !operator==(lhs, rhs);
}

} // end of namespace libelfpp

#endif //LIBELFPP_LIBELFPP_H
