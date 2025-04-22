cmake_minimum_required(VERSION 3.30)
project(cjbp_example1)

add_executable(cjbp_example1 main.cc)

set_target_properties(cjbp_example1 PROPERTIES CXX_STANDARD 17)
set_target_properties(cjbp_example1 PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(cjbp_example1 PRIVATE cxx_std_17)

# TODO: set the path to the cjbp library
set(cjbp_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake-build-debug")
find_package(cjbp REQUIRED)

get_target_property(cjbp_INCLUDE_DIRS cjbp::cjbp INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(cjbp_example1 PRIVATE ${cjbp_INCLUDE_DIRS})
target_link_libraries(cjbp_example1 PRIVATE cjbp::cjbp)
