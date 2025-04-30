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

#include <plasma/networking/type/varint.hpp>
#include <plasma/util/span.hpp>
#include <stdexcept>
#include <string>

namespace plasma::networking::type
{
namespace detail
{
constexpr const std::size_t string_max_size{(32767 * 3) + 3};
inline std::string read_string(const std::span<std::byte> data)
{
    std::size_t length_length{};
    const auto length{read_varint(data.subspan(0, 3), length_length)};
    if (static_cast<std::size_t>(length) > data.size())
    {
        throw std::out_of_range{"Failed to read string: out of range"};
    }
    return std::string{reinterpret_cast<const char *>(data.data()) + length_length,
                       reinterpret_cast<const char *>(data.data()) + length_length + length};
}

inline std::string read_string(const std::span<std::byte> data, std::size_t &length)
{
    std::size_t length_length{};
    const auto length_{read_varint(data.subspan(0, 3), length_length)};
    if (static_cast<std::size_t>(length_) > data.size())
    {
        throw std::out_of_range{"Failed to read string: out of range"};
    }
    length = length_ + length_length; // not setting the length until it is checked
    return std::string{reinterpret_cast<const char *>(data.data()) + length_length,
                       reinterpret_cast<const char *>(data.data()) + length};
}

inline std::size_t write_string(const std::string &string, const std::span<std::byte> data)
{
    const auto length_length{write_varint(string.length(), data)};
    if (string.length() + length_length > data.size())
    {
        throw std::out_of_range{"Failed to write string: out of range"};
    }
    return string.copy(reinterpret_cast<char *>(data.data()) + length_length, string.length()) + length_length;
}

inline std::size_t get_string_length(const std::u8string &string)
{
    return get_varint_length(string.length()) + string.length();
}

inline std::size_t get_string_length(const std::span<std::byte> data)
{
    std::size_t length_length;
    const auto length{read_varint(data, length_length)};
    if (data.size() < length + length_length)
    {
        throw std::out_of_range{"Failed to read string: out of range"};
    }
    return length + length_length;
}
} // namespace detail

class string_ref
{
  private:
    std::span<std::byte> max_data_;
    std::span<std::byte> data_;

  public:
    string_ref() = default;

    explicit string_ref(const std::span<std::byte> data)
        : max_data_{data}, data_{data.subspan(0, detail::get_string_length(data))} // checked by get_string_length
    {
    }

    operator std::string() const // causes a copy
    {
        return detail::read_string(data_);
    }

    string_ref &operator=(const std::string &value)
    {
        data_ = max_data_.subspan(0, detail::write_string(value, max_data_));
        return *this;
    }

    string_ref &offset(std::byte *const offset) noexcept
    {
        max_data_ = std::span{offset, max_data_.size()};
        data_ = std::span{offset, data_.size()};
        return *this;
    }

    std::span<const std::byte> used_data() const noexcept
    {
        return data_;
    }

    std::span<std::byte> &used_data() noexcept
    {
        return data_;
    }

    std::span<const std::byte> max_data() const noexcept
    {
        return max_data_;
    }

    std::span<std::byte> &max_data() noexcept
    {
        return max_data_;
    }

    std::size_t size() const noexcept
    {
        return data_.size();
    }

    std::size_t max_size() const noexcept
    {
        return max_data_.size();
    }
};
} // namespace plasma::networking::type
