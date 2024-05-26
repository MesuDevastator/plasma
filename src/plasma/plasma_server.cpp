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

#include <plasma/config/plasma_config.h>
#include <plasma/plugin/plugin.h>
#include <plasma/plasma_server.h>

namespace plasma
{
    plasma_server::plasma_server(const boost::program_options::variables_map& vm) :
        config_{}, vm_{ vm },
        descriptor_{
            .name = "plasma::server",
            .version = semantic_version,
            .description = "Reimplemented Minecraft server core",
            .authors = { "Mesu Devastator" },
            .website = "https://github.com/MesuDevastator/plasma",
            .license = "MIT"
        }
    {
    }

    const plasma::plugin::plugin_descriptor& plasma_server::get_descriptor() noexcept
    {
        return descriptor_;
    }

    void plasma_server::initialize(plasma::plugin::plugin_manager&)
    {
        config_.load();
        if (vm_.count("init"))
        {
            INF(lg_) << "Initialized configurations";
            return;
        }
    }

    plasma_server::~plasma_server()
    {
        config_.save();
    }
}
