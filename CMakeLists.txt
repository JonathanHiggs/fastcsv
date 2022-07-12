cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(ENABLE_DEVELOPER_MODE TRUE CACHE BOOL "Enable 'developer mode'")
set(OPT_WARNINGS_AS_ERRORS_DEVELOPER_DEFAULT TRUE)

project(
    fastcsv
    VERSION "0.0.1"
    DESCRIPTION "A fast csv parser with very few allocations"
    HOMEPAGE_URL "https://github.com/JonathanHiggs/fastcsv"
    LANGUAGES CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(FASTCSV_BUILD_TESTS "Build unit tests" ON)

find_package(fmt CONFIG REQUIRED)

if (FASTCSV_BUILD_TESTS)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
endif()

add_subdirectory(projects)