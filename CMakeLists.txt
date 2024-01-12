# Copyright (c) 2023-2024 Mesu Devastator

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.27)
project(Plasma CXX)

add_subdirectory(./lib/fmt)

set(CMAKE_CXX_STANDARD 23)
set(RELEASE_VERSION "0.1a")
set(BRANCH_NAME "")
set(COMMIT_HASH "")
# NOTE: Regenerate CMake build files every build,
#       otherwise COMPILE_TIME will not be changed (might get fixed).
set(COMPILE_TIME "")

find_package(Boost COMPONENTS log program_options REQUIRED)
find_package(Git REQUIRED)

if (GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
            OUTPUT_VARIABLE BRANCH_NAME
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%H
            OUTPUT_VARIABLE COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif ()

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost include directories: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
    message(STATUS "Boost library version: ${Boost_LIB_VERSION}")
endif ()

string(TIMESTAMP COMPILE_TIME "%Y-%m-%d %H:%M:%S UTC" UTC)

message(STATUS "Plasma version: ${RELEASE_VERSION} ${BRANCH_NAME}:${COMMIT_HASH} (${COMPILE_TIME})")
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/version.h.ini"
        "${CMAKE_CURRENT_SOURCE_DIR}/version.h"
        @ONLY
)

set(SRC_LIST
        main.cpp
        plasma/log.hpp)

add_executable(Plasma ${SRC_LIST})
add_dependencies(Plasma fmt::fmt)
target_include_directories(Plasma PUBLIC ./lib/fmt/include)
target_link_directories(Plasma PUBLIC ${FMT_BINARY_DIR})
target_link_libraries(Plasma ${Boost_LIBRARIES})
