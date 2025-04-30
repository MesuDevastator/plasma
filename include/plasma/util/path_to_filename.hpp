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

#include <plasma/util/literal_string.hpp>

namespace plasma::util
{
template <literal_string Path> struct path_parser final
{
  private:
    static constexpr const std::size_t path_len{[] {
        std::size_t len{};
        while (Path.value[len] != '\0')
        {
            ++len;
        }
        return len;
    }()};

    static constexpr std::size_t find_last_slash() noexcept
    {
        for (std::size_t i{path_len}; i > 0; --i)
        {
            std::size_t pos{i - 1};
            if (Path.value[pos] == '/' || Path.value[pos] == '\\')
            {
                return pos;
            }
        }
        return static_cast<std::size_t>(-1);
    }

  public:
    static constexpr const std::size_t last_slash_pos{find_last_slash()};
    static constexpr const std::size_t filename_start{
        (last_slash_pos == static_cast<std::size_t>(-1)) ? 0 : (last_slash_pos + 1)};
    static constexpr const std::size_t filename_len{path_len - filename_start};

    static constexpr inline std::array<char, filename_len + 1> value{[] {
        std::array<char, filename_len + 1> arr{};
        for (std::size_t i{}; i < filename_len; ++i)
        {
            arr[i] = Path.value[filename_start + i];
        }
        arr[filename_len] = '\0';
        return arr;
    }()};
};
} // namespace plasma::util