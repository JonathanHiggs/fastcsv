add_library(fastcsv INTERFACE)
add_library(fastcsv::fastcsv ALIAS fastcsv)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

target_sources(fastcsv PRIVATE ${SOURCE_FILES})

target_include_directories(fastcsv INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(fastcsv INTERFACE
    fmt::fmt
)

install(
    TARGETS fastcsv
    EXPORT fastcsv_target
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/fastcsv
    DESTINATION include/
    FILES_MATCHING PATTERN "*.hpp"
)

install(
    EXPORT fastcsv_target
    NAMESPACE fastcsv::
    FILE fastcsv-config.cmake
    DESTINATION share/fastcsv
)