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

#include <filesystem>
#include <fmt/format.h>
#include <boost/property_tree/ptree.hpp>
#include <yaml_parser.hpp>

#include <plasma/config/plasma_config.hpp>

namespace plasma::config
{
    plasma_config::plasma_config() noexcept :
        logging
        {
            .color_logging = true
        },
        world
        {
            .storage
            {
                .base_dir = ".",
                .backup_dir = "./backups"
            },
            .name = "world"
        },
        networking
        {
            .listen_address = "0.0.0.0",
            .listen_port = 25565,
            .max_players = 20,
            .timeout_milliseconds = 60000
        }
    {
    }

    void plasma_config::load()
    {
        INF(lg_) << "Loading plasma configuration";
        if (!std::filesystem::exists(file_path))
        {
            WRN(lg_) << fmt::format("Failed to find {}, initializing a new one", file_path);
            save();
            return;
        }
        boost::property_tree::ptree tree{};
        yaml_parser::read_yaml(file_path, tree);

        logging.color_logging = tree.get("logging.color_logging", logging.color_logging);

        world.storage.base_dir = tree.get("world.storage.base_dir", world.storage.base_dir.string());
        world.storage.backup_dir = tree.get("world.storage.backup_dir", world.storage.backup_dir.string());
        world.name = tree.get("world.name", world.name);

        networking.listen_address = tree.get("networking.listen_address", networking.listen_address);
        networking.listen_port = tree.get("networking.listen_port", networking.listen_port);
        networking.max_players = tree.get("networking.max_players", networking.max_players);
        networking.timeout_milliseconds = tree.get("networking.timeout_milliseconds", networking.timeout_milliseconds);

        save();
    }

    void plasma_config::save()
    {
        INF(lg_) << "Saving plasma configuration";
        boost::property_tree::ptree tree{};

        tree.put("logging.color_logging", logging.color_logging);

        tree.put("world.storage.base_dir", world.storage.base_dir.string());
        tree.put("world.storage.backup_dir", world.storage.backup_dir.string());
        tree.put("world.name", world.name);

        tree.put("networking.listen_address", networking.listen_address);
        tree.put("networking.listen_port", networking.listen_port);
        tree.put("networking.max_players", networking.max_players);
        tree.put("networking.timeout_milliseconds", networking.timeout_milliseconds);

        create_directories(std::filesystem::path{ file_path }.parent_path());
        yaml_parser::write_yaml(file_path, tree);
    }
}
