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

#include <plasma/extern.h>
#include <plasma/log.h>

namespace plasma
{
    class plasma_server;
    namespace networking
    {
        class PLASMA_EXTERN plasma_connection : public std::enable_shared_from_this<plasma_connection>
        {
        public:
            using pointer = std::shared_ptr<plasma_connection>;
            enum { max_raw_packet_length = 2097151 };
        private:
            class packet_node
            {
            private:
                friend class plasma_connection;
                const std::size_t max_length_;
                std::size_t cursor_;
                std::unique_ptr<std::byte[]> data_;
                packet_node(std::byte* const data, const std::size_t max_length) noexcept;
            };

            logger lg_;
            plasma_server& server_;
            boost::asio::ip::tcp::socket socket_;
            boost::uuids::uuid uuid_;
            std::queue<packet_node> send_queue_;
            std::mutex send_lock_;
            std::unique_ptr<std::byte[]> raw_buffer_;

            explicit plasma_connection(boost::asio::io_context& io_context, plasma_server& server, const boost::uuids::uuid& uuid);

            void handle_read(const boost::system::error_code& error, const std::size_t bytes_transferred, const pointer self);
            void handle_write(const boost::system::error_code& error, const pointer self);
        public:
            static pointer create(boost::asio::io_context& io_context, plasma_server& server, const boost::uuids::uuid& uuid) noexcept;

            boost::asio::ip::tcp::socket& socket() noexcept;

            const boost::uuids::uuid& uuid() const noexcept;

            void start();

            void kill();

            void send(std::byte* const data, const std::size_t max_length);
        };
    }
}
