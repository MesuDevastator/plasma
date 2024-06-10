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

#include <iostream>

#include <boost/smart_ptr.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include <plasma/extern.hpp>

namespace plasma::log
{
    extern boost::shared_ptr<std::ostream> clog_stream_ptr;
    extern bool logging_initialized;
    PLASMA_EXTERN extern bool color_enabled;

    template<typename TValue>
    void set_attr(const char* name, TValue value)
    {
        boost::log::attribute_cast<boost::log::attributes::mutable_constant<TValue>>(boost::log::core::get()->get_global_attributes()[name]).set(value);
    }

    PLASMA_EXTERN void initialize_logging_system();

    // For tests
    PLASMA_EXTERN void initialize_logging_system_test();
}

using logger = boost::log::sources::logger_mt;

// NOTE: These logging macros need to be surrounded by braces, since they are composed of multiple statements!
#if !defined(NDEBUG) || defined(_DEBUG)
#define TRC(lg) ::plasma::log::set_attr("Line", __LINE__);::plasma::log::set_attr("File", __FILE__);::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::trace);BOOST_LOG((lg))
#define DBG(lg) ::plasma::log::set_attr("Line", __LINE__);::plasma::log::set_attr("File", __FILE__);::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::debug);BOOST_LOG((lg))
#define INF(lg) ::plasma::log::set_attr("Line", __LINE__);::plasma::log::set_attr("File", __FILE__);::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::info);BOOST_LOG((lg))
#define WRN(lg) ::plasma::log::set_attr("Line", __LINE__);::plasma::log::set_attr("File", __FILE__);::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::warning);BOOST_LOG((lg))
#define ERR(lg) ::plasma::log::set_attr("Line", __LINE__);::plasma::log::set_attr("File", __FILE__);::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::error);BOOST_LOG((lg))
#define FTL(lg) ::plasma::log::set_attr("Line", __LINE__);::plasma::log::set_attr("File", __FILE__);::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::fatal);BOOST_LOG((lg))
#else
#define TRC(lg) ::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::trace);BOOST_LOG((lg))
#define DBG(lg) ::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::debug);BOOST_LOG((lg))
#define INF(lg) ::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::info);BOOST_LOG((lg))
#define WRN(lg) ::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::warning);BOOST_LOG((lg))
#define ERR(lg) ::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::error);BOOST_LOG((lg))
#define FTL(lg) ::plasma::log::set_attr("Severity", ::boost::log::trivial::severity_level::fatal);BOOST_LOG((lg))
#endif
