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

#include <fmt/format.h>

#include <plasma/log.hpp>
#include <plasma/networking/plasma_connection.hpp>
#include <plasma/networking/type/handshake_packet.hpp>
#include <plasma/networking/type/packet_exception.hpp>

namespace plasma::networking::type
{
handshake_packet::handshake_packet(std::unique_ptr<std::byte[]> data, std::span<std::byte> data_span, varint_ref length,
                                   varint_ref packet_id, std::span<std::byte> body_span, varint_ref protocol_version,
                                   string_ref server_address, ushort_ref server_port, varint_ref next_state) noexcept
    : packet{std::move(data), std::move(data_span), std::move(length), std::move(packet_id), std::move(body_span)},
      protocol_version_{std::move(protocol_version)}, server_address_{std::move(server_address)},
      server_port_{std::move(server_port)}, next_state_{std::move(next_state)}
{
}

std::unique_ptr<packet> handshake_packet::parse(std::span<std::byte> body)
{
    // Parsing
    varint_ref protocol_version{body};
    string_ref server_address{std::span{protocol_version.used_data().end(), body.end()}};
    ushort_ref server_port{std::span{server_address.used_data().end(), body.end()}};
    varint_ref next_state{std::span{server_port.used_data().end(), body.end()}};
    const auto body_size{protocol_version.size() + server_address.size() + server_port.size() + next_state.size()};
    const auto data_size{detail::get_varint_length(body_size + detail::get_varint_length(packet_id)) +
                         detail::get_varint_length(packet_id) + body_size}; // with size and packet id
    logger lg{};
    PLASMA_LOG(lg, info) << fmt::format(
        "Received handshake request, client protocol {}, server address {}, server port {}",
        static_cast<std::int32_t>(protocol_version), static_cast<std::string>(server_address),
        static_cast<std::uint16_t>(server_port));
    // Copying
    auto data{std::make_unique<std::byte[]>(data_size)};
    std::span data_span{data.get(), data_size};
    varint_ref length{std::span{data_span}};
    length = detail::get_varint_length(packet_id) + body_size;
    varint_ref packet_id_{std::span{length.used_data().end(), data_span.end()}};
    packet_id_ = packet_id;
    std::copy(body.begin(), body.end(), packet_id_.used_data().end());
    std::span new_body_span{packet_id_.used_data().end(), data_span.end()};
    protocol_version.offset(new_body_span.data());
    server_address.offset(new_body_span.data() + (server_address.used_data().begin() - body.begin()));
    server_port.offset(new_body_span.data() + (server_port.used_data().begin() - body.begin()));
    next_state.offset(new_body_span.data() + (next_state.used_data().begin() - body.begin()));
    return std::unique_ptr<packet>{new handshake_packet{std::move(data), data_span, length, packet_id_, new_body_span,
                                                        protocol_version, server_address, server_port, next_state}};
}

void handshake_packet::process(plasma_connection &connection)
{
    logger lg{};
    switch (static_cast<std::int32_t>(next_state_))
    {
    case 1: {
        PLASMA_LOG(lg, info) << "Entering status query";
        connection.status() = type::connection_status::status;
        break;
    }
    case 2: {
        PLASMA_LOG(lg, info) << "Entering login";
        connection.status() = type::connection_status::login;
        break;
    }
    default: {
        throw packet_exception{"Unknown status"};
    }
    }
}
} // namespace plasma::networking::type
