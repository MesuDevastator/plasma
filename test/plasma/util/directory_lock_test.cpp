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

#define BOOST_TEST_MODULE directory_lock_test
#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <plasma/util/directory_lock.h>
#include <plasma/log.h>

BOOST_AUTO_TEST_SUITE(directory_lock_test)

BOOST_AUTO_TEST_CASE(directory_locking)
{
    std::filesystem::path test_dir{ std::filesystem::temp_directory_path() / "plasma_test_dir" };
    create_directories(test_dir);
    BOOST_TEST(!plasma::util::directory_lock::is_locked(test_dir));
    {
        plasma::util::directory_lock lock{ test_dir };
        BOOST_TEST(plasma::util::directory_lock::is_locked(test_dir));
    }
    BOOST_TEST(!plasma::util::directory_lock::is_locked(test_dir));
    BOOST_WARN(remove_all(test_dir) != 2);
}

BOOST_AUTO_TEST_SUITE_END()