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

#include <boost/container_hash/hash.hpp>
#include <plasma/util/identifier.h>
#include <plasma/util/invalid_identifier_exception.h>

#include <fmt/format.h>

namespace plasma::util
{
    identifier::identifier(const std::string& namespace_, const std::string& path, [[maybe_unused]] std::unique_ptr<extra_data> extra_data) noexcept :
        namespace_{ namespace_ }, path{ path }
    {
    }

    identifier::identifier(const std::string& namespace_, const std::string& path) noexcept :
        identifier{ namespace_, path, nullptr }
    {
    }

    identifier::identifier(const std::array<std::string, 2>& id) noexcept :
        identifier{ id[0], id[1] }
    {
    }

    identifier::identifier(const std::string& id) noexcept :
        identifier{ split(id, namespace_separator) }
    {
    }

    identifier::identifier(const std::string& id, const char delimiter) noexcept :
        identifier{ split(id, delimiter) }
    {
    }

    std::array<std::string, 2> identifier::split(const std::string& id, const char delimiter) noexcept
    {
        std::array<std::string, 2> strings{ default_namespace, id };
        if (const auto i{ id.find(delimiter) }; i != std::string::npos)
        {
            strings[1] = id.substr(i + 1);
            if (i >= 1)
            {
                strings[0] = id.substr(0, i);
            }
        }
        return strings;
    }

    bool identifier::is_path_character_valid(const char character) noexcept
    {
        return character == '_' || character == '-' || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || character == '/' || character == '.';
    }

    bool identifier::is_namespace_character_valid(const char character) noexcept
    {
        return character == '_' || character == '-' || (character >= 'a' && character <= 'z') || (character >= '0' && character <= '9') || character == '.';
    }

    bool identifier::is_path_valid(const std::string& path) noexcept
    {
        for (const char chr : path)
        {
            if (!is_path_character_valid(chr))
            {
                return false;
            }
        }
        return true;
    }

    bool identifier::is_namespace_valid(const std::string& namespace_) noexcept
    {
        for (const char chr : namespace_)
        {
            if (!is_namespace_character_valid(chr))
            {
                return false;
            }
        }
        return true;
    }

    const std::string& identifier::validate_path(const std::string& namespace_, const std::string& path)
    {
        if (!is_path_valid(namespace_))
        {
            throw invalid_identifier_exception{ fmt::format("Non [a-z0-9_.-] character in path of location: {}{}{}", namespace_, namespace_separator, path) };
        }
        return path;
    }

    const std::string& identifier::validate_namespace(const std::string& namespace_, const std::string& path)
    {
        if (!is_namespace_valid(namespace_))
        {
            throw invalid_identifier_exception{ fmt::format("Non [a-z0-9_.-] character in namespace of location: {}{}{}", namespace_, namespace_separator, path) };
        }
        return namespace_;
    }

    identifier identifier::with_path(const std::string& path) const
    {
        return identifier{ namespace_, validate_path(namespace_, path) };
    }

    identifier::operator std::string() const
    {
        return fmt::format("{}{}{}", namespace_, namespace_separator, path);
    }

    bool identifier::operator==(const identifier& other) const
    {
        return namespace_ == other.namespace_ && path == other.path;
    }
}

namespace std
{
    std::size_t hash<plasma::util::identifier>::operator()(const plasma::util::identifier& identifier) const
    {
        hash<std::string> hasher{};
        std::size_t seed{ hasher(identifier.namespace_) };
        boost::hash_combine(seed, hasher(identifier.path));
        return seed;
    }
}
