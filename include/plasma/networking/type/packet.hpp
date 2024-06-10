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

#include <cstdint>
#include <cstddef>
#include <memory>
#include <plasma/networking/type/varint.hpp>
#include <plasma/networking/type/connection_status.hpp>
#include <plasma/extern.hpp>

namespace plasma::networking
{
    class plasma_connection;
    namespace type
    {
        class PLASMA_EXTERN packet
        {
        public:
            const std::size_t head_length;
            const std::size_t body_length;
            const std::size_t total_length;
            const std::int32_t packet_id;
            // Whole packet data including head
            const std::unique_ptr<std::byte[]> data;
            packet(const std::size_t body_length, const std::int32_t packet_id, const std::byte* const body);
            packet(const std::size_t head_length, const std::size_t body_length, const std::int32_t packet_id, const std::byte* const data);
            packet(const packet& other);
            void process(plasma::networking::plasma_connection& connection);

            class PLASMA_EXTERN handshake_packet
            {
            public:
                static constexpr const std::int32_t packet_id{ 0 };
                std::int32_t protocol_version;
                std::u8string server_address;
                std::uint16_t server_port;
                std::int32_t next_state;
                static handshake_packet parse(const std::byte* const body, const std::size_t max_length);
                packet create() const;
            };
        };
    }
}
