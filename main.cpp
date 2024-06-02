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

#include <cxx_detect.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <exception>
#include <memory>

#include <boost/program_options.hpp>
#include <fmt/format.h>

#if CXX_OS_WINDOWS
#include <windows.h>
#endif

#include <plasma/version.hpp>

#include <plasma/log.hpp>
#include <plasma/plugin/plugin_manager.hpp>
#include <plasma/plasma_server.hpp>

namespace
{
    auto plasma_logo{
        R"(__________.__                                )""\n"
        R"(\______   \  | _____    ______ _____ _____   )""\n"
        R"( |     ___/  | \__  \  /  ___//     \\__  \  )""\n"
        R"( |    |   |  |__/ __ \_\___ \|  Y Y  \/ __ \_)""\n"
        R"( |____|   |____(____  /____  >__|_|  (____  /)""\n"
        R"(                    \/     \/      \/     \/ )""\n" };
    std::unique_ptr<plasma::plugin::plugin_manager> manager;
}

int main(const int argc, const char* argv[])
{
    using namespace std::literals::chrono_literals;
#if CXX_OS_WINDOWS
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    setlocale(LC_ALL, ".utf-8");
#ifndef PLASMA_NOLOGO
    std::cout << plasma_logo;
#endif

    plasma::log::initialize_logging_system();
    logger lg{};
    TRC(lg) << "Logging system initialized";

    INF(lg) << plasma::full_version;

    {
        std::stringstream ss{};
        for (int i{}; i < argc; i++)
        {
            ss << fmt::format("[{}]:\"{}\" ", i, argv[i]);
        }
        DBG(lg) << "Console argument: " << ss.str();
    }
    boost::program_options::options_description desc{ "Plasma: Usage" };
    desc.add_options()
        ("help", "Show the help")
        ("version", "Show the version only")
        ("init", "Initialize configurations only")
        ("color", "Enable colored logging");
    boost::program_options::variables_map vm{};
    try
    {
        store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        notify(vm);
    }
    catch (const std::exception& e)
    {
        FTL(lg) << "Failed to parse command line: " << e.what();
        return 1;
    }

    if (vm.count("version"))
    {
        return 0;
    }

    if (vm.count("help"))
    {
        std::cerr << desc << std::endl;
        return 1;
    }

    manager = std::make_unique<plasma::plugin::plugin_manager>();
    manager->register_plugin(new plasma::plasma_server{ std::move(vm) });
    manager->initialize_plugins();
    std::atexit([]{
        dynamic_cast<plasma::plasma_server*>(manager->get_plugin(plasma::plasma_server::name).get())->stop();
    });
    auto handler{
        [](const int signal){
        if (signal == SIGINT)
        {
            logger lg{};
            FTL(lg) << "Caught SIGINT, terminating...";
            std::exit(0);
        }
    } };
    if (std::signal(SIGINT, handler) == SIG_ERR)
    {
        WRN(lg) << "Failed to set signal handler";
    }
    else
    {
        TRC(lg) << "Set signal handler";
    }
    dynamic_cast<plasma::plasma_server*>(manager->get_plugin(plasma::plasma_server::name).get())->start();
    return 0;
}
