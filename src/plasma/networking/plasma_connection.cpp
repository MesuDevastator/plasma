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


#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <plasma/networking/plasma_connection.hpp>
#include <plasma/plasma_server.hpp>

#include <fmt/format.h>

namespace plasma::networking
{
    void plasma_connection::handle_read(const boost::system::error_code& error, const std::size_t bytes_transferred, const std::size_t cursor, [[maybe_unused]] pointer self)
    {
        if (!error)
        {
            std::size_t packet_id_length{};
            std::int32_t packet_id{};
            try
            {
                packet_id = type::read_varint(raw_buffer_.get() + cursor, packet_id_length, bytes_transferred);
            }
            catch (const type::varint_exception&)
            {
                ERR(lg_) << "Invalid packet from connection " << to_string(uuid_);
                kill();
                return;
            }
            DBG(lg_) << fmt::format("[{}] Packet ID {}", packet_seq_, packet_id);
            type::packet packet{ cursor + packet_id_length, bytes_transferred - packet_id_length, packet_id, raw_buffer_.get() + cursor + packet_id_length };
            packet_seq_++;
            std::fill(raw_buffer_.get(), raw_buffer_.get() + cursor + bytes_transferred, std::byte{});
            start_read();    
        }
        else
        {
            ERR(lg_) << fmt::format("Failed to read connection {}, error \"{}\"", to_string(uuid_), error.what());
            kill();
        }
    }

    void plasma_connection::handle_read_head(const boost::system::error_code& error, [[maybe_unused]] const std::size_t bytes_transferred, const std::size_t cursor, pointer self)
    {
        if (!error)
        {
            if (cursor + 1 > type::varint_max_size)
            {
                ERR(lg_) << "Invalid packet from connection " << to_string(uuid_);
                kill();
                return;
            }
            if ((raw_buffer_[cursor] & type::continue_bit) == std::byte{})
            {
                std::int32_t length{};
                try
                {
                    length = type::read_varint(raw_buffer_.get());   
                }
                catch (const type::varint_exception&)
                {
                    ERR(lg_) << "Invalid packet from connection " << to_string(uuid_);
                    kill();
                    return;
                }
                DBG(lg_) << fmt::format("[{}] Packet length {}", packet_seq_, length);
                async_read(socket_, boost::asio::buffer(raw_buffer_.get() + cursor + 1, length), std::bind(&plasma_connection::handle_read, this, std::placeholders::_1, std::placeholders::_2, cursor + 1, self));
            }
            else
            {
                async_read(socket_, boost::asio::buffer(raw_buffer_.get() + cursor + 1, 1), std::bind(&plasma_connection::handle_read_head, this, std::placeholders::_1, std::placeholders::_2, cursor + 1, self));
            }
        }
        else
        {
            ERR(lg_) << fmt::format("Failed to read connection {}, error \"{}\"", to_string(uuid_), error.what());
            kill();
        }
    }

    void plasma_connection::handle_write(const boost::system::error_code& error, const pointer self)
    {
        if (!error)
        {
            std::lock_guard<std::mutex> lock{ send_lock_ };
            send_queue_.pop();
            if (!send_queue_.empty())
            {
                auto& packet{ send_queue_.front() };
                async_write(socket_, boost::asio::buffer(packet.data.get(), packet.total_length), std::bind(&plasma_connection::handle_write, this, std::placeholders::_1, self));
            }
        }
        else
        {
            ERR(lg_) << fmt::format("Failed to write connection {}, error \"{}\"", to_string(uuid_), error.what());
            kill();
        }
    }

    plasma_connection::plasma_connection(boost::asio::io_context& io_context, plasma_server& server, const boost::uuids::uuid& uuid) :
        server_{ server }, socket_{ io_context }, uuid_{ uuid }, raw_buffer_{ std::make_unique<std::byte[]>(max_raw_packet_length) }, packet_seq_{ 1 }
    {
    }

    plasma_connection::pointer plasma_connection::create(boost::asio::io_context& io_context, plasma_server& server, const boost::uuids::uuid& uuid) noexcept
    {
        return plasma_connection::pointer{ new plasma_connection{ io_context, server, uuid } };
    }

    boost::asio::ip::tcp::socket& plasma_connection::socket() noexcept
    {
        return socket_;
    }

    const boost::uuids::uuid& plasma_connection::uuid() const noexcept
    {
        return uuid_;
    }

    void plasma_connection::start_read()
    {
        async_read(socket_, boost::asio::buffer(raw_buffer_.get(), 1), std::bind(&plasma_connection::handle_read_head, this, std::placeholders::_1, std::placeholders::_2, 0, shared_from_this()));
    }

    void plasma_connection::start()
    {
        INF(lg_) << fmt::format("Incoming TCP connection from {}:{}, connection uuid {}", socket_.remote_endpoint().address().to_string(), socket_.remote_endpoint().port(), to_string(uuid_));
        start_read();
    }

    void plasma_connection::kill()
    {
        DBG(lg_) << "Killing connection " << to_string(uuid_);
        server_.remove_connection(uuid_);
        socket_.cancel();
        socket_.shutdown(boost::asio::socket_base::shutdown_both);
    }

    void plasma_connection::send(const type::packet& packet)
    {
        std::lock_guard<std::mutex> lock{ send_lock_ };
        send_queue_.push(packet);
        auto& new_packet{ send_queue_.front() };
        async_write(socket_, boost::asio::buffer(new_packet.data.get(), new_packet.total_length), std::bind(&plasma_connection::handle_write, this, std::placeholders::_1, shared_from_this()));
    }

    plasma_connection::~plasma_connection()
    {
        TRC(lg_) << "Destructing connection " << to_string(uuid_);
        socket_.close();
    }
}
