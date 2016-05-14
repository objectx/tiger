
cmake_minimum_required (VERSION 3.0)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library (Tiger src/Tiger.cxx ${CMAKE_CURRENT_SOURCE_DIR}/include/Tiger.hpp)
target_include_directories (Tiger PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_compile_features (Tiger PRIVATE cxx_auto_type cxx_delegating_constructors)
