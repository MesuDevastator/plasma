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

#include <array>
#include <functional>
#include <memory>
#include <string>

#include <plasma/extern.hpp>

namespace plasma::util
{
class PLASMA_EXTERN identifier
{
  private:
    static const std::string &validate_path(const std::string &namespace_, const std::string &path);
    static const std::string &validate_namespace(const std::string &namespace_, const std::string &path);

    explicit identifier(const std::array<std::string, 2> &id) noexcept;

  protected:
    class extra_data
    {
    };

    static std::array<std::string, 2> split(const std::string &id, const char delimiter) noexcept;

    identifier(const std::string &namespace_, const std::string &path,
               [[maybe_unused]] std::unique_ptr<extra_data> extra_data) noexcept;

  public:
    // TODO: DFU serialization
    static constexpr auto namespace_separator{':'};
    static constexpr auto default_namespace{"minecraft"};
    static constexpr auto realms_namespace{"realms"};

    std::string namespace_;
    std::string path;

    static bool is_path_valid(const std::string &path) noexcept;
    static bool is_path_character_valid(const char character) noexcept;
    static bool is_namespace_valid(const std::string &namespace_) noexcept;
    static bool is_namespace_character_valid(const char character) noexcept;

    identifier(const std::string &namespace_, const std::string &path) noexcept;
    explicit identifier(const std::string &id) noexcept;
    identifier(const std::string &id, const char delimiter) noexcept;

    identifier with_path(const std::string &path) const;

    virtual operator std::string() const;
    virtual bool operator==(const identifier &other) const;
};
} // namespace plasma::util

namespace std
{
template <> struct PLASMA_EXTERN hash<plasma::util::identifier>
{
    std::size_t operator()(const plasma::util::identifier &identifier) const;
};
} // namespace std
