/*
 * Copyright (c) 2023-2025 Mesu Devastator
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

#include <filesystem>
#include <fstream>

#include <fmt/format.h>

#include <plasma/log.hpp>
#include <plasma/util/directory_lock.hpp>

namespace plasma::util
{
directory_lock::directory_lock(const std::filesystem::path directory)
{
    if (is_locked(directory))
    {
        throw std::runtime_error{fmt::format("Failed to lock the locked directory {}", directory.string())};
    }
    create_directories(directory);
    lock_file_path_ = std::filesystem::path{directory / directory_lock_name};
    lock_file_ = std::ofstream{lock_file_path_, std::ios::trunc};
    lock_file_ << "plasma::util::directory_lock locked";
    lock_file_.flush();
}

bool directory_lock::is_locked(const std::filesystem::path directory)
{
    return exists(directory / directory_lock_name);
}

directory_lock::~directory_lock()
{
    if (lock_file_.is_open())
    {
        lock_file_.close();
    }
    remove(lock_file_path_);
}
} // namespace plasma::util
