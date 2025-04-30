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

#include <cstddef>
#include <cstdint>
#include <plasma/util/byteswap.hpp>
#include <plasma/util/span.hpp>
#include <stdexcept>
#include <type_traits>

namespace plasma::networking::type
{
namespace detail
{
constexpr const std::byte segment_bits{0x7f};
constexpr const std::byte continue_bit{0x80};
constexpr const std::size_t varint_max_size{5};
constexpr const std::size_t varlong_max_size{10};

inline std::int32_t read_varint(const std::span<std::byte> data)
{
    std::int32_t value{};
    std::int32_t position{};
    std::size_t i{};
    std::byte current{};
    while (true)
    {
        current = util::at(data, i);
        value |= static_cast<std::int32_t>(current & segment_bits) << position;
        if ((current & continue_bit) == std::byte{})
        {
            break;
        }
        position += 7;
        if (position >= 32)
        {
            throw std::out_of_range{"Failed to read varint: varint too big"};
        }
        ++i;
    }
    return value;
}

inline std::int32_t read_varint(const std::span<std::byte> data, std::size_t &length)
{
    std::int32_t value{};
    std::int32_t position{};
    std::size_t i{};
    std::byte current{};
    while (true)
    {
        current = util::at(data, i);
        value |= static_cast<std::int32_t>(current & segment_bits) << position;
        if ((current & continue_bit) == std::byte{})
        {
            break;
        }
        position += 7;
        if (position >= 32)
        {
            throw std::out_of_range{"Failed to read varint: varint too big"};
        }
        ++i;
    }
    length = i + 1;
    return value;
}

inline std::size_t write_varint(std::int32_t value, const std::span<std::byte> data)
{
    std::size_t i{};
    while (true)
    {
        if ((value & ~static_cast<std::int32_t>(segment_bits)) == 0)
        {
            util::at(data, i) = static_cast<std::byte>(value);
            return i + 1;
        }
        util::at(data, i) = static_cast<std::byte>((value & static_cast<std::int32_t>(segment_bits)) |
                                                   static_cast<std::int32_t>(continue_bit));
        value = static_cast<std::uint32_t>(value) >> 7;
        ++i;
    }
}

inline constexpr std::size_t get_varint_length(std::int32_t value) noexcept
{
    std::size_t i{1};
    for (; (value & ~static_cast<std::int32_t>(segment_bits)) != 0; value = static_cast<std::uint32_t>(value) >> 7, ++i)
    {
    }
    return i;
}

inline std::size_t get_varint_length(const std::span<std::byte> data)
{
    std::int32_t position{};
    std::size_t i{};
    std::byte current{};
    while (true)
    {
        current = util::at(data, i);
        if ((current & continue_bit) == std::byte{})
        {
            return i + 1;
        }
        position += 7;
        if (position >= 32)
        {
            throw std::out_of_range{"Failed to read varint: varint too big"};
        }
        ++i;
    }
}

inline std::int64_t read_varlong(const std::span<std::byte> data)
{
    std::int64_t value{};
    std::int32_t position{};
    std::size_t i{};
    std::byte current{};
    while (true)
    {
        current = util::at(data, i);
        value |= static_cast<std::int64_t>(current & segment_bits) << position;
        if ((current & continue_bit) == std::byte{})
        {
            break;
        }
        position += 7;
        if (position >= 64)
        {
            throw std::out_of_range{"Failed to read varlong: varlong too big"};
        }
        ++i;
    }
    return value;
}

inline std::int64_t read_varlong(const std::span<std::byte> data, std::size_t &length)
{
    std::int64_t value{};
    std::int32_t position{};
    std::size_t i{};
    std::byte current{};
    while (true)
    {
        current = util::at(data, i);
        value |= static_cast<std::int64_t>(current & segment_bits) << position;
        if ((current & continue_bit) == std::byte{})
        {
            break;
        }
        position += 7;
        if (position >= 64)
        {
            throw std::out_of_range{"Failed to read varlong: varlong too big"};
        }
        ++i;
    }
    length = i + 1;
    return value;
}

inline std::size_t write_varlong(std::int64_t value, const std::span<std::byte> data)
{
    std::size_t i{};
    while (true)
    {
        if ((value & ~static_cast<std::int64_t>(segment_bits)) == 0)
        {
            util::at(data, i) = static_cast<std::byte>(value);
            return i + 1;
        }
        util::at(data, i) = static_cast<std::byte>((value & static_cast<std::int64_t>(segment_bits)) |
                                                   static_cast<std::int64_t>(continue_bit));
        value = static_cast<std::uint64_t>(value) >> 7;
        ++i;
    }
}

inline constexpr std::size_t get_varlong_length(std::int64_t value) noexcept
{
    std::size_t i{1};
    for (; (value & ~static_cast<std::int64_t>(segment_bits)) != 0; value = static_cast<std::uint64_t>(value) >> 7, ++i)
    {
    }
    return i;
}

inline std::size_t get_varlong_length(const std::span<std::byte> data)
{
    std::int64_t position{};
    std::size_t i{};
    std::byte current{};
    while (true)
    {
        current = util::at(data, i);
        if ((current & continue_bit) == std::byte{})
        {
            return i + 1;
        }
        position += 7;
        if (position >= 64)
        {
            throw std::out_of_range{"Failed to read varlong: varlong too big"};
        }
        ++i;
    }
}
} // namespace detail

class varint_ref
{
  private:
    std::span<std::byte> max_data_;
    std::span<std::byte> data_;

  public:
    varint_ref() = default;

    explicit varint_ref(const std::span<std::byte> &data)
        : max_data_{data}, data_{data.subspan(0, detail::get_varint_length(data))} // checked by get_varint_length
    {
    }

    varint_ref(const varint_ref &other, std::byte *const offset) noexcept
        : max_data_{offset, other.max_data_.size()}, data_{offset, other.data_.size()}
    {
    }

    operator std::int32_t() const
    {
        return detail::read_varint(data_);
    }

    varint_ref &operator=(const std::int32_t value)
    {
        data_ = max_data_.subspan(0, detail::write_varint(value, max_data_));
        return *this;
    }

    varint_ref &offset(std::byte *const offset) noexcept
    {
        max_data_ = std::span{offset, max_data_.size()};
        data_ = std::span{offset, data_.size()};
        return *this;
    }

    std::span<const std::byte> used_data() const noexcept // returning a reference here would cause an implicit cast,
                                                          // which leads to returning a dangling reference
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

class varlong_ref
{
  private:
    std::span<std::byte> max_data_;
    std::span<std::byte> data_;

  public:
    varlong_ref() = default;

    explicit varlong_ref(const std::span<std::byte> &data)
        : max_data_{data}, data_{data.subspan(0, detail::get_varlong_length(data))}
    {
    }

    varlong_ref(const varlong_ref &other, std::byte *const offset) noexcept
        : max_data_{offset, other.max_data_.size()}, data_{offset, other.data_.size()}
    {
    }

    operator std::int64_t() const
    {
        return detail::read_varlong(data_);
    }

    varlong_ref &operator=(const std::int64_t value)
    {
        data_ = max_data_.subspan(0, detail::write_varlong(value, max_data_));
        return *this;
    }

    varlong_ref &offset(std::byte *const offset) noexcept
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

template <typename T>
concept numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <numeric T, std::endian Endianness = std::endian::big> class endian_number_ref
{
  private:
    std::span<std::byte> max_data_;
    std::span<std::byte> data_;

  public:
    endian_number_ref() = default;

    explicit endian_number_ref(const std::span<std::byte> &data)
        : max_data_{data}, data_{util::checked_subspan(data, 0, sizeof(T))}
    {
    }

    endian_number_ref(const endian_number_ref &other, std::byte *const offset) noexcept
        : max_data_{offset, other.max_data_.size()}, data_{offset, other.data_.size()}
    {
    }

    operator T() const
    {
        auto value{*reinterpret_cast<T *>(data_.data())};
        if constexpr (std::endian::native != Endianness)
        {
            value = plasma::util::byteswap<T>(value);
        }
        return value;
    }

    endian_number_ref &operator=(T value)
    {
        if constexpr (std::endian::native != Endianness)
        {
            value = plasma::util::byteswap<T>(value);
        }
        *reinterpret_cast<T *>(data_.data()) = value;
        return *this;
    }

    endian_number_ref &offset(std::byte *const offset) noexcept
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

using short_ref = endian_number_ref<std::int16_t>;
using ushort_ref = endian_number_ref<std::uint16_t>;
using int_ref = endian_number_ref<std::int32_t>;
using long_ref = endian_number_ref<std::int64_t>;
using float_ref = endian_number_ref<float>;
using double_ref = endian_number_ref<double>;
} // namespace plasma::networking::type
