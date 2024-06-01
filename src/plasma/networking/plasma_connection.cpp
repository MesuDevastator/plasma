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
#include <plasma/networking/plasma_connection.h>
#include <plasma/plasma_server.h>
#include <sstream>

#include <fmt/format.h>

namespace plasma::networking
{
    plasma_connection::packet_node::packet_node(std::byte* const data, const std::size_t max_length) noexcept :
        max_length_{ max_length }, data_{ std::make_unique<std::byte[]>(max_length) }
    {
        std::copy(data, data + max_length, data_.get());
    }

    void plasma_connection::handle_read(const boost::system::error_code& error, const std::size_t bytes_transferred, pointer self)
    {
        if (!error)
        {
            std::stringstream ss{ "Received data: " };
            for (std::size_t i{}; i < bytes_transferred; i++)
            {
                ss << static_cast<unsigned char>(raw_buffer_[i]) << " ";
            }
            DBG(lg_) << ss.str();
            std::fill(raw_buffer_.get(), raw_buffer_.get() + bytes_transferred, std::byte{ 0 });
            socket_.async_read_some(boost::asio::buffer(raw_buffer_.get(), max_raw_packet_length), std::bind(&plasma_connection::handle_read, this, std::placeholders::_1, std::placeholders::_2, self));
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
                auto& node{ send_queue_.front() };
                async_write(socket_, boost::asio::buffer(node.data_.get(), node.max_length_), std::bind(&plasma_connection::handle_write, this, std::placeholders::_1, self));
            }
        }
        else
        {
            ERR(lg_) << fmt::format("Failed to write connection {}, error \"{}\"", to_string(uuid_), error.what());
            kill();
        }
    }

    plasma_connection::plasma_connection(boost::asio::io_context& io_context, plasma_server& server, const boost::uuids::uuid& uuid) :
        server_{ server }, socket_{ io_context }, uuid_{ uuid }, raw_buffer_{ std::make_unique<std::byte[]>(max_raw_packet_length) }
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

    void plasma_connection::start()
    {
        INF(lg_) << fmt::format("Incoming TCP connection from {}:{}, connection uuid {}", socket_.remote_endpoint().address().to_string(), socket_.remote_endpoint().port(), to_string(uuid_));
        socket_.async_read_some(boost::asio::buffer(raw_buffer_.get(), max_raw_packet_length), std::bind(&plasma_connection::handle_read, this, std::placeholders::_1, std::placeholders::_2, shared_from_this()));
    }

    void plasma_connection::kill()
    {
        DBG(lg_) << "Killing connection " << to_string(uuid_);
        server_.remove_connection(uuid_);
        socket_.shutdown(boost::asio::socket_base::shutdown_both);
        socket_.close();
    }

    void plasma_connection::send(std::byte* const data, const std::size_t max_length)
    {
        bool pending{};
        std::lock_guard<std::mutex> lock{ send_lock_ };
        if (send_queue_.size() > 0)
        {
            pending = true;
        }
        send_queue_.push(packet_node{ data, max_length });
        if (pending)
        {
            return;
        }
        async_write(socket_, boost::asio::buffer(data, max_length), std::bind(&plasma_connection::handle_write, this, std::placeholders::_1, shared_from_this()));
    }
}
