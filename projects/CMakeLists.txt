add_subdirectory(fastcsv)

if (FASTCSV_BUILD_TESTS)
    add_subdirectory(fastcsv_tests)
endif()