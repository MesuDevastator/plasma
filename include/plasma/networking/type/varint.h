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
#include <plasma/extern.h>
#include <plasma/networking/type/varint_exception.h>

namespace plasma::networking::type
{
    constexpr const std::byte segment_bits{ 0x7f };
    constexpr const std::byte continue_bit{ 0x80 };
    PLASMA_EXTERN int32_t read_varint(std::byte* data, std::size_t max_length);
    PLASMA_EXTERN void write_varint(int32_t value, std::byte* data, std::size_t max_length);
    PLASMA_EXTERN int64_t read_varlong(std::byte* data, std::size_t max_length);
    PLASMA_EXTERN void write_varlong(int64_t value, std::byte* data, std::size_t max_length);
}
