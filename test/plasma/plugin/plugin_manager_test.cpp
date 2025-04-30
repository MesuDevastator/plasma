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

#define BOOST_TEST_MODULE plugin_manager_test
#include <boost/test/unit_test.hpp>
#include <plasma/log.hpp>
#include <plasma/plugin/plugin_loading_exception.hpp>
#include <plasma/plugin/plugin_manager.hpp>
#include <semver.hpp>

constexpr auto depended_test_plugin_name{"depended_test_plugin"};
constexpr auto test_plugin_name{"test_plugin"};
constexpr auto conflicted_test_plugin_name{"conflicted_test_plugin"};

namespace
{
class depended_test_plugin : public plasma::plugin::plugin
{
  private:
    bool initialized_{false};
    plasma::plugin::plugin_descriptor descriptor_{.name = depended_test_plugin_name};

  public:
    const plasma::plugin::plugin_descriptor &get_descriptor() noexcept override
    {
        return descriptor_;
    }

    void initialize(plasma::plugin::plugin_manager &) override
    {
        if (initialized_)
        {
            throw std::runtime_error{"Plugin is already initialized"};
        }
        initialized_ = true;
    }

    bool initialized() const noexcept
    {
        return initialized_;
    }
};

class conflicted_test_plugin : public plasma::plugin::plugin
{
  private:
    bool initialized_{false};
    plasma::plugin::plugin_descriptor descriptor_{.name = conflicted_test_plugin_name};

  public:
    const plasma::plugin::plugin_descriptor &get_descriptor() noexcept override
    {
        return descriptor_;
    }

    void initialize(plasma::plugin::plugin_manager &) override
    {
        if (initialized_)
        {
            throw std::runtime_error{"Plugin is already initialized"};
        }
        initialized_ = true;
    }

    bool initialized() const noexcept
    {
        return initialized_;
    }
};

class test_plugin : public plasma::plugin::plugin
{
  private:
    bool initialized_{false};
    plasma::plugin::plugin_descriptor descriptor_{.name = test_plugin_name,
                                                  .dependencies = {{.name = depended_test_plugin_name}},
                                                  .optional_dependencies = {{.name = "random_dependency"}},
                                                  .conflicts = {{.name = conflicted_test_plugin_name}}};

  public:
    const plasma::plugin::plugin_descriptor &get_descriptor() noexcept override
    {
        return descriptor_;
    }

    void initialize(plasma::plugin::plugin_manager &) override
    {
        if (initialized_)
        {
            throw std::runtime_error{"Plugin is already initialized"};
        }
        initialized_ = true;
    }

    bool initialized() const noexcept
    {
        return initialized_;
    }
};
} // namespace

BOOST_AUTO_TEST_SUITE(plugin_manager_test)

BOOST_AUTO_TEST_CASE(plugin_loading_unloading)
{
    plasma::log::initialize_logging_system_test();
    plasma::plugin::plugin_manager pm{};
    const auto test{new test_plugin{}};
    pm.register_plugin(test);
    pm.register_plugin(new depended_test_plugin{});
    pm.initialize_plugins();
    try
    {
        const auto &plugin{pm.get_plugin(test_plugin_name)};
        BOOST_REQUIRE(plugin != nullptr);
        const auto ptr{dynamic_cast<test_plugin *>(plugin.get())};
        BOOST_REQUIRE(ptr != nullptr);
        BOOST_REQUIRE(ptr == test);
        BOOST_CHECK(ptr->initialized());
    }
    catch (const std::out_of_range &)
    {
        BOOST_FAIL("Failed to load plugin");
    }
    pm.unload_plugin(test_plugin_name);
    try
    {
        pm.get_plugin(test_plugin_name);
        BOOST_FAIL("Failed to unload plugin");
    }
    catch (const std::out_of_range &)
    {
        // expected
    }
}

BOOST_AUTO_TEST_CASE(plugin_depending_conflicting)
{
    plasma::log::initialize_logging_system_test();
    plasma::plugin::plugin_manager pm{};
    pm.register_plugin(new test_plugin{});
    pm.register_plugin(new conflicted_test_plugin{});
    bool detected{};
    try
    {
        pm.initialize_plugins();
    }
    catch (const plasma::plugin::plugin_loading_exception &)
    {
        detected = true;
    }
    BOOST_REQUIRE(detected);
    detected = false;
    try
    {
        pm.unload_plugin(conflicted_test_plugin_name);
        pm.initialize_plugins();
    }
    catch (const plasma::plugin::plugin_loading_exception &)
    {
        detected = true;
    }
    BOOST_REQUIRE(detected);
    detected = true;
    try
    {
        pm.register_plugin(new depended_test_plugin{});
        pm.initialize_plugins();
    }
    catch (const plasma::plugin::plugin_loading_exception &)
    {
        detected = false;
    }
    BOOST_REQUIRE(detected);
}

BOOST_AUTO_TEST_SUITE_END()