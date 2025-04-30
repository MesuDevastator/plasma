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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <plasma/extern.hpp>
#include <plasma/networking/type/connection_status.hpp>
#include <plasma/networking/type/varint.hpp>

namespace plasma::networking
{
class plasma_connection;
}

namespace plasma::networking::type
{
class PLASMA_EXTERN packet
{
  private:
    std::unique_ptr<std::byte[]> data_;
    std::span<std::byte> data_span_;
    // Subspans
    varint_ref length_;
    varint_ref packet_id_;
    std::span<std::byte> body_span_;

  protected:
    packet(std::unique_ptr<std::byte[]> data, std::span<std::byte> data_span, varint_ref length, varint_ref packet_id,
           std::span<std::byte> body_span) noexcept;

  public:
    packet(const packet &other);

    std::span<const std::byte> data() const noexcept
    {
        return data_span_;
    }

    std::span<std::byte> &data() noexcept
    {
        return data_span_;
    }

    template <typename Self> auto &length(this Self &&self) noexcept
    {
        return self.length_;
    }

    template <typename Self> auto &packet_id(this Self &&self) noexcept
    {
        return self.packet_id_;
    }

    std::span<const std::byte> body() const noexcept
    {
        return body_span_;
    }

    std::span<std::byte> &body() noexcept
    {
        return body_span_;
    }

    static std::unique_ptr<packet> parse(std::span<std::byte> body, std::int32_t packet_id, connection_status status);

    virtual void process([[maybe_unused]] plasma_connection &connection)
    {
    }

    virtual ~packet() noexcept = default;
};
} // namespace plasma::networking::type
