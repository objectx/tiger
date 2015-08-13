
cmake_minimum_required (VERSION 3.0)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library (Tiger "src/Tiger.cxx")
target_compile_features (Tiger PRIVATE cxx_auto_type cxx_delegating_constructors)
