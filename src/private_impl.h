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
 * \file        private_impl.h
 * \brief       Header file declaring classes that should not be
 *              exposed to library users
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT License
 *
 * This header file declares classes and functions that implement public
 * interfaces and that should not be exposed to the user of the library.
 */

#ifndef LIBELFPP_PRIVATE_IMPL_H
#define LIBELFPP_PRIVATE_IMPL_H

#include "libelfpp/fileheader.h"
#include "libelfpp/segment.h"
#include <map>
#include <algorithm>

namespace libelfpp {

/// Map mapping ELF machines to strings
extern std::map<unsigned int, const char*> ELFMachineStrings;

/// Map mapping ABI codes to strings
extern std::map<unsigned int, const char*> ABIStrings;

/// Holds strings representing segment types
extern std::map<unsigned int, const char*> SegmentTypeStrings;

/// Template class for the two types of \p Elf_Ehdr
///
/// \tparam T The type of ELF header
template<class T>
struct ELFHeaderImplTypes;

/// Template implement type for 32 Bit ELFs
template<> struct
ELFHeaderImplTypes<Elf32_Ehdr> {
  /// Defines the type of program headers
  typedef Elf32_Phdr Phdr_t;
  /// Defines the type of section headers
  typedef Elf32_Shdr Shdr_t;
  /// Holds the header's class
  static const bool is64Bit = false;
};

/// Template implement type for 64 Bit ELFs
template<> struct
ELFHeaderImplTypes<Elf64_Ehdr> {
  /// Defines the type of program headers
  typedef Elf64_Phdr Phdr_t;
  /// Defines the type of section headers
  typedef Elf64_Shdr Shdr_t;
  /// Holds the header's class
  static const bool is64Bit = true;
};

/// Template implementation of ELF header.
///
/// \tparam T The ELF type
template<class T>
class ELFHeaderImpl final : public ELFFileHeader {

private:
  /// Holds the file's header
  T Header;
  /// Holds a pointer to an \p EndianessConverter
  const std::shared_ptr<EndianessConverter> Converter;

public:
  /// Constructor of \p ELFHeaderImpl. Initializes the header values of the
  /// ELF file.
  ///
  /// \param converter A Pointer to a instance of \p EndianessConverter
  /// \param encoding The encoding to use
  /// \param stream The file stream to load the header from
  ELFHeaderImpl(const std::shared_ptr<EndianessConverter> converter,
                const bool isLittleEndian, std::ifstream &stream) :
      Converter(converter) {

    std::fill_n(reinterpret_cast<char *>(&Header), sizeof(Header), '\0');
    Header.e_ident[EI_MAG0] = ELFMAG[0];
    Header.e_ident[EI_MAG1] = ELFMAG[1];
    Header.e_ident[EI_MAG2] = ELFMAG[2];
    Header.e_ident[EI_MAG3] = ELFMAG[3];
    Header.e_ident[EI_CLASS] =
        ELFHeaderImplTypes<T>::is64Bit ? ELFCLASS64 : ELFCLASS32;
    Header.e_ident[EI_DATA] = isLittleEndian ? ELFDATA2LSB : ELFDATA2MSB;
    Header.e_ident[EI_VERSION] = EV_CURRENT;
    Header.e_version = EV_CURRENT;
    Header.e_version = (*Converter)(Header.e_version);
    Header.e_ehsize = (sizeof(Header));
    Header.e_ehsize = (*Converter)(Header.e_ehsize);
    Header.e_shstrndx = (*Converter)((Elf32_Half) 1);
    Header.e_phentsize = sizeof(typename ELFHeaderImplTypes<T>::Phdr_t);
    Header.e_phentsize = (*Converter)(Header.e_phentsize);
    Header.e_shentsize = sizeof(typename ELFHeaderImplTypes<T>::Shdr_t);
    Header.e_shentsize = (*Converter)(Header.e_shentsize);

    stream.seekg(0);
    stream.read(reinterpret_cast<char *>(&Header), sizeof(Header));
  }

  // Returns the ELF file's class
  bool is64Bit() const {
    return ((*Converter) (Header.e_ident[EI_CLASS]) == ELFCLASS64);
  }

  // Returns the ELF file's version.
  unsigned char getVersion() const {
    return static_cast<unsigned char>((*Converter) (Header.e_version));
  }

  // Returns the ELF file's encoding
  bool isLittleEndian() const {
    return ((*Converter) (Header.e_ident[EI_DATA]) == ELFDATA2LSB);
  }

  // Returns the ELF file's ABI.
  unsigned int getABI() const {
    return static_cast<unsigned int>((*Converter) (Header.e_ident[EI_OSABI]));
  }

  // Returns the ELF file's ABI as string.
  const std::string getABIString() const {
    auto abi = getABI();
    if (abi < ABIStrings.size()) {
      return ABIStrings[abi];
    }
    return "Unknown";
  }

  // Returns the ELF file's type.
  unsigned int getELFType() const {
    return static_cast<unsigned int>((*Converter) (Header.e_type));
  }

  // Returns the ELF file's type as string.
  const std::string getELFTypeString() const {
    switch ((*Converter) (Header.e_type)) {
    case ET_NONE:
      return "None";
    case ET_REL:
      return "Relocatable Object";
    case ET_EXEC:
      return "Executable";
    case ET_DYN:
      return "Shared Object";
    case ET_CORE:
      return "Core File";
    default:
      return "Unknown";
    }
  }

  // Returns the ELF file's machine architecture.
  unsigned int getMachine() const {
    return static_cast<unsigned int>((*Converter) (Header.e_machine));
  }

  // Returns the ELF file's machine architecture as string.
  const std::string getMachineString() const {
    auto idx = getMachine();
    if (idx < ELFMachineStrings.size()) {
      return ELFMachineStrings[idx];
    }
    return "Unkown";
  }

  // Returns the ELF file's entry point.
  Elf64_Addr getEntryPoint() const {
    return (*Converter) (Header.e_entry);
  }

  // Returns the ELF file's section header number.
  Elf64_Half getSectionHeaderNumber() const {
    return (*Converter) (Header.e_shnum);
  }

  // Returns the ELF file's section header offset.
  Elf64_Off getSectionHeaderOffset() const {
    return (*Converter) (Header.e_shoff);
  }

  // Returns the size of the section headers.
  Elf64_Half getSectionHeaderSize() const {
    return (*Converter) (Header.e_shentsize);
  }

  // Returns the ELF file's program header number.
  Elf64_Half getProgramHeaderNumber() const {
    return (*Converter) (Header.e_phnum);
  }

  // Returns the ELF file's program header offset.
  Elf64_Off getProgramHeaderOffset() const {
    return (*Converter) (Header.e_phoff);
  }

  // Returns the size of the program headers.
  Elf64_Half getProgramHeaderSize() const {
    return (*Converter) (Header.e_phentsize);
  }

  // Returns the flags field of the file header.
  Elf64_Word getFlags() const {
    return (*Converter) (Header.e_flags);
  }

  // Returns the size of the file header in bytes.
  Elf64_Half getHeaderSize() const {
    return (*Converter) (Header.e_ehsize);
  }

  // Returns the ELF file's section header's string table index.
  Elf64_Half getSectionHeaderStringTableIndex() const {
    return (*Converter) (Header.e_shstrndx);
  }
}; // end of class ELFHeaderImpl


/// Templated implementation of \p Segment
///
/// \tparam T The ELF type
template<class T>
class SegmentImpl : public Segment {

private:
  /// The header of this segment
  T Header;
  /// The index of this segment
  Elf64_Half Index;
  /// The data associated with this segment
  char* Data;
  /// Pointer to an instance of \p EndianessConverter
  const std::shared_ptr<EndianessConverter> Converter;
  /// Holds the indices of associated sections
  std::vector<Elf64_Half> SectionIndices;

public:
  /// Constructor of \p SegmentImpl.
  ///
  /// \param converter Pointer to an instance of \p EndianessConverter
  SegmentImpl(const std::shared_ptr<EndianessConverter> converter) :
      Converter(converter), SectionIndices(), Data(nullptr) {
    std::fill_n(reinterpret_cast<char*>(&Header), sizeof(Header), '\0');
  }

  /// Destructor of \p SectionImpl.
  ~SegmentImpl() {
    delete[] Data;
    SectionIndices.clear();
  }

  // Returns index of segment
  Elf64_Half getIndex() const {
    return Index;
  }

  // Returns type of segment
  inline Elf64_Word getType() const {
    return (*Converter) (Header.p_type);
  }

  // Returns type of segment as string
  const std::string getTypeString() const {
    try {
      return std::string(SegmentTypeStrings.at(static_cast<unsigned int>(getType())));
    } catch (const std::out_of_range&) {
      return "UNKOWN";
    }
  }

  // Return flags of segment
  Elf64_Word getFlags() const {
    return (*Converter) (Header.p_flags);
  }

  // Return a string representing the flags of segment
  const std::string getFlagsString() const {
    std::string result = "";
    auto flags = getFlags();
    result += (flags & PF_R ? "R" : "");
    result += (flags & PF_W ? "W" : "");
    result += (flags & PF_X ? "X" : "");
    return result;
  }

  // Returns address alignment of the segment
  Elf64_Xword getAddressAlignment() const {
    return (*Converter) (Header.p_align);
  }

  // Returns virtual address of segment
  Elf64_Addr getVirtualAddress() const {
    return (*Converter) (Header.p_vaddr);
  }

  // Returns physical address of segment
  Elf64_Addr getPhysicalAddress() const {
    return (*Converter) (Header.p_paddr);
  }

  // Returns filesz member
  Elf64_Xword getFileSize() const {
    return (*Converter) (Header.p_filesz);
  }

  // Returns memsz member
  Elf64_Xword getMemorySize() const {
    return (*Converter) (Header.p_memsz);
  }

  // Returns offset of segment
  Elf64_Off getOffset() const {
    return (*Converter) (Header.p_offset);
  }

  // Returns segment data
  const char* getData() const {
    return Data;
  }

  // Returns segment data as string
  const std::string getDataString() const {
    std::string Ret {};
    Ret.assign(Data, getFileSize());
    return Ret;
  }

  // Return section number of segment
  Elf64_Half getSectionNumber() const {
    return static_cast<Elf64_Half>(SectionIndices.size());
  }

  // Return vector containing indices of associated sections
  const std::vector<Elf64_Half>& getAssociatedSections() const {
    return SectionIndices;
  }

protected:
  // loads the segment from file
  void loadSegment(std::ifstream& stream, const std::streampos offset) {
    stream.seekg(offset);
    stream.read(reinterpret_cast<char*>(&Header), sizeof(Header));

    Elf64_Xword Size = getFileSize();
    if (getType() != PT_NULL && Size != 0) {
      try {
        Data = new char[Size];
      } catch (std::bad_alloc&) {
        Data = nullptr;
        Size = 0;
      }
      if (Data != 0) {
        stream.seekg((*Converter) (Header.p_offset));
        stream.read(Data, Size);
      }
    }
  }

  // sets the index of this segment
  void setIndex(const Elf64_Half index) {
    Index = index;
  }

  // add index to indices of associated sections
  Elf64_Half addSectionIndex(const Elf64_Half index) {
    // The vector will be so small, that the overhead of an unordered_set would
    // be higher than searching the vector for every insert
    if (std::find(SectionIndices.begin(), SectionIndices.end(), index) ==
        SectionIndices.end()) {
      SectionIndices.push_back(index);
    }
    return static_cast<Elf64_Half >(SectionIndices.size());
  }

}; // end of class SegmentImpl

} // end of namespace libelfpp

#endif //LIBELFPP_PRIVATE_IMPL_H