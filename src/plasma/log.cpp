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

#include <plasma/log.hpp>

#include <filesystem>

namespace plasma::log
{
    boost::shared_ptr<std::ostream> clog_stream_ptr = boost::shared_ptr<std::ostream>{ &std::clog, boost::null_deleter{} };
    bool color_enabled{ true };
    bool logging_initialized{ false };

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

    void color_formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
    {
        auto severity{ rec[boost::log::trivial::severity] };
        if (severity && color_enabled)
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
        if (severity && color_enabled)
        {
            strm << "\x1b[0m";
        }
    }

    void initialize_logging_system()
    {
        if (logging_initialized)
        {
            return;
        }
        boost::log::add_common_attributes();
        auto console_sink{ boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>() };
        console_sink->set_formatter(&color_formatter);
        console_sink->locked_backend()->add_stream(plasma::log::clog_stream_ptr);

        std::filesystem::create_directory("./log");
        auto file_sink{ boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend>>(
            boost::log::keywords::target = "./log",
            boost::log::keywords::file_name = "./log/log_%N.log",
            boost::log::keywords::rotation_size = 10 * 1024 * 1024,
            boost::log::keywords::auto_flush = true,
            boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0)
        ) };
        file_sink->set_formatter(formatter);
        file_sink->locked_backend()->set_file_collector(boost::log::sinks::file::make_collector(boost::log::keywords::target = "./log"));
        file_sink->locked_backend()->scan_for_files();
#if defined(PLASMA_ALWAYS_TRACE) || !defined(NDEBUG) || defined(_DEBUG)
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
#else
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
#endif
        boost::log::core::get()->add_global_attribute("Severity", boost::log::attributes::mutable_constant<boost::log::trivial::severity_level>(boost::log::trivial::severity_level::info));
        boost::log::core::get()->add_global_attribute("File", boost::log::attributes::mutable_constant<const char*>(""));
        boost::log::core::get()->add_global_attribute("Line", boost::log::attributes::mutable_constant<int>(0));
        boost::log::core::get()->add_sink(console_sink);
        boost::log::core::get()->add_sink(file_sink);
        logging_initialized = true;
    }

    void initialize_logging_system_test()
    {
        if (logging_initialized)
        {
            return;
        }
        boost::log::add_common_attributes();
        auto console_sink{ boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>() };
        console_sink->set_formatter(&color_formatter);
        console_sink->locked_backend()->add_stream(plasma::log::clog_stream_ptr);
        boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
        boost::log::core::get()->add_global_attribute("Severity", boost::log::attributes::mutable_constant<boost::log::trivial::severity_level>(boost::log::trivial::severity_level::info));
        boost::log::core::get()->add_global_attribute("File", boost::log::attributes::mutable_constant<const char*>(""));
        boost::log::core::get()->add_global_attribute("Line", boost::log::attributes::mutable_constant<int>(0));
        boost::log::core::get()->add_sink(console_sink);
        logging_initialized = true;
    }
}
