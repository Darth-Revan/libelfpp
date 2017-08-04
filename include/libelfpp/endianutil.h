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
 * \file        endianutil.h
 * \brief       This header file declares a class for converting between
 *              little and big endian.
 * \author      Darth-Revan
 * \date        2017
 * \copyright   MIT License
 *
 * This header file declares a class for converting between little and big
 * endian.
 */

#ifndef LIBELFPP_ENDIANUTIL_H
#define LIBELFPP_ENDIANUTIL_H

#include <cstdint>

namespace libelfpp {

/// Class for convenient conversion between Big and Little Endian encoding
class EndianessConverter final {

private:
  /// \p true if conversion is needed
  bool NeedConv;

  /// Gets the host system's native encoding and returns it as \p Endianess.
  ///
  /// \return System's native encoding
  inline bool isHostLittleEndian() const {
    constexpr int tmp = 1;
    return (1 == *(char*) & tmp);
  }

public:
  /// Constructor of \p EndianessConverter. The host's encoding will be detected
  /// automatically.
  ///
  /// \param littleEndian Encoding is little endian (\p true)
  EndianessConverter(const bool littleEndian) {
    NeedConv = (isHostLittleEndian() != littleEndian);
  }

  /// Constructor of \p EndianessConverter.
  ///
  /// \param host_enc The host's encoding (\p true for Little Endian)
  /// \param file_enc The encoding of the ELF file (\p true for Little Endian)
  EndianessConverter(const bool host_enc, const bool file_enc) {
    NeedConv = (host_enc != file_enc);
  }

  /// Conversion function for int8_t.
  ///
  /// \param value The value to convert
  /// \return Converted value or \p value if no conversion needed
  int8_t operator()(int8_t value) const {
    return value;
  }

  /// Conversion function for uint8_t.
  ///
  /// \param value The value to convert
  /// \return Converted value or \p value if no conversion needed
  uint8_t operator()(uint8_t value) const {
    return value;
  }

  /// Conversion function for uint16_t.
  ///
  /// \param value The value to convert
  /// \return Converted value or \p value if no conversion needed
  uint16_t operator()(uint16_t value) const {
    if (!NeedConv)
      return value;
    value = static_cast<uint16_t>(((value & 0x00FF) << 8) |
        ((value & 0xFF00) >> 8));
    return value;
  }

  /// Conversion function for int16_t.
  ///
  /// \param value The value to convert
  /// \return Converted value or \p value if no conversion needed
  int16_t operator()(int16_t value) const {
    if (!NeedConv)
      return value;
    return (int16_t)(*this)((uint16_t) value);
  }

  /// Conversion function for int32_t.
  ///
  /// \param value The value to convert
  /// \return Converted value or \p value if no conversion needed
  int32_t operator()(int32_t value) const {
    if (!NeedConv)
      return value;
    return (int32_t)(*this)((uint32_t) value);
  }

  /// Conversion function for uint32_t.
  ///
  /// \param value The value to convert
  /// \return Converted value or \p value if no conversion needed
  uint32_t operator()(uint32_t value) const  {
    if (!NeedConv)
      return value;
    value = static_cast<uint32_t>(
        ((value & 0x000000FF) << 24) |
            ((value & 0x0000FF00) << 8) |
            ((value & 0x00FF0000) >> 8) |
            ((value & 0xFF000000) >> 24)
    );
    return value;
  }

  /// Conversion function for int64_t.
  ///
  /// \param value The value to convert
  /// \return Converted value or \p value if no conversion needed
  int64_t operator()(int64_t value) const {
    if (!NeedConv)
      return value;
    return (int64_t)(*this)((uint64_t) value);
  }

  /// Conversion function for uint64_t.
  ///
  /// \param value The value to convert
  /// \return Converted value or \p value if no conversion needed
  uint64_t operator()(uint64_t value) const {
    if (!NeedConv)
      return value;
    value = static_cast<uint64_t>(
        ((value & 0x00000000000000FFull) << 56) |
            ((value & 0x000000000000FF00ull) << 40) |
            ((value & 0x0000000000FF0000ull) << 24) |
            ((value & 0x00000000FF000000ull) << 8) |
            ((value & 0x000000FF00000000ull) >> 8) |
            ((value & 0x0000FF0000000000ull) >> 24) |
            ((value & 0x00FF000000000000ull) >> 40) |
            ((value & 0xFF00000000000000ull) >> 56)
    );
    return value;
  }

};

} // end of namespace libelfpp

#endif //LIBELFPP_ENDIANUTIL_H
