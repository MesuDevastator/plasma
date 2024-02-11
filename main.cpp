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

#include <iostream>
#include <exception>
#include <filesystem>

#include <boost/program_options.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include <version.hpp>

#include "plasma/log.hpp"

namespace
{
    const char* plasma_logo{
        "██████╗ ██╗      █████╗ ███████╗███╗   ███╗ █████╗ \n"
        "██╔══██╗██║     ██╔══██╗██╔════╝████╗ ████║██╔══██╗\n"
        "██████╔╝██║     ███████║███████╗██╔████╔██║███████║\n"
        "██╔═══╝ ██║     ██╔══██║╚════██║██║╚██╔╝██║██╔══██║\n"
        "██║     ███████╗██║  ██║███████║██║ ╚═╝ ██║██║  ██║\n"
        "╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝\n" };

    void initialize_logging_system()
    {
        boost::log::add_common_attributes();
#if !defined(NDEBUG) || defined(_DEBUG)
        auto formatter{
            boost::log::expressions::format("[%1%] [%2%:%3%] [%4%]: %5%")
            % boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
            % boost::log::expressions::attr<const char*>("File")
            % boost::log::expressions::attr<int>("Line")
            % boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity")
            % boost::log::expressions::message
        };
#else
        auto formatter{
            boost::log::expressions::format("[%1%] [%2%]: %3%")
            % boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
            % boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity")
            % boost::log::expressions::message
        };
#endif
        auto console_sink{ boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>() };
        console_sink->set_formatter(formatter);
        console_sink->locked_backend()->add_stream(plasma::log::clog_stream_ptr);

        std::filesystem::create_directory("./logs");
        auto file_sink{ boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>>(
            boost::log::keywords::target = "./logs",
            boost::log::keywords::file_name = "./logs/log_%N.log",
            boost::log::keywords::rotation_size = 10 * 1024 * 1024,
            boost::log::keywords::auto_flush = true,
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0)
        ) };
        file_sink->set_formatter(formatter);
        file_sink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(boost::log::keywords::target = "./logs"));
        file_sink->locked_backend()->scan_for_files();
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
        boost::log::core::get()->add_thread_attribute("File", boost::log::attributes::mutable_constant<const char*>(""));
        boost::log::core::get()->add_thread_attribute("Line", boost::log::attributes::mutable_constant<int>(0));
        boost::log::core::get()->add_sink(console_sink);
        boost::log::core::get()->add_sink(file_sink);
    }
}

int main(const int argc, const char* argv[])
{
#ifndef PLASMA_NOLOGO
    std::cout << plasma_logo;
#endif

    initialize_logging_system();
    logger lg{};
    TRC(lg) << "Logging system initialized";

    INF(lg) << g_full_version_string;

    boost::program_options::options_description desc{ "Plasma: Usage" };
    desc.add_options()
        ("help", "Show the help")
        ("init", "Initialize configurations only");
    boost::program_options::variables_map vm{};
    try
    {
        store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        notify(vm);
    }
    catch (const std::exception& e)
    {
        FTL(lg) << "Failed to parse command line: " << e.what();
        return 1;
    }
    if (vm.count("help"))
    {
        std::cerr << desc << std::endl;
        return 1;
    }
    return 0;
}
