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

#include <plasma/networking/type/packet.hpp>

namespace plasma::networking::type
{
    packet::packet(const std::size_t body_length, const std::int32_t packet_id, const std::byte* const body) :
        head_length{ get_varint_length(static_cast<std::int32_t>(body_length)) + get_varint_length(packet_id) }, body_length{ body_length }, total_length{ head_length + body_length }, packet_id{ packet_id }, data{ std::make_unique<std::byte[]>(total_length) }
    {
        const auto body_length_length{ write_varint(static_cast<std::int32_t>(body_length + get_varint_length(packet_id)), data.get(), total_length) };
        write_varint(packet_id, data.get() + body_length_length, total_length - body_length_length);
        std::copy(body, body + body_length, data.get() + total_length);
    }

    packet::packet(const std::size_t head_length, const std::size_t body_length, const std::int32_t packet_id, const std::byte* const data) :
        head_length{ head_length }, body_length{ body_length }, total_length{ head_length + body_length }, packet_id{ packet_id }, data{ std::make_unique<std::byte[]>(total_length) }
    {
        std::copy(data, data + total_length, this->data.get());
    }

    packet::packet(const packet& other) :
        head_length{ other.head_length }, body_length{ other.body_length }, total_length{ other.total_length }, packet_id{ other.packet_id }, data{ std::make_unique<std::byte[]>(other.total_length) }
    {
        std::copy(other.data.get(), other.data.get() + other.total_length, data.get());
    }
}
