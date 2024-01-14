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

#if defined __linux__
#include <client/linux/handler/exception_handler.h>
#else
 // TODO: Check for Windows / Mac
#endif

#include <memory>

#include "../module/module.hpp"
#include "../log.hpp"

namespace plasma::crash_report
{
    class crash_reporter : public plasma::module::module
    {
    private:
        std::unique_ptr<google_breakpad::ExceptionHandler> handler;
    
        static bool callback(const google_breakpad::MinidumpDescriptor& descriptor, void* context, bool succeeded)
        {
            crash_reporter* this_ptr{ reinterpret_cast<crash_reporter*>(context) };
            logger lg{};
            FTL(lg) << "Plasma crashed! Dump file saved to \"" << descriptor.path() << "\"";
            return succeeded;
        }

    public:
        const char* get_name() const noexcept override
        {
            return "plasma.crash_reporter";
        }

        void initialize(plasma::module::interface::interface_manager& interface_manager) override
        {
            logger lg{};
            INF(lg) << "Initializing crash reporter";
            google_breakpad::MinidumpDescriptor descriptor{ "." };
            handler = std::make_unique<google_breakpad::ExceptionHandler>(descriptor, nullptr, callback, this, true, -1);
        }
        
        void uninitialize(plasma::module::interface::interface_manager& interface_manager) override
        {
            logger lg{};
            INF(lg) << "Uninitializing crash reporter";
        }
    };
}
