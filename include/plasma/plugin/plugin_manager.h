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
#include <string>

#include <plasma/plugin/plugin.h>
#include <plasma/extern.h>
#include <plasma/log.h>

namespace plasma::plugin
{
    class plugin;
    class PLASMA_EXTERN plugin_manager
    {
    private:
        std::map<std::size_t, std::pair<std::unique_ptr<plugin>, bool>> plugins_;
        logger lg_{};
    public:
        plugin_manager();
        void register_plugin(plugin* plugin);
        void initialize_plugins();
        std::size_t unload_plugin(const std::string& name);
        const std::unique_ptr<plugin>& get_plugin(const std::string& name) const;
    };
}

