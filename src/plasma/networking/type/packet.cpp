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

#include "boost/system/detail/error_code.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <memory>
#include <plasma/networking/type/packet.hpp>
#include <plasma/networking/type/string.hpp>
#include <plasma/networking/plasma_connection.hpp>
#include <plasma/networking/type/packet_exception.hpp>

namespace plasma::networking::type
{
    packet::packet(const std::size_t body_length, const std::int32_t packet_id, const std::byte* const body) :
        head_length{ get_varint_length(static_cast<std::int32_t>(body_length)) + get_varint_length(packet_id) }, body_length{ body_length }, total_length{ head_length + body_length }, packet_id{ packet_id }, data{ std::make_unique<std::byte[]>(total_length) }
    {
        const auto body_length_length{ write_varint(static_cast<std::int32_t>(body_length + get_varint_length(packet_id)), data.get(), total_length) };
        write_varint(packet_id, data.get() + body_length_length, total_length - body_length_length);
        std::copy(body, body + body_length, data.get() + head_length);
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

    void packet::process(plasma::networking::plasma_connection& connection)
    {
        switch (connection.status_)
        {
            case connection_status::handshake:
            {
                if (packet_id == 0)
                {
                    const auto handshake_packet{ handshake_packet::parse(data.get() + head_length, body_length) };
                    DBG(connection.lg_) << fmt::format("Received handshake request, client procotol {}, server address {}, server port {}", handshake_packet.protocol_version, reinterpret_cast<const char*>(handshake_packet.server_address.data()), handshake_packet.server_port);
                    switch (handshake_packet.next_state)
                    {
                        case 1:
                        {
                            DBG(connection.lg_) << "Entering status query";
                            connection.status_ = connection_status::status;
                            break;
                        }
                        case 2:
                        {
                            DBG(connection.lg_) << "Entering login";
                            connection.status_ = connection_status::login;
                            break;
                        }
                        default:
                        {
                            throw packet_exception{ "Failed to process packet: unknown next state in handshake" };
                        }
                    }
                }
                else
                {
                    WRN(connection.lg_) << "Ignoring non-standard handshake packet";
                }
                break;
            }
            case connection_status::status:
            {
                if (packet_id == 1)
                {
                    TRC(connection.lg_) << "Received ping request: " << read_long(data.get() + head_length, body_length);
                    const packet pong{ sizeof(std::int64_t), 1, data.get() + head_length };
                    async_write(connection.socket_, boost::asio::buffer(pong.data.get(), pong.total_length), [&connection](const boost::system::error_code&, const std::size_t){ connection.kill(); });
                }
                else if (packet_id == 0)
                {
                    TRC(connection.lg_) << "Received status query request";
                    std::string motd{
                        R"({
                            "version": {
                                "name": "1.19.4",
                                "protocol": 766
                            },
                            "players": {
                                "max": 100,
                                "online": 5,
                                "sample": [
                                    {
                                        "name": "thinkofdeath",
                                        "id": "4566e69f-c907-48ee-8d71-d7ba5aa00d20"
                                    }
                                ]
                            },
                            "description": {
                                "text": "Hello, world!"
                            },
                            "favicon": "data:image/png;base64",
                            "enforcesSecureChat": false,
                            "previewsChat": false
                        })"
                    };
                    const auto length{ motd.length() + get_varint_length(motd.length()) };
                    auto body{ std::make_unique<std::byte[]>(length) };
                    write_string(std::u8string{ reinterpret_cast<const char8_t*>(motd.data()), motd.length() }, body.get(), length);
                    connection.send(packet{ length, 0, body.get() });
                }
                break;
            }
            case connection_status::login:
            {
                break;
            }
            case connection_status::configuration:
            {
                break;
            }
            case connection_status::play:
            {
                break;
            }
        }
    }

    packet::handshake_packet packet::handshake_packet::parse(const std::byte* const body, const std::size_t max_length)
    {
        std::size_t protocol_version_length{};
        const auto protocol_version{ read_varint(body, protocol_version_length, max_length) };

        std::size_t server_address_length{};
        const auto server_address{ read_string(body + protocol_version_length, server_address_length, max_length - protocol_version_length) };

        constexpr const std::size_t server_port_length{ sizeof(std::uint16_t) };
        const auto server_port{ read_ushort(body + protocol_version_length + server_address_length, max_length - protocol_version_length - server_address_length) };

        std::size_t next_state_length{};
        const auto next_state{ read_varint(body + protocol_version_length + server_address_length + server_port_length, next_state_length, max_length - protocol_version_length - server_address_length - server_port_length) };

        return handshake_packet{ protocol_version, server_address, server_port, next_state };
    }
}
