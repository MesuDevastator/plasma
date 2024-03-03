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
set(BUILD_SHARED_LIBS ON)

add_subdirectory(./lib/fmt)
add_subdirectory(./lib/mimalloc)
add_subdirectory(./lib/boost)

set(CMAKE_CXX_STANDARD 23)
set(RELEASE_VERSION "0.1a")
set(BRANCH_NAME "")
set(COMMIT_HASH "")


# add_compile_definitions(PLASMA_NOLOGO)

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

message(STATUS "Plasma version: ${RELEASE_VERSION} ${BRANCH_NAME}:${COMMIT_HASH}")
configure_file(version.hpp.in version.hpp @ONLY)

file(GLOB_RECURSE PLASMA_SRCS "include/*.h" "include/*.hpp" "src/*.cpp")
message(STATUS "Plasma sources: ${PLASMA_SRCS}")
add_executable(Plasma
    main.cpp
    ${PLASMA_SRCS}
)
add_dependencies(Plasma Boost::log Boost::program_options Boost::property_tree Boost::interprocess fmt::fmt mimalloc)
target_include_directories(Plasma PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/fmt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/cxx_detect
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/mimalloc/include
)
target_link_libraries(Plasma Boost::log Boost::program_options Boost::property_tree Boost::interprocess fmt::fmt mimalloc)

install(
    TARGETS Plasma
    EXPORT Plasma
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)
