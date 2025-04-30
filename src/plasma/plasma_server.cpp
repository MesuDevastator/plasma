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

#include <thread>

#include <fmt/format.h>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include <plasma/config/plasma_config.hpp>
#include <plasma/plasma_server.hpp>
#include <plasma/plugin/plugin.hpp>

namespace plasma
{
void plasma_server::start_accept()
{
    auto connection{plasma::networking::plasma_connection::create(*this, boost::uuids::random_generator{}())};
    acceptor_.async_accept(connection->socket(), [this, connection](const boost::system::error_code &error) {
        handle_accept(connection, error);
    });
}

void plasma_server::handle_accept(plasma::networking::plasma_connection::pointer connection,
                                  const boost::system::error_code &error)
{
    if (!error)
    {
        std::lock_guard<std::mutex> lock{connections_lock_};
        connections_.insert(std::pair{connection->uuid(), connection});
        post(pool_, [connection] { connection->start(); });
    }
    start_accept();
}

plasma_server::plasma_server(const boost::program_options::variables_map &vm)
    : config_{}, descriptor_{.name = name,
                             .version = semantic_version,
                             .description = "Reimplemented Minecraft server core",
                             .authors = {"Mesu Devastator"},
                             .website = "https://github.com/MesuDevastator/plasma",
                             .license = "MIT"},
      pool_{std::thread::hardware_concurrency() * 2}, vm_{vm}, context_{},
      acceptor_{context_} // TODO: configurable thread amount
{
}

const plasma::plugin::plugin_descriptor &plasma_server::get_descriptor() noexcept
{
    return descriptor_;
}

void plasma_server::initialize(plasma::plugin::plugin_manager &)
{
    config_.load();
    if (!vm_.count("color") && !config_.logging.color_logging)
    {
        plasma::log::color_enabled = false;
        PLASMA_LOG(lg_, info) << "Disabled colored logging";
    }
    try
    {
        boost::asio::ip::tcp::endpoint ep{boost::asio::ip::make_address(config_.networking.listen_address),
                                          config_.networking.listen_port};
        acceptor_ = boost::asio::ip::tcp::tcp::acceptor{context_, ep};
        PLASMA_LOG(lg_, info) << fmt::format("Accepting TCP connections on {}:{}", ep.address().to_string(), ep.port());
    }
    catch (const boost::system::system_error &)
    {
        PLASMA_LOG(lg_, fatal) << "Failed to listen " << config_.networking.listen_address;
        throw;
    }
    start_accept();
}

std::size_t plasma_server::remove_connection(const boost::uuids::uuid &uuid)
{
    std::lock_guard<std::mutex> lock{connections_lock_};
    return connections_.erase(uuid);
}

void plasma_server::start()
{
    context_.run();
}

void plasma_server::stop()
{
    std::lock_guard<std::mutex> lock{connections_lock_};
    for (auto &connection : connections_)
    {
        connection.second->kill();
    }
    connections_.clear();
    context_.stop();
}

plasma_server::~plasma_server()
{
    PLASMA_LOG(lg_, info) << "Stopping plasma server";
    config_.save();
}
} // namespace plasma
