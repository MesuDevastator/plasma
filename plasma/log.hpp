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
#include <boost/log/sources/severity_logger.hpp>

namespace plasma::log
{
    using logger = boost::log::sources::severity_logger<boost::log::trivial::severity_level>;

    inline const boost::shared_ptr<std::ostream> clog_stream_ptr{ &std::clog, boost::null_deleter{} };

    template<typename TValue>
    TValue set_get_attr(const char* name, TValue value)
    {
        auto attr{ boost::log::attribute_cast<boost::log::attributes::mutable_constant<TValue>>(boost::log::core::get()->get_thread_attributes()[name]) };
        attr.set(value);
        return attr.get();
    }
}
#if !defined(NDEBUG) || defined(_DEBUG)
#define TRC(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::plasma::log::set_get_attr("File", __FILE__))(::plasma::log::set_get_attr("Line", __LINE__))(::boost::log::keywords::severity = ::boost::log::trivial::severity_level::trace))
#define DBG(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::plasma::log::set_get_attr("File", __FILE__))(::plasma::log::set_get_attr("Line", __LINE__))(::boost::log::keywords::severity = ::boost::log::trivial::severity_level::debug))
#define INF(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::plasma::log::set_get_attr("File", __FILE__))(::plasma::log::set_get_attr("Line", __LINE__))(::boost::log::keywords::severity = ::boost::log::trivial::severity_level::info))
#define WRN(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::plasma::log::set_get_attr("File", __FILE__))(::plasma::log::set_get_attr("Line", __LINE__))(::boost::log::keywords::severity = ::boost::log::trivial::severity_level::warning))
#define ERR(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::plasma::log::set_get_attr("File", __FILE__))(::plasma::log::set_get_attr("Line", __LINE__))(::boost::log::keywords::severity = ::boost::log::trivial::severity_level::error))
#define FTL(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::plasma::log::set_get_attr("File", __FILE__))(::plasma::log::set_get_attr("Line", __LINE__))(::boost::log::keywords::severity = ::boost::log::trivial::severity_level::fatal))
#else
#define TRC(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::boost::log::keywords::severity = ::boost::log::trivial::severity_level::trace))
#define DBG(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::boost::log::keywords::severity = ::boost::log::trivial::severity_level::debug))
#define INF(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::boost::log::keywords::severity = ::boost::log::trivial::severity_level::info))
#define WRN(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::boost::log::keywords::severity = ::boost::log::trivial::severity_level::warning))
#define ERR(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::boost::log::keywords::severity = ::boost::log::trivial::severity_level::error))
#define FTL(lg) BOOST_LOG_STREAM_WITH_PARAMS((lg), (::boost::log::keywords::severity = ::boost::log::trivial::severity_level::fatal))
#endif

using namespace plasma::log;
