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
#include <vector>

#include "../module/module.hpp"
#include "../module/interface/interface.hpp"
#include "../log.hpp"

namespace plasma::hook
{
    class hook_manager : public plasma::module::module
    {
    public:
        class hook_interface : public plasma::module::interface::interface
        {
        private:
            hook_manager& manager_;
        public:
            hook_interface(hook_manager& manager) : manager_{ manager }
            {
            }

            const char* get_name() const noexcept override
            {
               return "plasma.hook_manager.hook_interface";
            }
        };
    private:
        
    public:
        const char* get_name() const noexcept override
        {
            return "plasma.hook_manager";
        }

        void initialize(plasma::module::interface::interface_manager& interface_manager) override
        {
            logger lg{};
            INF(lg) << "Initializing hook manager";
            interface_manager.register_interface(std::unique_ptr<plasma::module::interface::interface>{ new hook_interface{ *this } });
        }

        void uninitialize(plasma::module::interface::interface_manager& interface_manager) override
        {
            logger lg{};
            INF(lg) << "Uninitializing hook manager";
            interface_manager.unregister_interface(hook_interface{ *this }.get_name());
        }
    };
}
