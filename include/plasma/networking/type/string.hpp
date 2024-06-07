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

#include <plasma/extern.hpp>
#include <plasma/networking/type/varint.hpp>
#include <string>

namespace plasma::networking::type
{
    constexpr const std::size_t string_max_size{ (32767 * 3) + 3 };
    PLASMA_EXTERN std::u8string read_string(const std::byte* const data, const std::size_t max_length = string_max_size);
    PLASMA_EXTERN std::u8string read_string(const std::byte* const data, std::size_t& length, const std::size_t max_length = string_max_size);
    PLASMA_EXTERN std::size_t write_string(const std::u8string& string, std::byte* const data, const std::size_t max_length = string_max_size);
}
