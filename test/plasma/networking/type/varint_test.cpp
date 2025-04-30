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

#define BOOST_TEST_MODULE varint_test
#include <boost/test/unit_test.hpp>
#include <map>
#include <plasma/log.hpp>
#include <plasma/networking/type/varint.hpp>

namespace
{
std::byte varint_0[]{std::byte{0x00}};
std::byte varint_1[]{std::byte{0x01}};
std::byte varint_2[]{std::byte{0x02}};
std::byte varint_127[]{std::byte{0x7f}};
std::byte varint_128[]{std::byte{0x80}, std::byte{0x01}};
std::byte varint_255[]{std::byte{0xff}, std::byte{0x01}};
std::byte varint_25565[]{std::byte{0xdd}, std::byte{0xc7}, std::byte{0x01}};
std::byte varint_2097151[]{std::byte{0xff}, std::byte{0xff}, std::byte{0x7f}};
std::byte varint_2147483647[]{std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0x07}};
std::byte varint__1[]{std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0x0f}};
std::byte varint__2147483648[]{std::byte{0x80}, std::byte{0x80}, std::byte{0x80}, std::byte{0x80}, std::byte{0x08}};
std::map<std::int32_t, std::tuple<std::byte *, std::size_t>> varint_samples{
    {{0, std::tuple{varint_0, sizeof varint_0}},
     {1, std::tuple{varint_1, sizeof varint_1}},
     {2, std::tuple{varint_2, sizeof varint_2}},
     {127, std::tuple{varint_127, sizeof varint_127}},
     {128, std::tuple{varint_128, sizeof varint_128}},
     {255, std::tuple{varint_255, sizeof varint_255}},
     {25565, std::tuple{varint_25565, sizeof varint_25565}},
     {2097151, std::tuple{varint_2097151, sizeof varint_2097151}},
     {2147483647, std::tuple{varint_2147483647, sizeof varint_2147483647}},
     {-1, std::tuple{varint__1, sizeof varint__1}},
     {-2147483648, std::tuple{varint__2147483648, sizeof varint__2147483648}}}};
std::byte varlong_9223372036854775807[]{std::byte{0xff}, std::byte{0xff}, std::byte{0xff},
                                        std::byte{0xff}, std::byte{0xff}, std::byte{0xff},
                                        std::byte{0xff}, std::byte{0xff}, std::byte{0x7f}};
std::byte varlong__1[]{std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff},
                       std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0xff}, std::byte{0x7f}};
std::byte varlong__9223372036854775808[]{std::byte{0x80}, std::byte{0x80}, std::byte{0x80}, std::byte{0x80},
                                         std::byte{0x80}, std::byte{0x80}, std::byte{0x80}, std::byte{0x80},
                                         std::byte{0x80}, std::byte{0x01}};
std::map<std::int64_t, std::tuple<std::byte *, std::size_t>> varlong_samples{{
    {9223372036854775807ll, std::tuple{varlong_9223372036854775807, sizeof varlong_9223372036854775807}},
    {-1ll, std::tuple{varlong__1, sizeof varlong__1}},
    {-9223372036854775808ull, std::tuple{varlong__9223372036854775808, sizeof varlong__9223372036854775808}},
}};
} // namespace

BOOST_AUTO_TEST_SUITE(varint_test)

BOOST_AUTO_TEST_CASE(varint_read)
{
    plasma::log::initialize_logging_system_test();
    try
    {
        for (auto [i, varint] : varint_samples)
        {
            BOOST_TEST(plasma::networking::type::detail::read_varint(
                           std::span{std::get<0>(varint), plasma::networking::type::detail::varint_max_size}) == i);
            BOOST_TEST(plasma::networking::type::detail::get_varint_length(
                           std::span{std::get<0>(varint), plasma::networking::type::detail::varint_max_size}) ==
                       std::get<1>(varint));
            BOOST_TEST(plasma::networking::type::detail::get_varint_length(i) == std::get<1>(varint));
        }
        for (auto [i, varlong] : varlong_samples)
        {
            BOOST_TEST(plasma::networking::type::detail::read_varlong(
                           std::span{std::get<0>(varlong), plasma::networking::type::detail::varlong_max_size}) == i);
            BOOST_TEST(plasma::networking::type::detail::get_varlong_length(
                           std::span{std::get<0>(varlong), plasma::networking::type::detail::varlong_max_size}) ==
                       std::get<1>(varlong));
            BOOST_TEST(plasma::networking::type::detail::get_varlong_length(i) == std::get<1>(varlong));
        }
    }
    catch (std::out_of_range &e)
    {
        BOOST_FAIL(e.what());
    }
}

BOOST_AUTO_TEST_SUITE_END()