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