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

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid.hpp>
#include <map>
#include <mutex>

#include <plasma/config/plasma_config.hpp>
#include <plasma/log.hpp>
#include <plasma/networking/plasma_connection.hpp>
#include <plasma/plugin/plugin.hpp>

#include <plasma/version.hpp>

namespace plasma
{
class PLASMA_EXTERN plasma_server : public plasma::plugin::plugin
{
  private:
    friend networking::plasma_connection;
    logger lg_;
    plasma::config::plasma_config config_;
    plasma::plugin::plugin_descriptor descriptor_;
    boost::asio::thread_pool pool_;
    boost::program_options::variables_map vm_;
    boost::asio::io_context context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::map<boost::uuids::uuid, plasma::networking::plasma_connection::pointer> connections_;
    std::mutex connections_lock_;
    void start_accept();
    void handle_accept(plasma::networking::plasma_connection::pointer connection,
                       const boost::system::error_code &error);

  public:
    static constexpr auto name{"plasma::server"};
    explicit plasma_server(const boost::program_options::variables_map &vm);
    const plasma::plugin::plugin_descriptor &get_descriptor() noexcept override;
    void initialize(plasma::plugin::plugin_manager &manager) override;
    std::size_t remove_connection(const boost::uuids::uuid &uuid);
    void start();
    void stop();
    ~plasma_server();
};
} // namespace plasma
