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

#include <plasma/networking/type/string.hpp>
#include <plasma/networking/type/string_exception.hpp>

namespace plasma::networking::type
{
    std::u8string read_string(const std::byte* const data, const std::size_t max_length)
    {
        std::size_t length_length{};
        const auto length{ read_varint(data, length_length, 3) };
        if (static_cast<std::size_t>(length) > max_length)
        {
            throw string_exception{ "Failed to read string: unexpected eof" };
        }
        return std::u8string{ reinterpret_cast<const char*>(data + length_length), reinterpret_cast<const char*>(data + length_length + length) };
    }

    std::u8string read_string(const std::byte* const data, std::size_t& length_, const std::size_t max_length)
    {
        std::size_t length_length{};
        const auto length{ read_varint(data, length_length, 3) };
        if (static_cast<std::size_t>(length) > max_length)
        {
            throw string_exception{ "Failed to read string: unexpected eof" };
        }
        length_ = length + length_length;   // not setting the length until it is checked
        return std::u8string{ reinterpret_cast<const char*>(data + length_length), reinterpret_cast<const char*>(data + length_length + length) };
    }

    std::size_t write_string(const std::u8string& string, std::byte* const data, const std::size_t max_length)
    {
        const auto length_length{ write_varint(string.length(), data, max_length) };     // throws varint_exception if out of bounds
        if (string.length() - length_length > max_length)
        {
            throw string_exception{ "Failed to write string: unexpected eof" };
        }
        return string.copy(reinterpret_cast<char8_t*>(data + length_length), string.length()) + length_length;
    }
}
