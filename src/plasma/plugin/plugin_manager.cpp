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

#include <map>
#include <memory>

#include <plasma/log.hpp>
#include <plasma/plugin/plugin.h>

#include <plasma/plugin/plugin_manager.h>

namespace plasma::plugin
{
    plugin_manager::plugin_manager() = default;

    bool plugin_manager::load_plugin(plugin* plugin)
    {
        plasma::log::logger lg{};
        TRC(lg) << "Loading plugin " << plugin->get_name() << " " << plugin->get_version();
        if (plugins_.contains(plugin->get_name()))
        {
            return false;
        }
        plugin->initialize(*this);
        plugins_.insert({ plugin->get_name(), std::unique_ptr<class plugin>{ plugin } });
        return true;
    }

    std::size_t plugin_manager::unload_plugin(const char* name)
    {
        return plugins_.erase(name);
    }

    const std::unique_ptr<plugin>& plugin_manager::get_plugin(const char* name) const
    {
        return plugins_.at(name);
    }
}

