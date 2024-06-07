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

#pragma once

#include <boost/asio.hpp>
#include <boost/uuid.hpp>
#include <memory>
#include <queue>
#include <mutex>

#include <plasma/networking/type/connection_status.hpp>
#include <plasma/networking/type/varint.hpp>
#include <plasma/networking/type/packet.hpp>
#include <plasma/extern.hpp>
#include <plasma/log.hpp>

namespace plasma
{
    class plasma_server;
    namespace networking
    {
        class PLASMA_EXTERN plasma_connection : public std::enable_shared_from_this<plasma_connection>
        {
        public:
            using pointer = std::shared_ptr<plasma_connection>;
            enum { max_raw_packet_length = 2097151 + type::varint_max_size };
        private:
            friend type::packet;
            logger lg_;
            bool killed_;
            plasma_server& server_;
            boost::asio::ip::tcp::socket socket_;
            boost::uuids::uuid uuid_;
            std::queue<type::packet> send_queue_;
            std::mutex send_lock_;
            std::unique_ptr<std::byte[]> head_buffer_;
            std::unique_ptr<std::byte[]> body_buffer_;
            std::size_t packet_seq_;
            type::connection_status status_;
            explicit plasma_connection(boost::asio::io_context& io_context, plasma_server& server, const boost::uuids::uuid& uuid);
            void handle_read_head(const boost::system::error_code& error, const std::size_t bytes_transferred, const std::size_t cursor, const pointer self);
            void handle_read(const boost::system::error_code& error, const std::size_t bytes_transferred, const std::size_t length_length, const pointer self);
            void handle_write(const boost::system::error_code& error, const std::size_t bytes_transferred, const pointer self);
            void start_read();
        public:
            static pointer create(boost::asio::io_context& io_context, plasma_server& server, const boost::uuids::uuid& uuid) noexcept;
            boost::asio::ip::tcp::socket& socket() noexcept;
            const boost::uuids::uuid& uuid() const noexcept;
            void start();
            void kill();    // Removes registeration in server
            void send(const type::packet& packet);
            ~plasma_connection();
        };
    }
}
