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

#include <boost/container_hash/hash.hpp>

#include "module.hpp"
#include "interface/interface_manager.hpp"
#include "../log.hpp"

namespace plasma::module
{
    class module_manager final
    {
    private:
        std::map<std::size_t, std::unique_ptr<module>> modules_;
        interface::interface_manager interfaces_;
    public:
        module_manager() noexcept : interfaces_{}
        {
        }

        module_manager(interface::interface_manager interface_manager) noexcept : interfaces_{ std::move(interface_manager) }
        {
        }

        void load_module(std::unique_ptr<module> module)
        {
            module->initialize(interfaces_);
            modules_.insert({ boost::hash<const char*>{}(module->get_name()), std::move(module) });
        }

        void unload_module(std::unique_ptr<module> module)
        {
            module->uninitialize(interfaces_);
            modules_.erase(boost::hash<const char*>{}(module->get_name()));
        }

        const std::unique_ptr<module>& get_module(const char* name) const
        {
            return modules_.at(boost::hash<const char*>{}(name));
        }

        const interface::interface_manager& get_interface_manager() const
        {
            return interfaces_;
        }
    };
}
