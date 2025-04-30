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

#include <cstddef>
#include <cstdint>
#include <plasma/networking/plasma_connection.hpp>
#include <plasma/networking/type/handshake_packet.hpp>
#include <plasma/networking/type/packet_exception.hpp>
#include <plasma/plasma_server.hpp>

#include <fmt/format.h>

namespace plasma::networking
{
#if 0
void plasma_connection::handle_packet(const type::packet &packet)
{
    switch (status_)
    {
    case type::connection_status::handshake: {
        if (packet.packet_id() == type::handshake_packet::packet_id)
        {
            const auto handshake_packet{type::handshake_packet::parse(data.get() + head_length, body_length)};
            PLASMA_LOG(lg_, info) << fmt::format("Received handshake request, client protocol {}, server address {}, server port {}",
                                    handshake_packet.protocol_version,
                                    reinterpret_cast<const char *>(handshake_packet.server_address.data()),
                                    handshake_packet.server_port);
            switch (handshake_packet.next_state)
            {
            case 1: {
                PLASMA_LOG(lg_, info) << "Entering status query";
                status_ = type::connection_status::status;
                break;
            }
            case 2: {
                PLASMA_LOG(lg_, info) << "Entering login";
                status_ = type::connection_status::login;
                break;
            }
            default: {
                // Todo: unknown packet
            }
            }
        }
        else
        {
            PLASMA_LOG(lg_, warning) << "Ignoring non-standard handshake packet";
        }
        break;
    }
    case type::connection_status::status: {
        if (packet_id == 1)
        {
            PLASMA_LOG(connection.lg_, trace) << "Received ping request: " << read_long(data.get() + head_length, body_length);
            const packet pong{sizeof(std::int64_t), 1, data.get() + head_length};
            async_write(connection.socket_, boost::asio::buffer(pong.data.get(), pong.total_length),
                        [&connection](const boost::system::error_code &, const std::size_t) { connection.kill(); });
        }
        else if (packet_id == 0)
        {
            PLASMA_LOG(connection.lg_, trace) << "Received status query request";
            std::u8string motd{u8R"({
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
                        })"};
            const auto length{get_string_length(motd)};
            const auto body{std::make_unique<std::byte[]>(length)};
            write_string(motd, body.get(), length);
            connection.send(packet{length, 0, body.get()});
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
}
#endif

void plasma_connection::handle_timeout(const boost::system::error_code &error, [[maybe_unused]] pointer self)
{
    if (!error)
    {
        PLASMA_LOG(lg_, error) << fmt::format("Connection {} timed out after {}ms", to_string(uuid_),
                                              server_.config_.networking.timeout_milliseconds);
        kill();
    }
}

void plasma_connection::handle_read(const boost::system::error_code &error, const std::size_t bytes_transferred,
                                    [[maybe_unused]] pointer self)
{
    if (error)
    {
        PLASMA_LOG(lg_, error) << fmt::format("Failed to read connection {}, error \"{}\"", to_string(uuid_),
                                              error.message());
        kill();
    }
    try
    {
        // Only length field has been read at this moment
        type::varint_ref packet_id{std::span{body_buffer_.get(), bytes_transferred}};
        PLASMA_LOG(lg_, debug) << fmt::format("Connection {} >> Packet {}: ID {}", to_string(uuid_), packet_seq_,
                                              static_cast<std::int32_t>(packet_id));
        const auto packet{type::packet::parse(std::span{body_buffer_.get() + packet_id.used_data().size(),
                                                        bytes_transferred - packet_id.used_data().size()},
                                              packet_id, status_)};
        body_buffer_.reset();
        packet_seq_++;
        packet->process(*this);
    }
    catch (const type::packet_exception &e)
    {
        PLASMA_LOG(lg_, error) << "Invalid packet from connection " << to_string(uuid_);
        PLASMA_LOG(lg_, error) << "Exception: " << e.what();
        kill();
        return;
    }
    start_read();
}

void plasma_connection::handle_read_head(const boost::system::error_code &error,
                                         [[maybe_unused]] const std::size_t bytes_transferred, const std::size_t cursor,
                                         pointer self)
{
    if (error)
    {
        PLASMA_LOG(lg_, error) << fmt::format("Failed to read connection {}, error \"{}\"", to_string(uuid_),
                                              error.message());
        kill();
    }
    if (status_ == type::connection_status::handshake && cursor == 0 &&
        head_buffer_[cursor] == std::byte{0xfe}) // legacy SLP
    {
        std::byte legacy_buffer[2]{};
        socket_.async_read_some(boost::asio::buffer(legacy_buffer, 2),
                                [this, legacy_buffer, self](const auto &error, const auto bytes_transferred) {
                                    if (error)
                                    {
                                        PLASMA_LOG(lg_, error)
                                            << "Failed to read legacy packet head from connection " << to_string(uuid_);
                                        kill();
                                    }
                                    // TODO: implement legacy SLP response
                                    if (bytes_transferred == 0) // Beta 1.8 - 1.3
                                    {
                                        kill();
                                    }
                                    if (bytes_transferred == 1 && legacy_buffer[0] == std::byte{0x01}) // 1.4 - 1.5
                                    {
                                        kill();
                                    }
                                    if (bytes_transferred == 2 && legacy_buffer[0] == std::byte{0x01} &&
                                        legacy_buffer[1] == std::byte{0xfa}) // 1.6
                                    {
                                        kill();
                                    }
                                    PLASMA_LOG(lg_, error)
                                        << "Invalid legacy handshake packet from connection " << to_string(uuid_);
                                    kill();
                                });
        return;
    }
    if (cursor + 1 > type::detail::varint_max_size)
    {
        PLASMA_LOG(lg_, error) << "Failed to read packet head from connection " << to_string(uuid_);
        kill();
        return;
    }
    if ((head_buffer_[cursor] & type::detail::continue_bit) == std::byte{})
    {
        std::int32_t length{};
        try
        {
            length = type::detail::read_varint(std::span{head_buffer_.get(), type::detail::varint_max_size});
        }
        catch (const type::packet_exception &e)
        {
            PLASMA_LOG(lg_, error) << "Invalid packet from connection " << to_string(uuid_);
            PLASMA_LOG(lg_, error) << "Exception: " << e.what();
            kill();
            return;
        }
        PLASMA_LOG(lg_, debug) << fmt::format("Connection {} >> Packet {}: total length {}B", to_string(uuid_),
                                              packet_seq_, cursor + 1 + length);
        body_buffer_ = std::make_unique<std::byte[]>(length);
        async_read(socket_, boost::asio::buffer(body_buffer_.get(), length),
                   [this, self](const auto &error, const auto bytes_transferred) {
                       handle_read(error, bytes_transferred, self);
                   });
    }
    else
    {
        async_read(socket_, boost::asio::buffer(head_buffer_.get() + cursor + 1, 1),
                   [this, cursor, self](const auto &error, const auto bytes_transferred) {
                       handle_read_head(error, bytes_transferred, cursor + 1, self);
                   });
    }
}

void plasma_connection::handle_write(const boost::system::error_code &error,
                                     [[maybe_unused]] const std::size_t bytes_transferred, const pointer self)
{
    if (error)
    {
        PLASMA_LOG(lg_, error) << fmt::format("Failed to write connection {}, error \"{}\"", to_string(uuid_),
                                              error.what());
        kill();
    }
    std::lock_guard<std::mutex> lock{send_lock_};
    send_queue_.pop();
    if (!send_queue_.empty())
    {
        auto &packet{send_queue_.front()};
        async_write(socket_, boost::asio::buffer(packet.data().data(), packet.data().size_bytes()),
                    [this, self](const auto &error, const auto bytes_transferred) {
                        handle_write(error, bytes_transferred, self);
                    });
    }
}

plasma_connection::plasma_connection(plasma_server &server, const boost::uuids::uuid &uuid)
    : killed_{}, server_{server}, context_{}, socket_{context_}, uuid_{uuid},
      head_buffer_{std::make_unique<std::byte[]>(type::detail::varint_max_size)}, body_buffer_{}, packet_seq_{1},
      status_{type::connection_status::handshake}
{
}

plasma_connection::pointer plasma_connection::create(plasma_server &server, const boost::uuids::uuid &uuid) noexcept
{
    return plasma_connection::pointer{new plasma_connection{server, uuid}};
}

void plasma_connection::start_read()
{
    const auto self{shared_from_this()};
    async_read(socket_, boost::asio::buffer(head_buffer_.get(), 1),
               [this, self](const auto &error, const auto bytes_transferred) {
                   handle_read_head(error, bytes_transferred, 0, self);
               });
}

void plasma_connection::start()
{
    PLASMA_LOG(lg_, info) << fmt::format("Incoming TCP connection from {}:{}, connection uuid {}",
                                         socket_.remote_endpoint().address().to_string(),
                                         socket_.remote_endpoint().port(), to_string(uuid_));
    start_read();
    context_.run();
}

void plasma_connection::kill()
{
    if (killed_)
    {
        return;
    }
    killed_ = true;
    PLASMA_LOG(lg_, info) << "Killing connection " << to_string(uuid_);
    server_.remove_connection(uuid_);
    try
    {
        socket_.close();
    }
    catch (...)
    { /* ignored */
    }
    try
    {
        context_.stop();
    }
    catch (...)
    { /* ignored */
    }
}

void plasma_connection::send(const std::unique_ptr<type::packet> &packet)
{
    std::lock_guard<std::mutex> lock{send_lock_};
    send_queue_.push(*packet); // it's fine to copy parent packet class only here
    auto &new_packet{send_queue_.front()};
    PLASMA_LOG(lg_, debug) << fmt::format("Connection {} << Packet {}: total length {}B, ID {}", to_string(uuid_),
                                          packet_seq_, new_packet.data().size_bytes(),
                                          static_cast<std::int32_t>(new_packet.packet_id()));
    packet_seq_++;
    const auto self{shared_from_this()};
    async_write(socket_, boost::asio::buffer(new_packet.data().data(), new_packet.data().size_bytes()),
                [this, self](const auto &error, const auto bytes_transferred) {
                    handle_write(error, bytes_transferred, self);
                });
}

plasma_connection::~plasma_connection()
{
    PLASMA_LOG(lg_, trace) << "Destructing connection " << to_string(uuid_);
    kill();
}
} // namespace plasma::networking
