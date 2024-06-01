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

#include <cstddef>
#define BOOST_TEST_MODULE varint_test
#include <boost/test/unit_test.hpp>
#include <plasma/networking/type/varint.h>
#include <plasma/log.h>
#include <map>

namespace
{
    std::byte varint_0[]{ std::byte{ 0x00 } };
    std::byte varint_1[]{ std::byte{ 0x01 } };
    std::byte varint_2[]{ std::byte{ 0x02 } };
    std::byte varint_127[]{ std::byte{ 0x7f } };
    std::byte varint_128[]{ std::byte{ 0x80 }, std::byte{ 0x01 } };
    std::byte varint_255[]{ std::byte{ 0xff }, std::byte{ 0x01 } };
    std::byte varint_25565[]{ std::byte{ 0xdd }, std::byte{ 0xc7 }, std::byte{ 0x01 } };
    std::byte varint_2097151[]{ std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0x7f } };
    std::byte varint_2147483647[]{ std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0x07 } };
    std::byte varint__1[]{ std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0x0f } };
    std::byte varint__2147483648[]{ std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x08 } };
    std::map<int, std::byte*> varint_samples{ {
        { 0, varint_0 },
        { 1, varint_1 },
        { 2, varint_2 },
        { 127, varint_127 },
        { 128, varint_128 },
        { 255, varint_255 },
        { 25565, varint_25565 },
        { 2097151, varint_2097151 },
        { 2147483647, varint_2147483647 },
        { -1, varint__1 },
        { -2147483648, varint__2147483648 }
    } };
    std::byte varlong_9223372036854775807[]{ std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0x7f } };
    std::byte varlong__1[]{ std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0xff }, std::byte{ 0x7f } };
    std::byte varlong__9223372036854775808[]{ std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x80 }, std::byte{ 0x01 } };
    std::map<int64_t, std::byte*> varlong_samples{ {
        { 9223372036854775807ll, varlong_9223372036854775807 },
        { -1ll, varlong__1 },
        { -9223372036854775808ull, varlong__9223372036854775808 },
    } };
}

BOOST_AUTO_TEST_SUITE(varint_test)

BOOST_AUTO_TEST_CASE(varint_read)
{
    plasma::log::initialize_logging_system_test();
    try
    {
        for (auto [i, varint] : varint_samples)
        {
            BOOST_TEST(plasma::networking::type::read_varint(varint, 5) == i);
        }
        for (auto [i, varlong] : varlong_samples)
        {
            BOOST_TEST(plasma::networking::type::read_varlong(varlong, 10) == i);
        }
    }
    catch (plasma::networking::type::varint_exception& e)
    {
        BOOST_FAIL(e.what());
    }
}

BOOST_AUTO_TEST_SUITE_END()