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

#include <iostream>

#include <boost/core/null_deleter.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/smart_ptr.hpp>

#include <plasma/extern.hpp>
#include <plasma/util/path_to_filename.hpp>

namespace plasma::log
{
extern boost::shared_ptr<std::ostream> clog_stream_ptr;
extern bool logging_initialized;

template <typename TValue> TValue get_set_attr(const char *name, TValue value)
{
    auto attr{boost::log::attribute_cast<boost::log::attributes::mutable_constant<TValue>>(
        boost::log::core::get()->get_global_attributes()[name])};
    attr.set(value);
    return attr.get();
}

PLASMA_EXTERN extern bool color_enabled;

PLASMA_EXTERN void initialize_logging_system();

// For tests
PLASMA_EXTERN void initialize_logging_system_test();
} // namespace plasma::log

using logger = boost::log::sources::logger_mt;

#if !defined(NDEBUG) || defined(_DEBUG)
#define PLASMA_LOG(logger, sev)                                                                                        \
    BOOST_LOG_STREAM_WITH_PARAMS(                                                                                      \
        (logger), (::plasma::log::get_set_attr("File", ::plasma::util::path_parser<__FILE__>::value.data()))(          \
                      ::plasma::log::get_set_attr("Line", __LINE__))(                                                  \
                      ::plasma::log::get_set_attr("Severity", boost::log::trivial::sev)))
#else
#define PLASMA_LOG(logger, sev)                                                                                        \
    BOOST_LOG_STREAM_WITH_PARAMS((logger), (::plasma::log::get_set_attr("Severity", boost::log::trivial::sev)))
#endif
