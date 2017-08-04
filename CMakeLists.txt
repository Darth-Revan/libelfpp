cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# set C++11 as required
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

# set additional flags for GCC and Clang
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message("-- Setting additional flags for GCC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Werror -Wno-c++98-compat -Wno-c++98-compat-pedantic")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    message("-- Setting additional flags for Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Werror -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded")
endif()

# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)
set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

add_definitions(-DELFPP_VERSION="${VERSION}")

#configure target for building documentation with Doxygen
option(BUILD_DOCUMENTATION "Create HTML documentation of API with Doxygen" OFF)
if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed for building documentation")
    endif()
    message("-- Building documentation with Doxygen")
    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)
    add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generate documentation with Doxygen"
            VERBATIM)
endif()

project(libelfpp VERSION ${VERSION} LANGUAGES CXX)

INCLUDE(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("tclap/CmdLine.h" HAVE_TCLAP)
CHECK_INCLUDE_FILE_CXX("elf.h" HAVE_ELFH)

if (NOT HAVE_ELFH)
    message(FATAL_ERROR "The standard header elf.h could not be found, but is required to build the library!")
endif()

include_directories(include)
set(SOURCES include/libelfpp/ src/libelfpp.cpp src/private_impl.h src/private_impl.cpp)
add_library(elfpp SHARED ${SOURCES})

target_include_directories(elfpp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include> PRIVATE src)
install(DIRECTORY include/libelfpp DESTINATION include)
install(TARGETS elfpp DESTINATION lib)

add_executable(test_elfpp test/catch.h test/main.cpp)
target_link_libraries(test_elfpp elfpp)

# build example programs if desired
option(BUILD_EXAMPLES "Builds example binaries in directory 'examples'." OFF)
if(BUILD_EXAMPLES)
    add_executable(example examples/simple_example.cpp)
    target_link_libraries(example elfpp)

    if(NOT HAVE_TCLAP)
        message(WARNING "Building readelfpp required TCLAP, but could not be found!")
    else()
        add_executable(readelfpp examples/readelfpp.cpp examples/tinyformat.h)
        target_link_libraries(readelfpp elfpp)
    endif()
endif()