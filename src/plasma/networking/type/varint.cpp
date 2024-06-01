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

#include <cstdint>
#include <plasma/networking/type/varint.h>

namespace plasma::networking::type
{
    int32_t read_varint(std::byte* data, std::size_t max_length)
    {
        int32_t value{};
        int32_t position{};
        std::size_t i{};
        std::byte current{};
        while (true)
        {
            if (i >= max_length)
            {
                throw varint_exception{ "Failed to read varint: unexpected eof" };
            }
            current = data[i];
            value |= static_cast<int32_t>(current & segment_bits) << position;
            if ((current & continue_bit) == std::byte{})
            {
                break;
            }
            position += 7;
            if (position >= 32)
            {
                throw varint_exception{ "Failed to read varint: varint too big" };
            }
            i++;
        }
        return value;
    }

    void write_varint(int32_t value, std::byte* data, std::size_t max_length)
    {
        std::size_t i{};
        while (true)
        {
            if (i >= max_length)
            {
                throw varint_exception{ "Failed to write varint: unexpected eof" };
            }
            if ((value & ~static_cast<int32_t>(segment_bits)) == 0)
            {
                data[i] = static_cast<std::byte>(value);
                return;
            }
            data[i] = static_cast<std::byte>((value & static_cast<int32_t>(segment_bits)) | static_cast<int32_t>(continue_bit));
            value = static_cast<uint32_t>(value) >> 7;
        }
    }

    int64_t read_varlong(std::byte* data, std::size_t max_length)
    {
        int64_t value{};
        int32_t position{};
        std::size_t i{};
        std::byte current{};
        while (true)
        {
            if (i >= max_length)
            {
                throw varint_exception{ "Failed to read varint: unexpected eof" };
            }
            current = data[i];
            value |= static_cast<int64_t>(current & segment_bits) << position;
            if ((current & continue_bit) == std::byte{})
            {
                break;
            }
            position += 7;
            if (position >= 64)
            {
                throw varint_exception{ "Failed to read varint: varint too big" };
            }
            i++;
        }
        return value;
    }

    void write_varlong(int64_t value, std::byte* data, std::size_t max_length)
    {
        std::size_t i{};
        while (true)
        {
            if (i >= max_length)
            {
                throw varint_exception{ "Failed to write varint: unexpected eof" };
            }
            if ((value & ~static_cast<int64_t>(segment_bits)) == 0)
            {
                data[i] = static_cast<std::byte>(value);
                return;
            }
            data[i] = static_cast<std::byte>((value & static_cast<int64_t>(segment_bits)) | static_cast<int64_t>(continue_bit));
            value = static_cast<uint64_t>(value) >> 7;
        }
    }
}
