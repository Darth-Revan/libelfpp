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
#include "private_impl.h"
#include <cstring>
#include <iostream>

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

  if (Is64Bit) {
    FileHeader = std::make_shared<ELFHeaderImpl<Elf64_Ehdr>>(Converter, IsLittleEndian, Input);
  } else {
    FileHeader = std::make_shared<ELFHeaderImpl<Elf32_Ehdr>>(Converter, IsLittleEndian, Input);
  }

  loadSegmentsFromFile(Input);
  loadSectionsFromFile(Input);
  Input.close();
}

// Loads all segmetns from the file stream
Elf64_Half ELFFile::loadSegmentsFromFile(std::ifstream& stream) {
  Elf64_Half entrySize = FileHeader->getProgramHeaderSize();
  Elf64_Half segmentNumber = FileHeader->getProgramHeaderNumber();
  Elf64_Off offset = FileHeader->getProgramHeaderOffset();

  for (Elf64_Half iter = 0; iter < segmentNumber; ++iter) {
    std::shared_ptr<Segment> Seg;
    Elf64_Off baseOff, endOff, vBaseAddr, vEndAddr;

    if (Is64Bit) {
      Seg = std::make_shared<SegmentImpl<Elf64_Phdr>>(Converter);
    } else {
      Seg = std::make_shared<SegmentImpl<Elf32_Phdr>>(Converter);
    }
    Seg->loadSegment(stream, (std::streamoff) offset + iter * entrySize);
    Seg->setIndex(iter);
    Segments.push_back(Seg);

    // add associated sections
    baseOff = Seg->getOffset();
    endOff = baseOff + Seg->getFileSize();
    vBaseAddr = Seg->getVirtualAddress();
    vEndAddr = vBaseAddr + Seg->getMemorySize();

    for (const auto& Section : Sections) {
      if (Section->getFlags() & SHF_ALLOC) {
        if (vBaseAddr <= Section->getAddress() &&
            Section->getAddress() + Section->getSize() <= vEndAddr) {
          Seg->addSectionIndex(Section->getIndex());
        }
      } else {
        if (baseOff <= Section->getOffset() &&
            Section->getOffset() + Section->getSize() <= endOff) {
          Seg->addSectionIndex(Section->getIndex());
        }
      }
    }
  }

  return segmentNumber;
}


// Loads all sections from the file stream
Elf64_Half ELFFile::loadSectionsFromFile(std::ifstream& stream) {
  Elf64_Half entrySize = FileHeader->getSectionHeaderSize();
  Elf64_Half sectionNumber = FileHeader->getSectionHeaderNumber();
  Elf64_Off offset = FileHeader->getSectionHeaderOffset();

  for (Elf64_Half iter = 0; iter < sectionNumber; ++iter) {
    std::shared_ptr<Section> Sec;
    if (Is64Bit) {
      Sec = std::make_shared<SectionImpl<Elf64_Shdr>>(Converter);
    } else {
      Sec = std::make_shared<SectionImpl<Elf32_Shdr>>(Converter);
    }
    Sec->loadSection(stream, (std::streamoff) offset + iter * entrySize);
    Sec->setIndex(iter);
    Sections.push_back(Sec);
  }

  // get primary string section
  Elf64_Half StringIndex = FileHeader->getSectionHeaderStringTableIndex();
  if (StringIndex != SHN_UNDEF) {

    Section* tmp = Sections[StringIndex].get();
    if (FileHeader->is64Bit()) {
      StrSection =
          std::make_shared<StringSectionImpl<Elf64_Shdr> >(*dynamic_cast<SectionImpl<
              Elf64_Shdr> *>(tmp));
    } else {
      StrSection =
          std::make_shared<StringSectionImpl<Elf32_Shdr> >(*dynamic_cast<SectionImpl<
              Elf32_Shdr> *>(tmp));
    }

    for (const auto& Sec : Sections) {
      Sec->setName(StrSection->getString(Sec->getNameStringOffset()));
    }
  }

  return sectionNumber;
}

} // end of namespace libelfpp