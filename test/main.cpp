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

#define CATCH_CONFIG_MAIN
#include "catch.h"
#include "libelfpp/libelfpp.h"

using namespace libelfpp;

TEST_CASE("EndianessConverter", "[libelfpp]") {
  EndianessConverter Converter(true, true);
  REQUIRE(Converter(0xDEADBEEF) == 0xDEADBEEF);
  REQUIRE(Converter(0x01) == 0x01);
  EndianessConverter Converter2(false, false);
  REQUIRE(Converter2(0xDEADBEEF) == 0xDEADBEEF);
  REQUIRE(Converter2(0x01) == 0x01);
  EndianessConverter Converter3(false, true);
  REQUIRE(Converter3(0xDEADBEEF) == 0xEFBEADDE);
  REQUIRE(Converter3((uint16_t) 0x01) == 0x0100);
  REQUIRE(Converter3(0x00102442) == 0x42241000);
  EndianessConverter Converter4(true, false);
  REQUIRE(Converter4(0xDEADBEEF) == 0xEFBEADDE);
  REQUIRE(Converter4((uint16_t) 0x01) == 0x0100);
  REQUIRE(Converter4(0x00102442) == 0x42241000);
}

TEST_CASE("getVersionString", "[libelfpp]") {
  REQUIRE_FALSE(getVersionString().empty());
  REQUIRE(getVersionString().compare(ELFPP_VERSION) == 0);
}

TEST_CASE("ELFFile", "[libelfpp]") {
  REQUIRE_THROWS_AS(ELFFile("nonexistingfilename"), std::runtime_error);
  ELFFile file("libelfpp.so");
  REQUIRE(file.getName().compare("libelfpp.so") == 0);
}

ELFFile file("libelfpp.so");

TEST_CASE("Compare operators", "[libelfpp]") {
  ELFFile test("test_elfpp");
  REQUIRE(test != file);
}

TEST_CASE("Header access", "[libelfpp]") {
  auto header = file.getHeader();
  REQUIRE(file.getHeader());
  REQUIRE(header->getProgramHeaderNumber());
  REQUIRE(header->getEntryPoint());
  REQUIRE_FALSE(header->getELFTypeString().empty());
  REQUIRE_FALSE(header->getMachineString().empty());
  REQUIRE_FALSE(header->getABIString().empty());
#ifdef LITTLE_ENDIAN
  REQUIRE(header->isLittleEndian());
#else
  REQUIRE_FALSE(file.isLittleEndian());
#endif
#if UINTPTR_MAX == 0xffffffff
  REQUIRE_FALSE(file.is64Bit());
#elif UINTPTR_MAX == 0xffffffffffffffff
  REQUIRE(header->is64Bit());
#else
#error What the heck!? Your CPU is neither 32 nor 64 Bit!
#endif
}

TEST_CASE("Segment access", "[libelfpp]") {
  REQUIRE(file.segments().size() > 0);
  REQUIRE(file.segments().size() == file.getHeader()->getProgramHeaderNumber());
  REQUIRE_THROWS_AS(file.segments().at(-1000), std::out_of_range);
  auto Seg = file.segments().at(2);
  REQUIRE(Seg);
  REQUIRE(Seg->getFileSize() > 0);
  REQUIRE(Seg->getMemorySize() > 0);
  REQUIRE(Seg->getData());
  REQUIRE_FALSE(Seg->getDataString().empty());
  REQUIRE(Seg->getDataString().size() == Seg->getFileSize());
}


TEST_CASE("Section Access", "[libelfpp]") {
  REQUIRE(file.sections().size() > 0);
  REQUIRE(file.sections().size() == file.getHeader()->getSectionHeaderNumber());
  REQUIRE_THROWS_AS(file.sections().at(-1000), std::out_of_range);
  auto Sec = file.sections().at(10);
  REQUIRE(Sec);
  REQUIRE(Sec->getSize() > 0);
  REQUIRE(Sec->getNameStringOffset());
  REQUIRE(Sec->getData());
  REQUIRE(Sec->getAddress());
  REQUIRE_FALSE(Sec->getName().empty());
  REQUIRE(Sec->getName() == file.getStringSection()->getString(Sec->getNameStringOffset()));
  REQUIRE(Sec->getDataString().size() == Sec->getSize());
}

TEST_CASE("Dynamic Section access", "[libelfpp]") {
  auto dyn = file.getDynamicSection();
  REQUIRE(dyn);
  REQUIRE_FALSE(dyn->getDataString().empty());
  REQUIRE(dyn->getSize() > 0);
  REQUIRE(dyn->getSize() == file.sections()[dyn->getIndex()]->getSize());
  REQUIRE_FALSE(dyn->getName().empty());
  REQUIRE(dyn->getNumEntries() > 0);
  REQUIRE_FALSE(dyn->getEntry(dyn->getNumEntries() + 100));
  REQUIRE(dyn->getAllEntries().size() == dyn->getNumEntries());
  REQUIRE_FALSE(file.getNeededLibraries().empty());
}

TEST_CASE("Symbol access", "[libelfpp]") {
  REQUIRE(file.symbolSections().size() > 0);

  for (const auto SymSec : file.symbolSections()) {
    REQUIRE(SymSec);
    REQUIRE(SymSec->getNumSymbols() > 0);
    auto Sym = SymSec->getSymbol(0);
    REQUIRE(Sym->name.empty());
    REQUIRE(Sym->value == 0);
    REQUIRE(Sym->size == 0);
    REQUIRE(Sym->other == 0);
    REQUIRE(Sym->type == STT_NOTYPE);
    REQUIRE(Sym->bind == STB_LOCAL);
    REQUIRE(Sym->sectionIndex == SHN_UNDEF);
  }

  auto SymSec = file.symbolSections().at(0);
  auto Sym = SymSec->getSymbol(100000000);
  REQUIRE_FALSE(Sym);
  Sym = SymSec->getSymbol(30);
  REQUIRE(Sym);
  REQUIRE_FALSE(Sym->name.empty());
}

TEST_CASE("Relocation access", "[libelfpp]") {
  auto relocs = file.relocationSections();
  REQUIRE(relocs.size() > 0);
  auto reloc = relocs[0];
  REQUIRE_FALSE(reloc->getName().empty());
  REQUIRE(reloc->getNumEntries() > 0);
  REQUIRE(reloc->getNumEntries() == reloc->getAllEntries().size());
  auto entry = reloc->getEntry(reloc->getNumEntries() - 1);
  REQUIRE(entry);
  REQUIRE_FALSE(entry->SymbolInstance->name.empty());
  REQUIRE(entry->Offset);
  REQUIRE(entry->Info);
}

TEST_CASE("Note section access", "[libelfpp]") {
  auto notes = file.noteSections();
  REQUIRE(notes.size() > 0);
  auto note = notes[0];
  REQUIRE(note->getNumEntries() == note->getAllEntries().size());
  REQUIRE_FALSE(note->getEntry(note->getNumEntries()));
  if (note->getNumEntries() > 0) {
    REQUIRE(note->getEntry(0));
    REQUIRE(note->getEntry(0)->Type >= 0);  // negative types not allowed
  }
}


TEST_CASE("Read example file hello_world", "[test/hello_world]") {
  ELFFile file = ELFFile("hello_world");
  auto head = file.getHeader();
  REQUIRE(head);
  REQUIRE(head->isLittleEndian());
  REQUIRE_FALSE(head->is64Bit());
  REQUIRE(head->getVersion() == 1);
  REQUIRE(head->getELFTypeString() == "Executable");
  REQUIRE(head->getEntryPoint() == 134513904);
  REQUIRE(head->getProgramHeaderSize() == 32);
  REQUIRE(head->getProgramHeaderNumber() == 9);
  REQUIRE(head->getSectionHeaderNumber() == 28);
  REQUIRE(head->getSectionHeaderStringTableIndex() == 27);
  REQUIRE(head->getMachineString() == "Intel 80386");

  auto sections = file.sections();
  REQUIRE(sections.size() == 28);
  auto bss = sections[25];
  REQUIRE(bss->getName() == ".bss");
  REQUIRE(bss->getTypeString() == "NOBITS");
  REQUIRE(bss->getAddress() == 134520896);
  REQUIRE(bss->getOffset() == 4132);
  REQUIRE(bss->getSize() == 144);
  REQUIRE(bss->getFlagsString() == "WA");
  REQUIRE(bss->getInfo() == 0);

  auto segments = file.segments();
  REQUIRE(segments.size() == 9);
  auto interp = segments[1];
  REQUIRE(interp->getType() == PT_INTERP);
  REQUIRE(interp->getOffset() == 340);
  REQUIRE(interp->getVirtualAddress() == 134512980);
  REQUIRE(interp->getPhysicalAddress() == 134512980);
  REQUIRE(interp->getFileSize() == 19);
  REQUIRE(interp->getMemorySize() == 19);
  REQUIRE(interp->getFlagsString() == "R");
  REQUIRE(interp->getAddressAlignment() == 1);
  REQUIRE(interp->getAssociatedSections().size() == 1);

  auto dyn = file.getDynamicSection();
  REQUIRE(dyn);
  REQUIRE(dyn->getNumEntries() == 32);
  REQUIRE(dyn->getOffset() == 3836);
  REQUIRE(dyn->getName() == ".dynamic");
  auto init = dyn->getEntry(4);
  REQUIRE(init);
  REQUIRE(init->tag == DT_INIT);
  REQUIRE(init->getTypeString() == "INIT");
  REQUIRE(init->value == 134513672);
  init = dyn->getEntry(13);
  REQUIRE(init);
  REQUIRE(init->tag == DT_STRSZ);
  REQUIRE(init->getTypeString() == "STRSZ");
  REQUIRE(init->value == 259);

  auto req = file.getNeededLibraries();
  REQUIRE(req.size());
  REQUIRE(std::find(req.begin(), req.end(), "libc.so.6") != req.end());

  auto symbolSecs = file.symbolSections();
  REQUIRE(symbolSecs.size() == 1);
  auto syms = symbolSecs[0];
  REQUIRE(syms->getName() == ".dynsym");
  REQUIRE(syms->getNumSymbols() == 9);
  auto sym = syms->getSymbol(8);
  REQUIRE(sym);
  REQUIRE(sym->value == 134520896);
  REQUIRE(sym->size == 140);
  REQUIRE(sym->sectionIndex == 25);
  REQUIRE(sym->getBindString() == "GLOBAL");
  REQUIRE(sym->getTypeString() == "OBJECT");
  REQUIRE(sym->name == "_ZSt4cout");

  auto notes = file.noteSections();
  REQUIRE(notes.size() == 2);
  auto note = notes[0];
  REQUIRE(note);
  REQUIRE(note->getOffset() == 360);
  REQUIRE(note->getSize() == 32);
  REQUIRE(note->getNumEntries() == 1);
  REQUIRE(note->getEntry(0)->Name == "GNU");

  auto relocs = file.relocationSections();
  REQUIRE(relocs.size() == 2);
  auto reloc = relocs[1];
  REQUIRE(reloc);
  REQUIRE(reloc->getName() == ".rel.plt");
  REQUIRE(reloc->getNumEntries() == 5);
  REQUIRE(reloc->getOffset() == 992);
  auto entry = reloc->getEntry(3);
  REQUIRE(entry->Offset == 134520856);
  REQUIRE(entry->Type == R_386_JMP_SLOT);
  REQUIRE(entry->Info == 1799);
  REQUIRE(entry->Addend == 0);
  REQUIRE(entry->SymbolInstance->name == "_ZNSt8ios_base4InitD1Ev");
  REQUIRE(entry->SymbolInstance->value == 134513776);
}


TEST_CASE("Read example file fibonacci", "[test/fibonacci]") {
  ELFFile file = ELFFile("fibonacci");
  auto head = file.getHeader();
  REQUIRE(head);
  REQUIRE(head->isLittleEndian());
  REQUIRE(head->is64Bit());
  REQUIRE(head->getVersion() == 1);
  REQUIRE(head->getELFTypeString() == "Executable");
  REQUIRE(head->getEntryPoint() == 4195968);
  REQUIRE(head->getProgramHeaderSize() == 56);
  REQUIRE(head->getProgramHeaderNumber() == 9);
  REQUIRE(head->getSectionHeaderNumber() == 27);
  REQUIRE(head->getSectionHeaderStringTableIndex() == 26);
  REQUIRE(head->getMachineString() == "Advanced Micro Devices X86-64 processor");

  auto sections = file.sections();
  REQUIRE(sections.size() == 27);
  auto bss = sections[22];
  REQUIRE(bss->getName() == ".got.plt");
  REQUIRE(bss->getTypeString() == "PROGBITS");
  REQUIRE(bss->getAddress() == 6295552);
  REQUIRE(bss->getOffset() == 4096);
  REQUIRE(bss->getSize() == 64);
  REQUIRE(bss->getFlagsString() == "WA");
  REQUIRE(bss->getAddressAlignment() == 8);

  auto segments = file.segments();
  REQUIRE(segments.size() == 9);
  auto load = segments[2];
  REQUIRE(load->getType() == PT_LOAD);
  REQUIRE(load->getOffset() == 0);
  REQUIRE(load->getVirtualAddress() == 4194304);
  REQUIRE(load->getPhysicalAddress() == 4194304);
  REQUIRE(load->getFileSize() == 2644);
  REQUIRE(load->getMemorySize() == 2644);
  REQUIRE(load->getFlags() == 5);
  REQUIRE(load->getAddressAlignment() == 2097152);
  REQUIRE(load->getAssociatedSections().size() == 18);


  auto dyn = file.getDynamicSection();
  REQUIRE(dyn);
  REQUIRE(dyn->getNumEntries() == 32);
  REQUIRE(dyn->getOffset() == 3568);
  REQUIRE(dyn->getName() == ".dynamic");
  auto e = dyn->getEntry(5);
  REQUIRE(e);
  REQUIRE(e->tag == DT_FINI);
  REQUIRE(e->getTypeString() == "FINI");
  REQUIRE(e->value == 4196500);
  e = dyn->getEntry(19);
  REQUIRE(e);
  REQUIRE(e->tag == DT_JMPREL);
  REQUIRE(e->getTypeString() == "JMPREL");
  REQUIRE(e->value == 4195624);

  auto req = file.getNeededLibraries();
  REQUIRE(req.size());
  REQUIRE(std::find(req.begin(), req.end(), "libc.so.6") != req.end());

  auto symbolSecs = file.symbolSections();
  REQUIRE(symbolSecs.size() == 1);
  auto syms = symbolSecs[0];
  REQUIRE(syms->getName() == ".dynsym");
  REQUIRE(syms->getNumSymbols() == 9);
  auto sym = syms->getSymbol(4);
  REQUIRE(sym);
  REQUIRE(sym->value == 0);
  REQUIRE(sym->size == 0);
  REQUIRE(sym->sectionIndex == SHN_UNDEF);
  REQUIRE(sym->getBindString() == "GLOBAL");
  REQUIRE(sym->getTypeString() == "FUNC");
  REQUIRE(sym->name == "__libc_start_main");

  auto notes = file.noteSections();
  REQUIRE(notes.size() == 2);
  auto note = notes[0];
  REQUIRE(note);
  REQUIRE(note->getOffset() == 596);
  REQUIRE(note->getSize() == 32);
  REQUIRE(note->getNumEntries() == 1);
  REQUIRE(note->getEntry(0)->Name == "GNU");

  auto relocs = file.relocationSections();
  REQUIRE(relocs.size() == 2);
  auto reloc = relocs[0];
  REQUIRE(reloc);
  REQUIRE(reloc->getName() == ".rela.dyn");
  REQUIRE(reloc->getNumEntries() == 3);
  REQUIRE(reloc->getOffset() == 1248);
  auto entry = reloc->getEntry(1);
  REQUIRE(entry->Offset == 6295544);
  REQUIRE(entry->Type == R_X86_64_GLOB_DAT);
  REQUIRE(entry->Info == 17179869190);
  REQUIRE(entry->Addend == 0);
  REQUIRE(entry->SymbolInstance->name == "__libc_start_main");
  REQUIRE(entry->SymbolInstance->value == 0);
}