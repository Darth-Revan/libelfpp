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
 * \file        readelfpp.cpp
 * \brief       A simple clone of readelf using \p libelfpp
 * \author      Kevin Kirchner
 * \date        2017
 * \copyright   MIT LICENSE
 *
 * A simple clone of readelf from the GNU binutils using \p libelfpp. This
 * application is just a example and does not implement all features readelf
 * provides.
 */

#include <libelfpp/libelfpp.h>
#include <tclap/CmdLine.h>
#include "tinyformat.h"

using namespace libelfpp;


/// Prints the \p header of type \p ELFHeader to the output stream \p stream
/// in a readelf-like style.
///
/// \param header Pointer to the ELF header to print
/// \param stream The stream to print to
void printHeader(const std::shared_ptr<ELFFileHeader>& header,
                 std::ostream& stream) {

  stream << "ELF Header:\n" << std::left;
  tfm::format(stream, "%-39s %s\n", "Class:", (header->is64Bit() ? "ELF64" : "ELF32"));
  tfm::format(stream, "%-39s %s\n", "Version:",
              std::to_string(header->getVersion()) + (header->getVersion() == 1
                                                      ? " (current)" : ""));
  tfm::format(stream, "%-39s %s\n", "Encoding:",
              std::string("2's complement, ") +
                  (header->isLittleEndian() ? "Little" : "Big") + " Endian");
  tfm::format(stream, "%-39s %s\n", "OS/ABI:", header->getABIString());
  tfm::format(stream, "%-39s %s\n", "Type:", header->getELFTypeString());
  tfm::format(stream, "%-39s %s\n", "Machine:", header->getMachineString());
  tfm::format(stream, "%-39s 0x%X\n", "Entrypoint:", header->getEntryPoint());
  tfm::format(stream, "%-39s %u (Bytes in File)\n", "Start of Program Headers:",
              header->getProgramHeaderOffset());
  tfm::format(stream, "%-39s %u (Bytes in File)\n", "Start of Section Headers:",
              header->getSectionHeaderOffset());
  tfm::format(stream, "%-39s 0x%X\n", "Flags:", header->getFlags());
  tfm::format(stream, "%-39s %u (Bytes)\n", "Size of File Header:",
              header->getHeaderSize());
  tfm::format(stream, "%-39s %u (Bytes)\n", "Size of Program Header:",
              header->getProgramHeaderSize());
  tfm::format(stream, "%-39s %u\n", "Number of Program Headers:",
              header->getProgramHeaderNumber());
  tfm::format(stream, "%-39s %u\n", "Size of Section Header:",
              header->getSectionHeaderSize());
  tfm::format(stream, "%-39s %u\n", "Number of Section Headers:",
              header->getSectionHeaderNumber());
  tfm::format(stream, "%-39s %u\n", "Section Header String Table Index:",
              header->getSectionHeaderStringTableIndex());
}

/// Prints the sections of the file pointed to by \p pFile to the output
/// stream \p stream in a readelf-like style.
///
/// \param pFile Pointer to the file object
/// \param stream The stream to print to
void printSectionTable(const std::shared_ptr<ELFFile>& pFile,
                       std::ostream& stream) {
  stream << "Section Headers:\n" << std::left;
  tfm::format(stream, " [%-2s] %-17s %-17s %-17s %-10s\n", "No", "Name", "Type",
              "Address", "Offset");
  tfm::format(stream, "      %-17s %-17s %-17s %-10s\n", "Size", "Entry Size",
              "Flags Link Info", "Align");

  for (const auto& Section : pFile->sections()) {
    tfm::format(stream, " [%2u] %-17s %-17s %017X %08X\n", Section->getIndex(),
                Section->getName(), Section->getTypeString(),
                Section->getAddress(), Section->getOffset());
    tfm::format(stream, "      %017X %017X %5s %5u %5u %6u\n",
                Section->getSize(), Section->getEntrySize(),
                Section->getFlagsString(), Section->getLink(),
                Section->getInfo(), Section->getAddressAlignment());
  }
  stream << "Key to Flags:\n";
  stream << " W (write), A (alloc), X (execute), M (merge), S (strings), l (large)\n";
  stream << " I (info), L (link order), G (group), T (TLS), E (exclude), x (unkown)\n";
  stream << " O (extra OS processing required), o(OS specific), p (processor specific)\n";
}

/// Prints the segments of the file pointed to by \p pFile to the output
/// stream \p stream in a readelf-like style.
///
/// \param pFile Pointer to the file object
/// \param stream The stream to print to
void printSegmentTable(const std::shared_ptr<ELFFile>& pFile,
                       std::ostream& stream) {
  stream << "Program Headers:\n";
  tfm::format(stream, " %-20s %-20s %-20s %-20s\n", "Type", "Offset",
              "Virtual Address", "Physical Address");
  tfm::format(stream, " %-20s %-20s %-20s %-20s\n", "", "File Size",
              "Memory Size", " Flags  Align");

  for (const auto& Seg : pFile->segments()) {
    tfm::format(stream, " %-20s 0x%018X 0x%018X 0x%018X\n", Seg->getTypeString(),
                Seg->getOffset(), Seg->getVirtualAddress(),
                Seg->getPhysicalAddress());
    tfm::format(stream, " %-20s 0x%018X 0x%018X %6s %8X\n", "",
                Seg->getFileSize(), Seg->getMemorySize(), Seg->getFlagsString(),
                Seg->getAddressAlignment());
  }

  stream << "Mapping of Sections on Segments:\n";
  std::string sectionNames = std::string();

  for (const auto& Seg : pFile->segments()) {
    for (const auto& index : Seg->getAssociatedSections()) {
      sectionNames += pFile->sections().at(index)->getName() + " ";
    }
    tfm::format(stream, " %02d  %s\n", Seg->getIndex(), sectionNames);
    sectionNames.clear();
  }
}


/// Prints the dynamic section of the file pointed to by \p pFile to the
/// stream \p stream in readelf-like style.
///
/// \param pFile Pointer to the file object
/// \param stream The stream to print to
void printDynamicSection(const std::shared_ptr<ELFFile>& pFile,
                         std::ostream& stream) {
  tfm::format(stream, "Dynamic section contains %d entries:\n",
              pFile->getDynamicSection()->getNumEntries());

  tfm::format(stream, "  %-20s %-20s %-30s\n", "Tag", "Type", "Value");

  for (const auto& entry : pFile->getDynamicSection()->getAllEntries()) {
    tfm::format(stream, " 0x%018X %-20s %d\n", entry.tag, entry.getTypeString(), entry.value);
  }
}

/// Prints the symbol sections of an ELF file to the output stream \p stream
/// in a readelf-like style.
///
/// \param pFile Pointer to the file object
/// \param stream The stream to print to
void printSymbolSections(const std::shared_ptr<ELFFile>& pFile,
                         std::ostream& stream) {

  for (const auto& SymSec : pFile->symbolSections()) {
    tfm::format(stream, "Symbol table '%s' contains %d entries:\n",
                SymSec->getName(), SymSec->getNumSymbols());
    tfm::format(stream, "%6s:    %-15s %-5s %-8s %-8s %-5s %-25s\n", "Num",
                "Value", "Size", "Type", "Bind", "Ndx", "Name");

    std::shared_ptr<Symbol> Sym;
    for (size_t i = 0; i < SymSec->getNumSymbols(); ++i) {
      Sym = SymSec->getSymbol(i);
      if (!Sym) continue;

      tfm::format(stream, "%6d: %016X %5d %-8s %-8s %5d %-25s\n", i,
                  Sym->value, Sym->size, Sym->getTypeString(),
                  Sym->getBindString(), Sym->sectionIndex,
                  Sym->name.substr(0, 25));
    }
    stream << "\n";
  }
}


/// Prints the notes sections of an ELF file to the output stream \p stream
/// in a readelf-like style.
///
/// \param pFile Pointer to the file object
/// \param stream The stream to print to
void printNotesSections(const std::shared_ptr<ELFFile> pFile,
                        std::ostream& stream) {

  for (const auto& section : pFile->noteSections()) {
    tfm::format(stream, "Displaying notes found at file offset 0x%08X with length 0x%08X:\n",
                section->getOffset(),
                section->getSize());
    tfm::format(stream, "%-20s %-12s %-10s\n", "Owner", "Data size", "Type");

    for (size_t i = 0; i < section->getNumEntries(); i++) {
      auto entry = section->getEntry(i);
      tfm::format(stream, "%-20s 0x%08X 0x%08X\n", entry->Name, entry->Description.length(), entry->Type);
    }
    stream << "\n";
  }
}

/// Prints the relocation sections of an ELF file to the output stream \p stream
/// in a readelf-like style.
///
/// \param pFile Pointer to the file object
/// \param stream The stream to print to
void printRelocSections(const std::shared_ptr<ELFFile> pFile,
                        std::ostream& stream) {
  for (const auto& section : pFile->relocationSections()) {
    tfm::format(stream, "Relocation section '%s' at offset 0x%X contains %d entries:\n",
                section->getName(), section->getOffset(), section->getNumEntries());
    tfm::format(stream, "%-12s %-12s %-8s %-16s %-55s\n", "Offset", "Info",
                "Type", "Sym. Value", "Sym. Name + Addend");

    for (const auto& entry : section->getAllEntries()) {
      tfm::format(stream, "%012X %012X %08X %016X %s + %X\n",
                  entry->Offset, entry->Info, entry->Type,
                  entry->SymbolInstance->value,
                  entry->SymbolInstance->name.substr(0, 45), entry->Addend);
    }
    stream << "\n";
  }
}


/// Main function of \p readelfpp.
///
/// \param argc Number of arguments
/// \param argv Arguments as array of strings
/// \return Integer value indicating termination state (0 means 'sucess',
///         everything else means 'failure')
int main(int argc, const char* argv[]) {
  try {
    TCLAP::CmdLine Cmd("Simple clone of readelf", ' ', getVersionString());
    TCLAP::UnlabeledValueArg<std::string> FileNameArg("file", "The name of the ELF file to read", true, "", "filename");
    TCLAP::SwitchArg HeaderSwitch("f", "file-header", "Displays the information contained in the ELF header at the start of the file.", false);
    TCLAP::SwitchArg SegmentSwitch("l", "segments", "Displays the information contained in the file's segment headers, if it has any.", false);
    TCLAP::SwitchArg SectionSwitch("S", "sections", "Displays the information contained in the file's section headers, if it has any.", false);
    TCLAP::SwitchArg AllHeadersSwitch("e", "headers", "Display all the headers in the file.  Equivalent to -f -l -S.", false);
    TCLAP::SwitchArg SymbolSwitch("s", "symbols", "Displays the entries in symbol table section of the file, if it has one.", false);
    TCLAP::SwitchArg DynamicSwitch("d", "dynamic", "Displays the contents of the file's dynamic section, if it has one.", false);
    TCLAP::SwitchArg NotesSwitch("n", "notes", "Displays the contents of any notes sections, if any.", false);
    TCLAP::SwitchArg RelocSwitch("r", "relocs", "Displays the contents of the file's relocation section, if it has one.", false);

    Cmd.add(HeaderSwitch);
    Cmd.add(SegmentSwitch);
    Cmd.add(SectionSwitch);
    Cmd.add(AllHeadersSwitch);
    Cmd.add(SymbolSwitch);
    Cmd.add(DynamicSwitch);
    Cmd.add(FileNameArg);
    Cmd.add(NotesSwitch);
    Cmd.add(RelocSwitch);
    Cmd.parse(argc, argv);

    try {
      auto pFile = std::make_shared<ELFFile>(FileNameArg.getValue());

      bool All = AllHeadersSwitch.getValue();
      if (HeaderSwitch.getValue() || All) {
        printHeader(pFile->getHeader(), std::cout);
      }
      if (SectionSwitch.getValue() || All) {
        printSectionTable(pFile, std::cout);
      }
      if (SegmentSwitch.getValue() || All) {
        printSegmentTable(pFile, std::cout);
      }
      if (SymbolSwitch.getValue()) {
        printSymbolSections(pFile, std::cout);
      }
      if (DynamicSwitch.getValue()) {
        printDynamicSection(pFile, std::cout);
      }
      if (NotesSwitch.getValue()) {
        printNotesSections(pFile, std::cout);
      }
      if (RelocSwitch.getValue()) {
        printRelocSections(pFile, std::cout);
      }
    } catch (const std::runtime_error& err) {
      std::cerr << "ERROR: Creation of file " << FileNameArg.getValue() << " failed: " << err.what() << "\n";
      return 1;
    }
  } catch (TCLAP::ArgException& e) {
    std::cerr << "ERROR: " << e.error() << "\n";
    return 1;
  }
  return 0;
}