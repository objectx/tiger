
cmake_minimum_required (VERSION 3.0)

include_directories ("${CMAKE_SOURCE_DIR}/Tiger/include" "${CMAKE_SOURCE_DIR}/Catch/include")

add_executable (TestTiger "src/main.cpp")
target_link_libraries (TestTiger Tiger)
target_compile_features (TestTiger PRIVATE cxx_auto_type)

add_test (NAME TestTiger COMMAND TestTiger)
