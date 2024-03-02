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

#include <boost/program_options.hpp>

#include <plasma/log.hpp>
#include <plasma/config/plasma_config.h>
#include <plasma/plugin/plugin.h>
#include <plasma/plasma_server.h>

#include <version.hpp>

extern bool g_color_enabled;

namespace plasma
{
    plasma_server::plasma_server(boost::program_options::variables_map vm) :
        config_{}, vm_{ std::move(vm) }
    {
    }

    const char* plasma_server::get_name() noexcept
    {
        return "plasma";
    }

    const char* plasma_server::get_version() noexcept
    {
        return g_release_version;
    }

    void plasma_server::initialize(plasma::plugin::plugin_manager& manager)
    {
        logger lg{};
        config_.load();
        if (vm_.count("init"))
        {
            INF(lg) << "Initialized configurations";
            return;
        }
        g_color_enabled = config_.logging.color_enabled;
    }
}
