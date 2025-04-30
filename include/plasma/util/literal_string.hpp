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

#include <algorithm>
#include <cstddef>
#include <string_view>

namespace plasma::util
{
// https://zhuanlan.zhihu.com/p/32076857269
template <std::size_t N> struct literal_string final
{
  public:
    static constexpr const std::size_t size{N - 1};
    char value[N]{};

    constexpr literal_string(const char (&str)[N]) noexcept
    {
        std::ranges::copy(str, value);
    }

    constexpr operator std::string_view() const noexcept
    {
        return std::string_view{value, N};
    }

    constexpr bool operator==(std::string_view str) const noexcept
    {
        return std::ranges::equal(str, value);
    }
};

template <std::size_t N1, std::size_t N2>
constexpr bool operator==(const literal_string<N1> &str1, const literal_string<N2> &str2) noexcept
{
    return std::ranges::equal(str1.value, str2.value);
}

namespace literals
{
template <literal_string String> constexpr auto operator""_ls()
{
    return String;
}
} // namespace literals
} // namespace plasma::util
