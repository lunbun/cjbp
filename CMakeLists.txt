cmake_minimum_required(VERSION 3.30)
project(cjbp)

add_library(cjbp STATIC)

set_target_properties(cjbp PROPERTIES CXX_STANDARD 17)
set_target_properties(cjbp PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(cjbp PRIVATE cxx_std_17)
target_include_directories(cjbp PUBLIC include)

target_compile_options(cjbp PRIVATE
        "-fno-rtti"
        "$<$<CONFIG:DEBUG>:-O0;-g;-Wall;-Wextra;-Wpedantic>"
        "$<$<CONFIG:RELEASE>:-Ofast;-flto>")
target_compile_definitions(cjbp PRIVATE
        "$<$<CONFIG:RELEASE>:NDEBUG>")

add_subdirectory(include)
add_subdirectory(src)

install(TARGETS cjbp
        EXPORT cjbpConfig
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
export(TARGETS cjbp
        NAMESPACE cjbp::
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cjbpConfig.cmake")

if (BUILD_EXAMPLES)
    add_subdirectory(examples/class_file_reading)
endif ()
