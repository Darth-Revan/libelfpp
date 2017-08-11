# libelfpp

An easy-to-use object-oriented library for reading ELF files.

[![The MIT License](https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square)](http://opensource.org/licenses/MIT)

## Introduction

I recently worked on a project where I needed to read a lot of properties from
ELF files (especially section and segment stuff) in C++. The standard way of
doing this is by using the good old `libelf` or `libbfd`. But those are written
in and for C, so no chance to use features of modern C++ without writing large
wrappers.

There are a few libraries for handling ELFs in C++ out there, but those did not
work satisfactorily for me. So I decided to write a modern, feature-rich library
myself, from ground up.

This is the result: A shared library that can be used to easily parse ELF files
in C++ and extract various information from them. It uses modern features of C++
(at least C++ 11 required) and provides an abstract API to use, so you will not
need to worry about encoding stuff or supporting different address sizes.
Additionally, every aspect of an ELF file (if it is supported by the library) is
an object: the ELF file itself, every single section, symbols, relocation
entries, ...So you can just get some aspect of the file as an object and call
some function on it to get the information you need.

## Building

Building the library requires a C++11 compliant compiler (e.g. GCC >= 4.7 or
Clang >= 3.0). There are no external dependencies, except for the standard
header file `elf.h`, which should be part of the `libc` development headers on
most Linux distrubutions. Additionally you will need `CMake` to build the
Makefiles of the project.

In order to build the library, download and extract the source code and execute
the following commands:

```bash
cd /path/to/libelfpp
mkdir build && cd build
cmake ..
make elfpp
```

This will create a shared object called `libelfpp.so` which you can directly use
in your projects and link against. You may also install both the built library
and the library headers on your system by executing:

```bash
make install
```

Note that this will most probably need root permissions as the command will
write files to `/usr/include` and `/usr/lib`.

## Usage

Using the library is quite simple (in fact, this was one of the most important
development goals). To create a handler for an ELF file, simply:

```c++
#include <iostream>
#include <libelfpp/libelfpp.h>
...

try {
    // creates a handler for the file and throws an exception on failure
    auto file = libelfpp::ELFFile("/path/to/file");
} catch (const std::runtime_error& err) {
    std::cerr << "ERROR: " << err.what() << "\n";
}
```

Now you can simply call various member functions of the returned object in order
to get information from the file (see _API Documentation_ for more info).

Please note, that practically all pointers the library uses are smart pointers
(`shared_ptr` to be precise), so you do not need to worry about freeing pointers
or deallocating memory.

**Examples:**

Get the ELF file's entrypoint:

```c++
// the file object from above
std::cout << file.getHeader()->getEntrypoint() << "\n";
```

Check if the ELF file is a shared object:

```c++
if (file.getHeader()->getELFType() == ET_DYN) {
    std::cout << "This file is a shared object!\n";
}
```

Print the names of all sections in the file:

```c++
for (const auto& section : file.sections()) {
    std::cout << section->getName() << "\n";
}
```

Print the name of the first symbol section and the names of all symbols in it:

```c++
auto section = file.symbolSections()[0];
std::cout << "Section Name: " << section->getName() << "\n";
for (const auto& symbol : section->getAllSymbols()) {
    std::cout << symbol->name << " ";
}
```

Print the offset and addend of the 25th entry in the second relocation section:

```c++
auto reloc = file.relocationSections()[1];
auto entry = reloc->getEntry(24);
std::cout << entry->Offset << " " << entry->Addend << "\n";
```

There are two simple example programs in the subfolder `examples` to show usage
of the library. One is a simple command line utility that prints header
information of an ELF file, the other one is a simple `readelf`-like tool using
`libelfpp`. To build the example programs, pass the option `-DBUILD_EXAMPLES=ON`
to CMake before building and run:

```bash
make example
make readelfpp
```

## Testing

The source code includes a large selection of tests in the subfolder `test`.
Tests have been realized with `Catch` (included). The tests read and parse
4 ELF files (the library itself, two small example binaries and a small test
library). Source code of the example programs is included, of course.

To run the tests, execute the following in your build directory where you also
built the library:

```bash
make test_elfpp
./test_elfpp
```

This will compile the test executable and execute the tests. The test consist of
a few hundred single assertions and should test all aspects of the library.

The library has been tested for memory leaks and errors with
[Valgrind](http://www.valgrind.org/). To test it yourself, install the tool and
run:

```bash
valgrind --show-leak-kinds=all --leak-check=full ./test_elfpp
```

---

**Note**: Valgrind can only check executables, but not libraries. So, to check a
library, one has to write a program using the features that one wants to be
tested.

---

The results should not show any errors or warnings. If they do, please report
a bug (see _Bugs_).

## API Documentation

The source code of the library contains a lot of comments and documentation
formatted to be automatically extracted by _Doxygen_. The complete documentation
in HTML form can be generated from source by executing

```bash
make doc
```

in the build directory. The documentation only includes public classes and
functions that are exposed to library users, but not hidden implementation
details (although they are documented in Doxygen-style, too).

## Bugs

No bugs I know of. If you find something, please open an issue under [libelfpp Issues](https://github.com/Darth-Revan/libelfpp/issues).

## License

This software is provided under the terms of the MIT License. See `LICENSE` for
more information.