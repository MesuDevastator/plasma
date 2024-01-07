/*
 * Copyright (c) 2023 Mesu Devastator
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

#include <iostream>
#include <exception>
#include <filesystem>

#include <boost/program_options.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include <plasma/log.hpp>

#include "version.h"

namespace
{
	const char* plasma_logo{
		"██████╗ ██╗      █████╗ ███████╗███╗   ███╗ █████╗ \n"
		"██╔══██╗██║     ██╔══██╗██╔════╝████╗ ████║██╔══██╗\n"
		"██████╔╝██║     ███████║███████╗██╔████╔██║███████║\n"
		"██╔═══╝ ██║     ██╔══██║╚════██║██║╚██╔╝██║██╔══██║\n"
		"██║     ███████╗██║  ██║███████║██║ ╚═╝ ██║██║  ██║\n"
		"╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝╚═╝  ╚═╝\n"
	};
}

int main(const int argc, const char* argv[])
{
	std::cout << plasma_logo
			  << "Plasma " << g_release_version << std::endl
			  << "    - git " << g_branch_name << ":" << g_commit_hash << std::endl
			  << "    - cts " << g_compile_time << std::endl
			  << "    - boost " << g_boost_library_version << std::endl;

	// TODO: Add file sink
	boost::log::add_common_attributes();
	auto sink{ boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>() };
	sink->locked_backend()->add_stream(plasma::log::clog_stream_ptr);
	sink->set_formatter(boost::log::expressions::stream
		<< "[" << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S")
		<< "] [" << boost::log::trivial::severity << "] " << boost::log::expressions::message);
	boost::log::core::get()->add_sink(sink);
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
	logger lg{};
	TRC(lg) << "Logging system initialized";

	// TODO: Fill description of the options
	boost::program_options::options_description desc{ "[Plasma] Usage" };
	desc.add_options()
			("help", "Show the help")
			("init-settings", R"(Initialize "server.info" and "eula.info", then quits)");
	boost::program_options::variables_map vm{};
	try
	{
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
		if (vm.count("help"))
		{
			std::cerr << desc << std::endl;
			return 1;
		}
		DBG(lg) << "Working directory: " << std::filesystem::absolute(".");
	}
	catch (const std::exception& e)
	{
		FTL(lg) << "Failed to start plasma: " << e.what();
		return 1;
	}
	return 0;
}
