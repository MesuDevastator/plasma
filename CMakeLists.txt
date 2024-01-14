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
add_subdirectory(./lib/dobby)

set(CMAKE_CXX_STANDARD 23)
set(RELEASE_VERSION "0.1a")
set(BRANCH_NAME "")
set(COMMIT_HASH "")
# NOTE: Regenerate CMake build files every build,
#       otherwise COMPILE_TIME will not be changed (might get fixed).
set(COMPILE_TIME "")
set(Boost_USE_STATIC_LIBS ON)

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
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
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
    "version.h.ini"
    "version.h"
    @ONLY
)

add_custom_target(build-breakpad ALL
    COMMAND make
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/breakpad
    COMMENT "Build breakpad"
)
add_library(breakpad-client STATIC IMPORTED)
set_property(TARGET breakpad-client APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(breakpad-client PROPERTIES IMPORTED_LOCATION_NOCONFIG "${CMAKE_CURRENT_SOURCE_DIR}/lib/breakpad/src/client/linux/libbreakpad_client.a")
add_dependencies(breakpad-client build-breakpad)

add_executable(Plasma
    main.cpp
    plasma/log.hpp
    plasma/crash_report/crash_reporter.hpp
    plasma/hook/hook_error.hpp
    plasma/hook/hook_manager.hpp
    plasma/module/interface/interface_manager.hpp
    plasma/module/interface/interface.hpp
    plasma/module/module_manager.hpp
    plasma/module/module.hpp
)
add_dependencies(Plasma fmt::fmt breakpad-client dobby_static)
target_include_directories(Plasma PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt/include ${CMAKE_CURRENT_SOURCE_DIR}/lib/breakpad/src ${CMAKE_CURRENT_SOURCE_DIR}/lib/dobby/include)
target_link_libraries(Plasma ${Boost_LIBRARIES} fmt::fmt breakpad-client dobby_static)
