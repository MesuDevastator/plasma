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

#include "interface.hpp"
#include "../../log.hpp"

namespace plasma::module::interface
{
    class interface_manager final
    {
    private:
        std::map<std::size_t, std::unique_ptr<interface>> interfaces_;
    public:
        void register_interface(std::unique_ptr<interface> interface)
        {
            logger lg{};
            TRC(lg) << "Registering interface \"" << interface->get_name() << "\"";
            interfaces_.insert({ boost::hash<const char*>{}(interface->get_name()), std::move(interface) });
        }

        void unregister_interface(const char* name)
        {
            logger lg{};
            TRC(lg) << "Unregistering interface \"" << name << "\"";
            interfaces_.erase(boost::hash<const char*>{}(name));
        }

        const std::unique_ptr<interface>& get_interface(const char* name) const
        {
            return interfaces_.at(boost::hash<const char*>{}(name));
        }
    };
}
