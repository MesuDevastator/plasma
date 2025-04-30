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

#pragma once

#include <plasma/extern.hpp>
#include <plasma/networking/type/packet.hpp>
#include <plasma/networking/type/string.hpp>

namespace plasma::networking::type
{
/**
 * @brief The handshake packet
 * @note
 * @since 0.1.0
 */
class PLASMA_EXTERN handshake_packet : public packet
{
  private:
    varint_ref protocol_version_;
    string_ref server_address_;
    ushort_ref server_port_;
    varint_ref next_state_;
    handshake_packet(std::unique_ptr<std::byte[]> data, std::span<std::byte> data_span, varint_ref length,
                     varint_ref packet_id, std::span<std::byte> body_span, varint_ref protocol_version,
                     string_ref server_address, ushort_ref server_port, varint_ref next_state) noexcept;

  public:
    static constexpr const std::int32_t packet_id{0};
    handshake_packet(const handshake_packet &other);

    template <typename Self> auto &protocol_version(this Self &&self) noexcept
    {
        return self.protocol_version_;
    }

    template <typename Self> auto &server_address(this Self &&self) noexcept
    {
        return self.server_address_;
    }

    template <typename Self> auto &server_port(this Self &&self) noexcept
    {
        return self.server_port_;
    }

    template <typename Self> auto &next_state(this Self &&self) noexcept
    {
        return self.next_state_;
    }

    static std::unique_ptr<packet> parse(std::span<std::byte> body);

    void process(plasma_connection &connection) override;

    ~handshake_packet() noexcept override = default;
};
} // namespace plasma::networking::type
