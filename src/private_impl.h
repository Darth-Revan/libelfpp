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
#include "libelfpp/section.h"
#include <map>
#include <algorithm>

namespace libelfpp {

/// Map mapping ELF machines to strings
extern std::map<unsigned int, const std::string> ELFMachineStrings;

/// Map mapping ABI codes to strings
extern std::map<unsigned int, const std::string> ABIStrings;

/// Holds strings representing segment types
extern std::map<unsigned int, const std::string> SegmentTypeStrings;

/// Holds strings representing section types
extern std::map<unsigned int, const std::string> ELFSectionTypeStrings;

/// Holds chars representing section flags
extern std::map<unsigned int, const char> SectionFlagChars;

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
    try {
      return std::string(ABIStrings.at(getABI()));
    } catch (const std::out_of_range&) {
      return "Unknown";
    }
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
    try {
      return std::string(ELFMachineStrings.at(getMachine()));
    } catch (const std::out_of_range &) {
      return "Unknown";
    }
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
class SegmentImpl final : public Segment {

private:
  /// The header of this segment
  T Header;
  /// The index of this segment
  Elf64_Half Index;
  /// The data associated with this segment
  std::string Data;
  /// Pointer to an instance of \p EndianessConverter
  const std::shared_ptr<EndianessConverter> Converter;
  /// Holds the indices of associated sections
  std::vector<Elf64_Half> SectionIndices;

public:
  /// Constructor of \p SegmentImpl.
  ///
  /// \param converter Pointer to an instance of \p EndianessConverter
  SegmentImpl(const std::shared_ptr<EndianessConverter> converter) :
      Converter(converter), SectionIndices(), Data("") {
    std::fill_n(reinterpret_cast<char*>(&Header), sizeof(Header), '\0');
  }

  /// Destructor of \p SectionImpl.
  ~SegmentImpl() {
    Data.clear();
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
    return Data.c_str();
  }

  // Returns segment data as string
  const std::string getDataString() const {
    return Data;
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
        Data = std::string(Size, ' ');
      } catch (std::bad_alloc&) {
        Data = "";
        Size = 0;
      }
      if (!Data.empty()) {
        stream.seekg((*Converter) (Header.p_offset));
        stream.read(&Data[0], Size);
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


/// Templated implementation of class \p Section
template<class T>
class SectionImpl : virtual public Section {

protected:
  /// The header of this section
  T Header;
  /// The index of this section
  Elf64_Half Index;
  /// The name of this section
  std::string Name;
  /// Data associated with this section
  std::string Data;
  /// Pointer to an instance of \p EndianessConverter
  const std::shared_ptr<EndianessConverter> Converter;

public:
  /// Constructor of \p SectionImpl.
  ///
  /// \param converter A pointer to an object of \p EndianessConverter
  SectionImpl(const std::shared_ptr<EndianessConverter> converter) :
      Converter(converter), Name(""), Data() {
    std::fill_n(reinterpret_cast<char*>(&Header), sizeof(Header), '\0');
  }

  /// Copy constructor of \p SectionImpl.
  ///
  /// \param other The instance to copy
  SectionImpl(const SectionImpl& other) : Header(other.Header),
                                          Index(other.Index), Name(other.Name),
                                          Data(other.Data),
                                          Converter(other.Converter) {}

  /// Destructor of \p SectionImpl. Deletes all data read from the file
  /// associated with this section.
  virtual ~SectionImpl() {
    Data.clear();
  }

  Elf64_Half getIndex() const {
    return Index;
  }

  Elf64_Word getType() const {
    return (*Converter) (Header.sh_type);
  }

  const std::string getTypeString() const {
    try {
      return std::string(ELFSectionTypeStrings.at(static_cast<unsigned int>(getType())));
    } catch (const std::out_of_range&) {
      return "UNKOWN";
    }
  }

  Elf64_Xword getFlags() const {
    return (*Converter) (Header.sh_flags);
  }

  const std::string getFlagsString() const {
    std::string result = "";
    auto flags = getFlags();
    for (const auto& elem : SectionFlagChars) {
      if ((flags & elem.first) == elem.first) {
        result += elem.second;
      }
    }
    return result;
  }

  const char* getData() const {
    return Data.c_str();
  }

  const std::string getDataString() const {
    return Data;
  }

  const std::string getName() const {
    return Name;
  }

  Elf64_Word getInfo() const {
    return (*Converter) (Header.sh_info);
  }

  Elf64_Word getLink() const {
    return (*Converter) (Header.sh_link);
  }

  Elf64_Xword getAddressAlignment() const {
    return (*Converter) (Header.sh_addralign);
  }

  Elf64_Xword getEntrySize() const {
    return (*Converter) (Header.sh_entsize);
  }

  Elf64_Addr getAddress() const {
    return (*Converter) (Header.sh_addr);
  }

  Elf64_Off getOffset() const {
    return (*Converter) (Header.sh_offset);
  }

  Elf64_Xword getSize() const {
    return (*Converter) (Header.sh_size);
  }

  Elf64_Word getNameStringOffset() const {
    return (*Converter) (Header.sh_name);
  }

protected:
  /// Loads a section from a input stream at a specific stream offset.
  ///
  /// \param stream The input stream to load from
  /// \param offset The offset at which to load the section
  void loadSection(std::ifstream& stream, std::streampos offset) {
    std::fill_n(reinterpret_cast<char*>(&Header), sizeof(Header), '\0');
    stream.seekg(offset);
    stream.read(reinterpret_cast<char*>(&Header), sizeof(Header));

    Elf64_Xword Size = getSize();
    if (Data.empty() && getType() != SHT_NULL && getType() != SHT_NOBITS) {
      try {
        Data = std::string(Size, ' ');
      } catch (const std::bad_alloc&) {
        Data = "";
        Size = 0;
      }
      if (!Data.empty()) {
        stream.seekg((*Converter)(Header.sh_offset));
        stream.read(&Data[0], Size);
      }
    }
  }

  /// Sets the section's member \p Name. This will not touch the file itself.
  ///
  /// \param name The name for the section
  void setName(const std::string& name) {
    Name = name;
  }

  /// Sets the section's member \p Index. This will not touch the file itself.
  ///
  /// \param index The index for the section
  void setIndex(const Elf64_Half index) {
    Index = index;
  }

}; // end of class SectionImpl


// Template implementation of StringSection
template <class T>
class StringSectionImpl final : public SectionImpl<T>, virtual public StringSection {

public:
  StringSectionImpl(const std::shared_ptr<EndianessConverter> converter) : SectionImpl<T>(converter) {}

  /// Copy constructor of \p StringSectionImpl.
  ///
  /// \param other The instance to copy
  StringSectionImpl(const StringSectionImpl& other) : SectionImpl<T>(other) {}

  /// Constructor of \p StringSectionImpl. Constructs a new instance out of an
  /// existing instance of \p SectionImpl.
  ///
  /// \param other The base instance
  StringSectionImpl(const SectionImpl<T>& other) : SectionImpl<T>(other) {}

  // creates a new instance from a section pointer
  static const std::shared_ptr<StringSection> fromSection(const std::shared_ptr<Section>& base) {
    if (!base)
      return nullptr;

    std::shared_ptr<StringSection> Result = std::make_shared<StringSectionImpl<T>>(*dynamic_cast<SectionImpl<T>*>(base.get()));

    if (!Result) {
      return nullptr;
    }
    return Result;
  }

  /// Destructor of \p SectionImpl.
  virtual ~StringSectionImpl() {
    // Invokes base class destructor, so nothing to do here.
  }

  // Gets a string from the string section
  const std::string getString(const Elf64_Word index) const {
    if (index < getSize()) {
      return std::string(getData() + index);
    }
    return std::string();
  }

}; // end of class StringSectionImpl


/// Template implementation of DynamicSection
template <class T, class U>
class DynamicSectionImpl final : public SectionImpl<T>, virtual public DynamicSection {

public:
  /// Constructor of \p DynamicSectionImpl.
  ///
  /// \param converter Pointer to an endianess converter
  DynamicSectionImpl(const std::shared_ptr<EndianessConverter> converter) : SectionImpl<T>(converter) {}

  /// Copy constructor of \p DynamicSectionImpl.
  ///
  /// \param other The instance to copy
  DynamicSectionImpl(const DynamicSectionImpl& other) : SectionImpl<T>(other) {}

  /// Constructor of \p DynamicSectionImpl. Constructs a new instance out of an
  /// existing instance of \p DynamicSectionImpl.
  ///
  /// \param other The base instance
  DynamicSectionImpl(const SectionImpl<T>& other) : SectionImpl<T>(other) {}

  /// Destructor of \p DynamicSectionImpl.
  virtual ~DynamicSectionImpl() {
    // Invokes base class destructor, so nothing to do here.
  }

  // returns number of entries
  const Elf64_Xword getNumEntries() const {
    if (getEntrySize() != 0) {
      return getSize() / getEntrySize();
    }
    return 0;
  }

  // creates a new instance from a section pointer
  static const std::shared_ptr<DynamicSection> fromSection(const std::shared_ptr<Section>& base) {
    if (!base)
      return nullptr;

    std::shared_ptr<DynamicSection> Result = std::make_shared<DynamicSectionImpl<T, U>>(*dynamic_cast<SectionImpl<T>*>(base.get()));

    if (!Result) {
      return nullptr;
    }
    return Result;
  }

  // return entry at index \p index
  const std::shared_ptr<DynamicSectionEntry> getEntry(const Elf64_Xword index) const {
    if (index >= getNumEntries()) {
      return nullptr;
    }
    auto Result = std::make_shared<DynamicSectionEntry>();
    auto C = this->Converter;

    if (!getData() || (index + 1) * getEntrySize() > getSize()) {
      return nullptr;
    }

    const U* pEntry = reinterpret_cast<const U*>(getData() + index * getEntrySize());
    Result->tag = (*C) (pEntry->d_tag);

    switch(Result->tag) {
    case DT_NULL:
    case DT_SYMBOLIC:
    case DT_TEXTREL:
    case DT_BIND_NOW:
      Result->value = 0;
      break;
    case DT_NEEDED:
    case DT_PLTRELSZ:
    case DT_RELASZ:
    case DT_RELAENT:
    case DT_STRSZ:
    case DT_SYMENT:
    case DT_SONAME:
    case DT_RPATH:
    case DT_RELSZ:
    case DT_RELENT:
    case DT_PLTREL:
    case DT_INIT_ARRAYSZ:
    case DT_FINI_ARRAYSZ:
    case DT_RUNPATH:
    case DT_FLAGS:
    case DT_PREINIT_ARRAYSZ:
      Result->value = (*C) (pEntry->d_un.d_val);
      break;
    case DT_PLTGOT:
    case DT_HASH:
    case DT_STRTAB:
    case DT_SYMTAB:
    case DT_RELA:
    case DT_INIT:
    case DT_FINI:
    case DT_REL:
    case DT_DEBUG:
    case DT_JMPREL:
    case DT_INIT_ARRAY:
    case DT_FINI_ARRAY:
    case DT_PREINIT_ARRAY:
    default:
      Result->value = (*C) (pEntry->d_un.d_ptr);
      break;
    }
    return Result;
  }

  // return all entries
  const std::vector<DynamicSectionEntry> getAllEntries() const {
    std::shared_ptr<DynamicSectionEntry> Entry {nullptr};
    std::vector<DynamicSectionEntry> Result {};

    for (Elf64_Xword iter = 0; iter < getNumEntries(); ++iter) {
      Entry = getEntry(iter);
      if (Entry) {
        Result.push_back(*Entry);
      }
    }
    return Result;
  }

}; // end of class DynamicSectionImpl


} // end of namespace libelfpp

#endif //LIBELFPP_PRIVATE_IMPL_H
