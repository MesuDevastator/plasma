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

#include <span>
#include <stdexcept>

namespace plasma::util
{
template <typename T, std::size_t Extent = std::dynamic_extent>
std::span<T, Extent> checked_subspan(const std::span<T, Extent> span, const std::size_t offset,
                                     const std::size_t length = std::dynamic_extent)
{
    if (offset + length > span.size())
    {
        throw std::out_of_range{"Failed to create subspan: out of range"};
    }
    return span.subspan(offset, length);
}

template <typename T, std::size_t Extent = std::dynamic_extent>
std::span<T, Extent>::reference at(const std::span<T, Extent> span,
                                   const typename std::span<T, Extent>::size_type index)
{
    if (index >= span.size())
    {
        throw std::out_of_range{"Failed to access span: out of range"};
    }
    return span[index];
}

// template <typename T, std::size_t Extent = std::dynamic_extent>
// inline const std::span<typename std::add_const_t<std::remove_const_t<T>>, Extent>* to_const_span(const std::span<T,
// Extent>* span)
// {
//     // its technically an UB
//     return reinterpret_cast<decltype(to_const_span(span))>(span);
// }
} // namespace plasma::util