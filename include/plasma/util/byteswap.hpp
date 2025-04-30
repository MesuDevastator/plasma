/*
 * Copyright (c) 2023-2025 Mesu Devastator
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

#include <bit>
#if !defined(__cpp_lib_byteswap)
#include <algorithm>
#include <concepts>
#endif

namespace plasma::util
{
#if !defined(__cpp_lib_byteswap)
template <typename T>
concept byte_swappable = std::has_unique_object_representations_v<T> && std::is_integral_v<T>;

template <byte_swappable T> constexpr T byteswap(T value) noexcept
{
    auto value_representation{bit_cast<std::array<std::byte, sizeof(T)>>(value)};
    std::ranges::reverse(value_representation);
    return bit_cast<T>(value_representation);
}
#else
template <std::integral T> auto &byteswap{std::byteswap<T>};
#endif
} // namespace plasma::util
