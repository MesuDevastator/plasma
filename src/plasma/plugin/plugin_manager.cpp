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

#include <map>
#include <memory>

#include <fmt/format.h>

#include <plasma/plugin/plugin.h>
#include <plasma/plugin/plugin_manager.h>
#include <plasma/plugin/plugin_loading_exception.h>

namespace plasma::plugin
{
    plugin_manager::plugin_manager() = default;

    void plugin_manager::register_plugin(plugin* plugin)
    {
        if (plugin == nullptr)
        {
            throw plugin_loading_exception{ "Trying to register a null plugin" };
        }
        INF(lg_) << "Registering plugin " << plugin->get_descriptor().name << " " << plugin->get_descriptor().version;
        std::size_t hash{ std::hash<std::string>{}(plugin->get_descriptor().name) };
        TRC(lg_) << "Plugin " << plugin->get_descriptor().name << " name hash \"" << std::hex << hash << std::dec << "\"";
        if (plugins_.contains(hash))
        {
            throw plugin_loading_exception{ "Trying to register a duplicated plugin" };
        }
        plugins_.insert(std::pair{ hash, std::pair{ std::unique_ptr<class plugin>{ plugin }, false } });
    }

    void plugin_manager::initialize_plugins()
    {
        for (auto& [_, plugin] : plugins_)
        {
            if (plugin.second)
            {
                continue;
            }
            for (const auto& conflict : plugin.first->get_descriptor().conflicts)
            {
                if (plugins_.contains(std::hash<std::string>{}(conflict.name)))
                {
                    ERR(lg_) << fmt::format("Detected conflict plugin {} while loading {}", conflict.name, plugin.first->get_descriptor().name);
                    throw plugin_loading_exception{ "Plugin conflict detected" };
                }
            }
            auto initialize{
                [this](auto&& self, auto& plugin) -> void
                {
                    if (plugin.second)
                    {
                        return;
                    }
                    plugin.second = true;   // To prevent infinite recursion
                    INF(lg_) << "Initializing plugin " << plugin.first->get_descriptor().name << " " << plugin.first->get_descriptor().version;
                    try
                    {
                        for (const auto& dependency : plugin.first->get_descriptor().dependencies)
                        {
                            std::size_t hash{ std::hash<std::string>{}(dependency.name) };
                            if (!plugins_.contains(hash))
                            {
                                ERR(lg_) << fmt::format("Dependency plugin {} not found while loading {}", dependency.name, plugin.first->get_descriptor().name);
                                throw plugin_loading_exception{ "Plugin dependency not found" };
                            }
                            self(self, plugins_.at(hash));   // Throws std::out_of_range if dependency not found
                        }
                        for (const auto& optional_dependency : plugin.first->get_descriptor().optional_dependencies)
                        {
                            if (plugins_.contains(std::hash<std::string>{}(optional_dependency.name)))
                            {
                                self(self, plugins_[std::hash<std::string>{}(optional_dependency.name)]);
                            }
                            else
                            {
                                WRN(lg_) << "Optional dependency " << optional_dependency.name << " not found";
                            }
                        }
                        plugin.first->initialize(*this);
                    }
                    catch (...)
                    {
                        plugin.second = false;
                        throw;
                    }
                }
                                };
            initialize(initialize, plugin);
        }
    }

    std::size_t plugin_manager::unload_plugin(const std::string& name)
    {
        std::hash<std::string> hasher{};
        std::size_t hash{ hasher(name) };
        if (plugins_.at(hash).second)
        {
            for (const auto& [_, plugin] : plugins_)
            {
                for (const auto& dependency : plugin.first->get_descriptor().dependencies)
                {
                    if (dependency.name == name)
                    {
                        throw plugin_loading_exception{ "Failed to unload a depended plugin" };
                    }
                }
            }
        }
        return plugins_.erase(hash);
    }

    const std::unique_ptr<plugin>& plugin_manager::get_plugin(const std::string& name) const
    {
        return plugins_.at(std::hash<std::string>{}(name)).first;
    }
}

