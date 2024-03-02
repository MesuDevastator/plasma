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

#include <exception>
#include <filesystem>
#include <fstream>

#include <fmt/format.h>

#include <boost/interprocess/sync/file_lock.hpp>

#include <plasma/util/directory_lock.h>


namespace plasma::util
{
    directory_lock::directory_lock(std::filesystem::path directory)
    {
        lock_file_path_ = std::filesystem::path{ directory /= directory_lock_name };
        create_directories(directory);
        lock_file_ = std::ofstream{ lock_file_path_, std::ios::trunc };
        lock_file_ << "☃";
        lock_file_.flush();
        if (!lock_file_)
        {
            throw boost::interprocess::lock_exception{};
        }
        lock_ = boost::interprocess::file_lock{ lock_file_path_.c_str() };
        lock_.lock();
    }

    bool directory_lock::is_locked(std::filesystem::path directory)
    {
        try
        {
            directory_lock lock{ std::move(directory) };
        }
        catch (boost::interprocess::lock_exception&)
        {
            throw;
        }
        catch (boost::interprocess::interprocess_exception&)
        {
            return true;
        }
        return false;
    }

    directory_lock::~directory_lock()
    {
        lock_.unlock();
        if (lock_file_.is_open())
        {
            lock_file_.close();
        }
    }

    std::string directory_lock::directory_lock_name = "session.lock";
}
