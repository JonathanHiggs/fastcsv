add_library(fastcsv)
add_library(fastcsv::fastcsv ALIAS fastcsv)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

target_sources(fastcsv PRIVATE ${SOURCE_FILES})

target_compile_options(fastcsv PRIVATE
    "/std:c++20"                    # c++ standard
    "/bigobj"                       # increases the number of sections in .obj files
    "/FC"                           # display full path in diagnostics
    "/WX"                           # warnings as errors
    "/W4"                           # warning level [0,4]
    "/wd4099"                       # exclude: type first seen using #
    "/wd4100"                       # exclude: unreferenced parameter
    "/wd4201"                       # exclude: nameless struct/union
#   "/fsanitize=address"            # Address sanitizer
    "$<$<CONFIG:DEBUG>:/Oi>"        # replace calls with intrinsics
    "$<$<CONFIG:DEBUG>:/Zi>"        # generate complete debug info
    "$<$<CONFIG:DEBUG>:/JMC>"       # just my code
    "$<$<CONFIG:RELEASE>:/Ot>"      # prefer fast optimizations
)

target_include_directories(fastcsv PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(fastcsv PUBLIC
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