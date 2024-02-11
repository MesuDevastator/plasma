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
#include <memory>

#include "plugin.hpp"
#include "exception/plugin_loading_exception.hpp"

namespace plasma::plugin
{
    class plugin_manager
    {
    private:
        std::map<const char*, std::unique_ptr<plugin>> plugins_;
    public:
        plugin_manager() = default;
        void load_plugin(plugin* plugin)
        {
            if (plugins_.contains(plugin->get_name()))
            {
                throw exception::plugin_loading_exception{ plugin->get_name() };
            }
            plugin->initialize(*this);
            plugins_.insert({ plugin->get_name(), std::unique_ptr<class plugin>{ plugin } });
        }

        auto unload_plugin(const char* name)
        {
            return plugins_.erase(name);
        }
    };
}

