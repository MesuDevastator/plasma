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

#ifdef _WIN32
#include <windows.h>
#endif

#include <version.hpp>

#include <plasma/log.hpp>
#include <plasma/plugin/plugin_manager.h>
#include <plasma/plasma_server.h>

bool g_color_enabled{ true };

namespace
{
    const char* plasma_logo{
        "██████╗ ██╗      █████╗ ███████╗███╗   ███╗ █████╗ \n"
        "██╔══██╗██║     ██╔══██╗██╔════╝████╗ ████║██╔══██╗\n"
        "██████╔╝██║     ███████║███████╗██╔████╔██║███████║\n"
        "██╔═══╝ ██║     ██╔══██║╚════██║██║╚██╔╝██║██╔══██║\n"
        "██║     ███████╗██║  ██║███████║██║ ╚═╝ ██║██║  ██║\n"
        "╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝\n" };

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

#ifdef _WIN32
    bool enable_ansi_escape_sequence()
    {
        HANDLE handle_stderr{ GetStdHandle(STD_ERROR_HANDLE) };
        if (handle_stderr == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        DWORD mode{};
        if (!GetConsoleMode(handle_stderr, &mode))
        {
            return false;
        }

        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(handle_stderr, mode))
        {
            return false;
        }
        return true;
    }
#endif

    void color_formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
    {
        auto severity{ rec[boost::log::trivial::severity] };
        if (severity && g_color_enabled)
        {
            switch (severity.get())
            {
            case boost::log::trivial::severity_level::trace:
            case boost::log::trivial::severity_level::debug:
                strm << "\x1b[0;90m";
                break;
            case boost::log::trivial::severity_level::info:
                strm << "\x1b[0;37m";
                break;
            case boost::log::trivial::severity_level::warning:
                strm << "\x1b[0;33m";
                break;
            case boost::log::trivial::severity_level::error:
                strm << "\x1b[0;31m";
                break;
            case boost::log::trivial::severity_level::fatal:
                strm << "\x1b[0;91m";
                break;
            default:
                break;
            }
        }
        formatter(rec, strm);
        if (severity && g_color_enabled)
        {
            strm << "\x1b[0m";
        }
    }

    void initialize_logging_system()
    {
        boost::log::add_common_attributes();
        auto console_sink{ boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>() };
        console_sink->set_formatter(&color_formatter);
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
#if !defined(NDEBUG) || defined(_DEBUG)
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
#else
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
#endif
        boost::log::core::get()->add_thread_attribute("File", boost::log::attributes::mutable_constant<const char*>(""));
        boost::log::core::get()->add_thread_attribute("Line", boost::log::attributes::mutable_constant<int>(0));
        boost::log::core::get()->add_sink(console_sink);
        boost::log::core::get()->add_sink(file_sink);
#ifdef _WIN32
        if (!enable_ansi_escape_sequence())
        {
            g_color_enabled = false;
            logger lg{};
            WRN(lg) << "Failed to enable Win32 ANSI escape sequence support, colorful console output will be disabled";
        }
#endif
    }
}

int main(const int argc, const char* argv[])
{
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    setlocale(LC_ALL, ".utf-8");
#ifndef PLASMA_NOLOGO
    std::cout << plasma_logo;
#endif

    initialize_logging_system();
    logger lg{};
    TRC(lg) << "Logging system initialized";

    INF(lg) << g_full_version_string;

    {
        std::stringstream ss{};
        for (int i{}; i < argc; ++i)
        {
            ss << argv[i] << " ";
        }
        DBG(lg) << "Console argument: " << ss.str();
    }
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

    plasma::plugin::plugin_manager manager{};
    manager.load_plugin(new plasma::plasma_server{ std::move(vm) });
    return 0;
}
