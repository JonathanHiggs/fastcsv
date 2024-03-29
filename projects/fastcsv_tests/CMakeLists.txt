include(GoogleTest)

add_executable(fastcsv_tests)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/templates/locator.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/locator.cpp"
)

list(APPEND SOURCE_FILES "${CMAKE_CURRENT_BINARY_DIR}/locator.cpp")

target_sources(fastcsv_tests PRIVATE ${SOURCE_FILES})

if (MSVC)
    target_compile_options(fastcsv_tests PRIVATE
        "/std:c++20"                    # c++ standard
        "/FC"                           # display full path in diagnostics
        "/WX"                           # warnings as errors
        "/W4"                           # warning level [0,4]
        "$<$<CONFIG:DEBUG>:/Oi>"        # replace calls with intrinsics
        "$<$<CONFIG:DEBUG>:/Zi>"        # generate complete debug info
        "$<$<CONFIG:DEBUG>:/JMC>"       # just my code
        "$<$<CONFIG:RELEASE>:/Ot>"      # prefer fast optimizations
    )
else()
    target_compile_options(fastcsv_tests PRIVATE
        "-std=c++20"
        "-Wall"
        "-Wextra"
        "-pedantic"
        "-Werror"
    )
endif()

target_include_directories(fastcsv_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(fastcsv_tests PRIVATE
    fastcsv
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME fastcsv_tests COMMAND fastcsv_tests)

gtest_discover_tests(fastcsv_tests WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} DISCOVERY_MODE PRE_TEST)