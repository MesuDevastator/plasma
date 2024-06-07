/*
 * Copyright (c) 2023-2024 Mesu Devastator
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <plasma/extern.hpp>
#include <plasma/networking/type/varint_exception.hpp>

namespace plasma::networking::type
{
    constexpr const std::byte segment_bits{ 0x7f };
    constexpr const std::byte continue_bit{ 0x80 };
    constexpr const std::size_t varint_max_size{ 5 };
    constexpr const std::size_t varlong_max_size{ 10 };
    PLASMA_EXTERN std::int32_t read_varint(const std::byte* const data, const std::size_t max_length = varint_max_size);
    PLASMA_EXTERN std::int32_t read_varint(const std::byte* const data, std::size_t& length, const std::size_t max_length = varint_max_size);
    PLASMA_EXTERN std::size_t write_varint(std::int32_t value, std::byte* const data, const std::size_t max_length = varint_max_size);
    PLASMA_EXTERN std::size_t get_varint_length(std::int32_t value);
    PLASMA_EXTERN std::size_t get_varint_length(const std::byte* const data, const std::size_t max_length = varint_max_size);

    PLASMA_EXTERN std::int64_t read_varlong(const std::byte* const data, const std::size_t max_length = varlong_max_size);
    PLASMA_EXTERN std::int64_t read_varlong(const std::byte* const data, std::size_t& length, const std::size_t max_length = varlong_max_size);
    PLASMA_EXTERN std::size_t write_varlong(std::int64_t value, std::byte* const data, const std::size_t max_length = varlong_max_size);
    PLASMA_EXTERN std::size_t get_varlong_length(std::int64_t value);
    PLASMA_EXTERN std::size_t get_varlong_length(const std::byte* const data, const std::size_t max_length = varlong_max_size);

    PLASMA_EXTERN std::uint16_t read_ushort(const std::byte* const data, const std::size_t max_length = sizeof(std::uint16_t));
    PLASMA_EXTERN std::int64_t read_long(const std::byte* const data, const std::size_t max_length = sizeof(std::int64_t));
}
