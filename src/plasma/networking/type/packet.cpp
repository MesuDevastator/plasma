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

#include <fmt/format.h>
#include <memory>

#include <plasma/networking/plasma_connection.hpp>
#include <plasma/networking/type/connection_status.hpp>
#include <plasma/networking/type/handshake_packet.hpp>
#include <plasma/networking/type/packet.hpp>
#include <plasma/networking/type/packet_exception.hpp>
#include <plasma/networking/type/string.hpp>
#include <plasma/util/span.hpp>

namespace plasma::networking::type
{
packet::packet(std::unique_ptr<std::byte[]> data, std::span<std::byte> data_span, varint_ref length,
               varint_ref packet_id, std::span<std::byte> body_span) noexcept
    : data_{std::move(data)}, data_span_{std::move(data_span)}, length_{std::move(length)},
      packet_id_{std::move(packet_id)}, body_span_{std::move(body_span)}
{
}

packet::packet(const packet &other)
    : data_{std::make_unique<std::byte[]>(other.data_span_.size())}, data_span_{data_.get(), other.data_span_.size()},
      // Assuming spans are checked
      length_{std::span{data_span_.begin(), other.length_.max_size()}},
      packet_id_{std::span{length_.max_data().end(), other.packet_id_.max_size()}},
      body_span_{packet_id_.max_data().end(), other.body_span_.size()}
{
    std::ranges::copy(other.data_span_, data_.get());
}

std::unique_ptr<packet> packet::parse(const std::span<std::byte> body, const std::int32_t packet_id,
                                      const connection_status status)
{
    logger lg{};
    switch (status)
    {
    case connection_status::handshake: {
        if (packet_id == handshake_packet::packet_id)
        {
            return handshake_packet::parse(body);
        }
        else
        {
            PLASMA_LOG(lg, warning) << "Ignoring non-standard handshake packet";
        }
        break;
    }
    case type::connection_status::status: {
        if (packet_id == 1)
        {
            // TODO: Ping packet
        }
        else if (packet_id == 0)
        {
            // TODO: Status query packet
        }
        break;
    }
    case connection_status::login: {
        break;
    }
    case connection_status::configuration: {
        break;
    }
    case connection_status::play: {
        break;
    }
    }
    throw packet_exception{"Unknown packet"};
}
} // namespace plasma::networking::type
