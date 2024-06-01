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

#include <map>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/uuid.hpp>

#include <plasma/networking/plasma_connection.h>
#include <plasma/config/plasma_config.h>
#include <plasma/plugin/plugin.h>
#include <plasma/log.h>

#include <plasma/version.h>

namespace plasma
{
    class PLASMA_EXTERN plasma_server : public plasma::plugin::plugin
    {
    private:
        plasma::config::plasma_config config_;
        boost::program_options::variables_map vm_;
        plasma::plugin::plugin_descriptor descriptor_;
        logger lg_;
        boost::asio::io_context context_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::map<boost::uuids::uuid, plasma::networking::plasma_connection::pointer> connections_;
        void start_accept();
        void handle_accept(plasma::networking::plasma_connection::pointer connection, const boost::system::error_code& error);
    public:
        explicit plasma_server(const boost::program_options::variables_map& vm);

        const plasma::plugin::plugin_descriptor& get_descriptor() noexcept override;

        void initialize(plasma::plugin::plugin_manager& manager) override;

        void remove_connection(const boost::uuids::uuid& uuid);

        ~plasma_server();
    };
}
