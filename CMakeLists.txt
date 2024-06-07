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

cmake_minimum_required(VERSION 3.29)
project(Plasma LANGUAGES CXX)
option(PLASMA_NOLOGO "Disable Plasma logo" OFF)
option(PLASMA_ALWAYS_TRACE "Use trace logging even in release builds" OFF)

set(BUILD_SHARED_LIBS ON)
set(YAML_CPP_INSTALL ON)
set(MI_SECURE ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_SHARED_LINKER_FLAGS "-z noexecstack -z now -Wl,-rpath,.:../lib")
    set(CMAKE_EXE_LINKER_FLAGS "-z noexecstack -z now -Wl,-rpath,.:../lib")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fstack-protector-strong -fvisibility=hidden -fPIE")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wextra -fstack-protector-strong -fvisibility=hidden -fPIE")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -fstack-protector-strong -fvisibility=hidden -fPIE")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -Wall -Wextra -fstack-protector-strong -fvisibility=hidden -fPIE")
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_definitions(_FORTIFY_SOURCE=2)
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pie")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pie")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pie")
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pie")
    endif()
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_compile_definitions(_WIN32_WINNT=0x0601 NOMINMAX)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        link_libraries(ws2_32 wsock32)
    endif()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "i386" OR CMAKE_SYSTEM_PROCESSOR MATCHES "i686")
        add_compile_definitions(_X86_)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
        add_compile_definitions(_AMD64_)
    # arm64 support may be added later
    else()
        message(FATAL_ERROR "Plasma: Unknown system processor")
    endif()
endif()

add_subdirectory(./lib/fmt)
add_subdirectory(./lib/boost)
add_subdirectory(./lib/mimalloc)
add_subdirectory(./lib/semver)
add_subdirectory(./lib/yaml-cpp)

enable_testing()

set(RELEASE_VERSION "0.1.0-alpha")
set(BRANCH_NAME "")
set(COMMIT_HASH "")

if(PLASMA_NOLOGO)
    add_compile_definitions(PLASMA_NOLOGO)
endif()
if(PLASMA_ALWAYS_TRACE)
    add_compile_definitions(PLASMA_ALWAYS_TRACE)
endif()

find_package(Git REQUIRED)

if (GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
        OUTPUT_VARIABLE BRANCH_NAME
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
        OUTPUT_VARIABLE COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )
endif ()

file(GLOB __boost_libraries ABSOLUTE "${PROJECT_SOURCE_DIR}/lib/boost/libs" "${PROJECT_SOURCE_DIR}/lib/boost/libs/*")
foreach(dir ${__boost_libraries})
    if(IS_DIRECTORY "${dir}")
        if(EXISTS "${dir}/CMakeLists.txt")
            get_filename_component(library "${dir}" NAME)
            if(TARGET boost_${library})
                set(BOOST_LIBRARIES ${BOOST_LIBRARIES} boost_${library})
            endif()
        endif()
    endif()
endforeach()
message(STATUS "Plasma version: ${RELEASE_VERSION} ${BRANCH_NAME}:${COMMIT_HASH}")
configure_file(version.cpp.in version.cpp @ONLY)

set(DEPENDENCIES
    fmt
    mimalloc-static
    yaml-cpp
)
include_directories(
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/lib/fmt/include"
    "${PROJECT_SOURCE_DIR}/lib/cxx_detect"
    "${PROJECT_SOURCE_DIR}/lib/semver/include"
    "${PROJECT_SOURCE_DIR}/lib/yaml_parser/include"
    "${PROJECT_SOURCE_DIR}/lib/yaml_cpp/include"
)

file(GLOB_RECURSE PLASMA_SRCS "${PROJECT_SOURCE_DIR}/include/*.h" "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(plasma SHARED ${PLASMA_SRCS} "${PROJECT_SOURCE_DIR}/lib/yaml_parser/src/yaml_parser.cpp" "${PROJECT_BINARY_DIR}/version.cpp")
add_dependencies(plasma ${DEPENDENCIES} ${BOOST_LIBRARIES})
target_link_libraries(plasma ${DEPENDENCIES} ${BOOST_LIBRARIES})
target_compile_definitions(plasma PRIVATE PLASMA_LIB)

add_executable(plasma-bootstrap "${PROJECT_SOURCE_DIR}/main.cpp")
add_dependencies(plasma-bootstrap ${DEPENDENCIES} ${BOOST_LIBRARIES} plasma)
target_link_libraries(plasma-bootstrap ${DEPENDENCIES} ${BOOST_LIBRARIES} plasma)

file(GLOB_RECURSE TEST_SRCS "${PROJECT_SOURCE_DIR}/test/*.cpp")
foreach(test_case ${TEST_SRCS})
    get_filename_component(test_name ${test_case} NAME_WE)
    get_filename_component(test_path ${test_case} PATH)
    string(REPLACE "_" "-" test_name "${test_name}")
    message(STATUS "Adding test ${test_name} (${test_case})")
    add_executable(${test_name} ${test_case})
    add_dependencies(${test_name} ${DEPENDENCIES} ${BOOST_LIBRARIES} plasma)
    target_link_libraries(${test_name} ${DEPENDENCIES} ${BOOST_LIBRARIES} plasma boost_unit_test_framework)
    add_test(${test_name} ${PROJECT_BINARY_DIR}/${test_name})
endforeach()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS plasma ${BOOST_LIBRARIES} DESTINATION lib)
else()
    install(TARGETS plasma ${BOOST_LIBRARIES} DESTINATION bin)
endif()
install(TARGETS plasma-bootstrap DESTINATION bin)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/plasma" DESTINATION include)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        install(FILES "${CMAKE_SYSROOT}/bin/libgcc_s_seh-1.dll" DESTINATION bin)
        install(FILES "${CMAKE_SYSROOT}/bin/libstdc++-6.dll" DESTINATION bin)
        install(FILES "${CMAKE_SYSROOT}/bin/libwinpthread-1.dll" DESTINATION bin)
    endif()
endif()
